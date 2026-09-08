#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Skylark/SkylarkAll.h"

extern Landscape * g_land;

extern int HQualityBareEarth;
extern int PatchyPremium;
extern int HQualityHedge;
extern int HQualityTall2;
extern int HQualityHedgeScrub;
extern int HQualityTallVeg;
extern int HQualityTall;
extern int HQualityMetalRoad;
extern int HQualityWater;

extern CfgInt cfg_SkTramlinesPremium;
extern CfgFloat cfg_SkScrapesPremiumII;

extern double DensityScore[];
extern double HeightScore[]; 

bool not_nest_friendly(int x, int y) 
{

	/* NEW PROPOSED CODE */
	/* 
	*  "High" landscape elements are not friendly for nesting ( return thus TRUE ) 
	*  To double check if any exceptions.
	*/

	// return (g_land->SupplyLEHigh(x, y));

	/* OLD CODE */
	TTypesOfLandscapeElement ele = g_land->SupplyElementType(x, y);

	if (g_land->SupplyAttIsForest(x, y))
		return true;

	switch (ele) {
	case tole_Hedges:
	//case tole_HedgeBank: // ***CJT*** testing with and without this, h desigination to exe
	case tole_StoneWall: // 15
	case tole_Fence: //225
	case tole_Building: // 5
	case tole_Garden: // 10
	case tole_ActivePit: // 115
	case tole_PlantNursery:
	case tole_WindTurbine:
	case tole_Pylon:
	case tole_Pipeline:
	//Below added by Jordan 16/11/2023
	case tole_AmenityGrass:
	case tole_Canal:
	case tole_Carpark:
	case tole_FarmFeedingGround:
	case tole_FishFarm:
	case tole_Freshwater:
	case tole_LargeRoad:
	case tole_MetalledPath:
	case tole_Missing:
	case tole_Pond:
	case tole_Portarea:
	case tole_Railway:
	case tole_RefuseSite:
	case tole_River:
	case tole_Saltwater:
	case tole_SmallRoad:
	case tole_Stream:
	case tole_Track:
	case tole_UrbanNoVeg:
	case tole_UrbanPark:
	case tole_UrbanVeg:
	case tole_BuiltUpWithParkland:
	case tole_AlmondPlantation:
	case tole_EnergyCrop:
	case tole_OEnergyCrop:
	case tole_OBushFruit:
	case tole_FarmForest:
	case tole_OFarmForest:
	case tole_FarmYoungForest:
	case tole_OFarmYoungForest:
	case tole_ForestAisle:
	case tole_OliveGrove:
	case tole_BushFruit:
	case tole_Orchard:
	case tole_OOrchard:
	case tole_OrchardBand:
	case tole_YoungForest:
	case tole_WalnutPlantation:
	case tole_WoodlandMargin:
	case tole_WoodyEnergyCrop:

		return true;
	default:
		break;
	}

	return false;

}

double SkTerritories::PrePoly2Qual(int a_poly)
{
	/**
	* Used only on start-up when potential territories need to be determined.
	*/
	TTypesOfLandscapeElement l_type = TheLandscape->SupplyElementType(a_poly);
	double l_height;
	double l_score = -9999.0;
	//  bool ppremium = false;
	//  int l_index;

    //  Based on Attribute
	if (g_land->SupplyAttIsForest(a_poly))
	{
		l_score = HQualityTall2;
		
		// forest exception
		if ( l_type == tole_YoungForest ) 
			l_score = HQualityBareEarth + PatchyPremium;

		goto end_decision;
	}

	if (g_land->SupplyAttIsUrbanNoVeg(a_poly))
	{
		l_score = HQualityTallVeg;

		goto end_decision;
	}

	// Based on specific TOLE list 
	switch (l_type) {
	case tole_Hedges: // 130
	case tole_HedgeBank:
		l_score = HQualityHedge;
		break;
	case tole_PlantNursery:
	case tole_WindTurbine:
	case tole_Pylon:
	case tole_Pipeline:
	case tole_PitDisused: // 75
	case tole_ActivePit: // 115
	case tole_Building: // 5
	case tole_Orchard:
	case tole_OliveGrove:
	case tole_Vineyard:
	case tole_WalnutPlantation:
	case tole_AlmondPlantation:
	case tole_ChristmasTrees: //AHA
	case tole_OChristmasTrees: //AHA
	case tole_FarmYoungForest: //AHA
	case tole_OFarmYoungForest: //AHA
	case tole_BushFruit: //AHA
	case tole_OBushFruit: //AHA
	case tole_BuiltUpWithParkland:
	case tole_OOrchard:
	case tole_Airport:
	case tole_MushroomPerm:
	case tole_Portarea:
	case tole_RiverBed:
	case tole_UrbanNoVeg:

		l_score = HQualityTall2;
		break;
	case tole_Churchyard:
	case tole_Garden: //11
		break;
	case tole_Saltmarsh:
	case tole_NaturalGrassWet:
	case tole_Marsh: // 95
	case tole_Scrub: // 70
		l_score = HQualityHedgeScrub;
		break;
	case tole_RoadsideSlope:
	case tole_Field:
	case tole_EnergyCrop: //AHA
	case tole_OEnergyCrop: //AHA
	case tole_PermPasturePigs: //AHA
	case tole_RiceField:
	case tole_RoadsideVerge: // 13
	case tole_WaterBufferZone: //226
	case tole_FieldBoundary: // 160
	case tole_FarmBufferZone:
	case tole_FlowerStrip:
	case tole_UnsprayedFieldMargin:
	case tole_PermanentSetaside: // 7
	case tole_Heath:
	case tole_PermPastureLowYield:
	case tole_OPermPastureLowYield: //AHA
	case tole_PermPastureTussocky:
	case tole_PermPasture: // 35
	case tole_OPermPasture: // AHA
	case tole_FlowersPerm:
	case tole_NaturalGrassDry: // 110
	case tole_NaturalFarmGrass:
	case tole_BeetleBank:
	case tole_Wasteland:
	case tole_Vildtager:
	case tole_MownGrassStrip:
	case tole_GreenFallow: 

		// Must be potentially suitable vegetation element so score depends on
	// the vegetation height
		l_height = TheLandscape->SupplyVegHeight(a_poly);
		if (l_height >= 200.0)
		{
			l_score = HQualityTallVeg;
		}
		else
		{
			if (l_height >= 110.0)
			{
				l_score = HQualityTall;
			}
			else
			{
				l_score = SKOPTIMALHABITATSCORE;  // Start with a max
				if (!TheLandscape->SupplyVegPatchy(a_poly)) l_score *= 0.50;
			}
		}
		break;

	case tole_SmallRoad: // 120
	case tole_LargeRoad: // 121
		l_score = HQualityMetalRoad;
		break;

	case tole_MetalledPath:
	case tole_Carpark:
	case tole_FarmFeedingGround:
	case tole_Stream:
	case tole_HeritageSite:
	case tole_Pond:
	case tole_Freshwater: // 90
	case tole_FishFarm:
	case tole_AsparagusPerm:
	case tole_OAsparagusPerm:
	case tole_River: // 96
	case tole_Saltwater: // 80
	case tole_Coast: // 100
	case tole_RiversidePlants: // 98
	case tole_StoneWall: // 15
	case tole_Fence: //225
	case tole_Track: // 123
	case tole_ForestAisle:
	case tole_Railway: // 118
	case tole_AmenityGrass:
	case tole_Parkland:  //14
	case tole_UrbanPark:  //17
	case tole_UrbanVeg: // 9
	case tole_SolarPanel:
	case tole_SandDune:  //101
	case tole_UnknownGrass:
	case tole_DrainageDitch:
	case tole_Canal:
	case tole_RefuseSite:
	case tole_BareRock:
	case tole_Saltpans:
		l_score = HQualityWater; // Neutral
		break;
	case tole_Missing:
	case tole_Chameleon:
	case tole_Foobar: // 999 !! type unknown - should not happen
	default:
		static char l_errornum[20];
		sprintf(l_errornum, "%d", l_type);
		TheLandscape->Warn("SkTerritories::PrePoly2Qual(): ""Unknown tole_type", l_errornum);
		exit(1);
		break;
	
	}

	
end_decision: 

	if (TheLandscape->SupplyVegPatchy(a_poly)) l_score += PatchyPremium;
	if (l_score > SKOPTIMALHABITATSCORE) l_score = SKOPTIMALHABITATSCORE;
	return l_score;
}

double SkTerritories::PrePolyNQual(int a_poly, int* /*a_good_polys*/) {
	TTypesOfLandscapeElement l_type = TheLandscape->SupplyElementType(a_poly);
	double l_height;
	double l_score = -9999.0;
	bool ppremium = false;
	//  int l_index;

	 //  Based on Attribute
	if (g_land->SupplyAttIsForest(a_poly))
	{
		l_score = HQualityTall2;

		// forest exception
		if (l_type == tole_YoungForest)
			l_score = HQualityBareEarth + PatchyPremium;

		goto end_decision;
	}

	if (g_land->SupplyAttIsUrbanNoVeg(a_poly))
	{
		l_score = HQualityTallVeg;

		goto end_decision;
	}


	switch (l_type) {
	case tole_Hedges: // 130
	case tole_HedgeBank:
		l_score = HQualityHedge;
		break;
	case tole_PlantNursery:
	case tole_WindTurbine:
	case tole_WoodyEnergyCrop:
	case tole_WoodlandMargin:
	case tole_Pylon:
	case tole_PitDisused: // 75
	case tole_ActivePit: // 115
	case tole_Building: // 5
	case tole_Copse:
	case tole_Orchard:
	case tole_OOrchard:
	case tole_OliveGrove:
	case tole_AlmondPlantation:
	case tole_WalnutPlantation:
	case tole_Vineyard:
	case tole_ChristmasTrees: //AHA
	case tole_OChristmasTrees: //AHA
	case tole_BushFruit: //AHA
	case tole_OBushFruit: //AHA
	case tole_FarmYoungForest: //AHA
	case tole_OFarmYoungForest: //AHA
	case tole_BuiltUpWithParkland:
		l_score = HQualityTall2;
		break;
	case tole_Churchyard:
	case tole_Garden: //11
		l_score = HQualityTallVeg;
		break;
	case tole_Scrub: // 70
		l_score = HQualityHedgeScrub;
		break;
	case tole_RoadsideSlope:
	case tole_Field:
	case tole_EnergyCrop: //AHA
	case tole_OEnergyCrop: //AHA
	case tole_RiceField:
	case tole_RoadsideVerge: // 13
	case tole_WaterBufferZone:
	case tole_FlowerStrip:
	case tole_FlowersPerm:
	case tole_FieldBoundary: // 160
	case tole_FarmBufferZone:
	case tole_UnsprayedFieldMargin:
	case tole_PermanentSetaside: // 7
	case tole_Heath:
	case tole_PermPastureLowYield:
	case tole_OPermPastureLowYield: //AHA
	case tole_PermPastureTussocky:
	case tole_PermPasture: // 35
	case tole_OPermPasture: //AHA
	case tole_PermPasturePigs: //AHA
	case tole_OPermPasturePigs:
	case tole_OtherPermCrop:
	case tole_PermPastureTussockyWet:
	case tole_NaturalGrassDry: // 110
	case tole_NaturalFarmGrass:
	case tole_BeetleBank:
	case tole_Wasteland:
	case tole_Vildtager:
	case tole_MownGrassStrip:
	case tole_GreenFallow:
	case tole_NaturalGrassWet:
	case tole_Saltmarsh:
	case tole_Marsh: // 95
		// Must be potentially suitable vegetation element so score depends on
		// the vegetation height
		l_height = TheLandscape->SupplyVegHeight(a_poly);
		ppremium = TheLandscape->SupplyVegPatchy(a_poly);
		// If using skylark scrapes then we need the code below
		if (TheLandscape->SupplySkScrapes(a_poly) && (l_height >= 30))
		{
			if (l_height >= 200.0)
			{
				l_score = HQualityTallVeg;
			}
			else
			{
				if (l_height >= 110.0)
				{
					l_score = HQualityTall;
				}
				else if (l_height < 3.0)
				{
					l_score = HQualityBareEarth;
				}
				else
				{
					l_score = SKOPTIMALHABITATSCORE / 2.0; // Start with standard (less some to allow for doubling territory)
					double hh;
					if (l_height > 70) hh = 70; else hh = l_height;
					l_score += hh * cfg_SkScrapesPremiumII.value();
					int density = TheLandscape->SupplyVegDensity(a_poly);
					l_score += (DensityScore[density] + HeightScore[(int)l_height]);
				}
			}
		}
		else
		{
			// No skylark scrapes
			if (l_height >= 200.0)
			{
				l_score = HQualityTallVeg;
			}
			else
			{
				if (l_height >= 110.0)
				{
					l_score = HQualityTall;
					if (ppremium) l_score += PatchyPremium;
				}
				else if (l_height < 3.0)
				{
					l_score = HQualityBareEarth;
				}
				else
				{
					l_score = SKOPTIMALHABITATSCORE / 2.0; // Start with half standard - less some to allow doubling (taken up in the minimum territory accept score)
					int density;
					if (l_height <= 30) density = 0; else density = TheLandscape->SupplyVegDensity(a_poly);
					l_score += (DensityScore[density] + HeightScore[(int)l_height]);
					if (ppremium) l_score += PatchyPremium;
					else
					{
						if (TheLandscape->SupplyHasTramlines(a_poly)) l_score += cfg_SkTramlinesPremium.value();
					}
				}
			}
		}
		break;

	case tole_SmallRoad: // 120
	case tole_LargeRoad: // 121
		l_score = HQualityMetalRoad;
		break;

	case tole_MetalledPath:
	case tole_Carpark:
	case tole_FarmFeedingGround:
	case tole_Stream:
	case tole_HeritageSite:
	case tole_Pond:
	case tole_Freshwater: // 90
	case tole_FishFarm:
	case tole_River: // 96
	case tole_Saltwater: // 80
	case tole_Coast: // 100
	case tole_RiversidePlants: // 98
	case tole_StoneWall: // 15
	case tole_Fence:
	case tole_Track: // 123
	case tole_ForestAisle:
	case tole_Railway: // 118
	case tole_SolarPanel:
	case tole_AmenityGrass:
	case tole_Parkland:  //14
	case tole_UrbanPark:  //17
	case tole_UrbanVeg:	//9
	case tole_SandDune:  //101
	case tole_UnknownGrass:
	case tole_AsparagusPerm:
	case tole_OAsparagusPerm:
	case tole_DrainageDitch:
	case tole_Canal:
	case tole_RefuseSite:
	case tole_BareRock:
	case tole_UrbanNoVeg:
	case tole_Portarea:
	case tole_Pipeline:
	case tole_Airport:
	case tole_MushroomPerm:
	case tole_RiverBed:
	case tole_Saltpans:
		l_score = HQualityWater; // Neutral
		break;
	case tole_Chameleon:
	case tole_Missing:
	case tole_Foobar: // 999 !! type unknown - should not happen
	default:
		static char l_errornum[20];
		sprintf(l_errornum, "%d", l_type);
		TheLandscape->Warn("SkTerritories::PrePolyNQual(): ""Unknown tole_type", l_errornum);
		exit(1);
		break;

	}

end_decision:

	if (l_score > SKOPTIMALHABITATSCORE) l_score = SKOPTIMALHABITATSCORE;
	return l_score;
}
