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
<B>Farm.h This file contains the headers for the Farm class</B> \n
*/
//
// Farm.h
//
/*

Copyright (c) 2003, National Environmental Research Institute, Denmark (NERI)

All rights reserved.


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

*) Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
*) Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
*) Neither the name of the NERI nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef FARM_H
#define FARM_H

// This really needs to be a #define.
#define PROG_START 1

extern TTypesOfPopulation g_Species;


class Population_Manager;
class FarmManager;
class Farmer;
class BroadBeans;
class Carrots;
class CloverGrassGrazed1;
class CloverGrassGrazed2;
class FieldPeas;
class FieldPeasSilage;
class FodderBeet;
class FodderGrass;
class Maize;
class MaizeSilage;
class OMaizeSilage;
class OBarleyPeaCloverGrass;
class OCarrots;
class OCloverGrassGrazed1;
class OCloverGrassGrazed2;
class OCloverGrassSilage1;
class OFieldPeas;
class OFieldPeasSilage;
class OFirstYearDanger;
class OGrazingPigs;
class OOats;
class Oats;
class OrchardCrop;
class OFodderBeet;
class OPermanentGrassGrazed;
class OPotatoes;
class OSBarleySilage;
class OSeedGrass1;
class OSeedGrass2;
class OSpringBarley;
class OSpringBarleyExt;
class OSpringBarleyPigs;
class OWinterBarley;
class OWinterBarleyExt;
class OWinterRape;
class OWinterRye;
class OWinterWheatUndersown;
class OWinterWheatUndersownExt;
class PermanentGrassGrazed;
class PermanentGrassLowYield;
class PermanentGrassTussocky;
class PermanentSetAside;
class Potatoes;
class PotatoesIndustry;
class SeedGrass1;
class SeedGrass2;
class SetAside;
class SpringBarley;
class SpringBarleySpr;
class SpringBarleyPTreatment;
class SpringBarleySKManagement;
class SpringBarleyCloverGrass;
class SpringBarleySeed;
class SpringBarleySilage;
class SpringRape;
class SugarBeet;
class Triticale;
class OTriticale;
class WinterBarley;
class WinterRape;
class WinterRye;
class WinterWheat;
class WWheatPControl;
class WWheatPTreatment;
class WWheatPToxicControl;
class AgroChemIndustryCereal;
class WinterWheatStrigling;
class WinterWheatStriglingSingle;
class WinterWheatStriglingCulm;
class SpringBarleyCloverGrassStrigling;
class SpringBarleyStrigling;
class SpringBarleyStriglingCulm;
class SpringBarleyStriglingSingle;
class MaizeStrigling;
class WinterRapeStrigling;
class WinterRyeStrigling;
class WinterBarleyStrigling;
class FieldPeasStrigling;
class SpringBarleyPeaCloverGrassStrigling;
class YoungForestCrop;
class Hunter;
class OWinterWheat;
class NorwegianPotatoes;
class NorwegianOats;
class NorwegianSpringBarley;

class PLWinterWheat;
class PLWinterRape;
class PLWinterBarley;
class PLWinterTriticale;
class PLWinterRye;
class PLSpringWheat;
class PLSpringBarley;
class PLMaize;
class PLMaizeSilage;
class PLPotatoes;
class PLBeet;
class PLFodderLucerne1;
class PLFodderLucerne2;
class PLCarrots;
class PLSpringBarleySpr;
class PLWinterWheatLate;
class PLBeetSpr;
class PLBeans;

class PTOliveGroveTraditional;
class PTOliveGroveTradOrganic;
class PTOliveGroveIntensive;
class PTOliveGroveSuperIntensive;

class NLBeet;
class NLCarrots;
class NLMaize;
class NLPotatoes;
class NLSpringBarley;
class NLWinterWheat;
class NLCabbage;
class NLTulips;
class NLGrassGrazed1;
class NLGrassGrazed1Spring;
class NLGrassGrazed2;
class NLGrassGrazedLast;
class NLPermanentGrassGrazed;
class NLBeetSpring;
class NLCarrotsSpring;
class NLMaizeSpring;
class NLPotatoesSpring;
class NLSpringBarleySpring;
class NLCabbageSpring;
class NLCatchCropPea;
class NLOrchardCrop;
class NLPermanentGrassGrazedExtensive;
class NLGrassGrazedExtensive1;
class NLGrassGrazedExtensive1Spring;
class NLGrassGrazedExtensive2;
class NLGrassGrazedExtensiveLast;
class GenericCatchCrop;

class UKBeans;
class UKBeet;
class UKMaize;
class UKPermanentGrass;
class UKPotatoes;
class UKSpringBarley;
class UKTempGrass;
class UKWinterBarley;
class UKWinterRape;
class UKWinterWheat;

class PTPermanentGrassGrazed;
class PTWinterWheat;
class PTGrassGrazed;
class PTSorghum;
class PTFodderMix;
class PTTurnipGrazed;
class PTCloverGrassGrazed1;
class PTCloverGrassGrazed2;
class PTTriticale;
class PTOtherDryBeans;
class PTShrubPastures;
class PTCorkOak;
class PTVineyards;
class PTWinterBarley;
class PTBeans;
class PTWinterRye;

class DEOats;
class DESpringRye;
class DEWinterWheat;
class DEMazieSilage;
class DEPotatoes;
class DEMaize;
class DEWinterRye;
class DEWinterBarley;
class DEBeet;
class DEWinterRape;
class DETriticale;
class DECabbage;
class DECarrots;
class DEGrasslandSilageAnnual;
class DEGreenFallow_1year;
class DELegumes;
class DEOCabbages;
class DEOCarrots;
class DEOGrasslandSilageAnnual;
class DEOGreeFallow_1year;
class DEOLegumes;
class DEOMaize;
class DEOMaizeSilage;
class DEOOats;
class DEOPotatoes;
class DEOSpringRye;
class DEOTriticale;
class DEOWinterRye;
class DEOWinterRape;
class DEOWinterBarley;
class DEOWinterWheat;
class DEOSugarBeet;
class DEPermanentGrassGrazed;
class DEOPermanentGrassGrazed;
class DEPermanentGrassLowYield;
class DEWinterWheatlate;
class DEAsparagusEstablishedPlantation;
class DEHerbsPerennial_1year;
class DEHerbsPerennial_after1year;
class DESpringBarley;
class DEOrchard;
class DEPeas;
class DEOPeas;
class DEPotatoesIndustry;
class DEBushFruitPerm;
class DEOBushFruitPerm;
class DEOOrchard;
class DEOPermanentGrassLowYield;
class DEOAsparagusEstablishedPlantation;
class DEOHerbsPerennial_1year;
class DEOHerbsPerennial_after1year;

class DK_Cabbages;
class DK_Carrots;
class DK_Legume;
class DK_Legume_Whole;
class DK_OCabbages;
class DK_OCarrots;
class DK_OLegume;
class DK_OLegume_Whole;
class DK_OLegume_Whole;
class DK_OSugarBeets;
class DK_SugarBeets;
class DK_WinterWheat;
class DK_OWinterWheat;
class DK_CatchCrop;
class DK_SpringBarley;
class DK_OCatchCrop;
class DK_OSpringBarley;
class DK_OLegume_Beans;
class DK_OLegume_Peas;
class DK_OSpringOats;
class DK_OWinterWheat;
class DK_OWinterRye;
class DK_Legume_Beans;
class DK_Legume_Peas;
class DK_Legume_Whole;
class DK_SpringOats;
class DK_WinterWheat;
class DK_WinterRye;
class DK_GrassTussocky_Perm;

class DummyCropPestTesting;

enum class CatchCropTypes : unsigned
{
	cctGenericCatchCrop = 0
};

typedef vector<unsigned >polylist;
typedef vector < Hunter* > HunterList;

struct FarmerAttributes {
	double envA; 
	double riskA;
	int Cooperative;
	int Age;
	double riskSpan;
};

struct FarmFuncsCostBenefits {
	double LabourCost;
	double EconomicCost;
	double RiskAvoidance;
	double EnvImpact;
	FarmManagementCategory ManagementCategory;
};

struct pesticiderecord {
	double m_amounts[ppp_foobar];
	bool m_present;
};

/** \brief Used for storing permanent crop data for the farm rotation */
struct PermCropData {
	TTypesOfVegetation Tov;
	int Pct;
};

/** \brief Used for storing farmers field size vectors */
struct tpct {
	int index;
	int pct;
};

/**
\brief
A struct to hold the information required to trigger a farm event
*/
struct FarmEvent
{
	/**
	* m_lock - if true forces the event to be called (ie no probability or other tests)
	* m_startday - the first day when the next crop starts
	* m_first_year - a flag indicating it is the first year of running or not
	* m_run - keeps a running index of the operations through time
	* m_todo - is the farm operation to be done e.g. dk_sb_autumn_plough
	* m_event - is the tov type for the current crop
	* m_next_tov - is the tov for the next vegetation type in rotation
	* m_field - a pointer to the field in question
	*/
	bool       m_lock;
	int        m_startday;
	bool       m_first_year;
	long       m_run;
	int        m_todo;
	TTypesOfVegetation m_event;
	TTypesOfVegetation m_next_tov;
	LE* m_field;
	bool		m_forcespring;
	bool		m_forcespringOK;

	FarmEvent(TTypesOfVegetation a_event, LE* a_field, int a_todo, long a_run, bool a_lock, int a_start, bool a_first_year, TTypesOfVegetation a_nextcrop, bool a_forcespring, bool a_forcespringOK)
	{
		m_event = a_event;
		m_field = a_field;
		m_todo = a_todo;
		m_run = a_run;
		m_lock = a_lock;
		m_startday = a_start;
		m_first_year = a_first_year;
		m_next_tov = a_nextcrop;
		m_forcespring = a_forcespring;
		m_forcespringOK = a_forcespringOK;
	}
};

/**
\brief
Used during saving farm/hunter information
*/
struct farminfo {
	int m_farmref;
	int m_farmtype;
	int m_farmsize;
	int m_farmarable;
	int m_nofields;
	int m_openfields;
	int m_areaopenfields;
	APoint m_farmcentroid;
	APoint m_farmvalid;
	int m_NoHunters;
};

struct CropStartStruct
{
	int m_stMonth;
	int m_stDay;
};
/** \brief
* A data structure to hold hunter information for distribution
*/
class HunterInfo
{
public:
	/** \brief Unique reference number */
	int refID;
	/** \brief Hunter home x-coord */
	int homeX;
	/** \brief Hunter home y-coord */
	int homeY;
	/** \brief The hunter's is the farm reference number to where he hunts */
	vector<int> FarmHuntRef;
	/** \brief A list of farms that has been tested for duplicates*/
	vector<int> CheckedFarms;

	~HunterInfo() {
		FarmHuntRef.resize( 0 );
		CheckedFarms.resize( 0 );
	}
};

/**
\brief
The base class for all crops
*/
class Crop
{
protected:
  Farm      *m_farm;
  LE        *m_field;
  FarmEvent *m_ev;
  int        m_first_date;
  int        m_count;
  int        m_last_date;
  int		 m_ddegstoharvest;
  int		 m_base_elements_no;
  Landscape *m_OurLandscape;
  /** \brief Used to signal that the crop can be forced to start in spring */
  bool 		m_forcespringpossible = false;
  /** \brief The Crop type in terms of the TTypesOfCrops list (smaller list than tov, no country designation) */
  TTypesOfCrops m_toc;
  /** \brief Contains information on whether this is a winter crop, spring crop, or catch crop that straddles the year boundary (0,1,2) */
  int		m_CropClassification;
  /** \brief Holds a value that shifts test pesticide use by this many days in crops modified to use it */
  static int m_date_modifier;
  /**
  \brief
  Adds an event to this crop management
  */
  void SimpleEvent(long a_date, int a_todo, bool a_lock);

  /**
  \brief
  Adds an event to this crop management without relying on member variables
  */
  void SimpleEvent_(long a_date, int a_todo, bool a_lock, Farm* a_farm, LE* a_field);
  /**
  \brief
  Holds the translation between the farm operation enum for each crop and the farm management category associated with it
  */
  vector<FarmManagementCategory> m_ManagementCategories;
  /**  \brief	  
  Holds the translation between the farm operation enum for each cropand the farm management category associated with it
  */
  bool StartUpCrop(int a_spring, std::vector<std::vector<int>> a_flexdates, int a_todo);

public:
	TTypesOfVegetation m_tov;
	virtual ~Crop() {}
	Crop(TTypesOfVegetation a_tov, TTypesOfCrops a_toc, Landscape* a_L);
	int GetFirstDate( void ) { return m_first_date; }
	virtual bool Do( Farm *a_farm,  LE *a_field, FarmEvent *a_ev );
	/** \brief Chooses the next crop to grow in a field. */
	void ChooseNextCrop (int a_no_dates);
	//* \brief Get method for m_CropClassification */
	int GetCropClassification() { return m_CropClassification; }
	//* \brief Get method for m_CropClassification */
	void SetCropClassification(int a_classification) { m_CropClassification = a_classification; }
	//* \brief Sets the crop type */
	void GetCropType(TTypesOfCrops a_toc) { m_toc = a_toc; }
	//* \brief Gets the crop type */
	TTypesOfCrops GetCropType() { return m_toc; }
	//* \brief Get the translation between farm operation and management category */
	FarmManagementCategory LookUpManagementCategory(int a_todo) {
		return m_ManagementCategories[a_todo];
	}
	bool GetForceSpringOK() { return m_forcespringpossible; }
};


/** \brief Struct for storing ALMaSS crop type (#TTypesOfVegetation) with a corresponding value (mainly crop area).*/

// v. similar to PermCropData, but has double instead of int. 
//Used originally for saving Bedriftsmodel (original farm optimization model) type crops in almass type. Also used to store data on biomass_factors
//so changed the name of the member - from Pct to Number

	struct AlmassCrop {
		double Number;
		TTypesOfVegetation Tov;
	};

/**
\brief
Class for storing data for optimisation.
*/
class DataForOptimisation {

public:
	DataForOptimisation();

	//Data at a general level
	void Set_livestockTypes(TTypesOfFarmAnimals livestock_type){m_livestockTypes.push_back(livestock_type);};
	void Set_cropTypes(TTypesOfCrops crop_type){m_cropTypes.push_back(crop_type);};
	void Set_cropTypes_str (string crop_type){m_str_cropTypes.push_back(crop_type);};
	TTypesOfFarmAnimals Get_livestockTypes(int i){return m_livestockTypes[i];};
	string Get_cropTypes_str (int i) {return m_str_cropTypes[i];};
	TTypesOfCrops Get_cropTypes(int i) {return m_cropTypes[i];};

	//Individual data for farms (vector size is no_farms x given_Foobar)
	void Set_livestockNumber(int a_number){m_livestockNumbers.push_back(a_number);}; 
	void Set_cropArea(double a_area){m_cropAreas.push_back(a_area);};
	int Get_livestockNumber(int index){return m_livestockNumbers[index];}; 
	int Get_livestockNumbersSize(){return (int)m_livestockNumbers.size();};
	double Get_cropArea(int index){return m_cropAreas[index];};
	int Get_cropAreasSize(){return (int)m_cropAreas.size();};

	//Data for farms
	/** \brief Used by FarmManager::CreateFarms. Finds a matching almass number and returns farm's type.*/
	TTypesOfOptFarms Get_farmType (int a_almass_no);
	void Set_winterMax (int a_value, int i){m_winterMax[i]=a_value;};
	int Get_winterMax (int i) {return m_winterMax[i];};

	//Data for livestock
	void Set_livestockParameter(double a_value, int i){m_livestockParameters[i]=a_value;};
	double Get_livestockParameter(int i){return m_livestockParameters[i];};

	//Data for crops
	void Set_cropParameter (double a_value, int i){m_cropParameters[i]=a_value;};
	double Get_cropParameter (int i) {return m_cropParameters[i];};

	void Set_alfa (double a_value, int i){m_alfa[i]=a_value;};
	double Get_alfa (int i) {return m_alfa[i];};
	void Set_beta1 (double a_value, int i){m_beta1[i]=a_value;};
	double Get_beta1 (int i) {return m_beta1[i];};
	void Set_beta2 (double a_value, int i){m_beta2[i]=a_value;};
	double Get_beta2 (int i) {return m_beta2[i];};
	void Set_Nnorm (double a_value, int i){m_Nnorm[i]=a_value;};
	double Get_Nnorm (int i) {return m_Nnorm[i];};


	void Set_fixed (string a_value, int i){
		if(a_value=="yes" || a_value=="YES" || a_value=="Yes" || a_value=="1"){m_fixed[i]=true;}
		else {m_fixed[i]=false;}
	};
	bool Get_fixed (int i) {return m_fixed[i];};
	void Set_fodder (string a_value, int i){
		if(a_value=="yes" || a_value=="YES" || a_value=="Yes" || a_value=="1"){m_fodder[i]=true;}
		else {m_fodder[i]=false;}
	};
	bool Get_fodder (int i) {return m_fodder[i];};
	void Set_FUKey (double a_value, int i){m_FUKey[i]=a_value;};
	double Get_FUKey (int i) {return m_FUKey[i];};

	void Set_sellingPrice (double a_value, int i){m_sellingPrice[i]=a_value;};
	double Get_sellingPrice (int i) {return m_sellingPrice[i];};
	void Set_sellingPrice_lastyr (double a_value, int i){m_sellingPrice_lastyr[i]=a_value;};
	double Get_sellingPrice_lastyr (int i) {return m_sellingPrice_lastyr[i];};

	void Set_rotationMax (double a_value, int i){m_rotationMax[i]=a_value;};
	double Get_rotationMax (int i) {return m_rotationMax[i];};
	void Set_rotationMin (double a_value, int i){m_rotationMin[i]=a_value;};
	double Get_rotationMin (int i) {return m_rotationMin[i];};


	//versions for almass crops
	void Set_biomass_factor (double a_value, int i){m_biomass_factors[i] = a_value;};
	double Get_biomass_factor (int i) { return m_biomass_factors[i];} 

	void Set_cropTypes_almass(TTypesOfVegetation crop_type){m_cropTypes_almass.push_back(crop_type);};
	TTypesOfVegetation Get_cropTypes_almass (int i) {return m_cropTypes_almass[i];};
	int Get_cropTypes_almass_size (){return (int)m_cropTypes_almass.size();};
	void Set_cropTypes_almass_str (string crop_type){m_str_cropTypes_almass.push_back(crop_type);};
	string Get_cropTypes_almass_str (int i) {return m_str_cropTypes_almass[i];};

	void Set_noCrops (int no) {m_noCrops = no;};
	int Get_noCrops (){return m_noCrops;};

	void Set_emaize_price (double a_value){m_energy_maize_price.push_back(a_value);};
	double Get_emaize_price (int i){return m_energy_maize_price[i];};

	/** \brief Class storing farm information: farm's number used in ALMaSS system, farm's soil type, farm's size (business or private) and farm's real ID number.*/
	class FarmData{
	public:
		int md_almass_no;
		TTypesOfOptFarms md_farmType;
		TTypesOfSoils md_soilType;
		TTypesOfFarmSize md_farmSize;
		int md_farmRealID;
		int md_soilSubType;
	};

	vector<FarmData*>m_farm_data;


protected:

	/** \brief Initializes vector with -1 values.*/ //called in the constructor
    void InitializeVector(vector<double>&vector);
	/** \brief Number of crops used in the simulation.*/
	int m_noCrops; //how many crops will be optimised - the number is taken from one of the input files; it has to be the same in all input files with crop data!

//I. Data at a general level - changed to config variables

//II. Data for farms

	/** \brief Crop types saved in string format (Bedriftsmodel i.e. original crop optimization model crop mode).*/
	vector<string>m_str_cropTypes; //used in output function
	/** \brief Crop types saved as enumarator types (Bedriftsmodel i.e. original crop optimization model crop mode).*/
	vector<TTypesOfCrops>m_cropTypes;
	//for almass crop types
	/** \brief Crop types saved in string format (ALMaSS crop mode).*/
	vector<string>m_str_cropTypes_almass;
	/** \brief Crop types saved as enumarator types (ALMaSS crop mode).*/
	vector<TTypesOfVegetation>m_cropTypes_almass;
	/** \brief Livestock types saved as enumarator types.*/
	vector<TTypesOfFarmAnimals>m_livestockTypes;
	/** \brief Vector containing values of initial (i.e. in year the data is taken for) crop areas at farms included in the simulation.*/
	vector<double>m_cropAreas;
	/** \brief Vector containing numbers of all livestock types at farms included in the simulation.*/
	vector<int>m_livestockNumbers;

	/** \brief Maximal area taken by winter crops at a farm. [%]*/
	vector<int>m_winterMax;

//III. Data for livestock
	/** \brief Vector storing parameters of all livestock types.*/
	vector<double>m_livestockParameters; //full list of the parameters - enum declaration (TTypesOfLivestockParameters)

//IV. Data for crops
	//1. parameters that do not vary with farm type, soil type etc. (number of parameters = top_Foobar !)
		/** \brief Vector storing parameters of all crop types.*/
		vector<double>m_cropParameters; //full list of the parameters - enum declaration (enum TTypesOfParameters)
	//2. parameters that vary with a soil type
		/** \brief Crop parameter for response (growth) function (resp = alfa + beta1*N + beta2*N^2; defines relationship between yield and fertilizer). Soil type specific.*/
		vector<double>m_alfa;
		/** \brief Crop parameter for response (growth) function (resp = alfa + beta1*N + beta2*N^2; defines relationship between yield and fertilizer). Soil type specific.*/
		vector<double>m_beta1;
		/** \brief Crop parameter for response (growth) function (resp = alfa + beta1*N + beta2*N^2; defines relationship between yield and fertilizer). Soil type specific.*/
		vector<double>m_beta2;
		/** \brief Maximum amount of fertilizer (N) that can be applied for a given crop [kg N/ha]. Soil specific.*/
		vector<double>m_Nnorm;
		/** \brief Factor used to determine actual response (yield per ha) based on crop biomass at a harvest. Factor is crop specific and is equal to a ratio of the response 
		* for optimal fertilizer amount in Bedriftsmodel (original farm optimization model) to the biomass at a harvest for the corresponding ALMaSS crop grown with optimal fertilizer input.*/
		vector<double>m_biomass_factors;

	//3. parameters that vary with a farm type
		/** \brief Crop Boolean parameter - fixed/variable crop. Farm type specific.*/
		vector<bool>m_fixed;
		/** \brief Crop Boolean parameter - fodder/non-fodder crop. Farm type specific.*/
		vector<bool>m_fodder;
		/** \brief Crop parameter: Fodder unit key, i.e. a number of fodder units obtained from a given crop [FU/hkg]. Farm type specific.*/
		vector<double>m_FUKey;
	//4. parameters that vary with a farm and soil type
		/** \brief Selling price of a crop [DKK/hkg]. Farm type specific and soil type specific.*/
		vector<double>m_sellingPrice;
		/** \brief Selling price of a crop in a previous year [DKK/hkg]. Farm type specific and soil type specific.*/
		vector<double>m_sellingPrice_lastyr;
	//5. parameters that vary with a farm type, farm size and a soil type
		/** \brief Maximum acreage of a crop at a farm [%]. Farm type, soil type and farm size specific.*/
		vector<double>m_rotationMax;
		/** \brief Minimum acreage of a crop at a farm [%]. Farm type, soil type and farm size specific.*/
		vector<double>m_rotationMin;

	//6. Energy maize runs
		/** \brief Vector with energy maize prices for each year of simulation.*/
		vector<double>m_energy_maize_price;

};

/**
\brief
The base class for all farm types
*/
class Farm
{
public:

	string m_rotfilename;

	/** \brief FarmActions() will use the global date to manage all queued field events */
	virtual void FarmActions(void);
	/** \brief The starting point for farmer behaviour not part of crop management e.g. decision making */
	bool FarmerActions(void);

	// InitiateManagement() must be called after all the fields have
	// been added to the farm.
	virtual void InitiateManagement(void);

	void     AddField(LE* a_newfield);
	void     RemoveField(LE* a_field);
	Farm(FarmManager* a_manager);
	virtual ~Farm(void);
	void SetFarmNumber(int a_farm_num) { m_farm_num = a_farm_num; }
	int  GetFarmNumber(void) { return m_farm_num; }
	virtual bool Spraying_herbicides(TTypesOfVegetation) { return true; };
	virtual bool Spraying_fungins(TTypesOfVegetation) { return true; };
	virtual double Prob_multiplier() { return 1; };
	void Assign_rotation(vector<TTypesOfVegetation>a_new_rotation);
	FarmManager* GetFarmManager() { return m_OurManager; }
	/** \brief Returns the farm type */
	TTypesOfFarm GetFarmType() { return m_FarmType; }
	/** \brief Returns a list of fields with openness above a_openness */
	polylist* ListOpenFields(int a_openness);
	/** \brief Finds farm's centroids - x and y.*/ //14.01.13
	void Centroids();
	/** \brief Returns the number of the fields owned */
	int GetNoFields() { return (int)m_fields.size(); }
	/** \brief Returns the number of the fields above an openness of a_openness */
	int GetNoOpenFields(int a_openness);
	/** \brief Returns the area of the fields above an openness of a_openness */
	int GetAreaOpenFields(int a_openness);
	/** \brief Returns the valid coordinates of the first field owned by a farm */
	APoint GetValidCoords() { APoint pt;  pt.m_x = m_fields[0]->GetValidX(); pt.m_y = m_fields[0]->GetValidY(); return pt; }
	/** \brief Returns the maximum openness score of the fields */
	int GetMaxOpenness()
	{
		int op = 0;
		for (int i = 0; i < m_fields.size(); i++)
		{
			int openness = m_fields[i]->GetOpenness();
			if (openness > op) op = openness;
		}
		return op;
	}


	// And (*sigh*) the crop treatment functions... :-)
	  /** \brief carries out standard setting of data and deals with possible unsprayed margins, returns a pointer to UM or nullptr */
	LE* SetFunctionData(LE*, double, double, int, FarmToDo);
	/** \brief carries out standard setting of data when UMs should be ignored  */
	void SetFunctionDataNoUM(LE*, double, double, int, FarmToDo);
	virtual bool SleepAllDay(LE* a_field, double a_user, int a_days);
	virtual bool AutumnPlough(LE* a_field, double a_user, int a_days);
	virtual bool StubblePlough(LE* a_field, double a_user, int a_days);
	virtual bool StubbleCultivatorHeavy(LE* a_field, double a_user, int a_days);
	virtual bool AutumnHarrow(LE* a_field, double a_user, int a_days);
	virtual bool AutumnRoll(LE* a_field, double a_user, int a_days);
	virtual bool PreseedingCultivator(LE* a_field, double a_user, int a_days);
	virtual bool PreseedingCultivatorSow(LE* a_field, double a_user, int a_days, double a_seed_coating_amount = -1, PlantProtectionProducts a_ppp = ppp_foobar);
	virtual bool AutumnSow(LE* a_field, double a_user, int a_days, double a_seed_coating_amount = -1, PlantProtectionProducts a_ppp = ppp_foobar);
	virtual bool WinterPlough(LE* a_field, double a_user, int a_days);
	virtual bool WinterHarrow(LE* a_field, double a_user, int a_days);
	virtual bool DeepPlough(LE* a_field, double a_user, int a_days);
	virtual bool SpringPlough(LE* a_field, double a_user, int a_days);
	virtual bool SpringHarrow(LE* a_field, double a_user, int a_days);
	virtual bool SpringRoll(LE* a_field, double a_user, int a_days);
	virtual bool SpringSow(LE* a_field, double a_user, int a_days, double a_seed_coating_amount = -1, PlantProtectionProducts a_ppp = ppp_foobar);
	virtual bool SpringSowWithFerti(LE* a_field, double a_user, int a_days, double a_seed_coating_amount = -1, PlantProtectionProducts a_ppp = ppp_foobar);
	virtual bool AutumnSowWithFerti(LE* a_field, double a_user, int a_days, double a_seed_coating_amount = -1, PlantProtectionProducts a_ppp = ppp_foobar);
	virtual bool HerbicideTreat(LE* a_field, double a_user, int a_days);
	virtual bool GrowthRegulator(LE* a_field, double a_user, int a_days);
	virtual bool FungicideTreat(LE* a_field, double a_user, int a_days);
	virtual bool InsecticideTreat(LE* a_field, double a_user, int a_days);
	virtual bool BiocideTreat(LE* a_field, double a_user, int a_days);
	virtual bool OrganicInsecticide(LE* a_field, double a_user, int a_days);
	virtual bool OrganicHerbicide(LE* a_field, double a_user, int a_days);
	virtual bool OrganicFungicide(LE* a_field, double a_user, int a_days);
	virtual bool Pheromone(LE* a_field, double a_user, int a_days);
	virtual bool Molluscicide(LE* a_field, double a_user, int a_days);
	virtual bool RowCultivation(LE* a_field, double a_user, int a_days);
	virtual bool Strigling(LE* a_field, double a_user, int a_days);
	virtual bool StriglingSow(LE* a_field, double a_user, int a_days, double a_seed_coating_amount = -1, PlantProtectionProducts a_ppp = ppp_foobar);
	virtual bool StriglingHill(LE* a_field, double a_user, int a_days);
	virtual bool HillingUp(LE* a_field, double a_user, int a_days);
	virtual bool Water(LE* a_field, double a_user, int a_days);
	virtual bool Swathing(LE* a_field, double a_user, int a_days);
	virtual bool Harvest(LE* a_field, double a_user, int a_days);
	virtual bool HarvestLong(LE* a_field, double a_user, int a_days);
	virtual bool HarvestBushFruit(LE* a_field, double a_user, int a_days);
	virtual bool HarvestBF_Machine(LE* a_field, double a_user, int a_days);
	virtual bool CattleOut(LE* a_field, double a_user, int a_days);
	virtual bool CattleOut(LE* a_field, double a_user, int a_days, bool a_should_eat);
	virtual bool CattleOutLowGrazing(LE* a_field, double a_user, int a_days);
	virtual bool CattleIsOut(LE* a_field, double a_user, int a_days, int a_max);
	virtual bool CattleIsOut(LE* a_field, double a_user, int a_days, int a_max, bool a_should_eat);
	virtual bool CattleIsOutLow(LE* a_field, double a_user, int a_days, int a_max);
	virtual bool CattleIsOutLow2(LE* a_field, double a_user, int a_days, int a_max, int a_max_days);
	virtual bool PigsOut(LE* a_field, double a_user, int a_days);
	virtual bool PigsAreOut(LE* a_field, double a_user, int a_days);
	virtual bool PigsAreOutForced(LE* a_field, double a_user, int a_days);
	virtual bool CutToHay(LE* a_field, double a_user, int a_days);
	virtual bool CutWeeds(LE* a_field, double a_user, int a_days);
	virtual bool CutToSilage(LE* a_field, double a_user, int a_days);
	virtual bool CutOrch(LE* a_field, double a_user, int a_days); //added 27.08.12
	virtual bool StrawChopping(LE* a_field, double a_user, int a_days);
	virtual bool HayTurning(LE* a_field, double a_user, int a_days);
	virtual bool HayBailing(LE* a_field, double a_user, int a_days);
	virtual bool BurnStrawStubble(LE* a_field, double a_user, int a_days);
	virtual bool BurnTop(LE* a_field, double a_user, int a_days);
	virtual bool StubbleHarrowing(LE* a_field, double a_user, int a_days);
	virtual bool FP_NPKS(LE* a_field, double a_user, int a_days);
	virtual bool FP_NPK(LE* a_field, double a_user, int a_days);
	virtual bool FP_NC(LE* a_field, double a_user, int a_days);
	virtual bool FP_NK(LE* a_field, double a_user, int a_days);
	virtual bool FP_NS(LE* a_field, double a_user, int a_days);
	virtual bool FP_N(LE* a_field, double a_user, int a_days);
	virtual bool FP_PK(LE* a_field, double a_user, int a_days);
	virtual bool FP_P(LE* a_field, double a_user, int a_days);
	virtual bool FP_K(LE* a_field, double a_user, int a_days);
	virtual bool FP_SK(LE* a_field, double a_user, int a_days);
	virtual bool FP_LiquidNH3(LE* a_field, double a_user, int a_days);
	virtual bool FP_Slurry(LE* a_field, double a_user, int a_days);
	virtual bool FP_ManganeseSulphate(LE* a_field, double a_user, int a_days);
	virtual bool FP_Boron(LE* a_field, double a_user, int a_days);
	virtual bool FP_AmmoniumSulphate(LE* a_field, double a_user, int a_days);
	virtual bool FP_Manure(LE* a_field, double a_user, int a_days);
	virtual bool FP_GreenManure(LE* a_field, double a_user, int a_days);
	virtual bool FP_Sludge(LE* a_field, double a_user, int a_days);
	virtual bool FP_RSM(LE* a_field, double a_user, int a_days);
	virtual bool FP_Calcium(LE* a_field, double a_user, int a_days);
	virtual bool FA_NK(LE* a_field, double a_user, int a_days);
	virtual bool FA_NPKS(LE* a_field, double a_user, int a_days);
	virtual bool FA_NPK(LE* a_field, double a_user, int a_days);
	virtual bool FA_PK(LE* a_field, double a_user, int a_days);
	virtual bool FA_P(LE* a_field, double a_user, int a_days);
	virtual bool FA_K(LE* a_field, double a_user, int a_days);
	virtual bool FA_SK(LE* a_field, double a_user, int a_days);
	virtual bool FA_Slurry(LE* a_field, double a_user, int a_days);
	virtual bool FA_ManganeseSulphate(LE* a_field, double a_user, int a_days);
	virtual bool FA_AmmoniumSulphate(LE* a_field, double a_user, int a_days);
	virtual bool FA_Manure(LE* a_field, double a_user, int a_days);
	virtual bool FA_GreenManure(LE* a_field, double a_user, int a_days);
	virtual bool FA_Sludge(LE* a_field, double a_user, int a_days);
	virtual bool FA_RSM(LE* a_field, double a_user, int a_days);
	virtual bool FA_Calcium(LE* a_field, double a_user, int a_days);
	virtual bool BedForming(LE* a_field, double a_user, int a_days);
	virtual bool ShallowHarrow(LE* a_field, double a_user, int a_days);
	virtual bool HeavyCultivatorAggregate(LE* a_field, double a_user, int a_days);
	virtual bool FlowerCutting(LE* a_field, double a_user, int a_days);
	virtual bool BulbHarvest(LE* a_field, double a_user, int a_days);
	virtual bool StrawCovering(LE* a_field, double a_user, int a_days);
	virtual bool StrawRemoval(LE* a_field, double a_user, int a_days);
	virtual bool IrrigationStart(LE* a_field, double a_user, int a_days);
	virtual bool Irrigation(LE* a_field, double a_user, int a_days, int a_max);
	virtual bool Pruning(LE* a_field, double a_user, int a_days);
	virtual bool Shredding(LE* a_field, double a_user, int a_days);
	virtual bool LeafThinning(LE* a_field, double a_user, int a_days);
	virtual bool GreenHarvest(LE* a_field, double a_user, int a_days);
	virtual bool FruitHarvest(LE* a_field, double a_user, int a_days);
	virtual bool Suckering(LE* a_field, double a_user, int a_days);
	virtual bool Mowing(LE* a_field, double a_user, int a_days);
	virtual bool FiberCovering(LE* a_field, double a_user, int a_days);
	virtual bool FiberRemoval(LE* a_field, double a_user, int a_days);
	virtual bool FP_Cu(LE* a_field, double a_user, int a_days);
	virtual bool FA_N(LE* a_field, double a_user, int a_days);
	virtual bool FA_Cu(LE* a_field, double a_user, int a_days);
	virtual bool FA_Boron(LE* a_field, double a_user, int a_days);
	virtual bool FA_PKS(LE* a_field, double a_user, int a_days);
	virtual bool FP_PKS(LE* a_field, double a_user, int a_days);
	virtual bool HarvestShoots(LE* a_field, double a_user, int a_days);
	virtual bool ManualWeeding(LE* a_field, double a_user, int a_days);

	// User functions for the treatment steps:
	void AddNewEvent(TTypesOfVegetation a_event, long a_date,
		LE* a_field, int a_todo, long a_num,
		bool a_lock, int a_start,
		bool a_first_year, TTypesOfVegetation a_crop, FarmManagementCategory a_fmc,
		bool a_forcespring, bool a_forcespringOK);
	bool DoIt(double a_probability);
	bool DoIt_prob(double a_probability);
	TTypesOfFarm  GetType(void) { return m_farmtype; }
	string GetRotFilename(void) { return m_rotfilename; }
	int GetArea(void);
	int GetTotalArea(void);
	double GetAreaDouble(void);
	bool IsStockFarmer(void) { return m_stockfarmer; }
	virtual void MakeStockFarmer(void) { m_stockfarmer = true; }
	double GetIntensity(void) { return m_intensity; }
	void SetIntensity(double a_intensity) { m_intensity = a_intensity; }
	APoint GetCentroids() { APoint pt; pt.m_x = m_farm_centroidx; pt.m_y = m_farm_centroidy; return pt; }
	TTypesOfVegetation GetPreviousTov(int a_index) {
		int ind = a_index - 1;
		if (ind < 0) ind = (int)m_rotation.size() - 1;
		return m_rotation[ind];
	}
	TTypesOfVegetation GetCrop(int a_index) {
		return m_rotation[a_index];
	}
	TTypesOfVegetation GetNextCrop(int a_index) {
		int ind = a_index + 1;
		if (ind >= (int)m_rotation.size()) ind = ind - (int)m_rotation.size();
		return m_rotation[ind];
	}
	int GetNoCrops() {
		return (int)m_rotation.size();
	}
	void AddHunter(Hunter* a_hunter) {
		m_HuntersList.push_back(a_hunter);
	}
	void RemoveHunter(Hunter* a_hunter) {
		for (int h = 0; h < m_HuntersList.size(); h++) {
			if (m_HuntersList[h] == a_hunter) {
				m_HuntersList.erase(m_HuntersList.begin() + h);
			}
		}
	}

	int GetNextCropStartDate(LE* a_field, TTypesOfVegetation& a_curr_veg);

protected:
	/**\brief Pointer to the FarmManager.*/
	FarmManager* m_OurManager;
	LowPriority< FarmEvent* >  m_queue;
	vector< LE* >              m_fields;
	vector<TTypesOfVegetation> m_rotation;
	vector<PermCropData>		m_PermCrops;
	TTypesOfFarm  m_farmtype;
	/** \brief A list of hunters allocated to this farm */
	HunterList m_HuntersList;
	bool m_stockfarmer;
	int m_farm_num;
	int m_rotation_sync_index;
	/** \brief indicates the farm intensity - 1 = normal, otherwise linear scale */
	double m_intensity;
	int GetFirstDate(TTypesOfVegetation a_tov);
	int GetForceSpringOK(TTypesOfVegetation a_tov);
	virtual int GetFirstCropIndex(TTypesOfLandscapeElement a_type);
	virtual int GetNextCropIndex(int a_rot_index);
	virtual void HandleEvents(void);
	bool LeSwitch(FarmEvent* ev);
	void CheckRotationManagementLoop(FarmEvent* ev);
	void ReadRotation(std::string fname);

	void AssignPermanentCrop(TTypesOfVegetation tov, int pct); //MOVED FROM THE  USER DEFINED FARM
	int InvIntPartition(vector<tpct>* items, int target);
	/** \brief Farm's centroid, value x. Equal to the average of the x centroid values of all farm's fields.*/
	int m_farm_centroidx;
	/** \brief Farm's centroid, value y. Equal to the average of the y centroid values of all farm's fields.*/
	int m_farm_centroidy;
	/** \brief The farms farm type - see TTypesOfFarm enum for types - note the use of UserDefined farms */
	TTypesOfFarm m_FarmType; // default is tof_foobar

};


/**
\brief Inbuilt farm type
*/
class ConventionalCattle : public Farm
{
public:
  ConventionalCattle( FarmManager* a_manager );
//protected:
//  virtual TTypesOfVegetation GetFirstCrop( void );
//  virtual TTypesOfVegetation GetNextCrop( TTypesOfVegetation a_veg );
};

/**
\brief Inbuilt farm type
*/
class ConventionalPig : public Farm
{
public:
	ConventionalPig(FarmManager* a_manager);
//protected:
//  virtual TTypesOfVegetation GetFirstCrop( void );
//  virtual TTypesOfVegetation GetNextCrop( TTypesOfVegetation a_veg );
};

/**
\brief Inbuilt farm type
*/
class ConventionalPlant : public Farm
{
public:
	ConventionalPlant(FarmManager* a_manager);
  virtual void MakeStockFarmer( void ) { m_stockfarmer = false; }
//protected:
//  virtual TTypesOfVegetation GetFirstCrop( void );
//  virtual TTypesOfVegetation GetNextCrop( TTypesOfVegetation a_veg );
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class OrganicPlant : public Farm
{
public:
	OrganicPlant(FarmManager* a_manager);
  virtual void MakeStockFarmer( void ) { m_stockfarmer = false; }
//protected:
//  virtual TTypesOfVegetation GetFirstCrop( void );
//  virtual TTypesOfVegetation GetNextCrop( TTypesOfVegetation a_veg );
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class OrganicPig : public Farm
{
public:
	OrganicPig(FarmManager* a_manager);
//protected:
//  virtual TTypesOfVegetation GetFirstCrop( void );
//  virtual TTypesOfVegetation GetNextCrop( TTypesOfVegetation a_veg );
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class OrganicCattle : public Farm
{
public:
	OrganicCattle(FarmManager* a_manager);
//protected:
//  virtual TTypesOfVegetation GetFirstCrop( void );
//  virtual TTypesOfVegetation GetNextCrop( TTypesOfVegetation a_veg );
};

/**
\brief Inbuilt special purpose farm type
*/
class PesticideTrialControl : public Farm
{
public:
	PesticideTrialControl(FarmManager* a_manager);
  virtual void MakeStockFarmer( void ) { m_stockfarmer = false; }
};

/**
\brief Inbuilt special purpose farm type
*/
class PesticideTrialToxicControl : public Farm
{
public:
	PesticideTrialToxicControl(FarmManager* a_manager);
  virtual void MakeStockFarmer( void ) { m_stockfarmer = false; }
};

/**
\brief Inbuilt special purpose farm type
*/
class PesticideTrialTreatment : public Farm
{
public:
	PesticideTrialTreatment(FarmManager* a_manager);
  virtual void MakeStockFarmer( void ) { m_stockfarmer = false; }
};



/**
\brief Inbuilt special purpose farm type
*/
class ConvMarginalJord : public Farm
{
public:
	ConvMarginalJord(FarmManager* a_manager);
};


/**
\brief Inbuilt special purpose farm type
*/
class AgroChemIndustryCerealFarm1 : public Farm
{
public:
	AgroChemIndustryCerealFarm1(FarmManager* a_manager);
};

/**
\brief Inbuilt special purpose farm type
*/
class AgroChemIndustryCerealFarm2 : public Farm
{
public:
	AgroChemIndustryCerealFarm2(FarmManager* a_manager);
};

/**
\brief Inbuilt special purpose farm type
*/
class AgroChemIndustryCerealFarm3 : public Farm
{
public:
	AgroChemIndustryCerealFarm3(FarmManager* a_manager);
};

/**
\brief Inbuilt special purpose farm type
*/
class NoPesticideBaseFarm : public Farm
{
public:
	NoPesticideBaseFarm(FarmManager* a_manager);
};

/**
\brief Inbuilt special purpose farm type
*/
class NoPesticideNoPFarm : public Farm
{
public:
	NoPesticideNoPFarm(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
/**
The rotation is controlled by a file with the format:
N\n
Crop\n
Crop\n
Crop...\n
..........where N is the number of entries and "Crop" is replaced by the crop name for each entry in the rotation. There are 16 Userdefined farm types available numbered 1 to 16, but 2->16 have been excluded from the documentation because they are identical to this class in all but name.\n
*/
class UserDefinedFarm : public Farm
{
public:
	UserDefinedFarm(TTypesOfFarm farm_type, const char* fname, FarmManager* a_manager);
protected:
	virtual void InitiateManagement( void );
	//void AssignPermanentCrop(TTypesOfVegetation tov, int pct); //MOVED TO THE FARM CLASS
	//int InvIntPartition(vector<tpct>* items, int target);
	//void TranslateBitsToFields(int bits, vector<LE*> &f_cpy, TTypesOfVegetation tov);
};

class UserDefinedFarm1 : public Farm
{
public:
	UserDefinedFarm1(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm2 : public Farm
{
public:
	UserDefinedFarm2(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm3 : public Farm
{
public:
	UserDefinedFarm3(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm4 : public Farm
{
public:
	UserDefinedFarm4(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm5 : public Farm
{
public:
	UserDefinedFarm5(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm6 : public Farm
{
public:
	UserDefinedFarm6(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm7 : public Farm
{
public:
	UserDefinedFarm7(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm8 : public Farm
{
public:
	UserDefinedFarm8(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm9 : public Farm
{
public:
	UserDefinedFarm9(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm10 : public Farm
{
public:
	UserDefinedFarm10(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm11 : public Farm
{
public:
	UserDefinedFarm11(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm12 : public Farm
{
public:
	UserDefinedFarm12(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm13 : public Farm
{
public:
	UserDefinedFarm13(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm14 : public Farm
{
public:
	UserDefinedFarm14(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm15 : public Farm
{
public:
	UserDefinedFarm15(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm16 : public Farm
{
public:
	UserDefinedFarm16(FarmManager* a_manager);
};

/**
\brief A farm that can have its rotation defined by the user at runtime.
*/
class UserDefinedFarm17 : public Farm
{
public:
	UserDefinedFarm17(FarmManager* a_manager);
};

//--------------------------------------------------------------------------------------------
//--------------------------OptimisingFarm----------------------------------------------------
//--------------------------------------------------------------------------------------------

/**
\brief A class for storing livestock parameters and variables for optimising farms.
*/
class Livestock {
	public:
		/** \brief Constructor*/
		Livestock(TTypesOfFarmAnimals a_animalType, int a_number);
		/** \brief Type of livestock.*/
		TTypesOfFarmAnimals m_animalType;
		/** \brief Number of animals of a given type at a farm.*/
        int m_number;
        /** \brief Amount of usable animal fertilizer from a given type of livestock. [kg]*/
        double m_NanimUsable;
        /** \brief Amount of fodder needed for a given type of livestock. [fodder units]*/
        double m_FUdemand;
	};

/**
\brief A class for storing all parameters and results of crop optimisation.
*/
class CropOptimised {
	public:
		/** \brief Constructor*/
		CropOptimised(TTypesOfCrops a_cropType, double a_initialArea);
		CropOptimised(TTypesOfVegetation a_cropType, double a_initialArea); 
		CropOptimised(); //another constructor - for the fake crop

		/** \brief Initial area of a crop on a farm [ha].*/
		double m_initialArea;
		/** \brief Type/name of a crop (original farm optimization model crop types).*/
		TTypesOfCrops m_cropType;
		/** \brief Type/name of a crop (ALMaSS crops).*/
		TTypesOfVegetation m_cropType_almass;

	//parameters: here saved - to make it easier to read them (they are needed very often)
		/** \brief Maximum area in percent of farm's arable area for a given crop (depends on a farm size, farm type and soil type).*/
		double m_rotationMax;
		/** \brief Minimum area in percent of farm's arable area for a given crop (depends on a farm size, farm type and soil type).*/
		double m_rotationMin;

	//optimised variables
		/** \brief Total amount of fertilizer applied per ha of a crop [kg N/ha].*/
        double m_n;
        /** \brief Amount of purchased (and applied) fertilizer per ha of a crop [kg N/ha].*/
        double m_nt;
         /** \brief Optimal amount of fertilizer per ha of a crop supposing ferilizer price equals zero [kg N/ha]. Used in #OptimisingFarm::ActualProfit.*/
		double m_optimalN;

		/** \brief A value of selling price for non-fodder crops or a value of fodder units obtained from a hkg of a fodder crop [DKK/hkg].*/
		double m_benefit;
		/** \brief Response - yield of a crop per ha [hkg/ha].*/
        double m_resp;
        /** \brief Value of BI for herbicides [BI/ha].*/
        double m_BIHerb;
        /** \brief Value of BI for fung- and insecticides [BI/ha].*/
        double m_BIFi;
        /** \brief Summary value of BI for herbicides and fung- and insecticides [BI/ha].*/
        double m_BI;
        /** \brief Value of mechanical weed control for a crop - grooming [DKK/ha].*/
        double m_grooming;
        /** \brief Value of mechanical weed control for a crop - hoeing [DKK/ha].*/
        double m_hoeing;
        /** \brief Value of mechanical weed control for a crop - manual weeding [DKK/ha].*/ //!!!!! in bedriftsmodel data it says it is dkk per time...see more on this 10.07.13
        double m_weeding;
        /** \brief Value of the yield loss due to the limited use of herbicides [%].*/
        double m_lossHerb;
        /** \brief Value of the yield loss due to the limited use of fung- and insecticides [%].*/
        double m_lossFi;
        /** \brief Summary value of the yield loss due to the limited use of herbicides and fung- and insecticides [%].*/
        double m_totalLoss;
        /** \brief Costs of growing 1 ha of a crop. Include costs of labour and machine (constant), pesticides (herbicides and fung- and insecticides), mechanical weed control, fertilizer [DKK/ha].*/
        double m_costs_ha;
        /** \brief Value of income per ha of a crop. Includes value of sales and subsidy [DKK/ha].*/
        double m_income_ha;
        /** \brief Value of gross margin for a crop (#m_income_ha - #m_costs_ha) [DKK/ha].*/
        double m_GM;
        /** \brief Value of savings resulting from growing a fodder crop and not purchasing amount of fodder corresponding to the amount of fodder obtained from 1 ha of a given fodder crop [DKK/ha].*/
        double m_savings; 
        /** \brief Holds the value of #m_GM in case of non-fodder crops and a value of #m_savings in case of fodder crops [DKK/ha].*/
        double m_GM_Savings;
        /** \brief Area of a crop in percent of a farm's total area [%].*/
        double m_areaPercent;
        /** \brief Area of a crop that can be changed when checking for restrictions (=#m_areaPercent - #m_rotationMin) [%].*/
        double m_areaVariable;
        /** \brief Area of a crop in ha at a farm [ha].*/
        double m_area_ha;
};

/**
\brief
The Farm Manager class
*/
class FarmManager
{
public:
	/** \brief Farm manager constructor */
	FarmManager(Landscape * landscape);
	/** \brief Farm manager destructor */
	~FarmManager();
	/** \brief Runs the daily farm management for each farm, but also calculates annual spilled grain and maize */
	void FarmManagement();
	void InitiateManagement(void);
	/** \brief Forces all farmers to update social networks if needed */
	void UpdateSocialNetworks(bool a_geographic, bool a_associative, bool a_virtual);
	Landscape* GetLandscape(void) { return m_landscape; }
	void AddField(int a_OwnerIndex, LE* a_newland, int a_Owner);
	void RemoveField(int a_OwnerIndex, LE* a_field);
	int ConnectFarm(int a_owner);

	/** \brief Converts strings to tov_  */
	TTypesOfVegetation TranslateVegCodes(std::string& str); //moved from farm
	/** \brief Converts strings to toc_ */
	TTypesOfCrops TranslateCropCodes(string& str);
	/** \brief Converts toc_ to string */
	std::string BackTranslateCropCodes(TTypesOfCrops a_crop);
	void DumpFarmAreas();
	/** \brief dumps the farmrefs file to a standard named file */
	void DumpFarmrefs(const char* a_filename);
	/** \brief Returns the total farm area from the farm ref num */
	int GetFarmTotalSize(int a_farmref)
	{
		return (GetFarmPtr(a_farmref)->GetTotalArea());
	}
	/** \brief Returns the arable area from the farm ref num */
	int GetFarmArableSize(int a_farmref)
	{
		return (GetFarmPtr(a_farmref)->GetArea());
	}
	/** \brief Returns the farm type from the farm ref num */
	TTypesOfFarm GetFarmType(int a_farmref)
	{
		return (GetFarmPtr(a_farmref)->GetType());
	}
	/** \brief Returns the number of fields owned by a from the farm ref num */
	int GetFarmNoFields(int a_farmref)
	{
		return (GetFarmPtr(a_farmref)->GetNoFields());
	}
	/** \brief Returns the number of fields owned by a from the farm ref num */
	APoint GetFarmValidCoords(int a_farmref)
	{
		return (GetFarmPtr(a_farmref)->GetValidCoords());
	}
	
	/** \brief Returns the number of fields with openness more than a_openness */
	int GetFarmNoOpenFields(int a_farmref, int a_openness)
	{
		return (GetFarmPtr(a_farmref)->GetNoOpenFields(a_openness));
	}

	/** \brief Returns the area of fields with openness more than a_openness */
	int GetFarmAreaOpenFields(int a_farmref, int a_openness)
	{
		return (GetFarmPtr(a_farmref)->GetAreaOpenFields(a_openness));
	}

	/** \brief Returns the pointer to a farm with a specific number */
	Farm* GetFarmPtr( int a_owner ) {
		for (unsigned int i = 0; i < m_farms.size( ); i++) {
			if (a_owner == m_farms[ i ]->GetFarmNumber( )) {
				return m_farms[ i ];
			}
		}
		g_msg->Warn( "FarmManager::GetFarmPtr - missing farm ref", a_owner );
		exit( 92 );
	}
	
	/** \brief Get the norm for managment numbers for a crop and management cominbation */
	double GetManagementNorm(int a_crop, int a_managementtype) {	return m_MangagementNorms[a_crop][a_managementtype]; }

	/** \brief Returns the pointer to a farm with a specific index */
	Farm* GetFarmPtrIndex( int a_index) {
				return m_farms[ a_index ];
	}
	/** \brief Returns a random farm reference number */
	int GetRandomFarmRefnum() { return m_farms[g_random_fnc((int)m_farms.size())]->GetFarmNumber(); }
	/** \brief calculate all farm centroids */
	void CalcCentroids() { for (unsigned int i = 0; i < m_farms.size(); i++) m_farms[i]->Centroids(); }
	/** \brief Checks a list to see if a farm matches the illegal list of references */
	bool InIllegalList( int a_farm_ref, vector<int> * a_farmlist );
	/** \brief Add to a list if a farm is not already among the illegal list of references */
	void AddToIllegalList( int a_farm_ref, vector<int> * a_farmlist );
	/** \brief Finds the closest farm to this co-ordinate */
	int FindClosestFarm(HunterInfo a_hinfo, vector<int> *a_farmlist);
	/** \brief Finds the closest farm to this co-ordinate but uses a probability distribtution for acceptance */
	//int FindClosestFarmProb(HunterInfo a_hinfo, vector<int> *a_farmlist);
	/** \brief Finds the closest farm to this co-ordinate with openness more than a value*/
	int FindClosestFarmOpenness( HunterInfo a_hinfo, vector<int> * a_farmlist, int a_openness );
	/** \brief Finds the closest farm to this co-ordinate with openness more than a value but uses a probability distribtution for acceptance based on closeness */
	int FindClosestFarmOpennessProb( HunterInfo a_hinfo, vector<int> * a_farmlist, int a_openness );
	/** \brief Finds the closest farm to this co-ordinate with openness more than a value but uses a probability distribtution for acceptance based on closeness and another based on farm size */
	int FindClosestFarmOpennessProbSmallIsBest( HunterInfo a_hinfo, vector<int> * a_farmlist, int a_openness, vector<int> * a_farmsizelist );
	/** \brief Finds the closest farm to this co-ordinate with openness more than a value but uses a probability distribtution for acceptance based on closeness and another based on farm size */
	int FindClosestFarmOpennessProbNearRoostIsBest( HunterInfo a_hinfo, vector<int> * a_farmlist, int a_openness, vector<APoint> * a_farmsizelist );
	/** \brief Finds a farm openness more than a value not on the list */
	int FindFarmWithRandom(vector<int> * a_farmlist);
	/** \brief Finds a farm openness more than a value not on the list */
	int FindFarmWithOpenness(vector<int> * a_farmlist, int a_openness);
	/** \brief Finds a random farm with at least one field with openness above a_openness */
	int FindOpennessFarm(int a_openness);
	/** \brief Check if a farm has at least one field with openness above a_openness */
	bool CheckOpenness( int a_openness, int a_ref );
	/** \brief Gets the farm centroid as an APoint */
	APoint GetFarmCentroid(int a_farmref)
	{
		for (unsigned int i = 0; i < m_farms.size(); i++)
		{
			if (a_farmref == m_farms[i]->GetFarmNumber())
			{
				return m_farms[i]->GetCentroids();
			}
		}
		g_msg->Warn("FarmManager::GetFarmCentroid - missing farm ref", a_farmref);
		exit(92);
	}
	/** \brief Checks if we already have this ref */
	bool IsDuplicateRef(int a_ref, HunterInfo* a_hinfo);
	/** \brief Returns the average amount of spilled grain in KJ/m2 this year */
	double GetSpilledGrain();
	/** \brief Returns the average amount of spilled maize in KJ/m2 this year */
	double GetSpilledMaize();
	/** \brief Set m_SpilledGrain which is the flag for either 2013 (true) or 2014 (false) spilled grain distributions */
	void SetSpilledGrain( bool a_spilledgrain ) {
		m_SpilledGrain = a_spilledgrain;
	}
	/** \brief Returnes day degrees for the period March 1st - November 1st. Used for determining yields of crops that are not harvested.*/ 
	double GetDD (void){return daydegrees;}; 
	void SetDD (double a_dd){daydegrees = a_dd;};

	/** \brief Get a farm reference from the lookup table */
	int GetFarmNoLookupIndex(int a_ref) {
		for (int i = 0; i < (int)m_farms.size(); i++)
		{
			if (GetFarmNoLookup(i) == a_ref) return i;
		}
		g_msg->Warn("FarmManager::GetFarmNoLookupIndex: Farm ref not found", a_ref);
		exit(142);
	}
	/** \brief Get a farm reference from the lookup table */
	int GetFarmNoLookup(int a_ref) { return m_farmmapping_lookup[a_ref * 2]; }
	/** \brief Get a farm type from the lookup table */
	int GetFarmTypeLookup(int a_ref) { return m_farmmapping_lookup[a_ref * 2 + 1]; }
	/** \brief Returns the flag for renumbering */
	bool GetIsRenumbered() { return m_renumbered; }
	/** \brief Returns the farm ref index for a farmref */
	int GetRenumberedFarmRef(int a_farmref)
	{
		for (int i = 0; i < (int)m_farms.size(); i++)
		{
			if (m_farmmapping_lookup[i * 2] == a_farmref)
			{
				return i;
			}
		}
		g_msg->Warn( "FarmManager::GetRenumberedFarmRef(int a_farmref)  Farm reference number not found in m_farmmapping_lookup ", a_farmref );
		exit( 9 );
	}
	int GetNoFarms( ) {
		return (int)m_farms.size();
	}

	vector<Crop*>* GetCropMgtPlans(void) { return &m_cropprogs; }
	int GetCropMgtPlansIndex(TTypesOfVegetation a_tov) {
		for (unsigned int i = 0; i < m_cropprogs.size(); i++)
		{
			if (a_tov == m_cropprogs[i]->m_tov) return i;
		}
		g_msg->Warn(WARN_MSG, "Mismatch in rotation file looking for tov ", a_tov);
		exit(1);
	}
	TTypesOfCrops GetCropTypeFromPlan(TTypesOfVegetation a_tov) {
		return m_cropprogs[GetCropMgtPlansIndex(a_tov)]->GetCropType();
	}
	/** \brief Get a specific farm i */
	Farm* GetFarm(int i) { return m_farms[i]; }
	/** \brief Adds the event information to the management stats */
	void AddToManagementStats(FarmEvent* a_ev);
#ifdef __CALCULATE_MANAGEMENT_NORMS
	void OutputManagementNormStats();
#endif
	/** \brief Get the flag for use of socio-economic farmers */
	bool GetUseSocioEconomicFarmers() { return  m_UseSocioEconomicFarmers; }

	
protected:
	vector<Crop*> m_cropprogs;
	vector<Farm*> m_farms;

	/** \brief Flag for use of socio-economic farmers */
	static bool m_UseSocioEconomicFarmers;

	/** \brief Holds the list of yield prices indexed by toc_ type (TTypesOfCrop) */
	vector<double> m_YieldReturns;
	/** \brief Holds a local pointer to the landscape */
	Landscape* m_landscape;
	/** \brief Holds the values for profit per unit biomass per unit area for all crops - uses TTypesOfCrops */
	vector<double> m_CropYieldProfit;
	/** \brief Holds the values for crop management uses norms - uses TTypesOfCrops and  FarmManagementCategory as array size determinants */
	double m_MangagementNorms[toc_Foobar][fmc_Foobar];
	/** \brief Creates ther farms at simulation start */
	void  CreateFarms( const char *a_farmfile );
	/** \brief Daydegrees for period March 1st - November 1st. Used to determine yield of crops that are not harvested (and thus do not have values of biomass at harvest).*/
	double daydegrees;
	/** \brief Is it 2013 (true) or 2014 (false) as far as grain goes */
	bool m_SpilledGrain{false};
	/** \brief Used for a dynamic array of lookups converting farm references to internal renumbering */
	int* m_farmmapping_lookup;
	/** \brief A flag to show whether renumbering was already done */
	bool m_renumbered;
#ifdef __CALCULATE_MANAGEMENT_NORMS
	int m_managementnormstats[toc_Foobar][fmc_Foobar+1];
#endif
};


#endif // FARM_H
