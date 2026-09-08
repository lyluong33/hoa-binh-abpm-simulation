// Landscape.h
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

#define _CRTDBG_MAP_ALLOC

#ifndef TLANDSCAPE_H
#define TLANDSCAPE_H

#include <vector>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Sparse>

/** \brief Julian start dates of the month of January */
const int January   =   0;
/** \brief Julian start dates of the month of February */
const int February  =  31;
/** \brief Julian start dates of the month of March */
const int March     =  59;
/** \brief Julian start dates of the month of April */
const int April     =  90;
/** \brief Julian start dates of the month of May */
const int May       = 120;
/** \brief Julian start dates of the month of June */
const int June      = 151;
/** \brief Julian start dates of the month of July */
const int July      = 181;
/** \brief Julian start dates of the month of August */
const int August    = 212;
/** \brief Julian start dates of the month of September */
const int September = 243;
/** \brief Julian start dates of the month of October */
const int October   = 273;
/** \brief Julian start dates of the month of November */
const int November  = 304;
/** \brief Julian start dates of the month of December */
const int December  = 334;

// m_polymapping is a mapping from polygon numbers into
// the list of landscape elements, m_elems.

extern class Pesticide *g_pest;
extern class PollenNectarDevelopmentData * g_nectarpollen;
extern Landscape* g_map;

class RasterMap;
class SkTerritories;
class RodenticidePredators_Population_Manager;
class PesticideMap;
class PesticideTable;
class PopulationManagerList;

/** \brief A data structure to store the pollen and nectar information for each cell*/
struct PollenNectarCellInfo{
	int d_x_loc = -1;
	int d_y_loc = -1;
	double d_quality;
	double d_quantity;
};

/** \brief A list item entry of field polygon reference numbers with associated openness and goose food scores */
struct GooseFieldListItem
{
	int polyref;
	int geese;
	int geesesp[gs_foobar];
	int geeseTimed;
	int geesespTimed[gs_foobar];
	int roostdists[gs_foobar];
	double openness;
	double grain;
	double maize;
	double grass[gs_foobar];  // Available grass forage is species specific
	TTypesOfVegetation vegtype;
	std::string vegtypechr;
	double vegheight;
	double digestability;
	int vegphase;
	std::string previouscrop;
	std::string lastsownveg;
	std::string debugveg;
};

/** \brief A list of GooseFieldListItem s */
typedef std::vector<GooseFieldListItem> GooseFieldList;

//------------------------------------------------------------------------------

/** \brief A small class to generate pest incidence information */
class PestIncidenceManager
{
	/**
	* This class is a stub for expansion to more detailed versions. The initial version is simply a random number between 0 and 1
	* but more complex versions and file read information should be added.
	*/
public:
	/** \brief Generates maximum and minimum pest indicidence levels  */
	PestIncidenceManager(double a_max, double a_min);
	/** \brief Returns the current pesticide incidence level, the mean of minimum and maximum incidence level */
	double GetIncidenceLevel() { return m_current_incidence; }
	/** \brief Assigns a random number between 0 and 1.0 as the current incidence level */
	void RecalculateIncidence();
protected:
	/** \brief Returns the current pesticide incidence level, the mean of minimum and maximum incidence level in the GetIncidenceLevel or a random number between 0 and 1.0 in the RecalculateIncidence */
	double m_current_incidence;
	/** \brief Returns the maximum pesticide incidence level */
	double m_max_incidence;
	/** \brief Returns the minimum pesticide incidence level */
	double m_min_incidence;
	/** \brief Returns the range of pesticide incidence level, the subtraction of minimum from the maximum level */
	double m_range;
};
//------------------------------------------------------------------------------

/**
\brief
The landscape class containing all environmental and topographical data.
*/
class Landscape
{
	/** \brief The locks for all the polygon.*/
	std::vector<omp_nest_lock_t*> m_poly_omp_locks;
	/**
	\brief Version info. Initialized by the constructor. */
	char m_versioninfo[30];

	/** \brief List of all the farms. */
	//vector<Farm*> m_farms;
	FarmManager* m_FarmManager;

	/** \brief List of all landscape elements. The index is a sequential number, to get the polynum look this number up in m_polymapping */
	vector<LE*>   m_elems;

	/** \brief The big map */
	RasterMap* m_land;

	/** m_polymapping is a mapping from polygon numbers into
	* the list of landscape elements, m_elems. When using this it is important that it is the poly num and not the
	*  map index that is used. */
	vector<int>          m_polymapping;

	/** Vector to store the poly IDs with flower resource. */
	vector<int> m_poly_with_flowers;

	/** \brief For specialised pesticide recording */
	PesticideMap* m_PesticideMap = nullptr;
	/** \brief For specialised pesticide recording */
	PesticideTable* m_PesticideTable = nullptr;

	/** \brief A pest incidence manager */
	PestIncidenceManager* m_PestIncidenceManager;
	double m_pestincidencefactor;

	/** \brief Declaration of attributes for correcting coordinates before modulus operations, saves an indirection when doing inline function calls */
	/** \brief Area width */
	int           m_width;
	/** \brief Area length */
	int           m_height;
	/** \brief Area width times 10 */
	int           m_width10;
	/** \brief Area length times 10*/
	int           m_height10;
	/** \brief Area minimum extent, the smallest value of m_width or m_height   */
	int			m_minmaxextent;
	/** \brief Area maximum extent, the largest value of m_width or m_height   */
	int			m_maxextent;
	/** \brief Area of the whole landscape.*/
	int         m_area;

	/** \brief A flag to ensure centroid calculation on object construction */
	bool m_NeedCentroidCalculation;
	/** \brief A flag to ensure openness calculation on object construction */
	bool m_NeedOpennessCalculation;
	/** \brief A flag to signal that missing polygons exist */
	bool m_DoMissingPolygonsManipulations;

	/** \brief For veg area dumps */
	double* l_vegtype_areas;
	/** \brief A pointer to the current main population manager */
	PopulationManagerList* m_ThePopManagerList;
	/** \brief A curve relating goose intake rates in KJ/min to vegetation height for Pink-footed Goose (PF) */
	Polynomial2CurveClass* m_GooseIntakeRateVSVegetationHeight_PF;
	/** \brief A curve relating goose intake rates in KJ/min to vegetation height for Barnacle Goose (BG) */
	Polynomial2CurveClass* m_GooseIntakeRateVSVegetationHeight_BG;
	/** \brief A curve relating goose intake rates in KJ/min to vegetation height for Greylag Goose (GL) */
	Polynomial2CurveClass* m_GooseIntakeRateVSVegetationHeight_GL;

#ifdef __RECORDFARMEVENTS
	ofstream* m_farmeventfile;
#endif
	/** \brief Records total amount of grain / seed / maize available as forage for each polygon */
	void GrainDump();

	#ifdef __FLOWERTESTING
	std::map<int, int> m_polyid_habitat;
	int m_polyid_farm = -1;
	#endif
public:
	/** \brief The function to generate the vector list of pixies for polygons with flower resource.*/
	void RunHiddenYear();
	/** \brief Runs through all polygons and records the area of each vegetation type */
	void FillVegAreaData();
	/** \brief Records all area vegetation types and transform it into a table */
	inline double SupplyVegArea(int v) { return l_vegtype_areas[v]; }
	/** \brief Records all area vegetation types and transform it into a table */
	inline double GetVegArea(int v) { return l_vegtype_areas[v]; }
	/** \brief Saves the information on vegetation types and areas */
	void DumpVegAreaData(int a_day);
	/** \brief Evaluation to find Skylark territory */
	void SkylarkEvaluation(SkTerritories* a_skt);
	/** \brief Evaluation to find Rodenticide Predator territory */
	void RodenticidePredatorsEvaluation(RodenticidePredators_Population_Manager* a_rppm);
	/** \brief Set the pointer to the list of active population managers */
	PopulationManagerList* SupplyThePopManagerList() { return m_ThePopManagerList; }
	/** \brief Distance to the current main population manager, calculates distance from 2 coordinates to other 2 coordinates */
	int DistanceToP(APoint a_Here, APoint a_There);
	/** \brief Distance to the current main population manager, calculates distance from 2 coordinates to other 2 coordinates but do not square root (for efficiency) */
	int DistanceToPSquared(APoint a_Here, APoint a_There);
	/** \brief Set the pointer to the list of active population managers */
	void SetThePopManagerList(PopulationManagerList* a_ptr) { m_ThePopManagerList = a_ptr; }
	/** \brief Returns the current vegetation growth phase for a polygon */
	int SupplyVegPhase(int a_poly) {
		return m_elems[m_polymapping[a_poly]]->GetVegPhase();
	}
	/** \brief Returns Landscape::m_pestincidencefactor */
	double SupplyPestIncidenceFactor() { return m_pestincidencefactor; }
	/** \brief Sets an entry in the polymapping to a specific value */
	void SetPolymapping(int a_index, int a_val) { m_polymapping[a_index] = a_val; }
	/** \brief Returns the value of the m_polymapping array indexed by a_index */
	int GetPolymapping(int a_index) { return m_polymapping[a_index]; }
	/** \brief Returns the size of the Landscape::m_elems vector */
	int SupplyMaxPoly() { return int(m_elems.size()); }
#ifdef __RECORDFARMEVENTS
	/** \brief Records details of a farm event that has been called - applied for debugging */
	void RecordEvent(int a_farmno, int a_polyref, TTypesOfVegetation a_tov, int a_event, int a_day, int a_year) {
		std::string vegtype = VegtypeToString(a_tov);
		//boost::trim(vegtype);
		std::string eventtype = EventtypeToString(a_event);
		//boost::trim(eventtype);
		(*m_farmeventfile) << a_farmno << "\t" << a_polyref << "\t" << vegtype << "\t" << eventtype << "\t" << a_day << "\t" << a_year << "\n";
	}
#endif
	/** \brief Creates a vector containing a list of all the polygons that do not have tov_None as the vegetation type */
	list<LE*> GetAllVegPolys() {
		list<LE*> Plist;
		for (auto it = m_elems.begin(); it != m_elems.end(); ++it) {
			if ((*it)->GetVegType() != tov_None) {
				Plist.push_back(*it);
			}
		}
		return Plist;
	}



protected:
	/** \brief Write ASCII file of the ALMaSS map */
	void GISASCII_Output(string outpfile, int UTMX, int UTMY);
	/** \brief Array for containing the treatment counts */
	int   m_treatment_counts[last_treatment];
	/** \brief The largest polygon number used */
	int   m_LargestPolyNumUsed;
	/** \brief List of pond indexes */
	vector <int>m_PondIndexList;
	/** \brief List of pond polyrefs */
	vector <int>m_PondRefsList;
	/** \brief Is used to take a look in the input file for polygons to figure out what the format is (since multiple formats are allowed) */ 
	int ReadPolys1(const char* a_polyfile);
	/** \brief Based on the format of the file (since multiple formats are allowed), this reads the polygon information from an input file. This method sets up the Landscape::m_polymapping lookup table, checks validity of polygon and associated farm information and creates the LE instances to populate the Landscape::m_elems array, and if needed creates the links between unsprayed margins and the owner tole type */
	void  ReadPolys2(const char* a_polyfile, int a_format);
	/** \brief Checks for internal consistency in the polygon information, e.g. being sure that all polygons mentioned are actually in the map */
	void  PolysValidate(bool a_exit_on_invalid);
	/** \brief Checks whether all polygons are in the map, removing those that are not */
	bool  PolysRemoveInvalid(void);
	/** \brief Called if the landscape is manipulated to save the new version of the polyref input file */
	void  PolysDump(const char* a_filename);
	/** \brief Called if the landscape is manipulated to save the new version of the.lsb input file(i.e.the topographical map) */
	void  DumpMap(const char* a_filename);
	/** \brief Used to replace polygons or combine them when there is no special management associated with them.This just reduces the number of polygon entries and saves time and space */
	void  ConsolidatePolys(void);
	/** \brief Calculates the area for each polygon in the mapand stores it in LE::m_squares_in_map */
	void  CountMapSquares(void);
	/** \brief Renumbers the polygons from 0 to the number of LE instances - 1. This saves space, and also means that the polygon number becomes the same as the index in Landscape::m_elems, allowing quicker lookup(reduces indirection by 1) */
	void  PolysRenumber(void);
	/** \brief Called if there is any change in the polygons and loops through all polygons resetting the Landscape::m_polymapping value */
	void RebuildPolyMapping()
	{  // Rebuild m_polymapping.
		const int sz = (int)m_elems.size();
		for (int i = 0; i < sz; i++)
		{
			m_polymapping[m_elems[i]->GetMapIndex()] = i;
		}
	}
	/**\brief Check of polygon areas. Each polygon should be present in the map with the correct area recorded, otherwise this method raises an error*/
	void  ForceArea(void);
	/** \brief Maps the polygon numbers directly to the indices in m_elems */
	void  ChangeMapMapping(void);
	/** \brief NewElement Creates a new LE instance and where possible it will create the highest level in the class hierarchy e.g. tole_Portarea has no behaviour so can be created as a NonVegElement (LE->NonVegElement), removing the need to have a Portarea class*/
	LE* NewElement(TTypesOfLandscapeElement a_type);
	/** \brief A method for replacing missing values in the map with corrected ones */
	void RemoveMissingValues();
	/** \brief Records vegetation charateristics for the x, y location */
	void VegDump(int x, int y);
	/** \brief Records farm events carried out on the x, y locations */
	void EventDump(int x, int y, int x2, int y2);
	/** \brief Records pesticided application farm events carried out on the x,y location*/
	void EventDumpPesticides(int x1, int y1);
	/** \brief Prints the sum of day degrees. See #FarmManager::daydegrees.*/
	void DegreesDump();
	/** \brief Currently unused test for adding borders*/
	bool BorderNeed(TTypesOfLandscapeElement a_letype);
	/** \brief Adds a border of the specified type (could be an unsprayed margin) around a field  */
	int BorderAdd(LE* a_field, TTypesOfLandscapeElement a_type, int a_allAround=1);
	/** \brief Used to decide if a coordinate location should become a border/unsprayed margin */
	bool BorderTest(int a_fieldindex, int a_borderindex, int a_x, int a_y);
	/** \brief Removes field boundaries, hedgebanks and hedges if they are adjacent to a field*/
	void BorderRemoval(void);
	/** \brief Removes small polygons from the map */
	int RemoveSmallPolygons(void);
	/** \brief Creates a list of pond polygon refs/indexes for easy look up */
	void CreatePondList();
	/** \brief The starting point for adding beetlebanks to a field (can be used to add ‘banks’ of any tole_type */
	void AddBeetleBanks(TTypesOfLandscapeElement a_tole);
	/** \brief Determines whether a beetlebank can be added to the field depending on physical characteristics and adds it */
	int AddBeetleBankIfPossible(LE* a_field, TTypesOfLandscapeElement a_tole);
	/** \brief Finds a location in the middle of a field to ‘seed a beetlebank’ */
	bool FindFieldCenter(LE* a_field, int* x, int* y);
	/** \brief From a central location finds the longest axis of a field */
	int FindLongestAxis(int* x, int* y);
	/** \brief From a central location finds the biggest rectangle that fits inside the field */
	int FindBestRectangle(int a_polyindex, int* a_x, int* a_y, int* a_width, int* a_x_start, int* a_y_start);
	/** \brief Tests the length of a vector to find when we step outside a given polygon */
	void AxisLoop(int a_poly, int* a_x, int* a_y, int a_axis);
	/** \brief The starting point for adding patches to a field (can be used to add patches of any tole_type) */
	void AddPatches(TTypesOfLandscapeElement a_tole);
	/** \brief Determines whether a patch can be added to the field depending on physical characteristics and adds it */
	int AddPatchIfPossible(LE* a_field, TTypesOfLandscapeElement a_tole);

	// List of private methods and member elements, which are needed
	// when adding artificial hedgebanks. Forget I did this. One should
	// never store what is essentially local variables within the main
	// class definition.

	/** \brief Declaration of attributes for hedgebanks */
	vector<int> hb_hedges;
	vector<LE*> hb_new_hbs;

	int		m_x_add[16];
	int		m_y_add[16];
	int* hb_map;
	int         hb_width;
	int         hb_height;
	int         hb_size;
	int         hb_min_x, hb_max_x;
	int         hb_min_y, hb_max_y;
	int         hb_first_free_poly_num;
	int         hb_core_pixels;
	int         hb_border_pixels;
	/** \brief An attribute to hold the pesticide type being tested, if there is one, if not default is -1 */
	TTypesOfPesticide	m_PesticideType;
	/** \brief The entry point for adding hedgebanks. Hedgebanks are created by replacing part of hedge polygons with the #tole_HedgeBank If the hedge is wide this will be with a border on each side, if narrow then by inserting hedgebank between trees */
	void hb_Add(void);
	/** \brief Adds new hedgebanks */
	void hb_AddNewHedgebanks(int a_orig_poly_num);
	/** \brief Striping distribution function for the new hedgebanks LKM */
	int  hb_StripingDist(void);
	/** \brief Creates the necessary new hedgebank polygons in Landscape::m_elems */
	void hb_GenerateHBPolys(void);
	/** \brief Generate a list of polygon numbers for new hedgebank addition */
	void hb_FindHedges(void);
	/** \brief Finds a rectangle that encloses the hedge to be manipulated */
	bool hb_FindBoundingBox(int a_poly_num);
	/** \brief Adds a big number to all polygon refs in the map to create space to safely manipulate the hedge pixels */
	void hb_UpPolyNumbers(void);
	/** \brief Replaces all values in the map for a polygon with a temporary value ready for manipulation */
	void hb_ClearPolygon(int a_poly_num);
	/** \brief Identifies all border hedge pixels */
	void hb_PaintBorder(int a_color);
	/** \brief Checks for out of bounds hedgebank coordinates */
	bool hb_MapBorder(int a_x, int a_y);
	/** \brief Checks if a pixel has any non hedge neighbour cell */
	bool hb_HasOtherNeighbour(int a_x, int a_y);
	/** \brief Pushes a value to a pixel cell that denotes that is has a neighbour that is not hedge */
	bool hb_PaintWhoHasNeighbourColor(int a_neighbour_color, int a_new_color);
	/** \brief Tests for an already process neighbour cell */
	bool hb_HasNeighbourColor(int a_x, int a_y,	int a_neighbour_color);
	/** \brief Used to identify the distance a cell is from the centre of a hedge */
	void hb_MarkTopFromLocalMax(int a_color);
	/** \brief ‘Paints’ the core of a hedge with a negative number for use in deciding what becomes hedgebank later */
	void hb_MarkTheBresenhamWay(void);
	/** \brief Determines the width from centre to edge of hedge */
	int  hb_MaxUnpaintedNegNeighbour(int a_x, int a_y);
	/** \brief Remove high number colour bits from the map */
	void hb_ResetColorBits(void);
	/** \brief LKM */
	void hb_RestoreHedgeCore(int a_orig_poly_number);
	/** \brief Reverses the process carried out by Landscape::hb_UpPolyNumbers */
	void hb_DownPolyNumbers(void);

	//#define HB_TESTING
	/** \brief Saves the graphics for the landscape LKM */
	void DumpMapGraphics(const char* a_filename);

#ifdef  HB_TESTING
	/** \brief Debug to save the hb map */
	void hb_dump_map(int a_beginx, int a_width,

		int a_beginy, int a_height,
		char* a_filename,
		bool a_high_numbers);
	/** \brief Debug to save the hb intermediate map */
	int  hb_dump_color(int a_x, int a_y,
		bool a_high_numbers);
#endif

	/** \brief A flag indicating whether pesticide should be sprayed */
	bool m_toxShouldSpray;
	/** \brief Index for the LE signal loop */
	int  le_signal_index;
	/** \brief When the simulation is closed this method saves the number of each farm management that has been carried out */
	void DumpTreatCounters(const char* a_filename);

	/** \brief Declaration of attributes for country and coordinates */
	std::string   m_countryCode = "None";
	double         m_latitude = -1;   // decimal coordinates
	double         m_longitude = -1; // decimal coordinates
	/** \brief Flag to indicate the that the first year is run (i.e. before simulation starts) */
	bool m_firstyear;
	/** \brief A holder for the landscape name for output if needed */
	string LandscapeName;
public:
	//** \brief The Landscape destructor */
	~Landscape(void);
	//** \brief The Landscape constructor */
	Landscape(bool dump_exit = true);
	/** \brief The function to return whether it is in the hidden year. */
	bool IsHiddenYear(void) {return m_firstyear;}
	//** \brief Procedure for closing the simulation */
	void SimulationClosingActions();
	//** \brief Tick() is applied to run a year of simulations to remove any start-up effects, it is identical to TurnTheWorld() */
	void  Tick(void);
	//** \brief TurnTheWorld() is identical to Tick(), it is retained for historical reasons */
	void  TurnTheWorld(void);

	/** \brief Returns the number of ponds in the landscape */
	int HowManyPonds() const { return static_cast<int>(m_PondIndexList.size()); }
	/** \brief Returns random pond index */
	int SupplyRandomPondIndex();
	/** \brief Returns random pond polyref */
	int SupplyRandomPondRef();
	/** \brief Returns the index of a pond based on pondref or -1 if not found */
	int SupplyPondIndex(int a_pondref) const {
		for (int i = 0; i < static_cast<int>(m_PondIndexList.size()); i++) {
			if (m_PondRefsList[i] == a_pondref) return m_PondIndexList[i];
		}
		return -1;
	}
	/** \brief Sets a male as being present in a pond */
	void SetMaleNewtPresent(const int a_InPondIndex) { m_elems[a_InPondIndex]->SetMaleNewtPresent(true); }
	/** \brief Determines if a male is present in a pond */
	bool SupplyMaleNewtPresent(int a_InPondIndex) { return m_elems[a_InPondIndex]->IsMaleNewtPresent(); }
	/** \brief Returns a pointer to a farm owned by a farmer with a specific farm reference number */
	Farm* SupplyFarmPtr(int a_owner) { return m_FarmManager->GetFarmPtr(a_owner); }
	/** \brief Returns m_FarmManager, the pointer to the farm manager instance */
	FarmManager* SupplyFarmManagerPtr() { return m_FarmManager; }
	/** \brief Returns m_land, the pointer to the raster map instance */
	RasterMap* SupplyRasterMap() { return m_land; }
	/** \brief Returns m_LargestPolyNumUsed */
	int SupplyLargestPolyNumUsed() { return m_LargestPolyNumUsed; }
	/** \brief Returns m_toxShouldSpray, a flag indicating whether pesticide should be sprayed */
	bool SupplyShouldSpray() { return m_toxShouldSpray; }
	/** \brief Returns m_countryCode, e.g. "DK" for Denmark */
	std::string SupplyCountryCode() { return m_countryCode; };
	int SupplyTimezone() {
		if (m_countryCode == "PL")
			return 2;
		else if (m_countryCode == "PT")
			return 0;
		else
			return 1; // default Denmark +1
	}
	/** \brief Sets m_countryCode, e.g. "DK" for Denmark */
	void SetCountryCode(std::string country_code) { m_countryCode = country_code; };
	/** \brief Returns the current landscape name */
	string SupplyLandscapeName() { return LandscapeName; }

	/** \brief This is the jumping off point for any landscape related species setup, it creates function pointers to these special functions */
	void SetSpeciesFunctions(TTypesOfPopulation a_species);

	/** \brief Function to set the given polygon lock*/
	void SetPolygonLock(int a_polyref) { omp_set_nest_lock(m_poly_omp_locks[a_polyref]); }
	/** \brief Function to release the given polygon locks*/
	void ReleasePolygonLock(int a_polyref) { omp_unset_nest_lock(m_poly_omp_locks[a_polyref]); }
	/** \brief Returns m_latitude of the landscape location */
	double SupplyLatitude() { return	m_latitude; };
	/** \brief Returns m_longitude of the landscape location*/
	double SupplyLongitude() { return	m_longitude; };
	/** \brief Sets the latitude of the landscape location */
	void SetLatitude(double latitude) { m_latitude = latitude; };
	/** \brief Sets the longitude of the landscape location */
	void SetLongitude(double longitude) { m_longitude = longitude; };
	/** \brief Gets the digestibility of the vegetation based on an index to the Landscape::m_elems array */
	double SupplyVegDigestibilityVector(unsigned int a_index);
	/** \brief Gets the digestibility of the vegetation for a polygon given by a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyVegDigestibility(int a_polyref);
	/** \brief Gets the digestibility of the vegetation for a polygon given by a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyVegDigestibility(int a_x, int a_y);
	/** \brief Returns the height of the vegetation using the index to Landscape::m_elems */
	double SupplyVegHeightVector(unsigned int a_index);
	/** \brief Returns the height of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyVegHeight(int a_polyref);
	/** \brief Returns the height of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyVegHeight(int a_x, int a_y);
	/** \brief Returns the biomass of the vegetation using the index a_index to Landscape::m_elems */
	double SupplyVegBiomassVector(unsigned int a_index);
	/** \brief Returns the biomass of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyVegBiomass(int a_polyref);
	/** \brief Returns the biomass of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyVegBiomass(int a_x, int a_y);
	/** \brief Returns the density of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	int SupplyVegDensity(int a_polyref);
	/** \brief Returns the density of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	int SupplyVegDensity(int a_x, int a_y);
	/** \brief Returns the weed biomass of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyWeedBiomass(int a_polyref);
	/** \brief Returns the weed biomass of the vegetation using the polygon reference number a_polyref or based on the x, y coordinates given by a_x, a_y */
	double SupplyWeedBiomass(int a_x, int a_y);
	/** \brief Returns the presence of skylark scrapes in the vegetation using the polygon reference number a_polyref */
	bool SupplySkScrapes(int a_polyref);
	/** \brief Returns the insterested green biomass of the vegetation using the polygon reference number a_polyref */
	double SupplyInterestedGreenBiomass(int a_polyref);
	/** \brief Returns the interested green biomass of the vegetation using the polygon reference number a_polyref */
	double SupplyInterestedGreenBiomass(int a_x, int a_y);
	/** \brief Returns the interested green biomass of the vegetation+weeds using the polygon reference number a_polyref */
	double SupplyInterestedGreenBiomassTotal(int a_polyref);
	/** \brief Returns the interested green biomass of the vegetation+weeds using the x,y coordinate */
	double SupplyInterestedGreenBiomassTotal(int a_x, int a_y);
	/** \brief Returns the green biomass of the vegetation using the polygon reference number a_polyref */
	double SupplyGreenBiomass(int a_polyref);
	/** \brief Returns the green biomass of the vegetation using the polygon reference number a_polyref */
	double SupplyGreenBiomass(int a_x, int a_y);
	/** \brief Returns the green biomass of the vegetation+weeds using the polygon reference number a_polyref */
	double SupplyGreenBiomassTotal(int a_polyref);
	/** \brief Returns the green biomass of the vegetation+weeds using the x,y coordinate */
	double SupplyGreenBiomassTotal(int a_x, int a_y);
	/** \brief Returns the green biomass as a proportion of biomass of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyGreenBiomassProp(int a_polyref);
	/** \brief Returns the green biomass as a proportion of biomass of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyGreenBiomassProp(int a_x, int a_y);
	/** \brief Returns the vegetation growth stage of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyVegGrowthStage(int a_polyref);
	/** \brief Returns the vegetation growth stage of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyVegGrowthStage(int a_x, int a_y);
	/** \brief Returns the dead biomass of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyDeadBiomass(int a_polyref);
	/** \brief Returns the dead biomass of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyDeadBiomass(int a_x, int a_y);
	/** \brief Returns the green leaf area of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyLAGreen(int a_polyref);
	/** \brief Returns the green leaf area of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyLAGreen(int a_x, int a_y);
	/** \brief Returns leaf area in total of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyLATotal(int a_polyref);
	/** \brief Returns leaf area in total of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyLATotal(int a_x, int a_y);
	/** \brief Returns the vegetation cover proportion of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyVegCover(int a_polyref);
	/** \brief Returns the vegetation cover proportion of the vegetation using the polygon index to Landscape::m_elems */
	double SupplyVegCoverVector(unsigned int a_index);
	/** \brief Returns the vegetation cover proportion of the vegetation using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyVegCover(int a_x, int a_y);
	/** \brief Returns the last type of vegetation sown on a field using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	TTypesOfVegetation SupplyLastSownVeg(int a_polyref);
	/** \brief Returns the last type of vegetation sown on a field using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	TTypesOfVegetation SupplyLastSownVeg(int a_x, int a_y);
	/** \brief Returns the last type of vegetation sown on a field using the polygon index to Landscape::m_elems */
	TTypesOfVegetation SupplyLastSownVegVector(unsigned int a_index);
	/** \brief Returns the insect biomass on a polygon using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyInsects(int a_polyref);
	/** \brief Returns the insect biomass on a polygon using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	double SupplyInsects(int a_x, int a_y);
	/** \brief Removes larval food from a pond and returns true if it was possible, otherwise false */
	bool SubtractPondLarvalFood(double a_food, int a_polyrefindex);
	/** \brief Gets total seed coating for the centorid of a polygen. */
	double SupplySeedCoating(int a_polyref, PlantProtectionProducts a_ppp);
	/** \brief Returns true if there is any pesticide in the system at all at this point */
	bool SupplyPesticideDecay(PlantProtectionProducts a_ppp);
	/** \brief Gets total pesticide for a location. \note The pesticide engine is not included yet, so it always return 0. */
	double SupplyPesticide(int a_x, int a_y, PlantProtectionProducts a_ppp) {return 0;}
	/** \brief Gets the overspray flag */
	bool SupplyOverspray(int a_x, int a_y);
	/** \brief Gets plant pesticide for a location */
	double SupplyPesticideP(int a_x, int a_y, PlantProtectionProducts a_ppp);
	/** \brief Gets soil pesticide for a location */
	double SupplyPesticideS(int a_x, int a_y, PlantProtectionProducts a_ppp);
	/** \brief Gets plant surface pesticide for a location */
	double SupplyPesticidePlantSurface(int a_x, int a_y, PlantProtectionProducts a_ppp);
	/** \brief Gets pesticide within plant for a location */
	double SupplyPesticideInPlant(int a_x, int a_y, PlantProtectionProducts a_ppp);
	/** \brief Gets nectar pesticide for a location */
	double SupplyPesticideNectar(int a_x, int a_y, PlantProtectionProducts a_ppp);
	/** \brief Gets pollen pesticide for a location */
	double SupplyPesticidePollen(int a_x, int a_y, PlantProtectionProducts a_ppp);
	/** \brief Gets the sprayed pesticide type in the given location.*/
	PlantProtectionProducts SupplySprayPesticideType(int a_x, int a_y);
	/** \brief Gets the sprayed pesticide rate in the given location*/
	double SupplySprayPesticideRate(int a_x, int a_y);
	/** \brief Gets total pesticide for the centroid of a polygon */
	double SupplyPesticide(int a_polyref, PlantProtectionProducts a_ppp) {return 0;}
	/** \brief Gets plant pesticide for the centroid of a polygon */
	double SupplyPesticideP(int a_polyref, PlantProtectionProducts a_ppp);
	/** \brief Gets soil pesticide for the centroid of a polygon */
	double SupplyPesticideS(int a_polyref, PlantProtectionProducts a_ppp);
	/** \brief Gets plant surface pesticide for the centroid of a polygon*/
	double SupplyPesticidePlantSurface(int a_polyref, PlantProtectionProducts a_ppp);
	/** \brief Gets pesticide within plant for the centroid of a polygon*/
	double SupplyPesticideInPlant(int a_polyref, PlantProtectionProducts a_ppp);
	/** \brief Gets nectar pesticide for the centroid of a polygon*/
	double SupplyPesticideNectar(int a_polyref, PlantProtectionProducts a_ppp);
	/** \brief Gets pollen pesticide for the centroid of a polygon*/
	double SupplyPesticidePollen(int a_polyref, PlantProtectionProducts a_ppp);
	/** \brief Gets the sprayed pesticide type in the given field*/
	PlantProtectionProducts SupplySprayPesticideType(int a_polyref);
	/** \brief Gets the sprayed pesticide rate in the given field*/
	double SupplySprayPesticideRate(int a_polyref);
	/** \brief Gets type of pesticide effect from a reference */
	TTypesOfPesticide SupplyPesticideType(void) { return m_PesticideType; }
	/** \brief Gets the list of suitable goose foraging fields today */
	GooseFieldList* GetGooseFields(double);
	/** \brief Resets all grain */
	void ResetGrainAndMaize();
	/** \brief Causes openness to be calulated and stored for all polygons */
	void CalculateOpenness(bool a_realcalc);
	/** \brief Stores openness for all polygons to a standard file LKM - used??*/
	void WriteOpenness(void);
	/** \brief Reads openness values from a standard input file for all polygons LKM - used?? */
	void ReadOpenness(void);
	/** \brief Provides a measure of the shortest distance in 360 degree, e-g- looking NE % SW before tall obstacles are encountered at both ends. Searches from centroid.*/
	int CalculateFieldOpennessCentroid(int  a_pref);
	/** \brief Provides a measure of the shortest distance in 360 degree, e-g- looking NE % SW before tall obstacles are encountered at both ends. Checks all field 1m2 */
	int CalculateFieldOpennessAllCells(int  a_pref);
	/** \brief Provides a measure of the shortest distance in using a vector from a_cx,a_cy unitl tall obstacles are encountered in both +ve & -ve directions. */
	int LineHighTest(int a_cx, int a_cy, double a_offsetx, double a_offsety);
	/** \brief Get openness for a polygon */
	int SupplyOpenness(int a_poly) { return m_elems[m_polymapping[a_poly]]->GetOpenness(); }
	/** \brief Get openness for a location */
	int SupplyOpenness(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->GetOpenness(); }
	/** \brief Returns a pointer to a list of polygonrefs to large open fields within a range of location x,y */
	polylist* SupplyLargeOpenFieldsNearXY(int x, int y, int range, int a_openness);
	/** \brief For the roe deer, determines the extent of human activity and returns bool */
	bool SupplyIsHumanDominated(TTypesOfLandscapeElement a_tole_type);
	/** \brief Returns the soil type in ALMaSS types reference numbers */
	int SupplySoilType(int a_x, int a_y) {
		return m_elems[m_land->Get(a_x, a_y)]->GetSoilType();
	}
	/** \brief Returns the soil type in rabbit warren reference numbers */
	int SupplySoilTypeR(int a_x, int a_y) {
		return m_elems[m_land->Get(a_x, a_y)]->GetSoilTypeR();
	}
	/** \brief Returns the centroid of a polygon using the polygon reference number a_polyref */
	APoint SupplyCentroid(int a_polyref);
	/** \brief Returns the centroid of a polygon using the polygon index in m_elems */
	APoint SupplyCentroidIndex(int a_polyrefindex);
	/** \brief Returns the centroid x-coordinate of a polygon using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	int SupplyCentroidX(int a_polyref) {
		return m_elems[m_polymapping[a_polyref]]->GetCentroidX();
	}
	/** \brief Returns the centroid y-coordinate of a polygon using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	int SupplyCentroidY(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->GetCentroidY(); }
	/** \brief Returns the centroid x-coordinate of a polygon using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	int SupplyCentroidX(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->GetCentroidX(); }
	/** \brief Returns the centroid y-coordinate of a polygon using the polygon reference number a_polyref or based on the x,y coordinates given by a_x, a_y */
	int SupplyCentroidY(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->GetCentroidY(); }
	/** \brief Returns the farm intensity classification of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y */
	double SupplyFarmIntensity(int a_x, int a_y);
	/** \brief Returns the farm intensity classification of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y */
	double SupplyFarmIntensity(int a_polyref);
	/** \brief Returns the farm intensity classification of the polygon using the polygon index in m_elems */
	double SupplyFarmIntensityI(int a_polyindex);
	/** \brief Returns the landscape type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y. Returns the value as a #TTypesOfLandscapeElement */
	TTypesOfLandscapeElement SupplyElementType(int a_polyref);
	/** \brief Returns the landscape type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y. Returns the value as a #TTypesOfLandscapeElement */
	TTypesOfLandscapeElement SupplyElementType(int a_x, int a_y);
	/** \brief Returns the landscape type of the polygon using coordinates a_x, a_y, but allowing correction of coordinates for out of scope values. Returns the value as a #TTypesOfLandscapeElement */
	TTypesOfLandscapeElement SupplyElementTypeCC(int a_x, int a_y);
	/** \brief Returns the landscape type of the polygon owner (used in cases relating to e.g. unsprayed field margins) using coordinates a_x, a_y. Returns the value as a #TTypesOfLandscapeElement */
	TTypesOfLandscapeElement GetOwner_tole(int a_x, int a_y);
	/** \brief Returns the designation of country or urban of the polygon using coordinates a_x, a_y */
	int SupplyCountryDesig(int a_x, int a_y);
	/** \brief Returns the landscape element sub-type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y */
	int SupplyElementSubType(int a_polyref);
	/** \brief Returns the landscape element sub-type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y */
	int SupplyElementSubType(int a_x, int a_y);
	/** \brief Returns the vegetation type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y.  Returns the value as a # TTypesOfVegetation */
	TTypesOfVegetation       SupplyVegType(int a_x, int a_y);
	/** \brief Returns the vegetation type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y.  Returns the value as a # TTypesOfVegetation */
	TTypesOfVegetation       SupplyVegType(int polyref);
	/** \brief Returns the crop type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y.  Returns the value as a # TTypesOfCrops */
	TTypesOfCrops       SupplyCropType(int a_x, int a_y);
	/** \brief Returns the crop type of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y.  Returns the value as a # TTypesOfCrops */
	TTypesOfCrops       SupplyCropType(int polyref);
	/** \brief Returns the vegetation type of the polygon using the polygon index to m_elems.  Returns the value as a # TTypesOfVegetation */
	TTypesOfVegetation       SupplyVegTypeVector(unsigned int a_index);
	/** \brief Returns the grazing pressure of the polygon using the polygon index to m_elems */
	int  SupplyGrazingPressureVector(unsigned int a_index);
	/** \brief Returns the grazing pressure of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y */
	int  SupplyGrazingPressure(int a_polyref);
	/** \brief Returns the grazing pressure of the polygon using the polygon reference number a_polyref or coordinates a_x, a_y */
	int  SupplyGrazingPressure(int a_x, int a_y);
	/** \brief Returns m_poly_with_flowers, a list of polygons with flowering modelled */
	vector<int> SupplyPolyWithFlowers(void) { return m_poly_with_flowers; }

	// ------  ATTRIBUTES
	/** \brief Tests whether the polygon at a_x,a_y is designated as xxx */
	/** \brief Returns whether a polygon at coordinates a_x, a_y has the attribute High set */
	inline bool SupplyAttIsHigh(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->Is_Att_High(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y has the attribute Water set */
	inline bool SupplyAttIsWater(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->Is_Att_Water(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y has the attribute Forest set */
	inline bool SupplyAttIsForest(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->Is_Att_Forest(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y has the attribute Forest set */
	inline bool SupplyAttIsForest(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_Forest(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y has the attribute Woody */
	inline bool SupplyAttIsWoody(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->Is_Att_Woody(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y, or by a_polyref has the attribute High set */
	inline bool SupplyAttIsUrbanNoVeg(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->Is_Att_UrbanNoVeg(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y, or by a_polyref has the attribute High set */
	inline bool SupplyAttIsUrbanNoVeg(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_UrbanNoVeg(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y, or by a_polyref has the attribute Is Patchy set */
	inline bool SupplyAttIsVegPatchy(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_VegPatchy(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y, or by a_polyref has the attribute Is Patchy set */
	inline bool SupplyAttIsVegPatchy(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->Is_Att_VegPatchy(); };
	/** \brief Returns whether a polygon at coordinates a_x, a_y, or by a_polyref has the attribute Is Cereal set */
	inline bool SupplyAttIsVegCereal(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_VegCereal(); }
	/** \brief Returns whether a polygon at coordinates a_x, a_y, or by a_polyref has the attribute Is Mature Cereal set */
	inline bool SupplyAttIsVegMatureCereal(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_VegMatureCereal(); }
	/** \brief Returns whether a polygon a_polyref has the attribute Is Grass set */
	inline bool SupplyAttIsVegGrass(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_VegGrass(); }
	/** \brief Returns whether a polygon a_polyref has the attribute Is Goose Grass set */
	inline bool SupplyAttIsVegGooseGrass(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_VegGooseGrass(); }
	/** \brief Returns whether a polygon a_polyref has the attribute Is Maize set */
	inline bool SupplyAttIsVegMaize(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_VegMaize(); }
	// The four below are for species specific interactions with attribues
	/** \brief Returns the user defined boolean attribute of a polygon using the polygon reference number a_polyref, or coordinates a_x, a_y */
	inline bool SupplyAttUserDefinedBool(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_UserDefinedBool(); }
	/** \brief Returns the user defined boolean attribute of a polygon using the polygon reference number a_polyref, or coordinates a_x, a_y */
	inline bool SupplyAttUserDefinedBool(int a_x, int a_y) { return m_elems[m_land->Get(a_x, a_y)]->Is_Att_UserDefinedBool(); }
	/** \brief Returns the user defined integer attribute of a polygon using the polygon reference number a_polyref */
	inline int  SupplyAttUserDefinedInt(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->Is_Att_UserDefinedInt(); }
	/** \brief Sets the user defined boolean attribute of a polygon using the polygon reference number a_polyref */
	inline void SetAttUserDefinedBool(int a_polyref, bool a_value) { m_elems[m_polymapping[a_polyref]]->Set_Att_UserDefinedBool(a_value); }
	/** \brief ets the user defined integer attribute of a polygon using the polygon reference number a_polyref */
	inline void  SetAttUserDefinedInt(int a_polyref, int a_value) { m_elems[m_polymapping[a_polyref]]->Set_Att_UserDefinedInt(a_value); }
	/** \brief Returns whether the polygon is classfied as high */
	bool ClassificationHigh(TTypesOfLandscapeElement tole);
	/** \brief Returns whether the polygon is classfied as water */
	bool ClassificationWater(TTypesOfLandscapeElement tole);
	/** \brief Returns whether the polygon is classfied as a field */
	bool ClassificationFieldType(TTypesOfLandscapeElement a_tole);
	/** \brief Returns whether the polygon is classfied as urban with no vegetation */
	bool ClassificationUrbanNoVeg(TTypesOfLandscapeElement a_tole);
	/** \brief Returns whether the polygon is classfied as woodland */
	bool ClassificationForest(TTypesOfLandscapeElement a_tole);
	/** \brief Returns whether the polygon is classfied as woody */
	bool ClassificationWoody(TTypesOfLandscapeElement a_tole);

	/** \brief used to classify the userdefinedbool attribute and set it - this requires supplying the correct classification function */
	void Set_all_Att_UserDefinedBool(bool (*udf_fnc)(TTypesOfLandscapeElement))
	{
		for (auto& elem : m_elems)

		{
			elem->Set_Att_UserDefinedBool(udf_fnc(elem->GetElementType()));
		}
	}

	/** \brief used to classify the userdefinedint attribute and set it - this requires supplying the correct classification function */
	inline void Set_all_Att_UserDefinedInt(int (*udf_fnc)(TTypesOfLandscapeElement))
	{
		for (auto& elem : m_elems)

		{
			elem->Set_Att_UserDefinedInt(udf_fnc(elem->GetElementType()));
		}
	}

	/** \brief Returns whether a vegetation type is classified as Grass */ 
	bool ClassificationVegGrass(TTypesOfVegetation a_vege_type);
	/** \brief Returns whether a vegetation type is classified as cereal */
	bool ClassificationVegCereal(TTypesOfVegetation a_vege_type);
	/** \brief Returns whether a vegetation type is classified as cereal grown to maturity */
	bool ClassificationVegMatureCereal(TTypesOfVegetation a_vege_type);
	/** \brief Returns whether a vegetation type is classified as Goose Grass */
	bool ClassificationVegGooseGrass(TTypesOfVegetation a_vege_type);
	/** \brief Returns whether a vegetation type is classified as maize */
	bool ClassificationVegMaize(TTypesOfVegetation a_vege_type);
	/** \brief Returns whether a vegetation type is classified as a permanent crop */
	bool ClassificationPermCrop(TTypesOfVegetation a_vege_type);

	// To add here all Attributes as they come along 
	/** \brief Uses a point to an LE instance and sets all the attributes based on its tole type */
	inline void Set_TOLE_Att(LE* elem) {
		/** \brief Sets attribute to high */
		elem->Set_Att_High(ClassificationHigh(elem->GetElementType()));
		/** \brief Sets attribute to water */
		elem->Set_Att_Water(ClassificationWater(elem->GetElementType()));
		/** \brief Sets attribute to urban areas without vegetation */
		elem->Set_Att_UrbanNoVeg(ClassificationUrbanNoVeg(elem->GetElementType()));
		/** \brief Sets attribute to woodland */
		elem->Set_Att_Forest(ClassificationForest(elem->GetElementType()));
		/** \brief Sets attribute to woody */
		elem->Set_Att_Woody(ClassificationWoody(elem->GetElementType()));
	}

	// To add here all Veg (TOV) Attributes as they come along 
	/** \brief Uses a point to an LE instance and sets all the attributes based on its tov type */
	inline void Set_TOV_Att(LE* elem) {
		// patchy is set other places for now 
		/** \brief Sets attribute to grass vegetation */
		elem->Set_Att_VegGrass(ClassificationVegGrass(elem->GetVegType()));
		/** \brief Sets attribute to grass vegetation for geese */
		elem->Set_Att_VegGooseGrass(ClassificationVegGooseGrass(elem->GetVegType()));
		/** \brief Sets attribute to cereal*/
		elem->Set_Att_VegCereal(ClassificationVegCereal(elem->GetVegType()));
		/** \brief Sets attribute to mature cereal */
		elem->Set_Att_VegMatureCereal(ClassificationVegMatureCereal(elem->GetVegType()));
		/** \brief Sets attribute to maize */
		elem->Set_Att_VegMaize(ClassificationVegMaize(elem->GetVegType()));
	}

	// for easy compatibility of code after introduction of attributes 
	/** \brief Function for backwards code compatibility when Set_Att_High would now be used */
	inline bool SupplyLEHigh(int a_x, int a_y) { return SupplyAttIsHigh(a_x, a_y); }
	/** \brief Function for backwards code compatibility when TOV attributes would now be used */
	inline bool IsFieldType(TTypesOfLandscapeElement a_tole) { return ClassificationFieldType(a_tole); }
	/** \brief Function for backwards code compatibility when cereal attributes would now be used */
	inline bool SupplyIsCereal2(TTypesOfVegetation a_vege_type) { return ClassificationVegCereal(a_vege_type); }
	/** \brief Function for backwards code compatibility when grass attributes would now be used */
	inline bool SupplyIsGrass2(TTypesOfVegetation a_vege_type) { return ClassificationVegGrass(a_vege_type); }
	/** \brief Returns whether the polygon referenced by a_polyref or a_x, a_y has a vegetation type designated as patchy */
	inline bool SupplyVegPatchy(int a_polyref) { return SupplyAttIsVegPatchy(a_polyref); }
	/** \brief Returns whether the polygon referenced by a_polyref or a_x, a_y has a vegetation type designated as patchy */
	inline bool SupplyVegPatchy(int a_x, int a_y) { return SupplyAttIsVegPatchy(a_x, a_y); }

	// ----------------------------------------------------------------------------

	/** \brief Returns whether the polygon referenced by a_polyref or a_x, a_y has a vegetation type with tramlines */
	bool  SupplyHasTramlines(int a_x, int a_y);
	/** \brief Returns whether the polygon referenced by a_polyref or a_x, a_y has a vegetation type with tramlines */
	bool  SupplyHasTramlines(int a_polyref);
	/** \brief Returns the whether the vegetation was mown for the polygon referenced the index to Landscape::m_elems */
	bool  SupplyJustMownVector(unsigned int a_index);
	/** \brief Returns the whether the vegetation was mown for the polygon referenced by a_polyref or a_x, a_y */
	bool  SupplyJustMown(int a_polyref);
	/** \brief Returns whether the polygon referenced by an index to Landscape::m_elems has been sprayed in the last timestep */
	int   SupplyJustSprayedVector(unsigned int a_index);
	/** \brief Returns whether the polygon referenced by a_polyref or a_x, a_y has been sprayed in the last timestep */
	int   SupplyJustSprayed(int a_polyref);
	/** \brief Returns whether the polygon referenced by a_polyref or a_x, a_y has been sprayed in the last timestep */
	int   SupplyJustSprayed(int a_x, int a_y);
	/** \brief Returns the tree age for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyTreeAge(int a_Polyref);
	/** \brief Returns the tree age for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyTreeAge(int /* a_x */, int /* a_y */) { return 0; }
	/** \brief Returns the vegetation age for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyVegAge(int a_Polyref);
	/** \brief Returns the vegetation age for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyVegAge(int a_x, int a_y);
	/** \brief Returns the number of farms in the current landscape */
	int	  SupplyNumberOfFarms();
	/** \brief Returns the farm owner pointer for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyFarmOwner(int a_x, int a_y);
	/** \brief Returns the farm owner pointer for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyFarmOwner(int a_polyref);
	/** \brief Returns the farm owner reference number for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyFarmOwnerIndex(int a_x, int a_y);
	/** \brief Returns the farm owner reference number for the polygon referenced by a_polyref or a_x, a_y */
	int   SupplyFarmOwnerIndex(int a_polyref);

	/** \brief Returns the farm type for the polygon referenced by a_polyref or a_x, a_y */
	TTypesOfFarm SupplyFarmType(int a_polyref);
	/** \brief Returns the farm type for the polygon referenced by a_polyref or a_x, a_y */
	TTypesOfFarm SupplyFarmType(int a_x, int a_y);

	/** \brief Returns filename for the rotation file (if any) for the polygon referenced by a_polyref or a_x, a_y */
	string SupplyFarmRotFilename(int a_polyref);
	/** \brief Returns filename for the rotation file (if any) for the polygon referenced by a_polyref or a_x, a_y */
	string SupplyFarmRotFilename(int a_x, int a_y);

	/** \brief Returns the farm area of a farm that owns a particular polygon */
	int SupplyFarmArea(int a_polyref);
	/** \brief Returns the polygon area of a particular polygon using an index to m_elems */
	double SupplyPolygonAreaVector(int a_polyref);
	/** \brief Returns the polygon area of a particular polygon referenced by polygon */
	double SupplyPolygonArea(int a_polyref) { return m_elems[m_polymapping[a_polyref]]->GetArea(); }
	/** \brief Declaration of attribute, m_grain_dist, a binary condition for good or bad years of spilled grain */
	int m_grain_dist;
	/** \brief Returns m_grain_dist, a binary condition for good or bad years of spilled grain */
	int SupplyGrainDist() { return m_grain_dist; }
	/** \brief Sets m_grain_dist, a binary condition for good or bad years of spilled grain */
	void SetGrainDist(int a_dist) { m_grain_dist = a_dist; }
	/** \brief Sets the grain forage resource as seen from a goose standpoint at a polygon */
	void SetBirdSeedForage(int a_polyref, double a_fooddensity)
	{
		m_elems[m_polymapping[a_polyref]]->SetBirdSeed(a_fooddensity);
	}
	/** \brief Sets the maize forage resource as seen from a goose standpoint at a polygon */
	void SetBirdMaizeForage(int a_polyref, double a_fooddensity) {
		m_elems[m_polymapping[a_polyref]]->SetBirdMaize(a_fooddensity);
	}
	/** \brief Returns the leaf forage resource as seen from a goose standpoint at a polygon based on the height only */
	double SupplyGooseGrazingForageH(double a_height, GooseSpecies a_goose)
	{
		/**
		* \param a_height [in] The vegetation height (assumed grass or cereals). This needs to be checked before calling
		* \param a_goose [in] Is the type of goose calling which is needed to determine how to assess the value of the current forage availability (ie its different for different types of geese)
		* \return KJ/min
		*/
		if (a_goose == gs_Pinkfoot)
		{
			return m_GooseIntakeRateVSVegetationHeight_PF->GetY(a_height);
		}
		if (a_goose == gs_Barnacle)
		{
			if (a_height == 0.0) return 0.0;
			else return m_GooseIntakeRateVSVegetationHeight_BG->GetY(a_height);
		}
		if (a_goose == gs_Greylag)
		{
			return m_GooseIntakeRateVSVegetationHeight_GL->GetY(a_height);
		}
		Warn("Landscape::SupplyGooseGrazingForage", "Unknown Goose Type");
		exit(1);
	}

	/**
	* \brief Returns the leaf forage resource as seen from a goose standpoint at a polygon referenced by number based on height only */
	double SupplyGooseGrazingForageH(int a_polygon, GooseSpecies a_goose)
	{
		/**
		* \param a_polygon [in] The polygon refence number for the polygon we are interested in (assumed grass or cereals). This needs to be checked before calling
		* \param a_goose [in] Is the type of goose calling which is needed to determine how to assess the value of the current forage availability (ie its different for different types of geese)
		* \return KJ/min
		*/
		if (a_goose == gs_Pinkfoot)
		{
			return m_GooseIntakeRateVSVegetationHeight_PF->GetY(m_elems[m_polymapping[a_polygon]]->GetVegHeight());
		}
		if (a_goose == gs_Barnacle)
		{
			return m_GooseIntakeRateVSVegetationHeight_BG->GetY(m_elems[m_polymapping[a_polygon]]->GetVegHeight());
		}
		if (a_goose == gs_Greylag)
		{
			return m_GooseIntakeRateVSVegetationHeight_GL->GetY(m_elems[m_polymapping[a_polygon]]->GetVegHeight());
		}
		Warn("Landscape::SupplyGooseGrazingForage", "Unknown Goose Type");
		exit(1);
	}
	/**
	* \brief Returns the leaf forage resource as seen from a goose standpoint at a polygon referenced by x,y location
	* \param a_x [in] The x-coordinate in a polygon we are interested in (assumed grass or cereals). This needs to be checked before calling
	* \param a_y [in] The x-coordinate in a polygon we are interested in (assumed grass or cereals). This needs to be checked before calling
	* \param a_goose [in] Is the type of goose calling which is needed to determine how to assess the value of the current forage availability (ie its different for different types of geese)
	* \return KJ/min
	*/
	double GetActualGooseGrazingForage(int a_x, int a_y, GooseSpecies a_goose)
	{
		return m_elems[m_land->Get(a_x, a_y)]->GetGooseGrazingForage(a_goose);
	}
	/**
	* \brief Returns the leaf forage resource as seen from a goose standpoint at a polygon referenced by x,y location
	* The amount of food avaiable as grazing resource based on the vegetation height is species specific.
	* \param a_polygon [in] The polygon refence number for the polygon we are interested in (assumed grass or cereals). This needs to be checked before calling
	* \param a_goose [in] Is the type of goose calling which is needed to determine how to assess the value of the current forage availability (ie its different for different types of geese)
	* \return KJ/min
	*/
	double GetActualGooseGrazingForage(int a_polygon, GooseSpecies a_goose)
	{
		return m_elems[m_polymapping[a_polygon]]->GetGooseGrazingForage(a_goose);
	}
	/**
	* \brief Returns the grain forage resource
	*/
	double SupplyBirdSeedForage(int a_polyref)
	{
		return m_elems[m_polymapping[a_polyref]]->GetBirdSeed();
	}
	/**
	* \brief Returns the grain forage resource as seen from a goose standpoint at an x,y location
	*/
	double SupplyBirdSeedForage(int a_x, int a_y)
	{
		return m_elems[m_polymapping[m_land->Get(a_x, a_y)]]->GetBirdSeed();
	}
	/**
	* \brief Returns the maize forage resource
	*/
	double SupplyBirdMaizeForage(int a_polyref) {
		return m_elems[m_polymapping[a_polyref]]->GetBirdMaize();
	}
	/**
	* \brief Returns whether its cereal in stubble
	*/
	bool SupplyInStubble(int a_polyref) {
		return m_elems[m_polymapping[a_polyref]]->GetStubble();
	}
	/**
	* \brief Returns the maize forage resource as seen from a goose standpoint at an x,y location
	*/
	double SupplyBirdMaizeForage(int a_x, int a_y) {
		return m_elems[m_land->Get(a_x, a_y)]->GetBirdMaize();
	}
	/**
	* \brief This records the number of geese on the polygon the day before. To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	* simulation day number can be used to modify the return value - see GetGooseNumbers()
	*/
	void RecordGooseNumbers(int a_poly, int a_number);
	/**
	* \brief This records the number of geese of each species on the polygon the day before. To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	* simulation day number can be used to modify the return value - see GetGooseNumbers()
	*/
	void RecordGooseSpNumbers(int a_poly, int a_number, GooseSpecies a_goose);
	/**
	* \brief This records the number of geese on the polygon the day before at a predefined time. To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	* simulation day number can be used to modify the return value - see GetGooseNumbers()
	*/
	void RecordGooseNumbersTimed(int a_poly, int a_number);
	/**
	* \brief This records the number of geese of each species on the polygon the day before at a predefined time. To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	* simulation day number can be used to modify the return value - see GetGooseNumbers()
	*/
	void RecordGooseSpNumbersTimed(int a_poly, int a_number, GooseSpecies a_goose);
	/**
	* \brief Records the distance to the closest roost of goose species
	*/
	void RecordGooseRoostDist(int a_polyref, int a_dist, GooseSpecies a_goose);
	/**
	* \brief Removes grazing forage from a poly per m2
	*/
	void GrazeVegetation(int a_poly, double a_forage) {
		m_elems[m_polymapping[a_poly]]->GrazeVegetation(a_forage, true);
	}
	/**
	* \brief Removes grazing forage from a poly and divides this out per m2
	*/
	void GrazeVegetationTotal(int a_poly, double a_forage) {
		m_elems[m_polymapping[a_poly]]->GrazeVegetationTotal(a_forage);
	}
	/**
	* \brief This returns the number of geese on the polygon the day before.
	*/
	int GetGooseNumbers(int a_poly);
	/**
	* \brief This returns the number of geese which are legal quarry on the polygon the day before.
	*/
	int GetQuarryNumbers(int a_poly);
	/**
	* \brief This returns the number of geese on the polygon specifed by a_x, a_y the day before.
	*/
	int GetGooseNumbers(int a_x, int a_y);
	/** \brief Returns the last treatment recorded for the polygon */
	int   SupplyLastTreatment(int a_polyref, int* a_index);
	/** \brief Returns the last treatment recorded for the polygon */
	int   SupplyLastTreatment(int a_x, int a_y, int* a_index);
	// Weather data.

	// Energizer Bunny Edition. Use these if at all possible.
	// Returns the requested data for *today* (whatever that is). LKM check all below (until  1152)
	/** \brief Passes a request on to the associated Weather class function, the global amount of sunshine for the current day*/
	double SupplyGlobalRadiation();
	/** \brief Passes a request on to the associated Weather class function, the global mean amount of sunshine for the current day*/
	double SupplyGlobalRadiation(long a_date);
	/** \brief Passes a request on to the associated Weather class function, the amount of rain for the current day*/
	double SupplyRain(void);
	/** \brief Passes a request on to the associated Weather class function, the temperature for the current day*/
	double SupplyTemp(void);
	/** \brief Passes a request on to the associated Weather class function, the minimum temperature for the current day*/
	double SupplyMinTemp(void);
	/** \brief Passes a request on to the associated Weather class function, the maximum temperature for the current day*/
	double SupplyMaxTemp(void);
	/** \brief Passes a request on to the associated Weather class function, the minimum temperature for yesterday*/
	double SupplyMinTempYesterday(void);
	/** \brief Passes a request on to the associated Weather class function, the maximum temperature for yesterday*/
	double SupplyMaxTempYesterday(void);
	/** \brief Passes a request on to the associated Weather class function, the minimum temperature for tomorrow*/
	double SupplyMinTempTomorrow(void);
	/** \brief Passes a request on to the associated Weather class function, the maximum temperature for tomorrow*/
	double SupplyMaxTempTomorrow(void);
	/** \brief Passes a request on to the associated Weather class function, the soil temperature for the current day*/
	double SupplySoilTemp(void);
	/** \brief Passes a request on to the associated Weather class function, the mean humidity for the current day*/
	double SupplyHumidity(void);
	/** \brief Passes a request on to the associated Weather class function, the mean temperature for the current day*/
	double SupplyMeanTemp(long a_date, unsigned int a_period);
	/** \brief Passes a request on to the associated Weather class function, the wind speed for the current day*/
	double SupplyWind(void);
	/** \brief Passes a request on to the associated Weather class function, the wind direction in 4 directions for the current day*/
	int   SupplyWindDirection(void);
	/** \brief Passes a request on to the associated Weather class function, the wind direction in 8 directions for the current day*/
	int   SupplyWindDirection8(void);
	/** \brief Passes a request on to the associated Weather class function, the wind direction in radians for the current day*/
	double   SupplyWindDirectionRadians(void);
	/** \brief Passes a request on to the associated Weather class function, the snow depth for the current day*/
	double  SupplySnowDepth(void);
	/** \brief Passes a request on to the associated Weather class function, the snow cover for the current day*/
	bool  SupplySnowcover(void);
	/** \brief Passes a request on to the associated Weather class function, the day length for the current day*/
	int   SupplyDaylength(void);

	//Supply weather data in the given hour (0 to 23) for today
	/** \brief Returns the temperature in a given hour (0 to 23) */
	double SupplyTempHour(int hour);
	/** \brief Returns the soil temperature in a given hour (0 to 23) */
	double SupplySoilTempHour(int hour);
	/** \brief Returns the wind speed in a given hour (0 to 23) */
	double SupplyWindHour(int hour);
	/** \brief Returns the amount of rain in a given hour (0 to 23) */
	double SupplyRainHour(int hour);
	/** \brief Returns the amount of sunshine in a given hour (0 to 23) */
	double SupplyRadiationHour(int hour);

	// *Yawn*! These work for all dates as expected, but
	// they are slow, slow, slzzz.... LKM - check all below (until line 1171)
	/** \brief Passes a request on to the associated Weather class function, the amount of rain for all dates*/
	double SupplyRain(long a_date);
	/** \brief Passes a request on to the associated Weather class function, the temperature for all dates*/
	double SupplyTemp(long a_date);
	/** \brief Passes a request on to the associated Weather class function, the soil temperature for all dates*/
	double SupplySoilTemp(long a_date);
	/** \brief Passes a request on to the associated Weather class function, the wind speed for all dates*/
	double SupplyWind(long a_date);
	/** \brief Passes a request on to the associated Weather class function, the day degrees for all dates*/
	double SupplyDayDegrees(int a_polyref);
	/** \brief Passes a request on to the associated Weather class function, the total amount of rain from a_date and a_period of days*/
	double SupplyRainPeriod(long a_date, int a_period);
	/** \brief Passes a request on to the associated Weather class function, the total wind speed from a_date and a_period of days*/
	double SupplyWindPeriod(long a_date, int a_period);
	/** \brief Passes a request on to the associated Weather class function, the total temperature from a_date and a_period of days*/
	double SupplyTempPeriod(long a_date, int a_period);
	//  double SupplyGlobalRadiation( a_date );

	// Warning: Known spooky behaviour, but it works, sort of...
	/** \brief Passes a request on to the associated Weather class function, the snow cover for the current day*/
	bool  SupplySnowcover(long a_date);

	// Misc.
	/** \brief Get the in map polygon reference number from the x, y location */
	int  SupplyPolyRef(int a_x, int a_y);
	/** \brief Get the unsprayed margin reference number from the polygon at x,y */
	int  SupplyUMRef(int a_x, int a_y);
	/** \brief Get the pointer to the LE object associated with the polygon at x,y location or by using the polygon reference */
	LE* SupplyPolyLEptr(int a_x, int a_y);
	/** \brief Get the pointer to the LE object associated with the polygon at x,y location or by using the polygon reference */
	LE* SupplyPolyLEptr(int a_polyref);
	/** \brief Get the index to the m_elems array for a polygon at location x,y */
	int  SupplyPolyRefIndex(int a_x, int a_y);
	/** \brief Get the in map polygon reference number from the x,y location, and correcting for possible wrap around coordinates */
	int  SupplyPolyRefCC(int a_x, int a_y);
	/** \brief Returns the pointer to m_elemns */
	vector<LE*>* SupplyPolyListPtr() { return &m_elems; }
	/** \brief Gets the simulation landscape width */
	int  SupplySimAreaWidth(void);
	/** \brief Gets the simulation landscape height */
	int  SupplySimAreaHeight(void);
	/** \brief Returns which ever is larger, height or width of simulation landscape */
	int  SupplySimAreaMaxExtent(void);
	/** \brief Returns which ever is smaller, height or width of simulation landscape LKM - used??*/
	int  SupplySimAreaMinExtent(void);
	/** \brief Passes a request on to the associated Calendar class function, the day length*/
	int  SupplyDaylength(long a_date);
	/** \brief Passes a request on to the associated Calendar class function, the day in the year*/
	int  SupplyDayInYear(void);
	/** \brief Passes a request on to the associated Calendar class function, the hour of the day*/
	int  SupplyHour(void);
	/** \brief Passes a request on to the associated Calendar class function, the minute of the hour*/
	int  SupplyMinute(void);
	/** \brief Returns the number of polygons in the landscape */
	unsigned int SupplyNumberOfPolygons(void);
	TTypesOfLandscapeElement
	/** \brief Gets the TTypesOfVegetation type of a polygon using the m_elems */
		SupplyElementTypeFromVector(unsigned int a_index);
	/** \brief Gets the farm owner reference of a polygon, or -1 if not owned by a farmer */
	TTypesOfLandscapeElement   SupplyOwner_tole(int a_x, int a_y);

	/** \brief Gets the polygon reference number from the index to m_elems */
	int   SupplyPolyRefVector(unsigned int a_index);
	/** \brief Gets the index to the cell used to store pesticide information from the coordinates x, y */
	int   SupplyPesticideCell(int a_polyref);
	/** \brief Returns an x-coordinate guaranteed to be within the polygon referenced */
	int   SupplyValidX(int a_polyref);
	/** \brief Returns an y-coordinate guaranteed to be within the polygon referenced */
	int   SupplyValidY(int a_polyref);

	/** \brief Get the pesticide concentration per liter from a pond (must be a pond index on calling) */
	double SupplyPondPesticide(int a_poly_index) { return dynamic_cast<Pond*>(m_elems[a_poly_index])->SupplyPondPesticide(); }

	/** \brief Function to prevent wrap around errors with co-ordinates using x/y pair */
	void  CorrectCoords(int& x, int& y);
	/** \brief Function to prevent wrap around errors with co-ordinates using x/y pair */
	APoint  CorrectCoordsPt(int x, int y);
	/** \brief Function to prevent wrap around errors with co-ordinates using a APoint*/
	void  CorrectCoordsPointNoWrap(APoint& a_pt);
	/** \brief Function to prevent wrap around errors with width LKM*/
	int   CorrectWidth(int x);
	/** \brief Function to prevent wrap around errors with height LKM*/
	int   CorrectHeight(int y);
	/** \brief Sets MapValid as true for each polygon found */
	void SetPolyMaxMinExtents(void);
	/** \brief Calculate centroid for each polygon found */
	void CalculateCentroids(void);
	/** \brief Saves centroid information for each polygon found */
	void DumpCentroids(void);
	/** \brief used to calculate whether a building as rural or town - for rodenticide use */
	void BuildingDesignationCalc();
	/** \brief Function to move from midx & midy and outwards in concentric circles until a location that matches the polyref is found LKM*/
	void CentroidSpiralOut(int a_polyref, int& a_x, int& a_y);
	//void CentroidSpiralOutBlocks(int a_polyref, int &a_x, int &a_y);
	/** \brief Returns version info LKM*/
	const char* SupplyVersion(void) { return m_versioninfo; }

	// Debugging, warning and configuration methods.
	/** \brief Dumps all configuration values with a security level at or below a_level to a_dumpfile in alphabetical order */
	void DumpPublicSymbols(const char* a_dumpfile,
		CfgSecureLevel a_level) {
		g_cfg->DumpPublicSymbols(a_dumpfile, a_level);
	}
	/** \brief Dumps all configuration values, including private ones, to a_dumpfile and then calls exit() */
	void DumpAllSymbolsAndExit(const char* a_dumpfile) {
		g_cfg->DumpAllSymbolsAndExit(a_dumpfile);
	}
	/** \brief Reads and parses a_cfgfile for configuration values */
	bool ReadSymbols(const char* a_cfgfile) {
		return g_cfg->ReadSymbols(a_cfgfile);
	}
	/** \brief Dumps landscape information by area LKM*/
	void  DumpMapInfoByArea(const char* a_filename,
		bool a_append,
		bool a_dump_zero_areas,
		bool a_write_veg_names
	);

	/** \brief Wrapper for the g_msg Warn function */
	void  Warn(std::string a_msg1, std::string a_msg2);

	// This is really, really naughty, but for the sake of efficiency...
	// SupplyMagicMapP( int a_x, int a_y ) returns a pointer to the map
	// at position x, y. The value found through it is an internal magic
	// number from the landscape simulator, that uniquely identifies a
	// polygon. It is however not the polygon number. To find that one
	// must convert the magic number via
	// MagicMapP2PolyRef( int a_magic ).
	/** \brief Returns a pointer to the map at position x, y. The value found through it is an internal number from the landscape simulator, that uniquely identifies a polygon, it is not the polygon number - to find this apply MagicMap2PolyRef */
	int* SupplyMagicMapP(int a_x, int a_y);
	/** \brief Converts the internal number generated from SupplyMagicMapP to the polygon number */
	int   MagicMapP2PolyRef(int a_magic);

	// For the Roe Deer LKM - check all below for Roe Deer
	/** \brief Returns the width of roads */
	int   SupplyRoadWidth(int, int);
	/** \brief Returns the traffic load */
	double SupplyTrafficLoad(int a_x, int a_y);
	/** \brief Returns the traffic load */
	double SupplyTrafficLoad(int a_polyref);
	/** \brief Returns the height of trees*/
	int   SupplyTreeHeight(int, int) { return 0; }
	/** \brief Returns the width of undergrowth */
	int   SupplyUnderGrowthWidth(int, int) { return 0; }
	/** \brief Returns the height of trees*/
	int   SupplyTreeHeight(int /* polyref */) { return 0; }
	/** \brief Returns the width of undergrowth */
	int   SupplyUnderGrowthWidth(int /* polyref */) { return 0; }

	// For the debugging programmer.
	// This list of functions simply passes a request on to the associated Calendar class function.
	/** \brief Passes a request on to the associated Calendar class function, returns the simulation global date for the day, month and year supplied */
	long SupplyGlobalDate(void);
	/** \brief Passes a request on to the associated Calendar class function, returns m_year */
	int  SupplyYear(void);
	/** \brief Passes a request on to the associated Calendar class function, returns m_simulationyear  */
	int  SupplyYearNumber(void);
	/** \brief Passes a request on to the associated Calendar class function, returns m_month + 1 (the calendar month)  */
	int  SupplyMonth(void);
	/** \brief Passes a request on to the associated Calendar class function, returns the month name  (e.g., January)  */
	std::string SupplyMonthName(void);
	/** \brief Passes a request on to the associated Calendar class function, returns m_day_in_month  */
	int  SupplyDayInMonth(void);
	/** \brief Passes a request on to the associated Calendar class function, returns m_daylightproportion  */
	double SupplyDaylightProp() { return g_date->GetDaylightProportion(); }
	/** \brief Passes a request on to the associated Calendar class function, returns 1.0 - m_daylightproportion  */
	double SupplyNightProp() { return 1.0 - g_date->GetDaylightProportion(); }

	/** \brief Resets internal state for the LE loop generator */
	void      SupplyLEReset(void);
	/** \brief Returns -1 at if the end of the list of m_elems is reached, polyref otherwise */
	int       SupplyLENext(void);
	/** \brief Returns number of all landscape elements */
	int       SupplyLECount(void);
	/** \brief Returns current signal mask for polygon */
	LE_Signal SupplyLESignal(int a_polyref);
	/** \brief Writes the mask back out to the polygon */
	void      SetLESignal(int a_polyref, LE_Signal a_signal);
	/** \brief Records that a farm treatment of the type specified has occurred */
	void IncTreatCounter(int a_treat);
	/** \brief Returns element type translated from the ALMaSS reference number */
	TTypesOfLandscapeElement TranslateEleTypes(int EleReference);
	/** \brief Returns vegetation type translated from the ALMaSS reference number */
	TTypesOfVegetation       TranslateVegTypes(int VegReference);
	/** \brief Returns element type translated from the ALMaSS reference string */
    TTypesOfLandscapeElement TranslateEleTypesFromString(string EleReference);
	/** \brief  Returns a pointer to the object referred to by the polygon number */
	LE* SupplyLEPointer(int a_polyref);
	/** \brief Returns the ALMaSS reference number translated from the TTypesOfLandscapeElement type */
	int BackTranslateEleTypes(TTypesOfLandscapeElement EleReference);
	/** \brief Returns the ALMaSS reference number translated from the TTypesOfVegetation type */
	int BackTranslateVegTypes(TTypesOfVegetation VegReference);

	/** \brief Returns the text representation of a treatment type */
	std::string EventtypeToString(int a_event);
	/** \brief Returns the text representation of a TTypesOfLandscapeElement type */
	std::string PolytypeToString(TTypesOfLandscapeElement a_le_type);
	/** \brief Returns the text representation of a TTypesOfVegetation type */
	std::string VegtypeToString(TTypesOfVegetation a_veg);
	/** \brief Returns the text name of a TTypesOfFarm type */
	std::string FarmtypeToString(TTypesOfFarm a_farmtype);
};

/*  Below this line we have all the inlined function definitions only */

inline void  Landscape::TurnTheWorld( void )
{
  Tick();
}

// We are interested in *speed*. Inlining all the lookup functions
// here and in the associated classes will cost us very little
// but a few KB of memory.

inline double Landscape::SupplyVegDigestibilityVector(unsigned int a_index )
{
  return m_elems[ a_index ]->GetDigestibility();
}

inline double Landscape::SupplyVegDigestibility( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetDigestibility();
}

inline double Landscape::SupplyVegDigestibility( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetDigestibility();
}

inline double Landscape::SupplyVegHeightVector( unsigned int a_index )
{
  return m_elems[ a_index ]->GetVegHeight();
}

inline double Landscape::SupplyVegHeight( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetVegHeight();
}

inline double Landscape::SupplyVegHeight( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetVegHeight();
}

inline double Landscape::SupplyVegBiomassVector( unsigned int a_index )
{
  return m_elems[ a_index ]->GetVegBiomass();
}

inline double Landscape::SupplyVegBiomass( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetVegBiomass();
}

inline double Landscape::SupplyVegBiomass( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetVegBiomass();
}

inline double Landscape::SupplyWeedBiomass( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetWeedBiomass();
}

inline double Landscape::SupplyWeedBiomass( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetWeedBiomass();
}

inline int Landscape::SupplyVegDensity( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetVegDensity();
}

inline bool Landscape::SupplySkScrapes( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetSkScrapes();
}

inline int Landscape::SupplyVegDensity( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetVegDensity();
}

inline double Landscape::SupplyLATotal(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetLATotal();
}

inline double Landscape::SupplyLATotal(int a_polyref)
{
	return m_elems[m_polymapping[a_polyref]]->GetLATotal();
}

inline double Landscape::SupplyLAGreen( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetLAGreen();
}

inline double Landscape::SupplyInterestedGreenBiomass( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetInterestedGreenBiomass();
}

inline double Landscape::SupplyInterestedGreenBiomass(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetInterestedGreenBiomass();
}

inline double Landscape::SupplyInterestedGreenBiomassTotal(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetInterestedGreenBiomass() + m_elems[m_land->Get(a_x, a_y)]->GetWeedBiomass();
}

inline double Landscape::SupplyInterestedGreenBiomassTotal(int a_polyref)
{
	return m_elems[m_polymapping[a_polyref]]->GetInterestedGreenBiomass() + m_elems[m_polymapping[a_polyref]]->GetWeedBiomass();
}

inline double Landscape::SupplyGreenBiomass( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetGreenBiomass();
}

inline double Landscape::SupplyGreenBiomass(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetGreenBiomass();
}

inline double Landscape::SupplyGreenBiomassTotal(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetGreenBiomass() + m_elems[m_land->Get(a_x, a_y)]->GetWeedBiomass();
}

inline double Landscape::SupplyGreenBiomassTotal(int a_polyref)
{
	return m_elems[m_polymapping[a_polyref]]->GetGreenBiomass() + m_elems[m_polymapping[a_polyref]]->GetWeedBiomass();
}

inline double Landscape::SupplyGreenBiomassProp( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetGreenBiomassProp();
}

inline double Landscape::SupplyGreenBiomassProp( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetGreenBiomassProp();
}

inline double Landscape::SupplyVegGrowthStage( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetVegGrowthStage();
}

inline double Landscape::SupplyVegGrowthStage( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetVegGrowthStage();
}

inline double Landscape::SupplyDeadBiomass( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetDeadBiomass();
}

inline double Landscape::SupplyDeadBiomass( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetDeadBiomass();
}

inline double Landscape::SupplyLAGreen( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetLAGreen();
}

inline double Landscape::SupplyVegCover( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetVegCover();
}

inline double Landscape::SupplyVegCoverVector( unsigned int a_index )
{
  return m_elems[ a_index ]->GetVegCover();
}

inline double Landscape::SupplyVegCover( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetVegCover();
}

inline TTypesOfVegetation Landscape::SupplyLastSownVegVector(unsigned int a_index)
{
	return m_elems[a_index]->GetLastSownVeg();
}

inline TTypesOfVegetation Landscape::SupplyLastSownVeg(int a_polyref)
{
	return m_elems[m_polymapping[a_polyref]]->GetLastSownVeg();
}

inline TTypesOfVegetation Landscape::SupplyLastSownVeg(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetLastSownVeg();
}

inline int Landscape::SupplyVegAge(int a_polyref)
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetVegAge();
}

inline int Landscape::SupplyVegAge( int a_x, int a_y )
{
	return m_elems[m_land->Get(a_x, a_y)]->GetVegAge();
}

inline double Landscape::SupplyInsects( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetInsectPop();
}

inline double Landscape::SupplyInsects( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetInsectPop();
}

inline LE* Landscape::SupplyLEPointer( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]];
}

inline TTypesOfLandscapeElement Landscape::SupplyElementTypeFromVector( unsigned int a_index )
{
  return m_elems[ a_index ]->GetElementType();
}

inline TTypesOfLandscapeElement Landscape::SupplyElementType( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetElementType();
}

inline TTypesOfLandscapeElement Landscape::SupplyElementType(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetElementType();
}

inline TTypesOfLandscapeElement Landscape::GetOwner_tole(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetOwner_tole();
}

inline int Landscape::SupplyElementSubType( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetSubType();
}

inline int Landscape::SupplyElementSubType( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetSubType();
}

inline int Landscape::SupplyCountryDesig( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetCountryDesignation();
}

inline TTypesOfLandscapeElement
  Landscape::SupplyElementTypeCC( int a_x, int a_y )
{
  a_x = (a_x + m_width10) % m_width;
  a_y = (a_y + m_height10) % m_height;
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetElementType();
}

inline int Landscape::SupplyNumberOfFarms( ) {
	return m_FarmManager->GetNoFarms();
}

inline int Landscape::SupplyFarmOwner( int a_x, int a_y ) {
	return m_elems[ m_land->Get( a_x, a_y ) ]->GetOwnerFile();
}

inline int Landscape::SupplyFarmOwner( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetOwnerFile();
}

inline int Landscape::SupplyFarmOwnerIndex( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetOwnerIndex();
}

inline int Landscape::SupplyFarmOwnerIndex( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetOwnerIndex();
}

inline TTypesOfFarm Landscape::SupplyFarmType( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetOwner()->GetType();
}

inline TTypesOfFarm Landscape::SupplyFarmType( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetOwner()->GetType();
}

inline string Landscape::SupplyFarmRotFilename(int a_polyref)

{

	return m_elems[m_polymapping[a_polyref]]->GetOwner()->GetRotFilename();

}

inline string Landscape::SupplyFarmRotFilename(int a_x, int a_y)

{

	return m_elems[m_land->Get(a_x, a_y)]->GetOwner()->GetRotFilename();

}

/** \brief Returns the area of a polygon using the vector index as a reference */
inline double Landscape::SupplyPolygonAreaVector( int a_polyref )
{
  return m_elems[ a_polyref ]->GetArea();
}

inline int Landscape::SupplyFarmArea( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetOwner()->GetArea();
}

inline TTypesOfVegetation
  Landscape::SupplyVegType( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ] ]->GetVegType();
}

inline TTypesOfVegetation
  Landscape::SupplyVegTypeVector( unsigned int a_index )
{
  return m_elems[ a_index ]->GetVegType();
}

inline TTypesOfCrops
  Landscape::SupplyCropType( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ] ]->GetCropType();
}

inline int Landscape::SupplyGrazingPressure( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ] ]->GetCattleGrazing();
}

inline int Landscape::SupplyGrazingPressureVector( unsigned int a_index )
{
  return m_elems[ a_index ]->GetCattleGrazing();
}

inline int Landscape::SupplyGrazingPressure( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetCattleGrazing();
}



inline bool Landscape::SupplyHasTramlines( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ] ]->HasTramlines();
}

inline bool Landscape::SupplyHasTramlines( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->HasTramlines();
}

inline bool Landscape::SupplyJustMownVector( unsigned int a_index )
{
  return m_elems[ a_index ]->IsRecentlyMown();
}

inline bool Landscape::SupplyJustMown( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ] ]->IsRecentlyMown();
}

inline int Landscape::SupplyJustSprayedVector( unsigned int a_index )
{
  return m_elems[ a_index ]->IsRecentlySprayed();
}

inline int Landscape::SupplyJustSprayed( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ] ]->IsRecentlySprayed();
}

inline int Landscape::SupplyJustSprayed( int a_x, int a_y )
{
  return m_elems[  m_land->Get( a_x, a_y ) ]->IsRecentlySprayed();
}

inline double Landscape::SupplyTrafficLoad( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ] ]->GetTrafficLoad();
}


inline double Landscape::SupplyTrafficLoad(int a_x, int a_y)
{
    return m_elems[m_land->Get(a_x, a_y)]->GetTrafficLoad();
}

inline int Landscape::SupplyRoadWidth(int a_x, int a_y)
{
    TTypesOfLandscapeElement tole = m_elems[m_land->Get(a_x, a_y)]->GetElementType();
    if (tole == tole_LargeRoad) return 12;
    else if (tole == tole_SmallRoad) return 6;
    return 0;
}

inline int Landscape::SupplyTreeAge( int /* a_polyref */ )
{
  return 1;
}

inline TTypesOfVegetation
  Landscape::SupplyVegType( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetVegType();
}

inline TTypesOfCrops
  Landscape::SupplyCropType( int a_x, int a_y )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetCropType();
}

inline double Landscape::SupplyDayDegrees( int a_polyref )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetDayDegrees();
}

inline int   Landscape::SupplyLastTreatment( int a_polyref, int *a_index )
{
  return m_elems[ m_polymapping[ a_polyref ]]->GetLastTreatment( a_index );
}

inline int   Landscape::SupplyLastTreatment( int a_x, int a_y, int *a_index )
{
  return m_elems[ m_land->Get( a_x, a_y ) ]->GetLastTreatment( a_index );
}

inline double Landscape::SupplyGlobalRadiation()
{
  return g_weather->GetGlobalRadiation( );
}

inline double Landscape::SupplyGlobalRadiation( long a_date )
{
  return g_weather->GetGlobalRadiation( a_date );
}

inline double Landscape::SupplyRainPeriod( long a_date, int a_period )
{
  return g_weather->GetRainPeriod( a_date, a_period );
}

inline double Landscape::SupplyRain( long a_date )
{
  return g_weather->GetRain( a_date );
}

inline double Landscape::SupplyRain( void )
{
  return g_weather->GetRain();
}



inline double Landscape::SupplyMeanTemp( long a_date, unsigned int a_period )
{
  return g_weather->GetMeanTemp( a_date, a_period );
}

inline double Landscape::SupplyTemp(long a_date)
{
    return g_weather->GetTemp(a_date);
}

inline double Landscape::SupplySoilTemp(long a_date)
{
    return g_weather->GetSoilTemp(a_date);
}

inline double Landscape::SupplyTemp(void)
{
    return g_weather->GetTemp();
}

inline double Landscape::SupplyMinTemp(void)
{
    return g_weather->GetMinTemp();
}

inline double Landscape::SupplyMaxTemp(void)
{
    return g_weather->GetMaxTemp();
}

inline double Landscape::SupplyMinTempYesterday(void)
{
    return g_weather->GetMinTempYesterday();
}

inline double Landscape::SupplyMaxTempYesterday(void)
{
    return g_weather->GetMaxTempYesterday();
}

inline double Landscape::SupplyMinTempTomorrow(void)
{
    return g_weather->GetMinTempTomorrow();
}

inline double Landscape::SupplyMaxTempTomorrow(void)
{
    return g_weather->GetMaxTempTomorrow();
}

inline double Landscape::SupplySoilTemp(void)
{
    return g_weather->GetSoilTemp();
}

inline double Landscape::SupplySoilTempHour(int hour)
{
	return g_weather->GetSoilTempHour(hour);
}


inline double Landscape::SupplyHumidity(void)
{
	return g_weather->GetHumidity();
}

inline double Landscape::SupplyWind( long a_date )
{
  return g_weather->GetWind( a_date );
}

inline double Landscape::SupplyWindPeriod( long a_date, int a_period )
{
  return g_weather->GetWindPeriod( a_date, a_period );
}

inline double Landscape::SupplyTempPeriod( long a_date, int a_period )
{
  return g_weather->GetTempPeriod( a_date, a_period );
}



inline double Landscape::SupplyWind( void )
{
  return g_weather->GetWind();
}


inline int Landscape::SupplyWindDirection( void )
{
	return g_weather->GetWindDirection();
}

inline int Landscape::SupplyWindDirection8( void )
{
	return g_weather->GetWindDirection8();
}


inline double Landscape::SupplyWindDirectionRadians( void)
{
	return g_weather->GetWindDirectionRadians();
}


inline bool  Landscape::SupplySnowcover( long a_date )
{
  return g_weather->GetSnow( a_date );
}



inline bool  Landscape::SupplySnowcover( void )
{
  return g_weather->GetSnow();
}



inline double  Landscape::SupplySnowDepth( void )
{
  return g_weather->GetSnowDepth();
}

inline double Landscape::SupplyTempHour(int hour)
{
  return g_weather->GetTempHour(hour);
}

inline double Landscape::SupplyWindHour(int hour)
{
  return g_weather->GetWindHour(hour);
}
inline double Landscape::SupplyRainHour(int hour)
{
  return g_weather->GetRainHour(hour);
}
inline double Landscape::SupplyRadiationHour(int hour)
{
  return g_weather->GetRadiationHour(hour);
}

inline int Landscape::SupplyPolyRefVector( unsigned int a_index )
{
  return m_elems[ a_index ]->GetPoly();
}

inline unsigned int
  Landscape::SupplyNumberOfPolygons( void )
{
  return (unsigned int) m_elems.size();
}

inline int Landscape::SupplyPesticideCell(int a_polyref)
{
  return m_elems[ m_polymapping[ a_polyref ] ]->GetPesticideCell();
}

inline int Landscape::SupplyValidX(int a_polyref)
{
  return m_elems[ m_polymapping[ a_polyref ] ]->GetValidX();
}

inline int Landscape::SupplyValidY(int a_polyref)
{
  return m_elems[ m_polymapping[ a_polyref ] ]->GetValidY();
}

inline LE* Landscape::SupplyPolyLEptr(int a_polyref)
{
	return m_elems[m_polymapping[a_polyref]];
}

inline LE* Landscape::SupplyPolyLEptr(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)];
}

inline int   Landscape::SupplyPolyRef(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetPoly();
}

inline TTypesOfLandscapeElement   Landscape::SupplyOwner_tole(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetOwner_tole();
}

inline int   Landscape::SupplyUMRef(int a_x, int a_y)
{
	return m_elems[m_land->Get(a_x, a_y)]->GetUnsprayedMarginPolyRef();
}

inline int   Landscape::SupplyPolyRefIndex( int a_x, int a_y )
{
  return m_land->Get( a_x, a_y );
}

inline int   Landscape::SupplyPolyRefCC( int a_x, int a_y )
{
  a_x = (a_x + m_width10) % m_width;
  a_y = (a_y + m_height10) % m_height;
  return m_elems[ m_land->Get( a_x, a_y )]->GetPoly();
}



inline int*  Landscape::SupplyMagicMapP( int a_x, int a_y )
{
  return m_land->GetMagicP( a_x, a_y );
}

inline int   Landscape::MagicMapP2PolyRef( int a_magic )
{
  return m_elems[ a_magic ]->GetPoly();
}

inline int   Landscape::SupplyDaylength( long a_date )
{
  return g_date->DayLength( a_date );
}

inline int   Landscape::SupplyDaylength( void )
{
  return g_date->DayLength();
}

inline void Landscape::CorrectCoords(int &x, int &y)
{
	/**
	* m_width10 & m_height10 are used to avoid problems with co-ordinate values that are very large. Problems will only occur if coords passed are >10x the world width or height.
	*/
	x = (m_width10 + x) % m_width;
	y = (m_height10 + y) % m_height;
}

inline APoint Landscape::CorrectCoordsPt(int x, int y)
{
	/**
	* m_width10 & m_height10 are used to avoid problems with co-ordinate values that are very large. Problems will only occur if coords passed are >10x the world width or height.
	*/
	APoint pt;
	pt.m_x = (m_width10 + x) % m_width;
	pt.m_y = (m_height10 + y) % m_height;
	return pt;
}

inline void Landscape::CorrectCoordsPointNoWrap(APoint & a_pt)
{
	/**
	* This just cuts off extremes of coordinate values so that the point stays in landscape. Can't use a modulus or we get wrap around, and in this case we don't want that
	*/
	if (a_pt.m_x >= m_width) a_pt.m_x = m_width - 1;
	if (a_pt.m_y >= m_height) a_pt.m_y = m_height - 1;
	if (a_pt.m_x < 0) a_pt.m_x = 0;
	if (a_pt.m_y < 0) a_pt.m_y = 0;
}


inline int Landscape::CorrectWidth( int x )
{
   return (m_width10+x)%m_width;
}


inline int Landscape::CorrectHeight( int y )
{
  return (m_height10+y)%m_height;
}


inline void  Landscape::Warn( std::string a_msg1,  std::string a_msg2 )
{
  g_msg->Warn( WARN_MSG, a_msg1, a_msg2 );
}
/** 
\brief Get the hour of the day
*/
inline int Landscape::SupplyHour( void ) {
	return g_date->GetHour();
}
/**
\brief Get the minute of the hour
*/
inline int Landscape::SupplyMinute( void ) {
	return g_date->GetMinute();
}

inline int  Landscape::SupplyDayInYear( void )
{
  return g_date->DayInYear();
}

inline int Landscape::SupplyMonth( void )
{
  return g_date->GetMonth();
}

inline int Landscape::SupplyDayInMonth( void )
{
  return g_date->GetDayInMonth();
}

inline int Landscape::SupplyYear( void )
{
	return g_date->GetYear();
}

inline int Landscape::SupplyYearNumber( void )
{
	return g_date->GetYearNumber();
}

inline long Landscape::SupplyGlobalDate( void )
{
	return g_date->Date();
}

inline int   Landscape::SupplySimAreaWidth( void )
{
  return m_width;
}

inline int   Landscape::SupplySimAreaHeight( void )
{
  return m_height;
}

inline int   Landscape::SupplySimAreaMaxExtent( void )
{
  return m_maxextent;
}

inline TTypesOfLandscapeElement
  Landscape::TranslateEleTypes(int EleReference)
{
  return g_letype->TranslateEleTypes( EleReference );
}

inline TTypesOfLandscapeElement
  Landscape::TranslateEleTypesFromString(string EleReference)
{
  return g_letype->TranslateEleTypesFromString( EleReference );
}


inline TTypesOfVegetation
  Landscape::TranslateVegTypes(int VegReference)
{
  return g_letype->TranslateVegTypes( VegReference );
}

inline int
  Landscape::BackTranslateEleTypes(TTypesOfLandscapeElement EleReference)
{
  return g_letype->BackTranslateEleTypes( EleReference );
}

inline int
  Landscape::BackTranslateVegTypes(TTypesOfVegetation VegReference)
{
  return g_letype->BackTranslateVegTypes( VegReference );
}
#endif // LANDSCAPE_H

