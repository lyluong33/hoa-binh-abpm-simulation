/*************************************************************************
* SesExtension - see SesExtension.h. SPDX-License-Identifier: MIT
**************************************************************************/
#include "SesExtension.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "RegFarm.h"
#include "RegGlobals.h"
#include "RegPlot.h"

namespace ses {

namespace {
double Clamp01(double a_value) { return std::min(1.0, std::max(0.0, a_value)); }

/** Expansion capacity grows with household assets (0.25 at the poorest, 1 at the richest). */
double AssetFactor(double a_asset_index) { return 0.25 + 0.75 * Clamp01(a_asset_index); }

[[noreturn]] void Fail(const std::string& a_message) {
	std::cerr << "SesExtension: " << a_message << std::endl;
	exit(3);
}
}  // namespace

std::unique_ptr<Extension> Extension::Load(const std::string& a_inputdir, RegGlobalsInfo* a_g) {
	const std::string settings_file = a_inputdir + "/ses.txt";
	if (!std::ifstream(settings_file)) return nullptr;
	std::unique_ptr<Extension> ext(new Extension());
	ext->ReadSettings(settings_file);
	ext->ReadAttributes(a_inputdir + "/farm_attributes.txt");
	ext->m_outdir = a_g->OUTPUTFILE;
	ext->m_rng.seed(ext->m_settings.seed);
	ext->m_emulator.Load(a_inputdir + "/emulator.txt");
	if (ext->m_settings.aw_feedback > 0.0 && !ext->m_emulator.Loaded())
		Fail("AW_FEEDBACK > 0 needs emulator.txt (two-way coupling)");
	std::cout << "SesExtension: loaded (" << ext->m_attributes.size() << " farm types, awareness "
	          << (ext->m_settings.awareness ? "on" : "off") << ")" << std::endl;
	return ext;
}

Extension::~Extension() = default;

void Extension::ReadSettings(const std::string& a_file) {
	std::ifstream in(a_file);
	std::string line;
	while (std::getline(in, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back();
		if (line.empty() || line[0] == '#') continue;
		std::istringstream fields(line);
		std::string key;
		fields >> key;
		Settings& s = m_settings;
		if (key == "ACTIVITY") {
			std::string name, land_row;
			int stewardship = 0, tenure = 0;
			ChangeLimit limit;
			fields >> name >> land_row >> stewardship >> limit.max_expand >> limit.max_contract >> tenure;
			if (fields.fail()) Fail("malformed ACTIVITY line: " + line);
			limit.needs_secure_tenure = tenure != 0;
			s.land_type_of[name] = land_row;
			s.limits[name] = limit;
			if (stewardship) s.stewardship.insert(name);
		} else if (key == "AWARENESS") { std::string v; fields >> v; s.awareness = (v == "true" || v == "1");
		} else if (key == "AW_VALUE") { fields >> s.aw_value;
		} else if (key == "AW_SOCIAL") { fields >> s.aw_social;
		} else if (key == "AW_RADIUS") { fields >> s.aw_radius;
		} else if (key == "AW_FEEDBACK") { fields >> s.aw_feedback;
		} else if (key == "AW_DECAY") { fields >> s.aw_decay;
		} else if (key == "EXT_COVERAGE") { fields >> s.ext_coverage;
		} else if (key == "EXT_EFFECT") { fields >> s.ext_effect;
		} else if (key == "EXT_START") { fields >> s.ext_start;
		} else if (key == "NO_EXIT") { std::string v; fields >> v; s.no_exit = (v == "true" || v == "1");
		} else if (key == "SEED") { fields >> s.seed;
		} else {
			Fail("unknown key " + key);
		}
	}
	if (m_settings.land_type_of.empty()) Fail("no ACTIVITY lines in " + a_file);
}

void Extension::ReadAttributes(const std::string& a_file) {
	std::ifstream in(a_file);
	if (!in) Fail("cannot open " + a_file);
	std::string line;
	while (std::getline(in, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back();
		if (line.empty() || line[0] == '#') continue;
		std::istringstream fields(line);
		std::string name;
		FarmState state;
		int secure = 1;
		fields >> name >> state.awareness0 >> state.asset_index >> secure >> state.initial_activity;
		if (fields.fail()) Fail("malformed attribute line: " + line);
		state.active = true;
		state.awareness = state.awareness0;
		state.tenure_secure = secure != 0;
		m_attributes[name] = state;
	}
}

void Extension::InitFarms(const std::vector<RegFarmInfo*>& a_farms) {
	for (RegFarmInfo* farm : a_farms) {
		auto found = m_attributes.find(farm->getFarmName());
		if (found != m_attributes.end()) farm->sesState() = found->second;
	}
}

void Extension::Remember(LpView& a_lp, int a_col) {
	m_saved.push_back({a_col, a_lp.obj[a_col], a_lp.lb[a_col], a_lp.ub[a_col]});
}

void Extension::BeforeSolve(RegFarmInfo* a_farm, LpView& a_lp) {
	m_saved.clear();
	m_saved_rhs.clear();
	if (!a_farm) return;
	if (m_settings.no_exit) KeepHouseholdSolvent(a_lp);
	const FarmState& state = a_farm->sesState();
	if (!state.active) return;
	for (const auto& item : m_settings.land_type_of) {
		const int col = a_lp.col(item.first);
		if (col >= 0) Remember(a_lp, col);
	}
	if (m_settings.awareness && m_settings.aw_value != 0.0) {
		for (const std::string& name : m_settings.stewardship) {
			const int col = a_lp.col(name);
			if (col >= 0) a_lp.obj[col] += state.awareness * m_settings.aw_value;
		}
	}
	ApplyLimits(state, a_lp);
}

/** With NO_EXIT a farm with negative liquidity is not closed; its working capital
    is assumed to be covered by off-farm income or informal credit, so the two
    financial rows are not allowed to make the MIP infeasible. */
void Extension::KeepHouseholdSolvent(LpView& a_lp) {
	for (const char* name : {"LIQUIDITY", "FINANCING_RULE"}) {  // AgriPoliS stores row names in upper case
		const int row = a_lp.row(name);
		if (row >= 0 && a_lp.rhs[row] < 0.0) {
			m_saved_rhs.push_back({row, a_lp.rhs[row]});
			a_lp.rhs[row] = 0.0;
		}
	}
}

void Extension::ApplyLimits(const FarmState& a_state, LpView& a_lp) {
	std::map<std::string, std::vector<int>> lower_bounded;   // land row -> columns with lb > 0
	std::map<std::string, double> land;
	for (const auto& item : m_settings.land_type_of) {
		const std::string& activity = item.first;
		const int col = a_lp.col(activity), row = a_lp.row(item.second);
		if (col < 0 || row < 0) continue;
		land[item.second] = a_lp.rhs[row];
		double previous = 0.0;
		if (a_state.levels.empty()) previous = (activity == a_state.initial_activity) ? a_lp.rhs[row] : 0.0;
		else if (a_state.levels.count(activity)) previous = a_state.levels.at(activity);
		const ChangeLimit& limit = m_settings.limits.at(activity);
		double expand = limit.max_expand * AssetFactor(a_state.asset_index);
		if (limit.needs_secure_tenure && !a_state.tenure_secure) expand = 0.0;
		if (limit.max_expand < 1.0) a_lp.ub[col] = std::min(a_lp.ub[col], previous + expand * a_lp.rhs[row]);
		if (limit.max_contract < 1.0) {
			a_lp.lb[col] = std::min(previous * (1.0 - limit.max_contract), a_lp.rhs[row]);
			lower_bounded[item.second].push_back(col);
		}
	}
	// Land may have shrunk (plots given back): keep the lower bounds of each land type feasible.
	for (const auto& group : lower_bounded) {
		double sum = 0.0;
		for (int col : group.second) sum += a_lp.lb[col];
		const double available = land[group.first];
		if (sum > available && sum > 0.0)
			for (int col : group.second) a_lp.lb[col] *= available / sum;
	}
}

double Extension::AfterSolve(RegFarmInfo* a_farm, LpView& a_lp, bool a_production) {
	double bonus = 0.0;
	for (const Saved& saved : m_saved) {
		bonus += (a_lp.obj[saved.col] - saved.obj) * a_lp.x[saved.col];
		a_lp.obj[saved.col] = saved.obj;
		a_lp.lb[saved.col] = saved.lb;
		a_lp.ub[saved.col] = saved.ub;
	}
	if (a_production && a_farm && a_farm->sesState().active) {
		FarmState& state = a_farm->sesState();
		state.levels.clear();
		for (const auto& item : m_settings.land_type_of) {
			const int col = a_lp.col(item.first);
			if (col >= 0) state.levels[item.first] = a_lp.x[col];
		}
	}
	for (const auto& saved : m_saved_rhs) a_lp.rhs[saved.first] = saved.second;
	m_saved.clear();
	m_saved_rhs.clear();
	return bonus;
}

void Extension::EndOfPeriod(int a_iteration, const std::vector<RegFarmInfo*>& a_farms) {
	if (m_emulator.Loaded()) {
		std::map<std::string, double> hectares;
		for (RegFarmInfo* farm : a_farms)
			for (const auto& level : farm->sesState().levels) hectares[level.first] += level.second;
		m_emulator.Step(a_iteration, hectares);
		m_emulator.Write(m_outdir + "ses_landscape.dat", a_iteration);
	}
	WriteOutput(a_iteration, a_farms);
	if (m_settings.awareness) UpdateAwareness(a_iteration, a_farms);
}

/** Two-way coupling: experienced loss of the landscape indicator relative to the
    first period raises awareness (loss aversion: gains have no effect). */
double Extension::FeedbackTerm(double a_awareness) const {
	if (m_settings.aw_feedback <= 0.0 || !m_emulator.Loaded()) return 0.0;
	const double reference = m_emulator.PrimaryReference();
	if (reference <= 0.0) return 0.0;
	const double loss = std::max(0.0, (reference - m_emulator.Primary()) / reference);
	return m_settings.aw_feedback * loss * (1.0 - a_awareness);
}

void Extension::UpdateAwareness(int a_iteration, const std::vector<RegFarmInfo*>& a_farms) {
	const Settings& s = m_settings;
	std::uniform_real_distribution<double> uniform(0.0, 1.0);
	const double radius2 = s.aw_radius * s.aw_radius;
	std::vector<double> next(a_farms.size());
	for (size_t i = 0; i < a_farms.size(); i++) {
		FarmState& state = a_farms[i]->sesState();
		double a = state.awareness;
		double neighbours = a;
		RegPlotInfo* home = a_farms[i]->getFarmPlot();
		if (s.aw_social > 0.0 && home) {
			double sum = 0.0;
			int count = 0;
			for (size_t j = 0; j < a_farms.size(); j++) {
				RegPlotInfo* other = a_farms[j]->getFarmPlot();
				if (j == i || !other || !a_farms[j]->sesState().active) continue;
				const double dc = other->getCol() - home->getCol(), dr = other->getRow() - home->getRow();
				if (dc * dc + dr * dr <= radius2) { sum += a_farms[j]->sesState().awareness; count++; }
			}
			if (count > 0) neighbours = sum / count;
		}
		state.informed = a_iteration >= s.ext_start && uniform(m_rng) < s.ext_coverage;
		const double learning = s.aw_social * (neighbours - a);
		const double extension = state.informed ? s.ext_effect * (1.0 - a) : 0.0;
		const double decay = s.aw_decay * (a - state.awareness0);
		next[i] = Clamp01(a + learning + extension - decay + FeedbackTerm(a));
	}
	for (size_t i = 0; i < a_farms.size(); i++)
		if (a_farms[i]->sesState().active) a_farms[i]->sesState().awareness = next[i];
}

void Extension::WriteOutput(int a_iteration, const std::vector<RegFarmInfo*>& a_farms) {
	const bool first = (a_iteration == 0);
	std::ofstream out(m_outdir + "ses_farms.dat", first ? std::ios::out : std::ios::app);
	if (first) {
		out << "iteration\tfarm_id\tfarm_name\tawareness\tinformed\tasset_index\ttenure_secure";
		for (const auto& item : m_settings.land_type_of) out << "\t" << item.first;
		out << "\n";
	}
	for (RegFarmInfo* farm : a_farms) {
		const FarmState& state = farm->sesState();
		if (!state.active) continue;
		out << a_iteration << "\t" << farm->getFarmId() << "\t" << farm->getFarmName() << "\t" << state.awareness
		    << "\t" << state.informed << "\t" << state.asset_index << "\t" << state.tenure_secure;
		for (const auto& item : m_settings.land_type_of) {
			auto level = state.levels.find(item.first);
			out << "\t" << (level == state.levels.end() ? 0.0 : level->second);
		}
		out << "\n";
	}
}

}  // namespace ses
