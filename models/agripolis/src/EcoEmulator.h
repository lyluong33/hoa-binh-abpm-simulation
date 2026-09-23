/*************************************************************************
* EcoEmulator - statistical emulator of the ALMaSS Hoa Binh plant model,
* evaluated inside AgriPoliS every period (ABM-test, 2026).
* SPDX-License-Identifier: MIT
*
* The emulator maps the regional land-use composition - the share of each
* activity within its land type - to landscape indicators (e.g. understorey
* species richness, response diversity). It is fitted in Python on designed
* ALMaSS runs (src/hbabm/emulator.py) and read from <inputdir>/emulator.txt:
*
*   ACTIVITY <name> <land_row>
*   OUTPUT <name> <tau_years>
*   TERM <output> <coefficient> [<activity> [<activity>]]     (intercept, linear, product)
*   PRIMARY <output>                                           (indicator used for feedback)
*
* Ecological inertia: each indicator relaxes towards its emulated equilibrium,
*   V_t = V_{t-1} + (V*_t - V_{t-1}) * (1 - exp(-1 / tau)),
* where tau is estimated from the ALMaSS transients.
**************************************************************************/
#ifndef ECO_EMULATOR_H
#define ECO_EMULATOR_H

#include <map>
#include <string>
#include <vector>

namespace ses {

class EcoEmulator {
public:
	/** Reads the emulator file; returns false if it does not exist. */
	bool Load(const std::string& a_file);
	/** Updates the indicators for this period from total hectares per activity. */
	void Step(int a_iteration, const std::map<std::string, double>& a_hectares);
	void Write(const std::string& a_file, int a_iteration) const;

	double Primary() const;           ///< lagged value of the primary indicator
	double PrimaryReference() const;  ///< its value in the first period
	bool Loaded() const { return !m_outputs.empty(); }

private:
	struct Term { double coefficient; std::vector<int> activities; };
	struct Output {
		std::string name;
		double tau = 1.0;
		std::vector<Term> terms;
		double equilibrium = 0.0;
		double value = 0.0;
		double reference = 0.0;
	};
	std::vector<std::string> m_activities;
	std::vector<std::string> m_land_row;
	std::vector<Output> m_outputs;
	std::vector<double> m_shares;
	int m_primary = 0;
	bool m_started = false;

	int ActivityIndex(const std::string& a_name) const;
	int OutputIndex(const std::string& a_name) const;
};

}  // namespace ses

#endif
