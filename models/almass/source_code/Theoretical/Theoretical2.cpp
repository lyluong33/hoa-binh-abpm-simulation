/*
*******************************************************************************************************
Copyright (c) 2022, Christopher John Topping, Aarhus University
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
/** \file Theoretical2.cpp
\brief <B>The main source code for all predator lifestage and population manager classes</B>
*/
/**  
Version of 1 July 2022 \n
By Chris J. Topping \n 
*
* This is a minimal example of an ALMaSS agent-based model. It does not consider space therefore can be run with a dummy landscape. 
* The model considers competition for food and the effect of different before step actions.
*/
/******************************************************************************************************/

#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../ALMaSSDefines.h"
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Theoretical/Theoretical2.h"
#include "../Theoretical/Theoretical2_Population_Manager.h"

extern MapErrorMsg *g_msg;

using namespace std;

Theoretical2::Theoretical2(int p_x, int p_y,Landscape* p_L, Theoretical2_Population_Manager* p_NPM) : TAnimal(p_x,p_y,p_L)
{
	// Assign the pointer to the population manager
	m_OurPopulationManager = p_NPM;
	// Sets the initial state
	m_CurrentNAState = toTheoretical2s_InitialState;
}


Theoretical2::~Theoretical2(void)
{
	;
}

void Theoretical2::Step(void)
{
	if (m_StepDone || m_CurrentStateNo == -1) return;
	switch (m_CurrentNAState)
	{
	case toTheoretical2s_InitialState: // Initial state always starts with develop
		m_CurrentNAState = toTheoretical2s_Develop;
		break;
	case toTheoretical2s_Develop:
		// The foraging and development is done here
		m_CurrentNAState = st_Develop(); // Will return this behaviour or die
		break;
	case toTheoretical2s_Die:
		// The animal dies
		st_Die();
		break;
	default:
		m_OurLandscape->Warn("Theoretical2::Step()", "unknown state - default");
		exit(1);
	}
}

TTypeOfTheoretical2State Theoretical2::st_Develop(void)
{
	for (int i = 0; i < 10000; i++) m_data[i] = i*i;
	// Signals that the step is done for this timestep, there is not iteration for this species
	//if(g_rand_uni_fnc()	<= 0.05){
	//	return toTheoretical2s_Die;
	//}
	m_StepDone = true;
	// Signal finished with this behaviour, ready to do it again next timestep
	return toTheoretical2s_Develop;
}

void Theoretical2::st_Die(void)
{
	KillThis();
}
