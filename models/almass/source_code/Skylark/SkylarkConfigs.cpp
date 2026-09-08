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

/******************************************************************************/
/**************   Skylark Configuration (cfg) Parameters   ********************/
/******************************************************************************/
#include "../Landscape/ls.h"

 /** \brief The number of skylarks that start in the simulation */
 CfgInt cfg_SkStartNos("SK_STARTNOS",CFG_CUSTOM,6000);
 //****************  Clutch/Egg Related
 /** \brief The time taken to egg hatch under optimal conditions */
 CfgFloat   cfg_MinDaysToHatch("SK_MINDAYSTOHATCH",CFG_CUSTOM,10.2);
 //****************  Nestling Energetics
 CfgFloat   cfg_PEmax( "SK_PEMAX",CFG_CUSTOM,4.54); 
 CfgInt     cfg_NestLeavingChance("SK_NESTLEAVECHANCE",CFG_CUSTOM,23);
 CfgFloat   cfg_NestLeavingWeight("SK_NEST_LEAVING_WEIGHT",CFG_CUSTOM,20.5);
//**************** Prefledgeling Energetics
 CfgInt     cfg_fecundity_reduc("SK_FECUNDITY_REDUC",CFG_CUSTOM,0);
 CfgInt     cfg_fecundity_reduc_chance("SK_FECUNDITY_REDUC_CHANCE",CFG_CUSTOM,0); //0-100
 CfgInt     cfg_insecticide_direct_mortE("SK_INSECTICDEDIRECTMORTE",CFG_CUSTOM,0);
 CfgInt     cfg_insecticide_direct_mortN("SK_INSECTICDEDIRECTMORTN",CFG_CUSTOM,0);
 CfgInt     cfg_insecticide_direct_mortP("SK_INSECTICDEDIRECTMORTP",CFG_CUSTOM,0);
 CfgInt     cfg_insecticide_direct_mortM("SK_INSECTICDEDIRECTMORTM",CFG_CUSTOM,0);
 CfgInt     cfg_insecticide_direct_mortF("SK_INSECTICDEDIRECTMORTF",CFG_CUSTOM,0);
// Weight at which the nestlings leave the nest
 CfgInt     cfg_ClutchMortProb( "SK_CLUTCH_MORT_PROB",CFG_CUSTOM,350);
 CfgInt     cfg_NestlingMortProb( "SK_NEST_MORT_PROB",CFG_CUSTOM,23); 
 CfgInt     cfg_PreFledgeMortProb( "SK_PREFLEDGE_MORT_PROB",CFG_CUSTOM,50);
 CfgFloat   cfg_MeanHatchingWeight("SK_MEAN_HATCHING_WEIGHT",   CFG_CUSTOM,3.23);
 /** \brief Immigration mortality for juveniles */
 CfgInt     cfg_juvreturnmort("SK_JUVRETURNMORT",CFG_CUSTOM,35);
 /** \brief Immigration mortality for juveniles */
 CfgInt     cfg_adultreturnmort("SK_ADULTRETURNMORT",CFG_CUSTOM,35);
 /** \brief Used to change the timing of start of arrival in the landscape */
 CfgFloat	cfg_ReturnProbability("SK_RETURNPROBABILITY", CFG_CUSTOM, 10000);
 /** \brief The date at which breeding start can happen */
CfgInt		cfg_SkylarkFirstBreedingDate("SK_FIRSTBREEDINGDATE", CFG_CUSTOM, April);
 /** \brief Maximum immigration mortality for females */
 //CfgInt     cfg_femreturnmortmax("SK_FEMRETURNMORTMAX",CFG_CUSTOM,50);
 /** \brief Minimum immigration mortality for females */
 //CfgInt     cfg_femreturnmortmin("SK_FEMRETURNMORTMIN",CFG_CUSTOM,50);
 /** \brief Maximum immigration mortality for males */
 //CfgInt     cfg_malereturnmortmax("SK_MALERETURNMORTMAX",CFG_CUSTOM,50);
 /** \brief Minimum immigration mortality for males */
 //CfgInt     cfg_malereturnmortmin("SK_MALERETURNMORTMIN",CFG_CUSTOM,50);

// From Skylark_Clutch
// taken as 6.204*eggweight^-0.3965 * correction = 2.9 per hr per degree cooler
// correction = 0.75
// data from Avian Energetics, Kendeigh et al.
// 0.75 because cooling rate does not take into account nest or other eggs
// cooling rate of mallard was 50% of the above due to eggs nest so use 75%
// because skylark is smaller
 CfgFloat   cfg_Cooling_Rate_Eggs( "SK_COOLING_RATE_EGGS",CFG_CUSTOM,3.0);
 CfgFloat   cfg_EggTemp( "SK_EGGTEMP",CFG_CUSTOM,36.1);
 CfgFloat   cfg_MD_Threshold( "SK_MD_THRESHOLD",CFG_CUSTOM,25.8);
 CfgFloat   cfg_MeanExtractionRatePerMinute( "SK_EXTRACTION_RATE",CFG_CUSTOM, 0.0020948); //0.00125, 0.00265
 CfgInt     cfg_FoodTripsPerDay( "SK_FOOD_TRIPS_PER_DAY",CFG_CUSTOM,30);
 CfgFloat	cfg_sk_triplength("SK_TRIPLENGTH",CFG_CUSTOM,10.5);
 CfgInt     cfg_Breed_Res_Thresh1( "SK_BREED_RES_THRESH",CFG_CUSTOM,600);
 
/******************************************************************************/
 CfgFloat   cfg_ConversionEffReduc("SK_CONVEFFREDEC",CFG_CUSTOM,0.023077);
 CfgFloat   cfg_EM_Nestling_a("SK_EM_NESTLING_A",CFG_CUSTOM,0.8542);
 CfgFloat   cfg_EM_Nestling_b("SK_EM_NESTLING_B",CFG_CUSTOM,1.353);
/******************************************************************************/
CfgInt  cfg_strigling_preflg("SK_STRIGLING_PF",CFG_CUSTOM,72);
CfgInt  cfg_strigling_nestling("SK_STRIGLING_N",CFG_CUSTOM,72);
CfgInt  cfg_strigling_clutch("SK_STRIGLING_C",CFG_CUSTOM,72);

CfgFloat cfg_maxfeedrain("SK_MAXFEEDRAIN",CFG_CUSTOM, 10.0); // 4.7
CfgInt cfg_HQualityOpenTallVeg("SK_HQOOPENTALLVEG", CFG_CUSTOM, 16);
CfgInt cfg_HQualityTall("SK_HQTALL", CFG_CUSTOM, -2);
CfgInt cfg_HQualityTall2("SK_HQTALLVEGTWO", CFG_CUSTOM, -1000);
CfgInt cfg_HQualityTrack("SK_HQTRACK", CFG_CUSTOM, 10);
CfgInt cfg_HQualityMetalRoad("SK_HQMETALROAD", CFG_CUSTOM, -10);
CfgInt cfg_HQualityHedgeScrub("SK_HQHEDGESCRUB", CFG_CUSTOM, 0);
CfgInt cfg_HQualityNeutral("SK_HQNEUTRAL", CFG_CUSTOM, 0);
CfgInt cfg_HQualityHedge("SK_HQHEDGE", CFG_CUSTOM, -250);
CfgInt cfg_HQualityTallVeg("SK_HQTALLVEG", CFG_CUSTOM, -10);
CfgInt cfg_HQualityWater("SK_HQWATER", CFG_CUSTOM, 0);
CfgInt cfg_HQualityVeg30cm("SK_HQVEGTHIRTYCM", CFG_CUSTOM, 11);
CfgInt cfg_HQualityBareEarth("SK_HQBAREEARTH", CFG_CUSTOM, 3);
CfgInt cfg_PatchyPremium("SK_PATCHYPREMIUM", CFG_CUSTOM, 47);
CfgFloat cfg_SkScrapesPremiumII("SK_SKSCRAPESPREMIUMNEST", CFG_CUSTOM, 0.24);
CfgFloat cfg_tramline_foraging("SK_TRAMLINE_FORAGING_PROP",CFG_CUSTOM,0.45);
CfgInt cfg_SkTramlinesPremium("SK_TRAMLINEPREMIUM", CFG_CUSTOM, 6);

CfgFloat cfg_MaleSplitScale("SK_MALESPLITSCALE",CFG_CUSTOM,0.5);
CfgInt cfg_temphindpow("SK_TEMPHINDPOW",CFG_CUSTOM,3);
CfgInt cfg_rainhindpow("SK_RAINHINDPOW",CFG_CUSTOM,4);

CfgFloat cfg_hindconstantH_b("SK_HINDCONSTH_B",CFG_CUSTOM,-0.025);
CfgFloat cfg_hindconstantD_b("SK_HINDCONSTD_B", CFG_CUSTOM,-0.022);

CfgFloat cfg_densityconstant_a("SK_DENSITYCONST_A",CFG_CUSTOM,0);
CfgFloat cfg_densityconstant_b("SK_DENSITYCONST_B", CFG_CUSTOM,-0.26);
CfgInt   cfg_densityconstant_c("SK_DENSITYCONST_C", CFG_CUSTOM,10);
CfgFloat cfg_heightconstant_a("SK_HEIGHTCONST_A", CFG_CUSTOM,1);
CfgFloat cfg_heightconstant_b("SK_HEIGHTCONST_B", CFG_CUSTOM,-0.22);
CfgInt   cfg_heightconstant_c("SK_HEIGHTCONST_C", CFG_CUSTOM,38);
CfgFloat cfg_FemaleMinTerritoryAcceptScore ("SK_MINFEMACCEPTSCORE", CFG_CUSTOM, 300000);
CfgFloat cfg_NestPlacementMinQual("SK_NESTPLACEMENTMINQUAL",CFG_CUSTOM,15);

 //****** PESTICDE RELATED INPUT PARAMETERS HERE  ******
 /** \brief Can be used to trigger a response to pesticides for the nestlings */
 CfgFloat cfg_Skylark_nestling_NOEL("SK_NESTLING_NOEL",CFG_CUSTOM,0.001);
 /** \brief The proportion of pesticide accumulated from one day to the next */
 CfgFloat cfg_Skylark_nestling_Biodegredation("SK_NESTLING_BIODEG",CFG_CUSTOM,0.0);
 /** \brief Used for determining the pesticide response for prefledglings */
 CfgFloat cfg_Skylark_prefledegling_NOEL("SK_FLEDGE_NOEL",CFG_CUSTOM,0.001);
 /** \brief The proportion of pesticide remaining from one day to the next for prefledglings */
 CfgFloat cfg_Skylark_prefledegling_Biodegredation("SK_FLEDGE_BIODEG",CFG_CUSTOM,0.0);
 /** \brief Can be used to trigger a response to pesticides for the males */
 CfgFloat cfg_Skylark_male_NOEL("SK_MALE_NOEL",CFG_CUSTOM,0.001);
 /** \brief The proportion of pesticide remaining from one day to the next for males */
 CfgFloat cfg_Skylark_male_Biodegredation("SK_MALE_BIODEG",CFG_CUSTOM,0.0);
 /** \brief Can be used to trigger a response to pesticides for the females */
 CfgFloat cfg_Skylark_female_NOEL("SK_FEMALE_NOEL",CFG_CUSTOM,0.001);
 /** \brief The proportion of pesticide remaining from one day to the next for females */
 CfgFloat cfg_Skylark_female_Biodegredation("SK_FEMALE_BIODEG",CFG_CUSTOM,0.0);
 /** \brief The proportion of eggs assumed to crack from pesticide effects as a global effect */
 CfgFloat cfg_skylark_pesticide_globaleggshellreduction("SK_GLOBALPESTICIDEEGGSHELLREDUC", CFG_CUSTOM, 0.0);
 /** \brief The proportion of eggs assumed to crack from pesticide effects */
 CfgFloat cfg_skylark_pesticide_eggshellreduction("SK_PESTICIDEEGGSHELLREDUC", CFG_CUSTOM, 0.0);
 /** \brief If true egg shell reduction works at clutch level, if false it is considered per egg */
 CfgBool cfg_skylark_pesticide_eggshellreduction_perclutch("SK_PESTICIDEEGGSHELLREDUCPERCLUTCH", CFG_CUSTOM , false);
 //****** END PESTICDE RELATED INPUT PARAMETERS   ******
