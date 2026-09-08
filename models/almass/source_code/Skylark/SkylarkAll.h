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

//---------------------------------------------------------------------------
#ifndef Skylarks_All_H
#define Skylarks_All_H
//---------------------------------------------------------------------------

#define UNREFERENCED_PARAMETER(P) (P)
#define SKOPTIMALHABITATSCORE 50

using namespace std;

using SkylarkObject = enum {
	sob_Clutch,
	sob_Nestling,
	sob_PreFledgeling,
	sob_Male,
	sob_Female
};

using TTypesOfSkState = enum {
	toss_Initiation = 0,
	//Clutch
	toss_Developing,
	toss_Hatching,
	toss_CDying,
	//Nestling
	toss_NDeveloping,
	toss_NMaturing,
	toss_NDying,
	//PreFledgeling
	toss_PDeveloping,
	toss_PMaturing,
	toss_PDying,
	//MaleSK
	toss_MFlocking,
	toss_MFloating,
	toss_MArriving,
	toss_MImmigrating,
	toss_MEmigrating,
	toss_MTempLeavingArea,
	toss_MFindingTerritory,
	toss_AttractingAMate,
	toss_FollowingMate,
	toss_ScaringOffChicks,
	toss_MCaringForYoung,
	toss_MDying,
	toss_MRehousing,
	//FemaleSK
	toss_FFlocking,
	toss_FFloating,
	toss_FArriving,
	toss_FImmigrating,
	toss_FEmigrating,
	toss_FTempLeavingArea,
	toss_FFindingTerritory,
	toss_BuildingUpResources,
	toss_MakingNest,
	toss_PreparingForBreeding,
	toss_Laying,
	toss_StartingNewBrood,
	toss_EggHatching,
	toss_Incubating,
	toss_StoppingBreeding,
	toss_FCaringForYoung,
	toss_FDying,
	toss_GivingUpTerritory,
	// Destroy object state
	toss_Destroy,
};

// Forward declarations
class Skylark_Base;
class Skylark_Clutch;
class Skylark_Nestling;
class Skylark_PreFledgeling;
class Skylark_Female;
class Skylark_Male;
class Landscape;
class SkTerritories;
class TForm1;
class Population_Manager;
class Probe_Data;

//------------------------------------------------------------------------------
// Class Declarations
//------------------------------------------------------------------------------

template <class T>
class TSkylarkList : public vector<T> {
public:
	TSkylarkList() : vector<T>() {
	}

	//------------------------------------------------------------------------------
	double Probe(const Probe_Data* p_TheProbe) {
		// Counts through the list and goes through each area to see if the animal
		// is standing there and if the farm, veg or element conditions are met
		AnimalPosition Sp;
		double NumberSk = 0;
		// Four possibilites
		// either NoVegTypes or NoElementTypes or NoFarmTypes is >0 or all==0
		if (p_TheProbe->m_NoFarms != 0)
		{
			for (unsigned j = 0; j < this->GetItemsInContainer(); j++)
			{
				Sp = this->Get(j)->SupplyPosition();
				const unsigned Farm = this->Get(j)->SupplyFarmOwnerRef();
				for (unsigned i = 0; i < p_TheProbe->m_NoAreas; i++)
				{
					if (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.m_x < p_TheProbe->m_Rect[i].m_x2 && Sp
						.m_y < p_TheProbe->m_Rect[i].m_y2)
						for (unsigned k = 0; k < p_TheProbe->m_NoFarms; k++)
						{
							if (p_TheProbe->m_RefFarms[k] == Farm) NumberSk++; // it is in the square so increment number
						}
				}
			}
		}
		if (p_TheProbe->m_NoEleTypes != 0)
		{
			for (unsigned j = 0; j < this->GetItemsInContainer(); j++)
			{
				Sp = this->Get(j)->SupplyPosition();
				for (unsigned i = 0; i < p_TheProbe->m_NoAreas; i++)
				{
					if (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.m_x < p_TheProbe->m_Rect[i].m_x2 && Sp
						.m_y < p_TheProbe->m_Rect[i].m_y2)
						for (unsigned k = 0; k < p_TheProbe->m_NoEleTypes; k++)
						{
							if (p_TheProbe->m_RefEle[k] == Sp.m_EleType) NumberSk++; // it is in the square so increment number
						}
				}
			}
		}
		else
		{
			if (p_TheProbe->m_NoVegTypes != 0)
			{
				for (unsigned j = 0; j < this->GetItemsInContainer(); j++)
				{
					Sp = this->Get(j)->SupplyPosition();
					for (unsigned i = 0; i < p_TheProbe->m_NoAreas; i++)
					{
						if (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.m_x < p_TheProbe->m_Rect[i].m_x2
							&& Sp.m_y < p_TheProbe->m_Rect[i].m_y2)
						{
							for (unsigned k = 0; k < p_TheProbe->m_NoVegTypes; k++)
							{
								if (p_TheProbe->m_RefVeg[k] == Sp.m_VegType) NumberSk++; // it is in the square so increment number
							}
						}
					}
				}
			}
			else // both must be zero
			{
				for (unsigned j = 0; j < this->GetItemsInContainer(); j++)
				{
					Sp = this->Get(j)->SupplyPosition();
					for (unsigned i = 0; i < p_TheProbe->m_NoAreas; i++)
					{
						if (Sp.m_x >= p_TheProbe->m_Rect[i].m_x1 && Sp.m_y >= p_TheProbe->m_Rect[i].m_y1 && Sp.m_x < p_TheProbe->m_Rect[i].m_x2
							&& Sp.m_y < p_TheProbe->m_Rect[i].m_y2) NumberSk++; // it is in the square so increment number
					}
				}
			}
		}
		return NumberSk;
	}
	//------------------------------------------------------------------------------
};

//----------------------------------------------------------------------------

class skSpiralResult {
public:
	int x;
	int y;
	bool found;
};

//----------------------------------------------------------------------------

class Skylark_struct {
public:
	int x;
	int y;
	int bx;
	int by;
	int mh;
	Landscape* L;
	SkTerritories* Terrs;

	virtual ~Skylark_struct() {
	}
};

//------------------------------------------------------------------------------

class skClutch_struct : public Skylark_struct {
public:
	int No;
	Skylark_Female* Mum;
};

//------------------------------------------------------------------------------

class Nestling_struct : public Skylark_struct {
public:
	Skylark_Male* Dad;
};

//------------------------------------------------------------------------------

class PreFledgeling_struct : public Skylark_struct {
public:
	double size;
	Skylark_Male* Dad;
	bool sex;
	int age;
};

//------------------------------------------------------------------------------

class AdultSkylark_struct : public Skylark_struct {
public:
	double size;
	bool sex;
	int age;
};

//------------------------------------------------------------------------------

class skTTerritory {
public:
	vector<int> m_polys;
	vector<int> m_sizes;
	vector<int> m_hr_polys;
	vector<int> m_hr_sizes;

	int m_x_div10;
	int m_y_div10;
	int m_range_div10;
	bool m_nest_valid;
	int m_nest_pos_validx;
	int m_nest_pos_validy;
	double m_competitionscaler;
	double GetVirtualDiameter() const { return m_VirtualDiameter; }
	void SetVirtualDiameter(const double dia) { m_VirtualDiameter = dia; }

	int Size; // it is square so we need only one dimension
	int m_Location_x;
	int m_Location_y;
	int GetQuality(void) const { return m_Quality; }
	void SetQuality(const int a_qual) { m_Quality = a_qual; }
	Skylark_Male* Owner;
	Skylark_Female* F_Owner;
	skTTerritory(int x, int y, int TheSize, int TheQuality, int a_x_div10, int a_y_div10, int a_range_div10);
	// assume we store the quality here - question of efficiency
	void TestNestPossibility();
	void SetNestPossibility(const bool valid, const int nx, const int ny) {
		m_nest_valid = valid;
		m_nest_pos_validx = nx;
		m_nest_pos_validy = ny;
	}

protected:
	double m_VirtualDiameter;
	double m_heterogeneity;
	int m_Quality;
};

//------------------------------------------------------------------------------
class skTerritory_struct {
public:
	int x;
	int y;
	int size;
	int ref;
	double nqual;
	bool validnest;
};

//------------------------------------------------------------------------------

class SkQualGrid {
public:
	vector<int> m_sizes;
	vector<int> m_polys;
	double m_qual;

	int Insert(const int a_poly) {
		const int l_size = static_cast<int>(m_sizes.size());
		m_sizes.resize(l_size + 1);
		m_polys.resize(l_size + 1);
		m_polys[l_size] = a_poly;
		m_sizes[l_size] = 1;
		return l_size;
	}
};

//------------------------------------------------------------------------------
class SkTerritories {
private:
	skTTerritory* Territories[400000]; // up to 400000 possible territories
	int NoTerritories;
	int SimW, SimH;
	bool* Grid;

	// This one is inlined for speed, belongs to EvaluateHabitat1().
	// Many of the methods below are now inlined. FN, 2003-11-04.
	int EvaluateHabitatSquare(int xmin, int xmax, int ymin, int ymax, int NoPolygons);

	int m_sim_w_div_10, m_sim_h_div_10;
	int m_qual_grid_signal;
	SkQualGrid* * m_qual_grid;

	// The size of the next arrays.
	int m_hash_size;
	int* m_poly_seen;
	int* m_poly_size;
	// m_qual_cache is quality per unit area for each polygon.
	// Updated daily, but only if needed.
	double* m_qual_cache;
	bool m_qual_cache_filled;

	int m_for_iter_x[1000];
	int m_for_iter_y[1000];
	static int PreMakeForIterator(int a_min_incl, int a_max_excl, int* a_iter, int a_norm_max_excl);
	void PreFillTerrPolyLists(skTTerritory* a_terr);
	void PreFillQualGrid(void);
	void PreEvaluateQualGrid(SkQualGrid* a_grid, int a_x, int a_y, int a_width, int a_height);
	void PreFillQualCache(void);

	// PrePoly2Qual() is where the actual evaluation of each polygon
	// takes place.
	//double PrePoly2Qual( int a_poly, int * a_good_polys );

	// Coordinates are in grid squares, ie. map coordinates divided by 10.
	int PreEvaluateHabitat(int a_x, int a_y, int a_range_x, int a_range_y) const;
	int PreEvaluateHabitatStripX(int a_x, int a_y, int a_range_x) const;
	int PreEvaluateHabitatStripY(int a_x, int a_y, int a_range_x) const;

	void DumpMapGraphics(const char* a_filename, Landscape* a_map) const;

public:
	// New daily update method. Called by DoFirst() in the
	// population manager.
	void Tick(void) { m_qual_cache_filled = false; }
	void EvaluateAllTerritories(void);
	void PreCachePoly(int a_poly);
	double PrePoly2Qual(int a_poly);
	double PrePolyNQual(int a_poly, int* a_good_polys);
	int PolyRefData[2500];
	int PolySizeData[2500];
	double PolyHeightData[2500];
	TTypesOfLandscapeElement PolyTypeData[2500];
	Landscape* TheLandscape;
	void PreProcessLandscape2(Landscape* L);
	void ClaimGrid(int x, int y, int range) const;
	void UpdateQuality();
	bool IsGridPositionValid(const int& a_x, const int& a_y, int a_range) const;
	bool IsExtGridPositionValid(const int& x, const int& y, int range) const;
	int IsValid(int nx, int ny) const;
	void GetTerritoriesByDistance(int nx, int ny, vector<APoint>* alist) const;
	int Supply_quality(int ref) const;
	int Supply_x(int ref) const;
	int Supply_y(int ref) const;
	int Supply_size(int ref) const;
	skTTerritory* Supply_terr(int ref) const;
	void RemoveFemale(int ref) const;
	void RemoveMale(int ref) const;
	void Split(int ref);
	Skylark_Male* Supply_Owner(int ref) const;
	Skylark_Female* Supply_F_Owner(int ref) const;
	void Occupy(int ref, Skylark_Male* Male) const;
	void FemaleOccupy(int ref, Skylark_Female* Female) const;
	double EvaluateHabitatN(skTTerritory* a_terr);
	int SupplyNoTerritories() const;
	int SupplyNoMaleOccupied() const;
	int SupplyNoFemaleOccupied() const;
	bool SupplyIsNestValid(const int ref) const { return Territories[ref]->m_nest_valid; }
	APoint SupplyNestPosition(const int ref) const {
		const APoint p(Territories[ref]->m_nest_pos_validx, Territories[ref]->m_nest_pos_validy);
		return p;
	}
	SkTerritories(Landscape* L);
	~SkTerritories();
};

//------------------------------------------------------------------------------
using Skylark_ClutchList = TSkylarkList<Skylark_Clutch*>;
using Skylark_NestlingList = TSkylarkList<Skylark_Nestling*>;
using Skylark_PreFledgelingList = TSkylarkList<Skylark_PreFledgeling*>;
using Skylark_MaleList = TSkylarkList<Skylark_Male*>;
using Skylark_FemaleList = TSkylarkList<Skylark_Female*>;

//------------------------------------------------------------------------------

class Skylark_Population_Manager : public Population_Manager {
protected:
#ifdef __CJTDebug_10
  long refnum;
#endif
	void DoFirst() override;
	void ProbeReportPOM(int a_time);
	float ProbePOM(int ListIndex, const Probe_Data* p_TheProbe);
	int M_Mig_Mort;
	int F_Mig_Mort;
	double EMi; // EM in insect dry weight
	void LoadParameters();
	int m_StriglingMort[4];
	int m_TotalEggs;
	int m_TotalNestlings;
	int m_TotalPrefledgelings;
	int m_NoFledgeDeaths;
	int m_NoChickDeaths;
	int m_NoPestEffects;
	bool m_IsBadWeather;
	// SK POM Files
	FILE* SKPOM1;
	FILE* SKPOM2;

public:
	void CreateObjects(int ob_type, TAnimal* pTAo, void* null, Skylark_struct* data, int number);
	virtual void Init(void);

	void WriteSKPOM1(const int n, const int n2) const { fprintf(SKPOM1, "%i\t%i\n", n, n2); }
	void WriteSKPOM2(const int n, const int n2) const { fprintf(SKPOM2, "%i\t%i\n", n, n2); }

	int SupplyM_Mig_Mort() const { return M_Mig_Mort; }

	int SupplyF_Mig_Mort() const { return F_Mig_Mort; }

	int SupplyNoTerritories() const;
	int TheSkylarkTerrsSupply_x(int) const;
	int TheSkylarkTerrsSupply_y(int) const;
	int TheSkylarkTerrsSupply_size(int) const;
	int TheSkylarkTerrsSupply_quality(int) const;

	void SetM_Mig_Mort(const int m) { M_Mig_Mort = m; }

	void SetF_Mig_Mort(const int m) { F_Mig_Mort = m; }
	Skylark_Population_Manager(Landscape* L);

	~Skylark_Population_Manager(void) override;
	// Special interfact functions for outputs
	int TheFledgelingProbe() override;
	void BreedingPairsOutput(int Time) override;
	void FledgelingProbeOutput(int Total, int Time) override;
	int TheBreedingFemalesProbe(int ProbeNo) override;
	bool OpenTheBreedingPairsProbe() override;
	bool OpenTheFledgelingProbe() override;

	void incNoFledgeDeaths() { m_NoFledgeDeaths++; }

	void incNoChickDeaths() { m_NoChickDeaths++; }

	void incNoPestEffects() { m_NoPestEffects++; }

	void incTotalEggs(const int eggs) { m_TotalEggs += eggs; }

	void incTotalNestlings() { m_TotalNestlings++; }

	void incTotalPrefledgelings() { m_TotalPrefledgelings++; }

	void AddStriglingMort(const int lifestage) { m_StriglingMort[lifestage]++; }

	double SupplyEMi() const { return EMi; }

	bool IsBadWeather() const { return m_IsBadWeather; }

	SkTerritories* TheSkylarkTerrs;
	int *VegTypeFledgelings;
	bool OpenTheBreedingSuccessProbe() override;
	void BreedingSuccessProbeOutput(double, int, int, int, int, int, int, int) override;
	int TheBreedingSuccessProbe(int& BreedingFemales, int& YoungOfTheYear, int& TotalPop, int& TotalFemales, int& TotalMales,
	                            int& BreedingAttempts) override;
	void TheAOROutputProbe() override;
	virtual void TheRipleysOutputProbe(FILE* a_prb);

protected:
	void Catastrophe() override;
	virtual void ReHouse();
};

//------------------------------------------------------------------------------

class Skylark_Base : public TAnimal {
public:
#ifdef __CJTDebug_10
  long m_Ref;
#endif
	TTypesOfSkState m_CurrentSkState;
	int Age;
	double m_Size;
	int m_Born_x;
	int m_Born_y;
	/** \brief  The vegetation type where the skylark was born */
	int m_MyHome;
	// Object References
	SkTerritories* m_OurTerritories;
	Skylark_Population_Manager* m_OurPopulationManager;
	Skylark_Base(int x, int y, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by, int mh);
	virtual void ReInit(int x, int y, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by, int mh);
	virtual double On_FoodSupply(double /* food */) { return 0.0; }

	void AddStriglingMort(const int lifestage) const { m_OurPopulationManager->AddStriglingMort(lifestage); }

	// Returns the state number
	int WhatState() override { return m_CurrentSkState; }

	bool InSquare(rectangle R) const;

protected:
	static bool DailyMortality(int mort);
#ifdef __PESTICIDE_RA
	double m_pcide;
public:
	double m_pesticide_accumulation;
	double m_pcide_conc;
	virtual void PesticideResponse() {
	}
#endif
};

//------------------------------------------------------------------------------

class Skylark_Clutch : public Skylark_Base {
protected:
	// Behaviours
	int st_Developing();
	int st_Hatching();
	void st_Dying();
	// Attributes
	int MinDegrees;
	int m_baddays;
	bool OnFarmEvent(FarmToDo event) override;

public:
	// Behaviours
	void BeginStep(void) override;
	void Step(void) override;
	void EndStep(void) override;
	Skylark_Clutch(Skylark_Female* Mum, SkTerritories* Terrs, Landscape* L, int NoEggs, int x, int y, int mh, Skylark_Population_Manager* SPM);
	virtual void ReInit(Skylark_Female* Mum, SkTerritories* Terrs, Landscape* L, int NoEggs, int x, int y, int mh,
	                    Skylark_Population_Manager* SPM);
	void OnMumGone();

	void AddEgg() { Clutch_Size++; }

	void StartDeveloping() {
		m_CurrentSkState = toss_Developing;
		// Must tell the pop man how many eggs were made
		m_OurPopulationManager->incTotalEggs(Clutch_Size);
	}

	// Attributes
	int Clutch_Size;
	Skylark_Female* Mother;
};

//------------------------------------------------------------------------------

class Skylark_Nestling : public Skylark_Base {
	//Attributes
protected:
	bool Sex;
	int m_EM_fail;
	int m_NestLeavingChance;
	//  double Growth;
	Skylark_Male* m_Dad;
	double m_EM;
	double m_GrNeed;
	//Behaviours
	virtual int st_Developing();
	virtual void st_Maturing();
	virtual void st_Dying();
	bool OnFarmEvent(FarmToDo event) override;
#ifdef __PESTICIDE_RA
	void PesticideResponse(void) override;
#endif

public:
	Skylark_Nestling(int x, int y, Skylark_Male* Daddy, Landscape* L, SkTerritories* Terrs, Skylark_Population_Manager* SPM, int bx, int by,
	                 int mh);
	virtual void ReInit(int x, int y, Skylark_Male* Daddy, Landscape* L, SkTerritories* Terrs, Skylark_Population_Manager* SPM, int bx, int by,
	                    int mh);
	void BeginStep(void) override;
	void Step(void) override;
	void EndStep(void) override;
	//Interface
	double On_FoodSupply(double food) override;
	void OnDadDead();
	void OnYouHaveBeenEaten();
	void OnDeserted();
};

//------------------------------------------------------------------------------

class Skylark_PreFledgeling : public Skylark_Nestling {
protected:
	int st_Developing() override;
	void st_Maturing() override;
	void st_Dying() override;
	double GetFood() const;
	double GetFledgelingEM(int Age) const;
	bool OnFarmEvent(FarmToDo event) override;
#ifdef __PESTICIDE_RA
	void PesticideResponse() override;
#endif

public:
	Skylark_PreFledgeling(int x, int y, Landscape* L, SkTerritories* Terrs, Skylark_Male* Daddy, bool sex, double size, int age,
	                      Skylark_Population_Manager* SPM, int bx, int by, int mh);
	virtual void ReInit(int x, int y, Landscape* L, SkTerritories* Terrs, Skylark_Male* Daddy, bool sex, double size, int age,
	                    Skylark_Population_Manager* SPM, int bx, int by, int mh);
	void BeginStep(void) override;
	void Step(void) override;
	void EndStep(void) override;
};

//------------------------------------------------------------------------------

class Skylark_Adult : public Skylark_Base {
protected:
	// Methods
	virtual double RemoveEM(double food);
	double GetVegHindrance(int PolyRef) const;
	double GetWeatherHindrance() const;
	static bool GetBadWeather();
	// Attributes
	int GoodWeather;
	bool BSuccess;
	double MyExtractEff;
	int m_pesticide_affected;
	skTerritory_struct MyTerritory;
	vector<APoint>* m_aTerrlist;

public:
	Skylark_Adult(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by,
	              int mh);
	virtual void ReInit(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by,
	                    int mh);
	~Skylark_Adult() override;
	// Attributes
	bool Paired;
	virtual void CopyMyself(int a_sktype);
};

//------------------------------------------------------------------------------

class Skylark_Female : public Skylark_Adult {
protected:
	int st_Flocking();
	int st_Floating();
	bool st_Arriving();
	bool st_Immigrating();
	int st_Emigrating();
	int st_TempLeavingArea();
	int st_Finding_Territory();
	void st_Dying();
	int st_CaringForYoung();
	int st_BuildingUpResources();
	TTypesOfSkState st_MakingNest();
	int st_PreparingForBreeding();
	int st_GivingUpTerritory();
	TTypesOfSkState st_Laying();
	int st_StartingNewBrood();
	int st_EggHatching();
	int st_Incubating();
	int st_StoppingBreeding();
	double GetFood(int time);
	int CalculateEggNumber() const;
	int CalcFoodTime(double target) const;
	int GetMigrationMortality();
	double CheckForFields() const;
	void FeedYoung();
	bool OnFarmEvent(FarmToDo event) override;
#ifdef __PESTICIDE_RA
	void PesticideResponse() override;
#endif
	//bool TestNestPos( int x, int y );
	skSpiralResult Spiral(int x, int y, int radius);
	skSpiralResult Spiral2(int x, int y, int radius);
	// Attributes
	Skylark_Clutch* MyClutch;
	int m_Counter1;
	int m_NestTime;
	bool NestLoc;
	double Resources;
	int EggCounter;
	int m_BreedingAttempts;
	int m_BreedingSuccess;
	int m_EggNumber;
	bool m_pesticide_sprayed_die;
	int m_toowet;
	double m_MinFemaleAcceptScore;

public:
	Skylark_Male* MyMate;
	// Methods
	Skylark_Female(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by,
	               int mh);
	void ReInit(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by,
	            int mh) override;
	void BeginStep(void) override;
	void Step(void) override;
	void EndStep(void) override;
	void EstablishTerritory();

	//Interface
	int Supply_NestTime() const { return m_NestTime; }

	Skylark_Clutch* SupplyMyClutch() const { return MyClutch; }

	int Supply_BreedingAttempts() const;
	int Supply_BreedingSuccess() const;
	void ResetBreedingSuccess();

	void OnSetMyClutch(Skylark_Clutch* p_C) { MyClutch = p_C; }

	void OnEggsHatch();
	void OnClutchDeath();
	void OnBroodDeath();
	void OnBreedingSuccess();
	void OnStopFeedingChicks();
	void OnMateDying();
	void OnMateHomeless();
	void OnMaleNeverComesBack(const Skylark_Male* AMale);
	void OnBreedSuccess() { m_BreedingSuccess++; }
	void SensibleCopy();
};

//------------------------------------------------------------------------------

class Skylark_Male : public Skylark_Adult {
protected:
	// Attributes
	// Functions
	int st_Flocking();
	TTypesOfSkState st_Floating();
	bool st_Arriving() const;
	bool st_Immigrating();
	int st_Emigrating();
	int st_TempLeavingArea();
	int st_FindingTerritory();
	int st_AttractingAMate();
	int st_FollowingMate();
	void ConstructAHabitatTable();
	int EstablishingATerritory();
	int st_ScaringOffChicks();
	int st_CaringForYoung();
	void st_Dying();
	/** \brief  Daily re-evaluation of territory */
	void ReEvaluateTerritory();
	double GetFood(int time);
	void OptimiseHabitatSearchingOrder();
	int GetMigrationMortality();
	bool OnFarmEvent(FarmToDo event) override;
#ifdef __PESTICIDE_RA
	void PesticideResponse() override;
#endif
	// Attributes
	double m_XFNestAcceptScore;
	bool m_firstPF;
	int GoodWeather;
	int m_BroodSize;
	Skylark_Nestling* m_Brood[26]; //NB Nestling = base class for PreFledgling
	int No_HabitatTable_Refs;
	int m_MyMinTerritoryQual;

public:
	//for debugging
	Skylark_Female* MyMate;
	// to here
	int BroodAge;

public:
	// Attributes
	// Male's list of his habitats and their sizes & food availability
	// **CJT** could be more efficient to move these three to the territory
	vector<int> m_HabitatTable_PNum;
	vector<int> m_HabitatTable_Size;
	vector<double> m_InsectTable;
#ifdef __PESTICIDE_RA
	vector<double> m_PConcTable;
#endif
	//
	bool HaveTerritory;
	// Methods
	Skylark_Male(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by,
	             int mh);
	~Skylark_Male() override;
	void ReInit(int x, int y, double size, int age, SkTerritories* Terrs, Landscape* L, Skylark_Population_Manager* SPM, int bx, int by,
	            int mh) override;
	void BeginStep(void) override;
	void Step(void) override;
	void EndStep(void) override;
	// Interface functions
#ifdef __CJTDebug_5
  bool DoIExistN( Skylark_Nestling * N );
  bool DoIExistP( Skylark_PreFledgeling * N );
#endif
	bool OnEvicted();

	int SupplyNoHabitatRefs() const { return No_HabitatTable_Refs; }

	int SupplyBroodSize() const { return m_BroodSize; }

	int SupplyBroodAge(const int n) const { return m_Brood[n]->Age; }

	/** brief Provide todays territory quality */
	double Supply_TerritoryQual() const { return MyTerritory.nqual; }

	int SupplyBroodWeight(const int n) const { return static_cast<int>(m_Brood[n]->m_Size); }

#ifndef __PESTICIDE_RA_CHICK
	double OnFoodMessage(const int n, const double f) const {
		const double extra = m_Brood[n]->On_FoodSupply(f);
		return extra;
	}
#else
  double OnFoodMessage( int n, double f , double p )
  {
	  double extra = m_Brood[n]->On_FoodSupply( f );
	  m_Brood[n]->m_pesticide_accumulation+= (((f-extra)/f)*p);
	return extra;
  }
#endif

	void OnAddNestling(Skylark_Nestling* N);
	void OnAddPreFledgeling(Skylark_PreFledgeling* P, const Skylark_Nestling* N);
	skTerritory_struct Supply_Territory() const;
	int DefendTerritory() const;
	void OnEggHatch();
	void OnMateDying();
	void OnMateLeaving();
	void OnPairing(Skylark_Female* female);
	void OnBroodDeath();
	void OnNestLocation(int x, int y);
	void OnNestlingDeath(const Skylark_Nestling* N);
	void OnPreFledgelingDeath(const Skylark_PreFledgeling* P);
	void OnPreFledgelingMature(const Skylark_PreFledgeling* P);
	void OnMateNeverComesBack(const Skylark_Female* AFemale);
	void OnNestPredatation();
	void OnBroodDesertion();
	void OnReHouse();
	void SensibleCopy();
	bool SupplyNestValid() const { return m_OurPopulationManager->TheSkylarkTerrs->SupplyIsNestValid(MyTerritory.ref); }
	APoint SupplyNestLoc() const { return m_OurPopulationManager->TheSkylarkTerrs->SupplyNestPosition(MyTerritory.ref); }
};

//------------------------------------------------------------------------------
#endif
