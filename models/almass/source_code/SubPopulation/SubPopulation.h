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
/** \file SubPopulationAnimal.h
\brief <B>The main header code for base class of animal using sub-population method.</B>
*
Version of  Dec. 2023 \n
By Xiaodong \n \n
*/

//---------------------------------------------------------------------------
#ifndef SubPopulationAnimalH
#define SubPopulationAnimalH
//---------------------------------------------------------------------------
#include<Eigen/Dense>
//---------------------------------------------------------------------------
class SubPopulation;
class SubPopulation_Population_Manager;
//------------------------------------------------------------------------------

 enum SubPopulation_Object : int{
   tos_Egg=0,
   tos_Larva,
	tos_Pupa,
	tos_Adult,
   tos_Adult_no_wing,
	count
} ;


/**
\brief
The class for base animal using subpopulation method.
*/
class SubPopulation : public TAnimal
{
private:
   /** \brief Flag to show whether it is an empty subpopulation cell. */
   bool m_empty_cell;
   /** \brief A unique ID number for this species */
   unsigned m_SpeciesID;   

protected:
	/** \brief A static member for the cell area. */
	static double m_cell_size;
	/** \brief Variable to store the index of x in the population manager. */
   int m_index_x;
   /** \brief Variable to store the index of y in the population manager. */
   int m_index_y;
   /** \brief This is a time saving pointer to the correct population manager object */
   SubPopulation_Population_Manager*  m_OurPopulationManager;
   /** \brief Variable to store the weighted population density pointer*/
   double *m_popu_density;
   /** \brief Variable to store the raw population density in the cell.*/
   double m_raw_popu_density {0};
   /** \brief Variable to store the suitability pointer*/
   double *m_suitability;
   /** \brief Variable to track the whole population in the current cell. */
   double m_whole_population_in_cell;
   /** \brief Array to hold the population number for each life stage. */
   Eigen::VectorXd m_population_each_life_stage;
   /** \brief Variables to record numbers of the animals at different life stages and age in calender days */
   Eigen::MatrixXd m_animal_num_array;
   /** \brief Random weight for off-crop areas to make them not homogenous.*/
   double m_weight_biomass;



   /** \brief The variable to store the winter landscape type. */
   TTypesOfLandscapeElement m_winter_landscape_host;
   /** \brief The variable to store the summer landscape type. */
   TTypesOfLandscapeElement m_summer_landscape_host;
   /** \brief The staring and ending month for the winter landscape host. */
   std::vector<int> m_winter_landscape_host_period;
   /** \brief The starting and ending month for the summer landscape host. */
   std::vector<int> m_summer_landscape_host_period;
   /** \brief The flag to show whether it is part of farm. */
   bool m_farm_flag;
   /** \brief Static member to indicate that eggs only start to accumulate day degrees from the beginning of a year.*/
   static bool m_flag_eggs_dd_jan1;

public:
   /** \brief Subpopulation constructor */
   SubPopulation(int p_x, int p_y, Landscape* p_L, SubPopulation_Population_Manager* p_NPM, bool a_empty_flag, int a_index_x, int a_index_y, double* p_suitability=NULL, double* p_weight_density=NULL, int number=-1, int a_SpeciesID=999, TTypesOfLandscapeElement p_winter_landscape_host=tole_Foobar, TTypesOfLandscapeElement p_summer_landscape_host=tole_Foobar, bool p_farm_flag=false);
   /** \brief Subpopulation destructor */
   virtual ~SubPopulation()=default;
   /** \brief A typical interface function - this one returns the SpeciesID number as an unsigned integer */
   unsigned getSpeciesID() { return m_SpeciesID; }
   /** \brief A typical interface function - this one set the SpeciesID number*/
   void setSpeciesID(unsigned a_spID) { m_SpeciesID = a_spID; }
   /** \brief Return the number of animals in a specific life stage in the cell */
   double getNumforOneLifeStage(int source_type);
   /** \brief Return total population in the cell */
   double getTotalSubpopulation(void) {return m_whole_population_in_cell;}
   /** \brief Return the raw population density without weights.*/
   double getRawPopuDensity(void) {
	   //return m_raw_popu_density;
	   return m_whole_population_in_cell/m_cell_size;
   }
   /** \brief Add animal number for the given life stage at specific column (age) in the animal number array. */
   void addAnimalNumGivenStageColumn(int source_type, int a_column, double a_num);
   /** \brief Add animal number for the given life stage. */
   void addAnimalNumGivenStage(int source_type, Eigen::VectorXd& a_animal_num_array);
   /** \brief Function to remove one life stage completely. */
   void removeAnimalNumGivenStage(int source_type);
   /** \brief Remove propotion of animals for the given species and age. */
   double removeAnimalPortionGivenStageColumn(int source_type, int a_column, double a_prop);
   /** \brief Add the number of animal at the given source and column that moved from another grid. */
   void addAnimalFromAnotherCell(int source_type, int a_column, double a_num);
   /** \brief Get the numbers of the given life stage at different ages. */
   Eigen::VectorXd getArrayForOneLifeStage(int a_life_stage);
   /** \brief Function to calculate the weighted population density for the cell. */
   virtual void calPopuDensity(void);
   /** \brief Function to calculate the suitability for the cell. */
   virtual void calSuitability(void);
   /** \brief The main function to let the SubPopulation to perform different activities.*/
   virtual void Step();
   /** \brief The function to perform the development of the animal. */
   virtual void doDevelopment();
   /** \brief The function to perform the local movement of the animal. */
   virtual void doMovement();
   /** \brief The function to perform the reproduction of the animal. */
   virtual void doReproduction();
   /** \brief The function to perform the mortality of the animal. */
   virtual void doMortality();
   /** \brief This function is used to calculate the cell related weight to multiply with the temperature and age based mortality rate. In this base class, it always return 0, i.e., no extra cell related mortality*/
   virtual double calCellRelatedMortalityWeight() {return 0.0;};
   /** \brief Supply the animal array's address. */
   Eigen::MatrixXd* getAnimalMatrix(void) {return &(m_animal_num_array);}
   /** \brief Set the available period for winter landscape host. */
   void setWinterHostPeriod(int start_month, int end_month) {m_winter_landscape_host_period[0] = start_month; m_winter_landscape_host_period[1]=end_month;}
   /** \brief Set the available period for summer landscape host. */
   void setSummerHostPeriod(int start_month, int end_month) {m_summer_landscape_host_period[0] = start_month; m_summer_landscape_host_period[1]=end_month;}
   /** \brief The function to calculate the biotic mortality rate, by default it always returns 0. */
   virtual double calBioticMortalityRate(int a_life_stage) {return 0.0;}
   /** \brief The function to kill subpopulation by predators, which requires override in the descent class.*/
   virtual void killByPredator(double){}
   /** \brief The function to return whether it is part of farm. */
   bool isFarm() {return m_farm_flag;}
   /** \brief The fuction to call before Step() function. It is empty and must be override in the descent class if used.*/
   virtual void BeginStep(){;}
   /** \brief The fuction to call after Step() function. It is empty and must be override in the descent class if used.*/
   virtual void EndStep(){;}
   /** \brief Set the static cell size.*/
   static void setCellSize(double a_cell_size) { m_cell_size = a_cell_size; }
   /** \brief Set the static flag for eggs accumulating degree days*/
   static void setFlagEggsDDJan1(bool a_flag) {m_flag_eggs_dd_jan1 = a_flag;}
   /** \brief Get the random biomass weight.*/
   double getWeightBiomass(void) {return m_weight_biomass;}
   /** \brief Set the random biomass weight.*/
   void setWeightBiomass(double a_weight) {m_weight_biomass = a_weight;}
};

#endif