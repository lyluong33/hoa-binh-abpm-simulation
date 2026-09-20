/**
\file HB_PFG.h
\brief Plant functional group (PFG) occupancy model for the Hoa Binh landscape.

Each farmed polygon carries one HB_PFG_Patch agent. The patch tracks, for five
plant functional groups g, the occupancy o_g in [0,1]: the share of the
regional species pool of that group present in the patch (so that
richness = sum_g pool_g * o_g). Daily dynamics (Levins-type metapopulation):

    do_g/dt = c_g * S(t) * L_g(canopy) * (o_g + r_g * obar_g) * (1 - o_g) - e_g * o_g

with S(t) a temperature/moisture season factor, L_g = exp(-beta_g * canopy)
the light response, obar_g the landscape-mean occupancy (seed rain) and r_g
the seed-rain weight. A management operation of intensity I removes the share
I * s_{g,action} of the occupancy. Canopy cover relaxes towards the target of
the current vegetation type at a fixed yearly rate and is reset by clear-felling,
which produces the lagged responses after a change of land use.
*/
#ifndef HB_PFG_H
#define HB_PFG_H

#include <array>

class HB_PFG_Population_Manager;

constexpr int kNumPFG = 5;
using PFGArray = std::array<double, kNumPFG>;

/** \brief One vegetation patch (= one farmed polygon). */
class HB_PFG_Patch : public TAnimal {
public:
	HB_PFG_Patch(int a_x, int a_y, int a_polyref, const PFGArray& a_occupancy, double a_canopy,
		HB_PFG_Population_Manager* a_manager);
	void BeginStep() override {}
	void Step() override;
	void EndStep() override {}

	int PolyRef() const { return m_polyref; }
	const PFGArray& Occupancy() const { return m_occupancy; }
	double Canopy() const { return m_canopy; }
	double Richness() const;
	double DisturbanceThisYear() const { return m_disturbance_year; }
	void ResetYear() { m_disturbance_year = 0.0; }

private:
	int m_polyref;
	PFGArray m_occupancy;
	double m_canopy;
	double m_disturbance_year = 0.0;
	HB_PFG_Population_Manager* m_manager;

	void ApplyManagement(int a_tov);
	void UpdateCanopy(int a_tov);
	void Grow(double a_season);
};

#endif
