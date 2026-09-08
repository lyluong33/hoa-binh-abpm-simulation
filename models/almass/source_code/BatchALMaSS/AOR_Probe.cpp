#include <iostream>
#include <fstream>
#include <string.h>
#include "../Landscape/ls.h"
#include "PopulationManager.h"
#include "AOR_Probe.h"
#include <regex>
using namespace std;
/** \brief Flag for using the habitat location probe */
static CfgBool cfg_AOR_Habitat_Location_On("G_AOR_HABITAT_ON", CFG_CUSTOM, false);
/** \brief ALMaSS tole code for the habitat location probe to count in - default 20 = tole_Field */
static CfgInt cfg_AOR_Habitat_Location_Type("G_AOR_HABITAT_TYPE", CFG_CUSTOM, 20);

AOR_Probe::AOR_Probe(Population_Manager* a_owner, Landscape* a_Landscape, string a_filename)
{
	// Relies on a standard cfg_ to determine whether the AOR Habitat Location Probe is needed.
	m_owner = a_owner;
	m_TheLandscape = a_Landscape;
	m_target_tole = tole_Foobar;
	if (a_filename == "") a_filename = "NWordOutputPrb.txt"; // this is for backwards compatability - this output used to be called NWord
	m_ProbeFile.open(a_filename, ios::out);
	if (!m_ProbeFile) {
		g_msg->Warn(WARN_FILE, "Population_Manager::AOR_Probe: ""Unable to open NWord probe file:", a_filename);
		exit(1);
	}
	m_ProbeFile << "Year" << '\t' << "Day" << '\t' << "Total_no" << '\t' << "Cells50" << '\t' << "Occupied50" << '\t' << "Cells100" << '\t' << "Occupied100" << '\t' << "Cells200" << '\t' << "Occupied200" << '\t' << "Cells400" << '\t' << "Occupied400" << endl;
	if (cfg_AOR_Habitat_Location_On.value())
	{
		m_target_tole = m_TheLandscape->TranslateEleTypes(cfg_AOR_Habitat_Location_Type.value());
		a_filename = std::regex_replace(a_filename, std::regex("\\.txt"), "_HabitatLocation.txt");
		m_ProbeFileHL.open(a_filename, ios::out);
		if (!m_ProbeFileHL) {
			g_msg->Warn(WARN_FILE, "Population_Manager::AOR_Probe: ""Unable to open NWord probe file:", a_filename);
			exit(1);
		}
		m_ProbeFileHL << "Habitat counted from is ALMaSS type " << cfg_AOR_Habitat_Location_Type.value() << endl;
		m_ProbeFileHL << "Year" << '\t' << "Day" << '\t' << "Total_no" << '\t' << "Cells50" << '\t' << "Occupied50" << '\t' << "Cells100" << '\t' << "Occupied100" << '\t' << "Cells200" << '\t' << "Occupied200" << '\t' << "Cells400" << '\t' << "Occupied400" << endl;
	}
	m_gridcountsize[0] = 50;
	m_gridcountsize[1] = 100;
	m_gridcountsize[2] = 200;
	m_gridcountsize[3] = 400;
	for (int g = 0; g < 4; g++) {
		m_gridcountwidth[g] =  m_TheLandscape->SupplySimAreaWidth() / m_gridcountsize[g];
		if (m_TheLandscape->SupplySimAreaWidth() % m_gridcountsize[g] > 0) m_gridcountwidth[g]++; // add one if there is a bit of a square left
		m_gridcountheight[g] = m_TheLandscape->SupplySimAreaHeight() / m_gridcountsize[g];
		if (m_TheLandscape->SupplySimAreaHeight() % m_gridcountsize[g] > 0) m_gridcountheight[g]++; // add one if there is a bit of a square left
		m_totalcells[g] = m_gridcountwidth[g] * m_gridcountheight[g];
		m_gridcount[g].resize(m_totalcells[g]);
		m_gridcountHLoc[g].resize(m_totalcells[g]);
	}
}


void AOR_Probe::WriteData()
{
	int Counted[4];
	int OccupiedCells[4];
	for (int gsz = 0; gsz < 4; gsz++) {
		Counted[gsz] = 0;
		OccupiedCells[gsz] = 0;
		for (int i = 0; i < m_gridcountwidth[gsz]; i++) {
			for (int j = 0; j < m_gridcountheight[gsz]; j++) {
				int res = m_gridcount[gsz][i + j * m_gridcountwidth[gsz]];
				Counted[gsz] += res;
				if (res > 0) OccupiedCells[gsz]++;
			}
		}
	}
	m_ProbeFile << m_TheLandscape->SupplyYearNumber() << '\t' << (int)m_TheLandscape->SupplyDayInYear() << '\t' << Counted[0] << '\t';
	for (int c = 0; c < 3; c++) {
		m_ProbeFile << m_totalcells[c] << '\t' << OccupiedCells[c] << '\t';
	}
	m_ProbeFile << m_totalcells[3] << '\t' << OccupiedCells[3] << endl;
	if (cfg_AOR_Habitat_Location_On.value())
	{
		WriteDataHL();
	}
}

void AOR_Probe::WriteDataHL()
{
	int Counted[4];
	int OccupiedCells[4];
	for (int gsz = 0; gsz < 4; gsz++) {
		Counted[gsz] = 0;
		OccupiedCells[gsz] = 0;
		for (int i = 0; i < m_gridcountwidth[gsz]; i++) {
			for (int j = 0; j < m_gridcountheight[gsz]; j++) {
				int res = m_gridcountHLoc[gsz][i + j * m_gridcountwidth[gsz]];
				Counted[gsz] += res;
				if (res > 0) OccupiedCells[gsz]++;
			}
		}
	}
	m_ProbeFileHL << m_TheLandscape->SupplyYearNumber() << '\t' << (int)m_TheLandscape->SupplyDayInYear() << '\t' << Counted[0] << '\t';
	for (int c = 0; c < 3; c++) {
		m_ProbeFileHL << m_totalcells[c] << '\t' << OccupiedCells[c] << '\t';
	}
	m_ProbeFileHL << m_totalcells[3] << '\t' << OccupiedCells[3] << endl;
}

void AOR_Probe::DoProbe(int a_lifestage) {
	/** Counts all a_lifestage animals in each grid of each size */
	unsigned int total = (unsigned)m_owner->GetLiveArraySize(a_lifestage);
	// Empty old data
	for (int grid = 0; grid < 4; grid++) {
		for (int i = 0; i < m_totalcells[grid]; i++) m_gridcount[grid][i] = 0;
	}
	// For each animal get the location and place it in each of the (4) grids
	for (unsigned j = 0; j < total; j++)      //adult females
	{
		APoint pt = m_owner->SupplyAnimalPtr(a_lifestage, j)->SupplyPoint();
		for (int grid = 0; grid < 4; grid++) {
			int gx = pt.m_x / m_gridcountsize[grid];
			int gy = pt.m_y / m_gridcountsize[grid];
			m_gridcount[grid][gx + gy * m_gridcountwidth[grid]]++;
			if (cfg_AOR_Habitat_Location_On.value())
			{
				if (m_owner->SupplyAnimalPtr(a_lifestage, j)->SupplyPolygonType() == m_target_tole) m_gridcountHLoc[grid][gx + gy * m_gridcountwidth[grid]]++;
			}

		}
	}
	WriteData();
}
//-----------------------------------------------------------------------------

void AOR_Probe::DoProbeInHaitatType(int a_lifestage,TTypesOfLandscapeElement a_tole)
{
	/** Counts all a_lifestage animals in each grid of each size */
	unsigned int total = (unsigned)m_owner->GetLiveArraySize(a_lifestage);
	// Empty old data
	for (int grid = 0; grid < 4; grid++) {
		for (int i = 0; i < m_totalcells[grid]; i++) m_gridcountHLoc[grid][i] = 0;
	}
	// For each animal get the location and place it in each of the (4) grids
	for (unsigned j = 0; j < total; j++)      
	{
		APoint pt = m_owner->SupplyAnimalPtr(a_lifestage, j)->SupplyPoint();
		for (int grid = 0; grid < 4; grid++) {
			int gx = pt.m_x / m_gridcountsize[grid];
			int gy = pt.m_y / m_gridcountsize[grid];
			m_gridcountHLoc[grid][gx + gy * m_gridcountwidth[grid]]++;
		}
	}
	WriteData();
}

