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

#define _CRT_SECURE_NO_DEPRECATE



#define __WEED_CURVE 99 //99  // 99 is the weed curve

// The default no better information values (Mean of the four crop values)
#define EL_BUG_PERCENT_A 0.0
#define EL_BUG_PERCENT_B 0.2975
#define EL_BUG_PERCENT_C 0.095916647275
#define EL_BUG_PERCENT_D 0

// SBarley
#define EL_BUG_PERCENT_SB_A 0
#define EL_BUG_PERCENT_SB_B 0.380763296
#define EL_BUG_PERCENT_SB_C 0
#define EL_BUG_PERCENT_D 0

// WWheat
#define EL_BUG_PERCENT_WW_A 0.0
#define EL_BUG_PERCENT_WW_B 0.1283
#define EL_BUG_PERCENT_WW_C 0.0
#define EL_BUG_PERCENT_D 0

// WRye
#define EL_BUG_PERCENT_WRy_A 0.0
#define EL_BUG_PERCENT_WRy_B 0.395651915
#define EL_BUG_PERCENT_WRy_C 0.0
#define EL_BUG_PERCENT_D 0

//WRape
#define EL_BUG_PERCENT_WR_A 0.0
#define EL_BUG_PERCENT_WR_B 0.028271643
#define EL_BUG_PERCENT_WR_C 0.0
#define EL_BUG_PERCENT_D 0

//Cropped/Grazed Grass
#define EL_BUG_PERCENT_G_A 4.123817127
#define EL_BUG_PERCENT_G_B 0.151015629
#define EL_BUG_PERCENT_G_C -0.228228353
#define EL_BUG_PERCENT_D 0

//Setaside
#define EL_BUG_PERCENT_SA_A 10.72459109
#define EL_BUG_PERCENT_SA_B 0.4
#define EL_BUG_PERCENT_SA_C 2.529631141
#define EL_BUG_PERCENT_D 0

//Edges
#define EL_BUG_PERCENT_Edges_A 10.72459109
#define EL_BUG_PERCENT_Edges_B 0.8
#define EL_BUG_PERCENT_Edges_C 2.529631141
#define EL_BUG_PERCENT_D 0

#include <math.h>
#include "../Landscape/ls.h"
#include "Elements.h"


using namespace std;

extern TTypesOfPopulation g_Species;

extern class PollenNectarDevelopmentData * g_nectarpollen;
extern std::shared_ptr<Population_Manager_Base> g_AManager;
extern CfgArray_Double cfg_FloweringPeriodPhasesProportionArray;
extern CfgArray_Double cfg_FloweringPeriodPhasesLengthArray;

//extern void FloatToDouble(double &, float);
extern CfgInt cfg_pest_productapplic_startdate;
extern CfgInt cfg_pest_productapplic_period;
extern CfgInt cfg_farm_cattle_grass_low;

/** \brief First dates of cutting for field boundaries with grass.*/
static CfgArray_Int cfg_field_boundary_cut_start("FIELD_BOUNDARY_CUT_START", CFG_CUSTOM, 2, vector<int> {9999, 9999});
/** \brief Last dates of cutting for field boundaries with grass.*/
static CfgArray_Int cfg_field_boundary_cut_end("FIELD_BOUNDARY_CUT_END", CFG_CUSTOM, 2, vector<int> {120, 300});
/** \brief The daily chance for cutting of field boundaries with grass when it is within the cutting period.*/
static CfgArray_Double cfg_field_boundary_cut_chance("FIELD_BOUNDARY_CUT_CHANCE", CFG_CUSTOM, 2, vector<double> {0.5, 0.5});
/** \brief First date of cutting for flower strips */
static CfgInt cfg_flowerstripCutStart("ELE_FLOWERSTRIPCUT_START", CFG_CUSTOM, 366); // Default no cut 
/** \brief Last possible date of cutting for flower strips */
static CfgInt cfg_flowerstripCutEnd("ELE_FLOWERSTRIPCUT_END", CFG_CUSTOM, -1); // Default no cut 
/** \brief If after first date of cutting for flower strips, this is the daily chance it happens */
static CfgFloat cfg_flowerstripCutChance("ELE_FLOWERSTRIPCUT_CHANCE", CFG_CUSTOM, 0.0); // Default no cut 
/** \brief Flag to determine whether nectar and pollen models are used - should be set to true for pollinator models! */
CfgBool cfg_pollen_nectar_on("ELE_POLLENNECTAR_ON", CFG_CUSTOM, false);
/** \brief Flag to determine whether to calculate pond pesticide concentration */
CfgBool cfg_calc_pond_pesticide("POND_PEST_CALC_ON", CFG_CUSTOM, false);
/** \brief The multiplication factor assumed to account for ingress of pesticide from run-off and soil water to a pond*/
CfgFloat cfg_pondpesticiderunoff("POND_PEST_RUNOFFFACTOR", CFG_CUSTOM, 10.0);
/** \brief Controls whether random pond quality is used */
CfgBool cfg_randompondquality("POND_RANDOMQUALITY", CFG_CUSTOM, false);
/** \brief The number of days a goose count can be used */
CfgInt cfg_goosecountperiod("GOOSE_GOOSECOUNTPERIOD",CFG_CUSTOM,1);
/** \brief Scales the growth of vegetation - max value */
CfgFloat cfg_PermanentVegGrowthMaxScaler("VEG_GROWTHSCALERMAX", CFG_CUSTOM, 1.0);
/** \brief Scales the growth of vegetation - min value */
CfgFloat cfg_PermanentVegGrowthMinScaler("VEG_GROWTHSCALERMIN", CFG_CUSTOM, 1.0);
/** \brief Scales the growth of vegetation - max value */
CfgFloat cfg_PermanentVegGrowthMaxScalerField("VEG_FIELDGROWTHSCALERMAX", CFG_CUSTOM, 1.100);
/** \brief Scales the growth of vegetation - min value */
CfgFloat cfg_PermanentVegGrowthMinScalerField("VEG_FIELDGROWTHSCALERMIN", CFG_CUSTOM, 0.9);
/** \brief A scaling value used to change the insext density crop relationship which was based on DK 2000 data */
CfgFloat cfg_insectbiomassscaling("ELE_INSECTBIOMASSSCALER", CFG_CUSTOM, 0.5);
/** \brief Starting month for varying base development temperature for flower resource model. */
CfgInt cfg_month_varying_flower_base_temp("MONTH_VARING_FLOWER_BASE_TEMP", CFG_CUSTOM, 10);
/** \brief Flower resource base temperature increment per month from the starting month. */
CfgFloat cfg_base_temp_increment_flower("BASE_TEMP_INCREMENT_FLOWER", CFG_CUSTOM, 5);
/** \brief Weight to shorten flowering period and decreasing resource amount when it is cold. */
CfgFloat cfg_weight_cold_flower_resource("WEIGHT_CODE_FLOWER_RESOURCE", CFG_CUSTOM, 0.8);
/** \brief Growth stage for non-crop habitat, this is used for aphid*/
CfgFloat cfg_noncrop_growth_stage_with_green_biomass("NONCROP_GROWTH_STAGE_WITH_GREEN_BIOMASS", CFG_CUSTOM, 50);
/** \brief The extiction coefficient that is used in Beer's law. */
CfgFloat cfg_beer_law_extinction_coef("BEER_LAW_EXTINCTION_COEF", CFG_CUSTOM, 0.6);
/** \brief Monthly base development temperature for pollen.*/
CfgArray_Double cfg_PollenMonthBaseTemp("POLLEN_MONTH_BASE_TEMP", CFG_CUSTOM, 12, vector<double> {3.9, 3.9, 4, 3.5, 8, 9, 9, 6, 5, 0, 2, 2});
/** \brief Monthly base development temperature for NECTAR.*/
CfgArray_Double cfg_NectarMonthBaseTemp("NECTAR_MONTH_BASE_TEMP", CFG_CUSTOM, 12, vector<double> {3.6, 3.6, 3.9, 3.9, 9, 9, 9, 6, 5, 0, 2, 2});
/** \brief The interested biomass fraction for clover crop.*/
CfgFloat cfg_clover_interested_biomass_fraction("CLOVER_INTERESTED_BIOMASS_FRACTION", CFG_CUSTOM, 0.4);

const double c_SolarConversion[ 2 ] [ 81 ] = {
   {
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.28,
     0.56,0.84,1.12,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,1.4,
     1.4,1.4,1.26,1.12,0.98,0.84,0.7,0.56,0.42,0.28,0.14,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0
   },
   {
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0.242857,0.485714,0.728571,0.971429,1.214286,1.457143,1.7,1.7,1.7,1.7,
     1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,1.7,
     1.53,1.36,1.19,1.02,0.85,0.68,0.51,0.34,0.17,0,0,0,0,0,0
   }
  };

//extern CfgFloat cfg_goose_GrainDecayRateWinter;
//extern CfgFloat cfg_goose_GrainDecayRateSpring;
//extern CfgFloat cfg_goose_grass_to_winter_cereal_scaler;
/** \brief The decay rate for spilled grain for Harvest to Spring*/
CfgFloat cfg_goose_GrainDecayRateWinter{"GOOSE_GRAINDECAYRATEWINTER", CFG_CUSTOM, 0.958, 0.0, 1.0};// Halflife of 463 days
/** \brief The decay rate for spilled maize for Harvest to Spring*/
CfgFloat cfg_goose_MaizeDecayRateWinter{"GOOSE_MAIZEDECAYRATEWINTER", CFG_CUSTOM, 0.9, 0.0, 1.0};//
/** \brief The decay rate for spilled grain for  Spring*/
CfgFloat cfg_goose_MaizeDecayRateSpring{"GOOSE_MAIZEDECAYRATESPRING", CFG_CUSTOM, 0.9, 0.0, 1.0};//
/** \brief The decay rate for spilled grain for Spring until 1st July */
CfgFloat cfg_goose_GrainDecayRateSpring{"GOOSE_GRAINDECAYRATESPRING", CFG_CUSTOM, 0.95, 0.0, 1.0}; // Halflife of approx 17 days
/** \brief Use the same grain and maize decay rate (the winter one also for spring) */
CfgBool cfg_goose_UniformDecayRate{"GOOSE_UNIFORMDECAYRATE", CFG_CUSTOM, true};
/** \brief The scaler to go from energy intake from grass forage to winter cereal
* The default value of 1.0325 is a quick fix to account for higher energy intake on winter cereal - Based on Therkildsen & Madsen 2000 Energetics of feeding...*/
CfgFloat cfg_goose_grass_to_winter_cereal_scaler{"GOOSE_GRASS_TO_WINTER_CEREAL_SCALER", CFG_CUSTOM, 1.0325, 0.0,
                                                 10.0};


static CfgFloat cfg_beetlebankinsectscaler("ELE_BBINSECTSCALER",CFG_CUSTOM,1.0); // 1.0 means beetlebank is the same as hedgebank
static double g_weed_percent[ tov_Undefined ];
static double g_bug_percent_a[ tov_Undefined ];
static double g_bug_percent_b[ tov_Undefined ];
static double g_bug_percent_c[ tov_Undefined ];
static double g_bug_percent_d[ tov_Undefined ];
static CfgInt cfg_OrchardSprayDay( "TOX_ORCHARDSPRAYDAY", CFG_CUSTOM, 150 );
static CfgInt cfg_OrchardSprayDay2( "TOX_ORCHARDSPRAYDAYTWO", CFG_CUSTOM, 200000 );
CfgInt cfg_OrchardNoCutsDay( "TOX_ORCHARDNOCUTS", CFG_CUSTOM, -1 );
static CfgInt cfg_MownGrassNoCutsDay( "ELE_MOWNGRASSNOCUTS", CFG_CUSTOM, 2 );
static CfgInt cfg_UMPatchyChance( "UMPATCHYCHANCE", CFG_CUSTOM, 0 );
/** \brief The chance that a beetlebank being created is patchy or not */
static CfgFloat cfg_BBPatchyChance( "BEETLEBANKBPATCHYCHANCE", CFG_CUSTOM, 0.5 );
/** \brief The chance that a beetlebank being created is patchy or not */
static CfgFloat cfg_MGPatchyChance( "MOWNGRASSPATCHYCHANCE", CFG_CUSTOM, 0.5 );
/** \brief The chance that a setaside being created is patchy or not */
static CfgFloat cfg_SetAsidePatchyChance("SETASIDEPATCHYCHANCE", CFG_CUSTOM, 1.0);
static CfgFloat cfg_ele_weedscaling( "ELE_WEEDSCALING", CFG_CUSTOM, 1.0 );
/** \brief A constant relating the proportion of food units per m2. The value is calibrated to estimates of newt density. */
CfgFloat cfg_PondLarvalFoodBiomassConst("POND_LARVALFOODBIOMASSCONST", CFG_CUSTOM, 215.0);
/** \brief The instanteous rate of growth for larval food (r from logistic equation) */
CfgFloat cfg_PondLarvalFoodR("POND_LARVALFOODFOODR", CFG_CUSTOM, 0.15);
// Docs in Elements.h
CfgInt g_el_tramline_decaytime_days( "ELEM_TRAMLINE_DECAYTIME_DAYS", CFG_PRIVATE, 21 );
CfgInt g_el_herbicide_delaytime_days( "ELEM_HERBICIDE_DELAYTIME_DAYS", CFG_PRIVATE, 14 ); // 35
CfgInt g_el_strigling_delaytime_days( "ELEM_STRIGLING_DELAYTIME_DAYS", CFG_PRIVATE, 14 ); // 28

// Daydegree sum set on an element by ReduceVeg().
#define EL_GROWTH_DAYDEG_MAGIC l_el_growth_daydeg_magic.value()
static CfgInt l_el_growth_daydeg_magic( "ELEM_GROWTH_DAYDEG_MAGIC", CFG_PRIVATE, 100 );

// Date after which ReduceVeg() automatically sets the growth phase
// to harvest1. Cannot become a global configuration variable as it
// is calculated at runtime.
#define EL_GROWTH_DATE_MAGIC   (g_date->DayInYear(1,9))

// If the fraction used in the call to ReduceVeg() is *below* this
// value, then a phase transition to harvest1 is considered if
// there has been no previous 'forced' phase transition before this
// year.
#define EL_GROWTH_PHASE_SHIFT_LEVEL (l_el_growth_phase_shift_level.value())
static CfgFloat l_el_growth_phase_shift_level( "ELEM_GROWTH_PHASE_SHIFT_LEVEL", CFG_PRIVATE, 0.5 );


// Types of landscape elements. Default is 'unknown'.
// The conversion arrays are at the top in 'elements.cpp'!
// Change when adding or deleting element types.
// Outdated, not used anywhere in the landscape. *FN*
//#define EL_MAX_ELEM_TYPES 18

// Constant of proportionality between leaf area total and plant
// biomass.
#define EL_PLANT_BIOMASS  (l_el_plant_biomass_proport.value())  // Scaled to dry matter on Spring Barley for 2001 & 2002
// All other values are scaled relative to this as of 29/03/05
static CfgFloat l_el_plant_biomass_proport( "ELEM_PLANT_BIOMASS_PROPORT", CFG_PRIVATE, 41.45 );

// Default starting LAI Total. (NB LAIGreen=LAITotal/4)
#define EL_VEG_START_LAIT (l_el_veg_start_lait.value())
static CfgFloat l_el_veg_start_lait( "ELEM_VEG_START_LAIT", CFG_PRIVATE, 1.08 );

// Constant * biomass to get height
#define EL_VEG_HEIGHTSCALE  (l_el_veg_heightscale.value())
static CfgInt l_el_veg_heightscale( "ELEM_VEG_HEIGHTSCALE", CFG_PRIVATE, 16 );
//May+21
#define RV_CUT_MAY (l_el_rv_cut_may.value())
static CfgInt l_el_rv_cut_may( "ELEM_RV_CUT_MAY", CFG_PRIVATE, 142 );

#define RV_CUT_JUN (l_el_rv_cut_jun.value())
static CfgInt l_el_rv_cut_jun( "ELEM_RV_CUT_JUN", CFG_PRIVATE, 28 );

#define RV_CUT_JUL (l_el_rv_cut_jul.value())
static CfgInt l_el_rv_cut_jul( "ELEM_RV_CUT_JUL", CFG_PRIVATE, 35 );

#define RV_CUT_AUG (l_el_rv_cut_aug.value())
static CfgInt l_el_rv_cut_aug( "ELEM_RV_CUT_AUG", CFG_PRIVATE, 42 );

#define RV_CUT_SEP (l_el_rv_cut_sep.value())
static CfgInt l_el_rv_cut_sep( "ELEM_RV_CUT_SEP", CFG_PRIVATE, 49 );

#define RV_CUT_OCT (l_el_rv_cut_oct.value())
static CfgInt l_el_rv_cut_oct( "ELEM_RV_CUT_OCT", CFG_PRIVATE, 49 );

#define RV_MAY_1ST (l_el_rv_may_1st.value())
static CfgInt l_el_rv_may_1st( "ELEM_RV_MAY_1ST", CFG_PRIVATE, 121 );

#define RV_CUT_HEIGHT (l_el_rv_cut_height.value())
static CfgFloat l_el_rv_cut_height( "ELEM_RV_CUT_HEIGHT", CFG_PRIVATE, 10.0 );
#define RV_CUT_GREEN (l_el_rv_cut_green.value())
static CfgFloat l_el_rv_cut_green( "ELEM_RV_CUT_GREEN", CFG_PRIVATE, 1.5 );
#define RV_CUT_TOTAL (l_el_rv_cut_total.value())
static CfgFloat l_el_rv_cut_total( "ELEM_RV_CUT_TOTAL", CFG_PRIVATE, 2.0 );

CfgFloat l_el_o_cut_height( "ELEM_RV_CUT_HEIGHT", CFG_PRIVATE, 10.0 );
CfgFloat l_el_o_cut_green( "ELEM_RV_CUT_GREEN", CFG_PRIVATE, 1.5 );
CfgFloat l_el_o_cut_total( "ELEM_RV_CUT_TOTAL", CFG_PRIVATE, 2.0 );

// Default fraction between crop and weed biomasses.
#define EL_WEED_PERCENT (l_el_weed_percent.value())
static CfgFloat l_el_weed_percent( "ELEM_WEED_PERCENT", CFG_PRIVATE, 0.1 );

// Weed biomass regrowth slope after herbacide application.
#define EL_WEED_SLOPE (l_el_weed_slope.value())
static CfgFloat l_el_weed_slope( "ELEM_WEED_SLOPE", CFG_PRIVATE, 0.15 );

// Bug biomass regrowth slope after insecticide application.
#define EL_BUG_SLOPE (l_el_bug_slope.value())
static CfgFloat l_el_bug_slope( "ELEM_BUG_SLOPE", CFG_PRIVATE, 0.2 );

// Fraction of the weed biomass below which we are in pesticide
// regrowth phase, above we are in proportionality mode.
// CANNOT be 1.00!
#define EL_WEED_GLUE (l_el_weed_glue.value())
static CfgFloat l_el_weed_glue( "ELEM_WEED_GLUE", CFG_PRIVATE, 0.99 );

// Same as for weed, but bugs this time.
#define EL_BUG_GLUE (l_el_bug_glue.value())
static CfgFloat l_el_bug_glue( "ELEM_BUG_GLUE", CFG_PRIVATE, 0.50 );

/** \brief Used for birds that feed on grain on cereal fields 3% spill is expected
*
* Yield	%	kg/Ha spill	kJ/kg	kj/m
* 0.85	0.01	8.5	13680	11.628
* 0.85	0.02	17	13680	23.256
* 0.85	0.03	25.5	13680	34.884
* 0.85	0.04	34	13680	46.512
* 0.85	0.05	42.5	13680	58.14
* 0.85	0.06	51	13680	69.768
*/


// This is inversed prior to use. A multiplication is very much less
// expensive compared to a division.
//
// The original array supplied is:
// {1.11,1.06,1.01,0.99,0.96,0.92,0.92,0.93,0.97,0.99,1.02,1.06}
double LE::m_monthly_traffic[ 12 ] =
  {0.9009, 0.9434, 0.9901, 1.0101, 1.0417, 1.0870,
 1.0870, 1.0753, 1.0753, 1.0101, 0.9804,  0.9434};

double LE::m_largeroad_load[ 24 ] =
  {15,9,4,5,14,54,332,381,252,206,204,215,
 231,256,335,470,384,270,191,130,91,100,99,60};

double LE::m_smallroad_load[ 24 ] =
  {4,3,1,1,4,15,94,108,71,58,58,61,
 65,73,95,133,109,76,54,37,26,28,28,17};

double VegElement :: m_insect_biomass_parameters_a[9] = {EL_BUG_PERCENT_A, EL_BUG_PERCENT_SB_A, EL_BUG_PERCENT_WW_A, EL_BUG_PERCENT_WRy_A, EL_BUG_PERCENT_WR_A, EL_BUG_PERCENT_G_A, EL_BUG_PERCENT_Edges_A,EL_BUG_PERCENT_SA_A,0};
double VegElement :: m_insect_biomass_parameters_b[9] = {EL_BUG_PERCENT_B, EL_BUG_PERCENT_SB_B, EL_BUG_PERCENT_WW_B, EL_BUG_PERCENT_WRy_B, EL_BUG_PERCENT_WR_B, EL_BUG_PERCENT_G_B, EL_BUG_PERCENT_Edges_B,EL_BUG_PERCENT_SA_B,0};
double VegElement :: m_insect_biomass_parameters_c[9] = {EL_BUG_PERCENT_C, EL_BUG_PERCENT_SB_C, EL_BUG_PERCENT_WW_C, EL_BUG_PERCENT_WRy_C, EL_BUG_PERCENT_WR_C, EL_BUG_PERCENT_G_C, EL_BUG_PERCENT_Edges_C,EL_BUG_PERCENT_SA_C,0};
double VegElement :: m_SeasonalInsectScaler[12] = { 0.1, 0.1, 0.15, 0.25, 0.75, 1.0, 1.0, 1.0, 0.9, 0.75, 0.25, 0.1  };
double VegElement :: m_biomass_scale[tov_Undefined] = {};


void VegElement::SetBiomassScalers(TTypesOfVegetation a_tov) {
	switch (a_tov)
	{
	case	tov_BroadBeans:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FieldPeas:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FieldPeasSilage:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_OWinterWheat:	m_biomass_scale[a_tov] = 1.00 * 0.8;	break;
	case	tov_OWinterWheatUndersown:	m_biomass_scale[a_tov] = 1.00 * 0.8;	break;
	case	tov_OWinterWheatUndersownExt:	m_biomass_scale[a_tov] = 1.00 * 0.8 * 0.8;	break;
	case	tov_AgroChemIndustryCereal:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_BEBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_BEBeetSpring:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_BECatchPeaCrop:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_BEGrassGrazed1:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_BEGrassGrazed1Spring:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_BEGrassGrazed2:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_BEGrassGrazedLast:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_BEMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_BEMaizeCC:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_BEMaizeSpring:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_BEOrchardCrop:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_BEPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_BEPotatoesSpring:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_BEWinterBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_BEWinterBarleyCC:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_BEWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_BEWinterWheatCC:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_Carrots:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_CloverGrassGrazed1:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_CloverGrassGrazed2:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_DEAsparagusEstablishedPlantation:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEBushFruitPerm:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DECabbage:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DECarrots:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DEGrasslandSilageAnnual:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DEGreenFallow_1year:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DEHerbsPerennial_1year:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEHerbsPerennial_after1year:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DELegumes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DEMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEMaizeSilage:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEOAsparagusEstablishedPlantation:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DEOBushFruitPerm:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DEOCabbages:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DEOCarrots:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DEOGrasslandSilageAnnual:	m_biomass_scale[a_tov] = 1.1 * 0.8;	break;
	case	tov_DEOGreenFallow_1year:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOHerbsPerennial_1year:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOHerbsPerennial_after1year:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOLegume:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOMaize:	m_biomass_scale[a_tov] = 1.0 * 0.8;	break;
	case	tov_DEOMaizeSilage:	m_biomass_scale[a_tov] = 1.0 * 0.8;	break;
	case	tov_DEOOats:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOOrchard:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DEOPeas:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DEOPermanentGrassGrazed:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEOPermanentGrassLowYield:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEOPotatoes:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOrchard:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DEOSpringRye:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOSugarBeet:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOTriticale:	m_biomass_scale[a_tov] = 0.8;	break;
	case	tov_DEOWinterBarley:	m_biomass_scale[a_tov] = 0.8;	break;
	case	tov_DEOWinterRape:	m_biomass_scale[a_tov] = 0.8;	break;
	case	tov_DEOWinterRye:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DEOWinterWheat:	m_biomass_scale[a_tov] = 0.8;	break;
	case	tov_DEPeas:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DEPermanentGrassGrazed:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEPermanentGrassLowYield:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DEPotatoesIndustry:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DESpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DESpringRye:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DESugarBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DETriticale:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEWinterBarley:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEWinterRape:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEWinterRye:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DEWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DEWinterWheatLate:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DKBushFruit_Perm1:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKBushFruit_Perm2:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKCabbages:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKCarrots:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKCatchCrop:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKCerealLegume:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKCerealLegume_Whole:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKChristmasTrees_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKCloverGrassGrazed1:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKCloverGrassGrazed2:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKCloverGrassGrazed3:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKEnergyCrop_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKFarmForest_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKFarmYoungForest_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKFodderBeets:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKGrassGrazed_Perm:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKGrassLowYield_Perm:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DKGrassTussocky_Perm:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKGrazingPigs:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKGrazingPigs_Perm:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKLegume_Beans:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKLegume_Peas:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKLegume_Whole:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DKMaizeSilage:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DKMixedVeg:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOBushFruit_Perm1:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOBushFruit_Perm2:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOCabbages:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOCarrots:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOCatchCrop:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKOCerealLegume:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOCerealLegume_Whole:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOChristmasTrees_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKOCloverGrassGrazed1:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKOCloverGrassGrazed2:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKOCloverGrassGrazed3:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKOEnergyCrop_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKOFarmForest_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKOFarmYoungForest_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_DKOFodderBeets:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKOGrassGrazed_Perm:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKOGrassLowYield_Perm:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DKOGrazingPigs:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOGrazingPigs_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOLegume_Beans:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLegume_Beans_CC:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLegume_Peas:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLegume_Peas_CC:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLegume_Whole:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLegume_Whole_CC:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLegumeCloverGrass_Whole:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLentils:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOLupines:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOMaize:	m_biomass_scale[a_tov] = 1.00 * 0.8;	break;
	case	tov_DKOMaizeSilage:	m_biomass_scale[a_tov] = 1.00 * 0.8;	break;
	case	tov_DKOMixedVeg:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOOrchApple:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOOrchardCrop_Perm:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_DKOOrchCherry:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOOrchOther:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOOrchPear:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOPotato:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOPotatoIndustry:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOPotatoSeed:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOptimalFlowerMix1:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOptimalFlowerMix2:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOptimalFlowerMix3:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOrchApple:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOrchardCrop_Perm:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOrchCherry:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOrchOther:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOrchPear:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOSeedGrassRye_Spring:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOSetAside:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOSetAside_AnnualFlower:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOSetAside_PerennialFlower:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOSetAside_SummerMow:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOSpringBarley:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOSpringBarley_CC:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOSpringBarleyCloverGrass:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOSpringBarleySilage:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOSpringFodderGrass:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_DKOSpringOats:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOSpringOats_CC:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOSpringWheat:	m_biomass_scale[a_tov] = 1.0 * 0.8;	break;
	case	tov_DKOSugarBeets:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOVegSeeds:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKOWinterBarley:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOWinterCloverGrassGrazedSown:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKOWinterFodderGrass:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_DKOWinterRape:	m_biomass_scale[a_tov] = 1.071 * 0.8;	break;
	case	tov_DKOWinterRye:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOWinterRye_CC:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_DKOWinterWheat:	m_biomass_scale[a_tov] = 1.00 * 0.8;	break;
	case	tov_DKOWinterWheat_CC:	m_biomass_scale[a_tov] = 1.00 * 0.8;	break;
	case	tov_DKPlantNursery_Perm:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKPotato:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKPotatoIndustry:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKPotatoSeed:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSeedGrassFescue_Spring:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKSeedGrassRye_Spring:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKSetAside:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKSetAside_SummerMow:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKSpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSpringBarley_CC:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSpringBarley_Green:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSpringBarleyCloverGrass:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSpringBarleySilage:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSpringFodderGrass:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_DKSpringOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSpringOats_CC:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKSpringWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DKSugarBeets:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKUndefined:	m_biomass_scale[a_tov] = 0;	break;
	case	tov_DKVegSeeds:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_DKWinterBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKWinterCloverGrassGrazedSown:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_DKWinterFodderGrass:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_DKWinterRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_DKWinterRye:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKWinterRye_CC:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_DKWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_DKWinterWheat_CC:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_GenericCatchCrop:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIBufferZone:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIBufferZone_Perm:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FICaraway1:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_FICaraway2:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_FieldPeasStrigling:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIFabaBean:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIFeedingGround:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIGrasslandPasturePerennial1:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIGrasslandPasturePerennial2:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIGrasslandSilageAnnual:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIGrasslandSilagePerennial1:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIGrasslandSilagePerennial2:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIGreenFallow_1year:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FIGreenFallow_Perm:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FINaturalGrassland:	m_biomass_scale[a_tov] = 0.567;	break;	// actual yield
	case	tov_FINaturalGrassland_Perm:	m_biomass_scale[a_tov] = 0.567;	break;	// actual yield
	case	tov_FIOCaraway1:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_FIOCaraway2:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_FIOFabaBean:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOPotato_North:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOPotato_South:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOPotatoIndustry_North:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOPotatoIndustry_South:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOSpringBarley_Fodder:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOSpringBarley_Malt:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOSpringOats:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOSpringRape:	m_biomass_scale[a_tov] = 1.071 * 0.8;	break;
	case	tov_FIOSpringWheat:	m_biomass_scale[a_tov] = 1.0 * 0.8;	break;
	case	tov_FIOStarchPotato_North:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOStarchPotato_South:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOTurnipRape:	m_biomass_scale[a_tov] = 1.071 * 0.8;	break;
	case	tov_FIOWinterRye:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_FIOWinterWheat:	m_biomass_scale[a_tov] = 1.0 * 0.8;	break;
	case	tov_FIPotato_North:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIPotato_South:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIPotatoIndustry_North:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIPotatoIndustry_South:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FISpringBarley_Fodder:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FISpringBarley_Malt:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FISpringOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FISpringRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_FISpringWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_FISprSpringBarley_Fodder:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIStarchPotato_North:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIStarchPotato_South:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FISugarBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FITurnipRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_FIWinterRye:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FIWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_FlowerStrip1:	m_biomass_scale[a_tov] = 0.567;	break;	//0.567 low yield
	case	tov_FlowerStrip2:	m_biomass_scale[a_tov] = 0.676;	break;	// 
	case	tov_FlowerStrip3:	m_biomass_scale[a_tov] = 0.7857;	break;	// high yield
	case	tov_FodderBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FodderGrass:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_FRGrassland:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FRGrassland_Perm:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_FRMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_FRMaize_Silage:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_FRPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FRSorghum:	m_biomass_scale[a_tov] = 1;	break;	// should be similar to maize
	case	tov_FRSpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FRSpringOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FRSpringWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_FRSunflower:	m_biomass_scale[a_tov] = 1;	break;	// need to check this
	case	tov_FRWinterBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_FRWinterRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_FRWinterTriticale:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_FRWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_Heath:	m_biomass_scale[a_tov] = 0.567;	break;	//0.567 is scaled for actual yield
	case	tov_IRGrassland_no_reseed:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_IRGrassland_reseed:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_IRSpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_IRSpringOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_IRSpringWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_IRWinterBarley:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_IRWinterOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_IRWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_ITGrassland:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_ITOOrchard:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_ITOrchard:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_Lawn:	m_biomass_scale[a_tov] = 0.5;	break;
	case	tov_Maize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_MaizeSilage:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_MaizeStrigling:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NaturalGrass:	m_biomass_scale[a_tov] = 0.567;	break;	//0.567 is scaled for actual yield
	case	tov_NLBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NLBeetSpring:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NLCabbage:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_NLCabbageSpring:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_NLCarrots:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_NLCarrotsSpring:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_NLCatchCropPea:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NLGrassGrazed1:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLGrassGrazed1Spring:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLGrassGrazed2:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLGrassGrazedExtensive1:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLGrassGrazedExtensive1Spring:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLGrassGrazedExtensive2:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLGrassGrazedExtensiveLast:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLGrassGrazedLast:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_NLMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_NLMaizeSpring:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_NLOrchardCrop:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_NLPermanentGrassGrazed:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_NLPermanentGrassGrazedExtensive:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_NLPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NLPotatoesSpring:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NLSpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NLSpringBarleySpring:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NLTulips:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_NLWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_NoGrowth:	m_biomass_scale[a_tov] = 0;	break;
	case	tov_None:	m_biomass_scale[a_tov] = 0;	break;
	case	tov_NorwegianOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NorwegianSpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_NorwegianPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_Oats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_OBarleyPeaCloverGrass:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_OCarrots:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_OCloverGrassGrazed1:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_OCloverGrassGrazed2:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_OCloverGrassSilage1:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_OFieldPeas:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OFieldPeasSilage:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OFirstYearDanger:	m_biomass_scale[a_tov] = 0;	break;
	case	tov_OFodderBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_OGrazingPigs:	m_biomass_scale[a_tov] = 0.7857 * 0.8;	break;
	case	tov_OMaizeSilage:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_OOats:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OPermanentGrassGrazed:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_OPotatoes:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OrchardCrop:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_OSBarleySilage:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OSeedGrass1:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_OSeedGrass2:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_OSetAside:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_OSpringBarley:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OSpringBarleyClover:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OSpringBarleyExt:	m_biomass_scale[a_tov] = 0.857 * 0.8 * 0.8;	break;
	case	tov_OSpringBarleyGrass:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OSpringBarleyPigs:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OTriticale:	m_biomass_scale[a_tov] = 1.00 * 0.8;	break;
	case	tov_OWinterBarley:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_OWinterBarleyExt:	m_biomass_scale[a_tov] = 0.857 * 0.8 * 0.8;	break;
	case	tov_OWinterRape:	m_biomass_scale[a_tov] = 1.071 * 0.8;	break;
	case	tov_OWinterRye:	m_biomass_scale[a_tov] = 0.857 * 0.8;	break;
	case	tov_PermanentGrassGrazed:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_PermanentGrassLowYield:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PermanentGrassTussocky:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PermanentSetAside:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PlantNursery:	m_biomass_scale[a_tov] = 0.1;	break;
	case	tov_PLBeans:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLBeetSpr:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLCarrots:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PLFodderLucerne1:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_PLFodderLucerne2:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_PLMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PLMaizeSilage:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PLPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLSpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLSpringBarleySpr:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLSpringWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PLWinterBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLWinterRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_PLWinterRye:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PLWinterTriticale:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PLWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PLWinterWheatLate:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_Potatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PotatoesIndustry:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTBeans:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTCabbage:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTCabbage_Hort:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTCloverGrassGrazed1:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_PTCloverGrassGrazed2:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_PTCorkOak:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PTFodderMix:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_PTGrassGrazed:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_PTHorticulture:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PTMaize_Hort:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PTOats:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTOliveGroveIntensive:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PTOliveGroveSuperIntensive:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PTOliveGroveTraditional:	m_biomass_scale[a_tov] = 0.7857;	break;	// EZ: same as orchards
	case	tov_PTOliveGroveTradOrganic:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PTOtherDryBeans:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTPermanentGrassGrazed:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_PTPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTRyegrass:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_PTShrubPastures:	m_biomass_scale[a_tov] = 0.567;	break;
	case	tov_PTSorghum:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PTTriticale:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PTTurnipGrazed:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PTVineyards:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_PTWinterBarley:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PTWinterRye:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_PTWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_PTYellowLupin:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_SeedGrass1:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_SeedGrass2:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_SESpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SetAside:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_SEWinterRape_Seed:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_SEWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_SpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyCloverGrass:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyCloverGrassStrigling:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyGrass:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyPeaCloverGrassStrigling:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyPTreatment:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleySeed:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleySilage:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleySKManagement:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleySpr:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyStrigling:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyStriglingCulm:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringBarleyStriglingSingle:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_SpringRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_SpringWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_SugarBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_Triticale:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_UKBeans:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_UKBeet:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_UKMaize:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_UKPermanentGrass:	m_biomass_scale[a_tov] = 1.1;	break;
	case	tov_UKPotatoes:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_UKSpringBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_UKTempGrass:	m_biomass_scale[a_tov] = 1.2;	break;
	case	tov_UKWinterBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_UKWinterRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_UKWinterWheat:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_Wasteland:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;
	case	tov_WaterBufferZone:	m_biomass_scale[a_tov] = 0.567;	break;
	case	tov_WinterBarley:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_WinterBarleyStrigling:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_WinterRape:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_WinterRapeStrigling:	m_biomass_scale[a_tov] = 1.071;	break;
	case	tov_WinterRye:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_WinterRyeStrigling:	m_biomass_scale[a_tov] = 0.857;	break;
	case	tov_WinterWheat:	m_biomass_scale[a_tov] = 1;	break;	  // This gives approx 18 tonnes biomass for WW
	case	tov_WinterWheatShort:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_WinterWheatStrigling:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_WinterWheatStriglingCulm:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_WinterWheatStriglingSingle:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_WWheatPControl:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_WWheatPToxicControl:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_WWheatPTreatment:	m_biomass_scale[a_tov] = 1;	break;
	case	tov_YoungForest:	m_biomass_scale[a_tov] = 0.7857 * 0.67;	break;

	// Hoa Binh (HB) vegetation types - ABM-test coupling, 2026
	case	tov_HBMaizeIntensive:	m_biomass_scale[a_tov] = 1.0000;	break;
	case	tov_HBMaizeLowInput:	m_biomass_scale[a_tov] = 0.9000;	break;
	case	tov_HBOrchard:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_HBFallow:	m_biomass_scale[a_tov] = 0.7857;	break;
	case	tov_HBAcacia:	m_biomass_scale[a_tov] = 0.5264;	break;
	case	tov_HBNativeMix:	m_biomass_scale[a_tov] = 0.5264;	break;
	case	tov_HBRegeneration:	m_biomass_scale[a_tov] = 0.5264;	break;
	case	tov_HBProtectStrict:	m_biomass_scale[a_tov] = 0.5264;	break;
	case	tov_HBProtectUse:	m_biomass_scale[a_tov] = 0.5264;	break;
	case tov_Undefined:	m_biomass_scale[a_tov] = 0;	break;
	}
}

class LE_TypeClass * g_letype;

LE::LE(Landscape *L ) {

	m_Landscape = L;
	/**
	* The major job of this constructor is simply to provide default values for all members
	*/
	m_signal_mask = 0;
	m_lasttreat.resize(1);
	m_lasttreat[0] = sleep_all_day;
	m_lastindex = 0;
	m_running = 0;
	m_poison = false;
	m_owner_file = -1;
	m_owner_index = -1;
	
	// ATTRIBUTES
	m_att_high = false;
	m_att_water = false;
	m_att_forest = false;
	m_att_woody = false;
	m_att_urbannoveg = false;
	m_att_userdefinedbool = false;
	m_att_userdefinedint = 0;
	
	m_cattle_grazing = 0;
	m_default_grazing_level = 0; // this means any grazed elements must set this in their constructor.
	m_pig_grazing = false;
	m_weedddegs = 0.0;
	m_yddegs = 0.0;
	m_vegddegs = -1.0;
	m_olddays = 0;
	m_days_since_insecticide_spray = 0;
	m_tramlinesdecay = 0;
	m_mowndecay = 0;
	m_herbicidedelay = 0;
	m_border = NULL;
	m_unsprayedmarginpolyref = -1;
	m_valid_x = -1;
	m_valid_y = -1;
	m_is_in_map = false;
	m_squares_in_map = 0;
	m_management_loop_detect_date = 0;
	m_management_loop_detect_count = 0;
	m_repeat_start = false;
	m_skylarkscrapes = false;
	m_type = tole_Foobar;
	SetALMaSSEleType(-1);
	m_ddegs = 0.0;
	m_maxx = -1; // a very small number
	m_maxy = -1;
	m_minx = 9999999; // a very big number
	m_miny = 9999999;
	m_countrydesignation = -1; // default not set
	m_soiltype = -1;
	m_area = 0;
	m_centroidx = -1;
	m_centroidy = -1;
	m_vege_danger_store = -1;
	m_PesticideGridCell = -1;
	m_subtype = -1;
	m_owner = NULL;
	m_rot_index = -1;
	m_poly = -1;
	m_map_index = -1;
	m_almass_le_type = -1;
	m_farmfunc_tried_to_do = -1;
	SetStubble(false);
	m_birdseedforage = -1;
	m_birdmaizeforage = -1;
	m_openness = -1;
	m_vegage = -1;

	for (int i = 0; i<10; i++) SetMConstants(i, 1);
	for (int i = 0; i < 366; i++)
	{
		m_gooseNos[i] = 0;
		m_gooseNosTimed[i] = 0;
		for (int l = 0; l < gs_foobar; l++)
		{
			m_gooseSpNos[i][l] = 0;
			m_gooseSpNosTimed[i][l] = 0;
		}
	}
	for (int l = 0; l < gs_foobar; l++)
	{
		m_goosegrazingforage[l] = 0;
	}
	for (int i = 0; i < 25; i++)
	{
		MDates[0][i] = -1;
		MDates[1][i] = -1;
	}
	SetLastSownVeg(tov_Undefined);
	ClearManagementActionSum();
#ifdef FMDEBUG
	m_pindex = 0;
	for ( int i = 0; i < 256; i++ ) {
		m_pdates[ i ] = 0;
		m_ptrace[ i ] = 0;
	}
#endif
}

void LE::DoCopy(const LE* a_LE) {

	/**
	* The major job of this method is simply to copy values for all members from one LE to another
	*/
	m_signal_mask = a_LE->m_signal_mask;
	m_lasttreat = a_LE->m_lasttreat;
	m_lastindex = a_LE->m_lastindex;
	m_running = a_LE->m_running;
	m_poison = a_LE->m_poison;
	m_owner_file = a_LE->m_owner_file;
	m_owner_index = a_LE->m_owner_index;
	
	/* Copy Attributes */
	m_att_high = a_LE->m_att_high;
	m_att_water = a_LE->m_att_water;
	m_att_forest = a_LE->m_att_forest;
	m_att_woody = a_LE->m_att_woody;
	m_att_urbannoveg = a_LE->m_att_urbannoveg;
	m_att_userdefinedbool = a_LE->m_att_userdefinedbool;
	m_att_userdefinedint = a_LE->m_att_userdefinedint;

	m_cattle_grazing = a_LE->m_cattle_grazing;
	m_default_grazing_level = a_LE->m_default_grazing_level; // this means any grazed elements must set this in their constructor.
	m_pig_grazing = a_LE->m_pig_grazing;
	m_yddegs = a_LE->m_yddegs;
	m_olddays = a_LE->m_olddays;
	m_vegddegs = a_LE->m_vegddegs;
	m_days_since_insecticide_spray = a_LE->m_days_since_insecticide_spray;
	m_tramlinesdecay = a_LE->m_tramlinesdecay;
	m_mowndecay = a_LE->m_mowndecay;
	m_herbicidedelay = a_LE->m_herbicidedelay;
	m_border = a_LE->m_border;
	m_unsprayedmarginpolyref = a_LE->m_unsprayedmarginpolyref;
	m_valid_x = a_LE->m_valid_x;
	m_valid_y = a_LE->m_valid_y;
	m_is_in_map = a_LE->m_is_in_map;
	m_squares_in_map = a_LE->m_squares_in_map;
	m_management_loop_detect_date = a_LE->m_management_loop_detect_date;
	m_management_loop_detect_count = a_LE->m_management_loop_detect_count;
	m_repeat_start = a_LE->m_repeat_start;
	m_skylarkscrapes = a_LE->m_skylarkscrapes;
	m_type = a_LE->m_type;
	m_owner_tole = a_LE->m_owner_tole;
	m_birdseedforage = a_LE->m_birdseedforage;
	m_birdmaizeforage = a_LE->m_birdmaizeforage; 
	m_ddegs = a_LE->m_ddegs;
	m_maxx = a_LE->m_maxx; 
	m_maxy = a_LE->m_maxy;
	m_minx = a_LE->m_minx; 
	m_miny = a_LE->m_miny;
	m_countrydesignation = a_LE->m_countrydesignation; 
	m_soiltype = a_LE->m_soiltype;
	m_area = a_LE->m_area;
	m_centroidx = a_LE->m_centroidx;
	m_centroidy = a_LE->m_centroidy;
	m_vege_danger_store = a_LE->m_vege_danger_store;
	m_PesticideGridCell = a_LE->m_PesticideGridCell;
	m_subtype = a_LE->m_subtype;
	m_owner = a_LE->m_owner;
	m_rot_index = a_LE->m_rot_index;
	m_poly = a_LE->m_poly;
	m_map_index = a_LE->m_map_index;
	m_almass_le_type = a_LE->m_almass_le_type;
	m_farmfunc_tried_to_do = a_LE->m_farmfunc_tried_to_do;
	m_openness = a_LE->m_openness;
	m_vegage = a_LE->m_vegage;
	m_elevation = a_LE->m_elevation;
	m_aspect = a_LE->m_aspect;
	m_slope = a_LE->m_slope;

	for (int i = 0; i < 366; i++)
	{
		m_gooseNos[i] = a_LE->m_gooseNos[i];
		m_gooseNosTimed[i] = a_LE->m_gooseNosTimed[i];
		for (int l = 0; l < gs_foobar; l++)
		{
			m_gooseSpNos[i][l] = a_LE->m_gooseSpNos[i][l];
			m_gooseSpNosTimed[i][l] = a_LE->m_gooseSpNosTimed[i][l];
		}
	}
	for (int l = 0; l < gs_foobar; l++)
	{
		m_goosegrazingforage[l] = a_LE->m_goosegrazingforage[l];
	}
	for (int i = 0; i < 25; i++)
	{
		MDates[0][i] = a_LE->MDates[0][i];
		MDates[1][i] = a_LE->MDates[1][i];
	}
	for (int i = 0; i<10; i++) SetMConstants(i, a_LE->MConsts[i]);
}


LE::~LE( void ) {
}

#ifdef FMDEBUG
void LE::Trace( int a_value ) {
  m_farmfunc_tried_to_do = a_value;
#ifdef __RECORDFARMEVENTS
  m_Landscape->RecordEvent( m_owner_index, m_poly, ((VegElement*)this)->GetVegType(), a_value, g_date->DayInYear(), g_date->GetYearNumber());
#endif
  m_pdates[ m_pindex ] = g_date->DayInYear();
  m_ptrace[ m_pindex++ ] = a_value;
  m_pindex &= 0xff; // Circular buffer if need be.
}

void LE::ResetTrace( void ) {
  m_pindex = 0;
  for ( int i = 0; i < 256; i++ ) {
    m_pdates[ i ] = 0;
    m_ptrace[ i ] = 0;
  }
}

#else
// Compiles into nothing if FMDEBUG is #undef.
void LE::Trace( int a_value ) {
  m_farmfunc_tried_to_do = a_value;
}

void LE::ResetTrace( void ) {
}

#endif

void LE::SetCopyTreatment( int a_treatment ) {
  SetLastTreatment( a_treatment );
}

void LE::SetLastTreatment( int a_treatment ) {
  unsigned sz = (int) m_lasttreat.size();
  if ( m_lastindex == sz )
    m_lasttreat.resize( m_lastindex + 1 );

  m_lasttreat[ m_lastindex++ ] = a_treatment;

  // Count this treatment in the grand scope of things.
  m_Landscape->IncTreatCounter( a_treatment );
  // If we have a field margin then we need to tell it about this
  // but not if it is an insecticide spray etc..
  /* if (m_unsprayedmarginpolyref!=-1) { switch (a_treatment) { case  herbicide_treat: case  growth_regulator:
  case  fungicide_treat: case  insecticide_treat: case trial_insecticidetreat: case syninsecticide_treat: case  molluscicide:
  break; // Do not add sprayings default: LE* le=g_landscape_p->SupplyLEPointer(m_unsprayedmarginpolyref);
  le->SetCopyTreatment(a_treatment); // Now we also need to do something with the treatment

  break; }

  } */
}

int LE::GetLastTreatment() {
	/**
	* This will give the last event recorded for this LE 
	*  In most cases this will return sleep_all_day
	*/
	return m_lasttreat.back();
}

int LE::GetLastTreatment(int* a_index) {
	if (*a_index == (int)m_lastindex)
		return sleep_all_day;
	int i = (*a_index)++;
	int treat = m_lasttreat[i];
	return treat;
}

void LE::Tick( void ) {
  m_lastindex = 0;
  m_lasttreat[ 0 ] = sleep_all_day;
}

void VegElement::Tick(void) {
	LE::Tick();
	if (m_mowndecay > 0) m_mowndecay--;
	m_herbicidedelay--;
}

void Field::Tick(void) {
	VegElement::Tick();
	if (m_tramlinesdecay > 0) m_tramlinesdecay--;
}


void LE::DoDevelopment( void ) {
}

APoint LE::GetCentroid()
{
    APoint p;
    p.m_x=m_centroidx;
    p.m_y=m_centroidy;
    return p;
}

int LE::GetGooseNos( ) {
	/**
	* This simply looks X days behind at the moment and sums the total number of geese seen.The length of the backward count can be altered by
	* changing the config variable value cfg_goosecountperiod (default 1, only care about yesterday).
	*/
	int geese = 0;
	for (unsigned i = 1; i <= (unsigned)cfg_goosecountperiod.value( ); i++) {
		unsigned ind = ((unsigned)g_date->DayInYear( ) - i) % 365;
		geese += m_gooseNos[ ind ];
	}
	return geese;
}

int LE::GetQuarryNos() {
	/**
	* This simply looks X days behind at the moment and sums the total number of legal quarry species seen.The length of the backward count can be altered by
	* changing the config variable value cfg_goosecountperiod (default 1, only care about yesterday).
	*/
	int geese = 0;
	for (unsigned i = 1; i <= (unsigned)cfg_goosecountperiod.value(); i++) {
		unsigned ind = ((unsigned)g_date->DayInYear() - i) % 365;
		geese += m_gooseSpNos[ind][gs_Pinkfoot];
		geese += m_gooseSpNos[ind][gs_Greylag];
	}
	return geese;
}

int LE::GetGooseNosToday() {
	/**
	* This simply sums the total number of geese seen today.
	*/
	int geese = 0;
	for (unsigned i = 0; i < (unsigned)gs_foobar; i++) {
		geese += m_gooseSpNos[g_date->DayInYear()][i];
	}
	return geese;
}

int LE::GetGooseNosTodayTimed() {
	/**
	* This simply sums the total number of geese seen today at our predefined timepoint.
	*/
	int geese = 0;
	for (unsigned i = 0; i < (unsigned)gs_foobar; i++) {
		geese += m_gooseSpNosTimed[g_date->DayInYear()][i];
	}
	return geese;
}

/** \brief Returns the number of geese of a specific species on a field today.*/
int LE::GetGooseSpNosToday(GooseSpecies a_goose) {
	return m_gooseSpNos[g_date->DayInYear()][a_goose];
}

/** \brief Returns the number of geese of a specific species on a field today.*/
int LE::GetGooseSpNosTodayTimed(GooseSpecies a_goose) {
	return m_gooseSpNosTimed[g_date->DayInYear()][a_goose];
}
/** \brief Returns the distance to closest roost from the field.*/
int LE::GetGooseRoostDist(GooseSpecies a_goose) {
	return int(m_dist_to_closest_roost[a_goose]);
}


//---------------------------------------------------------------------------
void VegElement::SetVegGrowthScalerRand()
{
	m_growth_scaler = (g_rand_uni_fnc() * (cfg_PermanentVegGrowthMaxScaler.value() - cfg_PermanentVegGrowthMinScaler.value())) + cfg_PermanentVegGrowthMinScaler.value(); // Scales growth stochastically in a range given by the configs
}

VegElement::VegElement(TTypesOfLandscapeElement tole, Landscape* L) : LE(L) {
	m_interested_biomass_fraction = 1.1; //by default we are interested in all biomass
	m_type = tole;
	m_owner_tole = m_type;
	// Set default Attribute Flags 
	Set_Att_Veg(true);
	Set_Att_VegPatchy(false);
	Set_Att_VegCereal(false);
	Set_Att_VegMatureCereal(false);
	Set_Att_VegGooseGrass(false);
	Set_Att_VegGrass(false);
	Set_Att_VegMaize(false);
	m_growth_scaler = 1.0; // default
	m_veg_biomass = 0.0;
	m_weed_biomass = 0.0;
	m_veg_height = 0.0;
	m_veg_cover = 0.0;
	m_insect_pop = 1.0;
	m_vege_type = tov_NoGrowth;
	m_insect_biomass_parameters_index = 8; // No insects is the default so future constructors need to set this explicitly
	m_curve_num = g_crops->VegTypeToCurveNum(m_vege_type);
	m_weed_curve_num = 99; // 99 used as default weed curve
	m_yddegs = 0.0;
	m_vegddegs = -1.0;
	m_ddegs = 0.0;
	m_LAgreen = 0.0;
	m_LAtotal = 0.0;
	m_digestability = 1.0;
	for (int i = 0; i < 32; i++) m_oldnewgrowth[i] = 0.5;
	for (int i = 0; i < 14; i++) m_oldnewgrowth2[i] = 0.0;
	m_newoldgrowthindex = 0;
	m_newgrowthsum = 8.0;
	m_newgrowth = 0.0;
	m_new_weed_growth = 0.0;
	m_forced_phase_shift = false;
	m_force_growth = false;
	SetGrowthPhase(janfirst);
	m_total_biomass = 0.0;
	m_total_biomass_old = 0.0;
	m_CropType = toc_Foobar;
	// Set default for species specific calculations
	SpeciesSpecificCalculations = &VegElement::DoNothing;
	m_start_dd_flower = false;

	if (l_el_read_bug_percentage_file.value()) {
		//ReadBugPercentageFile();
		return;
	}
	// Default 0.1 - only those that differ need to be listed below - todo this and move this out of the constructor
	for (int i = 0; i < tov_Undefined; i++) { g_weed_percent[i] = 0.1; }
	g_weed_percent[tov_Carrots] = 0.1;
	g_weed_percent[ tov_BroadBeans ] = 0.1;
	g_weed_percent[ tov_Maize ] = 0.05;
	g_weed_percent[tov_MaizeSilage] = 0.05;
	g_weed_percent[tov_OMaizeSilage] = 0.05;
	g_weed_percent[tov_OCarrots] = 0.1;
	g_weed_percent[tov_Potatoes] = 0.1;
	g_weed_percent[tov_OPotatoes] = 0.1;
	g_weed_percent[tov_FodderGrass] = 0.1;
	g_weed_percent[tov_CloverGrassGrazed1] = 0.1;
	g_weed_percent[tov_CloverGrassGrazed2] = 0.1;
	g_weed_percent[tov_OCloverGrassGrazed1] = 0.1;
	g_weed_percent[tov_OCloverGrassGrazed2] = 0.1;
	g_weed_percent[tov_SpringBarley] = 0.1;
	g_weed_percent[tov_SpringBarleySpr] = 0.1;
	g_weed_percent[tov_SpringBarleyPTreatment] = 0.1;
	g_weed_percent[tov_SpringBarleySKManagement] = 0.1;
	g_weed_percent[tov_WinterWheat] = 0.1;
	g_weed_percent[tov_WinterWheatShort] = 0.1;
	g_weed_percent[tov_SpringBarleySilage] = 0.1;
	g_weed_percent[tov_SpringBarleySeed] = 0.1;
	g_weed_percent[tov_OGrazingPigs] = 0.1;
	g_weed_percent[tov_OCloverGrassSilage1] = 0.1;
	g_weed_percent[tov_SpringBarleyCloverGrass] = 0.1;
	g_weed_percent[tov_OSpringBarleyPigs] = 0.1;
	g_weed_percent[tov_OBarleyPeaCloverGrass] = 0.1;
	g_weed_percent[tov_OSpringBarley] = 0.1;
	g_weed_percent[tov_OSpringBarleyExt] = 0.1;
	g_weed_percent[tov_OSBarleySilage] = 0.1;
	g_weed_percent[ tov_OWinterWheat ] = 0.1;
	g_weed_percent[ tov_OWinterWheatUndersown ] = 0.1;
	g_weed_percent[ tov_OWinterWheatUndersownExt ] = 0.1;
	g_weed_percent[tov_WinterRape] = 0.05;
	g_weed_percent[tov_OWinterRape] = 0.05;
	g_weed_percent[tov_OWinterRye] = 0.1;
	g_weed_percent[tov_OWinterBarley] = 0.1;
	g_weed_percent[tov_OWinterBarleyExt] = 0.1;
	g_weed_percent[tov_WinterBarley] = 0.1;
	g_weed_percent[tov_WinterRye] = 0.1;
	g_weed_percent[ tov_OFieldPeas ] = 0.1;
	g_weed_percent[tov_OFieldPeas] = 0.1;
	g_weed_percent[ tov_OFieldPeasSilage ] = 0.1;
	g_weed_percent[tov_OOats] = 0.1;
	g_weed_percent[tov_Oats] = 0.1;
	g_weed_percent[tov_Heath] = 0.1;
	g_weed_percent[tov_OrchardCrop] = 0.1;
	g_weed_percent[ tov_FieldPeas ] = 0.1;
	g_weed_percent[ tov_FieldPeasSilage ] = 0.1;
	g_weed_percent[tov_SeedGrass1] = 0.1;
	g_weed_percent[tov_SeedGrass2] = 0.1;
	g_weed_percent[tov_OSeedGrass1] = 0.15;
	g_weed_percent[tov_OSeedGrass2] = 0.15;
	g_weed_percent[tov_OPermanentGrassGrazed] = 0.1;
	g_weed_percent[tov_SetAside] = 0.1;
	g_weed_percent[tov_OSetAside] = 0.1;
	g_weed_percent[tov_PermanentSetAside] = 0.1;
	g_weed_percent[tov_PermanentGrassLowYield] = 0.1;
	g_weed_percent[tov_PermanentGrassGrazed] = 0.1;
	g_weed_percent[tov_PermanentGrassTussocky] = 0.1;
	g_weed_percent[tov_FodderBeet] = 0.1;
	g_weed_percent[tov_SugarBeet] = 0.1;
	g_weed_percent[tov_OFodderBeet] = 0.1;
	g_weed_percent[tov_NaturalGrass] = 0.1;
	g_weed_percent[tov_None] = 0.1;
	g_weed_percent[tov_NoGrowth] = 0.1;
	g_weed_percent[tov_OFirstYearDanger] = 0.1;
	g_weed_percent[tov_Triticale] = 0.1;
	g_weed_percent[tov_OTriticale] = 0.1;
	g_weed_percent[tov_WWheatPControl] = 0.1;
	g_weed_percent[tov_WWheatPToxicControl] = 0.1;
	g_weed_percent[tov_WWheatPTreatment] = 0.1;
	g_weed_percent[tov_AgroChemIndustryCereal] = 0.1;
	g_weed_percent[tov_WinterWheatStrigling] = 0.1;
	g_weed_percent[tov_WinterWheatStriglingSingle] = 0.1;
	g_weed_percent[tov_WinterWheatStriglingCulm] = 0.1;
	g_weed_percent[tov_SpringBarleyCloverGrassStrigling] = 0.1;
	g_weed_percent[tov_SpringBarleyStrigling] = 0.1;
	g_weed_percent[tov_SpringBarleyStriglingSingle] = 0.1;
	g_weed_percent[tov_SpringBarleyStriglingCulm] = 0.1;
	g_weed_percent[tov_MaizeStrigling] = 0.1;
	g_weed_percent[tov_WinterRapeStrigling] = 0.1;
	g_weed_percent[tov_WinterRyeStrigling] = 0.1;
	g_weed_percent[tov_WinterBarleyStrigling] = 0.1;
	g_weed_percent[tov_FieldPeasStrigling] = 0.1;
	g_weed_percent[tov_SpringBarleyPeaCloverGrassStrigling] = 0.1;
	g_weed_percent[tov_YoungForest] = 0.1;
	g_weed_percent[tov_Wasteland] = 0.1;
	g_weed_percent[tov_WaterBufferZone] = 0.1;

	g_weed_percent[tov_PLWinterWheat] = 0.1;
	g_weed_percent[tov_PLWinterRape] = 0.05;
	g_weed_percent[tov_PLWinterBarley] = 0.1;
	g_weed_percent[tov_PLWinterTriticale] = 0.1;
	g_weed_percent[tov_PLWinterRye] = 0.1;
	g_weed_percent[tov_PLSpringWheat] = 0.1;
	g_weed_percent[tov_PLSpringBarley] = 0.1;
	g_weed_percent[tov_PLMaize] = 0.1;
	g_weed_percent[tov_PLMaizeSilage] = 0.1;
	g_weed_percent[tov_PLPotatoes] = 0.1;
	g_weed_percent[tov_PLBeet] = 0.1;
	g_weed_percent[tov_PLFodderLucerne1] = 0.1;
	g_weed_percent[tov_PLFodderLucerne2] = 0.1;
	g_weed_percent[tov_PLCarrots] = 0.1;
	g_weed_percent[tov_PLSpringBarleySpr] = 0.1;
	g_weed_percent[tov_PLWinterWheatLate] = 0.1;
	g_weed_percent[tov_PLBeetSpr] = 0.1;
	g_weed_percent[tov_PLBeans] = 0.1;

	g_weed_percent[tov_NLWinterWheat] = 0.1;
	g_weed_percent[tov_NLSpringBarley] = 0.1;
	g_weed_percent[tov_NLMaize] = 0.1;
	g_weed_percent[tov_NLPotatoes] = 0.1;
	g_weed_percent[tov_NLBeet] = 0.1;
	g_weed_percent[tov_NLCarrots] = 0.1;
	g_weed_percent[tov_NLCabbage] = 0.1;
	g_weed_percent[tov_NLTulips] = 0.1;
	g_weed_percent[tov_NLGrassGrazed1] = 0.1;
	g_weed_percent[tov_NLGrassGrazed1Spring] = 0.1;
	g_weed_percent[tov_NLGrassGrazed2] = 0.1;
	g_weed_percent[tov_NLGrassGrazedLast] = 0.1;
	g_weed_percent[tov_NLPermanentGrassGrazed] = 0.1;
	g_weed_percent[tov_NLSpringBarleySpring] = 0.1;
	g_weed_percent[tov_NLMaizeSpring] = 0.1;
	g_weed_percent[tov_NLPotatoesSpring] = 0.1;
	g_weed_percent[tov_NLBeetSpring] = 0.1;
	g_weed_percent[tov_NLCarrotsSpring] = 0.1;
	g_weed_percent[tov_NLCabbageSpring] = 0.1;
	g_weed_percent[tov_NLCatchCropPea] = 0.1;
	g_weed_percent[tov_NLOrchardCrop] = 0.1;
	g_weed_percent[tov_NLPermanentGrassGrazedExtensive] = 0.1;
	g_weed_percent[tov_NLGrassGrazedExtensive1] = 0.1;
	g_weed_percent[tov_NLGrassGrazedExtensive1Spring] = 0.1;
	g_weed_percent[tov_NLGrassGrazedExtensive2] = 0.1;
	g_weed_percent[tov_NLGrassGrazedExtensiveLast] = 0.1;

	g_weed_percent[tov_UKBeans] = 0.1;
	g_weed_percent[tov_UKBeet] = 0.1;
	g_weed_percent[tov_UKMaize] = 0.1;
	g_weed_percent[tov_UKPermanentGrass] = 0.1;
	g_weed_percent[tov_UKPotatoes] = 0.1;
	g_weed_percent[tov_UKSpringBarley] = 0.1;
	g_weed_percent[tov_UKTempGrass] = 0.1;
	g_weed_percent[tov_UKWinterBarley] = 0.1;
	g_weed_percent[tov_UKWinterRape] = 0.1;
	g_weed_percent[tov_UKWinterWheat] = 0.1;

	g_weed_percent[tov_BEBeet] = 0.1;
	g_weed_percent[tov_BEBeetSpring] = 0.1;
	g_weed_percent[tov_BECatchPeaCrop] = 0.1;
	g_weed_percent[tov_BEGrassGrazed1] = 0.1;
	g_weed_percent[tov_BEGrassGrazed1Spring] = 0.1;
	g_weed_percent[tov_BEGrassGrazed2] = 0.1;
	g_weed_percent[tov_BEGrassGrazedLast] = 0.1;
	g_weed_percent[tov_BEMaize] = 0.1;
	g_weed_percent[tov_BEMaizeSpring] = 0.1;
	g_weed_percent[tov_BEOrchardCrop] = 0.1;
	g_weed_percent[tov_BEPotatoes] = 0.1;
	g_weed_percent[tov_BEPotatoesSpring] = 0.1;
	g_weed_percent[tov_BEWinterBarley] = 0.1;
	g_weed_percent[tov_BEWinterWheat] = 0.1;
	g_weed_percent[tov_BEWinterBarleyCC] = 0.1;
	g_weed_percent[tov_BEWinterWheatCC] = 0.1;
	g_weed_percent[tov_BEMaizeCC] = 0.1;

	g_weed_percent[tov_PTPermanentGrassGrazed] = 0.1;
	g_weed_percent[tov_PTWinterWheat] = 0.1;
	g_weed_percent[tov_PTGrassGrazed] = 0.1;
	g_weed_percent[tov_PTSorghum] = 0.1;
	g_weed_percent[tov_PTFodderMix] = 0.1;
	g_weed_percent[tov_PTTurnipGrazed] = 0.1;
	g_weed_percent[tov_PTCloverGrassGrazed1] = 0.1;
	g_weed_percent[tov_PTCloverGrassGrazed2] = 0.1;
	g_weed_percent[tov_PTTriticale] = 0.1;
	g_weed_percent[tov_PTOtherDryBeans] = 0.1;
	g_weed_percent[tov_PTCorkOak] = 0.1;
	g_weed_percent[tov_PTVineyards] = 0.1;
	g_weed_percent[tov_PTWinterBarley] = 0.1;
	g_weed_percent[tov_PTBeans] = 0.1;
	g_weed_percent[tov_PTWinterRye] = 0.1;
	g_weed_percent[tov_PTRyegrass] = 0.1;
	g_weed_percent[tov_PTYellowLupin] = 0.1;
	g_weed_percent[tov_PTMaize] = 0.1;
	g_weed_percent[tov_PTMaize_Hort] = 0.1;
	g_weed_percent[tov_PTOats] = 0.1;
	g_weed_percent[tov_PTPotatoes] = 0.1;
	g_weed_percent[tov_PTHorticulture] = 0.1;
	g_weed_percent[tov_PTCabbage] = 0.1;
	g_weed_percent[tov_PTCabbage_Hort] = 0.1;
	g_weed_percent[tov_PTOliveGroveTraditional] = 0.1; // EZ: same as orchards
	g_weed_percent[tov_PTOliveGroveTradOrganic] = 0.1;
	g_weed_percent[tov_PTOliveGroveIntensive] = 0.1;
	g_weed_percent[tov_PTOliveGroveSuperIntensive] = 0.1;

	g_weed_percent[tov_DESugarBeet] = 0.1;
	g_weed_percent[tov_DECabbage] = 0.1;
	g_weed_percent[tov_DECarrots] = 0.1;
	g_weed_percent[tov_DEGrasslandSilageAnnual] = 0.1;
	g_weed_percent[tov_DEGreenFallow_1year] = 0.1;
	g_weed_percent[tov_DELegumes] = 0.1;
	g_weed_percent[tov_DEMaize] = 0.1;
	g_weed_percent[tov_DEMaizeSilage] = 0.1;
	g_weed_percent[tov_DEOats] = 0.1;
	g_weed_percent[tov_DEOCabbages] = 0.1;
	g_weed_percent[tov_DEOCarrots] = 0.1;
	g_weed_percent[tov_DEOGrasslandSilageAnnual] = 0.1;
	g_weed_percent[tov_DEOGreenFallow_1year] = 0.1;
	g_weed_percent[tov_DEOLegume] = 0.1;
	g_weed_percent[tov_DEOMaize] = 0.1;
	g_weed_percent[tov_DEOMaizeSilage] = 0.1;
	g_weed_percent[tov_DEOOats] = 0.1;
	g_weed_percent[tov_DEOPermanentGrassGrazed] = 0.1;
	g_weed_percent[tov_DEOPotatoes] = 0.1;
	g_weed_percent[tov_DEOSpringRye] = 0.1;
	g_weed_percent[tov_DEOSugarBeet] = 0.1;
	g_weed_percent[tov_DEOTriticale] = 0.1;
	g_weed_percent[tov_DEOWinterBarley] = 0.1;
	g_weed_percent[tov_DEOWinterRape] = 0.1;
	g_weed_percent[tov_DEOWinterRye] = 0.1;
	g_weed_percent[tov_DEOWinterWheat] = 0.1;
	g_weed_percent[tov_DEPermanentGrassGrazed] = 0.1;
	g_weed_percent[tov_DEPermanentGrassLowYield] = 0.1;
	g_weed_percent[tov_DEOPermanentGrassLowYield] = 0.1;
	g_weed_percent[tov_DEPotatoes] = 0.1;
	g_weed_percent[tov_DEPotatoesIndustry] = 0.1;
	g_weed_percent[tov_DESpringRye] = 0.1;
	g_weed_percent[tov_DETriticale] = 0.1;
	g_weed_percent[tov_DEWinterRye] = 0.1;
	g_weed_percent[tov_DEWinterBarley] = 0.1;
	g_weed_percent[tov_DEWinterRape] = 0.1;
	g_weed_percent[tov_DEWinterWheat] = 0.1;
	g_weed_percent[tov_DEWinterWheatLate] = 0.1;
	g_weed_percent[tov_DEAsparagusEstablishedPlantation] = 0.1;
	g_weed_percent[tov_DEOAsparagusEstablishedPlantation] = 0.1;
	g_weed_percent[tov_DEHerbsPerennial_1year] = 0.1;
	g_weed_percent[tov_DEHerbsPerennial_after1year] = 0.1;
	g_weed_percent[tov_DEOHerbsPerennial_1year] = 0.1;
	g_weed_percent[tov_DEOHerbsPerennial_after1year] = 0.1;
	g_weed_percent[tov_DESpringBarley] = 0.1;
	g_weed_percent[tov_DEOrchard] = 0.1;
	g_weed_percent[tov_DEOOrchard] = 0.1;
	g_weed_percent[tov_DEPeas] = 0.1;
	g_weed_percent[tov_DEOPeas] = 0.1;
	g_weed_percent[tov_DEBushFruitPerm] = 0.1;
	g_weed_percent[tov_DEOBushFruitPerm] = 0.1;

	g_weed_percent[tov_DKOLupines] = 0.1;
	g_weed_percent[tov_DKOLentils] = 0.1;
	g_weed_percent[tov_DKOLegume_Peas] = 0.1;
	g_weed_percent[tov_DKOLegume_Beans] = 0.1;
	g_weed_percent[tov_DKOLegume_Whole] = 0.1;
	g_weed_percent[tov_DKOLegume_Peas_CC] = 0.1;
	g_weed_percent[tov_DKOLegume_Beans_CC] = 0.1;
	g_weed_percent[tov_DKOLegume_Whole_CC] = 0.1;
	g_weed_percent[tov_DKCatchCrop] = 0.1;
	g_weed_percent[tov_DKOCatchCrop] = 0.1;
	g_weed_percent[tov_DKSugarBeets] = 0.1;
	g_weed_percent[tov_DKFodderBeets] = 0.1;
	g_weed_percent[tov_DKOFodderBeets] = 0.1;
	g_weed_percent[tov_DKOSugarBeets] = 0.1;
	g_weed_percent[tov_DKCabbages] = 0.1;
	g_weed_percent[tov_DKOCabbages] = 0.1;
	g_weed_percent[tov_DKCarrots] = 0.1;
	g_weed_percent[tov_DKOLegumeCloverGrass_Whole] = 0.1;
	g_weed_percent[tov_DKOCarrots] = 0.1;
	g_weed_percent[tov_DKLegume_Whole] = 0.1;
	g_weed_percent[tov_DKLegume_Peas] = 0.1;
	g_weed_percent[tov_DKLegume_Beans] = 0.1;
	g_weed_percent[tov_DKWinterWheat] = 0.1;
	g_weed_percent[tov_DKOWinterWheat] = 0.1;
	g_weed_percent[tov_DKWinterWheat_CC] = 0.1;
	g_weed_percent[tov_DKOWinterWheat_CC] = 0.1;
	g_weed_percent[tov_DKSpringBarley] = 0.1;
	g_weed_percent[tov_DKOSpringBarley] = 0.1;
	g_weed_percent[tov_DKSpringBarley_CC] = 0.1;
	g_weed_percent[tov_DKOSpringBarley_CC] = 0.1;
	g_weed_percent[tov_DKSpringBarleyCloverGrass] = 0.1;
	g_weed_percent[tov_DKOSpringBarleyCloverGrass] = 0.1;
	g_weed_percent[tov_DKCerealLegume] = 0.1;
	g_weed_percent[tov_DKOCerealLegume] = 0.1;
	g_weed_percent[tov_DKCerealLegume_Whole] = 0.1;
	g_weed_percent[tov_DKOCerealLegume_Whole] = 0.1;
	g_weed_percent[tov_DKOCloverGrassGrazed3] = 0.1;
	g_weed_percent[tov_DKOCloverGrassGrazed2] = 0.1;
	g_weed_percent[tov_DKOCloverGrassGrazed1] = 0.1;
	g_weed_percent[tov_DKOWinterCloverGrassGrazedSown] = 0.1;
	g_weed_percent[tov_DKCloverGrassGrazed3] = 0.1;
	g_weed_percent[tov_DKCloverGrassGrazed2] = 0.1;
	g_weed_percent[tov_DKCloverGrassGrazed1] = 0.1;
	g_weed_percent[tov_DKWinterCloverGrassGrazedSown] = 0.1;
	g_weed_percent[tov_DKSpringFodderGrass] = 0.1;
	g_weed_percent[tov_DKWinterFodderGrass] = 0.1;
	g_weed_percent[tov_DKOSpringFodderGrass] = 0.1;
	g_weed_percent[tov_DKOWinterFodderGrass] = 0.1;
	g_weed_percent[tov_DKGrazingPigs] = 0.1;
	g_weed_percent[tov_DKMaize] = 0.05;
	g_weed_percent[tov_DKMaizeSilage] = 0.05;
	g_weed_percent[tov_DKMixedVeg] = 0.1;
	g_weed_percent[tov_DKOGrazingPigs] = 0.1;
	g_weed_percent[tov_DKOMaize] = 0.05;
	g_weed_percent[tov_DKOMaizeSilage] = 0.05;
	g_weed_percent[tov_DKOMixedVeg] = 0.1;
	g_weed_percent[tov_DKOPotato] = 0.1;
	g_weed_percent[tov_DKOPotatoIndustry] = 0.1;
	g_weed_percent[tov_DKOPotatoSeed] = 0.1;
	g_weed_percent[tov_DKOptimalFlowerMix1] = 0.1;
	g_weed_percent[tov_DKOptimalFlowerMix2] = 0.1;
	g_weed_percent[tov_DKOptimalFlowerMix3] = 0.1;
	g_weed_percent[tov_DKOSeedGrassRye_Spring] = 0.1;
	g_weed_percent[tov_DKOSetAside] = 0.1;
	g_weed_percent[tov_DKOSetAside_AnnualFlower] = 0.1;
	g_weed_percent[tov_DKOSetAside_PerennialFlower] = 0.1;
	g_weed_percent[tov_DKOSetAside_SummerMow] = 0.1;
	g_weed_percent[tov_DKOSpringBarleySilage] = 0.1;
	g_weed_percent[tov_DKOSpringOats] = 0.1;
	g_weed_percent[tov_DKOSpringOats_CC] = 0.1;
	g_weed_percent[tov_DKOSpringWheat] = 0.1;
	g_weed_percent[tov_DKOVegSeeds] = 0.1;
	g_weed_percent[tov_DKOWinterBarley] = 0.1;
	g_weed_percent[tov_DKOWinterRape] = 0.05;
	g_weed_percent[tov_DKOWinterRye] = 0.1;
	g_weed_percent[tov_DKOWinterRye_CC] = 0.1;
	g_weed_percent[tov_DKPotato] = 0.1;
	g_weed_percent[tov_DKPotatoIndustry] = 0.1;
	g_weed_percent[tov_DKPotatoSeed] = 0.1;
	g_weed_percent[tov_DKSeedGrassFescue_Spring] = 0.1;
	g_weed_percent[tov_DKSeedGrassRye_Spring] = 0.1;
	g_weed_percent[tov_DKSetAside] = 0.1;
	g_weed_percent[tov_DKSetAside_SummerMow] = 0.1;
	g_weed_percent[tov_DKSpringBarley_Green] = 0.1;
	g_weed_percent[tov_DKSpringBarleySilage] = 0.1;
	g_weed_percent[tov_DKSpringOats] = 0.1;
	g_weed_percent[tov_DKSpringOats_CC] = 0.1;
	g_weed_percent[tov_DKSpringWheat] = 0.1;
	g_weed_percent[tov_DKUndefined] = 0.1;
	g_weed_percent[tov_DKVegSeeds] = 0.1;
	g_weed_percent[tov_DKWinterBarley] = 0.1;
	g_weed_percent[tov_DKWinterRape] = 0.05;
	g_weed_percent[tov_DKWinterRye] = 0.1;
	g_weed_percent[tov_DKWinterRye_CC] = 0.1;
	g_weed_percent[tov_DKOOrchardCrop_Perm] = 0.1;
	g_weed_percent[tov_DKOrchardCrop_Perm] = 0.1;
	g_weed_percent[tov_DKOrchApple] = 0.1;
	g_weed_percent[tov_DKOrchPear] = 0.1;
	g_weed_percent[tov_DKOrchCherry] = 0.1;
	g_weed_percent[tov_DKOrchOther] = 0.1;
	g_weed_percent[tov_DKOOrchApple] = 0.1;
	g_weed_percent[tov_DKOOrchPear] = 0.1;
	g_weed_percent[tov_DKOOrchCherry] = 0.1;
	g_weed_percent[tov_DKOOrchOther] = 0.1;
	g_weed_percent[tov_DKBushFruit_Perm1] = 0.1;
	g_weed_percent[tov_DKBushFruit_Perm2] = 0.1;
	g_weed_percent[tov_DKOBushFruit_Perm1] = 0.1;
	g_weed_percent[tov_DKOBushFruit_Perm2] = 0.1;
	g_weed_percent[tov_DKChristmasTrees_Perm] = 0.1;
	g_weed_percent[tov_DKOChristmasTrees_Perm] = 0.1;
	g_weed_percent[tov_DKEnergyCrop_Perm] = 0.1;
	g_weed_percent[tov_DKOEnergyCrop_Perm] = 0.1;
	g_weed_percent[tov_DKFarmForest_Perm] = 0.1;
	g_weed_percent[tov_DKOFarmForest_Perm] = 0.1;
	g_weed_percent[tov_DKGrazingPigs_Perm] = 0.1;
	g_weed_percent[tov_DKOGrazingPigs_Perm] = 0.1;
	g_weed_percent[tov_DKOGrassGrazed_Perm] = 0.1;
	g_weed_percent[tov_DKOGrassLowYield_Perm] = 0.1;
	g_weed_percent[tov_DKFarmYoungForest_Perm] = 0.1;
	g_weed_percent[tov_DKOFarmYoungForest_Perm] = 0.1;
	g_weed_percent[tov_DKPlantNursery_Perm] = 0.1;
	g_weed_percent[tov_DKGrassGrazed_Perm] = 0.1;
	g_weed_percent[tov_DKGrassLowYield_Perm] = 0.1;
	g_weed_percent[tov_DKCatchCrop] = 0.1;
	g_weed_percent[tov_DKGrassTussocky_Perm] = 0.1;

	g_weed_percent[tov_FIWinterWheat] = 0.1;
	g_weed_percent[tov_FIOWinterWheat] = 0.1;
	g_weed_percent[tov_FISugarBeet] = 0.1;
	g_weed_percent[tov_FIStarchPotato_North] = 0.1;
	g_weed_percent[tov_FIStarchPotato_South] = 0.1;
	g_weed_percent[tov_FIOStarchPotato_North] = 0.1;
	g_weed_percent[tov_FIOStarchPotato_South] = 0.1;
	g_weed_percent[tov_FISpringWheat] = 0.1;
	g_weed_percent[tov_FIOSpringWheat] = 0.1;
	g_weed_percent[tov_FITurnipRape] = 0.05;
	g_weed_percent[tov_FIOTurnipRape] = 0.05;
	g_weed_percent[tov_FISpringRape] = 0.05;
	g_weed_percent[tov_FIOSpringRape] = 0.05;
	g_weed_percent[tov_FIWinterRye] = 0.1;
	g_weed_percent[tov_FIOWinterRye] = 0.1;
	g_weed_percent[tov_FIPotato_North] = 0.1;
	g_weed_percent[tov_FIPotato_South] = 0.1;
	g_weed_percent[tov_FIOPotato_North] = 0.1;
	g_weed_percent[tov_FIOPotato_South] = 0.1;
	g_weed_percent[tov_FIPotatoIndustry_North] = 0.1;
	g_weed_percent[tov_FIPotatoIndustry_South] = 0.1;
	g_weed_percent[tov_FIOPotatoIndustry_North] = 0.1;
	g_weed_percent[tov_FIOPotatoIndustry_South] = 0.1;
	g_weed_percent[tov_FISpringOats] = 0.1;
	g_weed_percent[tov_FIOSpringOats] = 0.1;
	g_weed_percent[tov_FISpringBarley_Malt] = 0.1;
	g_weed_percent[tov_FIOSpringBarley_Malt] = 0.1;
	g_weed_percent[tov_FIFabaBean] = 0.1;
	g_weed_percent[tov_FIOFabaBean] = 0.1;
	g_weed_percent[tov_FISpringBarley_Fodder] = 0.1;
	g_weed_percent[tov_FISprSpringBarley_Fodder] = 0.1;
	g_weed_percent[tov_FIOSpringBarley_Fodder] = 0.1;
	g_weed_percent[tov_FIGrasslandPasturePerennial1] = 0.1;
	g_weed_percent[tov_FIGrasslandPasturePerennial2] = 0.1;
	g_weed_percent[tov_FIGrasslandSilagePerennial1] = 0.1;
	g_weed_percent[tov_FIGrasslandSilagePerennial2] = 0.1;
	g_weed_percent[tov_FINaturalGrassland] = 0.1; 
	g_weed_percent[tov_FIFeedingGround] = 0.1;
	g_weed_percent[tov_FIGreenFallow_1year] = 0.1;
	g_weed_percent[tov_FIBufferZone] = 0.1;
	g_weed_percent[tov_FIGrasslandSilageAnnual] = 0.1;
	g_weed_percent[tov_FICaraway1] = 0.1;
	g_weed_percent[tov_FICaraway2] = 0.1;
	g_weed_percent[tov_FIOCaraway1] = 0.1;
	g_weed_percent[tov_FIOCaraway2] = 0.1;
	g_weed_percent[tov_FINaturalGrassland_Perm] = 0.1; 
	g_weed_percent[tov_FIGreenFallow_Perm] = 0.1;
	g_weed_percent[tov_FIBufferZone_Perm] = 0.1;

	g_weed_percent[tov_SESpringBarley] = 0.1;
	g_weed_percent[tov_SEWinterRape_Seed] = 0.05;
	g_weed_percent[tov_SEWinterWheat] = 0.1;

	g_weed_percent[tov_IRSpringWheat] = 0.1;
	g_weed_percent[tov_IRSpringBarley] = 0.1;
	g_weed_percent[tov_IRSpringOats] = 0.1;
	g_weed_percent[tov_IRGrassland_no_reseed] = 0.1;
	g_weed_percent[tov_IRGrassland_reseed] = 0.1;
	g_weed_percent[tov_IRWinterWheat] = 0.1;
	g_weed_percent[tov_IRWinterBarley] = 0.1;
	g_weed_percent[tov_IRWinterOats] = 0.1;
	g_weed_percent[tov_FRWinterWheat] = 0.1;
	g_weed_percent[tov_FRWinterBarley] = 0.1;
	g_weed_percent[tov_FRWinterTriticale] = 0.1;
	g_weed_percent[tov_FRWinterRape] = 0.05;
	g_weed_percent[tov_FRMaize] = 0.05;
	g_weed_percent[tov_FRMaize_Silage] = 0.05;
	g_weed_percent[tov_FRSpringBarley] = 0.1;
	g_weed_percent[tov_FRGrassland] = 0.1;
	g_weed_percent[tov_FRGrassland_Perm] = 0.1;
	g_weed_percent[tov_FRSpringOats] = 0.1;
	g_weed_percent[tov_FRSunflower] = 0.05;
	g_weed_percent[tov_FRSpringWheat] = 0.1;
	g_weed_percent[tov_FRPotatoes] = 0.1;
	g_weed_percent[tov_FRSorghum] = 0.05;

	g_weed_percent[tov_ITGrassland] = 0.1;
	g_weed_percent[tov_ITOrchard] = 0.1;
	g_weed_percent[tov_ITOOrchard] = 0.1;

	m_newoldgrowthindex2 = 0;
}

void VegElement::ReadBugPercentageFile( void ) {
    FILE* lm_ifile=fopen(l_el_bug_percentage_file.value(), "r" );
  if ( !lm_ifile ) {
    g_msg->Warn( WARN_FILE, "PlantGrowthData::ReadBugPercentageFile(): Unable to open file", l_el_bug_percentage_file.value() );
    exit( 1 );
  }
  for ( int i = 0; i < tov_Undefined; i++ ) {
    int vegnum;
    // **cjt** modified 31/01/2004
    double weedpercent, bugpercent_a, bugpercent_b, bugpercent_c, bugpercent_d;
    if ( 2 != fscanf( lm_ifile, "%d %lf %lf %lf %lf %lf", & vegnum, & weedpercent, & bugpercent_a, & bugpercent_b,
         & bugpercent_c, & bugpercent_d ) ) {
           char vegnums[ 20 ];
           sprintf( vegnums, "%d", tov_Undefined );
           g_msg->Warn( WARN_FILE,
                "VegElement::ReadBugPercentageFile(): Unable to read"
                " sufficient number of int/double pairs from bug percentage file."" Lines expected:", vegnums );
           exit( 1 );
    }
    //FloatToDouble( g_weed_percent[ vegnum ], weedpercent);
	//FloatToDouble( g_bug_percent_a[ vegnum ] , bugpercent_a);
    //FloatToDouble( g_bug_percent_b[ vegnum ] , bugpercent_b);
    //FloatToDouble( g_bug_percent_c[ vegnum ] , bugpercent_c);
    //FloatToDouble( g_bug_percent_d[ vegnum ] , bugpercent_d);
	g_weed_percent[ vegnum ] = weedpercent;
	g_bug_percent_a[ vegnum ] = bugpercent_a;
    g_bug_percent_b[ vegnum ] = bugpercent_b;
    g_bug_percent_c[ vegnum ] = bugpercent_c;
    g_bug_percent_d[ vegnum ] = bugpercent_d;
  }
  fclose( lm_ifile );
}

void VegElement::RecalculateBugsNStuff(void) {
	/** This is the heart of the dynamics of vegetation elements. It calculates vegetation cover and uses this to determine vegetation biomass.
	It also calculates spilled grain and goose forage, as well a calculating insect biomass, vegetation density and dead biomass*/
	m_newgrowth = 0;
	m_veg_cover = 1.0 - (exp(-cfg_beer_law_extinction_coef.value()*m_LAtotal )); // Beer's Law to give cover
	double usefull_veg_cover = 1.0 - (exp(m_LAgreen * -0.4)); // This is used to calc growth rate
	// Need gloabal radiation today
	double glrad = m_Landscape->SupplyGlobalRadiation();
	// This is different for maize (a C4 plant)
	int ptype;
	if ((m_vege_type == tov_Maize) || (m_vege_type == tov_OMaizeSilage) || (m_vege_type == tov_MaizeSilage) || (m_vege_type == tov_MaizeStrigling)) ptype = 1; else ptype = 0;
	int index = (int)floor(0.5 + m_Landscape->SupplyTemp()) + 30; // There are 30 negative temps
	double radconv = c_SolarConversion[ptype][index];
	if (m_LAtotal >= m_oldLAtotal) {
		// we are in positive growth so grow depending on our equation
		m_newgrowth = usefull_veg_cover * glrad * radconv * m_biomass_scale[m_vege_type];
		if (m_owner_index != -1) { // This only works because only crops and similar structures have owners
			double fintensity = m_Landscape->SupplyFarmIntensity(m_poly);
			if (fintensity >= 1) {
				// 1 means extensive, so reduce vegetation biomass by 20%
				// NB this cannot be used with extensive crop types otherwise you get an additional 20% reduction
				// This way of doing things provides a quick and dirty general effect.
				m_veg_biomass += m_newgrowth * 0.8;
			}
			else m_veg_biomass += m_newgrowth;
		}
		else m_veg_biomass += m_newgrowth;
	}
	else {
		// Negative growth - so shrink proportional to the loss in LAI Total
		if (m_oldLAtotal > 0) {
			double temp_propotion = m_LAtotal / m_oldLAtotal;
			//m_newgrowth = m_veg_biomass*(temp_propotion-1);
			m_veg_biomass *= temp_propotion;
		}
	}
	/** Here we also want to know how much biomass we have on the field in total. So we multiply the current biomass by area */
	m_total_biomass = m_veg_biomass * m_area;
	m_total_biomass_old = m_total_biomass;
	// NB The m_weed_biomass is calculated directly from the curve in Curves.pre
	// rather than going through the rigmarole of converting leaf-area index
	m_veg_density = (int)(floor(0.5 + (m_veg_biomass / (1 + m_veg_height))));
	if (m_veg_density > 100) m_veg_density = 100; // to stop array bounds problems
	if (m_LAtotal == 0.0) m_green_biomass = 0.0;
	else m_green_biomass = m_veg_biomass * (m_LAgreen / (m_LAtotal));
	m_dead_biomass = m_veg_biomass - m_green_biomass;
	// Here we use our member function pointer to call the correct piece of code for our current species
	(this->*(SpeciesSpecificCalculations))();
}

void VegElement::SetSpeciesFunction(TTypesOfPopulation a_species)
{
	switch (a_species)
	{
	case TOP_Skylark:
		SpeciesSpecificCalculations = &VegElement::CalculateInsectBiomass;
		break;
	default:
		SpeciesSpecificCalculations = &VegElement::DoNothing;
	}
}

void VegElement::CalculateDigestibility()
{
	/** This is used for hare and voles. It is a 32-day running sum of the amount of new growth per day divided by total veg biomass with a minimum value of 0.5 */
	++m_newoldgrowthindex &= 31;
	if ((m_veg_biomass+m_weed_biomass) > 0) {
		switch (m_vege_type) {
		case tov_NoGrowth:
		case tov_None:
		case tov_OFirstYearDanger:
			m_digestability = 0.0;
			break;
		case tov_OPotatoes:
		case tov_Potatoes:
		case tov_PotatoesIndustry:
		case tov_PLMaizeSilage:
		case tov_PLPotatoes:
		case tov_NLPotatoes:
		case tov_NLPotatoesSpring:
		case tov_UKPotatoes:
		case tov_DEPotatoes:
		case tov_PTPotatoes:
			m_digestability = 0.5;
			break;
		default:
			//m_oldnewgrowth[m_newoldgrowthindex]=(newgrowth/m_veg_biomass);
			m_oldnewgrowth[m_newoldgrowthindex] = (m_newgrowth+m_new_weed_growth);
			m_newgrowthsum = 0.0;
			for (int i = 0; i < 32; i++) {
				m_newgrowthsum += m_oldnewgrowth[i];
			}
			m_digestability = m_newgrowthsum / (m_veg_biomass+m_weed_biomass);
			m_digestability += 0.5;
			if (m_digestability > 0.8) m_digestability = 0.8;
		}
	}
	else {
		m_oldnewgrowth[m_newoldgrowthindex] = 0.0;
		m_digestability = 0.0;
	}
}

void VegElement::CalculateAphidDrivers()
{
	/** This is meant to be used for aphids, but may calculate useful things for other organisms. */

	//Calculation of the green biomass percentage to the total biomass.
	m_greenbiomass_per = 0;
	if(m_veg_biomass+m_weed_biomass>0){
		m_greenbiomass_per = (m_green_biomass+m_weed_biomass)/(m_veg_biomass+m_weed_biomass);
	}
}

void VegElement::SetInterestedBiomassFractionForCrop(TTypesOfCrops a_crop_type){
	switch (a_crop_type){
		case toc_CloverGrassGrazed1:
		case toc_CloverGrassGrazed2:
		case toc_OCloverGrassGrazed1:
		case toc_OCloverGrassGrazed2:
			m_interested_biomass_fraction = cfg_clover_interested_biomass_fraction.value();
			break;
		default:
			m_interested_biomass_fraction = 1.0; //by default, all biomass is interesting
	}
}

void VegElement::SetInsectBiomassParametersIndex(TTypesOfCrops a_type){
	switch (a_type) {
		//spring barley: 1
	case toc_OSpringBarley:
	case toc_OSpringBarleyCloverGrass:
	case toc_OSpringBarleyExtensive:
	case toc_OSpringBarleyPigs:
	case toc_OSpringBarleyPeaCloverGrass:
	case toc_OSpringBarleySilage:
	case toc_OSpringWheat:
	case toc_SpringBarley:
	case toc_SpringBarleyCloverGrass:
	case toc_SpringBarleyPeaCloverGrass:
	case toc_SpringBarleySeed:
	case toc_SpringBarleySilage:
	case toc_OSBarleySilage:
	case toc_Oats:
	case toc_OBarleyPeaCloverGrass:
	case toc_OOats:
		m_insect_biomass_parameters_index = 1;
		break;
		//winter wheat: 2
	case toc_SpringWheat:
	case toc_Triticale:
	case toc_OTriticale:
	case toc_WinterBarley:
	case toc_WinterWheat:
	case toc_OWinterBarley:
	case toc_OWinterWheat:
	case toc_OWinterWheatUndersown:
	case toc_OWinterWheatUndersownExtensive:
	case toc_WinterTriticale:
		m_insect_biomass_parameters_index = 2;
		break;
		//winter rye: 3
	case toc_OWinterRye:
	case toc_OSpringRye:
	case toc_SpringRye:
	case toc_WinterRye:
		m_insect_biomass_parameters_index = 3;
		break;
		//winter rape: 4
	case toc_Beet:
	case toc_FodderBeet:
	case toc_OSugarBeet:
	case toc_Tulips:
	case toc_OPotatoes:
	case toc_OPotatoesIndustry:
	case toc_OPotatoesSeed:
	case toc_OStarchPotato:
	case toc_OCabbage:
	case toc_OCarrots:
	case toc_StarchPotato:
	case toc_SugarBeet:
	case toc_Potatoes:
	case toc_PotatoesIndustry:
	case toc_PotatoesSeed:
	case toc_PotatoesSpring:
	case toc_Cabbage:
	case toc_CabbageSpring:
	case toc_Carrots:
	case toc_CarrotsSpring:
	case toc_Maize:
	case toc_MaizeSilage:
	case toc_MaizeSpring:
	case toc_MaizeStrigling:
	case toc_OMaize:
	case toc_OMaizeSilage:
	case toc_OSpringRape:
	case toc_OWinterRape:
	case toc_SpringRape:
	case toc_WinterRape:
		m_insect_biomass_parameters_index = 4;
		break;
		//Cropped/Grazed Grass: 5
	case toc_OPermanentGrassGrazed:
	case toc_OPermanentGrassLowYield:
	case toc_CloverGrassGrazed1:
	case toc_CloverGrassGrazed2:
	case toc_CloverGrassGrazed3:
	case toc_GrassGrazed1:
	case toc_GrassGrazed2:
	case toc_GrassGrazedExtensive:
	case toc_GrassGrazedLast:
	case toc_PermanentGrassGrazed:
	case toc_PermanentGrassLowYield:
	case toc_PermanentGrassTussocky:
	case toc_OCloverGrassGrazed1:
	case toc_OCloverGrassGrazed2:
	case toc_OCloverGrassGrazed3:
	case toc_OCloverGrassSilage1:
		m_insect_biomass_parameters_index = 5;
		break;
		// Edges and really nice places 6
	case toc_PermanentSetAside:
		m_insect_biomass_parameters_index = 6;
		break;
		//Setaside etc: 7
	case toc_SetAside:
	case toc_OSetAside:
	case toc_OSetAside_Flower:
	case toc_YoungForestCrop:
		m_insect_biomass_parameters_index = 7;
		break;
	case toc_FarmForest:
	case toc_Sunflower:
	case toc_Turnip:
	case toc_Vineyards:
	case toc_VegSeeds:
	case toc_YellowLupin:
	case toc_AsparagusEstablishedPlantation:
	case toc_Beans:
	case toc_Beans_Whole:
	case toc_BushFruit:
	case toc_CatchCropPea:
	case toc_CorkOak:
	case toc_DummyCropPestTesting:
	case toc_FieldPeas:
	case toc_FieldPeasSilage:
	case toc_FieldPeasStrigling:
	case toc_FodderGrass:
	case toc_FodderLucerne1:
	case toc_FodderLucerne2:
	case toc_GenericCatchCrop:
	case toc_MixedVeg:
	case toc_OAsparagusEstablishedPlantation:
	case toc_OBeans:
	case toc_OBeans_Whole:
	case toc_OBushFruit:
	case toc_OFarmForest:
	case toc_OFieldPeas:
	case toc_OFieldPeasSilage:
	case toc_OFirstYearDanger:
	case toc_OFodderBeet:
	case toc_OFodderGrass:
	case toc_OGrazingPigs:
	case toc_OliveGrove:
	case toc_OLentils:
	case toc_OLupines:
	case toc_OMixedVeg:
	case toc_OOrchApple:
	case toc_OOrchardCrop:
	case toc_OOrchCherry:
	case toc_OOrchOther:
	case toc_OOrchPear:
	case toc_OrchApple:
	case toc_OrchardCrop:
	case toc_OrchCherry:
	case toc_OrchOther:
	case toc_OrchPear:
	case toc_ORyeGrass:
	case toc_OSeedGrass1:
	case toc_OSeedGrass2:
	case toc_OVegSeeds:
	case toc_OWinterBarleyExtensive:
	case toc_OYoungForestCrop:
	case toc_Ryegrass:
	case toc_ORyegrass:
	case toc_SeedGrass1:
	case toc_SeedGrass2:
	case toc_Sorghum:
		m_insect_biomass_parameters_index = 0;
		break;
		// Zero insects
	case toc_PlantNursery:
	case toc_GrazingPigs:
	case toc_Horticulture:
		m_insect_biomass_parameters_index = 8;
		break;

	}
}

void VegElement::CalculateInsectBiomass()
{
	// The insect calculation part
	// Bugmass = a + b(biomass) + c(height)
	//double temp_bugmass = //g_bug_percent_d[ m_vege_type ] // This was used as a scaler - now not used
	//	g_bug_percent_a[m_vege_type] + ((m_veg_biomass + m_weed_biomass) * g_bug_percent_b[m_vege_type])
	//	+ (m_veg_height * g_bug_percent_c[m_vege_type]);
	double temp_bugmass = m_insect_biomass_parameters_a[m_insect_biomass_parameters_index]
	+ ((m_dead_biomass*0.1+m_green_biomass + m_weed_biomass) * m_insect_biomass_parameters_b[m_insect_biomass_parameters_index])
	+ (m_veg_height *m_insect_biomass_parameters_c[m_insect_biomass_parameters_index]);
	temp_bugmass *= m_SeasonalInsectScaler[g_date->GetMonthRaw()]; // Raw version is zero based
	// Set a minimum value (regressions will otherwise sometimes give a -ve value
	if (temp_bugmass < 0.05) temp_bugmass = 0.05;
	temp_bugmass *= cfg_insectbiomassscaling.value();
	// Now need to check for deviations caused by management
	// First spot the deviation - this is easy because the only deviation that does
	// not affect the vegetation too is insecticide spraying
	if (m_days_since_insecticide_spray > 0) {
		// Need to change insects still, so grow towards the target, but only when 21 days from zero effect
		if (m_days_since_insecticide_spray < 21) m_insect_pop += (temp_bugmass - m_insect_pop) / m_days_since_insecticide_spray;
		m_days_since_insecticide_spray--;
	}
	else {
		m_insect_pop = temp_bugmass;
	}
}

void VegElement::CalcGooseForageResources()
{
	// For geese that eat spilled grain and maize we need to remove some of this daily (loss to other things than geese)
	// Get the Julian day
	int day = g_date->DayInYear();
	double rate, maizerate;
    if (cfg_goose_UniformDecayRate.value()){
        rate = cfg_goose_GrainDecayRateWinter.value();
        maizerate = cfg_goose_MaizeDecayRateWinter.value();
    }
    else{
        if ((day > March) && (day < July)){
            rate = cfg_goose_GrainDecayRateSpring.value();
            maizerate = cfg_goose_MaizeDecayRateSpring.value();
        }
        else {
            rate = cfg_goose_GrainDecayRateWinter.value();
            maizerate = cfg_goose_MaizeDecayRateWinter.value();
        }
    }

    //std::cout<<"DEBUG: grain decay rate is "<<rate<<"\n";
    if (m_birdseedforage>0){
        m_birdseedforage *= rate;
    }


	if (m_birdseedforage < 0.01) m_birdseedforage = 0.0;
    if (m_birdmaizeforage>0){
        m_birdmaizeforage *= maizerate;
    }

	if (m_birdmaizeforage < 0.01) m_birdmaizeforage = 0.0;
	// We also need to calculate non-grain forage for geese
	if (IsCereal()) {
		//if (m_green_biomass > 0.5)  //Testing if this could be a suitable fix for the cereals
		//{
			for (unsigned i = 0; i < gs_foobar; i++) {
				/** The 1.0325 is a quick fix to account for higher energy intake on winter cereal - Based on Therkildsen & Madsen 2000 Energetics of feeding...*/
				m_goosegrazingforage[ i ] = m_Landscape->SupplyGooseGrazingForageH( m_veg_height, (GooseSpecies)i ) * cfg_goose_grass_to_winter_cereal_scaler.value();
			}
		//}
		//else for (unsigned i = 0; i < gs_foobar; i++) {
		//	m_goosegrazingforage[i] = 0.0;
		//}
		//m_goosegrazingforage[gs_foobar] = 1; // Is cereal
	}
	/** or potentially it is a grazable grass */
	else {
		if (IsGooseGrass()) {
			for (unsigned i = 0; i < gs_foobar; i++) {
				//m_goosegrazingforage[ i ] = 0.0;
				m_goosegrazingforage[i] = m_Landscape->SupplyGooseGrazingForageH(m_veg_height, (GooseSpecies)i);
			}
		}
		else for (unsigned i = 0; i < gs_foobar; i++)	m_goosegrazingforage[i] = 0.0;
	}
}

void VegElement::RandomVegStartValues( double * a_LAtotal, double * a_LAgreen, double * a_veg_height, double * a_weed_biomass ) {
  * a_LAtotal = EL_VEG_START_LAIT * ( ( ( ( double )( g_random_fnc( 21 ) - 10 ) ) / 100.0 ) + 1.0 ); // +/- 10%
  * a_LAgreen = * a_LAtotal / 4.0;
  * a_veg_height = * a_LAgreen * EL_VEG_HEIGHTSCALE;
  * a_weed_biomass = * a_LAgreen * 0.1; // 10% weeds by biomass
}


void VegElement::SetGrowthPhase(int a_phase) {

	if (a_phase == sow) {
		m_vegddegs = 0.0;
	}
	else if (a_phase == harvest1) m_vegddegs = -1;
	if (a_phase == janfirst) {
		m_forced_phase_shift = false;
		/**
		* If it is the first growth phase of the year then we might cause some unnecessary hops if e.g. our biomass is 0 and we suddenly jump up to
		* 20 cm To stop this happening we check here and if our settings are lower than the targets we do nothing.
		*/
		if (g_crops->StartValid(m_curve_num, a_phase)) {
			double temp_veg_height = g_crops->GetStartValue(m_curve_num, a_phase, 2);
			if (temp_veg_height < m_veg_height) { // Otherwise we are better off with the numbers we have to start with
				// Now with added variability
				m_LAgreen = g_crops->GetStartValue(m_curve_num, a_phase, 0);
				m_LAtotal = g_crops->GetStartValue(m_curve_num, a_phase, 1);
				m_veg_height = g_crops->GetStartValue(m_curve_num, a_phase, 2);
			}
		}

	}
	else if (g_crops->StartValid(m_curve_num, a_phase)) {
		m_LAgreen = g_crops->GetStartValue(m_curve_num, a_phase, 0);
		m_LAtotal = g_crops->GetStartValue(m_curve_num, a_phase, 1);
		m_veg_height = g_crops->GetStartValue(m_curve_num, a_phase, 2);
	}
	else if (!m_force_growth) {
		// If we are in forced growth mode (which is very likely),
		// then do not choose a new set of starting values, as we have
		// already calculated our way to a reasonable set of values.
		//RandomVegStartValues( & m_LAtotal, & m_LAgreen, & m_veg_height, & m_weed_biomass ); // **CJT** Testing removal 17/02/2015
	}
	m_veg_phase = a_phase;
	m_yddegs = 0.0;
	m_ddegs = g_weather->GetDDDegs(g_date->Date());
	m_force_growth = false;

	if (m_veg_phase == janfirst) {
		// For some growth curves there is no growth in the first
		// two months of the year. This will more likely than
		// not cause a discontinuous jump in the growth curves
		// come March first. ForceGrowthSpringTest() tries
		// to avoid that by checking for positive growth values
		// for the January growth phase. If none are found, then
		// it initializes a forced growth/transition to the March
		// 1st starting values.
		ForceGrowthSpringTest(); // Removal of this causes continuous increase in vegetation growth year on year for any curve that does not have a hard reset (e.g. harvest).
	}
}


void VegElement::ForceGrowthTest( void ) {
  // Called whenever the farmer does something 'destructive' to a
  // field, that reduced the vegetaion.
  if ( g_date->DayInYear() >= g_date->DayInYear( 1, 11 )
       || ( g_date->DayInYear() < g_date->DayInYear( 1, 3 ) && m_force_growth ) ) {
         ForceGrowthInitialize();
  }
}



void VegElement::ForceGrowthSpringTest(void) {
	// Check if there are any positive growth differentials in the curve
	// for the first two months of the year. Do nothing if there is.
	// If we have any positive growth then no need to force either
	if (g_crops->GetLAgreenDiff(90000.0, 0.0, m_curve_num, janfirst) > 0.001
		|| g_crops->GetLAtotalDiff(90000.0, 0.0, m_curve_num, janfirst) > 0.001
		|| g_crops->GetHeightDiff(90000.0, 0.0, m_curve_num, janfirst) > 0.001) {
		return;
	}
	// No growth, force it.
	ForceGrowthInitialize();
}



void VegElement::ForceGrowthInitialize( void ) {
  double LAgreen_target;
  double Weed_target;
  double LAtotal_target;
  double veg_height_target;
  int next_phase, daysleft;

  // Figure out what our target phase is.
  if ( g_date->DayInYear() < g_date->DayInYear( 3, 1 ) ) {
    daysleft = g_date->DayInYear( 1, 3 ) - g_date->DayInYear();
    next_phase = marchfirst;
  } else if ( g_date->DayInYear() >= g_date->DayInYear( 1, 11 ) ) {
    daysleft = 366 - g_date->DayInYear(); // Adjusted from 365 to prevent occaisional negative values
    next_phase = janfirst;
  } else {
    return;
  }
  if ( daysleft <= 0 )
       // Uh! Oh! This really shouldn't happen.
         return;

  if ( !g_crops->StartValid( m_curve_num, next_phase ) ) {
    // If no valid starting values for next phase, then
    // preinitialize the random starting values! Ie. make the
    // choice here and then do not choose another set come
    // next phase transition, but use the values we already
    // got at that point in time.
    RandomVegStartValues( & LAtotal_target, & LAgreen_target, & veg_height_target, & Weed_target );
  }
  else {
	  //add +/- 20% variation
	  double vari = (g_rand_uni_fnc() * 0.4) + 0.8;
	  Weed_target = g_crops->GetStartValue(m_weed_curve_num, next_phase, 0) * vari;
	  LAgreen_target = g_crops->GetStartValue(m_curve_num, next_phase, 0) * vari;
	  LAtotal_target = g_crops->GetStartValue(m_curve_num, next_phase, 1) * vari;
	  veg_height_target = g_crops->GetStartValue(m_curve_num, next_phase, 2) * vari;
  }

  m_force_growth = true;
  m_force_Weed = ( Weed_target - m_weed_biomass ) / ( double )daysleft;
  m_force_LAgreen = ( LAgreen_target - m_LAgreen ) / ( double )daysleft;
  m_force_LAtotal = ( LAtotal_target - m_LAtotal ) / ( double )daysleft;
  m_force_veg_height = ( veg_height_target - m_veg_height ) / ( double )daysleft;
}


void VegElement::ForceGrowthDevelopment( void ) {
  if ( m_herbicidedelay <= 0 ) {
	m_weed_biomass += m_force_Weed; // ***CJT*** 12th Sept 2008 - rather than force growth, weeds might be allowed to grow on their own
	if(m_force_Weed>0) m_new_weed_growth = m_force_Weed;
  }
  m_LAgreen += m_force_LAgreen;
  m_LAtotal += m_force_LAtotal;
  m_veg_height += m_force_veg_height;

  if (m_LAgreen < 0)  m_LAgreen = 0;
  if (m_LAtotal < 0)   m_LAtotal = 0;
  if (m_veg_height < 0)  m_veg_height = 0;
}



void VegElement::ZeroVeg( void ) {
  m_LAgreen = 0.0;
  m_LAtotal = 0.0;
  m_veg_height = 0.0;
  m_veg_cover = 0.0;
  m_veg_biomass = 0.0;
  m_weed_biomass = 0.0;
  m_birdseedforage = 0.0;
  m_birdmaizeforage = 0.0;
  SetStubble(false);
  ForceGrowthTest();
  RecalculateBugsNStuff();
}


void VegElement::DoDevelopment(void) {
	m_new_weed_growth = 0.0;
	if (!m_force_growth) {
		//** First does the day degree calculations */
		m_yddegs = m_ddegs;
		double pos_temp_today = g_weather->GetDDDegs(g_date->Date());
		if (m_vegddegs != -1.0) 
			m_vegddegs += pos_temp_today; // Sum up the vegetation day degrees since sowing
		m_ddegs = pos_temp_today + m_yddegs; // and sum up the phase ddegs

		double dLAG = g_crops->GetLAgreenDiffScaled(m_ddegs, m_yddegs, m_curve_num, m_veg_phase, m_growth_scaler);
		double dLAT = g_crops->GetLAtotalDiffScaled(m_ddegs, m_yddegs, m_curve_num, m_veg_phase, m_growth_scaler);
		double dHgt = g_crops->GetHeightDiffScaled(m_ddegs, m_yddegs, m_curve_num, m_veg_phase, m_growth_scaler);

		m_LAgreen += dLAG;
		if (m_LAgreen < 0.0) m_LAgreen = 0.0;
		m_LAtotal += dLAT;
		if (m_LAtotal < 0.0) m_LAtotal = 0.0;
		m_veg_height += dHgt;
		if (m_veg_height < 0.0)    m_veg_height = 0.0;

		if (this->m_owner_index != -1) { // This only works because only crops and similar structures have owners
			if (m_herbicidedelay == 0)
			{
				// Need to force some weed growth so switch growth for weeds only
				// We want to follow the original curve but reset it
				m_weedddegs = pos_temp_today;
			} else m_weedddegs += pos_temp_today;
			/** Next grows weeds proportionally to day degrees and using the weed curve if no herbicide effect before calling RecalculateBugsNStuff to caculate insect biomass, cover, digestability etc..*/
			const double fintensity = m_Landscape->SupplyFarmIntensity(m_poly);
			if (m_herbicidedelay <= 0) {
				const double dWee = g_crops->GetLAtotalDiff(m_weedddegs, m_weedddegs -pos_temp_today, m_weed_curve_num, m_veg_phase);
				m_new_weed_growth = dWee * cfg_ele_weedscaling.value() * (1+fintensity);
				m_weed_biomass += m_new_weed_growth;
				if(m_new_weed_growth<0) m_new_weed_growth = 0.0;
			}
			if (m_weed_biomass < 0.0) m_weed_biomass = 0.0;
		}
	}
	else {
		ForceGrowthDevelopment();
	}

	// check here that m_LAtotal is bigger than m_LAgreen
	if (m_LAtotal < m_LAgreen)
	{
		char error_num[100];
		sprintf(error_num, "%f < %f (force growth = %d)", m_LAtotal, m_LAgreen, m_force_growth);
		g_msg->WarnAddInfo(WARN_TRIVIAL, "Landscape::DoDevelopment(): Leaf Area Total smaller than Leaf Area Green (Veg Growth Model inconsistent). Performing hack correction.", error_num);
		m_LAtotal = 1.1 * m_LAgreen;
		// exit(1);
	}
	RecalculateBugsNStuff();
	/** Here we need to set today's goose numbers to zero in case they are not written by the goose population manager (the normal situation) */
	ResetGeese();
	// Deal with any possible unsprayed margin, transferring info as necessary
	if (GetUnsprayedMarginPolyRef() != -1) {
		m_Landscape->SupplyLEPointer(GetUnsprayedMarginPolyRef())->SetCropDataAll(m_veg_height, m_veg_biomass, m_LAtotal, m_LAgreen, m_vege_type, m_weed_biomass, m_veg_cover,
			m_cattle_grazing, m_insect_pop, m_att_veg_patchy, m_veg_density, m_dead_biomass, m_green_biomass);
	}

	m_interested_green_biomass = m_green_biomass * m_interested_biomass_fraction;
}

void VegElement::ResetGeese( void ) {
	m_gooseNos[ g_date->DayInYear() ] = 0;
	for (unsigned i = 0; i < gs_foobar; i++) {
		m_gooseSpNos[ g_date->DayInYear() ][ (GooseSpecies)i ] = 0;
		m_gooseSpNosTimed[ g_date->DayInYear() ][ (GooseSpecies)i ] = 0;
	}
}
/** \brief The function that reduces the vegetation due to grazing
 *
 * On Nov 18 Andrey has updated the function to take into account the digestability.
 * the search revealed that there is no other model is using it apart from Goose model
 * so hopefully there would be no unintended effects
 * */
void VegElement::GrazeVegetationTotal( double a_grams )
{

	GrazeVegetation( (a_grams*m_digestability)/m_area, true );
}

void VegElement::GrazeVegetationHeight(double a_reduc)
{
	/**
	* Used to calculate the change in vegetation height and biomass as a result of grazing.
	* Input parameter is the change in height. 
	* Some assumptions:
	* 1 - The grazing takes all LA equally
	* 2 - That biomass is evenly distributed
	* 3 - That LA is proportional to biomass in some way, so LA is also evenly distributed
	*/
	if (m_veg_height - a_reduc < cfg_farm_cattle_grass_low.value()){
		a_reduc = m_veg_height - cfg_farm_cattle_grass_low.value() + 0.5;
	}
	double propreduc = 1.0 - (a_reduc / m_veg_height);
	m_veg_height *= propreduc;
	m_weed_biomass *= propreduc;
	m_veg_biomass *= propreduc;
	// Need to do something with the LA too - 
	m_LAgreen *= propreduc;
	m_LAtotal *= propreduc;
	m_oldLAtotal = m_LAtotal; // this stops double reduction of biomass later in RecalculateBugsNStuff();
}

void VegElement::GrazeVegetation( double a_reduc, bool a_force )
{
	/**
	* Used to calculate the change in vegetation height and biomass as a result of grazing.
	* Input parameter is the change in wet biomass/m2. The problem is to convert this into changes in LAtotal, LAgreen and height.
	* We have an area, biomass, total biomass, height and density. If biomass is missing we need to change height and biomass before continuing and
	* and do something with LA_total and LA_Green.
	* Some assumptions:
	* 1 - The grazing takes all LA equally
	* 2 - That biomass is evenly distributed
	* 3 - That LA is proportional to biomass in some way, so LA is also evenly distributed
	* 4 - That we can use the current grazing pressure to alter a_reduc
	*/
	if (!a_force) a_reduc *= m_default_grazing_level;
	if (a_reduc >= m_veg_biomass) return;
	double propreduc = 1.0 - (a_reduc / m_veg_biomass);
	m_veg_height *= propreduc;
	m_weed_biomass *= propreduc;
	m_veg_biomass -= a_reduc;
	// Need to do something with the LA too - 
	m_LAgreen *= propreduc;
	m_LAtotal *= propreduc;
	m_oldLAtotal = m_LAtotal; // this stops double reduction of biomass later in RecalculateBugsNStuff();
}

void VegElement::ReduceVeg(double a_reduc) {
	m_LAgreen *= a_reduc;
	m_LAtotal *= a_reduc;
	m_veg_height *= a_reduc;
	m_veg_biomass *= a_reduc;
	m_weed_biomass *= a_reduc;
	ForceGrowthTest();
	m_oldLAtotal = m_LAtotal; // this stops double reduction of biomass later in RecalculateBugsNStuff();
}

void VegElement::ReduceVeg_Extended(double a_reduc) {
	m_LAgreen *= a_reduc;
	m_LAtotal *= a_reduc;
	m_veg_height *= a_reduc;
	m_veg_biomass *= a_reduc;
	m_weed_biomass *= a_reduc;

	if (a_reduc < EL_GROWTH_PHASE_SHIFT_LEVEL) {
		m_yddegs = 0.0;
		m_ddegs = EL_GROWTH_DAYDEG_MAGIC;
	}

	if (g_date->DayInYear() >= EL_GROWTH_DATE_MAGIC && a_reduc < EL_GROWTH_PHASE_SHIFT_LEVEL && !m_forced_phase_shift) {
		SetGrowthPhase(harvest1);
		m_forced_phase_shift = true;
	}
	ForceGrowthTest();
	m_oldLAtotal = m_LAtotal; // this stops double reduction of biomass later in RecalculateBugsNStuff();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

PastureElement::PastureElement(TTypesOfLandscapeElement tole, Landscape* L) : VegElement(tole, L)
{
	SetVegGrowthScalerRand();
}


Field::Field(Landscape *L) : VegElement( tole_Field, L ) {
	SetGrazingLevel(0); // By default it should be zero.
	m_CropType = toc_Foobar; // This will be replaced when a crop is allocated
    SetVegGrowthScalerRand();
}

void VegElement::SetVegType(TTypesOfVegetation a_vege_type)
{
	m_vege_type = a_vege_type;
	m_curve_num = g_crops->VegTypeToCurveNum(a_vege_type);
	if (m_unsprayedmarginpolyref != -1) {
		// Must have an unsprayed margin so need to pass the information on to it
		LE* um = m_Landscape->SupplyLEPointer(m_unsprayedmarginpolyref);
		um->SetVegType(a_vege_type);
	}
	switch (a_vege_type) {
	case tov_NaturalGrass:
	case tov_FlowerStrip1:
	case tov_FlowerStrip2:
	case tov_FlowerStrip3:
	case tov_WaterBufferZone:
		m_insect_biomass_parameters_index = 6; // High insects
		break;
	default:
		// Do nothing, handled elsewhere
		break;
	}
}

void  VegElement::SetVegType(TTypesOfVegetation a_vege_type, TTypesOfVegetation a_weed_type)
{
	SetVegType(a_vege_type);
	// -1 is used as a signal not to change the weed type
	// this is because it may be specific to that field
	if (a_weed_type != tov_Undefined) m_weed_curve_num = a_weed_type;
}


void Field::DoDevelopment( void ) {
  VegElement::DoDevelopment();
  SetSprayedToday(false); // Reset the overspray flag in case it is set
}

int Field::GetAphidDensity() { 
 // Uses the known in field coordinates to assess aphids
	return g_AManager->GetAphidDensity(m_valid_x, m_valid_y);
}
void Field::SetVegGrowthScalerRand()
{
    m_growth_scaler = (g_rand_uni_fnc() * (cfg_PermanentVegGrowthMaxScalerField.value() - cfg_PermanentVegGrowthMinScalerField.value())) + cfg_PermanentVegGrowthMinScalerField.value(); // Scales growth stochastically in a range given by the configs
}

TTypesOfVegetation Field::GetPreviousTov(int a_index) {
	return m_owner->GetPreviousTov(a_index); 
}


void VegElement::SetCropData( double a_veg_height, double a_LAtotal, double a_LAgreen, TTypesOfVegetation a_veg,
     double a_cover, int a_grazed ) {
       m_veg_height = a_veg_height;
       m_LAtotal = a_LAtotal;
       m_LAgreen = a_LAgreen;
       m_vege_type = a_veg;
       m_veg_cover = a_cover;
       m_cattle_grazing = a_grazed;
}

void VegElement::SetCropDataAll( double a_veg_height, double a_biomass, double a_LAtotal, double a_LAgreen, TTypesOfVegetation a_veg, double a_wb, double a_cover, int a_grazed, double a_ins, 
	bool a_patchy, double a_dens, double a_deadbiomass, double a_greenbiomass ) {
       m_veg_height = a_veg_height;
       m_veg_biomass = a_biomass;
       m_LAtotal = a_LAtotal;
       m_LAgreen = a_LAgreen;
       m_vege_type = a_veg;
       m_weed_biomass = a_wb;
       m_veg_cover = a_cover;
       m_cattle_grazing = a_grazed;
       m_insect_pop = a_ins;
       m_veg_density = (int) a_dens;
       m_att_veg_patchy = a_patchy;
	   m_dead_biomass = a_deadbiomass;
	   m_green_biomass = a_greenbiomass;
	   // set the veg attributes 
	   GetLandscape()->Set_TOV_Att(this);
}

void VegElement::InsectMortality( double a_fraction ) {
  m_insect_pop *= a_fraction;
}

Hedges::Hedges(Landscape *L ) : VegElement(tole_Hedges, L) {
	SetVegType(tov_NaturalGrass);
	SetSubType(0);
}

HedgeBank::HedgeBank(Landscape *L) : VegElement(tole_HedgeBank, L) {
	SetVegType(tov_NaturalGrass);
	SetSubType(0);
}

PermanentSetaside::PermanentSetaside(Landscape *L) : VegElement(tole_PermanentSetaside, L) {
  SetVegType(tov_PermanentSetAside);
  SetDigestibility(0.8);
  SetVegGrowthScalerRand();
  if (g_rand_uni_fnc() < cfg_SetAsidePatchyChance.value()) 
	  Set_Att_VegPatchy(true); 
}


BeetleBank::BeetleBank(Landscape* L) : VegElement(tole_BeetleBank, L) {
	if (g_rand_uni_fnc() < cfg_BBPatchyChance.value())
		Set_Att_VegPatchy(true);
	SetVegType(tov_NaturalGrass);
}


FlowerStrip::FlowerStrip(Landscape* L, int a_type) : VegElement(tole_FlowerStrip, L) {
	if (a_type == 1) VegElement::SetVegType(tov_FlowerStrip1);
	else if (a_type == 2) VegElement::SetVegType(tov_FlowerStrip2);
	else VegElement::SetVegType(tov_FlowerStrip3);
	// Three options for the type, each will have a different pollen nectar curve set (type 1 is default)
	SetSubType(a_type);
	m_curve_num = g_crops->VegTypeToCurveNum(m_vege_type);
	// Initially assume that all at patchy
	VegElement::Set_Att_VegPatchy(true);
	VegElement::SetVegGrowthScalerRand();
}

void FlowerStrip::Cutting(int a_today)
{
	SetLastTreatment(mow);
	float biomassBefore = GetVegBiomass();
	SetGrowthPhase(harvest1);
	m_DateCut = a_today;
	m_veg_height = RV_CUT_HEIGHT;
	m_LAgreen = RV_CUT_GREEN;
	m_LAtotal = RV_CUT_TOTAL;
	RecalculateBugsNStuff();
	StoreLAItotal();
}
void FlowerStrip::DoDevelopment(void) {
	VegElement::DoDevelopment();
	const long today = g_date->DayInYear();
	if (today==1) m_DateCut =-1; // Reset the cutting record at the start of year
	if ((today >= cfg_flowerstripCutStart.value()) && (today <= cfg_flowerstripCutEnd.value()) && (m_DateCut == -1))
	{
		if (g_rand_uni_fnc() < cfg_flowerstripCutChance.value()) Cutting(today);
	}
}

FieldBoundary::FieldBoundary(Landscape* L, int a_type) : VegElement(tole_FieldBoundary, L) {
	SetVegType(tov_NaturalGrass);
	m_curve_num = g_crops->VegTypeToCurveNum(m_vege_type);
	m_DateCut = 1;
	// Three options for the type, each will have a different pollen nectar curve set (type 1 is default)
	SetSubType(a_type);
	// Initially assume that all at patchy
	VegElement::Set_Att_VegPatchy(true);
	VegElement::SetVegGrowthScalerRand();
}

void FieldBoundary::Cutting(int a_today)
{
	SetLastTreatment(mow);
	float biomassBefore = GetVegBiomass();
	SetGrowthPhase(harvest1);
	//m_DateCut = a_today;
	m_veg_height = RV_CUT_HEIGHT;
	m_LAgreen = RV_CUT_GREEN;
	m_LAtotal = RV_CUT_TOTAL;
	RecalculateBugsNStuff();
}

void FieldBoundary::DoDevelopment(void) {
	VegElement::DoDevelopment();
	m_digestability += 0.2;
	if (m_digestability > 0.8) m_digestability = 0.8;
	long today = g_date->DayInYear();
	if (today==1) m_DateCut = -cfg_field_boundary_cut_start.value().size(); // Reset the cutting record at the start of year

	//check cutting dates
	if(m_DateCut < 0){
		int temp_cut_index = m_DateCut + cfg_field_boundary_cut_start.value().size();
		//cout<<"Cut index:  "<<temp_cut_index<<endl;
		if ((today >= cfg_field_boundary_cut_start.value()[temp_cut_index]) && (today <= cfg_field_boundary_cut_end.value()[temp_cut_index]))
		{
			if (g_rand_uni_fnc() < cfg_field_boundary_cut_chance.value()[temp_cut_index]){
				Cutting(today);
				m_DateCut++;
			} //increment the cutting record
			//last cutting date, but still not cut, move to the next
			else if(today == cfg_field_boundary_cut_end.value()[temp_cut_index]){
				m_DateCut++;
			}
		}
	}
}

RoadsideVerge::RoadsideVerge(TTypesOfLandscapeElement tole, Landscape * L) : VegElement(tole, L) {
	SetVegType(tov_NaturalGrass);
	Set_Att_VegPatchy(true);
	m_DateCut = 0;
}

void RoadsideVerge::DoDevelopment( void ) {
  VegElement::DoDevelopment();
  // Add cutting functionality when ready.
  long today = g_date->DayInYear();

  if ( g_date->JanFirst() ) {
    // beginning of year so restart the cutting
    m_DateCut = 0;
  }

  if ( today > RV_MAY_1ST ) // No cutting before May 1st
  {
    long SinceCut = today - m_DateCut; // how many days since last cut
    int month = g_date->GetMonth();
    switch ( month ) {
      case 5:
        if ( g_random_fnc( 14 ) + SinceCut > RV_CUT_MAY ) Cutting( today );
      break;
      case 6:
        if ( g_random_fnc( 14 ) + SinceCut > RV_CUT_JUN ) Cutting( today );
      break;
      case 7:
        if ( g_random_fnc( 14 ) + SinceCut > RV_CUT_JUL ) Cutting( today );
      break;
      case 8:
        if ( g_random_fnc( 14 ) + SinceCut > RV_CUT_AUG ) Cutting( today );
      break;
      case 9:
        if ( g_random_fnc( 14 ) + SinceCut > RV_CUT_SEP ) Cutting( today );
      break;
      case 10:
        if ( g_random_fnc( 14 ) + SinceCut > RV_CUT_OCT ) Cutting( today );
      break;
      default:
      break;
    }
  }
}

void RoadsideVerge::Cutting( int a_today )
{
  SetLastTreatment( mow );
  float biomassBefore = GetVegBiomass();
  SetGrowthPhase(harvest1);
  m_DateCut = a_today;
  m_veg_height = RV_CUT_HEIGHT;
  m_LAgreen = RV_CUT_GREEN;
  m_LAtotal = RV_CUT_TOTAL;
  RecalculateBugsNStuff();
  StoreLAItotal();
}

WaterBufferZone::WaterBufferZone(Landscape *L) : VegElement(tole_WaterBufferZone, L) {
	this->SetVegType(tov_WaterBufferZone);
	this->Set_Att_VegPatchy(true);
	this->SetVegGrowthScalerRand();
}

void WaterBufferZone::DoDevelopment(void) {
	VegElement::DoDevelopment();
	// Add reseting veg functionality on January 1st
	long today = g_date->DayInYear();

	if (g_date->JanFirst()) {
		// beginning of year so restart the cutting
		ResetingVeg(today);
	}
}

void WaterBufferZone::ResetingVeg(int a_today)
{
	ZeroVeg();
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

MownGrassStrip::MownGrassStrip(Landscape *L) : VegElement(tole_MownGrassStrip, L) {
  SetVegType(tov_NaturalGrass);
  if (g_rand_uni_fnc() < cfg_MGPatchyChance.value()) 
	  this->Set_Att_VegPatchy(true); 
  m_DateCut = 0;
}

OrchardBand::OrchardBand( Landscape *L ) : VegElement(tole_OrchardBand, L) {
  this->SetVegType(tov_NaturalGrass);
  m_LastSprayed = 99999;
}

Orchard::Orchard(Landscape * L) : VegElement(tole_Orchard, L) {
	SetVegType(tov_NaturalGrass);
	m_DateCut = 0;
}

void Orchard::DoDevelopment( void ) {
  VegElement::DoDevelopment();
  long today = g_date->DayInYear();
  // Cutting functionality
  if ( g_date->JanFirst() ) {
    // beginning of year so restart the cutting
    m_DateCut = 0;
  }

  switch ( cfg_OrchardNoCutsDay.value() ) {
    case 4:
      if ( ( today == 259 ) || ( today == 122 ) || ( today == 92 ) || ( today == 196 ) )
        Cutting( today );
    break;
    case 3:
      if ( ( today == 259 ) || ( today == 122 ) || ( today == 92 ) ) Cutting( today );
    break;
    case 2:
      if ( ( today == 259 ) || ( today == 122 ) ) Cutting( today );
    break;
    case 1:
      if ( ( today == 259 ) ) Cutting( today );
    break;
    default: // No cut
    break;
  }
}

void Orchard::Cutting( int a_today ) {
  SetLastTreatment( mow );
  float biomassBefore = GetVegBiomass();
  SetMownDecay( 12 ); // 12 days of not suitable
  SetGrowthPhase( harvest1 );
  m_DateCut = a_today;
  m_veg_height = l_el_o_cut_height.value();
  m_LAgreen = l_el_o_cut_green.value();
  m_LAtotal = l_el_o_cut_total.value();
  RecalculateBugsNStuff();
  StoreLAItotal();
}

void MownGrassStrip::DoDevelopment( void ) {
  VegElement::DoDevelopment();
  long today = g_date->DayInYear();
  // Cutting functionality
  if ( g_date->JanFirst() ) {
    // beginning of year so restart the cutting
    m_DateCut = 0;
  }
  switch ( cfg_MownGrassNoCutsDay.value() ) {
    case 99:
		// Use to define special cutting behaviour e.g. cutting every 14 days after 1st May
		if (  ( today >= ( March + 15 ) ) && ( today % 42 == 0 )) {
			if (today < October) Cutting( today );
		}
    break;
	case 9:
		if ((today == 196)) // 15th July
			Cutting(today);
		break;
	case 8:
		if ((today == 259) || (today == 92)) Cutting(today);
		break;
	case 7:
		if ((today == 122)) // 1st May
			Cutting(today);
		break;
	case 6:
		if ((today == 92)) // 1st April
			Cutting(today);
		break;
    case 5:
      if ( ( today == 151 ) ) // 1st June
        Cutting( today );
    break;
    case 4:
      if ( ( today == 259 ) || ( today == 122 ) || ( today == 92 ) || ( today == 196 ) )
        Cutting( today );
    break;
    case 3:
      if ( ( today == 259 ) || ( today == 122 ) || ( today == 92 ) ) Cutting( today );
    break;
    case 2:
      if ( ( today == 259 ) || ( today == 122 ) ) Cutting( today );
    break;
    case 1:
      if ( ( today == 259 ) ) Cutting( today ); // 15 September
    break;
    default: // No cut
    break;
  }
}

void MownGrassStrip::Cutting( int a_today ) {
  SetLastTreatment( mow );
  float biomassBefore = GetVegBiomass();
  SetMownDecay( 21 ); // 21 days of not suitable
  SetGrowthPhase( harvest1 );
  m_DateCut = a_today;
  m_veg_height = l_el_o_cut_height.value();
  m_LAgreen = l_el_o_cut_green.value();
  m_LAtotal = l_el_o_cut_total.value();
  RecalculateBugsNStuff();
  StoreLAItotal();
}


void OrchardBand::DoDevelopment( void ) {
	VegElement::DoDevelopment();
	long today = g_date->DayInYear();
	if (m_LastSprayed<today) {
		m_herbicidedelay = today-m_LastSprayed;
		if (m_herbicidedelay > 5) m_herbicidedelay = 5;
			else if (m_herbicidedelay < 0) m_herbicidedelay = 0;
		this->ReduceVeg(0.9);
		if ((today == 0) || (today-m_LastSprayed > 90)) m_LastSprayed = 999999;
	}
}

NaturalGrass::NaturalGrass(TTypesOfLandscapeElement tole, Landscape *L) : VegElement(tole, L) {
	VegElement::SetVegType(tov_NaturalGrass);
	VegElement::Set_Att_VegPatchy(true);
	VegElement::SetVegGrowthScalerRand();
}

void NaturalGrass::DoDevelopment(void) {
	VegElement::DoDevelopment();
	// The assumption is that natural grass has a range of species, which means
	// there should be good food all year - but still it should vary with season
	// So we add a constant to the digestability of 0.2
	m_digestability += 0.2;
	if (m_digestability > 0.8) m_digestability = 0.8;
}

UnsprayedFieldMargin::UnsprayedFieldMargin(Landscape * L) : VegElement(tole_UnsprayedFieldMargin, L) {
  this->SetVegType(tov_NaturalGrass);
  if (g_random_fnc(100) < cfg_UMPatchyChance.value()) 
	  this->Set_Att_VegPatchy(true);
}

void UnsprayedFieldMargin::DoDevelopment( void ) {
/**
* All development is controlled by the controlling field polygon, so do nothing here
*/
	return;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


ForestElement::ForestElement(TTypesOfLandscapeElement tole, Landscape * L) : VegElement(tole, L) {
	;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

NonVegElement::NonVegElement(TTypesOfLandscapeElement tole, Landscape *L) : LE( L ) {
	m_type = tole;
	m_owner_tole = m_type;
}

Pond::Pond( Landscape * L ) : NonVegElement (tole_Pond, L) {
	m_LarvalFood = 0.01;
	m_MaleNewtPresent = false;
	m_LarvalFoodScaler = 0.0;
	m_pondpesticide = 0.0;
	if (cfg_randompondquality.value()) m_pondquality = g_rand_uni_fnc(); else m_pondquality = 1.0;
}

void Pond::DoDevelopment()
{
	LE::DoDevelopment();
	CalcLarvalFood();
	m_MaleNewtPresent = false;
}



void Pond::CalcLarvalFood()
{
/**
* The larval food is calculated assuming a logistic equation in the form of Nt+1 = Nt+(N*r * (1-N/K))
* t = one day, N is a scaler which is multiplied by a constant and the area of the pond to get the total larval food, K & r are carrying capacity and instantaneous reproductive rate respectively.
* K can change with season and this is currently hard coded, but could be an input variable later. The values are held in LarvalFoodMonthlyK\n
* The steps in the calculation are:\n
* - Enforce a assumed pond size for newts as maximum 400 m2
* - Ensure we never get zero larval food, so there is always something to grow the curve from.
* - Back calculate the current scaler value. This is needed because between time steps, food may be eaten by larvae. This is done based on the area and a fixed constant held in cfg_PondLarvalFoodBiomassConst
* - Calculate the new scaler based on the logistic equation as described above
* - Re-calculate the new total food biomass based on the area and a fixed constant held in cfg_PondLarvalFoodBiomassConst
*
*/
	double area = m_area;
	if (m_area > 400) area = 400;
	const double LarvalFoodMonthlyK[12] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
	// Back calculate the scaler
	m_LarvalFoodScaler = m_LarvalFood / (cfg_PondLarvalFoodBiomassConst.value() * area * m_pondquality);
	// Calculate the new scaler
	m_LarvalFoodScaler = m_LarvalFoodScaler + (m_LarvalFoodScaler*cfg_PondLarvalFoodR.value() * (1 - (m_LarvalFoodScaler / LarvalFoodMonthlyK[g_date->GetMonth() - 1])));
	// Provide some contest competition by preventing all the food from being used up, leaving minimum 1%
	if (m_LarvalFoodScaler < 0.01) m_LarvalFoodScaler = 0.01;
	// Calculate the new food biomass
	m_LarvalFood = m_LarvalFoodScaler * (cfg_PondLarvalFoodBiomassConst.value() * area * m_pondquality);
}

bool Pond::SubtractLarvalFood(double a_food)
{
	/**
	* If the total amount of food is low then there is a probability test to determine if food can be found. If failed the return code false means no food was removed.
	* If passed then this removes the amount of food passed in a_food and return true.
	*/
	m_LarvalFood -= a_food;
	if (m_LarvalFood < 0) return false;
	return true;
}


LargeRoad::LargeRoad( Landscape *L ) : NonVegElement(tole_LargeRoad, L) {
	;
}


double LargeRoad::GetTrafficLoad( void ) {
  return m_largeroad_load[ g_date->GetHour() ] * m_monthly_traffic[ g_date->GetMonth() - 1 ];
}


SmallRoad::SmallRoad( Landscape *L  ) : NonVegElement(tole_SmallRoad, L) {
	;
}


double SmallRoad::GetTrafficLoad( void ) {
  return m_smallroad_load[ g_date->GetHour() ] * m_monthly_traffic[ g_date->GetMonth() - 1 ];
}


LE_TypeClass* CreateLETypeClass()
{
	if (g_letype == NULL)
	{
		g_letype = new LE_TypeClass;
	}

	return g_letype;
}

//--------------------------------------------------------------------
// created 25/08/00
TTypesOfLandscapeElement LE_TypeClass::TranslateEleTypesFromString(string aEleReferenceStr) {
  // This returns the vegetation type (or crop type) as applicable
  if (aEleReferenceStr ==	"tole_Building")	return	tole_Building;
  if (aEleReferenceStr ==	"tole_UrbanNoVeg")	return	tole_UrbanNoVeg;
  if (aEleReferenceStr ==	"tole_UrbanVeg")	return	tole_UrbanVeg;
  if (aEleReferenceStr ==	"tole_Garden")	return	tole_Garden;
  if (aEleReferenceStr ==	"tole_AmenityGrass")	return	tole_AmenityGrass;
  if (aEleReferenceStr ==	"tole_RoadsideVerge")	return	tole_RoadsideVerge;
  if (aEleReferenceStr ==	"tole_Parkland")	return	tole_Parkland;
  if (aEleReferenceStr ==	"tole_StoneWall")	return	tole_StoneWall;
  if (aEleReferenceStr ==	"tole_BuiltUpWithParkland")	return	tole_BuiltUpWithParkland;
  if (aEleReferenceStr ==	"tole_UrbanPark")	return	tole_UrbanPark;
  if (aEleReferenceStr ==	"tole_Field")	return	tole_Field;
  if (aEleReferenceStr ==	"tole_PermPastureTussocky")	return	tole_PermPastureTussocky;
  if (aEleReferenceStr ==	"tole_PermPastureLowYield")	return	tole_PermPastureLowYield;
  if (aEleReferenceStr ==	"tole_UnsprayedFieldMargin")	return	tole_UnsprayedFieldMargin;
  if (aEleReferenceStr ==	"tole_PermanentSetaside")	return	tole_PermanentSetaside;
  if (aEleReferenceStr ==	"tole_PermPasture")	return	tole_PermPasture;
  if (aEleReferenceStr ==	"tole_DeciduousForest")	return	tole_DeciduousForest;
  if (aEleReferenceStr ==	"tole_Copse")	return	tole_Copse;
  if (aEleReferenceStr ==	"tole_ConiferousForest")	return	tole_ConiferousForest;
  if (aEleReferenceStr ==	"tole_YoungForest")	return	tole_YoungForest;
  if (aEleReferenceStr ==	"tole_Orchard")	return	tole_Orchard;
  if (aEleReferenceStr ==	"tole_BareRock")	return	tole_BareRock;
  if (aEleReferenceStr ==	"tole_OrchardBand")	return	tole_OrchardBand;
  if (aEleReferenceStr ==	"tole_MownGrassStrip")	return	tole_MownGrassStrip;
  if (aEleReferenceStr ==	"tole_MixedForest")	return	tole_MixedForest;
  if (aEleReferenceStr ==	"tole_Scrub")	return	tole_Scrub;
  if (aEleReferenceStr ==	"tole_PitDisused")	return	tole_PitDisused;
  if (aEleReferenceStr ==	"tole_Saltwater")	return	tole_Saltwater;
  if (aEleReferenceStr ==	"tole_Freshwater")	return	tole_Freshwater;
  if (aEleReferenceStr ==	"tole_Heath")	return	tole_Heath;
  if (aEleReferenceStr ==	"tole_Marsh")	return	tole_Marsh;
  if (aEleReferenceStr ==	"tole_River")	return	tole_River;
  if (aEleReferenceStr ==	"tole_RiversideTrees")	return	tole_RiversideTrees;
  if (aEleReferenceStr ==	"tole_RiversidePlants")	return	tole_RiversidePlants;
  if (aEleReferenceStr ==	"tole_Coast")	return	tole_Coast;
  if (aEleReferenceStr ==	"tole_SandDune")	return	tole_SandDune;
  if (aEleReferenceStr ==	"tole_NaturalGrassDry")	return	tole_NaturalGrassDry;
  if (aEleReferenceStr ==	"tole_ActivePit")	return	tole_ActivePit;
  if (aEleReferenceStr ==	"tole_Railway")	return	tole_Railway;
  if (aEleReferenceStr ==	"tole_LargeRoad")	return	tole_LargeRoad;
  if (aEleReferenceStr ==	"tole_SmallRoad")	return	tole_SmallRoad;
  if (aEleReferenceStr ==	"tole_Track")	return	tole_Track;
  if (aEleReferenceStr ==	"tole_Hedges")	return	tole_Hedges;
  if (aEleReferenceStr ==	"tole_HedgeBank")	return	tole_HedgeBank;
  if (aEleReferenceStr ==	"tole_BeetleBank")	return	tole_BeetleBank;
  if (aEleReferenceStr ==	"tole_Chameleon")	return	tole_Chameleon;
  if (aEleReferenceStr ==	"tole_FieldBoundary")	return	tole_FieldBoundary;
  if (aEleReferenceStr ==	"tole_RoadsideSlope")	return	tole_RoadsideSlope;
  if (aEleReferenceStr ==	"tole_MetalledPath")	return	tole_MetalledPath;
  if (aEleReferenceStr ==	"tole_Carpark")	return	tole_Carpark;
  if (aEleReferenceStr ==	"tole_Churchyard")	return	tole_Churchyard;
  if (aEleReferenceStr ==	"tole_NaturalGrassWet")	return	tole_NaturalGrassWet;
  if (aEleReferenceStr ==	"tole_Saltmarsh")	return	tole_Saltmarsh;
  if (aEleReferenceStr ==	"tole_Stream")	return	tole_Stream;
  if (aEleReferenceStr ==	"tole_HeritageSite")	return	tole_HeritageSite;
  if (aEleReferenceStr ==	"tole_Wasteland")	return	tole_Wasteland;
  if (aEleReferenceStr ==	"tole_UnknownGrass")	return	tole_UnknownGrass;
  if (aEleReferenceStr ==	"tole_WindTurbine")	return	tole_WindTurbine;
  if (aEleReferenceStr ==	"tole_Pylon")	return	tole_Pylon;
  if (aEleReferenceStr ==	"tole_IndividualTree")	return	tole_IndividualTree;
  if (aEleReferenceStr ==	"tole_PlantNursery")	return	tole_PlantNursery;
  if (aEleReferenceStr ==	"tole_Vildtager")	return	tole_Vildtager;
  if (aEleReferenceStr ==	"tole_WoodyEnergyCrop")	return	tole_WoodyEnergyCrop;
  if (aEleReferenceStr ==	"tole_WoodlandMargin")	return	tole_WoodlandMargin;
  if (aEleReferenceStr ==	"tole_PermPastureTussockyWet")	return	tole_PermPastureTussockyWet;
  if (aEleReferenceStr ==	"tole_Pond")	return	tole_Pond;
  if (aEleReferenceStr ==	"tole_FishFarm")	return	tole_FishFarm;
  if (aEleReferenceStr ==	"tole_RiverBed")	return	tole_RiverBed;
  if (aEleReferenceStr ==	"tole_DrainageDitch")	return	tole_DrainageDitch;
  if (aEleReferenceStr ==	"tole_Canal")	return	tole_Canal;
  if (aEleReferenceStr ==	"tole_RefuseSite")	return	tole_RefuseSite;
  if (aEleReferenceStr ==	"tole_Fence")	return	tole_Fence;
  if (aEleReferenceStr ==	"tole_WaterBufferZone")	return	tole_WaterBufferZone;
  if (aEleReferenceStr ==	"tole_Missing")	return	tole_Missing;
  // adding new elements ( year 2021 )
  if (aEleReferenceStr ==  "tole_Airport")	return  tole_Airport;
  if (aEleReferenceStr ==  "tole_Portarea")	return  tole_Portarea;
  if (aEleReferenceStr ==  "tole_Saltpans")	return  tole_Saltpans;
  if (aEleReferenceStr ==  "tole_Pipeline")	return  tole_Pipeline;
  if (aEleReferenceStr ==  "tole_SolarPanel")	return  tole_SolarPanel;
  // adding new forests 
  if (aEleReferenceStr ==  "tole_SwampForest")	return  tole_SwampForest;
  if (aEleReferenceStr ==  "tole_MontadoCorkOak")	return  tole_MontadoCorkOak;
  if (aEleReferenceStr ==  "tole_MontadoHolmOak")	return  tole_MontadoHolmOak;
  if (aEleReferenceStr ==  "tole_MontadoMixed")	return  tole_MontadoMixed;
  if (aEleReferenceStr ==  "tole_AgroForestrySystem")	return  tole_AgroForestrySystem;
  if (aEleReferenceStr ==  "tole_CorkOakForest")	return  tole_CorkOakForest;
  if (aEleReferenceStr ==  "tole_HolmOakForest")	return  tole_HolmOakForest;
  if (aEleReferenceStr ==  "tole_OtherOakForest")	return  tole_OtherOakForest;
  if (aEleReferenceStr ==  "tole_ChestnutForest")	return  tole_ChestnutForest;
  if (aEleReferenceStr ==  "tole_EucalyptusForest")	return  tole_EucalyptusForest;
  if (aEleReferenceStr ==  "tole_InvasiveForest")	return  tole_InvasiveForest;
  if (aEleReferenceStr ==  "tole_MaritimePineForest")	return  tole_MaritimePineForest;
  if (aEleReferenceStr ==  "tole_StonePineForest")	return  tole_StonePineForest;
  if (aEleReferenceStr ==  "tole_ForestAisle")	return  tole_ForestAisle;
  // adding new permanent crop fields
  if (aEleReferenceStr ==  "tole_Vineyard")	return  tole_Vineyard;
  if (aEleReferenceStr ==  "tole_OliveGrove")	return  tole_OliveGrove;
  if (aEleReferenceStr ==  "tole_RiceField")    return  tole_RiceField;
  if (aEleReferenceStr ==  "tole_OOrchard")    return  tole_OOrchard;
  if (aEleReferenceStr ==  "tole_BushFruit")    return  tole_BushFruit;
  if (aEleReferenceStr ==  "tole_OBushFruit")    return  tole_OBushFruit;
  if (aEleReferenceStr ==  "tole_ChristmasTrees")    return  tole_ChristmasTrees;
  if (aEleReferenceStr ==  "tole_OChristmasTrees")    return  tole_OChristmasTrees;
  if (aEleReferenceStr ==  "tole_EnergyCrop")    return  tole_EnergyCrop;
  if (aEleReferenceStr ==  "tole_OEnergyCrop")    return  tole_OEnergyCrop;
  if (aEleReferenceStr ==  "tole_FarmForest")    return  tole_FarmForest;
  if (aEleReferenceStr ==  "tole_OFarmForest")    return  tole_OFarmForest;
  if (aEleReferenceStr ==  "tole_PermPasturePigs")    return  tole_PermPasturePigs;
  if (aEleReferenceStr ==  "tole_OPermPasturePigs")    return  tole_OPermPasturePigs;
  if (aEleReferenceStr ==  "tole_OPermPasture")    return  tole_OPermPasture;
  if (aEleReferenceStr ==  "tole_OPermPastureLowYield")    return  tole_OPermPastureLowYield;
  if (aEleReferenceStr ==  "tole_FarmYoungForest")    return  tole_FarmYoungForest;
  if (aEleReferenceStr ==  "tole_OFarmYoungForest")    return  tole_OFarmYoungForest;
  if (aEleReferenceStr ==	"tole_AlmondPlantation")	return	tole_AlmondPlantation;
  if (aEleReferenceStr ==	"tole_WalnutPlantation")	return	tole_WalnutPlantation;
  if (aEleReferenceStr ==  "tole_FarmBufferZone")	return  tole_FarmBufferZone;
  if (aEleReferenceStr ==  "tole_NaturalFarmGrass")	return  tole_NaturalFarmGrass;
  if (aEleReferenceStr ==  "tole_GreenFallow")	return  tole_GreenFallow;
  if (aEleReferenceStr ==  "tole_FarmFeedingGround")	return  tole_FarmFeedingGround;
  if (aEleReferenceStr ==  "tole_FlowersPerm")	return  tole_FlowersPerm; // This is a type of crop
  if (aEleReferenceStr ==  "tole_AsparagusPerm")	return  tole_AsparagusPerm;
  if (aEleReferenceStr ==  "tole_MushroomPerm")	return  tole_MushroomPerm;
  if (aEleReferenceStr ==  "tole_OtherPermCrop")	return  tole_OtherPermCrop;
  if (aEleReferenceStr ==  "tole_OAsparagusPerm")	return  tole_OAsparagusPerm;
  if (aEleReferenceStr ==  "tole_FlowerStrip")	return  tole_FlowerStrip;

  g_msg->Warn( WARN_FILE, "LE_TypeClass::TranslateEleTypesFromString(): ""Unknown landscape element type string", 0);
  exit( 1 );
}

//--------------------------------------------------------------------
// created 25/08/00
TTypesOfLandscapeElement LE_TypeClass::TranslateEleTypes( int EleReference ) {
  // This returns the vegetation type (or crop type) as applicable
  switch ( EleReference ) {
  case	5:	return	tole_Building;
  case	8:	return	tole_UrbanNoVeg;
  case	9:	return	tole_UrbanVeg;
  case	11:	return	tole_Garden;
  case	12:	return	tole_AmenityGrass;
  case	13:	return	tole_RoadsideVerge;
  case	14:	return	tole_Parkland;
  case	15:	return	tole_StoneWall;
  case	16:	return	tole_BuiltUpWithParkland;
  case	17:	return	tole_UrbanPark;
  case	20:	return	tole_Field;
  case	27:	return	tole_PermPastureTussocky;
  case	26:	return	tole_PermPastureLowYield;
  case	31:	return	tole_UnsprayedFieldMargin;
  case	33:	return	tole_PermanentSetaside;
  case	35:	return	tole_PermPasture;
  case	40:	return	tole_DeciduousForest;
  case	41:	return	tole_Copse;
  case	50:	return	tole_ConiferousForest;
  case	55:	return	tole_YoungForest;
  case	56:	return	tole_Orchard;
  case	69:	return	tole_BareRock;
  case	57:	return	tole_OrchardBand;
  case	58:	return	tole_MownGrassStrip;
  case	60:	return	tole_MixedForest;
  case	70:	return	tole_Scrub;
  case	75:	return	tole_PitDisused;
  case	80:	return	tole_Saltwater;
  case	90:	return	tole_Freshwater;
  case	94:	return	tole_Heath;
  case	95:	return	tole_Marsh;
  case	96:	return	tole_River;
  case	97:	return	tole_RiversideTrees;
  case	98:	return	tole_RiversidePlants;
  case	100:	return	tole_Coast;
  case	101:	return	tole_SandDune;
  case	110:	return	tole_NaturalGrassDry;
  case	115:	return	tole_ActivePit;
  case	118:	return	tole_Railway;
  case	121:	return	tole_LargeRoad;
  case	122:	return	tole_SmallRoad;
  case	123:	return	tole_Track;
  case	130:	return	tole_Hedges;
  case	140:	return	tole_HedgeBank;
  case	141:	return	tole_BeetleBank;
  case	150:	return	tole_Chameleon;
  case	160:	return	tole_FieldBoundary;
  case	201:	return	tole_RoadsideSlope;
  case	202:	return	tole_MetalledPath;
  case	203:	return	tole_Carpark;
  case	204:	return	tole_Churchyard;
  case	205:	return	tole_NaturalGrassWet;
  case	206:	return	tole_Saltmarsh;
  case	207:	return	tole_Stream;
  case	208:	return	tole_HeritageSite;
  case	209:	return	tole_Wasteland;
  case	210:	return	tole_UnknownGrass;
  case	211:	return	tole_WindTurbine;
  case	212:	return	tole_Pylon;
  case	213:	return	tole_IndividualTree;
  case	214:	return	tole_PlantNursery;
  case	215:	return	tole_Vildtager;
  case	216:	return	tole_WoodyEnergyCrop;
  case	217:	return	tole_WoodlandMargin;
  case	218:	return	tole_PermPastureTussockyWet;
  case	219:	return	tole_Pond;
  case	220:	return	tole_FishFarm;
  case	221:	return	tole_RiverBed;
  case	222:	return	tole_DrainageDitch;
  case	223:	return	tole_Canal;
  case	224:	return	tole_RefuseSite;
  case	225:	return	tole_Fence;
  case	226:	return	tole_WaterBufferZone;
  case	2112:	return	tole_Missing;
  // adding new elements ( year 2021 )
  case  300:	return  tole_Airport;
  case  301:	return  tole_Portarea;
  case  302:	return  tole_Saltpans;
  case  303:	return  tole_Pipeline;
  case  304:	return  tole_SolarPanel;
  // adding new forests 
  case  400:	return  tole_SwampForest;
  case  401:	return  tole_MontadoCorkOak;
  case  402:	return  tole_MontadoHolmOak;
  case  403:	return  tole_MontadoMixed;
  case  404:	return  tole_AgroForestrySystem;
  case  405:	return  tole_CorkOakForest;
  case  406:	return  tole_HolmOakForest;
  case  407:	return  tole_OtherOakForest;
  case  408:	return  tole_ChestnutForest;
  case  409:	return  tole_EucalyptusForest;
  case  410:	return  tole_InvasiveForest;
  case  411:	return  tole_MaritimePineForest;
  case  412:	return  tole_StonePineForest;
  case  413:	return  tole_ForestAisle;
  // adding new permanent crop fields
  case  500:	return  tole_Vineyard;
  case  501:	return  tole_OliveGrove;
  case  502:    return  tole_RiceField;
  case  503:    return  tole_OOrchard;
  case  504:    return  tole_BushFruit;
  case  505:    return  tole_OBushFruit;
  case  506:    return  tole_ChristmasTrees;
  case  507:    return  tole_OChristmasTrees;
  case  508:    return  tole_EnergyCrop;
  case  509:    return  tole_OEnergyCrop;
  case  510:    return  tole_FarmForest;
  case  511:    return  tole_OFarmForest;
  case  512:    return  tole_PermPasturePigs;
  case  513:    return  tole_OPermPasturePigs;
  case  514:    return  tole_OPermPasture;
  case  515:    return  tole_OPermPastureLowYield;
  case  516:    return  tole_FarmYoungForest;
  case  517:    return  tole_OFarmYoungForest;
  case	518:	return	tole_AlmondPlantation;
  case	519:	return	tole_WalnutPlantation;
  case  520:	return  tole_FarmBufferZone;
  case  521:	return  tole_NaturalFarmGrass;
  case  522:	return  tole_GreenFallow;
  case  523:	return  tole_FarmFeedingGround;
  case  524:	return  tole_FlowersPerm; // This is a type of crop
  case  525:	return  tole_AsparagusPerm;
  case  526:	return  tole_MushroomPerm;
  case  527:	return  tole_OtherPermCrop;
  case  528:	return  tole_OAsparagusPerm;
  case  529:	return  tole_FlowerStrip;
	  //  case 999: return tole_Foobar;
      // !! type unknown - should not happen
    default:
      g_msg->Warn( WARN_FILE, "LE_TypeClass::TranslateEleTypes(): ""Unknown landscape element type:", int(EleReference));
      exit( 1 );
  }
}



//----------------------------------------------------------------------
// created 24/08/00
TTypesOfVegetation LE_TypeClass::TranslateVegTypes( int VegReference ) {
  // This returns the vegetation type (or crop type) as applicable
  switch ( VegReference ) {
    case 1:
      return tov_SpringBarley;
    case 2:
      return tov_WinterBarley;
    case 3:
      return tov_SpringWheat;
    case 4:
      return tov_WinterWheat;
    case 5:
      return tov_WinterRye;
    case 6:
      return tov_Oats;
    case 7:
      return tov_Triticale;
    case 8:
      return tov_Maize;
    case 13:
      return tov_SpringBarleySeed;
    case 14: return tov_SpringBarleyStrigling;
    case 15: return tov_SpringBarleyStriglingSingle;
    case 16: return tov_SpringBarleyStriglingCulm;
    case 17: return tov_WinterWheatStrigling;
    case 18: return tov_WinterWheatStriglingSingle;
    case 19: return tov_WinterWheatStriglingCulm;
    case 21:
      return tov_SpringRape;
    case 22:
      return tov_WinterRape;
    case 30:
      return tov_FieldPeas;
	case 31:
		return tov_FieldPeasSilage; //ok?
	case 32:
		return tov_BroadBeans;
	case 50:
      return tov_SetAside;
    case 54:
      return tov_PermanentSetAside;
    case 55:
      return tov_YoungForest;
	case 60:
		return tov_FodderBeet;
	case 61:
		return tov_SugarBeet;
	case 65:
      return tov_CloverGrassGrazed1;
    case 92:
      return tov_PotatoesIndustry;
    case 93:
      return tov_Potatoes;
    case 94:
      return tov_SeedGrass1;
    case 102:
      return tov_OWinterBarley;
    case 611:
      return tov_OWinterBarleyExt;
    case 103:
      return tov_OSBarleySilage;
    case 105:
      return tov_OWinterRye;
    case 106:
      return tov_OFieldPeasSilage;
    case 107:
      return tov_SpringBarleyGrass;
	case 108:
		return tov_SpringBarleyCloverGrass;
	case 109:
		return tov_SpringBarleySpr;
	case 113:
      return tov_OBarleyPeaCloverGrass;
    case 114:
      return tov_SpringBarleyPeaCloverGrassStrigling;
    case 115:
      return tov_SpringBarleySilage;
    case 122:
      return tov_OWinterRape;
    case 140:
      return tov_PermanentGrassGrazed;
    case 141:
      return tov_PermanentGrassLowYield;
    case 142:
      return tov_PermanentGrassTussocky;
    case 165:
      return tov_CloverGrassGrazed2;
    case 194:
      return tov_SeedGrass2;
    case 201:
      return tov_OSpringBarley;
	case 204:
		return tov_OWinterWheatUndersown;
	case 205:
		return tov_OWinterWheat;
	case 206:
      return tov_OOats;
    case 207:
      return tov_OTriticale;
	case 230:
		return tov_OFieldPeas;
	case 260:
		return tov_OFodderBeet;
	case 265:
      return tov_OCloverGrassGrazed1;
    case 270:
      return tov_OCarrots;
    case 271:
      return tov_Carrots;
	case 272:
		return tov_Wasteland;
    case 273:
      return tov_OGrazingPigs;
    case 293:
      return tov_OPotatoes;
    case 294:
      return tov_OSeedGrass1;
	case 306:
		return tov_OSpringBarleyPigs;
	case 307:
      return tov_OSpringBarleyGrass;
    case 308:
      return tov_OSpringBarleyClover;
    case 340:
      return tov_OPermanentGrassGrazed;
    case 365:
      return tov_OCloverGrassGrazed2;
	case 366:
		return tov_OCloverGrassSilage1;
    case 394:
      return tov_OSeedGrass2;
    case 400:
      return tov_NaturalGrass;
    case 401:
      return tov_None;
	case 402:
		return tov_NoGrowth;
	case 601:
      return tov_WWheatPControl;
    case 602:
      return tov_WWheatPToxicControl;
    case 603:
      return tov_WWheatPTreatment;
    case 604:
      return tov_AgroChemIndustryCereal;
    case 605:
      return tov_WinterWheatShort;
	case 606:
	  return tov_MaizeSilage;
	case 607:
		return tov_FodderGrass;
	case 608:
		return tov_SpringBarleyPTreatment;
    case 609:
      return tov_OSpringBarleyExt;
	case 610:
	  return tov_OMaizeSilage;
	case 612:
		return tov_SpringBarleySKManagement;
	case 613:
		return tov_Heath;
	case 700:
	  return tov_OrchardCrop;
	case 701:
		return tov_WaterBufferZone;
	case 702:
		return tov_FlowerStrip1;
	case 703:
		return tov_FlowerStrip2;
	case 704:
		return tov_FlowerStrip3;

	case 801:
		return tov_PLWinterWheat;
	case 802:
		return tov_PLWinterRape;
	case 803:
		return tov_PLWinterBarley;
	case 804:
		return tov_PLWinterTriticale;
	case 805:
		return tov_PLWinterRye;
	case 806:
		return tov_PLSpringWheat;
	case 807:
		return tov_PLSpringBarley;
	case 808:
		return tov_PLMaize;
	case 809:
		return tov_PLMaizeSilage;
	case 810:
		return tov_PLPotatoes;
	case 811:
		return tov_PLBeet;
	case 812:
		return tov_PLFodderLucerne1;
	case 813:
		return tov_PLFodderLucerne2;
	case 814:
		return tov_PLCarrots;
	case 815:
		return tov_PLSpringBarleySpr;
	case 816:
		return tov_PLWinterWheatLate;
	case 817:
		return tov_PLBeetSpr;
	case 818:
		return tov_PLBeans;

	case 850:
		return tov_NLBeet;
	case 851:
		return tov_NLCarrots;
	case 852:
		return tov_NLMaize;
	case 853:
		return tov_NLPotatoes;
	case 854:
		return tov_NLSpringBarley;
	case 855:
		return tov_NLWinterWheat;
	case 856:
		return tov_NLCabbage;
	case 857:
		return tov_NLTulips;
	case 858:
		return tov_NLGrassGrazed1;
	case 859:
		return tov_NLGrassGrazed2;
	case 860:
		return tov_NLPermanentGrassGrazed;
	case 861:
		return tov_NLCatchCropPea;
	case 862:
		return tov_NLBeetSpring;
	case 863:
		return tov_NLCarrotsSpring;
	case 864:
		return tov_NLMaizeSpring;
	case 865:
		return tov_NLPotatoesSpring;
	case 866:
		return tov_NLSpringBarleySpring;
	case 867:
		return tov_NLCabbageSpring;
	case 868:
		return tov_NLGrassGrazed1Spring;
	case 869:
		return tov_NLGrassGrazedLast;
	case 870:
		return tov_NLOrchardCrop;
	case 871:
		return tov_NLPermanentGrassGrazedExtensive;
	case 872:
		return tov_NLGrassGrazedExtensive1;
	case 873:
		return tov_NLGrassGrazedExtensive2;
	case 874:
		return tov_NLGrassGrazedExtensive1Spring;
	case 875:
		return tov_NLGrassGrazedExtensiveLast;

	case 900:
		return tov_PTPermanentGrassGrazed;
	case 901:
		return tov_PTWinterWheat;
	case 902:
		return tov_PTGrassGrazed;
	case 903:
		return tov_PTSorghum;
	case 904:
		return tov_PTFodderMix;
	case 905:
		return tov_PTTurnipGrazed;
	case 906:
		return tov_PTCloverGrassGrazed1;
	case 907:
		return tov_PTCloverGrassGrazed2;
	case 908:
		return tov_PTTriticale;
	case 909:
		return tov_PTOtherDryBeans;
	case 910:
		return tov_PTShrubPastures;
	case 911:
		return tov_PTCorkOak;
	case 912:
		return tov_PTVineyards;
	case 913:
		return tov_PTWinterBarley;
	case 914:
		return tov_PTBeans;
	case 915:
		return tov_PTWinterRye;
	case 916:
		return tov_PTOliveGroveTraditional;
	case 917:
		return tov_PTOliveGroveTradOrganic;
	case 918:
		return tov_PTOliveGroveIntensive;
	case 919:
		return tov_PTOliveGroveSuperIntensive;
	case 921:
		return tov_PTRyegrass;
	case 922:
		return tov_PTYellowLupin;
	case 923:
		return tov_PTMaize;
	case 924:
		return tov_PTOats;
	case 925:
		return tov_PTPotatoes;
	case 926:
		return tov_PTHorticulture;
	case 927:
		return tov_PTMaize_Hort;
	case 928:
		return tov_PTCabbage;
	case 929:
		return tov_PTCabbage_Hort;

	case 950:
		return tov_DEOats;
	case 951:
		return tov_DESpringRye;
	case 952:
		return tov_DEWinterWheat;
	case 953:
		return tov_DEMaizeSilage;
	case 954:
		return tov_DEPotatoes;
	case 955:
		return tov_DEMaize;
	case 956:
		return tov_DEWinterRye;
	case 957:
		return tov_DEWinterBarley;
	case 958:
		return tov_DESugarBeet;
	case 959:
		return tov_DEWinterRape;
	case 960:
		return tov_DETriticale;
	case	961:
		return tov_DECabbage;
	case	962:
		return tov_DECarrots;
	case	963:
		return tov_DEGrasslandSilageAnnual;
	case	964:
		return tov_DEGreenFallow_1year;
	case	965:
		return tov_DELegumes;
	case	966:
		return tov_DEOCabbages;
	case	967:
		return tov_DEOCarrots;
	case	968:
		return tov_DEOGrasslandSilageAnnual;
	case	969:
		return tov_DEOGreenFallow_1year;
	case	970:
		return tov_DEOLegume;
	case	971:
		return tov_DEOMaize;
	case	972:
		return tov_DEOMaizeSilage;
	case	973:
		return tov_DEOOats;
	case	974:
		return tov_DEOPermanentGrassGrazed;
	case	975:
		return tov_DEOPotatoes;
	case	976:
		return tov_DEOSpringRye;
	case	977:
		return tov_DEOSugarBeet;
	case	978:
		return tov_DEOTriticale;
	case	979:
		return tov_DEOWinterBarley;
	case	980:
		return tov_DEOWinterRape;
	case	981:
		return tov_DEOWinterRye;
	case	982:
		return tov_DEOWinterWheat;
	case	983:
		return tov_DEWinterWheatLate;
	case	984:
		return tov_DEPermanentGrassGrazed;
	case	985:
		return tov_DEPermanentGrassLowYield;
	case	986:
		return tov_DEPotatoesIndustry;
	case	987:
		return tov_DEPeas;
	case	988:
		return tov_DEOPeas;
	case	989:
		return tov_DEAsparagusEstablishedPlantation;
	case	990:
		return tov_DEHerbsPerennial_1year;
	case	991:
		return tov_DEHerbsPerennial_after1year;
	case	992:
		return tov_DESpringBarley;
	case	993:
		return tov_DEOrchard;
	case	994:
		return tov_DEBushFruitPerm;
	case	995:
		return tov_DEOAsparagusEstablishedPlantation;
	case	996:
		return tov_DEOOrchard;
	case	997:
		return tov_DEOBushFruitPerm;
	case	998:
		return tov_DEOPermanentGrassLowYield;
	case	999:
		return tov_DEOHerbsPerennial_1year;
	case	1000:
		return tov_DEOHerbsPerennial_after1year;

		
	case 1001: 
		return tov_DKOOrchardCrop_Perm;
	case 1002:
		return tov_DKOBushFruit_Perm1;
	case 1003: 
		return tov_DKOBushFruit_Perm2;
	case 1004: 
		return tov_DKChristmasTrees_Perm;
	case 1005: 
		return tov_DKOChristmasTrees_Perm;
	case 1006: 
		return tov_DKEnergyCrop_Perm;
	case 1007: 
		return tov_DKOEnergyCrop_Perm;
	case 1008: 
		return tov_DKFarmForest_Perm;
	case 1009: 
		return tov_DKOFarmForest_Perm;
	case 1010: 
		return tov_DKGrazingPigs_Perm;
	case 1011: 
		return tov_DKOGrazingPigs_Perm;
	case 1012: 
		return tov_DKOGrassGrazed_Perm;
	case 1013: 
		return tov_DKOGrassLowYield_Perm;
	case 1014: 
		return tov_DKFarmYoungForest_Perm;
	case 1015: 
		return tov_DKOFarmYoungForest_Perm;
	
		//1084-1090 not used as Christmas tree codes are merged

	case 1200:
		return tov_UKBeans;
	case 1201:
		return tov_UKBeet;
	case 1202:
		return tov_UKMaize;
	case 1203:
		return tov_UKPermanentGrass;
	case 1204:
		return tov_UKPotatoes;
	case 1205:
		return tov_UKSpringBarley;
	case 1206:
		return tov_UKTempGrass;
	case 1207:
		return tov_UKWinterBarley;
	case 1208:
		return tov_UKWinterRape;
	case 1209:
		return tov_UKWinterWheat;

	case 1250:
		return tov_BEBeet;
	case 1251:
		return tov_BEBeetSpring;
	case 1252:
		return tov_BECatchPeaCrop;
	case 1253:
		return tov_BEGrassGrazed1;
	case 1254:
		return tov_BEGrassGrazed1Spring;
	case 1255:
		return tov_BEGrassGrazed2;
	case 1256:
		return tov_BEGrassGrazedLast;
	case 1257:
		return tov_BEMaize;
	case 1258:
		return tov_BEMaizeSpring;
	case 1259:
		return tov_BEOrchardCrop;
	case 1260:
		return tov_BEPotatoes;
	case 1261:
		return tov_BEPotatoesSpring;
	case 1262:
		return tov_BEWinterBarley;
	case 1263:
		return tov_BEWinterWheat;
	case 1264:
		return tov_BEWinterWheatCC;
	case 1265:
		return tov_BEWinterBarleyCC;
	case 1266:
		return tov_BEMaizeCC;

	case 1016: 
		return tov_DKOLegume_Peas;
	case 1017: 
		return tov_DKOLegume_Whole;
	case 1018: 
		return tov_DKSugarBeets;
	case 1019: 
		return tov_DKOSugarBeets;
	case 1020: 
		return tov_DKCabbages;
	case 1021: 
		return tov_DKOCabbages;
	case 1022: 
		return tov_DKCarrots;
	case 1023: 
		return tov_DKOCarrots;
	case 1024: 
		return tov_DKLegume_Whole;
	case 1025: 
		return tov_DKLegume_Peas;
	case 1026: 
		return tov_DKWinterWheat;
	case 1027:
		return tov_DKOWinterWheat;
	case 1028:
		return tov_DKSpringBarley;
	case 1029:
		return tov_DKOSpringBarley;
	case 1030:
		return tov_DKCerealLegume;
	case 1031:
		return tov_DKOCerealLegume;
	case 1032:
		return tov_DKCerealLegume_Whole;
	case 1033:
		return tov_DKOCerealLegume_Whole;
	case 1034:
		return tov_DKBushFruit_Perm1;
	case 1035:
		return tov_DKBushFruit_Perm2;
	case 1036:
		return tov_DKSpringFodderGrass;
	case 1037:
		return tov_DKOWinterFodderGrass;
	case 1038:
		return tov_DKGrassGrazed_Perm;
	case 1039:
		return tov_DKGrassLowYield_Perm;
	case 1040:
		return tov_DKGrazingPigs;
	case 1041:
		return tov_DKMaize;
	case 1042:
		return tov_DKMaizeSilage;
	case 1043:
		return tov_DKMixedVeg;
	case 1044:
		return tov_DKOGrazingPigs;
	case 1045:
		return tov_DKOMaize;
	case 1046:
		return tov_DKOMaizeSilage;
	case 1047:
		return tov_DKOMixedVeg;
	case 1048:
		return tov_DKOPotato;
	case 1049:
		return tov_DKOPotatoIndustry;
	case 1050:
		return tov_DKOPotatoSeed;
	case 1051:
		return tov_DKOrchardCrop_Perm;
	case 1052:
		return tov_DKOSeedGrassRye_Spring;
	case 1053:
		return tov_DKOSetAside;
	case 1054:
		return tov_DKOSpringBarleySilage;
	case 1055:
		return tov_DKOSpringOats;
	case 1056:
		return tov_DKOSpringWheat;
	case 1057:
		return tov_DKOVegSeeds;
	case 1058:
		return tov_DKOWinterBarley;
	case 1059:
		return tov_DKOWinterRape;
	case 1060:
		return tov_DKOWinterRye;
	case 1061:
		return tov_DKPlantNursery_Perm;
	case 1062:
		return tov_DKPotato;
	case 1063:
		return tov_DKPotatoIndustry;
	case 1064:
		return tov_DKPotatoSeed;
	case 1065:
		return tov_DKSeedGrassFescue_Spring;
	case 1066:
		return tov_DKSeedGrassRye_Spring;
	case 1067:
		return tov_DKSetAside;
	case 1068:
		return tov_DKSetAside_SummerMow;
	case 1069:
		return tov_DKSpringBarley_Green;
	case 1070:
		return tov_DKSpringBarleySilage;
	case 1071:
		return tov_DKSpringOats;
	case 1072:
		return tov_DKSpringWheat;
	case 1073:
		return tov_DKUndefined;
	case 1074:
		return tov_DKVegSeeds;
	case 1075:
		return tov_DKWinterBarley;
	case 1076:
		return tov_DKWinterRape;
	case 1077:
		return tov_DKWinterRye;
	case 1078:
		return tov_DKOWinterCloverGrassGrazedSown;
	case 1079:
		return tov_DKOCloverGrassGrazed1;
	case 1080:
		return tov_DKOCloverGrassGrazed2;
	case 1081:
		return tov_DKCloverGrassGrazed1;
	case 1082:
		return tov_DKWinterCloverGrassGrazedSown;
	case 1083:
		return tov_DKCloverGrassGrazed2;
	case 1084:
		return tov_DKCloverGrassGrazed3;
	case 1085:
		return tov_DKOCloverGrassGrazed3;
	case 1086:
		return tov_DKGrassTussocky_Perm;
	case 1091:
		return tov_DKOLegumeCloverGrass_Whole;
	case 1092:
		return tov_DKLegume_Beans;
	case 1093:
		return tov_DKWinterFodderGrass;
	case 1094:
		return tov_DKOrchApple;
	case 1095:
		return tov_DKOrchPear;
	case 1096:
		return tov_DKOrchCherry;
	case 1097:
		return tov_DKOrchOther;
	case 1098:
		return tov_DKOOrchApple;
	case 1099:
		return tov_DKOOrchPear;
	case 1100:
		return tov_DKOOrchCherry;
	case 1101:
		return tov_DKOOrchOther;
	case 1102:
		return tov_DKFodderBeets;
	case 1103:
		return tov_DKOLegume_Beans;
	case 1104:
		return tov_DKOFodderBeets;
	case 1105:
		return tov_DKOSpringFodderGrass;
	case 1106:
		return tov_DKCatchCrop;
	case 1107:
		return tov_DKOCatchCrop;
	case 1108:
		return tov_DKSpringBarleyCloverGrass;
	case 1109:
		return tov_DKOSpringBarleyCloverGrass;
	case 1110:
		return tov_DKWinterWheat_CC;
	case 1111:
		return tov_DKWinterRye_CC;
	case 1112:
		return tov_DKSpringOats_CC;
	case 1113:
		return tov_DKSpringBarley_CC;
	case 1114:
		return tov_DKOWinterWheat_CC;
	case 1115:
		return tov_DKOWinterRye_CC;
	case 1116:
		return tov_DKOSpringOats_CC;
	case 1117:
		return tov_DKOSpringBarley_CC;
	case 1118:
		return tov_DKOLegume_Beans_CC;
	case 1119:
		return tov_DKOLegume_Peas_CC;
	case 1120:
		return tov_DKOLegume_Whole_CC;
	case 1121:
		return tov_DKOLupines;
	case 1122:
		return tov_DKOLentils;
	case 1123:
		return tov_DKOSetAside_AnnualFlower;
	case 1124:
		return tov_DKOSetAside_PerennialFlower;
	case 1125:
		return tov_DKOSetAside_SummerMow;
	case 1126:
		return tov_DKOptimalFlowerMix1;
	case 1127:
		return tov_DKOptimalFlowerMix2;
	case 1128:
		return tov_DKOptimalFlowerMix3;

	case 1300:
		return tov_SESpringBarley;
	case 1301:
		return tov_SEWinterRape_Seed;
	case 1302:
		return tov_SEWinterWheat;
	case 1400:
		return tov_FIWinterWheat;
	case 1401:
		return tov_FIOWinterWheat;
	case 1402:
		return tov_FISugarBeet;
	case 1403:
		return tov_FIStarchPotato_North;
	case 1404:
		return tov_FIStarchPotato_South;
	case 1405:
		return tov_FIOStarchPotato_North;
	case 1406:
		return tov_FIOStarchPotato_South;
	case 1407:
		return tov_FISpringWheat;
	case 1408:
		return tov_FIOSpringWheat;
	case 1409:
	    return tov_FITurnipRape;
	case 1410:
		return tov_FIOTurnipRape;
	case 1411:
		return tov_FISpringRape;
	case 1412:
		return tov_FIOSpringRape;
	case 1413:
		return tov_FIWinterRye;
	case 1414:
		return tov_FIOWinterRye;
	case 1415:
		return tov_FIPotato_North;
	case 1416:
		return tov_FIPotato_South;
	case 1417:
		return tov_FIOPotato_North;
	case 1418:
		return tov_FIOPotato_South;
	case 1419:
		return tov_FIPotatoIndustry_North;
	case 1420:
		return tov_FIPotatoIndustry_South;
	case 1421:
		return tov_FIOPotatoIndustry_North;
	case 1422:
		return tov_FIOPotatoIndustry_South;
	case 1423:
		return tov_FISpringOats;
	case 1424:
		return tov_FIOSpringOats;
	case 1425:
		return tov_FISpringBarley_Malt;
	case 1426:
		return tov_FIOSpringBarley_Malt;
	case 1427:
		return tov_FIFabaBean;
	case 1428:
		return tov_FIOFabaBean;
	case 1429:
		return tov_FISpringBarley_Fodder;
	case 1430:
		return tov_FIOSpringBarley_Fodder;
	case 1431:
		return tov_FIGrasslandPasturePerennial1;
	case 1432:
		return tov_FIGrasslandPasturePerennial2;
	case 1433:
		return tov_FIGrasslandSilagePerennial1;
	case 1434:
		return tov_FIGrasslandSilagePerennial2;
	case 1435:
		return tov_FINaturalGrassland;
	case 1436:
		return tov_FINaturalGrassland_Perm;
	case 1437:
		return tov_FIFeedingGround;
	case 1438:
		return tov_FIBufferZone;
	case 1439:
		return tov_FIBufferZone_Perm;
	case 1440:
		return tov_FIGreenFallow_1year;
	case 1441:
		return tov_FIGreenFallow_Perm;
	case 1442:
		return tov_FIGrasslandSilageAnnual;
	case 1443:
		return tov_FICaraway1;
	case 1444:
		return tov_FICaraway2;
	case 1445:
		return tov_FIOCaraway1;
	case 1446:
		return tov_FIOCaraway2;
	case 1447:
		return tov_FISprSpringBarley_Fodder;
	case 1500:
		return tov_IRSpringWheat;
	case 1501:
		return tov_IRSpringBarley;
	case 1502:
		return tov_IRSpringOats;
	case 1503:
		return tov_IRGrassland_no_reseed;
	case 1504:
		return tov_IRGrassland_reseed;
	case 1505:
		return tov_IRWinterBarley;
	case 1506:
		return tov_IRWinterWheat;
	case 1507:
		return tov_IRWinterOats;
	case 1600:
		return tov_FRWinterWheat;
	case 1601:
		return tov_FRWinterBarley;
	case 1602:
		return tov_FRWinterTriticale;
	case 1603:
		return tov_FRWinterRape;
	case 1604:
		return tov_FRMaize;
	case 1605:
		return tov_FRMaize_Silage;
	case 1606:
		return tov_FRSpringBarley;
	case 1607:
		return tov_FRGrassland;
	case 1608:
		return tov_FRGrassland_Perm;
	case 1609:
		return tov_FRSpringOats;
	case 1610:
		return tov_FRSunflower;
	case 1611:
		return tov_FRSpringWheat;
	case 1612:
		return tov_FRPotatoes;
	case 1613:
		return tov_FRSorghum;
	case 88:
		return tov_DummyCropPestTesting;
	case 1700:
		return tov_ITGrassland;
	case 1701:
		return tov_ITOrchard;
	case 1702:
		return tov_ITOOrchard;

	case 1900:
		return tov_HBMaizeIntensive;
	case 1901:
		return tov_HBMaizeLowInput;
	case 1902:
		return tov_HBOrchard;
	case 1903:
		return tov_HBFallow;
	case 1904:
		return tov_HBAcacia;
	case 1905:
		return tov_HBNativeMix;
	case 1906:
		return tov_HBRegeneration;
	case 1907:
		return tov_HBProtectStrict;
	case 1908:
		return tov_HBProtectUse;
    case 9999:
      return tov_Undefined;
    default: // No matching code so we need an error message of some kind
      g_msg->Warn( WARN_FILE, "LE_TypeClass::TranslateVegTypes(): ""Unknown vegetation type:", VegReference);
      exit( 1 );
  }
}

//-----------------------------------------------------------------------
// created 25/08/00
int LE_TypeClass::BackTranslateVegTypes( TTypesOfVegetation VegReference ) {
  // This returns the vegetation type (or crop type) as applicable
  switch ( VegReference ) {
    case tov_SpringBarley:
      return 1;
    case tov_WinterBarley:
      return 2;
    case tov_SpringWheat:
      return 3;
    case tov_WinterWheat:
      return 4;
    case tov_WinterRye:
      return 5;
    case tov_Oats:
      return 6;
    case tov_Triticale:
      return 7;
    case tov_Maize:
      return 8;
    case tov_SpringBarleySeed:
      return 13;
    case tov_SpringBarleyStrigling:
      return 14;
    case tov_SpringBarleyStriglingSingle:
      return 15;
    case tov_SpringBarleyStriglingCulm:
      return 16;
    case tov_WinterWheatStrigling:
		return 17;
    case tov_WinterWheatStriglingSingle:
		return 18;
    case tov_WinterWheatStriglingCulm:
		return 19;
    case tov_SpringRape:
      return 21;
    case tov_WinterRape:
      return 22;
    case tov_FieldPeas:
      return 30;
	case tov_FieldPeasSilage:
		return 31;
	case tov_BroadBeans:
		return 32;
    case tov_SetAside:
      return 50;
    case tov_PermanentSetAside:
      return 54;
    case tov_YoungForest:
      return 55;
	case tov_FodderBeet:
		return 60;
	case tov_SugarBeet:
		return 61;
	case tov_CloverGrassGrazed1:
      return 65;
    case tov_PotatoesIndustry:
      return 92;
    case tov_Potatoes:
      return 93;
    case tov_SeedGrass1:
      return 94;
    case tov_OWinterBarley:
      return 102;
    case tov_OWinterBarleyExt:
      return 611;
    case tov_OWinterRye:
      return 105;
    case tov_SpringBarleyGrass:
      return 107;
    case tov_SpringBarleyCloverGrass:
      return 108;
	case tov_SpringBarleySpr:
		return 109;
	case tov_OSBarleySilage:
		return 103;
	case tov_OBarleyPeaCloverGrass:
      return 113;
    case tov_SpringBarleyPeaCloverGrassStrigling:
      return 114;
    case tov_SpringBarleySilage:
      return 115;
    case tov_OWinterRape:
      return 122;
    case tov_PermanentGrassGrazed:
      return 140;
    case tov_PermanentGrassLowYield:
      return 141;
    case tov_PermanentGrassTussocky:
      return 142;
    case tov_CloverGrassGrazed2:
      return 165;
    case tov_SeedGrass2:
      return 194;
    case tov_OSpringBarley:
      return 201;
	case tov_OWinterWheatUndersown:
		return 204;
	case tov_OWinterWheat:
		return 205;
	case tov_OOats:
      return 206;
    case tov_OTriticale:
      return 207;
    case tov_OFieldPeas:
      return 230;
    case tov_OFieldPeasSilage:
      return 106;
	case tov_OFodderBeet:
		return 260;
	case tov_OCloverGrassGrazed1:
      return 265;
    case tov_OCarrots:
      return 270;
    case tov_Carrots:
      return 271;
    case tov_OPotatoes:
      return 293;
    case tov_OSeedGrass1:
      return 294;
	case tov_OSpringBarleyPigs:
	  return 306;
	case tov_OSpringBarleyGrass:
      return 307;
	case tov_OSpringBarleyClover:
      return 308;
    case tov_OPermanentGrassGrazed:
      return 340;
    case tov_OCloverGrassGrazed2:
      return 365;
	case tov_OCloverGrassSilage1:
		return 366;
    case tov_OSeedGrass2:
      return 394;
    case tov_NaturalGrass:
      return 400;
    case tov_None:
      return 401;
    case tov_NoGrowth:
      return 402;
    case tov_WWheatPControl:
      return 601;
    case tov_WWheatPToxicControl:
      return 602;
    case tov_WWheatPTreatment:
      return 603;
    case tov_AgroChemIndustryCereal:
      return 604;
    case tov_WinterWheatShort:
      return 605;
    case tov_MaizeSilage:
      return 606;
	case tov_FodderGrass:
		return 607;
    case tov_SpringBarleyPTreatment:
      return 608;
    case tov_OSpringBarleyExt:
      return 609;
    case tov_OMaizeSilage:
      return 610;
	case tov_SpringBarleySKManagement:
		return 612;
	case tov_Heath:
		return 613;
	case tov_OrchardCrop:
		return 700;
	case tov_WaterBufferZone:
		return 701;
	case tov_FlowerStrip1:
		return 702;
	case tov_FlowerStrip2:
		return 703;
	case tov_FlowerStrip3:
		return 704;

	case tov_PLWinterWheat:
		return 801;
	case tov_PLWinterRape:
		return 802;
	case tov_PLWinterBarley:
		return 803;
	case tov_PLWinterTriticale:
		return 804;
	case tov_PLWinterRye:
		return 805;
	case tov_PLSpringWheat:
		return 806;
	case tov_PLSpringBarley:
		return 807;
	case tov_PLMaize:
		return 808;
	case tov_PLMaizeSilage:
		return 809;
	case tov_PLPotatoes:
		return 810;
	case tov_PLBeet:
		return 811;
	case tov_PLFodderLucerne1:
		return 812;
	case tov_PLFodderLucerne2:
		return 813;
	case tov_PLCarrots:
		return 814;
	case tov_PLSpringBarleySpr:
		return 815;
	case tov_PLWinterWheatLate:
		return 816;
	case tov_PLBeetSpr:
		return 817;
	case tov_PLBeans:
		return 818;

	case tov_NLBeet:
		return 850;
	case tov_NLCarrots:
		return 851;
	case tov_NLMaize:
		return 852;
	case tov_NLPotatoes:
		return 853;
	case tov_NLSpringBarley:
		return 854;
	case tov_NLWinterWheat:
		return 855;
	case tov_NLCabbage:
		return 856;
	case tov_NLTulips:
		return 857;
	case tov_NLGrassGrazed1:
		return 858;
	case tov_NLGrassGrazed2:
		return 859;
	case tov_NLPermanentGrassGrazed:
		return 860;
	case tov_NLCatchCropPea:
		return 861;
	case tov_NLBeetSpring:
		return 862;
	case tov_NLCarrotsSpring:
		return 863;
	case tov_NLMaizeSpring:
		return 864;
	case tov_NLPotatoesSpring:
		return 865;
	case tov_NLSpringBarleySpring:
		return 866;
	case tov_NLCabbageSpring:
		return 867;
	case tov_NLGrassGrazed1Spring:
		return 868;
	case tov_NLGrassGrazedLast:
		return 869;
	case tov_NLOrchardCrop:
		return 870;
	case tov_NLPermanentGrassGrazedExtensive:
		return 871;
	case tov_NLGrassGrazedExtensive1:
		return 872;
	case tov_NLGrassGrazedExtensive2:
		return 873;
	case tov_NLGrassGrazedExtensive1Spring:
		return 874;
	case tov_NLGrassGrazedExtensiveLast:
		return 875;

	case tov_PTPermanentGrassGrazed:
		return 900;
	case tov_PTWinterWheat:
		return 901;
	case tov_PTGrassGrazed:
		return 902;
	case tov_PTSorghum:
		return 903;
	case tov_PTFodderMix:
		return 904;
	case tov_PTTurnipGrazed:
		return 905;
	case tov_PTCloverGrassGrazed1:
		return 906;
	case tov_PTCloverGrassGrazed2:
		return 907;
	case tov_PTTriticale:
		return 908;
	case tov_PTOtherDryBeans:
		return 909;
	case tov_PTShrubPastures:
		return 910;
	case tov_PTCorkOak:
		return 911;
	case tov_PTVineyards:
		return 912;
	case tov_PTWinterBarley:
		return 913;
	case tov_PTBeans:
		return 914;
	case tov_PTWinterRye:
		return 915;
	case tov_PTOliveGroveTraditional:
		return 916;
	case tov_PTOliveGroveTradOrganic:
		return 917;
	case tov_PTOliveGroveIntensive:
		return 918;
	case tov_PTOliveGroveSuperIntensive:
		return 919;
	case tov_PTRyegrass:
		return 921;
	case tov_PTYellowLupin:
		return 922;
	case tov_PTMaize:
		return 923;
	case tov_PTOats:
		return 924;
	case tov_PTPotatoes:
		return 925;
	case tov_PTHorticulture:
		return 926;
	case tov_PTMaize_Hort:
		return 927;
	case tov_PTCabbage:
		return 928;
	case tov_PTCabbage_Hort:
		return 929;

	case tov_DEOats:
		return 950;
	case tov_DESpringRye:
		return 951;
	case tov_DEWinterWheat:
		return 952;
	case tov_DEMaizeSilage:
		return 953;
	case tov_DEPotatoes:
		return 954;
	case tov_DEMaize:
		return 955;
	case tov_DEWinterRye:
		return 956;
	case tov_DEWinterBarley:
		return 957;
	case tov_DESugarBeet:
		return 958;
	case tov_DEWinterRape:
		return 959;
	case tov_DETriticale:
		return 960;
	case tov_DECabbage:
		return 961;
	case tov_DECarrots:
		return 962;
	case tov_DEGrasslandSilageAnnual:
		return 963;
	case tov_DEGreenFallow_1year:
		return 964;
	case tov_DELegumes:
		return 965;
	case tov_DEOCabbages:
		return 966;
	case tov_DEOCarrots:
		return 967;
	case tov_DEOGrasslandSilageAnnual:
		return 968;
	case tov_DEOGreenFallow_1year:
		return 969;
	case tov_DEOLegume:
		return 970;
	case tov_DEOMaize:
		return 971;
	case tov_DEOMaizeSilage:
		return 972;
	case tov_DEOOats:
		return 973;
	case tov_DEOPermanentGrassGrazed:
		return 974;
	case tov_DEOPotatoes:
		return 975;
	case tov_DEOSpringRye:
		return 976;
	case tov_DEOSugarBeet:
		return 977;
	case tov_DEOTriticale:
		return 978;
	case tov_DEOWinterBarley:
		return 979;
	case tov_DEOWinterRape:
		return 980;
	case tov_DEOWinterRye:
		return 981;
	case tov_DEOWinterWheat:
		return 982;
	case tov_DEWinterWheatLate:
		return 983;
	case tov_DEPermanentGrassGrazed:
		return 984;
	case tov_DEPermanentGrassLowYield:
		return 985;
	case tov_DEPotatoesIndustry:
		return 986;
	case tov_DEPeas:
		return 987;
	case tov_DEOPeas:
		return 988;
	case tov_DEAsparagusEstablishedPlantation:
		return 989;
	case tov_DEHerbsPerennial_1year:
		return 990;
	case tov_DEHerbsPerennial_after1year:
		return 991;
	case tov_DESpringBarley:
		return 992;
	case tov_DEOrchard:
		return 993;
	case tov_DEBushFruitPerm:
		return 994;
	case tov_DEOAsparagusEstablishedPlantation:
		return 995;
	case tov_DEOOrchard:
		return 996;
	case tov_DEOBushFruitPerm:
		return 997;
	case tov_DEOPermanentGrassLowYield:
		return 998;
	case tov_DEOHerbsPerennial_1year:
		return 999;
	case tov_DEOHerbsPerennial_after1year:
		return 1000;

	case tov_OGrazingPigs:
		return 271;
	case tov_Wasteland:
		return 272;
	case tov_DummyCropPestTesting:
		return 888;
	case tov_DKOOrchardCrop_Perm:     
		return   1001;
	case tov_DKOBushFruit_Perm1:
		return   1002;
	case tov_DKOBushFruit_Perm2:       
		return   1003;
	case tov_DKChristmasTrees_Perm:   
		return   1004;
	case tov_DKOChristmasTrees_Perm:  
		return   1005;
	case tov_DKEnergyCrop_Perm:       
		return   1006;
	case tov_DKOEnergyCrop_Perm:      
		return   1007;
	case tov_DKFarmForest_Perm:       
		return   1008;
	case tov_DKOFarmForest_Perm:      
		return   1009;
	case tov_DKGrazingPigs_Perm:      
		return   1010;
	case tov_DKOGrazingPigs_Perm:     
		return   1011;
	case tov_DKOGrassGrazed_Perm:     
		return   1012;
	case tov_DKOGrassLowYield_Perm:   
		return   1013;
	case tov_DKFarmYoungForest_Perm:  
		return   1014;
	case tov_DKOFarmYoungForest_Perm: 
		return   1015;

		//1084-1090 not used as Christmas tree codes are merged

	case tov_UKBeans:
		return 1200;
	case tov_UKBeet:
		return 1201;
	case tov_UKMaize:
		return 1202;
	case tov_UKPermanentGrass:
		return 1203;
	case tov_UKPotatoes:
		return 1204;
	case tov_UKSpringBarley:
		return 1205;
	case tov_UKTempGrass:
		return 1206;
	case tov_UKWinterBarley:
		return 1207;
	case tov_UKWinterRape:
		return 1208;
	case tov_UKWinterWheat:
		return 1209;
        
	case tov_BEBeet:
		return 1250;
	case tov_BEBeetSpring:
		return 1251;
	case tov_BECatchPeaCrop:
		return 1252;
	case tov_BEGrassGrazed1:
		return 1253;
	case tov_BEGrassGrazed1Spring:
		return 1254;
	case tov_BEGrassGrazed2:
		return 1255;
	case tov_BEGrassGrazedLast:
		return 1256;
	case tov_BEMaize:
		return 1257;
	case tov_BEMaizeSpring:
		return 1258;
	case tov_BEOrchardCrop:
		return 1259;
	case tov_BEPotatoes:
		return 1260;
	case tov_BEPotatoesSpring:
		return 1261;
	case tov_BEWinterBarley:
		return 1262;
	case tov_BEWinterWheat:
		return 1263;
	case tov_BEWinterWheatCC:
		return 1264;
	case tov_BEWinterBarleyCC:
		return 1265;
	case tov_BEMaizeCC:
		return 1266;

	case tov_DKOLegume_Peas:         
		return  1016;
	case tov_DKOLegume_Whole:   
		return  1017;
	case tov_DKSugarBeets:      
		return  1018;
	case tov_DKOSugarBeets:     
		return  1019;
	case tov_DKCabbages:        
		return  1020;
	case tov_DKOCabbages:       
		return  1021;
	case tov_DKCarrots:         
		return  1022;
	case tov_DKOCarrots:        
		return  1023;
	case tov_DKLegume_Whole:    
		return  1024;
	case tov_DKLegume_Peas:          
		return  1025;
	case tov_DKWinterWheat:     
		return  1026;
	case tov_DKOWinterWheat:
		return 1027;
	case tov_DKSpringBarley:
		return 1028;
	case tov_DKOSpringBarley:
		return 1029;
	case tov_DKCerealLegume:
		return 1030;
	case tov_DKOCerealLegume:
		return 1031;
	case tov_DKCerealLegume_Whole:
		return 1032;
	case tov_DKOCerealLegume_Whole:
		return 1033;
	case tov_DKBushFruit_Perm1:
		return   1034;
	case tov_DKBushFruit_Perm2:
		return   1035;
	case tov_DKSpringFodderGrass:
		return 1036;
	case tov_DKOWinterFodderGrass:
		return 1037;
	case tov_DKGrassGrazed_Perm:
		return 1038;
	case tov_DKGrassLowYield_Perm:
		return 1039;
	case tov_DKGrazingPigs:
		return 1040;
	case tov_DKMaize:
		return 1041;
	case tov_DKMaizeSilage:
		return 1042;
	case tov_DKMixedVeg:
		return 1043;
	case tov_DKOGrazingPigs:
		return 1044;
	case tov_DKOMaize:
		return 1045;
	case tov_DKOMaizeSilage:
		return 1046;
	case tov_DKOMixedVeg:
		return 1047;
	case tov_DKOPotato:
		return 1048;
	case tov_DKOPotatoIndustry:
		return 1049;
	case tov_DKOPotatoSeed:
		return 1050;
	case tov_DKOrchardCrop_Perm:
		return 1051;
	case tov_DKOSeedGrassRye_Spring:
		return 1052;
	case tov_DKOSetAside:
		return 1053;
	case tov_DKOSpringBarleySilage:
		return 1054;
	case tov_DKOSpringOats:
		return 1055;
	case tov_DKOSpringWheat:
		return 1056;
	case tov_DKOVegSeeds:
		return 1057;
	case tov_DKOWinterBarley:
		return 1058;
	case tov_DKOWinterRape:
		return 1059;
	case tov_DKOWinterRye:
		return 1060;
	case tov_DKPlantNursery_Perm:
		return 1061;
	case tov_DKPotato:
		return 1062;
	case tov_DKPotatoIndustry:
		return 1063;
	case tov_DKPotatoSeed:
		return 1064;
	case tov_DKSeedGrassFescue_Spring:
		return 1065;
	case tov_DKSeedGrassRye_Spring:
		return 1066;
	case tov_DKSetAside:
		return 1067;
	case tov_DKSetAside_SummerMow:
		return 1068;
	case tov_DKSpringBarley_Green:
		return 1069;
	case tov_DKSpringBarleySilage:
		return 1070;
	case tov_DKSpringOats:
		return 1071;
	case tov_DKSpringWheat:
		return 1072;
	case tov_DKUndefined:
		return 1073;
	case tov_DKVegSeeds:
		return 1074;
	case tov_DKWinterBarley:
		return 1075;
	case tov_DKWinterRape:
		return 1076;
	case tov_DKWinterRye:
		return 1077;
	case tov_DKOWinterCloverGrassGrazedSown:
		return 1078;
	case tov_DKOCloverGrassGrazed1:
		return 1079;
	case tov_DKOCloverGrassGrazed2:
		return 1080;
	case tov_DKCloverGrassGrazed1:
		return 1081;
	case tov_DKWinterCloverGrassGrazedSown:
		return 1082;
	case tov_DKCloverGrassGrazed2:
		return 1083;
	case tov_DKCloverGrassGrazed3:
		return 1084;
	case tov_DKOCloverGrassGrazed3:
		return 1085;
	case tov_DKGrassTussocky_Perm:
		return 1086;
	case tov_DKOLegumeCloverGrass_Whole:
		return  1091;
	case tov_DKLegume_Beans:
		return  1092;
	case tov_DKWinterFodderGrass:
		return 1093;
	case tov_DKOrchApple:
		return 1094;
	case tov_DKOrchPear:
		return 1095;
	case tov_DKOrchCherry:
		return 1096;
	case tov_DKOrchOther:
		return 1097;
	case tov_DKOOrchApple:
		return 1098;
	case tov_DKOOrchPear:
		return 1099;
	case tov_DKOOrchCherry:
		return 1100;
	case tov_DKOOrchOther:
		return 1101;
	case tov_DKFodderBeets:
		return 1102;
	case tov_DKOLegume_Beans:
		return 1103;
	case tov_DKOFodderBeets:
		return 1104;
	case tov_DKOSpringFodderGrass:
		return 1105;
	case tov_DKCatchCrop:
		return 1106;
	case tov_DKOCatchCrop:
		return 1107;
	case tov_DKSpringBarleyCloverGrass:
		return 1108;
	case tov_DKOSpringBarleyCloverGrass:
		return 1109;
	case tov_DKWinterWheat_CC:
		return 1110;
	case tov_DKWinterRye_CC:
		return 1111;
	case tov_DKSpringOats_CC:
		return 1112;
	case tov_DKSpringBarley_CC:
		return 1113;
	case tov_DKOWinterWheat_CC:
		return 1114;
	case tov_DKOWinterRye_CC:
		return 1115;
	case tov_DKOSpringOats_CC:
		return 1116;
	case tov_DKOSpringBarley_CC:
		return 1117;
	case tov_DKOLegume_Beans_CC:
		return 1118;
	case tov_DKOLegume_Peas_CC:
		return 1119;
	case tov_DKOLegume_Whole_CC:
		return 1120;
	case tov_DKOLupines:
		return 1121;
	case tov_DKOLentils:
		return 1122;
	case tov_DKOSetAside_AnnualFlower:
		return 1123;
	case tov_DKOSetAside_PerennialFlower:
		return 1124;
	case tov_DKOSetAside_SummerMow:
		return 1125;
	case tov_DKOptimalFlowerMix1:
		return 1126;
	case tov_DKOptimalFlowerMix2:
		return 1127;
	case tov_DKOptimalFlowerMix3:
		return 1128;

	case tov_FIWinterWheat:
		return 1400;
	case tov_FIOWinterWheat:
		return 1401;
	case tov_FISugarBeet:
		return 1402;
	case tov_FIStarchPotato_North:
		return 1403;
	case tov_FIStarchPotato_South:
		return 1404;
	case tov_FIOStarchPotato_North:
		return 1405;
	case tov_FIOStarchPotato_South:
		return 1406;
	case tov_FISpringWheat:
		return 1407;
	case tov_FIOSpringWheat:
		return 1408;
	case tov_FITurnipRape:
		return 1409;
	case tov_FIOTurnipRape:
		return 1410;
	case tov_FISpringRape:
		return 1411;
	case tov_FIOSpringRape:
		return 1412;
	case tov_FIWinterRye:
		return 1413;
	case tov_FIOWinterRye:
		return 1414;
	case tov_FIPotato_North:
		return 1415;
	case tov_FIPotato_South:
		return 1416;
	case tov_FIOPotato_North:
		return 1417;
	case tov_FIOPotato_South:
		return 1418;
	case tov_FIPotatoIndustry_North:
		return 1419;
	case tov_FIPotatoIndustry_South:
		return 1420;
	case tov_FIOPotatoIndustry_North:
		return 1421;
	case tov_FIOPotatoIndustry_South:
		return 1422;
	case tov_FISpringOats:
		return 1423;
	case tov_FIOSpringOats:
		return 1424;
	case tov_FISpringBarley_Malt:
		return 1425;
	case tov_FIOSpringBarley_Malt:
		return 1426;
	case tov_FIFabaBean:
		return 1427;
	case tov_FIOFabaBean:
		return 1428;
	case tov_FISpringBarley_Fodder:
		return 1429;
	case tov_FIOSpringBarley_Fodder:
		return 1430;
	case tov_FIGrasslandPasturePerennial1:
		return 1431;
	case tov_FIGrasslandPasturePerennial2:
		return 1432;
	case tov_FIGrasslandSilagePerennial1:
		return 1433;
	case tov_FIGrasslandSilagePerennial2:
		return 1434;
	case tov_FINaturalGrassland:
		return 1435;
	case tov_FINaturalGrassland_Perm:
		return 1436;
	case tov_FIFeedingGround:
		return 1437;
	case tov_FIBufferZone:
		return 1438;
	case tov_FIBufferZone_Perm:
		return 1439;
	case tov_FIGreenFallow_1year:
		return 1440;
	case tov_FIGreenFallow_Perm:
		return 1441;
	case tov_FIGrasslandSilageAnnual:
		return 1442;
	case tov_FICaraway1:
		return 1443;
	case tov_FICaraway2:
		return 1444;
	case tov_FIOCaraway1:
		return 1445;
	case tov_FIOCaraway2:
		return 1446;
	case tov_FISprSpringBarley_Fodder:
		return 1447;
	case tov_SESpringBarley:
		return 1300;
	case tov_SEWinterRape_Seed:
		return 1301;
	case tov_SEWinterWheat:
		return 1302;
	case tov_IRSpringWheat:
		return 1500;
	case tov_IRSpringBarley:
		return 1501;
	case tov_IRSpringOats:
		return 1502;
	case tov_IRGrassland_no_reseed:
		return 1503;
	case tov_IRGrassland_reseed:
		return 1504;
	case tov_IRWinterBarley:
		return 1505;
	case tov_IRWinterWheat:
		return 1506;
	case tov_IRWinterOats:
		return 1507;

	case tov_FRWinterWheat:
		return 1600;
	case tov_FRWinterBarley:
		return 1601;
	case tov_FRWinterTriticale:
		return 1602;
	case tov_FRWinterRape:
		return 1603;
	case tov_FRMaize:
		return 1604;
	case tov_FRMaize_Silage:
		return 1605;
	case tov_FRSpringBarley:
		return 1606;
	case tov_FRGrassland:
		return 1607;
	case tov_FRGrassland_Perm:
		return 1608;
	case tov_FRSpringOats:
		return 1609;
	case tov_FRSunflower:
		return 1610;
	case tov_FRSpringWheat:
		return 1611;
	case tov_FRPotatoes:
		return 1612;
	case tov_FRSorghum:
		return 1613;
	case tov_ITGrassland:
		return 1700;
	case tov_ITOrchard:
		return 1701;
	case tov_ITOOrchard:
		return 1702;
	case tov_HBMaizeIntensive:
		return 1900;
	case tov_HBMaizeLowInput:
		return 1901;
	case tov_HBOrchard:
		return 1902;
	case tov_HBFallow:
		return 1903;
	case tov_HBAcacia:
		return 1904;
	case tov_HBNativeMix:
		return 1905;
	case tov_HBRegeneration:
		return 1906;
	case tov_HBProtectStrict:
		return 1907;
	case tov_HBProtectUse:
		return 1908;
    case tov_Undefined:
      return 9999;
    default: // No matching code so we need an error message of some kind
      g_msg->Warn( WARN_FILE, "LE_TypeClass::BackTranslateVegTypes(): ""Unknown vegetation type:",int(VegReference));
      exit( 1 );
  }
}

//-----------------------------------------------------------------------
// created 25/08/00
int LE_TypeClass::BackTranslateEleTypes( TTypesOfLandscapeElement EleReference ) {

  // This returns the vegetation type (or crop type) as applicable
  switch ( EleReference )
  {
    case tole_Building:				return 5;
    case tole_UrbanNoVeg:			return 8;
	case tole_UrbanVeg:				return 9;
    case tole_Garden:				return 11;
    case tole_AmenityGrass:			return 12;
    case tole_RoadsideVerge:		return 13;
    case tole_Parkland:				return 14;
    case tole_StoneWall:			return 15;
    case tole_BuiltUpWithParkland:	return 16;
    case tole_UrbanPark:			return 17;
    case tole_Field:			    return 20;
    case tole_PermPastureTussocky:  return 27;
    case tole_PermPastureLowYield:  return 26;
    case tole_UnsprayedFieldMargin: return 31;
    case tole_PermanentSetaside:    return 33;
    case tole_PermPasture:		    return 35;
    case tole_DeciduousForest:      return 40;
	case tole_Copse:				return 41;
	case tole_ConiferousForest:     return 50;
    case tole_YoungForest:		    return 55;
    case tole_Orchard:			    return 56;
    case tole_BareRock:			    return 69;
	case tole_OrchardBand:			return 57;
	case tole_MownGrassStrip:			return 58;
	case tole_MixedForest:		    return 60;
	case tole_Scrub:				return 70;
	case tole_PitDisused:		    return 75;
    case tole_Saltwater:		    return 80;
    case tole_Freshwater:		    return 90;
    case tole_Heath:			    return 94;
    case tole_Marsh:				return 95;
    case tole_River:				return 96;
    case tole_RiversideTrees:	    return 97;
    case tole_RiversidePlants:      return 98;
    case tole_Coast:			    return 100;
	case tole_SandDune:				return 101;
	case tole_NaturalGrassDry:      return 110;
    case tole_ActivePit:		    return 115;
    case tole_Railway:				return 118;
    case tole_LargeRoad:			return 121;
    case tole_SmallRoad:			return 122;
    case tole_Track:				return 123;
    case tole_Hedges:				return 130;
    case tole_HedgeBank:			return 140;
    case tole_BeetleBank:		    return 141;
	case tole_Chameleon:			return 150;
	case tole_FieldBoundary:	    return 160;
 	case tole_RoadsideSlope:		return 201;
	case tole_MetalledPath:			return 202;
	case tole_Carpark:				return 203;
	case tole_Churchyard:			return 204;
	case tole_NaturalGrassWet:		return 205;
	case tole_Saltmarsh:			return 206;
	case tole_Stream:				return 207;
	case tole_HeritageSite:			return 208;
	case tole_Wasteland:			return 209; 
	case tole_UnknownGrass:			return 210;
	case tole_WindTurbine:			return 211;
	case tole_Pylon:				return 212;
	case tole_IndividualTree:		return 213;
	case tole_PlantNursery:			return 214;
	case tole_Vildtager:			return 215;
	case tole_WoodyEnergyCrop:		return 216;
	case tole_WoodlandMargin:		return 217;
	case tole_PermPastureTussockyWet:		return 218;
	case tole_Pond:                 return 219;
	case tole_FishFarm:                 return 220;
	case tole_RiverBed:		return 221;
	case tole_DrainageDitch:		return 222;
	case tole_Canal:	return 223;
	case tole_RefuseSite:	return 224;
	case tole_Fence:				return 225;
	case tole_WaterBufferZone:		return 226;
    // adding new elements ( year 2021 )
	case  tole_Airport:				return 300;
	case  tole_Portarea:			return 301;
	case  tole_Saltpans:			return 302;
	case  tole_Pipeline:			return 303;
	case  tole_SolarPanel:			return 304;
	// adding new forests 
	case  tole_SwampForest:			return 400;
	case  tole_MontadoCorkOak:		return 401;
	case  tole_MontadoHolmOak:		return 402;
	case  tole_MontadoMixed:		return 403;
	case  tole_AgroForestrySystem:	return 404;
	case  tole_CorkOakForest:		return 405;
	case  tole_HolmOakForest:		return 406;
	case  tole_OtherOakForest:		return 407;
	case  tole_ChestnutForest:		return 408;
	case  tole_EucalyptusForest:	return 409;
	case  tole_InvasiveForest:		return 410;
	case  tole_MaritimePineForest:	return 411;
	case  tole_StonePineForest:		return 412;
	case  tole_ForestAisle:			return 413;
	// adding new permanent crop fields
	case  tole_Vineyard:			return 500;
	case  tole_OliveGrove:			return 501;
	case  tole_RiceField:			return 502;
	case  tole_OOrchard:             return 503;
	case  tole_BushFruit:            return 504;
	case  tole_OBushFruit:           return 505;
	case  tole_ChristmasTrees:       return 506;
	case  tole_OChristmasTrees:      return 507;
	case  tole_EnergyCrop:           return 508;
	case  tole_OEnergyCrop:          return 509;
	case  tole_FarmForest:           return 510;
	case  tole_OFarmForest:          return 511;
	case  tole_PermPasturePigs:      return 512;
	case  tole_OPermPasturePigs:     return 513;
	case  tole_OPermPasture:         return 514;
	case  tole_OPermPastureLowYield: return 515;
	case  tole_FarmYoungForest:      return 516;
	case  tole_OFarmYoungForest:     return 517;
	case  tole_AlmondPlantation:	 return 518;
	case  tole_WalnutPlantation:	 return 519;
	case  tole_FarmBufferZone:		 return 520;
	case  tole_NaturalFarmGrass:	 return 521;
	case  tole_GreenFallow:			 return 522;
	case  tole_FarmFeedingGround:	 return 523;
	case  tole_FlowersPerm:			 return 524;
	case  tole_AsparagusPerm:		 return 525;
	case  tole_MushroomPerm:		 return 526;
	case  tole_OtherPermCrop:		 return 527;
	case  tole_OAsparagusPerm:		 return 528;
	case tole_FlowerStrip:			 return 529;

	case tole_Missing:		return 2112;

	//case tole_Foobar: return 999;
	// !! type unknown - should not happen
	default:
      g_msg->Warn( WARN_FILE, "LE_TypeClass::BackTranslateEleTypes(): ""Unknown landscape element type:", int(EleReference) );
      exit( 1 );
  }
}

//------------------------------------------------------------------------

void BeetleBank::DoDevelopment( void ) {
  VegElement::DoDevelopment();
  m_insect_pop = m_insect_pop * cfg_beetlebankinsectscaler.value()*3.0;
}

