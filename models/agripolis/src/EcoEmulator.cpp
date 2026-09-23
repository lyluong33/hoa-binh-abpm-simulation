/*************************************************************************
* EcoEmulator - see EcoEmulator.h. SPDX-License-Identifier: MIT
**************************************************************************/
#include "EcoEmulator.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

namespace ses {

namespace {
[[noreturn]] void Fail(const std::string& a_message) {
	std::cerr << "EcoEmulator: " << a_message << std::endl;
	exit(3);
}
}  // namespace

int EcoEmulator::ActivityIndex(const std::string& a_name) const {
	for (size_t i = 0; i < m_activities.size(); i++) if (m_activities[i] == a_name) return static_cast<int>(i);
	return -1;
}

int EcoEmulator::OutputIndex(const std::string& a_name) const {
	for (size_t i = 0; i < m_outputs.size(); i++) if (m_outputs[i].name == a_name) return static_cast<int>(i);
	return -1;
}

bool EcoEmulator::Load(const std::string& a_file) {
	std::ifstream in(a_file);
	if (!in) return false;
	std::string line;
	while (std::getline(in, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back();
		if (line.empty() || line[0] == '#') continue;
		std::istringstream fields(line);
		std::string key;
		fields >> key;
		if (key == "ACTIVITY") {
			std::string name, row;
			fields >> name >> row;
			m_activities.push_back(name);
			m_land_row.push_back(row);
		} else if (key == "OUTPUT") {
			Output output;
			fields >> output.name >> output.tau;
			if (output.tau <= 0.0) Fail("tau must be positive: " + line);
			m_outputs.push_back(output);
		} else if (key == "TERM") {
			std::string output, activity;
			Term term;
			fields >> output >> term.coefficient;
			while (fields >> activity) {
				const int index = ActivityIndex(activity);
				if (index < 0) Fail("unknown activity in " + line);
				term.activities.push_back(index);
			}
			const int out = OutputIndex(output);
			if (out < 0) Fail("TERM before OUTPUT: " + line);
			m_outputs[out].terms.push_back(term);
		} else if (key == "PRIMARY") {
			std::string name;
			fields >> name;
			m_primary = OutputIndex(name);
			if (m_primary < 0) Fail("unknown primary output " + name);
		} else {
			Fail("unknown key " + key);
		}
	}
	if (m_outputs.empty()) Fail("no OUTPUT in " + a_file);
	m_shares.assign(m_activities.size(), 0.0);
	std::cout << "EcoEmulator: " << m_outputs.size() << " indicators over " << m_activities.size()
	          << " activities" << std::endl;
	return true;
}

void EcoEmulator::Step(int /*a_iteration*/, const std::map<std::string, double>& a_hectares) {
	std::map<std::string, double> land_total;
	for (size_t i = 0; i < m_activities.size(); i++) {
		auto found = a_hectares.find(m_activities[i]);
		land_total[m_land_row[i]] += found == a_hectares.end() ? 0.0 : found->second;
	}
	for (size_t i = 0; i < m_activities.size(); i++) {
		auto found = a_hectares.find(m_activities[i]);
		const double total = land_total[m_land_row[i]];
		m_shares[i] = (total > 0.0 && found != a_hectares.end()) ? found->second / total : 0.0;
	}
	for (Output& output : m_outputs) {
		double value = 0.0;
		for (const Term& term : output.terms) {
			double product = term.coefficient;
			for (int index : term.activities) product *= m_shares[index];
			value += product;
		}
		output.equilibrium = value;
		if (!m_started) {
			output.value = value;
			output.reference = value;
		} else {
			output.value += (value - output.value) * (1.0 - std::exp(-1.0 / output.tau));
		}
	}
	m_started = true;
}

void EcoEmulator::Write(const std::string& a_file, int a_iteration) const {
	const bool first = (a_iteration == 0);
	std::ofstream out(a_file, first ? std::ios::out : std::ios::app);
	if (first) {
		out << "iteration";
		for (const auto& name : m_activities) out << "\tshare_" << name;
		for (const auto& output : m_outputs) out << "\t" << output.name << "\t" << output.name << "_equilibrium";
		out << "\n";
	}
	out << a_iteration;
	for (double share : m_shares) out << "\t" << share;
	for (const auto& output : m_outputs) out << "\t" << output.value << "\t" << output.equilibrium;
	out << "\n";
}

double EcoEmulator::Primary() const { return m_outputs.at(m_primary).value; }
double EcoEmulator::PrimaryReference() const { return m_outputs.at(m_primary).reference; }

}  // namespace ses
