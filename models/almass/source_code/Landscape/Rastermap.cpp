//
// rastermap.cpp
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
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include "MapErrorMsg.h"
#include "Rastermap.h"
#include "Configurator.h"
#include "../Landscape/ls.h"

using namespace std;

RasterMap::RasterMap(const char* a_mapfile, Landscape * a_landscape)
{
	m_landscape = a_landscape;
	FILE * IFile;

	IFile = fopen(a_mapfile, "rb");
	if (!IFile) {
		g_msg->Warn(WARN_FILE, "RasterMap::RasterMap(): Unable to open file",
			a_mapfile);
		exit(1);
	}
	fread(m_id, 1, 12, IFile);
	fclose(IFile);
	Init(a_mapfile, a_landscape);
}

void RasterMap::Init(const char* a_mapfile, Landscape* a_landscape)
{
	m_landscape = a_landscape;
	char error_num[20];
	int   mx;
	int   my;

	ifstream IFile(a_mapfile, ios::binary);
	if (!IFile.is_open()) {
		g_msg->Warn(WARN_FILE, "RasterMap::RasterMap(): Unable to open file",
			a_mapfile);
		exit(1);
	}

	IFile.read(m_id, 12); // Needs smth that is 12 characters 

	IFile.read((char*)&m_width, sizeof(int));
	IFile.read((char*)&m_height, sizeof(int));

	m_map = new int[m_width * m_height];
	if (m_map == NULL) {
		g_msg->Warn(WARN_FATAL,
			"RasterMap::RasterMap(): Out of memory.", "");
		exit(1);
	}

	IFile.read((char*)m_map, m_width * m_height * sizeof(int));

	mx = m_width;
	my = m_height;

	int pref = -1;
	int lastref = -1; // the first "last reference" is not good 

	for (int x = 0; x < mx; x++)
	{
		for (int y = 0; y < my; y++)
		{
			pref = Get(x, y);

			if (pref < 0)
			{
				// This fills in holes caused by negative polyrefs, but if two consequetive holes are found, then it will exit
				sprintf(error_num, "%d", pref);
				g_msg->Warn(WARN_FILE, "RasterMap::RasterMap(): Unknown/Negative polygon map number (could be a hole):", error_num);
				g_msg->WarnAddInfo(WARN_FILE, "X:", x);
				g_msg->WarnAddInfo(WARN_FILE, "Y:", y);

				if (lastref >= 0)
				{
					sprintf(error_num, "%d", lastref);
					g_msg->Warn(WARN_FILE, "Last polygon map number was good though, using it:", error_num);
					Put(x, y, lastref);
				}
				else
				{
					g_msg->Warn(WARN_FILE, "Last polygon map number was bad also, exiting","");
					exit(1);
				}
			}

			lastref = pref;

			if (m_landscape->GetPolymapping(Get(x, y)) == -1)
			{
				// Tripping out here means that the lsb file has a polygon that is not
				// listed in the polygonrefs file
				sprintf(error_num, "%d", Get(x, y));
				g_msg->Warn(WARN_FILE, "RasterMap::RasterMap():" " Unknown polygon ref number:", error_num);
				exit(1);
			}

			if (m_landscape->GetPolymapping(Get(x, y)) == -1)
			{
				// Tripping out here means that the lsb file has a polygon that is not
				// listed in the polygonrefs file
				sprintf(error_num, "%d", Get(x, y));
				g_msg->Warn(WARN_FILE, "RasterMap::RasterMap():" " Unknown polygon ref number:", error_num);
				exit(1);
			}

		}
	}

	IFile.close();

}


RasterMap::~RasterMap( void )
{
  delete[] m_map;
}


int RasterMap::CellReplacementNeighbour(int a_x, int a_y, int a_polyref)
{
	/** 
	* Replaces a cell value with the most common value from the surrounding 8 cells. 
	* If near the edge nothing is done. 
	* Return value is the new value of the cell or -1 if nothing is done.
	* First tests to make sure that the polygon we are removing is at these coords.
	*/
	if ((a_x < 1) || (a_x > m_width - 2) || (a_y < 1) || (a_y > m_height - 2)) return -1;
	int toreplace = Get(a_x, a_y);
	if (toreplace != a_polyref)
	{
		g_msg->Warn("RasterMap::CellReplacementNeighbour: x,y pair does not match polyref ", a_polyref);
		exit(0);
	}
	int surroundingcells[8];
	surroundingcells[0] = Get(a_x - 1, a_y - 1);
	surroundingcells[1] = Get(a_x    , a_y - 1);
	surroundingcells[2] = Get(a_x + 1, a_y - 1);
	surroundingcells[3] = Get(a_x - 1, a_y);
	surroundingcells[4] = Get(a_x + 1, a_y);
	surroundingcells[5] = Get(a_x - 1, a_y + 1);
	surroundingcells[6] = Get(a_x    , a_y + 1);
	surroundingcells[7] = Get(a_x + 1, a_y + 1);
	// Find out how many duplicates we have
	int count[8];
	for (int i = 0; i < 8; i++)
	{
		count[i] = 0;
		for (int j = 0; j < 8; j++)
		{
			if (surroundingcells[j] == surroundingcells[i]) count[i]++;
		}
	}
	int found = -1;
	int index = 0;
	for (int i = 0; i < 8; i++)
	{
		if (count[i]>found) index = i;
		found = count[i];
	}
	// Here we just make sure that we have not chosen another illegal. If so quietly ignore this replacement
	if (m_landscape->SupplyPolygonArea(surroundingcells[index]) < 2) return -1;
	// All OK so replace
	Put(a_x, a_y, surroundingcells[index]);
	return 1; // Signals that replacement occured
}

bool RasterMap::MissingCellReplace(int a_x, int a_y, bool a_fieldsonly)
{
	int surroundingcells[8];
	surroundingcells[0] = Get(a_x - 1, a_y - 1);
	surroundingcells[1] = Get(a_x, a_y - 1);
	surroundingcells[2] = Get(a_x + 1, a_y - 1);
	surroundingcells[3] = Get(a_x - 1, a_y);
	surroundingcells[4] = Get(a_x + 1, a_y);
	surroundingcells[5] = Get(a_x - 1, a_y + 1);
	surroundingcells[6] = Get(a_x, a_y + 1);
	surroundingcells[7] = Get(a_x + 1, a_y + 1);
	// If we prefer fields then check if we have any
	if (a_fieldsonly)
	{
		unsigned offset = unsigned(g_random_fnc(8) + 8); // [8,9,..,15)
		for (unsigned i = 0; i < 8; i++)
		{
			if (m_landscape->IsFieldType(m_landscape->SupplyElementType(surroundingcells[(i + offset) % 8])))
			{
				// We have a next door field, so use this
				Put(a_x, a_y, surroundingcells[(i + offset) % 8]);
				return true;
			}
		}

	}
	return false;
}

bool RasterMap::MissingCellReplaceWrap(int a_x, int a_y, bool a_fieldsonly)
{
	int surroundingcells[8];
	if (a_x >= 1 && a_y >= 1) surroundingcells[0] = Get(a_x - 1, a_y - 1); else surroundingcells[0] = Get(a_x, a_y);
	if (a_y >= 1) surroundingcells[1] = Get(a_x, a_y - 1); else surroundingcells[1] = Get(a_x, a_y);
	if (a_x < m_width-1 && a_y >= 1) surroundingcells[2] = Get(a_x + 1, a_y - 1); else surroundingcells[2] = Get(a_x, a_y);
	if (a_x >= 1) surroundingcells[3] = Get(a_x - 1, a_y); else surroundingcells[3] = Get(a_x, a_y);
	if (a_x < m_width-1) surroundingcells[4] = Get(a_x + 1, a_y); else surroundingcells[4] = Get(a_x, a_y);
	if (a_x >= 1 && a_y < m_height-1) surroundingcells[5] = Get(a_x - 1, a_y + 1); else surroundingcells[5] = Get(a_x, a_y);
	if (a_y < m_height-1) surroundingcells[6] = Get(a_x, a_y + 1); else surroundingcells[6] = Get(a_x, a_y);
	if (a_x < m_width-1 && a_y < m_height-1) surroundingcells[7] = Get(a_x + 1, a_y + 1); else surroundingcells[7] = Get(a_x, a_y);
	// If we prefer fields then check if we have any
	if (a_fieldsonly)
	{
		unsigned offset = unsigned(g_random_fnc(8) + 8); // [8,9,..,15)
		for (unsigned i = 0; i < 8; i++)
		{
			TTypesOfLandscapeElement tole = m_landscape->SupplyElementType(surroundingcells[(i + offset) % 8]);
			if ((tole == tole_Field) || (tole == tole_Orchard) || (tole == tole_PermanentSetaside) || (tole == tole_PermPasture) || (tole == tole_PermPastureLowYield)
				|| (tole == tole_PermPastureTussocky) || (tole == tole_PermPastureTussockyWet) || (tole == tole_Vildtager))
			{
				// We have a next door field, so use this
				Put(a_x, a_y, surroundingcells[(i + offset) % 8]);
				return true;
			}
		}

	}
/*	else
	{
		unsigned offset = unsigned(random(8) + 8); // [8,9,..,15)
		for (unsigned i = 0; i < 8; i++)
		{
			TTypesOfLandscapeElement tole = m_landscape->SupplyElementType(surroundingcells[(i + offset) % 8]);
			if ((tole != tole_Missing))
			{
				// We have a next door useful polygon, so use this
				Put(a_x, a_y, surroundingcells[(i + offset) % 8]);
				return true;
			}
		}
	}
*/
	return false;
}
