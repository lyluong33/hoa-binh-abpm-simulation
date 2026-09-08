//
// rastermap.h
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

#ifndef RASTERMAP_H
#define RASTERMAP_H

#include <cstdlib>
using namespace std;

class Landscape;

// #define to enable range checking of map coordinates.
//#define RASTERMAP_XY_RANGE
// Currently unused.

class RasterMap
{
    /** \brief A 12 character ID for the map */
  char  m_id[12];
  /** \brief The raster of the landscape */
  int  *m_map;
  /** \brief The width of the landscape */
  int   m_width;
  /** \brief The height of the landscape */
  int   m_height;
  /** \brief A pointer to the parent landscape class */
  Landscape * m_landscape;

public:
  // We want to force these to be inline functions.
  int   MapWidth( void ) { return m_width; }
  int   MapHeight( void ) { return m_height; }
  int   Get( int a_x, int a_y );
  char* GetID( void ) { return m_id; }
  int*  GetMagicP( int a_x, int a_y );
  void  Put( int a_x, int a_y, int a_elem )    { m_map[ a_y * m_width + a_x ] = a_elem; }
  RasterMap(const char* a_mapfile, Landscape * m_landscape);
  ~RasterMap( void );
  /** \brief A method for helping remove tiny polygons */
  int CellReplacementNeighbour(int a_x, int a_y, int a_polyref);
  /** \brief A method for removing missing polygons */
  bool MissingCellReplace(int a_x, int a_y, bool a_fieldsonly);
  /** \brief A method for removing missing polygons - tests for edge conditions */
  bool MissingCellReplaceWrap(int a_x, int a_y, bool a_fieldsonly);
protected:
	void Init(const char* a_mapfile, Landscape * m_landscape);
};



inline int* RasterMap::GetMagicP( int a_x, int a_y )
{
  return &m_map[ a_y * m_width + a_x ];
}



inline int RasterMap::Get( int a_x, int a_y ) {
#ifndef DNDEBUG
    if (a_x<0 || a_x>=m_width ||
      a_y<0 || a_y>=m_height ) {
    g_msg->Warn( WARN_BUG, "RasterMap::Get(): Coordinates out of range!", "" );
    g_msg->WarnAddInfo(WARN_BUG, "X Coord ", a_x);
    g_msg->WarnAddInfo(WARN_BUG, "Y Coord ", a_y);
    g_msg->WarnAddInfo(WARN_BUG, "X max ", m_width);
    g_msg->WarnAddInfo(WARN_BUG, "Y max ", m_height);
    exit(1);
  }
#endif
  return m_map[ a_y * m_width + a_x ];
}

#endif // RASTERMAP_H



