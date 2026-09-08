//
// plants.h
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


#ifndef PLANTS_H
#define PLANTS_H

#include <vector>
#include <algorithm>
#include <cstdio>
#include <fstream>
using namespace std;

const unsigned int MaxNoInflections = 15;

extern class PlantGrowthData * g_crops;
extern class PollenNectarDevelopmentData* g_nectarpollen;

/**\brief Growth phase indicator*/
/**
janfirst = from Jan 1st
sow = after sowing
marchfirst = after march 1st
harvest1 = after first harvest/cut
harvest2 = after 2nd harvest/cut and subsequent ones (if different veg behaviour to '3') 
*/
typedef enum {
  janfirst = 0,
  sow,
  marchfirst,
  harvest1,
  harvest2,
  vegphase_foobar
}
Growth_Phases;



// 0: Leaf area green.
// 1: Leaf area total.
// 2: Height.

class CropGrowth {
public:
  bool m_lownut = false; // Low nutrient status.
  double m_dds[5][MaxNoInflections] = { 0 }; // DDegs at inflection
  double m_slopes[5][3][MaxNoInflections] = { 0 };
  double m_start[5][3] = { 0 };
  bool m_start_valid[5] = { 0 };
};

extern class PlantGrowthData * g_crops;


class PlantGrowthData {
  vector < CropGrowth * > m_growth;
  vector <vector<vector<double>>> m_final_ddeg;
  vector < int > m_numbers;
  int m_num_crops;
  //FILE * m_ifile;
  ifstream m_ifile;
  double * m_weed_percent;
  double * m_bug_percent_a;
  double * m_bug_percent_b;
  double * m_bug_percent_c;
  int * m_bug_percent_d;

  double FindDiff( double a_ddegs, double a_yddegs, int a_plant, int a_phase, int a_type );
  unsigned int FindCropNum( ifstream& ist );
  //void SetVegNum( unsigned int a_i, const char * a_cropcurvefile );
  void SetVegNum( unsigned int a_i, ifstream& ist,const char * a_cropcurvefile );


  void MakeBugPercentArray( void );
  void ReadBugPercentageFile( void );

public:

  // NOTICE: These functions take *plant numbers* like in the
  // CropGrowth.txt file, *NOT* indices into the m_growth array.
	/** \brief Get the differential in LA green for the day degrees experienced */
	double GetLAgreenDiff(double a_ddegs, double a_yddegs, int a_plant, int a_phase) {
		return FindDiff(a_ddegs, a_yddegs, a_plant, a_phase, 0);
	}

  /** \brief Get the final positive growing day degrees based on LA green*/
  double GetMaxDdegLAgreen(int a_plant, int a_phase) {
    int index = m_numbers[ a_plant ];
    return m_final_ddeg.at(index).at(a_phase).at(0);
  }
	/** \brief Get the differential in LA total for the day degrees experienced */
	double GetLAtotalDiff(double a_ddegs, double a_yddegs, int a_plant, int a_phase) {
		return FindDiff(a_ddegs, a_yddegs, a_plant, a_phase, 1);
	}
  /** \brief Get the final positive growing day degrees based on LA total*/
  double GetMaxDdegLAtotal(int a_plant, int a_phase) {
    int index = m_numbers[ a_plant ];
    return m_final_ddeg.at(index).at(a_phase).at(1);
  }
	/** \brief Get the differential in veg height for the day degrees experienced */
	double GetHeightDiff(double a_ddegs, double a_yddegs, int a_plant, int a_phase) {
		return FindDiff(a_ddegs, a_yddegs, a_plant, a_phase, 2);
	}
  /** \brief Get the final positive growing day degrees based on veg height*/
  double GetMaxDdegHeight(int a_plant, int a_phase) {
    int index = m_numbers[ a_plant ];
    return m_final_ddeg.at(index).at(a_phase).at(2);
  }

	// The versions below allow scaling of the curve on calling
	/** \brief Get the differential in LA green for the day degrees experienced, scalable depending on plant growth ability */
	double GetLAgreenDiffScaled(double a_ddegs, double a_yddegs, int a_plant, int a_phase, double a_scaler) { return a_scaler* GetLAgreenDiff(a_ddegs, a_yddegs, a_plant, a_phase); }
	/** \brief Get the differential in LA total for the day degrees experienced, scalable depending on plant growth ability */
	double GetLAtotalDiffScaled(double a_ddegs, double a_yddegs, int a_plant, int a_phase, double a_scaler) { return a_scaler* GetLAtotalDiff(a_ddegs, a_yddegs, a_plant, a_phase); }
	/** \brief Get the differential in veg height for the day degrees experienced, scalable depending on plant growth ability */
	double GetHeightDiffScaled(double a_ddegs, double a_yddegs, int a_plant, int a_phase, double a_scaler) { return a_scaler* GetHeightDiff(a_ddegs, a_yddegs, a_plant, a_phase); }

	double GetStartValue(int a_veg_type, int a_phase, int a_type) {
		return m_growth[m_numbers[a_veg_type]]->m_start[a_phase][a_type];
	}

  bool StartValid( int a_veg_type, int a_phase );

  /* { return m_growth[ m_numbers[ a_veg_type ]]-> m_start_valid[ a_phase ]; } */
  // Number of different crops read from disk.
  int GetNumCrops() {
    return m_num_crops;
  }

  // Translation between growth curve file crop numbers and the internal
  // representation.
  //TTypesOfVegetation TranslateFileVegTypes( int a_filetype );
  //int BackTranslateFileVegTypes( TTypesOfVegetation a_systemtype );
  int VegTypeToCurveNum( TTypesOfVegetation VegReference );

  // **cjt** added 30/01/2004
  double GetWeedPercent( TTypesOfVegetation a_letype ) {
    return m_weed_percent[ a_letype ];
  }

  // **cjt** added 27/05/2003
  double GetBugPercentA( TTypesOfVegetation a_letype ) {
    return m_bug_percent_a[ a_letype ];
  }

  // **cjt** added 27/05/2003
  double GetBugPercentB( TTypesOfVegetation a_letype ) {
    return m_bug_percent_b[ a_letype ];
  }

  // **cjt** added 27/05/2003
  double GetBugPercentC( TTypesOfVegetation a_letype ) {
    return m_bug_percent_c[ a_letype ];
  }

  // **cjt** added 12/03/2003
  double GetBugPercentD( TTypesOfVegetation a_letype ) {
    return (double) m_bug_percent_d[ a_letype ];
  }

  bool GetNutStatus( int a_plant_num ) {
    return m_growth[ a_plant_num ]->m_lownut;
  }

  bool GetNutStatusExt( int a_plant ) {
    return m_growth[ m_numbers[ a_plant ]]->m_lownut;
  }

  PlantGrowthData( const char * a_cropcurvefile = "default" );
  ~PlantGrowthData();
};

PlantGrowthData* CreatePlantGrowthData();

#endif // PLANTS_H

