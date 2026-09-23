/*************************************************************************
* SesExtension - social-ecological extension of AgriPoliS for the Hoa Binh
* uplands (ABM-test, 2026). SPDX-License-Identifier: MIT
*
* Adds to every farm agent
*  - a conservation-awareness index a in [0,1], initialised from the land-
*    manager survey (farm_attributes.txt) and entering the farm MIP as a
*    non-market value a * AW_VALUE per hectare of stewardship activities;
*  - asset- and tenure-dependent limits on how fast perennial activities
*    (orchards, plantations) can be expanded or given up, which gives land use
*    the inertia of multi-year investments (Jezeer et al. 2019: assets, not
*    perceptions alone, condition which practices households adopt);
*  - awareness dynamics: social learning from neighbours, an extension
*    campaign, and - when coupled to ALMaSS - a response to the experienced
*    decline of the landscape biodiversity indicator (EcoEmulator.h,
*    AW_FEEDBACK > 0 = two-way coupling; emulator without feedback = one-way).
* Without <inputdir>/ses.txt the model behaves like the original AgriPoliS.
**************************************************************************/
#ifndef SES_EXTENSION_H
#define SES_EXTENSION_H

#include <functional>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "EcoEmulator.h"

class RegFarmInfo;
class RegGlobalsInfo;

namespace ses {

/** Per-farm state carried by RegFarmInfo. */
struct FarmState {
	bool active = false;            ///< attributes found for this farm type
	double awareness = 0.0;
	double awareness0 = 0.0;
	double asset_index = 0.5;
	bool tenure_secure = true;
	bool informed = false;
	std::map<std::string, double> levels;  ///< hectares per activity in the last production plan
	std::string initial_activity;
};

/** Limits on year-to-year change of one activity (shares per year). */
struct ChangeLimit {
	double max_expand = 1.0;      ///< of the land of its type
	double max_contract = 1.0;    ///< of its current level
	bool needs_secure_tenure = false;
};

/** Read/write access to the MIP of one farm, provided by RegLpInfo. */
struct LpView {
	std::vector<double>& obj;
	std::vector<double>& lb;
	std::vector<double>& ub;
	std::vector<double>& rhs;
	const std::vector<double>& x;
	std::function<int(const std::string&)> col;   ///< -1 if absent
	std::function<int(const std::string&)> row;   ///< -1 if absent
};

struct Settings {
	bool awareness = false;
	double aw_value = 0.0;          ///< EUR/ha at awareness 1
	double aw_social = 0.0;         ///< learning rate towards neighbours
	double aw_radius = 10.0;        ///< neighbourhood radius (plots)
	double aw_feedback = 0.0;       ///< response to biodiversity decline
	double aw_decay = 0.0;          ///< relaxation towards the initial value
	double ext_coverage = 0.0;      ///< share of farms reached per period
	double ext_effect = 0.0;        ///< awareness gain of a reached farm
	int ext_start = 1 << 30;        ///< first iteration of the campaign
	bool no_exit = false;           ///< households keep their land-use rights
	unsigned seed = 2026;
	std::set<std::string> stewardship;
	std::map<std::string, std::string> land_type_of;   ///< activity -> land row
	std::map<std::string, ChangeLimit> limits;
};

/** The extension; one instance per simulation, owned by RegManagerInfo. */
class Extension {
public:
	/** Loads ses.txt and farm_attributes.txt; returns nullptr if ses.txt is absent. */
	static std::unique_ptr<Extension> Load(const std::string& a_inputdir, RegGlobalsInfo* a_g);
	~Extension();

	void InitFarms(const std::vector<RegFarmInfo*>& a_farms);
	/** Called by RegLpInfo right before the solver; returns the bonus added to the objective. */
	void BeforeSolve(RegFarmInfo* a_farm, LpView& a_lp);
	/** Called right after the solver: restores the MIP and returns the non-market part of objval. */
	double AfterSolve(RegFarmInfo* a_farm, LpView& a_lp, bool a_production);
	/** End of a period (after Production): coupling, awareness update, output. */
	void EndOfPeriod(int a_iteration, const std::vector<RegFarmInfo*>& a_farms);

	bool NoExit() const { return m_settings.no_exit; }
	const Settings& GetSettings() const { return m_settings; }

private:
	Extension() = default;
	Settings m_settings;
	std::map<std::string, FarmState> m_attributes;   ///< by farm (type) name
	EcoEmulator m_emulator;                           ///< ALMaSS emulator (optional, emulator.txt)
	std::string m_outdir;
	std::mt19937 m_rng;

	struct Saved { int col; double obj, lb, ub; };
	std::vector<Saved> m_saved;
	std::vector<std::pair<int, double>> m_saved_rhs;

	void ReadSettings(const std::string& a_file);
	void ReadAttributes(const std::string& a_file);
	void ApplyLimits(const FarmState& a_state, LpView& a_lp);
	void KeepHouseholdSolvent(LpView& a_lp);
	void UpdateAwareness(int a_iteration, const std::vector<RegFarmInfo*>& a_farms);
	double FeedbackTerm(double a_awareness) const;
	void WriteOutput(int a_iteration, const std::vector<RegFarmInfo*>& a_farms);
	void Remember(LpView& a_lp, int a_col);
};

}  // namespace ses

#endif
