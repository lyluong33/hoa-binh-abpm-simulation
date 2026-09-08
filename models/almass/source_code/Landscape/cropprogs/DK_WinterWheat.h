//
// DK_WinterWheat.h
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

#ifndef DKWinterWheat_H
#define DKWinterWheat_H

#define DK_WW_BASE 61100

#define DK_WW_MN_S m_field->m_user[0]
#define DK_WW_MN_P m_field->m_user[1]
#define DK_WW_TILL_1 m_field->m_user[2]
#define DK_WW_TILL_2 m_field->m_user[3]

typedef enum {
	dk_ww_start = 1, // Compulsory, start event must always be 1 (one).
	dk_ww_harvest = DK_WW_BASE,
	dk_ww_autumn_plough,
	dk_ww_autumn_harrow,
	dk_ww_autumn_harrow_nt,
	dk_ww_autumn_roll,
	dk_ww_molluscicide,
	dk_ww_ferti_s1,
	dk_ww_ferti_p1,
	dk_ww_ferti_s2,
	dk_ww_ferti_p2,
	dk_ww_autumn_sow,
	dk_ww_herbicide1,
	dk_ww_ferti_s3,
	dk_ww_ferti_p3,
	dk_ww_ferti_s4,
	dk_ww_ferti_p4,
	dk_ww_ferti_s5,
	dk_ww_ferti_p5,
	dk_ww_ferti_s6,
	dk_ww_ferti_p6,
	dk_ww_ferti_s7,
	dk_ww_ferti_p7,
	dk_ww_herbicide2,
	dk_ww_herbicide3,
	dk_ww_herbicide4,
	dk_ww_herbicide5,
	dk_ww_herbicide6,
	dk_ww_herbicide7,
	dk_ww_fungicide1,
	dk_ww_fungicide2,
	dk_ww_insecticide1,
	dk_ww_insecticide2,
	dk_ww_gr,
	dk_ww_wait,
	dk_ww_foobar,
} DK_WinterWheatToDo;



class DK_WinterWheat : public Crop
{
public:
	bool  Do(Farm* a_farm, LE* a_field, FarmEvent* a_ev);
	DK_WinterWheat(TTypesOfVegetation a_tov, TTypesOfCrops a_toc, Landscape* a_L) : Crop(a_tov, a_toc, a_L)
	{
		m_first_date = g_date->DayInYear(17, 9);
		SetUpFarmCategoryInformation();
	}
	void SetUpFarmCategoryInformation() {
		const int elements = 2 + (dk_ww_foobar - DK_WW_BASE);
		m_base_elements_no = DK_WW_BASE - 2;

		FarmManagementCategory catlist[elements] =
		{
			  fmc_Others,	// zero element unused but must be here	
			  fmc_Others,	//	  dk_ww_start = 1, // Compulsory, start event must always be 1 (one).
			  fmc_Harvest,	//	  dk_ww_harvest = DK_ww_BASE,
			  fmc_Cultivation,	//	  dk_ww_autumn_plough,
			  fmc_Cultivation,	//	  dk_ww_autumn_harrow,
			  fmc_Cultivation,	//	  dk_ww_autumn_harrow_nt,
			  fmc_Others,	//	  dk_ww_autumn_roll,
			  fmc_Others, // dk_ww_molluscicide,
			  fmc_Fertilizer,	//	  dk_ww_ferti_s1,
			  fmc_Fertilizer,	//	  dk_ww_ferti_p1,
			  fmc_Fertilizer,	//	  dk_ww_ferti_s2,
			  fmc_Fertilizer,	//	  dk_ww_ferti_p2,
			  fmc_Cultivation,	//	  dk_ww_autumn_sow,
			  fmc_Herbicide,	//	  dk_ww_herbicide1,
			  fmc_Fertilizer,	//	  dk_ww_ferti_s3,
			  fmc_Fertilizer,	//	  dk_ww_ferti_p3,
			  fmc_Fertilizer,	//	  dk_ww_ferti_s4,
			  fmc_Fertilizer,	//	  dk_ww_ferti_p4,
			  fmc_Fertilizer,	//	  dk_ww_ferti_s5,
			  fmc_Fertilizer,	//	  dk_ww_ferti_p5,
			  fmc_Fertilizer,	//	  dk_ww_ferti_s6,
			  fmc_Fertilizer,	//	  dk_ww_ferti_p6,
			  fmc_Fertilizer,	//	  dk_ww_ferti_s7,
			  fmc_Fertilizer,	//	  dk_ww_ferti_p7,
			  fmc_Herbicide,	//	  dk_ww_herbicide2,
			  fmc_Herbicide,	//	  dk_ww_herbicide3,
			  fmc_Herbicide,	//	  dk_ww_herbicide4,
			  fmc_Herbicide,	//	  dk_ww_herbicide5,
			  fmc_Herbicide,	//	  dk_ww_herbicide6,
			  fmc_Herbicide,	//	  dk_ww_herbicide7,
			  fmc_Fungicide,	//	  dk_ww_fungicide1,
			  fmc_Fungicide,	//	  dk_ww_fungicide2,
			  fmc_Insecticide,	//	  dk_ww_insecticide1,
			  fmc_Insecticide,	//	  dk_ww_insecticide2,
			  fmc_Others, //		dk_ww_gr,
			  fmc_Others, //		dk_ww_wait,

				 // no foobar entry			

		};
		// Iterate over the catlist elements and copy them to vector						
		copy(begin(catlist), end(catlist), back_inserter(m_ManagementCategories));
	}
};


#endif // DK_WinterWheat_H