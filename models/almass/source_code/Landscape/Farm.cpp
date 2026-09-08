/**
\file
\brief
<B>Farm.cpp This file contains the source for the Farm class </B> \n
*/
/**
\file
 by Frank Nikolaisen & Chris J. Topping \n
 Version of June 2003 \n
 All rights reserved. \n
 \n
 Doxygen formatted comments in July 2008 \n
*/
//
// farm.cpp
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

//
#define _CRT_SECURE_NO_DEPRECATE

#include <string.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../ALMaSSDefines.h"
#include <math.h> //for optimisation
#include <time.h> //for dec. making
#include <unordered_set> // for crearing the crop mgt plans 

#define _CRTDBG_MAP_ALLOC

using namespace std;

#define _CRT_SECURE_NO_DEPRECATE
#include <random> //for dec. making - draw from a dist.
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Landscape/Map_cfg.h"
#include "../Landscape/cropprogs/DK_SpringBarley.h"
#include "../Landscape/cropprogs/DK_WinterWheat.h"
#include "Farm.h"

extern std::mt19937 g_generator;

extern Landscape * g_landscape_ptr;
/** \brief Used to enable/disable others networks than spatial network*/
CfgBool cfg_UseOnlySpatialNetwork("USE_ONLY_SPATIAL_NETWORKS", CFG_CUSTOM, true);
CfgInt cfg_NumberNeighboursSpatial("NUMBER_NEIGHBOURS_SPATIAL", CFG_CUSTOM, 5);
CfgInt cfg_NumberNeighboursVirtual("NUMBER_NEIGHBOURS_VIRTUAL", CFG_CUSTOM, 0);
CfgInt cfg_NumberNeighboursAssociative("NUMBER_NEIGHBOURS_ASSOCIATIVE", CFG_CUSTOM, 0);
CfgFloat cfg_DKCatchCropPct("DKCATCHCROPPCT", CFG_CUSTOM, 1.00);
CfgFloat cfg_DKCatchCropLegumePct("DKCATCHCROPLEGUMEPCT", CFG_CUSTOM, 1.00);
CfgFloat cfg_NLCatchCropPct("NLCATCHCROPPCT", CFG_CUSTOM, 1.00);
CfgFloat cfg_PTHorticulturePct("PTHORTICULTUREPCT", CFG_CUSTOM, 1.00);
/** \brief Used to define the type of management in olive gorves. By default all are managed intensively. Needs to sup up to 1.00 */
CfgFloat cfg_OliveGroveIntensivePct("OLIVEGROVEINTENSIVEPCT", CFG_CUSTOM, 1.00);
CfgFloat cfg_OliveGroveSuperIntensivePct("OLIVEGROVESUPERINTENSIVEPCT", CFG_CUSTOM, 0);
CfgFloat cfg_OliveGroveTradPct("OLIVEGROVETRADPCT", CFG_CUSTOM, 0);
CfgFloat cfg_OliveGroveTradOrgPct("OLIVEGROVETRADORGPCT", CFG_CUSTOM, 0); // EZ: not used but kept to be sure the percentages sum up to 1

CfgInt cfg_AphidDamageThreshold("COMMONAPHIDDAMAGETHRESHOLD", CFG_CUSTOM, 1000);


/** \brief Used to socio-economic calculations for farms */
CfgBool cfg_UseSocioEconomicFarm("USE_SOCIO_ECONOMIC_FARMS", CFG_CUSTOM, false);
CfgBool cfg_UseConsumat("USE_CONSUMAT_APPROACH", CFG_CUSTOM, false);
/** \brief Used to modify spraying dates of some test pesticides in selected crops - look for use of m_date_modifier */
static CfgInt  cfg_CropPesticideDateModifier("CROPPESTICIDEDATEMODIFER", CFG_CUSTOM, 0, -10, 10);
// Initialise any static member attributes (these will normally be parameters or flags)
bool FarmManager::m_UseSocioEconomicFarmers = false;

int Crop::m_date_modifier = 0;

CfgBool cfg_organic_extensive( "FARM_ORGANIC_EXTENSIVE", CFG_CUSTOM, false );
// Enable this for reading the farm reference file instead of the
// automated (not at all random) farm generation. Should not really
// be set to false, ever.
static CfgBool l_map_read_farmfile( "MAP_READ_FARMFILE", CFG_PRIVATE, true );
static CfgStr l_map_farmref_file( "MAP_FARMREF_FILE", CFG_CUSTOM, "farmrefs.txt" );

static CfgStr l_emaize_price_file( "EMAIZE_PRICE_FILE", CFG_CUSTOM, "EM_price.txt" );

/** \brief If set to true, the farmer decision making model is active.*/
CfgBool cfg_OptimisingFarms("OPTIMISING_FARMS",CFG_CUSTOM, false);
/** \brief If set to true, the original farm optimisation model's crop set is used in the farmer decision making model
In this mode the optimisation is carried out only once; it is used for the comparioson of the optimisaiton results with the original model.*/
CfgBool cfg_OptimiseBedriftsmodelCrops("OPTIMISE_BEDRIFTSMODEL_CROPS",CFG_CUSTOM, false); //12.03.13, set it to true to run the optimisation like in the Bedriftsmodel (with the same crops)
/** \brief If set to true, an output file with farm areas is produced.*/
CfgBool cfg_DumpFarmAreas ("DUMP_FARM_AREAS", CFG_CUSTOM, false);
/** \brief If set to true, the farm areas from the original farm optimisation model are used in the optimisation. If set to false,
the farm areas based on a map from 2005 are used.*/
CfgBool cfg_UseBedriftsmodelFarmAreas ("USE_BEDRIFTSMODEL_FARM_AREAS", CFG_CUSTOM, false);
/** \brief If set to yes, the only decision mode/startegy the farmers can use is deliberation (i.e. simplified optimisation).*/
CfgBool cfg_OnlyDeliberation ("ONLY_DELIBERATION", CFG_CUSTOM, true); //041213 if true, farmers will always choose to deliberate (see function ChooseDecisionMode)

/** \brief A parameter setting the maximum distance from a farm to another farm that can be considred a neighbour [km].*/
CfgFloat cfg_Neighbor_dist ("NEIGHBOR_DIST",CFG_CUSTOM,1.5);
/** \brief A parameter setting the minimum certainty level.*/
CfgFloat cfg_Min_certainty("MIN_CERTAINTY",CFG_CUSTOM,0);
/** \brief A parameter setting the minimum satisfaction level for profit.*/
CfgFloat cfg_Min_need_satisfaction1 ("MIN_NEED_SATISFACTION_ONE",CFG_CUSTOM, 100); //profit, %
/** \brief A parameter setting the minimum satisfaction level for yield.*/
CfgFloat cfg_Min_need_satisfaction2 ("MIN_NEED_SATISFACTION_TWO",CFG_CUSTOM, 100); //yield, %
/** \brief A parameter setting the proportion of farmers of a type profit maximiser.*/
CfgFloat cfg_Profit_max_proportion ("PROFIT_MAX_PROPORTION",CFG_CUSTOM, 100);//proportion in the population
/** \brief A parameter setting the proportion of farmers of a type yield maximiser.*/
CfgFloat cfg_Yield_max_proportion ("YIELD_MAX_PROPORTION",CFG_CUSTOM, 0);
/** \brief A parameter setting the proportion of farmers of a type environmentalist.*/
CfgFloat cfg_Environmentalist_proportion ("ENVIRONMENTALIST_PROPORTION",CFG_CUSTOM, 0);
/** \brief A parameter of the yield maximizer farmer type: it increases the chance of necessity of carrying out a pesticide spraying event
(i.e. the probability of a pest attack). Used to represent the yield maximiser's higher tendency to spray pesticide to protect crops
even at the expense of a profit.*/
CfgFloat cfg_Yield_max_pest_prob_multiplier ("YIELD_MAX_PEST_PROB_MULITPLIER",CFG_CUSTOM, 1.5);
/** \brief A parameter of the environmentalist farmer type: increases the chance that environmentalist does not find using
pesticides profitable. Used to represent environmentalist's reluctance to use pesticides.*/
CfgFloat cfg_Env_pest_multiplier ("ENV_PEST_MULTIPLIER",CFG_CUSTOM, 1.25); //>1
/** \brief A parameter of the environmentalist farmer type: reduces environmentalist's use of fertiliser. Used to represent
environmentalist's tendency to minimise the usage of fertlizers.*/
CfgFloat cfg_Env_fert_multiplier ("ENV_FERT_MULTIPLIER",CFG_CUSTOM, 0.8);//<1
CfgBool cfg_Sensitivity_analysis ("SENSITIVITY_ANALYSIS", CFG_CUSTOM, false);
/** \brief If set to true, crops are assigned area based on their gross margin proportion in the total GM for all crops with non-negative GM.*/
CfgBool cfg_Areas_Based_on_Distribution ("AREAS_BASED_ON_DISTRIBUTION", CFG_CUSTOM, false);
/** \brief If set to true, the energy maize crop is included in the simulation.*/
CfgBool cfg_MaizeEnergy ("MAIZE_ENERGY", CFG_CUSTOM, false);

/** \brief This parameter specifies the proportion of average number of animals on a farm for previous 3 years, below which a farmer decides not to grow energy maize anymore.*/
 CfgFloat cfg_AnimalsThreshold ("ANIMALS_THRESHOLD",CFG_CUSTOM, 0);
/** \brief This parameter specifies the relative difference in energy maize price which causes a farmer to deliberate.*/
 CfgFloat cfg_PriceChangeThreshold ("PRICE_CHANGE_THRESHOLD",CFG_CUSTOM, 0.2);
/** \brief This parameter specifies the life stage of a species whose numbers farmers use during their decision making.*/
 CfgInt cfg_LifeStage ("LIFE_STAGE", CFG_CUSTOM, 0);
 /** \brief This parameter specifies the day at which farmers observe the number of animals residing at their farm.*/
 CfgInt cfg_Animals_number_test_day ("ANIMALS_NUMBER_TEST_DAY", CFG_CUSTOM, 152); 

/** \brief Price of a fodder unit. [DKK/FU]*/
CfgFloat cfg_Price_FU ("PRICE_FU",CFG_CUSTOM, 1.157);
/** \brief Price of fertilizer. [DKK/kg]*/
CfgFloat cfg_Price_Nt ("PRICE_NT",CFG_CUSTOM, 1.93);
CfgFloat cfg_Price_SBarley ("PRICE_SBARLEY",CFG_CUSTOM, 83);
CfgFloat cfg_Price_Oats ("PRICE_OATS",CFG_CUSTOM, 75);
CfgFloat cfg_Price_WBarley ("PRICE_WBARLEY",CFG_CUSTOM, 93);
CfgFloat cfg_Price_WWheat ("PRICE_WWHEAT",CFG_CUSTOM, 94);
CfgFloat cfg_Price_Triticale ("PRICE_TRITICALE",CFG_CUSTOM, 80);
CfgFloat cfg_Price_WRape ("PRICE_WRAPE",CFG_CUSTOM, 163);
CfgFloat cfg_Price_SRape ("PRICE_SRAPE",CFG_CUSTOM, 163);

/** \brief A parameter setting the minimum proportion of fodder demand that has to be supplied from own fodder production on pig farms.*/
CfgFloat cfg_Min_fodder_prod_pig ("MIN_FODDER_PROD_PIG", CFG_CUSTOM, 20);
/** \brief A parameter setting the minimum proportion of fodder demand that has to be supplied from own fodder production on cattle farms. Not in use.*/
CfgFloat cfg_Min_fodder_prod_cattle ("MIN_FODDER_PROD_CATTLE", CFG_CUSTOM, 35);


/** \brief A fitting parameter for the probability of hunter acceptance of a farm wiht distance from home - slope */
CfgFloat cfg_ClosestFarmProbParam1( "CLOSESTFARMPROBPARAMONE", CFG_CUSTOM, 0.005 );
/** \brief A fitting parameter for the probability of hunter acceptance of a farm that is smaller */
CfgFloat cfg_FarmSizeProbParam1( "FARMSIZEPROBPARAMONE", CFG_CUSTOM, 1.5 );
/** \brief A fitting parameter for the probability of hunter acceptance of a farm with distance from home - scaler */
CfgFloat cfg_ClosestFarmProbParam2("CLOSESTFARMPROBPARAMTWO", CFG_CUSTOM, 1.0);
FarmManager * g_farmmanager; //added 19.03.13
/** \brief A fitting parameter for the probability of hunter acceptance of a farm with distance from roost */
CfgFloat cfg_RoostDistProbParam1( "ROOSTDISTPROBPARAMONE", CFG_CUSTOM, 1.5 );

/** \brief A switch to fix the grain distribution to a specific year or make a random pick between years. 
* Used for the goose model. 0, = random, 1 = 2013, 2 = 2014 */
CfgInt cfg_grain_distribution("GOOSE_GRAIN_DISTRIBUTION", CFG_CUSTOM, 0, 0, 2);

CfgStr cfgRotationFileDir("ROT_FILES_DIR", CFG_CUSTOM, ".");

/** \brief Used for sorting a farmers field size vector */
bool CompPcts (tpct i,tpct j) { return (i.pct<j.pct); }


bool Crop::StartUpCrop(int a_spring, std::vector<std::vector<int>> a_flexdates, int a_todo)
{
	m_field->ClearManagementActionSum();
	m_field->SetMDates(0, 0, a_flexdates[0][1]); // last possible day of harvest
	m_field->SetMDates(1, 0, a_flexdates[0][1]); // last possible day of harvest
	//flexdata[0] has the number of pairs
	// Determined by harvest date - used to see if at all possible
	int sz = int(a_flexdates.size());
	for (int i = 1; i < sz; i++) {
		m_field->SetMDates(0, i, a_flexdates.at(i).at(0)); // start date operation 
		m_field->SetMDates(1, i, a_flexdates.at(i).at(1)); // end date operation
	}
	m_field->SetMConstants(0, 1);
	// Check the next crop for early start, unless it is a spring crop
	// in which case we ASSUME that no checking is necessary!!!!
	// So DO NOT implement a crop that runs over the year boundary (i.e. from spring to spring!), at least not without fixing this.
	if (!m_ev->m_forcespringOK) {
		if (m_ev->m_startday > g_date->DayInYear(1, 7))
		{
			if (m_field->GetMDates(0, 0) >= m_ev->m_startday)
			{
				g_msg->Warn(WARN_BUG, g_landscape_ptr->VegtypeToString(m_ev->m_event), "::Do() : Harvest too late for the next crop to start!!!");
				exit(1);
			}
			// Now fix any late finishing problems
			for (int i = 1; i < sz; i++) {
				if (m_field->GetMDates(0, i) >= m_ev->m_startday) {
					m_field->SetMDates(0, i, m_ev->m_startday - 1); //move the starting date
				}
				if (m_field->GetMDates(1, i) >= m_ev->m_startday) {
					m_field->SetMConstants(i, 0);
					m_field->SetMDates(1, i, m_ev->m_startday - 1); //move the finishing date
				}
			}
		}
	}
	// Now no operations can be timed after the start of the next crop.

	if (!m_ev->m_first_year)
	{
		int today = g_date->Date();
		// Are we before July 1st?
		if (today < g_date->OldDays() + g_date->DayInYear(1, 7))
		{
			if (!m_ev->m_forcespring) // If we are forced to this then allow it 
			{
				// Yes, too early. We assumme this is because the last crop was late 
				g_msg->Warn(WARN_BUG, g_landscape_ptr->VegtypeToString(m_ev->m_event), "::Do() : Crop start attempt between 1st Jan & 1st July");
				exit(1);
			}
		}
		else
		{
			if (!m_ev->m_forcespring) 
				if (today > g_date->OldDays() + m_first_date + a_spring) // Adds 365 for spring crop 
			{
				// Yes too late - should not happen - raise an error
				g_msg->Warn(WARN_BUG, g_landscape_ptr->VegtypeToString(m_ev->m_event), "::Do() : Crop start attempt after last possible start date");
				exit(1);
			}
		}
	}
	else {
		SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 2), a_todo, false);
		return true;
	}
	return false; // not first year
}

Crop::Crop(TTypesOfVegetation a_tov, TTypesOfCrops a_toc, Landscape* a_L)
{
	m_ddegstoharvest = -1; // Set to -1 to indicate that this is not using ddegs to harvest, this will be reset in descendent classes as needed
	SetCropClassification(tocc_Winter); // Defualt classification is Winter - change this if necessary in the derived crop constructor
	m_toc = a_toc;
	m_tov = a_tov;
	m_date_modifier = cfg_CropPesticideDateModifier.value();
	m_OurLandscape = a_L;
}

bool Crop::Do( Farm * /* a_farm */, LE * /* a_field */, FarmEvent* /* a_ev */ ) {
  return true;
}

void Crop::SimpleEvent(long a_date, int a_todo, bool a_lock) {
	m_farm->AddNewEvent(m_field->GetVegType(), a_date, m_field, a_todo, m_field->GetRunNum(), a_lock, 0, false, (TTypesOfVegetation)0, fmc_Others, false, false);
}

void Crop::SimpleEvent_(long a_date, int a_todo, bool a_lock, Farm* a_farm, LE* a_field) {
	a_farm->AddNewEvent(a_field->GetVegType(), a_date, a_field, a_todo, a_field->GetRunNum(), a_lock, 0, false, (TTypesOfVegetation)0, fmc_Others, false, false);
}


void Crop::ChooseNextCrop (int a_no_dates){

/**The function finds the next crop to grow on a field where the current crop's management has finished. If necessary, it
adjusts current crop's management to the new crop.*/
	;
}

class Farmer;
/**
\brief
Starts the main management loop for the farm and performs some error checking
*/
void Farm::FarmActions( void ) {
	/** 
	* The main crop management events are done in HandleEvents by the crop management method within CropManagement. This calls all the fields and actions any events on the queue. Subsequently there are some checks for runaway or stuck management codes
	* The FarmerBehaviour section allows for implementation of Farmer behaviour - it implements the Step code of the animals populations approach to allow for complex behaviours to be added.
	*/
  HandleEvents();
  for ( unsigned int i = 0; i < m_fields.size(); i++ ) {
    // Check for infinite loop in management plan.
    int count = m_fields[ i ]->GetVegStore();
    if ( count >= 0 )
      m_fields[ i ]->SetVegStore( ++count );
    if ( count > 800 ) {
      // More than two years where nothing happened.
      // Raise 'Merry Christmas'!
      char error_num[ 20 ];
      sprintf( error_num, "%d", m_fields[ i ]->GetVegType() );
      g_msg->Warn( WARN_BUG, "I the Farm Manager caught infinite loop in tov type:", error_num );
      sprintf( error_num, "%d", m_fields[ i ]->m_tried_to_do );
      g_msg->Warn( WARN_BUG, "It was last seen trying to perform action # ""(or thereabouts):", error_num );
      exit( 1 );
    }
  }
}

/**
\brief
Call do function for any crop with an outstanding event. Signal if the crop has terminated.
*/
bool Farm::LeSwitch( FarmEvent * ev ) {
	// Ignore this event if it is from the execution of
	// a previous management plan.
	if (ev->m_field->GetRunNum() > ev->m_run)
		return false;
	// Store what we are trying to do this time.
	// ***FN*** To be cleaned up later.
	ev->m_field->m_tried_to_do = ev->m_todo;

	bool done = false;

	/* We loop through the loaded cropprog in the FarmManager (for the entire landscape)
	   and stop when we find the cropprog corresponding to this particular FarmEvent.
	   We then call the correct Do() function of the crop progs, beneifiting from runtime polymorphism
	   since all cropprogs are derived from the base class Crop */
	vector<Crop*> * cropprogs = m_OurManager->GetCropMgtPlans();
	for (unsigned int i = 0; i < cropprogs->size(); i++)
	{
		if ( (ev->m_event == (*cropprogs)[i]->m_tov) || 
			  (ev->m_event == tov_PlantNursery && (*cropprogs)[i]->m_tov == tov_YoungForest ) ) // Preserve logic from before: "uses youngforrest management because this effectively does nothing, but stops the crop management creating looping error"
		{ 
			done = (*cropprogs)[i]->Do(this, ev->m_field, ev);
			break;
		}

	}

	return done;
}


/**
\brief
Adds an event to the event queue for a farm
*/
void Farm::AddNewEvent(TTypesOfVegetation a_event, long a_date, LE* a_field, int a_todo, long a_run, bool a_lock,
	int a_start, bool a_first_year, TTypesOfVegetation a_crop, FarmManagementCategory a_fmc, bool a_forcespring, bool a_forcespringOK) {
	FarmEvent* ev = new FarmEvent(a_event, a_field, a_todo, a_run, a_lock, a_start, a_first_year, a_crop, a_forcespring, a_forcespringOK);
	m_queue.Push(ev, a_date);
}


/**
\brief
Return chance out of 0 to 100
*/
bool Farm::DoIt( double a_probability ) {
  return (g_rand_uni_fnc() < (a_probability/100.0));
}

/**
\brief
Return chance out of 0 to 1
*/
bool Farm::DoIt_prob(double a_probability) {
	return (g_rand_uni_fnc() < a_probability);
}

/**
\brief
Reads a rotation file into the rotation
*/
void Farm::ReadRotation(std::string str)
{
  ifstream ifile;
  ifile.open(str.c_str(),ios::in);
  if ( !ifile.is_open() ) {
      g_msg->Warn( "Cannot open file: ", str.c_str() );
      exit( 1 );
  }
  int nocrops;
  ifile >> nocrops;
  m_rotation.resize( nocrops );
  std::string cropref;
  for ( int i = 0; i < nocrops; i++ ) {
    ifile >> cropref;
    TTypesOfVegetation tov = g_farmmanager->TranslateVegCodes( cropref );
    m_rotation[ i ] = tov;
  }
  ifile.close();
}

/**
Rotation error check function
*/
void Farm::CheckRotationManagementLoop( FarmEvent * ev ) {
  if ( ev->m_field->GetMgtLoopDetectDate() == g_date->Date() ) {
    // The last crop managment plan stopped on the same day as
    // it was started.

    // Bump loop counter.
    ev->m_field->SetMgtLoopDetectCount( ev->m_field->GetMgtLoopDetectCount() + 1 );

    if ( ev->m_field->GetMgtLoopDetectCount() > ( long )( m_rotation.size() + 2 ) ) {
      // We have a loop.
      char errornum[ 20 ];
      sprintf( errornum, "%d", m_farmtype );
      g_msg->Warn( WARN_BUG, "Rotation management loop detected in farmtype ", errornum );
      exit( 1 );
    }
  } else {
    ev->m_field->SetMgtLoopDetectCount( 0 );
  }
}


/**
\brief
Returns the start date of the next crop in the rotation
*/
int Farm::GetNextCropStartDate( LE * a_field, TTypesOfVegetation & a_curr_veg ) {
  TTypesOfVegetation l_tov2;

  if ( a_field->GetRotIndex() < 0 || g_farm_fixed_crop_enable.value() //|| g_farm_test_crop.value()
	                                                                  ) {
    l_tov2 = a_curr_veg; // don't do it if no rotation
  } else {
    l_tov2 = m_rotation[ GetNextCropIndex( a_field->GetRotIndex() ) ];
  }
  a_curr_veg = l_tov2;
  return GetFirstDate( l_tov2 );
}


/**
\brief
If there are events to carry out do this, and perhaps start a new crop
*/
void Farm::HandleEvents(void) {
	if (m_queue.Empty())
		return;

	LowPriPair < FarmEvent* > pair = m_queue.Bottom();
	FarmEvent* ev = pair.m_element;
	while (pair.m_pri <= g_date->Date()) {
		m_queue.Pop();
		/** Call the management plan for the current crop and it this return done=true starts the next management. */
		if (LeSwitch(ev)) {
			// This crop management plan has terminated.
#ifdef __CALCULATE_MANAGEMENT_NORMS
			m_OurManager->AddToManagementStats(ev);
#endif

			// First check for an infinite loop in the rotation scheme,
			// ie. a scenario where all crops decide not to run given
			// the date.
			CheckRotationManagementLoop(ev);

			// Outdate any remaining events for this field.
			ev->m_field->BumpRunNum();

			// Crop treatment done, select and initiate new crop if in rotation.
			TTypesOfVegetation new_veg = ev->m_field->GetVegType();

			if (ev->m_field->GetRotIndex() >= 0) {
				int new_index = GetNextCropIndex(ev->m_field->GetRotIndex());
				new_veg = m_rotation[new_index];
				// Running in fixed crop mode?
				if (g_farm_fixed_crop_enable.value()) {
					new_veg = g_letype->TranslateVegTypes(g_farm_fixed_crop_type.value());
				}
				/** Sets the new index to the rotation. */
				ev->m_field->SetRotIndex(new_index);
				/** Save the new veg type as the LE vegetation type */
				ev->m_field->SetVegType(new_veg, tov_Undefined);
				if (ev->m_field->GetUnsprayedMarginPolyRef() != -1)
				{
					LE * um = g_landscape_ptr->SupplyLEPointer(ev->m_field->GetUnsprayedMarginPolyRef());
					um->SetVegType(new_veg, tov_Undefined);
				}
				/* Recalculate the attributes */
				ev->m_field->GetLandscape()->Set_TOV_Att(ev->m_field);
				/** Save the crop veg type as the  Field vegetation type */
				ev->m_field->SetCropType(m_OurManager->GetCropTypeFromPlan(new_veg));
			}
			// Reset the event list for this field.
			ev->m_field->ResetTrace();
			// Reset event timeout counter.
			ev->m_field->SetVegStore(0);

			// The next bit simply determines the start date of the next crop in
			// the rotation and passes this to the start crop event.
			// The crop is responsible for raising an error if the next crop is
			// not possible or otherwise handling the problem

			// 19/5-2003: Note: This code was moved out into a dedicated
			// method of the Farm class, GetNextCropStartDate(), as precisely
			// the same piece of code needs to be run during initialization of
			// farm management.
			TTypesOfVegetation l_tov = new_veg;
			int l_nextcropstartdate = GetNextCropStartDate(ev->m_field, l_tov);
			bool forcespringOK = GetForceSpringOK(l_tov);
			// Create 'start' event for today and put it on the queue. 
			AddNewEvent(new_veg, g_date->Date(), ev->m_field, PROG_START, ev->m_field->GetRunNum(), false, l_nextcropstartdate, false, l_tov, fmc_Others, ev->m_forcespring, forcespringOK);
			// Set starting date for rotation mgmt loop detection. 
			ev->m_field->SetMgtLoopDetectDate(g_date->Date());
            // Now reset the random slope factor for the field growth
            ev->m_field->SetVegGrowthScalerRand();
        }

		delete ev;

		if (m_queue.Empty())
			return;
		pair = m_queue.Bottom();
		ev = pair.m_element;

	}
}

/**
\brief
Farm constructor - creates an instance of each possible crop type
*/
Farm::Farm( FarmManager* a_manager ) 
{
	m_OurManager = a_manager;
	m_rotation_sync_index = -1;
	// Defaults that need to be overridden when necessary
	m_FarmType = tof_Foobar;
	m_stockfarmer = false;
	m_intensity = 0.0; // Unless changed
}


/**
\brief
Farm destructor - deletes all crop instances and empties event queues
*/
Farm::~Farm(void) {
	LowPriPair < FarmEvent * > pair;

	while (!m_queue.Empty()) {
		pair = m_queue.Bottom();
		m_queue.Pop();
		delete pair.m_element;
	}
}

/** \brief Returns the area of arable fields owned by that farm */
int Farm::GetArea() {
	int area = 0;
	for (unsigned int i = 0; i < m_fields.size(); i++) {
		if (m_fields[i]->GetElementType() == tole_Field) area += (int)m_fields[i]->GetArea();
	}
	return area;
}

/** \brief Returns the area of all fields owned by that farm */
int Farm::GetTotalArea() {
	int area = 0;
	for (unsigned int i = 0; i < m_fields.size(); i++) 
	{
		area += (int)m_fields[i]->GetArea();
	}
	return area;
}

/** \brief Returns the area of arable fields owned by that farm */
double Farm::GetAreaDouble() {
	double area = 0;
	for ( unsigned int i = 0; i < m_fields.size(); i++ ) {
		if (m_fields[i]->GetElementType()==tole_Field) area += m_fields[i]->GetArea();
	}
	return area;
}

/** Returns the number of fields owned by that farm with an openness above a_openness */
int Farm::GetNoOpenFields(int a_openness)
{
	int num = 0;
	for (unsigned int i = 0; i < m_fields.size(); i++) {
		if (m_fields[i]->GetOpenness() > a_openness) num++;
	}
	return num;
}

/** Returns the area of fields owned by that farm with an openness above a_openness */
int Farm::GetAreaOpenFields(int a_openness)
{
	int area = 0;
	for (unsigned int i = 0; i < m_fields.size(); i++) {
		if (m_fields[i]->GetOpenness() > a_openness) area += int(m_fields[i]->GetArea());
	}
	return area;
}

/**
\brief
Gets the first crop for the farm.
*/
/**
This method also synchronises farm rotations either within or between farms if needed. This is useful to try simple what if scenarios.
*/
int Farm::GetFirstCropIndex( TTypesOfLandscapeElement /* a_type */ ) {
  // If g_farm_fixed_rotation, then determine the first
  // crop number in the rotation rotation number.
  if ( g_farm_fixed_rotation_enable.value() ) {

    if ( !g_farm_fixed_rotation_farms_async.value() ) {
      // We are running all the farms synchronized, so
      // simply set the first crop to run on all farm fields.
      return 0;
    }

    // Each farm runs its fields sync'ed but independently from
    // the other farmers.

    // Determine if this farm has selected its own start index
    // and set it if not. m_rotation_sync_index is initialized
    // to -1 by the Farm::Farm() constructor.
    if ( -1 == m_rotation_sync_index ) {
      m_rotation_sync_index = g_random_fnc(int(m_rotation.size()));
    }
    // Return farm localized rotation index.
    return m_rotation_sync_index;
  }

  // Not synchronised, but we want to follow our rotation sequence, so check
  // if we have started this process, if not set the sync value.
  // afterwards just increment this.
  if ( -1 == m_rotation_sync_index ) {
	  if (int(m_rotation.size()) > 0) m_rotation_sync_index = g_random_fnc(int(m_rotation.size())); else  m_rotation_sync_index = 0;
  }
  else m_rotation_sync_index = (int) ((m_rotation_sync_index+1) % m_rotation.size());
  return m_rotation_sync_index;
}


/**
\brief
Returns the next crop in the rotation
*/
/**
Also provides the possibility of over-riding rotations using configuration settings
*/
int Farm::GetNextCropIndex( int a_rot_index ) {
  if ( !g_farm_enable_crop_rotation.value() ) {
    // Rotation not enabled.
    return a_rot_index;
  }

  if ( a_rot_index == -1 )
    return -1;

  if ( ( unsigned int ) ( ++a_rot_index ) == m_rotation.size() ) //AM comm: the last crop was the last element of the vector  - so go back to the element zero; otherwise just add 1 to the index
    a_rot_index = 0;

  return a_rot_index;
}


/**
\brief
Adds a field to a farm
*/
void Farm::AddField( LE * a_newfield ) {
  int i = (int) m_fields.size();

  m_fields.resize( i + 1 );
  m_fields[ i ] = a_newfield;
  // Must set the rot index to something other than -1, but identify it as not usefully set as yet.
	TTypesOfLandscapeElement ele = a_newfield->GetElementType();
	switch (ele) {
		case tole_PermPastureLowYield:
		case tole_YoungForest:
		case tole_PermPasture:
		case tole_PermPastureTussocky:
		case tole_PermanentSetaside:
		case tole_Orchard:
    	case tole_BushFruit:
    	case tole_OBushFruit:
    	case tole_OOrchard:
		case tole_PlantNursery:
		case tole_OliveGrove:
		case tole_WoodyEnergyCrop:
	  	case tole_PermPasturePigs:
	  	case tole_OPermPasturePigs:
	  	case tole_OPermPasture:
	  	case tole_OPermPastureLowYield:
    	case tole_ChristmasTrees:
    	case tole_OChristmasTrees:
    	case    tole_SolarPanel:
		case 	tole_EnergyCrop:
		case 	tole_OEnergyCrop:
		case 	tole_FarmForest:
		case 	tole_OFarmForest:
		case 	tole_FarmYoungForest:
		case 	tole_OFarmYoungForest:
		case 	tole_AlmondPlantation:
		case 	tole_WalnutPlantation:
		case 	tole_FarmBufferZone:
		case 	tole_NaturalFarmGrass:
		case 	tole_GreenFallow:
		case 	tole_FarmFeedingGround:
		case 	tole_FlowersPerm:
		case    tole_PermPastureTussockyWet:
		case 	tole_AsparagusPerm:
		case 	tole_MushroomPerm:
		case 	tole_OtherPermCrop:
		case 	tole_OAsparagusPerm:
			m_fields[i]->SetRotIndex(-2);
			break;
		default:
			m_fields[ i ]->SetRotIndex(-1);
	}
}

/**
\brief
Removes a field from a farm
*/
void Farm::RemoveField( LE * a_field ) {
  int nf = (int) m_fields.size();
  for ( int i = 0; i < nf; i++ ) {
    if ( m_fields[ i ] == a_field ) {
      m_fields.erase( m_fields.begin() + i );
      return;
    }
  }
  // If we reach here there is something wrong because the field is not a
  // member of this farm
  g_msg->Warn( WARN_BUG, "Farm::RemoveField(LE* a_field): ""Unknown field! ", "" );
  exit( 1 );
}


void Farm::Assign_rotation(vector<TTypesOfVegetation>a_new_rotation) {

	m_rotation.clear();
	m_rotation = a_new_rotation;

}

/**
\brief
Kicks off the farm's management
*/
void UserDefinedFarm::InitiateManagement( void ) {
	// First we need to assign the permanent crops if any.
	if (m_PermCrops.size()>0) {
		// We have something to do
		for (int i=0; i<(int)m_PermCrops.size(); i++) {
			AssignPermanentCrop(m_PermCrops[i].Tov, m_PermCrops[i].Pct);
		}
	}
	Farm::InitiateManagement();
}

/**
\brief
Kicks off the farm's management
*/
void Farm::InitiateManagement( void ) {
	for ( unsigned int i = 0; i < m_fields.size(); i++ ) {
		int rot_index =  m_fields[ i ]->GetRotIndex();
		TTypesOfVegetation new_veg = tov_Undefined;
		// If the field has been designated as non-rotating and therefore already has its veg type, then skip it.
		if ( rot_index < -1 ) {
			continue; /**\todo No permanent crops.*/
			// Check for any type of permanent element type with management plan.
			TTypesOfLandscapeElement ele = m_fields[ i ]->GetElementType();
			switch (ele) {
				case tole_AsparagusPerm:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEAsparagusEstablishedPlantation;
						m_fields[i]->SetCropType(toc_AsparagusEstablishedPlantation);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_DEAsparagusEstablishedPlantation;	//EZ: DE management used for NL
						m_fields[i]->SetCropType(toc_AsparagusEstablishedPlantation);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_AsparagusPerm! ", "");
						exit(1);
						break;
					}
				case tole_OAsparagusPerm:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEOAsparagusEstablishedPlantation;
						m_fields[i]->SetCropType(toc_OAsparagusEstablishedPlantation);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OAsparagusPerm! ", "");
						exit(1);
						break;
					}
				case tole_BushFruit:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKBushFruit_Perm1; //EZ: 2nd year of management used here /* \todo Should this be DEBushFruit indstead */
						m_fields[i]->SetCropType(toc_BushFruit);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKBushFruit_Perm1; //LKM: all strawberry management in this code
						m_fields[i]->SetCropType(toc_BushFruit);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_DKBushFruit_Perm1; //DK management used for BE
						m_fields[i]->SetCropType(toc_BushFruit);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_DKBushFruit_Perm1; //LKM: all strawberry management in this code
						m_fields[i]->SetCropType(toc_BushFruit);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_DKBushFruit_Perm1; //DK management used for NL
						m_fields[i]->SetCropType(toc_BushFruit);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "PL")
					{
						new_veg = tov_DKBushFruit_Perm1; //DK management used for PL
						m_fields[i]->SetCropType(toc_BushFruit);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_DKBushFruit_Perm1; //LKM: all strawberry management in this code /* \todo Could probably be removed if running FR landscapes (for now NL landscapes for FR crops) */
						m_fields[i]->SetCropType(toc_BushFruit);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_BushFruit! ", "");
						exit(1);
						break;
					}
				case tole_OBushFruit:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEOBushFruitPerm;
						m_fields[i]->SetCropType(toc_OBushFruit);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOBushFruit_Perm1; // LKM: all organic strawberry management in this code
						m_fields[i]->SetCropType(toc_OBushFruit);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OBushFruit! ", "");
						exit(1);
						break;
					}
				case tole_ChristmasTrees:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKChristmasTrees_Perm; //EZ: DK management used for DE
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKChristmasTrees_Perm;
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_DKChristmasTrees_Perm; //EZ: DK management used for DE 
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_DKChristmasTrees_Perm; /* \todo Could probably be removed if running FR landscapes (for now NL landscapes for FR crops) */
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_DKChristmasTrees_Perm; //EZ: DK management used for BE
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_ChristmasTrees! ", "");
						exit(1);
						break;
					}
				case tole_OChristmasTrees:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKOChristmasTrees_Perm; //EZ: DK crop code used for DE  -> to be changed later
						m_fields[i]->SetCropType(toc_YoungForestCrop);
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOChristmasTrees_Perm; // 
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OChristmasTrees! ", "");
						exit(1);
						break;
					}
				case tole_EnergyCrop:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKEnergyCrop_Perm;	//EZ: DK management used for DE
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKEnergyCrop_Perm;
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_DKEnergyCrop_Perm;
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_EnergyCrop! ", "");
						exit(1);
						break;
					}
				case tole_OEnergyCrop:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKOEnergyCrop_Perm;	//EZ: DK management used for DE
						m_fields[i]->SetCropType(toc_OYoungForestCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOEnergyCrop_Perm;
						m_fields[i]->SetCropType(toc_OYoungForestCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OEnergyCrop! ", "");
						exit(1);
						break;
					}
				case tole_FarmBufferZone:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_FIBufferZone_Perm;
						m_fields[i]->SetCropType(toc_PermanentSetAside);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_FIBufferZone_Perm;
						m_fields[i]->SetCropType(toc_PermanentSetAside);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_FarmBufferZone! ", "");
						exit(1);
						break;
					}
				case tole_FarmFeedingGround:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_FIFeedingGround;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_FarmFeedingGround! ", "");
						exit(1);
						break;
					}
				case tole_FarmForest:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKFarmForest_Perm;	//EZ: DK management used for DE
						m_fields[i]->SetCropType(toc_FarmForest);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKFarmForest_Perm;
						m_fields[i]->SetCropType(toc_FarmForest);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_DKFarmForest_Perm;
						m_fields[i]->SetCropType(toc_FarmForest);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_DKFarmForest_Perm;	//EZ: DK management used for FI
						m_fields[i]->SetCropType(toc_FarmForest);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_DKFarmForest_Perm;//LKM: DK management used for FR /* \todo Could probably be removed if running FR landscapes (for now NL landscapes for FR crops) */
						m_fields[i]->SetCropType(toc_FarmForest);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_DKFarmForest_Perm;	//EZ: DK management used for NL
						m_fields[i]->SetCropType(toc_FarmForest);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_FarmForest! ", "");
						exit(1);
						break;
					}
				case tole_OFarmForest:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKOFarmForest_Perm;	//DK management used for DE
						m_fields[i]->SetCropType(toc_OFarmForest);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOFarmForest_Perm;
						m_fields[i]->SetCropType(toc_OFarmForest);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_FarmForest! ", "");
						exit(1);
						break;
					}
				case tole_FarmYoungForest:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKFarmYoungForest_Perm;
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_DKFarmYoungForest_Perm;
						m_fields[i]->SetCropType(toc_YoungForestCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_FarmYoungForest! ", "");
						exit(1);
						break;
					}
				case tole_OFarmYoungForest:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOFarmYoungForest_Perm;
						m_fields[i]->SetCropType(toc_OYoungForestCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OFarmYoungForest! ", "");
						exit(1);
						break;
					}
				case tole_FlowersPerm:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_NLTulips;
						m_fields[i]->SetCropType(toc_Tulips);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_NLTulips;
						m_fields[i]->SetCropType(toc_Tulips);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_FlowersPerm! ", "");
						exit(1);
						break;
					}
				case tole_GreenFallow:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_FIGreenFallow_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_GreenFallow! ", "");
						exit(1);
						break;
					}
				case tole_NaturalFarmGrass:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_FINaturalGrassland_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_FINaturalGrassland_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_FINaturalGrassland_Perm;	//EZ: FI management used for NL
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_FINaturalGrassland_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_NaturalFarmGrass! ", "");
						exit(1);
						break;
					}
				case tole_Orchard:
				case tole_WoodyEnergyCrop:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_BEOrchardCrop;
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEOrchard;
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOrchardCrop_Perm;
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_DKOrchardCrop_Perm;	//EZ: DK crop code used for FI -> to be changed later
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_DKOrchardCrop_Perm;//LKM: DK management used for FR /* \todo Could probably be removed if running FR landscapes (for now NL landscapes for FR crops) */
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "IT")
					{
						new_veg = tov_ITOrchard;
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_NLOrchardCrop;
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "PL")
					{
						new_veg = tov_NLOrchardCrop;	/* \todo EZ: NL crop code used for PL -> to be changed later */
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_Orchard! ", "");
						exit(1);
						break;
					}
				case tole_OOrchard:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEOOrchard;
						m_fields[i]->SetCropType(toc_OOrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOOrchardCrop_Perm;
						m_fields[i]->SetCropType(toc_OOrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "IT")
					{
						new_veg = tov_ITOOrchard;
						m_fields[i]->SetCropType(toc_OOrchardCrop);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_Orchard! ", "");
						exit(1);
						break;
					}
				case tole_PermPasture:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEPermanentGrassGrazed;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKGrassGrazed_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_DKGrassGrazed_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI")
					{
						new_veg = tov_FINaturalGrassland_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_FRGrassland_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "IT")
					{
						new_veg = tov_ITGrassland;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_NLPermanentGrassGrazed;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "PL")
					{
						new_veg = tov_DEPermanentGrassGrazed; /* \todo EZ: DE crop code used for PL -> to be changed later */
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "PT")
					{
						new_veg = tov_PTPermanentGrassGrazed;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "UK")
					{
						new_veg = tov_UKPermanentGrass;
						m_fields[i]->SetCropType(toc_PermanentGrassGrazed);
						break;
					}

					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_PermPasture! ", "");
						exit(1);
						break;
					}
				case tole_OPermPasture:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEOPermanentGrassGrazed;
						m_fields[i]->SetCropType(toc_OPermanentGrassGrazed);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOGrassGrazed_Perm;
						m_fields[i]->SetCropType(toc_OPermanentGrassGrazed);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OPermPasture! ", "");
						exit(1);
						break;
					}
				case tole_PermPastureLowYield:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEPermanentGrassLowYield;
						m_fields[i]->SetCropType(toc_PermanentGrassLowYield);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKGrassLowYield_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassLowYield);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_NLPermanentGrassGrazedExtensive;
						m_fields[i]->SetCropType(toc_PermanentGrassLowYield);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "PL")
					{
						new_veg = tov_DEPermanentGrassLowYield; /* \todo EZ: DE crop code used for PL -> to be changed later */
						m_fields[i]->SetCropType(toc_PermanentGrassLowYield);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_PermPastureLowYield! ", "");
						exit(1);
						break;
					}
				case tole_OPermPastureLowYield:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEOPermanentGrassLowYield;
						m_fields[i]->SetCropType(toc_OPermanentGrassLowYield);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOGrassLowYield_Perm;
						m_fields[i]->SetCropType(toc_OPermanentGrassLowYield);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OPermPastureLowYield! ", "");
						exit(1);
						break;
					}
				case tole_PermPasturePigs:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKGrazingPigs_Perm;
						m_fields[i]->SetCropType(toc_GrazingPigs);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_PermPasturePigs! ", "");
						exit(1);
						break;
					}
				case tole_OPermPasturePigs:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKOGrazingPigs_Perm;
						m_fields[i]->SetCropType(toc_OGrazingPigs);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_OPermPasturePigs! ", "");
						exit(1);
						break;
					}
				case tole_PlantNursery:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DKPlantNursery_Perm;	//DK management used for DE
						m_fields[i]->SetCropType(toc_Horticulture);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKPlantNursery_Perm;
						m_fields[i]->SetCropType(toc_Horticulture);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_DKPlantNursery_Perm;
						m_fields[i]->SetCropType(toc_Horticulture);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FI") /* \todo LKM probably not needed, but here due to testing with not FI landscape */
					{
						new_veg = tov_DKPlantNursery_Perm;
						m_fields[i]->SetCropType(toc_Horticulture);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR") /* \todo LKM probably not needed, but here due to testing with not FR landscape */
					{
						new_veg = tov_DKPlantNursery_Perm;
						m_fields[i]->SetCropType(toc_Horticulture);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "PL")
					{
						new_veg = tov_DKPlantNursery_Perm;	//DK management used for PL
						m_fields[i]->SetCropType(toc_Horticulture);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "NL")
					{
						new_veg = tov_DKPlantNursery_Perm;	//DK management used for NL
						m_fields[i]->SetCropType(toc_Horticulture);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_PlantNursery! ", "");
						exit(1);
						break;
					}
				case tole_Vineyard:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DE")
					{
						new_veg = tov_DEOrchard;	/* \todo EZ: orchards used instead for DE */
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "BE")
					{
						new_veg = tov_BEOrchardCrop;	/* \todo EZ: orchards used instead for BE */
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					else if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "FR")
					{
						new_veg = tov_DKOrchardCrop_Perm;	/* \todo EZ: orchards used instead for BE */
						m_fields[i]->SetCropType(toc_OrchardCrop);
						break;
					}
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "PT")
					{
						new_veg = tov_PTVineyards;
						m_fields[i]->SetCropType(toc_Vineyards);
						break;
					}
					else {
						g_msg->Warn(WARN_BUG, "Farm::InitiateManagement(): "
							"Unable to allocate tov type to tole_Vineyard! ", "");
						exit(1);
						break;
					}

				// Permanent crops with no specific management plan - needs to be changed later
				case tole_OliveGrove:
				case tole_RiceField:
				case tole_AlmondPlantation:
				case tole_WalnutPlantation:
				case tole_MushroomPerm:
				case tole_OtherPermCrop:
					new_veg = tov_PermanentSetAside; // to be changed if the crop code is available
					m_fields[i]->SetCropType(toc_PermanentSetAside);
					break;

				// Old permanent crops with no specific management plan - needs to be changed later
				case tole_PermPastureTussocky:
					if (this->m_OurManager->GetLandscape()->SupplyCountryCode() == "DK")
					{
						new_veg = tov_DKGrassTussocky_Perm;
						m_fields[i]->SetCropType(toc_PermanentGrassTussocky);
						break;
					}
					else
					new_veg = tov_PermanentGrassTussocky;
					m_fields[i]->SetCropType(toc_PermanentGrassTussocky);
					break;
				case tole_YoungForest:
					new_veg = tov_YoungForest;
					m_fields[i]->SetCropType(toc_YoungForestCrop);
					break;
				case tole_PermanentSetaside:
					new_veg = tov_PermanentSetAside;
					m_fields[i]->SetCropType(toc_PermanentSetAside);
					break;

				default:
					if (rot_index != -4) {
						// Error
						g_msg->Warn("Unexpected negative value in Farm::InitiateManagement","");
						exit(0);
					}
					else
					{
						new_veg = m_fields[i]->GetVegType();
					}
			}
		}
		else {
			rot_index = GetFirstCropIndex(m_fields[i]->GetElementType());
			new_veg = m_rotation[rot_index];
			// Running in fixed crop mode?
			if (g_farm_fixed_crop_enable.value()) {
				int fv = g_farm_fixed_crop_type.value();
				new_veg = g_letype->TranslateVegTypes(fv);
			}
		}
		m_fields[ i ]->SetVegType( new_veg, tov_Undefined ); // lct: crop & weed
		m_fields[ i ]->SetRotIndex( rot_index );
		m_fields[ i ]->SetGrowthPhase( janfirst );
		if (m_fields[i]->GetUnsprayedMarginPolyRef() != -1)
		{
			LE* um = g_landscape_ptr->SupplyLEPointer(m_fields[i]->GetUnsprayedMarginPolyRef());
			um->SetVegType(new_veg, tov_Undefined);
			um->SetGrowthPhase(janfirst);
		}
		/** Save the crop veg type as the  Field vegetation type */
		m_fields[ i ]->SetCropType(m_OurManager->GetCropTypeFromPlan(new_veg));
		// Reset event timeout counter. We are now 800 days from
		// oblivion.
		long prog_start_date = g_date->Date();
		m_fields[i]->SetVegStore(0);
		TTypesOfVegetation l_tov = new_veg;
		int l_nextcropstartdate = GetNextCropStartDate(m_fields[i], l_tov);
		bool SpringOK = GetForceSpringOK(l_tov);
		AddNewEvent(new_veg, prog_start_date, m_fields[i], PROG_START, 0, false, l_nextcropstartdate, true, l_tov, fmc_Others, true, SpringOK);
	}
}

/**
\brief
Gets the start date for a crop type
*/
int Farm::GetFirstDate(TTypesOfVegetation a_tov2) {

	vector<Crop*>* crops_mgt_plans = m_OurManager->GetCropMgtPlans();
	for (unsigned int i = 0; i < crops_mgt_plans->size(); i++)
	{
		if (a_tov2 == (*crops_mgt_plans)[i]->m_tov)
			return (*crops_mgt_plans)[i]->GetFirstDate();
	}

	return 0;
}

/**
\brief
Gets the start date for a crop type
*/
int Farm::GetForceSpringOK(TTypesOfVegetation a_tov) {

	vector<Crop*>* crops_mgt_plans = m_OurManager->GetCropMgtPlans();
	for (unsigned int i = 0; i < crops_mgt_plans->size(); i++)
	{
		if (a_tov == (*crops_mgt_plans)[i]->m_tov)
			return (*crops_mgt_plans)[i]->GetForceSpringOK();
	}
	return 0;
}


TTypesOfVegetation FarmManager::TranslateVegCodes(std::string& astr) {
	// Changed to farm manager - was a Farm function, 19.03.13
	// This simply checks through the list of legal crop names and returns
	// the correct tov type

	string str = astr;

	// Unfortunately switch cannot use string so the long way:
	if (str == "SpringBarley")
		return tov_SpringBarley;
	if (str == "SpringBarleySpr")
		return tov_SpringBarleySpr;
	if (str == "SpringBarleyPTreatment")
		return tov_SpringBarleyPTreatment;
	if (str == "WinterBarley")
		return tov_WinterBarley;
	if (str == "SpringWheat")
		return tov_SpringWheat;
	if (str == "WinterWheat")
		return tov_WinterWheat;
	if (str == "WinterRye")
		return tov_WinterRye;
	if (str == "OrchardCrop")
		return tov_OrchardCrop;
	if (str == "Oats")
		return tov_Oats;
	if (str == "OOats")
		return tov_OOats;
	if (str == "Triticale")
		return tov_Triticale;
	if (str == "OTriticale")
		return tov_OTriticale;
	if (str == "Maize")
		return tov_Maize;
	if (str == "MaizeSilage")
		return tov_MaizeSilage;
	if (str == "SpringBarleySeed")
		return tov_SpringBarleySeed;
	if (str == "OSpringRape")
		return tov_SpringRape; /** @todo Create a OSpringRape crop class and management */
	if (str == "SpringRape")
		return tov_SpringRape;
	if (str == "WinterRape")
		return tov_WinterRape;
	if (str == "BroadBeans")
		return tov_BroadBeans;
	if (str == "FieldPeas")
		return tov_FieldPeas;
	if (str == "FieldPeasSilage")
		return tov_FieldPeasSilage;
	if (str == "Setaside")
		return tov_SetAside;
	if (str == "PermanentSetaside")
		return tov_PermanentSetAside;
	if (str == "SugarBeet")
		return tov_SugarBeet;
	if (str == "FodderBeet")
		return tov_FodderBeet;
	if (str == "OFodderBeet")
		return tov_OFodderBeet;
	if (str == "FodderGrass")
		return tov_FodderGrass;
	if (str == "CloverGrassGrazed1")
		return tov_CloverGrassGrazed1;
	if (str == "PotatoesIndustry")
		return tov_PotatoesIndustry;
	if (str == "Potatoes")
		return tov_Potatoes;
	if (str == "SeedGrass1")
		return tov_SeedGrass1;
	if (str == "OWinterBarley")
		return tov_OWinterBarley;
	if (str == "OWinterBarleyExt")
		return tov_OWinterBarleyExt;
	if (str == "SpringBarleySilage")
		return tov_SpringBarleySilage;
	if (str == "OWinterRye")
		return tov_OWinterRye;
	if (str == "OFieldPeasSilage")
		return tov_OFieldPeasSilage;
	if (str == "SpringBarleyGrass")
		return tov_SpringBarleyGrass;
	if (str == "SpringBarleyCloverGrass")
		return tov_SpringBarleyCloverGrass;
	if (str == "OBarleyPeaCloverGrass")
		return tov_OBarleyPeaCloverGrass;
	if (str == "OWinterRape")
		return tov_OWinterRape;
	if (str == "PermanentGrassGrazed")
		return tov_PermanentGrassGrazed;
	if (str == "PermanentGrassLowYield")
		return tov_PermanentGrassLowYield;
	if (str == "PermanentGrassTussocky")
		return tov_PermanentGrassTussocky;
	if (str == "CloverGrassGrazed2")
		return tov_CloverGrassGrazed2;
	if (str == "SeedGrass2")
		return tov_SeedGrass2;
	if (str == "OSpringWheat")
		return tov_OSpringBarley; /** @todo Create a OSpringWheat crop class and management */
	if (str == "OSpringBarley")
		return tov_OSpringBarley;
	if (str == "OSpringBarleyExt")
		return tov_OSpringBarleyExt;
	if (str == "OWinterWheat")
		return tov_OWinterWheat;
	if (str == "OWinterWheatUndersown")
		return tov_OWinterWheatUndersown;
	if (str == "OWinterWheatUndersownExt")
		return tov_OWinterWheatUndersownExt;
	if (str == "OOats")
		return tov_OOats;
	if (str == "OFieldPeas")
		return tov_OFieldPeas;
	if (str == "OCloverGrassGrazed1")
		return tov_OCloverGrassGrazed1;
	if (str == "OGrazingPigs")
		return tov_OGrazingPigs;
	if (str == "OCarrots")
		return tov_OCarrots;
	if (str == "Carrots")
		return tov_Carrots;
	if (str == "OPotatoes")
		return tov_OPotatoes;
	if (str == "OSeedGrass1")
		return tov_OSeedGrass1;
	if (str == "OSpringBarleyGrass")
		return tov_OSpringBarleyGrass;
	if (str == "OSpringBarleyClover")
		return tov_OSpringBarleyClover;
	if (str == "OPermanentGrassGrazed")
		return tov_OPermanentGrassGrazed;
	if (str == "OCloverGrassSilage1")
		return tov_OCloverGrassSilage1;
	if (str == "OCloverGrassGrazed2")
		return tov_OCloverGrassGrazed2;
	if (str == "OSeedGrass2")
		return tov_OSeedGrass2;
	if (str == "WWheatPControl")
		return tov_WWheatPControl;
	if (str == "WWheatPToxicControl")
		return tov_WWheatPToxicControl;
	if (str == "WWheatPTreatment")
		return tov_WWheatPTreatment;
	if (str == "AgroChemIndustryCereal")
		return tov_AgroChemIndustryCereal;
	if (str == "WinterWheatShort")
		return tov_WinterWheatShort;
	if (str == "WinterWheatStrigling")
		return tov_WinterWheatStrigling;
	if (str == "WinterWheatStriglingCulm")
		return tov_WinterWheatStriglingCulm;
	if (str == "WinterWheatStriglingSgl")
		return tov_WinterWheatStriglingSingle;
	if (str == "SpringBarleyCloverGrassStrigling")
		return tov_SpringBarleyCloverGrassStrigling;
	if (str == "SpringBarleyStrigling")
		return tov_SpringBarleyStrigling;
	if (str == "SpringBarleyStriglingSingle")
		return tov_SpringBarleyStriglingSingle;
	if (str == "SpringBarleyStriglingCulm")
		return tov_SpringBarleyStriglingCulm;
	if (str == "MaizeStrigling")
		return tov_MaizeStrigling;
	if (str == "WinterRapeStrigling")
		return tov_WinterRapeStrigling;
	if (str == "WinterRyeStrigling")
		return tov_WinterRyeStrigling;
	if (str == "WinterBarleyStrigling")
		return tov_WinterBarleyStrigling;
	if (str == "FieldPeasStrigling")
		return tov_FieldPeasStrigling;
	if (str == "SpringBarleyPeaCloverGrassStrigling")
		return tov_SpringBarleyPeaCloverGrassStrigling;
	if (str == "YoungForest")
		return tov_YoungForest;
	if (str == "OMaizeSilage")
		return tov_OMaizeSilage;
	if (str == "OSpringBarleySilage")
		return tov_OSBarleySilage;
	if (str == "OSpringBarleyPigs")
		return tov_OSpringBarleyPigs;
	if (str == "NorwegianPotatoes")
		return tov_NorwegianPotatoes;
	if (str == "NorwegianOats")
		return tov_NorwegianOats;
	if (str == "NorwegianSpringBarley")
		return tov_NorwegianSpringBarley;
	if (str == "PlantNursery")
		return tov_PlantNursery;
	if (str == "PLWinterWheat")
		return tov_PLWinterWheat;
	if (str == "PLWinterRape")
		return tov_PLWinterRape;
	if (str == "PLWinterBarley")
		return tov_PLWinterBarley;
	if (str == "PLWinterTriticale")
		return tov_PLWinterTriticale;
	if (str == "PLWinterRye")
		return tov_PLWinterRye;
	if (str == "PLSpringWheat")
		return tov_PLSpringWheat;
	if (str == "PLSpringBarley")
		return tov_PLSpringBarley;
	if (str == "PLMaize")
		return tov_PLMaize;
	if (str == "PLMaizeSilage")
		return tov_PLMaizeSilage;
	if (str == "PLPotatoes")
		return tov_PLPotatoes;
	if (str == "PLBeet")
		return tov_PLBeet;
	if (str == "PLFodderLucerne1")
		return tov_PLFodderLucerne1;
	if (str == "PLFodderLucerne2")
		return tov_PLFodderLucerne2;
	if (str == "PLCarrots")
		return tov_PLCarrots;
	if (str == "PLSpringBarleySpr")
		return tov_PLSpringBarleySpr;
	if (str == "PLWinterWheatLate")
		return tov_PLWinterWheatLate;
	if (str == "PLBeetSpr")
		return tov_PLBeetSpr;
	if (str == "PLBeans")
		return tov_PLBeans;

	if (str == "NLBeet")
		return tov_NLBeet;
	if (str == "NLCarrots")
		return tov_NLCarrots;
	if (str == "NLMaize")
		return tov_NLMaize;
	if (str == "NLPotatoes")
		return tov_NLPotatoes;
	if (str == "NLSpringBarley")
		return tov_NLSpringBarley;
	if (str == "NLWinterWheat")
		return tov_NLWinterWheat;
	if (str == "NLCabbage")
		return tov_NLCabbage;
	if (str == "NLTulips")
		return tov_NLTulips;
	if (str == "NLGrassGrazed1")
		return tov_NLGrassGrazed1;
	if (str == "NLGrassGrazed1Spring")
		return tov_NLGrassGrazed1Spring;
	if (str == "NLGrassGrazed2")
		return tov_NLGrassGrazed2;
	if (str == "NLGrassGrazedLast")
		return tov_NLGrassGrazedLast;
	if (str == "NLPermanentGrassGrazed")
		return tov_NLPermanentGrassGrazed;
	if (str == "NLBeetSpring")
		return tov_NLBeetSpring;
	if (str == "NLCarrotsSpring")
		return tov_NLCarrotsSpring;
	if (str == "NLMaizeSpring")
		return tov_NLMaizeSpring;
	if (str == "NLPotatoesSpring")
		return tov_NLPotatoesSpring;
	if (str == "NLSpringBarleySpring")
		return tov_NLSpringBarleySpring;
	if (str == "NLCabbageSpring")
		return tov_NLCabbageSpring;
	if (str == "NLCatchCropPea")
		return tov_NLCatchCropPea;
	if (str == "NLOrchardCrop")
		return tov_NLOrchardCrop;
	if (str == "NLPermanentGrassGrazedExtensive")
		return tov_NLPermanentGrassGrazedExtensive;
	if (str == "NLGrassGrazedExtensive1")
		return tov_NLGrassGrazedExtensive1;
	if (str == "NLGrassGrazedExtensive1Spring")
		return tov_NLGrassGrazedExtensive1Spring;
	if (str == "NLGrassGrazedExtensive2")
		return tov_NLGrassGrazedExtensive2;
	if (str == "NLGrassGrazedExtensiveLast")
		return tov_NLGrassGrazedExtensiveLast;

	if (str == "UKBeans")
		return tov_UKBeans;
	if (str == "UKBeet")
		return tov_UKBeet;
	if (str == "UKMaize")
		return tov_UKMaize;
	if (str == "UKPermanentGrass")
		return tov_UKPermanentGrass;
	if (str == "UKPotatoes")
		return tov_UKPotatoes;
	if (str == "UKSpringBarley")
		return tov_UKSpringBarley;
	if (str == "UKTempGrass")
		return tov_UKTempGrass;
	if (str == "UKWinterBarley")
		return tov_UKWinterBarley;
	if (str == "UKWinterRape")
		return tov_UKWinterRape;
	if (str == "UKWinterWheat")
		return tov_UKWinterWheat;

	if (str == "BEBeet")
		return tov_BEBeet;
	if (str == "BEBeetSpring")
		return tov_BEBeetSpring;
	if (str == "BECatchPeaCrop")
		return tov_BECatchPeaCrop;
	if (str == "BEGrassGrazed1")
		return tov_BEGrassGrazed1;
	if (str == "BEGrassGrazed1Spring")
		return tov_BEGrassGrazed1Spring;
	if (str == "BEGrassGrazed2")
		return tov_BEGrassGrazed2;
	if (str == "BEGrassGrazedLast")
		return tov_BEGrassGrazedLast;
	if (str == "BEMaize")
		return tov_BEMaize;
	if (str == "BEMaizeCC")
		return tov_BEMaizeCC;
	if (str == "BEMaizeSpring")
		return tov_BEMaizeSpring;
	if (str == "BEOrchardCrop")
		return tov_BEOrchardCrop;
	if (str == "BEPotatoes")
		return tov_BEPotatoes;
	if (str == "BEPotatoesSpring")
		return tov_BEPotatoesSpring;
	if (str == "BEWinterBarley")
		return tov_BEWinterBarley;
	if (str == "BEWinterBarleyCC")
		return tov_BEWinterBarleyCC;
	if (str == "BEWinterWheat")
		return tov_BEWinterWheat;
	if (str == "BEWinterWheatCC")
		return tov_BEWinterWheatCC;

	if (str == "PTPermanentGrassGrazed")
		return tov_PTPermanentGrassGrazed;
	if (str == "PTWinterWheat")
		return tov_PTWinterWheat;
	if (str == "PTGrassGrazed")
		return tov_PTGrassGrazed;
	if (str == "PTSorghum")
		return tov_PTSorghum;
	if (str == "PTFodderMix")
		return tov_PTFodderMix;
	if (str == "PTTurnipGrazed")
		return tov_PTTurnipGrazed;
	if (str == "PTCloverGrassGrazed1")
		return tov_PTCloverGrassGrazed1;
	if (str == "PTCloverGrassGrazed2")
		return tov_PTCloverGrassGrazed2;
	if (str == "PTTriticale")
		return tov_PTTriticale;
	if (str == "PTOtherDryBeans")
		return tov_PTOtherDryBeans;
	if (str == "PTShrubPastures")
		return tov_PTShrubPastures;
	if (str == "PTCorkOak")
		return tov_PTCorkOak;
	if (str == "PTVineyards")
		return tov_PTVineyards;
	if (str == "PTWinterBarley")
		return tov_PTWinterBarley;
	if (str == "PTBeans")
		return tov_PTBeans;
	if (str == "PTWinterRye")
		return tov_PTWinterRye;
	if (str == "PTRyegrass")
		return tov_PTRyegrass;
	if (str == "PTYellowLupin")
		return tov_PTYellowLupin;
	if (str == "PTMaize")
		return tov_PTMaize;
	if (str == "PTOats")
		return tov_PTOats;
	if (str == "PTPotatoes")
		return tov_PTPotatoes;
	if (str == "PTHorticulture")
		return tov_PTHorticulture;
	if (str == "PTCabbage")
		return tov_PTCabbage;
	if (str == "PTCabbage_Hort")
		return tov_PTCabbage_Hort;
	if (str == "PTMaize_Hort")
		return tov_PTMaize_Hort;
	if (str == "PTOliveGroveTraditional")
		return tov_PTOliveGroveTraditional;
	if (str == "PTOliveGroveTradOrganic")
		return tov_PTOliveGroveTradOrganic;
	if (str == "PTOliveGroveIntensive")
		return tov_PTOliveGroveIntensive;
	if (str == "PTOliveGroveSuperIntensive")
		return tov_PTOliveGroveSuperIntensive;

	if (str == "DEOats")
		return tov_DEOats;
	if (str == "DESpringRye")
		return tov_DESpringRye;
	if (str == "DEWinterWheat")
		return tov_DEWinterWheat;
	if (str == "DEWinterWheatLate")
		return tov_DEWinterWheatLate;
	if (str == "DEMaizeSilage")
		return tov_DEMaizeSilage;
	if (str == "DEPeas")
		return tov_DEPeas;
	if (str == "DEPotatoes")
		return tov_DEPotatoes;
	if (str == "DEPotatoesIndustry")
		return tov_DEPotatoesIndustry;
	if (str == "DEMaize")
		return tov_DEMaize;
	if (str == "DEWinterRye")
		return tov_DEWinterRye;
	if (str == "DEWinterBarley")
		return tov_DEWinterBarley;
	if (str == "DESugarBeet")
		return tov_DESugarBeet;
	if (str == "DEWinterRape")
		return tov_DEWinterRape;
	if (str == "DETriticale")
		return tov_DETriticale;
	if (str == "DECabbage")
		return tov_DECabbage;
	if (str == "DECarrots")
		return tov_DECarrots;
	if (str == "DEGrasslandSilageAnnual")
		return tov_DEGrasslandSilageAnnual;
	if (str == "DEGreenFallow_1year")
		return tov_DEGreenFallow_1year;
	if (str == "DELegumes")
		return tov_DELegumes;
	if (str == "DEOCabbages")
		return tov_DEOCabbages;
	if (str == "DEOCarrots")
		return tov_DEOCarrots;
	if (str == "DEOGrasslandSilageAnnual")
		return tov_DEOGrasslandSilageAnnual;
	if (str == "DEOGreenFallow_1year")
		return tov_DEOGreenFallow_1year;
	if (str == "DEOLegume")
		return tov_DEOLegume;
	if (str == "DEOMaize")
		return tov_DEOMaize;
	if (str == "DEOMaizeSilage")
		return tov_DEOMaizeSilage;
	if (str == "DEOOats")
		return tov_DEOOats;
	if (str == "DEOPeas")
		return tov_DEOPeas;
	if (str == "DEOPermanentGrassGrazed")
		return tov_DEOPermanentGrassGrazed;
	if (str == "DEOPotatoes")
		return tov_DEOPotatoes;
	if (str == "DEOSpringRye")
		return tov_DEOSpringRye;
	if (str == "DEOSugarBeet")
		return tov_DEOSugarBeet;
	if (str == "DEOTriticale")
		return tov_DEOTriticale;
	if (str == "DEOWinterBarley")
		return tov_DEOWinterBarley;
	if (str == "DEOWinterRape")
		return tov_DEOWinterRape;
	if (str == "DEOWinterRye")
		return tov_DEOWinterRye;
	if (str == "DEOWinterWheat")
		return tov_DEOWinterWheat;
	if (str == "DEPermanentGrassGrazed")
		return tov_DEPermanentGrassGrazed;
	if (str == "DEPermanentGrassLowYield")
		return tov_DEPermanentGrassLowYield;
	if (str == "DEOPermanentGrassLowYield")
		return tov_DEOPermanentGrassLowYield;
	if (str == "DEAsparagusEstablishedPlantation")
		return tov_DEAsparagusEstablishedPlantation;
	if (str == "DEOAsparagusEstablishedPlantation")
		return tov_DEOAsparagusEstablishedPlantation;
	if (str == "DEHerbsPerennial_1year")
		return tov_DEHerbsPerennial_1year;
	if (str == "DEHerbsPerennial_after1year")
		return tov_DEHerbsPerennial_after1year;
	if (str == "DEOHerbsPerennial_1year")
		return tov_DEOHerbsPerennial_1year;
	if (str == "DEOHerbsPerennial_after1year")
		return tov_DEOHerbsPerennial_after1year;
	if (str == "DESpringBarley")
		return tov_DESpringBarley;
	if (str == "DEOrchard")
		return tov_DEOrchard;
	if (str == "DEOOrchard")
		return tov_DEOOrchard;
	if (str == "DEOBushFruitPerm")
		return tov_DEOBushFruitPerm;

	if (str == "DummyCropPestTesting") return tov_DummyCropPestTesting;

	if (str == "SpringBarleySKManagement") return tov_SpringBarleySKManagement;

	// new tovs in DK
	if (str == "DKCabbages") return tov_DKCabbages;
	if (str == "DKCarrots") return tov_DKCarrots;
	if (str == "DKOLegumeCloverGrass_Whole") return tov_DKOLegumeCloverGrass_Whole;
	if (str == "DKLegume_Peas") return tov_DKLegume_Peas;
	if (str == "DKLegume_Beans") return tov_DKLegume_Beans;
	if (str == "DKLegume_Whole") return tov_DKLegume_Whole;
	if (str == "DKOCabbages") return tov_DKOCabbages;
	if (str == "DKOCarrots") return tov_DKOCarrots;
	if (str == "DKOLegume_Peas") return tov_DKOLegume_Peas;
	if (str == "DKOLegume_Beans") return tov_DKOLegume_Beans;
	if (str == "DKOLegume_Whole_CC") return tov_DKOLegume_Whole;
	if (str == "DKOLegume_Peas_CC") return tov_DKOLegume_Peas_CC;
	if (str == "DKOLegume_Beans_CC") return tov_DKOLegume_Beans_CC;
	if (str == "DKOLegume_Whole") return tov_DKOLegume_Whole_CC;
	if (str == "DKOSugarBeets") return tov_DKOSugarBeets;
	if (str == "DKSugarBeets") return tov_DKSugarBeets;
	if (str == "DKWinterWheat") return tov_DKWinterWheat;
	if (str == "DKOWinterWheat") return tov_DKOWinterWheat;
	if (str == "DKWinterWheat_CC") return tov_DKWinterWheat_CC;
	if (str == "DKOWinterWheat_CC") return tov_DKOWinterWheat_CC;
	if (str == "DKSpringBarley") return tov_DKSpringBarley;
	if (str == "DKOSpringBarley") return tov_DKOSpringBarley;
	if (str == "DKSpringBarley_CC") return tov_DKSpringBarley_CC;
	if (str == "DKOSpringBarley_CC") return tov_DKOSpringBarley_CC;
	if (str == "DKSpringBarleyCloverGrass") return tov_DKSpringBarleyCloverGrass;
	if (str == "DKOSpringBarleyCloverGrass") return tov_DKOSpringBarleyCloverGrass;
	if (str == "DKCerealLegume") return tov_DKCerealLegume;
	if (str == "DKOCerealLegume") return tov_DKOCerealLegume;
	if (str == "DKCerealLegume_Whole") return tov_DKCerealLegume_Whole;
	if (str == "DKOCerealLegume_Whole") return tov_DKOCerealLegume_Whole;
	if (str == "DKWinterCloverGrassGrazedSown") return tov_DKWinterCloverGrassGrazedSown;
	if (str == "DKCloverGrassGrazed1") return tov_DKCloverGrassGrazed1;
	if (str == "DKCloverGrassGrazed2") return tov_DKCloverGrassGrazed2;
	if (str == "DKCloverGrassGrazed3") return tov_DKCloverGrassGrazed3;
	if (str == "DKOWinterCloverGrassGrazedSown") return tov_DKOWinterCloverGrassGrazedSown;
	if (str == "DKOCloverGrassGrazed1") return tov_DKOCloverGrassGrazed1;
	if (str == "DKOCloverGrassGrazed2") return tov_DKOCloverGrassGrazed2;
	if (str == "DKOCloverGrassGrazed3") return tov_DKOCloverGrassGrazed3;
	if (str == "DKSpringFodderGrass") return tov_DKSpringFodderGrass;
	if (str == "DKWinterFodderGrass") return tov_DKWinterFodderGrass;
	if (str == "DKOWinterFodderGrass") return tov_DKOWinterFodderGrass;
	if (str == "DKOSpringFodderGrass") return tov_DKOSpringFodderGrass;
	if (str == "DKMaize") return tov_DKMaize;
	if (str == "DKMaizeSilage") return tov_DKMaizeSilage;
	if (str == "DKMixedVeg") return tov_DKMixedVeg;
	if (str == "DKOGrazingPigs") return tov_DKOGrazingPigs;
	if (str == "DKOMaize") return tov_DKOMaize;
	if (str == "DKOMaizeSilage") return tov_DKOMaizeSilage;
	if (str == "DKOMixedVeg") return tov_DKOMixedVeg;
	if (str == "DKOPotato") return tov_DKOPotato;
	if (str == "DKOPotatoIndustry") return tov_DKOPotatoIndustry;
	if (str == "DKOPotatoSeed") return tov_DKOPotatoSeed;
	if (str == "DKPotato") return tov_DKPotato;
	if (str == "DKPotatoIndustry") return tov_DKPotatoIndustry;
	if (str == "DKPotatoSeed") return tov_DKPotatoSeed;
	if (str == "DKOSeedGrassRye_Spring") return tov_DKOSeedGrassRye_Spring;
	if (str == "DKOSetAside") return tov_DKOSetAside;
	if (str == "DKOSetAside_AnnualFlower") return tov_DKOSetAside_AnnualFlower;
	if (str == "DKOSetAside_PerennialFlower") return tov_DKOSetAside_PerennialFlower;
	if (str == "DKOSetAside_SummerMow") return tov_DKOSetAside_SummerMow;
	if (str == "DKOSpringBarleySilage") return tov_DKOSpringBarleySilage;
	if (str == "DKOSpringOats") return tov_DKOSpringOats;
	if (str == "DKOSpringWheat") return tov_DKOSpringWheat;
	if (str == "DKOVegSeeds") return tov_DKOVegSeeds;
	if (str == "DKOWinterBarley") return tov_DKOWinterBarley;
	if (str == "DKOWinterRape") return tov_DKOWinterRape;
	if (str == "DKOWinterRye") return tov_DKOWinterRye;
	if (str == "DKSeedGrassFescue_Spring") return tov_DKSeedGrassFescue_Spring;
	if (str == "DKSetAside") return tov_DKSetAside;
	if (str == "DKSetAside_SummerMow") return tov_DKSetAside_SummerMow;
	if (str == "DKSpringBarley_Green") return tov_DKSpringBarley_Green;
	if (str == "DKSpringBarleySilage") return tov_DKSpringBarleySilage;
	if (str == "DKSpringOats") return tov_DKSpringOats;
	if (str == "DKOSpringOats_CC") return tov_DKOSpringOats_CC;
	if (str == "DKSpringOats_CC") return tov_DKSpringOats_CC;
	if (str == "DKSpringWheat") return tov_DKSpringWheat;
	if (str == "DKUndefined") return tov_DKUndefined;
	if (str == "DKVegSeeds") return tov_DKVegSeeds;
	if (str == "DKWinterBarley") return tov_DKWinterBarley;
	if (str == "DKSeedGrassRye_Spring") return tov_DKSeedGrassRye_Spring;
	if (str == "DKWinterRape") return tov_DKWinterRape;
	if (str == "DKWinterRye") return tov_DKWinterRye;
	if (str == "DKWinterRye_CC") return tov_DKWinterRye_CC;
	if (str == "DKOWinterRye_CC") return tov_DKOWinterRye_CC;
	if (str == "DKOOrchApple") 	return tov_DKOOrchApple;
	if (str == "DKOOrchPear") 	return tov_DKOOrchPear;
	if (str == "DKOOrchCherry") 	return tov_DKOOrchCherry;
	if (str == "DKOOrchOther") 	return tov_DKOOrchOther;
	if (str == "DKOrchApple") 	return tov_DKOrchApple;
	if (str == "DKOrchPear") 	return tov_DKOrchPear;
	if (str == "DKOrchCherry") 	return tov_DKOrchCherry;
	if (str == "DKOrchOther") 	return tov_DKOrchOther;
	if (str == "DKFodderBeet") return tov_DKFodderBeets;
	if (str == "DKOFodderBeet") return tov_DKOFodderBeets;
	if (str == "DKCatchCrop") return tov_DKCatchCrop;
	if (str == "DKOCatchCrop") return tov_DKOCatchCrop;
	if (str == "DKOLupines") return tov_DKOLupines;
	if (str == "DKOLentils") return tov_DKOLentils;
	if (str == "DKOptimalFlowerMix1") return tov_DKOptimalFlowerMix1;
	if (str == "DKOptimalFlowerMix2") return tov_DKOptimalFlowerMix2;
	if (str == "DKOptimalFlowerMix3") return tov_DKOptimalFlowerMix3;

	if (str == "DKOOrchardCrop_Perm") 	return tov_DKOOrchardCrop_Perm;
	if (str == "DKOrchardCrop_Perm") 	return tov_DKOrchardCrop_Perm;
	if (str == "DKBushFruit_Perm1")	return tov_DKBushFruit_Perm1;
	if (str == "DKBushFruit_Perm2")	return tov_DKBushFruit_Perm2;
	if (str == "DKOBushFruit_Perm1")	return tov_DKOBushFruit_Perm1;
	if (str == "DKOBushFruit_Perm2")	return tov_DKOBushFruit_Perm2;
	if (str == "DKChristmasTrees_Perm")	return tov_DKChristmasTrees_Perm;
	if (str == "DKOChristmasTrees_Perm")	return tov_DKOChristmasTrees_Perm;
	if (str == "DKEnergyCrop_Perm")	return tov_DKEnergyCrop_Perm;
	if (str == "DKOEnergyCrop_Perm")	return tov_DKOEnergyCrop_Perm;
	if (str == "DKFarmForest_Perm")	return tov_DKFarmForest_Perm;
	if (str == "DKOFarmForest_Perm")	return tov_DKOFarmForest_Perm;
	if (str == "DKGrazingPigs")		return tov_DKGrazingPigs;
	if (str == "DKGrazingPigs_Perm")	return tov_DKGrazingPigs_Perm;
	if (str == "DKOGrazingPigs_Perm")	return tov_DKOGrazingPigs_Perm;
	if (str == "DKOGrassGrazed_Perm")	return tov_DKOGrassGrazed_Perm;
	if (str == "DKOGrassLowYield_Perm")	return tov_DKOGrassLowYield_Perm;
	if (str == "DKGrassGrazed_Perm")	return tov_DKGrassGrazed_Perm;
	if (str == "DKGrassLowYield_Perm")	return tov_DKGrassLowYield_Perm;
	if (str == "DKGrassTussocky_Perm")	return tov_DKGrassTussocky_Perm;
	if (str == "DKFarmYoungForest_Perm")	return tov_DKFarmYoungForest_Perm;
	if (str == "DKOFarmYoungForest_Perm")	return tov_DKOFarmYoungForest_Perm;
	if (str == "DKPlantNursery_Perm")	return tov_DKPlantNursery_Perm;

	// new tovs in FI
	if (str == "FIWinterWheat")
		return tov_FIWinterWheat;
	if (str == "FIOWinterWheat")
		return tov_FIOWinterWheat;
	if (str == "FISugarBeet")
		return tov_FISugarBeet;
	if (str == "FIStarchPotato_North")
		return tov_FIStarchPotato_North;
	if (str == "FIStarchPotato_South")
		return tov_FIStarchPotato_South;
	if (str == "FIOStarchPotato_North")
		return tov_FIOStarchPotato_North;
	if (str == "FIOStarchPotato_South")
		return tov_FIOStarchPotato_South;
	if (str == "FISpringWheat")
		return tov_FISpringWheat;
	if (str == "FIOSpringWheat")
		return tov_FIOSpringWheat;
	if (str == "FITurnipRape")
		return tov_FITurnipRape;
	if (str == "FIOTurnipRape")
		return tov_FIOTurnipRape;
	if (str == "FISpringRape")
		return tov_FISpringRape;
	if (str == "FIOSpringRape")
		return tov_FIOSpringRape;
	if (str == "FIWinterRye")
		return tov_FIWinterRye;
	if (str == "FIOWinterRye")
		return tov_FIOWinterRye;
	if (str == "FIPotato_North")
		return tov_FIPotato_North;
	if (str == "FIPotato_South")
		return tov_FIPotato_South;
	if (str == "FIOPotato_North")
		return tov_FIOPotato_North;
	if (str == "FIOPotato_South")
		return tov_FIOPotato_South;
	if (str == "FIPotatoIndustry_North")
		return tov_FIPotatoIndustry_North;
	if (str == "FIPotatoIndustry_South")
		return tov_FIPotatoIndustry_South;
	if (str == "FIOPotatoIndustry_North")
		return tov_FIOPotatoIndustry_North;
	if (str == "FIOPotatoIndustry_South")
		return tov_FIOPotatoIndustry_South;
	if (str == "FISpringOats")
		return tov_FISpringOats;
	if (str == "FIOSpringOats")
		return tov_FIOSpringOats;
	if (str == "FISpringBarley_Malt")
		return tov_FISpringBarley_Malt;
	if (str == "FIOSpringBarley_Malt")
		return tov_FIOSpringBarley_Malt;
	if (str == "FIFabaBean")
		return tov_FIFabaBean;
	if (str == "FIOFabaBean")
		return tov_FIOFabaBean;
	if (str == "FISpringBarley_Fodder")
		return tov_FISpringBarley_Fodder;
	if (str == "FISprSpringBarley_Fodder")
		return tov_FISprSpringBarley_Fodder;
	if (str == "FIOSpringBarley_Fodder")
		return tov_FIOSpringBarley_Fodder;
	if (str == "FIGrasslandPasturePerennial1")
		return tov_FIGrasslandPasturePerennial1;
	if (str == "FIGrasslandPasturePerennial2")
		return tov_FIGrasslandPasturePerennial2;
	if (str == "FIGrasslandSilagePerennial1")
		return tov_FIGrasslandSilagePerennial1;
	if (str == "FIGrasslandSilagePerennial2")
		return tov_FIGrasslandSilagePerennial2;
	if (str == "FINaturalGrassland")
		return tov_FINaturalGrassland;
	if (str == "FINaturalGrassland_Perm")
		return tov_FINaturalGrassland_Perm;
	if (str == "FIFeedingGround")
		return tov_FIFeedingGround;
	if (str == "FIBufferZone")
		return tov_FIBufferZone;
	if (str == "FIBufferZone_Perm")
		return tov_FIBufferZone_Perm;
	if (str == "FIGreenFallow_1year")
		return tov_FIGreenFallow_1year;
	if (str == "FIGreenFallow_Perm")
		return tov_FIGreenFallow_Perm;
	if (str == "FIGrasslandSilageAnnual")
		return tov_FIGrasslandSilageAnnual;
	if (str == "FICaraway1")
		return tov_FICaraway1;
	if (str == "FICaraway2")
		return tov_FICaraway2;
	if (str == "FIOCaraway1")
		return tov_FIOCaraway1;
	if (str == "FIOCaraway2")
		return tov_FIOCaraway2;
	if (str == "SESpringBarley")
		return tov_SESpringBarley;
	if (str == "SEWinterRape_Seed")
		return tov_SEWinterRape_Seed;
	if (str == "SEWinterWheat")
		return tov_SEWinterWheat;

	if (str == "IRSpringWheat")
		return tov_IRSpringWheat;
	if (str == "IRSpringbarley")
		return tov_IRSpringBarley;
	if (str == "IRSpringOats")
		return tov_IRSpringOats;
	if (str == "IRGrassland_no_reseed")
		return tov_IRGrassland_no_reseed;
	if (str == "IRGrassland_reseed")
		return tov_IRGrassland_reseed;
	if (str == "IRWinterBarley")
		tov_IRWinterBarley;
	if (str == "IRWinterWheat")
		tov_IRWinterWheat;
	if (str == "IRWinterOats")
		tov_IRWinterOats;

	if (str == "FRWinterWheat") return tov_FRWinterWheat;
	if (str == "FRWinterBarley") return tov_FRWinterBarley;
	if (str == "FRWinterTriticale") return tov_FRWinterTriticale;
	if (str == "FRWinterRape") return tov_FRWinterRape;
	if (str == "FRMaize") return tov_FRMaize;
	if (str == "FRMaize_Silage") return tov_FRMaize_Silage;
	if (str == "FRSpringBarley") return tov_FRSpringBarley;
	if (str == "FRGrassland") return tov_FRGrassland;
	if (str == "FRGrassland_Perm") return tov_FRGrassland_Perm;
	if (str == "FRSpringOats") return tov_FRSpringOats;
	if (str == "FRSunflower") return tov_FRSunflower;
	if (str == "FRSpringWheat") return tov_FRSpringWheat;
	if (str == "FRPotatoes") return tov_FRPotatoes;
	if (str == "FRSorghum") return tov_FRSorghum;
	if (str == "ITGrassland") return tov_ITGrassland;
	if (str == "ITOrchard") return tov_ITOrchard;
	if (str == "ITOOrchard") return tov_ITOOrchard;

	// No match so issue a warning and quit
	g_msg->Warn(WARN_FILE, "FarmManager::TranslateVegCodes():"" Unknown Crop Code ", str.c_str());
	exit(1);
}

TTypesOfCrops FarmManager::TranslateCropCodes(std::string& astr) {
	// This simply checks through the list of legal crop names and returns
	// the correct crop type
	string str = astr;

	if (str == "AsparagusEstablishedPlantation") return toc_AsparagusEstablishedPlantation;
	if (str == "Beans") return toc_Beans;
	if (str == "Beans_Whole") return toc_Beans_Whole;
	if (str == "Beet") return toc_Beet;
	if (str == "BushFruit") return toc_BushFruit;
	if (str == "Cabbage") return toc_Cabbage;
	if (str == "CabbageSpring") return toc_CabbageSpring;
	if (str == "Carrots") return toc_Carrots;
	if (str == "CarrotsSpring") return toc_CarrotsSpring;
	if (str == "CatchCropPea") return toc_CatchCropPea;
	if (str == "CloverGrassGrazed1") return toc_CloverGrassGrazed1;
	if (str == "CloverGrassGrazed2") return toc_CloverGrassGrazed2;
	if (str == "CloverGrassGrazed3") return toc_CloverGrassGrazed3;
	if (str == "DummyCropPestTesting") return toc_DummyCropPestTesting;
	if (str == "FarmForest") return toc_FarmForest;
	if (str == "FieldPeas") return toc_FieldPeas;
	if (str == "FieldPeasSilage") return toc_FieldPeasSilage;
	if (str == "FieldPeasStrigling") return toc_FieldPeasStrigling;
	if (str == "FodderBeet") return toc_FodderBeet;
	if (str == "FodderGrass") return toc_FodderGrass;
	if (str == "FodderLucerne1") return toc_FodderLucerne1;
	if (str == "FodderLucerne2") return toc_FodderLucerne2;
	if (str == "GenericCatchCrop") return toc_GenericCatchCrop;
	if (str == "GrassGrazed1") return toc_GrassGrazed1;
	if (str == "GrassGrazed2") return toc_GrassGrazed2;
	if (str == "GrassGrazedExtensive") return toc_GrassGrazedExtensive;
	if (str == "GrassGrazedLast") return toc_GrassGrazedLast;
	if (str == "GrazingPigs") return toc_GrazingPigs;
	if (str == "Maize") return toc_Maize;
	if (str == "MaizeSilage") return toc_MaizeSilage;
	if (str == "MaizeSpring") return toc_MaizeSpring;
	if (str == "MaizeStrigling") return toc_MaizeStrigling;
	if (str == "MixedVeg") return toc_MixedVeg;
	if (str == "OAsparagusEstablishedPlantation") return toc_OAsparagusEstablishedPlantation;
	if (str == "Oats") return toc_Oats;
	if (str == "OBarleyPeaCloverGrass") return toc_OBarleyPeaCloverGrass;
	if (str == "OBeans") return toc_OBeans;
	if (str == "OBeans_Whole") return toc_OBeans_Whole;
	if (str == "OBushFruit") return toc_OBushFruit;
	if (str == "OCabbage") return toc_OCabbage;
	if (str == "OCarrots") return toc_OCarrots;
	if (str == "OCloverGrassGrazed1") return toc_OCloverGrassGrazed1;
	if (str == "OCloverGrassGrazed2") return toc_OCloverGrassGrazed2;
	if (str == "OCloverGrassGrazed3") return toc_OCloverGrassGrazed3;
	if (str == "OCloverGrassSilage1") return toc_OCloverGrassSilage1;
	if (str == "OFarmForest") return toc_OFarmForest;
	if (str == "OFieldPeas") return toc_OFieldPeas;
	if (str == "OFieldPeasSilage") return toc_OFieldPeasSilage;
	if (str == "OFirstYearDanger") return toc_OFirstYearDanger;
	if (str == "OFodderBeet") return toc_OFodderBeet;
	if (str == "OFodderGrass") return toc_OFodderGrass;
	if (str == "OGrazingPigs") return toc_OGrazingPigs;
	if (str == "OliveGrove") return toc_OliveGrove;
	if (str == "OLentils") return toc_OLentils;
	if (str == "OLupines") return toc_OLupines;
	if (str == "OMaize") return toc_OMaize;
	if (str == "OMaizeSilage") return toc_OMaizeSilage;
	if (str == "OMixedVeg") return toc_OMixedVeg;
	if (str == "OOats") return toc_OOats;
	if (str == "OOrchardCrop") return toc_OOrchardCrop;
	if (str == "OPermanentGrassLowYield") return toc_OPermanentGrassLowYield;
	if (str == "OrchApple") return toc_OrchApple;
	if (str == "OrchPear") return toc_OrchPear;
	if (str == "OrchCherry") return toc_OrchCherry;
	if (str == "OrchOther") return toc_OrchOther;
	if (str == "OOrchApple") return toc_OOrchApple;
	if (str == "OOrchPear") return toc_OOrchPear;
	if (str == "OOrchCherry") return toc_OOrchCherry;
	if (str == "OOrchOther") return toc_OOrchOther;
	if (str == "OPermanentGrassGrazed") return toc_OPermanentGrassGrazed;
	if (str == "OPermanentGrassLowYield") return toc_OPermanentGrassLowYield;
	if (str == "OPotatoes") return toc_OPotatoes;
	if (str == "OPotatoesIndustry") return toc_OPotatoesIndustry;
	if (str == "OPotatoesSeed") return toc_OPotatoesSeed;
	if (str == "OrchardCrop") return toc_OrchardCrop;
	if (str == "OSBarleySilage") return toc_OSBarleySilage;
	if (str == "OSeedGrass1") return toc_OSeedGrass1;
	if (str == "OSeedGrass2") return toc_OSeedGrass2;
	if (str == "OSetAside") return toc_OSetAside;
	if (str == "OSetAside_Flower") return toc_OSetAside_Flower;
	if (str == "OSpringBarley") return toc_OSpringBarley;
	if (str == "OSpringBarleyCloverGrass") return toc_OSpringBarleyCloverGrass;
	if (str == "OSpringBarleyExtensive") return toc_OSpringBarleyExtensive;
	if (str == "OSpringBarleyPeaCloverGrass") return toc_OSpringBarleyPeaCloverGrass;
	if (str == "OSpringBarleyPigs") return toc_OSpringBarleyPigs;
	if (str == "OSpringBarleySilage") return toc_OSpringBarleySilage;
	if (str == "OSpringRape") return toc_OSpringRape;
	if (str == "OSpringWheat") return toc_OSpringWheat;
	if (str == "OStarchPotato") return toc_OStarchPotato;
	if (str == "OSugarBeet") return toc_OSugarBeet;
	if (str == "OTriticale") return toc_OTriticale;
	if (str == "OVegSeeds") return toc_OVegSeeds;
	if (str == "OWinterBarley") return toc_OWinterBarley;
	if (str == "OWinterBarleyExtensive") return toc_OWinterBarleyExtensive;
	if (str == "OWinterRape") return toc_OWinterRape;
	if (str == "OWinterRye") return toc_OWinterRye;
	if (str == "OWinterWheat") return toc_OWinterWheat;
	if (str == "OWinterWheatUndersown") return toc_OWinterWheatUndersown;
	if (str == "OWinterWheatUndersownExtensive") return toc_OWinterWheatUndersownExtensive;
	if (str == "OYoungForestCrop") return toc_OYoungForestCrop;
	if (str == "PermanentGrassGrazed") return toc_PermanentGrassGrazed;
	if (str == "PermanentGrassLowYield") return toc_PermanentGrassLowYield;
	if (str == "PermanentGrassTussocky") return toc_PermanentGrassTussocky;
	if (str == "PermanentSetAside") return toc_PermanentSetAside;
	if (str == "PlantNursery") return toc_PlantNursery;
	if (str == "Potatoes") return toc_Potatoes;
	if (str == "PotatoesIndustry") return toc_PotatoesIndustry;
	if (str == "PotatoesSeed") return toc_PotatoesSeed;
	if (str == "PotatoesSpring") return toc_PotatoesSpring;
	if (str == "Ryegrass") return toc_Ryegrass;
	if (str == "ORyegrass") return toc_ORyegrass;
	if (str == "SeedGrass1") return toc_SeedGrass1;
	if (str == "SeedGrass2") return toc_SeedGrass2;
	if (str == "SetAside") return toc_SetAside;
	if (str == "SpringBarley") return toc_SpringBarley;
	if (str == "SpringBarleyCloverGrass") return toc_SpringBarleyCloverGrass;
	if (str == "SpringBarleyPeaCloverGrass") return toc_SpringBarleyPeaCloverGrass;
	if (str == "SpringBarleySeed") return toc_SpringBarleySeed;
	if (str == "SpringBarleySilage") return toc_SpringBarleySilage;
	if (str == "SpringRape") return toc_SpringRape;
	if (str == "SpringWheat") return toc_SpringWheat;
	if (str == "StarchPotato") return toc_StarchPotato;
	if (str == "SugarBeet") return toc_SugarBeet;
	if (str == "Sunflower") return toc_Sunflower;
	if (str == "Triticale") return toc_Triticale;
	if (str == "Tulips") return toc_Tulips;
	if (str == "VegSeeds") return toc_VegSeeds;
	if (str == "WinterBarley") return toc_WinterBarley;
	if (str == "WinterRape") return toc_WinterRape;
	if (str == "WinterRye") return toc_WinterRye;
	if (str == "WinterTriticale") return toc_WinterTriticale;
	if (str == "WinterWheat") return toc_WinterWheat;
	if (str == "YoungForestCrop") return toc_YoungForestCrop;
	if (str == "CorkOak") return toc_CorkOak;
	if (str == "Horticulture") return toc_Horticulture;
	if (str == "Turnip") return toc_Turnip;
	if (str == "Vineyards") return toc_Vineyards;
	if (str == "YellowLupin") return toc_YellowLupin;
	if (str == "Foobar") return toc_Foobar;
	// No match so issue a warning and quit
	g_msg->Warn(WARN_FILE, "FarmManager::TranslateCropCodes():"" Unknown Crop Code ", str.c_str());
	exit(142);
}

std::string FarmManager::BackTranslateCropCodes(TTypesOfCrops a_crop) {
	// This simply checks through the list of legal crop types and returns
	// the correct string

	if (a_crop == toc_AsparagusEstablishedPlantation) return "AsparagusEstablishedPlantation";
	if (a_crop == toc_Beans) return  "Beans";
	if (a_crop == toc_Beans_Whole) return  "Beans_Whole";
	if (a_crop == toc_Beet) return  "Beet";
	if (a_crop == toc_BushFruit) return  "BushFruit"; 
	if (a_crop == toc_Cabbage) return  "Cabbage";
	if (a_crop == toc_CabbageSpring) return  "CabbageSpring";
	if (a_crop == toc_Carrots) return  "Carrots";
	if (a_crop == toc_CarrotsSpring) return  "CarrotsSpring";
	if (a_crop == toc_CatchCropPea) return  "CatchCropPea";
	if (a_crop == toc_CloverGrassGrazed1) return  "CloverGrassGrazed1";
	if (a_crop == toc_CloverGrassGrazed2) return  "CloverGrassGrazed2";
	if (a_crop == toc_CloverGrassGrazed3) return  "CloverGrassGrazed3";
	if (a_crop == toc_CorkOak) return  "CorkOak";
	if (a_crop == toc_DummyCropPestTesting) return  "DummyCropPestTesting";
	if (a_crop == toc_FieldPeas) return  "FieldPeas";
	if (a_crop == toc_FieldPeasSilage) return  "FieldPeasSilage";
	if (a_crop == toc_FieldPeasStrigling) return  "FieldPeasStrigling";
	if (a_crop == toc_FodderBeet) return  "FodderBeet";
	if (a_crop == toc_FodderGrass) return  "FodderGrass";
	if (a_crop == toc_FodderLucerne1) return  "FodderLucerne1";
	if (a_crop == toc_FodderLucerne2) return  "FodderLucerne2";
	if (a_crop == toc_GenericCatchCrop) return  "GenericCatchCrop";
	if (a_crop == toc_GrassGrazed1) return  "GrassGrazed1";
	if (a_crop == toc_GrassGrazed2) return  "GrassGrazed2";
	if (a_crop == toc_GrassGrazedExtensive) return  "GrassGrazedExtensive";
	if (a_crop == toc_GrassGrazedLast) return  "GrassGrazedLast";
	if (a_crop == toc_GrazingPigs) return  "GrazingPigs";
	if (a_crop == toc_Horticulture) return  "Horticulture";
	if (a_crop == toc_Maize) return  "Maize";
	if (a_crop == toc_MaizeSilage) return  "MaizeSilage";
	if (a_crop == toc_MaizeSpring) return  "MaizeSpring";
	if (a_crop == toc_MaizeStrigling) return  "MaizeStrigling";
	if (a_crop == toc_MixedVeg) return  "MixedVeg";
	if (a_crop == toc_OAsparagusEstablishedPlantation) return "OAsparagusEstablishedPlantation";
	if (a_crop == toc_Oats) return  "Oats";
	if (a_crop == toc_OBarleyPeaCloverGrass) return  "OBarleyPeaCloverGrass"; 
	if (a_crop == toc_OBeans) return  "OBeans";
	if (a_crop == toc_OBeans_Whole) return  "OBeans_Whole";	
	if (a_crop == toc_OBushFruit) return  "OBushFruit";
	if (a_crop == toc_OCabbage) return  "OCabbage";
	if (a_crop == toc_OCarrots) return  "OCarrots";
	if (a_crop == toc_OCloverGrassGrazed1) return  "OCloverGrassGrazed1";
	if (a_crop == toc_OCloverGrassGrazed2) return  "OCloverGrassGrazed2";
	if (a_crop == toc_OCloverGrassGrazed3) return  "OCloverGrassGrazed3";
	if (a_crop == toc_OCloverGrassSilage1) return  "OCloverGrassSilage1";
	if (a_crop == toc_OFieldPeas) return  "OFieldPeas";
	if (a_crop == toc_OFieldPeasSilage) return  "OFieldPeasSilage";
	if (a_crop == toc_OFirstYearDanger) return  "OFirstYearDanger";
	if (a_crop == toc_OFodderGrass) return  "OFodderGrass";
	if (a_crop == toc_OFodderBeet) return  "OFodderBeet";
	if (a_crop == toc_OGrazingPigs) return  "OGrazingPigs";
	if (a_crop == toc_OliveGrove) return "OliveGrove";
	if (a_crop == toc_OLentils) return "OLentils";
	if (a_crop == toc_OLupines) return "OLupines";
	if (a_crop == toc_OMaize) return  "OMaize";
	if (a_crop == toc_OMaizeSilage) return  "OMaizeSilage";
	if (a_crop == toc_OOats) return  "OOats";
	if (a_crop == toc_OPermanentGrassGrazed) return  "OPermanentGrassGrazed";
	if (a_crop == toc_OPermanentGrassLowYield) return  "PermanentGrassLowYield";
	if (a_crop == toc_OPotatoes) return  "OPotatoes";
	if (a_crop == toc_OrchardCrop) return  "OrchardCrop";
	if (a_crop == toc_OSBarleySilage) return  "OSBarleySilage";
	if (a_crop == toc_OSeedGrass1) return  "OSeedGrass1";
	if (a_crop == toc_OSeedGrass2) return  "OSeedGrass2";
	if (a_crop == toc_OSpringBarley) return  "OSpringBarley";
	if (a_crop == toc_OSpringBarleyExtensive) return  "OSpringBarleyExtensive";
	if (a_crop == toc_OSpringBarleyPigs) return  "OSpringBarleyPigs";
	if (a_crop == toc_OTriticale) return  "OTriticale";
	if (a_crop == toc_OWinterBarley) return  "OWinterBarley";
	if (a_crop == toc_OWinterBarleyExtensive) return  "OWinterBarleyExtensive";
	if (a_crop == toc_OWinterRape) return  "OWinterRape";
	if (a_crop == toc_OWinterRye) return  "OWinterRye";
	if (a_crop == toc_OWinterWheat) return  "OWinterWheat";
	if (a_crop == toc_OWinterWheatUndersown) return  "OWinterWheatUndersown";
	if (a_crop == toc_OWinterWheatUndersownExtensive) return  "OWinterWheatUndersownExtensive";
	if (a_crop == toc_PermanentGrassGrazed) return  "PermanentGrassGrazed";
	if (a_crop == toc_PermanentGrassLowYield) return  "PermanentGrassLowYield";
	if (a_crop == toc_PermanentGrassTussocky) return  "PermanentGrassTussocky";
	if (a_crop == toc_PermanentSetAside) return  "PermanentSetAside";
	if (a_crop == toc_PlantNursery) return  "PlantNursery";
	if (a_crop == toc_Potatoes) return  "Potatoes";
	if (a_crop == toc_PotatoesIndustry) return  "PotatoesIndustry";
	if (a_crop == toc_PotatoesSpring) return  "PotatoesSpring";
	if (a_crop == toc_Ryegrass) return  "Ryegrass";
	if (a_crop == toc_ORyegrass) return  "ORyegrass";
	if (a_crop == toc_SeedGrass1) return  "SeedGrass1";
	if (a_crop == toc_SeedGrass2) return  "SeedGrass2";
	if (a_crop == toc_SetAside) return  "SetAside";
	if (a_crop == toc_Sorghum) return  "Sorghum";
	if (a_crop == toc_SpringBarley) return  "SpringBarley";
	if (a_crop == toc_SpringBarleyCloverGrass) return  "SpringBarleyCloverGrass";
	if (a_crop == toc_SpringBarleyPeaCloverGrass) return  "SpringBarleyPeaCloverGrass";
	if (a_crop == toc_SpringBarleySeed) return  "SpringBarleySeed";
	if (a_crop == toc_SpringBarleySilage) return  "SpringBarleySilage";
	if (a_crop == toc_SpringRape) return  "SpringRape";
	if (a_crop == toc_SpringRye) return  "SpringRye";
	if (a_crop == toc_SpringWheat) return  "SpringWheat";
	if (a_crop == toc_SugarBeet) return  "SugarBeet";
	if (a_crop == toc_Triticale) return  "Triticale";
	if (a_crop == toc_Tulips) return  "Tulips";
	if (a_crop == toc_Turnip) return  "Turnip";
	if (a_crop == toc_Vineyards) return  "Vineyards";
	if (a_crop == toc_WinterBarley) return  "WinterBarley";
	if (a_crop == toc_WinterRape) return  "WinterRape";
	if (a_crop == toc_WinterRye) return  "WinterRye";
	if (a_crop == toc_WinterTriticale) return "WinterTriticale";
	if (a_crop == toc_WinterWheat) return  "WinterWheat";
	if (a_crop == toc_YellowLupin) return  "YellowLupin";
	if (a_crop == toc_YoungForestCrop) return  "YoungForestCrop";
	if (a_crop == toc_OOrchardCrop) return  "OOrchardCrop";
	if (a_crop == toc_OrchApple) return  "OrchApple";
	if (a_crop == toc_OrchPear) return  "OrchPear";
	if (a_crop == toc_OrchCherry) return  "OrchCherry";
	if (a_crop == toc_OrchOther) return  "OrchOther";
	if (a_crop == toc_OOrchApple) return  "OOrchApple";
	if (a_crop == toc_OOrchPear) return  "OOrchPear";
	if (a_crop == toc_OOrchCherry) return  "OOrchCherry";
	if (a_crop == toc_OOrchOther) return  "OOrchOther";
	if (a_crop == toc_OPotatoes) return  "OPotatoes";
	if (a_crop == toc_OPotatoesIndustry) return  "OPotatoesIndustry";
	if (a_crop == toc_OPotatoesSeed) return  "OPotatoesSeed";
	if (a_crop == toc_OSetAside) return  "OSetAside";
	if (a_crop == toc_OSetAside_Flower) return  "OSetAside_Flower";
	if (a_crop == toc_OSpringBarleyCloverGrass) return  "OSpringBarleyCloverGrass";
	if (a_crop == toc_OSpringBarleyPeaCloverGrass) return  "OSpringBarleyPeaCloverGrass";
	if (a_crop == toc_OSpringBarleyPigs) return  "OSpringBarleyPigs";
	if (a_crop == toc_OSpringBarleySilage) return  "OSpringBarleySilage";
	if (a_crop == toc_OSpringRape) return  "OSpringRape";
	if (a_crop == toc_OSpringWheat) return  "OSpringWheat";
	if (a_crop == toc_OStarchPotato) return  "OStarchPotato";
	if (a_crop == toc_OSugarBeet) return  "OSugarBeet";
	if (a_crop == toc_OMixedVeg) return  "OMixedVeg";
	if (a_crop == toc_OVegSeeds) return  "OVegSeeds";
	if (a_crop == toc_PotatoesSeed) return  "PotatoesSeed";
	if (a_crop == toc_StarchPotato) return  "StarchPotato";
	if (a_crop == toc_VegSeeds) return  "VegSeeds";
	if (a_crop == toc_FarmForest) return "FarmForest";
	if (a_crop == toc_OFarmForest) return "OFarmForest";
	if (a_crop == toc_OYoungForestCrop) return "OYoungForestCrop";
	if (a_crop == toc_Foobar) return  "Foobar";

	// No match so issue a warning and quit
	g_msg->Warn(WARN_FILE, "FarmManager::TranslateCropCodes():"" Unknown Crop toc ", a_crop);
	exit(142);
}

void FarmManager::DumpFarmAreas() {

	//create a text file with the farm areas
	ofstream ofile("FarmTotalAreas_almass.txt", ios::out);
	ofile << "Farm no" << '\t' << "Area" << endl;

	//print each farms no and then the numbers of its neighbours
	for (int i = 0; i < (int)m_farms.size(); i++) {
		ofile << i << '\t' << m_farms[i]->GetArea() << endl;
	}

	ofile.close();
}

ConventionalCattle::ConventionalCattle(FarmManager* a_manager) : Farm(a_manager) // 0
{
  m_farmtype = tof_ConventionalCattle;
  m_rotfilename = "ConventionalCattle - internal rotation";
  m_stockfarmer = true;
  // Adjust as needed.
  m_rotation.resize( 9 );
  m_rotation[ 0 ] = tov_SpringBarleyCloverGrass;
  m_rotation[ 1 ] = tov_CloverGrassGrazed1;
  m_rotation[ 2 ] = tov_CloverGrassGrazed2;
  m_rotation[ 3 ] = tov_WinterWheat;
  m_rotation[ 4 ] = tov_SpringBarley;
  m_rotation[ 5 ] = tov_SpringBarleyCloverGrass;
  m_rotation[ 6 ] = tov_CloverGrassGrazed1;
  m_rotation[ 7 ] = tov_Maize; // was FodderBeet until 23/12/03
  m_rotation[ 8 ] = tov_SpringBarley;
}


ConventionalPig::ConventionalPig(FarmManager* a_manager) : Farm(a_manager) // 1
{
  m_farmtype = tof_ConventionalPig;
  m_rotfilename = "ConventionalPig - internal rotation";
  m_stockfarmer = true;

  // Adjust as needed.
  m_rotation.resize( 9 );
  m_rotation[ 0 ] = tov_WinterRape;
  m_rotation[ 1 ] = tov_WinterWheat;
  m_rotation[ 2 ] = tov_SpringBarley;
  m_rotation[ 3 ] = tov_SpringBarley;
  m_rotation[ 4 ] = tov_SetAside;
  m_rotation[ 5 ] = tov_FieldPeas;
  m_rotation[ 6 ] = tov_WinterWheat;
  m_rotation[ 7 ] = tov_WinterRye;
  m_rotation[ 8 ] = tov_WinterBarley;
}


ConventionalPlant::ConventionalPlant(FarmManager* a_manager) : Farm(a_manager) // 2
{
  m_farmtype = tof_ConventionalPlant;
  m_rotfilename = "ConventionalPlant - internal rotation";
  m_stockfarmer = false;

  // Adjust as needed.
  m_rotation.resize( 9 );
  m_rotation[ 0 ] = tov_WinterRape;
  m_rotation[ 1 ] = tov_WinterWheat;
  m_rotation[ 2 ] = tov_SpringBarley;
  m_rotation[ 3 ] = tov_SpringBarley;
  m_rotation[ 4 ] = tov_SetAside;
  m_rotation[ 5 ] = tov_FieldPeas;
  m_rotation[ 6 ] = tov_WinterWheat;
  m_rotation[ 7 ] = tov_WinterRye;
  m_rotation[ 8 ] = tov_WinterBarley;
}


OrganicCattle::OrganicCattle(FarmManager* a_manager) : Farm(a_manager) // 3
{
  m_farmtype = tof_OrganicCattle;
  m_rotfilename = "OrganicCattle.rot";
  m_stockfarmer = true;
  ReadRotation(m_rotfilename);
  }

OrganicPig::OrganicPig(FarmManager* a_manager) : Farm(a_manager) // 4
{
  m_farmtype = tof_OrganicPig;
  m_rotfilename = "OrganicPig.rot";
  m_stockfarmer = true;
  ReadRotation(m_rotfilename);
}

OrganicPlant::OrganicPlant(FarmManager* a_manager) : Farm(a_manager) // 5
{
  m_farmtype = tof_OrganicPlant;
  m_rotfilename = "OrganicPlant.rot";
  m_stockfarmer = false;
  ReadRotation(m_rotfilename);
}

PesticideTrialControl::PesticideTrialControl(FarmManager* a_manager) : Farm(a_manager) // 6
{
  m_farmtype = tof_PTrialControl;
  m_rotfilename = "PTrialControl - internal rotation";
  m_stockfarmer = false;
  m_rotation.resize( 1 );
  m_rotation[ 0 ] = tov_WWheatPControl;
}

PesticideTrialToxicControl::PesticideTrialToxicControl(FarmManager* a_manager) : Farm(a_manager) // 7
{
  m_farmtype = tof_PTrialToxicControl;
  m_rotfilename = "PTrialToxicControl - internal rotation";
  m_stockfarmer = false;
  m_rotation.resize( 1 );
  m_rotation[ 0 ] = tov_WWheatPToxicControl;
}

PesticideTrialTreatment::PesticideTrialTreatment(FarmManager* a_manager) : Farm(a_manager) // 8
{
  m_farmtype = tof_PTrialTreatment;
  m_rotfilename = "PesticideTrialTreatment.rot";
  m_stockfarmer = false;
  // This farm type reads its rotation from a special file PesticideTrialTreatment.rot
  ReadRotation(m_rotfilename);
}

ConvMarginalJord::ConvMarginalJord(FarmManager* a_manager ) : Farm(a_manager) // 9
{
  m_farmtype = tof_ConvMarginalJord;
  m_rotfilename = "ConvMarginalJord - internal rotation";
  m_stockfarmer = true;

  m_rotation.resize( 6 );
  m_rotation[ 0 ] = tov_SpringBarleyCloverGrass;
  m_rotation[ 1 ] = tov_CloverGrassGrazed1;
  m_rotation[ 2 ] = tov_CloverGrassGrazed2;
  m_rotation[ 3 ] = tov_SpringBarleyCloverGrass;
  m_rotation[ 4 ] = tov_CloverGrassGrazed1;
  m_rotation[ 5 ] = tov_FodderBeet;
}

AgroChemIndustryCerealFarm1::AgroChemIndustryCerealFarm1(FarmManager* a_manager ) : Farm(a_manager) // 10
{
  m_farmtype = tof_AgroChemIndustryCerealFarm1;
  m_rotfilename = "AgroChemIndustryCerealFarm1 - internal rotation";
  m_stockfarmer = true;
  // Adjust as needed.
  m_rotation.resize( 9 );
  m_rotation[ 0 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 1 ] = tov_CloverGrassGrazed1;
  m_rotation[ 2 ] = tov_CloverGrassGrazed2;
  m_rotation[ 3 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 4 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 5 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 6 ] = tov_FodderBeet;
  m_rotation[ 7 ] = tov_CloverGrassGrazed1;
  m_rotation[ 8 ] = tov_AgroChemIndustryCereal;
}

AgroChemIndustryCerealFarm2::AgroChemIndustryCerealFarm2(FarmManager* a_manager ) : Farm(a_manager) // 11
{
  m_farmtype = tof_AgroChemIndustryCerealFarm2;
  m_rotfilename = "AgroChemIndustryCerealFarm2 - internal rotation";
  m_stockfarmer = true;

  // Adjust as needed.
  m_rotation.resize( 3 );
  m_rotation[ 0 ] = tov_WinterRape;
  m_rotation[ 1 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 2 ] = tov_AgroChemIndustryCereal;
  /*  m_rotation[ 3] = tov_AgroChemIndustryCereal; m_rotation[ 4] = tov_SetAside; m_rotation[ 5] = tov_FieldPeas;
  m_rotation[ 6] = tov_AgroChemIndustryCereal; m_rotation[ 7] = tov_WinterRye; m_rotation[ 8] = tov_AgroChemIndustryCereal; */
}



AgroChemIndustryCerealFarm3::AgroChemIndustryCerealFarm3(FarmManager* a_manager ) : Farm(a_manager) // 12
{
  m_farmtype = tof_AgroChemIndustryCerealFarm3;
  m_rotfilename = "AgroChemIndustryCerealFarm3 - internal rotation";
  m_stockfarmer = false;

  // Adjust as needed.
  m_rotation.resize( 9 );
  m_rotation[ 0 ] = tov_SetAside;
  m_rotation[ 1 ] = tov_FieldPeas;
  m_rotation[ 2 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 3 ] = tov_WinterRye;
  m_rotation[ 4 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 5 ] = tov_WinterRape;
  m_rotation[ 6 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 7 ] = tov_AgroChemIndustryCereal;
  m_rotation[ 8 ] = tov_AgroChemIndustryCereal;
}

NoPesticideBaseFarm::NoPesticideBaseFarm(FarmManager* a_manager ) : Farm(a_manager) // 13
{
  m_farmtype = tof_NoPesticideBase;
  m_rotfilename = "NoPesticideBase - internal rotation";
  m_stockfarmer = false;

  // Adjust as needed.
  m_rotation.resize( 36 );
  m_rotation[ 0 ] = tov_SpringBarleyCloverGrass;
  m_rotation[ 1 ] = tov_CloverGrassGrazed1;
  m_rotation[ 2 ] = tov_CloverGrassGrazed2;
  m_rotation[ 3 ] = tov_WinterWheat;
  m_rotation[ 4 ] = tov_SpringBarley;
  m_rotation[ 5 ] = tov_SpringBarleyCloverGrass;
  m_rotation[ 6 ] = tov_CloverGrassGrazed1;
  m_rotation[ 7 ] = tov_FodderBeet;
  m_rotation[ 8 ] = tov_SpringBarley;
  m_rotation[ 9 ] = tov_WinterRape;
  m_rotation[ 10 ] = tov_WinterWheat;
  m_rotation[ 11 ] = tov_SpringBarley;
  m_rotation[ 12 ] = tov_SpringBarley;
  m_rotation[ 13 ] = tov_SetAside;
  m_rotation[ 14 ] = tov_FieldPeas;
  m_rotation[ 15 ] = tov_WinterWheat;
  m_rotation[ 16 ] = tov_WinterRye;
  m_rotation[ 17 ] = tov_WinterBarley;
  m_rotation[ 18 ] = tov_WinterRape;
  m_rotation[ 19 ] = tov_WinterWheat;
  m_rotation[ 20 ] = tov_SpringBarley;
  m_rotation[ 21 ] = tov_SpringBarley;
  m_rotation[ 22 ] = tov_SetAside;
  m_rotation[ 23 ] = tov_FieldPeas;
  m_rotation[ 24 ] = tov_WinterWheat;
  m_rotation[ 25 ] = tov_WinterRye;
  m_rotation[ 26 ] = tov_WinterBarley;
  m_rotation[ 27 ] = tov_WinterRape;
  m_rotation[ 28 ] = tov_WinterWheat;
  m_rotation[ 29 ] = tov_SpringBarley;
  m_rotation[ 30 ] = tov_SpringBarley;
  m_rotation[ 31 ] = tov_SetAside;
  m_rotation[ 32 ] = tov_FieldPeas;
  m_rotation[ 33 ] = tov_WinterWheat;
  m_rotation[ 34 ] = tov_WinterRye;
  m_rotation[ 35 ] = tov_WinterBarley;
}

NoPesticideNoPFarm::NoPesticideNoPFarm(FarmManager* a_manager ) : Farm(a_manager) // 14
{
  m_farmtype = tof_NoPesticideNoP;
  m_rotfilename = "NoPesticideNoP - internal rotation";
  m_stockfarmer = false;

  // Adjust as needed.
  m_rotation.resize( 17 );
  m_rotation[ 0 ] = tov_OBarleyPeaCloverGrass;
  m_rotation[ 1 ] = tov_OCloverGrassGrazed1;
  m_rotation[ 2 ] = tov_OCloverGrassGrazed2;
  m_rotation[ 3 ] = tov_OWinterRape;
  m_rotation[ 4 ] = tov_OFieldPeas;
  m_rotation[ 5 ] = tov_OBarleyPeaCloverGrass;
  m_rotation[ 6 ] = tov_OCloverGrassGrazed1;
  m_rotation[ 7 ] = tov_OWinterWheatUndersown;
  m_rotation[ 8 ] = tov_OCloverGrassGrazed1;
  m_rotation[ 9 ] = tov_OCloverGrassGrazed2;
  m_rotation[ 10 ] = tov_OFieldPeas;
  m_rotation[ 11 ] = tov_OBarleyPeaCloverGrass;
  m_rotation[ 12 ] = tov_OCloverGrassGrazed1;
  m_rotation[ 13 ] = tov_SetAside;
  m_rotation[ 14 ] = tov_OFieldPeas;
  m_rotation[ 15 ] = tov_OWinterRye;
  m_rotation[ 16 ] = tov_OFieldPeas;
}


UserDefinedFarm1::UserDefinedFarm1(FarmManager* a_manager ) : Farm(a_manager) // 14 + 1 = 15
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm1;
  m_rotfilename = "UserDefinedFarm1.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm1.rot
  ReadRotation(m_rotfilename);
 }

UserDefinedFarm2::UserDefinedFarm2(FarmManager* a_manager ) : Farm(a_manager) // 16
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm2;
  m_rotfilename = "UserDefinedFarm2.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm2.rot
  ReadRotation(m_rotfilename);
 }

UserDefinedFarm3::UserDefinedFarm3(FarmManager* a_manager ) : Farm(a_manager) // 17
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm3;
  m_rotfilename = "UserDefinedFarm3.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm3.rot
  ReadRotation(m_rotfilename);
}

UserDefinedFarm4::UserDefinedFarm4(FarmManager* a_manager ) : Farm(a_manager) // 18
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm4;
  m_rotfilename = "UserDefinedFarm4.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm4.rot
  ReadRotation(m_rotfilename);
 }

UserDefinedFarm5::UserDefinedFarm5(FarmManager* a_manager ) : Farm(a_manager) // 19
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm5;
  m_rotfilename = "UserDefinedFarm5.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm5.rot
  ReadRotation(m_rotfilename);
}

UserDefinedFarm6::UserDefinedFarm6(FarmManager* a_manager ) : Farm(a_manager) // 14 + 6 = 20
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm6;
  m_rotfilename = "UserDefinedFarm6.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm6.rot
  ReadRotation(m_rotfilename);
}

UserDefinedFarm7::UserDefinedFarm7(FarmManager* a_manager ) : Farm(a_manager) // 21
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm7;
  m_rotfilename = "UserDefinedFarm7.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm7.rot
  ReadRotation(m_rotfilename);
}

UserDefinedFarm8::UserDefinedFarm8(FarmManager* a_manager ) : Farm(a_manager) // 22
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm8;
  m_rotfilename = "UserDefinedFarm8.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm8.rot
  ReadRotation(m_rotfilename);
}

UserDefinedFarm9::UserDefinedFarm9(FarmManager* a_manager ) : Farm(a_manager) // 23
{
  m_stockfarmer = false;
  m_farmtype = tof_UserDefinedFarm9;
  m_rotfilename = "UserDefinedFarm9.rot";
  // This farm type reads its rotation from a special file UserDefinedFarm8.rot
  ReadRotation(m_rotfilename);
}

UserDefinedFarm10::UserDefinedFarm10(FarmManager* a_manager ) : Farm(a_manager) // 24
{
  m_stockfarmer = false;
  m_farmtype = tof_UserDefinedFarm10;
  m_rotfilename = "UserDefinedFarm10.rot";
  ReadRotation(m_rotfilename);

}

UserDefinedFarm11::UserDefinedFarm11(FarmManager* a_manager ) : Farm(a_manager) // 25
{
  m_stockfarmer = false;
  m_farmtype = tof_UserDefinedFarm11;
  m_rotfilename = "UserDefinedFarm11.rot";
  ReadRotation(m_rotfilename);
}

UserDefinedFarm12::UserDefinedFarm12(FarmManager* a_manager) : Farm(a_manager) // 26
{
	m_stockfarmer = false;
	m_farmtype = tof_UserDefinedFarm12;
	m_rotfilename = "UserDefinedFarm12.rot";
	ReadRotation(m_rotfilename);
}

UserDefinedFarm13::UserDefinedFarm13(FarmManager* a_manager ) : Farm(a_manager) // 27
{
  m_stockfarmer = false;
  m_farmtype = tof_UserDefinedFarm13;
  m_rotfilename = "UserDefinedFarm13.rot";
  ReadRotation(m_rotfilename);
}

UserDefinedFarm14::UserDefinedFarm14( FarmManager* a_manager ) : Farm(a_manager) // 28
{
  m_stockfarmer = true;
  m_farmtype = tof_UserDefinedFarm14;
  m_rotfilename = "UserDefinedFarm14.rot";
  ReadRotation(m_rotfilename);
  
}

UserDefinedFarm15::UserDefinedFarm15(FarmManager* a_manager) : Farm(a_manager) // 29
{
  m_stockfarmer = false;
  m_farmtype = tof_UserDefinedFarm15;
  m_rotfilename = "UserDefinedFarm15.rot";
  ReadRotation(m_rotfilename);
}

UserDefinedFarm16::UserDefinedFarm16(FarmManager* a_manager ) : Farm(a_manager) // 30
{
  m_stockfarmer = false;
  m_farmtype = tof_UserDefinedFarm16;
  m_rotfilename = "UserDefinedFarm16.rot";
  ReadRotation(m_rotfilename);
}


UserDefinedFarm17::UserDefinedFarm17(FarmManager* a_manager ) : Farm(a_manager) // 31
{
  m_stockfarmer = false;
  m_farmtype = tof_UserDefinedFarm17;
  m_rotfilename = "UserDefinedFarm17.rot";
  ReadRotation(m_rotfilename);
}

UserDefinedFarm::UserDefinedFarm( TTypesOfFarm farm_type, const char* fname, FarmManager* a_manager ) : Farm(a_manager) // Base class for the real ones
{
	/**
    User defined farms derived from this class have greater functionality and greater flexibility than those defined from
    Farm. There are three areas of added functionality:\n
    1) The ability to define whether the farm is arable or stock\n
	2) The ability to define farm intensity \n
    3) The capability of defining permanent crops (e.g. permanent grass or silage maize).
    This means that whatever field is assigned this crop in the first year will retain it subsequently. Further functionality
    here is that the ideal crop area can also be specified. If so then the farmer will identify the field closest to that
    area and assign the crop to that.\n
    */

	m_farmtype = farm_type;


    if ((std::string)cfgRotationFileDir.value()=="."){
        m_rotfilename = fname;
    }
    else{
        m_rotfilename = (std::string)cfgRotationFileDir.value()+ (std::string)"/"+(std::string)fname;


    }
    ifstream ifile;
    ifile.open(m_rotfilename,ios::in);
	if (!ifile.is_open()) {
		g_msg->Warn( WARN_FILE, " Unable to open file ", m_rotfilename );
		exit( 1 );
    }
	int input;
	double intensity;
	// First entry - arable/stock
	// Second entry - farm intensity. Zero = default, 1 extensive, 2 very extensive
	ifile >> input >> intensity;
	if (input==1) m_stockfarmer = true; else m_stockfarmer = false;
	SetIntensity(intensity);
	// Thrid entry - no permanent crops
	int permcrops;
	std::string cropref ="";
	ifile >> permcrops;
	// The problem here is that at the time when we reach this code there are no fields attached to the farm - so we need
	// to store this information - hence the vector of the vector of PermCropData
	PermCropData pcd;
	for (int i=0; i<permcrops; i++) {
		int pct;
		ifile >> cropref;
		TTypesOfVegetation tov = g_farmmanager->TranslateVegCodes( cropref );
		ifile >> pct; // 0-100%
		pcd.Pct=pct;
		pcd.Tov=tov;
		m_PermCrops.push_back(pcd);
	}
	// 4th entry
  int nocrops;
  ifile >> nocrops;
  m_rotation.resize( nocrops );
  for ( int i = 0; i < nocrops; i++ ) {
    ifile >> cropref;
    TTypesOfVegetation tov = g_farmmanager->TranslateVegCodes( cropref );
    m_rotation[ i ] = tov;
  }
  ifile.close();
}

/** \brief Used to assign a permanent crop to an otherwise rotational field polygon */
void Farm::AssignPermanentCrop(TTypesOfVegetation a_tov, int a_pct) {
	// Assumes that m_fields has all the fields already in it, and those that are occupied by permanent tole types
	// are denoted by -2 or -4 in rotindex, -1 is unassigned - can't use enum because positive values have other meanings
	
	// Here we can save some time if we have to allocate all the fields to one permanent crop
	if (a_pct == 100)
	{
		for (auto cfi = m_fields.begin(); cfi != m_fields.end(); ++cfi)
		{
			// Set the field as non-rotating
			(*cfi)->SetRotIndex(-4);
			// Assign the crop, and weeds are assumed to be undefined
			(*cfi)->SetVegType(a_tov, tov_Undefined);
		}
		return;
	}

	tpct a_tpct;
	// create a copy of the field vector and call it fields_cpy
	vector<LE*> fields_cpy;
	fields_cpy.resize(m_fields.size());
	copy(m_fields.begin(), m_fields.end(), fields_cpy.begin());
	vector<tpct> pcts;
	double area = 0.0;

	
	// First get the areas of all fields and then convert these to percentages
	int sz = (int) fields_cpy.size();
	// loop through the fields vector and remove any fields already assigned to permanent crops
	for (int i = sz - 1; i >= 0; i--) {
		if (fields_cpy[i]->GetRotIndex() < -1) {
			fields_cpy.erase(fields_cpy.begin() + i);
		}
	}
	// sum up the area
	for (auto cfi = fields_cpy.begin(); cfi != fields_cpy.end(); ++cfi) area += (*cfi)->GetArea();
	
	// Here we can take action if we only have a single field 
	int fnos = (int) fields_cpy.size();
	if (fnos<1) return; // No fields to allocate, jump out
	else if (fnos==1) {
		// Use the pct as the chance that our single field is assigned
		if (g_random_fnc(100)>=a_pct) return;
		else {
			// Add field by default
			fields_cpy[0]->SetRotIndex(-4);
			// Assign the crop
			fields_cpy[0]->SetVegType(a_tov, tov_Undefined);
			return;
		}
	}
	// By here we have more than one field to potentially allocate to
	for (int i=0; i<fnos; i++) {
		a_tpct.pct = (int) floor(0.5+((fields_cpy[i]->GetArea()/area)*100));
		a_tpct.index = i;
		pcts.push_back(a_tpct);
	}
	// We need to look for combinations of fields that are close to our target.
	// First we can sort the array and ignore any that are greater than pct
    sort (pcts.begin(), pcts.end(), CompPcts); // results in ordering on increasing pct
	// now remove all those with pct>target+10% to save loads of time in the inverse integer partition
	int index=-1;
	int ind=0;
	int apct=a_pct+10; // Lets assume we have a tolerance of 10%
	while ((index==-1) && (ind< (int)pcts.size())) {
		if (pcts[ind++].pct>apct) index=ind;
	}
	if (index!=-1) pcts.erase(pcts.begin()+index,pcts.end());
	// Now find the best combination of fields to get close to our target & set them
	int bits = InvIntPartition(&pcts,a_pct);
	int mask=1;
	ind = 0;
	int used=0;
	//double pctused = 0.0;
	double check = 0.0;
	for (int h=0; h< (int)pcts.size(); h++)	check+=fields_cpy[pcts[h].index]->GetArea();
	while (bits>0) {
		if ((bits & mask) > 0) {
			// Set the field as non-rotating
			fields_cpy[pcts[ind].index]->SetRotIndex(-4);
			// Assign the crop
			fields_cpy[pcts[ind].index]->SetVegType(a_tov, tov_Undefined);
			used += (int) fields_cpy[pcts[ind].index]->GetArea();
			//pctused = used/area;
			bits -= mask;
		}
		mask = mask << 1;
		ind++;
	}
}

/** \brief Finds all possible sums of the integers in the items array */
int Farm::InvIntPartition(vector<tpct>* items, int target){
	//Figure out how many bitmasks we need...
	//4 bits have a maximum value of 15, so we need 15 masks.
	//Calculated as:
	//    (2 ^ ItemCount) - 1
	long long int sz = (long long int) items->size();
	if (sz>63) {
		// This is a warning that we did no consider all combinations of fields but 2^63 combinations should be enough :)
		g_msg->WarnAddInfo(WARN_MSG,"Too many potential fields in UserDefinedFarm::InvIntPartition: ",double(sz));
		g_msg->WarnAddInfo(WARN_MSG,"Farm Number: ",GetFarmNumber());
		sz = 63;
	}
	long int calcs = (1 << sz);
 	//Spit out the corresponding calculation for each bitmask
	int sum;
	int found = 0;
	int diff = 100;
	for (long int i=1; i<calcs; i++) {
		//Get the items from our array that correspond to
		//the on bits in our mask
		sum = 0;
		int mask = 1;
		for (int bit=0; bit<sz; bit++) {
			if ((i & mask) > 0) {
				sum+=(*items)[bit].pct;
			}
			mask = mask << 1;
		}

		if (abs(sum-target) < diff ){
			found = i;
			diff = abs(sum-target);
			if (diff<1) break; //added 01.11.12 to prevent from checking all the combinations when the right one is found
		}
	}
	return found;
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

FarmManager::FarmManager(Landscape *landscape)
{
	m_landscape = landscape;
	g_farmmanager = this;
	if ( l_map_read_farmfile.value() ) {
		CreateFarms( l_map_farmref_file.value() );
	}
#ifdef __CALCULATE_MANAGEMENT_NORMS
	for (int i=0; i<toc_Foobar; i++) 
		for (int j=0; j<=fmc_Foobar; j++) m_managementnormstats[i][j] = 0;
#endif

}


FarmManager::~FarmManager()
{
#ifdef __CALCULATE_MANAGEMENT_NORMS
	OutputManagementNormStats();
#endif
	delete[] m_farmmapping_lookup; // The variable newed in CreateFarms()
	for ( unsigned int i = 0; i < m_farms.size(); i++ )
		delete m_farms[ i ];
	for (unsigned int i = 0; i < m_cropprogs.size(); i++)
		delete m_cropprogs[i];
}

void FarmManager::FarmManagement()
{
	/** The crop management does not need to be recursively called so is looped through for each farm first */
	for (unsigned int i = 0; i < m_farms.size(); i++) {
		m_farms[i]->FarmActions();
	}
}

void FarmManager::InitiateManagement()
{
	cout << "Farms initiating management and centroid calculation" << endl;
	for ( unsigned int i = 0; i < m_farms.size(); i++ )
	{
		m_farms[i]->InitiateManagement();
		m_farms[i]->Centroids();
	}
}

void FarmManager::AddField(int a_OwnerIndex, LE* a_newland, int a_Owner)
{
	m_farms[ a_OwnerIndex ]->AddField( a_newland );
	a_newland->SetOwner( m_farms[ a_OwnerIndex ], a_Owner, a_OwnerIndex );
}

void FarmManager::RemoveField(int a_OwnerIndex, LE* a_field)
{
	m_farms[ a_OwnerIndex ]->RemoveField( a_field );
}

int FarmManager::ConnectFarm( int a_owner )
{
  for ( unsigned int i = 0; i < m_farms.size(); i++ )
  {
    if ( a_owner == m_farms[ i ]->GetFarmNumber() )
	{
      // Found it. Set mapping and return.
      return i;
    }
  }
  // We didn't find the owner in the list of farms,
  // pregenerated in CreateFarms() above. Something
  // is not correct here, so raise an appropriate
  // error and exit.
  char error_num[ 20 ];
  sprintf( error_num, "%d", a_owner );
  g_msg->Warn( WARN_FILE, "FarmManager::ConnectFarm(): Unknown farm number"" referenced in polygon file:", error_num );
  exit( 1 );
}

void FarmManager::CreateFarms( const char * a_farmfile )
{
	int No, FType, NoFarms;
	string firstline = ""; // added for parsing between the two formats, could use "rubbish" below but better for readability
	string rubbish = ""; //put here the names of the parameters;
	float latitude, longitude;
	std::string country_code;

	ifstream ifile(a_farmfile);
	if (!ifile.is_open()) {
		g_msg->Warn( WARN_FILE, "Landscape::CreateFarms(): Unable to open file", a_farmfile );
		std::exit(1);
	}

	ifile >> NoFarms;
	m_farms.resize(NoFarms);

	m_farmmapping_lookup = new int[NoFarms * 2];

	for (int i = 0; i < NoFarms; i++)
	{
		// File format:
		//
		// Two colunms of numbers 0..number of farms, 0-number of farmtypes-1
		// the second column determines what type of farm we have
		ifile >> No >> FType;
		m_farmmapping_lookup[i * 2] = No;
		m_farmmapping_lookup[i * 2 + 1] = FType;
	}
	ifile.close();

	std::unordered_set<int> crop_set;

	if (g_farm_fixed_crop_enable.value()) {
		int fv = g_farm_fixed_crop_type.value();
		TTypesOfVegetation fixed_crop_veg;
		fixed_crop_veg = g_letype->TranslateVegTypes(fv);
		crop_set.insert((int)fixed_crop_veg);
	}

	// Add the possible permanent crops without checking they really are on the current landscape
	// more difficult to check if really presents 
	// as it is a combination of tole number and polygon belonging to a farm 
	// so have to iterate over all polygons. This is faster for now.

	for (int i = 0; i < NoFarms; i++)
	{
		/*
		//If we are testing crop management, then ignore farm type from
		// the file and set to fixed one instead.
		if ( g_farm_test_crop.value() ) {
		FType = g_farm_test_crop_farmtype.value();
		}
		*/
		// If we are running in fixed, sync'ed rotation mode, set all farms to
		// be of the requested type.
		if (g_farm_fixed_rotation_enable.value()) {
			FType = g_farm_fixed_rotation_farmtype.value();
		}

		switch (m_farmmapping_lookup[i * 2 + 1]) // FType
		{
		case 0:
			m_farms[i] = new ConventionalCattle(this);
			break;
		case 1:
			m_farms[i] = new ConventionalPig(this);
			break;
		case 2:
			m_farms[i] = new ConventionalPlant(this);
			break;
		case 3:
			m_farms[i] = new OrganicCattle(this);
			break;
		case 4:
			m_farms[i] = new OrganicPig(this);
			break;
		case 5:
			m_farms[i] = new OrganicPlant(this);
			break;
		case 6:
			m_farms[i] = new PesticideTrialControl(this);
			break;
		case 7:
			m_farms[i] = new PesticideTrialToxicControl(this);
			break;
		case 8:
			m_farms[i] = new PesticideTrialTreatment(this);
			break;
		case 9:
			m_farms[i] = new ConvMarginalJord(this);
			break;
		case 10:
			m_farms[i] = new AgroChemIndustryCerealFarm1(this);
			break;
		case 11:
			m_farms[i] = new AgroChemIndustryCerealFarm2(this);
			break;
		case 12:
			m_farms[i] = new AgroChemIndustryCerealFarm3(this);
			break;
		case 13:
			m_farms[i] = new NoPesticideBaseFarm(this);
			break;
		case 14:
			m_farms[i] = new NoPesticideNoPFarm(this);
			break;
		case 15:
			m_farms[i] = new UserDefinedFarm1(this);
			break;
		case 16:
			m_farms[i] = new UserDefinedFarm2(this);
			break;
		case 17:
			m_farms[i] = new UserDefinedFarm3(this);
			break;
		case 18:
			m_farms[i] = new UserDefinedFarm4(this);
			break;
		case 19:
			m_farms[i] = new UserDefinedFarm5(this);
			break;
		case 20:
			m_farms[i] = new UserDefinedFarm6(this);
			break;
		case 21:
			m_farms[i] = new UserDefinedFarm7(this);
			break;
		case 22:
			m_farms[i] = new UserDefinedFarm8(this);
			break;
		case 23:
			m_farms[i] = new UserDefinedFarm9(this);
			break;
		case 24:
			m_farms[i] = new UserDefinedFarm10(this);
			break;
		case 25:
			m_farms[i] = new UserDefinedFarm11(this);
			break;
		case 26:
			m_farms[i] = new UserDefinedFarm12(this);
			break;
		case 27:
			m_farms[i] = new UserDefinedFarm13(this);
			break;
		case 28:
			m_farms[i] = new UserDefinedFarm14(this);
			break;
		case 29:
			m_farms[i] = new UserDefinedFarm15(this);
			break;
		case 30:
			m_farms[i] = new UserDefinedFarm16(this);
			break;
		case 31:
			m_farms[i] = new UserDefinedFarm17(this);
			break;
			// NB the user defing farms below require an extra parameter in the rotation file denoting the intensity (0 or 1 = high low)
		case 32:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm18, "UserDefinedFarm18.rot", this);
			break;
		case 33:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm19, "UserDefinedFarm19.rot", this);
			break;
		case 34:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm20, "UserDefinedFarm20.rot", this);
			break;
		case 35:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm21, "UserDefinedFarm21.rot", this);
			break;
		case 36:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm22, "UserDefinedFarm22.rot", this);
			break;
		case 37:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm23, "UserDefinedFarm23.rot", this);
			break;
		case 38:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm24, "UserDefinedFarm24.rot", this);
			break;
		case 39:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm25, "UserDefinedFarm25.rot", this);
			break;
		case 40:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm26, "UserDefinedFarm26.rot", this);
			break;
		case 41:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm27, "UserDefinedFarm27.rot", this);
			break;
		case 42:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm28, "UserDefinedFarm28.rot", this);
			break;
		case 43:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm29, "UserDefinedFarm29.rot", this);
			break;
		case 44:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm30, "UserDefinedFarm30.rot", this);
			break;
		case 45:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm31, "UserDefinedFarm31.rot", this);
			break;
		case 46:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm32, "UserDefinedFarm32.rot", this);
			break;
		case 47:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm33, "UserDefinedFarm33.rot", this);
			break;
		case 48:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm34, "UserDefinedFarm34.rot", this);
			break;
		case 49:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm35, "UserDefinedFarm35.rot", this);
			break;
		case 50:
			m_farms[i] = new UserDefinedFarm(tof_UserDefinedFarm36, "UserDefinedFarm36.rot", this);
			break;
		default:
			g_msg->Warn(WARN_FILE, "FarmManager::CreateFarms(): Unknown farm type reference number: ", m_farmmapping_lookup[i * 2 + 1]);
			exit(1);
		}
		m_farms[i]->SetFarmNumber(i); // We use 'i' here because we renumber the farms internally. If the file did not come in this way then a dump file is created - tested in ReadPolys2
		/* Add crop/tov to the set (making for unique entries) */
		for (int j = 0; j < m_farms[i]->GetNoCrops(); j++) {
			crop_set.insert((int)m_farms[i]->GetCrop(j));
		}

	}

	// Load the selected cropprogs (for the entire lanscape) into the FarmManager
	for (std::unordered_set<int>::iterator itr = crop_set.begin(); itr != crop_set.end(); ++itr)
	{
		switch (*itr)
		{
		case tov_DKSpringBarley:
			m_cropprogs.push_back(new	DK_SpringBarley(tov_DKSpringBarley, toc_SpringBarley, g_landscape_ptr));
			break;
		case tov_DKWinterWheat:
			m_cropprogs.push_back(new	DK_WinterWheat(tov_DKWinterWheat, toc_WinterWheat, g_landscape_ptr));
			break;
		default:
			g_msg->Warn(WARN_FILE, "Landscape::CreateFarms(): Missing Cropprog definition for TOV code", to_string(*itr));
			exit(1);
		}
	}

	if (GetFarmNoLookup(NoFarms - 1) != NoFarms - 1)
		m_renumbered = false;
	else
		m_renumbered = true;
}

void FarmManager::DumpFarmrefs(const char* a_filename)
{
	ofstream opf(a_filename, ios::out);
	if (!opf.is_open())
	{
		g_msg->Warn(WARN_FILE, "Landscape::CreateFarms(): Unable to open file", a_filename);
		exit(1);
	}
	int NoFarms = (int) m_farms.size();
	opf << NoFarms << endl;
	for (int i = 0; i < NoFarms; i++)
	{
		opf << i << '\t' << m_farmmapping_lookup[i * 2 + 1] << endl;
	}
	opf.close();
}

double FarmManager::GetSpilledGrain()
{
	/*
	* This data is grain distributions based on 2013 & 2014 data from cereal fields in Jutland counted in September.
	* Counts made in squares of 0.0196 m2 and scaled up, hence the decimal numbers.
	* The numbers represent number of grains.
	*/
	double graindist2013[26] = {
		29.59, 172.68, 60.59, 39.68, 51.02, 81.63, 268.71, 134.84, 57.40, 30.61, 204.08, 683.67, 108.04,
		141.29, 505.10, 444.61, 293.37, 355.18, 386.90, 381.83, 372.45, 377.55, 320.70, 392.46, 392.86, 435.17
	};
	double graindist2014[28] = {
		109.33, 382.65, 94.19, 765.31, 29.15, 70.15, 1096.94, 436.51, 309.21, 286.28, 480.44, 249.73, 784.10,
		688.78, 2035.45, 920.80, 341.61, 12.24, 113.38, 80.17, 178.57, 480.44, 0.00, 2.83, 1447.12, 1846.94, 1017.86,
		477.74
	};
	int grain_dist_value = cfg_grain_distribution.value();
	if (grain_dist_value == 0)  // The default - random pick between the two years
	{
		if (m_SpilledGrain) return graindist2013[(int)(g_rand_uni_fnc() * 26)];
		else return graindist2014[(int)(g_rand_uni_fnc() * 28)];
	}
	if (grain_dist_value == 1) {
		return graindist2013[(int)(g_rand_uni_fnc() * 26)];
	}
	else
	{
		return graindist2014[(int)(g_rand_uni_fnc() * 28)];
	}
}

double FarmManager::GetSpilledMaize()
{
	/*
	* This data is maize distributions in kJ/m2 based on 2015, 2016 & 2017 field data from maize fields in Jutland
	* Two fields with extraordinarily high values were omitted (6812, 5393)
	*/
	double maizedist[17] = {
		102.7905327,
		58.19878648,
		85.65877728,
		110.9055748,
		30.65682555,
		63.11699379,
		59.05947276,
		41.9277173,
		95.57716202,
		14.42674144,
		6.311699379,
		20.73844082,
		12.62339876,
		25.24679751,
		146.070757,
		// 6812.0,
		5393.0};
	return maizedist[(int)(g_rand_uni_fnc() * 17)];
}

bool FarmManager::InIllegalList( int a_farm_ref, vector<int> * a_farmlist ) {
	/**
	* a_farmlist is a pointer to a list of integers with illegal farm ref numbers. The first entry lists the number of illegal numbers.
	*/
	unsigned sz = (unsigned) a_farmlist->size();
	for (unsigned f = 0; f < sz; f++) {
		if ((*a_farmlist)[f] == a_farm_ref) return true;
	}
	return false;
}

void FarmManager::AddToIllegalList( int a_farm_ref, vector<int> * a_farmlist ) {
	/**
	* a_farmlist is a pointer to a list of integers with illegal farm ref numbers.
	*/
	bool found = false;
	unsigned sz = (unsigned)a_farmlist->size();
	for (unsigned f = 0; f < sz; f++) {
		if ((*a_farmlist)[f] == a_farm_ref) {
			found = true;
			break;
		}
	}
	if (!found) {
		a_farmlist->push_back( a_farm_ref );
	}
}

bool FarmManager::IsDuplicateRef(int a_ref, HunterInfo* a_hinfo)
{
	for (int i = 0; i < int( a_hinfo->FarmHuntRef.size() ); i++)
	{
		
		if (a_ref == a_hinfo->FarmHuntRef[ i ]) return true;
	}
	return false;
}

int FarmManager::FindClosestFarm(HunterInfo a_hinfo, vector<int> * a_farmlist) {
	/**
	* Centroid calculation on farms must be called before calling this method for the first time. \n
	* a_farmlist is a pointer to a list of integers with illegal farm ref numbers. The first entry lists the number of illegal numbers.
	*/
	double best = 99999999999999999.0;
	int bestref = -1;
	double dist = best;
	for (unsigned i = 0; i < m_farms.size(); i++) {
		int fnum = m_farms[i]->GetFarmNumber();
		if (!InIllegalList(fnum, a_farmlist)) {
			if (!IsDuplicateRef(fnum, &a_hinfo)) {
				// Is possible to use this farm, so test it.
				APoint FPt = m_farms[i]->GetCentroids();
				dist = sqrt((double( FPt.m_x - a_hinfo.homeX ) * double( FPt.m_x - a_hinfo.homeX ) + double( FPt.m_y - a_hinfo.homeY ) * double( FPt.m_y - a_hinfo.homeY )));
 				if (dist < best) {
					best = dist;
					bestref = fnum;
				}
			}
		}
	}
	if (bestref == -1) {
		g_msg->Warn( "FarmManager::FindClosestFarm - Cannot find open farm.", "" );
		exit( 0 );
	}
	return bestref;
}

int FarmManager::FindClosestFarmOpenness( HunterInfo a_hunterinfo, vector<int> * a_farmlist, int a_openness ) {
	// Centroid calculation on farms must be called before calling this method for the first time
	double best = 99999999999999999.0;
	int bestref = -1;
	double dist = best;
	for (unsigned i = 0; i < m_farms.size(); i++) {
		if (m_farms[ i ]->GetMaxOpenness() > a_openness) {
			int fref = m_farms[ i ]->GetFarmNumber();
			if (!InIllegalList( fref, a_farmlist )) {
				if (!IsDuplicateRef( fref, &a_hunterinfo )) {
					APoint FPt = m_farms[ i ]->GetCentroids();
					dist = sqrt( (double( FPt.m_x - a_hunterinfo.homeX ) * double( FPt.m_x - a_hunterinfo.homeX ) + double( FPt.m_y - a_hunterinfo.homeY ) * double( FPt.m_y - a_hunterinfo.homeY )) );
					if (dist < best) {
						best = dist;
						bestref = fref;
					}
				}
			}
		}
	}
	if (bestref == -1) {
		g_msg->Warn( "FarmManager::FindClosestFarmOpenness( ) - Cannot find open farm.", "" );
		exit( 0 );
	}
	return bestref;
}

int FarmManager::FindClosestFarmOpennessProb( HunterInfo a_hunterinfo, vector<int> * a_farmlist, int a_openness ) {
	/**
	* Centroid calculation on farms must be called before calling this method for the first time. \n
	* a_farmlist is a pointer to a list of integers with illegal farm ref numbers. The first entry lists the number of illegal numbers.
	* The chance of acceptance is probability based, taking the nearest farm first then testing the next etc.. The probability is based on y = 1*EXP(-x*cfg)
	* with cfg being a fitting parameter in cfg_ClosestFarmProbParam1
	*/

	/** We use typedef here to create our own name for APoint - but it is just two unsigned ints. We will use 'x' for the farm num and 'y' for the distance. */
	typedef  APoint AFarmDist;
	/** We use a vector of AFarmDists to be able to sort it easily */
	vector <AFarmDist> farmdists;
	/** \brief Struct redefining operator < - used for sorting distances from smallest to largest.*/
	struct FarmDistSort {
		bool operator()( AFarmDist a, AFarmDist b ) {
			return a.m_y < b.m_y;
		}
	};
	for (unsigned i = 0; i < m_farms.size(); i++) {
		int fnum = m_farms[ i ]->GetFarmNumber();
		if (!InIllegalList( fnum, a_farmlist )) {
			if (m_farms[ i ]->GetMaxOpenness() > a_openness) {
				if (!IsDuplicateRef( fnum, &a_hunterinfo )) {
					// Is possible to use this farm, so test it.
					APoint FPt = m_farms[ i ]->GetCentroids();
					int dist = int(sqrt( (double( FPt.m_x - a_hunterinfo.homeX ) * double( FPt.m_x - a_hunterinfo.homeX ) + double( FPt.m_y - a_hunterinfo.homeY ) * double( FPt.m_y - a_hunterinfo.homeY )) ));
					if (dist>40000) dist = 40000;
					AFarmDist fd( int( fnum ), dist );
					farmdists.push_back( fd );
				}
			}
		}
	}
	// By here we have a list of farms and their distances - now we have the option to sort it or randomise it - one or other line below should be commented out.
	// sort(farmdists.begin(), farmdists.end(), FarmDistSort()); // Sort
	std::shuffle( farmdists.begin(), farmdists.end(), g_generator ); // Randomise
	// Now the vector is sorted/randomised we loop through and test probabilities
	for (int ch = 1; ch < 100000; ch++) // This loop just makes sure small chances don't mess up the run by tripping out early
	{
		int sz = int( farmdists.size() );
		for (int i = 0; i < sz; i++) {
			double chance = g_rand_uni_fnc();
			// Uses ch from the outer loop to scale probabilities - tripping out here will occur often for farms a long way from the area otherwise
			double calc = cfg_ClosestFarmProbParam2.value()* exp( (cfg_ClosestFarmProbParam1.value() / double( ch ))* farmdists[ i ].m_y );
			if (chance <= calc) return farmdists[ i ].m_x;
		}
	}
	g_msg->Warn( "FarmManager::FindClosestFarmProb", "- No suitable farm found" );
	exit( 0 );
}

int FarmManager::FindClosestFarmOpennessProbSmallIsBest( HunterInfo a_hunterinfo, vector<int> * a_farmlist, int a_openness, vector<int>* a_farmsizelist ) {
	/**
	* Centroid calculation on farms must be called before calling this method for the first time. \n
	* a_farmlist is a pointer to a list of integers with illegal farm ref numbers. The first entry lists the number of illegal numbers.
	* The chance of acceptance is probability based, taking the nearest farm first then testing the next etc.. The probability is based on y = 1*EXP(-x*cfg)
	* with cfg being a fitting parameter in cfg_ClosestFarmProbParam1
	*/

	/** We use typedef here to create our own name for APoint - but it is just two unsigned ints. We will use 'x' for the farm num and 'y' for the distance. */
	typedef  APoint AFarmDist;
	/** We use a vector of AFarmDists to be able to sort it easily */
	vector <AFarmDist> farmdists;
	/** \brief Struct redefining operator < - used for sorting distances from smallest to largest.*/
	struct FarmDistSort {
		bool operator()( AFarmDist a, AFarmDist b ) {
			return a.m_y < b.m_y;
		}
	};
	for (unsigned i = 0; i < m_farms.size(); i++) {
		int fnum = m_farms[ i ]->GetFarmNumber();
		if (!InIllegalList( fnum, a_farmlist )) {
			if (m_farms[ i ]->GetMaxOpenness() > a_openness) {
				if (!IsDuplicateRef( fnum, &a_hunterinfo )) {
					// Is possible to use this farm, so test it.
					APoint FPt = m_farms[ i ]->GetCentroids();
					int dist = int(sqrt( (double( FPt.m_x - a_hunterinfo.homeX ) * double( FPt.m_x - a_hunterinfo.homeX ) + double( FPt.m_y - a_hunterinfo.homeY ) * double( FPt.m_y - a_hunterinfo.homeY )) ));
					if (dist>40000) dist = 40000;
					AFarmDist fd( unsigned( fnum ), dist );
					farmdists.push_back( fd );
				}
			}
		}
	}
	// By here we have a list of farms and their distances - now we have the option to sort it or randomise it - one or other line below should be commented out.
	// sort(farmdists.begin(), farmdists.end(), FarmDistSort()); // Sort
	std::shuffle( farmdists.begin(), farmdists.end(), g_generator ); // Randomise
	// Now the vector is sorted/randomised we loop through and test probabilities
	for (int ch = 1; ch < 100000; ch++) // This loop just makes sure small chances don't mess up the run by tripping out early
	{
		int sz = int( farmdists.size() );
		for (int i = 0; i < sz; i++) {
			double chance = g_rand_uni_fnc();
			// Uses ch from the outer loop to scale probabilities - tripping out here will occur often for farms a long way from the area otherwise
			double calc = cfg_ClosestFarmProbParam2.value()* exp( (cfg_ClosestFarmProbParam1.value() / double( ch ))* farmdists[ i ].m_y );
			if (chance <= calc) {
				// We passed the first test now take a second test based on farm size
				chance = g_rand_uni_fnc();
				calc = pow( double( (*a_farmsizelist)[ farmdists[ i ].m_x ] / 4000.0 ), cfg_FarmSizeProbParam1.value() );
				if (chance>calc) return farmdists[ i ].m_x;
			}
		}
	}
	g_msg->Warn( "FarmManager::FindClosestFarmProb", "- No suitable farm found" );
	exit( 0 );
}

int FarmManager::FindClosestFarmOpennessProbNearRoostIsBest( HunterInfo a_hunterinfo, vector<int> * a_farmlist, int a_openness, vector<APoint>* a_roostlist ) {
	/**
	* Centroid calculation on farms must be called before calling this method for the first time. \n
	* a_farmlist is a pointer to a list of integers with illegal farm ref numbers. The first entry lists the number of illegal numbers.
	* The chance of acceptance is probability based, taking the nearest farm first then testing the next etc.. The probability is based on y = 1*EXP(-x*cfg)
	* with cfg being a fitting parameter in cfg_ClosestFarmProbParam1
	*/

	/** We use typedef here to create our own name for APoint - but it is just two unsigned ints. We will use 'x' for the farm num and 'y' for the distance. */
	typedef  APoint AFarmDist;
	/** We use a vector of AFarmDists to be able to sort it easily */
	vector <AFarmDist> farmdists;
	/** \brief Struct redefining operator < - used for sorting distances from smallest to largest.*/
	struct FarmDistSort {
		bool operator()( AFarmDist a, AFarmDist b ) {
			return a.m_y < b.m_y;
		}
	};
	for (unsigned i = 0; i < m_farms.size(); i++) {
		int fnum = m_farms[ i ]->GetFarmNumber();
		if (!InIllegalList( fnum, a_farmlist )) {
			if (m_farms[ i ]->GetMaxOpenness() > a_openness) {
				if (!IsDuplicateRef( fnum, &a_hunterinfo )) {
					// Is possible to use this farm, so test it.
					APoint FPt = m_farms[ i ]->GetCentroids();
					int dist = int(sqrt( (double( FPt.m_x - a_hunterinfo.homeX ) * double( FPt.m_x - a_hunterinfo.homeX ) + double( FPt.m_y - a_hunterinfo.homeY ) * double( FPt.m_y - a_hunterinfo.homeY )) ));
					if (dist>40000) dist = 40000;
					AFarmDist fd( unsigned( fnum ), dist );
					farmdists.push_back( fd );
				}
			}
		}
	}
	// By here we have a list of farms and their distances - now we have the option to sort it or randomise it - one or other line below should be commented out.
	// sort(farmdists.begin(), farmdists.end(), FarmDistSort()); // Sort
	std::shuffle( farmdists.begin(), farmdists.end(), g_generator ); // Randomise
	// Now the vector is sorted/randomised we loop through and test probabilities
	for (int ch = 1; ch < 100000; ch++) // This loop just makes sure small chances don't mess up the run by tripping out early
	{
		int sz = int( farmdists.size() );
		for (int i = 0; i < sz; i++) {
			double chance = g_rand_uni_fnc();
			// Uses ch from the outer loop to scale probabilities - tripping out here will occur often for farms a long way from the area otherwise
			double calc = cfg_ClosestFarmProbParam2.value()* exp( (cfg_ClosestFarmProbParam1.value() / double( ch ))* farmdists[ i ].m_y );
			if (chance <= calc) {
				// We passed the first test now take a second test based on roost distance
				chance = g_rand_uni_fnc();
				// Loop through each roost and find the closest to the farm - then do probability based on that distance.
				double dist = 10000;
				for (int r = 0; r < int( a_roostlist->size() ); r++) {
					double fdistroostx = farmdists[ i ].m_x - (*a_roostlist)[ r ].m_x;
					double fdistroosty = farmdists[ i ].m_y - (*a_roostlist)[ r ].m_y;
					double distf = sqrt( fdistroostx * fdistroostx + fdistroosty * fdistroostx );
					if (distf < dist) dist = distf;
				}
				calc = -0.01 + pow( dist / 10000.0, cfg_RoostDistProbParam1.value() );
				if (chance>calc) return farmdists[ i ].m_x;
			}
		}
	}
	g_msg->Warn( "FarmManager::FindClosestFarmProbNearRoostBest", "- No suitable farm found" );
	exit( 0 );
}

int FarmManager::FindFarmWithRandom( vector<int> * a_farmlist )
{
	int sz= (int)m_farms.size();
	int f = g_random_fnc(sz);
	while (InIllegalList(m_farms[f]->GetFarmNumber(), a_farmlist))
	{
		f = g_random_fnc(sz);
		if (a_farmlist->size() >= m_farms.size())
		{
			g_msg->Warn("FarmManager::FindFarmWithRandom"," - farm density rule means all hunters cannot be placed");
			exit(0);
		}
	}
	return m_farms[f]->GetFarmNumber();

}

int FarmManager::FindFarmWithOpenness(vector<int> * a_farmlist, int a_openness)
{
	// Centroid calculation on farms must be called before calling this method for the first time
	int sz = (int)m_farms.size();
	int seed = g_random_fnc( sz );
	for (unsigned i = 0; i < m_farms.size(); i++) {
		int index = (i + seed) % sz;
		if (m_farms[ index ]->GetMaxOpenness() > a_openness)
		{
				if (!InIllegalList( m_farms[ index ]->GetFarmNumber(), a_farmlist )) return m_farms[ index ]->GetFarmNumber();
		}
		else AddToIllegalList(m_farms[ index ]->GetFarmNumber(), a_farmlist );
	}
	g_msg->Warn("FarmManager::FindFarmWithOpenness", "- No suitable farm found");
	exit(0);
}

int FarmManager::FindOpennessFarm(int a_openness)
{
	int sz = (int) m_farms.size();
	int seed = g_random_fnc(sz);
	for (unsigned i = 0; i < m_farms.size(); i++)
	{
		int index = (i + seed) % sz;
		if (m_farms[index]->GetMaxOpenness() > a_openness) return m_farms[index]->GetFarmNumber();
	}
	return -1; // Should never happen but if it does we need to handle the -1 error code.
}

bool FarmManager::CheckOpenness( int a_openness, int a_ref ) {
	if (m_farms[ a_ref ]->GetMaxOpenness() > a_openness) return true;
	return false;
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------



void Farm::Centroids(){
/**Calculate a farm's centroid as an average of its fields' centroids.*/

	int sum_centroidx = 0;
	int sum_centroidy = 0;
	int no_fields = (int)m_fields.size();
	for (int i=0; i<no_fields; i++){
		sum_centroidx += m_fields[i]->GetCentroidX();
		sum_centroidy += m_fields[i]->GetCentroidY();
	}
    if (no_fields>0){
        m_farm_centroidx = sum_centroidx/no_fields;
        m_farm_centroidy = sum_centroidy/no_fields;
    }
    else{
        g_msg->Warn("Farm::Centroids", "Farm with no fields ("+ to_string(GetFarmNumber())+ ") is detected. Proceeding.");
        cout<<"Farm::Centroids: Farm with no fields ("<<GetFarmNumber() << ") is detected. Proceeding."<<endl;
        m_farm_centroidx = 0;
        m_farm_centroidy = 0;
    }
}

/**
\brief
Returns the list of fields more open than a_openness
*/
polylist* Farm::ListOpenFields( int a_openness )
{
	polylist* p_list = new polylist;
	int nf = (int) m_fields.size();
	for ( int i = 0; i < nf; i++ )
	{
		if ( m_fields[ i ]->GetOpenness() >= a_openness )
		{
			p_list->push_back( m_fields[i]->GetPoly());
		}
	}
	return p_list;
}

#ifdef __CALCULATE_MANAGEMENT_NORMS
void FarmManager::AddToManagementStats(FarmEvent* a_ev)
{
	vector<int> totals = a_ev->m_field->GetManagementTotals();
	int crop = int(a_ev->m_field->GetCropType());
	for (int i = 0; i < fmc_Foobar; i++) m_managementnormstats[crop][i]+= totals[i];
	m_managementnormstats[crop][fmc_Foobar]++;
}

void FarmManager::OutputManagementNormStats()
{
	ofstream ofile("ALMaSSCropNormsStats.txt", ios::out);
	if (!ofile.is_open()) {
		g_msg->Warn("FarmManager::OutPutManagementNormStats() Cannot open outpuit file ", "ALMaSSCropNormsStats.txt");
		exit(142);
	}
	ofile << int(toc_Foobar) << '\t' << "Crops" << endl;
	ofile << "Crop" << '\t';
	for (int i = 0; i < fmc_Foobar; i++) ofile << m_ManagementCategoryLabels[i] << '\t';
	ofile << "Instances" << endl;
	for (int i = 0; i < toc_Foobar; i++)
	{
		ofile << BackTranslateCropCodes(TTypesOfCrops( i )) << '\t';
		for (int j = 0; j < fmc_Foobar; j++)
		{
			if (m_managementnormstats[i][fmc_Foobar] > 0) ofile << m_managementnormstats[i][j]/double(m_managementnormstats[i][fmc_Foobar]) << '\t';
			else ofile << 0 << '\t';
		}
		ofile << m_managementnormstats[i][fmc_Foobar] << endl;
	}
	ofile.close();
}
#endif
