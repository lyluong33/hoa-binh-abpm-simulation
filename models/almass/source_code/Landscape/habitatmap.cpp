

#include "habitatmap.h"
#include "ls.h"
#include <memory>
#include "colmap.h"
#include "../lib/toojpeg/toojpeg.h"

extern Landscape* g_ALandscape;

FILE* file_pointer;

palette::palette()
{
    for (int i=0; i < 400; ++i)
    {
      red.push_back(char(255));
      green.push_back(char(255));
      blue.push_back(char(255));
    }


    almasscmap::colourmap colours = almasscmap::makeColourMap();
    almasscmap::rgbarray colour;

    colour=colours["CORAL"];
    red[tole_Hedges]=colour[0];
    green[tole_Hedges]=colour[1];
    blue[tole_Hedges]=colour[2];

    colour=colours["GOLD"];
    red[tole_RoadsideVerge]=colour[0];
    green[tole_RoadsideVerge]=colour[1];
    blue[tole_RoadsideVerge]=colour[2];

    colour=colours["DARK ORCHID"];
    red[tole_Railway]=colour[0];
    green[tole_Railway]=colour[1];
    blue[tole_Railway]=colour[2];

    colour=colours["PALE GREEN"];
    red[tole_FieldBoundary]=colour[0];
    green[tole_FieldBoundary]=colour[1];
    blue[tole_FieldBoundary]=colour[2];

    colour = colours["DARKORCHID3"];
    red[tole_FlowerStrip] = colour[0];
    green[tole_FlowerStrip] = colour[1];
    blue[tole_FlowerStrip] = colour[2];

    colour=colours["SIENNA"];
    red[tole_Marsh]=colour[0];
    green[tole_Marsh]=colour[1];
    blue[tole_Marsh]=colour[2];

    colour=colours["THISTLE"];
    red[tole_Scrub]=colour[0];
    green[tole_Scrub]=colour[1];
    blue[tole_Scrub]=colour[2];

    colour=colours["WHEAT"];
    red[tole_Field]=colour[0];
    green[tole_Field]=colour[1];
    blue[tole_Field]=colour[2];

    colour=colours["MEDIUM SEA GREEN"];
    red[tole_PermPastureLowYield]=colour[0];
    green[tole_PermPastureLowYield]=colour[1];
    blue[tole_PermPastureLowYield]=colour[2];

    colour=colours["SPRING GREEN"];
    red[tole_PermPastureTussocky]=colour[0];
    green[tole_PermPastureTussocky]=colour[1];
    blue[tole_PermPastureTussocky]=colour[2];

    colour=colours["KHAKI"];
    red[tole_PermanentSetaside]=colour[0];
    green[tole_PermanentSetaside]=colour[1];
    blue[tole_PermanentSetaside]=colour[2];

    colour=colours["MEDIUM FOREST GREEN"];
    red[tole_PermPasture]=colour[0];
    green[tole_PermPasture]=colour[1];
    blue[tole_PermPasture]=colour[2];

    colour=colours["YELLOW"];
    red[tole_NaturalGrassDry]=colour[0];
    green[tole_NaturalGrassDry]=colour[1];
    blue[tole_NaturalGrassDry]=colour[2];

    colour=colours["YELLOW GREEN"];
    red[tole_RiversidePlants]=colour[0];
    green[tole_RiversidePlants]=colour[1];
    blue[tole_RiversidePlants]=colour[2];

    colour=colours["FIREBRICK"];
    red[tole_PitDisused]=colour[0];
    green[tole_PitDisused]=colour[1];
    blue[tole_PitDisused]=colour[2];

    colour=colours["SEA GREEN"];
    red[tole_RiversideTrees]=colour[0];
    green[tole_RiversideTrees]=colour[1];
    blue[tole_RiversideTrees]=colour[2];

    colour=colours["FOREST GREEN"];
    red[tole_DeciduousForest]=colour[0];
    green[tole_DeciduousForest]=colour[1];
    blue[tole_DeciduousForest]=colour[2];

    colour=colours["DARK OLIVE GREEN"];
    red[tole_ConiferousForest]=colour[0];
    green[tole_ConiferousForest]=colour[1];
    blue[tole_ConiferousForest]=colour[2];

    colour=colours["DARK GREEN"];
    red[tole_MixedForest]=colour[0];
    green[tole_MixedForest]=colour[1];
    blue[tole_MixedForest]=colour[2];

    colour=colours["GREEN YELLOW"];
    red[tole_YoungForest]=colour[0];
    green[tole_YoungForest]=colour[1];
    blue[tole_YoungForest]=colour[2];

    colour=colours["GREY"];
    red[tole_StoneWall]=colour[0];
    green[tole_StoneWall]=colour[1];
    blue[tole_StoneWall]=colour[2];

    colour=colours["MEDIUM ORCHID"];
    red[tole_Garden]=colour[0];
    green[tole_Garden]=colour[1];
    blue[tole_Garden]=colour[2];

    colour=colours["ORCHID"];
    red[tole_Track]=colour[0];
    green[tole_Track]=colour[1];
    blue[tole_Track]=colour[2];

    colour=colours["INDIAN RED"];
    red[tole_SmallRoad]=colour[0];
    green[tole_SmallRoad]=colour[1];
    blue[tole_SmallRoad]=colour[2];

    colour=colours["RED"];
    red[tole_LargeRoad]=colour[0];
    green[tole_LargeRoad]=colour[1];
    blue[tole_LargeRoad]=colour[2];

    colour=colours["BLACK"];
    red[tole_Building]=colour[0];
    green[tole_Building]=colour[1];
    blue[tole_Building]=colour[2];

    colour=colours["BROWN"];
    red[tole_Saltmarsh]=colour[0];
    green[tole_Saltmarsh]=colour[1];
    blue[tole_Saltmarsh]=colour[2];

    colour=colours["DARK GREY"];
    red[tole_ActivePit]=colour[0];
    green[tole_ActivePit]=colour[1];
    blue[tole_ActivePit]=colour[2];

    colour=colours["BLUE"];
    red[tole_Freshwater]=colour[0];
    green[tole_Freshwater]=colour[1];
    blue[tole_Freshwater]=colour[2];

    colour=colours["CORNFLOWER BLUE"];
    red[tole_River]=colour[0];
    green[tole_River]=colour[1];
    blue[tole_River]=colour[2];

    colour=colours["GOLDENROD"];
    red[tole_Coast]=colour[0];
    green[tole_Coast]=colour[1];
    blue[tole_Coast]=colour[2];

    colour=colours["AQUAMARINE"];
    red[tole_Saltwater]=colour[0];
    green[tole_Saltwater]=colour[1];
    blue[tole_Saltwater]=colour[2];

    colour=colours["GREEN"];
    red[tole_HedgeBank]=colour[0];
    green[tole_HedgeBank]=colour[1];
    blue[tole_HedgeBank]=colour[2];

    colour=colours["BLUE VIOLET"];
    red[tole_Heath]=colour[0];
    green[tole_Heath]=colour[1];
    blue[tole_Heath]=colour[2];

    colour=colours["PURPLE"];
    red[tole_Orchard]=colour[0];
    green[tole_Orchard]=colour[1];
    blue[tole_Orchard]=colour[2];

    colour=colours["DARK SLATE BLUE"];
    red[tole_OrchardBand]=colour[0];
    green[tole_OrchardBand]=colour[1];
    blue[tole_OrchardBand]=colour[2];

    colour=colours["MAROON"];
    red[tole_MownGrassStrip]=colour[0];
    green[tole_MownGrassStrip]=colour[1];
    blue[tole_MownGrassStrip]=colour[2];

    colour=colours["MEDIUM SPRING GREEN"];
    red[tole_NaturalGrassWet]=colour[0];
    green[tole_NaturalGrassWet]=colour[1];
    blue[tole_NaturalGrassWet]=colour[2];

    colour=colours["LIGHT GREY"];
    red[tole_MetalledPath]=colour[0];
    green[tole_MetalledPath]=colour[1];
    blue[tole_MetalledPath]=colour[2];

    colour=colours["DARK SLATE GRAY"];
    red[tole_RoadsideSlope]=colour[0];
    green[tole_RoadsideSlope]=colour[1];
    blue[tole_RoadsideSlope]=colour[2];

    colour=colours["DIM GREY"];
    red[tole_HeritageSite]=colour[0];
    green[tole_HeritageSite]=colour[1];
    blue[tole_HeritageSite]=colour[2];

    colour=colours["CADET BLUE"];
    red[tole_Stream]=colour[0];
    green[tole_Stream]=colour[1];
    blue[tole_Stream]=colour[2];

    colour=colours["DARK TURQUOISE"];
    red[tole_Carpark]=colour[0];
    green[tole_Carpark]=colour[1];
    blue[tole_Carpark]=colour[2];

    colour=colours["MEDIUM AQUAMARINE"];
    red[tole_Churchyard]=colour[0];
    green[tole_Churchyard]=colour[1];
    blue[tole_Churchyard]=colour[2];

    colour=colours["ORANGE RED"];
    red[tole_Wasteland]=colour[0];
    green[tole_Wasteland]=colour[1];
    blue[tole_Wasteland]=colour[2];

    colour=colours["MEDIUM SEA GREEN"];
    red[tole_IndividualTree]=colour[0];
    green[tole_IndividualTree]=colour[1];
    blue[tole_IndividualTree]=colour[2];

    colour=colours["LIGHT BLUE"];
    red[tole_WindTurbine]=colour[0];
    green[tole_WindTurbine]=colour[1];
    blue[tole_WindTurbine]=colour[2];

    colour=colours["LIGHT STEEL BLUE"];
    red[tole_Vildtager]=colour[0];
    green[tole_Vildtager]=colour[1];
    blue[tole_Vildtager]=colour[2];

    colour=colours["MAGENTA"];
    red[tole_PlantNursery]=colour[0];
    green[tole_PlantNursery]=colour[1];
    blue[tole_PlantNursery]=colour[2];

    colour=colours["PINK"];
    red[tole_WoodyEnergyCrop]=colour[0];
    green[tole_WoodyEnergyCrop]=colour[1];
    blue[tole_WoodyEnergyCrop]=colour[2];

    colour=colours["PLUM"];
    red[tole_WoodlandMargin]=colour[0];
    green[tole_WoodlandMargin]=colour[1];
    blue[tole_WoodlandMargin]=colour[2];

    colour=colours["MAROON"];
    red[tole_Pylon]=colour[0];
    green[tole_Pylon]=colour[1];
    blue[tole_Pylon]=colour[2];

    colour=colours["MEDIUM BLUE"];
    red[tole_Pond]=colour[0];
    green[tole_Pond]=colour[1];
    blue[tole_Pond]=colour[2];

    colour=colours["STEEL BLUE"];
    red[tole_FishFarm]=colour[0];
    green[tole_FishFarm]=colour[1];
    blue[tole_FishFarm]=colour[2];

    colour = colours["DARKORCHID3"];
    red[tole_Vineyard] = colour[0];
    green[tole_Vineyard] = colour[1];
    blue[tole_Vineyard] = colour[2];

    colour = colours["DARKOLIVEGREEN3"];
    red[tole_OliveGrove] = colour[0];
    green[tole_OliveGrove] = colour[1];
    blue[tole_OliveGrove] = colour[2];

    colour = colours["YELLOWGREEN"];
    red[tole_RiceField] = colour[0];
    green[tole_RiceField] = colour[1];
    blue[tole_RiceField] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_MontadoCorkOak] = colour[0];
    green[tole_MontadoCorkOak] = colour[1];
    blue[tole_MontadoCorkOak] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_MontadoHolmOak] = colour[0];
    green[tole_MontadoHolmOak] = colour[1];
    blue[tole_MontadoHolmOak] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_MontadoMixed] = colour[0];
    green[tole_MontadoMixed] = colour[1];
    blue[tole_MontadoMixed] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_AgroForestrySystem] = colour[0];
    green[tole_AgroForestrySystem] = colour[1];
    blue[tole_AgroForestrySystem] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_CorkOakForest] = colour[0];
    green[tole_CorkOakForest] = colour[1];
    blue[tole_CorkOakForest] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_HolmOakForest] = colour[0];
    green[tole_HolmOakForest] = colour[1];
    blue[tole_HolmOakForest] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_OtherOakForest] = colour[0];
    green[tole_OtherOakForest] = colour[1];
    blue[tole_OtherOakForest] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_ChestnutForest] = colour[0];
    green[tole_ChestnutForest] = colour[1];
    blue[tole_ChestnutForest] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_EucalyptusForest] = colour[0];
    green[tole_EucalyptusForest] = colour[1];
    blue[tole_EucalyptusForest] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_MaritimePineForest] = colour[0];
    green[tole_MaritimePineForest] = colour[1];
    blue[tole_MaritimePineForest] = colour[2];

    colour = colours["SPRINGGREEN4"];
    red[tole_StonePineForest] = colour[0];
    green[tole_StonePineForest] = colour[1];
    blue[tole_StonePineForest] = colour[2];

    colour = colours["PALEGREEN4"];
    red[tole_InvasiveForest] = colour[0];
    green[tole_InvasiveForest] = colour[1];
    blue[tole_InvasiveForest] = colour[2];

    colour = colours["WHEAT2"];
    red[tole_Portarea] = colour[0];
    green[tole_Portarea] = colour[1];
    blue[tole_Portarea] = colour[2];

    colour = colours["SNOW3"];
    red[tole_Airport] = colour[0];
    green[tole_Airport] = colour[1];
    blue[tole_Airport] = colour[2];

    colour = colours["WHITESMOKE"];
    red[tole_Saltpans] = colour[0];
    green[tole_Saltpans] = colour[1];
    blue[tole_Saltpans] = colour[2];

    colour = colours["SEAGREEN4"];
    red[tole_SwampForest] = colour[0];
    green[tole_SwampForest] = colour[1];
    blue[tole_SwampForest] = colour[2];

    colour = colours["SLATEGRAY"];
    red[tole_Pipeline] = colour[0];
    green[tole_Pipeline] = colour[1];
    blue[tole_Pipeline] = colour[2];

    colour = colours["DARKSLATEGRAY4"];
    red[tole_SolarPanel] = colour[0];
    green[tole_SolarPanel] = colour[1];
    blue[tole_SolarPanel] = colour[2];

    colour = colours["PALEGREEN"];
    red[tole_ForestAisle] = colour[0];
    green[tole_ForestAisle] = colour[1];
    blue[tole_ForestAisle] = colour[2];
    
    //
    
    colour = colours["PURPLE"];
    red[tole_OOrchard] = colour[0];
    green[tole_OOrchard] = colour[1];
    blue[tole_OOrchard] = colour[2];

    colour = colours["MEDIUMORCHID"];
    red[tole_BushFruit] = colour[0];
    green[tole_BushFruit] = colour[1];
    blue[tole_BushFruit] = colour[2];

    colour = colours["MEDIUMORCHID"];
    red[tole_OBushFruit] = colour[0];
    green[tole_OBushFruit] = colour[1];
    blue[tole_OBushFruit] = colour[2];

    colour = colours["FORESTGREEN"];
    red[tole_ChristmasTrees] = colour[0];
    green[tole_ChristmasTrees] = colour[1];
    blue[tole_ChristmasTrees] = colour[2];

    colour = colours["FORESTGREEN"];
    red[tole_OChristmasTrees] = colour[0];
    green[tole_OChristmasTrees] = colour[1];
    blue[tole_OChristmasTrees] = colour[2];

    colour = colours["TURQUOISE1"];
    red[tole_EnergyCrop] = colour[0];
    green[tole_EnergyCrop] = colour[1];
    blue[tole_EnergyCrop] = colour[2];

    colour = colours["TURQUOISE1"];
    red[tole_OEnergyCrop] = colour[0];
    green[tole_OEnergyCrop] = colour[1];
    blue[tole_OEnergyCrop] = colour[2];

    colour = colours["GREEN2"];
    red[tole_FarmForest] = colour[0];
    green[tole_FarmForest] = colour[1];
    blue[tole_FarmForest] = colour[2];

    colour = colours["GREEN2"];
    red[tole_OFarmForest] = colour[0];
    green[tole_OFarmForest] = colour[1];
    blue[tole_OFarmForest] = colour[2];

    colour = colours["MEDIUM FOREST GREEN"];
    red[tole_PermPasturePigs] = colour[0];
    green[tole_PermPasturePigs] = colour[1];
    blue[tole_PermPasturePigs] = colour[2];

    colour = colours["MEDIUM FOREST GREEN"];
    red[tole_OPermPasturePigs] = colour[0];
    green[tole_OPermPasturePigs] = colour[1];
    blue[tole_OPermPasturePigs] = colour[2];

    colour = colours["MEDIUM FOREST GREEN"];
    red[tole_OPermPasture] = colour[0];
    green[tole_OPermPasture] = colour[1];
    blue[tole_OPermPasture] = colour[2];

    colour = colours["MEDIUM FOREST GREEN"];
    red[tole_OPermPastureLowYield] = colour[0];
    green[tole_OPermPastureLowYield] = colour[1];
    blue[tole_OPermPastureLowYield] = colour[2];

    colour = colours["MEDIUM FOREST GREEN"];
    red[tole_OPermPastureLowYield] = colour[0];
    green[tole_OPermPastureLowYield] = colour[1];
    blue[tole_OPermPastureLowYield] = colour[2];

    colour = colours["GREEN1"];
    red[tole_FarmYoungForest] = colour[0];
    green[tole_FarmYoungForest] = colour[1];
    blue[tole_FarmYoungForest] = colour[2];

    colour = colours["GREEN1"];
    red[tole_OFarmYoungForest] = colour[0];
    green[tole_OFarmYoungForest] = colour[1];
    blue[tole_OFarmYoungForest] = colour[2];

	colour = colours["PURPLE"];
	red[tole_AlmondPlantation] = colour[0];
	green[tole_AlmondPlantation] = colour[1];
	blue[tole_AlmondPlantation] = colour[2];

	colour = colours["PURPLE"];
	red[tole_WalnutPlantation] = colour[0];
	green[tole_WalnutPlantation] = colour[1];
	blue[tole_WalnutPlantation] = colour[2];

	colour = colours["MEDIUM FOREST GREEN"];
	red[tole_FarmBufferZone] = colour[0];
	green[tole_FarmBufferZone] = colour[1];
	blue[tole_FarmBufferZone] = colour[2];

	colour = colours["MEDIUM FOREST GREEN"];
	red[tole_NaturalFarmGrass] = colour[0];
	green[tole_NaturalFarmGrass] = colour[1];
	blue[tole_NaturalFarmGrass] = colour[2];

	colour = colours["MEDIUM FOREST GREEN"];
	red[tole_GreenFallow] = colour[0];
	green[tole_GreenFallow] = colour[1];
	blue[tole_GreenFallow] = colour[2];

    colour = colours["MEDIUM FOREST GREEN"];
    red[tole_FarmFeedingGround] = colour[0];
    green[tole_FarmFeedingGround] = colour[1];
    blue[tole_FarmFeedingGround] = colour[2];

    colour = colours["MEDIUM GREEN GREY"];
    red[tole_UrbanVeg] = colour[0];
    green[tole_UrbanVeg] = colour[1];
    blue[tole_UrbanVeg] = colour[2];

}

HabitatMap::HabitatMap(int a_SIZE)
{
    m_idata = new uchar[a_SIZE * a_SIZE * 3];
    MAPSIZE1 = a_SIZE;
    m_scalingW = g_ALandscape->SupplySimAreaWidth() / ( float )MAPSIZE1;
    m_scalingH = g_ALandscape->SupplySimAreaHeight() / ( float )MAPSIZE1;
}

HabitatMap::HabitatMap()
{
    int SIZE=948;
    m_idata = new uchar[SIZE * SIZE * 3];
    MAPSIZE1=SIZE;
    m_scalingW = g_ALandscape->SupplySimAreaWidth() / ( float )MAPSIZE1;
    m_scalingH = g_ALandscape->SupplySimAreaHeight() / ( float )MAPSIZE1;
}

HabitatMap::~HabitatMap()
{
	delete[] m_idata; 
}

void HabitatMap::DrawLandscape(TTypeofMap a_map_type)
{
    float x, y;
    // show pesticide load ?
    
    int yy;
    int k = 0;
    y = 0;

    if (a_map_type == TTypeofMap::TTypeofMapBiomass) { // Show Biomass
        int col;
        for (unsigned j = 0; j < MAPSIZE1; j++) {
            yy = int(y);
            x = 0;
            for (unsigned i = 0; i < MAPSIZE1; i++) {
                col = (int)g_ALandscape->SupplyVegBiomass(int(x), yy);
                if (col > 255) col = 255;
                m_idata[k++] = 64;
                m_idata[k++] = col;
                m_idata[k++] = 64;
                x += m_scalingW;
            }
            y += m_scalingH;
        }
    }
    else if (a_map_type == TTypeofMap::TTypeofMapFarmOwnership) { // Show farm ownership
        int owner;
        int red, green, blue;
        for (unsigned j = 0; j < MAPSIZE1; j++) {
            yy = int(y);
            x = 0;
            for (unsigned i = 0; i < MAPSIZE1; i++) {
                owner = g_ALandscape->SupplyFarmOwnerIndex(int(x), yy);
                if (owner != -1) {
                    red = ((owner * 17) % 255);
                    green = (red + (owner * 19) % 255);
                    blue = (green + (owner * 29) % 255);
                    m_idata[k++] = red;
                    m_idata[k++] = green;
                    m_idata[k++] = blue;
                }
                else {
                    m_idata[k++] = 0;
                    m_idata[k++] = 0;
                    m_idata[k++] = 0;
                }
                x += m_scalingW;
            }
            y += m_scalingH;
        }
    }
    else if (a_map_type == TTypeofMap::TTypeofMapVegetationType) { // tov show
        int red, green, blue;
        unsigned ref;
        for (unsigned j = 0; j < MAPSIZE1; j++) {
            yy = int(y);
            x = 0;
            for (unsigned i = 0; i < MAPSIZE1; i++) {
                ref = g_ALandscape->SupplyVegType(int(x), yy);
                red = ((ref * 17) % 255);
                green = (red + (ref * 19) % 255);
                blue = (green + (ref * 29) % 255);
                m_idata[k++] = red;
                m_idata[k++] = green;
                m_idata[k++] = blue;
                x += m_scalingW;
            }
            y += m_scalingH;
        }
    }
   
    else if (a_map_type == TTypeofMap::TTypeofMapSoilType)  // Soils
    {
        double soil;
        for (unsigned j = 0; j < MAPSIZE1; j++)
        {
            yy = (int)y;
            x = 0;
            for (unsigned i = 0; i < MAPSIZE1; i++)
            {
                soil = g_ALandscape->SupplySoilTypeR(int(x), yy) * 16;
                if (soil > 255) soil = 255;
                m_idata[k++] = 64;
                m_idata[k++] = soil;
                m_idata[k++] = soil;
                x += m_scalingW;
            }
            y += m_scalingH;
        }
    }
    else if (a_map_type == TTypeofMapElementType) {    // default
        unsigned ref;
        int col;
        for (unsigned j = 0; j < MAPSIZE1; j++) {
            yy = (int)y;
            x = 0;
            for (unsigned i = 0; i < MAPSIZE1; i++) {
                // Note that because of the way the data is stored, we have to reverse
                // the x,y co-ords
                ref = g_ALandscape->SupplyElementType(int(x), yy);
                if (ref == tole_Building)  // 24 is tole_Building
                {
                    if (g_ALandscape->SupplyCountryDesig(int(x), yy) == 1) // Country residence
                    {
                        m_idata[k++] = 255;
                        m_idata[k++] = 0;
                        m_idata[k++] = 0;
                    }
                    else
                    {
                        m_idata[k++] = 0;
                        m_idata[k++] = 0;
                        m_idata[k++] = 0;
                    }
                }
                else
                {
                    if (ref > 400) ref = 399; // The max number of colours
                    m_idata[k++] = colours.red[ref];
                    m_idata[k++] = colours.green[ref];
                    m_idata[k++] = colours.blue[ref];
                }
                x += m_scalingW;
            }
            y += m_scalingH;
        }
    }
}

void writeByte(unsigned char oneByte){
    fputc(oneByte, file_pointer);
}

void HabitatMap::SaveMapImage(const std::string& a_name, TTypeofMap a_map_type) {
    //first draw the landscape
    file_pointer = std::fopen(("./Images/"+a_name).c_str(), "w+");
    DrawLandscape(a_map_type);
    TooJpeg::writeJpeg(writeByte, m_idata, MAPSIZE1, MAPSIZE1, true);
    fclose(file_pointer);
}

void HabitatMap::SaveAllMaps(std::string a_name) {
    for(int i=0; i<5; i++) {
        std::string name = m_map_name_vet[i];
        name += "_";
        name += a_name;
        name += ".jpg";
        SaveMapImage(name, (TTypeofMap)i);
    }
}