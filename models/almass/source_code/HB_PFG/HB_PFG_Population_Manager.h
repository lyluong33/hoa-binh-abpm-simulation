/**
\file HB_PFG_Population_Manager.h
\brief Population manager of the Hoa Binh plant functional group model (see HB_PFG.h).

Parameters are read from the file named by config key HB_PFG_PARAMS_FILE
(default "hb_pfg_parameters.txt"); outputs are written each 31 December:
HB_PFG_patches.txt (one line per patch) and HB_PFG_landscape.txt (one line per year).
*/
#ifndef HB_PFG_POPULATION_MANAGER_H
#define HB_PFG_POPULATION_MANAGER_H

#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "HB_PFG.h"

/** \brief Demographic and response parameters of one plant functional group. */
struct PFGParameters {
	std::string name;
	double colonisation = 1.0;   ///< c_g per year
	double extinction = 0.3;     ///< e_g per year
	double shade_beta = 0.0;     ///< beta_g of the light response exp(-beta * canopy)
	double seed_rain = 0.5;      ///< r_g, weight of the landscape-mean occupancy
	double pool = 1.0;           ///< regional species pool size
	std::map<int, double> sensitivity;  ///< s_{g,action} keyed by HBAction
};

/** \brief Settings that are not specific to one group. */
struct PFGGlobals {
	double canopy_recovery = 0.2;  ///< share of the canopy gap closed per year
	double t_min = 12.0;           ///< temperature without growth (deg C)
	double t_opt = 24.0;           ///< temperature of full growth (deg C)
	double rain_half = 30.0;       ///< 30-day rain (mm) giving full moisture
};

class HB_PFG_Population_Manager : public Population_Manager {
public:
	explicit HB_PFG_Population_Manager(Landscape* a_landscape);
	~HB_PFG_Population_Manager() override;

	const std::vector<PFGParameters>& Groups() const { return m_groups; }
	const PFGGlobals& Globals() const { return m_globals; }
	double TargetCanopy(int a_tov) const;
	double SeasonFactor() const { return m_season; }
	const PFGArray& LandscapeMean() const { return m_mean; }

protected:
	void DoFirst() override;
	void DoLast() override;

private:
	std::vector<PFGParameters> m_groups;
	std::vector<HB_PFG_Patch*> m_patches;  ///< non-owning; patches live for the whole run
	PFGGlobals m_globals;
	std::map<int, double> m_canopy_target;
	std::map<int, PFGArray> m_init_occupancy;
	std::map<int, double> m_init_canopy;
	PFGArray m_mean{};
	double m_season = 0.0;
	std::ofstream m_patch_file;
	std::ofstream m_landscape_file;

	void ReadParameters(const std::string& a_file);
	void CreatePatches();
	void UpdateLandscapeMean();
	void WriteYear();
	int TovOfName(const std::string& a_name) const;
};

#endif
