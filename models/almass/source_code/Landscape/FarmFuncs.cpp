/**
\file
\brief
<B>Farmfuncs.cpp This file contains the source for implementing the farm events</B> \n
*/
/**
\file
 by Frank Nikolaisen & Chris J. Topping \n
 Initial version of June 2003, but under continual change. \n
 All rights reserved. \n
 \n
 Doxygen formatted comments in July 2008 \n
*/
//
// farmfuncs.cpp
//
/*
*******************************************************************************************************
Copyright (c) 2011, Christopher John Topping, Aarhus University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided
that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the
following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************************************
*/
#define UNREFERENCED_PARAMETER(P) (P)

#include "ls.h"
#include "../BatchALMaSS/PopulationManager.h"


using namespace std;

extern Landscape* g_landscape_ptr;
extern CfgArray_Double cfg_pest_product_amounts;
extern CfgInt l_pest_productapplic_startdate;
extern CfgInt l_pest_productapplic_enddate;
extern CfgInt l_pest_productapplic_period;
extern CfgBool cfg_OptimiseBedriftsmodelCrops;
extern CfgInt cfg_productapplicstartyear;
extern CfgInt cfg_productapplicendyear;

extern CfgBool cfg_UseSocioEconomicFarm;

#define DO_IT_PROB (l_farm_event_do_it_prob.value())
#define DO_IT_PROB_LONG (l_farm_event_do_it_prob_long.value())

static CfgInt l_farm_event_do_it_prob("FARM_EVENT_DO_IT_PROB", CFG_PRIVATE, 50);
static CfgInt l_farm_event_do_it_prob_long("FARM_EVENT_DO_IT_PROB_LONG", CFG_PRIVATE, 5);
static CfgFloat l_farm_cattle_veg_reduce("FARM_CATTLE_VEG_REDUCE", CFG_CUSTOM, 1.5);
static CfgFloat l_farm_cattle_veg_reduce2("FARM_CATTLE_VEG_REDUCE_LOW", CFG_CUSTOM, 0.50 );//1.00
CfgInt cfg_farm_cattle_grass_low("FARM_CATTLE_GRASS_LOW", CFG_CUSTOM, 4);
CfgInt cfg_farm_cattle_grass_high("FARM_CATTLE_GRASS_HIGH", CFG_CUSTOM, 15);
CfgInt cfg_farm_cattle_grazing_lvl_rot("FARM_CATTLE_GRAZING_LVL_ROT", CFG_CUSTOM, 4); 
CfgInt cfg_farm_cattle_grazing_lvl_pasture("FARM_CATTLE_GRAZING_LVL_PASTURE", CFG_CUSTOM, 3); 
CfgInt cfg_farm_cattle_grazing_lvl_lowyield("FARM_CATTLE_GRAZING_LVL_LOWYIELD", CFG_CUSTOM, 2); 
CfgInt cfg_farm_cattle_grazing_lvl_tussocky("FARM_CATTLE_GRAZING_LVL_TUSSOCKY", CFG_CUSTOM, 1); 
CfgArray_Double cfg_farm_cattle_reduce_height("FARM_CATTLE_REDUCE_HEIGHT", CFG_CUSTOM, 4, vector<double> {0.2, 0.3, 1.0, 8.0}); // Used with the new grazing code (only for DK at the moment)
static CfgFloat l_farm_pig_veg_reduce("FARM_PIG_VEG_REDUCE", CFG_CUSTOM, 0.98 );
static CfgFloat cfg_CustomInsecticidesSurvProp("CUSTOMINSECTIVIDESURVPROP", CFG_CUSTOM, 0.5,0,1.0);

// Determine whether insecticide and herbicide actually
// influences the insect population. By default they *do*
// kill off some of the little critters.
static CfgBool l_farm_insecticide_kills("FARM_INSECTICIDE_KILLS", CFG_CUSTOM, true );
static CfgBool l_farm_herbicide_kills("FARM_PESTICIDE_KILLS", CFG_CUSTOM, true );

/** \brief Provided to allow configuration control of the first insecticide spray in winter wheat - this changes the day in the month */
CfgInt cfg_WW_InsecticideDay("PEST_WWINSECTONEDAY", CFG_CUSTOM, 1);
/** \brief Provided to allow configuration control of the first insecticide spray in winter wheat - this changes the month */
CfgInt cfg_WW_InsecticideMonth("PEST_WWINSECTONEMONTH", CFG_CUSTOM, 5);
/** \brief Provided to allow configuration control of the proportion of farmers doing first conventional tillage in winter wheat - this changes between 0 and 1 */
CfgFloat cfg_WW_conv_tillage_prop1("TILLAGE_WWCONVONEPROP", CFG_CUSTOM, 1.0);
/** \brief Provided to allow configuration control of the proportion of farmers doing second conventional tillage in winter wheat - this changes between 0 and 1 */
CfgFloat cfg_WW_conv_tillage_prop2("TILLAGE_WWCONVTWOPROP", CFG_CUSTOM, 1.0);
/** \brief Provided to allow configuration control of the proportion of farmers doing first non-inversion tillage in winter wheat - this changes between 0 and 1 */
CfgFloat cfg_WW_NINV_tillage_prop1("TILLAGE_WWNINVONEPROP", CFG_CUSTOM, 1.0);
/** \brief Provided to allow configuration control of the proportion of farmers doing second non-inversion tillage in winter wheat - this changes between 0 and 1 */
CfgFloat cfg_WW_NINV_tillage_prop2("TILLAGE_WWNINVTWOPROP", CFG_CUSTOM, 1.0);
/** \brief Provided to allow configuration control of the proportion of farmers doing first insecticide spray in winter wheat - this changes between 0 and 1 */
CfgFloat cfg_WW_isecticide_prop1("PEST_WWINSECTONEPROP", CFG_CUSTOM, 1.0);
/** \brief Provided to allow configuration control of the proportion of farmers doing second insecticide spray in winter wheat - this changes between 0 and 1 */
CfgFloat cfg_WW_isecticide_prop2("PEST_WWINSECTTWOPROP", CFG_CUSTOM, 1.0);
/** \brief Provided to allow configuration control of the proportion of farmers doing third insecticide spray in winter wheat - this changes between 0 and 1 */
CfgFloat cfg_WW_isecticide_prop3("PEST_WWINSECTTHREEPROP", CFG_CUSTOM, 1.0);


/** \brief Provided to allow configuration control of the first insecticide spray in spring barley crops - this changes the day in the month */
CfgInt cfg_SB_InsecticideDay("PEST_SBINSECTONEDAY", CFG_CUSTOM, 15);
/** \brief Provided to allow configuration control of the first insecticide spray in spring barley crops - this changes the month */
CfgInt cfg_SB_InsecticideMonth("PEST_SBINSECTONEMONTH", CFG_CUSTOM, 5);
/** \brief Provided to allow configuration control of the first insecticide spray in OSR - this changes the day in the month */
CfgInt cfg_OSR_InsecticideDay("PEST_OSRINSECTONEDAY", CFG_CUSTOM, 30);
/** \brief Provided to allow configuration control of the first insecticide spray in OSR - this changes the month */
CfgInt cfg_OSR_InsecticideMonth("PEST_OSRINSECTONEMONTH", CFG_CUSTOM, 4);

/** \brief Provided to allow configuration control of the first insecticide spray in cabbage crops - this changes the day in the month */
CfgInt cfg_CAB_InsecticideDay("PEST_CABINSECTONEDAY", CFG_CUSTOM, 15);
/** \brief Provided to allow configuration control of the first insecticide spray in cabbage crops - this changes the month */
CfgInt cfg_CAB_InsecticideMonth("PEST_CABINSECTONEMONTH", CFG_CUSTOM, 5);

/** \brief Provided to allow configuration control of the first insecticide spray in tulip crops - this changes the day in the month */
CfgInt cfg_TU_InsecticideDay("PEST_TUINSECTONEDAY", CFG_CUSTOM, 15);
/** \brief Provided to allow configuration control of the first insecticide spray in tulip crops - this changes the month */
CfgInt cfg_TU_InsecticideMonth("PEST_TUINSECTONEMONTH", CFG_CUSTOM, 5);

/** \brief Provided to allow configuration control of the first insecticide spray in potatoes crops - this changes the day in the month */
CfgInt cfg_POT_InsecticideDay("PEST_POTINSECTONEDAY", CFG_CUSTOM, 15);
/** \brief Provided to allow configuration control of the first insecticide spray in potatoes crops - this changes the month */
CfgInt cfg_POT_InsecticideMonth("PEST_POTINSECTONEMONTH", CFG_CUSTOM, 5);

/** \brief Provided to allow configuration control of the first insecticide spray in beet crops - this changes the day in the month */
CfgInt cfg_BE_InsecticideDay("PEST_BEINSECTONEDAY", CFG_CUSTOM, 15);
/** \brief Provided to allow configuration control of the first insecticide spray in beet crops - this changes the month */
CfgInt cfg_BE_InsecticideMonth("PEST_BEINSECTONEMONTH", CFG_CUSTOM, 5);

/** \brief Provided to allow configuration control of the first insecticide spray in orchard crops - this changes the day in the month */
CfgInt cfg_ORCH_InsecticideDay("PEST_ORCHINSECTONEDAY", CFG_CUSTOM, 15);
/** \brief Provided to allow configuration control of the first insecticide spray in orchard crops - this changes the month */
CfgInt cfg_ORCH_InsecticideMonth("PEST_ORCHINSECTONEMONTH", CFG_CUSTOM, 4);

CfgFloat cfg_biocide_reduction_val("REDUCE_BIOCIDE_EFFECT", CFG_CUSTOM, 0.0);

//declared in elements.cpp
extern CfgFloat l_el_o_cut_height;
extern CfgFloat l_el_o_cut_green;
extern CfgFloat l_el_o_cut_total;

LE* Farm::SetFunctionData(LE* a_field, double a_insecteffect, double a_vegreduc, int a_tramlinetime, FarmToDo a_treatment)
{
	/**
	* Sets the standard data needed for most FarmFuncs.
	* In the case that something else is needed and there is a UM then also returns a pointer to the UM LE
	* so that further actions can be undertaken by the calling farm fuction.
	*
	* a_insecteffect is a fraction by which the current population size is multiplied - so 1.0 means no change and 0.1 means reduction by 90%.
	* a_vegezero indicates whether to remove vegetation.
	* a_treatment is whatever is being done e.g. autumn_plough
	*/
	LE* um = nullptr;
	// The trace below is for debugging checks
	#ifdef FMDEBUG
	a_field->Trace(a_treatment);
	#endif
	// Record the event for this field, so other objects can find out it has happened
	a_field->SetLastTreatment(a_treatment);
	// Apply mortality to the insects present, in this case 90%. This only affects the general insect model, any ALMaSS model species need to take their specific action.
	a_field->InsectMortality(a_insecteffect);
	// Reduce the vegetation
	if (a_vegreduc != 1.0)
		if (a_vegreduc == 0.0) a_field->ZeroVeg(); else a_field->ReduceVeg(a_vegreduc);
	// if the operation opens the tramlines record this
	if (a_tramlinetime > 0) a_field->SetTramlinesDecay(a_tramlinetime);
	// If the field has a field margin, then do all this to the field margin too. In events that don't occur on an unsprayed margin, e.g. insecticide, then is part is skipped.
	int pref = a_field->GetUnsprayedMarginPolyRef();
	if (pref != -1) {
		// Must have an unsprayed margin so need to pass the information on to it
		um = g_landscape_ptr->SupplyLEPointer(pref);
		um->SetLastTreatment(a_treatment);
		um->InsectMortality(a_insecteffect);
		if (a_vegreduc != 1.0)
			if (a_vegreduc == 0.0) um->ZeroVeg(); else um->ReduceVeg(a_vegreduc);
		if (a_tramlinetime > 0) um->SetTramlinesDecay(a_tramlinetime);
	}
	return um;
}

void Farm::SetFunctionDataNoUM(LE* a_field, double a_insecteffect, double a_vegreduc, int a_tramlinetime, FarmToDo a_treatment)
{
	/**
	* Sets the standard data needed for most FarmFuncs for when any UM should be ignored
	* a_insecteffect is a fraction by which the current population size is multiplied - so 1.0 means no change and 0.1 means reduction by 90%.
	* a_vegezero indicates whether to remove vegetation.
	* a_treatment is whatever is being done e.g. autumn_plough
	*/
	// The trace below is for debugging checks
	a_field->Trace(a_treatment);
	// Record the event for this field, so other objects can find out it has happened
	a_field->SetLastTreatment(a_treatment);
	// Apply mortality to the insects present, in this case 90%. This only affects the general insect model, any ALMaSS model species need to take their specific action.
	a_field->InsectMortality(a_insecteffect);
	// Reduce the vegetation
	if (a_vegreduc != 1.0)
		if (a_vegreduc == 0.0) a_field->ZeroVeg(); else a_field->ReduceVeg(a_vegreduc);
	// if the operation opens the tramlines record this
	if (a_tramlinetime > 0) a_field->SetTramlinesDecay(a_tramlinetime);
}

/**
\brief
Carry out a ploughing event in the autumn on a_field
*/
bool Farm::AutumnPlough( LE *a_field, double /*a_user*/, int a_days)
{
	// LE is a pointer to the field element
	// a_user is a pointer to the farm
	// a_days is the end of the operation time - today
	// if a_days <0 then the time to do it is passed
	// the line below reads 'plough if last day possible OR if not raining and pass a probability test
	if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		// Set the data for this, in this case 90% insect mortality and remove all veg, not tramline open time, the treatment is autumn plough
		SetFunctionData(a_field, 0.1, 0.0, 0, autumn_plough);
		return true;      // completed
	}
	return false;       // not completed
}

/**
\brief
Carry out a stubble ploughing event on a_field. This is similar to normal plough but shallow (normally 6-8cm, is special cases up to 12-15cm).
Done as a part of after-harvest treatments (instead of stubble cultivation) 
*/
bool Farm::StubblePlough(LE *a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.1, 0.0,  0,stubble_plough); // the third parameter of 0.0 creates a call to ZeroVeg
		return true;      // completed
	}
	return false;       // not completed
}

/**
\brief
Carry out a stubble cultivation event on a_field. This is non-inversion type of cultivation which can be done instead of autumn plough (on a depth up to 40 cm even, if necessary)
*/
bool Farm::StubbleCultivatorHeavy(LE *a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = stubble_cultivator_heavy;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.1, 0.0,  0,stubble_cultivator_heavy);
		return true;      // completed
	}
	return false;       // not completed
}

/**
\brief
Carry out a heavy cultivation event on a_field. This is non-inversion type of cultivation which can be done after fertilizers application on spring for a spring crop
*/
bool Farm::HeavyCultivatorAggregate(LE *a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.1, 0.0,  0,heavy_cultivator_aggregate);
		return true;      // completed
	}
	return false;       // not completed
}

/**
\brief
Nothing to to today on a_field
*/
bool Farm::SleepAllDay( LE *a_field, double /*a_user*/, int a_days)
{
	if (0 >= a_days) {
		SetFunctionData(a_field, 1.0, 1.0, 0, sleep_all_day);
		return true;
	}
	return false;
}

/**
\brief
Carry out a harrow event in the autumn on a_field
*/
bool Farm::AutumnHarrow( LE *a_field, double /*a_user*/, int a_days )
{
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  /** \brief carries out standard setting of data and deals with possible unsprayed margins, returns a pointer to UM or nullptr */
	  SetFunctionData(a_field, 0.1, 0.0,  0,autumn_harrow);
	  return true;
  }
  return false;
}

/**
\brief
Carry out a roll event in the autumn on a_field
*/
bool Farm::AutumnRoll( LE *a_field, double /*a_user*/, int a_days )
{
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	SetFunctionData(a_field, 0.9, 1.0, 0, autumn_roll);
	return true;
  }
  return false;
}

/**
\brief
Carry out preseeding cultivation on a_field (tilling set including cultivator and string roller to compact soil)
*/
bool Farm::PreseedingCultivator(LE *a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.1, 0.0,  0,preseeding_cultivator);
		return true;
	}
	return false;
}

/**
\brief
Carry out preseeding cultivation together with sow on a_field (tilling and sowing set including cultivator and string roller to compact soil)
*/
bool Farm::PreseedingCultivatorSow(LE *a_field, double /*a_user*/, int a_days, double a_seed_coating_amount, PlantProtectionProducts a_ppp)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 0.1, 0.0,  0,preseeding_cultivator_sow);
		a_field->SetGrowthPhase(sow); // as with AutumnSow treatment
		a_field->SetLastSownVeg(a_field->GetVegType()); // as with AutumnSow treatment
		if (um != nullptr) {
			um->SetGrowthPhase(sow);
			um->SetLastSownVeg(a_field->GetVegType());
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out a sowing event in the autumn on a_field
*/
bool Farm::AutumnSow(LE* a_field, double /*a_user*/, int a_days, double a_seed_coating_amount, PlantProtectionProducts a_ppp)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 1.0, 0.0, 0, autumn_sow);
		a_field->SetGrowthPhase(sow); // as with AutumnSow treatment
		a_field->SetLastSownVeg(a_field->GetVegType()); // as with AutumnSow treatment
		if (um != nullptr) {
			um->SetGrowthPhase(sow);
			um->SetLastSownVeg(a_field->GetVegType());
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out a ploughing event in the winter on a_field
*/
bool Farm::WinterPlough( LE *a_field, double /*a_user*/, int a_days )
{
    if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.1, 0.0, 0, winter_plough);
	return true;
  }
  return false;
}

/**
\brief
Carry out a deep ploughing event on a_field
*/
bool Farm::DeepPlough( LE *a_field, double /*a_user*/, int a_days )
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.1, 0.0,  0,deep_ploughing);
	return true;
  }
  return false;
}

/**
\brief
Carry out a ploughing event in the spring on a_field
*/
bool Farm::SpringPlough( LE *a_field, double /*a_user*/, int a_days )
{
	if ((a_days <= 0) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.1, 0.0,  0,spring_plough);
		return true;
  }
  return false;
}

/**
\brief
Physical/mechanical weed control in the between-row area using a rotary mower cutter
*/
bool Farm::Mowing(LE* a_field, double /*a_user*/, int a_days) {
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		a_field->Trace(mow);
		a_field->SetLastTreatment(mow);
		// 30% insect mortality
		a_field->InsectMortality(0.7);
		// remove vegetation partly
		a_field->ReduceVeg(0.8);
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(mow);
			um->InsectMortality(0.7);
			um->ReduceVeg(0.8);
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out a harrow event in the spring on a_field
*/
bool Farm::SpringHarrow( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = spring_harrow;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 0.7, 0.0, 0, spring_harrow);
	return true;
  }
  return false;
}

/**
\brief
Carry out a shallow harrow event on a_field, e.g., after grass cutting event
*/
bool Farm::ShallowHarrow(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = shallow_harrow;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.7, 0.8, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Carry out a roll event in the spring on a_field
*/
bool Farm::SpringRoll( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = spring_roll;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Carry out a sowing event in the spring on a_field
*/
bool Farm::SpringSow(LE* a_field, double /*a_user*/, int a_days, double a_seed_coating_amount, PlantProtectionProducts a_ppp)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 1.0, 1.0, 0, spring_sow);
		a_field->SetGrowthPhase(sow);
		a_field->SetLastSownVeg(a_field->GetVegType());
		if (um != nullptr)
		{
			um->SetGrowthPhase(sow);
			um->SetLastSownVeg(um->GetVegType());
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out a sowing event with start fertilizer in the spring on a_field
*/
bool Farm::SpringSowWithFerti(LE *a_field, double /*a_user*/, int a_days, double a_seed_coating_amount, PlantProtectionProducts a_ppp)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 1.0, 1.0, 0, spring_sow_with_ferti);
		a_field->SetGrowthPhase(sow);
		a_field->SetLastSownVeg(a_field->GetVegType());
		if (um != nullptr)
		{
			um->SetGrowthPhase(sow);
			um->SetLastSownVeg(um->GetVegType());
		}
		return true;
	}
	return false;
}


bool Farm::Suckering(LE* a_field, double a_user, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		a_field->Trace(suckering);
		a_field->SetLastTreatment(suckering);
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(green_harvest);
		}
		return true;

	}
	return false;
}


/**
\brief
Carry out a sowing event with start fertilizer in the autumn on a_field
*/
bool Farm::AutumnSowWithFerti(LE* a_field, double /*a_user*/, int a_days, double a_seed_coating_amount, PlantProtectionProducts a_ppp)
{
	FarmToDo treatment = autumn_sow_with_ferti;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		a_field->SetGrowthPhase(sow);
		a_field->SetLastSownVeg(a_field->GetVegType());
		if (um != nullptr)
		{
			um->SetGrowthPhase(sow);
			um->SetLastSownVeg(um->GetVegType());
		}
		return true;
	}
	return false;
}


/**
\brief
Apply NPKS fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_NPKS( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = fp_npks;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}


/**
\brief
Apply NPK fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_NPK( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = fp_npk;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Apply PKS fertilizer to a_field owned by an arable farmer
*/
bool Farm::FP_PKS(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_pks;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply PK fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_PK( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = fp_pk;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Apply P fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_P(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fp_p);
		return true;
	}
	return false;
}

/**
\brief
Apply N fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_N(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fp_n);
		return true;
	}
	return false;
}

/**
\brief
Apply N fertilizer, on a_field owned by a stock farmer
*/
bool Farm::FA_N(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fa_n);
		return true;
	}
	return false;
}

/**
\brief
Apply K fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_K(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fp_k);
		return true;
	}
	return false;
}


/**
\brief
Apply NS fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_NS(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fp_ns);
		return true;
	}
	return false;
}

/**
\brief
Apply NC fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_NC(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fp_nc);
		return true;
	}
	return false;
}

/**
\brief
Apply NK fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_NK(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fp_nk);
		return true;
	}
	return false;
}

/**
\brief
Apply NK fertilizer, on a_field owned by an stock farmer
*/
bool Farm::FA_NK(LE* a_field, double /*a_user*/, int a_days)
{

	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fa_nk);
		return true;
	}
	return false;
}


/**
\brief
Apply SK fertilizer, on a_field owned by an arable farmer
*/
bool Farm::FP_SK(LE* a_field, double /*a_user*/, int a_days)
{
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, fp_sk);
		return true;
	}
	return false;
}


/**
\brief
Apply liquid ammonia fertilizer to a_field owned by an arable farmer
*/
bool Farm::FP_LiquidNH3(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_liquidNH3;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}


/**
\brief
Apply slurry to a_field owned by an arable farmer
*/
bool Farm::FP_Slurry(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_slurry;
  if ( (0 >= a_days) || ((g_weather->GetTemp()>0)&&
                                 !g_weather->Raining() && DoIt(DO_IT_PROB)))
{
	  SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}


/**
\brief
Apply Manganse Sulphate to a_field owned by an arable farmer
*/
bool Farm::FP_ManganeseSulphate(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_manganesesulphate;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Apply soluble Boron to a_field owned by an arable farmer
*/
bool Farm::FP_Boron(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_boron;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply soluble Boron to a_field owned by a stock farmer
*/
bool Farm::FA_Boron(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_boron;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply Ammonium Sulphate to a_field owned by an arable farmer
*/
bool Farm::FP_AmmoniumSulphate(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_ammoniumsulphate;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Spread manure on a_field owned by an arable farmer
*/
bool Farm::FP_Manure(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_manure;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}


/**
\brief
Spread green manure on a_field owned by an arable farmer
*/
bool Farm::FP_GreenManure(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_greenmanure;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}


/**
\brief
Spread sewege on a_field owned by an arable farmer
*/
bool Farm::FP_Sludge(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_sludge;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
RSM (ammonium nitrate solution) applied on a_field owned by an arable farmer
*/
bool Farm::FP_RSM(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_rsm;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Calcium applied on a_field owned by an arable farmer
*/
bool Farm::FP_Calcium(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_calcium;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply NPKS fertilizer, on a_field owned by a stock farmer
*/
bool Farm::FA_NPKS(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_npks;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply NPK fertilizer to a_field owned by an stock farmer
*/
bool Farm::FA_NPK(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_npk;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply PKS fertilizer to a_field owned by an stock farmer
*/
bool Farm::FA_PKS(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_pks;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply PK fertilizer to a_field owned by an stock farmer
*/
bool Farm::FA_PK(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_pk;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply P fertilizer, on a_field owned by stock farmer
*/
bool Farm::FA_P(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_p;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply K fertilizer, on a_field owned by stock farmer
*/
bool Farm::FA_K(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_k;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply SK fertilizer, on a_field owned by an stock farmer
*/
bool Farm::FA_SK(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_sk;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Spready slurry on a_field owned by an stock farmer
*/

bool Farm::FA_Slurry(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_slurry;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply ammonium sulphate to a_field owned by an stock farmer
*/
bool Farm::FA_AmmoniumSulphate(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_ammoniumsulphate;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply manganese sulphate to a_field owned by an stock farmer
*/
bool Farm::FA_ManganeseSulphate(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_manganesesulphate;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}


/**
\brief
Spread manure on a_field owned by an stock farmer
*/
bool Farm::FA_Manure(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_manure;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}


/**
\brief
Spread green manure on a_field owned by an stock farmer
*/
bool Farm::FA_GreenManure(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_greenmanure;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}


/**
\brief
Spread sewege sludge on a_field owned by an stock farmer
*/
bool Farm::FA_Sludge(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_sludge;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
RSM (ammonium nitrate solution) applied on a_field owned by a stock farmer
*/
bool Farm::FA_RSM(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_rsm;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Calcium applied on a_field owned by a stock farmer
*/
bool Farm::FA_Calcium(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_calcium;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}


/**
\brief
Carry out a harrowing between crop rows on a_field
*/
bool Farm::RowCultivation(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = row_cultivation;
	if ((0 >= a_days) && (g_weather->GetRainPeriod(g_date->Date(), 3) < 0.1))
	{
		// Too much rain, just give up and claim we did it.
		
		return true;
	}

	if ((0 >= a_days) ||
		((g_weather->GetRainPeriod(g_date->Date(), 3) < 0.1) && DoIt(DO_IT_PROB))
		) {
		SetFunctionData(a_field, 0.25, 0.5, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Carry out a mechanical weeding on a_field
*/
bool Farm::Strigling(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = strigling;
	// Force strigling if it has not been done already!!!  This happens regardless of weather as of 26/10/2005
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 3) < 0.1) && DoIt(DO_IT_PROB))
		) {
		LE* um = SetFunctionData(a_field, 0.7, 0.05, EL_TRAMLINE_DECAYTIME, treatment);
		a_field->SetHerbicideDelay(EL_STRIGLING_DELAYTIME); // this delays regrowth of weed for a while
		if (um != nullptr) {
			um->SetHerbicideDelay(EL_STRIGLING_DELAYTIME);
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out a mechanical weeding followed by sowing on a_field
*/
bool Farm::StriglingSow( LE *a_field, double /*a_user*/, int a_days, double a_seed_coating_amount, PlantProtectionProducts a_ppp)
{
	FarmToDo treatment = strigling_sow;
	if ((0 >= a_days))// && (g_weather->GetRainPeriod(g_date->Date(),3)>0.1) )
	{
		LE* um = SetFunctionData(a_field, 0.7, 0.05, EL_TRAMLINE_DECAYTIME, treatment);
		a_field->SetHerbicideDelay(EL_STRIGLING_DELAYTIME); // this delays regrowth of weed for a while
		a_field->SetGrowthPhase(sow);
		a_field->SetLastSownVeg(a_field->GetVegType());
		if (um != nullptr) {
			um->SetHerbicideDelay(EL_STRIGLING_DELAYTIME);
			um->SetGrowthPhase(sow);
			um->SetLastSownVeg(a_field->GetVegType());
		}
		return true;
	}
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 3) < 0.1) && DoIt(DO_IT_PROB))
		) {
		LE* um = SetFunctionData(a_field, 0.7, 0.05, EL_TRAMLINE_DECAYTIME, treatment);
		a_field->SetGrowthPhase(sow);
		a_field->SetLastSownVeg(a_field->GetVegType());
		a_field->SetHerbicideDelay(EL_STRIGLING_DELAYTIME); // this delays regrowth of weed for a while
		if (um != nullptr)
		{
			um->SetHerbicideDelay(EL_STRIGLING_DELAYTIME);
			um->SetGrowthPhase(sow);
			um->SetLastSownVeg(a_field->GetVegType());
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out a mechanical weeding on a_field followed by hilling up (probably on potatoes)
*/
bool Farm::StriglingHill(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = strigling_hill;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.75, 0.25, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}


/**
\brief
Do hilling up on a_field, probably of potatoes
*/
bool Farm::HillingUp(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = hilling_up;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 0.75, 0.25, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Do bed forming up on a_field, probably of carrots
*/
bool Farm::BedForming(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = bed_forming;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 0.9, 0.0, EL_TRAMLINE_DECAYTIME, treatment);
		return true;
	}
	return false;
}

/**
\brief
Carry out a watering on a_field
*/
bool Farm::Water(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = water;

  /* Turn on this code to avoid watering on heavy soils
  int soiltype = a_field->GetSoilType();
  if ( soiltype < 1 || soiltype > 4 )
    return true;
*/
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Cut the crop on a_field and leave it lying (probably rape)
*/
bool Farm::Swathing(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = swathing;
  if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	  SetFunctionData(a_field, 0.5, 1.0, 0, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Carry out a harvest on a_field
*/
bool Farm::Harvest(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = harvest;
	//5 days good weather before
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5) < 0.1) && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 0.4, 1.0, 0, treatment);
		float biomassBefore = a_field->GetVegBiomass();
		a_field->SetGrowthPhase(harvest1);
		// Here we have to do a little skip to avoid too low insect populations after harvest, but a correct veg biomass
		double insects = a_field->GetInsectPop();
		a_field->RecalculateBugsNStuff();
		//else (cout << "Before: " << biomassBefore << ", after: " << a_field->GetVegBiomass());
		a_field->ResetDigestibility();
		a_field->SetInsectPop(insects);
		// Are we a cereal crop? If so we need to have some spilled grain
		double someseed = 0.0;
		double somemaize = 0.0;
		if (dynamic_cast<VegElement*>(a_field)->IsMatureCereal()) {
			someseed = m_OurManager->GetSpilledGrain();
			a_field->SetStubble(true);
		}
		else if (dynamic_cast<VegElement*>(a_field)->IsMaize()) {
			// Maize in stubble
			a_field->SetStubble(true);
			somemaize = m_OurManager->GetSpilledMaize();
		}
		a_field->SetBirdSeed(someseed);
		a_field->SetBirdMaize(somemaize);
		if (um != nullptr) {
			um->SetGrowthPhase(harvest1);
			double biomassBefore = um->GetVegBiomass();
			um->RecalculateBugsNStuff();
			um->ResetDigestibility();
			um->SetInsectPop(insects);
			um->SetBirdSeed(someseed);
			um->SetBirdMaize(somemaize);
			if ((somemaize > 0) || (someseed > 0)) um->SetStubble(true);
		}
		return true;
	}
	return false;
}


/**
\brief
Carry out a harvest on a_field (only differs in the DoIt chance cf harvest)
*/
bool Farm::HarvestLong(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = harvest;
	//5 days good weather before
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5)<0.1) && DoIt(DO_IT_PROB_LONG))
		) {
		LE* um = SetFunctionData(a_field, 0.4, 1.0, 0, treatment);
		a_field->SetGrowthPhase(harvest1);
		// Here we have to do a little skip to avoid too low insect populations after harvest, but a correct veg biomass
		double insects = a_field->GetInsectPop();
		double biomassBefore = a_field->GetVegBiomass();
		a_field->RecalculateBugsNStuff();
		a_field->ResetDigestibility();
		a_field->SetInsectPop(insects);
		// Are we a cereal crop? If so we need to have some spilled grain
		double someseed = 0.0;
		double somemaize = 0.0;
		if (dynamic_cast<VegElement*>(a_field)->IsMatureCereal()) {
			someseed = m_OurManager->GetSpilledGrain();
			a_field->SetStubble(true);
		}
		else if (dynamic_cast<VegElement*>(a_field)->IsMaize()) {
			// Maize in stubble
			a_field->SetStubble(true);
			somemaize = m_OurManager->GetSpilledMaize();
		}
		a_field->SetBirdSeed(someseed);
		a_field->SetBirdMaize(somemaize);
		if (um != nullptr) {
			um->SetGrowthPhase(harvest1);
			double biomassBefore = um->GetVegBiomass();
			um->RecalculateBugsNStuff();
			um->ResetDigestibility();
			um->SetInsectPop(insects);
			um->SetBirdSeed(someseed);
			um->SetBirdMaize(somemaize);
			if ((somemaize > 0) || (someseed > 0)) um->SetStubble(true);
		}
		return true;
	}
	return false;
}


/**
\brief
Carry out straw chopping on a_field
*/
bool Farm::StrawChopping( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = straw_chopping;
  if ( (0 >= a_days) ||
       ((g_weather->GetRainPeriod(g_date->Date(),5)<0.1) && DoIt(DO_IT_PROB))
  ) {
	  SetFunctionData(a_field, 0.4, 1.0, 0, treatment);
	return true;
  }
  return false;
}

/**
\brief
Carry out hay turning on a_field
*/
bool Farm::HayTurning( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = hay_turning;
  if ( (0 >= a_days) ||
       ((g_weather->GetRainPeriod(g_date->Date(),5)<0.1) && DoIt(DO_IT_PROB))
  ) {
	  SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Carry out hay bailing on a_field
*/
bool Farm::HayBailing( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = hay_bailing;
  if ( (0 >= a_days) ||
       ((g_weather->GetRainPeriod(g_date->Date(),5)<0.1) && DoIt(DO_IT_PROB))
  ) {
	  SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Carry out stubble harrowing on a_field
*/
bool Farm::StubbleHarrowing( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = stubble_harrowing;
  if ( (0 >= a_days) ||
       ((g_weather->GetRainPeriod(g_date->Date(),3)<0.1) && DoIt(DO_IT_PROB))
  ) {
	  SetFunctionData(a_field, 0.25, 0.0, 0, treatment);
	  return true;
  }
  return false;
}

/**
\brief
Carry out a harrow event in the winter on a_field
*/
bool Farm::WinterHarrow(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = autumn_harrow;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		// Apply 90% mortality to the insects
		a_field->InsectMortality(0.1);
		// Reduce the vegetation to zero
		a_field->ZeroVeg();
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(treatment);
			um->InsectMortality(0.1);
			um->ZeroVeg();
		}
		
		return true;
	}
	return false;
}

/**
\brief
Burn stubble on a_field
*/
bool Farm::BurnStrawStubble(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = burn_straw_stubble;
	if ((0 >= a_days) && (g_weather->GetRainPeriod(g_date->Date(), 3) > 0.1))
	{
		return true;
	}
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 3) < 0.1)
		&& DoIt(DO_IT_PROB)))
	{
		SetFunctionData(a_field, 0.4, 0.2, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Burn tops of e.g. potatoes on a_field
*/
bool Farm::BurnTop(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = burn_top;
	if ((0 >= a_days) && (g_weather->GetRainPeriod(g_date->Date(), 3) > 0.1))
	{
		return true;
	}
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 3) < 0.1)
		&& DoIt(DO_IT_PROB)))
	{
		SetFunctionData(a_field, 0.4, 0.2, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Carry out hay cutting on a_field
*/
bool Farm::CutToHay(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = cut_to_hay;
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5) < 0.1)
		&& DoIt(DO_IT_PROB)))
	{
		LE* um = SetFunctionData(a_field, 0.4, 0.2, 0, treatment);
		a_field->SetGrowthPhase(harvest1);
		a_field->SetVegHeight(10);
		if (um != nullptr) {
			um->SetGrowthPhase(harvest1);
			um->SetVegHeight(10);
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out weed topping on a_field
*/
bool Farm::CutWeeds(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = cut_weeds;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB)))
	{
		SetFunctionData(a_field, 0.8, 0.8, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Cut vegetation for silage on a_field
*/
bool Farm::CutToSilage( LE *a_field, double /*a_user*/, int a_days )
{
	FarmToDo treatment = cut_to_silage;
    if ( (0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
	LE* um = SetFunctionData(a_field, 0.4, 0.2, 0, treatment);
    a_field->SetVegHeight( 10 );
	a_field->SetGrowthPhase(harvest1);
	a_field->ResetDigestibility();
	if (um != nullptr) {
		um->SetVegHeight(10);
		um->SetGrowthPhase(harvest1);
		um->ResetDigestibility();
	}
	return true;
  }
  return false;
}

/**
\brief
Cut vegetation on orchard crop. //based on cut to silage - values from cutting function of orchard
*/
bool Farm::CutOrch(LE* a_field, double /* a_user */, int a_days)
{
	FarmToDo treatment = mow;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 0.4, 0.2, 0, treatment);
		a_field->SetMownDecay(12); // 12 days of not suitable
		a_field->SetVegParameters(l_el_o_cut_height.value(), l_el_o_cut_total.value(), l_el_o_cut_green.value(), 0); //is 0 ok for weed biomass
		if (um != nullptr) {
			um->SetVegParameters(l_el_o_cut_height.value(), l_el_o_cut_total.value(), l_el_o_cut_green.value(), 0);
			um->SetMownDecay(12); // 12 days of not suitable
		}
		return true;
	}
	return false;
}



/**
\brief
Flower cutting applied on a_field
*/
bool Farm::FlowerCutting(LE * a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = flower_cutting;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 0.5, 0.8, EL_TRAMLINE_DECAYTIME, treatment);
		a_field->SetVegHeight(35);
		if (um != nullptr) {
			um->SetVegHeight(35);
		}
		return true;
	}
	return false;
}

/**
\brief
Carry out a bulb harvest on a_field
*/
bool Farm::BulbHarvest(LE *a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = bulb_harvest;
	//5 days good weather before
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5) < 0.1) && DoIt(DO_IT_PROB))) {
		LE* um = SetFunctionData(a_field, 0.3, 0.0, EL_TRAMLINE_DECAYTIME, treatment);
		double insects = a_field->GetInsectPop();
		a_field->SetGrowthPhase(harvest1);
		double biomassBefore = a_field->GetVegBiomass();
		a_field->RecalculateBugsNStuff();
		a_field->ResetDigestibility();
		if (um != nullptr) {
			um->SetGrowthPhase(harvest1);
			double biomassBefore = um->GetVegBiomass();
			um->RecalculateBugsNStuff();
			um->ResetDigestibility();
		}
		return true;
	}
	return false;
}

/**
\brief
Straw covering applied on a_field
*/
bool Farm::StrawCovering(LE * a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = straw_covering;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Straw covering applied on a_field
*/
bool Farm::StrawRemoval(LE * a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = straw_removal;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Fiber covering applied on a_field
*/
bool Farm::FiberCovering(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fiber_covering;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Fiber covering removed from a_field
*/
bool Farm::FiberRemoval(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fiber_removal;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Harvest bush fruit on a_field - no tramlines since fruits are picked by hand, the bush stays on field with same vegetation height, thus suspects same % insects 
*/
bool Farm::HarvestBushFruit(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = harvest_bushfruit;
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5) < 0.1)
		&& DoIt(DO_IT_PROB)))
	{
		LE* um = SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		a_field->SetGrowthPhase(harvest1);
		if (um != nullptr) {
			um->SetGrowthPhase(harvest1);
		}
		return true;
	}
	return false;
}

/**
\brief
Harvest bush fruit on a_field - tramlines since fruits are picked by machine, the bush stays on field with same vegetation height, thus suspects same % insects
*/
bool Farm::HarvestBF_Machine(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = harvest_bushfruit;
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5) < 0.1)
		&& DoIt(DO_IT_PROB)))
	{
		LE* um = SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		a_field->SetGrowthPhase(harvest1);
		if (um != nullptr) {
			um->SetGrowthPhase(harvest1);
		}
		return true;
	}
	return false;
}

/**
\brief
Start a irrigation event on a_field today
*/
bool Farm::IrrigationStart(LE *a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = water;
	/**
	* This is the main initiate irrigation method and as such is called at the moment irrigation is started.
	*/
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5) < 0.1) && DoIt(DO_IT_PROB)))
	{
		LE* um = SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
		a_field->ToggleIrrigation();
		if (um != nullptr) {
			um->ToggleIrrigation();
		}
		return true;
	}
	return false;
}

/**
\brief
Generate an 'irrigation' event with a frequency defined by a_freq in the irrigation period on a_field
*/
bool Farm::Irrigation(LE *a_field, double /*a_user*/, int a_days, int a_max)
{
	FarmToDo treatment = water;
	if (!g_weather->Raining())
	{
		LE* um = SetFunctionData(a_field, 1.0, 1.0, EL_TRAMLINE_DECAYTIME, treatment);
	}
	if ((g_date->DayInYear() > a_max) || (0 >= a_days))
	{
		a_field->ToggleIrrigation();
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			if (um != nullptr) {
				um->ToggleIrrigation();
			}
		}
		return true; // Only returns true when finsihed irrigation
	}
	return false;
}

/**
\brief
HarvestShoots applied on a_field (e.g. asparagus) - details needs to be added (e.g., impact on biomass, influence/impacts in species code)!
*/
bool Farm::HarvestShoots(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = harvestshoots;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Pruning applied on a_field - details needs to be added (e.g., impact on biomass, influence/impacts in species code)!
*/
bool Farm::Pruning(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = pruning;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Shredding (destruction of the pruning residues with a shredders using hammer mower) applied on a_field
- details needs to be added (e.g., impact on biomass, influence/impacts in species code)!
*/
bool Farm::Shredding(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = shredding;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
LeafThinning (leaf removal to increase areation and sun exposure of fruits, e.g., grapes) applied on a_field
- details needs to be added (e.g., impact on biomass, influence/impacts in species code)!
*/
bool Farm::LeafThinning(LE* a_field, double a_user, int a_days)
{
	FarmToDo treatment = pruning;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
GreenHarvest (remove of excess production that may affect the desired quality of the fruits, e.g., grapes) applied on a_field
- details needs to be added (e.g., impact on biomass, influence/impacts in species code)!
*/
bool Farm::GreenHarvest(LE* a_field, double a_user, int a_days)
{

	FarmToDo treatment = green_harvest;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}


/**
\brief
FruitHarvest (harvest of the mature fruits, e.g., grapes) applied on a_field
- details needs to be added (e.g., impact on biomass, influence/impacts in species code)!
*/
bool Farm::FruitHarvest(LE* a_field, double a_user, int a_days)
{

	FarmToDo treatment = harvest;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply Cu fertilizer, on a_field owned by plant farmer (orchard)
*/
bool Farm::FP_Cu(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fp_cu;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Apply Cu fertilizer, on a_field owned by stock farmer (orchard)
*/
bool Farm::FA_Cu(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fa_cu;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		SetFunctionData(a_field, 1.0, 1.0, 0, treatment);
		return true;
	}
	return false;
}

/**
\brief
Manual weeding on a_field - no tramlines since weeding by hand, the bush stays on field with same vegetation height, thus suspects same % insects
*/
bool Farm::ManualWeeding(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = manual_weeding;
	if ((0 >= a_days) || ((g_weather->GetRainPeriod(g_date->Date(), 5) < 0.1)
		&& DoIt(DO_IT_PROB)))
	{
		SetFunctionData(a_field, 1.0, 0.95, 0, treatment);
		return true;
	}
	return false;
}


/******************************************************************************************************************************************/
/************************** BELOW HERE WE HAVE THOSE FUNCTIONS THAT ARE NOT APPLIED TO UNSPRAYED MARGINS **********************************/
/******************************************************************************************************************************************/

/** Pesticide and similar in this next section ********************************************************************************************/

/**
\brief
Apply herbicide to a_field
*/
bool Farm::HerbicideTreat(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = herbicide_treat;
	if (0 >= a_days) // Must do this
	{
		if (((a_field->GetSignal() & LE_SIG_NO_HERBICIDE) == 0))
		{
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			if (l_farm_herbicide_kills.value())
			{
				a_field->ReduceWeedBiomass(0.05);
			}
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
			a_field->SetHerbicideDelay(EL_HERBICIDE_DELAYTIME);
		}
		
		return true;
	}
	else if ((g_weather->GetWind() < 4.5) && (!g_weather->Raining()) && DoIt(DO_IT_PROB))
	{
		if (!(a_field->GetSignal() & LE_SIG_NO_HERBICIDE))
		{
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			if (l_farm_herbicide_kills.value())
			{
				a_field->ReduceWeedBiomass(0.05);
			}
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
			a_field->SetHerbicideDelay(EL_HERBICIDE_DELAYTIME);
		}
		
		return true;
	}
	return false;
}

/**
\brief
Apply growth regulator to a_field
*/
bool Farm::GrowthRegulator(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = growth_regulator;
	if (0 >= a_days)
	{

		if ((!g_weather->Raining()) && (g_weather->GetWind() < 4.5) && (!(a_field->GetSignal() & LE_SIG_NO_GROWTH_REG))) {
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	else if ((g_weather->GetWind() < 4.5) &&
		(!g_weather->Raining()) && DoIt(DO_IT_PROB)) {
		if (!(a_field->GetSignal() & LE_SIG_NO_GROWTH_REG)) {
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	return false;
}

/**
\brief
Apply fungicide to a_field
*/
bool Farm::FungicideTreat(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = fungicide_treat;
	if (0 >= a_days)
	{
		if ((!a_field->GetSignal() & LE_SIG_NO_FUNGICIDE))
		{
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	else if ((g_weather->GetWind() < 4.5) &&
		(!g_weather->Raining()) && DoIt(DO_IT_PROB)) {
		if (!(a_field->GetSignal() & LE_SIG_NO_FUNGICIDE)) {
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	return false;
}


/**
\brief
Apply insecticide to a_field
*/
bool Farm::InsecticideTreat(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = insecticide_treat;
	if (0 >= a_days)
	{
		if ((!(a_field->GetSignal() & LE_SIG_NO_INSECTICIDE)))
		{
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			if (l_farm_insecticide_kills.value())
			{
				a_field->Insecticide(0.36);
			}
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	else if ((g_weather->GetWind() < 4.5) &&
		(!g_weather->Raining()) && DoIt(DO_IT_PROB)) {
		if (!(a_field->GetSignal() & LE_SIG_NO_INSECTICIDE)) {
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			if (l_farm_insecticide_kills.value()) {
				a_field->Insecticide(0.36);
			}
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	return false;
}

/**
\brief
Apply Biocide to a_field
*/
bool Farm::BiocideTreat(LE* a_field, double /*a_user*/, int a_days)
{
	constexpr FarmToDo treatment = biocide;
	if ((0 >= a_days) || ((g_weather->GetWind() < 4.5) && (!g_weather->Raining()) && DoIt(DO_IT_PROB))) {
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		if (l_farm_insecticide_kills.value()) {
			a_field->Insecticide(1-(0.64*cfg_biocide_reduction_val.value()));
		}
		a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		
		return true;
	}
	return false;
}

/**
\brief
Biocide applied on a_field
*/
bool Farm::OrganicInsecticide(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = org_insecticide;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		/* LKM: Assume not applied for unsprayed margins
		*/		
		return true;
	}
	return false;
}

/**
\brief
Biocide applied on a_field
*/
bool Farm::OrganicHerbicide(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = org_herbicide;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		/* LKM: Assume not applied for unsprayed margins
		*/		
		return true;
	}
	return false;
}

/**
\brief
Biocide applied on a_field
*/
bool Farm::OrganicFungicide(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = org_fungicide;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		/* LKM: Assume not applied for unsprayed margins
		*/		
		return true;
	}
	return false;
}

/**
\brief
Pheromone applied on a_field - same as with org. pesticides for now
*/
bool Farm::Pheromone(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = pheromone;
	if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		/* LKM: Assume not applied for unsprayed margins
		*/		
		return true;
	}
	return false;
}


/**
\brief
Apply molluscicide to a_field
*/
bool Farm::Molluscicide(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = molluscicide;
	if (0 >= a_days) {
		if ((!g_weather->Raining()) && (g_weather->GetWind() < 4.5) &&
			(!(a_field->GetSignal() & LE_SIG_NO_MOLLUSC))) {
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	else if ((0 >= a_days) || (!g_weather->Raining() && DoIt(DO_IT_PROB))) {
		if (!(a_field->GetSignal() & LE_SIG_NO_MOLLUSC)) {
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			a_field->SetTramlinesDecay(EL_TRAMLINE_DECAYTIME);
		}
		
		return true;
	}
	return false;
}

/******************************************************************************************************************************************/
/****************************************** GRAZING MANAGEMENT BELOW THIS LINE **/
/******************************************************************************************************************************************/

/**
\brief
Start a grazing event on a_field today
*/
bool Farm::CattleOut(LE* a_field, double /*a_user*/, int a_days)
{
	/**
	* This is the main initiate grazing method and as such is called by all grazed field management plans at the moment cattle are put out.
	*/
	FarmToDo treatment = cattle_out;
	if ((0 >= a_days) || DoIt(DO_IT_PROB))
	{
		a_field->ToggleCattleGrazing();
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		// Reduce the vegetation because of grazing
		//double h=a_field->GetVegHeight();
		//double reduc = 1-(l_farm_cattle_veg_reduce.value()*((h-15)/15));
		//a_field->ReduceVeg_Extended( reduc );
		a_field->GrazeVegetation(l_farm_cattle_veg_reduce.value(), false);
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1)
		{
			// Must have an unsprayed margin so need to pass the information on to it
			// This happens if all arable fields are given unsprayed margins - they have no effect on grass unless it is sprayed with pesticides
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->ToggleCattleGrazing();
			um->SetLastTreatment(treatment);
			um->GrazeVegetation(l_farm_cattle_veg_reduce.value(), false);
		}
		
		return true;
	}
	return false;
}

/**
\brief
Start an intensive grazing event on a_field today
*/
bool Farm::CattleOut(LE* a_field, double /*a_user*/, int a_days, bool a_should_eat)
{
	/**
	* This is the main initiate grazing method and as such is called by all grazed field management plans at the moment cattle are put out.
	*/

	FarmToDo treatment = cattle_out;
	if ((0 >= a_days) || DoIt(DO_IT_PROB))
	{
		a_field->ToggleCattleGrazing();
		if (a_should_eat){
			a_field->Trace(treatment);
			a_field->SetLastTreatment(treatment);
			a_field->GrazeVegetationHeight(cfg_farm_cattle_reduce_height.value(a_field->GetCattleGrazing()-1));
			int pref = a_field->GetUnsprayedMarginPolyRef();
			if (pref != -1)
			{
				// Must have an unsprayed margin so need to pass the information on to it
				//This happens if all arable fields are given unsprayed margins - they have no effect on grass unless it is sprayed with pesticides
				LE* um = g_landscape_ptr->SupplyLEPointer(pref);
				um->ToggleCattleGrazing();
				um->SetLastTreatment(treatment);
				um->GrazeVegetationHeight(cfg_farm_cattle_reduce_height.value(a_field->GetCattleGrazing()-1));
			}
			
		}
		return true;
	}
	return false;
}

/**
\brief
Start a extensive grazing event on a_field today
*/
bool Farm::CattleOutLowGrazing(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = cattle_out_low;
	if ((0 >= a_days) || DoIt(DO_IT_PROB)) {
		a_field->ToggleCattleGrazing();
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		// Reduce the vegetation because of grazing
		//double h=a_field->GetVegHeight();
		//double reduc = 1-(l_farm_cattle_veg_reduce.value()*((h-15)/15));
		//a_field->ReduceVeg_Extended( reduc );
		a_field->GrazeVegetation(l_farm_cattle_veg_reduce2.value(), false);
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			   // This happens if all arable fields are given unsprayed margins - they have no effect on grass unless it is sprayed with pesticides
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->ToggleCattleGrazing();
			um->SetLastTreatment(treatment);
			um->GrazeVegetation(l_farm_cattle_veg_reduce2.value(), false);
		}
		
		return true;
	}
	return false;
}

/**
\brief
Generate a 'cattle_out' event for every day the cattle are on a_field
*/
bool Farm::CattleIsOut(LE* a_field, double /*a_user*/, int a_days, int a_max)
{
	/**
	* This is the main grazing method and as such is called by all grazed field management plans each day the cattle are out.
	*/
	FarmToDo treatment = cattle_out;
	double biomass = a_field->GetVegBiomass();
	if (biomass > 100.0)
	{
		a_field->SetLastTreatment(treatment);
		a_field->Trace(treatment);
		// Reduce the vegetation because of grazing
		//double h=a_field->GetVegHeight();
		//double reduc = 1-(l_farm_cattle_veg_reduce.value()*((h-15)/15));
		//a_field->ReduceVeg_Extended( reduc );
		a_field->GrazeVegetation(l_farm_cattle_veg_reduce.value(), false);
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1)
		{
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(treatment);
			um->GrazeVegetation(l_farm_cattle_veg_reduce.value(), false);
		}
	}
	int d1 = g_date->DayInYear(10, 9);
	if (d1 > a_max)	d1 = a_max;
	if ((((g_date->DayInYear() > d1) && (10 >= a_days)) && DoIt(10)) || (0 >= a_days))
	{
		a_field->ToggleCattleGrazing();
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->ToggleCattleGrazing();
		}
		
		return true;
	}
	return false;
}

/**
\brief
Generate a 'cattle_out' event for every day the cattle are on a_field
*/
bool Farm::CattleIsOut(LE* a_field, double /*a_user*/, int a_days, int a_max, bool a_should_eat)
{
	/**
	* This is the main grazing method and as such is called by all grazed field management plans each day the cattle are out.
	*/

	FarmToDo treatment = cattle_out;
	if (a_should_eat) {
		a_field->SetLastTreatment(treatment);
		a_field->Trace(treatment);
		a_field->GrazeVegetationHeight(cfg_farm_cattle_reduce_height.value(a_field->GetCattleGrazing()-1));
		a_field->SetGrowthPhase(harvest2);
	
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1)
		{
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(treatment);
			um->GrazeVegetationHeight(cfg_farm_cattle_reduce_height.value(a_field->GetCattleGrazing()-1));
			um->SetGrowthPhase(harvest2);
		}
	}
	int d1 = g_date->DayInYear(10, 9);
	if (d1 > a_max)	d1 = a_max;
	if ((((g_date->DayInYear() > d1) && (10 >= a_days)) && DoIt(10)) || (0 >= a_days))
	{
		a_field->ToggleCattleGrazing();
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->ToggleCattleGrazing();
		}
		
		return true;
	}
	return false;
}


/**
\brief
Generate a 'cattle_out_low' event for every day the cattle are on a_field
*/
bool Farm::CattleIsOutLow(LE* a_field, double /*a_user*/, int a_days, int a_max)
{
	FarmToDo treatment = cattle_out_low;
	// Generate a 'cattle_in_out' event for every day the cattle is on the
	// field.
	double biomass = a_field->GetVegBiomass();
	if (biomass > 100.0)
	{
		a_field->SetLastTreatment(treatment);
		a_field->Trace(treatment);
		// Reduce the vegetation because of grazing
		//double h=a_field->GetVegHeight();
		//double reduc = 1-(l_farm_cattle_veg_reduce2.value()*((h-15)/15));
		//a_field->ReduceVeg_Extended( reduc );
		a_field->GrazeVegetation(l_farm_cattle_veg_reduce2.value(), false);
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1)
		{
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(treatment);
			um->GrazeVegetation(l_farm_cattle_veg_reduce2.value(), false);
		}
	}
	int d1 = g_date->DayInYear(10, 9);
	if (d1 > a_max) d1 = a_max;
	if ((((g_date->DayInYear() > d1) && (10 >= a_days)) && DoIt(10)) || (0 >= a_days))
	{
		a_field->ToggleCattleGrazing();
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->ToggleCattleGrazing();
		}
		
		return true;
	}
	return false;
}

/**
\brief
Generate a 'cattle_out_low2' event for every day the cattle are on a_field, cattle is low grazing and can stay on the field till a_max date but no more days than defined by a_max_days
a_days defines the number of days from the day the cattle is put on the field (set by a flag in the management plan)
*/
bool Farm::CattleIsOutLow2(LE* a_field, double /*a_user*/, int a_days, int a_max, int a_max_days)
{
	FarmToDo treatment = cattle_out_low;
	// Generate a 'cattle_in_out' event for every day the cattle is on the
	// field.
	double biomass = a_field->GetVegBiomass();
	if (biomass > 100.0)
	{
		a_field->SetLastTreatment(treatment);
		a_field->Trace(treatment);
		// Reduce the vegetation because of grazing
		//double h=a_field->GetVegHeight();
		//double reduc = 1-(l_farm_cattle_veg_reduce2.value()*((h-15)/15));
		//a_field->ReduceVeg_Extended( reduc );
		a_field->GrazeVegetation(l_farm_cattle_veg_reduce2.value(), false);
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1)
		{
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(treatment);
			um->GrazeVegetation(l_farm_cattle_veg_reduce2.value(), false);
		}
	}
	if ((g_date->DayInYear() > a_max) || (a_max_days < a_days))
	{
		a_field->ToggleCattleGrazing();
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->ToggleCattleGrazing();
		}
		
		return true;
	}
	return false;
}

/**
\brief
Generate a 'pigs_out' event for every day the cattle are on a_field
*/
bool Farm::PigsOut(LE* a_field, double /*a_user*/, int a_days)
{
	FarmToDo treatment = pigs_out;
	if ((0 >= a_days) || DoIt(DO_IT_PROB))
	{
		a_field->TogglePigGrazing();
		a_field->Trace(treatment);
		a_field->SetLastTreatment(treatment);
		// Reduce the vegetation because of grazing
		a_field->ReduceVeg_Extended(l_farm_pig_veg_reduce.value());
		// make this a function of grazing pressure
		//and field size - perhaps in a later life
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->SetLastTreatment(pigs_out);
			um->ReduceVeg_Extended(l_farm_pig_veg_reduce.value());
		}
		
		return true;
	}
	return false;
}

/**
\brief
Start a pig grazing event on a_field today - no exceptions
*/
bool Farm::PigsAreOutForced(LE* a_field, double /*a_user*/, int /*a_days*/)
{
	FarmToDo treatment = pigs_out;
	a_field->SetLastTreatment(treatment);
	a_field->Trace(treatment);
	// Reduce the vegetation because of grazing
	a_field->ReduceVeg_Extended(l_farm_pig_veg_reduce.value());
	// make this a function of grazing pressure
	//and field size - perhaps in a later life
	int pref = a_field->GetUnsprayedMarginPolyRef();
	if (pref != -1) {
		// Must have an unsprayed margin so need to pass the information on to it
		LE* um = g_landscape_ptr->SupplyLEPointer(pref);
		um->SetLastTreatment(treatment);
		um->ReduceVeg_Extended(l_farm_pig_veg_reduce.value());
	}
	 // should be there a return true?
	return false;
}

/**
\brief
Start a pig grazing event on a_field today or soon
*/
bool Farm::PigsAreOut(LE* a_field, double /*a_user*/, int a_days)
{

	if ((0 >= a_days) || DoIt(50 / a_days)) {
		a_field->TogglePigGrazing();
		int pref = a_field->GetUnsprayedMarginPolyRef();
		if (pref != -1) {
			// Must have an unsprayed margin so need to pass the information on to it
			LE* um = g_landscape_ptr->SupplyLEPointer(pref);
			um->TogglePigGrazing();
		}
		return true;
	}
	return false;
}
