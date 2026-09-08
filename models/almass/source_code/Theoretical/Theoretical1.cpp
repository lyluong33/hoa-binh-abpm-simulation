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
/** \file Theoretical1.cpp
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
#include "../Theoretical/Theoretical1.h"
#include "../Theoretical/Theoretical1_Population_Manager.h"

extern MapErrorMsg* g_msg;

using namespace std;

Theoretical1::Theoretical1(int a_x, int a_y, Landscape* a_l_ptr, Theoretical1_Population_Manager* a_NPM_ptr) : TAnimal(
	a_x, a_y) {
	// Assign the pointer to the population manager
	m_OurPopulationManager = a_NPM_ptr;
	// Sets the initial state
	m_CurrentNAState = toTheoretical1s_InitialState;
	// Assigns a starting size
	m_Size = 50.0;
}

Theoretical1::~Theoretical1(void) {
}

void Theoretical1::Step(void) {
	if (m_StepDone || m_CurrentStateNo == -1) return;
	switch (m_CurrentNAState)
	{
	case toTheoretical1s_InitialState: // Initial state always starts with develop
		m_CurrentNAState = toTheoretical1s_Develop;
		break;
	case toTheoretical1s_Develop:
		// The foraging and development is done here
		m_CurrentNAState = st_Develop(); // Will return this behaviour or die
	// Signals that the step is done for this timestep, there is not iteration for this species
		m_StepDone = true;
		break;
	default:
		m_OurLandscape->Warn("Theoretical1::Step()", "unknown state - default");
		exit(1);
	}
}

TTypeOfTheoretical1State Theoretical1::st_Develop(void) {
	// Find out if anyone is small enough to eat, will return nullptr if there is nobody
	TAnimal* animal = m_OurPopulationManager->FindIndividual(0, this);
	if (animal != nullptr)
	{
		double const sz = static_cast<Theoretical1*>(animal)->GetSize();
		if (sz <= m_Size * 0.8)
		{
			// Uses the build in KillThis for TAnimal to remove the  eaten animal immediately
			animal->KillThis();
			// Increase by 0.5% of the prey size
			m_Size += sz * 0.005;
		}
	}
	double food = 0;
	// Calculate the food, get a bit more than a fair share if possible, but with stochastic variation
	if (m_OurPopulationManager->GetFood() > 1.5) food = g_rand_uni_fnc() + 0.6;
	// Remove the food from the food pool
	m_OurPopulationManager->SubtractFood(food);
	// Grow, for simplicity we assume the forage amount is translated directly to growth
	m_Size += food;
	// Signal finished with this behaviour, ready to do it again next timestep
	return toTheoretical1s_Develop;
}
