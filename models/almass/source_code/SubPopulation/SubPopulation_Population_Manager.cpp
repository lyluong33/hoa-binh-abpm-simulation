/*
*******************************************************************************************************
Copyright (c) 2021, Xiaodong Duan, Aarhus University
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
/** \file SubPopulationPopulationManager.cpp
\brief <B>The main source code for subpopulation population manager class</B>
*/
/**  \file SubPopulationPopulationManager.cpp
Version of  Dec. 2023 \n
By Xiaodong Duan \n \n
*/

//---------------------------------------------------------------------------

#include <string.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include "math.h"
#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../ALMaSSDefines.h"
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../SubPopulation/SubPopulation.h"
#include "../SubPopulation/SubPopulation_Population_Manager.h"

using namespace std;

CfgBool cfg_Subpopu_Base_Output_Used("SUBPOPU_BASE_OUTPUT_USED", CFG_CUSTOM, true);
CfgStr cfg_Subpopu_Base_Output_Filename( "SUBPOPU_BASE_OUTPUT_FILENAME", CFG_CUSTOM, "SubpopuBaseOutput.txt" );
CfgFloat cfg_Subpopu_Density_Threshold("SUBPOPU_DENSITY_THRESHOLD", CFG_CUSTOM, 500);
extern Landscape* g_landscape_ptr;
extern TTypesOfPopulation g_Species;
extern FarmManager* g_farmmanager;
//---------------------------------------------------------------------------

//#define SubpopulationSpatialResult

SubPopulation_Population_Manager::SubPopulation_Population_Manager(Landscape* L, string DevReproFile, int a_sub_w, int a_sub_h, int a_num_life_stage, int a_max_long_dist, int a_peak_long_dist, float a_scale_wind_speed, float a_max_wind_speed, int a_wind_direc_num, int a_wind_speed_step_size, int a_max_alive_day, bool a_open_output_file_flag) : Population_Manager_Base(L)
{
	//set the number of life stages
	m_num_life_stage = a_num_life_stage;
	m_ListNameLength = m_num_life_stage;

	//set the number of longest life time in calendar days
	m_max_alive_days = a_max_alive_day;

	//set the width and height of the grid
	m_sub_w = a_sub_w;
	m_sub_h = a_sub_h;


	//eggs start accumulate degree days from the first day of the year
	m_egg_start_dd_jan1st = true;
	SubPopulation::setFlagEggsDDJan1(m_egg_start_dd_jan1st);
	SubPopulation::setCellSize(m_sub_w*m_sub_h);

	m_egg_hatch_chance = 0;

	//create the array to store the base development temperature for each life stage
	m_lowest_temp_dev.resize(m_num_life_stage);
	m_turning_temp_dev.resize(m_num_life_stage);
	m_weight_dd_lower_turning_temp_dev.resize(m_num_life_stage);
	m_turning_temp_dev_used_flag = false;
	//create the vector to store the indix for the old enough ones for next life stage or death
	m_index_next_life_stage.resize(m_num_life_stage);
	/*
	for (int i=0; i<m_num_life_stage; i++){
		m_index_next_life_stage.at(i).push_back(-1);
	}
	*/
	
	//flying related parameters
	//create the movement masks
	//m_current_flying_array.resize(m_max_alive_days);
	//m_current_flying_array.fill(0.0);
	//m_current_landing_array.resize(m_max_alive_days);
	//m_current_landing_array.fill(0.0);
	m_max_long_distance = a_max_long_dist;
	m_peak_long_distance = a_peak_long_dist;
	m_scale_wind_speed = a_scale_wind_speed;
	m_max_flying_wind_speed = a_max_wind_speed;
	//the default wind direction number is 8
	m_wind_direction_num = a_wind_direc_num;
	m_wind_speed_step_size = a_wind_speed_step_size;
	//get the sampling number for the wind speed with the given the step size
	m_wind_speed_num = ceil(m_max_flying_wind_speed / m_wind_speed_step_size);

	//resize the landing mask array
	m_landing_masks.resize(m_wind_speed_num);
	for (int i=0; i<m_wind_speed_num; i++){
		m_landing_masks[i].resize(m_wind_direction_num);
	}

	//initialise the wind direction array, the first column is the x component, the second column is the y component
	m_wind_direction_array.resize(m_wind_direction_num, 2);
	double temp_degree = 2*M_PI/m_wind_direction_num;
	for (int i=0; i<m_wind_direction_num; i++){
		double temp_value = cos(temp_degree*(m_wind_direction_num-1-i));
		m_wind_direction_array(i,0) = temp_value;
		temp_value = sin(temp_degree*(m_wind_direction_num-1-i));
		m_wind_direction_array(i,1) = temp_value;
	}

	//cout<<m_wind_direction_array<<endl;

	//convert the longest distance to the number of cells
	//we need to make it with the largest distance multiplied by the incremental factor
	m_long_move_mask_x_num_half_array.resize(m_wind_speed_num);
	m_long_move_mask_x_num_quarter_array.resize(m_wind_speed_num);
	m_long_move_mask_y_num_half_array.resize(m_wind_speed_num);
	m_long_move_mask_y_num_quarter_array.resize(m_wind_speed_num);
	for (int i=0; i<m_wind_speed_num;i++){
		m_long_move_mask_x_num_half_array(i) = m_max_long_distance*pow(a_scale_wind_speed, i)/m_sub_w;
		m_long_move_mask_x_num_quarter_array(i) = m_long_move_mask_x_num_half_array(i)/4;
		m_long_move_mask_y_num_half_array(i) = m_max_long_distance*pow(a_scale_wind_speed, i)/m_sub_h;
		m_long_move_mask_y_num_quarter_array(i) = m_long_move_mask_y_num_half_array(i)/4;
	}

	//initialise the landing mask arrays
	for (int i=0; i<m_wind_speed_num;i++){
		//because we don't allow the subpopulations go against the wind, we only need to create the half sized mask depending on the wind direction
		//zero is North wind, clockwise
		m_landing_masks[i][0].resize(m_long_move_mask_y_num_half_array(i), 2*m_long_move_mask_x_num_quarter_array(i)+1);
		//2 is South wind
		m_landing_masks[i][2].resize(m_long_move_mask_y_num_half_array(i), 2*m_long_move_mask_x_num_quarter_array(i)+1);
		//1 is East wind
		m_landing_masks[i][1].resize(2*m_long_move_mask_y_num_quarter_array(i)+1, m_long_move_mask_x_num_half_array(i));
		//3 is West wind
		m_landing_masks[i][3].resize(2*m_long_move_mask_y_num_quarter_array(i)+1, m_long_move_mask_x_num_half_array(i));

		for (int j=0; j<m_wind_direction_num; j++){
			m_landing_masks[i][j].fill(0.0);
			//cout<<m_landing_masks[i][j]<<endl;
		}
	}


	double temp_longest_factored = m_max_long_distance*pow(a_scale_wind_speed, m_wind_speed_num-1);
	m_long_move_mask_x_num_half = temp_longest_factored/m_sub_w;
	m_long_move_mask_x_num = 2*m_long_move_mask_x_num_half +1;
	m_long_move_mask_y_num_half = temp_longest_factored/m_sub_h;
	m_long_move_mask_y_num = 2*m_long_move_mask_y_num_half+1;
	calLongMovementMask();

	m_landing_survival_rate = 1.0;

	if(DevReproFile==""){
		readDevReproFile("Subpopulation/subpopulation_default_info.txt");
	}
	else if(DevReproFile == "-1"){
		//do nothing
	}
	else{
		readDevReproFile(DevReproFile);
	}
		
	// Initialize the default population size to zero
	m_total_num_each_stage.resize(m_num_life_stage);
	m_total_num_each_stage.setZero();
	m_total_num_each_stage_field.resize(m_num_life_stage);
	m_total_num_each_stage_field.setZero();

	//set the number of cells
	m_num_x_range = SimW/m_sub_w;
	m_num_y_range = SimH/m_sub_h;
	m_size_cell = m_sub_w*m_sub_h;

	//Initialize the weighted population density array with zero
	m_cell_popu_density.resize(m_num_y_range, m_num_x_range);
	m_cell_popu_density.fill(0.0);

	//Initialize the suitability for each cell with zero
	m_cell_suitability.resize(m_num_y_range, m_num_x_range);
	m_cell_suitability.fill(0.0);

	//Initialize the pointer array for the subpopulation objects
	m_the_subpopulation_array.resize(m_num_y_range);
	for (int i=0; i<m_num_y_range; i++){
		m_the_subpopulation_array[i].resize(m_num_x_range);
		for (int j=0; j<m_num_x_range; j++){
			m_the_subpopulation_array[i][j] = NULL;
		}
	}

	m_first_flag_life_stage.resize(m_num_life_stage);
	for(int i=0; i<m_first_flag_life_stage.size(); i++){
		m_first_flag_life_stage[i] = true;
	}

	//check if we need to open the base storing file
	if (cfg_Subpopu_Base_Output_Used.value() && a_open_output_file_flag) {
        openSubpopulationBaseProbeFile(cfg_Subpopu_Base_Output_Filename.value());
    }

	m_hibernated_hatch_flag = false;

	//initialize the mortality related arrays
	m_current_mortality_array.resize(m_num_life_stage, m_max_alive_days);
	m_current_mortality_array.fill(0);
	m_lowest_temperature_die.resize(m_num_life_stage);
	m_lowest_temperature_die.fill(2);
	m_highest_temperature_die.resize(m_num_life_stage);
	m_highest_temperature_die.fill(40.0);


	//create the arrays for calculation of degree days
	m_accumu_degree_days.resize(m_num_life_stage, m_max_alive_days);
	m_accumu_degree_days.fill(0);
	m_index_new_old.resize(m_num_life_stage, 2); //the first is the newest index and the second is the oldest index
	m_index_new_old.fill(0); //They start from zero from the beginning.

	//do the initialisation
	if(DevReproFile=="") initialisePopulation();
}

SubPopulation_Population_Manager::~SubPopulation_Population_Manager(){
	for (int i=0; i<m_num_y_range; i++){
		for (int j=0; j<m_num_x_range; j++){
			if (m_the_subpopulation_array[i][j] !=  NULL){
				delete m_the_subpopulation_array[i][j];
			}
		}
	}
	
	if(cfg_Subpopu_Base_Output_Used.value() && m_subpopulation_base_prb_file){
		m_subpopulation_base_prb_file.close();
		//delete m_subpopulation_base_prb_file;
	}
}

void SubPopulation_Population_Manager::updateWholePopulationArray(int a_listindex, double number)
{	
	double temp_number = m_total_num_each_stage(a_listindex);
	temp_number += number;
	if (temp_number < 0){
		m_total_num_each_stage(a_listindex)=0;
	}
	else
	{
		m_total_num_each_stage(a_listindex)= temp_number;
	}
}

void SubPopulation_Population_Manager::updateWholePopulation(){
	for (int k=0; k<m_num_life_stage; k++){
		m_total_num_each_stage(k) = 0;
		m_total_num_each_stage_field(k) = 0;
		for (int i = 0; i < m_num_y_range; i++){
			for (int j = 0; j < m_num_x_range; j++){
				if(m_the_subpopulation_array[i][j]!=NULL){
					double temp_num = m_the_subpopulation_array[i][j]->getNumforOneLifeStage(k);
					if(temp_num>0){
						if(temp_num>1e-8){
							m_total_num_each_stage(k) += m_the_subpopulation_array[i][j]->getNumforOneLifeStage(k);
							if(m_the_subpopulation_array[i][j]->isFarm()){
								m_total_num_each_stage_field(k) += m_the_subpopulation_array[i][j]->getNumforOneLifeStage(k);
							}
						}
						else{
							m_the_subpopulation_array[i][j]->removeAnimalNumGivenStage(k);
						}
					}
					
				}
			}
		}
	}
}

/**
This is the main scheduling method for the population manager using subpopulation model.
*/
void SubPopulation_Population_Manager::Run( int NoTSteps ) {
	
  for ( int TSteps = 0; TSteps < NoTSteps; TSteps++ )
  {	
	cout<<"Day "<<m_TheLandscape->SupplyDayInYear()+1 << endl;
	//cout<<omp_get_thread_num()<<endl;
    DoFirst();
	#pragma omp parallel for collapse(2)
	for (int i = 0; i < m_num_y_range; i++){
		//parallel_for(m_num_x_range[&](int start, int end){
		for (int j = 0; j < m_num_x_range; j++){
			if(m_the_subpopulation_array[i][j]!=NULL){
				m_the_subpopulation_array[i][j]->BeginStep();
			}
		}
	}
    // call the step-method of all objects
	#pragma omp parallel for collapse(2)
	for (int i = 0; i < m_num_y_range; i++){
	//parallel_for(m_num_x_range[&](int start, int end){
		for (int j = 0; j < m_num_x_range; j++){
			if(m_the_subpopulation_array[i][j]!=NULL){
				SubPopulation* temp_pointer = m_the_subpopulation_array[i][j];
				m_the_subpopulation_array[i][j]->Step();
			}
			else {
				std::cout<<"NULL"<<endl;
			}
		}
	}
	#pragma omp parallel for collapse(2)
	for (int i = 0; i < m_num_y_range; i++){
		//parallel_for(m_num_x_range[&](int start, int end){
		for (int j = 0; j < m_num_x_range; j++){
			if(m_the_subpopulation_array[i][j]!=NULL){
				m_the_subpopulation_array[i][j]->EndStep();
			}
		}
	}
	DoLast();

	#ifdef __APHID_CALIBRATION
		writeCalibrationFiles();
	#endif

	#ifdef APHID_BIOMASS_DEBUG
	 ofstream bio_file;
	 bio_file.open("biomass.txt", ios::app );
	 
	 for (int i = 0; i<2200; i++){
		if (g_landscape_ptr->SupplyElementType(i)==tole_PermPastureTussocky){
			//bio_file<<g_landscape_ptr->SupplyVegPhase(i)<<"\t"<<g_landscape_ptr->SupplyVegBiomass(i)<<"\t"<<g_landscape_ptr->SupplyGreenBiomass(i)<<"\t";
			bio_file<<g_landscape_ptr->SupplyVegBiomass(i)<<"\t"<<g_landscape_ptr->SupplyGreenBiomass(i)<<"\n";
			break;
		}
	 }
	 
	
	 for (int i = 0; i<2600; i++){
		if (g_landscape_ptr->SupplyVegType(i)==tov_DKLegume_Beans){
			//bio_file<<g_landscape_ptr->SupplyVegPhase(i)<<"\t"<<g_landscape_ptr->SupplyVegBiomass(i)<<"\t"<<g_landscape_ptr->SupplyGreenBiomass(i)<<"\n";
			bio_file<<g_landscape_ptr->SupplyVegBiomass(i)<<"\t"<<g_landscape_ptr->SupplyGreenBiomass(i)<<"\n";
			break;
		}
	 }
	

	 bio_file.flush();
	 bio_file.close();
	#endif

  } // End of time step loop
}

double SubPopulation_Population_Manager::getTotalSubpopulationInCell(int x_indx, int y_indx){
	if(m_the_subpopulation_array[y_indx][x_indx]==NULL){
		return 0;
	}
	return m_the_subpopulation_array[y_indx][x_indx]->getTotalSubpopulation();
}

double SubPopulation_Population_Manager::getSubpopulationInCellLifeStage( int x_indx, int y_indx, int a_life_stage){
	if(m_the_subpopulation_array[y_indx][x_indx]==NULL){
		return 0;
	}
	return m_the_subpopulation_array[y_indx][x_indx]->getNumforOneLifeStage(a_life_stage);
}

SubPopulation* SubPopulation_Population_Manager::supplySubPopulationPointer(int indx, int indy){
	return m_the_subpopulation_array[indy][indx];
}

double SubPopulation_Population_Manager::supplyAllPopulationGivenStage(int index){
	if(index<m_num_life_stage){
		return m_total_num_each_stage(index);
	} 
	return 0;
}

void SubPopulation_Population_Manager::subpopuBaseOutputProbe(){
	for(int i=0; i<m_num_life_stage; i++){
	m_subpopulation_base_prb_file<<m_total_num_each_stage(i)<<"\t";
	}
	for(int i=0; i<m_num_life_stage; i++){
	m_subpopulation_base_prb_file<<m_total_num_each_stage_field(i)<<"\t";
	}
	m_subpopulation_base_prb_file<<g_landscape_ptr->SupplyDaylength()<<"\t";
	

	m_subpopulation_base_prb_file<<g_landscape_ptr->SupplyTemp();


	#ifndef SubpopulationSpatialResult
	m_subpopulation_base_prb_file<<"\n";
	#endif

	#ifdef SubpopulationSpatialResult
	//the population in each grid
	for (int index_y = 0; index_y < m_num_y_range; index_y++){
		for (int index_x = 0; index_x < m_num_x_range; index_x++){
			m_subpopulation_base_prb_file<<"\t";
			//for(int i=0; i<m_num_life_stage; i++){
				//store the population at each life stage
			//	m_subpopulation_base_prb_file<<m_the_subpopulation_array[index_y][index_x]->getNumforOneLifeStage(i)<<"\t";
			//}
			double temp_counter = 0;
			for (int i=5;i<9;i++){
				if(m_the_subpopulation_array[index_y][index_x] != NULL){
					temp_counter += m_the_subpopulation_array[index_y][index_x]->getNumforOneLifeStage(i);
				}
			}
			m_subpopulation_base_prb_file<<temp_counter;
		}
	}
	m_subpopulation_base_prb_file<<"\n";
	#endif

	m_subpopulation_base_prb_file.flush();
}

bool SubPopulation_Population_Manager::openSubpopulationBaseProbeFile(string a_file_name) {
  m_subpopulation_base_prb_file.open(a_file_name, ios::out );
  if ( !m_subpopulation_base_prb_file) {
    g_msg->Warn( WARN_FILE, "SubPopulationPopulation_Manager::openSubpopulationBaseProbeFile(): ""Unable to open probe file", a_file_name );
    exit( 1 );
  }

  #ifdef SubpopulationSpatialResult
  m_subpopulation_base_prb_file<<m_num_life_stage<<"\n";
  m_subpopulation_base_prb_file<<m_num_x_range<<"\t"<<m_num_y_range<<"\n";
  m_subpopulation_base_prb_file.flush();
  #endif

  return true;
}

unsigned SubPopulation_Population_Manager::GetLiveArraySize(int a_listindex) {
	return unsigned(supplyAllPopulationGivenStage(a_listindex));
}

double SubPopulation_Population_Manager::calHatchChance() {
	if(m_accumu_degree_days(0, 0) < m_development_degree_day(0, 0)/2) m_egg_hatch_chance = 0;
	else if(m_accumu_degree_days(0, 0) < m_development_degree_day(0, 0)) m_egg_hatch_chance = m_accumu_degree_days(0, 0)/m_development_degree_day(0, 0);
	else m_egg_hatch_chance = 1;
	return m_egg_hatch_chance;
}

void SubPopulation_Population_Manager::readDevReproFile(string inputfile){
	ifstream ist(inputfile, ios::in);
	int row_num;
    ist >> row_num;
    ist >> m_num_life_stage;
    m_development_degree_day.resize(row_num-1, m_num_life_stage);

	//load the name for life stages, although we don't use them
	for(int j=0; j<m_num_life_stage;j++){
		string temp_name;
		ist >> temp_name;
	}

	//load the development and reproducing degree days
    for (int i=0; i<row_num-1; i++){
        for(int j=0; j<m_num_life_stage;j++){
            int temp_int;
            ist >> temp_int;
            m_development_degree_day(i,j)=temp_int;
            //cout<<m_activities_priorities(i,j);
			//if(i==0){
			//	if (m_max_alive_days < temp_int){
			//		m_max_alive_days = temp_int;
			//	}
			//}
        }
    }

	//load the base development temperature
	for (int j=0; j<m_num_life_stage; j++){
		double temp_dou;
		ist >> temp_dou;
		m_lowest_temp_dev(j) = temp_dou;
	}
}

void SubPopulation_Population_Manager::relocatePopulation(){
	m_current_wind_speed = g_landscape_ptr->SupplyWind();
	//cout<<"wind speed: "<<m_current_wind_speed<<endl;
	m_current_wind_direction = g_landscape_ptr->SupplyWindDirection();
	//cout<<"wind direction: "<<m_current_wind_direction<<endl;
	m_current_wind_speed_index = int(m_current_wind_speed/m_wind_speed_step_size);
	//cout<<"wind speed index: "<<m_current_wind_speed_index<<endl;

	//land the flying subpopulations
	for(int ind_y=0; ind_y < m_num_y_range; ind_y++){
		for(int ind_x=0; ind_x <m_num_x_range; ind_x++){
			if(m_the_subpopulation_array[ind_y][ind_x] != NULL){
				for(int i=0; i<m_flying_life_stage_array.size(); i++){
					if(m_the_subpopulation_array[ind_y][ind_x]->getNumforOneLifeStage(m_flying_life_stage_array[i])>0){
						doFlying(i, ind_x, ind_y);
					}
				}
			}
		}
	}	
}

void SubPopulation_Population_Manager::DoLast(){	
	doDevelopment();
	doSpeciesLastThing();
	addNewDay();
	updateWholePopulation();
	//store results if it is needed
	if(m_subpopulation_base_prb_file){
		subpopuBaseOutputProbe();
	}	
}

void SubPopulation_Population_Manager::calLongMovementMask(){
	//We only calculate the masks for the north wind, and then we can use the rotation to get the masks for other wind directions
	//This is only for 4 directions of wind, if we want to have more directions, we need to change the code
	//The purpose of this is to speed up the calculation of flying
	//double min_cos_value = m_peak_long_distance/sqrt(pow(m_peak_long_distance,2)+pow(m_max_long_distance,2));
	for (int i=0; i<m_wind_speed_num; i++){
		for ( int x=-m_long_move_mask_x_num_quarter_array(i); x<m_long_move_mask_x_num_quarter_array(i)+1; x++){
			for (int y=-1; y>-m_long_move_mask_y_num_half_array(i)-1; y--){
				//we calculate the base mask without the consideration of the wind direction first
				double temp_mask_value = calLandingCurve(sqrt(pow(x*m_sub_w,2)+pow(y*m_sub_h,2)), m_peak_long_distance*pow(m_scale_wind_speed, i), m_max_long_distance*pow(m_scale_wind_speed, i));
				//apply the wind direction, only the south wind is calculated
				int j = 0;
				//calculate the cosine value between the direction of the cell and the wind direction
				double temp_cos = -1;
				temp_cos = (y*m_wind_direction_array(j,1)+x*m_wind_direction_array(j,0))/(sqrt(x*x+y*y)*sqrt(m_wind_direction_array(j,0)*m_wind_direction_array(j,0)+m_wind_direction_array(j,1)*m_wind_direction_array(j,1)));
				//square the cosine value to get the mask value
				//if(temp_cos<min_cos_value)
				//	temp_cos = 0;
				//temp_cos*=temp_cos;
				//temp_cos*=temp_cos;
				//if(temp_cos<0.00001)
				//	temp_cos = 0;
				m_landing_masks[i][j](-y-1, m_long_move_mask_x_num_quarter_array(i)+x) = temp_mask_value * temp_cos;
			}
		}
		//set the small values to zero
		//we use the furthest distance to calculate the threshold of small values
		double temp_min = m_landing_masks[i][0](m_long_move_mask_y_num_half_array(i)-1, m_long_move_mask_x_num_quarter_array(i));
		m_landing_masks[i][0] = (m_landing_masks[i][0].array()<temp_min).select(0, m_landing_masks[i][0]);

		//normalise the mask
		m_landing_masks[i][0].array() /= m_landing_masks[i][0].sum();

		//cout<<m_landing_masks[i][0](m_long_move_mask_y_num_half_array(i)-1, m_long_move_mask_x_num_quarter_array(i))<<"  min value!!!!!!!"<<endl;

		//Transpose the mask to get the mask for the west wind -3
		m_landing_masks[i][3] = m_landing_masks[i][0].transpose();
		//row wise reverse the north mask (0) to get the south mask (2)
		m_landing_masks[i][2] = m_landing_masks[i][0].rowwise().reverse();
		//column wise reverse the west mask (3) to get the east mask (1)
		m_landing_masks[i][1] = m_landing_masks[i][3].colwise().reverse();
	}


	//fstream tempoutputfile("landing_mask.txt", ios::out);
	//tempoutputfile<<m_landing_masks[0][0]<<endl<<endl<<endl;
	//tempoutputfile.close();
}

void SubPopulation_Population_Manager::doDevelopment(){
	double current_temp = g_landscape_ptr->SupplyTemp();
	double current_degree_days = 0;
	//std::cout<<m_index_next_life_stage<<endl;
	//m_index_next_life_stage = -1;

	//update hibernate flag
	//This is only for the overwintering eggs
	if(!m_hibernated_hatch_flag&&g_landscape_ptr->SupplyMonth()==4) m_hibernated_hatch_flag = true;
	if(m_hibernated_hatch_flag &&g_landscape_ptr->SupplyMonth()==11) m_hibernated_hatch_flag = false;

	for (int i=0; i < m_num_life_stage; i++){
		//clear the data from yesterday
		m_index_next_life_stage[i].clear();
		//not used, consider deleting 13/08/2023 XD
		//First, calculate the degree days for today
		//THe degree days can only be accumulated when the curent temperature is higher than the lowest development temperature
		//for over winter eggs, no need to accumulate degree days
		//if (i==0 && m_development_degree_day(0, i) < 0) {
		//	if (m_hibernated_hatch_flag){
		//		m_index_next_life_stage[i].push_back(0); //for the overwintering eggs, they always stay at zero.
		//	}
		//	continue;
		//}

		//calculate the degree days using hourly temperature
		double current_degree_days = 0;
		for (int time_counter= 0; time_counter<24; time_counter++){
			double current_temp = g_landscape_ptr->SupplyTempHour(time_counter);
			//single linear degree days calculation
			if(!m_turning_temp_dev_used_flag){
				if(current_temp > m_lowest_temp_dev(i)){
					current_degree_days += (current_temp - m_lowest_temp_dev(i));
				}
			}
			//double linear degree days calculation
			else{
				if(current_temp >= m_turning_temp_dev(i)){
					current_degree_days += (current_temp - m_lowest_temp_dev(i));
				}
				else if(current_temp <= m_turning_temp_dev(i) && current_temp >0){
					current_degree_days += current_temp*m_weight_dd_lower_turning_temp_dev(i);
				}
			}
		}
		current_degree_days /= 24.0;

		//stop egg development in the second half of a year
		if(g_landscape_ptr->SupplyMonth()>=7 && i==0 && m_egg_start_dd_jan1st){
			//set the degree days to 0 if it is larger than 0
			if(m_accumu_degree_days(0,0)>0){
				m_accumu_degree_days(0,0) = 0;
			}
			m_egg_hatch_chance = 0;
			continue;
		}

		//decide which ones should be added
		//new is smaller than old
		if(m_index_new_old(i,0)<m_index_new_old(i,1)){
			m_accumu_degree_days(i, Eigen::seq(0, m_index_new_old(i, 0))).array() += current_degree_days;
			m_accumu_degree_days(i, Eigen::seq(m_index_new_old(i, 1), m_max_alive_days-1)).array() += current_degree_days;
		}

		//new is the same as old, this can only happen when the index is 0 (the first day of the simulation)
		else if(m_index_new_old(i, 0)==m_index_new_old(i, 1)){
			m_accumu_degree_days(i, m_index_new_old(i, 1)) += current_degree_days;
		}
		//update the ones in between the new and old
		else{
			m_accumu_degree_days(i, Eigen::seq(m_index_new_old(i, 1), m_index_new_old(i, 0))).array() += current_degree_days;
		}

		//calculate the hatch chance for eggs
		if(i==0){
			m_egg_hatch_chance = 0;
			if(m_egg_start_dd_jan1st && g_landscape_ptr->SupplyMonth()<7){
				m_egg_hatch_chance = calHatchChance();	
			}
			continue;
		}

		//check whether they need to go to next life stage or die
		while(isEnoughNextLifeStage(i)){
			m_index_next_life_stage[i].push_back(m_index_new_old(i, 1));
			//set it to zero
			m_accumu_degree_days(i, m_index_new_old(i, 1)) = 0.0;
			m_index_new_old(i, 1) += 1;
			//when it reaches the end, set it to zero
			if (m_index_new_old(i,1) >= m_max_alive_days){
				m_index_new_old(i,1) = 0;
			}
		}		
	}
	
	//std::cout << current_temp <<endl;
	//std::cout << m_index_new_old <<endl;
	//for (int temp_next:m_index_next_life_stage.at(5))
	//	std::cout << temp_next <<endl;
	//std::cout << m_accumu_degree_days<<endl;
	//std::cout << m_current_mortality_array << endl;
}

bool SubPopulation_Population_Manager::isEnoughNextLifeStage(int a_life_stage){
	//cout<<m_development_degree_day<<endl;
	return m_accumu_degree_days(a_life_stage, m_index_new_old(a_life_stage,1))>=m_development_degree_day(0, a_life_stage);
}

void SubPopulation_Population_Manager::addNewDay(){
	m_index_new_old.col(0).array()+=1;

	//If eggs only accumulate degree days from the first day of a year
	//Always put the new born eggs in the first place(i.e., newest index is always 0)
	if(m_egg_start_dd_jan1st){
		m_index_new_old(0,0) = 0;
		m_index_new_old(0,1) = 0;
	}

	//Set it to zero when they reaches the end
	for (int i=0; i < m_num_life_stage; i++){
		if (m_index_new_old(i,0) >= m_max_alive_days){
			m_index_new_old(i,0) = 0;
		}
	}
}

int SubPopulation_Population_Manager::calNextStage(int current_stage, double density){
	int next_stage;

	//if(current_stage == 2) //to adult, randomly choose female or male
	//{
	//	next_stage = g_rand_uni()>0.5 ? current_stage+1 :current_stage+2;
	//}
	//else 
	if (current_stage == 3 || current_stage == 4) // already adult, die next
	{
		next_stage = -1;
	}
	else if(current_stage<2)// otherwise next life stage
	{
		next_stage = current_stage+1;
	}
	else{
		if(density>=0){
			if(density<cfg_Subpopu_Density_Threshold.value()){
				next_stage = 4;
			}
			else{
				next_stage = 3; //too crowded, winged ones
			}
		}
		else{
			next_stage=-1;
		}
	}	
	return next_stage;
}

void SubPopulation_Population_Manager::DoFirst(){
   updateDevelopmentSeason();
   updateMortalityArray();
   double current_wind_speed = g_landscape_ptr->SupplyWind();
	if (current_wind_speed<m_max_flying_wind_speed){
		relocatePopulation(); //let the species move when it is not too windy
	}
}

void SubPopulation_Population_Manager::setOldIndex(int life_stage, int p_value){
	//1 is the old one
	m_accumu_degree_days(life_stage, Eigen::seq(m_index_new_old(life_stage, 0), p_value)).setZero();
	m_index_new_old(life_stage,1) = p_value;
}


void SubPopulation_Population_Manager::doFlying(int ind, int index_x, int index_y){


	int life_stage = m_flying_life_stage_array[ind];
	int landing_life_stage = m_landing_life_stage_array[ind];
	Eigen::MatrixXd* temp_animal_matrix_ptr = m_the_subpopulation_array[index_y][index_x]->getAnimalMatrix();
	

	Eigen::MatrixXd* current_landing_array_ptr = &m_landing_masks[m_current_wind_speed_index][m_current_wind_direction];
	double flying_total_num = 0;
	//cout<<m_the_subpopulation_array[index_y][index_x]->getNumforOneLifeStage(life_stage)<<endl;
	


	for(int index_second = 0; index_second<m_max_alive_days; index_second++){
		flying_total_num =( *temp_animal_matrix_ptr)(life_stage, index_second);
		//cout<<index_second<<"flying_total_num: "<<flying_total_num<<endl;
		if(flying_total_num>0){
			#pragma omp parallel for collapse(2)
			for(int y=0; y<current_landing_array_ptr->rows(); y++){
				for(int x=0; x<current_landing_array_ptr->cols(); x++){
					int temp_index_x_landing;
					int temp_index_y_landing;
					if(m_current_wind_direction == 0 || m_current_wind_direction == 2){
							temp_index_x_landing = index_x-m_long_move_mask_x_num_quarter_array(m_current_wind_speed_index)+x;
					}
					//East
					if(m_current_wind_direction == 1){
						temp_index_x_landing = index_x-x-1;			
					}
					//West				
					if(m_current_wind_direction == 3){
						temp_index_x_landing = index_x+x+1;			
					}

					//wrap around
					if(temp_index_x_landing<0) temp_index_x_landing = m_num_x_range + temp_index_x_landing;
					if(temp_index_x_landing>=m_num_x_range) temp_index_x_landing = temp_index_x_landing - m_num_x_range;	
							
					if(m_current_wind_direction == 0){
						temp_index_y_landing = index_y+y+1;			
					}				
					if(m_current_wind_direction == 2){
						temp_index_y_landing = index_y-y-1;			
					}

					//north or south wind
					if(m_current_wind_direction == 1 || m_current_wind_direction == 3){
						temp_index_y_landing = index_y-m_long_move_mask_y_num_quarter_array(m_current_wind_speed_index)+y;
					}

					//wrap around
					if(temp_index_y_landing<0) temp_index_y_landing = m_num_y_range + temp_index_y_landing;
					if(temp_index_y_landing>=m_num_x_range) temp_index_y_landing = temp_index_y_landing - m_num_y_range;



					//only land them whe the suitability is greater than 0
					double temp_landing_number = flying_total_num*(*current_landing_array_ptr)(y,x);
					if(temp_landing_number>0 && m_cell_suitability(temp_index_y_landing, temp_index_x_landing) >0){
						//int temp_landing_age = m_index_new_old(landing_life_stage, 0);
						m_the_subpopulation_array[temp_index_y_landing][temp_index_x_landing]->addAnimalNumGivenStageColumn(landing_life_stage, index_second, temp_landing_number*m_landing_survival_rate);
					}
				}
			}
		}
	}
	m_the_subpopulation_array[index_y][index_x]->removeAnimalNumGivenStage(life_stage);
}

bool SubPopulation_Population_Manager::isWinterHostTole(TTypesOfLandscapeElement a_ele){
	if (std::count(m_winter_hosts_tole.begin(), m_winter_hosts_tole.end(), a_ele)){
		return true;
	}

	else {
		return false;
	}
}

bool SubPopulation_Population_Manager::isSummerHostTole(TTypesOfLandscapeElement a_ele){
	if (std::count(m_summer_hosts_tole.begin(), m_summer_hosts_tole.end(), a_ele)){
		return true;
	}

	else {
		return false;
	}
}

bool SubPopulation_Population_Manager::isWinterHostToc(TTypesOfCrops a_ele){
	if (std::count(m_winter_hosts_toc.begin(), m_winter_hosts_toc.end(), a_ele)){
		return true;
	}

	else {
		return false;
	}
}

bool SubPopulation_Population_Manager::isSummerHostToc(TTypesOfCrops a_ele){
	if (std::count(m_summer_hosts_toc.begin(), m_summer_hosts_toc.end(), a_ele)){
		return true;
	}

	else {
		return false;
	}
}

void SubPopulation_Population_Manager :: updateWholePopulationArray(Eigen::VectorXd a_array){
	m_total_num_each_stage += a_array;
}

void SubPopulation_Population_Manager :: updateMortalityArray(void){
	//get the current temperature
	//double current_temperature = g_landscape_ptr->SupplyTemp();
	//egg will not die because of cold or hot
	//m_current_mortality_array.row(0).fill(0.01);
	//others will die because of too cold and too hot
	//for (int i=1; i<m_num_life_stage; i++){
	//	if(current_temperature<m_lowest_temperature_die(i) || current_temperature>m_highest_temperature_die(i)){
	//		m_current_mortality_array.row(i).fill(1); //all die if it is too cold or too hot
	//	}
	//	else{
	//		m_current_mortality_array.row(i).fill(0.01);
	//	}
	//}
	m_current_mortality_array.fill(0.01);
}

double SubPopulation_Population_Manager :: calLandingCurve(double a_dis, double a_peak, double a_furthest){
	if(a_dis<=a_peak && a_dis>0){
		return -1*a_dis*a_dis + 2*a_peak*a_dis;
	}
	if(a_dis>a_peak && a_dis <= a_furthest){
		return a_peak*a_peak*pow(a_furthest+1-a_dis, 2)/pow(a_furthest+1-a_peak, 2); //plus one to make it reach the furthest
	}
	return 0.0;
}

void SubPopulation_Population_Manager :: initialisePopulation(){
	//this is one a general subpopulation species
	m_ListNames[0]="Egg";
	m_ListNames[1]="Larva";
	m_ListNames[2]="Pupa";
	m_ListNames[3]="Adult";
	m_ListNames[4]="Adult wingless";
	m_SimulationName = "General Subpopulation Simulation";

	//suppose we only have one type of host
	//m_summer_hosts_tole.push_back(tole_Field);
	//m_mul_hosts_flag = false; //no host changes
	readHosts("Subpopulation/subpopulation_default_host.txt");
	
	//This needs to be changed in the decent class
	m_flying_life_stage_array.push_back(3);
	m_landing_life_stage_array.push_back(4); //by default, they don't drop theri wings
	m_local_moving_life_stage_array.push_back(3);

	//we add 1000 eggs to the grid when it is a suitable one
	int index_cell_i = 0;
	int index_cell_j = 0;
	struct_SubPopulation* sp;
	sp = new struct_SubPopulation;
	sp->NPM = this;
	sp->L = g_landscape_ptr;
	//creat the aphid in each cell
	for (int i=0; i<SimW; i=i+m_sub_w) //x, but coloumn number
	{
		index_cell_j = 0;
		for(int j=0; j<SimH; j=j+m_sub_h) //y, but row number
		{
			sp->x = i+m_sub_w/2.0; //use the center of the cell as the location;
			sp->y = j+m_sub_h/2.0;;
			sp->w = m_sub_w;
			sp->h = m_sub_h;
			sp->NPM = this;
			sp->empty_flag = false;
			sp->index_x = index_cell_i;
			sp->index_y = index_cell_j;
			TTypesOfLandscapeElement current_landtype = g_landscape_ptr->SupplyElementType(i, j);
			sp->starting_popu_density = &(m_cell_popu_density(index_cell_j, index_cell_i));
			sp->starting_suitability = &(m_cell_suitability(index_cell_j, index_cell_i));
			
			double current_biomass = g_landscape_ptr->SupplyVegBiomass(i, j);

			//for the default subpopulation species, we only have one host
			bool current_winter_host_flag = isSummerHostTole(current_landtype);
			if(current_winter_host_flag>0){
				//sp->starting_suitability = 1;
				m_the_subpopulation_array[index_cell_j][index_cell_i] = CreateObjects(NULL, sp, 1000);
				m_cell_suitability(index_cell_j, index_cell_i) = 1;
			}
			else{
				sp->empty_flag = true;
				//sp->starting_suitability = 0;
				m_the_subpopulation_array[index_cell_j][index_cell_i] = CreateObjects(NULL, sp, 0);
				m_cell_suitability(index_cell_j, index_cell_i) = 0;
			}			
			index_cell_j++;
		}
		index_cell_i++;
	}
	delete sp;
}

SubPopulation* SubPopulation_Population_Manager::CreateObjects(TAnimal *pvo, struct_SubPopulation* data, int number){
   SubPopulation*  new_Sub;
   new_Sub = new SubPopulation(data->x, data->y, data->L, data->NPM, data->empty_flag, data->index_x, data->index_y, data->starting_suitability, data->starting_popu_density, number);
   // always 0 since it is a subpopulation model

   //the simulation starts from Jan, we only have some eggs from the beginning.
   if(number > 0){
		//updateWholePopulationArray(toa_Egg, number);
		new_Sub -> addAnimalNumGivenStageColumn(tos_Egg, 0, number);
	}
   return new_Sub;
}


int SubPopulation_Population_Manager::supplyAgeInDay(int lifestage_index, int column_index){
	int current_newest_index = supplyNewestIndex(lifestage_index);
	if(current_newest_index>=column_index || (current_newest_index<column_index && column_index <= current_newest_index)){
		return current_newest_index-column_index;
	}
	else{
		return m_max_alive_days-column_index+current_newest_index;
	}
}

void SubPopulation_Population_Manager::SetNoProbesAndSpeciesSpecificFunctions(int a_pn)
{
    // This function relies on the fact that g_Species has been set already
   m_TheLandscape->SetSpeciesFunctions(g_Species);
}

void SubPopulation_Population_Manager::readHosts(string a_file_name){

	//reset all the vectors
	m_winter_hosts_tole.clear();
	m_winter_hosts_tole_period.clear();
	m_winter_hosts_tole_period_map.clear();
	m_winter_hosts_toc.clear();
	m_summer_hosts_tole.clear();
	m_summer_hosts_tole_period.clear();
	m_summer_hosts_tole_period_map.clear();
	m_summer_hosts_toc.clear();

	ifstream ist(a_file_name, ios::in);
	string one_line;
	string host_id;
	int month_id;

	//get the host switch flag
	getline(ist, one_line);
	istringstream iss(one_line);
	int summer_winter;
    iss >> summer_winter;
    if (summer_winter==1) {
		m_mul_hosts_flag=false;
		m_summer_host_on=true;
		m_winter_host_on=false;
	}//only summer hosts are uses since the species does not switch hosts

	if (summer_winter==2) {
		m_mul_hosts_flag=true;
		m_winter_host_on=true;
		m_summer_host_on=false;
	}//start with winter host since the simulation starts from jan.

	//get the summer host tole
	getline(ist, one_line);
	iss = istringstream(one_line);
	int temp_index = -1;
	while(iss>>host_id){
		temp_index += 1;
		if(host_id!="NONE"){
			TTypesOfLandscapeElement host_tole = m_TheLandscape->TranslateEleTypesFromString(host_id);
			m_summer_hosts_tole.push_back(host_tole);
			m_summer_hosts_tole_period_map.insert({ host_tole, temp_index});
		}
	}

	//get the summer host tole period
	getline(ist, one_line);
	iss = istringstream(one_line);
	while(iss>>month_id){
		if(month_id<0){
			break;
		}
		else{
			std::vector<int> temp(2, 1);
			temp[0] = month_id;
			iss>>month_id;
			temp[1] = month_id;
			m_summer_hosts_tole_period.push_back(temp);
		}
	}

	//get the summer host toc
	getline(ist, one_line);
	iss = istringstream(one_line);
	string veg_str;
	while(iss>>veg_str){
		if(veg_str!="NONE"){
			m_summer_hosts_toc.push_back(g_farmmanager->TranslateCropCodes(veg_str));
		}
	}

	//get the winter host tole
	getline(ist, one_line);
	iss = istringstream(one_line);
	temp_index = -1;
	while(iss>>host_id){
		temp_index += 1;
		if(host_id!="NONE"){
			TTypesOfLandscapeElement host_tole = m_TheLandscape->TranslateEleTypesFromString(host_id);
			m_winter_hosts_tole.push_back(host_tole);
			m_winter_hosts_tole_period_map.insert({host_tole, temp_index});
		}
	}

	//get the winter host tole period
	getline(ist, one_line);
	iss = istringstream(one_line);
	while(iss>>month_id){
		if(month_id<0){
			break;
		}
		else{
			std::vector<int> temp(2, 1);
			temp[0] = month_id;
			iss>>month_id;
			temp[1] = month_id;
			m_winter_hosts_tole_period.push_back(temp);
		}
	}

	//get the winter host toc
	getline(ist, one_line);
	iss = istringstream(one_line);
	while(iss>>veg_str){
		if(veg_str!="NONE"){
			m_winter_hosts_toc.push_back(g_farmmanager->TranslateCropCodes(veg_str));
		}
	}

}

int SubPopulation_Population_Manager::calOffspringStage(int current_stage, double *offspring_num, double a_age, double a_density, double a_growth_stage, double* a_propotion, bool winter_host_flag){
	if(current_stage == 3 || current_stage == 4){
		*offspring_num = 1;
		return 0;
	}
	*offspring_num = 0;
	return -1;
}
