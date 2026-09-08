//
// DK_SpringBarley.h
//
/*
*******************************************************************************************************
Copyright (c) 2021, Christopher John Topping, Aarhus University, updated by Luna Kondrup Marcussen, Aarhus University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided
that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the
following dissbaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
the following dissbaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INSBMUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISSBMAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INSBMUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INSBMUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************************************
*/

#ifndef DKSpringBarley_H
#define DKSpringBarley_H

#define DK_SBM_BASE 61300
#define DK_SB_AUTUMPLOUGH	a_field->m_user[1]
#define DK_SB_FORCESPRING	a_field->m_user[2]
#define DK_SB_TILL_C	a_field->m_user[3]
#define DK_SB_TILL_S	a_field->m_user[4]

typedef enum {
	dk_sb_start = 1, // Compulsory, start event must always be 1 (one).
	dk_sb_harvest = DK_SBM_BASE,
	dk_sb_autumn_plough,
	dk_sb_ferti_s1,
	dk_sb_ferti_p1,
	dk_sb_ferti_s2,
	dk_sb_ferti_p2,
	dk_sb_spring_plough,
	dk_sb_spring_harrow_nt,
	dk_sb_spring_sow,
	dk_sb_herbicide1,
	dk_sb_herbicide2,
	dk_sb_herbicide3,
	dk_sb_herbicide4,
	dk_sb_insecticide,
	dk_sb_water1,
	dk_sb_water2,
	dk_sb_fungicide1,
	dk_sb_fungicide2,
	dk_sb_gr1,
	dk_sb_gr2,
	dk_sb_straw_chopping,
	dk_sb_hay_bailing,
	dk_sb_stubble_harrow,
	dk_sb_plough,
	dk_sb_wait,
	dk_sb_foobar,
} DK_SpringBarleyToDo;



class DK_SpringBarley : public Crop
{
public:
	bool  Do(Farm* a_farm, LE* a_field, FarmEvent* a_ev);
	DK_SpringBarley(TTypesOfVegetation a_tov, TTypesOfCrops a_toc, Landscape* a_L) : Crop(a_tov, a_toc, a_L)
	{
		m_first_date = g_date->DayInYear(1, 12);
		SetUpFarmCategoryInformation();
	}
	void SetUpFarmCategoryInformation() {
		const int elements = 2 + (dk_sb_foobar - DK_SBM_BASE);
		m_base_elements_no = DK_SBM_BASE - 2;

		FarmManagementCategory catlist[elements] =
		{
			  fmc_Others,	// zero element unused but must be here	
			  fmc_Others,	//	  dk_sb_start = 1, // Compulsory, start event must always be 1 (one).
			  fmc_Harvest,	//	  dk_sb_harvest = DK_SBM_BASE,
			  fmc_Cultivation,	//	  dk_sb_autumn_plough,
			  fmc_Fertilizer,	//	  dk_sb_ferti_s1,
			  fmc_Fertilizer,	//	  dk_sb_ferti_p1,
			  fmc_Fertilizer,	//	  dk_sb_ferti_s2,
			  fmc_Fertilizer,	//	  dk_sb_ferti_p2,
			  fmc_Cultivation,	//	  dk_sb_spring_plough,
			  fmc_Cultivation,	//	  dk_sb_spring_harrow_nt,
			  fmc_Cultivation,	//	  dk_sb_spring_sow,
			  fmc_Herbicide,	//	  dk_sb_herbicide1,
			  fmc_Herbicide,	//	  dk_sb_herbicide2,
			  fmc_Herbicide,	//	  dk_sb_herbicide3,
			  fmc_Herbicide,	//	  dk_sb_herbicide4,
			  fmc_Insecticide,	//	  dk_sb_insecticide,
			  fmc_Watering,	//	  dk_sb_water1,
			  fmc_Watering,	//	  dk_sb_water2,
			  fmc_Fungicide,	//	  dk_sb_fungicide1,
			  fmc_Fungicide,	//	  dk_sb_fungicide2,
			  fmc_Others, //		dk_sb_gr1,
			  fmc_Others, //		dk_sb_gr2,
			  fmc_Others,	//	  dk_sb_straw_chopping,
			  fmc_Others,	//	  dk_sb_hay_bailing,
			  fmc_Cultivation, // dk_sb_stubble_harrow,
			  fmc_Cultivation, // dk_sb_plough,
			  fmc_Others, // dk_sb_wait

				 // no foobar entry			

		};
		// Iterate over the catlist elements and copy them to vector						
		copy(begin(catlist), end(catlist), back_inserter(m_ManagementCategories));
	}
};


#endif // DK_SpringBarley_H