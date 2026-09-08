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

// Includes from ALMaSS
#define __CJTDebug_7

#include <string.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../BatchALMaSS/AOR_Probe.h"
#include "../Skylark/SkylarkAll.h"
#include "../Skylark/Skylarks_toletov.h"

int g_stopdate = July + 14;

// Constants

Landscape* g_land;

FILE* StriglingMort;

const string SimulationName = "Skylark";

//extern ALMaSSGUI* g_MainForm;

/**  */
/** Parameters under control of the config file ******************* */
/**  */
extern CfgInt cfg_SkStartNos;
extern CfgInt cfg_PmEventfrequency;
extern CfgInt cfg_PmEventsize;
extern CfgInt cfg_CatastropheEventStartYear;

extern CfgFloat cfg_Skylark_nestling_NOEL;
extern CfgFloat cfg_Skylark_nestling_Biodegredation;
extern CfgFloat cfg_Skylark_prefledegling_NOEL;
extern CfgFloat cfg_Skylark_prefledegling_Biodegredation;
extern CfgFloat cfg_Skylark_male_NOEL;
extern CfgFloat cfg_Skylark_male_Biodegredation;
extern CfgFloat cfg_Skylark_female_NOEL;
extern CfgFloat cfg_Skylark_female_Biodegredation;
extern CfgInt cfg_fecundity_reduc;
extern CfgInt cfg_fecundity_reduc_chance;
extern CfgInt cfg_insecticide_direct_mortE;
extern CfgInt cfg_insecticide_direct_mortN;
extern CfgInt cfg_insecticide_direct_mortP;
extern CfgInt cfg_insecticide_direct_mortM;
extern CfgInt cfg_insecticide_direct_mortF;
extern CfgFloat cfg_skylark_pesticide_globaleggshellreduction;
extern CfgFloat cfg_skylark_pesticide_eggshellreduction;
extern CfgBool cfg_skylark_pesticide_eggshellreduction_perclutch;

extern CfgFloat cfg_MaleSplitScale;
extern CfgFloat cfg_MinDaysToHatch;
extern CfgFloat cfg_PEmax;
extern CfgInt cfg_NestLeavingChance;
extern CfgFloat cfg_NestLeavingWeight;
extern CfgInt cfg_ClutchMortProb;
extern CfgInt cfg_NestlingMortProb;
extern CfgInt cfg_PreFledgeMortProb;
extern CfgFloat cfg_MeanHatchingWeight;
extern CfgFloat cfg_tramline_foraging;
extern CfgFloat cfg_Cooling_Rate_Eggs;
extern CfgFloat cfg_EggTemp;
extern CfgFloat cfg_MD_Threshold;
extern CfgFloat cfg_MeanExtractionRatePerMinute;
extern CfgFloat cfg_SK_hindrance_scale;
extern CfgInt cfg_FoodTripsPerDay;
extern CfgFloat cfg_sk_triplength;
extern CfgInt cfg_Breed_Res_Thresh1;
extern CfgFloat cfg_ConversionEffReduc;
extern CfgFloat cfg_EM_Nestling_a;
extern CfgFloat cfg_EM_Nestling_b;
extern CfgInt cfg_strigling_preflg;
extern CfgInt cfg_strigling_nestling;
extern CfgInt cfg_strigling_clutch;
extern CfgFloat cfg_FemaleMinTerritoryAcceptScore;
extern CfgFloat cfg_NestPlacementMinQual;
extern CfgInt cfg_HQualityOpenTallVeg;
extern CfgInt cfg_HQualityTall;
extern CfgInt cfg_HQualityTall2;
extern CfgInt cfg_HQualityTrack;
extern CfgInt cfg_HQualityMetalRoad;
extern CfgInt cfg_HQualityHedgeScrub;
extern CfgInt cfg_HQualityNeutral;
extern CfgInt cfg_HQualityHedge;
extern CfgInt cfg_HQualityTallVeg;
extern CfgInt cfg_HQualityWater;
extern CfgInt cfg_HQualityVeg30cm;
extern CfgInt cfg_HQualityBareEarth;
extern CfgInt cfg_PatchyPremium;
extern CfgInt cfg_SkTramlinesPremium;
extern CfgFloat cfg_SkScrapesPremiumII;

extern CfgFloat cfg_hindconstantH_b;
extern CfgFloat cfg_hindconstantD_b;
extern CfgFloat cfg_densityconstant_a;
extern CfgFloat cfg_densityconstant_b;
extern CfgInt cfg_densityconstant_c;
extern CfgFloat cfg_heightconstant_a;
extern CfgFloat cfg_heightconstant_b;
extern CfgInt cfg_heightconstant_c;

extern CfgInt cfg_SkylarkFirstBreedingDate;
extern CfgFloat cfg_ReturnProbability;
extern CfgInt cfg_juvreturnmort;
extern CfgInt cfg_adultreturnmort;
extern CfgFloat cfg_maxfeedrain;
extern CfgInt cfg_rainhindpow;
extern CfgInt cfg_temphindpow;
extern CfgBool cfg_ReallyBigOutputMonthly_used;
extern CfgBool cfg_CfgRipleysOutputUsed;
extern CfgBool cfg_AorOutput_used;
extern CfgBool cfg_ReallyBigOutputUsed;

bool CompareDist(const APoint i, const APoint j) { return i.m_y > j.m_y; }

const double EM_nest_T[14] = {
	0.0, 0.04958283, 0.077473171, 0.117759221, 0.148748489, 0.179737758, 0.322288393, 0.362574442, 0.38426693, 0.409058345, 0.436948687,
	0.399761565, 0.418355126, 0.440047614,
};

const double CE_nest[15] = // Conversion Efficiency (energy per gram gained)
{
	0, 0, 0.91082709, 0.827867467, 0.752463941, 0.683928293, 0.621634983, 0.565015451, 0.513552918, 0.466777677, 0.424262801, 0.38562025,
	0.350497326, 0.318573455, 0.289557263
};

static double NestLeavingWeight;
//**************** Prefledgeling Energetics
//****************
static int ClutchMortProb; // PO/AP data
static int NestlingMortProb;
static int PreFledgeMortProb;
static double MeanHatchingWeight;
static double Cooling_Rate_Eggs;
static double EggTemp; //was 37
static double MD_Threshold; // was 12
static double MeanExtractionRatePerMinute;
static int FoodTripsPerDay; // Poulsen
static int Breed_Res_Thresh1;
static double MD_Hatch; // value calculated in LoadParameters
const int terrnos[6] = {0, 1, 2, 4, 5, 10};
const int terrsize[6] = {0, 500, 250, 125, 100, 50};
const double VegQuality[31] = {
	30, 30, 29.6, 27.2, 24.8, 23, 21.2, 19.6, 18.2, 17, 15.8, 15, 14.2, 13.6, 13, 12.6, 12.2, 12, 11.8, 11.6, 11.6, 11.6, 11.6, 11.6, 11.6, 11.6,
	11.4, 11.4, 11.2, 11.2, 10.8
};
const double KcalPerGInsect = 5.0;
const double KcalPerGInsect_inv = 1.0 / KcalPerGInsect;
const double KcalPerGInsect_kg_inv = KcalPerGInsect_inv / 1000.0;
// Below EM assumes constant 36 degrees C
const double EM_coldblood1 = 0.4257;
const double EM_coldblood2 = 0.482;
const double EM_warmblood1 = 0.3456;
const double EM_warmblood2 = 5.9856;

// From Skylark_Male
const int HomeRangeDiameter = 280;
const int HomeRangeDiameterDiv20 = 14;

double RainHindrance[21];
double TempHindrance[31];
double VegHindranceH[111];
double VegHindranceD[111];
double DensityScore[111];
double HeightScore[111]; // There are 110 possible heights to consider
double TerrHeterogeneity[1000];
const double VeryHighDensityVeg = 2200.0; //Estimate nearly full growth in wheat
double MaxFeedRain; //Estimate of really serious amount of rain

// Overwintering mortality
int JuvenileReturnMort; //35
// below are mean values for male and female
// below are actual values for male and female
/*
int M_Mig_Mort_min; // = 11;
int M_Mig_Mort_max; // = 51;
int F_Mig_Mort_min; // = 29;
int F_Mig_Mort_max; // = 71;
*/

// From Skylark_Female
const double Breed_Res_Thresh2 = Breed_Res_Thresh1 * 4; //four eggs assumed
const double Breed_Temp_Thresh = 5.0;

//double SkYoungWetDay = 1000000; // will be reassigned in Init
/*
const double SkYoungColdDay = 9.0; // 9.0 degress
const double SkYoungWindyDay = 4.0; // 4.0 m per sec
*/
const double ExtraBroodHeat = 2.4 / KcalPerGInsect; // from Parus major g dw insect

//-----------------------
double IsTramline;
double NotTramline;
double FemaleNestAcceptScore;
double XFNestAcceptScore;

/** \brief The minimum quality for nest placement */
double NestPlacementMinQual = 15;
int HQualityOpenTallVeg = 16;
int HQualityTall = 0;
int HQualityTall2 = -1000; // -1000 used to invalidate a territory
int HQualityTrack = 10;
int HQualityMetalRoad = -10;
int HQualityHedgeScrub = 0;
int HQualityNeutral = 0;
int HQualityHedge = -1050; // was 1500 500 is too low
int HQualityWater = 0;
int HQualityVeg30cm = 11;

int PatchyPremium = 10; //10

int HQualityTallVeg = 1;
int HQualityBareEarth = 0;
int TramlinePremium = 5; //5

int HQualityGood = 30;
//int ScrapePremium = 10; //10

/* const int NaturalGrassPremium=10; const int PermanentSetasidePremium=20;  // 10 is max possible */
//-----------------------------------------------------------------------------
/* case  sleep_all_day: break; case  autumn_plough: break; case  autumn_harrow: break; case  autumn_roll: break;
case  autumn_sow: break; case  winter_plough: break; case  deep_ploughing: break; case  spring_plough: break;
case  spring_harrow: break; case  spring_roll: break; case  spring_sow: break; case  fp_npks: break; case  fp_npk: break;
case  fp_pk: break; case  fp_liquidNH3: break; case  fp_slurry: break; case  fp_manganesesulphate: break; case  fp_manure:
break; case  fp_greenmanure: break; case  fp_sludge: break; case  fa_npk: break; case  fa_pk: break; case  fa_slurry: break;
case  fa_ammoniumsulphate: break; case  fa_manure: break; case  fa_greenmanure: break; case  fa_sludge: break;
case  herbicide_treat: break; case  growth_regulator: break; case  fungicide_treat: break; case  insecticide_treat:
syninsecticide_treat break; case  molluscicide: break; case  row_cultivation: break; case  strigling: break; case  hilling_up:
break; case  water: break; case  swathing: break; case  harvest: break; case  cattle_in_out: break; case  cut_to_hay: break;
case  cut_to_silage: break; case  straw_chopping: break; case  hay_turning: break; case  hay_bailing: break; case  burning:
break; case  stubble_harrowing: break; case  autumn_or_spring_plough: break; case  burn_straw_stubble: break; */
//-----------------------------------------------------------------------------
//                       Skylark_Population_Manager
//-----------------------------------------------------------------------------
//
//******* Below are essential methods needed by all Population_Managers ********
//

Skylark_Population_Manager::Skylark_Population_Manager(Landscape* L) : Population_Manager(L, 5) {
	// Five lists are needed:
	// Clutches,Nestlings,PreFledgelings,Males,Females

	M_Mig_Mort = 0;
	F_Mig_Mort = 0;
	//
	/*
	F_Mig_Mort_min = cfg_femreturnmortmin.value();
	F_Mig_Mort_max = cfg_femreturnmortmax.value();
	M_Mig_Mort_min = cfg_malereturnmortmin.value();
	M_Mig_Mort_max = cfg_malereturnmortmax.value();
	*/
	//
	JuvenileReturnMort = cfg_juvreturnmort.value();
	//SkYoungWetDay = cfg_skyoungwetday.value();

	TheSkylarkTerrs = new SkTerritories(L);
#ifdef __CJTDebug_10
  refnum = 0;
#endif
	Init();

	this->OpenTheBreedingPairsProbe();
	this->OpenTheBreedingSuccessProbe();
	this->OpenTheFledgelingProbe();
}

//-----------------------------------------------------------------------------
void Skylark_Population_Manager::Init(void) {
	if (cfg_CfgRipleysOutputUsed.value()) { OpenTheRipleysOutputProbe(); }
	LoadParameters();
	// Data for the FledgelingProbe
	VegTypeFledgelings = new int[tov_Undefined + 1];
	for (int i = 0; i < tov_Undefined+1; i++) VegTypeFledgelings[i] = 0;
	// autom. called by constructor
	m_SimulationName = "Skylark";
	// Create 100 male and female skylarks

	int temp_thread_num = omp_get_max_threads();
	int start_num_in_tread = cfg_SkStartNos.value() / temp_thread_num + 1;

	#pragma omp parallel
	{
		const auto aps = new AdultSkylark_struct;
		aps->size = 38;
		aps->Terrs = TheSkylarkTerrs;
		aps->L = m_TheLandscape;
		g_land = m_TheLandscape;
		aps->sex = true;

		for (int i = 0; i < start_num_in_tread; i++)
		{
			aps->x = g_random_fnc(m_TheLandscape->SupplySimAreaWidth());
			aps->y = g_random_fnc(m_TheLandscape->SupplySimAreaHeight());
			aps->bx = aps->x;
			aps->by = aps->y;
			aps->age = g_random_fnc(5);
			aps->mh = tov_Undefined;
			CreateObjects(3, nullptr, nullptr, aps, 1);
		}
		aps->sex = false;
		for (int i = 0; i < start_num_in_tread; i++)
		{
			aps->x = g_random_fnc(m_TheLandscape->SupplySimAreaWidth());
			aps->y = g_random_fnc(m_TheLandscape->SupplySimAreaHeight());
			aps->bx = aps->x;
			aps->by = aps->y;
			aps->age = g_random_fnc(5);
			aps->mh = tov_Undefined;
			CreateObjects(4, nullptr, nullptr, aps, 1);
		}
		delete aps;
	}
	// Load List of Animal Classes
	m_ListNames[0] = "Clutch";
	m_ListNames[1] = "Nestling";
	m_ListNames[2] = "Pre-Fledgeling";
	m_ListNames[3] = "Male";
	m_ListNames[4] = "Female";
	m_ListNameLength = 5;
	m_population_type = TOP_Skylark;

	// Load State Names
	StateNames[toss_Initiation] = "Initiation";
	//Clutch
	StateNames[toss_Developing] = "CDeveloping";
	StateNames[toss_Hatching] = "CHatching";
	StateNames[toss_CDying] = "CDying";
	//Nestling
	StateNames[toss_NDeveloping] = "NDeveloping";
	StateNames[toss_NMaturing] = "NMaturing";
	StateNames[toss_NDying] = "NDying";
	//PreFledgeling
	StateNames[toss_PDeveloping] = "PDeveloping";
	StateNames[toss_PMaturing] = "PMaturing";
	StateNames[toss_PDying] = "PDying";
	//MaleSK
	StateNames[toss_MFlocking] = "MFlocking";
	StateNames[toss_MFloating] = "MFloating";
	StateNames[toss_MArriving] = "MArriving";
	StateNames[toss_MImmigrating] = "MImmigrating";
	StateNames[toss_MEmigrating] = "MEmigrating";
	StateNames[toss_MTempLeavingArea] = "MTempLeavingArea";
	StateNames[toss_MFindingTerritory] = "MFindingTerritory";
	StateNames[toss_AttractingAMate] = "MAttractingAMate";
	StateNames[toss_FollowingMate] = "MFollowingMate";
	StateNames[toss_ScaringOffChicks] = "MScaringOffChicks";
	StateNames[toss_MCaringForYoung] = "MCaringForYoung";
	StateNames[toss_MDying] = "MDying";
	//FemaleSK
	StateNames[toss_FFlocking] = "FFlocking";
	StateNames[toss_FFloating] = "FFloating";
	StateNames[toss_FArriving] = "FArriving";
	StateNames[toss_FImmigrating] = "FImmigrating";
	StateNames[toss_FEmigrating] = "FEmigrating";
	StateNames[toss_FTempLeavingArea] = "FTempLeavingArea";
	StateNames[toss_FFindingTerritory] = "FFindingTerritory";
	StateNames[toss_BuildingUpResources] = "FBuildingUpResources";
	StateNames[toss_MakingNest] = "FMakingNest";
	StateNames[toss_PreparingForBreeding] = "FPreparingForBreeding";
	StateNames[toss_Laying] = "FLaying";
	StateNames[toss_StartingNewBrood] = "FStartingNewBrood";
	StateNames[toss_EggHatching] = "FEggHatching";
	StateNames[toss_Incubating] = "FIncubating";
	StateNames[toss_StoppingBreeding] = "FStoppingBreeding";
	StateNames[toss_FCaringForYoung] = "FCaringForYoung";
	StateNames[toss_FDying] = "FDying";
	StateNames[toss_GivingUpTerritory] = "FGivingUpTerritory";
	StateNames[toss_Destroy] = "Being Destroyed";
	// determine whether we should shuffle, or sort or do nothing to skylarks
	// after each time step.
	BeforeStepActions[0] = 3; // 0 = Shuffle, 1 = SortX, 2 = SortY, 3 = do nothing
	BeforeStepActions[1] = 3; // 0 = Shuffle, 1 = SortX, 2 = SortY, 3 = do nothing
	BeforeStepActions[2] = 3; // 0 = Shuffle, 1 = SortX, 2 = SortY, 3 = do nothing
	BeforeStepActions[3] = 0; // 0 = Shuffle, 1 = SortX, 2 = SortY, 3 = do nothing
	BeforeStepActions[4] = 0; // 0 = Shuffle, 1 = SortX, 2 = SortY, 3 = do nothing
	// Zero any counters
	m_TotalEggs = 0;
	m_TotalNestlings = 0;
	m_TotalPrefledgelings = 0;
	m_NoFledgeDeaths = 0;
	m_NoChickDeaths = 0;
	m_NoPestEffects = 0;

	if (cfg_ReallyBigOutputUsed.value()) { OpenTheReallyBigProbe(); }
	else ReallyBigOutputPrb = nullptr;
#ifdef __SKPOM
  SKPOM1 = fopen( "SkylarkPOM1.txt", "w" );
  SKPOM2 = fopen( "SkylarkPOM2.txt", "w" );
#endif
}

//-----------------------------------------------------------------------------

void Skylark_Population_Manager::LoadParameters(void) {
	IsTramline = cfg_tramline_foraging.value();
	NotTramline = 1 - IsTramline;

	HQualityOpenTallVeg = cfg_HQualityOpenTallVeg.value();
	HQualityTall = cfg_HQualityTall.value();
	HQualityTall2 = cfg_HQualityTall2.value();
	HQualityTrack = cfg_HQualityTrack.value();
	HQualityMetalRoad = cfg_HQualityMetalRoad.value();
	HQualityHedgeScrub = cfg_HQualityHedgeScrub.value();
	HQualityNeutral = cfg_HQualityNeutral.value();
	HQualityHedge = cfg_HQualityHedge.value();
	HQualityTallVeg = cfg_HQualityTallVeg.value();
	HQualityWater = cfg_HQualityWater.value();
	HQualityVeg30cm = cfg_HQualityVeg30cm.value();
	HQualityBareEarth = cfg_HQualityBareEarth.value();
	PatchyPremium = cfg_PatchyPremium.value();
	FemaleNestAcceptScore = cfg_FemaleMinTerritoryAcceptScore.value();
	NestPlacementMinQual = cfg_NestPlacementMinQual.value();
	MaxFeedRain = cfg_maxfeedrain.value(); //Estimate of really serious amount of rain

	NestLeavingWeight = cfg_NestLeavingWeight.value();
	ClutchMortProb = cfg_ClutchMortProb.value();
	NestlingMortProb = cfg_NestlingMortProb.value();
	PreFledgeMortProb = cfg_PreFledgeMortProb.value();
	MeanHatchingWeight = cfg_MeanHatchingWeight.value();
	Cooling_Rate_Eggs = cfg_Cooling_Rate_Eggs.value();
	EggTemp = cfg_EggTemp.value(); //was 37
	MD_Threshold = cfg_MD_Threshold.value(); // was 12
	MeanExtractionRatePerMinute = cfg_MeanExtractionRatePerMinute.value(); //Fitting parameter
	FoodTripsPerDay = cfg_FoodTripsPerDay.value(); // Poulsen
	MD_Hatch = cfg_MinDaysToHatch.value() * 24 * 60 * (EggTemp - MD_Threshold); // 9.8 fastest development from field data
	//MD_Hatch = cfg_MinDaysToHatch.value() * 24 * 60;
	Breed_Res_Thresh1 = cfg_Breed_Res_Thresh1.value();

	for (int h = 0; h <= 110; h++)
	{
		DensityScore[h] = 0.0;
		VegHindranceD[h] = 1.0;
		HeightScore[h] = 0.0;
		VegHindranceH[h] = 1.0;
	}
	// Vegetation hindrance is made up of two parts - a densithy and height part
	// Each a linear after a threshold level, down to another threshold (zero)

	double threshold = cfg_heightconstant_c.value();
	for (int h = static_cast<int>(threshold); h <= 110; h++)
	{
		VegHindranceH[h] = 1 + h * cfg_hindconstantH_b.value();
		if (VegHindranceH[h] <= 0) VegHindranceH[h] = 0;
	}
	threshold = cfg_densityconstant_c.value();
	for (int d = static_cast<int>(threshold); d <= 110; d++)
	{
		VegHindranceD[d] = 1 + d * cfg_hindconstantD_b.value();
		if (VegHindranceD[d] <= 0) VegHindranceD[d] = 0;
	}

	// The next two arrays are related to nest choice
	threshold = cfg_densityconstant_c.value();
	for (int h = static_cast<int>(threshold); h <= 110; h++)
	{
		// Power function
		// DensityScore[ h ] = cfg_densityconstant_a.value() * pow( h - ( threshold - 1 ), cfg_densityconstant_b.value() );
		// Linear
		DensityScore[h] = cfg_densityconstant_a.value() + (h - threshold) * (h - threshold) * cfg_densityconstant_b.value();
		// Catch all just in case:
		//if ( DensityScore[ h ] < 0 ) DensityScore[ h ] = 0;
	}
	threshold = cfg_heightconstant_c.value();
	for (int h = static_cast<int>(threshold); h <= 110; h++)
	{
		// Power function
		//double hh = cfg_heightconstant_a.value() * pow( h - ( threshold - 1 ), cfg_heightconstant_b.value() );
		//HeightScore[ h ] = hh - ( 1 - hh ); // This expands the range of the curve
		// Linear
		HeightScore[h] = cfg_heightconstant_a.value() + (h - threshold) * cfg_heightconstant_b.value();
		// Catch all just in case:
		//if ( HeightScore[ h ] < 0 ) HeightScore[ h ] = 0;
	}
	for (int i = 0; i <= 30; i++)
	{
		//    TempHindrance[ i ] =  (1-( pow( 30 - i, cfg_temphindpow.value() ) / pow( 30, cfg_temphindpow.value() ) ));
		if (i > 20) TempHindrance[i] = 1.0;
		else
			TempHindrance[i] = 0.0 + 1.0 * (1 - pow(static_cast<double>(20) - i, static_cast<double>(cfg_temphindpow.value())) / pow(
				static_cast<double>(20), static_cast<double>(cfg_temphindpow.value())));
	}
	for (int i = 0; i <= 10; i++)
	{
		RainHindrance[i] = pow(static_cast<double>(10) - i, static_cast<double>(cfg_rainhindpow.value())) / pow(
			10.0, static_cast<double>(cfg_rainhindpow.value()));
	}

	m_NoFledgeDeaths = 0;
	m_NoChickDeaths = 0;
	m_NoPestEffects = 0;
	FILE* MyFile = fopen("PestEffects.txt", "w");
	fclose(MyFile);
	OpenTheBreedingSuccessProbe();
}

//-----------------------------------------------------------------------------

Skylark_Population_Manager::~Skylark_Population_Manager(void) {
	// delete all lists
	delete TheSkylarkTerrs;
	delete []VegTypeFledgelings;
#ifdef __SKPOM
  fclose(SKPOM1);
  fclose(SKPOM2);
#endif
}

//-----------------------------------------------------------------------------

void Skylark_Population_Manager::CreateObjects(const int ob_type, TAnimal* pTAo, void* /*null*/, Skylark_struct* data, int number) {
	Skylark_Clutch* New_Clutch;
	Skylark_Nestling* New_Nestling;
	Skylark_PreFledgeling* New_PreFledgeling;
	Skylark_Male* New_Male;
	Skylark_Female* New_Female;

	skClutch_struct* cs;
	Nestling_struct* ns;
	PreFledgeling_struct* pfs;
	AdultSkylark_struct* as;

	bool aCopy = false;
	if (number == -1)
	{
		aCopy = true;
		number = 1;
	}

	for (int i = 0; i < number; i++)
	{
#ifdef __CJTDebug_10
		refnum++;
#endif
		if (ob_type == 0)
		{
			cs = dynamic_cast<skClutch_struct*>(data);

			New_Clutch = new Skylark_Clutch(cs->Mum, cs->Terrs, cs->L, cs->No, cs->x, cs->y, cs->mh, this);
			PushIndividual(ob_type, New_Clutch);
			IncLiveArraySize(ob_type);
			// Give mum the pointer to the clutch
			cs->Mum->OnSetMyClutch(New_Clutch);
			
#ifdef __CJTDebug_10
			New_Clutch->m_Ref = refnum;
#endif
		}
		if (ob_type == 1)
		{
			ns = dynamic_cast<Nestling_struct*>(data);

			New_Nestling = new Skylark_Nestling(ns->x, ns->y, ns->Dad, ns->L, ns->Terrs, this, ns->bx, ns->by, ns->mh);
			PushIndividual(ob_type, New_Nestling);
			IncLiveArraySize(ob_type);
			// Give Dad a pointer to nestling
			ns->Dad->OnAddNestling(New_Nestling);
			
			// Count this as a chick hatched
			m_TotalNestlings++;
#ifdef __CJTDebug_10
			New_Nestling->m_Ref = refnum;
#endif
		}
		if (ob_type == 2)
		{
			pfs = dynamic_cast<PreFledgeling_struct*>(data);

			New_PreFledgeling = new Skylark_PreFledgeling(pfs->x, pfs->y, pfs->L, pfs->Terrs, pfs->Dad, pfs->sex, pfs->size, pfs->age, this,
															pfs->bx, pfs->by, pfs->mh);

			PushIndividual(ob_type, New_PreFledgeling);
			IncLiveArraySize(ob_type);
			if (pfs->Dad) pfs->Dad->OnAddPreFledgeling(New_PreFledgeling, static_cast<Skylark_Nestling*>(pTAo));
			
			// Count this as a nest leaver
			m_TotalPrefledgelings++;
		}
		if (ob_type == 3)
		{
			as = dynamic_cast<AdultSkylark_struct*>(data);
			// Male

			New_Male = new Skylark_Male(as->x, as->y, as->size, as->age, as->Terrs, as->L, this, as->bx, as->by, as->mh);
			if (aCopy) New_Male->SensibleCopy();
			PushIndividual(ob_type, New_Male);
			IncLiveArraySize(ob_type);
			
#ifdef __CJTDebug_10
			New_Male->m_Ref = refnum;
#endif
		}
		if (ob_type == 4)
		{
			as = dynamic_cast<AdultSkylark_struct*>(data);
			// Female

			New_Female = new Skylark_Female(as->x, as->y, as->size, as->age, as->Terrs, as->L, this, as->bx, as->by, as->mh);
			if (aCopy) New_Female->SensibleCopy();
			PushIndividual(ob_type, New_Female);
			IncLiveArraySize(ob_type);
			
#ifdef __CJTDebug_10
			New_Female->m_Ref = refnum;
#endif
		}
	}
}

//-----------------------------------------------------------------------------
//*************** End of essential methods ***************
//
//************ Below are non-essential methods ***********
//

int Skylark_Population_Manager::TheFledgelingProbe() {
	int NumberSk = 0;
	for (unsigned j = 0; j < GetLiveArraySize(3); j++)
	{
		const auto MS = dynamic_cast<Skylark_Male*>(SupplyAnimalPtr(3, j));
		if (MS->Age == 0)
		{
			NumberSk++; // it is in the square so increment number
			VegTypeFledgelings[MS->m_MyHome]++;
		}
	}
	for (unsigned j = 0; j < GetLiveArraySize(4); j++)
	{
		const auto FS = dynamic_cast<Skylark_Female*>(SupplyAnimalPtr(4, j));
		if (FS->Age == 0)
		{
			NumberSk++; // it is in the square so increment number
			VegTypeFledgelings[FS->m_MyHome]++;
		}
	}
	return NumberSk;
}

//-----------------------------------------------------------------------------
int Skylark_Population_Manager::TheBreedingFemalesProbe(int /*ProbeNo*/) {
	int NumberSk = 0;
	for (unsigned j = 0; j < GetLiveArraySize(3); j++)
	{
		const auto MS = dynamic_cast<Skylark_Male*>(SupplyAnimalPtr(3, j));
		/* for (unsigned i=0; i<TheProbe[ProbeNo]->m_NoAreas; i++) { if ((MS->m_Born_x>=(int)TheProbe[ProbeNo]->m_Rect[i].m_x1)
		&& (MS->m_Born_y>=(int)TheProbe[ProbeNo]->m_Rect[i].m_y1) && (MS->m_Born_x<(int)TheProbe[ProbeNo]->m_Rect[i].m_x2)
		&& (MS->m_Born_y<(int)TheProbe[ProbeNo]->m_Rect[i].m_y2) && (MS->Paired==true) && (MS->HaveTerritory==true))
		NumberSk++; // it is in the square so increment number } */
		if (MS->Paired == true && MS->HaveTerritory == true) NumberSk++; // it is in the square so increment number
	}
	return NumberSk;
}

//-----------------------------------------------------------------------------
int Skylark_Population_Manager::TheBreedingSuccessProbe(int& BreedingFemales, int& YoungOfTheYear, int& TotalPop, int& TotalFemales,
                                                        int& TotalMales, int& BreedingAttempts) {
	int NumberA = 0;
	int HowMany = 0;
	BreedingAttempts = 0;
	const unsigned sz = static_cast<int>(GetLiveArraySize(4));
	for (unsigned j = 0; j < sz; j++)
	{
		auto FS = dynamic_cast<Skylark_Female*>(SupplyAnimalPtr(4, j));
		int success = FS->Supply_BreedingSuccess();
#ifdef __HazelParry1
		 if ( success >0 ) {
           NumberA += success;
           HowMany++;
           FS->ResetBreedingSuccess();
         }
#else
		if (success != -1)
		{
			NumberA += success;
			HowMany++;
		}
#endif
		BreedingAttempts += FS->Supply_BreedingAttempts();
	}
	BreedingFemales = HowMany;
	TotalFemales = sz;
	TotalMales = static_cast<int>(GetLiveArraySize(3));
	YoungOfTheYear = TheFledgelingProbe();
	TotalPop = TotalFemales + TotalMales;
	return NumberA;
}

//-----------------------------------------------------------------------------

void Skylark_Population_Manager::BreedingPairsOutput(const int time) {
	FILE* MyFile = fopen("BreedingPairs.txt", "a");
	if (!MyFile)
	{
		g_msg->Warn(WARN_FILE, "Skylark_Population_Manager::BreedingPairsOutput():", "Cannot open file for append: BreedingPairs.txt");
		exit(0);
	}
	int no = 0;
	const int szf = static_cast<int>(GetLiveArraySize(4));
	const int szm = static_cast<int>(GetLiveArraySize(3));
	for (int j = 0; j < szf; j++)
	{
		const auto FS = dynamic_cast<Skylark_Female*>(SupplyAnimalPtr(4, j));
		if (FS->Paired)
		{
			// Below is a POM test for farm owner 50 - an identifier used to select a single experimental field or set of fields in the polyref file.
			if (g_land->SupplyFarmOwner(FS->Supply_m_Location_x(), FS->Supply_m_Location_y()) == 50)
			{
				const TTypesOfSkState behav = FS->m_CurrentSkState;
				switch (behav)
				{
				case toss_BuildingUpResources:
				case toss_PreparingForBreeding:
				case toss_MakingNest:
				case toss_Laying:
				case toss_EggHatching:
				case toss_Incubating:
				case toss_FCaringForYoung:
					no++;
				//					g_MainForm->m_aMap->Spot( 3, FS->Supply_m_Location_x(), FS->Supply_m_Location_y() );
					break;
				default: ;
				}
			}
		}
	}
	fprintf(MyFile, "%d\t%d\t%d\t%d\n", time, no, szf, szm);
	fclose(MyFile);
}

//-----------------------------------------------------------------------------
void Skylark_Population_Manager::TheAOROutputProbe() { m_AOR_Probe->DoProbe(sob_Female); }
//-----------------------------------------------------------------------------

void Skylark_Population_Manager::TheRipleysOutputProbe(FILE* a_prb) {
	Skylark_Female* FS;
	unsigned totalF = 0;
	for (unsigned j = 0; j < GetLiveArraySize(sob_Female); j++) //adult females
	{
		FS = dynamic_cast<Skylark_Female*>(SupplyAnimalPtr(4, j));
		if (FS->Paired) totalF++;
	}
	const int w = m_TheLandscape->SupplySimAreaWidth();
	const int h = m_TheLandscape->SupplySimAreaHeight();
	fprintf(a_prb, "%d %d %d %d %d\n", 0, w, 0, h, totalF);
	for (unsigned j = 0; j < GetLiveArraySize(sob_Female); j++) //adult females
	{
		FS = dynamic_cast<Skylark_Female*>(SupplyAnimalPtr(4, j));
		if (FS->Paired)
		{
			const int x = FS->Supply_m_Location_x();
			const int y = FS->Supply_m_Location_y();
			fprintf(a_prb, "%d\t%d\n", x, y);
		}
	}
	fflush(a_prb);
}

//-----------------------------------------------------------------------------

void Skylark_Population_Manager::BreedingSuccessProbeOutput(const double No, const int BreedingFemales, const int YoungOfTheYear,
                                                            const int TotalPop, const int TotalFemales, const int TotalMales, const int time,
                                                            const int BreedingAttempts) {
	FILE* MyFile = fopen("BreedingSuccess.txt", "a");
	if (!MyFile)
	{
		g_msg->Warn(WARN_FILE, "Skylark_Population_Manager::BreedingAttemptsOutput():", "Cannot open file for append: BreedingAttempts.txt");
		exit(0);
	}
	// Do some calculations
	const double floaters = (TotalPop - (YoungOfTheYear + BreedingFemales * 2)) / static_cast<double>(TotalPop - YoungOfTheYear);
	const double HatchSuccess = m_TotalNestlings / static_cast<double>(m_TotalEggs + 1);
	const double FledgeSuccess = m_TotalPrefledgelings / static_cast<double>(m_TotalNestlings + 1);
	const double BreedingSuccess = m_TotalPrefledgelings / static_cast<double>(m_TotalEggs + 1);
	// Print the results and data
	fprintf(MyFile, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%g\t%g\t%g\t%g\t%g\t\n", time, BreedingFemales, BreedingAttempts, YoungOfTheYear, TotalPop,
	        TotalFemales, TotalMales, floaters, No, HatchSuccess, FledgeSuccess, BreedingSuccess);
	//fprintf( MyFile, "%i\t%i\t%i\t%i\n", m_StriglingMort[ 0 ], m_StriglingMort[ 1 ],
	//     m_StriglingMort[ 2 ], m_StriglingMort[ 3 ] );
	for (int h = 0; h < 4; h++) m_StriglingMort[h] = 0;
	fclose(MyFile);
	// Zero any counters
	m_TotalEggs = 0;
	m_TotalNestlings = 0;
	m_TotalPrefledgelings = 0;
}

//-----------------------------------------------------------------------------

bool Skylark_Population_Manager::OpenTheBreedingPairsProbe() {
	FILE* MyFile = fopen("BreedingPairs.txt", "w");
	if (!MyFile)
	{
		g_msg->Warn(WARN_FILE, "Skylark_Population_Manager::FledgelingProbeOutput():", "Cannot open file: BreedingPairs.txt");
		exit(0);
	}
	fclose(MyFile);
	return true;
}

//-----------------------------------------------------------------------------

bool Skylark_Population_Manager::OpenTheBreedingSuccessProbe() {
	FILE* MyFile = fopen("BreedingSuccess.txt", "w");
	if (!MyFile)
	{
		g_msg->Warn(WARN_FILE, "Skylark_Population_Manager::FledgelingProbeOutput():", "Cannot open file: BreedingSuccess.txt");
		exit(0);
	}
	fclose(MyFile);
	return true;
}

//-----------------------------------------------------------------------------

void Skylark_Population_Manager::FledgelingProbeOutput(const int Total, const int time) {
	FILE* MyFile = fopen("SkFledgelingProbe.txt", "a");
	if (!MyFile)
	{
		g_msg->Warn(WARN_FILE, "Skylark_Population_Manager::FledgelingProbeOutput():", "Cannot open file for append: SkFledgelingProbe.txt");
		exit(0);
	}
	fprintf(MyFile, "%8d %6d", time, Total);
	// Now get the areas
	for (int i = 0; i < tov_Undefined+1; i++) fprintf(MyFile, " %3d %5d %9.0f", i, VegTypeFledgelings[i], m_TheLandscape->SupplyVegArea(i));
	fprintf(MyFile, "\n");
	fclose(MyFile);
	// Data for the FledgelingProbe needs clearing
	for (int i = 0; i < tov_Undefined+1; i++) { VegTypeFledgelings[i] = 0; }
}
//-----------------------------------------------------------------------------

bool Skylark_Population_Manager::OpenTheFledgelingProbe() {
	FILE* MyFile = fopen("SkFledgelingProbe.txt", "w");
	if (!MyFile) return false;
	FILE* MyFile2 = fopen("TOV_KeyForFledgelingProbe.txt", "w");
	if (!MyFile2) return false;
	fprintf(MyFile2, "This is the tov to veg-type key\n");
	const int howMany = tov_Undefined;
	for (int i = 0; i <= howMany; i++)
	{
		fprintf(MyFile2, "%d %s\n", i, m_TheLandscape->VegtypeToString(static_cast<TTypesOfVegetation>(i)).c_str());
	}
	fclose(MyFile);
	fclose(MyFile2);
	return false;
}

//-----------------------------------------------------------------------------

int Skylark_Population_Manager::SupplyNoTerritories() const { return TheSkylarkTerrs->SupplyNoTerritories(); }

int Skylark_Population_Manager::TheSkylarkTerrsSupply_x(const int r) const { return TheSkylarkTerrs->Supply_x(r); }

int Skylark_Population_Manager::TheSkylarkTerrsSupply_y(const int r) const { return TheSkylarkTerrs->Supply_y(r); }

int Skylark_Population_Manager::TheSkylarkTerrsSupply_size(const int r) const { return TheSkylarkTerrs->Supply_size(r); }

int Skylark_Population_Manager::TheSkylarkTerrsSupply_quality(const int r) const { return TheSkylarkTerrs->Supply_quality(r); }

//-----------------------------------------------------------------------------

void Skylark_Population_Manager::DoFirst(void) {
	//printf("Day %d  Terrs: %d  Occupied M/F %d/%d\n",g_date->DayInYear(),TheSkylarkTerrs->SupplyNoTerritories(), TheSkylarkTerrs->SupplyNoMaleOccupied(), TheSkylarkTerrs->SupplyNoFemaleOccupied());
	// Bad weather criteria - not used as of 24 June 2009 because removing this had no impacts on the observed POM testing.
	if (g_weather->GetRain() > 0.0 && g_weather->GetWindPeriod(g_date->Date(), 7) > 45.0 && g_weather->GetTempPeriod(g_date->Date(), 7) <
		40.0) m_IsBadWeather = true;
	else m_IsBadWeather = false;
	TheSkylarkTerrs->Tick();
	const double temp = m_TheLandscape->SupplyTemp();
	EMi = 31.2 - 0.44 * temp;
	int today = m_TheLandscape->SupplyDayInYear();
	if (today == January)
	{
		for (int h = 0; h < 4; h++) m_StriglingMort[h] = 0;
		TheSkylarkTerrs->PreProcessLandscape2(m_TheLandscape);
		// Now re-house any males that already had territories
		//ReHouse(); // Only needed if PreProcessLandsape2 is called after arrival
		// Determine this years mortality
		F_Mig_Mort = cfg_adultreturnmort.value(); //F_Mig_Mort_min + random( F_Mig_Mort_max - F_Mig_Mort_min );
		//     F_Mig_Mort=50;
		//assume that male and female mortality is equal
		M_Mig_Mort = F_Mig_Mort;
		//M_Mig_Mort = M_Mig_Mort_min + random( M_Mig_Mort_max - M_Mig_Mort_min );
		//     F_Mig_Mort=F_Mig_Mort_min+random(F_Mig_Mort_max-F_Mig_Mort_min);
		// **CJT** testing code below
		FILE* MyFile = fopen("PestEffects.txt", "a");
		fprintf(MyFile, "%d %d %d\n", m_NoChickDeaths, m_NoFledgeDeaths, m_NoPestEffects);
		fclose(MyFile);
		m_NoFledgeDeaths = 0;
		m_NoChickDeaths = 0;
		m_NoPestEffects = 0;
	}

#ifdef __SKPOM
	// Skylark POM outputs
#ifdef __KALONOSCRAPES
	/* USE FOR KALO & NO SCRAPES */
switch (today) {
		case 70:
			return ProbeReportPOM( today );
			break;
		case 80:
			return ProbeReportPOM( today );
			break;
		case 89:
			return ProbeReportPOM( today );
			break;
		case 99:
			return ProbeReportPOM( today );
			break;
		case 115:
			return ProbeReportPOM( today );
			break;
		case 123:
			return ProbeReportPOM( today );
			break;
		case 130:
			return ProbeReportPOM( today );
			break;
		case 140:
			return ProbeReportPOM( today );
			break;
		case 147:
			return ProbeReportPOM( today );
			break;
		case 153:
			return ProbeReportPOM( today );
			break;
		case 160:
			return ProbeReportPOM( today );
			break;
		case 168:
			return ProbeReportPOM( today );
			break;
		case 177:
			return ProbeReportPOM( today );
			break;
		case 185:
			return ProbeReportPOM( today );
			break;
		case 192:
			return ProbeReportPOM( today );
			break;
		case 200:
			return ProbeReportPOM( today );
			break;
		case 209:
			return ProbeReportPOM( today );
			break;
		default: ;
	}
#else
#ifdef __KALOSCRAPES
/* USE FOR KALO &  SCRAPES */
switch (today) {
		case 67:
			return ProbeReportPOM( today );
			break;
		case 70:
			return ProbeReportPOM( today );
			break;
		case 80:
			return ProbeReportPOM( today );
			break;
		case 89:
			return ProbeReportPOM( today );
			break;
		case 99:
			return ProbeReportPOM( today );
			break;
		case 113:
			return ProbeReportPOM( today );
			break;
		case 125:
			return ProbeReportPOM( today );
			break;
		case 130:
			return ProbeReportPOM( today );
			break;
		case 131:
			return ProbeReportPOM( today );
			break;
		case 140:
			return ProbeReportPOM( today );
			break;
		case 147:
			return ProbeReportPOM( today );
			break;
		case 153:
			return ProbeReportPOM( today );
			break;
		case 162:
			return ProbeReportPOM( today );
			break;
		case 169:
			return ProbeReportPOM( today );
			break;
		case 178:
			return ProbeReportPOM( today );
			break;
		case 183:
			return ProbeReportPOM( today );
			break;
		default: ;
	}
#else
	switch (today) {
		case 105:
			return ProbeReportPOM( today );
			break;
		case 119:
			return ProbeReportPOM( today );
			break;
		case 133:
			return ProbeReportPOM( today );
			break;
		case 147:
			return ProbeReportPOM( today );
			break;
		case 162:
			return ProbeReportPOM( today );
			break;
		case 177:
			return ProbeReportPOM( today );
			break;
		case 192:
			return ProbeReportPOM( today );
			break;
		case 207:
			return ProbeReportPOM( today );
			break;
		default: ;
	}
#endif

#endif

#endif
	//Sigh, everyday we need to update the territory qualities, but at least it only needs doing here!
	TheSkylarkTerrs->EvaluateAllTerritories();
}

//-----------------------------------------------------------------------------

void Skylark_Population_Manager::ProbeReportPOM(const int a_time) {
	for (int ProbeNo = 0; ProbeNo < m_NoProbes; ProbeNo++)
	{
		int No = 0;
		const unsigned Index = SupplyListIndexSize();
		for (unsigned listindex = 0; listindex < Index; listindex++)
		{
			if (TheProbe[ProbeNo]->m_TargetTypes[listindex]) No += static_cast<int>(ProbePOM(listindex, TheProbe[ProbeNo]));
		}
		TheProbe[ProbeNo]->FileOutput(No, a_time, ProbeNo);
	}
}
//-----------------------------------------------------------------------------
/**
\brief Modified probe for POM Output
*/
float Skylark_Population_Manager::ProbePOM(const int ListIndex, const Probe_Data* p_TheProbe) {
	// Counts through the list and goes through each area to see if the animal
	// is standing there and if the farm conditions are met
	float NumberSk = 0;
	if (ListIndex == 4)
	{
		if (p_TheProbe->m_NoFarms != 0)
		{
			for (unsigned j = 0; j < GetLiveArraySize(ListIndex); j++)
			{
				const unsigned Farm = SupplyAnimalPtr(ListIndex, j)->SupplyFarmOwnerRef();
				for (unsigned k = 0; k < p_TheProbe->m_NoFarms; k++)
				{
					if (p_TheProbe->m_RefFarms[k] == Farm)
					{
						const auto FS = dynamic_cast<Skylark_Female*>(SupplyAnimalPtr(ListIndex, j));
						const auto state = static_cast<TTypesOfSkState>(FS->WhatState());
						switch (state)
						{
						case toss_MakingNest:
						case toss_PreparingForBreeding:
						case toss_Laying:
						case toss_StartingNewBrood:
						case toss_EggHatching:
						case toss_Incubating:
						case toss_FCaringForYoung:
							NumberSk++; // it is in the square so increment number
						default: ;
						}
					}
				}
			}
		}
	}
	else if (ListIndex == 3)
	{
		if (p_TheProbe->m_NoFarms != 0)
		{
			for (unsigned j = 0; j < GetLiveArraySize(ListIndex); j++)
			{
				const unsigned Farm = SupplyAnimalPtr(ListIndex, j)->SupplyFarmOwnerRef();
				for (unsigned k = 0; k < p_TheProbe->m_NoFarms; k++)
				{
					if (p_TheProbe->m_RefFarms[k] == Farm)
					{
						const auto MS = dynamic_cast<Skylark_Male*>(SupplyAnimalPtr(ListIndex, j));
						if (MS->HaveTerritory) NumberSk++; // it is in the square so increment number
					}
				}
			}
		}
	}
	return NumberSk;
}
//-----------------------------------------------------------------------------

void Skylark_Population_Manager::ReHouse(void) {
	// assignment to get rid of warning
	const unsigned size = GetLiveArraySize(sob_Male);
	for (unsigned j = 0; j < size; j++)
	{
		const auto MS = dynamic_cast<Skylark_Male*>(SupplyAnimalPtr(sob_Male, j));
		if (MS->HaveTerritory) { MS->OnReHouse(); }
	}
}
//-----------------------------------------------------------------------------

void Skylark_Population_Manager::Catastrophe(void) {
	// This version simply alters populations on 1st January - it is very dangerous to
	// add individuals in many of the models so beware!!!!

	// First do we have the right day?
	const int today = m_TheLandscape->SupplyDayInYear();
	if (today != 1) return;
	// First do we have the right year?
	const int year = m_TheLandscape->SupplyYearNumber() - cfg_CatastropheEventStartYear.value();
	if (year % cfg_PmEventfrequency.value() != 0) return;

	// assignment to get rid of warning
	Skylark_Adult* AS = nullptr;
	// Now if the % decrease is higher or lower than 100 we need to do different things
	int esize = cfg_PmEventsize.value();
	if (esize < 100)
	{
		unsigned size2 = static_cast<int>(GetLiveArraySize(sob_Male));
		for (unsigned j = 0; j < size2; j++)
		{
			if (g_random_fnc(100) > esize)
			{
				const auto MS = dynamic_cast<Skylark_Male*>(SupplyAnimalPtr(3, j));
				if (MS->MyMate != nullptr) MS->MyMate->OnMateDying();
				MS->Dying(); // Kill it
			}
		}
		size2 = static_cast<int>(GetLiveArraySize(sob_Female));
		for (unsigned j = 0; j < size2; j++)
		{
			if (g_random_fnc(100) > esize)
			{
				const auto FS = dynamic_cast<Skylark_Female*>(SupplyAnimalPtr(4, j));
				if (FS->MyMate != nullptr) FS->MyMate->OnMateDying();
				FS->Dying(); // Kill it
			}
		}
	}
	else if (esize > 100)
	{
		// This is a tricky thing to do because we need to duplicate birds, but dare not mess
		// mate pointers etc up.
		// This also requires a copy method in the target birds
		// esize also needs translating  120 = 20%, 200 = 100%
		if (esize < 200)
		{
			esize -= 100;
			for (unsigned i = 3; i < 5; i++)
			{
				const unsigned size2 = static_cast<int>(GetLiveArraySize(i));
				for (unsigned j = 0; j < size2; j++)
				{
					if (g_random_fnc(100) < esize)
					{
						AS = dynamic_cast<Skylark_Adult*>(SupplyAnimalPtr(i, j));
						AS->CopyMyself(i); // Duplicate it
					}
				}
			}
		}
		else
		{
			esize -= 100;
			esize /= 100; // this will throw away fractional parts so will get 1, 2, 3  from 200, 350 400
			for (unsigned i = 3; i < 5; i++)
			{
				const unsigned size2 = static_cast<int>(GetLiveArraySize(i));
				for (unsigned j = 0; j < size2; j++)
				{
					for (int e = 0; e < esize; e++)
					{
						AS = dynamic_cast<Skylark_Adult*>(SupplyAnimalPtr(i, j));
						AS->CopyMyself(i); // Duplicate it
					}
				}
			}
		}
	}
	else return; // No change so do nothing
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                             skTTerritory
//-----------------------------------------------------------------------------
skTTerritory::skTTerritory(const int x, const int y, const int TheSize, const int TheQuality, const int a_x_div10, const int a_y_div10,
                           const int a_range_div10) {
	m_x_div10 = a_x_div10;
	m_y_div10 = a_y_div10;
	m_range_div10 = a_range_div10;
	m_Location_x = x;
	m_Location_y = y;
	Size = TheSize;
	m_VirtualDiameter = Size;
	m_Quality = TheQuality;
	Owner = nullptr;
	F_Owner = nullptr;
	m_competitionscaler = 1.0;
	m_heterogeneity = 1.0;
	m_nest_valid = false;
	m_nest_pos_validx = x;
	m_nest_pos_validy = y;
}
//-----------------------------------------------------------------------------

void skTTerritory::TestNestPossibility() {
	bool result = false;
	const auto terrsurface = new bool [m_range_div10 * m_range_div10];
	for (int i = 0; i < m_range_div10; i++) { for (int j = 0; j < m_range_div10; j++) { terrsurface[i + j * m_range_div10] = true; } }

	const int skAvoid = __SKTALLAVOIDENCE / 10;

	for (int rx = -skAvoid; rx < m_range_div10 + skAvoid; rx++)
	{
		const int rrx = rx * 10;
		for (int ry = -skAvoid; ry < m_range_div10 + skAvoid; ry++)
		{
			const int rry = ry * 10;
			for (int sx = 0; sx < 10; sx++)
			{
				for (int sy = 0; sy < 10; sy++)
				{
					const int x = m_Location_x + sx + rrx;
					const int y = m_Location_y + sy + rry;
					if (x < 0 || y < 0 || x >= g_land->SupplySimAreaWidth() || y >= g_land->SupplySimAreaHeight())
					{
					}
					else if (not_nest_friendly(x, y))
					{
						result = true;
						// No point in testing the rest of this square
						sx = 11;
						sy = 11;
					}
				}
			}
			if (result)
			{
				// Get the location in our 10x10m grid
				const int gx = rx;
				const int gy = ry;
				// Invalidate the area around this point
				int tx1 = gx + skAvoid;
				int tx2 = gx - skAvoid;
				int ty1 = gy + skAvoid;
				int ty2 = gy - skAvoid;
				if (tx1 > m_range_div10) tx1 = m_range_div10;
				if (ty1 > m_range_div10) ty1 = m_range_div10;
				if (tx2 < 0) tx2 = 0;
				if (ty2 < 0) ty2 = 0;
				for (int xx = tx2; xx < tx1; xx++) { for (int yy = ty2; yy < ty1; yy++) { terrsurface[xx + yy * m_range_div10] = false; } }
			}
			result = false;
		}
	}
	m_nest_valid = false;

	for (int i = 0; i < m_range_div10; i++)
	{
		for (int j = 0; j < m_range_div10; j++)
		{
			if (terrsurface[i + j * m_range_div10])
			{
				m_nest_valid = true;
				m_nest_pos_validx = m_Location_x + 10 * i;
				m_nest_pos_validy = m_Location_y + 10 * j;
				i = m_range_div10;
				break;
			}
		}
	}
	delete [] terrsurface;
	if (!m_nest_valid) m_Quality = 0;
}

SkTerritories::SkTerritories(Landscape* L) {
	TheLandscape = L;
	NoTerritories = 0; // must be set to zero otherwise will try to delete
	// non-existant territories on first call of PreProcessLandscape2()
	SimW = L->SupplySimAreaWidth();
	SimH = L->SupplySimAreaHeight();
	m_sim_w_div_10 = SimW / 10;
	m_sim_h_div_10 = SimH / 10;
	Grid = new bool[m_sim_w_div_10 * m_sim_h_div_10];
	m_qual_grid_signal = -1;
}

//-----------------------------------------------------------------------------

SkTerritories::~SkTerritories() {
	for (int i = 0; i < NoTerritories; i++) delete Territories[i];
	if (m_poly_seen != nullptr) free(m_poly_seen);
	if (m_poly_size != nullptr) free(m_poly_size);
	if (m_qual_cache != nullptr) free(m_qual_cache);

	for (int i = 0; i < m_sim_w_div_10 * m_sim_h_div_10; i++) { delete m_qual_grid[i]; }
	if (m_qual_grid != nullptr) free(m_qual_grid);
}

//-----------------------------------------------------------------------------

int SkTerritories::Supply_x(const int ref) const { return Territories[ref]->m_Location_x; }

//-----------------------------------------------------------------------------

int SkTerritories::SupplyNoTerritories() const { return NoTerritories; }

//-----------------------------------------------------------------------------

int SkTerritories::SupplyNoMaleOccupied() const {
	int no = 0;
	for (int i = 0; i < NoTerritories; i++) { if (Territories[i]->Owner != nullptr) no++; }
	return no;
}
//-----------------------------------------------------------------------------

int SkTerritories::SupplyNoFemaleOccupied() const {
	int no = 0;
	for (int i = 0; i < NoTerritories; i++) { if (Territories[i]->F_Owner != nullptr) no++; }
	return no;
}
//-----------------------------------------------------------------------------

skTTerritory* SkTerritories::Supply_terr(const int ref) const { return Territories[ref]; }

Skylark_Male* SkTerritories::Supply_Owner(const int ref) const { return Territories[ref]->Owner; }

//-----------------------------------------------------------------------------

Skylark_Female* SkTerritories::Supply_F_Owner(const int ref) const { return Territories[ref]->F_Owner; }

//-----------------------------------------------------------------------------

int SkTerritories::Supply_y(const int ref) const { return Territories[ref]->m_Location_y; }

//-----------------------------------------------------------------------------

int SkTerritories::Supply_size(const int ref) const { return Territories[ref]->Size; }
//-----------------------------------------------------------------------------

void SkTerritories::DumpMapGraphics(const char* a_filename, Landscape* a_map) const {
	const int l_width = a_map->SupplySimAreaWidth();
	const int l_height = a_map->SupplySimAreaHeight();
	const unsigned int linesize = l_width * 3;
	const auto frame_buffer = static_cast<unsigned char*>(malloc(sizeof(unsigned char) * linesize * l_height));

	if (frame_buffer == nullptr)
	{
		g_msg->Warn(WARN_FILE, "SkTerritories::DumpMapGraphics(): Out of memory!", "");
		exit(1);
	}

	for (int y = 0, i = 0; y < l_height; y++)
	{
		for (int x = 0; x < l_width; x++)
		{
			const int eletype = a_map->SupplyElementType(x, y);
			int localcolor = 16777215 / eletype;

			if (eletype == static_cast<int>(tole_Field))
			{
				int category;
				const double hei = a_map->SupplyVegHeight(x, y);
				if (hei > 50.0) category = 0;
				else category = static_cast<int>(200.0 - hei * 4.0);
				localcolor = category * 65536 + 65535;
			}

			frame_buffer[i++] = static_cast<unsigned char>(localcolor & 0xff);
			frame_buffer[i++] = static_cast<unsigned char>(localcolor >> 8 & 0xff);
			frame_buffer[i++] = static_cast<unsigned char>(localcolor >> 16 & 0xff);
		}
	}

	for (int i = 0; i < NoTerritories; i++)
	{
		const int l_x = Territories[i]->m_Location_x;
		const int l_y = Territories[i]->m_Location_y;
		const int l_s = Territories[i]->Size;
		// Horizontal lines of territory bounding box.
		for (int x = l_x; x < l_x + l_s; x++)
		{
			int y = l_y;
			if (x >= 0 && x < l_width && y >= 0 && y < l_height)
			{
				int pixel_p = x * 3 + linesize * y;
				frame_buffer[pixel_p++] = 200;
				frame_buffer[pixel_p++] = 0;
				frame_buffer[pixel_p] = 0;
			}
			y = l_y + l_s - 1;
			if (x >= 0 && x < l_width && y >= 0 && y < l_height)
			{
				int pixel_p = x * 3 + linesize * y;
				frame_buffer[pixel_p++] = 200;
				frame_buffer[pixel_p++] = 0;
				frame_buffer[pixel_p] = 0;
			}
		}
		for (int y = l_y; y < l_y + l_s; y++)
		{
			int x = l_x;
			if (x >= 0 && x < l_width && y >= 0 && y < l_height)
			{
				int pixel_p = x * 3 + linesize * y;
				frame_buffer[pixel_p++] = 200;
				frame_buffer[pixel_p++] = 0;
				frame_buffer[pixel_p] = 0;
			}
			x = l_x + l_s - 1;
			if (x >= 0 && x < l_width && y >= 0 && y < l_height)
			{
				int pixel_p = x * 3 + linesize * y;
				frame_buffer[pixel_p++] = 200;
				frame_buffer[pixel_p++] = 0;
				frame_buffer[pixel_p] = 0;
			}
		}
	}

	FILE* l_file = fopen(a_filename, "w");
	if (!l_file)
	{
		g_msg->Warn(WARN_FILE, "SkTerritories::DumpMapGraphics(): ""Unable to open file for writing: %s\n", a_filename);
		exit(1);
	}

	fprintf(l_file, "P6\n%d %d %d\n", l_width, l_height, 255);
	fwrite(frame_buffer, sizeof(unsigned char), linesize * l_height, l_file);
	fclose(l_file);

	free(frame_buffer);
}

extern CfgInt g_map_maxpolyref;

void SkTerritories::PreCachePoly(const int a_poly) { m_qual_cache[a_poly] = PrePoly2Qual(a_poly); }

void SkTerritories::PreFillQualGrid(void) {
	//  if ( m_qual_grid_signal == 2 )
	//    return;
	if (m_qual_grid_signal != 2)
	{
		// First time we are ever called. Allocate the memory for the
		// quality computations, and fill in the 10x10m quality grid.
		m_qual_grid_signal = 2;
		m_hash_size = g_land->SupplyLargestPolyNumUsed() + 1;
		// Using malloc because this allows us to use a test of available memory
		m_qual_grid = static_cast<SkQualGrid* *>(malloc(sizeof(SkQualGrid*) * m_sim_w_div_10 * m_sim_h_div_10));
		m_poly_seen = static_cast<int*>(malloc(sizeof(int) * m_hash_size));
		m_poly_size = static_cast<int*>(malloc(sizeof(int) * m_hash_size));
		m_qual_cache = static_cast<double*>(malloc(sizeof(double) * m_hash_size));

		bool l_ok = true;
		for (int i = 0; i < m_sim_w_div_10 * m_sim_h_div_10; i++)
		{
			m_qual_grid[i] = new SkQualGrid;
			if (m_qual_grid[i] == nullptr)
			{
				l_ok = false;
				break;
			}
		}

		if (m_qual_grid == nullptr || m_poly_seen == nullptr || m_qual_cache == nullptr || m_poly_size == nullptr || !l_ok)
		{
			g_msg->Warn(WARN_FILE, "SkTerritories::PreFillQualGrid(): ""Out of memory!\n", "");
			exit(1);
		}
	}

	for (int i = 0; i < m_hash_size; i++) { m_poly_seen[i] = -1; }

	TheLandscape->SkylarkEvaluation(this);
	int i = 0;
	for (int y = 0; y < m_sim_h_div_10; y++)
	{
		for (int x = 0; x < m_sim_w_div_10; x++) { PreEvaluateQualGrid(m_qual_grid[i++], x * 10, y * 10, 10, 10); }
	}
	//PreFillQualCache();
}

void SkTerritories::PreEvaluateQualGrid(SkQualGrid* a_grid, const int a_x, const int a_y, const int a_width, const int a_height) {
	int l_polygons = 0;
	double qual = 0;
	if (a_grid->m_polys.size() == 0)
	{
		for (int y = a_y; y < a_y + a_height; y++)
		{
			for (int x = a_x; x < a_x + a_width; x++)
			{
				const int l_poly = TheLandscape->SupplyPolyRef(x, y);
				const int l_i = m_poly_seen[l_poly];
				if (l_i >= 0)
				{
					a_grid->m_sizes[l_i]++;
					continue;
				}
				m_poly_seen[l_poly] = a_grid->Insert(l_poly);
				PolyRefData[l_polygons++] = l_poly;
			}
		}
		for (int i = 0; i < static_cast<int>(a_grid->m_polys.size()); i++)
		{
			qual += a_grid->m_sizes[i] * m_qual_cache[a_grid->m_polys[i]];
			m_poly_seen[PolyRefData[i]] = -1; // Get ready for the next square - done here because here we know how may -1s to use
		}
	}
	else
	{
		for (int i = 0; i < static_cast<int>(a_grid->m_polys.size()); i++) { qual += a_grid->m_sizes[i] * m_qual_cache[a_grid->m_polys[i]]; }
	}
	a_grid->m_qual = qual;
}

/* \brief Once only fill of quality for males */
void SkTerritories::PreFillQualCache(void) {
	/**
	This method is called only once since the male cues on returning to the landscape are thought to be
	structural and not vegatation based (e.g. he waits in ploughed fields). Later he will use the same
	assessment as the females, but this is not possible early in the season before vegetation grows.
	*/
	m_qual_cache_filled = true;

	for (int i = 0; i < m_sim_w_div_10 * m_sim_h_div_10; i++)
	{
		const int l_gmax = static_cast<int>(m_qual_grid[i]->m_polys.size());
		for (int g = 0; g < l_gmax; g++)
		{
			const int l_this_poly = m_qual_grid[i]->m_polys[g];
			if (m_poly_seen[l_this_poly] == -1)
			{
				m_poly_seen[l_this_poly] = 1;
				m_qual_cache[l_this_poly] = PrePoly2Qual(l_this_poly);
			}
		}
	}
	for (int i = 0; i < m_hash_size; i++)
	{
		m_poly_seen[i] = -1; // m_poly_seen is used elsewhere so is reset here.
	}
}

inline int SkTerritories::PreEvaluateHabitat(const int a_x, const int a_y, const int a_range_x, const int a_range_y) const {
	double l_qual = 0.0;

	int l_max_x = a_x + a_range_x;
	int l_max_y = a_y + a_range_y;
	if (l_max_x > m_sim_w_div_10) l_max_x = m_sim_w_div_10;
	if (l_max_y > m_sim_h_div_10) l_max_y = m_sim_h_div_10;

	for (int x = a_x; x < l_max_x; x++)
	{
		for (int y = a_y; y < l_max_y; y++)
		{
			const int l_i = x + y * m_sim_w_div_10;
			l_qual += m_qual_grid[l_i]->m_qual;
		}
	}
	return static_cast<int>(l_qual);
}

inline int SkTerritories::PreEvaluateHabitatStripX(const int a_x, const int a_y, const int a_range_x) const {
	double l_qual = 0.0;
	const int l_y_fixed = a_y * m_sim_w_div_10;

	int l_max_x = a_x + a_range_x;
	if (l_max_x > m_sim_w_div_10) l_max_x = m_sim_w_div_10;

	for (int x = a_x; x < l_max_x; x++)
	{
		const int l_i = x + l_y_fixed;
		l_qual += m_qual_grid[l_i]->m_qual;
		/*
			int l_gmax = (int)m_qual_grid[ l_i ]->m_polys.size();
		    for ( int g = 0; g < l_gmax; g++ ) {
		      l_qual += ( double )( m_qual_grid[ l_i ]->m_sizes[ g ] ) * m_qual_cache[ m_qual_grid[ l_i ]->m_polys[ g ]];
		    }
		*/
	}
	return static_cast<int>(l_qual);
}

inline int SkTerritories::PreEvaluateHabitatStripY(const int a_x, const int a_y, const int a_range_y) const {
	double l_qual = 0.0;

	int l_max_y = a_y + a_range_y;
	if (l_max_y > m_sim_h_div_10) l_max_y = m_sim_h_div_10;

	for (int y = a_y; y < l_max_y; y++)
	{
		const int l_i = a_x + y * m_sim_w_div_10;
		l_qual += m_qual_grid[l_i]->m_qual;
		/*
			int l_gmax = (int)m_qual_grid[ l_i ]->m_polys.size();
		    for ( int g = 0; g < l_gmax; g++ ) {
		      l_qual += ( double )( m_qual_grid[ l_i ]->m_sizes[ g ] ) * m_qual_cache[ m_qual_grid[ l_i ]->m_polys[ g ]];
		    }
		*/
	}
	return static_cast<int>(l_qual);
}

void SkTerritories::PreProcessLandscape2(Landscape* L) {
	//time_t l_time = time( NULL );
	//printf("%s\n", asctime(localtime(&l_time)));

	// if this is not the first year then delete the old territories
	for (int i = 0; i < NoTerritories; i++) delete Territories[i];
	NoTerritories = 0;

	PreFillQualGrid();

	// Zero all Grid Positions
	for (int i = 0; i < m_sim_w_div_10; i++)
		for (int j = 0; j < m_sim_h_div_10; j++) Grid[i + j * m_sim_w_div_10] = false;

	int x = g_random_fnc(m_sim_w_div_10); // was 3
	int y = g_random_fnc(m_sim_h_div_10); // was 0
	int range = 5; // Start with a 50m territory
	// Next start the loop
	int tries = m_sim_w_div_10 * m_sim_h_div_10;

	while (tries--)
	{
		const int targetqual = static_cast<int>(cfg_FemaleMinTerritoryAcceptScore.value() * (0.9 + g_rand_uni_fnc() / 5.0));
		if (!IsGridPositionValid(x, y, range))
		{
			if (++x >= m_sim_w_div_10)
			{
				x = g_random_fnc(3);
				if (++y >= m_sim_h_div_10) { y = 0; }
			}
			continue;
		}

		// Location was OK, ie. not occupied.

		// Try the smallest possible territory located right at our starting
		// position.
		int qual = PreEvaluateHabitat(x, y, range, range);

		// Is it suitable?
		if (qual >= targetqual)
		{
			// If so create it and claim the area on the grid
			// Is OK so create a territory
			if (NoTerritories > 199999)
			{
				static char errornum[20];
				sprintf(errornum, "%d", NoTerritories);
				L->Warn("SkTerritories::PreProcessLandscape2(): ""Too many territories!", errornum);
				exit(1);
			}
			Territories[NoTerritories] = new skTTerritory(x * 10, y * 10, range * 10, qual, x, y, range);
			// Here we have some variation in the quality requirements
			Territories[NoTerritories++]->TestNestPossibility();
			// Build the per-territory lists of poygons and associated
			// areas.
			PreFillTerrPolyLists(Territories[NoTerritories - 1]);

			// Claim the territory in the grid
			ClaimGrid(x, y, range);
			x += range - 1;
			range = 5;
			continue;
		}

		// Not suitable too low quality, but can increase the area
		bool done = false;
		bool success = false;
		do
		{
			if (++range > 28)
			{
				// Max. allowed territory size is 280m square.
				done = true;
			}
			else
			{
				if (IsExtGridPositionValid(x, y, range))
				{
					// Check Quality
					// No point in rechecking the area already checked so just
					// check the new area
					qual += PreEvaluateHabitatStripX(x, y + range - 1, range);
					qual += PreEvaluateHabitatStripY(x + range - 1, y, range);
					//qual= PreEvaluateHabitat( x, y, range, range );
					if (qual >= targetqual)
					{
						// If so create it and claim the area on the grid
						// Is OK so create a territory
						if (NoTerritories < 0 || NoTerritories > 199999)
						{
							static char errornum[20];
							sprintf(errornum, "%d", NoTerritories);
							L->Warn("SkTerritories::PreProcessLandscape2():"" Too many territories!", errornum);
							exit(0);
						}
						success = true;
						done = true;
						Territories[NoTerritories] = new skTTerritory(x * 10, y * 10, range * 10, qual, x, y, range);
						Territories[NoTerritories++]->TestNestPossibility();
						// Build the per-territory lists of poygons and associated
						// areas.
						PreFillTerrPolyLists(Territories[NoTerritories - 1]);
						ClaimGrid(x, y, range);
						x += range - 1;
						range = 5;
					}
				}
			}
		}
		while (!done);

		if (!success)
		{
			Grid[x + y * m_sim_w_div_10] = true;
			if (++x >= m_sim_w_div_10)
			{
				// Next line down
				x = g_random_fnc(3);
				if (++y >= m_sim_h_div_10) { y = 0; }
			}
			range = 5;
		}
	}
	if (NoTerritories < 1)
	{
		g_msg->Warn("SkTerritories::PreProcessLandscape2(): ""Not enough territories! ", NoTerritories);
		exit(1);
	}
}

void SkTerritories::PreFillTerrPolyLists(skTTerritory* a_terr) {
	int x_min_inc = a_terr->m_x_div10;
	int x_max_exc = a_terr->m_x_div10 + a_terr->m_range_div10;
	int y_min_inc = a_terr->m_y_div10;
	int y_max_exc = a_terr->m_y_div10 + a_terr->m_range_div10;
	int l_polygons = 0;

	for (int y = y_min_inc; y < y_max_exc; y++)
	{
		for (int x = x_min_inc; x < x_max_exc; x++)
		{
			const int l_i = x + y * m_sim_w_div_10;
			const int l_gmax = static_cast<int>(m_qual_grid[l_i]->m_polys.size());
			for (int g = 0; g < l_gmax; g++)
			{
				const int l_poly = m_qual_grid[l_i]->m_polys[g];
				if (m_poly_seen[l_poly] == 1)
				{
					m_poly_size[l_poly] += m_qual_grid[l_i]->m_sizes[g];
					continue;
				}
				m_poly_seen[l_poly] = 1;
				m_poly_size[l_poly] = m_qual_grid[l_i]->m_sizes[g];
				PolyRefData[l_polygons++] = l_poly;
			}
		}
	}

	a_terr->m_polys.resize(l_polygons);
	a_terr->m_sizes.resize(l_polygons);
	for (int i = 0; i < l_polygons; i++)
	{
		const int l_poly = PolyRefData[i];
		m_poly_seen[l_poly] = -1;
		a_terr->m_polys[i] = l_poly;
		a_terr->m_sizes[i] = m_poly_size[l_poly];
	}

	// Now build the same lists for the home range,
	// centered on this territory.
	const int l_offset = a_terr->m_range_div10 >> 1;
	x_min_inc = a_terr->m_x_div10 + l_offset - HomeRangeDiameterDiv20;
	x_max_exc = a_terr->m_x_div10 + l_offset + HomeRangeDiameterDiv20;
	y_min_inc = a_terr->m_y_div10 + l_offset - HomeRangeDiameterDiv20;
	y_max_exc = a_terr->m_y_div10 + l_offset + HomeRangeDiameterDiv20;

	l_polygons = 0;

	const int l_c = PreMakeForIterator(x_min_inc, x_max_exc, m_for_iter_x, m_sim_w_div_10);
	PreMakeForIterator(y_min_inc, y_max_exc, m_for_iter_y, m_sim_h_div_10);

	for (int l_y = 0; l_y < l_c; l_y++)
	{
		const int y = m_for_iter_y[l_y];
		for (int l_x = 0; l_x < l_c; l_x++)
		{
			const int l_i = m_for_iter_x[l_x] + y * m_sim_w_div_10;
			const int l_gmax = static_cast<int>(m_qual_grid[l_i]->m_polys.size());
			for (int g = 0; g < l_gmax; g++)
			{
				const int l_poly = m_qual_grid[l_i]->m_polys[g];
				if (m_poly_seen[l_poly] == 1)
				{
					m_poly_size[l_poly] += m_qual_grid[l_i]->m_sizes[g];
					continue;
				}
				m_poly_seen[l_poly] = 1;
				m_poly_size[l_poly] = m_qual_grid[l_i]->m_sizes[g];
				PolyRefData[l_polygons++] = l_poly;
			}
		}
	}

	a_terr->m_hr_polys.resize(l_polygons);
	a_terr->m_hr_sizes.resize(l_polygons);
	for (int i = 0; i < l_polygons; i++)
	{
		const int l_poly = PolyRefData[i];
		m_poly_seen[l_poly] = -1;
		a_terr->m_hr_polys[i] = l_poly;
		a_terr->m_hr_sizes[i] = m_poly_size[l_poly];
	}
}

int SkTerritories::PreMakeForIterator(const int a_min_incl, const int a_max_excl, int* a_iter, const int a_norm_max_excl) {
	int i, index = 0;
	int med_max;
	int high_max = -1;

	if (a_max_excl >= a_norm_max_excl)
	{
		med_max = a_norm_max_excl;
		high_max = a_max_excl;
	}
	else { med_max = a_max_excl; }

	for (i = a_min_incl; i < 0; i++) { a_iter[index++] = i + a_norm_max_excl; }
	for (; i < med_max; i++) { a_iter[index++] = i; }
	for (; i < high_max; i++) { a_iter[index++] = i - a_norm_max_excl; }
	return index;
}

void SkTerritories::EvaluateAllTerritories(void) { for (int i = 0; i < NoTerritories; i++) { EvaluateHabitatN(Territories[i]); } }

double SkTerritories::EvaluateHabitatN(skTTerritory* a_terr) {
	//a_good_polys = 0;
	int a_area_polys = 0;
	const int l_polygons = static_cast<int>(a_terr->m_polys.size());
	double l_qual = 0.0;

	if (l_polygons > 2500)
	{
		g_land->Warn("SkTerritories::EvaluateHabitat1(): - Polygons > 2500", "");
		exit(1);
	}

	for (int i = 0; i < l_polygons; i++)
	{
		const int l_poly = a_terr->m_polys[i];
		l_qual += static_cast<double>(a_terr->m_sizes[i]) * PrePolyNQual(l_poly, nullptr);
		//l_qual += ( double )( a_terr->m_sizes[ i ] ) * PrePoly2Qual( l_poly );
		a_area_polys += a_terr->m_sizes[i];
	}

	l_qual *= a_terr->m_competitionscaler; //* a_terr->GetHeterogeneity());
	// Update territory quality
	a_terr->SetQuality(static_cast<int>(l_qual));
	// return the quality to calling method
	return l_qual;
}

bool SkTerritories::IsGridPositionValid(const int& a_x, const int& a_y, const int a_range) const {
	const int l_max_x = a_x + a_range;
	if (l_max_x > m_sim_w_div_10)
	{
		//l_max_x = m_sim_w_div_10;
		return false;
	}

	const int l_max_y = a_y + a_range;
	if (l_max_y > m_sim_h_div_10)
	{
		//l_max_y = m_sim_h_div_10;
		return false;
	}

	for (int i = a_x; i < l_max_x; i++) { for (int j = a_y; j < l_max_y; j++) { if (Grid[i + j * m_sim_w_div_10]) { return false; } } }
	// No occupied squares found so
	return true;
}

bool SkTerritories::IsExtGridPositionValid(const int& x, const int& y, const int range) const {
	// The commented out code **CJT** below could be used if we want
	// to invalidate the whole area up to the point where we hit problems
	// However, this will lead to large territory-less holes in the
	// landscape

	const int l_max_x = x + range;
	if (l_max_x > m_sim_w_div_10)
	{
		// l_max_x = m_sim_w_div_10;
		return false;
	}

	const int l_max_y = y + range;
	if (l_max_y > m_sim_h_div_10)
	{
		//l_max_y = m_sim_h_div_10;
		return false;
	}

	//Needs only to check the left most column and bottom row
	// The leftmost column
	for (int j = y; j < l_max_y; j++) { if (Grid[x + j * m_sim_w_div_10]) { return false; } }
	// The bottom row
	for (int i = x; i < l_max_x; i++) { if (Grid[i + y * m_sim_w_div_10]) { return false; } }
	// All OK
	return true;
}

void SkTerritories::ClaimGrid(const int x, const int y, const int range) const {
	int l_max_x = x + range;
	if (l_max_x > m_sim_w_div_10) l_max_x = m_sim_w_div_10;

	int l_max_y = y + range;
	if (l_max_y > m_sim_h_div_10) l_max_y = m_sim_h_div_10;

	for (int i = x; i < l_max_x; i++) { for (int j = y; j < l_max_y; j++) { Grid[i + j * m_sim_w_div_10] = true; } }
}

//------------------------------------------------------------------------------

int SkTerritories::IsValid(const int nx, const int ny) const {
	int found = -1;
	for (int i = 0; i < NoTerritories; i++)
	{
		if (Territories[i]->m_Location_x <= nx && Territories[i]->m_Location_x + Territories[i]->Size > nx && Territories[i]->m_Location_y <= ny
			&& Territories[i]->m_Location_y + Territories[i]->Size > ny)
		{
			found = i;
			break;
		}
	}
	return found;
}

//-----------------------------------------------------------------------------

void SkTerritories::GetTerritoriesByDistance(const int nx, const int ny, vector<APoint>* alist) const {
	//	alist->resize(NoTerritories);
	APoint p;
	for (int i = 0; i < NoTerritories; i++)
	{
		int dist = abs(Territories[i]->m_Location_x - nx);
		dist += abs(Territories[i]->m_Location_y - ny);
		p.m_x = i;
		p.m_y = dist;
		alist->push_back(p);
	}
	sort(alist->begin(), alist->end(), CompareDist);
}
//-----------------------------------------------------------------------------

int SkTerritories::Supply_quality(const int ref) const { return Territories[ref]->GetQuality(); }

//-----------------------------------------------------------------------------

void SkTerritories::Occupy(const int ref, Skylark_Male* Male) const { Territories[ref]->Owner = Male; }

//-----------------------------------------------------------------------------

void SkTerritories::FemaleOccupy(const int ref, Skylark_Female* Female) const { Territories[ref]->F_Owner = Female; }

//-----------------------------------------------------------------------------

void SkTerritories::RemoveFemale(const int ref) const { Territories[ref]->F_Owner = nullptr; }

//-----------------------------------------------------------------------------

void SkTerritories::RemoveMale(const int ref) const { if (ref != -1) Territories[ref]->Owner = nullptr; }

//-----------------------------------------------------------------------------

void SkTerritories::Split(const int ref) {
	/** Duplicates the old territory.\n */
	Territories[NoTerritories++] = new skTTerritory(Territories[ref]->m_Location_x, Territories[ref]->m_Location_y, Territories[ref]->Size,
	                                                Territories[ref]->GetQuality(), Territories[ref]->m_x_div10, Territories[ref]->m_y_div10,
	                                                Territories[ref]->m_range_div10);
	PreFillTerrPolyLists(Territories[NoTerritories - 1]);
	Territories[NoTerritories - 1]->SetNestPossibility(Territories[ref]->m_nest_valid, Territories[ref]->m_nest_pos_validx,
	                                                   Territories[ref]->m_nest_pos_validy);
	Territories[ref]->m_competitionscaler *= cfg_MaleSplitScale.value();
	Territories[NoTerritories - 1]->m_competitionscaler *= cfg_MaleSplitScale.value();
	/** Reduces the virtual size. */
	const double Vdir = Territories[ref]->GetVirtualDiameter() / 1.41;
	// 1.41 is the theoretical diameter scale when dividing one territory into two
	Territories[ref]->SetVirtualDiameter(Vdir);
	Territories[NoTerritories - 1]->SetVirtualDiameter(Vdir);
	//printf("New %d\t%d\n",g_land->SupplyDayInYear(),NoTerritories);
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                       Skylark_Base
//-----------------------------------------------------------------------------

Skylark_Base::Skylark_Base(const int x, const int y, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, const int bx,
                           const int by, const int mh) : TAnimal(x, y) {
	m_OurTerritories = Terrs;
	m_OurPopulationManager = SPM;
	// Make sure that the skylark starts in the correct state
	m_CurrentSkState = toss_Initiation;
	m_CurrentStateNo = 0;
	m_Born_x = bx;
	m_Born_y = by;
	m_MyHome = mh;
#ifdef __PESTICIDE_RA
	m_pesticide_accumulation = 0;
#endif
}
//-----------------------------------------------------------------------------

void Skylark_Base::ReInit(const int x, const int y, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, const int bx,
                          const int by, const int mh) {
	m_Location_x = x;
	m_Location_y = y;
	m_AgeDays = 0;
	m_StepDone = false;
	// Assign the pointer to the population manager
	m_OurTerritories = Terrs;
	m_OurPopulationManager = SPM;
	// Make sure that the skylark starts in the correct state
	m_CurrentSkState = toss_Initiation;
	m_CurrentStateNo = 0;
	m_Born_x = bx;
	m_Born_y = by;
	m_MyHome = mh;
#ifdef __PESTICIDE_RA
	m_pesticide_accumulation = 0;
#endif
	}
//---------------------------------------------------------------------------------------------------------------------------------

bool Skylark_Base::DailyMortality(const int mort) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: Must be called once each day and if true then death
	//returns true if the object must die
	const int ch = g_random_fnc(10000);
	if (ch < mort) return true;
	return false;
}

//-----------------------------------------------------------------------------

//**CJT** Must change for de-bugged code in Population Manager - but it is never
// used in skylark at the moment
bool Skylark_Base::InSquare(const rectangle p_R) const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_Location_x >= static_cast<int>(p_R.m_x1) && m_Location_x < static_cast<int>(p_R.m_x2) && m_Location_y >= static_cast<int>(p_R.m_y1) &&
		m_Location_y < static_cast<int>(p_R.m_y2)) return true;
	return false;
}

//-----------------------------------------------------------------------------
//                      Skylark_Clutch
//-----------------------------------------------------------------------------

Skylark_Clutch::Skylark_Clutch(Skylark_Female* Mum, SkTerritories* Terrs, Landscape* L, const int NoEggs, const int x, const int y,
                               const int m_MyHome, Skylark_Population_Manager* SPM) : Skylark_Base(x, y, Terrs, L, SPM, x, y, m_MyHome) {
	Age = 0;
	Clutch_Size = NoEggs;
	MinDegrees = 0;
	Mother = Mum;
	m_baddays = 0;
}
//------------------------------------------------------------------------------

void Skylark_Clutch::ReInit(Skylark_Female* Mum, SkTerritories* Terrs, Landscape* L, const int NoEggs, const int x, const int y,
                            const int a_MyHome, Skylark_Population_Manager* SPM) {
	Skylark_Base::ReInit(x, y, Terrs, L, SPM, x, y, a_MyHome);
	Age = 0;
	Clutch_Size = NoEggs;
	MinDegrees = 0;
	Mother = Mum;
	m_baddays = 0;
}
//------------------------------------------------------------------------------

bool Skylark_Clutch::OnFarmEvent(FarmToDo event) {
	switch (event)
	{
	case sleep_all_day:
		break;
// Here begins soil cultivation and cutting mortality
	case autumn_plough:
	case stubble_plough:
	case stubble_cultivator_heavy:
	case heavy_cultivator_aggregate:
	case autumn_harrow:
	case preseeding_cultivator:
	case preseeding_cultivator_sow:
	case autumn_roll:
	case autumn_sow:
	case autumn_sow_with_ferti:
	case winter_plough:
	case deep_ploughing:
	case spring_plough:
	case spring_harrow:
	case shallow_harrow:
	case spring_roll:
	case spring_sow:
	case spring_sow_with_ferti:
    case fp_liquidNH3:
    case fp_greenmanure:
    case row_cultivation:
    case hilling_up:
    case harvest:
	case green_harvest:
	case harvestshoots:
    case harvest_bushfruit:
    case cut_to_hay:
    case cut_to_silage:
    case straw_chopping:
    case hay_turning:
    case autumn_or_spring_plough:
    case burn_straw_stubble:
    case burn_top: 
	case flammebehandling:
    case mow:
	case bed_forming:
	case flower_cutting:
	case bulb_harvest:
	case pruning:
	case shredding:
	case winter_harrow:
	case suckering:
	case summer_harrow:
	case summer_plough:
	case summer_sow:
#ifdef TEST_ISSUE_DEATH_WARRANT
      printf( "Skylark_Clutch::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
		m_CurrentSkState = toss_CDying;
		break;
// Here begins spray mortality
	case fp_npks:
	case fp_npk:
	case fp_pk:
	case fp_k:
	case fp_p:
	case fp_ammoniumsulphate:
	case fp_manganesesulphate:
	case fp_manure:
	case fp_sludge:
	case fp_rsm:
	case fp_calcium:
	case fp_cu:
	case fp_n:
	case fp_nc:
	case fp_nk:
	case fp_ns:
	case fp_pks:
	case fp_sk:
	case fp_boron:
	case fa_npks:
    case fa_npk:
    case fa_n: 
	case fa_boron:
	case fa_cu:
	case fa_nk:
	case fa_pks:
	case fa_pk:
	case fa_k:
	case fa_p:
    case fa_sk:
    case fa_ammoniumsulphate:
	case fa_manganesesulphate:
	case fa_manure:
	case fa_greenmanure:
	case fa_sludge:
	case fa_rsm:
	case fa_calcium:
	case herbicide_treat:
	case growth_regulator:
	case fungicide_treat:
	case insecticide_treat:
	case org_insecticide:
	case trial_insecticidetreat:
	case syninsecticide_treat:
	case org_fungicide:
	case org_herbicide:
		if (g_random_fnc(1000) < cfg_insecticide_direct_mortE.value())
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Female::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_CDying;
		}
		break;
	case fp_slurry:
	case fa_slurry:
	case swathing: /*\todo why are these three different from other spray/cultivation effects? */
		if (g_random_fnc(100) < 2)
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Clutch::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_CDying;
		}
		break;
// Here begins strigling mortality
	case strigling_sow:
	case strigling_hill:
	case strigling:
#ifndef __NoStriglingEffect
		if (g_random_fnc(100) < cfg_strigling_clutch.value())
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Clutch::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_CDying;
			AddStriglingMort(1);
		}
#endif
		break;
// Here begins grazing mortality
	case cattle_out:
		if (m_OurLandscape->SupplyGrazingPressure(m_Location_x, m_Location_y) > 0)
			if (g_random_fnc(1000) < 20)
			{
				// was 20
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_Clutch::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
				m_CurrentSkState = toss_CDying;
			}
		break;
	case cattle_out_low:
		if (m_OurLandscape->SupplyGrazingPressure(m_Location_x, m_Location_y) > 0)
			if (g_random_fnc(1000) < 5)
			{
				//was 5
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_Clutch::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
				m_CurrentSkState = toss_CDying;
			}
		break;
	case pigs_out:
#ifndef __NoPigsOutEffect
		if (g_random_fnc(100) < 20)
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Clutch::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_CDying;
		}
#endif
		break;
//Here begins non-mortality events
		/*\todo should these be reassessed? */
	case product_treat:
	case glyphosate:
	case biocide:
	case molluscicide:
	case last_treatment:
	case pheromone:
	case water:
	case hay_bailing:
	case stubble_harrowing:
	case cut_weeds:
	case manual_weeding:
	case straw_covering:
	case straw_removal:
	case fiber_removal:
	case fiber_covering:
	case trial_control:
	case trial_toxiccontrol:
		break;
	default:
		g_land->Warn("Skylark_Clutch::OnFarmEvent(): Unknown event type:", m_OurLandscape->EventtypeToString(event));
		exit(1);
	}
	if (m_CurrentSkState == toss_CDying) return true;
	return false;
}

//------------------------------------------------------------------------------

void Skylark_Clutch::OnMumGone(void) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE )
    DEADCODEError();
#endif
	// the clutch cannot survive so go directly to die and do not pass go
	Mother = nullptr;
#ifdef TEST_ISSUE_DEATH_WARRANT
  printf( "Skylark_Clutch::OnMumGone() %d\n", ( int )g_date->Date() );
#endif
	m_CurrentSkState = toss_Destroy; // will kill it at end of step!
	m_CurrentStateNo = -1;
}

//------------------------------------------------------------------------------

void Skylark_Clutch::BeginStep(void) { CheckManagement(); }

//------------------------------------------------------------------------------

void Skylark_Clutch::Step(void) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_StepDone || m_CurrentSkState == toss_Destroy) return;
#ifdef __CJTDebug_5
  if ( Mother ) {
    if ( Mother->SupplyMyClutch() != this ) {
      g_land->Warn( "Skylark_Step::Step(): Debug_5 ", "" );
      exit( 1 );
    }
  }
#endif
	switch (m_CurrentSkState)
	{
	case toss_Initiation: // Initial state. Will wait here until told by female that she is incubatig
		m_StepDone = true;
		m_CurrentSkState = toss_Initiation;
		break;
	case toss_Developing:
		m_StepDone = true;
		break;
	case toss_Hatching:
		m_StepDone = true;
		break;
	case toss_CDying:
		m_StepDone = true;
		break;
	default:
		g_land->Warn("Skylark_CLutch::Step(): Unknown State ", "");
		exit(1);
	}
}

//------------------------------------------------------------------------------

void Skylark_Clutch::EndStep(void) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_CurrentSkState == toss_Destroy) return;
	switch (m_CurrentSkState)
	{
	case toss_Initiation:
		break;
	case toss_Developing: // Develop
		switch (st_Developing())
		{
		case 2:
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_Clutch::EndStep() st_Developing() %d\n", ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_CDying; // die
			break;
		case 1:
			m_CurrentSkState = toss_Hatching; // hatch
			break;
		case 0:
			break;
		}
		break;
	case toss_Hatching: // Hatching
		switch (st_Hatching())
		{
		case 0:
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_Clutch::EndStep() st_Hatching() %d\n", ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_CDying;
			break;
		case 1: // Successful hatch
			m_CurrentSkState = toss_Destroy; // will kill it at end of step!
			m_CurrentStateNo = -1;
			m_StepDone = true; // otherwise it will loop forever
			break;
		}
		break;
	case toss_CDying: // Dying
		st_Dying();
		m_StepDone = true;
		break;
	default:
		g_msg->Warn("Skylark_Clutch::EndStep(): Unknown state: ", m_CurrentSkState);
		exit(0);
		break;
	}
}

//------------------------------------------------------------------------------
int Skylark_Clutch::st_Developing() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (DailyMortality(ClutchMortProb))
	{
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1001 );
#endif
		return 2; //transfer to state die
	}
	if (Age > 0) // No development on the first day when just laid
	{
		// Timing : This function is called once at the end of each day before
		// the next day
		// Develop according to ambient temperature, and how long the female
		// is incubating for
		const int incubated_time = Mother->Supply_NestTime(); // this includes night
		const int off_nest_time = 24 * 60 - incubated_time;
		//double Rain = m_OurLandscape->SupplyRain();
		//int Daylength = m_OurLandscape->SupplyDaylength();

		//m_OurPopulationManager->WriteToTest2File(m_OurLandscape->SupplyDayInYear(),off_nest_time);
		// they start at EggTemp degrees and cool to 'cool'
		const double cool_rate_pmin = Cooling_Rate_Eggs / 60.0;
		double Temp = m_OurLandscape->SupplyTemp(); // 9.97 = COrrection for day/night temps in �tjylland
		if (Temp >= EggTemp) Temp = EggTemp - 0.000001;

		// If off_nest_time > daylength/2 then the birds are spending more time off the nest than on.
		// in this case the trip length must go up
		//double TripNumber=cfg_FoodTripsPerDay.value();
		const double TripLength = cfg_sk_triplength.value();
		/*
			if ( off_nest_time >= ( Daylength - 10 ) ) { // -10 allows us not to get a division by zero later
				if ( ++m_baddays > 5 ) {
					#ifdef __SKPOM
							m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1003 );
					#endif
					return 2; //TransferToState(Die);
				}
			} else {
				  if ( off_nest_time > ( Daylength / 2.0 ) ) {
					// getting cold, so assume a different strategy or maximising the number of on nest times of 10 minutes
					int on_nest = Daylength - off_nest_time;
					TripNumber = on_nest / 10.0;
					TripLength = off_nest_time / TripNumber;
				  }
				  else {
						TripLength = 10;
				  }
		    }
			*/
		//
		// We have the time incubated - but this includes re-warming time. So make
		// the assumption that warming/cooling are linear and equal rates
		// Now calculate the minimum temperature
		//    double cool = EggTemp - ( ( cool_rate_pmin * TripLength ) * ( ( EggTemp - Temp ) / 2.0 ) );
		double cool = EggTemp - cool_rate_pmin * TripLength * ((EggTemp - Temp) / 2.0);
		// This cannot be lower than ambient (There is a problem here since ambient
		// temperature when the eggs are not incubated is higher than the mean used
		// here because foraging is in the daytime
		if (cool < Temp) cool = Temp;
		// Now calculate how many minutes were spent cooling down to this temp (if
		// cool is higher than Temp then cooltime should be the same as TripLength
		// Next line to prevent overflow
		const double cooltime = (EggTemp - cool) / (cool_rate_pmin * ((EggTemp - Temp) / 2.0));
		// Eggs will spend incubated time-cooltime at EggTemp, off_nest_time-cooltime
		// at cool, and 2xcooltime at the mean of EggTemp and cool
		const double AverageOffNestTemp = (cool + EggTemp) / 2.0;
		/* if ( Rain > 5 ) { MinDegrees += ( ( 24 * 60 ) - Daylength ) * ( EggTemp - MD_Threshold ); if ( m_baddays++ > 5 ) {
		m_OurPopulationManager->WriteToTest2File( m_OurLandscape->SupplyDayInYear(), 1009 );
		return 2; //TransferToState(Die); } } else */
		{
			MinDegrees += static_cast<int>((incubated_time - cooltime * 2.0) * (EggTemp - MD_Threshold)); // took *2 away
			double offnesttemp = cool - MD_Threshold;
			if (offnesttemp < 0) offnesttemp = 0; // No development, rather than minus
			MinDegrees += static_cast<int>((off_nest_time - cooltime) * offnesttemp);
			double transitiontemp = static_cast<int>(AverageOffNestTemp) - MD_Threshold;
			if (transitiontemp < 0) transitiontemp = 0; // No development, rather than minus
			MinDegrees += static_cast<int>(2.0 * cooltime * transitiontemp); // was 1.0 - this change assumes immediate re-heating
		}
		/* double scalingfactor = 1.0; int Daylength = m_OurLandscape->SupplyDaylength( );
		int daytimeonnest=Daylength-off_nest_time; double Rain = m_OurLandscape->SupplyRain(); if (Rain >10) {
		MinDegrees+=(24*60)-Daylength; if (m_baddays++>5) {
		m_OurPopulationManager->WriteToTest2File( m_OurLandscape->SupplyDayInYear(), 1009 ); return 2; //TransferToState(Die); } }
		else { if ((daytimeonnest * scalingfactor) >= off_nest_time) { MinDegrees+=24*60;  // No problem, got all day } else {
		if (daytimeonnest<=0) { MinDegrees+=incubated_time; if (m_baddays++>5) {
		m_OurPopulationManager->WriteToTest2File( m_OurLandscape->SupplyDayInYear(), 1009 ); return 2; //TransferToState(Die);
		}; // This counter just keeps going up until death } else { // Here we have used some time being cold, but not too much
		int uncompensatedtime=off_nest_time-(daytimeonnest*scalingfactor);
		MinDegrees+=(24*60)-uncompensatedtime;  // Only part of the day } } } */
	}

	// Have we achieved enough minute degrees
	if (MinDegrees > MD_Hatch)
	{
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1(m_OurLandscape->SupplyDayInYear(),Age);
#endif
		return 1; //TransferToState(Hatch);
	} // if they reach the age of 18 they die if not hatched
	if (Age == 18)
	{
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1002 );
#endif
		return 2; //TransferToState(Die);
	}
	Age++;
	return 0;
}

//------------------------------------------------------------------------------

int Skylark_Clutch::st_Hatching() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: Occurs at the end of a day (e.g. midnight-1 minute)
	// Must create Clutch_Size number of Skylark_Hatchling Objects
	// They tell Mother that they have been created when they exist in the system

	if (Mother->Paired)
	{
		const auto Ch = new Nestling_struct;
		Ch->Dad = Mother->MyMate; // Needs a pointer to dad not mum
		Ch->x = m_Location_x;
		Ch->y = m_Location_y;
		Ch->L = m_OurLandscape;
		Ch->Terrs = m_OurTerritories;
		Ch->bx = m_Born_x;
		Ch->by = m_Born_y;
		Ch->mh = m_MyHome;
#ifdef __CJTDebug_7
		if (Clutch_Size == 0)
		{
			g_land->Warn("Skylark_Clutch::st_Hatching(): DeBug7 ", nullptr);
			exit(1);
		}
#endif
		m_OurPopulationManager->CreateObjects(1, this, nullptr, Ch, Clutch_Size);
		// object destroyed by m_OurPopulationManager
		delete Ch;
		Mother->OnEggsHatch(); // Sets in motion feeding and she tells male
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------

void Skylark_Clutch::st_Dying() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: Zero time
	// Only called before object is destroyed
	if (Mother) Mother->OnClutchDeath();
	m_CurrentSkState = toss_Destroy; // will kill it at end of step!
	m_CurrentStateNo = -1;
}

//------------------------------------------------------------------------------
//               Skylark_Nestling
//------------------------------------------------------------------------------

Skylark_Nestling::Skylark_Nestling(const int x, const int y, Skylark_Male* Daddy, Landscape* L, SkTerritories* Terrs,
                                   Skylark_Population_Manager* SPM, const int bx, const int by, const int mh) : Skylark_Base(
	x, y, Terrs, L, SPM, bx, by, mh) {
	m_Dad = Daddy;
	//Growth = 0;
	// Must choose sex
	if (g_random_fnc(2) == 1) Sex = true;
	else Sex = false; // M or F
	m_Size = MeanHatchingWeight + (g_random_fnc(25) - 11) * MeanHatchingWeight / 1000.0;
	Age = 1;
	m_EM_fail = 0;
	// From Skylarks Energy Calcs Mar05.xls
	m_EM = 0.3058 * m_Size + 0.5221;
	m_GrNeed = cfg_PEmax.value(); // No of Kcals possible to use for growth on day 1
	m_NestLeavingChance = cfg_NestLeavingChance.value();
}
//------------------------------------------------------------------------------

void Skylark_Nestling::ReInit(const int x, const int y, Skylark_Male* Daddy, Landscape* L, SkTerritories* Terrs, Skylark_Population_Manager* SPM,
                              const int bx, const int by, const int mh) {
	Skylark_Base::ReInit(x, y, Terrs, L, SPM, bx, by, mh);
	m_Dad = Daddy;
	//Growth = 0;
	// Must choose sex
	if (g_random_fnc(2) == 1) Sex = true;
	else Sex = false; // M or F
	m_Size = MeanHatchingWeight + (g_random_fnc(25) - 11) * MeanHatchingWeight / 1000.0;
	Age = 1;
	m_EM_fail = 0;
	// From Skylarks Energy Calcs Mar05.xls
	m_EM = 0.3058 * m_Size + 0.5221;
	m_GrNeed = cfg_PEmax.value(); // No of Kcals possible to use for growth on day 1
	m_NestLeavingChance = cfg_NestLeavingChance.value();
}

//------------------------------------------------------------------------------
bool Skylark_Nestling::OnFarmEvent(FarmToDo event) {
	switch (event)
	{
	case sleep_all_day:
		break;
// Here begins non-mortality events
	case fp_npks:
	case fp_npk:
	case fp_pk:
	case fp_k:
	case fp_p:
	case fp_ammoniumsulphate:
	case fp_manganesesulphate:
	case fp_manure:
	case fp_greenmanure:
	case fp_sludge:
	case fp_rsm:
	case fp_calcium:
	case fp_boron:
	case fp_cu:
	case fp_n:
	case fp_nc:
	case fp_nk:
	case fp_ns:
	case fp_pks:
	case fp_sk:
	case fa_npk:
	case fa_npks:
    case fa_n:
    case fa_pk:
	case fa_k:
	case fa_p:
	case fa_ammoniumsulphate:
	case fa_manganesesulphate:
	case fa_manure:
	case fa_sludge:
	case fa_rsm:
	case fa_calcium:
	case fa_boron:
	case fa_cu:
	case fa_nk:
	case fa_pks:
	case fa_sk:
    case herbicide_treat:
    case growth_regulator:
    case fungicide_treat:
    case insecticide_treat:
    case trial_insecticidetreat:
	case trial_control:
	case trial_toxiccontrol:
    case molluscicide:
    case water:
    case hay_bailing:
    case cut_weeds:
    case manual_weeding:
	case straw_covering:
	case fiber_covering:
    case fiber_removal: 
	case straw_removal:
	case product_treat:
	case glyphosate:
	case biocide:
	case org_insecticide:
	case last_treatment:
	case org_fungicide:
	case org_herbicide:
	case pheromone:
	case suckering:
		break;
// Here begins soil cultivation and cutting mortality
	case stubble_plough:
	case stubble_cultivator_heavy:
	case heavy_cultivator_aggregate:
	case autumn_harrow:
	case preseeding_cultivator:
	case preseeding_cultivator_sow:
	case autumn_roll:
	case autumn_sow:
	case autumn_sow_with_ferti:
	case burn_top:
	case flammebehandling:
	case winter_plough:
	case deep_ploughing:
	case spring_plough:
	case spring_harrow:
	case green_harvest:
	case harvestshoots:
	case shallow_harrow:
	case pruning:
	case shredding:
	case summer_harrow:
	case summer_plough:
	case summer_sow:
	case winter_harrow:
	case spring_roll:
	case spring_sow:
	case spring_sow_with_ferti:
    case autumn_plough:
    case fp_liquidNH3:
    case fa_greenmanure:
    case row_cultivation:
    case hilling_up:
    case harvest:
    case harvest_bushfruit:
    case cut_to_hay:
    case cut_to_silage:
    case straw_chopping:
    case hay_turning:
    case stubble_harrowing:
    case autumn_or_spring_plough:
    case burn_straw_stubble:
    case mow:
	case bed_forming:
	case flower_cutting:
	case bulb_harvest:
#ifdef TEST_ISSUE_DEATH_WARRANT
      printf( "Skylark_Nestling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
		m_CurrentSkState = toss_NDying;
		break;
/*\todo what is the special case with the below three? */
	case fp_slurry:
	case fa_slurry:
	case swathing:
		if (g_random_fnc(100) < 2)
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Nestling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_NDying;
		}
		break;
// Here begins spray mortality
	case syninsecticide_treat:
		if (g_random_fnc(1000) < cfg_insecticide_direct_mortN.value())
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Nestling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_NDying;
		}
		break;
// Here begins strigling mortality
	case strigling:
	case strigling_sow:
	case strigling_hill:
#ifndef __NoStriglingEffect
		if (g_random_fnc(100) < cfg_strigling_nestling.value())
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Nestling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_NDying;
			AddStriglingMort(2);
		}
#endif
		break;
// Here begins grazing mortality
	case cattle_out:
		if (m_OurLandscape->SupplyGrazingPressure(m_Location_x, m_Location_y) > 0)
			if (g_random_fnc(1000) < 20)
			{
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_Nestling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
				m_CurrentSkState = toss_NDying;
			}
		break;
	case cattle_out_low:
		if (m_OurLandscape->SupplyGrazingPressure(m_Location_x, m_Location_y) > 0)
			if (g_random_fnc(1000) < 5)
			{
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_Nestling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
				m_CurrentSkState = toss_NDying;
			}
		break;
	case pigs_out:
#ifndef __NoPigsOutEffect
		if (g_random_fnc(100) < 20)
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Nestling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_NDying;
		}
#endif
		break;
	default:
		g_land->Warn("Skylark_Nestling::OnFarmEvent(): ""Unknown event type:", m_OurLandscape->EventtypeToString(event));
		exit(1);
	}
	if (m_CurrentSkState == toss_NDying) return true;
	return false;
}

//------------------------------------------------------------------------------

void Skylark_Nestling::BeginStep(void) {
	// If you are not dead already find out if you are killed today by agriculture
	if (m_CurrentStateNo != -1) CheckManagement();
}

//------------------------------------------------------------------------------

void Skylark_Nestling::Step(void) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_StepDone) return;
	switch (m_CurrentSkState)
	{
	case toss_Initiation: // Initial
		m_CurrentSkState = toss_NDeveloping;
		m_StepDone = true;
		break;
	case toss_Destroy:
		m_StepDone = true;
		break;
	case toss_NDeveloping: // Develop
		m_StepDone = true;
		break;
	case toss_NDying: // Dying
		m_StepDone = true;
		break;
	default:
		g_land->Warn("Skylark_Nestling::Step(): Unknown State ", nullptr);
		exit(1);
	}
}

//------------------------------------------------------------------------------

void Skylark_Nestling::EndStep(void) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_CurrentSkState == toss_Destroy) return;
#ifdef __CJTDebug_5
  if ( Dad )
    if ( !Dad->DoIExistN( this ) ) {
      g_land->Warn( "Skylark_Nestling::EndStep - Do I ExistN ", NULL ); exit( 1 );
    }
#endif
	switch (m_CurrentSkState)
	{
	case toss_NDeveloping: // Develop
		switch (st_Developing())
		{
		case 3:
			if (m_Dad != nullptr) m_Dad->OnNestPredatation(); // Kill the whole brood
			break;
		case 2:
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_Nestling::EndStep() : st_Developing() %d\n", ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_NDying; // die
			break;
		case 1:
			// Maturing must be called here because it happens the same day as develop
			st_Maturing(); // Do Maturing
			m_CurrentSkState = toss_Destroy;
			m_CurrentStateNo = -1;
			m_StepDone = true;
			break;
		}
		break;
	case toss_NDying: // Dying
		st_Dying();
		m_StepDone = true;
		break;
	default:
		/* char errornum[20]; sprintf(errornum, "%d", m_CurrentSkState );
		g_land->Warn("Skylark_Nestling::EndStep(): Unknown state: ", errornum); exit(0); */
		break;
	}
#ifdef __PESTICIDE_RA
	PesticideResponse();
#endif
}

//------------------------------------------------------------------------------

int Skylark_Nestling::st_Developing() {
	// On entry GrNeed is set to the remaining food that could have been eaten
	// today
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing of this function call is at the last moment of each day
	if (DailyMortality(NestlingMortProb))
	{
		return 3; //send brood eaten to Dad, will call OnYouHaveBeenEaten for all nestlings
	}
	// EM will be 0 if enough food has been given
	/*
	if ( m_EM > 0 )
	  {
		  if ( m_EM_fail > 2 )
		  {
	#ifdef __SKPOM
			  m_OurPopulationManager->WriteSKPOM2( m_OurLandscape->SupplyDayInYear(), 2002 );
	#endif
			  return 2; //TransferToState(Die) Did not get EM for 2 days
		  }
	  } else
	  {
		  m_EM_fail = 0; // got EM so unset flag
	  }
	*/
	if (Age >= 6)
	{
		if (m_Size >= NestLeavingWeight)
		{
			if (g_random_fnc(100) < m_NestLeavingChance)
			{
#ifdef __SKPOM
			  m_OurPopulationManager->WriteSKPOM2( m_OurLandscape->SupplyDayInYear(), Age );
#endif
				return 1;
			}
			m_NestLeavingChance *= 4;
		}
	}
	if (Age == 11)
	{
#ifdef __SKPOM
	  m_OurPopulationManager->WriteSKPOM2( m_OurLandscape->SupplyDayInYear(), 2004 );
#endif
		return 2; // Taken too long so die
	}
	Age++; // age
	// From Skylarks Energy Calcs Mar05.xls
	if (Age < 6) m_EM = 0.31 * m_Size + 0.52;
	else m_EM = 0.24 * m_Size + 6.83;
	// Below is the correction for thermoregulation (Below 4days this is really a
	// female cost - but it is subtracted here instead).
	// Assumes that 50% of the time (night) that the chicks are covered and at 36)
	//  EM += EM_nest_T[ Age ] * ( 36 - ( ( 0.5 * m_OurLandscape->SupplyTemp() ) + 18 ) );
	m_EM += EM_nest_T[Age] * (36 - m_OurLandscape->SupplyTemp());
	m_GrNeed = cfg_PEmax.value();
	return 0; // still growing and not dead
}
//------------------------------------------------------------------------------

void Skylark_Nestling::st_Maturing() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (!m_Dad) return; // If Dad is gone then something went wrong and we died
	// Timing: Occurs at 1 minute to midnight
	// When created they must update dads pointer
	const auto fps = new PreFledgeling_struct;
	fps->x = m_Location_x;
	fps->y = m_Location_y;
	fps->size = m_Size;
	fps->Dad = m_Dad;
	fps->age = Age;
	fps->L = m_OurLandscape;
	fps->Terrs = m_OurTerritories;
	fps->sex = Sex;
	fps->bx = m_Born_x;
	fps->by = m_Born_y;
	fps->mh = m_MyHome;
	m_OurPopulationManager->CreateObjects(2, this, nullptr, fps, 1);
	delete fps;
	// m_OurPopulationManager will destroy object
}

//------------------------------------------------------------------------------

void Skylark_Nestling::OnDadDead() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Dad is dead - so must not send him a message
	// So forget who he is (messaging to Dad must be checked for NULL on sending)
	m_Dad = nullptr;
}

//------------------------------------------------------------------------------

void Skylark_Nestling::OnYouHaveBeenEaten() {
	// Nobody to tell, nothing to do but vanish as soon as possible
	m_CurrentSkState = toss_Destroy;
	m_CurrentStateNo = -1;
#ifdef __SKPOM
  m_OurPopulationManager->WriteSKPOM2(m_OurLandscape->SupplyDayInYear(),2001);
#endif
}

//------------------------------------------------------------------------------

void Skylark_Nestling::OnDeserted() {
	// Nobody to tell, nothing to do but vanish as soon as possible
	m_CurrentSkState = toss_Destroy;
	m_CurrentStateNo = -1;
#ifdef __SKPOM
  m_OurPopulationManager->WriteSKPOM2(m_OurLandscape->SupplyDayInYear(),2005);
#endif
}

//------------------------------------------------------------------------------

double Skylark_Nestling::On_FoodSupply(double a_food) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif

	if (m_EM > 0) // Must maintain this
	{
		if (a_food > m_EM)
		{
			a_food -= m_EM;
			m_EM = 0;
		}
		else
		{
			m_EM -= a_food;
			a_food = 0;
		}
	}
	if (m_GrNeed > a_food)
	{
		m_GrNeed -= a_food;
		// Does the growing here
		m_Size += a_food * CE_nest[Age]; // Conv Eff is g dw insect to g ww bird
		a_food = 0;
	}
	else
	{
		// Does the growing here at the maximum allowed rate
		m_Size += m_GrNeed * CE_nest[Age]; // Conv Eff is g dw insect to g ww bird
		a_food -= m_GrNeed;
		m_GrNeed = 0;
	}
	return a_food;
}

//------------------------------------------------------------------------------

void Skylark_Nestling::st_Dying() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: Zero time
	// Only called before object is destroyed
	// Checks that Dad exists
	if (m_Dad != nullptr) m_Dad->OnNestlingDeath(this);
	m_CurrentSkState = toss_Destroy; // will kill it at end of step!
	m_CurrentStateNo = -1;
}

//------------------------------------------------------------------------------
//                       Skylark_PreFledgeling
//------------------------------------------------------------------------------

Skylark_PreFledgeling::Skylark_PreFledgeling(const int x, const int y, Landscape* L, SkTerritories* Terrs, Skylark_Male* Daddy, const bool sex,
                                             const double size, const int age, Skylark_Population_Manager* SPM, const int bx, const int by,
                                             const int mh) : Skylark_Nestling(x, y, Daddy, L, Terrs, SPM, bx, by, mh) {
	Sex = sex; // must be reset because the nestling constructor can change it
	m_Size = size;
	Age = age;
	m_EM = 0;
	m_GrNeed = 0;
	m_CurrentSkState = toss_PDeveloping; // must develop today
}
//------------------------------------------------------------------------------

void Skylark_PreFledgeling::ReInit(const int x, const int y, Landscape* L, SkTerritories* Terrs, Skylark_Male* Daddy, const bool sex,
                                   const double size, const int age, Skylark_Population_Manager* SPM, const int bx, const int by, const int mh) {
	Skylark_Nestling::ReInit(x, y, Daddy, L, Terrs, SPM, bx, by, mh);
	Sex = sex; // must be reset because the nestling constructor can change it
	m_Size = size;
	Age = age;
	m_EM = 0;
	m_GrNeed = 0;
	m_CurrentSkState = toss_PDeveloping; // must develop today
}
//------------------------------------------------------------------------------

bool Skylark_PreFledgeling::OnFarmEvent(FarmToDo event) {
	switch (event)
	{
	case sleep_all_day:
		break;
// Here begins non-mortality events
	case fp_npks:
	case fp_npk:
	case fp_pk:
	case fp_k:
	case fp_sk:
	case fp_p:
	case fp_slurry:
	case fp_ammoniumsulphate:
	case fp_manganesesulphate:
	case fp_manure:
	case fp_greenmanure:
	case fp_sludge:
	case fp_boron:
	case fp_cu:
	case fp_n:
	case fp_nc:
	case fp_nk:
	case fp_ns:
	case fp_pks:
	case fp_rsm:
	case fp_calcium:
	case fa_npks:
    case fa_npk:
    case fa_n: 
    case fa_pk:
	case fa_k:
	case fa_boron:
	case fa_cu:
	case fa_nk:
	case fa_pks:
	case fa_sk:
	case fa_p:
	case fa_slurry:
	case fa_ammoniumsulphate:
	case fa_manganesesulphate:
	case fa_manure:
	case fa_greenmanure:
	case fa_sludge:
	case fa_rsm:
	case fa_calcium:
    case herbicide_treat:
    case growth_regulator:
    case fungicide_treat:
    case insecticide_treat:
    case trial_insecticidetreat:
    case molluscicide:
    case water:
	case straw_covering:
	case straw_removal:
	case fiber_covering:
	case fiber_removal:
	case fp_liquidNH3:
	case manual_weeding:
	case product_treat:
	case glyphosate:
	case biocide:
	case org_insecticide:
	case last_treatment:
	case org_fungicide:
	case org_herbicide:
	case pheromone:
	case suckering:
	case trial_control:
	case trial_toxiccontrol:
/*\todo should the below not have an effect? */
    case swathing:
	case row_cultivation:
    case harvest:
	case green_harvest:
	case harvestshoots:
    case harvest_bushfruit:
    case cattle_out:
    case cattle_out_low:
    case hay_bailing:
    case mow:
    case cut_weeds:
	case pruning:
    case pigs_out:
		break;
// Here begins soil cultivation and cutting mortality
	case stubble_plough:
	case stubble_cultivator_heavy:
	case heavy_cultivator_aggregate:
	case autumn_harrow:
	case preseeding_cultivator:
	case preseeding_cultivator_sow:
	case autumn_roll:
	case autumn_sow:
	case autumn_sow_with_ferti:
	case winter_plough:
	case deep_ploughing:
	case spring_plough:
	case spring_harrow:
	case shallow_harrow:
	case spring_roll:
	case winter_harrow:
	case summer_harrow:
	case summer_plough:
	case summer_sow:
	case spring_sow:
	case spring_sow_with_ferti:
	case autumn_plough:
	case hilling_up:
	case cut_to_hay:
	case cut_to_silage:
	case straw_chopping:
	case hay_turning:
	case stubble_harrowing:
	case autumn_or_spring_plough:
	case burn_straw_stubble:
	case burn_top:
	case flammebehandling:
	case shredding:
	case bed_forming:
	case flower_cutting:
	case bulb_harvest:
		if (Age <= 20)
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_PreFledgeling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_PDying;
		}
		break;
// Here begins spray mortality
	case syninsecticide_treat:
		if (g_random_fnc(1000) < cfg_insecticide_direct_mortP.value())
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_PreFledgeling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_PDying;
		}
		break;
// Here begins strigling mortality
	case strigling_sow:
	case strigling:
	case strigling_hill:
#ifndef __NoStriglingEffect
		if (Age <= 20 && g_random_fnc(100) < 50)
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_PreFledgeling::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_PDying;
			AddStriglingMort(3);
		}
#endif
		break;
	default:
		g_land->Warn("Skylark_PreFledgeling::OnFarmEvent(): ""Unknown event type:", m_OurLandscape->EventtypeToString(event));
		exit(1);
	}
	if (m_CurrentSkState == toss_PDying) return true;
	return false;
}

//------------------------------------------------------------------------------

void Skylark_PreFledgeling::BeginStep(void) { CheckManagement(); }

//------------------------------------------------------------------------------

void Skylark_PreFledgeling::Step() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_StepDone) return;
	switch (m_CurrentSkState)
	{
	case toss_Initiation: // Initial
		// Should never access this code becuase it should go
		// to m_CurrentSkState=toss_PDeveloping;
		g_land->Warn("Skylark_PreFeldgeling::Step(): Ilegal ", nullptr);
		exit(1);
	case toss_PDeveloping: // Develop
		m_StepDone = true;
		break;
	case toss_PMaturing: // Maturing
		m_StepDone = true;
		break;
	case toss_PDying: // Dying
		m_StepDone = true;
		break;
	case toss_Destroy:
		m_StepDone = true;
		break;
	default:
		g_land->Warn("Skylark_PreFeldgeling::Step(): Unknown State ", nullptr);
		exit(1);
	}
}
//------------------------------------------------------------------------------

void Skylark_PreFledgeling::EndStep() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_CurrentSkState == toss_Destroy) return;
#ifdef __CJTDebug_5
  if ( Dad )
    if ( !Dad->DoIExistP( this ) ) {
      g_land->Warn( "Skylark_PreFeldgeling::EndStep(): DeBug5 ", NULL );
      exit( 1 );
    }
#endif
	switch (m_CurrentSkState)
	{
	case toss_PDeveloping: // Develop
		switch (st_Developing())
		{
		case 2:
#ifdef TEST_ISSUE_DEATH_WARRANT
          printf( "Skylark_PreFledgeling::EndStep() : st_Developing() : %d\n", ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_PDying; // -> die
			break;
		case 1:
			m_CurrentSkState = toss_PMaturing; // -> Maturing
		// This state must be completed now so make the call from here to stop
		// it being made the next day
			st_Maturing();
			m_CurrentSkState = toss_Destroy; // will kill it at end of step!
			m_CurrentStateNo = -1;
			m_StepDone = true;
			break;
		}
		break;
	case 8: // Maturing
		g_land->Warn("Skylark_PreFledling::EndStep(): Maturing ", nullptr);
		exit(1);
	case 9: // Dying
		st_Dying();
		m_StepDone = true;
		break;
	default:
		/* char errornum[20]; sprintf(errornum, "%d", m_CurrentSkState );
		g_land->Warn("Skylark_PreFledgeling::EndStep(): Unknown state: ", errornum); exit(0); */
		break;
	}
	// Need to find out if it is poisoned
#ifdef __PESTICIDE_RA
	PesticideResponse();
#endif
}

//------------------------------------------------------------------------------

void Skylark_PreFledgeling::st_Dying() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_Dad != nullptr) m_Dad->OnPreFledgelingDeath(this);
	m_CurrentSkState = toss_Destroy; // will kill it at end of step!
	m_CurrentStateNo = -1;
}

//------------------------------------------------------------------------------

int Skylark_PreFledgeling::st_Developing() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: called at the end of each day
	if (DailyMortality(PreFledgeMortProb))
	{
		//m_OurPopulationManager->WriteToTest2File(m_OurLandscape->SupplyDayInYear(),3001);
		return 2; //transfer to state die
	}
	if (Age++ == 31)
	{
		//m_OurPopulationManager->WriteToTest2File((int)3000, 3000);
		m_Size = 38.0; // Give it a size
		return 1; // Next stage
	}
	// Get Next days EM
	m_EM = 0; // m_OurPopulationManager->SupplyEMi();   // KCal
	m_GrNeed = 0; // gdw
	return 0; // still developing
}

//------------------------------------------------------------------------------

double Skylark_PreFledgeling::GetFood() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Food it obtained from the local area
	// This will depend upon the habitat quality/weather & searching efficiency
	// This is possibly too complicated just now so:
	// First prototype model will assume that for the last 10 days the birds
	// can find 10% of their need for each day after 20 days so at 30 days they
	// are completely independent and cannot starve

	// 1st prototype
	double food;
	if (Age >= 20) { food = m_OurPopulationManager->SupplyEMi() * 0.1 * (Age - 20); }
	else food = 0;
	return food;
}

//------------------------------------------------------------------------------

double Skylark_PreFledgeling::GetFledgelingEM(int /*Age*/) const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Speed optimisation - this is the same for every bird so done in PopMan
	return m_OurPopulationManager->SupplyEMi(); // in gdw needed
}

//------------------------------------------------------------------------------

void Skylark_PreFledgeling::st_Maturing() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: occurs at the end of the day
	// Mortality after this time, until they return is taken up in the
	// immigration mortality (total 35%)
	const auto aps = new AdultSkylark_struct;
	aps->x = m_Location_x;
	aps->y = m_Location_y;
	aps->size = m_Size;
	aps->Terrs = m_OurTerritories;
	aps->L = m_OurLandscape;
	aps->sex = Sex;
	aps->age = 0;
	aps->bx = m_Born_x;
	aps->mh = m_MyHome;
	aps->by = m_Born_y;
	if (Sex == true) m_OurPopulationManager->CreateObjects(3, this, nullptr, aps, 1);
	else m_OurPopulationManager->CreateObjects(4, this, nullptr, aps, 1);
	// object will be destroyed by death state
	// but must let Dad know anyway
	delete aps;
	if (m_Dad != nullptr) m_Dad->OnPreFledgelingMature(this);
	//   m_OurPopulationManager->WriteToTest2File(3,3);
}

//-----------------------------------------------------------------------------
//                             Skylark_Adult
//-----------------------------------------------------------------------------

Skylark_Adult::Skylark_Adult(const int x, const int y, const double size, const int age, SkTerritories* Terrs, Landscape* L,
                             Skylark_Population_Manager* SPM, const int bx, const int by, const int mh) : Skylark_Base(
	x, y, Terrs, L, SPM, bx, by, mh) {
	m_Size = size;
	Age = age;
	BSuccess = false;
	MyExtractEff = (0.90 + g_random_fnc(21) / 100.0) * MeanExtractionRatePerMinute;
	GoodWeather = 0;
	Paired = false;
	MyTerritory.ref = -1;
	m_aTerrlist = new vector<APoint>;
	m_pesticide_affected = -1;
}
//-----------------------------------------------------------------------------
void Skylark_Adult::ReInit(const int x, const int y, const double size, const int age, SkTerritories* Terrs, Landscape* L,
                           Skylark_Population_Manager* SPM, const int bx, const int by, const int mh) {
	Skylark_Base::ReInit(x, y, Terrs, L, SPM, bx, by, mh);
	m_Size = size;
	Age = age;
	BSuccess = false;
	MyExtractEff = (0.90 + g_random_fnc(21) / 100.0) * MeanExtractionRatePerMinute;
	GoodWeather = 0;
	Paired = false;
	MyTerritory.ref = -1;
	m_aTerrlist = new vector<APoint>;
	m_pesticide_affected = -1;
}
//-----------------------------------------------------------------------------

Skylark_Adult::~Skylark_Adult() { delete m_aTerrlist; }
//-----------------------------------------------------------------------------

/** \brief Extreme weather conditions check */
bool Skylark_Adult::GetBadWeather() {
	//return m_OurPopulationManager->IsBadWeather();
	return false;
}
//-----------------------------------------------------------------------------

double Skylark_Adult::RemoveEM(double food) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// remove metabolic requirements
	const double used = m_OurPopulationManager->SupplyEMi();
#ifdef __PESTICIDE_RA_ADULT
	// If using pesticide risk assessment then the total mg of pesticide collected that day are now in m_pcide
	// But, there well be more than we wanted, so now we take a mean as mg/kcal resource
	m_pcide_conc = m_pcide / food;
	// Note there is an assumption here that they only eat insects. We know this to be incorrect so depending upon the
	// aim of the assessment the line below may need to be modified (e.g. by scaling by the relative calorific content of
	// insects compared to plant matter.
	m_pesticide_accumulation = m_pesticide_accumulation + m_pcide_conc * used;
#endif
	food -= used;
	if (food < 0) food = 0;
	return food; // Food still in gdw insects
}

//------------------------------------------------------------------------------

double Skylark_Adult::GetWeatherHindrance() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	int rain = static_cast<int>(m_OurLandscape->SupplyRain()) / 2;
	// Assume half of this falls at night
	// Make sure the indices remain in bounds
	if (rain > 10) rain = 10;
	int temp = static_cast<int>(floor(0.5 + m_OurLandscape->SupplyTemp()));
	if (temp < 0) temp = 0;
	if (temp > 20) temp = 20;
	double hind = RainHindrance[rain];
	hind *= TempHindrance[temp];
	return hind;
}

//------------------------------------------------------------------------------

double Skylark_Adult::GetVegHindrance(const int polyref) const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	/* CJT Changed in favour of a function of veg density double veg_biomass = m_OurLandscape->SupplyVegBiomass(polyref);
	double veg_height = m_OurLandscape->SupplyVegHeight(polyref); int bio = (int)floor(0.5+veg_biomass/(VeryHighDensityVeg/
	double(HindranceBiomassCategories))); // 0-24, 25-49, 50-74,74-100 categories
	int hei=(int)floor(0.5+veg_height/(100.0/double(HindranceHeightCategories)));
	if (hei>HindranceHeightCategories-1) hei=HindranceHeightCategories-1;
	if (bio>HindranceBiomassCategories-1) bio=HindranceBiomassCategories-1; return VegHindrance[hei][bio]; */
	if (m_OurLandscape->SupplyVegPatchy(polyref)) return 1.0;
	const int veg_height = static_cast<int>(floor(0.5 + m_OurLandscape->SupplyVegHeight(polyref)));
	if (veg_height < 26) return 1.0;
	const int veg_dens = m_OurLandscape->SupplyVegDensity(polyref);
	//    double vh = ( VegHindranceH[ veg_height ] * 2 + VegHindranceD[ veg_dens ] ) / 3.0;
	//double vh = (0.33*( DensityScore[ veg_dens ] + 2 * HeightScore[ ( int )veg_height ] ) * 0.33333333)+0.67;
	double vh = (VegHindranceD[veg_dens] + 2 * VegHindranceH[veg_height]) * 0.3333;
	//cout<<veg_dens<<" "<<veg_height<<" "<<vh<<endl;
#ifdef __TRAMLINES_ON
	// Tramlines affect the amount of veg hindrance only
	// vh must be increased by a factor corresponding to the tramline effect
	// IF USING SKLARK SCRAPES !
	if (m_OurLandscape->SupplySkScrapes(polyref)) { vh += (1.0 - vh) * cfg_tramline_foraging.value(); }
	else if (m_OurLandscape->SupplyHasTramlines(polyref)) { vh += (1.0 - vh) * cfg_tramline_foraging.value(); }
#endif
	return vh;
}

//------------------------------------------------------------------------------
//                          Skylark_Male
//------------------------------------------------------------------------------

Skylark_Male::Skylark_Male(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx,
                           int by, int mh) : Skylark_Adult(x, y, size, age, Terrs, L, SPM, bx, by, mh) {
	/**
	* The Male constructor. This initialises some important parameters.
	*/
	GoodWeather = 0;
	HaveTerritory = false;
	MyMate = nullptr;
	m_BroodSize = 0; // don't start life with chicks!
	BSuccess = false;
	m_firstPF = false;
	/** There is some individual variation around the territory acceptance score, up to 10% max */
	m_MyMinTerritoryQual = static_cast<int>(cfg_FemaleMinTerritoryAcceptScore.value() * (g_rand_uni_fnc() / 10.0 + 1.0)); //= 1.0 to 1.1
	m_XFNestAcceptScore = m_MyMinTerritoryQual * (g_rand_uni_fnc() + 2.1);
	for (int i = 0; i < 6; i++) m_Brood[i] = nullptr;
}
//------------------------------------------------------------------------------

void Skylark_Male::ReInit(const int x, const int y, const double size, const int age, SkTerritories* Terrs, Landscape* L,
                          Skylark_Population_Manager* SPM, const int bx, const int by, const int mh) {
	Skylark_Adult::ReInit(x, y, size, age, Terrs, L, SPM, bx, by, mh);
	/**
	* The Male constructor. This initialises some important parameters.
	*/
	GoodWeather = 0;
	HaveTerritory = false;
	MyMate = nullptr;
	m_BroodSize = 0; // don't start life with chicks!
	BSuccess = false;
	m_firstPF = false;
	/** There is some individual variation around the territory acceptance score, up to 10% max */
	m_MyMinTerritoryQual = static_cast<int>(cfg_FemaleMinTerritoryAcceptScore.value() * (g_rand_uni_fnc() / 10.0 + 1.0)); //= 1.0 to 1.1
	m_XFNestAcceptScore = m_MyMinTerritoryQual * (g_rand_uni_fnc() + 2.1);
	for (int i = 0; i < 6; i++) m_Brood[i] = nullptr;
}
//------------------------------------------------------------------------------

Skylark_Male::~Skylark_Male() {
}

//------------------------------------------------------------------------------
bool Skylark_Male::OnFarmEvent(FarmToDo event) {
	switch (event)
	{
	case sleep_all_day:
		break;
	case autumn_or_spring_plough:
	case autumn_plough:
	case autumn_roll:
	case autumn_sow:
	case autumn_sow_with_ferti:
	case bed_forming:
	case biocide:
	case bulb_harvest:
	case burn_straw_stubble:
	case burn_top:
	case cattle_out:
	case cattle_out_low:
	case cut_to_hay:
	case cut_to_silage:
	case cut_weeds:
	case deep_ploughing:
	case fa_ammoniumsulphate:
	case fa_boron:
	case fa_calcium:
	case fa_cu:
	case fa_greenmanure:
	case fa_k:
	case fa_manganesesulphate:
	case fa_manure:
	case fa_n:
	case fa_nk:
	case fa_npk:
	case fa_npks:
	case fa_p:
	case fa_pk:
	case fa_pks:
	case fa_rsm:
	case fa_sk:
	case fa_sludge:
	case fa_slurry:
	case fiber_covering:
	case fiber_removal:
	case flammebehandling:
	case flower_cutting:
	case fp_ammoniumsulphate:
	case fp_boron:
	case fp_calcium:
	case fp_cu:
	case fp_greenmanure:
	case fp_k:
	case fp_liquidNH3:
	case fp_manganesesulphate:
	case fp_manure:
	case fp_n:
	case fp_nc:
	case fp_nk:
	case fp_npk:
	case fp_npks:
	case fp_ns:
	case fp_p:
	case fp_pk:
	case fp_pks:
	case fp_rsm:
	case fp_sk:
	case fp_sludge:
	case fp_slurry:
	case fungicide_treat:
	case glyphosate:
	case green_harvest:
	case growth_regulator:
	case harvest:
	case harvest_bushfruit:
	case harvestshoots:
	case hay_bailing:
	case hay_turning:
	case heavy_cultivator_aggregate:
	case herbicide_treat:
	case hilling_up:
	case insecticide_treat:
	case last_treatment:
	case manual_weeding:
	case molluscicide:
	case mow:
	case org_fungicide:
	case org_herbicide:
	case org_insecticide:
	case pheromone:
	case pigs_out:
	case preseeding_cultivator:
	case preseeding_cultivator_sow:
	case product_treat:
	case pruning:
	case row_cultivation:
	case shallow_harrow:
	case shredding:
	case spring_harrow:
	case spring_plough:
	case spring_roll:
	case spring_sow:
	case spring_sow_with_ferti:
	case straw_chopping:
	case straw_covering:
	case straw_removal:
	case strigling:
	case strigling_hill:
	case strigling_sow:
	case stubble_cultivator_heavy:
	case stubble_harrowing:
	case stubble_plough:
	case suckering:
	case summer_harrow:
	case summer_plough:
	case summer_sow:
	case swathing:
	case trial_control:
	case trial_insecticidetreat:
	case trial_toxiccontrol:
	case water:
	case winter_harrow:
	case winter_plough:
	case autumn_harrow:
		break;
// Here begins spray mortality
	case syninsecticide_treat:
		if (g_random_fnc(1000) < cfg_insecticide_direct_mortM.value())
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Male::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_MDying;
		}
		break;
	default:
		g_land->Warn("Skylark_Male::OnFarmEvent(): ""Unknown event type:", m_OurLandscape->EventtypeToString(event));
		exit(1);
	}
	if (m_CurrentSkState == toss_MDying) return true;
	return false;
}

//------------------------------------------------------------------------------

void Skylark_Male::BeginStep() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif

	if (m_CurrentSkState == toss_Destroy) return;
	CheckManagement();

	switch (m_CurrentSkState)
	{
	case toss_MFloating: // Floating
		m_CurrentSkState = st_Floating(); // Legal returns are toss_MFlocking, toss_MFindingTerritory, toss_MTempLeavingArea
		m_StepDone = true;
		break;
	case toss_MCaringForYoung: // CaringForYoung
		if (st_CaringForYoung()) m_CurrentSkState = toss_ScaringOffChicks; //ScareOffChicks
		m_StepDone = true;
	case toss_AttractingAMate:
	case toss_ScaringOffChicks:
	case toss_FollowingMate:
		ReEvaluateTerritory();
		break;
	default:
		/* char errornum[20]; sprintf(errornum, "%d", m_CurrentSkState );
		g_land->Warn("Skylark_Male::BeginStep(): Unknown state: ", errornum); exit(0); */
		break;
	}
}

//------------------------------------------------------------------------------

void Skylark_Male::Step() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_StepDone || m_CurrentSkState == toss_Destroy) return;
	switch (m_CurrentSkState)
	{
	case toss_Initiation: // Initial
		m_CurrentSkState = toss_MFlocking;
		break;
	case toss_MFlocking: // Flocking
		switch (st_Flocking())
		{
		case 3:
			m_CurrentSkState = toss_MFindingTerritory; // -> find terr.
			break;
		case 2:
			m_CurrentSkState = toss_MTempLeavingArea; // -> temp.leave
			break;
		case 1:
			// use 0,0 as a default location
			m_Location_x = 0;
			m_Location_y = 0;
			m_CurrentSkState = toss_MEmigrating; // -> Emigration
			break;
		case 0:
			m_StepDone = true;
		}
		break;
	case toss_MArriving: // Arriving
		if (st_Arriving()) m_CurrentSkState = toss_MFindingTerritory; // -> find terr.
		else { m_CurrentSkState = toss_MTempLeavingArea; }
		m_StepDone = true;
		break;
	case toss_MImmigrating: // Immigration  - is instant
		if (st_Immigrating()) m_CurrentSkState = toss_MArriving; // -> arrival
		else
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Male::Step() : st_Immigrating() : %d\n", ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_MDying; // -> die
		}
		break;
	case toss_MEmigrating: // Emigration
		if (st_Emigrating() == 1) m_CurrentSkState = toss_MImmigrating; // -> immigr.
		m_StepDone = true;
		break;
	case toss_MTempLeavingArea: // Temp.leave.area
		if (st_TempLeavingArea()) m_CurrentSkState = toss_MArriving; // -> arrival
		m_StepDone = true;
		break;
	case toss_MFindingTerritory: // FindingTerritory
		switch (st_FindingTerritory())
		{
		case 1:
			// Must do this now in case someone else gets this territory
			EstablishingATerritory();
			m_CurrentSkState = toss_AttractingAMate; // -> attract mate
			break;
		case 2:
			HaveTerritory = false;
			m_CurrentSkState = toss_MFloating; // -> floating
		//m_CurrentSkState = toss_MTempLeavingArea;
			m_StepDone = true;
			break;
		}
		break;
	case toss_AttractingAMate: // Attract mate
		if (st_AttractingAMate())
		{
			m_CurrentSkState = toss_MFlocking; // -> flocking
		}
		m_StepDone = true;
		break;
	case toss_FollowingMate: // FollowingMate
		st_FollowingMate();
		m_StepDone = true;
		break;
	case toss_ScaringOffChicks: // ScaringOffChicks - instant
		if (st_ScaringOffChicks()) m_CurrentSkState = toss_FollowingMate; // -> follow mate
		break;
	case toss_MDying: // Dying
		st_Dying();
		m_StepDone = true;
		break;
	default:
		const string st = m_OurPopulationManager->SupplyStateNames(m_CurrentSkState);
		g_land->Warn("Skylark_Male::Step: Unknown State", st.c_str());
		exit(0);
		break;
	}
	if (m_BroodSize) BroodAge = m_Brood[0]->Age;
	else BroodAge = -1;
}
//------------------------------------------------------------------------------

void Skylark_Male::EndStep() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_CurrentSkState == toss_Destroy) return;
#ifdef __PESTICIDE_RA
	PesticideResponse();
#endif
}

//------------------------------------------------------------------------------

void Skylark_Male::st_Dying() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (MyMate)
	{
		if (MyMate->MyMate != this)
		{
			g_land->Warn("Skylark_Male::st_Dying: DeBug1 ", nullptr);
			exit(1);
		}
		MyMate->OnMateDying();
	}
	if (MyTerritory.ref != -1) m_OurTerritories->RemoveMale(MyTerritory.ref);
	MyTerritory.ref = -1; // No territory
	HaveTerritory = false;
	MyMate = nullptr;
	Paired = false;
	if (m_BroodSize > 0) { for (int i = 0; i < m_BroodSize; i++) { m_Brood[i]->OnDadDead(); } }
	m_CurrentStateNo = -1;
	m_CurrentSkState = toss_Destroy;
}

//------------------------------------------------------------------------------

int Skylark_Male::st_Flocking() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	/**
	* The bird is assumed to move around the area waiting until weather conditions are suitable to begin Emigration, depending upon the
	* time of year. Only arrives here after breeding, so poor conditions will trigger a transition to Emigration.
	* A transition to Emigration will also occur if the bird has not emigrated before October.<br>
	*/
#ifdef FOR_DEMONSTRATION
  // TODO if we want it to look nice on the screen then the birds must flock
  // this will not affect the rest of the simulation  - just look better
  // Can we be bothered????
  // Just for now set the location to 0,0
  m_Location_x = 0;
  m_Location_y = 0;
#endif

	if (m_OurLandscape->SupplyDayInYear() > October)
	{
		GoodWeather = 0;
		return 1; //TransferToState(Emigration);
	}
	const bool bad = GetBadWeather();
	if (bad && m_OurLandscape->SupplyDayInYear() >= September) return 1; //TransferToState(Emigration);
	return 0; // Carry on
}
//------------------------------------------------------------------------------

bool Skylark_Male::st_Arriving() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (GetBadWeather()) return false; // TransferToState(Flocking)
	m_aTerrlist->clear();
	int x = m_Born_x;
	int y = m_Born_y;
	if (MyTerritory.ref >= 0)
	{
		x = MyTerritory.x;
		y = MyTerritory.y;
	}
	m_OurPopulationManager->TheSkylarkTerrs->GetTerritoriesByDistance(x, y, m_aTerrlist);
	return true; // TransferToState(FindTerritory)
}

//------------------------------------------------------------------------------

bool Skylark_Male::st_Immigrating() {
	/**
	* An instantaneous state which determines the chance of migration mortality. If he does die then he has to inform any old mate that
	* he has gone. If she is already paired with another bird then can just forget her. If not dying then the bird transitions to st_Arrival.
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (g_random_fnc(100) < GetMigrationMortality() || Age > 5)
	{
		if (MyMate != nullptr)
		{
			// Needs to be sure that she is not mated to someone else
			if (!MyMate->Paired)
			{
				MyMate->OnMaleNeverComesBack(this); //stop a bogus on matedeath message
			}
			// In either case forget about mate - he no longer cares about this female
			MyMate = nullptr;
			BSuccess = false;
#ifdef __CJTDebug_8
      if ( m_BroodSize != 0 )
        int rubbish = 0;
      for ( int i = 0; i < 6; i++ ) {
        if ( m_Brood[ i ] != NULL )
          int rubbish = 0;
      }
#endif
		}
		return false; // TransfertoState(Die)
	} // TransferToState(Arrival)
	return true;
}

//------------------------------------------------------------------------------

int Skylark_Male::st_Emigrating() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: Called once per day in the middle of a day
	// In the emigration state GoodWeather must be 0 on first entry
	const int today = m_OurLandscape->SupplyDayInYear();
	if (today >= g_stopdate) return 0; //don't come back this year
	if (m_OurLandscape->SupplyTemp() > 0.0) GoodWeather++;
	else GoodWeather = 0;
	if (GoodWeather >= 3) // Third day of good weather
	{
		const int chance = g_random_fnc(cfg_ReturnProbability.value()); // number between 0-99999
		if (Age < 1)
		{
			// below returning probability is divided by 30 as the number of days
			if (chance < 300 && today >= March) return 1;
			if (today >= April) return 1;
		}
		else
		{
			if (chance < 7 && today >= January) return 1;
			if (chance < 33 && today >= February) return 1;
			if (chance < 300 && today >= March) return 1;
			if (today >= April) return 1;
		}
	}
	return 0; // return value of 0 do nothing, 1 go to immigration
}

//------------------------------------------------------------------------------

bool Skylark_Male::OnEvicted() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// returns true if evicted or otherwise false
	if (!Paired)
	{
		m_aTerrlist->clear();
		m_OurPopulationManager->TheSkylarkTerrs->GetTerritoriesByDistance(MyTerritory.x, MyTerritory.y, m_aTerrlist);
		m_OurTerritories->RemoveMale(MyTerritory.ref);
		MyTerritory.ref = -1;
		HaveTerritory = false;
		m_CurrentSkState = toss_MFindingTerritory;
		return true;
	} // can't evict I'm paired
	return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void Skylark_Male::OnReHouse() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	int mindist = 100000000;
	int index = -1;
	// Needs to sort through the new list of territories to find one closest to its m_Location_x, m_Location_y
	const int Nterrs = m_OurTerritories->SupplyNoTerritories();
	for (int i = 0; i < Nterrs; i++)
	{
		const Skylark_Base* TheOwner = m_OurTerritories->Supply_Owner(i);
		if (!TheOwner)
		{
			const int tx = m_OurTerritories->Supply_x(i);
			const int ty = m_OurTerritories->Supply_y(i);
			const int dist = static_cast<int>(sqrt(static_cast<float>(tx * tx + ty * ty)));
			if (dist < mindist)
			{
				mindist = dist;
				index = i;
			}
		}
	}
	if (index != -1)
	{
		MyTerritory.ref = index;
		// store territory data
		MyTerritory.x = m_OurTerritories->Supply_x(index);
		MyTerritory.y = m_OurTerritories->Supply_y(index);
		MyTerritory.size = m_OurTerritories->Supply_size(index);
		MyTerritory.validnest = m_OurTerritories->SupplyIsNestValid(index);
		EstablishingATerritory();
		if (Paired)
		{
			m_OurTerritories->FemaleOccupy(index, MyMate);
			MyMate->EstablishTerritory();
		}
	}
	else
	{
		if (Paired)
		{
			MyMate->OnMateHomeless();
			MyMate = nullptr;
			Paired = false;
		}
		MyTerritory.ref = -1;
		HaveTerritory = false;
		m_CurrentSkState = toss_MFindingTerritory;
	}
}

//------------------------------------------------------------------------------

void Skylark_Male::OnMateDying() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Brood size is reduced to zero because he forgets about the young
	// So must also tell them to forget him
	for (int i = 0; i < m_BroodSize; i++)
	{
		m_Brood[i]->OnDadDead();
		m_Brood[i] = nullptr;
	}
	m_BroodSize = 0;
	MyMate = nullptr;
	Paired = false;
	BSuccess = false;
	const int today = m_OurLandscape->SupplyDayInYear();
	if (today >= g_stopdate) { m_CurrentSkState = toss_MFlocking; }
	else m_CurrentSkState = toss_AttractingAMate; // TransferToState(AttractAMate);
}

//------------------------------------------------------------------------------

void Skylark_Male::OnMateLeaving() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Brood size is reduced to zero because he forgets about the young
	// He must tell them to forget about him as well
	for (int i = 0; i < m_BroodSize; i++)
	{
		m_Brood[i]->OnDadDead(); // OK he is lying
		m_Brood[i] = nullptr;
	}
	m_BroodSize = 0;
	if (!BSuccess)
	{
		// She has left him, so in this case he gives up
		MyMate = nullptr;
		const double qual = m_OurTerritories->Supply_terr(MyTerritory.ref)->GetQuality();
		// Store the quality for today so the female can read it
		if (m_OurLandscape->SupplyDayInYear() < g_stopdate && qual > m_MyMinTerritoryQual)
		{
			MyTerritory.nqual = qual;
			m_CurrentSkState = toss_AttractingAMate; // TransferToState(AttractAMate);
		}
		else
		{
			// Too late to try again
			m_CurrentSkState = toss_MFlocking;
			HaveTerritory = false;
			// Must de-register himself as territory owner
			m_OurTerritories->RemoveMale(MyTerritory.ref);
			// Just to be sure we can set the ref to -1 = no territory
			MyTerritory.ref = -1;
		}
	}
	else
	{
		// She has stopped breeding for the year
		m_CurrentSkState = toss_MFlocking; // TransferToState(Flocking);
		HaveTerritory = false;
		// Must de-register himself as territory owner
		m_OurTerritories->RemoveMale(MyTerritory.ref);
		// Just to be sure we can set the ref to -1 = no territory
		MyTerritory.ref = -1;
	}
	// In all cases
	Paired = false;
}

//------------------------------------------------------------------------------

void Skylark_Male::OnBroodDeath() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	for (int i = 0; i < m_BroodSize; i++) m_Brood[i] = nullptr;
	m_BroodSize = 0;
	if (Paired)
	{
		m_CurrentSkState = toss_FollowingMate; // Follow mate
		MyMate->OnBroodDeath();
	}
	else if (HaveTerritory) m_CurrentSkState = toss_AttractingAMate;
	else { m_CurrentSkState = toss_MFlocking; }
}

//-----------------------------------------------------------------------------

void Skylark_Male::OnAddPreFledgeling(Skylark_PreFledgeling* P, const Skylark_Nestling* N) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Must now replace nestling pointer with one to the new object
	int n = -1;
	for (int i = 0; i < m_BroodSize; i++) { if (N == m_Brood[i]) n = i; }
	if (n == -1)
	{
		// Very nasty problems
		g_land->Warn("Skylark_Male::Step: Maturing PreFl. not in Male's list", nullptr);
		exit(1);
	}
	m_Brood[n] = P;
	if (!m_firstPF) { MyMate->OnBreedSuccess(); }
	m_firstPF = true;
}

//-----------------------------------------------------------------------------

#ifdef __CJTDebug_5
bool Skylark_Male::DoIExistN( Skylark_Nestling * N ) {
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
  // Must now replace nestling pointer with one to the new object
  int n = -1;
  for ( int i = 0; i < m_BroodSize; i++ ) {
    if ( N == m_Brood[ i ] ) n = i;
  }
  if ( n == -1 ) return false; else
    return true;
}

bool Skylark_Male::DoIExistP( Skylark_PreFledgeling * N ) {
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
  // Must now replace nestling pointer with one to the new object
  int n = -1;
  for ( int i = 0; i < m_BroodSize; i++ ) {
    if ( N == m_Brood[ i ] ) n = i;
  }
  if ( n == -1 ) return false; else
    return true;
}

#endif
//-----------------------------------------------------------------------------

void Skylark_Male::OnPreFledgelingDeath(const Skylark_PreFledgeling* P) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
  bool found = false;
#endif
	// remove the dead one
	for (int i = 0; i < m_BroodSize; i++)
	{
		if (m_Brood[i] == P)
		{
			m_Brood[i] = m_Brood[--m_BroodSize];
			m_Brood[m_BroodSize] = nullptr;
#ifdef __CJTDebug_5
      found = true;
#endif
		}
	}

#ifdef __CJTDebug_4
  if ( m_BroodSize < 0 ) {
    OnArrayBoundsError();
  }
#endif

#ifdef __CJTDebug_5
  if ( found == false ) {
    g_land->Warn( "Skylark_Male::OnPreFledgelingDeath: DeBug5 ", NULL );
    exit( 1 );
  }
#endif
	// If all dead do on Brood death
	// remove the matured one
	if (m_BroodSize == 0) { OnBroodDeath(); }
}

//-----------------------------------------------------------------------------

void Skylark_Male::OnPreFledgelingMature(const Skylark_PreFledgeling* P) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	for (int i = 0; i < m_BroodSize; i++)
	{
		if (m_Brood[i] == P)
		{
			m_Brood[i] = m_Brood[--m_BroodSize];
			m_Brood[m_BroodSize] = nullptr;
			BSuccess = true;
			if (MyMate) MyMate->OnBreedingSuccess();
		}
	}
	// If all matured do on Brood death
	// remove the matured one
#ifdef __CJTDebug_4
  if ( m_BroodSize < 0 ) {
    OnArrayBoundsError();
  }
#endif
	if (m_BroodSize == 0)
	{
		if (Paired)
		{
			// it all depends what the female is doing
			// what he must do.
			// if she is also feeding he needs to tell here to stop
			MyMate->OnStopFeedingChicks();
			m_CurrentSkState = toss_FollowingMate;
		}
		else if (HaveTerritory) m_CurrentSkState = toss_AttractingAMate;
		else { m_CurrentSkState = toss_MFlocking; }
	}
}

//-----------------------------------------------------------------------------
void Skylark_Male::OnMateNeverComesBack(const Skylark_Female* AFemale) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Just a safety check
	if (MyMate == AFemale)
	{
		MyMate = nullptr;
		Paired = false;
		BSuccess = false;
	}
}

//-----------------------------------------------------------------------------

skTerritory_struct Skylark_Male::Supply_Territory() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	return MyTerritory;
}

//-----------------------------------------------------------------------------

int Skylark_Male::st_TempLeavingArea() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing - called once per day
	// In the emigration state GoodWeather must be 0 on first entry
	if (!GetBadWeather()) GoodWeather++;
	else GoodWeather = 0;
	if (GoodWeather >= 7) // 7 days of good weather
	{
		return 1; // comeback to the area
	}
	return 0; // return value of 0 do nothing, 1 go to Arrival
}

//------------------------------------------------------------------------------

int Skylark_Male::st_FindingTerritory() {
	Skylark_Male* TheOwner;
	if (m_aTerrlist->size() == 0)
	{
		/** This option picks 10 territories at random and tests them.
		This will happen if the bird arrives here without an m_aTerrlist, or because it has exhausted its list of possible territories.
		*/
#ifdef __CJTDebug_5
		  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
		const int Nterrs = m_OurTerritories->SupplyNoTerritories();
		int territories[10][2];
		// Select 10 territories
		for (int i = 0; i < 10; i++)
		{
			territories[i][0] = g_random_fnc(Nterrs);
			TheOwner = m_OurTerritories->Supply_Owner(territories[i][0]);
			// If there is an owner
			if (TheOwner != nullptr) { if (TheOwner->DefendTerritory() >= Age) { territories[i][1] = -1; } }
			else { territories[i][1] = m_OurTerritories->Supply_quality(territories[i][0]); }
		}
		int best_qual = 0; // much too low
		int best = -1;
		for (int i = 0; i < 10; i++)
		{
			if (territories[i][1] > best_qual)
			{
				best_qual = territories[i][1];
				best = i;
			}
		}
		if (best_qual > m_MyMinTerritoryQual) // Territory acceptable
		{
			// save ref number
			TheOwner = m_OurTerritories->Supply_Owner(territories[best][0]);
			if (TheOwner)
			{
				// Must send a get out message to the other bird
				// if he is already breeding then fail
				if (TheOwner->OnEvicted()) return 2; // fail
			}
			MyTerritory.ref = territories[best][0];
			// store territory data
			MyTerritory.x = m_OurTerritories->Supply_x(MyTerritory.ref);
			MyTerritory.y = m_OurTerritories->Supply_y(MyTerritory.ref);
			MyTerritory.size = m_OurTerritories->Supply_size(MyTerritory.ref);
			MyTerritory.validnest = m_OurTerritories->SupplyIsNestValid(MyTerritory.ref);
			return 1; //TransferToState(EstablishTerritory);
		}
	}
	else
	{
		/** This option is for birds with a valid m_aTerrlist to search.
		 It takes the 10 nearest territories one at a time and tests to see if they are OK
		 If so establish in one of them, otherwise wait until another day to test the next 10
		 */

		for (int i = 0; i < 10; i++)
		{
			int sz = m_aTerrlist->size() - 1;
			int qual = 0;
			if (sz >= 0)
			{
				const APoint p = (*m_aTerrlist)[sz--];
				m_aTerrlist->pop_back();
				TheOwner = m_OurTerritories->Supply_Owner(p.m_x);
				// If there is an owner
				if (TheOwner != nullptr) { if (TheOwner->DefendTerritory() < Age) { qual = m_OurTerritories->Supply_quality(p.m_x); } }
				else { qual = m_OurTerritories->Supply_quality(p.m_x); }
				if (qual > m_MyMinTerritoryQual) // Territory acceptable
				{
					if (TheOwner)
					{
						// Must send a get out message to the other bird
						// if he is already breeding then fail
						if (TheOwner->OnEvicted()) return 2; // fail
					}
					MyTerritory.ref = p.m_x;
					// store territory data
					MyTerritory.x = m_OurTerritories->Supply_x(MyTerritory.ref);
					MyTerritory.y = m_OurTerritories->Supply_y(MyTerritory.ref);
					MyTerritory.size = m_OurTerritories->Supply_size(MyTerritory.ref);
					MyTerritory.validnest = m_OurTerritories->SupplyIsNestValid(MyTerritory.ref);
					return 1; //TransferToState(EstablishTerritory);
				}
			}
		}
	}

#ifdef FOR_DEMONSTRATION
  m_Location_x = 0;
  m_Location_y = 0;
#endif
	return 2; //TransferToState(Floating);
}

//------------------------------------------------------------------------------

int Skylark_Male::EstablishingATerritory() {
	/*
	The male claims the territory, primarily a programming construct, but resulting in the male being registered as occupying the territory.<br>
	*/

#ifdef __CJTDebug_5
	if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	m_aTerrlist->clear();
	// Stand in the centre.
	m_Location_x = MyTerritory.x + MyTerritory.size / 2;
	if (m_Location_x >= m_OurPopulationManager->SupplySimW()) m_Location_x = m_Location_x % m_OurPopulationManager->SupplySimW();
	m_Location_y = MyTerritory.y + MyTerritory.size / 2;
	if (m_Location_y >= m_OurPopulationManager->SupplySimH()) m_Location_y = m_Location_y % m_OurPopulationManager->SupplySimH();
	// claim occupation
	m_OurTerritories->Occupy(MyTerritory.ref, this);
	HaveTerritory = true;
	return 0; // TransferToState(AttractMate)
}

//-----------------------------------------------------------------------------

int Skylark_Male::DefendTerritory() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	if (Paired == true || BSuccess == true) return 999; // very old
	return Age;
	//can't be ousted if paired or owned the territory before
}

//------------------------------------------------------------------------------

int Skylark_Male::GetMigrationMortality() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	Age++;
	if (Age == 1) return JuvenileReturnMort;
	return m_OurPopulationManager->SupplyM_Mig_Mort();
}

//------------------------------------------------------------------------------

int Skylark_Male::st_AttractingAMate() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	/** Is called once each day. If a female comes along and pairs then she will trigger a move to FollowMate via Skylark_Male::OnPairing
	*/
	if (m_OurLandscape->SupplyDayInYear() > g_stopdate)
	{
		m_OurTerritories->RemoveMale(MyTerritory.ref);
		MyTerritory.ref = -1; // No territory
		HaveTerritory = false;
		return 1; // TransferToState(Flocking)
	}
	return 0; // Do nothing
}

//-----------------------------------------------------------------------------

TTypesOfSkState Skylark_Male::st_Floating() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (GetBadWeather()) return toss_MTempLeavingArea;
	m_Location_x = g_random_fnc(m_OurPopulationManager->SupplySimW());
	m_Location_y = g_random_fnc(m_OurPopulationManager->SupplySimH());
	if (m_OurLandscape->SupplyDayInYear() > g_stopdate) { return toss_MFlocking; }
#ifdef FOR_DEMONSTRATION
    m_Location_x = 0;
    m_Location_y = 0;
#endif
	return toss_MFindingTerritory; //TransferToState(FindTerritory);
}
//-----------------------------------------------------------------------------

int Skylark_Male::st_FollowingMate() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	/**
	* This is simply waiting for a message to say that either female is dead, female leaves or the eggs hatch.
	* The male occupies the same location as the female. When the female's eggs hatch there is a transition to st_CaringForYoung.
	* If the female dies or abandons the territory, there is a transition to AttractingAMate.
	* This method also has another function in that it sets up the food tables for the female, hence must be called before female activity starts.
	*/
	if (Paired)
	{
		GetFood(0); //needs to set up the food tables for the female
	}
	else
	{
		m_CurrentSkState = toss_MFloating;
		if (HaveTerritory)
		{
			HaveTerritory = false;
			// Must de-register himself as territory owner
			m_OurTerritories->RemoveMale(MyTerritory.ref);
			// Just to be sure we can set the ref to -1 = no territory
			MyTerritory.ref = -1;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------

void Skylark_Male::ReEvaluateTerritory() {
	/**
	The method is designed to keep the measure of territory quality up to date and avoids the need for the female
	to do this unless finding a nest position.
	The second usage is to provide the potential to reduce territory size should conditions improve. The reverse situation
	results in desertion by the female so is not handled here.
	*/
	double qual = 0;
	if (MyTerritory.validnest) { qual = m_OurTerritories->Supply_terr(MyTerritory.ref)->GetQuality(); }
	else qual = 0;
	if (qual > m_XFNestAcceptScore)
	{
		if (m_OurTerritories->Supply_terr(MyTerritory.ref)->GetVirtualDiameter() < 50) return;
		// He really has too much, so split it down into 2
		m_OurTerritories->Split(MyTerritory.ref);
		qual = m_OurTerritories->Supply_terr(MyTerritory.ref)->GetQuality();
	}
	// Store the quality for today so the female can read it
	MyTerritory.nqual = qual;
}
//-----------------------------------------------------------------------------

void Skylark_Male::OnEggHatch() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// NB this must not occur if there are still chicks to scare off -
	// if it does then they are left in limbo!
	// Brood information is already put here by CreatObjects
#ifdef __CJTDebug_7
	if (m_BroodSize < 1)
	{
		g_land->Warn("Skylark_Male::OnEggHatch: DeBug7 ", nullptr);
		exit(1);
	}
#endif
	m_CurrentSkState = toss_MCaringForYoung; // TransferToState(CareForYoung);
}

//-----------------------------------------------------------------------------
void Skylark_Male::OnAddNestling(Skylark_Nestling* N) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Add nestling and record the new brood member
	m_Brood[m_BroodSize++] = N;
#ifdef __CJTDebug_4
  if ( m_BroodSize > 25 ) {
    OnArrayBoundsError();
  }
#endif
}

//-----------------------------------------------------------------------------

void Skylark_Male::OnNestlingDeath(const Skylark_Nestling* N) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
  bool found = false;
#endif
	// remove the dead one
	m_BroodSize--;
#ifdef __CJTDebug_4
  if ( m_BroodSize < 0 ) {
    OnArrayBoundsError();
  }
#endif
	for (int i = 0; i <= m_BroodSize; i++)
	{
		if (m_Brood[i] == N)
		{
			m_Brood[i] = m_Brood[m_BroodSize];
			m_Brood[m_BroodSize] = nullptr;
#ifdef __CJTDebug_5
      found = true;
#endif
		}
	}
#ifdef __CJTDebug_5
  if ( found == false ) {
    g_land->Warn( "Skylark_Male::OnNestlingDeath - Debug 5 ", NULL ); exit( 1 );
  }
#endif
	// If all dead do on Brood death
	// remove the matured one
	if (m_BroodSize == 0) { OnBroodDeath(); }
}

//-----------------------------------------------------------------------------

void Skylark_Male::OnNestPredatation() {
	for (int i = 0; i < m_BroodSize; i++) { m_Brood[i]->OnYouHaveBeenEaten(); }
	m_BroodSize = 0;
	OnBroodDeath();
}

//-----------------------------------------------------------------------------

void Skylark_Male::OnBroodDesertion() {
	for (int i = 0; i < m_BroodSize; i++) { m_Brood[i]->OnDeserted(); }
	m_BroodSize = 0;
	OnBroodDeath();
}

//-----------------------------------------------------------------------------

void Skylark_Male::OnNestLocation(int x, const int y) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	if (x == 0) x = 2;
	m_Location_x = x - 1;
	m_Location_y = y;
}

//-----------------------------------------------------------------------------

void Skylark_Male::OnPairing(Skylark_Female* female) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	Paired = true;
	if (MyMate) { if (MyMate != female) MyMate->OnMaleNeverComesBack(this); }
	MyMate = female;
	ConstructAHabitatTable();
	BSuccess = false;
	m_CurrentSkState = toss_FollowingMate;
}

//-----------------------------------------------------------------------------

void Skylark_Male::ConstructAHabitatTable() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// This function must be called when the female arrives and
	// again when the eggs first hatch
	// This function constructs a table listing
	// the reference of each habitat object
	// and the area of that object within the skylarks home range
	// First must find out which habitats are in HomeRange
	// this is done by searching the landscape raster and counting how many
	// 1 meter areas are present for each habitat object

	// These days this is easy, as the information is already
	// available in the territory as a pre-chewed list provided
	// for this very use by PreProcessLandscape2()! :-)
	const skTTerritory* l_terr = m_OurTerritories->Supply_terr(MyTerritory.ref);
	const int l_polys = static_cast<int>(l_terr->m_hr_polys.size());
#ifdef __PESTICIDE_RA
	m_PConcTable.resize(l_polys);
#endif
	m_HabitatTable_PNum.resize(l_polys);
	m_HabitatTable_Size.resize(l_polys);
	m_InsectTable.resize(l_polys);
	for (int i = 0; i < l_polys; i++)
	{
		m_HabitatTable_PNum[i] = l_terr->m_hr_polys[i];
		m_HabitatTable_Size[i] = l_terr->m_hr_sizes[i];
	}
	No_HabitatTable_Refs = l_polys;
}

//-----------------------------------------------------------------------------

int Skylark_Male::st_ScaringOffChicks() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_BroodSize > 0)
	{
		for (int i = 0; i < m_BroodSize; i++)
		{
			m_Brood[i]->OnDadDead();
			m_Brood[i] = nullptr;
		}
		m_BroodSize = 0; // all chicks gone
		MyMate->OnBreedingSuccess();
		MyMate->OnStopFeedingChicks();
		BSuccess = true;
	}
	m_firstPF = false;
	return 1; // TransferToState(FollowMate)
}

//-----------------------------------------------------------------------------

int Skylark_Male::st_CaringForYoung() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
  if ( m_BroodSize < 1 )
    DEADCODEError();
#endif
	double spare; // spare food
	int HungryChicks = m_BroodSize;
	int FeedingProbs[26];
	double ChickFood[26];
#ifdef __PESTICIDE_RA_CHICK
  double ChickPoison[ 26 ];
#endif
	if (m_Brood[0]->Age == 31)
	{
		return 1; // Scare off chicks
	}
	// Check for bad weather
	const double RainToday = m_OurLandscape->SupplyRain();
	if (RainToday > MaxFeedRain) return 0; // No feeding today

	// can only get food in daylight - dawn & dusk
	const int food_time = static_cast<int>(static_cast<double>(m_OurLandscape->SupplyDaylength() - 120));
	const double resources_all = GetFood(food_time); // dw g insects
	// remove metabolic requirements
	double resources = RemoveEM(resources_all);
	if (resources > 0)
	// Now give the food to the chicks
	// The chance that a chick get a portion is proportional to their size
	{
		// Zero the feeding probabilities
		for (int i = 0; i < m_BroodSize; i++)
		{
			FeedingProbs[i] = 0;
			ChickFood[i] = 0;
#ifdef __PESTICIDE_RA_CHICK
      ChickPoison[ i ] = 0;
#endif
		}
		resources /= FoodTripsPerDay; // Split into portions (food trips per day)
#ifdef __PESTICIDE_RA_CHICK
    double pcide_remain = resources * m_pcide_conc;
#endif
		int BroodWeight = 0;
		for (int i = 0; i < m_BroodSize; i++)
		{
			// Add up brood weight
			const int weight = static_cast<int>(floor(m_Brood[i]->m_Size + 0.5));
			BroodWeight += weight;
			for (int j = i; j < m_BroodSize; j++) FeedingProbs[j] += weight;
		}

		while (resources > 0.01 && HungryChicks > 0)
		{
			for (int portion = 0; portion < FoodTripsPerDay; portion++)
			{
				// For each portion
				const int choose = g_random_fnc(BroodWeight);
				for (int i = 0; i < m_BroodSize; i++)
				{
					if (choose < FeedingProbs[i])
					{
						ChickFood[i] += resources; // store the food for that chick
#ifdef __PESTICIDE_RA_CHICK
            ChickPoison[ i ] += pcide_remain;
#endif
						break;
					}
				}
			}
			// Now give the food to the chicks
			// Better do this than feed each a portion at a time, it is more efficient
			// and stops problems with growth between portions
			spare = 0;
			for (int i = 0; i < m_BroodSize; i++)
			{
				double extra = m_Brood[i]->On_FoodSupply(ChickFood[i]);
#ifdef __PESTICIDE_RA_CHICK
		// If we want a chick response to pesticide then the line below should be lit up, and the response
		// specified in the PesticideResponse method
		m_Brood[ i ]->m_pesticide_accumulation += ChickPoison[i]* ((ChickFood[ i ]-extra)/ChickFood[ i ]);
#endif
				if (extra > 0) // chick must be full
				{
					FeedingProbs[i] = -1; // no chance of food
					HungryChicks--;
					spare += extra;
				}
			}
			resources = spare / FoodTripsPerDay;
		}
	}
	return 0; // no change
}

//------------------------------------------------------------------------------

double Skylark_Male::GetFood(int time) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif

#ifdef __CJTDebug_4
  if ( No_HabitatTable_Refs > 250 )
    OnArrayBoundsError();
#endif
	// This must be called each day before the female's code is executed
	double food = 0;
	// First find out how much food is available from each habitat
	// Note that the pesticide risk assessment code is integrated here.
	// Stage 1 create the insect data
	for (int i = 0; i < No_HabitatTable_Refs; i++)
	{
		// Insects = Insects * Hindrance
		// Insect table holds insects per squaremetre
#ifdef __PESTICIDE_RA
		m_PConcTable[i] = m_OurLandscape->SupplyPesticide(m_HabitatTable_PNum[i], ppp_1);
#endif
		const double WHindrance = GetWeatherHindrance();
		const double hind = GetVegHindrance(m_HabitatTable_PNum[i]);
		const double insects = m_OurLandscape->SupplyInsects(m_HabitatTable_PNum[i]);
		// Insects = Insects * Hindrance
		// Insect table holds insects per squaremetre
		m_InsectTable[i] = insects * hind * WHindrance;
	}
	// Now sort habitats into optimal searching order (i.e. most insect/m2)
	OptimiseHabitatSearchingOrder();
	// Now extract insects available from each for as long as spends searching
	int index = 0;
	// Note at present this routine assumes that 1 min is spent per m2
	// if this assumption is changed then we must alter 'time' accordingly
#ifdef __PESTICIDE_RA
	m_pcide = 0.0;
#endif
	double fd;
	while (time > 0)
	{
		const int area = m_HabitatTable_Size[index];
		if (time > area)
		{
			time -= area;
			// Food+= area*WeatherHindrance*ExtractionEfficiency*Insects per m2
			fd = area * MyExtractEff * m_InsectTable[index];
			food += fd;
#ifdef __PESTICIDE_RA
			m_pcide += fd * m_PConcTable[index] * KcalPerGInsect_kg_inv; // Needs converting from kcal to mg;
#endif
			index++;
#ifdef __CJTDebug_4
      if ( index > 249 )
        OnArrayBoundsError();
#endif
		}
		else
		{
			// Food+=
			// minutes left*WeatherHindrance*ExtractionEfficiency*Insects per m2
			fd = time * MyExtractEff * m_InsectTable[index];
			food += fd;
#ifdef __PESTICIDE_RA
			m_pcide += m_PConcTable[index] * time * KcalPerGInsect_kg_inv; // Needs converting from kcal to mg;
#endif
			time = 0;
		}
	}
	return food;
}

//------------------------------------------------------------------------------

void Skylark_Male::OptimiseHabitatSearchingOrder() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// This method sorts the HabitatTable on the basis of the highest
	// available insects per square metre basis
	// It also puts the available insects into the table

	// Sort the entries on the basis of InsectTable
	int interChange = 1;
	int pass = 0;
	while (pass++ <= No_HabitatTable_Refs - 1 && interChange)
	{
		interChange = 0;
		for (int j = 0; j < No_HabitatTable_Refs - pass; j++)
			if (m_InsectTable[j] < m_InsectTable[j + 1])
			{
				// set the flag
				interChange = 1;
				// The poly ref
				int hold1 = m_HabitatTable_PNum[j];
				m_HabitatTable_PNum[j] = m_HabitatTable_PNum[j + 1];
				m_HabitatTable_PNum[j + 1] = hold1;
				// the size
				hold1 = m_HabitatTable_Size[j];
				m_HabitatTable_Size[j] = m_HabitatTable_Size[j + 1];
				m_HabitatTable_Size[j + 1] = hold1;
				// The insects
				double hold2 = m_InsectTable[j];
				m_InsectTable[j] = m_InsectTable[j + 1];
				m_InsectTable[j + 1] = static_cast<int>(hold2);
				// The Pesticide
#ifdef __PESTICIDE_RA
				hold2 = m_PConcTable[j];
				m_PConcTable[j] = m_PConcTable[j + 1];
				m_PConcTable[j + 1] = static_cast<int>(hold2);
#endif
			}
	}
}

//-----------------------------------------------------------------------------
//                             Skylark_Female
//-----------------------------------------------------------------------------

Skylark_Female::Skylark_Female(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx,
                               int by, int mh) : Skylark_Adult(x, y, size, age, Terrs, L, SPM, bx, by, mh) {
	MyMate = nullptr;
	MyClutch = nullptr;
	ResetBreedingSuccess();
	m_BreedingAttempts = 0;
	m_MinFemaleAcceptScore = 0;
}
//------------------------------------------------------------------------------

void Skylark_Female::ReInit(const int x, const int y, const double size, const int age, SkTerritories* Terrs, Landscape* L,
                            Skylark_Population_Manager* SPM, const int bx, const int by, const int mh) {
	Skylark_Adult::ReInit(x, y, size, age, Terrs, L, SPM, bx, by, mh);
	MyMate = nullptr;
	MyClutch = nullptr;
	ResetBreedingSuccess();
	m_BreedingAttempts = 0;
	m_MinFemaleAcceptScore = 0;
}
//------------------------------------------------------------------------------

bool Skylark_Female::OnFarmEvent(FarmToDo event) {
	switch (event)
	{
	case sleep_all_day:
		break;
// Here begins events that destroy the nest
	case preseeding_cultivator:
	case summer_harrow:
	case strigling_sow:
	case strigling:
	case strigling_hill:
		// If she is in nest building then the nest is destroyed
		if (m_CurrentSkState == toss_MakingNest || m_CurrentSkState == toss_PreparingForBreeding)
		{
			// Make a state change
			m_CurrentSkState = toss_StartingNewBrood;
			AddStriglingMort(0);
		}
		break;
	case autumn_plough:
	case stubble_plough:
	case stubble_cultivator_heavy:
	case row_cultivation:
	case autumn_harrow:
	case preseeding_cultivator_sow:
	case heavy_cultivator_aggregate:
	case autumn_sow:
	case autumn_sow_with_ferti:
	case winter_plough:
	case deep_ploughing:
	case summer_plough:
	case summer_sow:
	case winter_harrow:
	case spring_plough:
	case spring_harrow:
	case shallow_harrow:
	case spring_sow:
	case spring_sow_with_ferti:
	case autumn_or_spring_plough:
	case bed_forming:
	case stubble_harrowing:
		// If she is in nest building then the nest is destroyed
		if (m_CurrentSkState == toss_MakingNest || m_CurrentSkState == toss_PreparingForBreeding)
		{
			// Make a state change
			m_CurrentSkState = toss_StartingNewBrood;
		}
		break;
// Here begins non-mortality events
	case spring_roll:
	case autumn_roll:
	case fa_boron: //AHA
    case fp_npks:
	case molluscicide:
    case fp_npk:
    case fp_pk:
	case fp_k:
    case fp_sk: //AHA
	case fp_p:
	case fa_calcium:
	case fa_pks:
	case fiber_covering:
    case fp_n: //AHA
    case fp_liquidNH3:
    case fp_slurry:
	case fp_ammoniumsulphate:
	case fp_manganesesulphate:
	case fp_manure:
	case fp_greenmanure:
	case fp_sludge:
	case fp_rsm:
	case fp_calcium:
    case fp_boron: //AHA
	case fp_cu:
	case fp_nc:
	case fp_nk:
	case fp_ns:
	case fp_pks:
    case fa_cu: //AHA
	case fa_npk:
    case fa_npks:
    case fa_pk:
	case fa_k:
    case fa_sk: //AHA
	case fa_p:
    case fa_n:
    case fa_nk: //AHA
    case fa_slurry:
    case fa_ammoniumsulphate:
	case fa_manganesesulphate:
	case fa_manure:
	case fa_greenmanure:
	case fa_sludge:
	case fa_rsm:
	case cut_weeds:
	case herbicide_treat:
	case growth_regulator:
	case fungicide_treat:
	case insecticide_treat:
	case org_insecticide:
	case hilling_up:
	case water:
	case swathing:
	case harvest:
	case harvest_bushfruit: //AHA
	case green_harvest:
	case harvestshoots:
	case pruning: //AHA
	case fiber_removal: //AHA
	case shredding: //AHA
	case manual_weeding: //AHA
	case cattle_out:
	case cattle_out_low:
	case cut_to_hay:
	case cut_to_silage:
	case straw_chopping:
	case hay_turning:
	case hay_bailing:
	case trial_insecticidetreat:
	case burn_straw_stubble:
	case flammebehandling:
	case burn_top: //AHA
	case flower_cutting:
	case bulb_harvest:
	case straw_covering:
	case straw_removal:
	case mow:
	case last_treatment:
	case org_fungicide:
	case org_herbicide:
	case pheromone:
	case suckering:
	case trial_control:
	case trial_toxiccontrol:
	case product_treat:
	case glyphosate:
	case biocide:
		break;
// Here begins spray mortality
	case syninsecticide_treat:
		if (g_random_fnc(1000) < cfg_insecticide_direct_mortF.value())
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Female::OnFarmEvent() : %d : %d\n", event, ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_FDying;
		}
		break;
// Here begins events that cause females to give up territory
	case pigs_out:
#ifndef __NoPigsOutEffect
		if (Paired)
		{
			if (g_random_fnc(100) < 2) // 2%
			{
				// Too much disturbance so give up
				m_CurrentSkState = toss_GivingUpTerritory;
			}
		}
#endif
		break;
	default:
		g_land->Warn("Skylark_Female::OnFarmEvent(): ""Unknown event type:", m_OurLandscape->EventtypeToString(event));
		exit(1);
	}
	if (m_CurrentSkState == toss_FDying) return true;
	return false;
}

//------------------------------------------------------------------------------

void Skylark_Female::BeginStep() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_CurrentSkState == toss_Destroy) return;
	CheckManagement();
	if (m_MinFemaleAcceptScore < 0.0) m_MinFemaleAcceptScore = 0.0;
	switch (m_CurrentSkState)
	{
	case toss_FFloating: // Floating
		switch (st_Floating())
		{
		case 2:
			m_CurrentSkState = toss_FFlocking;
			break;
		case 3:
			m_CurrentSkState = toss_FTempLeavingArea; // -> temp.leave
			break;
		case 1:
			m_CurrentSkState = toss_FFindingTerritory; // -> find terr.
			break;
		case 0:
			break;
		}
		m_StepDone = true;
		break;
	case toss_FCaringForYoung: // CaringForYoung
		switch (st_CaringForYoung())
		{
		case 2: //StopBreeding
			m_CurrentSkState = toss_StoppingBreeding;
			break;
		case 1:
			m_CurrentSkState = toss_StartingNewBrood;
			break;
		default:
			break;
		}
		m_StepDone = true;
		break;
	default:
		/* char errornum[20]; sprintf(errornum, "%d", m_CurrentSkState );
		g_land->Warn("Skylark_Female::BeginStep(): Unknown state: ", errornum); exit(0); */
		break;
	}
}

//------------------------------------------------------------------------------

void Skylark_Female::Step() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (m_StepDone || m_CurrentSkState == toss_Destroy) return;
#ifdef __CJTDebug_5
  if ( MyClutch ) {
    if ( MyClutch->Mother != this ) {
      g_land->Warn( "Skylark_Female::Step: DeBug5 ", NULL ); exit( 1 );
    }
  }
#endif
	switch (m_CurrentSkState)
	{
	case toss_Initiation: // Initial
		m_CurrentSkState = toss_FFlocking;
		break;
	case toss_FFlocking: // Flocking
		switch (st_Flocking())
		{
		case 1:
			// use 0,0 as a default location
			m_Location_x = 0;
			m_Location_y = 0;
			m_CurrentSkState = toss_FEmigrating; // -> Emigration
			break;
		}
		m_StepDone = true;
		break;
	case toss_FFloating: // Floating
		m_StepDone = true;
		break;
	case toss_FArriving: // Arriving - instant
		if (st_Arriving()) m_CurrentSkState = toss_FFindingTerritory; // -> find terr.
		else { m_CurrentSkState = toss_FTempLeavingArea; }
		m_StepDone = true;
		break;
	case toss_FImmigrating: // Immigration - instant
		if (st_Immigrating()) m_CurrentSkState = toss_FArriving; // -> arrival
		else
		{
#ifdef TEST_ISSUE_DEATH_WARRANT
        printf( "Skylark_Female::Step() : st_Immigrating() : %d\n", ( int )g_date->Date() );
#endif
			m_CurrentSkState = toss_FDying; // -> die
		}
		break;
	case toss_FEmigrating: // Emigration
		if (st_Emigrating() == 1) m_CurrentSkState = toss_FImmigrating; // -> immigr.
		m_StepDone = true;
		break;
	case toss_FTempLeavingArea: // temp.leave.area
		if (st_TempLeavingArea()) m_CurrentSkState = toss_FArriving; // -> arrival
		m_StepDone = true;
		break;
	case toss_FFindingTerritory: // FindingTerritory
		switch (st_Finding_Territory())
		{
		case 1:
			m_CurrentSkState = toss_BuildingUpResources; // -> building res.
#ifdef __CJTDebug_5
          if ( !Paired ) {
            g_land->Warn( "Skylark_Female::Step: !Paired ", NULL ); exit( 1 );
          }
#endif
			break;
		case 2:
			// Should never be paired here
			m_CurrentSkState = toss_FFloating; // -> floating
			break;
		case 3:
			m_CurrentSkState = toss_FTempLeavingArea;
			break;
		}
		m_StepDone = true; // will now go one to EndStep
		break;
	case toss_BuildingUpResources: // Building Resources
#ifdef __CJTDebug_5
		if ( !Paired ) {
        g_land->Warn( "Skylark_Female::Step: !Paired ", NULL ); exit( 1 );
      }
#endif
		switch (st_BuildingUpResources())
		{
		case 1:
			m_CurrentSkState = toss_PreparingForBreeding; // -> prepare for breed.
			break;
		case 2:
			m_CurrentSkState = toss_GivingUpTerritory; // -> give up territory
			break;
		}
		m_StepDone = true;
		break;
	case toss_MakingNest: // Making nest
		m_CurrentSkState = st_MakingNest();
		m_StepDone = true; // will now go one to EndStep
		break;
	case toss_PreparingForBreeding: // PreparingForBreeding
#ifdef __CJTDebug_5
      if ( !Paired ) {
        g_land->Warn( "Skylark_Female::Step: !Paired ", NULL ); exit( 1 );
      }
#endif
		switch (st_PreparingForBreeding())
		{
		case 0: // Do nothing
			break;
		case 1:
			m_CurrentSkState = toss_MakingNest; // -> make nest
			NestLoc = false; // no nest location yet
			break;
		case 2: // Too late -> give up for this year
			m_CurrentSkState = toss_GivingUpTerritory;
			break;
		}
		m_StepDone = true; // will now go one to EndStep
		break;
	case toss_Laying: // Laying
		m_CurrentSkState = st_Laying();
		if (m_CurrentSkState == toss_Incubating) m_toowet = 0;
		m_StepDone = true;
		break;
	case toss_StartingNewBrood: // StartingNewBrood
		switch (st_StartingNewBrood())
		{
		case 1:
			m_CurrentSkState = toss_MakingNest; // -> make nest
			break;
		case 2:
			m_CurrentSkState = toss_GivingUpTerritory; // -> give up terr.
			break;
		case 3:
			m_CurrentSkState = toss_FFlocking; // -> flocking
			break;
		}
		m_StepDone = true; // will now go one to EndStep
		break;
	case toss_EggHatching: // EggHatching - happens after the young hatch at 1 min to midnight
		if (st_EggHatching()) m_CurrentSkState = toss_FCaringForYoung; // -> care for young
		m_StepDone = true;
		break;
	case toss_Incubating: // Incubation
		switch (st_Incubating())
		{
		case 1:
			{
				g_land->Warn("Skylark_Female::Step: EggHatching ", nullptr);
				exit(1);
			}
		//m_CurrentSkState=35; // -> egg hatch
			break;
		case 2:
			g_land->Warn("Skylark_Female::Step: Illegal Return Value ", nullptr);
			exit(1);
		case 3:
			m_CurrentSkState = toss_StartingNewBrood; // -> start new brood
			MyClutch->OnMumGone();
			MyClutch = nullptr;
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1004 );
#endif
			break;
		}
		m_StepDone = true; // will now go one to EndStep
		break;
	case toss_StoppingBreeding: // StopBreed. - instant
		if (st_StoppingBreeding())
		{
			m_CurrentSkState = toss_FFlocking; // -> flocking
		}
		break;
	case toss_FDying: // Dying
		st_Dying();
		m_StepDone = true;
		break;
	case toss_GivingUpTerritory: // giving up terr. - one day
		if (st_GivingUpTerritory() == 1) m_CurrentSkState = toss_FFloating; // -> floating
		m_StepDone = true;
		break;
	default:
		const string st = m_OurPopulationManager->SupplyStateNames(m_CurrentSkState);
		g_land->Warn("Skylark_Female::Step: Unknown State", st.c_str());
		exit(1);
	}
}

//------------------------------------------------------------------------------

void Skylark_Female::EndStep() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Need to find out if it is poisoned
#ifdef __PESTICIDE_RA
	PesticideResponse();
#endif
}

//-----------------------------------------------------------------------------

void Skylark_Female::OnStopFeedingChicks() {
	// Only a problem if she has to be told to stop feeding
	if (m_CurrentSkState == toss_FCaringForYoung) m_CurrentSkState = toss_StartingNewBrood;
}

//-----------------------------------------------------------------------------

void Skylark_Female::OnBreedingSuccess() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	BSuccess = true;
}

//-----------------------------------------------------------------------------
int Skylark_Female::st_Flocking() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	/**
	* The bird is assumed to move around the area waiting until weather conditions are suitable to begin Emigration, depending upon the
	* time of year. Only arrives here after breeding, so poor conditions will trigger a transition to Emigration.
	* A transition to Emigration will also occur if the bird has not emigrated before October.
	*/
#ifdef FOR_DEMONSTRATION
  // Removes itinerant dots from the screen, but has no effect on simulation
  m_Location_x = 0;
  m_Location_y = 0;
#endif

	if (m_OurLandscape->SupplyDayInYear() > October)
	{
		GoodWeather = 0;
		return 1; //TransferToState(Emigration);
	}
	const bool bad = GetBadWeather();
	if (bad && m_OurLandscape->SupplyDayInYear() >= September) return 1; //TransferToState(Emigration);
	return 0; // Carry on
}

//------------------------------------------------------------------------------

bool Skylark_Female::st_Arriving() {
	/**
	Ensures that the bird is not paired on arrival and checks the weather status. If bad weather then returns
	initiates a transition to st_TempLeaveArea otherwise to st_FindTerritory.
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	Paired = false;
	if (GetBadWeather()) return false;
	m_Location_y = static_cast<int>(floor(g_rand_uni_fnc() * m_OurLandscape->SupplySimAreaHeight()));
	m_Location_x = static_cast<int>(floor(g_rand_uni_fnc() * m_OurLandscape->SupplySimAreaWidth()));
	return true; // TransferToState(FindTerritory)
}

//------------------------------------------------------------------------------

bool Skylark_Female::st_Immigrating() {
	/**
	An instantaneous state which determines the chanve of migration mortality. If she does die then she has to inform any old mate that
	she has gone. If he is already paired with another bird then can just forget him. If not dying then breeding success for this year is
	reset and the bird transitions to st_Arrival.
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (g_random_fnc(100) < GetMigrationMortality() || Age > 5)
	{
		if (MyMate != nullptr)
		{
			// if she has no mate she don't care
			// Otherwise he receives a bogus on Mate death message
			if (!MyMate->Paired)
			{
				//Not paired so he is told to forget her
				MyMate->OnMateNeverComesBack(this);
			}
			// He is paired so don't need to tell him anything - forget him
			// or have already told him so forget him anyway
			MyMate = nullptr;
#ifdef __CJTDebug_8
      if ( MyClutch != NULL )
        int rubbish = 0;
#endif
		}
		return false; // TransfertoState(Die)
	}
	ResetBreedingSuccess();
	m_BreedingAttempts = 0;
	m_pesticide_affected = -1; // reset the sprayed flag
	m_MinFemaleAcceptScore = cfg_FemaleMinTerritoryAcceptScore.value();
	return true; // TransferToState(Arrival)
}

//----------------------------------------------------------------------------

int Skylark_Female::st_Emigrating() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
#ifdef __PESTICIDE_RA_ADULT
	// If using reset pesticide affect
	m_pesticide_affected = -1;
#endif
	/**
	Is called once a day. This state determines the return to breeding areas based on probabilities that alter with date. Once suitable
	conditions prevail there will be a transition to st_Immigrating.
	*/
	const int today = m_OurLandscape->SupplyDayInYear();
	if (today >= g_stopdate) return 0; //don't come back this year
	if (m_OurLandscape->SupplyTemp() > 0.0) GoodWeather++;
	else GoodWeather = 0;

	if (GoodWeather >= 7) // 7 days of good weather
	{
		const int chance = g_random_fnc(cfg_ReturnProbability.value()); // number between 0-9999 as default
		if (Age < 1) // if first year
		{
			if (chance < 300 && today >= March + 15) return 1;
			if (today >= April + 15) return 1;
		}
		else
		{
			if (chance < 7 && today >= January + 14) return 1;
			if (chance < 33 && today >= February + 14) return 1;
			if (chance < 400 && today >= March + 14) return 1;
			if (chance < 1000 && today >= April + 14) return 1;
			if (today > May) return 1;
		}
	}
	return 0; // return value of 0 do nothing, 1 go to immigration
}

//------------------------------------------------------------------------------

int Skylark_Female::st_TempLeavingArea() {
	/**
	Waits for one week of weather not classified as bad by Skylark_Adult::GetBadWeather
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing - called once per day
	if (!GetBadWeather()) GoodWeather++;
	else GoodWeather = 0;
	if (GoodWeather >= 7) // Week of not terrible weather
	{
		return 1; // comeback to the area
	}
	return 0; // return value of 0 do nothing, 1 go to Arrival
}

//------------------------------------------------------------------------------

int Skylark_Female::st_Floating() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (GetBadWeather()) return 3;
	m_Location_x = g_random_fnc(m_OurPopulationManager->SupplySimW());
	m_Location_y = g_random_fnc(m_OurPopulationManager->SupplySimH());
	if (m_OurLandscape->SupplyDayInYear() > g_stopdate) { return 2; }
#ifdef FOR_DEMONSTRATION
    m_Location_x = 0;
    m_Location_y = 0;
#endif
	return 1; //TransferToState(FindTerritory);
}

//------------------------------------------------------------------------------

int Skylark_Female::st_Finding_Territory() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (GetBadWeather()) return 3;
	double score = 0;
	skTerritory_struct testTerr;
	// First tries to find old mate if breeding was successful
	if (BSuccess == true && MyMate != nullptr)
	{
		// Check if mate has a territory
		if (MyMate->HaveTerritory == true && MyMate->Paired == false)
		{
			testTerr = MyMate->Supply_Territory();
			// **CJT** Changed below to EvaluateHabitatNest 10/05/04
			//score = m_OurTerritories->EvaluateHabitatNest( m_OurTerritories->Supply_terr( testTerr.ref ), true );
			score = testTerr.nqual;
		}
		if (score > m_MinFemaleAcceptScore)
		{
			//Establish the territory
			EstablishTerritory();
			return 1; // TransferToState(BuildUpResources)
		}
	}
	// If old-mate did not have a good territory or was not there
	// then start a search for a good unoccupied territory
	{
		// Male't territory was not good so lie to him and say we did not
		// come back
		if (MyMate) if (MyMate->Paired == false) MyMate->OnMateNeverComesBack(this);
		MyMate = nullptr; // Forget him he is no good!
		// Sorts through the list of territories until it finds a suitable one
		// will remember the last 'one' until if finds a vacant really good one
		const int NoTerrs = m_OurTerritories->SupplyNoTerritories();
		int remember = -1;
		for (int i = 0; i < NoTerrs; i++)
		{
			Skylark_Male* A_Male = m_OurTerritories->Supply_Owner(i);
			// Is territory occupied?
			if (A_Male != nullptr)
			{
				// Check if a female is already present
				if (m_OurTerritories->Supply_F_Owner(i) == nullptr)
				{
#ifdef __CJTDebug_5
          if ( A_Male->Paired ) {
            g_msg->Warn( WARN_BUG, "Skylark_Female::st_Finding_Territory(): Male already paired!", "" );
            exit( 1 ); // Inconsistency
          }
#endif
					testTerr = A_Male->Supply_Territory();
					/*  *** CJT *** Changed 4/6/2009. Now requires the male to set the value in his m_MyTerritory
					**CJT** Changed below to EvaluateHabitatNest 10/05/04
					 score = m_OurTerritories->EvaluateHabitatNest( m_OurTerritories->Supply_terr( testTerr.ref ), true );
					*/
					if (testTerr.nqual >= m_MinFemaleAcceptScore + 5)
					{
						//Pair with this one
						//Establish the territory
						MyMate = A_Male;
						EstablishTerritory();
						Paired = true;
						return 1; // TransferToState(BuildUpResources)
					}
					if (testTerr.nqual >= m_MinFemaleAcceptScore) remember = i;
				}
			}
		}
		if (remember == -1)
		{
#ifdef FOR_DEMONSTRATION
      m_Location_x = 0;
      m_Location_y = 0;
#endif
			return 2; // TransferToState(Floating);
		}
		//Pair with the male remember
		MyMate = m_OurTerritories->Supply_Owner(remember);
		//Establish the territory
		EstablishTerritory();
		return 1; // TransferToState(BuildUpResources)
	}
}

//------------------------------------------------------------------------------

void Skylark_Female::EstablishTerritory() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	MyMate->OnPairing(this);
	Paired = true;
	MyTerritory = MyMate->Supply_Territory();
	Resources = 0;
	// Move 31-01-2007 to Immigrating m_BreedingAttempts = 0;
	m_OurTerritories->FemaleOccupy(MyTerritory.ref, this);
	m_Location_x = (MyTerritory.size / 2 + MyTerritory.x + 5) % m_OurPopulationManager->SupplySimW(); // next to male
	m_Location_y = (MyTerritory.size / 2 + MyTerritory.y) % m_OurPopulationManager->SupplySimH();
	BSuccess = false; // reset the breeding success flag
}

//------------------------------------------------------------------------------

void Skylark_Female::st_Dying() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (MyMate)
	{
		if (MyMate->MyMate != this)
		{
			g_land->Warn("Skylark_Female::st_Dying - Debug1 ", nullptr);
			exit(1);
		}
		MyMate->OnMateDying();
		MyMate = nullptr;
	}
	if (MyTerritory.ref != -1)
	{
		m_OurTerritories->RemoveFemale(MyTerritory.ref);
		MyTerritory.ref = -1;
	}
	if (MyClutch)
	{
		MyClutch->OnMumGone();
		MyClutch = nullptr;
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1005 );
#endif
	}
	m_CurrentSkState = toss_Destroy; // will kill it at end of step!
	m_CurrentStateNo = -1;
}

//------------------------------------------------------------------------------

void Skylark_Female::OnBroodDeath() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	if (Paired) // if !Paired then ignore them, must have deserted
	{
		// Start a new brood if not already done so naturally
		if (!MyClutch)
		{
			m_Counter1 = 5;
			m_CurrentSkState = toss_StartingNewBrood;
		}
	}
}

//-----------------------------------------------------------------------------

void Skylark_Female::OnClutchDeath() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	if (Paired) // if !Paired then ignore the message, must have deserted
	{
#ifdef __CJTDebug_7
		if (!MyClutch)
		{
			g_land->Warn("Skylark_Female::OnClutchDeat - No clutch to die ", nullptr);
			exit(1);
		}
#endif
		MyClutch = nullptr;
		m_Counter1 = 5;
		m_CurrentSkState = toss_StartingNewBrood; //Go to new brood start
	}
}

//-----------------------------------------------------------------------------

void Skylark_Female::OnEggsHatch() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// This function must not be called until the nestling objects have been
	// created
#ifdef __CJTDebug_7
	if (!MyClutch)
	{
		g_land->Warn("Skylark_Female::OnEggsHatch - No clutch to hatch ", nullptr);
		exit(1);
	}
#endif
	MyClutch = nullptr;
	m_CurrentSkState = toss_EggHatching; //Go to EggHatching
	//  Tell the male
	MyMate->OnEggHatch();
}

//-----------------------------------------------------------------------------

void Skylark_Female::OnMateDying() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	// Assumes that she forgets about any active brood
	MyMate = nullptr;
	if (MyClutch)
	{
		MyClutch->OnMumGone();
		MyClutch = nullptr;
	}
	if (MyTerritory.ref != -1)
	{
		m_OurTerritories->RemoveFemale(MyTerritory.ref);
		MyTerritory.ref = -1;
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1006 );
#endif
	}
	if (m_OurLandscape->SupplyDayInYear() > g_stopdate) m_CurrentSkState = toss_FFlocking;
	else m_CurrentSkState = toss_FFloating;
}

//-----------------------------------------------------------------------------

void Skylark_Female::OnMateHomeless() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	if (MyClutch)
	{
		MyClutch->OnMumGone();
		MyClutch = nullptr;
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1007 );
#endif
	}
	MyMate = nullptr;
	Paired = false;
	BSuccess = false;
	MyTerritory.ref = -1;
	if (m_OurLandscape->SupplyDayInYear() > g_stopdate) m_CurrentSkState = toss_FFlocking;
	else m_CurrentSkState = toss_FFloating;
}

//-----------------------------------------------------------------------------

void Skylark_Female::OnMaleNeverComesBack(const Skylark_Male* AMale) {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	if (MyMate == AMale)
	{
		MyMate = nullptr;
		Paired = false;
		BSuccess = false;
	} //This could cause problems if this function is called late
}

//-----------------------------------------------------------------------------

int Skylark_Female::Supply_BreedingAttempts() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	return m_BreedingAttempts;
}

//-----------------------------------------------------------------------------

int Skylark_Female::Supply_BreedingSuccess() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	return m_BreedingSuccess;
}

//-----------------------------------------------------------------------------

void Skylark_Female::ResetBreedingSuccess() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0xDEADC0DE )
    DEADCODEError();
#endif
	m_BreedingSuccess = -1;
}

//-----------------------------------------------------------------------------

int Skylark_Female::st_BuildingUpResources() {
	/**
	* The female forages from her home range each day. On 1st April she will make a transition to preparing for breeding
	* which determines the time needed for egg production and nest building.<br>
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	const int today = m_OurLandscape->SupplyDayInYear();
	if (today >= g_stopdate)
	{
		return 2; // GiveUpTerritory
	}
	if (GetBadWeather()) return 2;
	if (m_OurLandscape->SupplyTemp() > Breed_Temp_Thresh)
	{
		const double qual = MyMate->Supply_TerritoryQual();
		if (qual < m_MinFemaleAcceptScore)
		{
			// May need to give up territory or might be because the crop has not
			// started growing in which case she will wait until May
			if (today >= May + 14) return 2; // GiveUpTerritory
		}
		EggCounter = 0;
		if (today > cfg_SkylarkFirstBreedingDate.value()) return 1; // transferToState(PrepareForBreeding)
	}
	return 0; // just wait
}

//-----------------------------------------------------------------------------

int Skylark_Female::st_PreparingForBreeding() {
	/**
	* Builds up resources whilst waiting for good enough weather to begin nest building.
	* Transitions  to MakingNest or GiveUpTerritory (if too late in the season).
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing: Called once per day
	// EggCounter was set to Zero on leaving building up resources
	// Egg counter is the refraction period for producing the
	// eggs - nesbuilding time
	long today = m_OurLandscape->SupplyDayInYear();
	if (today > g_stopdate) return 2; // too late give up
	Resources += RemoveEM(GetFood(m_OurLandscape->SupplyDaylength()));
	if (EggCounter++ > 5)
	{
		//    if ( ( Resources < Breed_Res_Thresh2 ) || ( m_OurLandscape->SupplySnowcover() ) == true ) return 0; // snow or not enough resources
		if (m_OurLandscape->SupplySnowcover() == true) return 0; // snow
		// Check that the temp is above 5 for the last three days
		today = m_OurLandscape->SupplyGlobalDate();
		double t = m_OurLandscape->SupplyTemp(today);
		if (t < 5.0) return 0; // too cold
		t = m_OurLandscape->SupplyTemp(today - 1);
		if (t < 5.0) return 0; // too cold
		t = m_OurLandscape->SupplyTemp(today - 2);
		if (t < 5.0) return 0; // too cold
		// Everything is OK so build
		// takes one day less to build nest on first attempt (3 rather than 4)
		if (m_BreedingAttempts > 0) m_Counter1 = 4;
		else { m_Counter1 = 3; }
		return 1; // TransferToState(MakeNest)
	}
	return 0;
}
//-----------------------------------------------------------------------------

TTypesOfSkState Skylark_Female::st_MakingNest() {
	/**
	* Tests for the necessary territory quality, if OK finds a nest location and builds the nest. Nest attempts are assumed to be
	* breeding attempts.
	*/
#ifdef __PESTICIDE_RA_ADULT
	// If pesticide affected then breeding is delayed
	if (m_pesticide_affected > -1)
	{
		if (m_OurLandscape->SupplyDayInYear() - m_pesticide_affected < 10)
		{
			NestLoc = false;
			return toss_PreparingForBreeding;
		}
	}
#endif

	if (MyMate == nullptr)
	{
		g_msg->Warn(WARN_BUG, "Skylark_Female::st_MakingNest(): No mate!", "");
		exit(1);
	}
	const int today = m_OurLandscape->SupplyDayInYear();
	if (!NestLoc)
	{
		// First make a test whether territory is still OK
		const double qual = MyMate->Supply_TerritoryQual();
		if (qual < m_MinFemaleAcceptScore)
		{
			// May need to give up territory or might be because the crop has not
			// started growing in which case she will wait until May
			if (today >= June)
			{
				return toss_GivingUpTerritory; // GiveUpTerritory
			}
			return toss_MakingNest;
		}

		// Everything OK so find a nest location
		const bool valid = MyMate->SupplyNestValid();
		if (valid)
		{
			const APoint p = MyMate->SupplyNestLoc();
			m_Location_x = p.m_x;
			m_Location_y = p.m_y;
			// Must tell the male
			if (MyMate) MyMate->OnNestLocation(p.m_x, p.m_y);
		}
		else
		{
			if (today >= May + 14)
			{
				return toss_GivingUpTerritory; // GiveUpTerritory
			}
			return toss_MakingNest;
		}
		// The line below counts breeding attempts as nest building
		// the alternative is to count this from one egg laid in St_Laying()
		if (m_BreedingSuccess == -1) m_BreedingSuccess = 0; //Signal breeding was possible
		//m_BreedingAttempts++;
	}
	// Nest location must exist
	if (m_Counter1-- > 0) return toss_MakingNest;
	m_Counter1 = -1; // Code for deciding many days before laying starts
	return toss_Laying; //TransferToState Laying;
}
//-----------------------------------------------------------------------------

TTypesOfSkState Skylark_Female::st_Laying() {
	if (MyMate == nullptr)
	{
		g_msg->Warn(WARN_BUG, "Skylark_Female::st_Laying(): No mate!", "");
		exit(1);
	}
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Timing called Once each day
	// lays 1 egg per day until the correct number of eggs are laid
	// Counter1 must be set to -1 on first time called
	if (m_Counter1 == -1)
	{
		// If we have not calculated how many eggs we need do that
		m_Counter1 = CalculateEggNumber();
		m_EggNumber = m_Counter1;
#ifdef __PESTICIDE_RA_ADULT
		// If pesticide affected then breeding is delayed
		if (m_pesticide_affected > -1)
		{
			if (m_OurLandscape->SupplyPesticideType() == ttop_ReproductiveEffects)
			{
				if (m_OurLandscape->SupplyDayInYear() - m_pesticide_affected < 10) return toss_PreparingForBreeding;
			}
			else if (m_OurLandscape->SupplyPesticideType() == ttop_eggshellthinning)
			{
				if (cfg_skylark_pesticide_eggshellreduction_perclutch.value())
				{
					if (g_rand_uni_fnc() <= cfg_skylark_pesticide_eggshellreduction.value()) m_EggNumber = 0;
				}
				else
				{
					// This code assumes egg independence
					for (int e = 0; e < m_EggNumber; e++)
					{
						if (g_rand_uni_fnc() <= cfg_skylark_pesticide_eggshellreduction.value()) m_EggNumber--;
					}
				}
				if (m_EggNumber < 1)
				{
					m_BreedingAttempts++;
					return toss_PreparingForBreeding;
				}
			}
			m_Counter1 = m_EggNumber;
			// this is needed in case we adjusted the egg number (m_counter is used to count down the number of days of laying.
		}
#endif
		// This code is a special test for egg shell thickness effects - it can be linked to the pesticide effect above, or assumed to be 
		// a general effect
		if (cfg_skylark_pesticide_globaleggshellreduction.value() > 0.0 && m_OurLandscape->SupplyYearNumber() > 9)
		{
			/*		// This code assumes egg independence
					for (int e = 0; e < m_EggNumber; e++)
					{
						if (g_rand_uni() <= cfg_skylark_pesticide_eggshellreduction.value()) m_Counter1--;
					}
					m_EggNumber = m_Counter1;
			*/
			// this code assumes clutch effects of thin egg shells
			if (g_rand_uni_fnc() <= cfg_skylark_pesticide_globaleggshellreduction.value()) m_EggNumber = 0;
			if (m_EggNumber < 1)
			{
				m_BreedingAttempts++;
				return toss_PreparingForBreeding;
			}
		}

		// The line below counts breeding attempts as at least one egg
		m_BreedingAttempts++;
		auto cs = new skClutch_struct;
		cs->L = m_OurLandscape;
		cs->Mum = this;
		cs->No = 0;
		cs->Terrs = m_OurTerritories;
#ifdef FOR_DEMONSTRATION
    cs->x = m_Location_x + 3;
    cs->y = m_Location_y + 3;
#else
		cs->x = m_Location_x;
		cs->y = m_Location_y;
#endif
		cs->bx = m_Location_x;
		cs->by = m_Location_y;
		// This sets the veg type for ever for the life of this nest and any eggs it contains
		cs->mh = m_OurLandscape->SupplyVegType(m_Location_x, m_Location_y);
		if (cs->x >= m_OurPopulationManager->SupplySimW()) cs->x -= m_OurPopulationManager->SupplySimW();
		if (cs->y >= m_OurPopulationManager->SupplySimH()) cs->y -= m_OurPopulationManager->SupplySimH();
		m_OurPopulationManager->CreateObjects(0, this, nullptr, cs, 1);
		delete cs;
	}
	// Put an egg in the clutch
	MyClutch->AddEgg();
	if (--m_Counter1 == 0)
	{
		// All eggs must be laid
		m_Counter1 = 18; // This will count how many day spent incubating
		MyClutch->StartDeveloping();
		return toss_Incubating; //TransferToState(Incubation);
	}
	return toss_Laying;
}

//-----------------------------------------------------------------------------

int Skylark_Female::st_GivingUpTerritory() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// If she is incubating give a message to the clutch that she is gone
	if (MyClutch)
	{
		MyClutch->OnMumGone();
		MyClutch = nullptr;
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1008 );
#endif
	}
	// Will carry on feeding the young whilst they are needing fed
	if (MyMate->SupplyBroodSize() > 0)
	{
		FeedYoung();
		return 0; // carry on
	}
	// No young to look after so leave the territory
	EggCounter = 0;
	NestLoc = false;
	// No longer paired
	Paired = false;
	// Tell the male you are leaving
	MyMate->OnMateLeaving();
	// if no breeding success then forget him
	if (!BSuccess) MyMate = nullptr;
	// de-register the territory ownership (if she owns one)
	if (MyTerritory.ref != -1) m_OurTerritories->RemoveFemale(MyTerritory.ref);
	MyTerritory.ref = -1; // No territory
	return 1; // TransferToState(Floating);
}

//-----------------------------------------------------------------------------

int Skylark_Female::st_StartingNewBrood() {
	/**
	* Any current nests or clutches are removed. The female assesses the habitat quality of the territory. If still suitable (i.e. above MTQ)
	* she will make a transition to Make Nest, otherwise she will go to Give Up Territory.<br>
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	NestLoc = false;
	if (MyClutch)
	{
		MyClutch->OnMumGone();
		MyClutch = nullptr;
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1009 );
#endif
	}
	const double qual = MyMate->Supply_TerritoryQual();
	if (qual < m_MinFemaleAcceptScore)
	{
		return 2; // transferToState(GiveUpTerritory)
	}
	if (m_BreedingAttempts < 4 && m_OurLandscape->SupplyDayInYear() < g_stopdate)
	{
		// Was July-4
		if (m_BreedingAttempts > 0) m_Counter1 = 4;
		else m_Counter1 = 3;
		m_toowet = 0;
		return 1; // TransferToState(MakeNest)
	}
	st_StoppingBreeding();
	return 3; // Transfer to state flocking
}
//-----------------------------------------------------------------------------

int Skylark_Female::st_Incubating() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	/**
	* Incubation occurs as described in development below. The female spends that time off
	* the nest required to find energy to cover her basal metabolic requirements, plus that energy required to warm the eggs.
	* Incubation continues until the eggs hatch and there is a transition to Care For Young,
	* or the incubation period (MID) is exceeded, at which point the female will Start New Brood.
	* This state can only be left by a call to OnEggHatch being altered on creation of a nestling or to StartNewBrood
	*/
	// Must calculate time needed to feed
	// will ignore time off nest due to disturbance
	m_NestTime = 0;
	if (--m_Counter1 > 0)
	{
		// ExtraBroodHeat extra because of heat for eggs
		const double calories_needed = m_OurPopulationManager->SupplyEMi();
		int foodtime = CalcFoodTime(calories_needed);
		const int Daylength = m_OurLandscape->SupplyDaylength();
		const double rain = m_OurLandscape->SupplyRain();
		if (rain > MaxFeedRain) { m_toowet++; }
		if (m_toowet > 2)
		{
			//m_OurPopulationManager->WriteToTest2File( m_OurLandscape->SupplyDayInYear(), 1003 );
			return 3; //TransferToState(StartNewBrood);
		}
		if (foodtime > Daylength) foodtime = Daylength;
		m_NestTime = 24 * 60 - foodtime; //add night time
		return 0; // wait
	}
	//m_OurPopulationManager->WriteToTest2File( m_OurLandscape->SupplyDayInYear(), 1002 );
	return 3; // TransferToState(StartNewBrood);
}
//-----------------------------------------------------------------------------

int Skylark_Female::st_EggHatching() {
	/**
	*  A transition is made to Care for Young.<br>
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	m_toowet = 0;
	return 1; // TransferToState(CareForYoung)
}

//-----------------------------------------------------------------------------

int Skylark_Female::st_StoppingBreeding() {
	/**
	* Called when the female stops breeding for the year. Removes the pair bond and tell the male she is leaving.
	* She deregisters her territory ownership and if there has been no breeding success then she forgets the male.
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	if (MyClutch)
	{
		MyClutch->OnMumGone();
		MyClutch = nullptr;
#ifdef __SKPOM
    m_OurPopulationManager->WriteSKPOM1( m_OurLandscape->SupplyDayInYear(), 1010 );
#endif
	}
	// No longer paired
	Paired = false;
	// Tell the male you are leaving
	MyMate->OnMateLeaving();
	// if no breeding success then forget him
	if (!BSuccess) MyMate = nullptr;
	// de-register the territory ownership
	m_OurTerritories->RemoveFemale(MyTerritory.ref);
	MyTerritory.ref = -1; // No territory
	// TransferToState(Flocking)
	return 3;
}
//-----------------------------------------------------------------------------

int Skylark_Female::st_CaringForYoung() {
	/**
	* Calls Skylark_Female::FeedYoung to get food and give it to the chicks. Once the chicks reach 18 days of age if more breeding is possible
	* then the bird will start a new brood unless it is late in the season or there have been too many breeding attempts in which case she will
	* stop breeding when the chicks are 30 days old.
	* In previous versions there was a bad weather component, but this has been removed since it did not contribute to the
	* POM fit. If future analysis finds a relationship it should be incorporated here.
	*/
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	FeedYoung(); // get food and give it to the young
	const int ch_age = MyMate->SupplyBroodAge(0);
	if (ch_age == 18)
	// Must stop feeding and start to build a nest. She is ready to
	// build nest when the young are 18 days old
	{
		if (m_BreedingAttempts < 4 && m_OurLandscape->SupplyDayInYear() < g_stopdate)
		{
			NestLoc = false; // no nest location now
			return 1; //Start a new brood
		}
		if (ch_age == 30) return 2;
	}
	return 0;
}
//-----------------------------------------------------------------------------

void Skylark_Female::FeedYoung() {
#ifdef __CJTDebug_5
	if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	/**
	Can only get food in daylight - dawn & dusk.
	Must keep them warm if the weather is bad - the quick version below catagorizes this into
	good or bad weather
	*/
	const double RainToday = m_OurLandscape->SupplyRain();
	if (RainToday > MaxFeedRain) return; // No feeding today
	const double TempToday = m_OurLandscape->SupplyTemp();
	const int food_time = m_OurLandscape->SupplyDaylength() - 120;
	const int BrSize = MyMate->SupplyBroodSize();
	m_NestTime = 24 * 60 - food_time; //add night time
	int FeedingProbs[16];
	double ChickFood[16];
#ifdef __PESTICIDE_RA_CHICK
	double ChickPoison[ 16 ];
#endif
	double spare;
	const double resources_all = GetFood(food_time /*, pcide*/); // kcal insects
	// remove metabolic requirements
	double resources = RemoveEM(resources_all);
	int HungryChicks = BrSize;
	if (resources > 0)
	{
		// Zero the feeding probabilities
		for (int i = 0; i < HungryChicks; i++)
		{
			FeedingProbs[i] = 0;
			ChickFood[i] = 0;
#ifdef __PESTICIDE_RA_CHICK
			ChickPoison[ i ] = 0;
#endif
		}
		// if the chicks are 1-4 days old then she must warm them with energy
		// From Mertens (1972)
		// M = 0.0719*power(brood_weight,0.613)*(BroodTemp-AmbientTemp); // Brood temp = 35 double broodweight=0;
		int BroodWeight = 0;
		for (int i = 0; i < HungryChicks; i++)
		{
			// Add up brood weight
			const int weight = static_cast<int>(floor(MyMate->SupplyBroodWeight(i) + 0.5));
			BroodWeight += weight;
			// While we are at it we might as well create the FeedinbProbs array too
			for (int j = i; j < HungryChicks; j++) FeedingProbs[j] += static_cast<int>(floor(0.5 + weight));
		}
		if (MyMate->SupplyBroodAge(0) < 6) { resources -= pow(BroodWeight, 0.613) * 0.0719 * (35 - TempToday); }
		// Now give the food to the chicks
		// The chance that a chick get a portion is proportional to their size
		resources /= FoodTripsPerDay; // Split into 30 portions (food trips per day)
#ifdef __PESTICIDE_RA_CHICK
		double pcide_remain = resources * m_pcide_conc;
#endif
		while (resources > 0.01 && HungryChicks > 0)
		{
			for (int portion = 0; portion < FoodTripsPerDay; portion++)
			{
				// For each portion
				const int choose = g_random_fnc(BroodWeight);
				for (int i = 0; i < BrSize; i++)
				{
					if (choose < FeedingProbs[i])
					{
						ChickFood[i] += resources; // store the food for that chick
#ifdef __PESTICIDE_RA_CHICK
						ChickPoison[ i ] += pcide_remain;
#endif
						break;
					}
				}
			}
			// Now give the food to the chicks
			// Better do this than feed each a portion at a time, it is more efficient
			// and stops problems with growth between portions
			spare = 0;
			for (int i = 0; i < BrSize; i++)
			{
#ifdef __PESTICIDE_RA_CHICK
				double extra = MyMate->OnFoodMessage( i, ChickFood[ i ] , ChickPoison[i] );
#else
				double extra = MyMate->OnFoodMessage(i, ChickFood[i]);
#endif
				if (extra > 0) // chick must be full
				{
					FeedingProbs[i] = -1; // no chance of food
					HungryChicks--;
					spare += extra;
				}
			}
			resources = spare / FoodTripsPerDay;
		}
	}
}
//-----------------------------------------------------------------------------

double Skylark_Female::GetFood(int time /*, double & pcide */) {
	// returns insects in g dry weight
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Extract insects available from each habitat in the male's table
	// for as long as she spends searching
	double food = 0;
	int index = 0;
#ifdef __PESTICIDE_RA
	m_pcide = 0.0;
#endif
	double fd;
	while (time > 0)
	{
#ifdef __CJTDebug_4
    if ( index > 249 )
      OnArrayBoundsError();
#endif
		const int area = MyMate->m_HabitatTable_Size[index];
		if (time > area)
		{
			time -= area;
			// Food+= area*WeatherHindrance*ExtractionEfficiency*Insects per m2
			fd = area * MyExtractEff * MyMate->m_InsectTable[index];
			food += fd;
#ifdef __PESTICIDE_RA
			// m_pcide has total mg collected
			m_pcide += MyMate->m_PConcTable[index] * fd * KcalPerGInsect_kg_inv; // Food needs converting from kcal to kg
			//m_pcide += 53.0 * fd * KcalPerGInsect_kg_inv; // Food needs converting from kcal to kg
#endif
			index++;
			if (index == MyMate->SupplyNoHabitatRefs()) time = 0; // Nowhere left to look
		}
		else
		{
			// Food+=
			// minutes left*WeatherHindrance*ExtractionEfficiency*Insects per m2
			fd = time * MyExtractEff * MyMate->m_InsectTable[index];
			food += fd;
#ifdef __PESTICIDE_RA
			m_pcide += MyMate->m_PConcTable[index] * fd * KcalPerGInsect_kg_inv; // Food needs converting from kcal to kg
			//m_pcide += 53.0 * fd * KcalPerGInsect_kg_inv; // Food needs converting from kcal to kg
#endif
			time = 0;
		}
	}
	return food;
}

//-----------------------------------------------------------------------------

int Skylark_Female::CalculateEggNumber() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Data from PO/AP study at Kal
	int eggs;
	int chance;
	switch (m_BreedingAttempts)
	{
	case 1:
		chance = g_random_fnc(160);
		if (chance < 9) eggs = 2;
		else if (chance < 37 + 9) eggs = 3;
		else if (chance < 99 + 37 + 9) eggs = 4;
		else eggs = 5;
		break;
	default:
		chance = g_random_fnc(125);
		if (chance < 29) eggs = 3;
		else if (chance < 29 + 80) eggs = 4;
		else eggs = 5;
		break;
	}
	if (eggs < 1) eggs = 1;
	return eggs;
}

//---------------------------------------------------------------------------
int Skylark_Female::CalcFoodTime(const double target) const
// FoodTime is in minutes
{
	if (MyMate == nullptr)
	{
		g_msg->Warn(WARN_BUG, "Skylark_Female::CalcFoodTime(): No Mate!", "");
		exit(1);
	}

#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	// Must calcuate how much time it takes to get 'target' food
	double FoodGathered = 0.0;
	int FoodTime = 0; // How much time used
	int used = 0;
	// Starting with the best sources first, use time to get food
	// The male ought to have sorted the table already (or yesterday)
	while (FoodGathered < target && used < MyMate->SupplyNoHabitatRefs())
	{
		double available = MyMate->m_HabitatTable_Size[used] * MyMate->m_InsectTable[used];
		// calcualte for, weather and extraction rate
		available *= MyExtractEff;
		if (available < target - FoodGathered)
		{
			FoodGathered += available;
			//used++;
			// Calc time used
			FoodTime += MyMate->m_HabitatTable_Size[used++]; // assume 1 min per m2
		}
		else
		{
			// Enough Food here to reach target
			// foodpermin = available/area * ExtractionEff * VegHindrance
			// mins = (target-FoodGathered)/FoodperMin
			const double FoodPerMin = available / MyMate->m_HabitatTable_Size[used];
			FoodTime += static_cast<int>(floor(0.5 + (target - FoodGathered) / FoodPerMin));
			break;
		}
	}
	return FoodTime;
}

//------------------------------------------------------------------------------

int Skylark_Female::GetMigrationMortality() {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif
	Age++; // Get one year older (if you live)
	if (Age == 1) return JuvenileReturnMort;
	return m_OurPopulationManager->SupplyF_Mig_Mort();
}

//------------------------------------------------------------------------------

double Skylark_Female::CheckForFields() const {
#ifdef __CJTDebug_5
  if ( IsAlive() != 0x0DEADC0DE ) DEADCODEError();
#endif

	// Checks each 1m in the territory and returns the proportion of field<4cm
	// Note only arable fields are returned as field - permanent pasture is not
	int TallFieldVeg = 0;
	for (int xx = MyTerritory.x; xx < MyTerritory.x + MyTerritory.size; xx++)
	{
		for (int yy = MyTerritory.y; yy < MyTerritory.y + MyTerritory.size; yy++)
		{
			int Cx = xx;
			int Cy = yy;
			m_OurLandscape->CorrectCoords(Cx, Cy);
			const int PolyRef = m_OurLandscape->SupplyPolyRef(Cx, Cy);
			if (m_OurLandscape->SupplyVegHeight(PolyRef) > 50.0 && (m_OurLandscape->SupplyElementType(PolyRef) == tole_Field || m_OurLandscape->
				SupplyElementType(PolyRef) == tole_UnsprayedFieldMargin)) TallFieldVeg++;
		}
	}
	return TallFieldVeg / static_cast<double>(MyTerritory.size * MyTerritory.size);
}
//-----------------------------------------------------------------------------

void Skylark_Adult::CopyMyself(const int a_sktype) {
	const auto aps = new AdultSkylark_struct;
	aps->x = m_Location_x;
	aps->y = m_Location_y;
	aps->size = m_Size;
	aps->Terrs = m_OurTerritories;
	aps->L = m_OurLandscape;
	aps->sex = false;
	aps->age = 0;
	aps->bx = m_Born_x;
	aps->mh = m_MyHome;
	aps->by = m_Born_y;
	m_OurPopulationManager->CreateObjects(a_sktype, this, nullptr, aps, -1);
	// object will be destroyed by death state
	// but must let Dad know anyway
	delete aps;
}
//-----------------------------------------------------------------------------

void Skylark_Female::SensibleCopy() {
	// This is called if a new bird is made as a copy of another and needs to have some sensible information added
	m_CurrentSkState = toss_FEmigrating;
}
//-----------------------------------------------------------------------------

void Skylark_Male::SensibleCopy() {
	// This is called if a new bird is made as a copy of another and needs to have some sensible information added
	m_CurrentSkState = toss_MEmigrating;
}
//-----------------------------------------------------------------------------
#ifdef __PESTICIDE_RA

void Skylark_Nestling::PesticideResponse(void) {
	if (m_pesticide_accumulation / m_Size > cfg_Skylark_nestling_NOEL.value())
	{
		// NOEL triggered, so we need to die
		m_CurrentSkState = toss_NDying;
	}
	m_pesticide_accumulation *= cfg_Skylark_nestling_Biodegredation.value();
}

void Skylark_PreFledgeling::PesticideResponse() {
	if (m_pesticide_accumulation > cfg_Skylark_prefledegling_NOEL.value())
	{
		// NOEL triggered, so we need to die
		m_CurrentSkState = toss_PDying;
	}
	m_pesticide_accumulation *= cfg_Skylark_prefledegling_Biodegredation.value();
}

void Skylark_Male::PesticideResponse() {
	if (m_pesticide_accumulation > cfg_Skylark_male_NOEL.value())
	{
	}
	m_pesticide_accumulation *= cfg_Skylark_male_Biodegredation.value();
}

void Skylark_Female::PesticideResponse() {
	if (m_pesticide_accumulation * 1000 / m_Size > cfg_Skylark_female_NOEL.value())
	{
		// * 1000 to go from g to kg
		m_pesticide_affected = 1;
	}
	m_pesticide_accumulation *= cfg_Skylark_female_Biodegredation.value();
}
#endif
//-----------------------------------------------------------------------------
