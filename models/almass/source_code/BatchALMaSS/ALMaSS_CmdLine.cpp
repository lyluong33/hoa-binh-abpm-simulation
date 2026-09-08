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


using namespace std;

#include "assert.h"
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cmath>
#include <vector>
#include <list>
#include <string>
#include <chrono>
#pragma warning( push )
#pragma warning( disable : 4100)
#pragma warning( disable : 4127)
#pragma warning( disable : 4244)
#pragma warning( disable : 4267)
#pragma warning( pop ) 
#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../ALMaSSDefines.h"
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../BatchALMaSS/CurveClasses.h"
#include "../SubPopulation/SubPopulation.h"
#include "../SubPopulation/SubPopulation_Population_Manager.h"
#include "../Theoretical/Theoretical1.h"
#include "../Theoretical/Theoretical1_Population_Manager.h"
#include "../Theoretical/Theoretical2.h"
#include "../Theoretical/Theoretical2_Population_Manager.h"
#include "../Skylark/SkylarkAll.h"
#include "../Landscape/habitatmap.h"

#define _CRT_SECURE_NO_DEPRECATE


const double g_randmaxp = RAND_MAX + 1.0;

static CfgInt cfg_DayInMonth( "PRB_DAYINMONTH", CFG_CUSTOM, 1 );
static CfgInt cfg_DisturbersNo("DISTURBERS_NO", CFG_CUSTOM, 1, 0, 1000);
static CfgBool cfg_UseEasyPop( "VOLE_USEEASYPOP", CFG_CUSTOM, false );
static CfgBool cfg_VoleCatastrophe_on( "VOLE_CATASTROPHE_ON", CFG_CUSTOM, false );
static CfgInt cfg_VoleCatastrophe_interval( "VOLE_CATASTROPHE_I", CFG_CUSTOM, 365 * 5 );
static CfgInt cfg_VoleCatastrophe_mortality( "VOLE_CATASTROPHE_M", CFG_CUSTOM, 90 );
CfgStr cfg_IniFileName( "INI_FILE", CFG_CUSTOM, "BatchALMaSS.ini"  );
CfgStr cfg_AORProbeName("AOR_PROBE_NAME", CFG_CUSTOM, "AOR_Probe.txt");
CfgStr cfg_ProbeName("PROBE_NAME", CFG_CUSTOM, "Probe.res");
CfgBool cfg_OldProbeNames("OLDSTYLEPROBENAMES", CFG_CUSTOM, true);// if true will write Prob.res and AOR_Probe as before, if false-- adds the species name
extern CfgBool cfg_dumpvegjan;
extern CfgStr cfg_dumpvegjanfile;
extern CfgBool cfg_dumpvegjune;
extern CfgStr cfg_dumpvegjunefile;
static CfgInt cfg_tracelevel("G_TRACELEVEL", CFG_CUSTOM, 4);
extern CfgBool cfg_AorOutput_used;

extern PopulationManagerList g_PopulationManagerList;
extern CfgInt cfg_LadybirdAphidPreySpecies;
extern CfgStr cfg_AphidSpeciesName;
extern CfgInt cfg_LadybirdAphidPreyIndex;

//---------------------------------------------------------------------------

void RunTheSim();
void CloseDownSim();
bool ReadBatchINI(const char *filename);
void SetOutputFiles();
void GetProbeInput_ini(std::shared_ptr<Population_Manager_Base>, std::string filename);
bool CreatePopulationManager();
Landscape* CreateLandscape();

//-----------------------------------------------------------------------------

// Globals
Landscape* g_ALandscape;
std::shared_ptr<Population_Manager_Base> g_AManager;
int g_torun;
char* g_files[100];
char* g_Predfiles[100];
char g_ResultsDir[255];
char g_PredResultsDir[255];
int g_time;
int g_Year;
int g_Steps;
int g_NoProbes;
unsigned g_NoOfPredProbes;
PopulationManagerList g_PopulationManagerList;
ALMaSS_MathFuncs g_AlmassMathFuncs;

extern TTypesOfPopulation g_Species;

	//-----------------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

// int boost_random010000(int /* a_range */) {
    //unsigned result=*g_chance010000++;
	//return result;
// }
//------------------------------------------------------------------------------

void delay(int secs) {
	time_t start_time, cur_time;
	time(&start_time);
	do
	{
	   time(&cur_time);
	 } while((cur_time - start_time) < secs);
}
//------------------------------------------------------------------------------

/*
#if (defined __UNIX) | ( defined __BORLANDC__)
void FloatToDouble(double &d, float f) {
	d = f;
}
#else
void FloatToDouble(double &d, float f) {
    char * num = 0;
    num = new char[_CVTBUFSIZE];
    errno_t err = _gcvt_s(num, _CVTBUFSIZE,f,8);
    if (err!=0) {
      assert(0);
    }
    d = atof(num);
	delete [] num;
}
#endif
*/
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	auto start_clock = std::chrono::high_resolution_clock::now();
#ifdef _DEBUG
	// Get the current state of the flag
	// and store it in a temporary variable
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

	// Turn On (OR) - Keep freed memory blocks in the
	// heap's linked list and mark them as freed
	tmpFlag |= _CRTDBG_DELAY_FREE_MEM_DF;

	// Turn Off (AND) - prevent _CrtCheckMemory from
	// being called at every allocation request
	tmpFlag &= ~_CRTDBG_CHECK_ALWAYS_DF;
	// Set the new state for the flag
	_CrtSetDbgFlag( tmpFlag );
#endif
	
	const char* ConfigurationFile;
	std::string ErrorFileName;
	switch (argc)
	{
		case 3:
			cout << "Running with config file: " << argv[1] << "\n";
			ConfigurationFile = argv[1];
			cout << "Error file: " << argv[2] << "\n";
			ErrorFileName = argv[2];
			break;
		case 2:
			cout << "Running with config file: " << argv[1] << "\n";
			ConfigurationFile = argv[1];
			ErrorFileName = "ErrorFile.txt";
			break;
		default:
			ConfigurationFile = "TIALMaSSConfig.cfg";
			ErrorFileName = "ErrorFile.txt";
			break;

	}

	// Must come first. Used by the configurator below.
	g_msg = CreateErrorMsg();
	g_msg->SetFilename(ErrorFileName);
	g_msg->SetWarnLevel(WARN_MSG);
	g_cfg->ReadSymbols(ConfigurationFile);
	// Configurator instantiation is automatic.
	g_cfg = CreateConfigurator();

	//g_cfg->DumpAllSymbolsAndExit( "allsymbols.cfg" );
	// Read the BatchALMaSS.ini to be sure g_Species is set before its needed
	if (!ReadBatchINI(cfg_IniFileName.value())) {
		char ch;
		cout << "Problem with ini file";
		cin >> ch;
	}

	g_init_random_seed_fnc();

	g_date = CreateCalendar();
	cout << "Created Calendar Object" << endl;

	g_weather = CreateWeather();
	cout << "Created Weather Object " << endl;

	g_letype =  CreateLETypeClass();
	cout << "Created LE_TypeClass Object" << endl;

	g_crops = CreatePlantGrowthData();
	cout << "Created PlantGrowthData Object " << endl;

	cout << "Creating Landscape Object ..." << endl;
	g_ALandscape = new Landscape();
	cout << "Landscape Object Done" << endl;

	/* Now latitude and longitude given, or using DK as default. Calculate daylength */
	g_date->CreateDaylength(g_ALandscape->SupplyLatitude(), g_ALandscape->SupplyLongitude(), g_ALandscape->SupplyTimezone() );


	g_ALandscape->RunHiddenYear();

	g_ALandscape->SetThePopManagerList(&g_PopulationManagerList);
	if (!CreatePopulationManager())
		return false;
	else
		if (g_Species != -1)
		{
			cout << "Population Manager Created" << endl; // Now got to get the probe files read in
		}
		/* Landscape module is still operating with raw pointers (therefore we give the reference to g_PopulationManagerList):
		* this will be changed in the next stage of operation
		* However the pointers inside the g_PopulationManagerList are smart, this is not a problem, since all reference operations on the
		* smart pointers are the same as on the raw ones
		* */

	
	SetOutputFiles();
	// Ready to go
	RunTheSim();
	CloseDownSim();	
	auto stop_clock = std::chrono::high_resolution_clock::now();
    cout << std::chrono::duration_cast<std::chrono::seconds>(stop_clock - start_clock).count()<< " seconds" << endl;
	return 0;
}
//------------------------------------------------------------------------------

bool CreatePopulationManager() {
  // SET UP THE ANIMAL POPULATION
  // THE LANDSCAPE MUST BE SETUP BEFORE THE CALL HERE

  // Create the Population Manager
  if ( g_Species == TOP_NoSpecies ) {
        g_AManager = std::make_shared<None_Population_Manager>(g_ALandscape);
		// Do not set in g_PopulationManagerList
  }
  else if (g_Species == TOP_Theoretical_1)
  {
    g_AManager = std::make_shared<Theoretical1_Population_Manager>(g_ALandscape);
    g_PopulationManagerList.SetPopulation(g_AManager, TOP_Theoretical_1);
  }
  else if (g_Species == TOP_Theoretical_2){
	g_AManager = std::make_shared<Theoretical2_Population_Manager>(g_ALandscape);
    g_PopulationManagerList.SetPopulation(g_AManager, TOP_Theoretical_1);
  }
  else if (g_Species == TOP_Theoretical_3){
	g_AManager = std::make_shared<SubPopulation_Population_Manager>(g_ALandscape);
	g_PopulationManagerList.SetPopulation(g_AManager, TOP_Theoretical_3);
  }
  else if ( g_Species == TOP_Skylark ) {
	g_AManager = std::make_shared<Skylark_Population_Manager>(g_ALandscape);
	g_PopulationManagerList.SetPopulation(g_AManager, TOP_Skylark);
	std::static_pointer_cast<Skylark_Population_Manager>(g_AManager)->OpenTheBreedingPairsProbe();
	std::static_pointer_cast<Skylark_Population_Manager>(g_AManager)->OpenTheBreedingSuccessProbe();
	std::static_pointer_cast<Skylark_Population_Manager>(g_AManager)->OpenTheFledgelingProbe();
  }
  else {
	  g_msg->Warn("Population_Manager::CreatePopulationManager()  Unknown species number> ", int(g_Species));
	  exit(0);
  }

  g_AManager->SetNoProbesAndSpeciesSpecificFunctions(g_NoProbes);
  return true;
}
//---------------------
void SetOutputFiles(){
    if (cfg_OldProbeNames.value()){
        // Use the regular naming of the Probe.res and AOR_Probe (from the config)
        // If there is no PM set there is nothing to report so we only need to cover the case of the only PM
        for (int i = 0; i<TOP_foobar; i++){
            auto PM = g_PopulationManagerList.GetPopulation_smart(i);
            if (PM!= nullptr){
                if (cfg_AorOutput_used.value()) {
                    PM->OpenTheAOROutputProbe(cfg_AORProbeName.value());
                }
                GetProbeInput_ini(PM, cfg_ProbeName.value());
            }
        }
    }
    else{
    // We create the probes with naming format of simulationname_Probe.res and simulationname_AOR_Probe.txt
        for (int i = 0; i<TOP_foobar; i++){
            auto PM = g_PopulationManagerList.GetPopulation_smart(i);
            if (PM!= nullptr){
                if (cfg_AorOutput_used.value()) {
                    PM->OpenTheAOROutputProbe(PM->SupplySimulationName() + "_" + cfg_AORProbeName.value());
                }
                GetProbeInput_ini(PM, PM->SupplySimulationName()+"_"+cfg_ProbeName.value());
            }

        }
    }
}
//------------------------------------------------------------------------------
void GetProbeInput_ini(std::shared_ptr<Population_Manager_Base> p_PM, std::string ProbeName) {
    ofstream * AFile=NULL;

    string NoProbes_String;


    NoProbes_String.append(g_ResultsDir);

    for ( int NProbes = 0; NProbes < g_NoProbes; NProbes++ ) {
        // Must read the probe from a file
        p_PM->TheProbe[ NProbes ] = new Probe_Data;
        p_PM->ProbeFileInput( ( char * ) g_files[ NProbes ], NProbes );


        NoProbes_String.append(ProbeName);
        if ( NProbes == 0 ) {
            AFile = p_PM->TheProbe[ NProbes ]->OpenFile( NoProbes_String );
            if ( !AFile ) {
                g_msg->Warn( "BatchALMSS - cannot open Probe File", "" );
                exit( 1 );
            }
        } else
            p_PM->TheProbe[ NProbes ]->SetFile( AFile );
    }
}

//-----------------------------------------------------------------------------
bool ReadBatchINI(const char* filename) {
  // Must read the TIBatch.INI
  // Read the INI file
  FILE * Fi = NULL;
  char answer = 'X';
  std::cout<<"Batch INI file: "<<filename<<endl;
  Fi=fopen(filename, "r" );
  while ( Fi==NULL ) {
      // Issue and error warning
      cout << "INI File Missing: ";
      cout << filename << endl;
	  cout << "Try Again?";
      cin >> answer;
    if ( answer != 'Y' ) exit(0);
	Fi = fopen(filename, "r" );
  }
  char Data[ 255 ];
  fscanf( Fi, "%d\r", & g_NoProbes );
  for ( int i = 0; i < g_NoProbes; i++ ) {
    fscanf( Fi, "%s\r", Data);
    g_files[ i ] = new char[ 255 ];
    strcpy( g_files[ i ], Data );
  }
  fscanf( Fi, "%s\r", Data );
  strcpy( g_ResultsDir, Data );
  fscanf( Fi, "%d\r", & g_torun );
  g_torun *= 365; // the number of years and multiplied by 365 to get days
  fscanf( Fi, "%d\r", & g_Species );
  fclose( Fi );
  switch (g_Species) {
  case TOP_NoSpecies:
	  cout << "Running Landscape only,  animal species not selected" << endl;
	  break;
  case TOP_Theoretical_1:
	  cout << "Running Theoretical_1" << endl;
	  break;
  case TOP_Theoretical_2:
	  cout << "Running Theoretical_2" << endl;
	  break;
  case TOP_Theoretical_3:
	  cout << "Running Theoretical_3" << endl;
	  break;
  case TOP_Skylark:
	  cout << "Running Skylark" << endl;
	  break;
  }
  return true;
}

//-----------------------------------------------------------------------------
void CloseDownSim() {
	if (g_AManager)
	{
		g_ALandscape->SimulationClosingActions();
		// Close the probe file
		if (g_AManager->TheProbe[0] != NULL) g_AManager->TheProbe[0]->CloseFile();
		// delete all probes
		for (int i = 0; i < g_NoProbes; i++) delete g_AManager->TheProbe[i];
	}
	/*
	 * the following delete statements will all be removed once we move to smart pointers entirely
	 * For now, we can remove the tests, since removing a null pointer has no effect
	 * */

	delete g_ALandscape;
	delete g_msg;
	delete g_crops;
	delete g_letype;
	delete g_weather;
	delete g_date;


    for ( int NProbes = 0; NProbes < g_NoProbes; NProbes++ ) {
        delete [] g_files[NProbes];
    }
    for ( int NProbes = 0; NProbes < ( int )g_NoOfPredProbes; NProbes++ ) {
        delete [] g_Predfiles[NProbes];
    }
}

//------------------------------------------------------------------------------

void RunTheSim()
{
	HabitatMap mapTosave;
	for (int i = 0; i < g_torun; i++)
	{
		g_ALandscape->TurnTheWorld();
		// Update the Date
		g_time++;
		int day = g_ALandscape->SupplyDayInMonth();
		int month = g_ALandscape->SupplyMonth();
		


		//save images of the landscape
		if(day==15){
			std::string month_name = g_ALandscape->SupplyMonthName();
			int year = g_ALandscape->SupplyYear();
			mapTosave.SaveAllMaps(month_name+"_"+std::to_string(year));
		}
	
		if ((day == 1) && (month == 1)){
             g_Year++;
			 cout << "Year; " << g_Year << '\n';
		}
        cout << g_time << '\n';

		for (int p = 0; p < TOP_foobar; p++){
			int popu_num = g_PopulationManagerList.GetPopulationManagerNum(p);
			if (popu_num < 1) continue;
			for (int i = 0; i < popu_num; i++){
				if (g_PopulationManagerList.GetPopulation(p, i) != nullptr){
					g_PopulationManagerList.GetPopulation(p, i)->Run(1);
				}
			}
		}
    
        //for 1 day-time step, always 0
        g_AManager->SetStepCounterInADay(0);
		if (g_Species == TOP_Skylark ){
		    char str[255];
		    strcpy(str, g_AManager->SpeciesSpecificReporting(g_Species, g_time));
        }
		g_ALandscape->DumpVegAreaData(g_time);
	}
}
//---------------------------------------------------------------------------

Landscape* CreateLandscape() {
    g_ALandscape = new Landscape();
	return g_ALandscape;
}
//---------------------------------------------------------------------------
