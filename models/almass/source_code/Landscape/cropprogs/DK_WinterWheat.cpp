//
// DK_WinterWheat.cpp
//
/*
*******************************************************************************************************
Copyright (c) 2021, Christopher John Topping, Aarhus University, updated by Luna Kondrup Marcussen, Aarhus University
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

#include "../../Landscape/ls.h"
#include "../../Landscape/cropprogs/DK_WinterWheat.h"
#include "math.h"


extern CfgFloat cfg_ins_app_prop1;
extern CfgFloat cfg_herbi_app_prop;
extern CfgFloat cfg_fungi_app_prop1;
extern CfgFloat cfg_greg_app_prop;
extern CfgFloat cfg_DKCatchCropPct;

bool DK_WinterWheat::Do(Farm* a_farm, LE* a_field, FarmEvent* a_ev)
{
    m_farm = a_farm;
    m_field = a_field;
    m_ev = a_ev;
    bool done = false; // The boolean value done indicates when we are totally finished with this plan (i.e. it is set to true, m_farm, m_field).
    bool flag = false;
    int d1 = 0;
    int noDates = 1;
    TTypesOfVegetation l_tov = tov_DKWinterWheat;
    int l_nextcropstartdate;

    switch (m_ev->m_todo)
    {
    case dk_ww_start:
    {
        a_field->ClearManagementActionSum();

        DK_WW_MN_S = false;
        DK_WW_MN_P = false;

        m_last_date = g_date->DayInYear(31, 8); // Should match the last flexdate below
            //Create a 2d array of 1 plus the number of operations you use. Change only 4+1 to what you need in the line below
        std::vector<std::vector<int>> flexdates(1 + 1, std::vector<int>(2, 0));
        // Set up the date management stuff
                // Start and stop dates for all events after harvest
        flexdates[0][1] = g_date->DayInYear(31, 8); // last possible day of harvest
        // Now these are done in pairs, start & end for each operation. If its not used then -1
        flexdates[1][0] = -1; // This date will be moved back as far as necessary and potentially to flexdates 1 (start op 1)
        flexdates[1][1] = g_date->DayInYear(31, 8); // This date will be moved back as far as necessary and potentially to flexdates 1 (end op 1) // harvest

        // Below if this is a spring crop use 365, otherwise first parameter is always 0, second parameter is fixed, and the third is the start up operation in the first year
        if (StartUpCrop(0, flexdates, int(dk_ww_wait))) break;

        // End single block date checking code. Please see next line comment as well.
        // Reinit d1 to first possible starting date.
        d1 = g_date->OldDays() + g_date->DayInYear(1, 8);
        // OK, let's go.
        SimpleEvent(d1, dk_ww_autumn_harrow, false);
    }
    break;
   
    case dk_ww_autumn_harrow:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.50)) {
            if (!m_farm->AutumnHarrow(m_field, 0.0,
                g_date->DayInYear(16, 9) - g_date->DayInYear())) { // changed from 31/8 to fit rotation with CloverGrassGrazed2 (as preceding crop)
                SimpleEvent(g_date->Date() + 1, dk_ww_autumn_harrow, true);
                break;
            }
            DK_WW_TILL_1 = true; // we need to remember farmers doing stubble harrow
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 9), dk_ww_autumn_plough, false);
            break;
        } 
        else if (m_ev->m_lock || m_farm->DoIt_prob(0.25 / 0.40)) {
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 9), dk_ww_autumn_plough, false);
            break;
        }// 15% no cultivation
        else SimpleEvent(g_date->Date() + 1, dk_ww_autumn_harrow_nt, false);
        break; // no till 15% 
     
       
    case dk_ww_autumn_plough: // 85% doing this
        if (!m_farm->AutumnPlough(m_field, 0.0,
            g_date->DayInYear(17, 9) - g_date->DayInYear())) {// changed from 15/9 to fit rotation
            SimpleEvent(g_date->Date() + 1, dk_ww_autumn_plough, true);
            break;
        }
        SimpleEvent(g_date->Date(), dk_ww_autumn_roll, false);
        break;

    case dk_ww_autumn_roll:
        if (!m_farm->AutumnRoll(m_field, 0.0,
            g_date->DayInYear(17, 9) - g_date->DayInYear())) {// changed from 15/9 to fit rotation
            SimpleEvent(g_date->Date() + 1, dk_ww_autumn_roll, true);
            break;
        }
        SimpleEvent(g_date->Date() + 1, dk_ww_molluscicide, false);
        break;

    case dk_ww_autumn_harrow_nt:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.80)) { // 80% of the 15% not doing the first harrow or plough
            if (!m_farm->AutumnHarrow(m_field, 0.0,
                g_date->DayInYear(17, 9) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_autumn_harrow_nt, true);
                break;
            }
        }
            SimpleEvent(g_date->Date() + 1, dk_ww_molluscicide, false);
            break;
        
    case dk_ww_molluscicide:
        if (m_field->GetSoilType() != 2 && m_field->GetSoilType() != 6) // on clay soils (NL KLEI & VEEN)
        {
            if (m_ev->m_lock || m_farm->DoIt_prob(0.10)) {
                if (!m_farm->Molluscicide(m_field, 0.0,
                    g_date->DayInYear(18, 9) - g_date->DayInYear())) { // changed from 16/9 to fit rotation
                    SimpleEvent(g_date->Date() + 1, dk_ww_molluscicide, true);
                    break;
                }
            }
        }
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(7, 9), dk_ww_autumn_sow, false);
        break;

    case dk_ww_autumn_sow:
        if (!m_farm->PreseedingCultivatorSow(m_field, 0.0,
            g_date->DayInYear(25, 9) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_autumn_sow, true);
            break;
        }
        if (a_farm->IsStockFarmer()) //Stock Farmer
        {
            SimpleEvent(g_date->Date(), dk_ww_ferti_s1, false);
            break;
        }
        else SimpleEvent(g_date->Date(), dk_ww_ferti_p1, false);
        break;

    case dk_ww_ferti_s1: // preventing Mn depletion
        if (m_ev->m_lock || m_farm->DoIt_prob(0.03)) {
            if (!m_farm->FA_AmmoniumSulphate(m_field, 0.0,
                g_date->DayInYear(25, 9) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 7, dk_ww_ferti_s2, false);
        break;

    case dk_ww_ferti_p1:// preventing Mn depletion
        if (m_ev->m_lock || m_farm->DoIt_prob(0.03)) {
            if (!m_farm->FP_AmmoniumSulphate(m_field, 0.0,
                g_date->DayInYear(25, 9) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 7, dk_ww_ferti_p2, false);
        break;

    case dk_ww_ferti_s2:
        if (m_field->GetSoilType() == tos_Sand || m_field->GetSoilType() == tos_LoamySand || m_field->GetSoilType() == tos_SandyLoam || m_field->GetSoilType() == tos_SandyClayLoam) // on sandy soils (NL ZAND & LOSS)
        {
            if (!m_farm->FA_ManganeseSulphate(m_field, 0.0,
                g_date->DayInYear(3, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s2, true);
                break;
            }
        }
        else if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->FA_ManganeseSulphate(m_field, 0.0,
                g_date->DayInYear(3, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s2, true);
                break;
            }
            DK_WW_MN_S = true; // we need to remember who sprays Mn
        }
        SimpleEvent(g_date->Date() + 1, dk_ww_herbicide1, false);
        break;

    case dk_ww_ferti_p2:
        if (m_field->GetSoilType() == tos_Sand || m_field->GetSoilType() == tos_LoamySand || m_field->GetSoilType() == tos_SandyLoam || m_field->GetSoilType() == tos_SandyClayLoam) // on sandy soils (NL ZAND & LOSS)
        {
            if (!m_farm->FP_ManganeseSulphate(m_field, 0.0,
                g_date->DayInYear(3, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p2, true);
                break;
            }
        }
        else if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->FP_ManganeseSulphate(m_field, 0.0,
                g_date->DayInYear(3, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p2, true);
                break;
            }
            DK_WW_MN_P = true; // we need to remember who sprays Mn
        }
        SimpleEvent(g_date->Date() + 1, dk_ww_herbicide1, false);
        break;

    case dk_ww_herbicide1:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.75)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(4, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_herbicide1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 1, dk_ww_herbicide2, false);
        break;

    case dk_ww_herbicide2:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(5, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_herbicide2, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 7, dk_ww_herbicide3, false);
        break;

    case dk_ww_herbicide3:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(13, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_herbicide3, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 1, dk_ww_herbicide4, false);
        break;

    case dk_ww_herbicide4:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.30)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(14, 10) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_herbicide4, true);
                break;
            }
        }
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 2) + 365, dk_ww_wait, false);
        break;

    case dk_ww_wait:
        if (!m_farm->SleepAllDay(m_field, 0.0,
            g_date->DayInYear(28, 2) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_wait, true);
            break;
        }
        if (a_farm->IsStockFarmer()) //Stock Farmer
        {
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3), dk_ww_ferti_s3, false);
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3), dk_ww_ferti_s4, false);
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(10, 3), dk_ww_ferti_s5, false);
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 4), dk_ww_gr, false);
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 4), dk_ww_herbicide5, false);
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 4), dk_ww_fungicide1, false);
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 5) + m_date_modifier, dk_ww_insecticide1, false); // main thread
            break;
        }
        else SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3), dk_ww_ferti_p3, false);
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3), dk_ww_ferti_p4, false);
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(10, 3), dk_ww_ferti_p5, false);
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 4), dk_ww_gr, false);
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 4), dk_ww_herbicide5, false);
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 4), dk_ww_fungicide1, false);
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 5) + m_date_modifier, dk_ww_insecticide1, false); // main thread
        break;

    case dk_ww_ferti_s3:
        if (m_field->GetSoilType() == tos_Sand || m_field->GetSoilType() == tos_LoamySand || m_field->GetSoilType() == tos_SandyLoam || m_field->GetSoilType() == tos_SandyClayLoam) // on sandy soils (NL ZAND & LOSS)
        {
            if (!m_farm->FA_ManganeseSulphate(m_field, 0.0,
                g_date->DayInYear(31, 3) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s3, true);
                break;
            }
        }
        else if (DK_WW_MN_S == true) {
            if (m_ev->m_lock || m_farm->DoIt_prob(0.20 / 0.25)) {
                if (!m_farm->FA_ManganeseSulphate(m_field, 0.0,
                    g_date->DayInYear(31, 3) - g_date->DayInYear())) {
                    SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s3, true);
                    break;
                }
            }
        }
        break; // end of thread

    case dk_ww_ferti_p3:
        if (m_field->GetSoilType() == tos_Sand || m_field->GetSoilType() == tos_LoamySand || m_field->GetSoilType() == tos_SandyLoam || m_field->GetSoilType() == tos_SandyClayLoam) // on sandy soils (NL ZAND & LOSS)
        {
            if (!m_farm->FP_ManganeseSulphate(m_field, 0.0,
                g_date->DayInYear(31, 3) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p3, true);
                break;
            }
        }
        else if (DK_WW_MN_P == true) {
            if (m_ev->m_lock || m_farm->DoIt_prob(0.20 / 0.25)) {
                if (!m_farm->FP_ManganeseSulphate(m_field, 0.0,
                    g_date->DayInYear(3, 10) - g_date->DayInYear())) {
                    SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p3, true);
                    break;
                }
            }
        }
        break;

    case dk_ww_ferti_s4:
        if (!m_farm->FA_Slurry(m_field, 0.0,
            g_date->DayInYear(15, 4) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s4, true);
            break;
        }
        break; // end of thread

    case dk_ww_ferti_p4:
        if (!m_farm->FP_Slurry(m_field, 0.0,
            g_date->DayInYear(15, 4) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p4, true);
            break;
        }
        break; // end of thread

    case dk_ww_ferti_s5:
        if (!m_farm->FA_NPK(m_field, 0.0,
            g_date->DayInYear(20, 3) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s5, true);
            break;
        }
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(10, 4), dk_ww_ferti_s6, false);
        break;

    case dk_ww_ferti_p5:
        if (!m_farm->FP_NPK(m_field, 0.0,
            g_date->DayInYear(20, 3) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p5, true);
            break;
        }
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(10, 4), dk_ww_ferti_p6, false);
        break;

    case dk_ww_ferti_s6:
        if (!m_farm->FA_NPK(m_field, 0.0,
            g_date->DayInYear(20, 4) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s6, true);
            break;
        }
        break; // end of thread

    case dk_ww_ferti_p6:
        if (!m_farm->FP_NPK(m_field, 0.0,
            g_date->DayInYear(20, 4) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p6, true);
            break;
        }
        break; // end of thread   

    case dk_ww_gr:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.80)) {
            if (!m_farm->GrowthRegulator(m_field, 0.0,
                g_date->DayInYear(31, 5) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_gr, true);
                break;
            }
        }
        break; // end of thread

    case dk_ww_herbicide5:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.90)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(15, 4) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_herbicide5, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 14, dk_ww_herbicide6, false);
        break;

    case dk_ww_herbicide6:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(30, 4) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_herbicide6, true);
                break;
            }
        }
        break; // end of thread

    case dk_ww_fungicide1:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.98)) {
            if (!m_farm->FungicideTreat(m_field, 0.0,
                g_date->DayInYear(15, 5) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_fungicide1, true);
                break;
            }
            SimpleEvent(g_date->Date() + 14, dk_ww_fungicide2, false);
            break;
        }
        break; // end of thread
        
    case dk_ww_fungicide2:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.98)) {
            if (!m_farm->FungicideTreat(m_field, 0.0,
                g_date->DayInYear(31, 5) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_fungicide2, true);
                break;
            }
        }
        break; // end of thread    
    
    case dk_ww_insecticide1:
#ifdef ECOSTACK_BIOPESTICIDE
        if (m_ev->m_lock || m_farm->DoIt_prob(0.60)) {
#else
        if (m_ev->m_lock || m_farm->DoIt_prob(0.30)) {
#endif
            // here we check whether we are using ERA pesticide or not
            d1 = g_date->DayInYear(31, 5) - g_date->DayInYear();
            if (!m_field->GetLandscape()->SupplyShouldSpray()) // Not using pesticide spray
            {
#ifdef ECOSTACK_BIOPESTICIDE
                flag = m_farm->BiocideTreat(m_field, 0.0, d1);
#else
                flag = m_farm->InsecticideTreat(m_field, 0.0, d1);
#endif
            }
            if (!flag) {
                SimpleEvent(g_date->Date() + 1, dk_ww_insecticide1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date()+14, dk_ww_insecticide2, false);
        break;

    case dk_ww_insecticide2:
#ifdef ECOSTACK_BIOPESTICIDE
        if (m_ev->m_lock || m_farm->DoIt_prob(0.10)) {
#else
        if (m_ev->m_lock || m_farm->DoIt_prob(0.05)) {
#endif
            // here we check whether we are using ERA pesticide or not
            d1 = g_date->DayInYear(15, 6) - g_date->DayInYear();
            if (!m_field->GetLandscape()->SupplyShouldSpray()) // Not using pesticide spray
            {
#ifdef ECOSTACK_BIOPESTICIDE
                flag = m_farm->BiocideTreat(m_field, 0.0, d1);
#else
                flag = m_farm->InsecticideTreat(m_field, 0.0, d1);
#endif
            }
            if (!flag) {
                SimpleEvent(g_date->Date() + 1, dk_ww_insecticide2, true);
                break;
            }
        }
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(20, 6), dk_ww_herbicide7, false);
        break;

    case dk_ww_herbicide7: // only if for fodder- not allowed for consume
        if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(14, 8) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_herbicide7, true);
                break;
            }
        }
        if (a_farm->IsStockFarmer()) //Stock Farmer
        {
            SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s7, false);
            SimpleEvent(g_date->Date() + 14, dk_ww_harvest, false); // main thread
            break;
        }
        else SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p7, false);
        SimpleEvent(g_date->Date() + 14, dk_ww_harvest, false); // main thread
        break;

    case dk_ww_ferti_s7:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.02)) {
            if (!m_farm->FA_Calcium(m_field, 0.0,
                g_date->DayInYear(15, 8) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_s7, true);
                break;
            }
        }
        break; // end of thread

    case dk_ww_ferti_p7:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.02)) {
            if (!m_farm->FP_Calcium(m_field, 0.0,
                g_date->DayInYear(15, 8) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_ww_ferti_p7, true);
                break;
            }
        }
        break; // end of thread

    case dk_ww_harvest:
        if (!m_farm->Harvest(m_field, 0.0, m_field->GetMDates(1, 0) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_ww_harvest, true);
            break;
        }
        done = true;
        break;

    default:
        g_msg->Warn(WARN_BUG, "DK_WinterWheat::Do(): "
            "Unknown event type! ", "");
        exit(1);
    }
    return done;
}