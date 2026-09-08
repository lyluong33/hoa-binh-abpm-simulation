//
// hedgebanks.cpp
//
// This file implements the methods in the Landscape class needed for
// adding artificial hedgebanks.

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

#include "ls.h"
#include "Map_cfg.h"
#include <math.h>


#define HB_MAGIC 1000
#define HB_MAGIC_COLOR 999
#define HB_MAGIC_PAINTER_BIT 0x40000000

// As a fraction.
static CfgFloat l_map_art_hb_width( "MAP_ART_HB_WIDTH",
				    CFG_CUSTOM, 0.3 );
static CfgFloat l_map_art_hb_seg_len("MAP_ART_HB_SEG_LEN",
				     CFG_CUSTOM, 100.0 );
static CfgInt   l_map_art_hb_nums( "MAP_ART_HB_NUMS",
				 CFG_CUSTOM, 3 );
static CfgInt   l_map_art_hb_tran_prob( "MAP_ART_HB_TRAN_PROB",
					CFG_CUSTOM, 30 );
// As a fraction.
static CfgFloat l_map_art_hb_core_thres("MAP_ART_HB_STRIPE_THRES",
					CFG_CUSTOM, 0.4 );


void Landscape::hb_Add( void )
{
  // Make a local copy of the actual map parameters for speed
  // purposes. We are going to use these *a lot*.
  hb_map    = m_land->GetMagicP( 0, 0 );
  hb_width  = m_land->MapWidth();
  hb_height = m_land->MapHeight();
  hb_size   = hb_width * hb_height;

  //#ifdef HB_TESTING
  //m_elems[m_polymapping
  //[hb_map[750 + 600*hb_width]]]->SetElementType( tole_Hedges );
  //#endif

  // Generate a list of polygon numbers + HB_MAGIC for all hedge elements
  // in our current map.
  hb_FindHedges();

  // As one lemming at the cliff edge said to the other: "Let's Go!"

  // For every hedge polygon:
  cout << "Hedges: " << int(hb_hedges.size()) << endl;
  fflush(stdout);

  double l_inc = 50.0/((double)hb_hedges.size());
  double l_count = 0.001;

  for ( unsigned int i=0; i<hb_hedges.size(); i++ ) {
    l_count += l_inc;
    if ( l_count > 1.0 ) {
      l_count = 0.001;
      cout <<".";
      fflush(stdout);
    }
    // Find the enclosing bounding box for our current polygon.
    if ( ! hb_FindBoundingBox( hb_hedges[i] )) {
      // Might want to raise a runtime error upon ending here.
      // The test for hb_FindBoundingBox() fails if we have a polygon
      // without any pixels in the main map!
      continue;
    }

    // Move all polygon numbers in our bounding box up! by HB_MAGIC
    // to make room for our 'paint' values in the space below.
    hb_UpPolyNumbers();

    hb_ClearPolygon( hb_hedges[i] + HB_MAGIC );

    hb_PaintBorder( 0 );

    int l_color = 0;
    while ( hb_PaintWhoHasNeighbourColor( l_color,
					  l_color + 1 )) {
      l_color++;
    }

    double l_percent = (double)hb_core_pixels/
      ((double)hb_core_pixels+(double)hb_border_pixels);

    if ( l_percent > l_map_art_hb_core_thres.value()) {
		// We ought to have at least *some* visible core area
		// within the polygon, so do a simple scan.

		// l_color now happens to contain the maximum contour/color value
		// within our current polygon.
		int l_min = (int)((double)l_color * l_map_art_hb_width.value());
		while ( l_color > l_min ) {
			hb_MarkTopFromLocalMax( l_color );
			hb_ResetColorBits();
			l_color--;
		}
	} else {
		// Very thin hedge (1->2 meters),
		// use special border case scan when adding hedge banks.
		hb_MarkTheBresenhamWay();
    }

    hb_RestoreHedgeCore( hb_hedges[i] + HB_MAGIC );

    // All the previous steps were just preparations.
    // Now do the serious stuff.
    hb_AddNewHedgebanks( hb_hedges[i] );

#ifdef HB_TESTING
    // Sanity check.
    for ( int j=0; j<hb_size; j++ ) {
      if ( m_polymapping[ hb_map[ j ] - HB_MAGIC ] == -1 ) {
	      cout << "Oh dear!" << endl;
	      exit(1);
      }
    }

    char ffname[24];
    sprintf( ffname, "test%02d.ppm", i);
#endif

    // Restore order to the map before continuing.
    hb_DownPolyNumbers();

#ifdef HB_TESTING
    hb_dump_map( 0, hb_width, 0, hb_height, ffname, false );
#endif
  }

  cout << endl;;
#ifdef HB_TESTING
  // Quick reminder.
  for ( unsigned int i=0; i<m_elems.size(); i++ ) {
    if ( m_elems[i]->GetArea() < 0.9 &&
	 m_elems[i]->GetElementType() != tole_FieldBoundary ) {
      // Note: Field boundaries have not been scanned yet at this point,
      // so they will have a zero area right here and now.
      cout << "Warning: Polygon " << m_elems[i]->GetPoly() << "of type " << PolytypeToString( m_elems[i]->GetElementType() << "came up with area" << m_elems[i]->GetArea() << endl;
    }
  }
  exit(0);
#endif
}



void Landscape::hb_GenerateHBPolys( void )
{
  hb_new_hbs.resize( l_map_art_hb_nums.value() );

  for ( int i =0; i<l_map_art_hb_nums.value(); i++ ) {
    hb_new_hbs[ i ] = NewElement( tole_HedgeBank );
    hb_new_hbs[ i ]->SetArea( 0.0 );
    hb_new_hbs[ i ]->SetSubType( i );
    m_elems.resize( m_elems.size() + 1 );
    m_elems[ m_elems.size() - 1 ] = hb_new_hbs[ i ];
    m_polymapping[ hb_first_free_poly_num ] =
       (int) m_elems.size() - 1;
    hb_new_hbs[ i ]->SetPoly( hb_first_free_poly_num++ );
  }
}



void Landscape::hb_AddNewHedgebanks( int /* a_orig_poly_num */ )
{
  hb_GenerateHBPolys();

  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++ ) {
    for ( int l_x=hb_min_x; l_x<=hb_max_x; l_x++ ) {
      int i = l_y*hb_width + l_x;
      int l_val = hb_map[ i ];
      if ( l_val >= 0 && l_val < HB_MAGIC ) {
	hb_map[ i ] = hb_StripingDist() + HB_MAGIC;
	/*
	m_elems[m_polymapping[hb_map[ i ]-HB_MAGIC]]->AddArea( 1.0 );
	m_elems[m_polymapping[hb_map[ i ]-HB_MAGIC]]->m_squares_in_map++;
	m_elems[m_polymapping[a_orig_poly_num]]->AddArea( -1.0 );
	if ( m_elems[m_polymapping[a_orig_poly_num]]->GetArea() < 0.0 ) {
	  printf("%d %d !!!\n", l_x, l_y );
	  exit(1);
	}
	*/
	//m_elems[m_polymapping[a_orig_poly_num]]->m_squares_in_map--;
      }
    }
  }
}



int Landscape::hb_StripingDist( void )
{
  // Striping distribution function for the new hedgebanks.
  static LE* l_curr_ele = NULL;

  if ( !l_curr_ele ) {
    l_curr_ele = hb_new_hbs[ g_random_fnc( l_map_art_hb_nums.value()) ];
    return l_curr_ele->GetPoly();
  }

  if ( g_random_fnc(100) < l_map_art_hb_tran_prob.value() )
    l_curr_ele = hb_new_hbs[ g_random_fnc( l_map_art_hb_nums.value()) ];

  return l_curr_ele->GetPoly();
}



bool Landscape::hb_FindBoundingBox( int a_poly_num )
{
  hb_min_x = hb_width + 1;
  hb_max_x = -1;

  hb_min_y = hb_height + 1;
  hb_max_y = -1;

  bool l_found = false;

  // Search for every pixel which belongs to the current polygon, and
  // min and max values for x and y. Values are inclusive.
  for ( int l_y=0; l_y<hb_height; l_y++ ) {
    for ( int l_x=0; l_x<hb_width; l_x++ ) {
      if ( hb_map[ l_y*hb_width + l_x ] == a_poly_num ) {
	l_found = true;
	if ( l_x < hb_min_x)
	  hb_min_x = l_x;
	else if ( l_x > hb_max_x)
	  hb_max_x = l_x;
	if ( l_y < hb_min_y)
	  hb_min_y = l_y;
	else if ( l_y > hb_max_y)
	  hb_max_y = l_y;
      }
    }
  }

  return l_found;
}



void Landscape::hb_FindHedges( void )
{
  for ( unsigned int i=0; i<m_elems.size(); i++ ) {
    if ( tole_Hedges == m_elems[i]->GetElementType() ) {
      hb_hedges.resize( hb_hedges.size() + 1 );
      hb_hedges[ hb_hedges.size() - 1 ] =
	m_elems[i]->GetPoly();
    }
  }
}



void Landscape::hb_UpPolyNumbers( void )
{
  for ( int i=0; i<hb_size; i++ ) {
    hb_map[ i ] += HB_MAGIC;
  }
}



void Landscape::hb_ResetColorBits( void )
{
  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++ ) {
    for ( int l_x=hb_min_x; l_x<=hb_max_x; l_x++ ) {
      int l_coord = l_y*hb_width + l_x;
      if ( hb_map[l_coord] < 0 )
	hb_map[l_coord] |= HB_MAGIC_PAINTER_BIT;
    }
  }
}



void Landscape::hb_RestoreHedgeCore( int a_orig_poly_number )
{
  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++ ) {
    for ( int l_x=hb_min_x; l_x<=hb_max_x; l_x++ ) {
      int l_coord = l_y*hb_width + l_x;
      if (  hb_map[l_coord] < 0 ) {
		hb_map[l_coord] = a_orig_poly_number;
      }
    }
  }
}



inline bool Landscape::hb_MapBorder(int a_x, int a_y )
{
  if ( a_x < 0 ||
       a_y < 0 ||
       a_x >= hb_width ||
       a_y >= hb_height ) {
    return true;
  }

  return false;
}



inline bool Landscape::hb_HasOtherNeighbour( int a_x, int a_y )
{
  bool l_has_neighbour = false;
  bool loop=false;

  // Who said C++ needs the goto statement? ;-)
  do {
    a_x -= 1;
    a_y -= 1;

    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
	l_has_neighbour = true;
	break;
    }

    a_x++;
    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
      l_has_neighbour = true;
      break;
    }

    a_x++;
    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
      l_has_neighbour = true;
      break;
    }

    a_x -= 2;
    a_y += 1;
    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
      l_has_neighbour = true;
      break;
    }

    a_x += 2;
    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
      l_has_neighbour = true;
      break;
    }

    a_x -= 2;
    a_y += 1;

    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
      l_has_neighbour = true;
      break;
    }

    a_x++;
    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
      l_has_neighbour = true;
      break;
    }

    a_x++;
    if ( hb_MapBorder( a_x, a_y ) ||
	 hb_map[ a_x + a_y*hb_width ] >= HB_MAGIC ) {
      l_has_neighbour = true;
      break;
    }
  } while ( loop );  // was while (false) CJT 16-05-06

  return l_has_neighbour;
}



void Landscape::hb_ClearPolygon( int a_poly_num )
{
  // 'Paint' every pixel which belongs to the current polygon with the
  // value HB_MAGIC_COLOR.
  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++ ) {
    for ( int l_x=hb_min_x; l_x<=hb_max_x; l_x++ ) {
      int l_coord = l_y*hb_width + l_x;
      if ( hb_map[ l_coord ] == a_poly_num ) {
	hb_map[ l_coord ] = HB_MAGIC_COLOR;
      }
    }
  }
}



void Landscape::hb_PaintBorder( int a_color )
{
  hb_border_pixels = 0;
  hb_core_pixels   = 0;
  // Paint every pixel which belongs to the current polygon, and which
  // does have a neighbour *not* equal to HB_MAGIC_COLOR (our marker value).
  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++ ) {
    for ( int l_x=hb_min_x; l_x<=hb_max_x; l_x++ ) {
      int l_coord = l_y*hb_width + l_x;
      if ( hb_map[ l_coord ] == HB_MAGIC_COLOR ) {
		if ( hb_HasOtherNeighbour( l_x, l_y )) {
		  hb_map[ l_coord ] = a_color;
		  hb_border_pixels++;
		} else {
		  hb_core_pixels++;
		}
      }
    }
  }
}



inline bool Landscape::hb_HasNeighbourColor( int a_x, int a_y,
					     int a_neighbour_color )
{
  bool l_neighbour_has_color = false;
  bool loop=false;

  do {
    a_x -= 1;
    a_y -= 1;
    if ( a_x >= 0 && a_y >= 0 &&
	 hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }

    a_x++;
    if ( hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }

    a_x++;
    if ( a_x < hb_width &&
	 hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }

    a_x -= 2;
    a_y += 1;
    if ( a_x >= 0 &&
	 hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }

    a_x += 2;
    if ( a_x < hb_width &&
	 hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }

    a_x -= 2;
    a_y += 1;

    if ( a_y >= hb_height )
      break;

    if ( a_x >= 0 &&
	 hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }

    a_x++;
    if ( hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }

    a_x++;
    if ( a_x < hb_width &&
	 hb_map[ a_x + a_y*hb_width ] == a_neighbour_color ) {
      l_neighbour_has_color = true;
      break;
    }
  } while ( loop );  // was while (false) CJT 16-05-06

  return l_neighbour_has_color;
}



bool Landscape::hb_PaintWhoHasNeighbourColor( int a_neighbour_color,
					      int a_new_color )
{
  // Paint every pixel which belongs to the current polygon, and which
  // has a neighbour with a color equal to a_neighbour_color.

  bool l_at_least_one = false;
  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++ ) {
    for ( int l_x=hb_min_x; l_x<=hb_max_x; l_x++ ) {
      if ( hb_map[l_y*hb_width + l_x] == HB_MAGIC_COLOR &&
	   hb_HasNeighbourColor( l_x, l_y, a_neighbour_color )) {
	hb_map[l_y*hb_width + l_x] = a_new_color;
	l_at_least_one = true;
      }
    }
  }

  return l_at_least_one;
}



inline int Landscape::hb_MaxUnpaintedNegNeighbour( int a_x,
						   int a_y )
{
  int l_max_neg = 0;
  int l_val;
  bool loop=false; // This is just to avoid the warning about constant expression if we use: do {...} while (false)

  do {
    a_x -= 1;
    a_y -= 1;
    if ( a_x >= 0 && a_y >= 0 ) {
      l_val = hb_map[ a_x + a_y*hb_width ];
      if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
	l_max_neg = l_val;
      }
    }

    a_x++;
    l_val = hb_map[ a_x + a_y*hb_width ];
    if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
      l_max_neg = l_val;
    }

    a_x++;
    if ( a_x < hb_width ) {
      l_val = hb_map[ a_x + a_y*hb_width ];
      if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
	l_max_neg = l_val;
      }
    }

    a_x -= 2;
    a_y += 1;
    if ( a_x >= 0 ) {
      l_val = hb_map[ a_x + a_y*hb_width ];
      if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
	l_max_neg = l_val;
      }
    }

    a_x += 2;
    if ( a_x < hb_width ) {
      l_val = hb_map[ a_x + a_y*hb_width ];
      if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
	l_max_neg = l_val;
      }
    }

    a_x -= 2;
    a_y += 1;

    if ( a_y >= hb_height )
      break;

    if ( a_x >= 0 ) {
      l_val = hb_map[ a_x + a_y*hb_width ];
      if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
	l_max_neg = l_val;
      }
    }

    a_x++;
    l_val = hb_map[ a_x + a_y*hb_width ];
    if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
      l_max_neg = l_val;
    }

    a_x++;
    if ( a_x < hb_width ) {
      l_val = hb_map[ a_x + a_y*hb_width ];
      if ( l_val < l_max_neg && (l_val & HB_MAGIC_PAINTER_BIT)) {
	l_max_neg = l_val;
      }
    }
  } while ( loop );

  return l_max_neg;
}



void Landscape::hb_MarkTheBresenhamWay( void )
{
  double l_hb     = l_map_art_hb_width.value()*10000.0;
  double l_up     = 10000.0 - l_hb;
  bool  l_is_hb  = true;
  double l_length = (double)(g_random_fnc( (int)l_hb )) * 0.0001 *
    l_map_art_hb_seg_len.value();
  double l_hb_remain = 0.0;
  double l_up_remain = 0.0;

  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++) {
    // Note: The missing update of l_x below is intentional.
    for ( int l_x=hb_min_x; l_x<=hb_max_x; ) {
      int l_coord = l_y*hb_width + l_x;
      if ( hb_map[ l_coord ] < HB_MAGIC ) {
	// 'Painting' with a negative value turns a pixel into
	// hedgebank 'core', ie. hedge!

	// Paint segment length with appropriate type.
	if ( l_length >= 1.0 ) {
	  if ( l_is_hb ) {
	    hb_map[ l_coord ] = 0;
	  } else {
	    hb_map[ l_coord ] = -1;
	  }
	  l_length -= 1.0;
	  l_x++;
	} else {
	  // Change state.
	  if ( l_is_hb ) {
	    l_is_hb     = false;
	    l_hb_remain = l_length;
	    l_length = (double)(g_random_fnc( (int)l_up )) * 0.0001 *
	      l_map_art_hb_seg_len.value() + l_up_remain;
	  } else {
	    l_is_hb     = true;
	    l_up_remain = l_length;
	    l_length = (double)(g_random_fnc( (int)l_hb )) * 0.0001 *
	      l_map_art_hb_seg_len.value() + l_hb_remain;
	  }
	}
      } else {
	// Pixel not part of the hedge, just update the coordinate.
	l_x++;
      }
    }
  }
}


void Landscape::hb_MarkTopFromLocalMax( int a_color )
{
  /** 
  Sweeps through the map.If we find a pixel, which has a value
  equal to a_color and *not* a negative neighbour, then mark it as
  negative with an absolute value equal to the number of steps from
  here and out to the center/perimeter polygons border, calculated
  as the given percentage. \n
  If one or more negative numbers are found among the neighbours,
  some that were not marked during this round, then assign a
  negative value equal to the minimum found plus one.
  */
  for ( int l_y=hb_min_y; l_y<=hb_max_y; l_y++ ) {
    for ( int l_x=hb_min_x; l_x<=hb_max_x; l_x++ ) {
      int l_coord = l_y*hb_width + l_x;
      int l_max;
      if ( hb_map[ l_coord ] == a_color ) {
	l_max = hb_MaxUnpaintedNegNeighbour( l_x, l_y );
	if ( l_max < 0 ) {
	  if ( ++l_max < 0 ) {
	    hb_map[ l_coord ]  = l_max;
	  } else {
	    // l_max was precisely -1
	    hb_map[ l_coord ]  = -1;
	  }
	} else {
	  // We are a local maxima.
	  hb_map[ l_coord ]  = -a_color;
	}
	hb_map[ l_coord ] ^= HB_MAGIC_PAINTER_BIT;
      }
    }
  }
}


void Landscape::hb_DownPolyNumbers( void )
{
  for ( int i=0; i<hb_size; i++ ) {
    if ( hb_map[ i ] >= HB_MAGIC ) {
      hb_map[ i ] -= HB_MAGIC;
    }
  }
}


#ifdef HB_TESTING

#define SV_UINT32 unsigned int
#define SV_INT32  int
#define SV_UINT8  unsigned char
#define SV_INT8   char


const unsigned int hb_dump_colors [tole_Foobar] = {
        0x00AAFFAA, //    tole_Hedges
        0x0011BBCC, //    tole_RoadsideVerge, // 13
        0x0011AAAA, //    tole_Railway, // 118
        0x00339933, //    tole_FieldBoundary, // 160
        0x00AA6666, //    tole_Marsh, // 95
        0x0022AA22, //    tole_Scrub, // 70
        0x00CCCCCC, //    tole_Field,  //grey
	0x0099aa99, //    tole_PermPastureTussocky, // 26
        0x00FF3311, //    tole_UnsprayedFieldMargin,
	0x00BBCCBB, //    tole_PermanentSetaside, // 33
        0x00BBFFBB, //    tole_PermPasture, // 35
        0x0099FF99, //    tole_NaturalGrassDry, // 110
        0x0044AA55, //    tole_RiversidePlants, // 98
        0x00558855, //    tole_PitDisused, // 75
        0x0022CC22, //    tole_RiversideTrees, // 97
        0x0022AA22, //    tole_DeciduousForest, // 40
        0x00227722, //    tole_MixedForest,     // 60
        0x00225522, //    tole_ConiferousForest, // 50
	0x00223322, //    tole_YoungForest, // 55
        0x00EEEEEE, //    tole_StoneWall, // 15
        0x0088AA88, //    tole_Garden, //11
        0x00448888, //    tole_Track,  // 123
        0x00777777, //    tole_SmallRoad,  // 122
        0x00444444, //    tole_LargeRoad,  // 121
        0x00666666, //    tole_Building, // 5
        0x00888888, //    tole_Urban, // 10
        0x00222222, //    tole_ActivePit, // 115
        0x00DD3333, //    tole_Freshwater, // 90
        0x00FF8888, //    tole_River, // 96
        0x00FF1111, //    tole_Saltwater,  // 80
        0x00DD1111, //    tole_Coast, // 100
	0x005555FF, //    tole_HedgeBank, // No official number. 140. FN.
	0x0088FFFF, //    tole_Heath,
        0x000000FF  //    tole_BareRock
};



void Landscape::hb_dump_map( int a_beginx, int a_width,
			     int a_beginy, int a_height,
			     char* a_filename,
			     bool a_high_numbers )
{
  SV_UINT32 linesize   = a_width*3;
  SV_UINT8* linebuffer = (SV_UINT8*)malloc(sizeof(SV_UINT8)* linesize);

  if ( linebuffer == NULL ) {
    printf("hb_dump_color_map(): Out of memory!\n",
	   "" );
    exit(1);
  }

  FILE* l_file = fopen( a_filename, "w" );
  if ( ! l_file ) {
    printf("hb_dump_color_map(): Unable to open file for writing: %s\n",
	   a_filename );
    exit(1);
  }

  fprintf( l_file, "P6\n%d %d %d\n",
	   a_width,
	   a_height,
	   255 );

  for ( int line=a_beginy; line< a_beginy + a_height; line++ ) {
    int i = 0;
    for ( int column=a_beginx; column < a_beginx + a_width; column++ ) {
      int localcolor = hb_dump_color( column, line, a_high_numbers );
      linebuffer [ i++ ] =  localcolor        & 0xff;
      linebuffer [ i++ ] = (localcolor >>  8) & 0xff;
      linebuffer [ i++ ] = (localcolor >> 16) & 0xff;
    }
    fwrite( linebuffer, sizeof(SV_UINT8), linesize, l_file );
  }

  fclose( l_file );
  free( linebuffer );
}


inline int Landscape::hb_dump_color( int a_x, int a_y,
				     bool a_high_numbers )
{
  int l_map_val = hb_map[ a_x + a_y*hb_width ];

  if ( l_map_val < 0 )
    return 0x000000ff;

  if ( a_high_numbers && m_polymapping[l_map_val] != -1 )
    l_map_val -= HB_MAGIC;

  if ( m_polymapping[l_map_val] != -1 ) {
    return hb_dump_colors[ m_elems[m_polymapping
				  [l_map_val]]->GetElementType() ];
  }

  return 0x00ffffff - 32*l_map_val;
}

#endif // HB_TESTING


