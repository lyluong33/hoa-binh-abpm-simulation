#ifndef HABITATMAP_H
#define HABITATMAP_H
#include <memory>
#include <vector>

class Landscape;


typedef enum{
    TTypeofMapBiomass = 0,
    TTypeofMapFarmOwnership = 1,
    TTypeofMapVegetationType = 2,
    TTypeofMapSoilType = 3,
    TTypeofMapElementType = 4


}TTypeofMap;


typedef unsigned char uchar;

struct palette
{
    palette();
    std::vector<uchar> red;
    std::vector<uchar> green;
    std::vector<uchar> blue;
};

class HabitatMap
{
protected:
    unsigned MAPSIZE1;
    uchar* m_idata;
    float m_scalingW;
    float m_scalingH;
    char* m_map_name_vet[6] = {"Biomass", "FarmOwnership", "VegetationType", "SoilType", "ElementType"};

    palette colours;
    int m_colours[3][12] {
	    {
		    255, 000, 000, 128, 255, 000, 255, 255, 128, 128, 255, 064
	    },
	    {
		    000, 255, 000, 128, 255, 255, 000, 128, 255, 128, 255, 064
	    },
	    {
	    	000, 000, 255, 128, 000, 255, 255, 128, 128, 255, 255, 064
	    }
    };

public:
    HabitatMap(int SIZE);
    HabitatMap();
    ~HabitatMap();

    void DrawLandscape(TTypeofMap a_map_type);
    void SaveMapImage(const std::string& a_name, TTypeofMap a_map_type);
    void SaveAllMaps(std::string a_name);
    unsigned mapSize() {return MAPSIZE1;}
};

#endif // HABITATMAP_H
