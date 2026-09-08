#pragma once
typedef vector < int > ListOfCells;

class AOR_Probe
{
protected:
	ofstream m_ProbeFile;
	ofstream m_ProbeFileHL;
	int m_gridcountwidth[4];
	int m_gridcountheight[4];
	int m_gridcountsize[4];
	int m_totalcells[4];
	ListOfCells m_gridcount[4];
	ListOfCells m_gridcountHLoc[4];

	Landscape* m_TheLandscape;
	TTypesOfLandscapeElement m_target_tole;
	Population_Manager* m_owner;
public:
	AOR_Probe(Population_Manager* a_owner, Landscape* a_TheLandscape, string a_filename);
	virtual ~AOR_Probe() {
		CloseFile();
	}

	virtual void CloseFile() {
		m_ProbeFile.close();
	}
	void WriteData();
	void WriteDataHL();
	virtual void DoProbe(int a_lifestage);
	/** \brief Records the locations of all beetles and classifies them as to in a habitat type or not */
	virtual void DoProbeInHaitatType(int a_lifestage, TTypesOfLandscapeElement a_tole);
};