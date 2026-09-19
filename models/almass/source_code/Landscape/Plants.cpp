//
// plants.cpp
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

#define _CRT_SECURE_NO_DEPRECATE

#include <cstdio>
#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include "Configurator.h"
#include "MapErrorMsg.h"
#include "ls.h"


class PlantGrowthData *g_crops;
class PollenNectarDevelopmentData * g_nectarpollen;

static CfgStr l_map_cropcurves_file("MAP_CROPCURVES_FILE", CFG_CUSTOM, "curves.pre");

double PlantGrowthData::FindDiff( double a_ddegs, double a_yddegs, int a_plant,
			  int a_phase, int a_type )
{
  // Check for valid plant number at runtime?
  // This is broken for growth curves where one can risk passing
  // more than a single inflection point in the growth curve in a
  // single day...

  int index = m_numbers[ a_plant ];
  unsigned int oldindex=0, newindex=0;

  if ( m_growth[ index ]->m_dds[ a_phase ][ 0 ] == 99999 ) {
    return 0.0;
  }

  for ( unsigned int i=0; i<MaxNoInflections; i++ ) {
    // In other words: If the current value for summed day degrees
    // is smaller than the X position of the *next* inflection
    // point, then we are in the correct interval.
    if ( m_growth[ index ]->m_dds[ a_phase ][ i+1 ] > a_ddegs ) {
      newindex = i;
      break;
      //      return m_growth[ index ]->m_slopes[ a_phase ][ a_type ][i];
    }
  }

  for ( unsigned int i=0; i<MaxNoInflections; i++ ) {
    if ( m_growth[ index ]->m_dds[ a_phase ][ i+1 ] > a_yddegs ) {
      oldindex = i;
      break;
      //      return m_growth[ index ]->m_slopes[ a_phase ][ a_type ][i];
    }
  }

  double diff;

  if ( newindex > oldindex ) {
    // We have passed an inflection point between today and yesterday.
    // First add the increment from yesterdays day degree sum up to
    // the inflection point.
    double dddif =
      m_growth[ index ]->m_dds[ a_phase ][ newindex ] - a_yddegs;
    diff =
      m_growth[ index ]->m_slopes[ a_phase ][ a_type ][oldindex]*
      dddif;

    // Then from the inflection point up to today.
    dddif = a_ddegs -
      m_growth[ index ]->m_dds[ a_phase ][ newindex ];
    diff +=
      m_growth[ index ]->m_slopes[ a_phase ][ a_type ][ newindex ]*
      dddif;
  } else {
    // No inflection point passed.
    diff = m_growth[ index ]->m_slopes[ a_phase ][ a_type ][ newindex ] *
      (a_ddegs - a_yddegs);
  }
  return diff;
}

unsigned int PlantGrowthData::FindCropNum( ifstream& ist )
{
  int NoPlants;

  m_numbers.resize(201);
  for ( unsigned int i=0; i<201; i++) {
    m_numbers[ i ] = -1;
  }

  //m_ifile = fopen(a_cropcurvefile, "r" );
  //ifstream m_ifile(a_cropcurvefile);
  /*
  if (!m_ifile){
    g_msg->Warn(WARN_FILE, "PlantGrowthData::PlantGrowthData: Unable to open file",
		a_cropcurvefile );
    exit(1);
  }
  */
  //fscanf( m_ifile , "%d", &NoPlants );  // How many tables to read in
  ist >> NoPlants;
  m_growth.resize( NoPlants );
  m_num_crops = NoPlants;

  m_final_ddeg.resize(NoPlants);
  for(int i=0; i<m_num_crops; i++){
	m_final_ddeg.at(i).resize(5);
	for(int j= 0; j<5; j++){
		m_final_ddeg.at(i).at(j).resize(3); 
	}
  }
  return NoPlants;
}



void PlantGrowthData::SetVegNum( unsigned int a_i, ifstream& ist, const char * a_cropcurvefile )
{
  int ThisPlant;

  // Find out what crop and what nutrient status
  //fscanf( m_ifile, "%d", &ThisPlant);

  ist >> ThisPlant;
  // Check if valid plant number (from the file).
  if ( ThisPlant < 0 || ThisPlant > 200 ) {
    g_msg->Warn(WARN_FILE, "PlantGrowthData::FindCropNum(): Illegal plant number"
		" specified in", a_cropcurvefile );
    exit(1);
  }

  m_numbers[ ThisPlant ] = a_i;

  // if greater than 100 then it is low nutrient
  if ( ThisPlant > 100 ) {
    m_growth[ a_i ]->m_lownut = true;
  } else {
    m_growth[ a_i ]->m_lownut = false;
  }
}


PlantGrowthData::PlantGrowthData( const char* a_vegcurvefile )
{
  // Just finds out how many veg curves there are.

  ifstream ist;
  std::string filename;

  if (strcmp(a_vegcurvefile, "default") == 0)
	  filename = l_map_cropcurves_file.value();
  else
	  filename = a_vegcurvefile;

  std::cout << "Reading Plant Curves File " << filename << "\n";
  ist.open(filename);
  if (!ist) {
	  g_msg->Warn(WARN_FILE, "PlantGrowthData::PlantGrowthData() Unable to open file",
		  filename);
	  exit(1);
  }

  unsigned int NoPlants = FindCropNum(ist);

  for ( unsigned int i=0; i<NoPlants; i++) {
    CropGrowth* temp;
    temp = new CropGrowth;
    m_growth[ i ] = temp;
    SetVegNum( i, ist, a_vegcurvefile );

/*
    for (int mmm=0; mmm < 10; ++mmm) {
        int myin;
        ist >> myin;
        cout <<
    }
*/

    for (unsigned int j=0; j<5; j++) { // for each growth phase
      // 'Local' index into crop growth curves.
      int lk = 0;
      for (unsigned int k=0; k<MaxNoInflections; k++) {
	// for each inflection point
      	int entry;
        //fscanf( m_ifile, "%d", &entry );
        ist >> entry;
	double f1=0,f2=0,f0=0;
	if ( entry == -1 ) {
          // Crop start data.
          m_growth[ i ]->m_start_valid[j] = true;
          //fscanf( m_ifile, "%g %g %g",&f1,&f0,&f2);
          ist >> f1 >> f0 >> f2;
		  //FloatToDouble(m_growth[ i ]->m_start[j][1],f1);
		  //FloatToDouble(m_growth[ i ]->m_start[j][0],f0);
		  //FloatToDouble(m_growth[ i ]->m_start[j][2],f2);
		  m_growth[ i ]->m_start[j][1]=f1;
		  m_growth[ i ]->m_start[j][0]=f0;
		  m_growth[ i ]->m_start[j][2]=f2;
	} else {
	  // Add inflection point to normal growth curves.
	  m_growth[ i ]->m_dds[j][lk] = (double)entry;
      //fscanf( m_ifile, "%g %g %g",&f1,&f0,&f2);
      ist >> f1 >> f0 >> f2;
	  //FloatToDouble(m_growth[ i ]->m_slopes[j][1][lk],f1);
	  //FloatToDouble(m_growth[ i ]->m_slopes[j][0][lk],f0);
	  //FloatToDouble(m_growth[ i ]->m_slopes[j][2][lk],f2);
	  m_growth[ i ]->m_slopes[j][1][lk]=f1;
	  m_growth[ i ]->m_slopes[j][0][lk]=f0;
	  m_growth[ i ]->m_slopes[j][2][lk]=f2;
	  lk++;
	}
      } // MaxNoInflections
    } // Growth Phases
  } // NoPlants
  //fclose( m_ifile );

  double temp_ddeg = -1;
  for ( unsigned int i=0; i<NoPlants; i++) {
	for (unsigned int j=0; j<5; j++) {
		if (m_growth[i]->m_start_valid[j] == false && m_growth[i]->m_dds[j][0]>99998){ //no growth curve, so use -1
			m_final_ddeg.at(i).at(j).at(0) = -1;
			m_final_ddeg.at(i).at(j).at(1) = -1;
			m_final_ddeg.at(i).at(j).at(2) = -1;
			continue;
		}

		//there is a growth curve, check the maximum growth day degrees
		for (unsigned int l=0; l<3; l++){
			temp_ddeg = -1;
			unsigned int start_index = 	MaxNoInflections-2;
			if (m_growth[i]->m_start_valid[j]){
				start_index = MaxNoInflections-3;
			}
			temp_ddeg = m_growth[i]->m_dds[j][start_index+1];	
			for (int k=start_index; k>=0; k--) {
				if(m_growth[i]->m_slopes[j][l][k]>0){
					m_final_ddeg.at(i).at(j).at(l) = temp_ddeg;
					break;
				}
				else if(m_growth[i]->m_slopes[j][l][k]<0){
					m_final_ddeg.at(i).at(j).at(l) = temp_ddeg; //m_growth[i]->m_dds[j][k];
					break;
				}
				else{
					temp_ddeg = m_growth[i]->m_dds[j][k];
				}
			}
		}
	}
  }	
}

PlantGrowthData::~PlantGrowthData()
{
  for ( unsigned int i=0; i<m_growth.size(); i++ )
    delete m_growth[i];
}

int PlantGrowthData::VegTypeToCurveNum(TTypesOfVegetation VegReference)
{
  char error_num[20];

  switch (VegReference)
    {
    case tov_SpringBarleySilage:
	case tov_SpringBarley:
	case tov_SpringBarleySpr:
	case tov_SpringBarleyPTreatment:
	case tov_SpringBarleySKManagement:
    case tov_SpringBarleyStrigling:
    case tov_SpringBarleyStriglingSingle:
    case tov_SpringBarleyStriglingCulm:
	case tov_NorwegianSpringBarley:
	case tov_PLSpringBarley: /** \todo EZ: needs to be changed later */ 
	case tov_PLSpringBarleySpr:
	case tov_NLSpringBarley:
	case tov_NLSpringBarleySpring:
	case tov_UKSpringBarley:
	case tov_DESpringRye: /** \todo EZ: needs to be changed later */ 
	case tov_DESpringBarley:
	case tov_DKSpringBarley:
	case tov_DKSpringBarley_CC:
	case tov_FISpringBarley_Malt:
	case tov_FISpringBarley_Fodder:
	case tov_FISprSpringBarley_Fodder:
	case tov_SESpringBarley:
	case tov_DKSpringBarley_Green:
	case tov_DKSpringBarleySilage:	
	case tov_FRSpringBarley:	
	case tov_IRSpringBarley:
      return 1; // Conv. spring barley

	case tov_OSpringBarleyPigs:
	case tov_OSpringBarley:
	case tov_OSpringBarleyExt:
	case tov_DKOSpringBarley:
	case tov_DKOSpringBarley_CC:
	case tov_FIOSpringBarley_Malt:
	case tov_FIOSpringBarley_Fodder:
	case tov_DKOSpringBarleySilage:
	case tov_DEOSpringRye:
		return 101; // Org. spring barley

    case tov_WinterBarley:
    case tov_WinterBarleyStrigling:
	case tov_PLWinterBarley: /** \todo EZ: needs to be changed later */ 
	case tov_UKWinterBarley:
	case tov_BEWinterBarley:
	case tov_BEWinterBarleyCC:
	case tov_PTWinterBarley: /** \todo AAS: needs to be changed later */ 
	case tov_DEWinterBarley: /** \todo EZ: needs to be changed later */ 
	case tov_DKWinterBarley:
	case tov_IRWinterBarley:
	case tov_FRWinterBarley:
      return 2; // Conv. winter barley

    case tov_OWinterBarley:
    case tov_OWinterBarleyExt:
	case tov_DEOWinterBarley: /** \todo EZ: needs to be changed later */ 
	case tov_DKOWinterBarley:
      return 102; // Org. winter barley

	case tov_IRSpringWheat: 
	case tov_DKSpringWheat: 
	case tov_FRSpringWheat:
	case tov_FISpringWheat:
	case tov_PLSpringWheat:
	  return 3; // Conv. spring wheat

	case tov_DKOSpringWheat: 
	case tov_FIOSpringWheat: 
	  return 103; // Org. spring wheat

    case tov_WinterWheat:
    case tov_WinterWheatShort:
    case tov_WinterWheatStrigling:
    case tov_WinterWheatStriglingCulm:
    case tov_WinterWheatStriglingSingle:
    case tov_AgroChemIndustryCereal:
    case tov_WWheatPControl:
    case tov_WWheatPToxicControl:
    case tov_WWheatPTreatment:
	case tov_PLWinterWheat: /** \todo EZ: needs to be changed later */ 
	case tov_PLWinterWheatLate:
	case tov_DummyCropPestTesting:	// just for testing of spraying distribution
	case tov_NLWinterWheat:
	case tov_DKWinterWheat:
	case tov_DKWinterWheat_CC:
	case tov_FIWinterWheat:
	case tov_SEWinterWheat:
	case tov_FRWinterWheat:
	case tov_UKWinterWheat:
	case tov_BEWinterWheat:
	case tov_BEWinterWheatCC:
	case tov_PTWinterWheat:
	case tov_DEWinterWheat:	/** \todo EZ: needs to be changed later */ 
	case tov_DEWinterWheatLate:	/** \todo EZ: needs to be changed later */ 
	case tov_IRWinterWheat: 
	  return 4; // Conv. winter wheat

	case tov_OWinterWheat:
	case tov_OWinterWheatUndersown:
	case tov_DEOWinterWheat: /** \todo EZ: needs to be changed later */ 
	case tov_DKOWinterWheat:
	case tov_DKOWinterWheat_CC:
	case tov_FIOWinterWheat:
		return 104; // Org. winter wheat

    case tov_WinterRye:
    case tov_WinterRyeStrigling:
	case tov_PLWinterRye: /** \todo EZ: needs to be changed later */ 
	case tov_FIWinterRye:
	case tov_DKWinterRye:
	case tov_DKWinterRye_CC:
	case tov_PTWinterRye: /** \todo AAS: needs to be changed later */ 
	case tov_DEWinterRye: /** \todo EZ: needs to be changed later */ 
      return 5; // Conv. winter rye

    case tov_OWinterRye:
	case tov_DKOWinterRye_CC:
	case tov_FIOWinterRye:
	case tov_DEOWinterRye: /** \todo EZ: needs to be changed later */ 
	case tov_DKOWinterRye:
    return 105; // Org. winter Rye

    case tov_Oats:
	case tov_NorwegianOats:
	case tov_DEOats: /** \todo EZ: needs to be changed later */
	case tov_PTOats: /** \todo EZ: needs to be changed later */ 
	case tov_FISpringOats:
	case tov_DKSpringOats:
	case tov_DKSpringOats_CC:
	case tov_IRSpringOats:
	case tov_IRWinterOats: /** \todo LKM: maybe needs to be changed later as it is a winter crop */ 
      return 6; // Conv. oats

    case tov_OOats:
	case tov_FIOSpringOats:
	case tov_DEOOats: /** \todo EZ: needs to be changed later */ 
	case tov_DKOSpringOats:
	case tov_DKOSpringOats_CC:
      return 106; // Org. oats

	case tov_HBMaizeIntensive: // Hoa Binh: maize growth curve
	case tov_HBMaizeLowInput:
    case tov_Maize:
    case tov_MaizeSilage:
    case tov_MaizeStrigling:
	case tov_PLMaize: /** \todo EZ: needs to be changed later */ 
	case tov_PLMaizeSilage:	/** \todo EZ: needs to be changed later */ 
	case tov_NLMaize:
	case tov_NLMaizeSpring:
	case tov_UKMaize:
	case tov_BEMaize:
	case tov_BEMaizeCC:
	case tov_BEMaizeSpring:
	case tov_PTSorghum:	/** \todo AAS: needs to be changed later */
	case tov_DEMaize: /** \todo EZ: needs to be changed later */ 
	case tov_DEMaizeSilage: /** \todo EZ: needs to be changed later */ 
	case tov_PTMaize: /** \todo EZ: needs to be changed later */ 
	case tov_PTMaize_Hort: /** \todo EZ: needs to be changed later */ 
	case tov_DKMaize:
	case tov_DKMaizeSilage:
	case tov_FRMaize:
	case tov_FRMaize_Silage:
	case tov_FRSorghum:  /** \todo LKM: needs to be changed later when a sorghum curve is ready */ 
	case tov_FRSunflower: /** \todo LKM: needs to be changed later when a sunflower curve is ready */ 
      return 8; // Conv. maize

    case tov_OMaizeSilage:
	case tov_DEOMaize: 	/** \todo EZ: needs to be changed later */ 
	case tov_DEOMaizeSilage: /** \todo EZ: needs to be changed later */ 
	case tov_DKOMaize:
	case tov_DKOMaizeSilage:
	  return 108; // Org. maize

    case tov_SpringBarleyCloverGrass:
    case tov_SpringBarleyCloverGrassStrigling:
    case tov_SpringBarleySeed:
    case tov_SpringBarleyPeaCloverGrassStrigling:
	case tov_DKCerealLegume:
	case tov_DKCerealLegume_Whole:
	case tov_DKSpringBarleyCloverGrass:
      return 13; // Conv. spring barley, undersown

    case tov_OBarleyPeaCloverGrass:
    case tov_OSBarleySilage:
    case tov_OSpringBarleyGrass:
    case tov_OSpringBarleyClover:
	case tov_DKOCerealLegume:
	case tov_DKOCerealLegume_Whole:
	case tov_DKOSpringBarleyCloverGrass:
      return 113; // Org. spring barley, undersown

    case tov_WinterRape:
    case tov_WinterRapeStrigling:
	case tov_PLWinterRape: /** \todo EZ: needs to be changed later */ 
	case tov_SEWinterRape_Seed:
	case tov_FRWinterRape:
	case tov_DKWinterRape:
	case tov_DEWinterRape: /** \todo EZ: needs to be changed later */ 
	case tov_UKWinterRape:
      return 22; // Conv. winter rape

    case tov_OWinterRape:
	case tov_DEOWinterRape: /** \todo EZ: needs to be changed later */ 
	case tov_DKOWinterRape:
      return 122; // Org. winter rape

	case tov_PermanentGrassLowYield:
	case tov_DEPermanentGrassLowYield:
	case tov_DEOPermanentGrassLowYield:
	case tov_PermanentGrassTussocky:
	case tov_DKGrassLowYield_Perm:
	case tov_DKOGrassLowYield_Perm:
	case tov_DKGrassTussocky_Perm:
		return 25; // Permanent grass grazed, low yield

	case tov_PermanentGrassGrazed:
	case tov_OPermanentGrassGrazed:
	case tov_DEPermanentGrassGrazed:
	case tov_DEOPermanentGrassGrazed:
	case tov_DKOGrassGrazed_Perm:
	case tov_DKGrassGrazed_Perm:
	case tov_DKGrazingPigs_Perm: /** \todo LKM: maybe needs to be changed later if a specific curve is developed? */ 
	case tov_DKOGrazingPigs_Perm: /** \todo LKM: maybe needs to be changed later if a specific curve is developed? */ 
	case tov_PTPermanentGrassGrazed: /** \todo AAS: needs to be changed later */
	case tov_FINaturalGrassland_Perm:
	case tov_FRGrassland_Perm:
	case tov_NLPermanentGrassGrazed:
	case tov_NLPermanentGrassGrazedExtensive:
	case tov_UKPermanentGrass:
	case tov_FIGreenFallow_Perm:
      return 26; // Permanent grass grazed

    case tov_SeedGrass1:
    case tov_SeedGrass2:
	case tov_OSeedGrass1:
	case tov_OSeedGrass2:
	case tov_FICaraway1: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_FICaraway2: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_FIOCaraway1: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_FIOCaraway2: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOSeedGrassRye_Spring: 
	case tov_DKOVegSeeds: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKSeedGrassFescue_Spring:
	case tov_DKSeedGrassRye_Spring:
	case tov_DKVegSeeds: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
		return 27; // seed grass
	
	case tov_DEGrasslandSilageAnnual:
		return 28; // clovergrass for silage

    case tov_FodderGrass:
    case tov_CloverGrassGrazed1:
    case tov_CloverGrassGrazed2:
    case tov_OCloverGrassGrazed1:
    case tov_OCloverGrassGrazed2:
    case tov_OGrazingPigs:
	case tov_PLFodderLucerne1: /** \todo EZ: needs to be changed later */
	case tov_PLFodderLucerne2: /** \todo EZ: needs to be changed later */ 
	case tov_UKTempGrass:
	case tov_PTCloverGrassGrazed1: /** \todo AAS: maybe needs to be changed later because it is winter/autumn sown */ 
	case tov_PTCloverGrassGrazed2:	/** \todo AAS: needs to be changed later */
	case tov_DKWinterCloverGrassGrazedSown: /** \todo LKM: maybe needs to be changed later because it is winter/autumn sown */ 
	case tov_DKCloverGrassGrazed1:
	case tov_DKCloverGrassGrazed2:
	case tov_DKCloverGrassGrazed3:
	case tov_DKOWinterCloverGrassGrazedSown: /** \todo LKM: maybe needs to be changed later because it is winter/autumn sown */ 
	case tov_DKOCloverGrassGrazed1:
	case tov_DKOCloverGrassGrazed2:
	case tov_DKOCloverGrassGrazed3:
	case tov_DKSpringFodderGrass:
	case tov_DKWinterFodderGrass:
	case tov_DKOWinterFodderGrass:
	case tov_DKOSpringFodderGrass:
	case tov_DKGrazingPigs:
	case tov_DKOGrazingPigs:
        return 29; // Clover grass grazed

	case tov_FIGrasslandPasturePerennial1:
	case tov_FIGrasslandPasturePerennial2:
	case tov_FIGrasslandSilagePerennial1:
	case tov_FIGrasslandSilagePerennial2:
	case tov_FIGrasslandSilageAnnual:
		return 65; // Conv. grass for silage

	case tov_OCloverGrassSilage1:
	case tov_DEOGrasslandSilageAnnual:
		return 165; // Org. grass for silage

	case tov_NLGrassGrazed1:
	case tov_NLGrassGrazed1Spring:
	case tov_NLGrassGrazedExtensive1:
	case tov_NLGrassGrazedExtensive1Spring:
	case tov_NLGrassGrazed2:
	case tov_NLGrassGrazedLast:
	case tov_NLGrassGrazedExtensive2:
	case tov_NLGrassGrazedExtensiveLast:
	case tov_BEGrassGrazed1:
	case tov_BEGrassGrazed1Spring:
	case tov_BEGrassGrazed2:
	case tov_BEGrassGrazedLast:
	case tov_PTGrassGrazed:	/** \todo AAS: needs to be changed later */ 
	case tov_PTFodderMix: /** \todo AAS: needs to be changed later */ 
	case tov_PTRyegrass: /** \todo AAS: needs to be changed later */ 
	case tov_FIFeedingGround:
	case tov_FIGreenFallow_1year:
	case tov_DEGreenFallow_1year:
	case tov_DEOGreenFallow_1year:
	case tov_FRGrassland:
	case tov_ITGrassland:
	case tov_IRGrassland_no_reseed:
	case tov_IRGrassland_reseed:
		return 70; // Grass grazed

    case tov_OFieldPeas:
    case tov_OFieldPeasSilage:
    case tov_FieldPeas:
    case tov_FieldPeasStrigling:
	case tov_FieldPeasSilage:
	case tov_BroadBeans: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_PLBeans: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_UKBeans: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_NLCatchCropPea: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKCatchCrop: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOCatchCrop: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_BECatchPeaCrop: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_PTYellowLupin: /** \todo AAS: needs to be changed later */
	case tov_PTOtherDryBeans: /** \todo AAS: needs to be changed later */ 
	case tov_PTBeans: /** \todo AAS: needs to be changed later */ 
	case tov_PTHorticulture: /** \todo AAS: needs to be changed later */ 
	case tov_DELegumes:
	case tov_DEOLegume: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLegume_Peas: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLegume_Beans: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLegume_Whole: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLegume_Peas_CC: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLegume_Beans_CC: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLegume_Whole_CC: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLegumeCloverGrass_Whole: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKLegume_Whole: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKLegume_Peas:
	case tov_DKLegume_Beans: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLupines: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOLentils: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_FIFabaBean: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_FIOFabaBean: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DEPeas:
	case tov_DEOPeas: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
      return 30; // Conv. peas

    case tov_Carrots:
	case tov_PLCarrots:	/** \todo EZ: needs to be changed later */ 
	case tov_DECarrots:	/** \todo EZ: needs to be changed later */ 
	case tov_NLCarrots:
	case tov_NLCabbage:	/** \todo EZ: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DECabbage:	/** \todo EZ: maybe needs to be changed later if a specific curve is developed */ 
	case tov_NLCarrotsSpring:
	case tov_NLCabbageSpring:	/** \todo EZ: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKCabbages: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKCarrots:
	case tov_DKMixedVeg:/** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_PTTurnipGrazed: /** \todo AAS: needs to be changed later */ 
	case tov_DEAsparagusEstablishedPlantation: /** \todo EZ: needs to be changed later */ 
	case tov_DEHerbsPerennial_1year:  /** \todo EZ: needs to be changed later */ 
	case tov_DEHerbsPerennial_after1year:  /** \todo EZ: needs to be changed later */ 
	case tov_PTCabbage:/** \todo AAS: needs to be changed later */
	case tov_PTCabbage_Hort: /** \todo AAS: needs to be changed later */
      return 41; // Conv. carrots

    case tov_OCarrots:
	case tov_DEOCarrots:
	case tov_DEOCabbages: /** \todo EZ: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOCabbages: /** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DKOCarrots:
	case tov_DKOMixedVeg:/** \todo LKM: maybe needs to be changed later if a specific curve is developed */ 
	case tov_DEOHerbsPerennial_1year:  /** \todo EZ: needs to be changed later */
	case tov_DEOHerbsPerennial_after1year:  /** \todo EZ: needs to be changed later */
	case tov_DEOAsparagusEstablishedPlantation: //Needs to be changed later
      return 141; // Org. carrots

    case tov_Potatoes:
	case tov_NorwegianPotatoes:
	case tov_PLPotatoes:	/** \todo EZ: needs to be changed later */ 
	case tov_PotatoesIndustry:
	case tov_NLPotatoes:
	case tov_NLPotatoesSpring:
	case tov_FIStarchPotato_North:
	case tov_FIStarchPotato_South:
	case tov_FIPotato_North:
	case tov_FIPotato_South:
	case tov_FIPotatoIndustry_North:
	case tov_FIPotatoIndustry_South:
	case tov_DKPotato:
	case tov_DKPotatoIndustry:
	case tov_DKPotatoSeed:
	case tov_UKPotatoes:
	case tov_BEPotatoes:
	case tov_BEPotatoesSpring:
	case tov_DEPotatoes: 	/** \todo EZ: needs to be changed later */ 
	case tov_DEPotatoesIndustry: 	/** \todo EZ: needs to be changed later */ 
	case tov_PTPotatoes: 	/** \todo AAS: needs to be changed later */ 
	case tov_FRPotatoes:
		return 50; // Conv. potato

	case tov_FIOPotato_North:
	case tov_FIOPotato_South:
	case tov_FIOPotatoIndustry_North:
	case tov_FIOPotatoIndustry_South:
	case tov_OPotatoes:
	case tov_FIOStarchPotato_North:
	case tov_FIOStarchPotato_South:
	case tov_DEOPotatoes: 	/** \todo EZ: needs to be changed later */ 
	case tov_DKOPotato:
	case tov_DKOPotatoIndustry:
	case tov_DKOPotatoSeed:
		return 150; // Org. potato

	case tov_SugarBeet:
	case tov_FodderBeet:
	case tov_PLBeet:	/** \todo EZ: needs to be changed later */ 
	case tov_PLBeetSpr:
	case tov_NLBeet:
	case tov_NLBeetSpring:
	case tov_DESugarBeet:
	case tov_UKBeet:
	case tov_BEBeet:
	case tov_BEBeetSpring:
	case tov_DKSugarBeets:
	case tov_DKFodderBeets:
	case tov_FISugarBeet:
		return 60; // Conv. beet

	case tov_OFodderBeet:
	case tov_DEOSugarBeet:
	case tov_DKOSugarBeets:
	case tov_DKOFodderBeets:
		return 160; // Org. beet

      // Special growth mode for green but unused elements.
      // tov_PermanentSetAside Does not change growth phase no matter
      // how hard one tries to do just that.
    case tov_PermanentSetAside: 
		return 92; // Permanent setaside

	case tov_HBFallow: // Hoa Binh: set-aside (herbaceous regrowth) curve
	case tov_Heath:
    case tov_SetAside:
    case tov_OSetAside:
	case tov_DKOSetAside:
	case tov_DKSetAside:
	case tov_DKSetAside_SummerMow:
	case tov_DKOSetAside_SummerMow:
	case tov_DKOSetAside_AnnualFlower: /** \todo LKM: needs to be changed later - need a curve that matched annual flowers*/ 
	case tov_DKOSetAside_PerennialFlower: /** \todo LKM: needs to be changed later - need a curve that matched perennial flowers*/ 
	case tov_DKOptimalFlowerMix1:  /** \todo LKM: needs to be changed later - need a curve that matched annual flowers*/ 
	case tov_DKOptimalFlowerMix2: /** \todo LKM: needs to be changed later - need a curve that matched perennial flowers*/ 
	case tov_DKOptimalFlowerMix3: /** \todo LKM: needs to be changed later - need a curve that matched rye grass / tussocky grass*/ 
		return 112; // Setaside

	case tov_HBOrchard: // Hoa Binh: understorey under canopy uses the natural-grass curve
	case tov_HBAcacia:
	case tov_HBNativeMix:
	case tov_HBRegeneration:
	case tov_HBProtectStrict:
	case tov_HBProtectUse:
	case tov_YoungForest:
	case tov_NaturalGrass:
	case tov_FlowerStrip1:
	case tov_FlowerStrip2:
	case tov_FlowerStrip3:
	case tov_Wasteland:
	case tov_WaterBufferZone:
	case tov_PTOliveGroveTraditional: /** \todo EZ: for now same as orchards - needs to be changed later */ 
	case tov_PTOliveGroveTradOrganic: /** \todo EZ: for now same as orchards - needs to be changed later */ 
	case tov_PTOliveGroveIntensive: /** \todo EZ: for now same as orchards - needs to be changed later */ 
	case tov_PTOliveGroveSuperIntensive: /** \todo EZ: for now same as orchards - needs to be changed later */ 
	case tov_PTShrubPastures:	//Needs to be changed later @AAS
	case tov_PTCorkOak:	/** \todo AAS: needs to be changed later */ 
	case tov_PTVineyards: /** \todo EZ: for now same as orchards - needs to be changed later */ 
	case tov_FIBufferZone:
	case tov_FIBufferZone_Perm:
	case tov_FINaturalGrassland:
		/** \todo Below here these crop curves do not seem like they should be the same as natural grass? CJT 3/4/2023 */
	case tov_OrchardCrop:
	case tov_NLOrchardCrop:
	case tov_BEOrchardCrop:
	case tov_DKOrchardCrop_Perm:
	case tov_DKOOrchardCrop_Perm:
	case tov_DKBushFruit_Perm1:
	case tov_DKBushFruit_Perm2:
	case tov_DKOBushFruit_Perm1:
	case tov_DKOBushFruit_Perm2:
	case tov_DKChristmasTrees_Perm:
	case tov_DKOChristmasTrees_Perm:
	case tov_DKEnergyCrop_Perm:
	case tov_DKOEnergyCrop_Perm:
	case tov_DKFarmForest_Perm:
	case tov_DKOFarmForest_Perm:
	case tov_DKFarmYoungForest_Perm:
	case tov_DKOFarmYoungForest_Perm:
	case tov_ITOrchard:
	case tov_ITOOrchard:
	case tov_DKOrchApple:
	case tov_DKOrchPear:
	case tov_DKOrchCherry:
	case tov_DKOrchOther:
	case tov_DKOOrchApple:
	case tov_DKOOrchPear:
	case tov_DKOOrchCherry:
	case tov_DKOOrchOther:
	case tov_DEOrchard:
	case tov_DEOOrchard:
	case tov_DEBushFruitPerm:
	case tov_DEOBushFruitPerm:
		return 90; // Natural grass

	case tov_NoGrowth: 
	case tov_PlantNursery:
	case tov_GenericCatchCrop:
	case tov_DKPlantNursery_Perm:
		return 91; // No growth

	case tov_Lawn: 
		return 94; // Lawn
	
	case tov_Triticale:
	case tov_PLWinterTriticale:	/** \todo EZ: needs to be changed later */ 
	case tov_PTTriticale:	/** \todo AAS: needs to be changed later */ 
	case tov_DETriticale:	/** \todo EZ: needs to be changed later */ 
	case tov_FRWinterTriticale:
		return 7; // Conv. triticale
	
	case tov_OTriticale:
	case tov_DEOTriticale:
		return 107; // Org. triticale
	
	case tov_FITurnipRape: /** \todo LKM: needs to be changed later if specific curve is developed */ 
	case tov_FISpringRape:
    case tov_SpringRape: 
		return 21; // Conv. spring rape
	
	case tov_FIOTurnipRape: /** \todo LKM: needs to be changed later if specific curve is developed */ 
	case tov_FIOSpringRape: 
		return 121; // Org. spring rape

	case tov_NLTulips:	
		return 80; // Tulips

	default: // No matching code so we need an error message of some kind
      sprintf( error_num, "%d", VegReference );
      g_msg->Warn( WARN_FILE,
		   "PlantGrowthData::VegTypeToCurveNum(): Unknown vegetation type:",
		   error_num );
      exit( 1 );
    }
}


bool  PlantGrowthData::StartValid( int a_veg_type, int a_phase )
{
  int a=m_numbers[ a_veg_type ];
  CropGrowth* p=m_growth[a];
  return p-> m_start_valid[ a_phase ];
}


PlantGrowthData* CreatePlantGrowthData()
{
	if (g_crops == NULL)
	{
		g_crops = new PlantGrowthData;
	}

	return g_crops;
}


