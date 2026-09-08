//
// MapCFG.h
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

#ifndef MAP_CFG_H
#define MAP_CFG_H

extern CfgBool  g_farm_fixed_crop_enable;
extern CfgInt  g_farm_fixed_crop_type;

extern CfgBool  g_farm_fixed_rotation_enable;
extern CfgBool  g_farm_fixed_rotation_farms_async;
extern CfgInt   g_farm_fixed_rotation_farmtype;

//extern CfgBool  g_farm_test_crop;
extern CfgInt   g_farm_test_crop_type;
extern CfgStr   g_farm_test_crop_filename;
extern CfgInt   g_farm_test_crop_daystorun;
extern CfgBool  g_farm_test_crop_reportdaily;
extern CfgInt   g_farm_test_crop_farmtype;
extern CfgInt   g_farm_test_crop_sample_x;
extern CfgInt   g_farm_test_crop_sample_y;

extern CfgBool  g_farm_enable_crop_rotation;

extern CfgBool  g_map_le_borders;
extern CfgInt  g_map_le_borderstype;
extern CfgBool  g_map_le_borderremoval;
extern CfgBool  g_map_le_border_limited;
extern CfgBool  g_map_orchards_borders;
extern CfgInt   g_map_le_borders_min_field_size;
extern CfgInt   g_map_le_borderwidth;
extern CfgInt   g_map_le_minfieldwidth;
extern CfgInt   g_map_orchardsborderwidth;
extern CfgInt   g_map_le_border_chance;
// Unsprayed margins code...
extern CfgInt  g_map_le_unsprayedmargins_chance;
extern CfgBool g_map_le_unsprayedmargins;
extern CfgInt g_map_le_umargin_min_field_size;
// ...to here

extern CfgInt   g_map_maxpolyref;

// 
extern CfgInt  g_farm_PermPastureLowYieldManagement;
extern CfgInt  g_farm_YoungForestManagement;
extern CfgInt  g_farm_PermPastureManagement;
extern CfgInt  g_farm_PermPastureTussockyManagement;
extern CfgInt  g_farm_PermSetasideManagement;
extern CfgInt  g_farm_OrchardManagement;
extern CfgInt  g_farm_PlantNurseryManagement;

#endif
