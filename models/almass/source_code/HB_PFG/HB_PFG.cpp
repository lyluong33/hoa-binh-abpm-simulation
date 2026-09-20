/**
\file HB_PFG.cpp
\brief Patch agent of the Hoa Binh plant functional group model (see HB_PFG.h).
*/
#include <algorithm>
#include <cmath>
#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Landscape/cropprogs/HB_ScheduledPlan.h"
#include "HB_PFG.h"
#include "HB_PFG_Population_Manager.h"

namespace {
constexpr double kDaysPerYear = 365.0;
constexpr double kClearingIntensity = 0.8;  // operations at least this intense remove standing canopy
}

HB_PFG_Patch::HB_PFG_Patch(int a_x, int a_y, int a_polyref, const PFGArray& a_occupancy, double a_canopy,
	HB_PFG_Population_Manager* a_manager)
	: TAnimal(a_x, a_y), m_polyref(a_polyref), m_occupancy(a_occupancy), m_canopy(a_canopy), m_manager(a_manager) {}

double HB_PFG_Patch::Richness() const {
	double richness = 0.0;
	const auto& groups = m_manager->Groups();
	for (int g = 0; g < kNumPFG; g++) richness += groups[g].pool * m_occupancy[g];
	return richness;
}

void HB_PFG_Patch::Step() {
	if (m_StepDone || m_CurrentStateNo == -1) return;
	const int tov = m_OurLandscape->SupplyVegType(m_polyref);
	ApplyManagement(tov);
	UpdateCanopy(tov);
	Grow(m_manager->SeasonFactor());
	m_StepDone = true;
}

void HB_PFG_Patch::ApplyManagement(int a_tov) {
	const HBPlanRegistry& plans = HBPlanRegistry::Instance();
	const auto& groups = m_manager->Groups();
	int index = 0;
	for (int treatment = m_OurLandscape->SupplyLastTreatment(m_polyref, &index); treatment != sleep_all_day;
		treatment = m_OurLandscape->SupplyLastTreatment(m_polyref, &index)) {
		HBPlanEvent event;
		if (!plans.FindByTreatment(a_tov, treatment, event)) continue;
		const int action = static_cast<int>(event.action);
		for (int g = 0; g < kNumPFG; g++) {
			auto s = groups[g].sensitivity.find(action);
			const double loss = event.intensity * (s == groups[g].sensitivity.end() ? 0.0 : s->second);
			m_occupancy[g] *= std::max(0.0, 1.0 - loss);
		}
		m_disturbance_year += event.intensity;
		if (event.action == HBAction::clearfell) m_canopy = 0.0;
		else if (event.intensity >= kClearingIntensity) m_canopy = std::min(m_canopy, m_manager->TargetCanopy(a_tov));
	}
}

void HB_PFG_Patch::UpdateCanopy(int a_tov) {
	const double target = m_manager->TargetCanopy(a_tov);
	m_canopy += (target - m_canopy) * m_manager->Globals().canopy_recovery / kDaysPerYear;
}

void HB_PFG_Patch::Grow(double a_season) {
	const auto& groups = m_manager->Groups();
	const PFGArray& mean = m_manager->LandscapeMean();
	for (int g = 0; g < kNumPFG; g++) {
		const PFGParameters& p = groups[g];
		const double o = m_occupancy[g];
		const double light = std::exp(-p.shade_beta * m_canopy);
		const double colonisation = p.colonisation / kDaysPerYear * a_season * light * (o + p.seed_rain * mean[g]) * (1.0 - o);
		const double extinction = p.extinction / kDaysPerYear * o;
		m_occupancy[g] = std::clamp(o + colonisation - extinction, 0.0, 1.0);
	}
}
