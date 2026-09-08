//
// treatment.h
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

#ifndef TREATMENT_H
#define TREATMENT_H

typedef enum {
	start = 1,             //  1, must be first.
	sleep_all_day,
	autumn_plough,
	stubble_plough,
	stubble_cultivator_heavy,
	autumn_harrow,
	autumn_roll,
	preseeding_cultivator,
	preseeding_cultivator_sow,
	autumn_sow,			// 10
	winter_plough,
	deep_ploughing,
	spring_plough,
	spring_harrow,
	spring_roll,		// 15
	spring_sow,
	summer_plough,
	summer_harrow,
	summer_sow,
	fp_npks,
	fp_npk,
	fp_pk,
	fp_p,
	fp_k,
	fp_sk,
	fp_liquidNH3,			// 20
	fp_slurry,
	fp_manganesesulphate,
	fp_ammoniumsulphate,
	fp_manure,
	fp_greenmanure,        // 25
	fp_sludge,
	fp_rsm,
	fp_calcium,
	fa_npks,
	fa_npk,			// 30
	fa_pk,
	fa_p,
	fa_k,
	fa_sk,
	fa_slurry,
	fa_manganesesulphate,
	fa_ammoniumsulphate,
	fa_manure,			// 35
	fa_greenmanure,
	fa_sludge,
	fa_rsm,
	fa_calcium,
	herbicide_treat,		// 40
	growth_regulator,
	fungicide_treat,
	insecticide_treat,
	org_insecticide,
	org_herbicide,
	org_fungicide,
	molluscicide,
	row_cultivation,		// 45
	strigling,
	flammebehandling,
	hilling_up,
	water,
	swathing,				// 50
	harvest,
	cattle_out,
	pigs_out,
	cut_to_hay,
	cut_to_silage,		// 55
	straw_chopping,
	hay_turning,
	hay_bailing,
	stubble_harrowing,
	autumn_or_spring_plough,	//60
	burn_straw_stubble,
	mow,
	cut_weeds,
	strigling_sow,
	trial_insecticidetreat,		// 65
	trial_toxiccontrol,
	trial_control,
	syninsecticide_treat,
	cattle_out_low,
	product_treat,		// 70
	glyphosate,
	spring_sow_with_ferti,
	biocide,
	strigling_hill,
	bed_forming,		//75
	shallow_harrow,
	heavy_cultivator_aggregate,
	flower_cutting,
	bulb_harvest,
	straw_covering,		//80
	straw_removal,
	pruning,
	shredding,
	green_harvest,
	fiber_covering,   //85
	fiber_removal,
	suckering,
	fp_boron,
	fp_n,
	fp_nk,        //90
	fp_ns,
	fp_nc,
	autumn_sow_with_ferti,
	harvest_bushfruit,
	fp_cu,        //95
	fa_n,
	fa_cu,
	fa_boron,
	fa_nk,
	burn_top,
	winter_harrow,
	fa_pks,
	fp_pks,
	harvestshoots,
	manual_weeding,
	pheromone,
	last_treatment // Must be last, used for size computations
				   // of internal arrays etc.
} FarmToDo;

#endif // TREATMENT_H
