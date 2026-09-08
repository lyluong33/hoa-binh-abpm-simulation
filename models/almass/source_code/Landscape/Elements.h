//
// Elements.h
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

#ifndef ELEMENTS_H
#define ELEMENTS_H

#define FMDEBUG
#include <fstream>

class APoint;

typedef unsigned int LE_Signal;

static CfgBool l_el_read_bug_percentage_file("ELEM_READ_BUG_PERCENTAGE_FILE", CFG_CUSTOM, false);
static CfgStr l_el_bug_percentage_file("ELEM_BUG_PERCENTAGE_FILE", CFG_CUSTOM,"bugpercents.txt");

#define LE_SIG_NO_FUNGICIDE   0x0001
#define LE_SIG_NO_INSECTICIDE 0x0002
#define LE_SIG_NO_SYNG_INSECT 0x0004
#define LE_SIG_NO_GROWTH_REG  0x0008
#define LE_SIG_NO_MOLLUSC     0x0010
#define LE_SIG_NO_HERBICIDE   0x0020

// Number of user defined storage locations for farm management
// states. For ridiculously active farm management plans this
// might have to be increased. Ie. bump this number if there is
// a chance of more than 10 farm events for an given polygon on
// any given crop plan.
#define EL_MAX_USERSPACE 10 // Has to be a #define.

// Decay time for treatment generated tramlines (in days).
#define EL_TRAMLINE_DECAYTIME (g_el_tramline_decaytime_days.value())
extern CfgInt g_el_tramline_decaytime_days;
// Delay time before regrowth of weeds after herbicide treatment.
#define EL_HERBICIDE_DELAYTIME (g_el_herbicide_delaytime_days.value())
#define EL_STRIGLING_DELAYTIME (g_el_strigling_delaytime_days.value())
extern CfgInt g_el_herbicide_delaytime_days;
extern CfgInt g_el_strigling_delaytime_days;
extern CfgInt cfg_farm_cattle_grazing_lvl_rot;
extern CfgInt cfg_farm_cattle_grazing_lvl_pasture;
extern CfgInt cfg_farm_cattle_grazing_lvl_lowyield;
extern CfgInt cfg_farm_cattle_grazing_lvl_tussocky;
// Will be declared in other .h files, but these are needed here to break circular class definitions.
class Farm;
extern class LE_TypeClass *g_letype;
enum TTypesOfPopulation : int;

class LE_TypeClass
{
 public:
  TTypesOfLandscapeElement TranslateEleTypes(int EleReference);
  TTypesOfVegetation       TranslateVegTypes(int VegReference);

  int BackTranslateEleTypes(TTypesOfLandscapeElement EleReference);
  int BackTranslateVegTypes(TTypesOfVegetation VegReference);
  int VegTypeToCurveNum(    TTypesOfVegetation VegReference);
  TTypesOfLandscapeElement TranslateEleTypesFromString(string aEleReferenceStr);
};

LE_TypeClass* CreateLETypeClass();

class LE {
public:
  LE(Landscape * L);
  virtual ~LE( void );
  virtual int GetAphidDensity() { return false; }
  int m_tried_to_do;
  int m_squares_in_map;
  /** \brief For management testing of skylark scrapes*/
  bool m_skylarkscrapes;
  ///** \brief Variable used to record the current grazing pressure by e.g. voles */
  //double m_volegrazing;
  // /** \brief Variable used to reduce the grazing pressure */
  //double m_voleResregrowth;
  ///** \brief Variable used to record the current grazing pressure by e.g. voles */
  //double m_volegrazingDensity;
  ///** \brief Change the current grazing pressure by e.g. voles */
  //void AddVoleGrazing(int a_number) { m_volegrazing+=a_number; }
  ///** \brief Get the current total grazing pressure by e.g. voles */
  //double GetVoleGrazing( void ) {return m_volegrazing; }
  ///** \brief Calculate the current grazing pressure by e.g. voles */
  //void CalcVoleGrazingDensity( void ) { m_volegrazing *= m_voleResregrowth; m_volegrazingDensity = (m_volegrazing/m_area); } 
  ///** \brief Get the current grazing pressure by e.g. voles */
  //double GetVoleGrazingDensity( void ) {return m_volegrazingDensity; }
  ///** \brief Reset the current grazing pressure by e.g. voles */
  ////void ResetVoleGrazing( void ) { m_volegrazing = 0.0; }
  long  m_user[ EL_MAX_USERSPACE ];
  bool  m_repeat_start;
  Landscape* GetLandscape(void) { return m_Landscape; }
  LE_Signal GetSignal( void ) { return m_signal_mask; }
  void      SetSignal( LE_Signal a_signal ) { m_signal_mask = a_signal; }
  int  GetSubType( void ) { return m_subtype; }
  void SetSubType( int a_subtype ) { m_subtype = a_subtype; }
  int  GetALMaSSEleType(void) { return m_almass_le_type; }
  void SetALMaSSEleType(int a_type) { m_almass_le_type = a_type; }
  virtual TTypesOfLandscapeElement GetElementType(void) { return m_type; }
  virtual TTypesOfLandscapeElement GetOwner_tole(void) { return m_owner_tole; }
  void SetOwner_tole(TTypesOfLandscapeElement a_tole) { m_owner_tole = a_tole; }
  void   SetElementType( int a_type ) { m_type = (TTypesOfLandscapeElement) a_type; }
  void   Trace( int a_value );
  void   ResetTrace( void );
  long   GetRunNum( void ) { return m_running; }
  void   BumpRunNum( void ) { m_running++; }
  long   GetMgtLoopDetectDate ( void ) { return m_management_loop_detect_date; }
  void   SetMgtLoopDetectDate ( long a_num ) { m_management_loop_detect_date = a_num; }
  long   GetMgtLoopDetectCount ( void ) { return m_management_loop_detect_count; }
  void   SetMgtLoopDetectCount ( long a_num ) { m_management_loop_detect_count = a_num; }
  virtual void Tick( void );
  virtual TTypesOfVegetation GetVegType(void) { return tov_None; }
  virtual TTypesOfCrops GetCropType(void) { return toc_Foobar; }
  virtual void SetCropType(TTypesOfCrops /* a_crop */) { ; }
  int    GetVegStore( void ) { return m_vege_danger_store; }
  void   SetVegStore( int a_veg )
    { m_vege_danger_store = a_veg; }
  void ZeroVegAge() { m_vegage=0; }
  int GetVegAge() { return m_vegage; }
  virtual void  DoDevelopment( void );
  virtual void  RecalculateBugsNStuff( void ) { }
  virtual int GetVegPhase(void) { return vegphase_foobar; }
  virtual double GetVegGrowthStage(void) {return 100;}
  virtual double GetVegCover(void) { return 0.0; }
  virtual double GetVegHeight( void ) { return 0.0; }
  virtual APoint GetCentroid();
  virtual int GetCentroidX() { return m_centroidx; }
  virtual int GetCentroidY() { return m_centroidy; }
  virtual void SetCentroid(int x, int y) { m_centroidx=x; m_centroidy=y; }
  virtual double GetDigestibility( void ) { return 0.0; }
  virtual int GetVegDensity( void ) { return 0; }
  virtual bool GetSkScrapes( void ) { return false; }
  virtual void SetVegGrowthScalerRand() { ; }
  virtual double GetDayDegrees( void ) { return m_ddegs; }
  virtual double GetLAGreen( void ) { return 0.0; }
  virtual double GetLATotal( void ) { return 0.0; }
  virtual double GetGreenBiomass( void ) { return 0.0; }
  virtual double GetInterestedGreenBiomass ( void ) { return 0.0; }
  virtual double GetGreenBiomassProp (void) {return 0.0;}
  virtual double GetDeadBiomass( void ) { return 0.0; }
  virtual double GetVegBiomass( void ) { return 0.0; }
  virtual double GetWeedBiomass( void ) { return 0.0; }
  virtual double GetInsectPop( void ) { return 0.0; }
  virtual void  SetInsectPop( double  /* i */ ) { ; }
  virtual void  Insecticide( double /* a_fraction */ ) { ; }
  virtual void  InsectMortality( double /* a_fraction */ ) { ; }
  virtual void  ReduceWeedBiomass( double /* a_fraction */ ) { ; }
  virtual void  ToggleCattleGrazing( void ) {;}
  virtual void  TogglePigGrazing( void ) {;}
  virtual void  ToggleIrrigation(void) { ; }
  virtual void  SetVegBiomass( int /* a_biomass */ ) {;}
  virtual void  SetVegType(TTypesOfVegetation /* a_type */) { ; }
  virtual void  SetVegType( TTypesOfVegetation /* a_type */, TTypesOfVegetation /* a_weed_type */  ) {;}
  virtual void  SetVegHeight(double /* a_height */) { ; }
  virtual void  SetDigestibility(double /* a_digestability */) { ; }

  virtual void  SetGrazingLevel(int /*a_grazing */) { ; }
  virtual void  SetVegParameters(double /* a_height */, double /* a_LAtotal */, double /* a_LAgreen */, double /* a_WeedBiomass */) { ; }
  virtual void SetCropData(double, double, double, TTypesOfVegetation, double, int) { ; }
  virtual void SetCropDataAll(double,double,double, double,TTypesOfVegetation, double, double , int, double , bool, double, double, double) {;}
  virtual void  StoreLAItotal() {;}
  virtual void  SetGrowthPhase( int /* a_phase */ ) {;}
  virtual void  ForceGrowthInitialize( void ) {;}
  virtual void  ForceGrowthTest( void ) {;}
  virtual void  ForceGrowthDevelopment( void ) {;}
  virtual void  ZeroVeg( void ) {;}
  virtual void  ReduceVeg( double /* a_reduc */ ) {;}
  virtual void  ReduceVeg_Extended(double /* a_reduc */) { ; }
  virtual void  GrazeVeg_Extended(double /* a_reduc */) { ; }
  virtual double GetTrafficLoad(void) { return 0.0; }
  virtual TTypesOfVegetation GetPreviousTov(int /* a_index */) { return tov_None;  }
  bool  GetPoison( void ) { return m_poison; }
  void  SetPoison( bool a_poison ) { m_poison = a_poison; }
  // -- ATTRIBUTES 
  bool  Is_Att_High( void ) { return m_att_high; }
  void  Set_Att_High(bool a_high) { m_att_high = a_high; }
  bool  Is_Att_Water(void) { return m_att_water; }
  void  Set_Att_Water(bool a_water) { m_att_water = a_water; }
  bool  Is_Att_Forest(void) { return m_att_forest; }
  void  Set_Att_Forest(bool a_forest) { m_att_forest = a_forest; }
  bool  Is_Att_Woody(void) { return m_att_woody; }
  void  Set_Att_Woody(bool a_woody) { m_att_woody = a_woody; }
  bool  Is_Att_UrbanNoVeg(void) { return m_att_urbannoveg; }
  void  Set_Att_UrbanNoVeg(bool a_unv) { m_att_urbannoveg = a_unv; }
  bool  Is_Att_UserDefinedBool(void) { return m_att_userdefinedbool; }
  void  Set_Att_UserDefinedBool(bool a_value) { m_att_userdefinedbool = a_value; }
  int   Is_Att_UserDefinedInt(void) { return m_att_userdefinedint; }
  void  Set_Att_UserDefinedInt(int a_value) { m_att_userdefinedint = a_value; }
  // -- VEG ATTRIBUTES 
  virtual bool Is_Att_Veg(void) { return false; }
  virtual bool Is_Att_VegPatchy(void) { return false; }
  virtual void Set_Att_VegPatchy(bool /* p */) { ; }
  virtual bool Is_Att_VegCereal() { return false; }
  virtual void Set_Att_VegCereal(bool /* p */) { ; }
  virtual bool Is_Att_VegGrass() { return false; }
  virtual void Set_Att_VegGrass(bool /* p */) { ; }
  virtual bool Is_Att_VegMatureCereal() { return false; }
  virtual void Set_Att_VegMatureCereal(bool /* p */) { ; }
  virtual bool Is_Att_VegMaize() { return false; }
  virtual void Set_Att_VegMaize(bool /* p */) { ; }
  virtual bool Is_Att_VegGooseGrass() { return false; }
  virtual void Set_Att_VegGooseGrass(bool /* p */) { ; }
  
  // for compatbility - to reduce out 
  inline void SetVegPatchy(bool p) { Set_Att_VegPatchy(p); }

  // -- END ATTRIBUTES
  int  GetCattleGrazing( void ) { return m_cattle_grazing; }
  bool  GetPigGrazing( void ) { return m_pig_grazing; }
  bool  HasTramlines( void ) { return ( m_tramlinesdecay>0 ); }
  bool  IsRecentlyMown( void ) { return ( m_mowndecay>0 ); }
  int   IsRecentlySprayed( void ) { return ( m_herbicidedelay ); }
  /** \brief Returns the polyref number for this polygon */
  int   GetPoly( void ) { return m_poly; }
  int   GetMapIndex( void ) { return m_map_index; }
  void  SetMapIndex( int a_map_index ) { m_map_index = a_map_index; }
  int   GetOwnerFile( void ) { return m_owner_file; }
  int   GetOwnerIndex( void ) { return m_owner_index; }
  int   GetLastTreatment(int* a_index);
  /** \brief Returns the last treatment recorded for the polygon */
  int   GetLastTreatment();
  /** \brief clears the management action counters */
  void ClearManagementActionSum() {
	  m_ManagementActionCounts.resize(fmc_Foobar+1);
	  for (int i = 0; i < (int)fmc_Foobar; i++) m_ManagementActionCounts[i] = 0;
  }
  /** \brief Record management action throught a crop management action */
  void AddManagementActionDone(FarmManagementCategory a_action) { m_ManagementActionCounts[int(a_action)]++; }
  /** Returns the a ptr to the current management actions total */
  vector<int> GetManagementTotals() { return m_ManagementActionCounts; }

  Farm* GetOwner( void ) { return m_owner; }
  double GetArea( void ) { return m_area; }
  /** \brief Sets the correct species simulation behaviour for the current species - implemented in VegElement */
  virtual void SetSpeciesFunction(TTypesOfPopulation a_species) { ; }
  double GetGooseGrazingForage(GooseSpecies a_goose) { return m_goosegrazingforage[a_goose]; }
  double GetBirdSeed( void ) {
	  return m_birdseedforage;
  }
  void SetBirdSeed( double a_forage ) {
	  m_birdseedforage = a_forage;
  }
  double GetBirdMaize(void) {
	  return m_birdmaizeforage;
  }
 /*
 double GetIsCereal(void) {
	  if ( m_goosegrazingforage[gs_foobar] == 1) return true;
	  return false;
  }
*/
  void SetBirdMaize( double a_forage ) {
	  m_birdmaizeforage = a_forage;
  }
  int GetCountryDesignation( void ) { return m_countrydesignation; }
  void SetCountryDesignation( int a_designation ) { m_countrydesignation = a_designation; }
  long  GetOldDays( void ) { return m_olddays; }
  void  SetOwner( Farm *a_owner, int a_owner_num, int a_owner_index )
  {
    m_owner = a_owner;
    m_owner_file = a_owner_num;
    m_owner_index = a_owner_index;
  }
  void  SetPoly( int a_poly ) { m_poly = a_poly; }
  void  SetMapValid( bool a_valid ) { m_is_in_map = a_valid; }
  bool  GetMapValid( void ) { return m_is_in_map; }
  void  SetArea( double a_area ) { m_area = a_area; }
  /** /brief Reduces the total vegetation biomass as a result of grazing in descendent classes, per m2 mass */
  virtual void GrazeVegetation( double /* m_grams */, bool )  { ; }
  /** /brief Reduces the vegetation by length of grass (in cm) */
  virtual void  GrazeVegetationHeight(double a_reduc) { ; }
  /** /brief Reduces the total vegetation biomass as a result of grazing in descendent classes, total polygon mass */
  virtual void GrazeVegetationTotal( double /* m_grams */ )  
  {
	  ;
  }
  /** /brief Gets the soil type ref number for the polygon */
  int   GetSoilType() {
	  /**
	  EZ: NEW SOIL CLASSIFICATION (FAO 2006)
	  Agricultural Usage	AlmCode SoilType	FAO texture code	Description				Crop management code
		None				0					-					Water					-
		None				1					S					Sand (unspecified)		Light (sandy type)
		Poor				2					LS					Loamy sand				Light (sandy type)
		Poor				3					SL					Sandy loam				Light (sandy type)
		Poor				4					SCL					Sandy clay loam			Light (sandy type)
		Average				5					SiL					Silt loam				Average (silty loam type)
		Average				6					SiCL				Silty clay loam			Average (silty loam type)
		Good				7					CL					Clay loam				Heavy (clay & loam type)
		Good				8					L					Loam					Heavy (clay & loam type)
		Good				9					Si					Silt					Average (silty loam type)
		Good				10					SC					Sandy clay				Heavy (clay & loam type)
		Good				11					SiC					Silty clay				Heavy (clay & loam type)
		Good				12					C					Clay					Heavy (clay & loam type)
		Good				13					HC					Heavy clay				Heavy (clay & loam type)
		Good				14					O					Organic & peat			Organic & peat

	  */
	  return m_soiltype;
  }
  /** /brief Gets the soil type ref number for the polygon for rabbit warrens*/
  int   GetSoilTypeR() {
	  /**
	  Those types that are Sandy return 1, otherwise suitable for burrows 0, and 3 if unsuitable. If not soil types are specified then heavy soils suitable for long-term burrows are returned as default.
	  */
	  switch (m_soiltype)
	  {
		  case 0: return 3;
		  case 1: return 1;
		  case 2: return 1;
		  case 3: return 1;
		  case 4: return 1;
		  case 5: return 0;
		  case 6: return 0;
		  case 7: return 0;
		  case 8: return 0;
		  case 9: return 0;
		  case 10: return 0;
		  case 11: return 0;
		  case 12: return 0;
		  case 13: return 0;
		  case 14: return 0;
		  default:
			  return 3;
	  }
  }
  void  SetSoilType( int a_st ) {
	  m_soiltype = a_st;
  }
  /**
  * \brief Records the last vegetation type to be sown
  */
  void SetLastSownVeg(TTypesOfVegetation a_tov) {
	  m_lastsownvegtype = a_tov;
  }
  /**
  * \brief Returns the last vegetation type to be sown
  */
  TTypesOfVegetation GetLastSownVeg() {
	  return m_lastsownvegtype;
  }
  void  SetLastTreatment(int a_treatment);
  void SetSprayedToday(bool a_didit) { m_sprayedtoday = a_didit; }
  bool GetSprayedToday() { return m_sprayedtoday; }
  void  SetCopyTreatment(int a_treatment);
  void  SetOldDays( long a_days ) { m_olddays = a_days; }
  void  SetRotIndex( int a_index ) { m_rot_index = a_index; }

  int   GetRotIndex( void ) { return m_rot_index; }
  void  SetTramlinesDecay( int a_decaytime_days )
    { m_tramlinesdecay = a_decaytime_days; }
  void  SetHerbicideDelay( int a_decaytime_days )
    { m_herbicidedelay = a_decaytime_days; }
  void  SetMownDecay( int a_decaytime_days )
    { m_mowndecay = a_decaytime_days; }
  void  AddArea( double a_area_diff ) { m_area += a_area_diff; }
  LE*	GetBorder( void ) { return m_border; }
//  LE*	GetUnsprayedMargin( void ) { return m_unsprayedmargin; }
  int	GetUnsprayedMarginPolyRef( void ) { return m_unsprayedmarginpolyref; }
  void  SetBorder( LE* a_border ) { m_border = a_border; }
//  void  SetUnsprayedMargin( LE* a_unsprayedmargin ) { m_unsprayedmargin = a_unsprayedmargin; }
  void  SetUnsprayedMarginPolyRef( int a_unsprayedmargin ) { m_unsprayedmarginpolyref = a_unsprayedmargin; }
  void  SetPesticideCell( int a_cell ) { m_PesticideGridCell = a_cell; }
  int   GetPesticideCell( ) { return m_PesticideGridCell; }
  void  SetValidXY( int a_valid_x, int a_valid_y )
    {
      m_valid_x = a_valid_x;
      m_valid_y = a_valid_y;
    }
  int	GetValidX( void ) { return m_valid_x; }
  int	GetValidY( void ) { return m_valid_y; }

  int	GetMaxX( void ) { return m_maxx; }
  int	GetMaxY( void ) { return m_maxy; }
  int	GetMinX( void ) { return m_minx; }
  int	GetMinY( void ) { return m_miny; }
  void	SetMaxX( int x ) { m_maxx=x; }
  void	SetMaxY( int y ) { m_maxy=y; }
  void	SetMinX( int x ) { m_minx=x; }
  void	SetMinY( int y ) { m_miny=y; }
  int   GetMDates(int a, int b) {return MDates[a][b];}
  void  SetMDates(int a, int b, int c) {MDates[a][b]=c;}
  int   GetMConstants(int a) {return MConsts[a];}
  void  SetMConstants(int a, int c) {MConsts[a]=c;}
  /** \brief Get the total of day degrees for the crop/veg on this element */
  double GetVegDDegs() {
	  return m_vegddegs; // 
  }
  /** \brief For goose model functionality, openness score for the polygon*/
  int GetOpenness( void ) { return m_openness; }
  /** \brief Get elevation data */
  double GetElevation(void) { return m_elevation; }
  double GetSlope(void) { return m_slope; }
  double GetAspect(void) { return m_aspect; }
  /** For goose model functionality, records the number of geese on a_day day */
  void SetGooseNos(int a_number, int a_day) { m_gooseNos[a_day] = a_number; }
  /** For goose model functionality, records the number of geese on a_day day */
  void SetGooseSpNos(int a_number, int a_day, GooseSpecies a_goose) {
	  m_gooseSpNos[a_day][a_goose] = a_number;
  }
  /** For goose model functionality, records the number of geese on a_day day */
  void SetGooseNosTimed(int a_number, int a_day) { m_gooseNosTimed[a_day] = a_number; }
  /** For goose model functionality, records the number of geese on a_day day */
  void SetGooseSpNosTimed(int a_number, int a_day, GooseSpecies a_goose) {
	  m_gooseSpNosTimed[a_day][a_goose] = a_number;
  }
  /** For goose model functionality, records the distance to the closest roost.*/
  void SetGooseRoostDist(int a_dist, GooseSpecies a_goose) { m_dist_to_closest_roost[a_goose] = a_dist; }
  /** \brief For goose model functionality, returns the number of geese today */
  int GetGooseNosToday();
  /** \brief For goose model functionality, returns the number of geese today at a predefined time */
  int GetGooseNosTodayTimed();
  /** \brief For goose model functionality, returns the number of geese yesterday */
  int GetGooseNos( );
  /** \brief For goose model functionality, returns the number of geese yesterday which can be hunted as legal quarry */
  int GetQuarryNos();
  /** \brief For goose model functionality, returns the number of geese per species yesterday */
  int GetGooseSpNosToday(GooseSpecies a_goose);
  /** \brief For goose model functionality, returns the number of geese per species yesterday at a predefined time */
  int GetGooseSpNosTodayTimed(GooseSpecies a_goose);
  /** \brief For goose model functionality, returns the distance to closest roost */
  int GetGooseRoostDist(GooseSpecies a_goose);
  /** \brief Records the openness statistic for this polygon */
  void SetOpenness( int a_openness ) { m_openness=a_openness; }
  /** \brief Records the elevation of the polygon */
  void SetElevation(double a_elevation) { m_elevation = a_elevation; }
  void SetSlope(double a_slope) { m_slope = a_slope; }
  void SetAspect(double a_aspect) { m_aspect = a_aspect; }
  /** \brief sets growth record to zero in descendent classes */
  virtual void ResetDigestibility() { ; }
  /** \brief a copy function to be used because a copy constuctor won't work */
  void DoCopy(const LE* a_Le);
  /** \brief Sets a male newt as present/absent in descendent classes - here only to prevent need for dynamic casts */
  virtual void SetMaleNewtPresent(bool) { ; };
  /** \brief Sets a male newt as present/absent in descendent classes - here only to prevent need for dynamic casts */
  virtual bool IsMaleNewtPresent() { return false; };
  /** \brief Sets the in stubble flag */
  void SetStubble(bool a_flag) { m_instubble = a_flag; }
  /** \brief Return the in stubble flag */
  bool GetStubble() { return m_instubble; }

  protected:
  Landscape * m_Landscape;
  vector<int> m_ManagementActionCounts;

#ifdef FMDEBUG
  int   m_ptrace[256]; // Change bitmap in 'elements.cpp' if this
  int   m_pdates[256]; // needs to be increased!
  int   m_pindex;      // Points to the next *empty* location, so last
                       // program step is in the location below this...
#endif
  long  m_running;
  LE	*m_border;
  int    m_unsprayedmarginpolyref;
  // May be -1 if no unsprayed margin or this element is a margin!
  // Valid map coordinate inside this landscape element
  // before(!) adding the border.
  int   m_valid_x;
  int   m_valid_y;
  int m_centroidx;
  int m_centroidy;
  int   m_vege_danger_store;
  int   m_PesticideGridCell;
  // Whether this polygon is actually mentioned in the main map.
  bool  m_is_in_map;
  // Quick hack to discern different hedgebank types.
  int   m_subtype;
  int m_maxx;
  int m_minx;
  int m_maxy;
  int m_miny;
  long  m_olddays;
  long  m_management_loop_detect_date;
  long  m_management_loop_detect_count;
  Farm *m_owner {nullptr};
  int m_countrydesignation;
  int   m_owner_file;
  int   m_owner_index;
  int   m_rot_index;
  /** \brief flag to indicate an overspray situation */
  bool  m_sprayedtoday;
  /** \brief The polyref number for this polygon */
  int   m_poly;
  // Copy of the index stored in the main map at run time.
  // Needed by the pesticide machinery.
  int   m_map_index;
  /** \brief This holds the ALMaSS element type reference number */
  int   m_almass_le_type;
  int   m_farmfunc_tried_to_do;
  int   m_tramlinesdecay;
  int   m_mowndecay;
  int   m_herbicidedelay;
  TTypesOfLandscapeElement m_type;
  TTypesOfLandscapeElement    m_owner_tole;
  /** \brief The element area in m2 */
  double m_area;
  /** \brief The number of geese each day */
  int m_gooseNos[366];
  /** \brief The number of geese of each species each day */
  int m_gooseSpNos[366][gs_foobar];
  /** \brief The number of geese at a predefined time per day */
  int m_gooseNosTimed[366];
  /** \brief The number of geese of each species at a predefined time per day */
  int m_gooseSpNosTimed[366][gs_foobar];
  /** \brief A flag describing the state of the field from a visual perspective - will be in stubble following a cereal crop until either soil cultivation or March 1st */
  bool m_instubble;
  /** \brief The grain forage present in seeds/m2 */
  double m_birdseedforage;
  /** \brief The maize forage present in seeds/m2 */
  double m_birdmaizeforage;
  /** \brief The grazing forage present in KJ/min. The last value indicates cereal 1 or not -1 */
  double m_goosegrazingforage[ gs_foobar ];
  /** \brief Records the last vegegetation type that was sown on the element */
  TTypesOfVegetation m_lastsownvegtype;
  /** \brief The openness metric for a field (if any) */
  int m_openness;
  /** \brief Elevation data */
  double m_elevation;
  double m_slope;
  double m_aspect;
  /** \brief An array holding the distance to the nearest goose roost */
  double m_dist_to_closest_roost[gs_foobar];
  int m_vegage;
  int m_soiltype;
  int m_days_since_insecticide_spray;
  // -- ATTRIBUTES
  bool  m_att_high;
  bool  m_att_water;
  bool  m_att_forest;
  bool  m_att_woody;
  bool  m_att_urbannoveg;
  /** \brief a userdefined attribute which can be set by a species model without changing other standard values */
  bool  m_att_userdefinedbool;
  /** \brief a userdefined attribute which can be set by a species model without changing other standard values */
  int   m_att_userdefinedint;
  // -- END ATTRIBUTES
  bool  m_poison;
  int  m_cattle_grazing;
  int m_default_grazing_level;
  bool  m_pig_grazing;
  double m_vegddegs; // Total of day degrees for the crop/veg on this element.
  double m_yddegs; // Yesterdays total of day degrees for this element.
  double m_weedddegs;  // Total of day degrees for this element when today
  double m_ddegs;  // Total of day degrees for this element when today
  // is taken into consideration.
  vector <int> m_lasttreat;
  unsigned int m_lastindex;
  // Only one copy of these is stored for use by all landscape elements.
  static double m_monthly_traffic[];
  static double m_largeroad_load[];
  static double m_smallroad_load[];
  int MDates[2][25];
  int MConsts[10];
  LE_Signal m_signal_mask;
  /** \brief to record the chance of osmia nesting */
  double m_OsmiaNestProb;
  /** \brief to record the number of possible osmia nests */
  int m_maxOsmiaNests;
 /** \brief to record the number of actual osmia nests */
  int m_currentOsmiaNests;
};

//---------------------------------------------------------------------------

/** \brief Struct for storing actual data on crop type, area of a field it is grown in, biomass at harvest, numbers of 
pesticide applications and missed pesticide applications.*/ 
struct CropActualValues {
	bool taken;
	TTypesOfVegetation tov_type;
	double biomass_at_harvest;
	bool harvested;
	double area;
	int no_herb_app; //number of herbicide applications
	int missed_herb_app;
	int no_fi_app;
	int missed_fi_app;
};


class VegElement : public LE {
protected:
	TTypesOfCrops m_CropType;
	/** The parameters for calculation of insect biomass*/
	static double m_insect_biomass_parameters_a[9];
	static double m_insect_biomass_parameters_b[9];
	static double m_insect_biomass_parameters_c[9];
	int m_insect_biomass_parameters_index{ 0 }; //default to 0
	static double m_SeasonalInsectScaler[12];
	static double m_biomass_scale[tov_Undefined];
  /** \brief Fraction of the the interested biomass. */
  double m_interested_biomass_fraction;
public:
	VegElement(TTypesOfLandscapeElement tole, Landscape* L);
	virtual ~VegElement(void) { ; };
	virtual void  DoDevelopment(void);
	virtual double GetDigestibility(void) { return m_digestability; }
	virtual double GetLAGreen(void) { return m_LAgreen; }
	virtual double GetLATotal(void) { return m_LAtotal; }
	virtual double GetVegCover(void) { return m_veg_cover; }
	virtual double GetVegHeight(void) { return m_veg_height; }
	virtual double GetVegBiomass(void) { return m_veg_biomass; }
	virtual double GetDeadBiomass(void) { return m_dead_biomass; }
	virtual double GetGreenBiomass(void) { return m_green_biomass; }
  virtual double GetInterestedGreenBiomass(void) { return m_interested_green_biomass; }
	virtual double GetGreenBiomassProp(void) { return m_greenbiomass_per; }
	virtual double GetWeedBiomass(void) { return m_weed_biomass; }
	virtual int GetVegDensity(void) { return m_veg_density; }
	virtual bool GetSkScrapes(void) { return m_skylarkscrapes; }
	static void SetBiomassScalers(TTypesOfVegetation a_tov);
	void Tick(void) override;
	virtual void SetVegGrowthScalerRand();
	virtual double GetInsectPop(void) { return m_insect_pop; }
	virtual void SetInsectPop(double insects) { m_insect_pop = insects; }
	virtual TTypesOfVegetation GetVegType(void) { return m_vege_type; }
	virtual TTypesOfCrops GetCropType() { return m_CropType; }
	virtual void SetCropType(TTypesOfCrops a_type) { 
    m_CropType = a_type; 
    SetInsectBiomassParametersIndex(a_type); 
    switch(a_type){
      case toc_GrassGrazed1:
	    case toc_GrassGrazed2:
	    case toc_GrassGrazedExtensive:
	    case toc_GrassGrazedLast:
		case toc_CloverGrassGrazed1:
		case toc_CloverGrassGrazed2:
		case toc_CloverGrassGrazed3:
		case toc_OCloverGrassGrazed1:
		case toc_OCloverGrassGrazed2:
		case toc_OCloverGrassGrazed3:
		case toc_FodderGrass:
		case toc_OFodderGrass:
			SetGrazingLevel(cfg_farm_cattle_grazing_lvl_rot.value());
			break;
		case toc_OPermanentGrassGrazed:
		case toc_PermanentGrassGrazed:
			SetGrazingLevel(cfg_farm_cattle_grazing_lvl_pasture.value());
			break;
		case toc_PermanentGrassLowYield:
		case toc_OPermanentGrassLowYield:
			SetGrazingLevel(cfg_farm_cattle_grazing_lvl_lowyield.value());
			break;
		case toc_PermanentGrassTussocky:
			SetGrazingLevel(cfg_farm_cattle_grazing_lvl_tussocky.value());
			break;
      default:
			SetGrazingLevel(0);
    }
  }
	void SetInsectBiomassParametersIndex(TTypesOfCrops a_type);
  void SetInterestedBiomassFractionForCrop(TTypesOfCrops a_crop_type);
	virtual void  Insecticide(double a_fraction)
	{
		m_insect_pop *= a_fraction;
		m_days_since_insecticide_spray += 28;
	}

	// -- VEG ATTRIBUTES 
	virtual bool Is_Att_Veg(void) { return m_att_veg; }
	virtual void Set_Att_Veg(bool p) { m_att_veg = p; }
	virtual bool Is_Att_VegPatchy(void) { return m_att_veg_patchy; }
	virtual void Set_Att_VegPatchy(bool p) { m_att_veg_patchy = p; }
	virtual bool Is_Att_VegCereal() { return m_att_veg_cereal; }
	virtual void Set_Att_VegCereal(bool p) { m_att_veg_cereal = p; }
	virtual bool Is_Att_VegMatureCereal() { return m_att_veg_maturecereal; }
	virtual void Set_Att_VegMatureCereal(bool p) { m_att_veg_maturecereal = p; }
	virtual bool Is_Att_VegMaize() { return m_att_veg_maize; }
	virtual void Set_Att_VegMaize(bool p) { m_att_veg_maize = p; }
	virtual bool Is_Att_VegGrass() { return m_att_veg_grass; }
	virtual void Set_Att_VegGrass(bool p) { m_att_veg_grass = p; }
	virtual bool Is_Att_VegGooseGrass() { return m_att_veg_goosegrass; }
	virtual void Set_Att_VegGooseGrass(bool p) { m_att_veg_goosegrass = p; }


	// for easy compatibility of code after introduction of attributes  
	inline bool IsMatureCereal() { return Is_Att_VegMatureCereal(); }
	inline bool IsMaize() { return Is_Att_VegMaize(); }
	inline bool IsCereal() { return Is_Att_VegCereal(); }
	inline bool IsGooseGrass() { return Is_Att_VegGooseGrass(); }
	inline void SetVegPatchy(bool p) { Set_Att_VegPatchy(p); }

	// -- END VEG ATTRIBUTES 

	virtual void  InsectMortality(double a_fraction);
	virtual void  ReduceWeedBiomass(double a_fraction)
	{
		m_weed_biomass *= a_fraction;
	}
	virtual void  ToggleCattleGrazing(void) { if (m_cattle_grazing == 0) m_cattle_grazing = m_default_grazing_level; else m_cattle_grazing = 0;; }
	virtual void  TogglePigGrazing(void) { m_pig_grazing = !m_pig_grazing; }
	/** /brief Reduce the total vegetation biomass as a result of grazing per m2 */
	virtual void GrazeVegetation(double a_grams, bool a_force);
	/** /brief Reduces the vegetation by length of grass (in cm) */
	virtual void  GrazeVegetationHeight(double a_reduc);
	/** /brief Reduce the total vegetation biomass as a result of grazing per polygon */
	virtual void GrazeVegetationTotal(double a_grams);
	// Outdated! Use ReduceVeg() below instead. Total biomass is
	// (re)calculated from the total leaf area, it cannot just be
	// set or changed to an arbitrary value.
	//virtual void  SetVegBiomass( int a_veg_biomass )
	//  { m_veg_biomass = a_veg_biomass; }
	virtual void SetVegType(TTypesOfVegetation a_vege_type);
	virtual void SetVegType(TTypesOfVegetation a_vege_type, TTypesOfVegetation a_weed_type);
	virtual void SetCropData(double, double, double, TTypesOfVegetation, double, int);
	virtual void SetCropDataAll(double, double, double, double, TTypesOfVegetation, double, double, int, double, bool, double, double, double);
	virtual void SetVegHeight(double a_veg_height) { m_veg_height = a_veg_height; }
	virtual void SetDigestibility(double a_digestability) { m_digestability = a_digestability; }
	virtual void SetGrazingLevel(int a_grazing) { m_default_grazing_level = a_grazing; }
	virtual void SetVegParameters(double a_veg_height, double a_LAtotal, double a_LAgreen, double a_WeedBiomass) {
		m_veg_height = a_veg_height;
		m_LAtotal = a_LAtotal;
		m_LAgreen = a_LAgreen;
		m_weed_biomass = a_WeedBiomass;
		//RecalculateBugsNStuff();
	}
	virtual void  SetGrowthPhase(int a_phase);
	virtual void  StoreLAItotal() { m_oldLAtotal = m_LAtotal; }
	virtual void  ForceGrowthInitialize(void);
	virtual void  ForceGrowthTest(void);
	virtual void  ForceGrowthDevelopment(void);
	virtual void  ZeroVeg(void);
	virtual void  ReduceVeg(double a_reduc);
	virtual void  ReduceVeg_Extended(double a_reduc);

	/** \brief sets growth record to zero */
	virtual void ResetDigestibility()
	{
		StoreLAItotal();
		for (int i = 0; i < 32; i++)  m_oldnewgrowth[i] = 0.0;
	}

	virtual int GetVegPhase() { return m_veg_phase; }
	/** \brief Sets the correct species simulation behaviour for the current species */
	virtual void SetSpeciesFunction(TTypesOfPopulation a_species);
  /** \brief Function to set the instrested biomass fraction.*/
  void SetInterestedBiomassFraction(double a_interested_biomass_fraction) { m_interested_biomass_fraction = a_interested_biomass_fraction; }

protected:
	TTypesOfVegetation m_vege_type;
	int   m_curve_num;
	int   m_weed_curve_num;
	int   m_veg_phase;
	double   m_growth_scaler;
	int   m_nutrient_status;
	double m_LAtotal;
	double m_oldLAtotal;
	double m_LAgreen;
	double m_insect_pop;
	double m_veg_biomass;
	double m_total_biomass;
	double m_total_biomass_old;
	double m_green_biomass;
  double m_interested_green_biomass;
	double m_dead_biomass;
	double m_weed_biomass;
	double m_veg_height;
	int m_veg_density;

	// -- VEG ATTRIBUTES FLAGS
	bool m_att_veg;
	bool m_att_veg_patchy;
	bool m_att_veg_cereal;
	bool m_att_veg_maturecereal;
	bool m_att_veg_maize;
	bool m_att_veg_grass;
	bool m_att_veg_goosegrass;
	bool m_att_veg_forest;
	// -- END VEG ATTRIBUTES FLAGS

	double m_veg_cover;
	double m_newgrowth;
	double m_new_weed_growth;
	bool  m_forced_phase_shift;
	double m_digestability;
	double m_oldnewgrowth[32];
	double m_oldnewgrowth2[14];
	int m_newoldgrowthindex;
	int m_newoldgrowthindex2;
	double m_newgrowthsum;
	double m_newgrowthruningaveragesum;
	double m_newgrowthruningaverage;
	double m_greenbiomass_per;
	double m_force_Weed;
	double m_force_LAgreen;
	double m_force_LAtotal;
	double m_force_veg_height;
	bool  m_force_growth;
	/** \brief The flag to start accumulating day degrees for flower resource.*/
	bool m_start_dd_flower;
	ifstream m_ifile;
	void  ReadBugPercentageFile(void);
	/** \brief This method is responsible for */
	virtual void  RecalculateBugsNStuff(void);
	/** \brief Calculates spilled grain and grazing forage resources for geese */
	void CalcGooseForageResources();
	void  RandomVegStartValues(double* a_LAtotal, double* a_LAgreen, double* a_veg_height, double* a_weed_biomass);
	void  ForceGrowthSpringTest(void);
	/** \brief Sets vegetation digestability for hare and vole */
	void CalculateDigestibility();
	/** \brief Determines the amount of new growth */
	void CalculateAphidDrivers();
	/** \brief An insect biomass calculation for those species that need it */
	void CalculateInsectBiomass();
	/** \brief Reset geese numbers to zero in case this was not done by the population manager (the normal situation). */
	void ResetGeese(void);
	int m_crop_index;

	/** \brief Contains the day degrees since the sowing, this is only used for the crops. */
	double m_acc_DD_flower_resource;
	/** \brief Contains the number of days of the flowring period, this is only used for the crops*/
	double m_num_flowring_peroid;

	/** \brief A useful function pointer that we can use to specify species specific calculations. The default does nothing. */
	void (VegElement::* SpeciesSpecificCalculations)(void);
	/** \brief The default for SpeciesSpecificCalculations */
	void DoNothing(void) { return; }
};


class Field : public VegElement
{
	/**
	* Extends the VegElement class with crop related behaviour and attributes.
	* Adds a functional crop type, as well as the possibility to get previous tov types
	*/
protected:
public:
	/** \brief Overides the veg element version to skip pollen and nectar reset on Jan 1st */
	void Tick(void) override;
	Field(Landscape* L);
	virtual void DoDevelopment(void);
	TTypesOfVegetation GetPreviousTov(int a_index);
	virtual TTypesOfCrops GetCropType() { return m_CropType; }
	//virtual void SetCropType(TTypesOfCrops a_type) { m_CropType = a_type;  SetInsectBiomassParametersIndex(a_type); }
	int GetAphidDensity();
    void SetVegGrowthScalerRand();
};


class PermanentSetaside : public VegElement
{
public:
  PermanentSetaside(Landscape *L);
};


class Hedges : public VegElement
{
public:
  Hedges(Landscape *L);
  virtual void DoDevelopment( void ) {
    VegElement::DoDevelopment();
    m_insect_pop=0;
  }
};


class HedgeBank : public VegElement
{
public:
  HedgeBank(Landscape *L);
  virtual void DoDevelopment(void) {
	VegElement::DoDevelopment();
	m_insect_pop = m_insect_pop * 3.0;
  }
};


class BeetleBank : public VegElement
{
public:
	BeetleBank(Landscape* L);
	virtual void DoDevelopment(void);
};


/**
\brief Element class for flower strips
*/
class FlowerStrip : public VegElement
{
public:
	FlowerStrip(Landscape* L, int a_type);
	virtual void DoDevelopment(void);
	void SetSubType(int a_subtype) {
		if (a_subtype == 2) SetVegType(tov_FlowerStrip2);
		else if (a_subtype == 3) SetVegType(tov_FlowerStrip3);
		else SetVegType(tov_FlowerStrip1);
		// Once the tov type is fixed set the veg curve number
		m_curve_num = g_crops->VegTypeToCurveNum(m_vege_type);
	}
protected:
	void Cutting(int a_today);
	long m_DateCut{-1};
};

/**
\brief Element class for field boudaries
*/
class FieldBoundary : public VegElement
{
public:
	FieldBoundary(Landscape* L, int a_type);
	virtual void DoDevelopment(void);
protected:
	void Cutting(int a_today);
	long m_DateCut;
};


class RoadsideVerge : public VegElement
{
  // Attributes
  void Cutting(int a_today);
  long m_DateCut;

 public:
  RoadsideVerge( TTypesOfLandscapeElement tole, Landscape *L );
  // Is needed due to special development for this element
  // type, cutting of the roadside verges.
  virtual void DoDevelopment( void );
};

class WaterBufferZone : public VegElement
{
	void ResetingVeg(int a_today);
public:
	WaterBufferZone(Landscape *L);
	// Is needed due to special development for this element
	// We want to reset the vegetation every year
	virtual void DoDevelopment(void);
};

class NaturalGrass : public VegElement
{
public:
  NaturalGrass( TTypesOfLandscapeElement tole, Landscape * L);
  virtual void DoDevelopment();
};

class UnsprayedFieldMargin : public VegElement
{
public:
  UnsprayedFieldMargin(Landscape *L);
  virtual void DoDevelopment( void );
  /** This is a special element having the following properties:
  *
  * It is formed from a field to which it has a reference number.
  * The vegetation is formed of crop * 0.9 in biomass + weeds.
  * Agricultural operations occur following those of the field to which the element is attached, except for spraying events which do not happen.
  * Weeds grow following their own special growth curve.
  */
};

//---------------------------------------------------------------------------


class ForestElement : public VegElement
{
public:
  ForestElement(TTypesOfLandscapeElement tole, Landscape *L );
  virtual void DoDevelopment( void ) {
    VegElement::DoDevelopment();
    m_insect_pop=0;
  }
};

class PastureElement : public VegElement
{
public:
	PastureElement(TTypesOfLandscapeElement tole, Landscape* L);
};

class Orchard : public VegElement
{
  void Cutting(int a_today);
  long m_DateCut;

public:
  Orchard(Landscape *L);
  // Is needed due to special development for this element
  // type, cutting of the roadside verges.
  virtual void  DoDevelopment( void );
};

class OrchardBand : public VegElement
{
  long m_LastSprayed;

public:
  OrchardBand( Landscape *L );
  // Is needed due to special development for this element
  // type, cutting of the roadside verges.
  virtual void  DoDevelopment( void );
};

class MownGrassStrip : public VegElement
{
  void Cutting(int a_today);
  long m_DateCut;

public:
  MownGrassStrip(Landscape *L);
  /** 
  * DoDevelopment is needed due to special development for this element type, i.e. cutting of the grass.
  */
  virtual void  DoDevelopment( void );
};

class NonVegElement : public LE
{
public:
  NonVegElement( TTypesOfLandscapeElement tole, Landscape *L );
};

/** \brief Ponds are defined as freshwater bodies between 25m2 and 5000m2. For newts the effective size is limited to 400m2 */
/**
* Currently the only pond 'behviour' is related to provision of food for newt larvae.
* This is determined daily and can be seasonally adjusted.
*/
class Pond : public NonVegElement {
protected:
	/** \brief  The amount of larval food present  */
	double m_LarvalFood;
	/** \brief The proportion of larval food per m2  */
	double m_LarvalFoodScaler;
	/** \brief Holds the pesticide content per unit pond water */
	double m_pondpesticide;
	/** \brief Flag for presence of a male newt */
	bool m_MaleNewtPresent;
	/** \brief a factor used to alter the pond qualities (default behaviour is random 0.0-1.0) */
	double m_pondquality;
public:
	Pond( Landscape *L );
	/**
	* DoDevelopment is needed due to special development for this element type, i.e. larval food calculations for newts.
	*/
	virtual void  DoDevelopment(void);
	/** \brief  Calculates the amount of larval food present  */
	void CalcLarvalFood();
	/** \brief  Called by a larva when feeding, removes an age specific amount of larval food  */
	bool SubtractLarvalFood(double a_food);
	/** \brief supply the current pesticide concentration per litre */
	double SupplyPondPesticide() { return m_pondpesticide; }
	/** \brief Sets a male as present/absent */
	virtual void SetMaleNewtPresent(bool a_ispresent) { m_MaleNewtPresent = a_ispresent; };
	/** \brief Gets whether there is a male newt as present */
	bool IsMaleNewtPresent() { return m_MaleNewtPresent; };
};

class LargeRoad : public NonVegElement
{
public:
  virtual double GetTrafficLoad( void );
  LargeRoad( Landscape *L );
};



class SmallRoad : public NonVegElement
{
public:
  virtual double GetTrafficLoad( void );
  SmallRoad( Landscape *L );
};

#endif // ELEMENTS_H

