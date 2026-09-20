/**
\file HB_PFG_Population_Manager.cpp
\brief Population manager of the Hoa Binh plant functional group model.
*/
#include <algorithm>
#include <sstream>
#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Landscape/cropprogs/HB_ScheduledPlan.h"
#include "HB_PFG.h"
#include "HB_PFG_Population_Manager.h"

static CfgStr cfg_hb_pfg_params_file("HB_PFG_PARAMS_FILE", CFG_CUSTOM, "hb_pfg_parameters.txt");

namespace {
const HBAction kSensitivityOrder[] = {HBAction::cut, HBAction::herbicide, HBAction::burn,
	HBAction::sow, HBAction::harvest, HBAction::clearfell};

[[noreturn]] void Fail(const std::string& a_message, const std::string& a_detail) {
	g_msg->Warn(WARN_FILE, ("HB_PFG: " + a_message).c_str(), a_detail.c_str());
	exit(1);
}
}  // namespace

HB_PFG_Population_Manager::HB_PFG_Population_Manager(Landscape* a_landscape) : Population_Manager(a_landscape, 1) {
	m_ListNames[0] = "HB_PFG_Patch";
	m_ListNameLength = 1;
	m_SimulationName = "HB_PFG";
	ReadParameters(cfg_hb_pfg_params_file.value());
	CreatePatches();
	UpdateLandscapeMean();

	m_patch_file.open("HB_PFG_patches.txt", std::ios::out);
	m_patch_file << "year\tpolyref\ttov\tcanopy";
	for (const auto& g : m_groups) m_patch_file << "\t" << g.name;
	m_patch_file << "\trichness\tdisturbance\n";
	m_landscape_file.open("HB_PFG_landscape.txt", std::ios::out);
	m_landscape_file << "year\tn_patches";
	for (const auto& g : m_groups) m_landscape_file << "\t" << g.name;
	m_landscape_file << "\trichness\tcanopy\n";
}

HB_PFG_Population_Manager::~HB_PFG_Population_Manager() {
	m_patch_file.close();
	m_landscape_file.close();
}

int HB_PFG_Population_Manager::TovOfName(const std::string& a_name) const {
	std::string name = a_name;
	return static_cast<int>(m_TheLandscape->SupplyFarmManagerPtr()->TranslateVegCodes(name));
}

void HB_PFG_Population_Manager::ReadParameters(const std::string& a_file) {
	std::ifstream in(a_file);
	if (!in) Fail("cannot open parameter file ", a_file);
	std::string line;
	while (std::getline(in, line)) {
		if (line.empty() || line[0] == '#' || line[0] == '\r') continue;
		std::istringstream fields(line);
		std::string key;
		fields >> key;
		if (key == "PFG") {
			PFGParameters p;
			fields >> p.name >> p.colonisation >> p.extinction >> p.shade_beta >> p.seed_rain >> p.pool;
			for (HBAction action : kSensitivityOrder) {
				double s = 0.0;
				fields >> s;
				p.sensitivity[static_cast<int>(action)] = s;
			}
			if (fields.fail()) Fail("malformed PFG line ", line);
			m_groups.push_back(p);
		} else if (key == "CANOPY" || key == "CANOPY_INIT") {
			std::string tov;
			double value = 0.0;
			if (!(fields >> tov >> value)) Fail("malformed canopy line ", line);
			(key == "CANOPY" ? m_canopy_target : m_init_canopy)[TovOfName(tov)] = value;
		} else if (key == "INIT") {
			std::string tov;
			PFGArray occupancy{};
			fields >> tov;
			for (double& o : occupancy) fields >> o;
			if (fields.fail()) Fail("malformed INIT line ", line);
			m_init_occupancy[TovOfName(tov)] = occupancy;
		} else if (key == "GLOBAL") {
			std::string name;
			double value = 0.0;
			if (!(fields >> name >> value)) Fail("malformed GLOBAL line ", line);
			if (name == "CANOPY_RECOVERY") m_globals.canopy_recovery = value;
			else if (name == "T_MIN") m_globals.t_min = value;
			else if (name == "T_OPT") m_globals.t_opt = value;
			else if (name == "RAIN_HALF") m_globals.rain_half = value;
			else Fail("unknown GLOBAL ", name);
		} else {
			Fail("unknown key ", key);
		}
	}
	if (static_cast<int>(m_groups.size()) != kNumPFG) Fail("expected five PFG lines in ", a_file);
}

void HB_PFG_Population_Manager::CreatePatches() {
	const HBPlanRegistry& plans = HBPlanRegistry::Instance();
	const unsigned n_polygons = m_TheLandscape->SupplyNumberOfPolygons();
	for (unsigned i = 0; i < n_polygons; i++) {
		const int polyref = m_TheLandscape->SupplyPolyRefVector(i);
		const int tov = m_TheLandscape->SupplyVegType(polyref);
		if (!plans.IsHBVegetation(tov)) continue;
		auto init = m_init_occupancy.find(tov);
		const PFGArray occupancy = init != m_init_occupancy.end() ? init->second : PFGArray{0.05, 0.05, 0.05, 0.05, 0.05};
		auto canopy = m_init_canopy.find(tov);
		const double canopy0 = canopy != m_init_canopy.end() ? canopy->second : TargetCanopy(tov);
		auto* patch = new HB_PFG_Patch(m_TheLandscape->SupplyCentroidX(polyref), m_TheLandscape->SupplyCentroidY(polyref),
			polyref, occupancy, canopy0, this);
		PushIndividual(0, patch);
		IncLiveArraySize(0);
		m_patches.push_back(patch);
	}
	if (m_patches.empty()) Fail("no polygons with Hoa Binh vegetation types", "");
}

double HB_PFG_Population_Manager::TargetCanopy(int a_tov) const {
	auto found = m_canopy_target.find(a_tov);
	return found == m_canopy_target.end() ? 0.0 : found->second;
}

void HB_PFG_Population_Manager::UpdateLandscapeMean() {
	m_mean.fill(0.0);
	const double n = static_cast<double>(m_patches.size());
	for (const HB_PFG_Patch* patch : m_patches)
		for (int g = 0; g < kNumPFG; g++) m_mean[g] += patch->Occupancy()[g] / n;
}

void HB_PFG_Population_Manager::DoFirst() {
	const double temp = m_TheLandscape->SupplyTemp();
	const double rain30 = m_TheLandscape->SupplyRainPeriod(g_date->Date(), 30);
	const double warmth = std::clamp((temp - m_globals.t_min) / (m_globals.t_opt - m_globals.t_min), 0.0, 1.0);
	const double moisture = std::clamp(rain30 / m_globals.rain_half, 0.0, 1.0);
	m_season = warmth * moisture;
	UpdateLandscapeMean();
}

void HB_PFG_Population_Manager::DoLast() {
	if (g_date->GetMonth() == 12 && g_date->GetDayInMonth() == 31) WriteYear();
}

void HB_PFG_Population_Manager::WriteYear() {
	const int year = g_date->GetYear();
	const double n = static_cast<double>(m_patches.size());
	PFGArray mean{};
	double richness = 0.0, canopy = 0.0;
	for (HB_PFG_Patch* patch : m_patches) {
		const int tov = m_TheLandscape->SupplyVegType(patch->PolyRef());
		m_patch_file << year << "\t" << patch->PolyRef() << "\t" << tov << "\t" << patch->Canopy();
		for (int g = 0; g < kNumPFG; g++) {
			m_patch_file << "\t" << patch->Occupancy()[g];
			mean[g] += patch->Occupancy()[g] / n;
		}
		m_patch_file << "\t" << patch->Richness() << "\t" << patch->DisturbanceThisYear() << "\n";
		richness += patch->Richness() / n;
		canopy += patch->Canopy() / n;
		patch->ResetYear();
	}
	m_landscape_file << year << "\t" << m_patches.size();
	for (double m : mean) m_landscape_file << "\t" << m;
	m_landscape_file << "\t" << richness << "\t" << canopy << "\n";
	m_patch_file.flush();
	m_landscape_file.flush();
}
