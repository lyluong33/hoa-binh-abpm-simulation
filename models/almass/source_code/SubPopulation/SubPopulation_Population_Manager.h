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
/** \file SubPopulation_Population_Manager.h 
\brief <B>The main header code for base population manager class using subpopulation method</B>
*/
/**  \file SubPopulation_Population_Manager.h
Version of  Dec. 2023 \n
By Xiaodong Duan \n \n
*/

//---------------------------------------------------------------------------
#ifndef SubPopulation_Population_ManagerH
#define SubPopulation_Population_ManagerH

#include <Eigen/Dense>
#include "SubPopulation.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

class SubPopulation;
class SubPopulation_Population_Manager;

/**
* \brief
* Used for creation of a new struct_SubPopulation object
*/
class struct_SubPopulation
{
 public:
  /** \brief x-coord */
  int x;
  /** \brief y-coord */
  int y;
  /** \brief area width */
  int w;
  /** \brief area height */
  int h;
  /** \brief species ID */
  int species;
  /** \brief Landscape pointer */
  Landscape* L;
  /** \brief SubPopulation_Population_Manager pointer */
  SubPopulation_Population_Manager * NPM;
  /** \brief Indicator show whether it is an empty subpopulation. */
  bool empty_flag;
  /** \brief Starting suitability for the subpopulation. */
  double* starting_suitability;
  /** \brief Starting weighted population density for the subpopulation. */
  double* starting_popu_density;
  /** \brief The x-index for the SubPopulation ojbect.*/
  int index_x;
   /** \brief The y-index for the SubPopulation ojbect.*/
  int index_y;
};

/**
* \brief
* The class to handle all subpopulation-based animal population related matters in the whole landscape.
*/
class SubPopulation_Population_Manager : public Population_Manager_Base
{
private:
   /** \brief Array to hold the total number of alive animals under different life stages in the whole landscape */
   Eigen::VectorXd m_total_num_each_stage;
   /** \brief Array to hold the population in the field areas.*/
   Eigen::VectorXd m_total_num_each_stage_field;
   /** \brief The size of each subpopulation cell. */
   double m_size_cell;
   /** \brief The array to store the index for the ones go to the next life stage or die. -1 means nothing. */
   std::vector<std::vector<int>> m_index_next_life_stage;
   /** \brief The lowest temperature that will cause the species die. */
   Eigen::VectorXd m_lowest_temperature_die;
   /** \brief The highest temperature that will cause the species die. */
   Eigen::VectorXd m_highest_temperature_die;

   friend class Subpopulation;

protected:
   /** \brief The array to store the average max and std development degree days for each life stage, max and std degree days for reproducing. */
   Eigen::MatrixXd m_development_degree_day;
   /** \brief The number of subpopulation cell in x range. */
   int m_num_x_range;
   /** \brief The number of subpopulation cell in y range. */
   int m_num_y_range;
   /** \brief The number of life stages for the model animal. */
   int m_num_life_stage;
   /** \brief Flag to show whether it has both winter and summer host. */
   bool m_mul_hosts_flag;
   /** \brief Variable to record the width of the subpopulation cell. */
   int m_sub_w;
   /** \brief Variable to record the height of the subpopulation cell. */
   int m_sub_h;
   /** \brief Array for weighted population density in each subpopulation cell */
   Eigen::MatrixXd m_cell_popu_density;
   /** \brief Array for suitable value in each subpopulation cell */
   Eigen::MatrixXd m_cell_suitability;
   /** \brief Vector to store  all the pointers for the subpopulation object. */
   std::vector<std::vector<SubPopulation*>> m_the_subpopulation_array;
   /** \brief The longest distance that the species can fly for long distance dispersal. */
   int m_max_long_distance;
   /** \brief The distance for the peak amount of landing.*/
   int m_peak_long_distance;
   /** \brief The scale for wind speed. */
   float m_scale_wind_speed;
   /** \brief The largest wind speed for winged adults to fly. */
   float m_max_flying_wind_speed;
   /** \brief The sampling step size for wind speed. */
   int m_wind_speed_step_size;
   /** \brief The number of wind directions. */
   int m_wind_direction_num;
   /** \brief The number of wind speed samples. */
   int m_wind_speed_num;
   /** \brief Variable to store the wind speed for the current day.*/
   double m_current_wind_speed;
   /** \brief Variable to store the wind direction for the current day.*/
   int m_current_wind_direction;
   /** \brief Variable to store the wind speed index for the current day.*/
   int m_current_wind_speed_index;
   /** \brief The longest distance that the species can move for short distance dispersal. */ 
   double m_max_short_distance;
   /** \brief Variable to record the dimension of the long distance movement mask. - x */
   int m_long_move_mask_x_num;
   /** \brief Variable to record the dimension of the long distance movement mask. - y*/
   int m_long_move_mask_y_num;
   /** \brief Variable to record the dimension of the short distance movement mask. - x */
   int m_long_move_mask_x_num_half;
   /** \brief Variable to record the dimension of the short distance movement mask. - y*/
   int m_long_move_mask_y_num_half;
   /** \brief Array to store the number of cells for longest flying at different wind speeds. - x*/
   Eigen::VectorXi m_long_move_mask_x_num_half_array;
   /** \brief Array to store the number of cells for longest flying at different wind speeds. - y*/
   Eigen::VectorXi m_long_move_mask_y_num_half_array;
   /** \brief Array to store the number of cells for longest flying distance which is perpendicular to the wind direction. - x*/
   Eigen::VectorXi m_long_move_mask_x_num_quarter_array;
   /** \brief Array to store the number of cells for longest flying distance which is perpendicular to the wind direction. - y*/
   Eigen::VectorXi m_long_move_mask_y_num_quarter_array;
   /** \brief Array for accumulated degree days for each life circle. */
   Eigen::MatrixXd m_accumu_degree_days;
   /** \brief Array for index of the newest and oldest life stages, 0 is the newest one while 1 is the oldest one  */
   Eigen::MatrixXi m_index_new_old;
   /** \brief The longest alive day among all the life stages. */
   int m_max_alive_days;
   /** \brief Lowest development temperatures for each life stage. This is the last step in each day.*/
   Eigen::VectorXd m_lowest_temp_dev;
   /** \brief The turning temperatures for calculating degree days.*/
   Eigen::VectorXd m_turning_temp_dev;
   /** \brief The weight used to calculate degree days when the temperature is lower than the turning temperature. */
   Eigen::VectorXd m_weight_dd_lower_turning_temp_dev;
   /** \brief The flat to show turning temperature is used. By default, it is false, i.e., not used.*/
   bool m_turning_temp_dev_used_flag;
   /** \brief Flag variable to indicate hibernated eggs are ready to hatch. */
   bool m_hibernated_hatch_flag;
   /** \brief Array to track whether it is the first existence for a life stage. */
   std::vector<bool> m_first_flag_life_stage;
   /** \brief The vector to hold winter host landscape type. */
   std::vector <TTypesOfLandscapeElement> m_winter_hosts_tole;
   /** \brief The vector to store the exsiting period for winter host tole*/
   std::vector <std::vector<int>> m_winter_hosts_tole_period;
   /** \brief The map to store the index for the winter host period. */
   std::map <int, int> m_winter_hosts_tole_period_map;
   /** \brief The vector to hold winter host vegetation type. */
   std::vector <TTypesOfCrops> m_winter_hosts_toc;
   /** \brief The vector to hold summer host landscape type. */
   std::vector <TTypesOfLandscapeElement> m_summer_hosts_tole;
   /** \brief The vector to store the exsiting period for summer host tole*/
   std::vector <std::vector<int>> m_summer_hosts_tole_period;
   /** \brief The map to store the index for the summer host period. */
   std::map <int, int> m_summer_hosts_tole_period_map;
   /** \brief The vector to hold sumber host vegetation type. */
   std::vector <TTypesOfCrops> m_summer_hosts_toc;
   /** \brief This a lookup table for the age and temperature dependent mortality rate. This is the same for the whole landscape for each day. Further cell specific related calculate should be done in the SubPopulation class */
   Eigen::MatrixXd m_current_mortality_array;
   /** \brief The array to hold the wind directions. */
   Eigen::MatrixXd m_wind_direction_array;
   /** \brief The array to store the landing masks. The vector is indexed by the wind speed index first and the second index is the wind direction. Each element is a 2-D dimensional Eigen matrix.*/
   std::vector<std::vector<Eigen::MatrixXd>> m_landing_masks;
   /** \brief The vector to store the life stages that can fly. */
   std::vector<int>  m_flying_life_stage_array;
   /** \brief The vector to store the life stage when a flying one lands, it could be different from the flying one, e.g., they drop their wings. */
   std::vector<int> m_landing_life_stage_array;
   /** \brief The vector to store the life stags that can move locally. */
   std::vector<int>  m_local_moving_life_stage_array;
   /** \brief The file for the storing the data. */
   std::ofstream m_subpopulation_base_prb_file;
   /** \brief The flag to enable summer host.*/
   bool m_summer_host_on;
   /** \brief The flag to enable winter host.*/
   bool m_winter_host_on;
   /** \brief The flag to make eggs only start to accumulate degree days in Jan. 1st. By default, this is set as true.*/
   bool m_egg_start_dd_jan1st;
   /** \brief The egg hatch chance for the given day.*/
   double m_egg_hatch_chance;
   /** \brief The landing survival rate for flying.*/
   double m_landing_survival_rate;

public:
   /** \brief This is to store the running species number, for aphid, 0: English grain, 1: Pea, 2: Black bean, 3: Peach potato.*/
   int m_running_species_number;
   /** \brief The constructor. */
   SubPopulation_Population_Manager(Landscape* L, string DevReproFile ="", int a_sub_w=10, int a_sub_h=10, int a_num_life_stage=5, int a_max_long_dist = 1000, int a_peak_long_dist = 100, float a_scale_wind_speed = 1.1, float a_max_wind_speed = 16, int a_wind_direc_num = 8, int a_wind_speed_step_size = 2, int a_max_alive_day = 300, bool a_open_output_file_flag=true);
   /** \brief The destructor */
   virtual ~SubPopulation_Population_Manager (void);
   /** \brief Supply the population size at the given life stage for the whole landscape.*/
   double supplyAllPopulationGivenStage(int index);
   /** \brief Supply the whole population size at the given cell.*/
   double supplyTotalPopulationInCell(int x_indx, int y_indx);
   /** \brief Supply the total subpopulation size in the given cell. */
   double getTotalSubpopulationInCell(int x_indx, int y_indx);
   /** \brief Supply the subpopulation size at the given life stage in the given cell. */
   double getSubpopulationInCellLifeStage( int x_indx, int y_indx, int a_life_stage);
   /** \brief Supply number of subpopulation in x coordinate. */
   int supplyCellNumX(){return m_num_x_range;}
   /** \brief Supply number of subpopulation in y coordinate. */
   int supplyCellNumY(){return m_num_y_range;}
   /** \brief Supply the width of a subpopulation cell. */
   double supplyCellWidth(){return m_sub_w;}
   /** \brief Supply the height of a subpopulation cell. */
   double supplyCellHeight(){return m_sub_h;}
   /** \brief Supply the size of aphid subpopulation cell. */
   double supplySizeSubpopulationCell() {return m_size_cell;}
   /** \brief Open the storing file. */
   bool openSubpopulationBaseProbeFile(string a_file_name);
   /** \brief The function to store results. */
   virtual void subpopuBaseOutputProbe();
   /** \brief Supply the suitability in the given cell. */
   double getSuitabilityInCell(int x_indx, int y_indx) {return m_cell_suitability(y_indx, x_indx);}
   /** \brief Supply the pointer of the subpopulation object in the given cell. */
   SubPopulation* supplySubPopulationPointer(int indx, int indy);
   /** \brief Supply the number of life stages */
   int supplyLifeStageNum(){return m_num_life_stage;}
   /** \brief Supply the maximum number of column in the development array. */
   int supplyMaxColNum() {return m_max_alive_days;}
   /** \brief Supply the indices which are ready for the next life stage. */
   std::vector<int> supplyOldEnoughIndex(int life_stage_index) {return m_index_next_life_stage[life_stage_index];}
   /** \brief Supply the index for the new ones. */
   int supplyNewestIndex(int life_stage_index) { return m_index_new_old(life_stage_index, 0);}
   /** \brief Supply the population size of the given life stage. */
   virtual unsigned GetLiveArraySize(int a_listindex);
   /** \brief Function to read the development time and life stages */
   void readDevReproFile(string inputfile);
   /** \brief Relocate the population in each cell based on the suitability and movement ability. */
   void relocatePopulation(void);
   /** \brief Function to be called after all the SubPopulations are looped. */
   virtual void DoLast();
   /** \brief Function to be called before all the SubPopulations are looped. */
   virtual void DoFirst();
   /** \brief Function to calculate the movement mask. */
   virtual void calLongMovementMask(void);
   /** \brief Function to make the development for all the subpopulation object. */
   void doDevelopment();
   /** \brief Add new day to the newest and oldest array. */
   void addNewDay();
   /** \brief Return the next life stage based on the density*/
   virtual int calNextStage(int current_stage, double density = 1);
   /** \brief Calculate the offspring life stage. */
   virtual int calOffspringStage(int current_stage, double *offspring_num=NULL, double a_age=1, double a_density = -1, double a_growth_stage=0, double* a_propotion=NULL, bool winter_host_flag=false);
   /** \brief The function to update the development season. Do nothing by default. */
   virtual void updateDevelopmentSeason(){};
   /** \brief The function to update the whole population array for the given life stage in the whole landscape. */
   void updateWholePopulationArray(int a_listindex, double number);
   /** \brief The function to update the whole population array for all the life stages in the whole landscape. */
   void updateWholePopulationArray(Eigen::VectorXd a_array);
   /** \brief The function to set the flag indicating whether a given life stage is the first time of existing.*/
   void setFirstFlagLifeStage(int life_stage, bool pvalue) {m_first_flag_life_stage[life_stage] = pvalue;}
   /** \brief The function to supply the flag of first for the given life stage.*/
   bool supplyFirstFlagLifeStage(int life_stage) {return m_first_flag_life_stage[life_stage];}
   /** \brief The function to set the oldest index for the given life stage. */
   void setOldIndex(int life_stage, int p_value);
   /** \brief The function to fly the winged adults. */
   void doFlying(int ind, int index_x, int index_y);
   /** \brief Test whether it is a winter host tole. */
   bool  isWinterHostTole(TTypesOfLandscapeElement a_ele);
   /** \brief Test whether it is a summer host tole. */
   bool  isSummerHostTole(TTypesOfLandscapeElement a_ele);
   /** \brief Test whether it is a winter host toc. */
   bool  isWinterHostToc(TTypesOfCrops a_ele);
   /** \brief Test whether it is a summer host toc. */
   bool  isSummerHostToc(TTypesOfCrops a_ele);
   /** \brief The function to supply the development season. */
   virtual unsigned supplyDevelopmentSeason () {return 0;};
   /** \brief This function is used to update the daily mortality rate for all the ages. In this base class, it only depends on the temperature. The lowest and highest temperature that will make the species die. The optimal temperature will make the mortality rate to be 0.*/
   virtual void updateMortalityArray (void);
   /** \brief The function to supply the mortality rate for the given life stage and the age. */
   double supplyMortality(int a_life_stage, int a_age) {return m_current_mortality_array(a_life_stage, a_age);}
   /** \brief The function to supply the whole base mortality rate array. */
   Eigen::MatrixXd* supplyMortalityWholeArrayPtr(void) {return &m_current_mortality_array;}
   /** \brief The function to supply the 1D base morality rate array for the given life stage. */
   Eigen::VectorXd supplyMortalityStageArray(int a_life_stage) {return m_current_mortality_array.row(a_life_stage);}
   /** \brief The function to calculate the landing curve along the wind direction. */
   double calLandingCurve(double a_dis, double a_peak, double a_furthest);
   /** \brief The function to initialise the population when starting the simulation which requires rewritten in the derived class. */
   virtual void initialisePopulation();
   /** \brief The function to create the subpopulation object. */
   SubPopulation* CreateObjects(TAnimal *pvo, struct_SubPopulation* data, int number);
   /** \brief The function to return the age in days given an element in the subpopulation table. */
   int supplyAgeInDay(int lifestage_index, int column_index);
   /** \brief The function to return the accumulated degree days given an element in the subpopulation table. */
   double supplyAgeInDayDegree(int lifestage_index, int column_index) {return m_accumu_degree_days(lifestage_index, column_index);}
   /** \brief Supply the vector of life stages for flying. */
   std::vector<int> supplyVecFlyingLifeStages() {return m_flying_life_stage_array; };
   /** \brief Supply the number of life stages that can fly. */
   int supplyNumFlyingLifeStages() {return m_flying_life_stage_array.size();}
   /** \brief Supply the vector of life stages that can do local movement. */
   std::vector<int> supplyVecLocMoveLifeStages() {return m_local_moving_life_stage_array; };
   /** \brief Supply the number of life stages that can do local movement. */
   int supplyNumLocMovLifeStages() {return m_local_moving_life_stage_array.size();}
   /** \brief Supply the number of animals for the given life stage. */
   virtual unsigned GetPopulationSize(int a_listindex) {
        return supplyAllPopulationGivenStage(a_listindex);
    }

   /** \brief The function to check whether it is ready for the next life stage. */
   virtual bool isEnoughNextLifeStage(int a_life_stage);
   /** \brief The function to supply the available period vector for the given winter tole host. */
   std::vector<int> supplyWinterTolePeriod(TTypesOfLandscapeElement a_tole) {return m_winter_hosts_tole_period[m_winter_hosts_tole_period_map[a_tole]]; }
   /** \brief The function to supply the available period vector for the given summer tole host. */
   std::vector<int> supplySummerTolePeriod(TTypesOfLandscapeElement a_tole) {return m_summer_hosts_tole_period[m_summer_hosts_tole_period_map[a_tole]]; }

   /** \brief The special last thing for the model species, nothing is done here. This function requires override in the descent class.*/
   virtual void doSpeciesLastThing(){;}
   /** \brief Update whole population info for the whole landscape. */
   void updateWholePopulation();
   /** \brief Sets up probe and species specifics */
	virtual void SetNoProbesAndSpeciesSpecificFunctions(int a_pn);
   /** \brief The fuction to read the host lists for the model species. */
   virtual void readHosts(string a_file_name);
   /** \brief The function to calculate the hatch chance for eggs, this function must be override in the derived class.*/
   virtual double calHatchChance();
   /** \brief The function to supply the egg's hatch chance.*/
   double supplyEggHatchChance() {return m_egg_hatch_chance;}

   /** \brief Write probe files for calibration*/
   virtual void writeCalibrationFiles(void){;};
   /** \brief Return whether the summer host is available.*/
   bool supplySummerHostOn(void){return m_summer_host_on;}
   /** \brief Return whether the winter host is available.*/
   bool supplyWinterHostOn(void){return m_winter_host_on;}

protected:
   /** \brief The main function to loop all the subpopulations to do their activities. */
   virtual void Run(int NoTSteps);
};

#endif
