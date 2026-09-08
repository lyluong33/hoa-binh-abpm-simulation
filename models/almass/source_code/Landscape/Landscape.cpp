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
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Landscape/Map_cfg.h"
#include "../Skylark/SkylarkAll.h"


using namespace std;

// Version info.
static const int version_major = 1;
static const int version_minor = 0;
static const int version_revision = 0;
static const char * version_date = "2014-02-03";

class PollenNectarData;
class SkTerritories;

static CfgStr l_map_nectarpollencomposition("MAP_NECTARPOLLEN_COMPOSITION", CFG_CUSTOM, "habitat_stacks.csv");
extern CfgBool l_pest_enable_pesticide_engine;
extern CfgInt l_pest_NoPPPs;
CfgBool cfg_rodenticide_enable("RODENTICIDE_ENABLE",CFG_CUSTOM,false);
CfgBool cfg_rodenticide_reporting_enable("RODENTICIDE_REPORTING_ENABLE",CFG_CUSTOM,false);

static CfgBool cfg_map_usesoiltypes("MAP_USESOILTYPES",CFG_CUSTOM,false);
static CfgBool cfg_CalculateCentroids("MAP_CALCULATE_CENTROIDS",CFG_CUSTOM,false);

static CfgFloat cfg_PestIncidenceMax("PESTINCIDENCEMAX", CFG_CUSTOM, 1.0);
static CfgFloat cfg_PestIncidenceMin("PESTINCIDENCEMIN", CFG_CUSTOM, 0.0);


extern CfgBool cfg_OptimiseBedriftsmodelCrops;
extern CfgBool cfg_OptimisingFarms;
extern CfgBool cfg_DumpFarmAreas;
extern CfgBool cfg_MaizeEnergy;
extern CfgBool cfg_pollen_nectar_on;

// Hedge Subtype controls
CfgInt cfg_HedgeSubtypeMinimum("HEDGE_SUBTYPEMINIMUM",CFG_CUSTOM,0);
CfgInt cfg_HedgeSubtypeMaximum("HEDGE_SUBTYPEMAXIMUM",CFG_CUSTOM,3); // actual max is 2 (<3)

// For beetle banks, patches or borders
static CfgInt cfg_FarmAreaPercentage("FARM_AREA_PERCENTAGE", CFG_CUSTOM, 100); // out of 100

// Border between fields
static CfgBool cfg_BordersBetweenFields("BORDER_BETWEEN_FIELDS", CFG_CUSTOM, true);

// Beetlebank variables
static CfgBool cfg_AddBeetleBanks("BBANKS_ADD",CFG_CUSTOM,false);
static CfgInt cfg_BeetleBankWidth("BBANK_WIDTH",CFG_CUSTOM,4);
static CfgInt cfg_BeetleBankMaxDistance("BBANK_MAX_DIST", CFG_CUSTOM, 150);
static CfgInt cfg_BeetleBankChance("BBANK_CHANCE", CFG_CUSTOM, 100); // out of 100
CfgBool cfg_BeetleBankInvert("BBANK_INVERT",CFG_CUSTOM,false);
CfgInt cfg_BeetleBankMinX("BBANK_MINX",CFG_CUSTOM,0);
CfgInt cfg_BeetleBankMinY("BBANK_MINY",CFG_CUSTOM,0);
CfgInt cfg_BeetleBankMaxX("BBANK_MAXX",CFG_CUSTOM,100000);
CfgInt cfg_BeetleBankMaxY("BBANK_MAXY",CFG_CUSTOM,100000);
static CfgInt cfg_BeetleBankType("BBANK_TYPE",CFG_CUSTOM, (int)tole_BeetleBank);

// Patch variables
static CfgBool cfg_AddPatches("PATCHES_ADD", CFG_CUSTOM, false);
static CfgInt cfg_PatchesMinWidth("PATCHES_MIN_WIDTH", CFG_CUSTOM, 10);
static CfgInt cfg_PatchesMaxWidth("PATCHES_MAX_WIDTH", CFG_CUSTOM, 100);
static CfgInt cfg_PatchesFieldAreaMaxPercentage("PATCHES_MAX_PERCENTAGE", CFG_CUSTOM, 15);
static CfgInt cfg_PatchesType("PATCHES_TYPE", CFG_CUSTOM, (int)tole_BeetleBank);

// Veg area dump config variables
CfgBool cfg_dumpvegjan( "G_VEGAREASJAN_ON", CFG_CUSTOM, false );
CfgStr cfg_dumpvegjanfile( "G_VEGAREASJAN_FILENAME", CFG_CUSTOM, "DUMPVEG_JAN.TXT" );
CfgBool cfg_dumpvegjune( "G_VEGAREASJUNE_ON", CFG_CUSTOM, false );
CfgStr cfg_dumpvegjunefile( "G_VEGAREASJUNE_FILENAME", CFG_CUSTOM, "DUMPVEG_JUNE.TXT" );

// Crops configuration options
CfgFloat cfg_strigling_prop( "CROPS_STRIGLING_PROPORTION", CFG_CUSTOM, 1.0 );
CfgFloat cfg_silage_prop( "CROPS_SILAGE_PROPORTION", CFG_CUSTOM, 1.0 );
CfgFloat cfg_ins_app_prop1("CROPS_INSECTICIDE_APPLIC_ONE_PROPORTION", CFG_CUSTOM, 1.0);
CfgFloat cfg_ins_app_prop2("CROPS_INSECTICIDE_APPLIC_TWO_PROPORTION", CFG_CUSTOM, 1.0);
CfgFloat cfg_ins_app_prop3("CROPS_INSECTICIDE_APPLIC_THREE_PROPORTION", CFG_CUSTOM, 1.0);
CfgFloat cfg_herbi_app_prop("CROPS_HERBICIDE_APPLIC_PROPORTION", CFG_CUSTOM, 1.0);
CfgFloat cfg_fungi_app_prop1("CROPS_FUNGICIDE_APPLIC_ONE_PROPORTION", CFG_CUSTOM, 1.0);
CfgFloat cfg_fungi_app_prop2("CROPS_FUNGICIDE_APPLIC_TWO_PROPORTION", CFG_CUSTOM, 1.0);
CfgFloat cfg_fungi_app_prop3("CROPS_FUNGICIDE_APPLIC_THREE_PROPORTION", CFG_CUSTOM, 1.0);
CfgFloat cfg_greg_app_prop("CROPS_GROWTHREGULATOR_APPLIC_PROPORTION", CFG_CUSTOM, 1.0);

// Below define the area for custom actions, if valid_x, and valid_y fall in the
// area defined by x,y,size, then something can be set on that polygon
// The something is hard-coded each time it is used
CfgInt cfg_l_treatment_x( "LAND_TREATMENTX", CFG_CUSTOM, 0 );
CfgInt cfg_l_treatment_y( "LAND_TREATMENTY", CFG_CUSTOM, 0 );
CfgInt cfg_l_treatment_size( "LAND_TREATMENTSIZE", CFG_CUSTOM, -1 );
CfgBool cfg_l_usecustompoly( "LAND_USECUSTOMPOLY", CFG_CUSTOM, false );

// The config variable below notes the year from the start of the sim when
// the pesticide engine will start to spray its pesticide
// A test for the value m_toxShouldSpray in landscape is required before sprays
// can be used.
CfgInt cfg_productapplicstartyear("PEST_PROCTAPPLICSTARTYEAR",CFG_CUSTOM,9999999);
CfgInt cfg_productapplicendyear("PEST_PROCTAPPLICENDYEAR",CFG_CUSTOM,-1);
CfgInt cfg_pesticidetesttype("PESTICIDETESTYPE",CFG_CUSTOM, -1);

/** \brief Flag determining whether we are using the pesticide map */
CfgBool cfg_pesticidemapon("PEST_MAP_ON", CFG_CUSTOM, false);
/** \brief Flag determining whether we are using the pesticide table */
CfgBool cfg_pesticidetableon("PEST_TABLE_ON", CFG_CUSTOM, false);
/** \brief The first simulation year the pesticide is mapped*/
CfgInt cfg_pesticidemapstartyear("PEST_MAP_STARTYEAR", CFG_CUSTOM, 0);
/** \brief The numer of years of pesticide mapping */
CfgInt cfg_pesticidemapnoyears("PEST_MAP_NOYEARS", CFG_CUSTOM, 1);
/** \brief The interval between maps */
CfgInt cfg_pesticidemapdayinyear("PEST_MAP_DAYINYEAR", CFG_CUSTOM, 364);
/** \brief The output cell size for pesticides - a performance penalty if this does not match the pesticide cell size set by PEST_GRIDSIZE in pesticide.h */
CfgInt cfg_pesticidemapcellsize("PEST_MAP_CELLSIZE", CFG_CUSTOM, 10);
/** \brief True for specific pesticide, false for general pesticides */
CfgBool cfg_pesticidemaptype("PEST_MAP_TYPE", CFG_CUSTOM, false);



// Local configuration options:
/** \brief If freshwater area is below this it is designated a pond */
static CfgInt cfg_MaxPondSize( "MAP_MAXPONDSIZE", CFG_CUSTOM, 5000 );
static CfgInt l_map_no_pesticide_fields( "MAP_NO_PESTICIDE_FIELDS", CFG_CUSTOM, 0 );
static CfgBool l_map_print_version_info( "MAP_PRINT_VERSION_INFO", CFG_CUSTOM, true );
/** \brief Should git version info be printed to file and console?*/ 
CfgBool l_map_print_git_version_info( "MAP_PRINT_GIT_VERSION_INFO", CFG_CUSTOM, false );

// Whether to exit on the condition that the number of covered squares
// on the map becomes zero upon adding the border to a very slim polygon.
static CfgBool l_map_exit_on_zero_area( "MAP_EXIT_ON_ZERO_AREA", CFG_CUSTOM, true );

// Whether to check in the map for the existence of polygons that are
// mentioned in the polygon reference file.
static CfgBool l_map_check_polygon_xref( "MAP_CHECK_POLYGON_XREF", CFG_CUSTOM, true );

// Whether to add artificial hedgebanks to every hedge polygon in the map.
static CfgBool l_map_art_hedgebanks( "MAP_ART_HEDGEBANKS", CFG_CUSTOM, false );

static CfgStr l_map_map_file( "MAP_MAP_FILE", CFG_CUSTOM, "map.lsb" );
static CfgStr l_map_poly_file( "MAP_POLY_FILE", CFG_CUSTOM, "polygonrefs.txt" );
static CfgStr cfg_farmeventfile("FARMEVENT_FILENAME", CFG_CUSTOM, "FarmEvents.txt");
static CfgStr cfg_total_polnek_file("TOTAL_POLNEK_FILENAME", CFG_CUSTOM, "Total_PollenNectarResources.txt");
static CfgStr cfg_tov_polnek_file("TOV_POLNEK_FILENAME", CFG_CUSTOM, "PollenNectarResourcesTOV.txt");
static CfgStr cfg_habitat_polnek_file("HABITAT_POLNEK_FILENAME", CFG_CUSTOM, "PollenNectarResourcesHabitat.txt");

static CfgInt l_map_chameleon_replace_num( "MAP_CHAMELEON_REPLACE_NUM", CFG_CUSTOM, 58 );

//'''''''''''''''''' CIPE LANDSCAPE MAKER CODE HERE  //'''''''''''''''''''''
static CfgBool l_map_CIPEmaker_enable( "MAP_CIPEMAKER_ENABLE", CFG_CUSTOM, false );
//'''''''''''''''''' CIPE LANDSCAPE MAKER CODE ABOVE  //'''''''''''''''''''''

static CfgBool l_map_dump_enable( "MAP_DUMP_ENABLE", CFG_CUSTOM, false );
static CfgBool l_map_removesmallpolygons("MAP_REMOVESMALLPOLYGONS", CFG_CUSTOM, false);
CfgStr l_map_dump_map_file( "MAP_DUMP_MAP_FILE", CFG_CUSTOM, "dump.lsb" );
static CfgBool l_map_dump_gfx_enable( "MAP_DUMP_GFX_ENABLE", CFG_CUSTOM, false );
static CfgStr l_map_dump_gfx_file( "MAP_DUMP_GFX_FILE", CFG_CUSTOM, "dump.ppm" );
static CfgBool l_map_dump_exit( "MAP_DUMP_EXIT", CFG_CUSTOM, false );
CfgStr l_map_dump_poly_file( "MAP_DUMP_POLY_FILE", CFG_CUSTOM, "dump_polyrefs.txt" );
static CfgInt l_map_umargin_width( "MAP_UMARGINWIDTH", CFG_CUSTOM, 10 );
static CfgStr l_map_dump_margin_file( "MAP_DUMP_MARGIN_FILE", CFG_CUSTOM, "dumpunsprayedmargins.txt" );
static CfgBool l_map_dump_treatcounts_enable( "MAP_DUMP_TREATCOUNTS_ENABLE", CFG_CUSTOM, false );
static CfgStr l_map_dump_treatcounts_file( "MAP_DUMP_TREATCOUNTS_FILE", CFG_CUSTOM, "treatment_counts.txt" );

static CfgBool l_map_dump_veg_enable( "MAP_DUMP_VEG_ENABLE", CFG_CUSTOM, false );
static CfgBool l_map_dump_grain_enable( "MAP_DUMP_GRAIN_ENABLE", CFG_CUSTOM, false );
static CfgInt  l_map_dump_grain_each_x_days("MAP_DUMP_GRAIN_EACH_X_DAYS", CFG_CUSTOM, 1);
CfgInt l_map_dump_veg_x( "MAP_DUMP_VEG_X", CFG_CUSTOM, 100 );
CfgInt l_map_dump_veg_y( "MAP_DUMP_VEG_Y", CFG_CUSTOM, 100 );
static CfgBool l_map_dump_event_enable( "MAP_DUMP_EVENT_ENABLE", CFG_CUSTOM, false );
static CfgInt l_map_dump_event_x1( "MAP_DUMP_EVENT_XA", CFG_CUSTOM, 4287 );
static CfgInt l_map_dump_event_y1( "MAP_DUMP_EVENT_YA", CFG_CUSTOM, 2909 );
static CfgInt l_map_dump_event_x2( "MAP_DUMP_EVENT_XB", CFG_CUSTOM, 4333 );
static CfgInt l_map_dump_event_y2( "MAP_DUMP_EVENT_YB", CFG_CUSTOM, 2889 );
/*
static CfgBool l_map_renumberpolys("MAP_RENUMBERPOLY", CFG_CUSTOM, false);
*/
/** \brief Used to consolidate polygons with no special behaviour into a single polygon of that type */
static CfgBool l_map_consolidatepolys("MAP_CONSOLIDATEPOLYS", CFG_CUSTOM, false);
static CfgBool l_map_calc_openness("MAP_CALC_OPENNESS",CFG_CUSTOM,false );
/** \brief Used if an ASCII file for use in GIS applications should be written */
static CfgBool l_map_write_ascii( "MAP_WRITE_ASCII", CFG_CUSTOM, false );
/** \brief If we write an ASCII file provide UTM-x of lower lefthand corner */
static CfgInt l_map_ascii_utm_x( "MAP_ASCII_UTM_X", CFG_CUSTOM, 0 );
/** \brief If we write an ASCII file provide UTM-y of lower lefthand corner */
static CfgInt l_map_ascii_utm_y( "MAP_ASCII_UTM_Y", CFG_CUSTOM, 0 );
/** \brief If we write an ASCII file what should be the mapped entity? 1 = polyref, 2 = elementype */
static CfgInt l_map_ascii_map_entity( "MAP_ASCII_MAP_ENTITY", CFG_CUSTOM, 1 );
/*
static CfgBool l_map_read_openness("MAP_READ_OPENNESS",CFG_CUSTOM,true );
static CfgBool l_map_write_openness("MAP_WRITE_OPENNESS",CFG_CUSTOM,false );
static CfgStr l_map_opennessfile("MAP_OPENNESSFILE",CFG_CUSTOM,"FieldOpennessScores.txt");
*/
// For building centroid calculations
static CfgInt cfg_mintownbuildingdistance("MAP_MINTOWNBUILDINGDISTANCE",CFG_CUSTOM, 100);
static CfgInt cfg_mintownbuildingnumber("MAP_MINTOWNBUILDINGNUMBER",CFG_CUSTOM, 6);

extern CfgBool cfg_rectangularmaps_on;
/*
extern CfgFloat cfg_P1A;// Parameter 1
extern CfgFloat cfg_P1B; // Parameter 2
extern CfgFloat cfg_P1C; // Parameter 3
extern CfgFloat cfg_P1D; // The scaler to change to from g/dw to KJ
extern CfgBool  cfg_P1E; // reverse axis values
extern CfgFloat cfg_P1F; // Max x-value - at this point the curve tends to 0, must stope here to avoid negative values
extern CfgFloat cfg_P1G; // Min x-value
extern CfgStr   cfg_P1H;
*/
// For hareas
extern double g_FarmIntensivenessH;
extern double g_VegHeightForageReduction;
/*
extern CfgFloat cfg_G6A;// Parameter 1
extern CfgFloat cfg_G6B; // Parameter 2
extern CfgFloat cfg_G6C; // Parameter 3
extern CfgFloat cfg_G6D; // The scaler to change to from g/dw to KJ
extern CfgBool  cfg_G6E; // reverse axis values
extern CfgFloat cfg_G6F; // Max x-value - at this point the curve tends to 0, must stope here to avoid negative values
extern CfgFloat cfg_G6G; // Min x-value
extern CfgStr   cfg_G6H;

extern CfgFloat cfg_B6A;// Parameter 1
extern CfgFloat cfg_B6B; // Parameter 2
extern CfgFloat cfg_B6C; // Parameter 3
extern CfgFloat cfg_B6D; // The scaler to change to from g/dw to KJ
extern CfgBool  cfg_B6E; // reverse axis values
extern CfgFloat cfg_B6F; // Max x-value - at this point the curve tends to 0, must stope here to avoid negative values
extern CfgFloat cfg_B6G; // Min x-value
extern CfgStr   cfg_B6H;
*/
/** \brief Controlling whether the functional response curves used are written to disk */
CfgBool cfg_WriteCurve{"CURVE_WRITE", CFG_CUSTOM, false};
/**
The P1 curve replicates a curve fitted by Therkildsen, O.R. & Madsen, J. 2000: Assessment of food intake rates in pinkfooted geese Anser brachyrhynchus
based on examination of oesophagus contents. - Wildl. Biol. 6: 167-172.\n
Assimilation of energy is based on Madsen 1985 Ornis Scand. p 222 - 228: Relations between spring habitat...
Conversion from g/dw to kJ is using 19.8 (Madsen 1985 Vol 16 p. 222 - 228 Ornis Scandinavica)
The P2 & P3 curves are fitted based on Durant et al 2003 J Anim Ecol 72, 220-231.\n
Y-values are kJ per minute, x-values are grass height. The formula is a simple 2nd order polynomial which is the then scaled to get to kJ.\n
The H1 curve is based on Amano et al. 2004 Alleviating grazing damage... Journal of Applied Ecology 41(4): 675-688.\n
The H2 curve is based on Clausen et al. (2018) Agriculture, Ecosystems & Environment 259: 72-76.
The Petti1 curve is based on Pettifor et al (2000). Journal of Applied Ecology 37: 103-135.
*/
/** \brief Coefficient A in a second order polynomial function */
CfgFloat cfg_P1A{"POLYNOMIALTWO_ONE_A", CFG_CUSTOM, -0.1884};
/** \brief Coefficient B in a second order polynomial function */
CfgFloat cfg_P1B{"POLYNOMIALTWO_ONE_B", CFG_CUSTOM, 3.1349};
/** \brief The constant C in a second order polynomial function */
CfgFloat cfg_P1C{"POLYNOMIALTWO_ONE_C", CFG_CUSTOM, 0.0};
/** \brief Scaler for assimilation of energy from grass for pinkfeet.
* Madsen 1985 Vol 16 p. 222 - 228 Ornis Scandinavica */
CfgFloat cfg_P1D{"POLYNOMIALTWO_ONE_D", CFG_CUSTOM, 0.404};
/** \brief Logical config to control if the curve should be reversed (i.e. 1 - value) */
CfgBool cfg_P1E{"POLYNOMIALTWO_ONE_E", CFG_CUSTOM, false};
/** \brief Max x-value - at this point the curve tends to 0, must stop here to avoid negative values */
CfgFloat cfg_P1F{"POLYNOMIALTWO_ONE_F", CFG_CUSTOM, 16.63};
/** \brief Min x-value */
CfgFloat cfg_P1G{"POLYNOMIALTWO_ONE_G", CFG_CUSTOM, 0.00};
/** \brief File name for grass functional response for pinkfeet */
CfgStr cfg_P1H{"POLYNOMIALTWO_ONE_H", CFG_CUSTOM, "KJIntakeAtVaryingGrassHeights_PF"};

/** \brief Coefficient A in a second order polynomial function */
CfgFloat cfg_G6A{"POLYNOMIALTWO_TWO_A", CFG_CUSTOM, -0.1094};
/** \brief Coefficient B in a second order polynomial function */
CfgFloat cfg_G6B{"POLYNOMIALTWO_TWO_B", CFG_CUSTOM, 2.6695};
/** \brief The constant C in a second order polynomial function */
CfgFloat cfg_G6C{"POLYNOMIALTWO_TWO_C", CFG_CUSTOM, 0.0};
/** \brief Scaler for assimilation of energy from grass for pinkfeet.
* Madsen 1985 Vol 16 p. 222 - 228 Ornis Scandinavica */
CfgFloat cfg_G6D{"POLYNOMIALTWO_TWO_D", CFG_CUSTOM, 0.404};
/** \brief Logical config to control if the curve should be reversed (i.e. 1 - value) */
CfgBool cfg_G6E{"POLYNOMIALTWO_TWO_E", CFG_CUSTOM, false};
/** \brief Max x-value - at this point the curve tends to 0, must stop here to avoid negative values */
CfgFloat cfg_G6F{"POLYNOMIALTWO_TWO_F", CFG_CUSTOM, 24.4013};
/** \brief Min x-value */
CfgFloat cfg_G6G{"POLYNOMIALTWO_TWO_G", CFG_CUSTOM, 0.00};
/** \brief File name for grass functional response for greylags */
CfgStr cfg_G6H{"POLYNOMIALTWO_TWO_H", CFG_CUSTOM, "KJIntakeAtVaryingGrassHeights_GL"};

/** \brief Coefficient A in a second order polynomial function */
CfgFloat cfg_B6A{"POLYNOMIALTWO_THREE_A", CFG_CUSTOM, -0.066};
/** \brief Coefficient B in a second order polynomial function */
CfgFloat cfg_B6B{"POLYNOMIALTWO_THREE_B", CFG_CUSTOM, 0.88};
/** \brief The constant C in a second order polynomial function */
CfgFloat cfg_B6C{"POLYNOMIALTWO_THREE_C", CFG_CUSTOM, 5.0};
/** \brief Scaler for assimilation of energy from grass for pinkfeet.
* Madsen 1985 Vol 16 p. 222 - 228 Ornis Scandinavica */
CfgFloat cfg_B6D{"POLYNOMIALTWO_THREE_D", CFG_CUSTOM, 0.404};
/** \brief Logical config to control if the curve should be reversed (i.e. 1 - value) */
CfgBool cfg_B6E{"POLYNOMIALTWO_THREE_E", CFG_CUSTOM, false};
/** \brief Max x-value - at this point the curve tends to 0, must stop here to avoid negative values */
CfgFloat cfg_B6F{"POLYNOMIALTWO_THREE_F", CFG_CUSTOM, 13.4761};
/** \brief Min x-value */
CfgFloat cfg_B6G{"POLYNOMIALTWO_THREE_B", CFG_CUSTOM, 0.00};
/** \brief File name for grass functional response for barnacles */
CfgStr cfg_B6H{"POLYNOMIALTWO_THREE_H", CFG_CUSTOM, "KJIntakeAtVaryingGrassHeights_BG"};

/** \brief The pollen and nectar curve number for flower strip.*/
CfgInt cfg_FlowerStripPollenNectarCurveNum{"FLOWER_STRIP_POLLEN_NECTAR_CURVE_NUM", CFG_CUSTOM, 59};
extern CfgInt cfg_farm_cattle_grazing_lvl_pasture;
extern CfgInt cfg_farm_cattle_grazing_lvl_lowyield;
extern CfgInt cfg_farm_cattle_grazing_lvl_tussocky;
/** \brief The distance to select neary-by polygons for each location in a map.*/
CfgFloat cfg_distance_to_select_nearby_polygons{"DISTANCE_TO_SELECT_NEARBY_POLYGONS", CFG_CUSTOM, 600.0};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** \brief
* A generally useful array of fast divide calculators by multiplication
*/
double g_speedy_Divides[2001];

/** \brief
* m_polymapping is a mapping from polygon numbers into
* the list of landscape elements, m_elems. When using this it is important that it is the poly num and not the
* map index that is used in calling.
*/
vector<int> * m_polymapping;
Landscape * g_landscape_ptr;

// End Globals ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void Landscape::ReadOpenness()
{
	/**
	* Reads in openness scores for all polygons. Makes a check that the number of polygons in the file matches the number in the landscape first.
	*/ 
/*
    int sz = (int) m_elems.size();
	ifstream ifile(l_map_opennessfile.value(),ios::in);
	if ( !ifile.is_open() )
	{
		Warn("Landscape::ReadOpenness() Cannot open input file ", "FieldOpennessScores.txt" );
	}
	int n, openness, polyref, cx, cy;
	ifile >> n;
	if (n != sz)
	{
		Warn("Landscape::ReadOpenness() number of entries differs from number of polygons ", l_map_opennessfile.value());
		std::exit(0);
	}
	for (int i=0; i<n; i++)
	{
		ifile >> polyref >> openness >> cx >> cy;
		// We have read a polygon reference in, but we need to find the associated m_elems entry. 
		// This can be done using the polymapping.
		int polyindex = m_polymapping[polyref];
		if (polyref != m_elems[polyindex]->GetPoly())
		{
			g_msg->Warn("Landscape::ReadOpenness() mismatch in polyref number at entry ", i);
			std::exit(0);
		}
		// All OK so save this
		m_elems[polyindex]->SetOpenness(openness);
		m_elems[polyindex]->SetCentroid(cx, cy);
	}
	ifile.close();
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::WriteOpenness()
{
	/**
	* Writes in openness scores for all polygons to FieldOpennessScores.txt
	*/ 
/*
    int sz = (int) m_elems.size();
	ofstream ofile(l_map_opennessfile.value(),ios::out);
	if ( !ofile.is_open() )
	{
		Warn("Landscape::WriteOpenness() Cannot open output file ", l_map_opennessfile.value() );
	}
	ofile << sz << endl;
	for (int i=0; i<sz; i++)
	{
		ofile << m_elems[i]->GetPoly() << '\t' << m_elems[i]->GetOpenness() << '\t' << m_elems[i]->GetCentroidX()<<'\t'<<m_elems[i]->GetCentroidY()<<'\t'<< endl;
	}
	ofile.close();
}
//-----------------------------------------------------------------------------------------------------------------------------
*/
void Landscape::CalculateOpenness( bool a_realcalc)
{
	/**
	* First must calculate centroid. Runs through the list of elements and any that are marsh, field, or pasture will have an openness score calculated
	*/
	cout << "In CalculateOpenness" << endl;

	for (unsigned int i = 0; i < m_elems.size(); i++)
	{
		TTypesOfLandscapeElement tole = m_elems[i]->GetElementType();
		switch (tole)
		{
		case tole_Field:
		case tole_Marsh:
        case tole_Scrub:
		case tole_PermPastureLowYield:
		case tole_PermPastureTussocky:
        case tole_PermanentSetaside:
        case tole_PermPasture:
        case tole_NaturalGrassDry:
		case tole_NaturalGrassWet:
			if (a_realcalc)
			{
				cout << i << " ";
				m_elems[i]->SetOpenness(CalculateFieldOpennessAllCells(i));
			}
			else m_elems[i]->SetOpenness(0);
			break;
		default:
			m_elems[i]->SetOpenness(0);
			break;
		}
	}
	if (a_realcalc) cout << endl;
}
//-----------------------------------------------------------------------------------------------------------------------------

int Landscape::CalculateFieldOpennessCentroid(int  a_pref)
{
	// Get the centre point for this field
	int d0 = m_maxextent; // this is the width of the landscape and will always be at least as big as the biggest return value possible
	int cx = m_elems[a_pref]->GetCentroidX();
	int cy = m_elems[a_pref]->GetCentroidY();

	/** Starts with North West and moves round the points of the compass 180 degrees. */
	double offsetx = -1;
	double offsety = -1;
	double dx = 1.0 / 45.0;
	double dy = 0.0;
	for (int deg = 0; deg<90; deg++)
	{
		/** runs a line out and also in 180 degrees, two lines. */
		int d1 = LineHighTest(cx, cy, offsetx, offsety);
		int d2 = LineHighTest(cx, cy, 0 - offsetx, 0 - offsety);
		int d = d1;
		if (d1 > d2) d = d2;
		if (d0 > d) d0 = d;
		offsetx = offsetx + dx;
		offsety = offsety + dy;
	}
	offsetx = 1;
	offsety = 1;
	dy = 0 - dx;
	dx = 0;
	for (int deg = 0; deg<90; deg++)
	{
		/** runs a line out and also in 180 degrees, two lines. */
		int d1 = LineHighTest(cx, cy, offsetx, offsety);
		int d2 = LineHighTest(cx, cy, 0 - offsetx, 0 - offsety);
		int d = d1;
		if (d1 > d2) d = d2;
		if (d0 > d) d0 = d;
		offsetx = offsetx + dx;
		offsety = offsety + dy;
	}
	return d0;
}
//-----------------------------------------------------------------------------------------------------------------------------

int Landscape::CalculateFieldOpennessAllCells(int  a_pref)
{
	int dline;
	int d0 = 0;
	int minX = m_elems[a_pref]->GetMinX();
	int minY = m_elems[a_pref]->GetMinY();
	int maxX = m_elems[a_pref]->GetMaxX();
	int maxY = m_elems[a_pref]->GetMaxY();
	for (int ax = minX; ax <= maxX; ax+=10)
	{
		for (int ay = minY; ay <= maxY; ay+=10)
		{
			dline = m_maxextent; // this is the width of the landscape and will always be at least as big as the biggest return value possible
			// Get a possible point for this field
			int cx = ax;
			int cy = ay;
			if (m_land->Get(ax, ay) == a_pref)
			{
				/** 
				* Starts with North West and moves round the points of the compass 180 degrees. 
				* For each point tested we want the minimum length found, but between points we are interested in the max
				*/
				double offsetx = -1;
				double offsety = -1;
				double dx = 1.0 / 45.0;
				double dy = 0.0;
				for (int deg = 0; deg<90; deg++)
				{
					/** runs a line out and also in 180 degrees, two lines. */
					int d1 = LineHighTest(cx, cy, offsetx, offsety);
					int d2 = LineHighTest(cx, cy, 0 - offsetx, 0 - offsety);
					int d = d1;
					if (d1 > d2) d = d2;
					if (dline > d) dline = d; // get the minimum
					offsetx = offsetx + dx;
					offsety = offsety + dy;
				}
				offsetx = 1;
				offsety = 1;
				dy = 0 - dx;
				dx = 0;
				for (int deg = 0; deg<90; deg++)
				{
					/** runs a line out and also in 180 degrees, two lines. */
					int d1 = LineHighTest(cx, cy, offsetx, offsety);
					int d2 = LineHighTest(cx, cy, 0 - offsetx, 0 - offsety);
					int d = d1;
					if (d1 > d2) d = d2;
					if (dline > d) dline = d;
					offsetx = offsetx + dx;
					offsety = offsety + dy;
				}
				if (dline > d0) d0 = dline; // Get the maximum. Here we might also want to do something like create statistics from the range of dline
			}
		}
	}
	return d0;
}
//-----------------------------------------------------------------------------------------------------------------------------

inline int Landscape::LineHighTest(int a_cx, int a_cy, double a_offsetx, double a_offsety)
{
	/**
	* \return d1 is the distance from cx,cy to the obstruction.
	* Runs out a line along a vector set by offsetx & offsety, from cx & cy until it meets too many high elements.
	*/
	int d1=-1;
	int counter = 1;
	bool found = false;
	while (!found)
	{
		/** Will only stop when two consecutive squares return 'high'. */
		int x = (int) (a_cx + a_offsetx * counter);
		int y = (int) (a_cy + a_offsety * counter);
		if (x<1 || x >= (m_width-2) || y<1 || y >= (m_height-2)) return counter;
		/** The criteria for threat or non-open are based on geese and include tall objects and roads */
		TTypesOfLandscapeElement tole = this->SupplyElementType(x,y);
		if ((tole == tole_LargeRoad) || (tole == tole_SmallRoad) || (tole == tole_HedgeBank)) return counter;
		if (SupplyLEHigh(x,y))
		{
			x = (int) (a_cx + a_offsetx * (counter+1));
			y = (int) (a_cy + a_offsety * (counter+1));
			if (SupplyLEHigh(x,y)) found = true;
			d1=counter;
		}
		counter++;
	}
	return d1;
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::SupplyIsHumanDominated(TTypesOfLandscapeElement a_tole_type)
{
	switch (a_tole_type)
	{
	case tole_Railway:  //118
	case tole_Garden:  //11//tole20
	case tole_Track:  //123
	case tole_SmallRoad:  //122
	case tole_LargeRoad:  //121
	case tole_Building:  //5
	case tole_ActivePit:  //115
	case tole_AmenityGrass:  //12
	case tole_Parkland:  //14
	case tole_UrbanNoVeg:  //8
	case tole_UrbanPark:  //17
	case tole_BuiltUpWithParkland:  //16
	case tole_RoadsideSlope:  //201
	case tole_MetalledPath:  //202
	case tole_Carpark:  //203
	case tole_Churchyard:  //204
	case tole_HeritageSite:  //208
	case tole_Wasteland: // 209
	case tole_PlantNursery: // 
	case tole_Pylon:
	case tole_WindTurbine:
	case tole_FishFarm: // 220
	case tole_UrbanVeg: // Urban vegetated but not garden or park  9
	case tole_RefuseSite: // 224
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::ClassificationPermCrop(TTypesOfVegetation a_vege_type)
{
	/** Currently (18/03/2015) only used for goose foraging, so silage maize does not produce grain */
	switch (a_vege_type)
	{
	case tov_PermanentGrassGrazed: 
	case tov_DEPermanentGrassGrazed:
	case tov_DEOPermanentGrassGrazed:
	case tov_NLPermanentGrassGrazed: 
	case tov_NLPermanentGrassGrazedExtensive:
	case tov_PermanentGrassTussocky: 
	case tov_PermanentSetAside:
	case tov_PermanentGrassLowYield:
	case tov_DEPermanentGrassLowYield:
	case tov_DEOPermanentGrassLowYield:
	case tov_YoungForest:
	case tov_OrchardCrop: 
	case tov_NLOrchardCrop:
	case tov_PTOliveGroveTraditional:
	case tov_PTOliveGroveTradOrganic:
	case tov_PTOliveGroveIntensive:
	case tov_PTOliveGroveSuperIntensive:
	case tov_PTPermanentGrassGrazed:
	case tov_PTShrubPastures:
	case tov_PTCorkOak:
	case tov_PTVineyards:
	case tov_DKPlantNursery_Perm:
	case tov_DKOrchardCrop_Perm:
	case tov_DKOrchApple:
	case tov_DKOrchPear:
	case tov_DKOrchCherry:
	case tov_DKOrchOther:
	case tov_DKOOrchApple:
	case tov_DKOOrchPear:
	case tov_DKOOrchCherry:
	case tov_DKOOrchOther:
	case tov_DEAsparagusEstablishedPlantation:
	case tov_DEOrchard:
	case tov_DEOAsparagusEstablishedPlantation:
	case tov_DEOOrchard:
	case tov_DKBushFruit_Perm1:
	case tov_DKBushFruit_Perm2:
	case tov_DEBushFruitPerm:
	case tov_DEOBushFruitPerm:
	case tov_DKChristmasTrees_Perm:
	case tov_DKOChristmasTrees_Perm:
	case tov_DKGrassTussocky_Perm:
	case tov_FINaturalGrassland_Perm:
		return true;
	default: // No matching code so is should not be permanent crop
		return false;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------


bool Landscape::ClassificationVegMaize(TTypesOfVegetation a_vege_type)
{
	/** Currently (18/03/2015) only used for goose foraging, so silage maize does not produce grain */
	switch (a_vege_type)
	{
	case tov_Maize:
	case tov_OMaizeSilage:
	case tov_MaizeSilage:
	case tov_PLMaize:
	case tov_PLMaizeSilage:
	case tov_NLMaize:
	case tov_NLMaizeSpring:
	case tov_UKMaize:
	case tov_DEMaize:
	case tov_DEMaizeSilage:
	case tov_DEOMaize:
	case tov_DEOMaizeSilage:
	case tov_PTMaize:
	case tov_PTMaize_Hort:
	case tov_DKMaize:
	case tov_DKMaizeSilage:
	case tov_DKOMaize:
	case tov_DKOMaizeSilage:

		return true;
	default: // No matching code so is should not be maize
		return false;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::ClassificationVegGooseGrass(TTypesOfVegetation a_vege_type)
{
	switch (a_vege_type)
	{
	case tov_NaturalGrass:
	case tov_PermanentGrassGrazed:
	case tov_PermanentGrassLowYield:
	case tov_PermanentGrassTussocky:
	case tov_NLPermanentGrassGrazed:
	case tov_NLPermanentGrassGrazedExtensive:
	case tov_SeedGrass1:
	case tov_SeedGrass2:
	case tov_OSeedGrass1:
	case tov_OSeedGrass2:
	case tov_CloverGrassGrazed1:
	case tov_CloverGrassGrazed2:
	case tov_OCloverGrassGrazed1:
	case tov_OCloverGrassGrazed2:
	case tov_DKOGrassLowYield_Perm:
	case tov_FodderGrass:
	case tov_PLFodderLucerne1:
	case tov_PLFodderLucerne2:
	case tov_NLGrassGrazed1:
	case tov_NLGrassGrazed1Spring:
	case tov_NLGrassGrazed2:
	case tov_NLGrassGrazedLast:
	case tov_NLGrassGrazedExtensive1:
	case tov_NLGrassGrazedExtensive1Spring:
	case tov_NLGrassGrazedExtensive2:
	case tov_NLGrassGrazedExtensiveLast:
	case tov_FIGrasslandPasturePerennial1:
	case tov_FIGrasslandPasturePerennial2:
	case tov_FIGrasslandSilagePerennial1:
	case tov_FIGrasslandSilagePerennial2:
	case tov_FINaturalGrassland:
	case tov_FINaturalGrassland_Perm:
	case tov_FIFeedingGround:
	case tov_FIBufferZone:
	case tov_FIBufferZone_Perm:
	case tov_FIGreenFallow_1year:
	case tov_FIGreenFallow_Perm:
	case tov_FIGrasslandSilageAnnual:
	case tov_DKGrassGrazed_Perm:
	case tov_DKGrassLowYield_Perm:
	case tov_DKGrassTussocky_Perm:
	case tov_UKPermanentGrass:
	case tov_UKTempGrass:
	case tov_PTRyegrass:
	case tov_PTYellowLupin:
	case tov_PTCloverGrassGrazed1:
	case tov_PTCloverGrassGrazed2:
	case tov_PTPermanentGrassGrazed:
	case tov_PTGrassGrazed:
	case tov_IRGrassland_no_reseed:
	case tov_IRGrassland_reseed:
	case tov_DEPermanentGrassGrazed:
	case tov_DEOPermanentGrassGrazed:
	case tov_DEPermanentGrassLowYield:
	case tov_DEOPermanentGrassLowYield:
	case tov_FRGrassland_Perm:
		return true;
	default:
		return false;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::ClassificationVegMatureCereal(TTypesOfVegetation a_vege_type)
{
	switch (a_vege_type)
	{   
	    case tov_SpringBarley:
		case tov_SpringBarleySpr:
		case tov_WinterBarley:
		case tov_SpringWheat:
		case tov_WinterWheat:
		case tov_WinterRye:
		case tov_Oats:
		case tov_Triticale:
		case tov_SpringBarleySeed:
		case tov_SpringBarleyStrigling:
		case tov_SpringBarleyStriglingSingle:
		case tov_SpringBarleyStriglingCulm:
		case tov_WinterWheatStrigling:
		case tov_WinterWheatStriglingSingle:
		case tov_WinterWheatStriglingCulm:
		case tov_OWinterBarley:
		case tov_OWinterBarleyExt:
		case tov_OWinterRye:
		case tov_SpringBarleyGrass:
		case tov_SpringBarleyCloverGrass:
		case tov_SpringBarleyPeaCloverGrassStrigling:
		case tov_OSpringBarley:
		case tov_OSpringBarleyPigs:
		case tov_OWinterWheatUndersown:
		case tov_OWinterWheat:
		case tov_OOats:
		case tov_OTriticale:
		case tov_WWheatPControl:
		case tov_WWheatPToxicControl:
		case tov_WWheatPTreatment:
		case tov_AgroChemIndustryCereal:
		case tov_SpringBarleyPTreatment:
		case tov_SpringBarleySKManagement:
		case tov_OSpringBarleyExt:
		case tov_OSpringBarleyGrass:
		case tov_OSpringBarleyClover:
		case tov_PLWinterWheat:
		case tov_PLWinterBarley:
		case tov_PLWinterRye:
		case tov_PLWinterTriticale:
		case tov_PLSpringWheat:
		case tov_PLSpringBarley:
		case tov_PLSpringBarleySpr:
		case tov_PLWinterWheatLate:
		case tov_NLSpringBarley:
		case tov_NLWinterWheat:
		case tov_NLSpringBarleySpring:
		case tov_DKWinterWheat:
		case tov_DKOWinterWheat:
		case tov_DKSpringBarley:
		case tov_DKOSpringBarley:
		case tov_DKWinterWheat_CC:
		case tov_DKOWinterWheat_CC:
		case tov_DKSpringBarley_CC:
		case tov_DKOSpringBarley_CC:
		case tov_DKSpringBarleyCloverGrass:
		case tov_DKOSpringBarleyCloverGrass:
		case tov_FIWinterWheat:
		case tov_FIOWinterWheat:
		case tov_FISpringWheat:
		case tov_FIOSpringWheat:
		case tov_FIWinterRye:
		case tov_FIOWinterRye:
		case tov_FISpringOats:
		case tov_FIOSpringOats:
		case tov_FISpringBarley_Malt:
		case tov_FIOSpringBarley_Malt:
		case tov_FISpringBarley_Fodder:
		case tov_FISprSpringBarley_Fodder:
		case tov_FIOSpringBarley_Fodder:
		case tov_SESpringBarley:
		case tov_SEWinterWheat:
		case tov_FRWinterWheat:
		case tov_FRWinterBarley:
		case tov_FRWinterTriticale:
		case tov_DKCerealLegume:
		case tov_DKOCerealLegume:
		case tov_DKCerealLegume_Whole:
		case tov_DKOCerealLegume_Whole:
		case tov_DKOSpringBarleySilage:
		case tov_DKOSpringOats:
		case tov_DKOSpringWheat:
		case tov_DKOWinterBarley:
		case tov_DKOWinterRape:
		case tov_DKOWinterRye:
		case tov_DKOWinterRye_CC:
		case tov_DKWinterRye_CC:
		case tov_DKSpringBarley_Green:
		case tov_DKSpringBarleySilage:
		case tov_DKSpringOats:
		case tov_DKSpringOats_CC:
		case tov_DKOSpringOats_CC:
		case tov_DKSpringWheat:
		case tov_DKWinterBarley:
		case tov_DKWinterRape:
		case tov_DKWinterRye:
		case tov_UKSpringBarley:
		case tov_UKWinterBarley:
		case tov_UKWinterWheat:
		case tov_DEWinterWheat:
		case tov_DEWinterWheatLate:
		case tov_DEWinterBarley:
		case tov_DEWinterRye:
		case tov_DETriticale:
		case tov_DEOats:
		case tov_DESpringRye:
		case tov_DEOWinterWheat:
		case tov_DEOWinterBarley:
		case tov_DEOWinterRye:
		case tov_DEOTriticale:
		case tov_DEOOats:
		case tov_DEOSpringRye:
		case tov_DESpringBarley:	
		case tov_PTOats:
		case tov_PTWinterWheat:
		case tov_PTSorghum:
		case tov_PTTriticale:
		case tov_PTWinterBarley:
		case tov_PTWinterRye:
		case tov_IRSpringWheat:
		case tov_IRSpringBarley:
		case tov_IRSpringOats:
		case tov_IRWinterWheat:
		case tov_IRWinterBarley:
		case tov_IRWinterOats:
			return true;
		default:
			return false;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::ClassificationVegGrass(TTypesOfVegetation a_vege_type)
{
	switch (a_vege_type)
	{
	case tov_NaturalGrass:
	case tov_PermanentGrassGrazed:
	case tov_NLPermanentGrassGrazed:
	case tov_NLPermanentGrassGrazedExtensive:
	case tov_UKPermanentGrass:
	case tov_PermanentGrassLowYield:
	case tov_PermanentGrassTussocky:
	case tov_PermanentSetAside:
	case tov_SetAside:
	case tov_OSetAside:
	case tov_SeedGrass1:
	case tov_SeedGrass2:
	case tov_OSeedGrass1:
	case tov_OSeedGrass2:
	case tov_CloverGrassGrazed1:
	case tov_CloverGrassGrazed2:
	case tov_OCloverGrassGrazed1:
	case tov_OCloverGrassGrazed2:
	case tov_OrchardCrop:
	case tov_NLOrchardCrop:
	case tov_YoungForest:
	case tov_FodderGrass:
	case tov_Heath:
	case tov_WaterBufferZone:
	case tov_PTPermanentGrassGrazed:
	case tov_PTCloverGrassGrazed1:
	case tov_PTCloverGrassGrazed2:
	case tov_PTCorkOak:
	case tov_PTVineyards:
	case tov_PTRyegrass:
	case tov_PTYellowLupin:
	case tov_DKGrazingPigs_Perm:
	case tov_DKOGrazingPigs_Perm:
	case tov_DKOGrassGrazed_Perm:
	case tov_DKOGrassLowYield_Perm:
	case tov_DKGrassTussocky_Perm:
	case tov_FIGrasslandPasturePerennial1:
	case tov_FIGrasslandPasturePerennial2:
	case tov_FIGrasslandSilagePerennial1:
	case tov_FIGrasslandSilagePerennial2:
	case tov_FINaturalGrassland:
	case tov_FINaturalGrassland_Perm:
	case tov_FIFeedingGround:
	case tov_FIBufferZone:
	case tov_FIBufferZone_Perm:
	case tov_FIGreenFallow_1year:
	case tov_FIGreenFallow_Perm:
	case tov_FIGrasslandSilageAnnual:
	case tov_DKPlantNursery_Perm:
	case tov_DKOrchardCrop_Perm:
	case tov_DKGrassGrazed_Perm:
	case tov_DKGrassLowYield_Perm:
	case tov_DKSeedGrassFescue_Spring:
	case tov_DKSeedGrassRye_Spring:
	case tov_DKOSeedGrassRye_Spring:
	case tov_DKWinterCloverGrassGrazedSown:
	case tov_DKCloverGrassGrazed1:
	case tov_DKCloverGrassGrazed2:
	case tov_DKCloverGrassGrazed3:
	case tov_DKOWinterCloverGrassGrazedSown:
	case tov_DKOCloverGrassGrazed1:
	case tov_DKOCloverGrassGrazed2:
	case tov_DKOCloverGrassGrazed3:
	case tov_DKOVegSeeds:
	case tov_DKGrazingPigs:
	case tov_DKOGrazingPigs:
	case tov_DKSpringFodderGrass:
	case tov_DKWinterFodderGrass:
	case tov_DKOWinterFodderGrass:
	case tov_DKOSpringFodderGrass:
	case tov_DKSetAside:
	case tov_DKSetAside_SummerMow:
	case tov_DEPermanentGrassGrazed:
	case tov_DEPermanentGrassLowYield:
	case tov_DEOPermanentGrassLowYield:
	case tov_DEOPermanentGrassGrazed:
	case tov_DEGrasslandSilageAnnual:
	case tov_DEOGrasslandSilageAnnual:
	case tov_DEGreenFallow_1year:
	case tov_DEOGreenFallow_1year:
	case tov_DEOrchard:
	case tov_DEOOrchard:
	case tov_DKOSetAside:
	case tov_DKOSetAside_SummerMow:
	case tov_DKOrchApple:
	case tov_DKOrchPear:
	case tov_DKOrchCherry:
	case tov_DKOrchOther:
	case tov_DKOOrchApple:
	case tov_DKOOrchPear:
	case tov_DKOOrchCherry:
	case tov_DKOOrchOther:
	case tov_FRGrassland_Perm:
	case tov_DKOptimalFlowerMix1:
	case tov_DKOptimalFlowerMix2:
	case tov_DKOptimalFlowerMix3:
		return true;
	default: 
		return false;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::ClassificationVegCereal(TTypesOfVegetation a_vege_type)
{
	switch (a_vege_type)
	{
	case tov_SpringBarley:
	case tov_SpringBarleySpr:
	case tov_WinterBarley:
	case tov_SpringWheat:
	case tov_WinterWheat:
	case tov_WinterRye:
	case tov_Oats:
	case tov_Triticale:
	case tov_SpringBarleySeed:
	case tov_SpringBarleyStrigling:
	case tov_SpringBarleyStriglingSingle:
	case tov_SpringBarleyStriglingCulm:
	case tov_WinterWheatStrigling:
	case tov_WinterWheatStriglingSingle:
	case tov_WinterWheatStriglingCulm:
	case tov_OWinterBarley:
	case tov_OWinterBarleyExt:
	case tov_OWinterRye:
	case tov_SpringBarleyGrass:
	case tov_SpringBarleyCloverGrass:
	case tov_SpringBarleyPeaCloverGrassStrigling:
	case tov_OSpringBarley:
	case tov_OSpringBarleyPigs:
	case tov_OWinterWheatUndersown:
	case tov_OWinterWheat:
	case tov_OOats:
	case tov_OTriticale:
	case tov_WWheatPControl:
	case tov_WWheatPToxicControl:
	case tov_WWheatPTreatment:
	case tov_AgroChemIndustryCereal:
	case tov_SpringBarleyPTreatment:
	case tov_SpringBarleySKManagement:
	case tov_OSpringBarleyExt:
	case tov_OSpringBarleyGrass:
	case tov_OSpringBarleyClover:
	case tov_PLWinterWheat:
	case tov_PLWinterBarley:
	case tov_PLWinterRye:
	case tov_PLWinterTriticale:
	case tov_PLSpringWheat:
	case tov_PLSpringBarley:
	case tov_NLWinterWheat:
	case tov_NLSpringBarley:
	case tov_PLSpringBarleySpr:
	case tov_PLWinterWheatLate:
	case tov_PLMaize:
	case tov_PLMaizeSilage:
	case tov_Maize:
	case tov_MaizeSilage:
	case tov_MaizeStrigling:
	case tov_NLMaize:
	case tov_NLSpringBarleySpring:
	case tov_NLMaizeSpring:
	case tov_OBarleyPeaCloverGrass:
	case tov_OSBarleySilage:
	case tov_DKWinterWheat:
	case tov_DKOWinterWheat:
	case tov_DKWinterWheat_CC:
	case tov_DKOWinterWheat_CC:
	case tov_DKSpringBarley:
	case tov_DKOSpringBarley:
	case tov_DKSpringBarley_CC:
	case tov_DKOSpringBarley_CC:
	case tov_DKSpringBarleyCloverGrass:
	case tov_DKOSpringBarleyCloverGrass:
	case tov_FIWinterWheat:
	case tov_FIOWinterWheat:
	case tov_FISpringWheat:
	case tov_FIOSpringWheat:
	case tov_FIWinterRye:
	case tov_FIOWinterRye:
	case tov_FISpringOats:
	case tov_FIOSpringOats:
	case tov_FISpringBarley_Malt:
	case tov_FIOSpringBarley_Malt:
	case tov_FISpringBarley_Fodder:
	case tov_FISprSpringBarley_Fodder:
	case tov_FIOSpringBarley_Fodder:
	case tov_SESpringBarley:
	case tov_SEWinterWheat:
	case tov_FRWinterWheat:
	case tov_FRWinterBarley:
	case tov_FRWinterTriticale:
	case tov_DKCerealLegume:
	case tov_DKOCerealLegume:
	case tov_DKCerealLegume_Whole:
	case tov_DKOCerealLegume_Whole:
	case tov_DKOSpringBarleySilage:
	case tov_DKOSpringOats:
	case tov_DKOSpringOats_CC:
	case tov_DKOSpringWheat:
	case tov_DKOWinterBarley:
	case tov_DKOWinterRape:
	case tov_DKOWinterRye:
	case tov_DKOWinterRye_CC:
	case tov_DKSpringBarley_Green:
	case tov_DKSpringBarleySilage:
	case tov_DKSpringOats:
	case tov_DKSpringOats_CC:
	case tov_DKSpringWheat:
	case tov_DKWinterBarley:
	case tov_DKWinterRape:
	case tov_DKWinterRye:
	case tov_DKWinterRye_CC:
	case tov_UKMaize:
	case tov_UKSpringBarley:
	case tov_UKWinterBarley:
	case tov_UKWinterWheat:
	case tov_DEWinterWheat:
	case tov_DEWinterWheatLate:
	case tov_DEOats:
	case tov_DESpringRye:
	case tov_DEMaize:
	case tov_DEMaizeSilage:
	case tov_DEWinterRye:
	case tov_DEWinterBarley:
	case tov_DETriticale:
	case tov_DEOWinterWheat:
	case tov_DEOOats:
	case tov_DEOMaize:
	case tov_DEOMaizeSilage:
	case tov_DEOWinterRye:
	case tov_DEOWinterBarley:
	case tov_DEOTriticale:
	case tov_DESpringBarley:
	case tov_PTMaize:
	case tov_PTMaize_Hort:
	case tov_PTOats:
	case tov_PTWinterWheat:
	case tov_PTSorghum:
	case tov_PTTriticale:
	case tov_PTWinterBarley:
	case tov_PTWinterRye:
	case tov_IRSpringWheat:
	case tov_IRSpringBarley:
	case tov_IRSpringOats:
	case tov_IRWinterWheat:
	case tov_IRWinterBarley:
	case tov_IRWinterOats:
		return true;
	default: return false;
    }
}
//-----------------------------------------------------------------------------------------------------------------------------

// All TOLEs that admit and require an owner information 
bool Landscape::ClassificationFieldType(TTypesOfLandscapeElement a_tole) {
	switch (a_tole)
	{
    // Annual rotational field, with varying crops on it 
	case tole_Field:
	// (semi-)permanent managed crops:
	case tole_Orchard:
	case tole_PermanentSetaside:
	case tole_PermPasture:
	case tole_PermPastureLowYield:
	case tole_PermPastureTussocky:
	case tole_PermPastureTussockyWet: // tole NOT implemented yet 
	// case tole_Vildtager:
	case tole_YoungForest:
	case tole_WoodyEnergyCrop:
	// new (2021):
	case tole_PlantNursery:
	case tole_MontadoCorkOak:
	case tole_MontadoHolmOak:
	case tole_MontadoMixed:
	case tole_AgroForestrySystem:
	case tole_CorkOakForest:
	case tole_HolmOakForest:
	case tole_OtherOakForest:
	case tole_ChestnutForest:
	case tole_EucalyptusForest:
	case tole_MaritimePineForest:
	case tole_StonePineForest:
	case tole_Vineyard:
	case tole_OliveGrove:
	case tole_RiceField:
	// new (2021 DK Perm Crops):
	case tole_OOrchard:
	case tole_BushFruit:
	case tole_OBushFruit:
	case tole_ChristmasTrees:
	case tole_OChristmasTrees:
	case tole_EnergyCrop:
	case tole_OEnergyCrop:
	case tole_FarmForest:
	case tole_OFarmForest:
	case tole_PermPasturePigs:
	case tole_OPermPasturePigs:
	case tole_OPermPasture:
	case tole_OPermPastureLowYield:
	case tole_FarmYoungForest:
	case tole_OFarmYoungForest:
	// new (2021 PT Perm Crops):
	case tole_AlmondPlantation:
	case tole_WalnutPlantation:
	// new (2021 FI Perm crops):
	case tole_FarmBufferZone:
	case tole_NaturalFarmGrass:
	case tole_GreenFallow:
	case tole_FarmFeedingGround:
	// new (2021 DE Perm Crops):
	case tole_FlowersPerm:
	case tole_AsparagusPerm:
	case tole_OAsparagusPerm:
	case tole_MushroomPerm:
	case tole_OtherPermCrop:
		return true;
	default:
		return false;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

// All TOLEs that in general can be expected "to have height over 2m" 
bool Landscape::ClassificationHigh(TTypesOfLandscapeElement tole){

	if (ClassificationForest(tole))
		return true;

	switch (tole) {
	// NonVegetated
	case tole_BuiltUpWithParkland:
	case tole_PitDisused:
	case tole_UrbanNoVeg:
	case tole_UrbanVeg:
	case tole_Garden:
	case tole_StoneWall:
	case tole_WindTurbine:
	case tole_Pylon:
	case tole_ActivePit:
	case tole_Building:
	// Vegetated
	case tole_RiversideTrees:
	case tole_PlantNursery:
	case tole_Hedges:
	// orchards and orchard bands
	case tole_WoodyEnergyCrop:
	case tole_Orchard:
	case tole_OrchardBand:
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------

// All TOLEs that are waterbodies
bool Landscape::ClassificationWater(TTypesOfLandscapeElement tole) {
	
	switch (tole) {
	case tole_Saltwater:
	case tole_River:
	case tole_Pond:
	case tole_RiverBed:
	case tole_Freshwater:
	case tole_Canal:
	case tole_Stream:
	case tole_DrainageDitch:
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------

// All TOLEs that artificial man-made landscapes with little to no "real" vegetation  
bool Landscape::ClassificationUrbanNoVeg(TTypesOfLandscapeElement tole) {

	switch (tole) {
	case tole_UrbanNoVeg:
	case tole_Airport:
	case tole_Portarea:
	case tole_Carpark:
	case tole_SmallRoad:
	case tole_LargeRoad:
	case tole_RefuseSite:
	case tole_ActivePit:
	case tole_MetalledPath:
	case tole_Building:
	case tole_AmenityGrass:
	case tole_MownGrassStrip:
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::ClassificationForest(TTypesOfLandscapeElement tole) {

	switch (tole) {
	case tole_WoodyEnergyCrop:
	case tole_DeciduousForest:
	case tole_MixedForest:
	case tole_ConiferousForest:
	case tole_Copse:
	case tole_IndividualTree:
	case tole_WoodlandMargin:
	case tole_Scrub:
	case tole_SwampForest:
	case tole_MontadoCorkOak:
	case tole_MontadoHolmOak:
	case tole_MontadoMixed:
	case tole_AgroForestrySystem:
	case tole_CorkOakForest:
	case tole_HolmOakForest:
	case tole_OtherOakForest:
	case tole_ChestnutForest:
	case tole_EucalyptusForest:
	case tole_InvasiveForest:
	case tole_MaritimePineForest:
	case tole_StonePineForest:
	case tole_YoungForest:
	case tole_RiversideTrees:
	case tole_FarmForest:
	case tole_OFarmForest:
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::ClassificationWoody(TTypesOfLandscapeElement tole) {

	if (ClassificationForest(tole))
		return true;

	switch (tole) {
	case tole_Hedges:
	case tole_HedgeBank:
	case tole_Vineyard:
	case tole_OliveGrove:
	case tole_Orchard:
	case tole_OOrchard:
	case tole_ChristmasTrees:
	case tole_OChristmasTrees:
	case tole_FarmYoungForest:
	case tole_OFarmYoungForest:
	case tole_OrchardBand:
	case tole_ForestAisle:
	case tole_WoodyEnergyCrop:
	case tole_AlmondPlantation:
	case tole_WalnutPlantation:
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::SubtractPondLarvalFood(double a_food,int a_polyrefindex) {
	return dynamic_cast<Pond*>(m_elems[a_polyrefindex])->SubtractLarvalFood(a_food);
}
//-----------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------

GooseFieldList* Landscape::GetGooseFields(double a_minopenness)
{
	/**
	* Here we need to go through all possible goose feeding locations to find out if they have any forage in them, and then
	* create a list of those to return. \n
	*
	* To make this efficient we need to have a list of fields.
	*/
	GooseFieldList* alist = new GooseFieldList;
	/**
	* First must calculate centroid. Runs through the list of elements and any that have an openness score bigger than our target are saved.
	*/
	GooseFieldListItem gfli;
	for (unsigned int i = 0; i < m_elems.size(); i++)
	{
		if (m_elems[i]->GetOpenness() > a_minopenness)
		{
			for (int g = gs_Pinkfoot; g < gs_foobar; g++)
			{
				gfli.grass[g] = m_elems[i]->GetGooseGrazingForage((GooseSpecies)g);
				gfli.geesesp[g] = m_elems[i]->GetGooseSpNosToday((GooseSpecies)g);
				gfli.geesespTimed[g] = m_elems[i]->GetGooseSpNosTodayTimed((GooseSpecies)g);
				gfli.roostdists[g] = m_elems[i]->GetGooseRoostDist((GooseSpecies)g);
			}
			gfli.grain = m_elems[i]->GetBirdSeed();
			gfli.maize = m_elems[ i ]->GetBirdMaize(); 
			gfli.openness = m_elems[ i ]->GetOpenness();
			int pref = m_elems[ i ]->GetPoly();
			gfli.polyref = pref;
			gfli.geese = m_elems[i]->GetGooseNosToday();
			gfli.geeseTimed = m_elems[i]->GetGooseNosTodayTimed();
			gfli.vegtype = m_elems[i]->GetVegType();
			gfli.vegtypechr = VegtypeToString(m_elems[i]->GetVegType());
			gfli.vegheight = m_elems[i]->GetVegHeight();
			gfli.digestability = m_elems[i]->GetDigestibility();
			gfli.vegphase = m_elems[i]->GetVegPhase();
			gfli.previouscrop = VegtypeToString( m_elems[ i ]->GetPreviousTov( m_elems[ i ]->GetRotIndex() ) );
			gfli.lastsownveg = VegtypeToString( m_elems[ i ]->GetLastSownVeg() );
			alist->push_back(gfli);
		}
	}
	return alist;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::ResetGrainAndMaize()
{
	/**
	* To avoid grain from previous years sticking onto fields and confusing the
	* goose foraging model, we make a hard reset of everything.
	*/
	for (unsigned int i = 0; i < m_elems.size(); i++)
	{
		m_elems[i]->SetBirdSeed(0.0);
		m_elems[i]->SetBirdMaize(0.0);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

/**
Landscape::DistanceToP - calculates distance from 2 coordinates to other 2 coordinates.
*/
int Landscape::DistanceToP(APoint a_Here, APoint a_There)
{
	int x = (a_Here.m_x - a_There.m_x);
	int y = (a_Here.m_y - a_There.m_y);
	if (x < 0) x = x * -1;
	if (y < 0) y = y * -1;
	if (x > (m_width / 2))  x = m_width - x;
	if (y > (m_height / 2))  y = m_height - y;
	if ((x != 0) || (y != 0))
	{ 
		return (int)(sqrt(double((x * x) + (y * y))));
	}
	else return 0; // in the case where object is standing on a_There
}
//---------------------------------------------------------------------------
/**
Landscape::DistanceToPSquared - calculates distance from 2 coordinates to other 2 coordinates but do not square root (for efficiency).
*/
int Landscape::DistanceToPSquared(APoint a_Here, APoint a_There)
{
	int x = (a_Here.m_x - a_There.m_x);
	int y = (a_Here.m_y - a_There.m_y);
	if (x < 0) x = x * -1;
	if (y < 0) y = y * -1;
	if (x > (m_width / 2))  x = m_width - x;
	if (y > (m_height / 2))  y = m_height - y;
	if ((x != 0) || (y != 0))
	{ 
		return ((x * x) + (y * y));
	}
	else return 0; // in the case where object is standing on a_There
}
//---------------------------------------------------------------------------
double Landscape::SupplyFarmIntensityI( int a_polyindex ) {
	return m_elems[ a_polyindex ]->GetOwner()->GetIntensity();
}
//-----------------------------------------------------------------------------------------------------------------------------

double Landscape::SupplyFarmIntensity( int a_x, int a_y ) {
	return m_elems[ m_land->Get( a_x, a_y ) ]->GetOwner()->GetIntensity();
}
//-----------------------------------------------------------------------------------------------------------------------------

double Landscape::SupplyFarmIntensity(int a_polyref) {
	Farm* fptr = m_elems[m_polymapping[a_polyref]]->GetOwner();
	if (fptr != nullptr) return fptr->GetIntensity();
	return 0.0;
}
//-----------------------------------------------------------------------------------------------------------------------------

APoint Landscape::SupplyCentroid( int a_polyref ) {
	return m_elems[ m_polymapping[ a_polyref ] ]->GetCentroid();
}
//-----------------------------------------------------------------------------------------------------------------------------

APoint Landscape::SupplyCentroidIndex( int a_polyrefindex ) {
	return m_elems[ a_polyrefindex ]->GetCentroid();
}


//-----------------------------------------------------------------------------------------------------------------------------

polylist* Landscape::SupplyLargeOpenFieldsNearXY(int a_x, int a_y, int a_range, int a_openness)
{
	/**
	* There is a limit of 1000 polygons to return. There are probably more speed efficient ways to do this, but the simplest is
	* to simply trawl through the list of polygons and pull out each polygon with an openness score > a_openness - then see if the
	* centroid is within range.
	* NB calling method must delete the polylist* passed back!
	*
	* 18/12/2013 this method has been rendered obselete by changes to hunters referring to famers rather than a co-ordinate and range.
	*/
	polylist* p_list = new polylist;
	unsigned sz = (unsigned) m_elems.size();
	for (unsigned i=0; i<sz; i++)
	{
		if ( m_elems[i]->GetOpenness() > a_openness)
		{
			APoint pt = m_elems[i]->GetCentroid();
			int dx, dy;
			if (a_x>pt.m_x) dx = a_x-pt.m_x; else dx = pt.m_x-a_x;
			if (a_y>pt.m_y) dy = a_y-pt.m_y; else dy = pt.m_y-a_y;
			dx++; dy++; // prevents crash with maths below.
		    /**  Calculates distance from location a_x, a_y. This is done using an approximation to pythagorus to avoid a speed problem. */
			int dist;
			/** NB this will crash if either a_dy or a_dx are zero! */
			if (dx>dy) dist = dx + (dy * dy) /(2 * dx); else dist = dy + (dx * dx) /(2 * dy);
			if (dist<=a_range) p_list->push_back( m_elems[i]->GetPoly());
		}
	}
	return p_list;
}
//-----------------------------------------------------------------------------------------------------------------------------

Landscape::Landscape( bool dump_exit ) {
/**
* The landscape constructor sets up all the mapping, environment and management for the landscape simulation.
*/
	// Set up operation flags
	bool didRenumber = false;
	bool didCalcCentroids = false;
	bool didConsolidate = false;
	bool didCalcOpenness = false;
	bool didCalcOther = false;
	m_NeedCentroidCalculation = false;
	m_NeedOpennessCalculation = false;
	m_DoMissingPolygonsManipulations = false;
	// Set up globals
	g_landscape_ptr = this;
	for (int i = 1; i <= 2000; i++) {
		g_speedy_Divides[i] = 1 / double(i);
	}
	// The following arrays correspond to:
	// 0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, 180, 202.5, 225, 247.5, 270, 292.5, 315, 337.5
	// E,  ENE, NE,  NNE,  N,   NNW,  NW,   WNW,   W,   WSW,  SW,   SSW,   S,   SSE,  SE,   ESE 
	int x_add[16] = { 1, 2, 1, 1, 0, -1, -1, -2, -1, -2, -1, -1, 0, 1, 1, 2 };
	int y_add[16] = { 0, -1, -1, -2, -1, -2, -1, -1, 0, 1, 1, 2, 1, 2, 1, 1 };
	for (int i = 0; i < 16; i++) {
		m_x_add[i] = x_add[i];
		m_y_add[i] = y_add[i];
	}
	
	sprintf(m_versioninfo, "%d.%d.%d :: %s", version_major, version_minor, version_revision, version_date);

	if (l_map_print_version_info.value()) {
		cout << "This program uses the Landscape simulator " <<  m_versioninfo << endl;
	}
#ifdef __GITVERSION
	if (l_map_print_git_version_info.value())
	{
		std::cout << "hash=" << GIT_HASH << ", time=" << COMPILE_TIME << ", branch=" << GIT_BRANCH << std::endl;
	}
#endif


	// For creating a list of all cfg symbols when needed.
	//g_cfg->DumpPublicSymbols( "publicsymbols.cfg", CFG_PUBLIC );
	//exit(1);

	m_LargestPolyNumUsed = -1;
	m_pestincidencefactor = 1.0;

	// Reset treatment counters.
	for (int i = 0; i < last_treatment; i++) {
		m_treatment_counts[i] = 0;
	}
	// Due to difficulties with ordering of events here we take a sneak peek at the polyref file
	cout << "Setting country and lat/long " << l_map_poly_file.value() << endl;
	int format = ReadPolys1(l_map_poly_file.value());

	cout << "Creating FarmManager Object" << endl;
	m_FarmManager = new FarmManager(this);

	// Set the vegetation type biomass scalers
	for (int v = 0; v < tov_Undefined; v++) { VegElement::SetBiomassScalers(static_cast<TTypesOfVegetation>(v)); }
	cout << "Reading polygon reference file " << l_map_poly_file.value() << endl;
	ReadPolys2(l_map_poly_file.value(), format); // lct: creates LE ( Landscape->m_elems[i] ) associated to each polygon, and associted field polygons to farm owners, and sets flags for borders and unsprayed margin processing

	cout << "Creating RasterMap Object from file " << l_map_map_file.value() << endl;
	m_land = new RasterMap(l_map_map_file.value(), this);
	m_width = m_land->MapWidth();
	m_height = m_land->MapHeight();
	m_area = m_width * m_height;
	if (m_width > m_height) m_maxextent = m_width; else m_maxextent = m_height;
	m_width10 = 10 * m_width;
	m_height10 = 10 * m_height;
	if (m_width < m_height) m_minmaxextent = m_width; else m_minmaxextent = m_height;

	m_PestIncidenceManager = new PestIncidenceManager(cfg_PestIncidenceMax.value(), cfg_PestIncidenceMin.value() );
	// Validate polygons, ie. ensure those reference in the
	// polygon file also shows up in the map.
	cout << "In PolysValidate" << endl;
	PolysValidate(false);
	cout << "In PolysRemoveInvalid" << endl;
	PolysRemoveInvalid();
	cout << "Creating ponds" << endl;
	if (cfg_MaxPondSize.value() > 0) {
		// This takes any small freshwater and converts it to a pond.
		for (unsigned int i = 0; i < m_elems.size(); i++) {
			if (m_elems[i]->GetElementType() == tole_Freshwater) {
				if (m_elems[i]->GetArea() <= cfg_MaxPondSize.value()) {
					Pond* pond = new Pond(this);
					pond->DoCopy(m_elems[i]);
					pond->SetALMaSSEleType(g_letype->BackTranslateEleTypes(tole_Pond));
					pond->SetElementType(tole_Pond);
					// lct: We  need to free the previous memory , then link to new element
					delete m_elems[i];
					m_elems[i] = dynamic_cast<LE*>(pond);
				}
			}
		}
	}
	cout << "In PolysValidate second time" << endl;
	PolysValidate(true);

	/**
	  * Next job after checking the basic map validity is to deal with any missing polygon information
	  * This is done by identifying the polygons that are large contiguous areas and making them wasteland. The rest
	  * are left and nibbled away to join adjacent polygons. This is a time consuming operation so is only done if
	  * there have been any missing info polygons found.*/
	if (m_DoMissingPolygonsManipulations)
	{
		cout << "In DoMissingPolygonsManipulations" << endl;
		// Find big continuous polygons
		for (unsigned int i = 0; i < m_elems.size(); i++)
		{
			if (m_elems[i]->GetElementType() == tole_Missing)
			{
				double area = m_elems[i]->GetArea();
				int areaMinrect = (m_elems[i]->GetMaxX() - m_elems[i]->GetMinX()) * (m_elems[i]->GetMaxY() - m_elems[i]->GetMinY());
				if ((areaMinrect / area > 4) || (area < 1000))
				{
					// Unlikely to be a field, or if so a very narrow odd one. We will assume this is a missing data issue.
				}
				else
				{
					// Big poly with more than 25% of the included rectangle covered, must be a field of some sort.
					// create a new wasteland and swap this in to the m_elems, then delete the old missing polygon
					LE * wl = NewElement(tole_Wasteland);
					wl->SetPoly(m_elems[i]->GetPoly());
					wl->SetArea(floor(0.5 + area)); // lct: To Remove This was already rounded wl->SetArea(area) should be enough
					wl->SetSoilType(m_elems[i]->GetSoilType());
					wl->SetUnsprayedMarginPolyRef(-1);
					wl->SetCentroid(-1, -1);
					wl->SetOpenness(0);
					delete m_elems[i];
					m_elems[i] = wl;
				}
			}
		}
		// By here all the big ones should be safely tidied away to wasteland and now we need to deal with the raster map.
		RemoveMissingValues();
		for (unsigned int i = 0; i < m_elems.size(); i++)
		{
			// Now we deal with all the little ones that were not by fields
			if (m_elems[i]->GetElementType() == tole_Missing)
			{
				LE * wl = NewElement(tole_Wasteland);
				wl->SetPoly(m_elems[i]->GetPoly());
				wl->SetArea(m_elems[i]->GetArea());
				wl->SetSoilType(m_elems[i]->GetSoilType());
				wl->SetUnsprayedMarginPolyRef(-1);
				wl->SetCentroid(-1, -1);
				wl->SetOpenness(0);
				delete m_elems[i];
				m_elems[i] = wl;
			}
		}
		cout << "In PolysValidate third time" << endl;
		PolysValidate(false);
		if (PolysRemoveInvalid()) {
			cout << "In PolysValidate fourth time" << endl;
			PolysValidate(true);
		}
		g_msg->Warn("Landscape::Landscape(): Dump and normal exit to follow after resolving missing polygons. ", "");
		didCalcOther = true;
	}

	// ChangeMapMapping() also enters a valid starting
	// coordinate for the border generating farm method below.
	cout << "In ChangeMapMapping" << endl;
	ChangeMapMapping();

	/**
	* To be sure we have enough space to do map manipulations if required, then the polygons need to be renumbered - unless they are done already.
	*/
	if ((m_LargestPolyNumUsed != ((int)m_elems.size() - 1)))
	{
		cout << "In poly renumber" << endl;

		PolysRenumber();
		didRenumber = true;
	}
	// do we want to remove small polygons?
	if (l_map_removesmallpolygons.value())
	{
		cout << "In Landscape::Landscape() Small polygon removal" << endl;
		int removed = RemoveSmallPolygons();
		g_msg->Warn("Landscape::Landscape(): Dump and normal exit to follow after removing small polygons and map dump. Polygons removed:", removed);
		didCalcOther = true;
	}
	// Do we want to re-write the current files and consolidate polys?
	else if (l_map_consolidatepolys.value())
	{
		cout << "In consolidate polys" << endl;
		didConsolidate = true;
		ConsolidatePolys();
	}
	else if (g_map_le_borderremoval.value())
	{
		cout << "In map_le_borderremoval" << endl;
		// Does not use centroids so is safe to use here
		BorderRemoval();
		CountMapSquares();
		ForceArea();
		g_msg->Warn(WARN_FILE, "Landscape::Landscape() - BorderRemoval "" map dump to follow.", "");
		didCalcOther = true;
	}


	// By the time we reach this point we need to have completed all major polygon removal and are ready to calculate centroids if they are needed. There
	// are two reasons for this - 1) that we did not have them in the original polyref file, 2) that we changed the map
	if (didConsolidate || didCalcOther || m_NeedCentroidCalculation)
	{
		PolysValidate(false);
		if (PolysRemoveInvalid()) PolysValidate(true);
		ChangeMapMapping();
		PolysRenumber();
		CalculateCentroids();
		didCalcCentroids = true;
	}
	if (didConsolidate || didCalcOther || m_NeedCentroidCalculation || didCalcCentroids || l_map_calc_openness.value())
	{
		if (l_map_calc_openness.value()) CalculateOpenness(true);
		else CalculateOpenness(false);
		didCalcOpenness = true;
	}
	if (didCalcCentroids || didConsolidate || didCalcOpenness || didCalcOther || m_NeedCentroidCalculation || didRenumber || !m_FarmManager->GetIsRenumbered())
	{
		// We need to dump the map and polyrefs
		string filestr = l_map_map_file.value();
		std::size_t ind = filestr.find(".lsb"); // Find the starting position of substring in the string
		if (ind != std::string::npos) {
			filestr.erase(ind, 4); // erase function takes two parameter, the starting index in the string from where you want to erase characters and total no of characters you want to erase.
			std::cout << filestr << "\n";
		}
		string dumpmap = filestr + "_dump.lsb";
		string farmstr = filestr + "_farmref_dump.txt";
		string polystr = filestr + "_polyref_dump.txt";

		m_FarmManager->DumpFarmrefs(farmstr.c_str());
		DumpMap(dumpmap.c_str());
		PolysDump(polystr.c_str());
		if (dump_exit) {
			g_msg->Warn(WARN_FILE, "Landscape::Landscape() ""Normal exit after dump.", "Remember to rename the new map and polyref file.");
			exit(0);
		}
		else {
			g_msg->Warn(WARN_FILE, "Landscape::Landscape() ""Dump file done", "Remember to rename the new map and polyref file.");			
		}
	}

	/**
	* After the main structure is created the constructor can carry out optional more complex manipulations.
	* These are adding hedgebanks, field boundaries, softening orchard boundaries, adding unsprayed margins, and adding beetlebanks. 
	*/

	// Below here we have the more complicated map manipulations. These will need recalculation of centroids and openness after they are run.
	// However, we really do not want to get here with invalid centroids, hence forced dump and exit for manipulations up to this point.

	didCalcOther = false;
	// Add artificial hedgebanks to the hedges in the landscape,
	// if requested.
	if (l_map_art_hedgebanks.value()) {
		hb_Add();
		didCalcOther = true;
	}
	else if (g_map_le_borders.value())  // Can't create borders and hedgerows at the same time, so independently checked
	{
		cout << "Generating LE Borders around fields" << endl;
		cout << "Border chance = " << g_map_le_border_chance.value() << endl;
		cout << "Border width = " << g_map_le_borderwidth.value() << endl;
		cout << "Adding Borders" << endl;

		// Keep track of how much area the borders take up per farm
		std::vector<int> border_area_per_farm(m_FarmManager->GetNoFarms(), 0);
		std::vector<int> total_area_per_farm;
		for (int f = 0; f < m_FarmManager->GetNoFarms(); f++) {
			total_area_per_farm.push_back(m_FarmManager->GetFarmTotalSize(f));
		}
		unsigned sz = (unsigned)m_elems.size();
		int begin = 1;
		// If the flag is set to true, we first try to limit the border to one side of the field (begin == 0)
		// If not we only run through the fields once and add borders all around the edge  (begin == 1)
		if (g_map_le_border_limited.value()) {
			begin = 0;
		}
		for (int allAround = begin; allAround <= 1; allAround++){
			int Nborders = 0;
			for (unsigned i = 0; i < sz; i++) {
				if (m_elems[i]->GetBorder() != NULL || (allAround == begin && m_elems[i]->GetOwner() != NULL && g_random_fnc(100) < g_map_le_border_chance.value())){
					// Border around this element, so must be a farm field.
					// If the field is too small then ignore it
					if (m_elems[i]->GetArea() > g_map_le_borders_min_field_size.value()){
						int farm_id = m_elems[i]->GetOwnerIndex();
						
						// Check if farm already has enough border area
						if (border_area_per_farm[farm_id] < total_area_per_farm[farm_id] * cfg_FarmAreaPercentage.value() * 0.01) {
							TTypesOfLandscapeElement  t = g_letype->TranslateEleTypes(g_map_le_borderstype.value());
							int area_temp = BorderAdd(m_elems[i], t, allAround);
							if (area_temp > 0) {
								Nborders++;
								border_area_per_farm[farm_id] += area_temp;
							}
						} 
					}
				}
			}
			g_msg->Warn(WARN_MSG, "Landscape::Landscape(): In border adding iteration: ", allAround);
			g_msg->WarnAddInfo(WARN_MSG, "Landscape::Landscape(): Fields with border successfully added: ", Nborders);
		}
		didCalcOther = true;
	}
	else   // Some special code to 'soften' the edges of orchards  
		if (g_map_orchards_borders.value())
		{
			// Generate border around each *orchard* landscape element.
			for (unsigned int i = 0; i < m_elems.size(); i++) {
				if (m_elems[i]->GetElementType() == tole_Orchard)
				{
					TTypesOfLandscapeElement  t = g_letype->TranslateEleTypes(g_map_le_borderstype.value());
					int area = BorderAdd(m_elems[i], t);
				}
			}
			didCalcOther = true;
		}
		else
			// Unsprayed Margin Code....
			if (g_map_le_unsprayedmargins.value())
			{
				cout << "Adding unsprayed margins" << endl;

				// Generate border around each *farm* landscape element.
				for (unsigned int i = 0; i < m_elems.size(); i++)
				{
					if (m_elems[i]->GetUnsprayedMarginPolyRef() == 1)
					{
						// But not if the field is too small to have them
						if (m_elems[i]->GetArea() > g_map_le_umargin_min_field_size.value())
						{
							int area = BorderAdd(m_elems[i], tole_UnsprayedFieldMargin);
						}
						else   m_elems[i]->SetUnsprayedMarginPolyRef(-1);
					}
				}
				didCalcOther = true;
			}
			else if (cfg_AddBeetleBanks.value())
			{
				cout << "Adding beetle banks/strips now" << endl;
				AddBeetleBanks((TTypesOfLandscapeElement)cfg_BeetleBankType.value());
				didCalcOther = true;
			}
			else if (cfg_AddPatches.value())
			{
				cout << "Adding patches now" << endl;
				AddPatches((TTypesOfLandscapeElement)cfg_PatchesType.value());
				didCalcOther = true;
			}

	/** After special manipulations the graphics for the landscape can be saved. */
	if (l_map_dump_gfx_enable.value())
	{
		DumpMapGraphics(l_map_dump_gfx_file.value());
	}
	/**
	* If there were any structural changes to the map by the time the execution reaches towards the end of the constructor, then a series of 
	* checks are run, before the map is saved as a new map with a _dump added to the end of the current map name. This ensures that the 
	* saved map is internally consistent.
	*/
	if (l_map_dump_enable.value() || didCalcOther)
	{
		CountMapSquares();
		ForceArea();
		PolysValidate(false);
		if (PolysRemoveInvalid()) PolysValidate(true);
		ChangeMapMapping();
		PolysRenumber();
		CalculateCentroids();
		CalculateOpenness(l_map_calc_openness.value());
		string filestr = l_map_map_file.value();
		std::size_t ind = filestr.find(".lsb"); // Find the starting position of substring in the string
		if (ind != std::string::npos) {
			filestr.erase(ind, 4); // erase function takes two parameter, the starting index in the string from where you want to erase characters and total no of characters you want to erase.
			std::cout << filestr << "\n";
		}
		string dumpmap = filestr + "_dump.lsb";
		string farmstr = filestr + "_farmref_dump.txt";
		string polystr = filestr + "_polyref_dump.txt";
		m_FarmManager->DumpFarmrefs(farmstr.c_str());
		cout << "Dumping map" << endl;
		DumpMap(dumpmap.c_str());
		cout << "Dumping polygon refs file" << endl;
		PolysDump(polystr.c_str());
		if (dump_exit) {
			g_msg->Warn(WARN_FILE, "Landscape::Landscape() ""Normal exit after dump.", "Remember to rename the new map and polyref file.");
			exit(0);
		}
		else {
			g_msg->Warn(WARN_FILE, "Landscape::Landscape() ""Dump file done", "Remember to rename the new map and polyref file.");
		}
	}

	// Set the type of hedgebanks.
	int l_subtype = cfg_HedgeSubtypeMinimum.value();
	for (unsigned int i = 0; i < m_elems.size(); i++) {
		if (m_elems[i]->GetElementType() == tole_HedgeBank) {
			m_elems[i]->SetSubType(l_subtype);
			if (++l_subtype >= cfg_HedgeSubtypeMaximum.value())
				l_subtype = cfg_HedgeSubtypeMinimum.value();
		}
	}

	// And another to set the type of hedges
	// ***CJT*** 2003-12-02
	l_subtype = 0;
	for (unsigned int i = 0; i < m_elems.size(); i++) {
		if (m_elems[i]->GetElementType() == tole_Hedges) {
			m_elems[i]->SetSubType(l_subtype);
			if (++l_subtype >= 3)
				l_subtype = 0;
		}
	}
	/**
	* Counts up the ponds and store them, at this point the constructor is finished with structural polygon handling.<br>
	*/
	CreatePondList();
	/**
	* The Farm managment is then initiated, which means that from this point all polygons will have a type of vegetation allocated.
	* Once all polygons have their tov types, the attributes based on tov type are set.
	*/
	cout << "Initiating farm management" << endl;
	m_FarmManager->InitiateManagement();
	//Set the TOV attributes
	for (unsigned int i = 0; i < m_elems.size(); i++) {
		Set_TOV_Att(m_elems[i]);
	}

	g_date->Reset();

	/*
	if ( g_farm_test_crop.value() ) {
	TestCropManagement();
	exit( 0 );
	}
	*/

	// Set up treatment flags
	// Reset internal state for the LE loop generator.
	// Compulsory!
	SupplyLEReset();
	// Get number of *all* landscape elements.
	int l_count = SupplyLECount();

	// Now loop through then.
	for (int i = 0; i < l_count; i++) {
		// Fetch next LE by its polygon reference number. Alternative
		// loop mechanism: This will return -1 at end-of-loop.
		int a_poly = SupplyLENext();

		// Skip uninteresting polygons by type, ownership,
		// phase of the moon, whatever.
		//    if ( these_are_not_the_droids_we_are_looking_for( a_poly )) {
		if (SupplyElementType(a_poly) != tole_Field)
			continue;

		// Example: Set x% of them to ignore insecticide of all types.
		if (g_random_fnc(100) < l_map_no_pesticide_fields.value()) {
			// Get current signal mask for polygon.
			LE_Signal l_signal = SupplyLESignal(a_poly);
			// Logical OR in/AND out the signals you are interested in.
			// The current signals are at the top of Elements.h
			//l_signal |= LE_SIG_NO_INSECTICIDE | LE_SIG_NO_SYNG_INSECT | LE_SIG_NO_HERBICIDE | LE_SIG_NO_FUNGICIDE | LE_SIG_NO_GROWTH_REG;
			//l_signal |= LE_SIG_NO_INSECTICIDE | LE_SIG_NO_SYNG_INSECT | LE_SIG_NO_HERBICIDE;
			// Write the mask back out to the polygon.
			SetLESignal(a_poly, l_signal);
		}
	}
	// for veg area dumps
	l_vegtype_areas = (double *)malloc(sizeof(double) * (tov_Undefined + 1));

	if (l_vegtype_areas == NULL) {
		g_msg->Warn(WARN_BUG, "Landscape::Landscape(): Out of memory!", "");
		exit(1);
	}
	FILE * outf;
	if (cfg_dumpvegjan.value()) {
		outf = fopen(cfg_dumpvegjanfile.value(), "w");
		if (!outf) {
			g_msg->Warn(WARN_FILE, "Landscape::DumpMapInfoByArea(): ""Unable to create file", cfg_dumpvegjanfile.value());
			exit(1);
		}
		else
			fclose(outf);
	}

	if (cfg_dumpvegjune.value()) {
		outf = fopen(cfg_dumpvegjunefile.value(), "w");
		if (!outf) {
			g_msg->Warn(WARN_FILE, "Landscape::DumpMapInfoByArea(): ""Unable to create file", cfg_dumpvegjunefile.value());
			exit(1);
		}
		else
			fclose(outf);
	}

	/**
	* There are a number of options to dump vegetation, event etc. information if selected, this depends on different configureation variables as needed.
	*/
	if (l_map_dump_veg_enable.value()) {
		FILE * f;
		f = fopen("VegDump.txt", "w");
		if (!f) {
			g_msg->Warn(WARN_BUG, "Landscape::Landscape(): VegDump.txt could not be created", "");
			exit(1);
		}
		fprintf(f, "Year\tDayOfYear\tMonth\tDay\tHeight\tBiomass\tGrazed\tDensity\tCover\tWeedBiomass\ttovNum\tInsectBiomass\tLATotal\tLAGreen\tDigestibility\tGreenBiomass\tDeadBiomass\tGooseGrazing\tSpilledGrain\n");
		fclose(f);
	}
    if (l_map_dump_grain_enable.value()) {
        FILE * f;
        f = fopen("GrainDump.txt", "w");
        if (!f) {
            g_msg->Warn(WARN_BUG, "Landscape::Landscape(): GrainDump.txt could not be created", "");
            exit(1);
        }
        fprintf(f, "SimDay\tYear\tDayOfYear\tMonth\tDay\tpoly\tvegtype\tGrain\tMaize\n");
        fclose(f);
    }
	if (l_map_dump_event_enable.value()) {
		FILE * f;
		f = fopen("EventDump.txt", "w");
		if (!f) {
			g_msg->Warn(WARN_BUG, "Landscape::Landscape(): EventDump.txt could not be created", "");
			exit(1);
		}
		fclose(f);
	}

	if (cfg_DumpFarmAreas.value()){
		m_FarmManager->DumpFarmAreas();
	}

	// If we are testing a pesticide then set the enum attribute
	m_PesticideType = (TTypesOfPesticide)cfg_pesticidetesttype.value();

	/**
	* Optional output files are created here as needed depending on defines or config variables
	*/
	// Write ASCII file:
	if (l_map_write_ascii.value()) {
		int x = l_map_ascii_utm_x.value();
		int y = l_map_ascii_utm_y.value();
		GISASCII_Output("AsciiLandscape.txt", x, y);
	}

#ifdef __FLOWERTESTING
	fstream ofilepntotal(cfg_total_polnek_file.value(), ios::out);
	ofilepntotal.close();
	fstream ofilepntov(cfg_tov_polnek_file.value(), ios::out);
	ofilepntov.close();
	fstream ofilepn(cfg_habitat_polnek_file.value(), ios::out);
	ofilepn.close();
#endif
#ifdef __RECORDFARMEVENTS
	m_farmeventfile = new ofstream(cfg_farmeventfile.value(), ofstream::out);
	// write header 
	(*m_farmeventfile) << "Farm No \t Parcel No \t  TOV \t FarmToDo \t Day \t Year \n";
#endif
/**
* There is a also some special set up required for some species models which is done at the end of the constructor 
*/
	//Pinkfeet
	m_GooseIntakeRateVSVegetationHeight_PF = new Polynomial2CurveClass(cfg_P1A.value(), cfg_P1B.value(), cfg_P1C.value(), cfg_P1D.value(), cfg_P1E.value(), cfg_P1F.value(), cfg_P1G.value(), cfg_P1H.value());
	//Barnacle
	m_GooseIntakeRateVSVegetationHeight_BG = new Polynomial2CurveClass(cfg_B6A.value(), cfg_B6B.value(), cfg_B6C.value(), cfg_B6D.value(), cfg_B6E.value(), cfg_B6F.value(), cfg_B6G.value(), cfg_B6H.value());
	//Greylag
	m_GooseIntakeRateVSVegetationHeight_GL = new Polynomial2CurveClass(cfg_G6A.value(), cfg_G6B.value(), cfg_G6C.value(), cfg_G6D.value(), cfg_G6E.value(), cfg_G6F.value(), cfg_G6G.value(), cfg_G6H.value());
	if (cfg_WriteCurve.value()) {
		m_GooseIntakeRateVSVegetationHeight_GL->WriteDataFile(10);
		m_GooseIntakeRateVSVegetationHeight_BG->WriteDataFile(10);
		m_GooseIntakeRateVSVegetationHeight_PF->WriteDataFile(10);
	}

	m_poly_omp_locks.resize(m_elems.size());
	for (int i=0; i<m_poly_omp_locks.size(); i++){
		m_poly_omp_locks[i] = new omp_nest_lock_t;
		omp_init_nest_lock(m_poly_omp_locks[i]);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::RunHiddenYear()
{
	// Run a year to remove any start up effects
	cout << "Running initial start-up year" << endl;
	m_firstyear = true;
	g_date->Reset2();
	for (unsigned int i = 0; i < 365; i++)
	{
		Tick();
	}
	m_firstyear = false;
}

void Landscape::ConsolidatePolys()
{
	/**
	* Runs through the map checking each cell for polygon type. If it is in our replace list then it re-written as the first instance
	* of that polygon type encountered. All subsequent instances of that type are then deleted.<br>
	* replaceList contains the types of all tole types with no behaviour that can be consolidated. This list needs to be kept up-to-date.
	*/
		const int TypesToReplace = 16;
	    TTypesOfLandscapeElement replaceList[TypesToReplace] = { 
			tole_River,
			tole_RiversidePlants,
			tole_RiversideTrees,
			tole_StoneWall,
			tole_BareRock,
			tole_BuiltUpWithParkland,
		    tole_Carpark, 
			tole_Churchyard,
			tole_Coast,
			tole_Garden,
			tole_HeritageSite,
		    tole_IndividualTree,
			tole_PlantNursery,
			tole_Saltwater,
			tole_SandDune,
			tole_UrbanNoVeg
		};
	    int foundList[TypesToReplace];
		cout << "Consolidating polygons with no special behaviour" << endl;
		for (int i = 0; i < TypesToReplace; i++) foundList[i] = -1;
		int mapwidth = m_land->MapWidth();
		int mapheight = m_land->MapHeight();
		for (int x = 0; x < mapwidth; x++) 
		{
			for (int y = 0; y < mapheight; y++) 
			{
				int ele = m_land->Get(x, y);
				TTypesOfLandscapeElement tole = m_elems[m_polymapping[ele]]->GetElementType();
				for (int t = 0; t < TypesToReplace; t++) 
				{
					if (tole == replaceList[t])
					{
						// Must do something with this cell
						if (foundList[t] == -1) foundList[t] = ele;
						else
						{
							// Need to replace this cell
							m_land->Put(x, y, foundList[t]);
						}
					}
				}
			}
		}
	// At this point there should be many polygons that are not in the map. So we need to run the valid test.
	g_msg->Warn(WARN_FILE, "Landscape::ConsolidatePolys() - ""Consolidate map dump.", "");
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::DumpMap( const char * a_filename ) 
{
	int * l_map = m_land->GetMagicP(0, 0); // Hmmm - this is a nasty way round the class data protection. Gets a pointer direct to m_map in rastermap.
	/*  FILE * l_file;
  l_file = fopen(a_filename, "wb" );
  if ( !l_file ) {
    g_msg->Warn( WARN_FILE, "Landscape::DumpMap(): Unable to open file", a_filename );
    exit( 0 );
  }

  char * l_id = m_land->GetID();


	  fwrite( l_id, 1, 12, l_file );
  fwrite( & m_width, 1, sizeof( int ), l_file );
  if (cfg_rectangularmaps_on.value() )
  {
      fwrite( & m_height, 1, sizeof( int ), l_file );
  }
  for ( int i = 0; i < m_width * m_height; i++ ) 
  {
		LE* le = m_elems[m_polymapping[l_map[i]]];
        int l_poly = le->GetPoly();
        fwrite( & l_poly, 1, sizeof( int ), l_file );
  }
  fclose( l_file );
  */
	ofstream OFile( a_filename, ios::binary);
  char id[12] = { "An LSB File" };
  OFile.write(id, 12);
  OFile.write((char*)&m_width, sizeof (int));
  OFile.write((char*)&m_height, sizeof (int));
  OFile.write((char*)l_map, m_width*m_height*sizeof (int));
  OFile.close();
}
//-----------------------------------------------------------------------------------------------------------------------------


Landscape::~Landscape( void ) {
	for (int i=0; i<m_poly_omp_locks.size(); i++){
		omp_destroy_nest_lock(m_poly_omp_locks[i]);
		delete m_poly_omp_locks[i];
	}
  if ( l_map_dump_treatcounts_enable.value() ) {
    DumpTreatCounters( l_map_dump_treatcounts_file.value() );
  }

#ifdef __RECORDFARMEVENTS
  m_farmeventfile->close();
  delete m_farmeventfile;
#endif
  
  for ( unsigned int i = 0; i < m_elems.size(); i++ )
    delete m_elems[ i ];

  free( l_vegtype_areas );
  delete m_land;
  //delete g_rotation;
  delete m_PestIncidenceManager;
  delete m_FarmManager;
  delete m_GooseIntakeRateVSVegetationHeight_PF;
  delete m_GooseIntakeRateVSVegetationHeight_BG;
  delete m_GooseIntakeRateVSVegetationHeight_GL;
  // delete g_msg; // Must be last.

  if (m_PesticideMap)
  {
	  delete m_PesticideMap;
  }

  if (m_PesticideTable)
  {
	  delete m_PesticideTable;
  }
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::SimulationClosingActions()
{
	/**
	* These are the things that are needed to be done after the simulation ends, but before landscape objects are deleted.
	*/
	;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::Tick(void) {
	/** If the farmer decision making model is on, the function:
	- calls #FarmManager::ActualProfit() function on March 1st each year starting from the 2nd year of the simulation
	- starting from the 7th year, it saves the previous year's crop set for each farm and calls the #FarmManager::ChooseDecisionMode_for_farms()
	function
	- saves all crop prices as last year prices (needed if crop prices are not constant)
	- saves the number of day degrees in a period March 1st - November 1st. See #FarmManager::daydegrees.
	- modifies energy maize price each year if the model is run with energy maize.
	*/
	g_date->Tick();
	g_weather->Tick();


	// Remember todays LAItotal for veg elements
	for (unsigned int i = 0; i < m_elems.size(); i++) {
		m_elems[i]->StoreLAItotal();
	}

	if (g_date->JanFirst()) {
		// Update the growth curve phases if needed.
		if (g_rand_uni_fnc() > 0.5) {
			m_FarmManager->SetSpilledGrain(true);
			SetGrainDist(1);
		}
		else {
			m_FarmManager->SetSpilledGrain(false);
			SetGrainDist(0);
		}
		for (unsigned int i = 0; i < m_elems.size(); i++) {
			m_elems[i]->SetGrowthPhase(janfirst);
		}
		// Update the pest incidene for this year
		m_PestIncidenceManager->RecalculateIncidence();
		m_pestincidencefactor = m_PestIncidenceManager->GetIncidenceLevel() * 2.0; // 0.0 to 2.0
	}
	else if (g_date->MarchFirst()) {
		for (unsigned int i = 0; i < m_elems.size(); i++) {
			m_elems[i]->SetGrowthPhase(marchfirst);
			m_elems[i]->SetStubble(false);
		}
		m_toxShouldSpray = false;
	}

	// Grow the green stuff and let the bugs have some too.
	#pragma omp parallel for
	for (int i = 0; i < m_elems.size(); i++) {
		m_elems[i]->Tick();
		m_elems[i]->DoDevelopment();
	}

	/*  Testing removal - may cause issues with increasing or decreasing permanent vegetation
	if ( g_date->DayInYear() == g_date->DayInYear( 1, 11 ) ) {
	// Set all elements to smooth curve transition mode at November 1st.
	for ( unsigned int i = 0; i < m_elems.size(); i++ ) {
	m_elems[ i ]->ForceGrowthInitialize();
	}
	//save the day degrees - used for crops that are not harvested instead of biomass to determine yield; done on Nov 1st
	//m_FarmManager->SetDD (SupplyTempPeriod( g_date->Date(), 245)); // from Nov 1 - 245 days back till March 1;
	//DegreesDump();
	}
	*/

	// Put the farmers to work.
	m_FarmManager->FarmManagement();

	if (!m_firstyear) {
		// Dump event information if necessary
		if (l_map_dump_veg_enable.value())
			VegDump(l_map_dump_veg_x.value(), l_map_dump_veg_y.value());
		if (l_map_dump_event_enable.value())
			EventDump(l_map_dump_event_x1.value(), l_map_dump_event_y1.value(), l_map_dump_event_x2.value(), l_map_dump_event_y2.value());
		//EventDumpPesticides( l_map_dump_event_x1.value(), l_map_dump_event_y1.value() );

		if (l_map_dump_grain_enable.value()&&(g_date->Date()%l_map_dump_grain_each_x_days.value()==0)){
			GrainDump();
		}
	}

	//debug pollen nectar model

	#ifdef __FLOWERTESTING

	fstream ofilepn(cfg_habitat_polnek_file.value(), ios::app);
	//ofilepn << SupplyDayInYear() << '\t';
	int temp_i = -1;
	for (auto [key, value] : m_polyid_habitat)
	{
		if(value != -1){
			temp_i ++;
			if(temp_i != 0) ofilepn << ',';
			ofilepn <<  PolytypeToString(SupplyElementType(value))+"-"+std::to_string(key)<< ','<<  SupplySugar(value) << ',' << SupplyNectar(value).m_quantity << ','<<SupplyPollen(value).m_quality << ','<<SupplyPollen(value).m_quantity;
		}
		
	}
	ofilepn<< endl;
	ofilepn.close();
	
	fstream ofilepntov(cfg_tov_polnek_file.value(), ios::app);
	std::string veg_name = VegtypeToString(SupplyVegType(m_polyid_farm));
	size_t start_pos = veg_name.find(",");
    if(start_pos != std::string::npos)
    	veg_name = veg_name.replace(start_pos, 1, "_");
	start_pos = veg_name.find(",");
    if(start_pos != std::string::npos)
    	veg_name = veg_name.replace(start_pos, 1, "_");
	ofilepntov <<  veg_name << ','<<  SupplySugar(m_polyid_farm) << ',' << SupplyNectar(m_polyid_farm).m_quantity << ','<<SupplyPollen(m_polyid_farm).m_quality << ','<<SupplyPollen(m_polyid_farm).m_quantity << endl;
	ofilepntov.close();
	

	/** To get some habitat specific data */


	fstream ofilepntotal(cfg_total_polnek_file.value(), ios::app);
	double nectar=0;
    double sugar=0;
    double pollen=0;
   
    for (int i= 0; i<m_poly_with_flowers.size(); i++){
        int temp_poly_id = m_poly_with_flowers.at(i);
        nectar += SupplyTotalNectar(temp_poly_id);
        double temp_nectar_quality = SupplySugar(temp_poly_id);
        sugar += SupplyPolygonArea(temp_poly_id) * temp_nectar_quality;
        pollen += SupplyTotalPollen(temp_poly_id);
    }
	ofilepntotal << nectar << ','<<  sugar<< ','<<  pollen<<','<< g_date->DayInYear() <<','<<SupplyTemp()<<endl;
	ofilepntotal.close();
	#endif

}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::SetPolyMaxMinExtents()
{
  // All polygon manipulation is settled now we need to give the polygons some information about themselves
  // This takes a little time but save time later one
	cout << "Setting max min polygon extents" << endl;
	int mwidth = m_land->MapWidth();
	int mheight = m_land->MapHeight();
	for ( int x = 0; x < mwidth; x++ ) {
		for ( int y = 0; y < mheight; y++ ) {
			int polyindex = m_land->Get( x, y );
			// Mark that we have seen this polygon.
			unsigned int ele_ref= polyindex;
			if (m_elems[m_polymapping[ele_ref]]->GetMaxX() < x) m_elems[m_polymapping[ele_ref]]->SetMaxX(x);
			if (m_elems[m_polymapping[ele_ref]]->GetMaxY() < y) m_elems[m_polymapping[ele_ref]]->SetMaxY(y);
			if (m_elems[m_polymapping[ele_ref]]->GetMinX() > x) m_elems[m_polymapping[ele_ref]]->SetMinX(x);
			if (m_elems[m_polymapping[ele_ref]]->GetMinY() > y) m_elems[m_polymapping[ele_ref]]->SetMinY(y);
			m_elems[m_polymapping[ele_ref]]->SetMapValid(true);
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

std::string Landscape::SupplyMonthName(void)
{
	int m = g_date->GetMonth();
	char error_num[10];

	switch (m) {
	case 1:
		return "January";
	case 2:
		return "February";
	case 3:
		return "March";
	case 4:
		return "April";
	case 5:
		return "May";
	case 6:
		return "June";
	case 7:
		return "July";
	case 8:
		return "August";
	case 9:
		return "September";
	case 10:
		return "October";
	case 11:
		return "November";
	case 12:
		return "December";
	default:
		sprintf(error_num, "%d", m);
		g_msg->Warn(WARN_FILE, "Landscape::SupplyMonthName(): Unknown event type:", error_num);
		exit(1);
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
void Landscape::GrainDump(){
	/**
	* Records total amount of grain/seed/maize available as forage for each polygon with forage available. Information is dumped to GrainDump.txt
	*/
	FILE * vfile=fopen("GrainDump.txt", "a" );
    if (!vfile) {
        g_msg->Warn( WARN_FILE, "Landscape::GrainDump(): Unable to open file", "GrainDump.txt" );
        exit( 1 );
    }
    int day = g_date->Date();
    int year = g_date->GetYear();
    int yearday = SupplyDayInYear();
    std::string monthname = SupplyMonthName();
    int day_month = SupplyDayInMonth();
    for (unsigned int i = 0; i < m_elems.size(); i++) {
        double grain = (m_elems[ i ]-> GetBirdSeed())*(m_elems[ i ]-> GetArea());
        double maize = m_elems[ i ]-> GetBirdMaize()*(m_elems[ i ]-> GetArea());
        long    poly = m_elems[i] -> GetVegType();
        if ((grain>0)||(maize>0)){
            fprintf( vfile, "%4d\t%4d\t%4d\t%10s\t%3d\t%5d\t%5d\t%7.3f\t%7.3f\n",day, year, yearday, monthname.c_str(), day_month, i, poly, grain,maize );
        }


    }

    fclose( vfile );
}
//-----------------------------------------------------------------------------------------------------------------------------
void Landscape::VegDump( int x, int y ) {
	/**
	* Records vegetation charateristics for the x,y location. Information is dumped to VegDump.txt
	*/
  FILE * vfile=fopen("VegDump.txt", "a" );
  if (!vfile) {
    g_msg->Warn( WARN_FILE, "Landscape::VegDump(): Unable to open file", "VegDump.txt" );
    exit( 1 );
  }
  //int year = SupplyYearNumber();
  int year = g_date->GetYear();
  int day = SupplyDayInYear();
  std::string monthname = SupplyMonthName();
  int day_month = SupplyDayInMonth();
  double hei = SupplyVegHeight( x, y );
  double bio = SupplyVegBiomass( x, y );
  double cover = SupplyVegCover( x, y );
  double density = bio / ( hei + 1 );
  double weeds = SupplyWeedBiomass( x, y );
  double insects = SupplyInsects( x, y );
  double LATotal = SupplyLATotal(x, y);
  double LAGreen = SupplyLAGreen(x, y);
  double digest = SupplyVegDigestibility(x, y);
  double GreenBiomass = SupplyGreenBiomass(x,y);
  double DeadBiomass = SupplyDeadBiomass(x,y);
  int grazed = SupplyGrazingPressure(x, y);
  double ggraze = GetActualGooseGrazingForage(m_land->Get(x, y), gs_Pinkfoot);
  //int VegType = BackTranslateVegTypes(SupplyVegType(x, y));
  int VegType = SupplyVegType(x, y);
  double grain = SupplyBirdSeedForage(x, y);


  fprintf( vfile, "%4d\t%4d\t%10s\t%3d\t%3.2f\t%3.2f\t%3d\t%3.2f\t%3.2f\t%3.2f\t%3i=%s\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\t%3.2f\n", year, day, monthname.c_str(), day_month, hei, bio, grazed, density, cover, weeds, VegType, VegtypeToString((TTypesOfVegetation)VegType).c_str(), insects, LATotal, LAGreen, digest, GreenBiomass, DeadBiomass, ggraze, grain );
  fclose( vfile );
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::DegreesDump(){

	ofstream ofile ("Daydegrees.txt", ios::app);	
	//print degrees
	ofile << m_FarmManager->GetDD();
	ofile  << endl;
	ofile.close();
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::EventDump( int x1, int y1, int x2, int y2 ) {
  FILE * vfile=fopen("EventDump.txt", "a" );
  if (!vfile) {
    g_msg->Warn( WARN_FILE, "Landscape::EventDump(): Unable to open file", "EventDump.txt" );
    exit( 1 );
  }
  FarmToDo event;
  int i = 0;
  int day = SupplyDayInYear();
  fprintf( vfile, "%d: ", day );
  while ( ( event = ( FarmToDo )SupplyLastTreatment( x1, y1, & i ) ) != sleep_all_day ) {
    fprintf( vfile, "%d ", event );
  }
  i = 0;
  fprintf( vfile, " - " );
  while ( ( event = ( FarmToDo )SupplyLastTreatment( x2, y2, & i ) ) != sleep_all_day ) {
    fprintf( vfile, "%d ", event );
  }
  fprintf( vfile, "\n" );
  fclose( vfile );
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::EventDumpPesticides( int x1, int y1 ) {
  FILE * vfile=fopen("EventDump.txt", "a" );
  if (!vfile) {
    g_msg->Warn( WARN_FILE, "Landscape::EventDump(): Unable to open file", "EventDump.txt" );
    exit( 1 );
  }
  FarmToDo a_event;
  int i = 0;
  int day = this->SupplyGlobalDate();
  int herb = 0;
  int fung = 0;
  int ins = 0;
  while ( ( a_event = ( FarmToDo )SupplyLastTreatment( x1, y1, & i ) ) != sleep_all_day ) {
 	  if (a_event == herbicide_treat )
	  {
			  herb++;
	  }
	  else if (a_event == fungicide_treat ) fung++;
	  else if (a_event == insecticide_treat) ins++;
  }
  if (herb+fung+ins >0 ) fprintf( vfile, "%d\t%d\t%d\t%d\n", day, herb, fung, ins );
  i = 0;
  fclose( vfile );
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::PolysValidate( bool a_exit_on_invalid ) {
   // First loop just sets the MapValid as false (and checks for a major screw-up if this elemenent does not exist even in the list
  for ( unsigned int i = 0; i < m_elems.size(); i++ ) {
    m_elems[ i ]->SetMapValid( false );
    if ( m_polymapping[ m_elems[ i ]->GetPoly() ] == -1 ) {
      char l_err[ 20 ];
      sprintf( l_err, "%d", m_elems[ i ]->GetPoly() );
      g_msg->Warn( WARN_FILE, "Landscape::PolysValidate(): Invalid polymapping ", l_err );
      exit( 1 );
    }
  }
  // Now go through the whole map and for each polygon found set MapValid as true.
  SetPolyMaxMinExtents();
  if ( a_exit_on_invalid ) {
    for ( unsigned int i = 0; i < m_elems.size(); i++ ) {
      if ( !m_elems[ i ]->GetMapValid() ) {
        char l_err[ 20 ];
        sprintf( l_err, "%d", m_elems[ i ]->GetPoly() );
        g_msg->Warn( WARN_FILE, "Landscape::PolysValidate(): Invalid polygon ", l_err );
        exit( 0 );
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::PolysRemoveInvalid(void)
{
	/**
	*  PolysValidate or ChangeMapMapping must be called after this method
	*/
	bool didsomething = false;
	vector < LE* > l_temp;
	cout << "Tidying up the polygon map in PolysRemoveInvalid" << endl;
	unsigned int sz = (int)m_elems.size();
	for (unsigned int i = 0; i < sz; i++) {
		if (m_elems[i]->GetMapValid()) {
			unsigned int j = (int)l_temp.size();
			l_temp.resize(j + 1);
			l_temp[j] = m_elems[i];
		}
		else {
			// cout << "Deleted m_elems index:" << m_polymapping[ m_elems[ i ]->GetPoly() ] << " Polynumber :" << m_elems[ i ]->GetPoly() << BackTranslateEleTypes(m_elems[ i ]->GetElementType()) << endl;
			m_polymapping[m_elems[i]->GetPoly()] = -1;
			delete m_elems[i];
			didsomething = true;
		}
	}

	for (unsigned int i = 0; i < l_temp.size(); i++) {
		m_elems[i] = l_temp[i];
	}
	m_elems.resize(l_temp.size());
	RebuildPolyMapping();
	return didsomething;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::PolysDump(const char * a_filename)
{
	ofstream outf(a_filename, ios::out);
	int l_num_polys = 0;

	if (!outf.is_open()) {
		g_msg->Warn(WARN_FILE, "Landscape::PolysDump(): Unable to open file", a_filename);
		exit(1);
	}

	// Count up number if active polygons in our list.
	unsigned sz = (unsigned)m_elems.size();
	for (unsigned int i = 0; i < sz; i++) {
		if (m_elems[i]->GetMapValid())
			l_num_polys++;
	}

	outf << this->SupplyCountryCode() << " " << "Latitude " << this->SupplyLatitude() << " Longitude " << this->SupplyLongitude() << "\n";
	outf << l_num_polys << endl;
	outf << "PolyType" << '\t' << "PolyRefNum" << '\t' << "Area" << '\t' << "FarmRef" << '\t' << "UnSprayedMarginRef" << '\t' << "SoilType" << '\t' << "Openness" << '\t' << "CentroidX" << '\t' << "CentroidY" << '\t' << "Elevation" << '\t' << "Slope" << '\t' << "Aspect" << '\t' << "PollenNectarCurve" << endl;

	// Now we can output the file
	// The last column needs to be changed from -1 to curve number after defining GetPollenNectarCurveRef()
	for (unsigned int i = 0; i < m_elems.size(); i++)
	{
		if (m_elems[i]->GetMapValid())
		{
			outf << m_elems[i]->GetALMaSSEleType() << '\t' << m_elems[i]->GetPoly() << '\t' << m_elems[i]->GetArea() << '\t' <<
				m_elems[i]->GetOwnerFile() << '\t' << m_elems[i]->GetUnsprayedMarginPolyRef() << '\t' << m_elems[i]->GetSoilType() << '\t' << m_elems[i]->GetOpenness()
				<< '\t' << m_elems[i]->GetCentroidX() << '\t' << m_elems[i]->GetCentroidY() << '\t' << m_elems[i]->GetElevation() << '\t' << m_elems[i]->GetSlope() << '\t' << m_elems[i]->GetAspect() << '\t' << -1 << endl;
		}
	}
	outf.close();
}
//-----------------------------------------------------------------------------------------------------------------------------

int Landscape::ReadPolys1(const char* a_polyfile)
{
	// Read first line and figure out what format i have 
	int format = 0;
	int NoPolygons;
	float latitude, longitude;
	std::string country_code;
	string rubbish = "";
	string firstline = ""; // added for parsing between the two formats
	ifstream pifile(a_polyfile);
	if (!pifile.is_open()) {
		g_msg->Warn(WARN_FILE, "Landscape::ReadPolys1(): Unable to open file", a_polyfile);
	}
	// Read first line and figure out what format we have 
	getline(pifile, firstline);

	// Default Location
	this->SetCountryCode("DK");
	this->SetLatitude(56.166666);
	this->SetLongitude(10.1999992);

	if (!(std::stringstream(firstline) >> NoPolygons))
	{
		/**
		* This is a new file format 2020/2021 to include country code, latitude, longitude, aspect, slope, elevation, and the pollen/nectar curve number
		*/
		format = 1;
		std::stringstream(firstline) >> country_code >> rubbish >> latitude >> rubbish >> longitude;
		pifile >> NoPolygons;
		this->SetCountryCode(country_code);
		this->SetLatitude(latitude);
		this->SetLongitude(longitude);
	}
	m_elems.resize(NoPolygons);
	pifile.close();
	return format;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::ReadPolys2(const char * a_polyfile, int a_format)
{
	/** The polygon file consists of 9 or 13 columns:\n
	* 1. Polygon Number 2. Type 3. Area as a double 4. Owner (-1 = now owner),  5- -1 or unsprayed margin polynum\n,
	* column 6 is soil type (-1 if not used), column 7 is the polygon openness score (-1 if unset), 8 is the x-coordinate of the centroid, and 9 is the y-coordinate of the centroid. If either of these centroid coords are -1, then the centroid calculation will be forced. \n
	*If this is to be used then #cfg_map_usesoiltypes needs to be set as true (default true).\n
	*/

	// Need to figure out if the farms are already renumbered - as the Farm manager
	bool farmsrenum = m_FarmManager->GetIsRenumbered();

	int NoPolygons = int(m_elems.size());
	string rubbish = ""; //put here the names of the parameters;
	ifstream ifile(a_polyfile);
	if (!ifile.is_open()) {
		g_msg->Warn(WARN_FILE, "Landscape::ReadPolys2(): Unable to open file", a_polyfile);
		std::exit(1);
	}
	char error_num[20];
		/** As of December 2014 there is a header row of information to be skipped here */
		for (int i = 0; i < ((a_format == 0) ? 10 : 19); i++) { ifile >> rubbish; }

	/** As of July 2013 we have the need to use really big maps, hence polygon reference numbers need to be kept within reasonable bounds.
	* It is now a requirement that any polyref number is < twice the total number of polygons in the file.
	*/
	int np = NoPolygons;
	if (NoPolygons < 10000) np = 10000; // this is just to cope with old maps with < 10000 polygons that do not follow the rules for new maps
	m_polymapping.resize(np * 2);

	// Set all mappings to unused.
	for (int i = 0; i < np * 2; i++) {
		m_polymapping[i] = -1;
	}

	int ElemIndex = 0;

	for (int x = 0; x < NoPolygons; x++)
	{
		int PolyNum, Owner, PolyType, RealPolyType, URef, SoilType, openness, Centroid_x, Centroid_y;
		float Elevation = -1, Slope = -1, Aspect = -1;
		int PollenNectarCurve = 0;

		TTypesOfLandscapeElement Type;
		float Area;
		ifile >> PolyType >> PolyNum >> Area >> Owner >> URef >> SoilType >> openness >> Centroid_x >> Centroid_y;
		
		if (a_format == 1)
		{
			ifile >> Elevation >> Slope >> Aspect >> PollenNectarCurve;
		}

		// ToDo: Check input validity for elev, slope and aspect once agreed

		// Here we make some tests to check input validity
		if ((SoilType > 16) || (PolyNum<0))
		{
			std::sprintf(error_num, "%d", NoPolygons);
				g_msg->Warn(WARN_FILE, "Landscape::ReadPolys(): Polygon file empty before "
					"reading number of specified polygons (old polygon file format?):", error_num);
				std::exit(1);
		}

		// Owner is the farm number or -1. If farms have not been renumbered then this needs mapped to the index in the list of farms.
		// Because we create this sequentially in Farm.cpp we have a constant index.
		if ((-1 != Owner) && !farmsrenum)
		{
			// Need to replace the Owner with the new renumbered ref.
			Owner = m_FarmManager->GetRenumberedFarmRef(Owner);
		}

		RealPolyType = PolyType;
		/** This is a quick way to replace one type with another for some simulations, polygon type 150 therefore has some special uses */
		if (PolyType == 150) 
		{
			PolyType = l_map_chameleon_replace_num.value();
		}

		Type = g_letype->TranslateEleTypes(PolyType);
		if (Type == tole_Missing)
		{
			m_DoMissingPolygonsManipulations = true;
		}
		if (-1 == m_polymapping[PolyNum])
		{
			// First time we have encountered this polygon number.
			// Borders are not mapped in this list.
			m_polymapping[PolyNum] = ElemIndex;
			LE * newland = NewElement(Type);
			m_elems[ElemIndex++] = newland;
			newland->SetPoly(PolyNum);
			newland->SetMapIndex(PolyNum);
			newland->SetArea(floor(0.5 + Area));
			newland->SetALMaSSEleType(RealPolyType);
			newland->SetSoilType(SoilType);
			newland->SetUnsprayedMarginPolyRef(URef);
			newland->SetCentroid(Centroid_x,Centroid_y);
			newland->SetOpenness(openness);
			newland->SetElevation(Elevation);
			newland->SetSlope(Slope);
			newland->SetAspect(Aspect);
			/** Here we need to set the pollen curve - there are a few possible options
			* The PollenNectarCurve variable holds either: \n
			* -3 (relies on tov type), we set to zero for now but later the program will alter it when the veg is changed. This should only be for fields. \n
			* -2 (not needed for this poly - default curve 0, \n
			* -1 column not filled in, default to zero curve, \n
			* or a curve reference from the habitat set
			* */
			// Just for fun, or maybe because we might need it later, remember the actual largest polynum used
			if (PolyNum>m_LargestPolyNumUsed) m_LargestPolyNumUsed = PolyNum;
			// Now set any centroid or openness recalcuation flags
			if ((Centroid_x < 0) || (Centroid_y < 0)) m_NeedCentroidCalculation= true;
			if (openness < 0) m_NeedOpennessCalculation = true;
			// Two types of possible errors: Landscape element that is a field,
			// but doesn't belong to a farm, or a farm element not of type field.
			// Check for both cases.
			if (-1 == Owner && ClassificationFieldType(Type) == true ) {
				// No owner but field polygon.
				sprintf(error_num, "%d", PolyNum);
				g_msg->Warn(WARN_FILE, "Landscape::ReadPolys(): Farm polygon does not belong to a farm:", error_num);
				exit(1);
			}
			if (-1 != Owner && ClassificationFieldType(Type) == false ) {
				// An owner but not field elements.
				sprintf(error_num, "%d", PolyNum);
				g_msg->Warn(WARN_FILE, "Landscape::ReadPolys(): Farm polygon does not have element type tole_Field:", error_num);
				exit(1);
			}

			if (-1 != Owner)
			{
				m_FarmManager->ConnectFarm(Owner);
				m_FarmManager->AddField(Owner, newland, Owner);

				// Code to generate unsprayed margins....
				if (newland->GetElementType() == tole_Field)
				{
					if (g_map_le_unsprayedmargins.value())
					{
						if (g_random_fnc(100) < g_map_le_unsprayedmargins_chance.value())
						{
							// This is a farm field, so signal adding a margin
							newland->SetUnsprayedMarginPolyRef(1);
						}
					}
				}
				// ..to here
			}
		}
		else {
			sprintf(error_num, "%d", PolyNum);
			g_msg->Warn(WARN_FILE, "Landscape::ReadPolys2(): Duplicate polygon in file", error_num);
			exit(1);
		}
	}
	ifile.close();
	/** With the polygons renumbered, we can safely set the hb_first_free_poly_num to the number of polygons we have. */
	hb_first_free_poly_num = m_elems[NoPolygons - 1]->GetPoly() + 1;
	for (int i = 0; i < NoPolygons; i++)
	{
		if (m_elems[i]->GetUnsprayedMarginPolyRef() != -1)
		{
			TTypesOfLandscapeElement tole = m_elems[i]->GetElementType();
			int umref = m_elems[i]->GetUnsprayedMarginPolyRef();
			m_elems[m_polymapping[umref]]->SetOwner_tole(tole);
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::CountMapSquares( void ) {
	/**
	* Runs through the list of polygons in Landscape::m_elems and zeros the areas. 
	* Then traverses the entire landscape and counts the number of map squares associated with each polygon, storing this as the 
	* polygon area ( LE:m_squares_in_map ).
	*/
	int mapwidth = m_land->MapWidth();
	int mapheight = m_land->MapHeight();
	for ( unsigned int i = 0; i < m_elems.size(); i++ ) {
		m_elems[i]->SetArea(0);
		m_elems[ i ]->m_squares_in_map=0;
	}

	for ( int x = 0; x < mapwidth; x++ ) {
		for ( int y = 0; y < mapheight; y++ ) {
			int l_ele = m_land->Get( x, y );
			m_elems[ l_ele ]->m_squares_in_map++;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::PolysRenumber(void)
{
	/**
	* This saves space in arrays by removing unused polygon numbers and renumbering, then triggering a dump of the 
	* landscape files.
	* It uses a shortcut in that the polygons are already mapped to an array without space, so we use the index of this array to 
	* define the new polygon number.
	* There is one problem however, that is if the polygon is a field then it might have a an unsprayed field margin reference, 
	* and that reference cannot be renumbered in this way. This requires a two step pass, first to find the unsprayed margin polygons, then to 
	* map the new and old numbers, then pass through again and reset all fields to point to the correct new polygon number.
	*/
	cout << "In Landscape::Landscape() Polygon renumber." << endl;
	vector<int>unsprayedmargins_original;
	vector<int>unsprayedmargins_new;

	for (unsigned int i = 0; i < m_elems.size(); i++)
	{
		// Need to reset the poly number
		int index = m_elems[i]->GetMapIndex(); // This is the number currently in the map matrix
		// Deal with potential unsprayed margins by recording them here
		if (m_elems[i]->GetALMaSSEleType() == 31)
		{
			int oldpoly = m_elems[i]->GetPoly();
			unsprayedmargins_original.push_back(oldpoly);
			unsprayedmargins_new.push_back(i);
		}
		m_elems[i]->SetPoly(index); // The map index and the polygon number are now one and the same
		m_polymapping[index] = i; // The polymapping is now linked via index to the m_elems index (i)
	}
	// Now run through the m_elems and replace original unsprayed margin refs with the new ones
	for (unsigned int i = 0; i < m_elems.size(); i++)
	{
		int old = m_elems[i]->GetUnsprayedMarginPolyRef();
		if (old!=-1)
		{
			bool found = false;
			for (unsigned j = 0; j < unsprayedmargins_original.size(); j++)
			{
				if (old == unsprayedmargins_original[j])
				{
					m_elems[i]->SetUnsprayedMarginPolyRef(unsprayedmargins_new[j]);
					found = true;
					break;
				}
			}
			if (!found)
			{
				g_msg->Warn( "Landscape::Landscape() Mismatch in unsprayed margins refs: ", old);
			}
		}
	}
		m_LargestPolyNumUsed = (int) m_elems.size()-1;
	g_msg->Warn(WARN_FILE, "Landscape::Landscape() ""Map to be dumped due to polygon renumber", "");
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::ForceArea( void ) {
  int l_area_sum = 0;

  for ( unsigned int i = 0; i < m_elems.size(); i++ ) {
    m_elems[ i ]->SetArea( ( double )m_elems[ i ]->m_squares_in_map );
    if ( m_elems[ i ]->m_squares_in_map > 0 ) {
      m_elems[ i ]->SetMapValid( true );
      l_area_sum += m_elems[ i ]->m_squares_in_map;
    }
  }

  if ( l_area_sum != m_width * m_height ) {
    g_msg->Warn( WARN_BUG, "Landscape::ForceArea(): Polygon areas doesn't"" sum up to map area!", "" );
    exit( 1 );
  }
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::RemoveMissingValues(void)
{
	bool found; // the flag for something left to work with
	int mapwidth = m_land->MapWidth();
	int mapheight = m_land->MapHeight();
	int counter = 0;
	do
	{
		found = false; counter++;
		for (int x = 1; x < mapwidth-1; x++)
		{
			for (int y = 1; y < mapheight-1; y++)
			{
				int apoly = m_land->Get(x,y);
				if (m_elems[m_polymapping[apoly]]->GetElementType() == tole_Missing)
				{
					m_land->MissingCellReplace(x, y, true);
				}
			}
		}
		counter++;
	} while (counter<50);
	// Now we only have the edges to deal with 
	for (int x = 0; x < mapwidth; x++)
	{
		for (int y = 0; y < mapheight; y++)
		{
			int apoly = m_land->Get(x, y);
			if (m_elems[m_polymapping[apoly]]->GetElementType() == tole_Missing)
			{
				found = true;
				counter++;
				m_land->MissingCellReplaceWrap(x, y, true);
			}
		}
	}
	// Now we the ones that are not next to fields to deal with 
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::ChangeMapMapping( void ) {
	/**
	Our map is an array of polygon indentifiers, where we really want to know the associated landscape element of a X-Y coordinate pair.  \n
	Changing this to m_elems[] indices will save us one redirection when inquiring information from the landscape, and only costs us the fixed
	translation step performed here at startup. \n
	*/
	cout << "In Change Map Mapping" << endl;
	int mapwidth = m_land->MapWidth();
	int mapheight = m_land->MapHeight();
	int oldindex = -1;
	for ( int x = 0; x < mapwidth; x++ ) 
	{
		for ( int y = 0; y < mapheight; y++ ) 
		{
			int polynum = m_land->Get( x, y ); // the polyref e.g. = 1, m_polymapping[ polynum ] = 0
			m_elems[ m_polymapping[ polynum ]]->SetMapIndex( m_polymapping[ polynum ] ); // Here we set index in the map to the index in elements, i.e. 0
			m_elems[ m_polymapping[ polynum ]]->SetMapValid( true );
			// Do the translation.
			m_land->Put( x, y, m_polymapping[ polynum ] ); // and now we write this to the map, i.e. 0
			// This coordinate is now valid. Throw these coordinates into
			// the associated landscape element.
			int index = m_polymapping[ SupplyPolyRef( x, y ) ];
			if ( index != oldindex ) 
			{
				m_elems[ index ]->SetValidXY( x, y );
				oldindex = index;
			}
		}
	}
	RebuildPolyMapping(); 
/*
// Check that all of the polygons are mentioned in the map.
	if ( l_map_check_polygon_xref.value() ) 
	{
		for ( unsigned int i = 0; i < m_elems.size(); i++ ) 
		{
			if ( !m_elems[ i ]->GetMapValid() ) {
				char poly[ 20 ];
				sprintf( poly, "%d", m_elems[ i ]->GetPoly() );
				g_msg->Warn( WARN_FILE, "Landscape::ChangeMapMapping(): ""Polygon number referenced but not in map file: ", poly );
				exit( 1 );
			}
		}
	}
*/
}
//-----------------------------------------------------------------------------------------------------------------------------

int Landscape::RemoveSmallPolygons()
{
	/**
	* The method removes small polygons from the map and joins them with polygons that are close too them - there different rules
	* depending on the type of polygon we have. Some small polygons can be legal, others need something done. Field polygons with 
	* management also have some minimum restrictions which are different from normal polygons.<br>
	* This method is not written for efficiency, so simple brute force approaches are taken - it will take time to execute.<br>
	* <br>
	* Rules:<br>
	* - Linear features: Isolated cells of these are possible, but they should not be separate polygons. These need to be joined 
	* together to form bigger, if not contiguous polygons
	* - other 1m cell polygons need to be removed and replaced with the most common surrounding cell
	* - Field polygons: If they are less than 1000m2 then they are assumed to be grassland
	*
	*/
	// To be sure we first count the map squares
	CountMapSquares();
	// Then force the area to match the counted squares
	ForceArea();
	// Next find polygons that don't match other rules and are single cells - remove these
	int removed = 0;
	for (int i = 0; i < m_elems.size(); i++)
	{
		TTypesOfLandscapeElement tole = m_elems[i]->GetElementType();
		int area = int(m_elems[i]->GetArea());
		if (area == 1)
		{
			switch (tole)
			{
				case tole_FieldBoundary:
				case tole_HedgeBank:
				case tole_Hedges:
				case tole_IndividualTree:
				case tole_MetalledPath:
				case tole_River:
				case tole_RiversidePlants:
				case tole_RiversideTrees:
				case tole_RoadsideSlope:
				case tole_RoadsideVerge:
				case tole_SmallRoad:
				case tole_StoneWall:
				case tole_Fence:
				case tole_Stream:
				case tole_Track:
				case tole_UnsprayedFieldMargin:
				case tole_WaterBufferZone:
				case tole_FlowerStrip:
					// These could be part of a bigger polygon, so leave them alone for now.
					break;
				default:
					// Get rid of this one.
					APoint pt = m_elems[i]->GetCentroid();
					// Just check that the centroid is correct
					int poly = m_elems[i]->GetPoly();
					if (poly != i)
					{
						g_msg->Warn(WARN_FILE, "Landscape::RemoveSmallPolygons: Centroid not in polygon: ", i); 
						exit(99);
					}
					// The next line just fixes the map, and replaces the cell value
					if (m_land->CellReplacementNeighbour(pt.m_x, pt.m_y, poly) == 1) {
						// Now we need to remove the polygon. This means we must also rebuild the map later.
						m_elems[i]->SetMapValid(false);
						removed++;
					}
			}
		}
		// Field removal below 100m2
		else if (tole == tole_Field)
		{
			if (area < 100) {
				// Need to copy the useful information to new grassland element and switch that one in, removing the field.
				NaturalGrass* grass = new NaturalGrass(tole_NaturalGrassDry, this);
				grass->DoCopy(m_elems[i]);
				grass->SetALMaSSEleType(g_letype->BackTranslateEleTypes(tole_NaturalGrassDry));
				grass->SetElementType(tole_NaturalGrassDry);
				grass->SetOwner(NULL,-1,-1);
				delete m_elems[i]; // remove the old LE
				m_elems[i] = dynamic_cast<LE*>(grass);
			}
		}
	}
	PolysRemoveInvalid();
	ChangeMapMapping();
	return removed;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::BorderRemoval() {
	/**
	* Removes field boundaries, hedgebanks and hedges if they are adjacent to a field . This can be used for example if we want to
	* be sure of the amount of added elements or their width.
	*/
	// This does not need to be efficient, just do the job
	for (int x=1; x<(m_width-1); x++)
		for (int y=1; y<(m_height-1); y++)
		{
			TTypesOfLandscapeElement tole = SupplyElementType(x,y);
			if ((tole==tole_FieldBoundary) || (tole==tole_HedgeBank) || (tole==tole_Hedges) || (tole == tole_FlowerStrip))
			{
				if ( SupplyElementType(x-1,y-1) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x-1,y-1);
					m_land->Put( x, y, fieldindex );

				}
				else
				if ( SupplyElementType(x-1,y) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x-1,y);
					m_land->Put( x, y, fieldindex );

				}
				else
				if ( SupplyElementType(x-1,y+1) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x-1,y+1);
					m_land->Put( x, y, fieldindex );

				}
				else
				if ( SupplyElementType(x,y-1) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x,y-1);
					m_land->Put( x, y, fieldindex );

				}
				else
				if ( SupplyElementType(x,y+1) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x,y+1);
					m_land->Put( x, y, fieldindex );

				}
				else
				if ( SupplyElementType(x+1,y-1) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x+1,y-1);
					m_land->Put( x, y, fieldindex );

				}
				else
				if ( SupplyElementType(x+1,y) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x+1,y);
					m_land->Put( x, y, fieldindex );

				}
				else
				if ( SupplyElementType(x+1,y+1) == tole_Field)
				{
					// Set the x,y location to be this field
					int fieldindex = SupplyPolyRefIndex(x+1,y+1);
					m_land->Put( x, y, fieldindex );

				}
			}
		}
}
//-----------------------------------------------------------------------------------------------------------------------------

int Landscape::BorderAdd(LE* a_field, TTypesOfLandscapeElement a_type, int a_allAround) {

	LE* border;
	int width;
	// If it is an unsprayed field margin
	if (a_type == tole_UnsprayedFieldMargin) {
		width = l_map_umargin_width.value();
	}
	else { // otherwise it is a border
		width = g_map_le_borderwidth.value();
	}

	if(a_field->GetBorder() != NULL){
		border = a_field->GetBorder();
	}
	else {
		// Create border and add it to list of elements
		border = NewElement(a_type);
		m_polymapping[hb_first_free_poly_num] = (int)m_elems.size();
		m_elems.resize(m_elems.size() + 1);
		m_elems[m_elems.size() - 1] = border;

		// If it is an unsprayed field margin
		if (a_type == tole_UnsprayedFieldMargin) {
			a_field->SetUnsprayedMarginPolyRef(hb_first_free_poly_num);
			TTypesOfLandscapeElement tole = a_field->GetElementType();
			border->SetOwner_tole(tole);
		}
		else { // otherwise it is a border
			a_field->SetBorder(border);
		}

		// Add more information about border 
		border->SetMapIndex(hb_first_free_poly_num);
		border->SetPoly(hb_first_free_poly_num++);
		border->SetSlope(a_field->GetSlope());
		border->SetAspect(a_field->GetAspect());
		border->SetElevation(a_field->GetElevation());
		if (ClassificationFieldType(a_type) == true) {
			border->SetOwner(a_field->GetOwner(), a_field->GetOwner()->GetFarmNumber(), a_field->GetOwnerIndex());
		}
	}
	// Check that centroids are inside the polygon and valid. 
	int fieldpoly = a_field->GetPoly(); // fieldpoly is the polygon number
	int borderpoly = border->GetPoly(); // borderpoly is the polygon number
	int borderindex = m_polymapping[borderpoly]; // borderindex is the elems index for the border
	int fieldindex = m_polymapping[fieldpoly]; // fieldindex is the elems index

	// Create coordinates for circle with radius equal to the width of the border to be used for creating the border				
	std::vector<int> x_circle;
	std::vector<int> y_circle;
	int n_circle = 0;
	for (int dx = -width; dx <= width; dx++) {
		for (int dy = -width; dy <= width; dy++) {
			int dist = hypot(dx, dy);
			if (dist <= width) {
				x_circle.push_back(dx);
				y_circle.push_back(dy);
				n_circle++;
			}
		}
	}

	// Find the centre of the field
	int cx = a_field->GetCentroidX();
	int cy = a_field->GetCentroidY();
	FindFieldCenter(a_field, &cx, &cy);
	// Find the rectangle that describes the field the best
	// Returns the alignment of the longest axis in the rectangle 
	// and sets width and starting point on short side of best rectangle
	int field_width = 0;
	int x_start_perp = 0;
	int y_start_perp = 0;
	int alignment = FindBestRectangle(a_field->GetPoly(), &cx, &cy, &field_width, &x_start_perp, &y_start_perp);

	// Check that field is not too narrow
	if (field_width <= g_map_le_minfieldwidth.value()) {
		if (a_type == tole_UnsprayedFieldMargin) a_field->SetUnsprayedMarginPolyRef(-1);
		return -1;
	}

	int x1 = cx;
	int y1 = cy;
	int x2 = cx;
	int y2 = cy;
	if (a_allAround == 0) { // Only calculate points if they are going to be used;
		AxisLoop(a_field->GetPoly(), &x1, &y1, alignment + 1);
		AxisLoop(a_field->GetPoly(), &x2, &y2, alignment + 7);
	}

	// Loop through all pixels in rectangle surrounding the field
	int min_x = a_field->GetMinX();
	int max_x = a_field->GetMaxX();
	int min_y = a_field->GetMinY();
	int max_y = a_field->GetMaxY();
	//if (max(x1, x2) < max_x - width) max_x = max(x1, x2);
	//if (max(y1, y2) < max_y - width) max_y = max(y1, y2);
	//if (min(x1, x2) > min_x + width) min_x = min(x1, x2);
	//if (min(y1, y2) < min_y + width) min_y = min(y1, y2);
	int area = 0;
	for (int x = min_x; x <= max_x; x++) { // Loop through x
		for (int y = min_y; y <= max_y; y++) { // Loop through y
			// Make sure we are inside map
			if ((x >= 0) && (y >= 0) && (x < m_width) && (y < m_height)) {
				int positionGood = 1;
				if (a_allAround == 0) {
					// Only add on one side of line between P1(x1,y1) and P2(x2,y2)
					positionGood = std::copysign(1, (x2 - x1) * (y - y1) - (y2 - y1) * (x - x1));
				}
				if (positionGood >= 0) {
					int index = m_land->Get(x, y);
					if (index == fieldindex) { // Check that the current pixel is in the field
						for (int i = 0; i < n_circle; i++) { // Look in a circle around the pixel with a radius of the border width  
							if (BorderTest(fieldindex, borderindex, x + x_circle[i], y + y_circle[i])) { // Test if pixel should be border
								m_land->Put(x, y, borderindex); // Add this pixel to the border element in the map
								area += 1;
								a_field->AddArea(-1.0); // Subtract area from field
								break; // Added border so stop testing
							}
						}
					}
				}
			}
		}
	}

	// If nothing was added to the border remove it again 
	if (area == 0) {
		border->SetMapValid(false);
		// If it is an unsprayed field margin also remove border from field 
		if (a_type == tole_UnsprayedFieldMargin) {
			a_field->SetUnsprayedMarginPolyRef(-1);
		}
	}
	else{
		border->AddArea(area);
	}

	return area;
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::BorderTest(int a_fieldindex, int a_borderindex, int a_x_neighbour, int a_y_neighbour) {
	// If the neighbouring cell is outside the landscape 
	if ((a_x_neighbour >= m_width) || (a_x_neighbour < 0) || (a_y_neighbour >= m_height) || (a_y_neighbour < 0)) {
		return true;
	}
	// If the neighbouring cell is not the field itself, a border, an individual tree, pylon or wind turbine 
	int index = m_land->Get(a_x_neighbour, a_y_neighbour);
	TTypesOfLandscapeElement tole = g_landscape_ptr->SupplyElementTypeFromVector(index);
	TTypesOfLandscapeElement border_tole = g_landscape_ptr->SupplyElementTypeFromVector(a_borderindex);
	if ((index != a_fieldindex) && (index != a_borderindex) && (tole != tole_IndividualTree) && (tole != tole_Pylon) && (tole != tole_WindTurbine) && (tole != tole_FieldBoundary) && (tole != border_tole) && (tole!= tole_Field || cfg_BordersBetweenFields.value())) {
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::CreatePondList()
{
	/** Just creates an unordered list of polyref numbers and m_elems indices for all ponds. This is for easy look-up by e.g. newts */
	for (unsigned int i = 0; i < m_elems.size(); i++)
	{
		if (m_elems[i]->GetElementType() == tole_Pond) {
			m_PondIndexList.push_back(i);
			m_PondRefsList.push_back(m_elems[i]->GetPoly());
		}
	}
}

int Landscape::SupplyRandomPondIndex() { 
	return m_PondIndexList[int(g_rand_uni_fnc()*m_PondIndexList.size())]; 
}

int Landscape::SupplyRandomPondRef() { 
	if (m_PondIndexList.size()>0) return m_PondRefsList[int(g_rand_uni_fnc()*m_PondIndexList.size())]; 
	return -1;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::AddBeetleBanks( TTypesOfLandscapeElement a_tole ) {
	// **************************************************************************************
	/**
	Beetle-bank addition - tests whether we can add a bank (or more) to this field, and then decides where to put it an adds it.
	*/
	// **************************************************************************************
		/**
	* For each element, if it is a field then assess whether should have a beetle bank.
	* This will depend on whether it is in the region defined for adding the bank, and a probability.
	* This code requires polygon centroids to be active, either calculated or via l_map_read_openness == true.
	*
	* To provide for more flexibilty, a tole_type is passed, and beetlebanks may be created of this tole_type instead of tole_BeetleBank
	*/
	int BBs=0;
	int tx1 = cfg_BeetleBankMinX.value();
	int tx2 = cfg_BeetleBankMaxX.value();
	int ty1 = cfg_BeetleBankMinY.value();
	int ty2 = cfg_BeetleBankMaxY.value();
	bool doit = false;
	unsigned sz=(unsigned) m_elems.size();

	std::vector<int> beetle_bank_area_per_farm(m_FarmManager->GetNoFarms(), 0);

	for (unsigned i=0; i<sz; i++){
		if (m_elems[ i ]->GetElementType() == tole_Field){
			doit = false;
			int cx = m_elems[ i ]->GetCentroidX();
			int cy = m_elems[ i ]->GetCentroidY();
			if (!cfg_BeetleBankInvert.value()){
				if ((cx >= tx1) && (cy >= ty1) && (cx <= tx2) && (cy <= ty2)){
					doit = true;
				}
			}
			else if ((cx < tx1) || (cy < ty1) || (cx > tx2) || (cy > ty2)){
				doit = true;
			}
			if (doit){
				int farm_id = m_elems[i]->GetOwnerIndex();
				int farm_area = m_FarmManager->GetFarmTotalSize(farm_id);
				// Field is in the right area, use chance to see if beetle bank should be added 
				if (g_random_fnc(100)<cfg_BeetleBankChance.value()){
					// Check if farm already has enough beetle bank area
					if (beetle_bank_area_per_farm[farm_id] < farm_area * cfg_FarmAreaPercentage.value() * 0.01) {
						int area_temp = AddBeetleBankIfPossible(m_elems[i], a_tole);
						if (area_temp > 0) {
							BBs++;
							beetle_bank_area_per_farm[farm_id] += area_temp;
						}
					}
				}
			}
		}
	}
	g_msg->Warn( WARN_MSG, "Landscape::AddBeetleBanks(): Fields with BeetleBanks successfully added: ", BBs);
}
//-----------------------------------------------------------------------------------------------------------------------------
// **************************************************************************************

int Landscape::AddBeetleBankIfPossible(LE* a_field, TTypesOfLandscapeElement a_tole) {
	/**
	Beetle bank placement rules are:\n
	Field is above cfg_BeetleBankMaxDistance (default = 150m) long and wide
	Beetle banks are place along the long direction of the field
	*/
	int cx = a_field->GetCentroidX();
	int cy = a_field->GetCentroidY();
	// The centroid is the only estimate we have (and it at least should be in the field).
	// So start here and find the centre
	if (!FindFieldCenter(a_field, &cx, &cy)) return -1;
	
	// Find the rectangle that describes the field the best
	// Returns the alignment of the longest axis in the rectangle 
	// and sets width and starting point on short side of best rectangle
	int width = 0;
	int x_start_perp = 0;
	int y_start_perp = 0;
	int alignment = FindBestRectangle(a_field->GetPoly(), &cx, &cy, &width, &x_start_perp, &y_start_perp);
	if (alignment == -1) return -1;
	// Get alignment of shortest axis (perpendicular to longest axis)
	int perp = alignment + 4;
  	if (perp >= 8) perp -= 8; 
	// Find the number of beetle banks (if any)
	if (width < cfg_BeetleBankMaxDistance.value()) return -1;
	int numberOfBanks = floor((float)width / cfg_BeetleBankMaxDistance.value());
	// Correct for the length of the step
	if (alignment == 2 || alignment == 6) width /= sqrt(2);
	if (alignment == 1 || alignment == 3 || alignment == 5 || alignment == 7) width /= sqrt(5);
	// Find the distance between the banks so they are equidistant 
	float distBetweenBanks = (float)width / (numberOfBanks+1);
	// Now create the banks along the longest axis of the best rectangle
	int beetle_bank_total_area = 0;
	for (int n = 1; n <= numberOfBanks; n++) {
		// Find the appropriate length and starting point 
		int x_start = x_start_perp - n * distBetweenBanks * m_x_add[perp];
		int y_start = y_start_perp - n * distBetweenBanks * m_y_add[perp];
		int x_end = x_start_perp - n * distBetweenBanks * m_x_add[perp];
		int y_end = y_start_perp - n * distBetweenBanks * m_y_add[perp];
		AxisLoop(a_field->GetPoly(), &x_start, &y_start, alignment);
		x_start -= (cfg_BeetleBankWidth.value() + 1) * m_x_add[alignment];
		y_start -= (cfg_BeetleBankWidth.value() + 1) * m_y_add[alignment];
		AxisLoop(a_field->GetPoly(), &x_end, &y_end, alignment + 8);
		x_end -= (cfg_BeetleBankWidth.value() + 1) * m_x_add[alignment + 8];
		y_end -= (cfg_BeetleBankWidth.value() + 1) * m_y_add[alignment + 8];
		APoint p_start(x_start, y_start);
		APoint p_end(x_end, y_end);
		int length = DistanceToP(p_start, p_end);
		// Correct for the length of the step
		if (alignment == 2 || alignment == 6) length /= sqrt(2);
		if (alignment == 1 || alignment == 3 || alignment == 5 || alignment == 7) length /= sqrt(5);
		
		// Make the new landscape element
		LE* BeetleBank;
		BeetleBank = NewElement(a_tole);
		BeetleBank->SetALMaSSEleType(g_letype->BackTranslateEleTypes(a_tole));
		BeetleBank->SetVegPatchy(true);
		m_polymapping[hb_first_free_poly_num] = (int)m_elems.size();
		m_elems.resize(m_elems.size() + 1);
		m_elems[m_elems.size() - 1] = BeetleBank;
		BeetleBank->SetMapIndex(hb_first_free_poly_num);
		BeetleBank->SetPoly(hb_first_free_poly_num++);
		BeetleBank->SetSlope(a_field->GetSlope());
		BeetleBank->SetAspect(a_field->GetAspect());
		BeetleBank->SetElevation(a_field->GetElevation());
		BeetleBank->SetOwner(a_field->GetOwner(), a_field->GetOwner()->GetFarmNumber(), a_field->GetOwnerIndex());
		int area = 0;
		int width = cfg_BeetleBankWidth.value();
		// Correct for the length of the step (opposite of length correction, to keep area constant)
		if (alignment == 2 || alignment == 6) width *= sqrt(2);
		if (alignment == 1 || alignment == 3 || alignment == 5 || alignment == 7) width *= sqrt(5);
		// Vectors are used to add all cells in the region even if the step along the longest direction is in both x and y at the same time
		// If the alignment is 45 degrees (or 135 degrees) the perpendicular step has 2 substeps
		// If the alignment is 22.5 degrees (or 67.5, 112.5, 157.5 degrees) the perpendicular step has 5 substeps 
		vector<vector<int>> x_direction = { {0},  { -1, -1, 0, 1, 0}, { -1, 0 }, { 0, 0, 1, 0, 1},  { -1 }, { 0, 0, -1, 0, -1 }, { 0, -1 }, { 1, 1, 0, -1, 0 } };
		vector<vector<int>> y_direction = { {-1}, { 0, 0, -1, 0, -1}, { 0, -1 }, { 1, 1, 0, -1, 0}, { 0 },  { 1, 1, 0, -1, 0 },  { 1, 0 }, { 0, 0, -1, 0, -1 } };
		vector<int> l_direction = { 1, 5, 2, 5, 1, 5, 2, 5 };
		int tx = 0;
		int ty = 0;

		int iteration = 0;
		for (int w = 0; w < width; w++) {
			if (w > 0) {
				if (w % l_direction[alignment] == 0) iteration++;
				tx += x_direction[alignment][w - iteration * l_direction[alignment]];
				ty += y_direction[alignment][w - iteration * l_direction[alignment]];
			}
			for (int i = 0; i < length; i++) {
				int x_temp = tx + x_start - i * m_x_add[alignment];
				int y_temp = ty + y_start - i * m_y_add[alignment];
				if ((x_temp >= 0) && (y_temp >= 0) && (x_temp < m_width) && (y_temp < m_height)) {
					// Only add to beetle bank if we are still in the field 
					if (a_field->GetPoly() == m_land->Get(x_temp, y_temp)) {
						m_land->Put(x_temp, y_temp, (int)m_elems.size() - 1);
						area += 1;
						a_field->AddArea(-1.0);
					}
				}
			}
		}
		BeetleBank->SetArea(double(area));
		beetle_bank_total_area += area;
		BeetleBank->SetValidXY(x_start + 0.5 * length * m_x_add[alignment], y_start + 0.5 * length * m_y_add[alignment]);
		BeetleBank->SetMapValid(true);
		
	}
	return beetle_bank_total_area;
}
//-----------------------------------------------------------------------------------------------------------------------------

bool Landscape::FindFieldCenter(LE* a_field, int* x, int* y) {
	// Start at x,y
	// works by selecting the point that is a mean of the co-ords of the centers of 8 axes from this point that are in the field.
	// Then do it again, and again until we don't move more than 1m or we have tried too many times

	int ourpoly=SupplyPolyRef(*(x),*(y));
	if (ourpoly!=a_field->GetPoly()) return false;
	int centers[2][16];
	int tries=0;
	int diff=999;
	int x1=*(x);
	int y1=*(y);
	int centreX=x1;
	int centreY=y1;
	// NB we might escape without bounds checking here because the polygon number does not wrap round - will only ever be a problem if we go SimX+1,SimY+1
	while ((diff>1) && (tries++<100)) {
		for (unsigned v=0; v<8; v++) {
			x1=centreX;
			y1=centreY;
			AxisLoop(ourpoly, &x1, &y1, v);
			centers[0][v]=x1-m_x_add[v];
			centers[1][v]=y1-m_y_add[v];
			x1=centreX;
			y1=centreY;
			AxisLoop(ourpoly, &x1, &y1, v+8);
			centers[0][v+8]=x1-m_x_add[v+8];
			centers[1][v+8]=y1-m_y_add[v+8];
		}
		int oldx=centreX;
		int oldy=centreY;
		centreX=0;
		centreY=0;
		for (int h=0; h<16; h++) {
			centreX+=centers[0][h];
			centreY+=centers[1][h];
		}
		centreX/=16;
		centreY/=16;
		diff=abs(oldx-centreX)+abs(oldy-centreY);
	}
	*(x)=centreX;
	*(y)=centreY;
	int tourpoly=SupplyPolyRef(*(x),*(y));
	if (tourpoly!=ourpoly) {
		return false; // can happen eg if there is a pond in the middle of the field
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------

int Landscape::FindBestRectangle(int a_polyindex, int* a_x, int* a_y, int* a_width, int* a_x_start, int* a_y_start)
{
	/**
	This method finds the rectangle that describes the field the best, which means the orientation of the rectangle 
	that gives the biggest in-field area to the total area of the rectangle. The orientation of the longest axis of 
	the rectangle is returned. 
	The width (lenght of shortest axis) as well as starting point (x and y) on the short side of the rectangle is also saved.
	*/
	int ourpoly = SupplyPolyRef(*(a_x), *(a_y));
	int dist[8];
	int dist_steps[8];
	int distx[16];
	int disty[16];
	int found = -1;
	int perp = -1;
	int max_area = 0;
	int fx[16];
	int fy[16];
	// Find the distance from the center to the edge in 16 different directions (spaced with 22.5 degrees)
	for (unsigned v = 0; v < 16; v++){
		int x1 = *(a_x);
		int y1 = *(a_y);
		AxisLoop(ourpoly, &x1, &y1, v);
		x1 -= m_x_add[v];
		y1 -= m_y_add[v];
		fx[v] = x1;
		fy[v] = y1;
		distx[v] = abs(*(a_x)-x1);
		disty[v] = abs(*(a_y)-y1);
	}
	// Calculate the length of each of the 8 axis
	for (int di = 0; di < 8; di++){
		int ddx = distx[di] + distx[di + 8];
		int ddy = disty[di] + disty[di + 8];
		int temp_dist = sqrt(pow(ddx, 2) + pow(ddy, 2));
		dist[di] = temp_dist;
		if (di == 2 || di == 6)  dist_steps[di] = temp_dist / sqrt(2);
		else if (di == 1 || di == 3 || di == 5 || di == 7)  dist_steps[di] = temp_dist / sqrt(5);
		else dist_steps[di] = temp_dist;
	}
	// Calculate the area to decide which rectangle is the best
	double area_fraction = 0;
	for (int i = 0; i < 4; i++){
		// Check how much of area is belonging to polygon 
		int in_area = 0;
		int out_area = 0;
		// Loop over every cell in rectangle 
		for (int l = 0; l < dist_steps[i]; l++) {
			int temp_x_axis1 = fx[i] - l * m_x_add[i];
			int temp_y_axis1 = fy[i] - l * m_y_add[i];
			for (int h = 0; h < dist_steps[i + 4]; h++) {
				int temp_x_axis2 = temp_x_axis1 - distx[i + 4] - h * m_x_add[i + 4];
				int temp_y_axis2 = temp_y_axis1 - disty[i + 4] - h * m_y_add[i + 4];
				if ((temp_x_axis2 >= 0) && (temp_y_axis2 >= 0) && (temp_x_axis2 < m_width) && (temp_y_axis2 < m_height)) {
					if (a_polyindex == m_land->Get(temp_x_axis2, temp_y_axis2)) in_area++;
					else out_area++;
				}
			}
		}
		// Check which rectangle has the best area ratio
		double temp_frac = (double)in_area / (in_area + out_area);
		if (temp_frac > area_fraction) {
			area_fraction = temp_frac;
			// Check which side is the longest
			if (dist[i] > dist[i + 4]) { found = i; perp = i + 4; }
			else { found = i + 4; perp = i; }
			// Save width and starting point on short side of rectangle
			*a_width = dist[perp];
			*a_x_start = fx[perp];
			*a_y_start = fy[perp];
		}
	}
	return found;
}

int Landscape::FindLongestAxis(int* a_x, int* a_y) 
{
	int ourpoly=SupplyPolyRef(*(a_x),*(a_y));
	int dist[8];
	int distx[16];
	int disty[16];
	int found = -1;
	double length = 0;
	int dx[16];
	int dy[16];
	int fx[16];
	int fy[16];
	for (unsigned v=0; v<16; v++) 
    {
		int x1=*(a_x);
		int y1=*(a_y);
		AxisLoop(ourpoly, &x1, &y1, v);
		x1 -= m_x_add[v];
		y1 -= m_y_add[v];
		dx[v] = abs(*(a_x)-x1);
		dy[v] = abs(*(a_y)-y1);
		fx[v] = x1;
		fy[v] = y1;
		distx[v] = dx[v];
		disty[v] = dy[v];
	}
	for (int di = 0; di < 8; di++)
	{
		int ddx = distx[di] + distx[di + 8];
		int ddy = disty[di] + disty[di + 8];
		dist[di] = sqrt(pow(ddx, 2) + pow(ddy, 2));
		if (dist[di] > length)
		{
			found = di;
			length = dist[di];
		}
	}
	// Correct for the length of the step
	if (found == 2 || found == 6 || found == 10 || found == 14) length /= sqrt(2);
	if (found == 1 || found == 3 || found == 5 || found == 7 || found == 9 || found == 11 || found ==13 || found == 15) length /= sqrt(5);

	if (found == -1) return 0;
    // Now need to find the middle of the axis.
	int l = length*0.5;
	if (fx[found] > fx[found + 8]) *(a_x) = fx[found + 8] + m_x_add[found] * l;  else *(a_x) = fx[found + 8] - m_x_add[found + 8] * l;
	if (fy[found] > fy[found + 8]) *(a_y) = fy[found + 8] + m_y_add[found] * l;  else *(a_y) = fy[found + 8] - m_y_add[found + 8] * l;

	return found;
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::AxisLoop(int a_polyindex, int* a_x, int* a_y, int a_axis) {
	/**
	* Starting at a_x,a_y each location is tested along a vector given by m_x_add & m_y_add until we step outside the polygon.
	* a_x & a_y are modified on return.
	*/
	int ap1 = a_polyindex;
	while (ap1 == a_polyindex)
	{
		*(a_x) += m_x_add[a_axis];
		*(a_y) += m_y_add[a_axis];
        // Before we try to get a polyindex from the map, check we are still on the world
		if (*(a_x) < 0) 
        {
			return;
		}
		if (*(a_y) < 0)
		{
			return;
		}
		if (*(a_x) >= m_width) 
		{
			return;
		}
		if (*(a_y) >= m_height) 
		{
			return;
		}
        // OK still in the map, get the polyindex
		ap1 = m_land->Get((*a_x), (*a_y)); // NB this returns the m_elemens index not the polyref (ChangeMapMapping has been called by here)
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

// **************************************************************************************
/* End beetle bank addition code */
// **************************************************************************************/

void Landscape::AddPatches(TTypesOfLandscapeElement a_tole) {
	// **************************************************************************************
	/**
	Patches addition - tests whether we can add a patch to this field, and then decides where to put it an adds it.
	*/
	// **************************************************************************************
	/**
	* For each element, if it is a field then assess whether should have a patch.
	* This will depend on the probabilities and the size of the field.
	* To provide for more flexibilty, a tole_type is passed, and patches may be created of any tole_type.
	*/
	int Npatches = 0;
	unsigned sz = (unsigned)m_elems.size();

	std::vector<int> patches_area_per_farm(m_FarmManager->GetNoFarms(), 0);

	for (unsigned i = 0; i < sz; i++) {
		if (m_elems[i]->GetElementType() == tole_Field) {
			int farm_id = m_elems[i]->GetOwnerIndex();
			int farm_area = m_FarmManager->GetFarmTotalSize(farm_id);
			// Check if farm already has enough beetle bank area
			if (patches_area_per_farm[farm_id] < farm_area * cfg_FarmAreaPercentage.value() * 0.01) {
				int area_temp = AddPatchIfPossible(m_elems[i], a_tole);
				if (area_temp > 0) {
					Npatches++;
					patches_area_per_farm[farm_id] += area_temp;
				}
			}
		}
	}
	g_msg->Warn(WARN_MSG, "Landscape::AddPatches(): Fields with patch successfully added: ", Npatches);
}

int Landscape::AddPatchIfPossible(LE* a_field, TTypesOfLandscapeElement a_tole) {
	/**
	Patch placement rules are:\n
	The field is above 1 / (cfg_PatchesFieldAreaMaxPercentage * 0.01) * cfg_PatchesMinWidth ** 2 m2 (default is 1/0.15 * 10**2 = 666 m2.
	The patch can not be bigger than cfg_PatchesMaxWidth ** 2 m2 (default is 100**2 = 10.000 m2)
	Patches are place in the middle of the field
	*/

	// Check if field is to small to add patch
	if (a_field->GetArea() * cfg_PatchesFieldAreaMaxPercentage.value() * 0.01 < pow(cfg_PatchesMinWidth.value(),2))
	{
		cout << "Field area of " << a_field->GetArea() << " is to small for a patch" << endl;
		return -1;
	}
	int cx = a_field->GetCentroidX();
	int cy = a_field->GetCentroidY();
	// The centroid is the only estimate we have (and it at least should be in the field).
	// So start here and find the centre
	if (!FindFieldCenter(a_field, &cx, &cy)) return -1;

	// Make the new landscape element
	LE* Patch;
	Patch = NewElement(a_tole);
	Patch->SetALMaSSEleType(g_letype->BackTranslateEleTypes(a_tole));
	Patch->SetVegPatchy(true);
	m_polymapping[hb_first_free_poly_num] = (int)m_elems.size();
	m_elems.resize(m_elems.size() + 1);
	m_elems[m_elems.size() - 1] = Patch;
	Patch->SetMapIndex(hb_first_free_poly_num);
	Patch->SetPoly(hb_first_free_poly_num++);
	Patch->SetSlope(a_field->GetSlope());
	Patch->SetAspect(a_field->GetAspect());
	Patch->SetElevation(a_field->GetElevation());
	Patch->SetOwner(a_field->GetOwner(), a_field->GetOwner()->GetFarmNumber(), a_field->GetOwnerIndex());
	int area = 0;

	// Add patch by spiraling out from the center cx, cy
	int x = 0, y = 0, dx = 0;
	int dy = -1;
	int t = cfg_PatchesMaxWidth.value();
	int temp;
	for (int i = 0; i < pow(t,2); i++) {
		//cout << x << "," << y << endl;
		if ((-t / 2 <= x) && (x <= t / 2) && (-t / 2 <= y) && (y <= t / 2)) {
			// Only add to patch if we are still in the field
			if ((cx + x >= 0) && (cy + y >= 0) && (cx + x < m_width) && (cy + y < m_height)) {
				if (a_field->GetPoly() == m_land->Get(cx + x, cy + y)) {
					m_land->Put(cx + x, cy + y, (int)m_elems.size() - 1);
					area += 1;
					a_field->AddArea(-1.0);
					// Check that the area is less than cfg_PatchesFieldAreaMaxPercentage or exit for-loop
					if (area > a_field->GetArea() * cfg_PatchesFieldAreaMaxPercentage.value() * 0.01) break;
				}
			}
		}
		if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))) {
			temp = -dy;
			dy = dx;
			dx = temp;
		}
		x += dx;
		y += dy;
	}
	Patch->SetArea(double(area));
	Patch->SetValidXY(cx, cy);
	Patch->SetMapValid(true);

	return area;
}

void Landscape::SkylarkEvaluation(SkTerritories* a_skt) {
	for (unsigned i=0; i<m_elems.size(); i++) {
		a_skt->PreCachePoly(m_elems[i]->GetPoly());
	}
}

void Landscape::CalculateCentroids()
{
	/**
	* Finds a location inside each polygon as a roughly calculated centre point.
	* The point will be within the polygon.
	* This also uses the stored Max/Min coordinates for each polygon that form a rectangle around it.
	*/
	cout << "In Centroid Calculations" << endl;
	// For each polygon
	for (int p = 0; p< (int)m_elems.size(); p++)
	{
		// Calcuate the actual centre
		int x1 = m_elems[p]->GetMinX();
		int y1 = m_elems[p]->GetMinY();
		int x2 = m_elems[p]->GetMaxX();
		int y2 = m_elems[p]->GetMaxY();

		int midx = (x1 + x2) / 2;
		int midy = (y1 + y2) / 2;
		// Now from midx & midy we move outwards in concentric circles until we find a location that matches our polyref.
		int polyindex = p; // Change mapmapping has been called by now, so the map contains m_elems indices.
		CentroidSpiralOut(polyindex, midx, midy);
		// Now we want to be sure that we are in the middle of the polygon not on the edge. This is tricky for complex shaped polygons, 
		// but we have a stab at it by using the FindLongestAxis method. This puts us in the centre of the longest axis in 8 directions
		// from this point
		FindLongestAxis(&midx, &midy);
		m_elems[p]->SetCentroid(midx, midy);
	}
	BuildingDesignationCalc();
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::CentroidSpiralOut(int a_polyref, int &a_x, int &a_y)
{
	if (SupplyPolyRefIndex(a_x, a_y) == a_polyref) return; // Found it so return
	// Otherwise its not found so we need to start to spiral out
	int loop = 1;
	int sx = a_x;
	int sy = a_y;
	do {
		a_y = sy - loop;
		for (int i = 0 - loop; i <= loop; i++)
		{
			a_x = sx + i;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
				return; // Found it so return
		}
		a_y = sy + loop;
		for (int i = 0 - loop; i <= loop; i++)
		{
			a_x = sx + i;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
				return; // Found it so return
		}
		a_x = sx + loop;
		for (int j = 0 - (loop - 1); j< loop; j++)
		{
			a_y = sy + j;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
				return; // Found it so return
		}
		a_x = sx - loop;
		for (int j = 0 - (loop - 1); j< loop; j++)
		{
			a_y = sy + j;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
				return; // Found it so return
		}
		loop++;
	} while (loop<m_minmaxextent); // This stopping rule should hopefully not be needed, it is set very high.
	g_msg->Warn("Landscape::CentroidSpiralOut: Failure of centroid main loop. Looking for polygon index ",a_polyref);
	a_x = m_elems[a_polyref]->GetMinX();
	a_y = m_elems[a_polyref]->GetMinY();
}
//-----------------------------------------------------------------------------------------------------------------------------
/* Not used
void Landscape::CentroidSpiralOutBlocks(int a_polyref, int &a_x, int &a_y)
{
	if (SupplyPolyRefIndex(a_x, a_y) == a_polyref) return; // Found it so return
	// Otherwise its not found so we need to start to spiral out until we find a 10x10m block of our field.
	int loop = 1;
	int sx = a_x;
	int sy = a_y;
	do {
		a_y = sy - loop;
		for (int i = 0 - loop; i <= loop; i++)
		{
			a_x = sx + i;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
			{
				APoint pt(a_x, a_y);
				double dists = -1;
				double best = -1;
				int ind = -1;
				for (int d = 0; d < 8; d++)
				{
					AxisLoopLtd(a_polyref, &pt, d,10);
#ifdef __BORLANDC__
					dists = sqrt(double((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y)));
#else
					//dists = sqrt((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y));
					dists = (abs(a_x - pt.m_x) + abs(a_y - pt.m_y));
#endif
					if (dists > best) { best = dists; ind = d; }
				}
				if (ind > -1)
				{
					//					best /= 2;
					if (best >= 10)
					{
						a_x += m_x_add[ind] * 10;
						a_y += m_y_add[ind] * 10;
						return;
					}
				}
				return; // Found it so return even without the 10 distance
			}
		}
		a_y = sy + loop;
		for (int i = 0 - loop; i <= loop; i++)
		{
			a_x = sx + i;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
			{
				APoint pt(a_x, a_y);
				double dists = -1;
				double best = -1;
				int ind = -1;
				for (int d = 0; d < 8; d++)
				{
					AxisLoopLtd(a_polyref, &pt, d,10);
#ifdef __BORLANDC__
					dists = sqrt(double((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y)));
#else
					//dists = sqrt((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y));
					dists = (abs(a_x - pt.m_x) + abs(a_y - pt.m_y));
#endif
					if (dists > best) { best = dists; ind = d; }
				}
				if (ind > -1)
				{
					//					best /= 2;
					if (best >= 10)
					{
						a_x += m_x_add[ind] * 10;
						a_y += m_y_add[ind] * 10;
						return;
					}
				}
				return; // Found it so return even without the 10 distance
			}
		}
		a_x = sx + loop;
		for (int j = 0 - (loop - 1); j< loop; j++)
		{
			a_y = sy + j;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
			{
				APoint pt(a_x, a_y);
				double dists = -1;
				double best = -1;
				int ind = -1;
				for (int d = 0; d < 8; d++)
				{
					AxisLoopLtd(a_polyref, &pt, d,10);
#ifdef __BORLANDC__
					dists = sqrt(double((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y)));
#else
					//dists = sqrt((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y));
					dists = (abs(a_x - pt.m_x) + abs(a_y - pt.m_y));
#endif
					if (dists > best) { best = dists; ind = d; }
				}
				if (ind > -1)
				{
					//					best /= 2;
					if (best >= 10)
					{
						a_x += m_x_add[ind] * 10;
						a_y += m_y_add[ind] * 10;
						return;
					}
				}
				return; // Found it so return even without the 10 distance
			}
		}
		a_x = sx - loop;
		for (int j = 0 - (loop - 1); j< loop; j++)
		{
			a_y = sy + j;
			CorrectCoords(a_x, a_y);
			if (SupplyPolyRefIndex(a_x, a_y) == a_polyref)
			{
				APoint pt(a_x, a_y);
				double dists = -1;
				double best = -1;
				int ind = -1;
				for (int d = 0; d < 8; d++)
				{
					AxisLoopLtd(a_polyref, &pt, d,10);
#ifdef __BORLANDC__
					dists = sqrt(double((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y)));
#else
					//dists = sqrt((a_x - pt.m_x)*(a_x - pt.m_x) + (a_y - pt.m_y) * (a_y - pt.m_y));
					dists = (abs(a_x - pt.m_x) + abs(a_y - pt.m_y));
#endif
					if (dists > best) { best = dists; ind = d; }
				}
				if (ind > -1)
				{
					//					best /= 2;
					if (best >= 10)
					{
						a_x += m_x_add[ind] * 10;
						a_y += m_y_add[ind] * 10;
						return;
					}
				}
				return; // Found it so return even without the 10 distance
			}
		}
		loop++;
	} while (loop<m_width); // This stopping rule should hopefully not be needed, it is set very high.
	exit(0);
}
//-----------------------------------------------------------------------------------------------------------------------------*/

void Landscape::DumpCentroids()
{
	ofstream centroidfile("PolygonCentroids.txt", ios::out);
	centroidfile<<"Polyref"<<'\t'<<"CX"<<'\t'<<"CY"<<'\t'<<"Type"<<'\t'<<"Area"<<'\t'<<"Country Designation"<<endl;
	for (int p = 0; p< (int)m_elems.size(); p++)
	{
		centroidfile<<m_elems[p]->GetPoly()<<'\t'<<m_elems[p]->GetCentroidX()<<'\t'<<m_elems[p]->GetCentroidY()<<'\t'<<m_elems[p]->GetElementType()<<'\t'<<m_elems[p]->GetArea()<<'\t'<<m_elems[p]->GetCountryDesignation()<<endl;
	}
	centroidfile.close();
}
//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::BuildingDesignationCalc()
{
	/** 
	* Runs through all elements and identifies the ones where rodenticide bait may be placed. If it is a building then
	* we count the number of buildings near to it and designate it town if there are more than #cfg_mintownbuildingnumber. 
	*/
	cout << "In BuildingDesignationCalc" << endl;
	for (int p = 0; p< (int)m_elems.size(); p++)
	{
		TTypesOfLandscapeElement tole = m_elems[p]->GetElementType();
		if ( tole == tole_Building)
		{
			int cx = m_elems[p]->GetCentroidX();
			int cy = m_elems[p]->GetCentroidY();
			int near = 0;
			for (int j = 0; j< (int)m_elems.size(); j++)
			{
				if (m_elems[j]->GetElementType() == tole_Building)
				{
					int nx = m_elems[j]->GetCentroidX();
					int ny = m_elems[j]->GetCentroidY();
					int dx =abs(cx-nx);
					int dy =abs(cy-ny);
					if ((dx < cfg_mintownbuildingdistance.value()) && (dy < cfg_mintownbuildingdistance.value())) near++;
					if (near > cfg_mintownbuildingdistance.value()) break;
				}
			}
			if (near <= cfg_mintownbuildingnumber.value()) m_elems[p]->SetCountryDesignation(1); // Not enough buildings close by, so it is a country building
			else m_elems[p]->SetCountryDesignation(0);
		}
		else if (tole == tole_YoungForest) 
		{
			m_elems[p]->SetCountryDesignation(2);
		}
		else if ((tole == tole_DeciduousForest) || ( tole == tole_MixedForest) || ( tole == tole_ConiferousForest ) ) m_elems[p]->SetCountryDesignation(3);
			
	}
}
//-----------------------------------------------------------------------------------------------------------------------------


void Landscape::RecordGooseNumbers(int a_polyref, int a_number)
{
	/**
	* This records the number of geese on the polygon the day before. To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	* simulation day number can be used to modify the return value - see GetGooseNumbers() \n
	* Note that the record is the for the day before because this method is called by DoFirst, so there are no geese today
	*/
	int day = SupplyDayInYear();
	m_elems[m_polymapping[a_polyref]]->SetGooseNos(a_number, day);
}

//-----------------------------------------------------------------------------------------------------------------------------

void Landscape::RecordGooseNumbersTimed(int a_polyref, int a_number)
{
	/**
	* This records the number of geese on the polygon the day before at a predefined time. To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	* simulation day number can be used to modify the return value - see GetGooseNumbers() \n
	*/
	int day = SupplyDayInYear();
	m_elems[m_polymapping[a_polyref]]->SetGooseNosTimed(a_number, day);
}

//-----------------------------------------------------------------------------------------------------------------------------
void Landscape::RecordGooseSpNumbers(int a_polyref, int a_number, GooseSpecies a_goose) {
	  /**
	  * This records the number of geese on the polygon the day before by species.
	  * To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	  * simulation day number can be used to modify the return value - see GetGooseNumbers() \n
	  * Note that the record is the for the day before because this method is called by DoFirst, so there are no geese today
	  */
	  int day = SupplyDayInYear();
	  m_elems[m_polymapping[a_polyref]]->SetGooseSpNos(a_number, day, a_goose);
  }
  //-----------------------------------------------------------------------------------------------------------------------------
  void Landscape::RecordGooseSpNumbersTimed(int a_polyref, int a_number, GooseSpecies a_goose) {
	  /**
	  * This records the number of geese on the polygon the day before at a predefined time by species.
	  * To prevent lots of unnecessary clearing of values two values are saved, the number and simulation day - on reading the
	  * simulation day number can be used to modify the return value - see GetGooseNumbers() \n
	  */
	  int day = SupplyDayInYear();
	  m_elems[m_polymapping[a_polyref]]->SetGooseSpNosTimed(a_number, day, a_goose);
  }
  //-----------------------------------------------------------------------------------------------------------------------------
  void Landscape::RecordGooseRoostDist(int a_polyref, int a_dist, GooseSpecies a_goose) {
	  /**
	  * This records the distance to the closest roost for a goose species.
	  */
	  m_elems[m_polymapping[a_polyref]]->SetGooseRoostDist(a_dist, a_goose);
  }
  //-----------------------------------------------------------------------------------------------------------------------------
  /**
  * \brief This returns the number of geese on the polygon the day before.
  */
  int Landscape::GetGooseNumbers(int a_polyref) {
	  return m_elems[m_polymapping[a_polyref]]->GetGooseNos();
  }
  //-----------------------------------------------------------------------------------------------------------------------------
  /**
  * \brief This returns the number of geese on the polygon the day before.
  */
  int Landscape::GetQuarryNumbers(int a_polyref) {
	  return m_elems[m_polymapping[a_polyref]]->GetQuarryNos();
  }
  //-----------------------------------------------------------------------------------------------------------------------------
    /**
  * \brief This returns the number of geese on the polygon the day before.
  */
  int Landscape::GetGooseNumbers(int a_x, int a_y)
  {
	  return m_elems[m_land->Get(a_x, a_y)]->GetGooseNos();
  }
  //-----------------------------------------------------------------------------------------------------------------------------

void Landscape::GISASCII_Output(string outpfile, int UTMX, int UTMY) {
	/** Here we write a ASCII file of the current map. Useful when visualizing output from
	simulations. The function will output the entity that is defined in the config:
	l_map_ascii_map_entity. The default is polyref number (l_map_ascii_map_entity = 1).
	\param [in] outpfile Name of the output file
	\param [in] UTMX Utm x-coordinate of the lower lefthand corner of the map
	\param [in] UTMY Utm y-coordinate of the lower lefthand corner of the map
	*/
	ofstream OFILE;
	OFILE.open(outpfile.c_str());
	if (!OFILE) {
		g_msg->Warn(WARN_FILE, "Landscape::GISASCII_Output() "
			"Unable to open file for writing:",
			outpfile);
		exit(1);
	}
	OFILE << "ncols          " << m_width << endl;
	OFILE << "nrows          " << m_height << endl;
	OFILE << "xllcorner      " << UTMX << endl;
	OFILE << "yllcorner      " << UTMY << endl;
	OFILE << "cellsize       " << 1 << endl;
	OFILE << "NODATA_value   " << -9999 << endl;
	// The polyref loop
	if (l_map_ascii_map_entity.value() == 1) {
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				OFILE << '\t' << SupplyPolyRef(x, y);
			}
			OFILE << endl;
		}
	}
	// The element type loop
	if (l_map_ascii_map_entity.value() == 2) {
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				OFILE << '\t' << int(SupplyElementType(x, y));
			}
			OFILE << endl;
		}
		OFILE.close();
	}
}

void Landscape::SupplyLEReset(void) {
	le_signal_index = 0;
}

int Landscape::SupplyLENext(void) {
	if ((unsigned int)le_signal_index == m_elems.size()) {
		return -1;
	}
	return m_elems[le_signal_index++]->GetPoly();
}

int Landscape::SupplyLECount(void) {
	return (int)m_elems.size();
}

LE_Signal Landscape::SupplyLESignal(int a_polyref) {
	return m_elems[m_polymapping[a_polyref]]->GetSignal();
}

void Landscape::SetLESignal(int a_polyref, LE_Signal a_signal) {
	m_elems[m_polymapping[a_polyref]]->SetSignal(a_signal);
}


void Landscape::IncTreatCounter(int a_treat) {
	if (a_treat < 0 || a_treat >= last_treatment) {
		char errornum[20];
		sprintf(errornum, "%d", a_treat);
		g_msg->Warn(WARN_BUG, "Landscape::IncTreatCounter(): Index"" out of range!", errornum);
		exit(1);
	}
	m_treatment_counts[a_treat] ++;
}


void Landscape::DumpTreatCounters(const char * a_filename) {
	FILE * l_file = fopen(a_filename, "w");
	if (!l_file) {
		g_msg->Warn(WARN_FILE, "Landscape::DumpTreatCounters(): ""Unable to open file for writing: %s\n", a_filename);
		exit(1);
	}

	for (int i = start; i < last_treatment; i++) {
		fprintf(l_file, "%3d %s %10d\n", i, EventtypeToString(i).c_str(), m_treatment_counts[i]);
	}
	fclose(l_file);
}

void Landscape::DumpMapGraphics(const char * a_filename) {
	unsigned int linesize = m_maxextent * 3;
	unsigned char * frame_buffer = (unsigned char *)malloc(sizeof(unsigned char)* linesize);

	if (frame_buffer == NULL) {
		g_msg->Warn(WARN_FILE, "Landscape::DumpMapGraphics(): Out of memory!", "");
		exit(1);
	}

	FILE * l_file = fopen(a_filename, "w");
	if (!l_file) {
		g_msg->Warn(WARN_FILE, "Landscape::DumpMapGraphics(): ""Unable to open file for writing: %s\n", a_filename);
		exit(1);
	}

	fprintf(l_file, "P6\n%d %d %d\n", m_width, m_height, 255);

	for (int y = 0; y < m_height; y++) {
		int i = 0;
		for (int x = 0; x < m_width; x++) {
			int eletype = (int)SupplyElementType(x, y);
			int localcolor = 16777215 / eletype;

			if (eletype == (int)tole_Field) {
				int category;
				double hei = SupplyVegHeight(x, y);
				if (hei > 50.0) category = 0; else category = (int)(200.0 - (hei * 4.0));
				localcolor = ((category * 65536) + 65535);
			}

			frame_buffer[i++] = (unsigned char)(localcolor & 0xff);
			frame_buffer[i++] = (unsigned char)((localcolor >> 8) & 0xff);
			frame_buffer[i++] = (unsigned char)((localcolor >> 16) & 0xff);
		}
		fwrite(frame_buffer, sizeof(unsigned char), linesize, l_file);
	}

	fclose(l_file);

	free(frame_buffer);
}

void Landscape::FillVegAreaData() {
	for (unsigned int i = 0; i < (tov_Undefined + 1); i++) {
		l_vegtype_areas[i] = 0.0;
	}

	// Sum up statistics on element type.
	for (unsigned int i = 0; i < m_elems.size(); i++) {
		l_vegtype_areas[m_elems[i]->GetVegType()] += m_elems[i]->GetArea();
	}
}

void Landscape::DumpMapInfoByArea(const char * a_filename, bool a_append, bool a_dump_zero_areas, bool a_write_veg_names) {
	FillVegAreaData();
	FILE * outf;
	if (a_append) {
		outf = fopen(a_filename, "a");
		if (!outf) {
			g_msg->Warn(WARN_FILE, "Landscape::DumpMapInfoByArea(): ""Unable to open file for appending", a_filename);
			exit(1);
		}
	}
	else {
		outf = fopen(a_filename, "w");
		if (!outf) {
			g_msg->Warn(WARN_FILE, "Landscape::DumpMapInfoByArea(): ""Unable to open file for writing", a_filename);
			exit(1);
		}
	}

	// Emit element type info.
	for (unsigned int i = 0; i < tov_Undefined + 1; i++) {
		if (i == tov_OFirstYearDanger)
			continue;
		if (!a_dump_zero_areas && l_vegtype_areas[i] < 0.5)
			continue;

		fprintf(outf, "Date: %6ld\tVeg Type: %3d\tArea: %10.0f", g_date->OldDays() + g_date->DayInYear() - 364, i, l_vegtype_areas[i]);
		if (a_write_veg_names)
			fprintf(outf, "\tVeg Name:%s\n", VegtypeToString((TTypesOfVegetation)i).c_str()); 
		else
			fprintf(outf, "\n");
	}

	fclose(outf);
}



LE * Landscape::NewElement(TTypesOfLandscapeElement a_type) {
	/**
	* Here we reduce the amount of classes that need to be created by lumping those with no special, or the same behaviour into the base class, and then differentiating with 
	* an attribute specifying the tole type.
	*/
	LE * elem;
	int subtype = 1;

	switch (a_type) {
    // NonVegetated Elements
	case tole_BareRock:
	case tole_AmenityGrass:
	case tole_Fence:
	case tole_Parkland:
	case tole_Saltwater:
	case tole_River:
	case tole_SandDune:
	case tole_Track:
	case tole_MetalledPath:
	case tole_Stream:
	case tole_HeritageSite:
	case tole_RiverBed:
	case tole_RefuseSite:
	case tole_Coast:
	case tole_Chameleon:
	case tole_Carpark:
	case tole_Churchyard:
	case tole_FishFarm:
	case tole_Canal:
	case tole_Freshwater:
	case tole_BuiltUpWithParkland:
	case tole_PitDisused:
	case tole_UrbanNoVeg:
	case tole_UrbanVeg:
	case tole_StoneWall:
	case tole_WindTurbine:
	case tole_Pylon:
	case tole_ActivePit:
	case tole_Airport:
	case tole_Portarea:
	case tole_Saltpans:
	case tole_Pipeline:
		elem = new NonVegElement(a_type, this);
		break;
	case tole_DrainageDitch:
	case tole_Garden:
	case tole_UrbanPark:
		elem = new VegElement(a_type, this);
		break;
	case tole_Building:
		elem = new NonVegElement(a_type, this);
		elem->SetCountryDesignation(0); // default = 0 = town
		break;
	case tole_SmallRoad:
		elem = new SmallRoad(this);
		break;
	case tole_LargeRoad:
		elem = new LargeRoad(this);
		break;
	case tole_Pond:
		elem = new Pond(this);
		break;
	// Vegetated Elements
	case tole_Railway:
	case tole_RiversidePlants:
		elem = new VegElement(a_type, this);
		// default tov_noGrowth in the constructor
		break;
	case tole_WoodyEnergyCrop: // not really wused 
	case tole_DeciduousForest:
	case tole_MixedForest:
	case tole_ConiferousForest:
	case tole_Copse:
	case tole_IndividualTree:
	case tole_WoodlandMargin:
	case tole_Scrub:
	case tole_SwampForest:
	case tole_MontadoCorkOak:
	case tole_MontadoHolmOak:
	case tole_MontadoMixed:
	case tole_AgroForestrySystem:
	case tole_CorkOakForest:
	case tole_HolmOakForest:
	case tole_OtherOakForest:
	case tole_ChestnutForest:
	case tole_EucalyptusForest:
	case tole_InvasiveForest:
	case tole_MaritimePineForest:
	case tole_StonePineForest:
	case tole_RiversideTrees:
		elem = new ForestElement(a_type, this);
		break;	
	case tole_YoungForest:
		// Vegetation type set in Farm InitiateManagement
		elem = new ForestElement(a_type, this);
		elem->SetVegPatchy(true);
		break;
	case tole_PlantNursery:
		// Vegetation type set in Farm InitiateManagement
		elem = new VegElement(a_type, this);		
		break;
	case tole_Vildtager:
		elem = new VegElement(a_type, this);
		elem->SetVegType(tov_NaturalGrass);
		break;
	case tole_ForestAisle:
	case tole_SolarPanel:
		elem = new VegElement(a_type, this);
		elem->SetVegType(tov_NaturalGrass);
		elem->SetVegPatchy(true);
		break;
	case tole_Marsh:
	case tole_Saltmarsh:
		elem = new VegElement(a_type, this);
		elem->SetVegType(tov_NaturalGrass);
		elem->SetVegGrowthScalerRand();
		break;
	case tole_Hedges:
		elem = new Hedges(this);
		break;
	case tole_HedgeBank:
		elem = new HedgeBank(this);
		break;
	case tole_BeetleBank:
		elem = new BeetleBank(this);
		break;
	case tole_FieldBoundary:
		elem = new FieldBoundary(this, a_type);
		//elem->SetVegType(tov_NaturalGrass);
		break;
	case tole_FlowerStrip:
		// Currently one third are given each subtype, but this can be altered afterwards using SetSubType
		if (g_rand_uni_fnc() < 0.33) subtype = 2; else if (g_rand_uni_fnc() < 0.33) subtype = 3;
		elem = new FlowerStrip(this,subtype);
		break;
	case tole_Orchard:
		// Vegetation type set in Farm InitiateManagement
		elem = new Orchard(this);
		break;
	case tole_OrchardBand:
		elem = new OrchardBand(this);
		break;
	case tole_MownGrassStrip:
		elem = new MownGrassStrip(this);
		break;
	case tole_Heath:
		elem = new VegElement(a_type, this);
		elem->SetVegType(tov_Heath);
		elem->SetVegPatchy(true);
		elem->SetVegGrowthScalerRand();
		break;
	case tole_Field:
		elem = new Field(this);
		break;
    // Other permament crops
	case  tole_Vineyard:
	case  tole_OliveGrove:
		elem = new VegElement(a_type, this);
		break;
	case  tole_RiceField:
	case  tole_AlmondPlantation:
	case  tole_WalnutPlantation:
	case  tole_FarmBufferZone:
	case  tole_NaturalFarmGrass:
	case  tole_GreenFallow:
	case  tole_FarmFeedingGround:
	case  tole_FlowersPerm:
	case  tole_AsparagusPerm:
	case  tole_OAsparagusPerm:
	case  tole_MushroomPerm:
	case  tole_OtherPermCrop:
		// lct: temporary, to develop (Vegetation type set in Farm InitiateManagement)
		elem = new VegElement(a_type, this); 
		//break;
	// Pastures and Grass  
	case tole_PermanentSetaside:
		// Vegetation type set in Farm InitiateManagement
		elem = new PermanentSetaside(this);
		break;
	case tole_PermPasture:
	case  tole_PermPasturePigs:
	case  tole_OPermPasturePigs:
	case  tole_OPermPasture:
		// Vegetation type set in Farm InitiateManagement
		elem = new PastureElement(a_type, this);
		elem->SetGrazingLevel(cfg_farm_cattle_grazing_lvl_pasture.value());
		break;
	case  tole_OPermPastureLowYield:
	case tole_PermPastureLowYield:
		// Vegetation type set in Farm InitiateManagement
		elem = new PastureElement(a_type, this);
		elem->SetGrazingLevel(cfg_farm_cattle_grazing_lvl_lowyield.value());
		break;
	case tole_PermPastureTussocky:
		// Vegetation type set in Farm InitiateManagement
		elem = new PastureElement(a_type, this);
		elem->SetVegPatchy(true);
		elem->SetDigestibility(0.8);
		elem->SetGrazingLevel(cfg_farm_cattle_grazing_lvl_tussocky.value());
		break;
	case tole_NaturalGrassDry:
	case tole_NaturalGrassWet:
	case tole_UnknownGrass:
		elem = new NaturalGrass(a_type, this);
		break;
	case  tole_OOrchard: // Vegetation type set in Farm InitiateManagement
	case  tole_BushFruit:
	case  tole_OBushFruit:
	case  tole_ChristmasTrees:
	case  tole_OChristmasTrees:
	case  tole_EnergyCrop:
	case  tole_OEnergyCrop:
	case  tole_FarmForest:
	case  tole_OFarmForest:
	case  tole_FarmYoungForest:
	case  tole_OFarmYoungForest:
		elem = new VegElement(a_type, this);
	case tole_Wasteland:
		elem = new NaturalGrass(a_type, this);
		elem->SetVegType(tov_Wasteland);
		elem->SetVegPatchy(true);
		break;
	case tole_WaterBufferZone:
		elem = new WaterBufferZone(this);
		break;
	case tole_RoadsideSlope:
		elem = new RoadsideVerge(a_type, this);
		break;
	case tole_RoadsideVerge:
		elem = new RoadsideVerge(a_type, this);
		elem->SetVegGrowthScalerRand();
		break;
	case tole_UnsprayedFieldMargin:
		elem = new UnsprayedFieldMargin(this);
		break;
	case tole_Missing:
		elem = new LE(this); // These should never be actually used.
		break;
	default:
		g_msg->Warn(WARN_FILE, "Landscape::NewElement(): Unknown landscape element requested:", int(a_type));
		exit(1);
	} //switch

	// Set the TOLE attributes
	Set_TOLE_Att(elem);
	// Set the external code 
	elem->SetALMaSSEleType(g_letype->BackTranslateEleTypes(a_type));
	return elem;
}

std::string Landscape::EventtypeToString(int a_event) {
	switch (a_event) {
	case start:
		return "start";
	case sleep_all_day:
		return "sleep_all_day";
	case autumn_plough:
		return "autumn_plough";
	case stubble_plough:
		return "stubble_plough";
	case stubble_cultivator_heavy:
		return "stubble_cultivator_heavy";
	case heavy_cultivator_aggregate:
		return "heavy_cultivator_aggregate";
	case autumn_harrow:
		return "autumn_harrow";
	case preseeding_cultivator:
		return "preseeding_cultivator";
	case preseeding_cultivator_sow:
		return "preseeding_cultivator_sow";
	case autumn_roll:
		return "autumn_roll";
	case autumn_sow:
		return "autumn_sow";
	case winter_plough:
		return "winter_plough";
	case winter_harrow:
		return "winter_harrow";
	case deep_ploughing:
		return "deep_ploughing";
	case spring_plough:
		return "spring_plough";
	case spring_harrow:
		return "spring_harrow";
	case spring_roll:
		return "spring_roll";
	case spring_sow:
		return "spring_sow";
	case spring_sow_with_ferti:
		return "spring_sow_with_ferti";
	case summer_plough:
		return "summer_plough";
	case summer_harrow:
		return "summer_harrow";
	case summer_sow:
		return "summer_sow";
	case fp_npks:
		return "fp_npks";
	case fp_npk:
		return "fp_npk";
	case fp_pk:
		return "fp_pk";
	case fp_p:
		return "fp_p";
	case fp_k:
		return "fp_k";
	case fp_sk:
		return "fp_sk";
	case fp_liquidNH3:
		return "fp_liquidNH3";
	case fp_slurry:
		return "fp_slurry";
	case fp_ammoniumsulphate:
		return "fp_ammoniumsulphate";
	case fp_manganesesulphate:
		return "fp_manganesesulphate";
	case fp_manure:
		return "fp_manure";
	case fp_greenmanure:
		return "fp_greenmanure";
	case fp_sludge:
		return "fp_sludge";
	case fp_rsm:
		return "fp_rsm";
	case fp_calcium:
		return "fp_calcium";
	case fa_npks:
		return "fa_npks";
	case fa_npk:
		return "fa_npk";
	case fa_pk:
		return "fa_pk";
	case fa_p:
		return "fa_p";
	case fa_k:
		return "fa_k";
	case fa_sk:
		return "fa_sk";
	case fa_slurry:
		return "fa_slurry";
	case fa_ammoniumsulphate:
		return "fa_ammoniumsulphate";
	case fa_manganesesulphate:
		return "fa_manganesesulphate";
	case fa_manure:
		return "fa_manure";
	case fa_greenmanure:
		return "fa_greenmanure";
	case fa_sludge:
		return "fa_sludge";
	case fa_rsm:
		return "fa_rsm";
	case fa_calcium:
		return "fa_calcium";
	case herbicide_treat:
		return "herbicide_treat";
	case growth_regulator:
		return "growth_regulator";
	case fungicide_treat:
		return "fungicide_treat";
	case insecticide_treat:
		return "insecticide_treat";
	case org_insecticide:
		return "organic insecticide";
	case org_herbicide:
		return "organic herbicide";
	case org_fungicide:
		return "organic fungicide";
	case product_treat:
		return "pesticide_product_treat";
	case syninsecticide_treat:
		return "syninsecticide_treat";
	case molluscicide:
		return "molluscicide";
	case row_cultivation:
		return "row_cultivation";
	case strigling:
		return "strigling";
	case flammebehandling:
		return "flammebehandling";
	case hilling_up:
		return "hilling_up";
	case water:
		return "water";
	case swathing:
		return "swathing";
	case harvest:
		return "harvest";
	case cattle_out:
		return "cattle_out";
	case cattle_out_low:
		return "cattle_out_low";
	case pigs_out:
		return "pigs_out";
	case cut_to_hay:
		return "cut_to_hay";
	case cut_to_silage:
		return "cut_to_silage";
	case straw_chopping:
		return "straw_chopping";
	case hay_turning:
		return "hay_turning";
	case hay_bailing:
		return "hay_bailing";
	case stubble_harrowing:
		return "stubble_harrowing";
	case autumn_or_spring_plough:
		return "autumn_or_spring_plough";
	case burn_straw_stubble:
		return "burn_straw_stubble";
	case burn_top:
		return "burn_top";
	case mow:
		return "mow";
	case cut_weeds:
		return "cut_weeds";
	case strigling_sow:
		return "strigling_sow";
	case trial_insecticidetreat:
		return "PesticideTrialTreatment";
	case trial_toxiccontrol:
		return "PesticideTrialToxic";
	case trial_control:
		return "PesticideTrialControl";
	case glyphosate:
		return "Glyphosate on setaside";
	case biocide:
		return "biocide";
	case strigling_hill:
		return "strigling_hill";
	case bed_forming:
		return "bed_forming";
	case flower_cutting:
		return "flower_cutting";
	case bulb_harvest:
		return "bulb_harvest";
	case straw_covering:
		return "straw_covering";
	case straw_removal:
		return "straw_removal";
	case pruning:
		return "pruning";
	case shredding:
		return "shredding";
	case green_harvest:
		return "green_harvest";
	case fiber_covering:
		return "				fiber_covering";
	case fiber_removal:
		return "				 fiber_removal";
	case shallow_harrow:
		return "shallow harrow";
	case fp_boron:
		return "fp_boron";
	case fp_n:
		return "fp_n";
	case fp_nk:
		return "fp_nk";
	case fa_nk:
		return "fa_nk";
	case fp_ns:
		return "fp_ns";
	case fp_nc:
		return "fp_nc";
	case autumn_sow_with_ferti:
		return "autumn_sow_with_ferti";
	case harvest_bushfruit:
		return "harvest_bushfruit";
	case fp_cu:
		return "fp_cu";
	case fa_n:
		return "fa_n";
	case fa_cu:
		return "fa_cu";
	case fa_boron:
		return "fa_boron";
	case fa_pks:
		return "fa_pks";
	case fp_pks:
		return "fp_pks";
	case harvestshoots:
		return "harvest_shoots";
	case manual_weeding:
		return "manual_weeding";
	case pheromone:
		return "pheromone";
	default:
		g_msg->Warn(WARN_FILE, "Landscape::EventtypeToString(): Unknown event type:", int(a_event));
		exit(1);
	}
}



std::string Landscape::PolytypeToString(TTypesOfLandscapeElement a_le_type) {

	switch (a_le_type) {
	case tole_Hedges:
		return "                Hedge";
	case tole_RoadsideVerge:
		return "       Roadside Verge";
	case tole_Railway:
		return "              Railway";
	case tole_FieldBoundary:
		return "       Field Boundary";
	case tole_Marsh:
		return "                Marsh";
	case tole_Scrub:
		return "                Scrub";
	case tole_Field:
		return "                Field";
	case tole_PermPastureTussocky:
		return " PermPastureTussocky";
	case tole_PermanentSetaside:
		return "   Permanent Setaside";
	case tole_PermPasture:
		return "    Permanent Pasture";
	case tole_PermPastureLowYield:
		return " PermPastureLowYield";
	case tole_NaturalGrassDry:
		return "        Natural Grass";
	case tole_NaturalGrassWet:
		return "    Natural Grass Wet";
	case tole_RiversidePlants:
		return "     Riverside Plants";
	case tole_PitDisused:
		return "          Pit Disused";
	case tole_RiversideTrees:
		return "      Riverside Trees";
	case tole_DeciduousForest:
		return "     Deciduous Forest";
	case tole_MixedForest:
		return "         Mixed Forest";
	case tole_ConiferousForest:
		return "    Coniferous Forest";
	case tole_YoungForest:
		return "         Young Forest";
	case tole_StoneWall:
		return "           Stone Wall";
	case tole_Garden:
		return "               Garden";
	case tole_Track:
		return "                Track";
	case tole_SmallRoad:
		return "           Small Road";
	case tole_LargeRoad:
		return "           Large Road";
	case tole_Building:
		return "             Building";
	case tole_ActivePit:
		return "           Active Pit";
	case tole_Pond:
		return "                 Pond";
	case tole_Freshwater:
		return "          Fresh Water";
	case tole_River:
		return "                River";
	case tole_Saltwater:
		return "            Saltwater";
	case tole_Coast:
		return "                Coast";
	case tole_BareRock:
		return "            Bare Rock";
	case tole_HedgeBank:
		return "            Hedgebank";
	case tole_Heath:
		return "                Heath";
	case tole_Orchard:
		return "              Orchard";
	case tole_OrchardBand:
		return "         Orchard Band";
	case tole_MownGrassStrip:
		return "     Mown Grass Strip";
	case tole_UnsprayedFieldMargin:
		return " UnsprayedFieldMargin";
	case tole_AmenityGrass:
		return "         AmenityGrass";
	case tole_Parkland:
		return "             Parkland";
	case tole_UrbanNoVeg:
		return "           UrbanNoVeg";
	case tole_UrbanVeg:
		return "           UrbanVeg";
	case tole_UrbanPark:
		return "            UrbanPark";
	case tole_BuiltUpWithParkland:
		return "  BuiltUpWithParkland";
	case tole_SandDune:
		return "             SandDune";
	case tole_Copse:
		return "                Copse";
	case tole_RoadsideSlope:
		return "        RoadsideSlope";
	case tole_MetalledPath:
		return "         MetalledPath";
	case tole_Carpark:
		return "             Carpark";
	case tole_Churchyard:
		return "          Churchyard";
	case tole_Saltmarsh:
		return "           Saltmarsh";
	case tole_Stream:
		return "              Stream";
	case tole_HeritageSite:
		return "        HeritageSite";
	case tole_BeetleBank:  //141
		return "         Beetle Bank";
	case tole_UnknownGrass:
		return "       Unknown Grass";
	case tole_Wasteland:
		return " Waste/Building Land";
	case tole_IndividualTree:
		return "      IndividualTree";
	case tole_PlantNursery:
		return "        PlantNursery";
	case tole_Vildtager:
		return "           Vildtager";
	case tole_WindTurbine:
		return "         WindTurbine";
	case tole_WoodyEnergyCrop:
		return "     WoodyEnergyCrop";
	case tole_WoodlandMargin:
		return "      WoodlandMargin";
	case tole_Pylon:
		return "               Pylon";
	case tole_FishFarm:
		return "            FishFarm";
	case tole_Fence:
		return "            Fence";
	case tole_WaterBufferZone:
		return "	Unsprayed buffer zone around water";
	case tole_DrainageDitch:
		return "		DrainageDitch";
	case tole_Vineyard:
		return "Vineyard";
	case tole_OliveGrove:
		return "OliveGrove";
	case tole_RiceField:
		return "Rice Field";
	case tole_MontadoCorkOak:
		return "MontadoCorkOak";
	case tole_MontadoHolmOak:
		return "MontadoHolmOak";
	case tole_MontadoMixed:
		return "MontadoMixed";
	case tole_AgroForestrySystem:
		return "AgroForestrySystem";
	case tole_CorkOakForest:
		return "CorkOakForest";
	case tole_HolmOakForest:
		return "HolmOakForest";
	case tole_OtherOakForest:
		return "OtherOakForest";
	case tole_ChestnutForest:
		return "ChestnutForest";
	case tole_EucalyptusForest:
		return "EucalyptusForest";
	case tole_MaritimePineForest:
		return "MaritimePineForest";
	case tole_StonePineForest:
		return "StonePineForest";
	case tole_InvasiveForest:
		return "InvasiveForest";
	case tole_Portarea:
		return "Portarea";
	case tole_Airport:
		return "Airport";
	case tole_Saltpans:
		return "Saltpans";
	case tole_SwampForest:
		return "SwampForest";
	case tole_Pipeline:
		return "Pipeline";
	case tole_SolarPanel:
		return "SolarPanel";
	case tole_ForestAisle:
		return "ForestAisle";
	case  tole_OOrchard:             
		return "OOrchard";
	case  tole_BushFruit:            
		return "BushFruit";
	case  tole_OBushFruit:           
		return "OBushFruit";
	case  tole_ChristmasTrees:       
		return "ChristmasTrees";
	case  tole_OChristmasTrees:      
		return "OChristmasTrees";
	case  tole_EnergyCrop:           
		return "EnergyCrop";
	case  tole_OEnergyCrop:          
		return "OEnergyCrop";
	case  tole_FarmForest:           
		return "FarmForest";
	case  tole_OFarmForest:          
		return "OFarmForest";
	case  tole_PermPasturePigs:      
		return "PermPasturePigs";
	case  tole_OPermPasturePigs:     
		return "OPermPasturePigs";
	case  tole_OPermPasture:         
		return "OPermPasture";
	case  tole_OPermPastureLowYield: 
		return "OPermPastureLowYield";
	case  tole_FarmYoungForest:      
		return "FarmYoungForest";
	case  tole_OFarmYoungForest:     
		return "OFarmYoungForest";
	case tole_AlmondPlantation:
		return "AlmondPlantation";
	case tole_WalnutPlantation:
		return "WalnutPlantation";
	case tole_FarmBufferZone:
		return "FarmBufferZone";
	case tole_NaturalFarmGrass:
		return "NaturalFarmGrass";
	case tole_GreenFallow:
		return "GreenFallow";
	case tole_FarmFeedingGround:
		return "FarmFeedingGround";
	case tole_FlowersPerm:
		return "FlowersPerm";
	case tole_AsparagusPerm:
		return "AsparagusPerm";
	case tole_OAsparagusPerm:
		return "Organic AsparagusPerm";
	case tole_MushroomPerm:
		return "MushroomPerm";
	case tole_OtherPermCrop:
		return "OtherPermCrop";
	case tole_FlowerStrip:
		return "Flower Strip";
	case tole_Foobar:
	default:
		g_msg->Warn(WARN_FILE, "Landscape::PolytypeToString(): Unknown event type: ",int(a_le_type));
		exit(1);
	}
}



std::string Landscape::VegtypeToString(TTypesOfVegetation a_veg) {

	switch (a_veg) {
	case tov_None:
		return "None                 ";
	case tov_NoGrowth:
		return "NoGrowth             ";
	case tov_NaturalGrass:
		return "NaturalGrass         ";
	case  tov_FlowerStrip1:
		return "Flower Strip Type 1  ";
	case  tov_FlowerStrip2:
		return "Flower Strip Type 2  ";
	case  tov_FlowerStrip3:
		return "Flower Strip Type 3  ";
	case tov_Carrots:
		return "Carrots              ";
	case tov_BroadBeans:
		return "BroadBeans           ";
	case tov_FodderGrass:
		return "FodderGrass          ";
	case tov_CloverGrassGrazed1:
		return "CloverGrassGrazed1   ";
	case tov_CloverGrassGrazed2:
		return "CloverGrassGrazed2   ";
	case tov_FieldPeas:
		return "FieldPeas            ";
	case tov_FieldPeasSilage:
		return "FieldPeasSilage      ";
	case tov_FodderBeet:
		return "FodderBeet           ";
	case tov_GenericCatchCrop:
		return "GenericCatchCrop     ";
	case tov_SugarBeet:
		return "SugarBeet            ";
	case tov_OFodderBeet:
		return "OFodderBeet          ";
	case tov_Lawn:
		return "Lawn                 ";
	case tov_Maize:
		return "Maize                ";
	case tov_MaizeSilage:
		return "MaizeSilage          ";
	case tov_OMaizeSilage:
		return "OMaizeSilage         ";
	case tov_OrchardCrop:
		return "OrchardCrop          ";
	case tov_Oats:
		return "Oats                 ";
	case tov_OBarleyPeaCloverGrass:
		return "OBarleyPeaCloverGrass";
	case tov_OCarrots:
		return "OCarrots             ";
	case tov_OCloverGrassGrazed1:
		return "OCloverGrassGrazed1  ";
	case tov_OCloverGrassGrazed2:
		return "OCloverGrassGrazed2  ";
	case tov_OCloverGrassSilage1:
		return "OCloverGrassSilage1  ";
	case tov_OFieldPeas:
		return "OFieldPeas           ";
	case tov_OFieldPeasSilage:
		return "OFieldPeasSilage     ";
	case tov_OFirstYearDanger:
		return "OFirstYearDanger     ";
	case tov_OGrazingPigs:
		return "OGrazingPigs         ";
	case tov_OOats:
		return "OOats                ";
	case tov_OPermanentGrassGrazed:
		return "OPermanentGrassGrazed";
	case tov_OPotatoes:
		return "OPotatoes            ";
	case tov_OSBarleySilage:
		return "OSBarleySilage       ";
	case tov_OSeedGrass1:
		return "OSeedGrass1          ";
	case tov_OSeedGrass2:
		return "OSeedGrass2          ";
	case tov_OSetAside:
		return "OSetAside             ";
	case tov_OSpringBarley:
		return "OSpringBarley        ";
	case tov_OSpringBarleyExt:
		return "OSpringBarleyExt     ";
	case tov_OSpringBarleyClover:
		return "OSpringBarleyClover  ";
	case tov_OSpringBarleyGrass:
		return "OSpringBarleyGrass   ";
	case tov_OSpringBarleyPigs:
		return "OSpringBarleyPigs    ";
	case tov_OTriticale:
		return "OTriticale           ";
	case tov_OWinterBarley:
		return "OWinterBarley        ";
	case tov_OWinterBarleyExt:
		return "OWinterBarleyExt     ";
	case tov_OWinterRape:
		return "OWinterRape          ";
	case tov_OWinterRye:
		return "OWinterRye           ";
	case tov_OWinterWheatUndersown:
		return "OWinterWheatUndersown";
	case tov_OWinterWheat:
		return "OWinterWheat";
	case tov_OWinterWheatUndersownExt:
		return "OWinterWheatUsowExt  ";
	case tov_PermanentGrassGrazed:
		return "PermanentGrassGrazed ";
	case tov_PermanentGrassLowYield:
		return "PermanentGrassLowYield";
	case tov_PermanentGrassTussocky:
		return "PermanentGrassTussocky";
	case tov_PermanentSetAside:
		return "PermanentSetAside    ";
	case tov_Potatoes:
		return "PotatoesEat          ";
	case tov_PotatoesIndustry:
		return "PotatoesIndustry     ";
	case tov_SeedGrass1:
		return "SeedGrass1           ";
	case tov_SeedGrass2:
		return "SeedGrass2           ";
	case tov_SetAside:
		return "SetAside             ";
	case tov_SpringBarley:
		return "SpringBarley         ";
	case tov_SpringBarleySpr:
		return "SpringBarleySpr      ";
	case tov_SpringBarleyPTreatment:
		return "SpringBarleyPTreat   ";
	case tov_SpringBarleySKManagement:
		return "SpringBarleySKMan    ";
	case tov_SpringBarleyCloverGrass:
		return "SprBarleyCloverGrass ";
	case tov_SpringBarleyGrass:
		return "SpringBarleyGrass    ";
	case tov_SpringBarleySeed:
		return "SpringBarleySeed     ";
	case tov_SpringBarleySilage:
		return "SpringBarleySilage   ";
	case tov_SpringRape:
		return "SpringRape           ";
	case tov_SpringWheat:
		return "SpringWheat          ";
	case tov_AgroChemIndustryCereal:
		return "AgroChemIndustry Cereal      ";
	case tov_Triticale:
		return "Triticale            ";
	case tov_WinterBarley:
		return "WinterBarley         ";
	case tov_WinterRape:
		return "WinterRape           ";
	case tov_WinterRye:
		return "WinterRye            ";
	case tov_WinterWheat:
		return "WinterWheat          ";
	case tov_WinterWheatShort:
		return "WinterWheatShort     ";
	case tov_WWheatPControl:
		return "P Trial Control      ";
	case tov_WWheatPToxicControl:
		return "P Trial Toxic Control";
	case tov_WWheatPTreatment:
		return "P Trial Treatment    ";
	case tov_Undefined:
		return "Undefined            ";
	case tov_WinterWheatStrigling:
		return "WWStrigling          ";
	case tov_WinterWheatStriglingSingle:
		return "WWStriglingSingle    ";
	case tov_WinterWheatStriglingCulm:
		return "WWStriglingCulm      ";
	case tov_SpringBarleyCloverGrassStrigling:
		return "SBPCGStrigling       ";
	case tov_SpringBarleyStrigling:
		return "SBarleyStrigling     ";
	case tov_SpringBarleyStriglingSingle:
		return "SBarleyStriglingSgl  ";
	case tov_SpringBarleyStriglingCulm:
		return "SBarleyStriglingCulm ";
	case tov_MaizeStrigling:
		return "MaizseStrigling      ";
	case tov_WinterRapeStrigling:
		return "WRapeStrigling       ";
	case tov_WinterRyeStrigling:
		return "WRyeStrigling        ";
	case tov_WinterBarleyStrigling:
		return "WBStrigling          ";
	case tov_FieldPeasStrigling:
		return "FieldPeasStrigling   ";
	case tov_SpringBarleyPeaCloverGrassStrigling:
		return "SBPeaCloverGrassStr  ";
	case tov_YoungForest:
		return "Young Forest         ";
	case tov_Wasteland:
		return "Wasteland            ";
	case tov_Heath:
		return "Heath/Grass          ";
	case  tov_PlantNursery:
		return "Plant Nursery        ";
	case  tov_NorwegianPotatoes:
		return "Norwegian Potatoes       ";
	case  tov_NorwegianOats:
		return "Norwegian Oats       ";
	case  tov_NorwegianSpringBarley:
		return "Norwegian Spr. Barley";
	case tov_WaterBufferZone:
		return "Unsprayed buffer zone around water";
	case tov_PLWinterWheat:
		return "Polish Winter Wheat  ";
	case tov_PLWinterRape:
		return "Polish Winter Rape   ";
	case tov_PLWinterBarley:
		return "Polish Winter Barley   ";
	case tov_PLWinterTriticale:
		return "Polish Winter Triticale   ";
	case tov_PLWinterRye:
		return "Polish Winter Rye   ";
	case tov_PLSpringWheat:
		return "Polish Spring Wheat   ";
	case tov_PLSpringBarley:
		return "Polish Spring Barley   ";
	case tov_PLMaize:
		return "Polish Maize   ";
	case tov_PLMaizeSilage:
		return "Polish Maize Silage   ";
	case tov_PLPotatoes:
		return "Polish Potatoes   ";
	case tov_PLBeet:
		return "Polish Beet   ";
	case tov_PLFodderLucerne1:
		return "Polish Fodder Lucerne first year   ";
	case tov_PLFodderLucerne2:
		return "Polish Fodder Lucerne second/third year   ";
	case tov_PLCarrots:
		return "Polish Carrots   ";
	case tov_PLSpringBarleySpr:
		return "Polish Spring Barley with Spring Plough  ";
	case tov_PLWinterWheatLate:
		return "Polish Winter Wheat late sown	";
	case tov_PLBeetSpr:
		return "Polish Beet with Spring Plough  ";
	case tov_PLBeans:
		return "Polish Beans  ";

	case tov_PTOliveGroveTraditional:
		return "Portugese OliveGrove Traditional";
	case tov_PTOliveGroveTradOrganic:
		return "Portugese OliveGrove Traditional Organic";
	case tov_PTOliveGroveIntensive:
		return "Portugese OliveGrove Intensive";
	case tov_PTOliveGroveSuperIntensive:
		return "Portugese OliveGrove Super intensive";

	case tov_NLBeet:
		return "Dutch Beet  ";
	case tov_NLCarrots:
		return "Dutch Carrots  ";
	case tov_NLMaize:
		return "Dutch Maize  ";
	case tov_NLPotatoes:
		return "Dutch Potatoes  ";
	case tov_NLSpringBarley:
		return "Dutch Spring Barley  ";
	case tov_NLWinterWheat:
		return "Dutch Winter Wheat  ";
	case tov_NLCabbage:
		return "Dutch Cabbage  ";
	case tov_NLTulips:
		return "Dutch Tulips  ";
	case tov_NLGrassGrazed1:
		return "Dutch TemporalGrassGrazed1  ";
	case tov_NLGrassGrazed1Spring:
		return "Dutch TemporalGrassGrazed1 after catch crop  ";
	case tov_NLGrassGrazed2:
		return "Dutch TemporalGrassGrazed2  ";
	case tov_NLGrassGrazedLast:
		return "Dutch TemporalGrassGrazedLast  ";
	case tov_NLPermanentGrassGrazed:
		return "Dutch PermanentGrassGrazed  ";
	case tov_NLBeetSpring:
		return "Dutch Beet after catch crop  ";
	case tov_NLCarrotsSpring:
		return "Dutch Carrots after catch crop  ";
	case tov_NLMaizeSpring:
		return "Dutch Maize after catch crop  ";
	case tov_NLPotatoesSpring:
		return "Dutch Potatoes after catch crop  ";
	case tov_NLSpringBarleySpring:
		return "Dutch Spring Barley after catch crop  ";
	case tov_NLCabbageSpring:
		return "Dutch Cabbage after catch crop  ";
	case tov_NLCatchCropPea:
		return "Dutch Catch Pea Crop  ";
	case tov_NLOrchardCrop:
		return "Dutch Orchard Crop  ";
	case tov_NLPermanentGrassGrazedExtensive:
		return "Dutch PermanentGrassGrazedExtensive  ";
	case tov_NLGrassGrazedExtensive1:
		return "Dutch TemporalGrassGrazedExtensive1  ";
	case tov_NLGrassGrazedExtensive1Spring:
		return "Dutch TemporalGrassGrazedExtensive1 after catch crop  ";
	case tov_NLGrassGrazedExtensive2:
		return "Dutch TemporalGrassGrazedExtensive2  ";
	case tov_NLGrassGrazedExtensiveLast:
		return "Dutch TemporalGrassGrazedExtensiveLast  ";

	case tov_UKBeans:
		return "UK Beans  ";
	case tov_UKBeet:
		return "UK Beet   ";
	case tov_UKMaize:
		return "UK Maize   ";
	case tov_UKPermanentGrass:
		return "UK Permanent Grass  ";
	case tov_UKPotatoes:
		return "UK Potatoes   ";
	case tov_UKSpringBarley:
		return "UK Spring Barley   ";
	case tov_UKTempGrass:
		return "UK Temp Grass  ";
	case tov_UKWinterBarley:
		return "UK Winter Barley   ";
	case tov_UKWinterRape:
		return "UK Winter Rape   ";
	case tov_UKWinterWheat:
		return "UK Winter Wheat  ";

		case tov_BEBeet:
		return "Belgium Beet  ";
	case tov_BEBeetSpring:
		return "Belgium Beet after catch crop  ";
	case tov_BECatchPeaCrop:
		return "Belgium Catch Pea Crop  ";
	case tov_BEGrassGrazed1:
		return "Belgium TemporalGrassGrazed1  ";
	case tov_BEGrassGrazed1Spring:
		return "Belgium TemporalGrassGrazed1 after catch crop  ";
	case tov_BEGrassGrazed2:
		return "Belgium TemporalGrassGrazed2  ";
	case tov_BEGrassGrazedLast:
		return "Belgium TemporalGrassGrazedLast  ";
	case tov_BEMaize:
	case tov_BEMaizeCC:
		return "Belgium Maize  ";
	case tov_BEMaizeSpring:
		return "Belgium Maize after catch crop  ";
	case tov_BEOrchardCrop:
		return "Belgium Orchard Crop  ";
	case tov_BEPotatoes:
		return "Belgium Potatoes   ";
	case tov_BEPotatoesSpring:
		return "Belgium PotatoesSpring   ";
	case tov_BEWinterBarley:
	case tov_BEWinterBarleyCC:
		return "Belgium Winter Barley   ";
	case tov_BEWinterWheat:
	case tov_BEWinterWheatCC:
		return "Belgium Winter Wheat  ";

	case tov_PTPermanentGrassGrazed:
		return "Portuguese PermanentGrassGrazed";
	case tov_PTWinterWheat:
		return "Portuguese Winter Wheat";
	case tov_PTGrassGrazed:
		return "Portuguese TemporalGrassGrazed";
	case tov_PTSorghum:
		return "Portuguese Sorghum";
	case tov_PTFodderMix:
		return "Portuguese Fodder Mix";
	case tov_PTTurnipGrazed:
		return "Portuguese TurnipGrazed";
	case tov_PTCloverGrassGrazed1:
		return "Portuguese CloverGrassGrazed1";
	case tov_PTCloverGrassGrazed2:
		return "Portuguese CloverGrassGrazed2";
	case tov_PTTriticale:
		return "Portuguese Triticale";
	case tov_PTOtherDryBeans:
		return "Portuguese OtherDryBeans";
	case tov_PTShrubPastures:
		return "Portuguese ShrubPastures";
	case tov_PTCorkOak:
		return "Portuguese CorkOak";
	case tov_PTVineyards:
		return "Portuguese Vineyards";
	case tov_PTWinterBarley:
		return "Portuguese Winter Barley";
	case tov_PTBeans:
		return "Portuguese Beans";
	case tov_PTWinterRye:
		return "Portuguese Winter Rye";
	case tov_PTRyegrass:
		return "Portuguese Ryegrass";
	case tov_PTYellowLupin:
		return "Portuguese Yellow Lupin";
	case tov_PTMaize:
		return "Portuguese Maize";
	case tov_PTOats:
		return "Portuguese Oats";
	case tov_PTPotatoes:
		return "Portuguese Potatoes";
	case tov_PTHorticulture:
		return "Portuguese Horticulture";
	case tov_PTCabbage:
		return "Portuguese Cabbage";
	case tov_PTCabbage_Hort:
		return "Portuguese Cabbage followed by Horticulture";
	case tov_PTMaize_Hort:
		return "Portuguese Maize followed by Horticulture";

	case tov_DESugarBeet: return "German Sugar Beet";
	case tov_DECabbage: return "German Cabbage";
	case tov_DECarrots: return "German Carrots";
	case tov_DEGrasslandSilageAnnual: return "German GrasslandSilageAnnual";
	case tov_DEGreenFallow_1year: return "German Green Fallow";
	case tov_DELegumes: return "German Legumes";
	case tov_DEMaize: return "German Maize";
	case tov_DEMaizeSilage: return "German Maise Silage";
	case tov_DEOats: return "German Oats";
	case tov_DEOCabbages: return "German Org Cabbage";
	case tov_DEOCarrots: return "German Org Carrots";
	case tov_DEOGrasslandSilageAnnual: return "German Org Grass Silage";
	case tov_DEOGreenFallow_1year: return "German Org Grass Fallow";
	case tov_DEOLegume: return "German Org Legume";
	case tov_DEOMaize: return "German Org Maise";
	case tov_DEOMaizeSilage: return "German Org Mail Silage";
	case tov_DEOOats: return "German Org Oats";
	case tov_DEOPeas: return "German Org Peas";
	case tov_DEOPermanentGrassGrazed: return "DE Org Perm Pasture Grazed";
	case tov_DEOPotatoes: return "German Org Potatoes";
	case tov_DEOSpringRye: return "German Org Spring Rye";
	case tov_DEOSugarBeet: return "German Org Sugar Beet";
	case tov_DEOTriticale: return "German Org Tricale";
	case tov_DEOWinterBarley: return "German Org Winter Barley";
	case tov_DEOWinterRape: return "German Org Winter Rape";
	case tov_DEOWinterRye: return "German Org Winter Rye";
	case tov_DEOWinterWheat: return "German Org Winter Wheat";
	case tov_DEPeas: return "German Peas";
	case tov_DEPermanentGrassGrazed: return "German Perm Grass Grazed";
	case tov_DEPermanentGrassLowYield: return "German Perm Grass Low Yield";
	case tov_DEOPermanentGrassLowYield: return "German Organic Perm Grass Low Yield";
	case tov_DEPotatoes: return "German Potaotes";
	case tov_DEPotatoesIndustry: return "German Potaotes Industry";
	case tov_DESpringRye: return "German Spring Rye";
	case tov_DETriticale: return "German Triticale";
	case tov_DEWinterRye: return "German Winter Rye";
	case tov_DEWinterBarley: return "German Winter Barley";
	case tov_DEWinterRape: return "German Winter Rape";
	case tov_DEWinterWheat: return "German Winter Wheat";
	case tov_DEWinterWheatLate: return "German Winter Wheat Late";
	case tov_DEAsparagusEstablishedPlantation: return "German Asparagus Established Plantation";
	case tov_DEOAsparagusEstablishedPlantation: return "German Organic Asparagus Established Plantation";
	case tov_DEHerbsPerennial_1year: return "German Perennial Herbs 1 year cultivation";
	case tov_DEHerbsPerennial_after1year: return "German Perennial Herbs aftter 1 year cultivation";
	case tov_DEOHerbsPerennial_1year: return "German Organic Perennial Herbs 1 year cultivation";
	case tov_DEOHerbsPerennial_after1year: return "German Organic Perennial Herbs aftter 1 year cultivation";
	case tov_DESpringBarley: return "German Spring Barley";
	case tov_DEOrchard: return "German Orchard Crop";
	case tov_DEOOrchard: return "German Organic Orchard Crop";
	case tov_DEBushFruitPerm:	return "German Fruit Bushes, Strawberries, harvest years";
	case tov_DEOBushFruitPerm:	return "German Organic Fruit Bushes, Strawberries, harvest years";
	case tov_DummyCropPestTesting:
		return "Dummy Crop for Testing of Pesticide Sparying Distribution  ";

	case tov_DKOOrchardCrop_Perm:
		return "Danish Ecological Orchard Crop, Permanent";
	case tov_DKBushFruit_Perm1:
		return "Danish Fruit Bushes, strawberries";
	case tov_DKBushFruit_Perm2:
		return "Danish Fruit Bushes, blackcurrant";
	case tov_DKOBushFruit_Perm1:
		return "Danish Ecological strawberries";
	case tov_DKOBushFruit_Perm2:
		return "Danish Ecological Fruit Bushes blackcurrant";
	case tov_DKChristmasTrees_Perm:
		return "Danish Christmas Trees, 10+ years in total";
	case tov_DKOChristmasTrees_Perm:
		return "Danish Ecological Christmas Trees, 10+ years in total";
	case tov_DKEnergyCrop_Perm:
		return "Danish Energy Crop";
	case tov_DKOEnergyCrop_Perm:
		return "Danish Ecological Energy Crop";
	case tov_DKFarmForest_Perm:
		return "Danish Farm Forest";
	case tov_DKOFarmForest_Perm:
		return "Danish Ecological Farm Forest";
	case tov_DKGrazingPigs_Perm:
		return "Danish Grazing Pasture for Pigs";
	case tov_DKOGrazingPigs_Perm:
		return "Danish Ecological Grazing Pasture for Pigs";
	case tov_DKOGrassGrazed_Perm:
		return "Danish Ecological Grazing Pasture";
	case tov_DKOGrassLowYield_Perm:
		return "Danish Ecological Grazing Pasture Low Yield";
	case tov_DKFarmYoungForest_Perm:
		return "Danish Farm Young Forest";
	case tov_DKOFarmYoungForest_Perm:
		return "Danish Ecological Farm Young Forest";
	case tov_DKOLegume_Peas:         
		return  "Danish Ecolological Legumes, Peas";
	case tov_DKOLegume_Beans:
		return  "Danish Ecolological Legumes, Beans";
	case tov_DKOLegume_Whole:   
		return  "Danish Ecolological Legumes Whole";
	case tov_DKOLegume_Peas_CC:
		return  "Danish Ecolological Legumes, Peas, followed by catch crop";
	case tov_DKOLegume_Beans_CC:
		return  "Danish Ecolological Legumes, Beans, followed by catch crop";
	case tov_DKOLegume_Whole_CC:
		return  "Danish Ecolological Legumes Whole, followed by catch crop";
	case tov_DKOLentils:
		return "Danish Ecological Lentils w. Oats";
	case tov_DKOLupines:
		return "Danish Ecological Lupines";
	case tov_DKSugarBeets:      
		return  "Danish Sugar Beet";
	case tov_DKOSugarBeets:     
		return  "Danish Ecological Sugar Beet";
	case tov_DKCabbages:        
		return  "Danish Cabbages";
	case tov_DKCatchCrop:
		return  "Danish Catch Crop";
	case tov_DKOCabbages:       
		return  "Danish Ecological Cabbages";
	case tov_DKCarrots:         
		return  "Danish Carrots";
	case tov_DKOLegumeCloverGrass_Whole:
		return  "Danish Ecological Legume W. Grass Undersown, Whole";
	case tov_DKOCarrots:        
		return  "Danish Ecological Carrots";
	case tov_DKLegume_Whole:    
		return  "Danish Legume Whole";
	case tov_DKLegume_Peas:          
		return  "Danish Legume Peas";
	case tov_DKLegume_Beans:
		return  "Danish Legume Beans";
	case tov_DKWinterWheat:     
		return  "Danish Winter Wheat";
	case tov_DKOWinterWheat:
		return  "Danish Ecological Winter Wheat";
	case tov_DKSpringBarley:
		return  "Danish Spring Barley";
	case tov_DKOSpringBarley:
		return  "Danish Ecological Spring Barley";
	case tov_DKWinterWheat_CC:
		return  "Danish Winter Wheat, followed by catch crop";
	case tov_DKOWinterWheat_CC:
		return  "Danish Ecological Winter Wheat, followed by catch crop";
	case tov_DKSpringBarley_CC:
		return  "Danish Spring Barley, followed by catch crop";
	case tov_DKOSpringBarley_CC:
		return  "Danish Ecological Spring Barley, followed by catch crop";
	case tov_DKSpringBarleyCloverGrass:
		return  "Danish Spring Barley w. Undersown Clover Grass";
	case tov_DKOSpringBarleyCloverGrass:
		return  "Danish Ecological Spring Barley w. Undersown Clover Grass";
	case tov_DKOCatchCrop:
		return  "Danish Ecological Catch Crop";
	case tov_DKCerealLegume:
		return  "Danish Cereal and Legume";
	case tov_DKOCerealLegume:
		return  "Danish Ecological Cereal and Legume";
	case tov_DKCerealLegume_Whole:
		return  "Danish Cereal and Legume, Whole crop";
	case tov_DKOCerealLegume_Whole:
		return  "Danish Ecological Cereal and Legume, Whole crop";
	case tov_DKWinterCloverGrassGrazedSown:
		return "Danish Clover Grass Grazed 1st year, autumn sow";
	case tov_DKCloverGrassGrazed1:
		return "Danish Clover Grass Grazed 1st year, sown in cover crop earlier";
	case tov_DKCloverGrassGrazed2:
		return "Danish Clover Grass Grazed next years";
	case tov_DKCloverGrassGrazed3:
		return "Danish Clover Grass Grazed last year, followed by winter crop";
	case tov_DKOWinterCloverGrassGrazedSown:
		return "Danish Ecological Clover Grass Grazed 1st year, autumn sow";
	case tov_DKOCloverGrassGrazed1:
		return "Danish Ecological Clover Grass Grazed 1st year, sown in cover crop earlier";
	case tov_DKOCloverGrassGrazed2:
		return "Danish Ecological Clover Grass Grazed next years";
	case tov_DKOCloverGrassGrazed3:
		return "Danish Ecological Clover Grass Grazed last year, followed by winter crop";
	case tov_DKSpringFodderGrass:
		return "Danish Fodder Grass, established in spring";
	case tov_DKWinterFodderGrass:
		return "Danish Fodder Grass, established in autumn";
	case tov_DKOWinterFodderGrass:
		return "Danish Ecological Fodder Grass in autumn";
	case tov_DKOSpringFodderGrass:
		return "Danish Ecological Fodder Grass in spring";
	case tov_DKGrazingPigs:
		return "Danish Grazing Pigs";
	case tov_DKMaize:
		return "Danish Maize";
	case tov_DKMaizeSilage:
		return "Danish Maize for Silage";
	case tov_DKMixedVeg:
		return "Danish Mixed Vegetables";
	case tov_DKOGrazingPigs:
		return "Danish Ecological Grazing Pigs";
		case tov_DKOMaize :
		return "Danish Ecological Maize";
	case tov_DKOMaizeSilage:
		return "Danish Ecological Maize for Silage";
	case tov_DKOMixedVeg:
		return "Danish Ecological Mixed Vegetables";
	case tov_DKOPotato:
		return "Danish Ecological Potatoes";
	case tov_DKOPotatoIndustry:
		return "Danish Ecological Potatoes for Industry";
	case tov_DKOPotatoSeed:
		return "Danish Ecological Potatoes for Seeds";
	case tov_DKOSeedGrassRye_Spring:
		return "Danish Ecological Rye Seed Grass, Spring";
	case tov_DKOSetAside:
		return "Danish Ecological Set Aside, spring mow";
	case tov_DKOSetAside_AnnualFlower:
		return "Danish Ecological Set Aside, annual flowers";
	case tov_DKOSetAside_PerennialFlower:
		return "Danish Ecological Set Aside, perennial flowers";
	case tov_DKOSetAside_SummerMow:
		return "Danish Ecological Set Aside, summer mow";
	case tov_DKOSpringBarleySilage:
		return "Danish Ecological Spring Barley for Silage";
	case tov_DKOSpringOats:
		return "Danish Ecological Spring Oats";
	case tov_DKOSpringWheat:
		return "Danish Ecological Spring Wheat";
	case tov_DKOVegSeeds:
		return "Danish Ecological Vegetable Seeds";
	case tov_DKOWinterBarley:
		return "Danish Ecological Winter Barley";
	case tov_DKOWinterRape:
		return "Danish Ecological Winter Rape";
	case tov_DKOWinterRye:
		return "Danish Ecological Winter Rye";
	case tov_DKOWinterRye_CC:
		return "Danish Ecological Winter Rye, followed by catch crop";
	case tov_DKWinterRye_CC:
		return "Danish Winter Rye, followed by catch crop";
	case tov_DKPotato:
		return "Danish Potatoes";
	case tov_DKPotatoIndustry:
		return "Danish Potatoes for Industry";
	case tov_DKPotatoSeed:
		return "Danish Potatoes for Seeds";
	case tov_DKSeedGrassFescue_Spring:
		return "Danish Fescue Seed Grass, Spring";
	case tov_DKSeedGrassRye_Spring:
		return "Danish Rye Seed Grass, Spring";
	case tov_DKSetAside:
		return "Danish Set Aside";
	case tov_DKSetAside_SummerMow:
		return "Danish Set Aside, Summer Mow";
	case tov_DKSpringBarley_Green:
		return "Danish Spring Barley for Green";
	case tov_DKSpringBarleySilage:
		return "Danish Spring Barley for Silage";
	case tov_DKSpringOats:
		return "Danish Spring Oats";
	case tov_DKSpringOats_CC:
		return "Danish Spring Oats, followed by catch crop";
	case tov_DKOSpringOats_CC:
		return "Danish Ecological Spring Oats, followed by catch crop";
	case tov_DKSpringWheat:
		return "Danish Spring Wheat";
	case tov_DKUndefined:
		return "Danish Undefined";
	case tov_DKVegSeeds:
		return "Danish Vegetable Seeds";
	case tov_DKWinterBarley:
		return "Danish Winter Barley";
	case tov_DKWinterRape:
		return "Danish Winter Rape";
	case tov_DKWinterRye:
		return "Danish Winter Rye";
	case tov_DKPlantNursery_Perm:
		return "Danish Plant Nursery, Permanent";
	case tov_DKOrchardCrop_Perm:
		return "Danish Orchard Crop, Permanent";
	case tov_DKGrassGrazed_Perm:
		return "Danish Grass Grazed, Permanent";
	case tov_DKGrassLowYield_Perm:
		return "Danish Grass Low Yeld, Permanent";
	case tov_DKGrassTussocky_Perm:
		return "Danish Grass Tussocky, Permanent";
	case tov_DKOrchApple:
		return "Danish Apple Orchard";
	case tov_DKOrchPear:
		return "Danish Pear Orchard";
	case tov_DKOrchCherry:
		return "Danish Cherry Orchard";
	case tov_DKOrchOther:
		return "Danish Other Orchard";
	case tov_DKOOrchApple:
		return "Danish Ecological Apple Orchard";
	case tov_DKOOrchPear:
		return "Danish Ecological Pear Orchard";
	case tov_DKOOrchCherry:
		return "Danish Ecological Cherry Orchard";
	case tov_DKOOrchOther:
		return "Danish Ecological Other Orchard";
	case tov_DKFodderBeets:
		return  "Danish Fodder Beet";
	case tov_DKOFodderBeets:
		return  "Danish Ecological Fodder Beet";
	case tov_DKOptimalFlowerMix1:
		return "Danish Optimal Flower Mix 1st year - est. annual flowers";
	case tov_DKOptimalFlowerMix2:
		return "Danish Optimal Flower Mix 2nd year - perennial flowers";
	case tov_DKOptimalFlowerMix3:
		return "Danish Optimal Flower Mix 3rd year - ryegrass";

	case tov_FIWinterWheat:
		return  "Finnish Winter Wheat";
	case tov_FIOWinterWheat:
		return  "Finnish Ecological Winter Wheat";
	case tov_FISugarBeet:
		return  "Finnish Sugar Beet";
	case tov_FIStarchPotato_North:
		return  "Finnish Starch Potato North";
	case tov_FIStarchPotato_South:
		return  "Finnish Starch Potato South";
	case tov_FIOStarchPotato_North:
		return  "Finnish Ecological Starch Potato North";
	case tov_FIOStarchPotato_South:
		return  "Finnish Ecological Starch Potato South";
	case tov_FISpringWheat:
		return "Finnish Spring Wheat";
	case tov_FIOSpringWheat:
		return "Finnish Ecological Spring Wheat";
	case tov_FITurnipRape:
		return "Finnish Turnip Rape";
	case tov_FIOTurnipRape:
		return "Finnish Ecological Turnip Rape";
	case tov_FISpringRape:
		return "Finnish Spring Rape";
	case tov_FIOSpringRape:
		return "Finnish Ecological Spring Rape";
	case tov_FIWinterRye:
		return "Finnish Winter Rye";
	case tov_FIOWinterRye:
		return "Finnish Ecological Winter Rye";
	case tov_FIPotato_North:
		return  "Finnish Potato North";
	case tov_FIPotato_South:
		return  "Finnish Potato South";
	case tov_FIOPotato_North:
		return  "Finnish Ecological Potato North";
	case tov_FIOPotato_South:
		return  "Finnish Ecological Potato South";
	case tov_FIPotatoIndustry_North:
		return  "Finnish Potato Industry North";
	case tov_FIPotatoIndustry_South:
		return  "Finnish Potato Industry South";
	case tov_FIOPotatoIndustry_North:
		return  "Finnish Ecological Potato Industry North";
	case tov_FIOPotatoIndustry_South:
		return  "Finnish Ecological Potato Industry South";
	case tov_FISpringOats:
		return "Finnish Spring Oats";
	case tov_FIOSpringOats:
		return "Finnish Ecological Spring Oats";
	case tov_FISpringBarley_Malt:
		return "Finnish Spring Barley Malt";
	case tov_FIOSpringBarley_Malt:
		return "Finnish Ecological Spring Barley Malt";
	case tov_FIFabaBean:
		return "Finnish Faba Bean";
	case tov_FIOFabaBean:
		return "Finnish Ecological Faba Bean";
	case tov_FISpringBarley_Fodder:
		return "Finnish Spring Barley Fodder";
	case tov_FISprSpringBarley_Fodder:
		return "Finnish Spr Spring Barley Fodder";
	case tov_FIOSpringBarley_Fodder:
		return "Finnish Ecological Spring Barley Fodder";
	case tov_FIGrasslandPasturePerennial1:
		return "Finnish Grassland Pasture Perennial first year";
	case tov_FIGrasslandPasturePerennial2:
		return "Finnish Grassland Pasture Perennial next years";
	case tov_FIGrasslandSilagePerennial1:
		return "Finnish Grassland Silage Perennial first year";
	case tov_FIGrasslandSilagePerennial2:
		return "Finnish Grassland Silage Perennial next years";
	case tov_FINaturalGrassland:
		return "Finnish Natural Grassland";
	case tov_FINaturalGrassland_Perm:
		return "Finnish Permanent Natural Grassland";
	case tov_FIFeedingGround:
		return "Finnish Feeding Ground";
	case tov_FIBufferZone:
		return "Finnish Buffer Zone";
	case tov_FIBufferZone_Perm:
		return "Finnish Permanent Buffer Zone";
	case tov_FIGreenFallow_1year:
		return "Finnish Green Fallow for one season only";
	case tov_FIGreenFallow_Perm:
		return "Finnish Permanent Green Fallow";
	case tov_FIGrasslandSilageAnnual:
		return "Finnish Grassland Silage Annual";
	case tov_FICaraway1:
		return "Finnish Caraway first year";
	case tov_FICaraway2:
		return "Finnish Caraway next years";
	case tov_FIOCaraway1:
		return "Finnish Ecological Caraway first year";
	case tov_FIOCaraway2:
		return "Finnish Ecological Caraway next years";
	case tov_SESpringBarley:
		return "Swedish Spring Barley";
	case tov_SEWinterRape_Seed:
		return "Swedish Winter Rape Seed";
	case tov_SEWinterWheat:
		return "Swedish Winter Wheat";
	case tov_IRSpringWheat:
		return "Irish Spring Wheat";
	case tov_IRSpringBarley:
		return "Irish Spring Barley";
	case tov_IRSpringOats:
		return "Irish Spring Oats";
	case tov_IRGrassland_no_reseed:
		return "Irish Grassland, no reseeding";
	case tov_IRGrassland_reseed:
		return "Irish Grassland, reseeding";
	case tov_IRWinterWheat:
		return "Irish Winter Wheat";
	case tov_IRWinterBarley:
		return "Irish Winter Barley";
	case tov_IRWinterOats:
		return "Irish Winter Oats";
	case tov_FRWinterWheat:
		return "French Winter Wheat";
	case tov_FRWinterBarley:
		return "French Winter Barley";
	case tov_FRWinterTriticale:
		return "French Winter Triticale";
	case tov_FRWinterRape:
		return "French Winter Rape";
	case tov_FRMaize:
		return "French Maize";
	case tov_FRMaize_Silage:
		return "French Maize Silage";
	case tov_FRSpringBarley:
		return "French Spring Barley";
	case tov_FRGrassland:
		return "French Grassland";
	case tov_FRGrassland_Perm:
		return "French Permanent Grassland";
	case tov_FRSpringOats:
		return "French Spring Oats";
	case tov_FRSunflower:
		return "French Sunflower";
	case tov_FRSpringWheat:
		return "French Spring Wheat";
	case tov_FRPotatoes:
		return "French Potatoes";
	case tov_FRSorghum:
		return "French Sorghum";
	case tov_ITGrassland:
		return "Italian Grassland";
	case tov_ITOrchard:
		return "Italian Orchard, apple";
	case tov_ITOOrchard:
		return "Italian Organic Orchard, apple";
	default:
		g_msg->Warn(WARN_FILE, "Landscape::VegtypeToString(): Unknown event type:", int(a_veg));
		exit(1);
	}
}

std::string Landscape::FarmtypeToString(TTypesOfFarm a_farmtype) {
	switch (a_farmtype) 
	{
	case tof_ConventionalCattle:
		 return "Conventional Cattle";
	case tof_ConventionalPig:
		 return "Conventional Pig";
	case tof_ConventionalPlant:
		 return "Conventional Plant";
	case tof_OrganicCattle:
		 return "Organic Cattle";
	case tof_OrganicPig:
		 return "Organic Pig";
	case tof_OrganicPlant:
		 return "Organic Plant";
	case tof_PTrialControl:
		 return "PTrialControl";
	case tof_PTrialTreatment:
		 return "PTrialTreatment";
	case tof_PTrialToxicControl:
		 return "PTrialToxicControl";
	case tof_ConvMarginalJord:
		 return "ConvMarginalJord";
	case tof_AgroChemIndustryCerealFarm1:
		 return "AgroChemIndustryCerealFarm1";
	case tof_AgroChemIndustryCerealFarm2:
		 return "AgroChemIndustryCerealFarm2";
	case tof_AgroChemIndustryCerealFarm3:
		 return "AgroChemIndustryCerealFarm3";
	case tof_NoPesticideBase:
		 return "NoPesticideBase";
	case tof_NoPesticideNoP:
		 return "NoPesticideNoP";
	case tof_UserDefinedFarm1:
		 return "UserDefinedFarm1";
	case tof_UserDefinedFarm2:
		 return "UserDefinedFarm2";
	case tof_UserDefinedFarm3:
		 return "UserDefinedFarm3";
	case tof_UserDefinedFarm4:
		 return "UserDefinedFarm4";
	case tof_UserDefinedFarm5:
		 return "UserDefinedFarm5";
	case tof_UserDefinedFarm6:
		 return "UserDefinedFarm6";
	case tof_UserDefinedFarm7:
		return "UserDefinedFarm7";
	case tof_UserDefinedFarm8:
		return "UserDefinedFarm8";
	case tof_UserDefinedFarm9:
		return "UserDefinedFarm9";
	case tof_UserDefinedFarm10:
		return "UserDefinedFarm10";
	case tof_UserDefinedFarm11:
		return "UserDefinedFarm11";
	case tof_UserDefinedFarm12:
		return "UserDefinedFarm12";
	case tof_UserDefinedFarm13:
		return "UserDefinedFarm13";
	case tof_UserDefinedFarm14:
		return "UserDefinedFarm14";
	case tof_UserDefinedFarm15:
		return "UserDefinedFarm15";
	case tof_UserDefinedFarm16:
		return "UserDefinedFarm16";
	case tof_UserDefinedFarm17:
		return "UserDefinedFarm17";
	case tof_UserDefinedFarm18:
		return "UserDefinedFarm18";
	case tof_UserDefinedFarm19:
		return "UserDefinedFarm19";
	case tof_UserDefinedFarm20:
		return "UserDefinedFarm20";
	case tof_UserDefinedFarm21:
		return "UserDefinedFarm21";
	case tof_UserDefinedFarm22:
		return "UserDefinedFarm22";
	case tof_UserDefinedFarm23:
		return "UserDefinedFarm23";
	case tof_UserDefinedFarm24:
		return "UserDefinedFarm24";
	case tof_UserDefinedFarm25:
		return "UserDefinedFarm25";
	case tof_UserDefinedFarm26:
		return "UserDefinedFarm26";
	case tof_UserDefinedFarm27:
		return "UserDefinedFarm27";
	case tof_UserDefinedFarm28:
		return "UserDefinedFarm28";
	case tof_UserDefinedFarm29:
		return "UserDefinedFarm29";
	case tof_UserDefinedFarm30:
		return "UserDefinedFarm30";
	case tof_UserDefinedFarm31:
		return "UserDefinedFarm31";
	case tof_UserDefinedFarm32:
		return "UserDefinedFarm32";
	case tof_UserDefinedFarm33:
		return "UserDefinedFarm33";
	case tof_UserDefinedFarm34:
		return "UserDefinedFarm34";
	case tof_UserDefinedFarm35:
		return "UserDefinedFarm35";
	case tof_UserDefinedFarm36:
		return "UserDefinedFarm36";
	case tof_OptimisingFarm:
		return "OptimisingFarm";
	default:
		g_msg->Warn(WARN_FILE, "Landscape::FarmtypeToString(): Unknown farm type:", int(a_farmtype));
		exit(1);
	}
	
}

bool Landscape::BorderNeed(TTypesOfLandscapeElement a_letype) {
	static char error_num[20];
	bool AddBorder = false;
	switch (a_letype) {
		// No border is needed toward these neighbouring element types.
	case tole_BeetleBank:
	case tole_UnsprayedFieldMargin:
	case tole_OrchardBand:
	case tole_FieldBoundary:
	case tole_FlowerStrip:
	case tole_MownGrassStrip:
		break;

	case tole_Marsh:
	case tole_IndividualTree:
	case tole_PlantNursery:
	case tole_Vildtager:
	case tole_WindTurbine:
	case tole_WoodyEnergyCrop:
	case tole_WoodlandMargin:
	case tole_Pylon:
	case tole_NaturalGrassDry:
	case tole_Railway:
	case tole_Scrub:
	case tole_Field:
	case tole_PermanentSetaside:
	case tole_PermPasture:
	case tole_PermPastureTussocky:
	case tole_PermPastureLowYield:
	case tole_PitDisused:
	case tole_RiversideTrees:
	case tole_DeciduousForest:
	case tole_MixedForest:
	case tole_YoungForest:
	case tole_ConiferousForest:
	case tole_StoneWall:
	case tole_Fence:
	case tole_Garden:
	case tole_Track:
	case tole_SmallRoad:
	case tole_LargeRoad:
	case tole_Building:
	case tole_ActivePit:
	case tole_Pond:
	case tole_FishFarm:
	case tole_Freshwater:
	case tole_River:
	case tole_Saltwater:
	case tole_Coast:
	case tole_BareRock:
	case tole_Heath:
	case tole_Orchard:
	case tole_AmenityGrass:
	case tole_Parkland:
	case tole_UrbanNoVeg:
	case tole_UrbanVeg:
	case tole_UrbanPark:
	case tole_BuiltUpWithParkland:
	case tole_SandDune:
	case tole_Copse:
	case tole_NaturalGrassWet:
	case tole_RoadsideSlope:
	case tole_MetalledPath:
	case tole_Carpark:
	case tole_Churchyard:
	case tole_Saltmarsh:
	case tole_Stream:
	case tole_HeritageSite:
		AddBorder = true;
		break;

	default:
		g_msg->Warn(WARN_BUG, "Landscape::BorderNeed(): Unknown element type: ", int(a_letype));
		exit(1);
	}
	return AddBorder;
}



/* Caution: The trace file file generated has an offset * in the reported date off by one! This looks like
* something is severely broken, but in reality I'm just * too lazy to fix this (design problem). */
/*
void Landscape::TestCropManagement( void ) {
FILE * tracefile;

tracefile = fopen(g_farm_test_crop_filename.value(), "w" )
if (!tracefile){
g_msg->Warn( WARN_FILE, "Landscape::TestCropManagement(): ""Unable to open file for writing: ",
g_farm_test_crop_filename.value() );
exit( 1 );
}

// Loop through the polygons and find one with 'our'
// vegetation type.
int l_poly;
for ( unsigned int i = 0; i < m_elems.size(); i++ ) {
if ( m_elems[ i ]->GetVegType() == g_farm_test_crop_type.value() ) {
l_poly = m_elems[ i ]->GetPoly();
break;
}
}

//int poly = SupplyPolyRef( g_farm_test_crop_sample_x.value(),
//			    g_farm_test_crop_sample_y.value() );
//int poly = 943;
int inf = 0;
int d = 0;
while ( 1 ) {  //Mysterious - what does this do?
Tick();
if ( ++inf > 364 ) {
printf( "%6d\n", ++d );
inf = 0;
}
}

fprintf( tracefile, "Polygon: %d\n", l_poly );
fprintf( tracefile, "EleType: %d\n", SupplyElementType( l_poly ) ),
fprintf( tracefile, "VegType: %d\n", SupplyVegType( l_poly ) );

for ( int x = 0; x < g_farm_test_crop_daystorun.value(); x++ ) {
Tick();
int index = 0, last;
int day = SupplyDayInMonth();
int month = SupplyMonth();
int year = SupplyYear();
long date = SupplyGlobalDate();
int veg = SupplyVegType( l_poly );

do {
last = SupplyLastTreatment( l_poly, & index );

// Comment out if you want to know *everything*,
// including nothing, that happens evey day on the test field.
if ( last == sleep_all_day && !g_farm_test_crop_reportdaily.value() )
continue;

fprintf( tracefile, "%2d %2d %4d %5ld %s %2d :: ""%7.2f %7.2f %7.2f %7.2f\n", day, month, year, date, EventtypeToString( last ),
veg, SupplyVegBiomass( l_poly ), SupplyVegCover( l_poly ), SupplyVegHeight( l_poly ), SupplyInsects( l_poly ) );
} while ( last != sleep_all_day );
}

fclose( tracefile );
}
*/

void Landscape::DumpVegAreaData(int a_day) {

	if (cfg_dumpvegjan.value()) {
		if ((a_day % 365) == 0) { // Jan 1st
			DumpMapInfoByArea(cfg_dumpvegjanfile.value(), true, true, true);
			return;
		}
	}
	if (cfg_dumpvegjune.value()) {
		if ((a_day % 365) == 152) { // 1st June
			DumpMapInfoByArea(cfg_dumpvegjunefile.value(), true, true, true);
		}
	}

}
//------------------------------------------------------------------------------

void Landscape::SetSpeciesFunctions(TTypesOfPopulation a_species) {
	// SetSpeciesFunction cannot be static - so we need to find an object instead
	for (auto& elem : m_elems) { 
		elem->SetSpeciesFunction(a_species); 
	}
}
//-----------------------------------------------------------------------------------------------------------------------------

PestIncidenceManager::PestIncidenceManager(double a_max, double a_min)
{
	m_max_incidence = a_max;
	m_min_incidence = a_min;
	m_current_incidence = (a_min + a_max) / 2.0;
	m_range = a_max - a_min;
}

void PestIncidenceManager::RecalculateIncidence()
{
	// This is the most basic version which is just a random number between 0 and 1.0
	m_current_incidence = (g_rand_uni_fnc()*m_range)+m_min_incidence;
}
