//
// MapCFG.h
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

//
//
// CAUTION!
//
// This file does not show all the possible configuration variables.
// Local ones are placed in the individual .cpp files to conserve
// compiler resources.
//

#include "Configurator.h"

// Whether to test a single crop in stand alone mode, or run normally.
// Set to false in production code.
//CfgBool  g_farm_test_crop( "FARM_TEST_CROP", CFG_CUSTOM, false );

// The numbers come from /docs/CropNumbers.txt.
// Eg. '22' is WinterRape.
CfgInt   g_farm_test_crop_type( "FARM_TEST_CROP_TYPE",
				CFG_CUSTOM, 22 );
CfgStr   g_farm_test_crop_filename( "FARM_TEST_CROP_FILENAME",
				    CFG_CUSTOM,
				    "OWinterRape_plant_1.txt" );
CfgInt   g_farm_test_crop_daystorun("FARM_TEST_CROP_DAYSTORUN",
				    CFG_CUSTOM, 36500 );
CfgBool  g_farm_test_crop_reportdaily("FARM_TEST_CROP_REPORTDAILY",
				      CFG_CUSTOM, false );

// Number is the same as used in the farm reference files and
// Landscape::CreateFarms()
//  1: ConventionalPig
//  2: ConventionalPlant
//  3: OrganicCattle
//  4: OrganicPig
//  5: OrganicPlant
//  6: PesticideTrialControl
//  7: PesticideTrialToxicControl
//  8: PesticideTrialTreatment
// 9: ConvMarginalJord
// 10: AgroChemIndustryCerealFarm1
// 11: AgroChemIndustryCerealFarm2
// 12: AgroChemIndustryCerealFarm3
// 13: NoPesticideBaseFarm
// 14: NoPesticideNoPFarm
//....
// 18: TestFarm1
// 19: TestFarm2
// 20: TestFarm3
// 21: TestFarm4
// 22: TestFarm5
// 23: TestFarm6
CfgInt   g_farm_test_crop_farmtype("FARM_TEST_CROP_FARMTYPE",
				   CFG_CUSTOM, 5 );
// The coordinate we will be picking the farming events from.
// Need to be adjusted for the particular map we will be using.
CfgInt   g_farm_test_crop_sample_x("FARM_TEST_CROP_SAMPLE_X",
				   CFG_CUSTOM, 1328 );
CfgInt   g_farm_test_crop_sample_y("FARM_TEST_CROP_SAMPLE_Y",
				   CFG_CUSTOM, 1328 );


// Wether to enable covering *all* farming fields with
// a single crop, specified below. This setting, if true,
// overrides FARM_ENABLE_CROP_ROTATION, ie. rotation will
// always be turned off if this setting is true.
CfgBool  g_farm_fixed_crop_enable("FARM_FIXED_CROP_ENABLE",
			   CFG_CUSTOM, false );

// The fixed crop to use if enabled above.
// The numbers come from /docs/CropNumbers.txt.
// Eg. '22' is WinterRape.
CfgInt   g_farm_fixed_crop_type( "FARM_FIXED_CROP_TYPE",
				 CFG_CUSTOM,
				 22 );

// Enabling rotation globally.
CfgBool  g_farm_enable_crop_rotation("FARM_ENABLE_CROP_ROTATION",
				     CFG_CUSTOM, true );

// Setting all rotations to be the same on all fields and run in sync.
CfgBool  g_farm_fixed_rotation_enable("FARM_FIXED_ROTATION_ENABLE",
				      CFG_CUSTOM, false );
// If FARM_FIXED_ROTATION_ENABLE is true above, this flag
// will allow each farm to run its rotation out of sync
// with the others. Ie. each farm selects its own, independent
// index in the fixed rotation selected by FARM_FIXED_ROTATION_FARMTYPE
// below.
CfgBool  g_farm_fixed_rotation_farms_async("FARM_FIXED_ROTATION_FARMS_ASYNC",
				     CFG_CUSTOM, false );

// Precisely which rotation scheme to run. Counted from zero!
// The current list of choices is:
//
//  0: ConventionalCattle
//  1: ConventionalPig
//  2: ConventionalPlant
//  3: OrganicCattle
//  4: OrganicPig
//  5: OrganicPlant
//  6: PesticideTrialControl
//  7: PesticideTrialToxicControl
//  8: PesticideTrialTreatment
// 9: ConvMarginalJord
// 10: AgroChemIndustryCerealFarm1
// 11: AgroChemIndustryCerealFarm2
// 12: AgroChemIndustryCerealFarm3
// 13: NoPesticideBaseFarm
// 14: NoPesticideNoPFarm
//....
// 18: TestFarm1
// 19: TestFarm2
// 20: TestFarm3
// 21: TestFarm4
// 22: TestFarm5
// 23: TestFarm6

/** 
* Fixes the farm type to this type regardless of what is in the farmrefs.txt input file. 
* Only works when running in fixed, sync'ed rotation mode
*/
CfgInt   g_farm_fixed_rotation_farmtype("FARM_FIXED_ROTATION_FARMTYPE", CFG_CUSTOM, 0);

/**
* If true will make and use new borders around certain types
* of farmed areas. The area used is 'stolen' from a parent
* landscape element. */
CfgBool g_map_le_borders("MAP_LE_BORDERS", CFG_CUSTOM, false);

/**
* If MAP_LE_BORDERS is true then set the minium field size in m2 that can have a border added.
*/
CfgInt g_map_le_borders_min_field_size("MAP_LE_BORDERS_MIN_FIELD_SIZE", CFG_CUSTOM, 10000);

/**
* If MAP_LE_BORDERS is true then set the type of polygon created to be this ALMaSS number type.
* To look up ALMaSS polygon type references see LE_TypeClass::BackTranslateEleTypes 
* Default value is FieldBoundary (160)
*/
CfgInt g_map_le_borderstype("MAP_LE_BORDERSTYPE", CFG_CUSTOM, 160);

/**
* If true will remove field boundaries and hedges, assigning them to a nearby field.
*/
CfgBool g_map_le_borderremoval( "MAP_LE_BORDERREMOVAL", CFG_CUSTOM, false );

/**
* If true will try to only make a border on one side of the field.
*/
CfgBool g_map_le_border_limited("MAP_LE_BORDER_LIMITED", CFG_CUSTOM, false);

/** 
* If true will make and use new borders around tole_orchard
* areas. The area used is 'stolen' from a parent
* landscape element. */
CfgBool g_map_orchards_borders( "MAP_ORCHARDS_BORDERS", CFG_CUSTOM, false );

/**
* The width of the field border if any.
*/
CfgInt g_map_le_borderwidth("MAP_LE_BORDER_WIDTH", CFG_CUSTOM, 1);

/**
* The minimum width of the rectangle surrounding a field that allows for a border
*/
CfgInt g_map_le_minfieldwidth("MAP_LE_BORDER_MIN_FIELD_WIDTH", CFG_CUSTOM, 10);

/**
* The width of the orchard borders if any.
*/
CfgInt g_map_orchardsborderwidth("MAP_ORCHARDS_BORDER_WIDTH", CFG_CUSTOM, 5);

/** 
* Percentile chance of a field getting a border, if MAP_LE_BORDERS is enabled above. 
*/
CfgInt  g_map_le_border_chance( "MAP_LE_BORDER_CHANCE", CFG_CUSTOM, 25 );
// Unsprayed margins code...
CfgInt  g_map_le_unsprayedmargins_chance( "MAP_LE_UMARGIN_CHANCE", CFG_CUSTOM,25 );
CfgBool g_map_le_unsprayedmargins( "MAP_LE_UNSPRAYEDMARGINS", CFG_CUSTOM,false );
CfgInt g_map_le_umargin_min_field_size("MAP_LE_UMARGIN_MIN_FIELD_SIZE", CFG_CUSTOM, 20000);
// ...to here


/** 
* This is the maximum polygon reference number allowed in the 'a_polyfile' and 'a_mapfile' supplied to the Landscape object constructor.
*/
CfgInt g_map_maxpolyref( "MAP_MAX_POLYREF", CFG_PUBLIC, 200000 );


/** \brief Parameters setting managment (tov type) for tole element types describing permanent crops */
/** \brief Management for permanent pasture low yield */
CfgInt g_farm_PermPastureLowYieldManagement("FARM_PERM_PASTURE_LOW_YIELD_MANAG", CFG_CUSTOM, 141);
/** \brief Management for permanent pasture low yield */
CfgInt g_farm_YoungForestManagement("FARM_YOUNG_FOREST_MANAG", CFG_CUSTOM, 55);
/** \brief Management for permanent pasture low yield */
CfgInt g_farm_PermPastureManagement("FARM_PERM_PASTURE_MANAG", CFG_CUSTOM, 140);
/** \brief Management for permanent pasture low yield */
CfgInt g_farm_PermPastureTussockyManagement("FARM_PERM_PASTURE_TUSSOCKY_MANAG", CFG_CUSTOM, 142);
/** \brief Management for permanent pasture low yield */
CfgInt g_farm_PermSetasideManagement("FARM_PERM_SETASIDE_MANAG", CFG_CUSTOM, 54);
/** \brief Management for permanent pasture low yield */
CfgInt g_farm_OrchardManagement("FARM_ORCHARD_MANAG", CFG_CUSTOM, 700);
/** \brief Management for permanent pasture low yield */
CfgInt g_farm_PlantNurseryManagement("FARM_PLANT_NURSERY_MANAG", CFG_CUSTOM, 55);

