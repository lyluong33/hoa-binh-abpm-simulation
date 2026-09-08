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
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "Configurator.h"
#include "MapErrorMsg.h"
#include "ls.h"

CfgStr   l_map_rotation_files_prefix("MAP_ROTATION_FILES_PREFIX",
				     CFG_CUSTOM, "" );

class CropRotation *g_rotation;



CropRotation::CropRotation( int a_num_crops )
{
  FILE *inpfile;
  char filename[50];

  m_rots.resize( a_num_crops );
  m_start.resize( a_num_crops );

  for (int i=0; i<a_num_crops; i++) {
    m_rots[ i ] = new Rotation;
    m_start[ i ] = new Starter;
  }

  for (int i=0; i<NoFarmTypes; i++) {
    // Now works for any number of rotations.
    sprintf( filename, "%sFarmType_%d.rot",
	     l_map_rotation_files_prefix.value(),
	     i );

    inpfile = fopen(filename, "r" );
	if (!inpfile) {
      g_msg->Warn( WARN_FILE, "CropRotation::CropRotation():"
		   " Unable to open file ", filename);
      exit(1);
    }
    for (int j=0; j<a_num_crops; j++) {
      fscanf( inpfile, "%d", (int*)&(m_rots[ j ]->CropNum[ i ]) );
      for (int k=0; k<4; k++) {
	      fscanf( inpfile, "%d", (int*)&m_rots[ j ]->NewCrop[ i ][ k ] );
	      fscanf( inpfile, "%d", &m_rots[ j ]->Percent[ i ][ k ] );
      }
    }
    fclose( inpfile );

    sprintf( filename, "FarmType_%d.stt", i );

     inpfile = fopen(filename, "r" );
	 if (!inpfile) {
      g_msg->Warn( WARN_FILE, "CropRotation::CropRotation():"
		   " Unable to open file ", filename);
      exit(1);
    }
    for (int j=0; j<a_num_crops; j++) {
      fscanf( inpfile, "%d %d",
	      (int*)&m_start[ j ]->CropNum[ i ],
	      &m_start[ j ]->Percent[ i ] );
    }
    fclose(inpfile);
  }
}



CropRotation::~CropRotation( void )
{
  for (unsigned int i=0; i<m_rots.size(); i++) {
    delete m_rots[ i ];
    delete m_start[ i ];
  }
}


TTypesOfVegetation
  CropRotation::GetNextCrop( int a_farmtype, int a_current_crop )
{
  //int NumCrops = g_crops->GetNumCrops();
  for ( int i=0; i<4; i++ ) {
    int percent = m_rots[ a_current_crop ]->Percent[ a_farmtype ][ i ];

    // Lazy evaluation for experts. ;-)
    if ( (3==i) ||
	 (-1 == m_rots[ a_current_crop ]->Percent[ a_farmtype ][ i+1 ]) ||
	 (g_random_fnc(100) < percent) ) {
      return m_rots[ a_current_crop ]->NewCrop[ a_farmtype ][ i ];
    }
  }
  g_msg->Warn( WARN_BUG, "CropRotation::GetNextCrop():"
	       " Unable to recover crop type?!", "");
  exit(1);
}



TTypesOfVegetation
  CropRotation::GetFirstCrop( int a_farmtype, bool *a_low_nutrient )
{
  int num_crops = g_crops->GetNumCrops();

  for ( int i=0; i<num_crops; i++ ) {
    int percent = m_start[ i ]->Percent[ a_farmtype ];

    if ( (num_crops==i) ||
	 (-1 == m_start[ i+1 ]->Percent[ a_farmtype ]) ||
	 (g_random_fnc(100) < percent) ) {
      *a_low_nutrient = g_crops->GetNutStatus( i );
      return (TTypesOfVegetation) i;
    }
  }
  g_msg->Warn( WARN_BUG, "CropRotation::GetFirstCrop():"
	       " Unable to recover crop type?!", "");
  exit(1);
}

