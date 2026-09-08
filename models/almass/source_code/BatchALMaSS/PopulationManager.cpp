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
/**
\file
\brief
<B>PopulationManager.cpp This is the code file for the population manager and associated classes</B> \n
*/
/**
\file
 by Chris J. Topping and Xiaodong Duan\n
 Version of 17th Dec. 2023 \n
 All rights reserved. \n
 \n
*/
//---------------------------------------------------------------------------
//
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <numeric>
#include <random>

#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "AOR_Probe.h"

using namespace std;

//------------------------------------------------------------------------------

// Set static defaults
int TAnimal::m_SimulationWidth = 0;
int TAnimal::m_SimulationHeight = 0;
int TAnimal::m_DayInYear = 0;
double TAnimal::m_TemperatureToday = 0.0;
Landscape* TAnimal::m_OurLandscape = nullptr;

// Configuration parameters
CfgInt cfg_ProbeMaxAreas("PROBE_MAX_AREAS", CFG_CUSTOM, 10, 1, 16);
CfgInt cfg_ProbeTargetTypesNo("PROBE_TARGET_TYPES_NO", CFG_CUSTOM, 10, 1, 16);

static CfgStr cfg_RipleysOutput_filename("G_RIPLEYSOUTPUT_FILENAME", CFG_CUSTOM, "RipleysOutput.txt");
static CfgStr cfg_ReallyBigOutput_filename("G_REALLYBIGOUTPUT_FILENAME", CFG_CUSTOM, "ReallyBigOutput.txt");
CfgBool cfg_RipleysOutputMonthly_used("G_RIPLEYSOUTPUTMONTHLY_USED", CFG_CUSTOM, false);
CfgBool cfg_CfgRipleysOutputUsed("G_RIPLEYSOUTPUT_USED", CFG_CUSTOM, false);
CfgBool cfg_AorOutput_used("G_AOROUTPUT_USED", CFG_CUSTOM, false);
CfgBool cfg_ReallyBigOutputUsed("G_REALLYBIGOUTPUT_USED", CFG_CUSTOM, false);
CfgBool cfg_Fixed_random_sequence("G_FIXEDRANDOMSEQUENCE", CFG_CUSTOM, false);
CfgInt cfg_AorOutput_interval("G_AORSOUTPUT_INTERVAL", CFG_CUSTOM, 1);
CfgInt cfg_AorOutput_day("G_AOROUTPUT_DAY", CFG_CUSTOM, 60);
CfgInt cfg_AorOutputFirstYear("G_AOROUTPUT_FIRSTYEAR", CFG_CUSTOM, 1);
CfgInt cfg_RipleysOutput_interval("G_RIPLEYSOUTPUT_INTERVAL", CFG_CUSTOM, 1);
CfgInt cfg_RipleysOutput_day("G_RIPLEYSOUTPUT_DAY", CFG_CUSTOM, 60);
CfgInt cfg_RipleysOutputFirstYear("G_RIPLEYSOUTPUT_FIRSTYEAR", CFG_CUSTOM, 1);
CfgInt cfg_ReallyBigOutput_interval("G_REALLYBIGOUTPUT_INTERVAL", CFG_CUSTOM, 1);
CfgInt cfg_ReallyBigOutput_day1("G_REALLYBIGOUTPUT_DAY_ONE", CFG_CUSTOM, 1);
CfgInt cfg_ReallyBigOutput_day2("G_REALLYBIGOUTPUT_DAY_TWO", CFG_CUSTOM, 91);
CfgInt cfg_ReallyBigOutput_day3("G_REALLYBIGOUTPUT_DAY_THREE", CFG_CUSTOM, 182);
CfgInt cfg_ReallyBigOutput_day4("G_REALLYBIGOUTPUT_DAY_FOUR", CFG_CUSTOM, 274);
CfgInt cfg_ReallyBigOutputFirstYear("G_REALLYBIGOUTPUT_FIRSTYEAR", CFG_CUSTOM, 1);
CfgInt cfg_FixedRandomSeed("G_FIXEDRANDOMSEED", CFG_CUSTOM, 0);

// Catastrophe config variables
CfgInt cfg_CatastropheEventStartYear("PM_CATASTROPHEEVENTSTARTYEAR", CFG_CUSTOM, 999999);
// this means unless this is altered it events will not happen
CfgInt cfg_PmEventfrequency("PM_EVENTFREQUENCY", CFG_CUSTOM, 999999); // every X years
CfgInt cfg_PmEventday("PM_EVENTDAY", CFG_CUSTOM, March); // default 1st March
CfgInt cfg_PmEventsize("PM_EVENTSIZE", CFG_CUSTOM, 100); // The percentage change in population size, 100=no change
static CfgInt cfg_DayInMonth("PRB_DAYINMONTH", CFG_CUSTOM, 1);

/** \brief The size to create the map guard.*/
CfgInt cfg_MapGuardCellSize("MAP_GUARD_CELL_SIZE", CFG_CUSTOM, 100);

extern std::mt19937 g_generator;
char g_Str[255];

TTypesOfPopulation g_Species;

/** \This is the global variable to store the number of threads*/
#ifdef _OPENMP
int g_thread_count = omp_get_max_threads();
#else
int g_thread_count = 1;
#endif


//---------------------------------------------------------------------------
//                        Population Manager
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Probe_Data::CloseFile() const {
	if (m_MyFile != nullptr)
	{
		if (m_MyFile->is_open()) m_MyFile->close();
		delete m_MyFile;
	}
};

/**
\brief
Function to compare to TAnimal's m_Location_x
*/
class CompareX {
public:
	bool operator()(TAnimal* & A1, TAnimal* & A2) const {
		return A1->Supply_m_Location_x() < A2->Supply_m_Location_x();
	}
};

class CompareY {
public:
	bool operator()(TAnimal*& A1, TAnimal*& A2) const { return A1->Supply_m_Location_y() < A2->Supply_m_Location_y(); }
};

/**
\brief
Function to compare to TAnimal's m_CurrentStateNo to anything but -1
*/
class CompareStateAlive {
public:
	bool operator()(TAnimal* & A1) const { return A1->GetCurrentStateNo() != -1; }
};

/**
\brief
Function to compare to TAnimal's Current behavioural state
*/
/**
NB WhatState must be reimplemented by all descendents of TAnimal that use this functionality (not all do)
*/
class CompareState {
public:
	bool operator()(TAnimal* & A1, TAnimal* & A2) const { return A1->WhatState() < A2->WhatState(); }
};

/**
\brief
Function to compare to TAnimal's m_CurrentStateNo
*/
class CompareStateR {
public:
	bool operator()(TAnimal*& A1, TAnimal*& A2) const { return A1->GetCurrentStateNo() > A2->GetCurrentStateNo(); }
};

/**
\brief
Function to compare to TAnimal's m_CurrentStateNo to -1
*/
class CompareStateDead {
public:
	bool operator()(TAnimal* A1) const { return A1->GetCurrentStateNo() == -1; }
};

//---------------------------------------------------------------------------
/**
Constructor for the Population_Manager_Base class
*/
Population_Manager_Base::Population_Manager_Base(Landscape* L) {
	// Keep a pointer to the landscape this population is in
	m_TheLandscape = L;
	// Set the simulation bounds
	SimH = m_TheLandscape->SupplySimAreaHeight();
	SimW = m_TheLandscape->SupplySimAreaWidth();
	SimHH = m_TheLandscape->SupplySimAreaHeight() / 2;
	SimWH = m_TheLandscape->SupplySimAreaWidth() / 2;
	TAnimal::SetSimulationHeight(SimH);
	TAnimal::SetSimulationWidth(SimW);
	TAnimal::SetOurLandscape(L);
}
//---------------------------------------------------------------------------

/**
Constructor for the Population_Manager class
*/
Population_Manager::Population_Manager(Landscape* a_l_ptr, int a_numberLifeStages) : Population_Manager_Base(a_l_ptr) {
	// create N empty arrays
	TheSubArraysIterators.resize(a_numberLifeStages);
	TheSubArrayPreviousIndex.resize(a_numberLifeStages);
	TheSubArrayThreadIndex.resize(a_numberLifeStages);
	TheSubArrays.resize(a_numberLifeStages);
	TheSubArraysSizes.resize(a_numberLifeStages);
	m_LifeStageOrderVec.resize(a_numberLifeStages);
	for (int i = 0; i < a_numberLifeStages; i++)
	{
		TheSubArrayPreviousIndex[i] = -1;
		TheSubArrayThreadIndex[i] = -1;
		TheSubArrays[i].resize(g_thread_count);
		TheSubArraysSizes[i].resize(g_thread_count);
		for (int j = 0; j < g_thread_count; j++){ 
			TheSubArrays[i][j] = new forward_list<TAnimal*>;
			TheSubArraysSizes[i][j] = 0;
		}
		m_LifeStageOrderVec[i] = i;
	}

	BeforeStepActions.resize(a_numberLifeStages);
	for (int i = 0; i < a_numberLifeStages; i++)
	{
		// Set default BeforeStepActions
		BeforeStepActions[i] = 0;
		m_ListNames[i] = "Unknown";
		m_LiveArraySize.push_back(0);
	}
	// modify this if they need to in descendent classes
	StateNamesLength = 0; // initialise this variable.
	if (cfg_RipleysOutputMonthly_used.value()) { OpenTheMonthlyRipleysOutputProbe(); }
	// Ensure the GUI pointer is NULL in case we are not in GUI mode
	m_SeasonNumber = 0; // Ensure that we start in season number 0. Season number is incremented at day in year 183

	//initialise map guard thing
	m_guard_cell_size = cfg_MapGuardCellSize.value();
	m_guard_cell_height_num = SimH / m_guard_cell_size;
	m_guard_cell_width_num = SimW / m_guard_cell_size;


	m_MapGuard.resize(m_guard_cell_height_num);
	for(int j = 0; j < m_guard_cell_height_num; j++){
		m_MapGuard[j].resize(m_guard_cell_width_num);
		for(int k = 0; k < m_guard_cell_width_num; k++){
			m_MapGuard[j][k] = new omp_nest_lock_t;
			omp_init_nest_lock(m_MapGuard[j][k]);
		}
	}

	m_is_paralleled = false; //by default it is not paralleled
}
//---------------------------------------------------------------------------

/**
Destructor for the Population_Manager class
*/
Population_Manager::~Population_Manager(void) {
	// clean-up // no need to delete members of the array
	if (cfg_CfgRipleysOutputUsed.value()) { CloseTheRipleysOutputProbe(); }
	if (cfg_RipleysOutputMonthly_used.value()) { CloseTheMonthlyRipleysOutputProbe(); }
	if (cfg_ReallyBigOutputUsed.value()) { CloseTheReallyBigOutputProbe(); }
	if (cfg_AorOutput_used.value()) delete m_AOR_Probe;
	for (int i = 0; i < TheSubArrays.size(); i++)
	{
		for (int j = 0; j < TheSubArrays[i].size(); j++){
		for (auto it = TheSubArrays[i][j]->begin(); it != TheSubArrays[i][j]->end(); ++it){
				if(*it != nullptr)
					delete *it;
			}
			if(TheSubArrays[i][j] != nullptr)
				delete TheSubArrays[i][j];
		} 
	}


	for(int j = 0; j < m_guard_cell_height_num; j++){
		for(int k = 0; k < m_guard_cell_width_num; k++){
			omp_destroy_nest_lock(m_MapGuard[j][k]);
			delete m_MapGuard[j][k];
		}
	}
}

void Population_Manager::SetNoProbesAndSpeciesSpecificFunctions(int a_pn) {
	// This function relies on the fact that g_Species has been set already
	m_NoProbes = a_pn;
	m_TheLandscape->SetSpeciesFunctions(g_Species);
}

void Population_Manager::OpenTheAOROutputProbe(const string& a_AORFilename) {
	m_AORProbeFileName = a_AORFilename;
	m_AOR_Probe = new AOR_Probe(this, m_TheLandscape, a_AORFilename);
}
//-----------------------------------------------------------------------------


/**
Can be used in descendent classes
*/
void Population_Manager::DoFirst() {
}

//---------------------------------------------------------------------------

/**
Can be used in descendent classes
*/
void Population_Manager::DoBefore() {
}

//---------------------------------------------------------------------------

/**
This is the main scheduling method for the population manager. \n
Note the structure of Shuffle_or_Sort(), DoFirst(), BeginStep, DoBefore(), Step looping until all are finished, DoAfter(), EndStep, DoLast(). \n
*/
void Population_Manager::Run(int NoTSteps) {
	/**
	Can do multiple time-steps here inside one landscape time-step (a day). This is used in the roe deer model to provide 10 minute behavioural time-steps.
	*/
	for (int t_steps = 0; t_steps < NoTSteps; t_steps++)
	{
		const unsigned size1 = SupplyListIndexSize();
		/**
		* It is necessary to remove any dead animals before the timestep starts. It is possible that animals are killed after their population manager Run method has been executed.
		* This is the case with geese and hunters. Checking death first prevents this becomming a problem.
		*/
		for (unsigned listindex = 0; listindex < size1; listindex++)
		{
			// Must check each object in the list for m_CurrentStateNo==-1
			m_LiveArraySize[listindex] = PartitionLiveDead(listindex);
		}

		//Rest the fast read support variables for supply animal ptr
		for (unsigned listindex = 0; listindex < size1; listindex++)
		{
			TheSubArrayPreviousIndex[listindex] = 0;
			TheSubArrayThreadIndex[listindex] = 0;
			TheSubArraysIterators[listindex] = TheSubArrays[listindex][0]->begin();

			//let's find the first non-empty subarray
			for (int j = 0; j < TheSubArraysSizes[listindex].size(); j++){
				if(TheSubArraysSizes[listindex][j] > 0){
					TheSubArrayThreadIndex[listindex] = j;
					TheSubArraysIterators[listindex] = TheSubArrays[listindex][j]->begin();
					break;
				}
			}
		}

		// begin step actions ...
		// set all stepdone to false.... is this really necessary??
		for (int listindex = 0; listindex < size1; listindex++)
		{
			#pragma omp parallel
			{
			int thread_id = omp_get_thread_num();
			for(auto it = TheSubArrays[listindex][thread_id]->begin(); it != TheSubArrays[listindex][thread_id]->end(); ++it){
				(*it)->SetStepDone(false);
			}
			}
		}
		//Only do this when single thread is used
		if(omp_get_max_threads()<=1){
			for (unsigned listindex = 0; listindex < size1; listindex++)
			{
				// Call the Shuffle/Sort procedures
				if(m_LiveArraySize[listindex] > 1)
					Shuffle_or_Sort(listindex);
			}
		}
		const int day = m_TheLandscape->SupplyDayInYear();
		const int year = m_TheLandscape->SupplyYearNumber();
		// Need to check if Ripleys Statistic needs to be saved
		if (cfg_CfgRipleysOutputUsed.value())
		{
			const int Year = m_TheLandscape->SupplyYearNumber();
			if (Year >= cfg_RipleysOutputFirstYear.value())
			{
				if (Year % cfg_RipleysOutput_interval.value() == 0)
				{
					if (cfg_RipleysOutput_day.value() == day)
					{
						TheRipleysOutputProbe(RipleysOutputPrb); // Do the Ripley Probe
					}
				}
			}
		}
		// Need to check if AOR output needs to be saved
		if (cfg_AorOutput_used.value())
		{
			if (year >= cfg_AorOutputFirstYear.value())
			{
				if (year % cfg_AorOutput_interval.value() == 0)
				{
					if (cfg_AorOutput_day.value() == day)
					{
						// Do the AOR Probe
						TheAOROutputProbe();
					}
				}
			}
		}
		// Need to check if Monthly Ripleys Statistic needs to be saved
		if (cfg_RipleysOutputMonthly_used.value())
		{
			if (m_TheLandscape->SupplyDayInMonth() == 1)
			{
				if (year >= cfg_RipleysOutputFirstYear.value())
				{
					if (year % cfg_RipleysOutput_interval.value() == 0)
					{
						int month = m_TheLandscape->SupplyMonth();
						// Do the Ripley Probe
						switch (month)
						{
						case 1:
							TheRipleysOutputProbe(RipleysOutputPrb1);
							break;
						case 2:
							TheRipleysOutputProbe(RipleysOutputPrb2);
							break;
						case 3:
							TheRipleysOutputProbe(RipleysOutputPrb3);
							break;
						case 4:
							TheRipleysOutputProbe(RipleysOutputPrb4);
							break;
						case 5:
							TheRipleysOutputProbe(RipleysOutputPrb5);
							break;
						case 6:
							TheRipleysOutputProbe(RipleysOutputPrb6);
							break;
						case 7:
							TheRipleysOutputProbe(RipleysOutputPrb7);
							break;
						case 8:
							TheRipleysOutputProbe(RipleysOutputPrb8);
							break;
						case 9:
							TheRipleysOutputProbe(RipleysOutputPrb9);
							break;
						case 10:
							TheRipleysOutputProbe(RipleysOutputPrb10);
							break;
						case 11:
							TheRipleysOutputProbe(RipleysOutputPrb11);
							break;
						case 12:
							TheRipleysOutputProbe(RipleysOutputPrb12);
							break;
						default:
							g_msg->Warn("Population_Manager::Run ", "Wrong month");
							exit(-1);
						}
					}
				}
			}
		}
		// Need to check if Really Big Probe needs to be saved
		if (cfg_ReallyBigOutputUsed.value())
		{
			if (year >= cfg_ReallyBigOutputFirstYear.value())
			{
				if (year % cfg_ReallyBigOutput_interval.value() == 0)
				{
					if (cfg_ReallyBigOutput_day1.value() == day || cfg_ReallyBigOutput_day2.value() == day ||
						cfg_ReallyBigOutput_day3.value() == day || cfg_ReallyBigOutput_day4.value() == day ||
						cfg_ReallyBigOutput_day1.value() == -1)
					{
						// Do the Ripley Probe
						TheReallyBigOutputProbe();
					}
				}
			}
		}
		// Set the static variables for TAnimal (speed optimisation)
		TAnimal::SetDayInYear(day);
		TAnimal::SetTempToday(m_TheLandscape->SupplyTemp());
		RunStepMethods();
	} // End of time step loop
}
//---------------------------------------------------------------------------

void Population_Manager::RunStepMethods() {
	//int size2;
	//const int size1 = SupplyListIndexSize();
	DoFirst();
	// call the begin-step-method of all objects
	//for (int listindex = 0; listindex < size1; listindex++)
	for (auto& listindex : m_LifeStageOrderVec)
	{
		//size2 = GetLiveArraySize(listindex);
		#pragma omp parallel
		{
			int thread_id = omp_get_thread_num();
			for(auto it = TheSubArrays[listindex][thread_id]->begin(); it != TheSubArrays[listindex][thread_id]->end(); ++it){
				int loc_x = (*it)->Supply_m_Location_x();
				int loc_y = (*it)->Supply_m_Location_y();
				int loc_x_cell = loc_x / m_guard_cell_size;
				int loc_y_cell = loc_y / m_guard_cell_size;
				(*it)->SetGuardMapIndex(loc_x_cell, loc_y_cell);
				omp_set_nest_lock(m_MapGuard[loc_y_cell][loc_x_cell]);
				(*it)->BeginStep();
				if(m_is_paralleled){
					loc_x_cell = (*it)->SupplyGuardCellX();
					loc_y_cell = (*it)->SupplyGuardCellY();
				}
				omp_unset_nest_lock(m_MapGuard[loc_y_cell][loc_x_cell]);
			}
		}
	}
	DoBefore();

	// call the step-method of all objects
	do
	{
		for (auto& listindex : m_LifeStageOrderVec)
		{
			//size2 = GetLiveArraySize(listindex);
			//cout<<"Parallel starts..."<<endl;
			#pragma omp parallel
			{
				int thread_id = omp_get_thread_num();
				//cout<<" thread_id: "<<thread_id<<endl;
				//cout<<"listindex: "<<listindex<<" thread_id: "<<thread_id<<" j: "<<j<<endl;
				for(auto it = TheSubArrays[listindex][thread_id]->begin(); it != TheSubArrays[listindex][thread_id]->end(); ++it){
					//let's check the guard map
					int loc_x = (*it)->Supply_m_Location_x();
					int loc_y = (*it)->Supply_m_Location_y();
					int loc_x_cell = loc_x / m_guard_cell_size;
					int loc_y_cell = loc_y / m_guard_cell_size;
					(*it)->SetGuardMapIndex(loc_x_cell, loc_y_cell);
					omp_set_nest_lock(m_MapGuard[loc_y_cell][loc_x_cell]);
					(*it)->Step();

					//only update the position if multiple threads are used
					if(m_is_paralleled){
						loc_x_cell = (*it)->SupplyGuardCellX();
						loc_y_cell = (*it)->SupplyGuardCellY();
					}
					omp_unset_nest_lock(m_MapGuard[loc_y_cell][loc_x_cell]);
					
				}
			}
			DoSpecialBetweenLifeStages(listindex);
			//cout<<"Parallel ends!!!"<<endl;
		} // for listindex
	}
	while (!StepFinished());
	DoAfter();
	// call the end-step-method of all objects
	for (auto& listindex : m_LifeStageOrderVec)
	{
		//size2 = GetLiveArraySize(listindex);
		#pragma omp parallel
		{
			int thread_id = omp_get_thread_num();
			for(auto it = TheSubArrays[listindex][thread_id]->begin(); it != TheSubArrays[listindex][thread_id]->end(); ++it){
				int loc_x = (*it)->Supply_m_Location_x();
				int loc_y = (*it)->Supply_m_Location_y();
				int loc_x_cell = loc_x / m_guard_cell_size;
				int loc_y_cell = loc_y / m_guard_cell_size;
				(*it)->SetGuardMapIndex(loc_x_cell, loc_y_cell);
				omp_set_nest_lock(m_MapGuard[loc_y_cell][loc_x_cell]);
				(*it)->EndStep();
				if(m_is_paralleled){
					loc_x_cell = (*it)->SupplyGuardCellX();
					loc_y_cell = (*it)->SupplyGuardCellY();
				}
				omp_unset_nest_lock(m_MapGuard[loc_y_cell][loc_x_cell]);
			}
		}
	}
	// ----------------
	// end of this step actions
	// For each animal list

	DoLast();
}
//---------------------------------------------------------------------------

/** Returns true if and only if all objects have finished the current step */
bool Population_Manager::StepFinished(void) {
	for (int listindex = 0; listindex < TheSubArrays.size(); listindex++)
	{
		for (int j=0; j < TheSubArrays[listindex].size(); j++){
			for (auto it = TheSubArrays[listindex][j]->begin(); it != TheSubArrays[listindex][j]->end(); ++it){
				if (!(*it)->GetStepDone()) return false;
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------

/**
Can be used in descendent classes
*/
void Population_Manager::DoAfter() {
	//TODO: Add your source code here
}

//---------------------------------------------------------------------------
/**
Collects some data to describe the number of animals in each state at the end of the day
*/
void Population_Manager::DoLast() {
	//TODO: Add your source code here
}
//---------------------------------------------------------------------------

void Population_Manager::DisplayLocations() {
	/**
	* Used to update the graphics when control is not returned to the ALMaSS_GUI between timesteps.
	*/
}
//---------------------------------------------------------------------------

/**
Default probe file input
*/

int Population_Manager_Base::ProbeFileInput(char* p_Filename, int p_ProbeNo) {

	int data = 0;
	int data2 = 0;
	char S[255];
	FILE* PFile = fopen(p_Filename, "r");
	if (!PFile)
	{
		m_TheLandscape->Warn("Population Manager - cannot open Probe File ", p_Filename);
		exit(0);
	}
	fgets(S, 255, PFile); // dummy line
	fgets(S, 255, PFile); // dummy line
	fscanf(PFile, "%d\n", &data); // Reporting interval
	TheProbe[p_ProbeNo]->m_ReportInterval = data;
	fgets(S, 255, PFile); // dummy line
	fscanf(PFile, "%d\n", &data); // Write to file
	if (data == 0) TheProbe[p_ProbeNo]->m_FileRecord = false;
	else TheProbe[p_ProbeNo]->m_FileRecord = true;
	fgets(S, 255, PFile); // dummy line
	for (int i = 0; i < cfg_ProbeTargetTypesNo.value(); i++)
	{
		fscanf(PFile, "%d", &data);
		if (data > 0) TheProbe[p_ProbeNo]->m_TargetTypes[i] = true;
		else TheProbe[p_ProbeNo]->m_TargetTypes[i] = false;
	}

	fgets(S, 255, PFile); // dummy line
	fgets(S, 255, PFile); // dummy line
	fscanf(PFile, "%d", &data);
	TheProbe[p_ProbeNo]->m_NoAreas = data;
	fgets(S, 255, PFile); // dummy line
	fgets(S, 255, PFile); // dummy line
	fscanf(PFile, "%d", &data2); // No References areas
	fgets(S, 255, PFile); // dummy line
	fgets(S, 255, PFile); // dummy line
	fscanf(PFile, "%d", &data); // Type reference for probe
	if (data == 1) TheProbe[p_ProbeNo]->m_NoEleTypes = data2;
	else TheProbe[p_ProbeNo]->m_NoEleTypes = 0;
	if (data == 2) TheProbe[p_ProbeNo]->m_NoVegTypes = data2;
	else TheProbe[p_ProbeNo]->m_NoVegTypes = 0;
	if (data == 3) TheProbe[p_ProbeNo]->m_NoFarms = data2;
	else TheProbe[p_ProbeNo]->m_NoFarms = 0;
	fgets(S, 255, PFile); // dummy line
	fgets(S, 255, PFile); // dummy line
	// Now read in the areas data
	TheProbe[p_ProbeNo]->m_FullLandscapeProbe = false; // Default to full landscape

	for (int i = 0; i < cfg_ProbeMaxAreas.value(); i++)
	{
		// any -1 value in coords will result in using the full landscape
		fscanf(PFile, "%d", &data);
		if (data == -1) TheProbe[p_ProbeNo]->m_FullLandscapeProbe = true;
		TheProbe[p_ProbeNo]->m_Rect[i].m_x1 = data;
		fscanf(PFile, "%d", &data);
		if (data == -1) TheProbe[p_ProbeNo]->m_FullLandscapeProbe = true;
		TheProbe[p_ProbeNo]->m_Rect[i].m_y1 = data;
		fscanf(PFile, "%d", &data);
		if (data == -1) TheProbe[p_ProbeNo]->m_FullLandscapeProbe = true;
		TheProbe[p_ProbeNo]->m_Rect[i].m_x2 = data;
		fscanf(PFile, "%d", &data);
		if (data == -1) TheProbe[p_ProbeNo]->m_FullLandscapeProbe = true;
		TheProbe[p_ProbeNo]->m_Rect[i].m_y2 = data;
	}
	fgets(S, 255, PFile); // dummy line
	fgets(S, 255, PFile); // dummy line
	if (TheProbe[p_ProbeNo]->m_NoVegTypes > 0)
	{
		for (int i = 0; i < 25; i++)
		{
			fscanf(PFile, "%d", &data);
			if (data != 999) TheProbe[p_ProbeNo]->m_RefVeg[i] = m_TheLandscape->TranslateVegTypes(data);
		}
	}
	else if (TheProbe[p_ProbeNo]->m_NoFarms > 0)
	{
		for (int i = 0; i < 25; i++)
		{
			fscanf(PFile, "%d", &data);
			if (data != 999) TheProbe[p_ProbeNo]->m_RefFarms[i] = data;
		}
	}
	else
	{
		for (int i = 0; i < 25; i++)
		{
			fscanf(PFile, "%d", &data);
			if (data != 999) TheProbe[p_ProbeNo]->m_RefEle[i] = m_TheLandscape->TranslateEleTypes(data);
		}
	}
	fclose(PFile);
	return data2; // number of data references
}


//-----------------------------------------------------------------------------

/**
Special pesticide related probe. Overridden in descendent classes
*/
void Population_Manager::ImpactedProbe() {
}
//-----------------------------------------------------------------------------

/**
Default data probe. Rarely used in actuality but always available
*/
unsigned Population_Manager::Probe(int ListIndex, Probe_Data* p_TheProbe) {
	// Counts through the list and goes through each area to see if the animal
	// is standing there and if the farm, veg or element conditions are met
	AnimalPosition Sp;
	long NumberSk = 0;
	// Four possibilities
	// either NoVegTypes or NoElementTypes or NoFarmTypes is >0 or all==0
	int temp_thread_num = omp_get_max_threads();
	if (p_TheProbe->m_NoFarms != 0)
	{
		for (int thread_id=0; thread_id<temp_thread_num; thread_id++){
			for(auto it = TheSubArrays[ListIndex][thread_id]->begin(); it != TheSubArrays[ListIndex][thread_id]->end(); ++it){
				if((*it)->GetCurrentStateNo() < 0){
					continue;	
				}
				Sp = (*it)->SupplyPosition();
				unsigned Farm = (*it)->SupplyFarmOwnerRef();
				for (int i = 0; i < p_TheProbe->m_NoAreas; i++)
				{
					if ((p_TheProbe->m_FullLandscapeProbe) || (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.
						m_x <= p_TheProbe->m_Rect[i].m_x2 && Sp.m_y <= p_TheProbe->m_Rect[i].m_y2))
						for (int k = 0; k < p_TheProbe->m_NoFarms; k++)
						{
							if (p_TheProbe->m_RefFarms[k] == Farm) NumberSk++; // it is in the square so increment number
						}
				}
			}
		}
	}
	else if (p_TheProbe->m_NoEleTypes != 0)
	{
		for (int thread_id=0; thread_id<temp_thread_num; thread_id++){
			for(auto it = TheSubArrays[ListIndex][thread_id]->begin(); it != TheSubArrays[ListIndex][thread_id]->end(); ++it){
				if((*it)->GetCurrentStateNo() < 0){
					continue;	
				}
				for (unsigned i = 0; i < p_TheProbe->m_NoAreas; i++)
				{
				if ((p_TheProbe->m_FullLandscapeProbe) || (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.
					m_x <= p_TheProbe->m_Rect[i].m_x2 && Sp.m_y <= p_TheProbe->m_Rect[i].m_y2))
					for (unsigned k = 0; k < p_TheProbe->m_NoEleTypes; k++)
					{
						if (p_TheProbe->m_RefEle[k] == Sp.m_EleType) NumberSk++;
						// it is in the square so increment number
					}
				}
			}
		}
	}
	else
	{
		if (p_TheProbe->m_NoVegTypes != 0)
		{
			for (int thread_id=0; thread_id<temp_thread_num; thread_id++){
				for(auto it = TheSubArrays[ListIndex][thread_id]->begin(); it != TheSubArrays[ListIndex][thread_id]->end(); ++it){
					if((*it)->GetCurrentStateNo() < 0){
						continue;	
					}
					Sp = (*it)->SupplyPosition();
					for (unsigned i = 0; i < p_TheProbe->m_NoAreas; i++)
					{
						if ((p_TheProbe->m_FullLandscapeProbe) || (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.
							m_x <= p_TheProbe->m_Rect[i].m_x2 && Sp.m_y <= p_TheProbe->m_Rect[i].m_y2))
						{
							for (unsigned k = 0; k < p_TheProbe->m_NoVegTypes; k++)
							{
								if (p_TheProbe->m_RefVeg[k] == Sp.m_VegType) NumberSk++;
								// it is in the square so increment number
							}
						}
					}
				}
			}
		}
		else // both must be zero
		{
			unsigned sz = static_cast<long>(GetLiveArraySize(ListIndex));

			// It is worth checking whether we have a total dump of all individuals
			// if so don't bother with asking each where it is
			if (p_TheProbe->m_FullLandscapeProbe)   return sz; 
			// Asking for a subset - need to test them all
			for (int thread_id=0; thread_id<temp_thread_num; thread_id++){
				for(auto it = TheSubArrays[ListIndex][thread_id]->begin(); it != TheSubArrays[ListIndex][thread_id]->end(); ++it){
					if((*it)->GetCurrentStateNo() < 0){
						continue;	
					}
					Sp = (*it)->SupplyPosition();
					for (unsigned i = 0; i < p_TheProbe->m_NoAreas; i++)
					{
						if (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.m_x <=
							p_TheProbe->m_Rect[i].m_x2 && Sp.m_y <= p_TheProbe->m_Rect[i].m_y2) NumberSk++;
						// it is in the square so increment number
					}
				}
			}
		}
	}
	return NumberSk;
}
//-----------------------------------------------------------------------------


/**
This method handles species specific outputs. This is one place to do it. More commonly this is done in descendent classes
*/
char* Population_Manager::SpeciesSpecificReporting(int a_species, int a_time) {
	strcpy(g_Str, "");
	if (a_species == -1) { return g_Str; }
	// Vole Model
	if (a_species == 1)
	{
		ProbeReport(a_time);
#ifdef __SpecificPesticideEffectsVinclozolinLike__
	ImpactProbeReport( a_time );
#endif
#ifdef __WithinOrchardPesticideSim__
	ImpactProbeReport( a_time );
#endif
	}
	// Skylark Model
	else if (a_species == 0)
	{
		int No;
		int a_day = a_time % 365;
		if (a_time % 365 == 364)
		{
			//Write the Breeding Attempts Probe
			int BreedingFemales, YoungOfTheYear, TotalPop, TotalFemales, TotalMales, BreedingAttempts;
			No = TheBreedingSuccessProbe(BreedingFemales, YoungOfTheYear, TotalPop, TotalFemales, TotalMales,
			                             BreedingAttempts);
			float bs = 0;
			if (BreedingFemales > 0)
			{
				bs = No / static_cast<float>(BreedingFemales);
				//bs = successful breeding attempt per attempting to breed female
			}
			BreedingSuccessProbeOutput(bs, BreedingFemales, YoungOfTheYear, TotalPop, TotalFemales, TotalMales, a_time,
			                           BreedingAttempts);
		}
		if (a_day == 152)
		{
			// Need to fill in the landscape from 1st June, it will change before
			// the count of fledgelings is needed
			m_TheLandscape->FillVegAreaData();
		}
		if (a_day == 197)
		{
			for (int ProbeNo = 0; ProbeNo < m_NoProbes; ProbeNo++)
			{
				No = TheFledgelingProbe();
				// Do some output
				FledgelingProbeOutput(No, a_time);
			}
		}
#ifdef __SKPOM
    // This is clunky but for purposes of validation we want probes throughout the year
	// First get the breeding pairs data.
	BreedingPairsOutput(a_day);
#else
		ProbeReport(a_time);
#endif
	}
	else return ProbeReport(a_time);
	return g_Str;
}

/**
open the Ripley probe
*/
bool Population_Manager::OpenTheRipleysOutputProbe() {
	RipleysOutputPrb = new ofstream(cfg_RipleysOutput_filename.value());
	if (!RipleysOutputPrb)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	return true;
}

//-----------------------------------------------------------------------------
/**
open 12 ripley output probes, one for each month
*/
bool Population_Manager::OpenTheMonthlyRipleysOutputProbe() {
	RipleysOutputPrb1 = new ofstream("RipleyOutput_Jan.txt");
	if (!RipleysOutputPrb1)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb2 = new ofstream("RipleyOutput_Feb.txt");
	if (!RipleysOutputPrb2)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb3 = new ofstream("RipleyOutput_Mar.txt");
	if (!RipleysOutputPrb3)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb4 = new ofstream("RipleyOutput_Apr.txt");
	if (!RipleysOutputPrb4)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb5 = new ofstream("RipleyOutput_May.txt");
	if (!RipleysOutputPrb5)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb6 = new ofstream("RipleyOutput_Jun.txt");
	if (!RipleysOutputPrb6)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb7 = new ofstream("RipleyOutput_Jul.txt");
	if (!RipleysOutputPrb7)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb8 = new ofstream("RipleyOutput_Aug.txt");
	if (!RipleysOutputPrb8)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb9 = new ofstream("RipleyOutput_Sep.txt");
	if (!RipleysOutputPrb9)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb10 = new ofstream("RipleyOutput_Oct.txt");
	if (!RipleysOutputPrb10)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb11 = new ofstream("RipleyOutput_Nov.txt");
	if (!RipleysOutputPrb11)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	RipleysOutputPrb12 = new ofstream("RipleyOutput_Dec.txt");
	if (!RipleysOutputPrb12)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_RipleysOutput_filename.value());
		exit(1);
	}
	return true;
}

//-----------------------------------------------------------------------------

/**
open the probe
*/
bool Population_Manager::OpenTheReallyBigProbe() {
	ReallyBigOutputPrb = new ofstream(cfg_ReallyBigOutput_filename.value());
	if (!ReallyBigOutputPrb)
	{
		g_msg->Warn(WARN_FILE, "Population_Manager::OpenTheRipleysOutputProbe(): ""Unable to open probe file",
		            cfg_ReallyBigOutput_filename.value());
		exit(1);
	}
	return true;
}
//-----------------------------------------------------------------------------

/**
close the probe
*/
void Population_Manager::CloseTheRipleysOutputProbe() {
	if (cfg_RipleysOutputMonthly_used.value()) RipleysOutputPrb->close();
	RipleysOutputPrb = nullptr;
}
//-----------------------------------------------------------------------------

/**
close the monthly probes
*/
void Population_Manager::CloseTheMonthlyRipleysOutputProbe() const {
	RipleysOutputPrb1->close();
	RipleysOutputPrb2->close();
	RipleysOutputPrb3->close();
	RipleysOutputPrb4->close();
	RipleysOutputPrb5->close();
	RipleysOutputPrb6->close();
	RipleysOutputPrb7->close();
	RipleysOutputPrb8->close();
	RipleysOutputPrb9->close();
	RipleysOutputPrb10->close();
	RipleysOutputPrb11->close();
	RipleysOutputPrb12->close();
}

//-----------------------------------------------------------------------------
/**
close the probe
*/
void Population_Manager::CloseTheReallyBigOutputProbe() {
	if (ReallyBigOutputPrb != nullptr) ReallyBigOutputPrb->close();
	ReallyBigOutputPrb = nullptr;
}

//-----------------------------------------------------------------------------

/** Store results for big output probe. This method must be overridden in descendent classes */
void Population_Manager::TheReallyBigOutputProbe() {
}
//-----------------------------------------------------------------------------

/**  Store results for ripley probe. This method must be overridden in descendent classes */
void Population_Manager::TheRipleysOutputProbe(ofstream* /* a_Prob */) {
}
//---------------------------------------------------------------------------

/** Store results for AOR probe. This method must be overridden in descendent classes */
void Population_Manager::TheAOROutputProbe() {
}
//---------------------------------------------------------------------------


/**
Gets a random individual that is not a_me
*/
TAnimal* Population_Manager::FindIndividual(unsigned Type, TAnimal* a_me) {
	int i = static_cast<int>(GetLiveArraySize(Type) * g_rand_uni_fnc());
	int temp_thread_num = omp_get_max_threads();

	int temp_animal_num = 0;
	for(int j=0; j<temp_thread_num; j++){
		for(auto it = TheSubArrays[Type][j]->begin(); it != TheSubArrays[Type][j]->end(); ++it){
			if(temp_animal_num >= i){
				if((*it) != a_me){
					return (*it);
				}
			}
			if(temp_animal_num == i){
				return nullptr;
			}
			temp_animal_num++;
		}
	}
	return nullptr;
}

/**
Finds the closest individual to an x,y point that is not a_me
*/
TAnimal* Population_Manager::FindClosest(int x, int y, unsigned Type, TAnimal* a_me) {

	int distance = 100000000;
	TAnimal* TA = nullptr;
	int temp_thread_num = omp_get_max_threads();
	for(int j=0; j<temp_thread_num; j++){
		for(auto it = TheSubArrays[Type][j]->begin(); it != TheSubArrays[Type][j]->end(); ++it){
			int dx = (*it)->Supply_m_Location_x();
			int dy = (*it)->Supply_m_Location_y();
			dx = dx - x;
			dx *= dx;
			dy = dy - y;
			dy *= dy;
			int d = dx + dy;
			if (d < distance)
			{
				if ((*it) != a_me)
				{
					if ((*it)->GetCurrentStateNo() != -1)
					{
						TA = (*it);
						distance = d;
					}
				}
			}
		}
	}
	return TA;
}

/**
Sort TheSubArrays w.r.t. the m_Location_x attribute
*/
void Population_Manager::SortX(unsigned Type) {
	int temp_thread_num = omp_get_max_threads();
	for(int i=0; i<temp_thread_num; i++){
		TheSubArrays[Type][i]->sort([](TAnimal* A1, TAnimal*A2) { return A1->Supply_m_Location_x() < A2->Supply_m_Location_x(); } );
	}
}

//-----------------------------------------------------------------------------

/**
Sort TheSubArrays w.r.t. the m_Location_y attribute
*/
void Population_Manager::SortY(unsigned Type) {
	int temp_thread_num = omp_get_max_threads();
	for(int i=0; i<temp_thread_num; i++){
		TheSubArrays[Type][i]->sort([](TAnimal* A1, TAnimal*A2) { return A1->Supply_m_Location_y() < A2->Supply_m_Location_y(); } );
	}
}

//-----------------------------------------------------------------------------

/**
Sort TheSubArrays w.r.t. the current state attribute
*/
void Population_Manager::SortState(unsigned Type) {
	int temp_thread_num = omp_get_max_threads();
	for(int i=0; i<temp_thread_num; i++){
		TheSubArrays[Type][i]->sort([](TAnimal* A1, TAnimal*A2) { return A1->WhatState() < A2->WhatState(); } );
	}
}

//-----------------------------------------------------------------------------

/**
Sort TheSubArrays w.r.t. the current state attribute in reverse order
*/
void Population_Manager::SortStateR(unsigned Type) {
	int temp_thread_num = omp_get_max_threads();
	for(int i=0; i<temp_thread_num; i++){
		TheSubArrays[Type][i]->sort([](TAnimal* A1, TAnimal*A2) { return A1->GetCurrentStateNo() > A2->GetCurrentStateNo(); } );
	}
}

/**
Delete the animals in the array
*/
unsigned Population_Manager::PartitionLiveDead(unsigned Type) {
	m_LiveArraySize[Type] = 0;
	for (int j=0; j < TheSubArrays[Type].size(); j++){
		//new ones were created, add them to the array
		TheSubArraysSizes[Type][j] = 0;
		auto prev_it = TheSubArrays[Type][j]->before_begin();
		for (auto it = TheSubArrays[Type][j]->begin(); it != TheSubArrays[Type][j]->end();){
			//still alive
			if((*it)->GetCurrentStateNo()>=0){
				m_LiveArraySize[Type]++;
				TheSubArraysSizes[Type][j] += 1;
				prev_it = it;
				++it; //only increment if not deleted
			}
			else{
				delete *it;
				it = TheSubArrays[Type][j]->erase_after(prev_it);
			}
		}
	}

	//let's check if there is thread without any live animals, then we move some to that thread from another thread
	for(int i=0; i<TheSubArrays[Type].size(); i++){
		if(TheSubArraysSizes[Type][i]==0){
			//find another thread with more than one animal
			for(int j=0; j<TheSubArrays[Type].size(); j++){
				if(TheSubArraysSizes[Type][j]>1){
					int temp_moved_size = TheSubArraysSizes[Type][j]/2;
					for(int k=0; k<=temp_moved_size; k++){
						TheSubArrays[Type][i]->push_front(TheSubArrays[Type][j]->front());
						TheSubArrays[Type][j]->pop_front();
						TheSubArraysSizes[Type][i] += 1;
						TheSubArraysSizes[Type][j] -= 1;
					}
					break;
				}
			}
		}	
	}
	return m_LiveArraySize[Type];
}


//-----------------------------------------------------------------------------

/**
	Run once through the list swapping randomly chosen elements
*/
void Population_Manager::Shuffle(unsigned Type) {
	int temp_thread_num = omp_get_max_threads();
	for(int i=0; i<temp_thread_num; i++){
		TheSubArrays[Type][i]->sort([](TAnimal* a, TAnimal*b) { return g_rand_uni_fnc()>=0.5; } );
	}
}


//-----------------------------------------------------------------------------

/**
This method is used to determine whether the array of animals should be shuffled or sorted. \n
To do nothing ensure that the BeforeStepActions[] is set appropriately // 0 = Shuffle, 1 = SortX, 2 = SortY, 3 = sortXIndex, 4 = do nothing
*/
void Population_Manager::Shuffle_or_Sort(unsigned Type) {
	switch (BeforeStepActions[Type])
	{
	case 0:
		Shuffle(Type);
		break;
	case 1:
		SortX(Type);
		break;
	case 2:
		SortY(Type);
		break;
	case 3: // Do nothing
		break;
	case 4:
		if (g_rand_uni_fnc() < static_cast<double>(1. / 500)) Shuffle(Type);
		break;
	default:
		m_TheLandscape->Warn("Population_Manager::Shuffle_or_Sort", "- BeforeStepAction Unknown");
		exit(1);
	}
}

//-----------------------------------------------------------------------------
/**
Is it the first day of the month?
*/
bool Population_Manager::BeginningOfMonth() {
	if (m_TheLandscape->SupplyDayInMonth() == cfg_DayInMonth.value()) return true;
	return false;
}

//------------------------------------------------------------------------------

void Population_Manager::Catastrophe() {
	/**
	This method MUST be overidden in descendent classes if this functionality is
	does not match with the animals requirements
	*/
}
//---------------------------------------------------------------------------


char* Population_Manager::ProbeReport(int Time) {
	char str[100]; // 100 out to be enough!!
	strcpy(g_Str, "");
	for (int ProbeNo = 0; ProbeNo < m_NoProbes; ProbeNo++)
	{
		int No = 0;
		// See if we need to record/update this one
		// if time/months/years ==0 or every time
		if (TheProbe[ProbeNo]->m_ReportInterval == 3 || (TheProbe[ProbeNo]->m_ReportInterval == 2 && BeginningOfMonth())
			|| (TheProbe[ProbeNo]->m_ReportInterval == 1 && Time % 365 == 0))
		{
			// Goes through each area and sends a value to OutputForm
			unsigned Index = SupplyListIndexSize();
			for (unsigned listindex = 0; listindex < Index; listindex++)
			{
				if (TheProbe[ProbeNo]->m_TargetTypes[listindex]) No += static_cast<int>(Probe(
					listindex, TheProbe[ProbeNo]));
			}
			TheProbe[ProbeNo]->FileOutput(No, Time, ProbeNo);
			sprintf(str, " %d ", No);
			strcat(g_Str, str);
		}
	}
	return g_Str;
}
//-----------------------------------------------------------------------------

char* Population_Manager::ProbeReportTimed(int Time) {
	char str[100]; // 100 ought to be enough!!
	strcpy(g_Str, "");
	for (int ProbeNo = 0; ProbeNo < m_NoProbes; ProbeNo++)
	{
		int No = 0;
		unsigned Index = SupplyListIndexSize();
		for (unsigned listindex = 0; listindex < Index; listindex++)
		{
			if (TheProbe[ProbeNo]->m_TargetTypes[listindex]) No += static_cast<int>(
				Probe(listindex, TheProbe[ProbeNo]));
		}
		TheProbe[ProbeNo]->FileOutput(No, Time, ProbeNo);
		sprintf(str, " %d ", No);
		strcat(g_Str, str);
	}
	return g_Str;
}
//-----------------------------------------------------------------------------

/**
Special probe
*/
void Population_Manager::ImpactProbeReport(int a_Time) {
	for (int ProbeNo = 0; ProbeNo < 1; ProbeNo++)
	{
		// See if we need to record/update this one
		// if time/months/years ==0 or every time
		if (TheProbe[ProbeNo]->m_ReportInterval == 3 || (TheProbe[ProbeNo]->m_ReportInterval == 2 && BeginningOfMonth())
			|| (TheProbe[ProbeNo]->m_ReportInterval == 1 && a_Time % 365 == 0)) { ImpactedProbe(); }
	}
}
//-----------------------------------------------------------------------------


/**
Provides the location of an animal in terms of x,y,elementtype and vegetation type
*/
AnimalPosition TAnimal::SupplyPosition() const {
	AnimalPosition AnP;
	AnP.m_x = m_Location_x;
	AnP.m_y = m_Location_y;
	AnP.m_EleType = m_OurLandscape->SupplyElementType(m_Location_x, m_Location_y);
	AnP.m_VegType = m_OurLandscape->SupplyVegType(m_Location_x, m_Location_y);
	return AnP;
}

//-----------------------------------------------------------------------------

/**
Provides the farmer reference for the location of a TAnimal
*/
unsigned TAnimal::SupplyFarmOwnerRef() const { return m_OurLandscape->SupplyFarmOwner(m_Location_x, m_Location_y); }
//-----------------------------------------------------------------------------
/**

TAnimal Constructor
*/
TAnimal::TAnimal(int x, int y, Landscape* L) {
	m_OurLandscape = L;
	m_Location_x = x;
	m_Location_y = y;
	m_guard_cell_x = m_Location_x/cfg_MapGuardCellSize.value();
	m_guard_cell_y = m_Location_y/cfg_MapGuardCellSize.value();
}
//-----------------------------------------------------------------------------
/**

TAnimal Constructor2
*/
TAnimal::TAnimal(int x, int y) {
	m_Location_x = x;
	m_Location_y = y;
	m_guard_cell_x = m_Location_x/cfg_MapGuardCellSize.value();
	m_guard_cell_y = m_Location_y/cfg_MapGuardCellSize.value();
}
//-----------------------------------------------------------------------------

/**
TALMaSSObject Constructor
*/
TALMaSSObject::TALMaSSObject() {
	m_StepDone = false;
	m_CurrentStateNo = 0;
}

//-----------------------------------------------------------------------------

/**
Checks to see if there has been a management event at the TAnimals' x,y location. If so calls an event handler to handle the management event.
*/
void TAnimal::CheckManagement(void) {
	FarmToDo event;
	int i = 0;
	while ((event = static_cast<FarmToDo>(m_OurLandscape->SupplyLastTreatment(m_Location_x, m_Location_y, &i))) !=
		sleep_all_day) { if (OnFarmEvent(event)) break; }
}

//-----------------------------------------------------------------------------

/**
Checks to see if there has been a management event at the x,y location. If so calls an event handler to handle the management event.
*/
void TAnimal::CheckManagementXY(int a_x, int a_y) {
	FarmToDo event;
	int i = 0;
	while ((event = static_cast<FarmToDo>(m_OurLandscape->SupplyLastTreatment(a_x, a_y, &i))) != sleep_all_day)
	{
		OnFarmEvent(event);
	}
}

//-----------------------------------------------------------------------------

void TALMaSSObject::OnArrayBoundsError() { exit(1); }

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//           probe_data
//-----------------------------------------------------------------------------

/**
Basic output function of the default probe data file. \n
This just counts numbers in specified areas
*/
void Probe_Data::FileOutput(int No, int time, int ProbeNo) const {
	if (m_FileRecord)
	{
		if (ProbeNo == 0)
		{
			// First probe so write the time and a new line
			*m_MyFile << endl;
			*m_MyFile << time << '\t' << No;
		}
		else *m_MyFile << '\t' << No;
	}
	(*m_MyFile).flush();
}
//-----------------------------------------------------------------------------

void Probe_Data::FileAppendOutput(int No, int time) const {
	m_MyFile->open(m_MyFileName, ios::app);
	if (!m_MyFile->is_open())
	{
		g_msg->Warn(static_cast<MapErrorState>(0), "Cannot open file for append: ", m_MyFileName);
		exit(0);
	}
	if (m_FileRecord) { *m_MyFile << time << '\t' << No << endl; }
	m_MyFile->close();
}
//-----------------------------------------------------------------------------

/**
Constructor for probe_data
*/
Probe_Data::Probe_Data(): m_MyFile(nullptr), m_Time(0), m_MyFileName{}, m_FileRecord(false),
                          m_FullLandscapeProbe(false), m_ReportInterval(0), m_NoAreas(0), m_Rect{}, m_NoEleTypes(0),
                          m_NoVegTypes(0), m_NoFarms(0), m_RefVeg{}, m_RefEle{}, m_RefFarms{}, m_TargetTypes{} {
}

//-----------------------------------------------------------------------------

/**
Opens the default probe data output file
*/
ofstream* Probe_Data::OpenFile(const string& a_Nme) {
	m_MyFile = new ofstream(a_Nme);
	if (!m_MyFile->is_open())
	{
		g_msg->Warn(static_cast<MapErrorState>(0), "probe_data::OpenFile - Cannot open file: ", a_Nme);
		exit(0);
	}
	return m_MyFile;
}
//-----------------------------------------------------------------------------

/**
Sets the filename for the default probe data output
*/
void Probe_Data::SetFile(ofstream* F) { m_MyFile = F; }

//-----------------------------------------------------------------------------

void Population_Manager::PushIndividual(const unsigned a_listindex, TAnimal* a_individual_ptr) {
	int temp_thread_num = omp_get_thread_num();
	TheSubArrays[a_listindex][temp_thread_num]->push_front(a_individual_ptr);
}
//-----------------------------------------------------------------------------

int PopulationManagerList::SupplyActivePopulationsCount() {
	return std::accumulate(m_populationarray.begin(), m_populationarray.end(), 0);
}
//-----------------------------------------------------------------------------
