//
// DK_SpringBarley.cpp
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
#include "../../Landscape/cropprogs/DK_SpringBarley.h"
#include "math.h"


extern CfgFloat cfg_ins_app_prop1;
extern CfgFloat cfg_herbi_app_prop;
extern CfgFloat cfg_fungi_app_prop1;
extern CfgFloat cfg_greg_app_prop;
extern CfgFloat cfg_DKCatchCropPct;

bool DK_SpringBarley::Do(Farm* a_farm, LE* a_field, FarmEvent* a_ev)
{
    m_farm = a_farm;
    m_field = a_field;
    m_ev = a_ev;
    bool done = false; // The boolean value done indicates when we are totally finished with this plan (i.e. it is set to true, m_farm, m_field).
    bool flag = false;
    int d1 = 0;
    int noDates = 1;
    TTypesOfVegetation l_tov = tov_DKSpringBarley; 
    int l_nextcropstartdate;

    switch (m_ev->m_todo)
    {
    case dk_sb_start:
    {
        a_field->ClearManagementActionSum();
        DK_SB_FORCESPRING = false;
        DK_SB_TILL_C = false; // 85% of all on clay soil do till
        DK_SB_TILL_S = false; // 85% of all on sandy soil do till
        m_last_date = g_date->DayInYear(25, 8); // Should match the last flexdate below
        //Create a 2d array of 1 plus the number of operations you use. Change only 4+1 to what you need in the line below
        std::vector<std::vector<int>> flexdates(3 + 1, std::vector<int>(2, 0));
        // Set up the date management stuff
        // Start and stop dates for all events after harvest
        flexdates[0][1] = g_date->DayInYear(1, 8); // last possible day of harvest - this is in effect day before the earliest date that a following crop can use
        // Now these are done in pairs, start & end for each operation. If its not used then -1
        flexdates[1][0] = -1; // This date will be moved back as far as necessary and potentially to flexdates 1 (start op 1)
        flexdates[1][1] = g_date->DayInYear(24, 8); // This date will be moved back as far as necessary and potentially to flexdates 1 (end op 1)
        flexdates[2][0] = -1;  // This date will be moved back as far as necessary and potentially to flexdates 1 (start op 2)
        flexdates[2][1] = g_date->DayInYear(25, 8); // This date will be moved back as far as necessary and potentially to flexdates 1 (end op 2)
        flexdates[3][0] = g_date->DayInYear(2, 8); // This date will be moved back as far as necessary and potentially to flexdates 1 (start op 3)
        flexdates[3][1] = g_date->DayInYear(25, 8); // This date will be moved back as far as necessary and potentially to flexdates 1 (end op 3) // tweaked to fit rot (prev. 1/9)

        // Below if this is a spring crop use 365, otherwise first parameter is always 0, second parameter is fixed, and the third is the start up operation in the first year
        int isSpring = 0;
        if (StartUpCrop(isSpring, flexdates, int(dk_sb_spring_plough))) break;

        // End single block date checking code. Please see next line comment as well.
        // Reinit d1 to first possible starting date.
        d1 = g_date->OldDays() + g_date->DayInYear(1, 9);
        // OK, let's go.
        // Here we queue up the first event which changes dependent on whether it is a forced spring sow or not
        if (m_ev->m_forcespring) {
            if (a_farm->IsStockFarmer()) //Stock Farmer
            {
                SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3) + 365, dk_sb_ferti_s1, false);
                break;
            }
            else SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3) + 365, dk_sb_ferti_p1, false);
            DK_SB_FORCESPRING = true;
            break;
        }
        else SimpleEvent(d1, dk_sb_wait, false);
        break;
    }
    break;
    case dk_sb_wait:
        if (!m_farm->SleepAllDay(m_field, 0.0, g_date->DayInYear(1, 11) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_sb_wait, true);
            break;
        }
        SimpleEvent(g_date->Date(), dk_sb_autumn_plough, false);
        break;
    
    // done if many weeds, or waste plants from earlier catch crops 
    case dk_sb_autumn_plough:
        if (m_field->GetSoilType() != tos_Sand && m_field->GetSoilType() != tos_LoamySand && m_field->GetSoilType() != tos_SandyLoam && m_field->GetSoilType() != tos_SandyClayLoam) // on clay soils (NL KLEI & VEEN)
        {
            if (m_ev->m_lock || m_farm->DoIt_prob(0.85)) {
                if (!m_farm->AutumnPlough(m_field, 0.0, g_date->DayInYear(1, 11) - g_date->DayInYear())) {
                    SimpleEvent(g_date->Date() + 1, dk_sb_autumn_plough, true);
                    break;
                }
                DK_SB_TILL_C = true; // need to remember who did till
            }
        }
        if (a_farm->IsStockFarmer()) //Stock Farmer
        {
            SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3) + 365, dk_sb_ferti_s1, false);
            break;
        } 
        else SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 3) + 365, dk_sb_ferti_p1, false);
        break;

    case dk_sb_ferti_s1:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.50)) {
            if (!m_farm->FA_Slurry(m_field, 0.0,
                g_date->DayInYear(15, 4) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_ferti_s1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 1, dk_sb_ferti_s2, false);
        break;

    case dk_sb_ferti_p1:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.50)) {
            if (!m_farm->FP_Slurry(m_field, 0.0,
                g_date->DayInYear(15, 4) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_ferti_p1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 1, dk_sb_ferti_p2, false);
        break;

    case dk_sb_ferti_s2:
        if (!m_farm->FA_NPKS(m_field, 0.0,
            g_date->DayInYear(16, 4) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_sb_ferti_s2, true);
            break;
        }
        SimpleEvent(g_date->Date() + 1, dk_sb_spring_plough, false);
        break;

    case dk_sb_ferti_p2:
        if (!m_farm->FP_NPKS(m_field, 0.0,
            g_date->DayInYear(16, 4) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_sb_ferti_p2, true);
            break;
        }
        SimpleEvent(g_date->Date() + 1, dk_sb_spring_plough, false);
        break;

    case dk_sb_spring_plough:
        if (m_field->GetSoilType() == tos_Sand || m_field->GetSoilType() == tos_LoamySand || m_field->GetSoilType() == tos_SandyLoam || m_field->GetSoilType() == tos_SandyClayLoam) // on sandy soils (NL ZAND & LOSS)
        {
            if (m_ev->m_lock || m_farm->DoIt_prob(0.85)) {
                if (!m_farm->SpringPlough(m_field, 0.0,
                    g_date->DayInYear(20, 4) - g_date->DayInYear())) {
                    SimpleEvent(g_date->Date() + 1, dk_sb_spring_plough, true);
                    break;
                }
                DK_SB_TILL_S = true; // need to remember who did till
                SimpleEvent(g_date->Date(), dk_sb_spring_sow, false);
                break;
            }
            SimpleEvent(g_date->Date(), dk_sb_spring_harrow_nt, false);
            break;
        }
        else if (DK_SB_TILL_C == false) {
            SimpleEvent(g_date->Date(), dk_sb_spring_harrow_nt, false);
            break;
        }
        else SimpleEvent(g_date->Date(), dk_sb_spring_sow, false);
        break;


    case dk_sb_spring_harrow_nt: // 80% of the 15% of clay and 15% of sandy soils that do no till (nt) harrows before sowing 
        if (m_ev->m_lock || m_farm->DoIt_prob(0.80)) {
            if (!m_farm->SpringHarrow(m_field, 0.0,
                g_date->DayInYear(20, 4) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_spring_harrow_nt, true);
                break;
            }
        }
        SimpleEvent(g_date->Date(), dk_sb_spring_sow, false);
        break;

    case dk_sb_spring_sow:
        if (m_ev->m_lock || m_farm->DoIt_prob(1.00)) {
            if (!m_farm->PreseedingCultivatorSow(m_field, 0.0,
                g_date->DayInYear(30, 4) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_spring_sow, true);
                break;
            }
        } 
        SimpleEvent(g_date->Date() + 1, dk_sb_herbicide1, false); // herbi thread
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 5), dk_sb_fungicide1, false); // fungi thread
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 5), dk_sb_gr1, false); // growth regulator thread
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 6) + m_date_modifier, dk_sb_insecticide, false); // insecti thread
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 7), dk_sb_water1, false); // water thread
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(7, 7), dk_sb_herbicide4, false); // main thread
        break;

    case dk_sb_herbicide1: // if sown layout of grass together with the barley
        if (m_ev->m_lock || m_farm->DoIt_prob(0.10)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(5, 5) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_herbicide1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 14, dk_sb_herbicide2, false);
        break;

    case dk_sb_herbicide2: 
        if (m_ev->m_lock || m_farm->DoIt_prob(1.0)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(20, 5) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_herbicide2, true);
                break;
            }
        }
        SimpleEvent(g_date->Date() + 14, dk_sb_herbicide3, false);
        break;

    case dk_sb_herbicide3:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.15)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0,
                g_date->DayInYear(5, 6) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_herbicide3, true);
                break;
            }
        }
        break; // end of thread

    case dk_sb_fungicide1:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.95)) {
            if (!m_farm->FungicideTreat(m_field, 0.0,
                g_date->DayInYear(31, 5) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_fungicide1, true);
                break;
            }
        }
        SimpleEvent(g_date->Date()+14, dk_sb_fungicide2, false);
        break;

    case dk_sb_fungicide2:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.20)) {
            if (!m_farm->FungicideTreat(m_field, 0.0,
                g_date->DayInYear(15, 6) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_fungicide2, true);
                break;
            }
        }
        break; // end of thread

    case dk_sb_gr1:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->GrowthRegulator(m_field, 0.0,
                g_date->DayInYear(31, 5) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_gr1, true);
                break;
            }
            break; // end of thread
        }
        SimpleEvent(g_date->Date() + 30, dk_sb_gr2, false);
        break;

    case dk_sb_gr2:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.30/0.75)) {
            if (!m_farm->GrowthRegulator(m_field, 0.0,
                g_date->DayInYear(30, 6) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_gr2, true);
                break;
            }
        }
        break; // end of thread

    case dk_sb_insecticide:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.40)) {
            // here we check whether we are using ERA pesticide or not
            d1 = g_date->DayInYear(31, 7) - g_date->DayInYear();
            if (!m_field->GetLandscape()->SupplyShouldSpray()) // Not using pesticide spray
            {
                flag = m_farm->InsecticideTreat(m_field, 0.0, d1);
            }
            if (!flag) {
                SimpleEvent(g_date->Date() + 1, dk_sb_insecticide, true);
                break;
            }
        }
        break; // end of thread

    case dk_sb_water1:
        if (m_field->GetSoilType() == tos_Sand || m_field->GetSoilType() == tos_LoamySand || m_field->GetSoilType() == tos_SandyLoam || m_field->GetSoilType() == tos_SandyClayLoam) // on sandy soils (NL ZAND & LOSS)
        {
            if (m_ev->m_lock || m_farm->DoIt_prob(0.80)) {
                if (!m_farm->Water(m_field, 0.0,
                    g_date->DayInYear(10, 7) - g_date->DayInYear())) {
                    SimpleEvent(g_date->Date() + 1, dk_sb_water1, true);
                    break;
                }
                SimpleEvent(g_date->Date()+14, dk_sb_water2, false); 
                break;
            }
        }
        break; // end of thread

    case dk_sb_water2:
        if (!m_farm->Water(m_field, 0.0,
            g_date->DayInYear(31, 7) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_sb_water2, true);
            break;
        }
        break; // end of thread

    case dk_sb_herbicide4:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.30)) {
            if (!m_farm->HerbicideTreat(m_field, 0.0, m_field->GetMDates(1, 0) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_herbicide4, true);
                break;
            }
            d1 = g_date->Date() + 14;
            if (d1 < g_date->OldDays() + g_date->DayInYear(1, 8)) d1 = g_date->OldDays() + g_date->DayInYear(1, 8);
            SimpleEvent(d1, dk_sb_harvest, false);
            break;
        }
        SimpleEvent(g_date->OldDays() + g_date->DayInYear(1, 8), dk_sb_harvest, false); 
        break;

    case dk_sb_harvest:
        if (!m_farm->Harvest(m_field, 0.0, m_field->GetMDates(1, 1) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_sb_harvest, true);
            break;
        }
            SimpleEvent(g_date->Date() + 1, dk_sb_straw_chopping, false);
            break;

    case dk_sb_straw_chopping:
        if (m_ev->m_lock || m_farm->DoIt_prob(0.25)) {
            if (!m_farm->StrawChopping(m_field, 0.0, m_field->GetMDates(1, 2) - g_date->DayInYear())) {
                SimpleEvent(g_date->Date() + 1, dk_sb_straw_chopping, true);
                break;
            }
                done = true;
            break;
        }
        else if (m_ev->m_lock || m_farm->DoIt_prob(0.75 / 0.75)) {
            SimpleEvent(g_date->Date() + 1, dk_sb_hay_bailing, false);
            break;
        }

    case dk_sb_hay_bailing:
        if (!m_farm->HayBailing(m_field, 0.0, m_field->GetMDates(1, 3) - g_date->DayInYear())) {
            SimpleEvent(g_date->Date() + 1, dk_sb_hay_bailing, true);
            break;
        }
       done = true;
       break; 

    default:
        g_msg->Warn(WARN_BUG, "DK_SpringBarley::Do(): "
            "Unknown event type! ", "");
        exit(1);
    }
    return done;
}
