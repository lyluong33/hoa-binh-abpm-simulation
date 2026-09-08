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
/** \file SubPopulation.cpp
\brief <B>The main source code for subpopulation base class</B>
*/
/**  \file SubPopulation.cpp
Version of  Dec. 2023 \n
By Xiaodong Duan \n \n
*/

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
#include "../BatchALMaSS/ALMaSS_Random.h"

extern MapErrorMsg *g_msg;
using namespace std;

double SubPopulation::m_cell_size = 1;
bool SubPopulation::m_flag_eggs_dd_jan1 = true;

SubPopulation::SubPopulation(int p_x, int p_y, Landscape* p_L, SubPopulation_Population_Manager* p_NPM, bool a_empty_flag, int a_index_x, int a_index_y, double* p_suitability, double* p_weight_density, int number, int a_SpeciesID, TTypesOfLandscapeElement p_winter_landscape_host, TTypesOfLandscapeElement p_summer_landscape_host, bool p_farm_flag) : TAnimal(p_x,p_y)
{
	// Assign the pointer to the population manager
	m_OurPopulationManager = p_NPM;
	m_SpeciesID = a_SpeciesID; // This can be any number since its just here to demonstrate how you might add some information
	m_animal_num_array.resize(m_OurPopulationManager->supplyLifeStageNum(),m_OurPopulationManager->supplyMaxColNum());
	m_animal_num_array.fill(0); //set it to be zero
	m_empty_cell = a_empty_flag;
	m_popu_density = p_weight_density;
	m_suitability = p_suitability;
	m_index_x = a_index_x;
	m_index_y = a_index_y;
	m_whole_population_in_cell = 0;
	m_population_each_life_stage.resize(m_OurPopulationManager->supplyLifeStageNum());
	m_population_each_life_stage.fill(0);
	//by default (-1), there is nothing to create
	//if (number >0){
	//	addAnimalNumGivenStageColumn(0,0,number);
	//}
	//using the centroid to set the position for the subpopulation object
	m_Location_x = p_x;//m_index_x * m_OurPopulationManager->supplyCellWidth() + m_OurPopulationManager->supplyCellWidth()/2;
	m_Location_y = p_y;//m_index_y * m_OurPopulationManager->supplyCellHeight() + m_OurPopulationManager->supplyCellHeight()/2;
	m_winter_landscape_host = p_winter_landscape_host;
	m_summer_landscape_host = p_summer_landscape_host;
	m_farm_flag = p_farm_flag;	 
	//m_winter_landscape_host_period.resize(2); // default winter period Nov. - Feb.
	//m_winter_landscape_host_period.at(0) = 11; 
	//m_winter_landscape_host_period.at(1) = 2;
	//m_summer_landscape_host_period.resize(2); // default summer period June - Sept.
	//m_summer_landscape_host_period.at(0) = 7;
	//m_summer_landscape_host_period.at(1) = 9;

	m_weight_biomass = 1.0;
}

double SubPopulation::getNumforOneLifeStage(int source_type)
{
	return m_population_each_life_stage(source_type);
}

void SubPopulation::addAnimalNumGivenStageColumn(int source_type, int a_column, double a_num){
	//if(a_num<0) a_num = floor(a_num);
	double temp_num = -1*m_animal_num_array(source_type, a_column);
	m_animal_num_array(source_type, a_column) += a_num;
	if (a_num<=0 && m_animal_num_array(source_type, a_column)<1){
		m_animal_num_array(source_type, a_column) = 0;
		a_num = temp_num;
	}
	//update the whole population in the cell
	m_whole_population_in_cell += a_num;
	if(m_whole_population_in_cell<=0){
		m_whole_population_in_cell = 0;
	}
	//update the population for each life stage
	m_population_each_life_stage(source_type) += a_num;
	if(a_num<0 && m_population_each_life_stage(source_type)<0){
		m_population_each_life_stage(source_type) = 0;
	}

	//m_OurPopulationManager->updateWholePopulationArray(source_type, a_num);

}

void SubPopulation::addAnimalNumGivenStage(int source_type, Eigen::VectorXd& a_animal_num_array){
	//for (int i = 0; i<m_OurPopulationManager->supplyMaxColNum(); i++){
	//	if((a_animal_num_array)(i)<0) (a_animal_num_array)(i) = floor((a_animal_num_array)(i));
	//}
	
	m_animal_num_array.row(source_type)+=(a_animal_num_array);
	//for (int i = 0; i<m_OurPopulationManager->supplyMaxColNum(); i++){
	//	if(m_animal_num_array(source_type, i)<0) m_animal_num_array(source_type, i) = 0;
	//}
	double temp_count = a_animal_num_array.sum();
	
	m_whole_population_in_cell += temp_count;
	//if(m_whole_population_in_cell < 0){
	//	m_whole_population_in_cell = 0.0;
	//}
	m_population_each_life_stage(source_type) += temp_count;
	//if(m_population_each_life_stage(source_type) <0 ) m_population_each_life_stage(source_type) = 0;

	//m_OurPopulationManager->updateWholePopulationArray(source_type, temp_count);
}

void SubPopulation::removeAnimalNumGivenStage(int source_type){
	m_animal_num_array.row(source_type).fill(0.0);
	double temp_num = m_population_each_life_stage(source_type);

	m_whole_population_in_cell -= temp_num;
	if(m_whole_population_in_cell<0) m_whole_population_in_cell = 0;

	//m_OurPopulationManager->updateWholePopulationArray(source_type, temp_num);
	m_population_each_life_stage(source_type) = 0;
}

void SubPopulation::Step(){
	// There is nothing, so do nothing.
	calPopuDensity();
	calSuitability();
	if (getTotalSubpopulation() <= 0) {
	//	m_empty_cell = true;
		return;
	}

	//update the suitability of the current cell
	//calPopuDensity();
	//calSuitability();
	doMortality();
	doMovement();
	doDevelopment();
	doReproduction();
}

void SubPopulation::doDevelopment() {
	//grow up from yesterday or old enough to die
	for (int i = 0; i < m_OurPopulationManager->supplyLifeStageNum(); i++){
		//decide which life path should follow, if there is more than one
		int life_circle = 0;

		//first check if eggs accumulate degree days from the beginning of the year
		if(i==0 && m_flag_eggs_dd_jan1){
			//if yes, then check if the hatch chance is greater than 0
			double temp_egg_hatch_chance = m_OurPopulationManager->supplyEggHatchChance();
			if(temp_egg_hatch_chance > 0){
				if (m_animal_num_array(0, 0)>0){
					if(m_animal_num_array(0, 0) < 1){
						removeAnimalNumGivenStage(0);
						continue;
					}
					int next_life_stage_index = m_OurPopulationManager->calNextStage(i, *m_popu_density);
					double temp_number_to_next_stage = m_animal_num_array(0, 0)*temp_egg_hatch_chance;
					if (next_life_stage_index > 0){							
						addAnimalNumGivenStageColumn(next_life_stage_index, m_OurPopulationManager->supplyNewestIndex(next_life_stage_index), temp_number_to_next_stage);
						//check if it is the first time
						if (m_OurPopulationManager->supplyFirstFlagLifeStage(next_life_stage_index)){
							m_OurPopulationManager->setFirstFlagLifeStage(next_life_stage_index, false);
							m_OurPopulationManager->setOldIndex(next_life_stage_index, m_OurPopulationManager->supplyNewestIndex(next_life_stage_index));
						}
					}
					addAnimalNumGivenStageColumn(i, 0, -1*temp_number_to_next_stage);
				}
			}
			continue;
		}
		
		std::vector<int> old_enough_index = m_OurPopulationManager->supplyOldEnoughIndex(i);
		//check if old enough ones exit
		if (old_enough_index.size() >= 0){
			//let them go to next life stage or die
			for (int j=0; j<old_enough_index.size(); j++){
				int temp_current_index = old_enough_index[j];
				//check if they are old enough to the next stage or they are old enough to die
				if (m_animal_num_array(i, temp_current_index)>0){
					int next_life_stage_index = m_OurPopulationManager->calNextStage(i, *m_popu_density);
					if (next_life_stage_index > 0){							
						//m_animal_num_array(next_life_stage_index, m_OurPopulationManager->supplyNewestIndex(next_life_stage_index)) += m_animal_num_array(i, old_enough_index);
						addAnimalNumGivenStageColumn(next_life_stage_index, m_OurPopulationManager->supplyNewestIndex(next_life_stage_index), m_animal_num_array(i, temp_current_index));
						//check if it is the first time
						if (m_OurPopulationManager->supplyFirstFlagLifeStage(next_life_stage_index)){
							m_OurPopulationManager->setFirstFlagLifeStage(next_life_stage_index, false);
							m_OurPopulationManager->setOldIndex(next_life_stage_index, m_OurPopulationManager->supplyNewestIndex(next_life_stage_index));
						}

						//m_OurPopulationManager->updateWholePopulationArray(next_life_stage_index, m_animal_num_array(i, old_enough_index));
					}
					//m_OurPopulationManager->updateWholePopulationArray(i, -1*m_animal_num_array(i, old_enough_index));
					addAnimalNumGivenStageColumn(i, temp_current_index, -1*m_animal_num_array(i, temp_current_index));
					//they are too old, all die
					//m_animal_num_array(i, temp_current_index) = 0;
				}
			}
		}
	}
	//mortality rate
}

Eigen::VectorXd SubPopulation:: getArrayForOneLifeStage(int a_life_stage){
	return m_animal_num_array.row(a_life_stage);
}

void SubPopulation :: doMortality(){
	//first, we generate a random number
	//double current_rand = g_rand_uni();
	//get the whole mortality array and multiply with the cell relate weight
	Eigen::MatrixXd* temp_mortality_array = m_OurPopulationManager->supplyMortalityWholeArrayPtr();
	double temp_mortality = 0;
	//std::cout<<(m_OurPopulationManager->supplyMortalityWholeArray());
	//std::cout<<temp_mortality_array;
	double current_temp_cell_mortality = calCellRelatedMortalityWeight();
	//cout<<"current_temp_cell_mortality: "<<current_temp_cell_mortality<<endl;

	for (int i=0; i<m_OurPopulationManager->supplyLifeStageNum();i++){
		double temp_biotic_mortality = calBioticMortalityRate(i);
		for (int j=0; j<m_OurPopulationManager->supplyMaxColNum();j++){
			if (m_animal_num_array(i,j)>0){
				temp_mortality = (*temp_mortality_array)(i,j);

				//only add cell related mortality to non-eggs
				if(i!=0){
					temp_mortality += current_temp_cell_mortality;
				}
				
				temp_mortality += temp_biotic_mortality;

				//except the eggs, let them all die if it is not suitable for the species
				if (i!=0){
					if (temp_mortality > 0.95 && *m_suitability >= 1) temp_mortality = 0.95;
					if (temp_mortality > 0.95 && *m_suitability < 1) temp_mortality = max(0.95, 1 - *m_suitability);
				}
				//set to zero if it is negative
				if(temp_mortality<0) temp_mortality=0;
				//make sure they die due to too old
				if((*temp_mortality_array)(i, j)>=1){
					temp_mortality = 1;
				}

				//not mortality, just return
				if(temp_mortality<=0) continue;


				//small value, remove all
				if(m_animal_num_array(i,j)<1e-6 && m_population_each_life_stage(i) <1){
					removeAnimalNumGivenStage(i);
					continue;
				}
				//make them die, note it is negative
				double died_number = -1*m_animal_num_array(i,j) * temp_mortality;
				if (died_number < 0){
					addAnimalNumGivenStageColumn(i,j, died_number);
				}
			}
		}
	}
}


//In the base subpopulation class, only one way of reproduction: adults -> 3 eggs per day. This function need rewriting for all the derived classes
void SubPopulation::doReproduction()
{
	//only do this when there is adults
	for (int i = 3; i<m_OurPopulationManager->supplyLifeStageNum();i++){
		if(m_population_each_life_stage(i)>0)
		{
			double temp_birth_num = 0;
			int temp_birth_life_stage = m_OurPopulationManager->calOffspringStage(i, &temp_birth_num);
			if(temp_birth_life_stage>=0 && temp_birth_num >0){
				temp_birth_num *= m_population_each_life_stage(i);
				addAnimalNumGivenStageColumn(temp_birth_life_stage, m_OurPopulationManager->supplyNewestIndex(0), temp_birth_num);
			}
		}
	}
}


//Do movement, need override in the descent class
void SubPopulation::doMovement()
{
	//local movement
	vector<int> temp_local_move_stages = m_OurPopulationManager->supplyVecLocMoveLifeStages();
	int move_index = 0;
	for (int move_index_in_vec = 0; move_index_in_vec < temp_local_move_stages.size(); move_index_in_vec++){
		move_index = temp_local_move_stages[move_index_in_vec];
		if(m_population_each_life_stage(move_index) > 0){
			int index_x = g_vector_x[g_random_fnc(8)];
			int index_y = g_vector_y[g_random_fnc(8)];
			int land_index_x = m_index_x + index_x;
			int land_index_y = m_index_y + index_y;
			if(!(land_index_x<0 || land_index_x>=m_OurPopulationManager->supplyCellNumX() || land_index_y<0 || land_index_y>=m_OurPopulationManager->supplyCellNumY())){
				if(m_OurPopulationManager->getSuitabilityInCell(land_index_x, land_index_y)>0 && (m_OurPopulationManager->getTotalSubpopulationInCell(land_index_x, land_index_y)< m_whole_population_in_cell)){
					Eigen::VectorXd temp_move_array = m_animal_num_array.row(move_index)*0.01;
					m_OurPopulationManager->supplySubPopulationPointer(land_index_x, land_index_y)->addAnimalNumGivenStage(move_index, temp_move_array);
					temp_move_array.array() *= -1;
					addAnimalNumGivenStage(move_index, temp_move_array);
				}
			}
		}
	}
	
}

double SubPopulation::removeAnimalPortionGivenStageColumn(int source_type, int a_column, double a_prop){
	if(m_animal_num_array(source_type, a_column)<0) return 0;
	double return_value = m_animal_num_array(source_type, a_column) * a_prop;
	m_animal_num_array(source_type, a_column) -= return_value;
	m_whole_population_in_cell -= return_value;
	m_population_each_life_stage(source_type) -= return_value;
	
	return return_value;
}


void SubPopulation::addAnimalFromAnotherCell(int source_type, int a_column, double a_num){
	m_animal_num_array(source_type, a_column) += a_num;
	m_whole_population_in_cell += a_num;
	m_population_each_life_stage(source_type) += a_num;
}

void SubPopulation::calSuitability(void){
	*m_suitability = 1;
	if(m_OurPopulationManager->isSummerHostTole( m_OurLandscape->SupplyElementType(m_Location_x, m_Location_y))) return;
	if(m_OurPopulationManager->isSummerHostToc( m_OurLandscape->SupplyCropType(m_Location_x, m_Location_y))) return;
	*m_suitability = 0;
}

void SubPopulation::calPopuDensity(void){
	*m_popu_density = -1;
	double current_biomass = m_OurLandscape->SupplyGreenBiomass(m_Location_x, m_Location_y);
	if(current_biomass > 0)
		*m_popu_density = m_whole_population_in_cell/current_biomass/m_OurPopulationManager->supplySizeSubpopulationCell();
}