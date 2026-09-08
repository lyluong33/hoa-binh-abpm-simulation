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

/** \file Theoretical1_Population_Manager.cpp
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

#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Theoretical/Theoretical1.h"
#include "../Theoretical/Theoretical1_Population_Manager.h"

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------

Theoretical1_Population_Manager::Theoretical1_Population_Manager(Landscape* L) : Population_Manager(L, 1) {
	// Load List of Animal Classes
	m_ListNames[0] = "Theoretical1";
	m_ListNameLength = 1;
	m_SimulationName = "Theoretical1";
	// Create some animals by creating a data structure and passing this to CreateObjects below
	const auto sp = new struct_Theoretical1;
	sp->NPM = this;
	sp->L = m_TheLandscape; // Not needed for this simulation, but a standard part of the ALMaSS TAnimal class.
	for (int i = 0; i < 10000; i++) // Assume we start with 10000 individuals
	{
		sp->m_x = g_random_fnc(SimW); // We assigning a location, but for this version of the model it is not actually used.
		sp->m_y = g_random_fnc(SimH);
		CreateObjects(0, nullptr, sp, 1); // 0 = our Theoretical1 life stage, there is only one
		IncLiveArraySize(0); // Recoded a new live individual
	}
	// Normally would be an input parameter, but for demonstration this choice is hard coded.
	BeforeStepActions[0] = 3; // 3 = do nothing, 0 = randomise
	// Open the output file
	m_SizeClassesFile.open("Theoretical_SizeClasses.txt", ios::out);
}

//---------------------------------------------------------------------------

Theoretical1_Population_Manager::~Theoretical1_Population_Manager(void) {
	// Close the output file when the population manager is destroyed
	m_SizeClassesFile.close();
}
//---------------------------------------------------------------------------

void Theoretical1_Population_Manager::CreateObjects(const int a_obType, TAnimal* /*unused*/, struct_Theoretical1* a_data, const int a_number) {
	for (int i = 0; i < a_number; i++)
	{
		auto new_Theoretical1 = new Theoretical1(a_data->m_x, a_data->m_y, a_data->L, a_data->NPM);
		PushIndividual(a_obType, new_Theoretical1);
	}
}
//---------------------------------------------------------------------------

void Theoretical1_Population_Manager::DoFirst() {
	/** 
	* If it is the first day of the months, then allocate the individuals to 10 size bins between zero and max size.
	* Determines the maximum and minimul sizes and ouputs the result every simulation month.
	* Finally sets the amount of food available today based on the number of individuals alive.
	*/
	if (g_date->GetDayInMonth() == 1)
	{
		int bins[10];
		for (int i = 0; i < 10; i++) bins[i] = 0;
		double min = 1000000000;
		double max = 0;
		// find the min and max size, then allocate all to 10 bins
		const unsigned size1 = GetLiveArraySize(0);
		for (unsigned j = 0; j < size1; j++)
		{
			if (static_cast<Theoretical1*>(SupplyAnimalPtr(0, j))->GetSize() < min)
				min = static_cast<Theoretical1*>(SupplyAnimalPtr(0, j))->GetSize();
			if (static_cast<Theoretical1*>(SupplyAnimalPtr(0, j))->GetSize() > max)
				max = static_cast<Theoretical1*>(SupplyAnimalPtr(0, j))->GetSize();
		}
		// Calculate teh bin size
		const double bin = max / 10.0;
		// Now loop through all the animals allocating the individuals to the bins
		for (unsigned j = 0; j < size1; j++)
		{
			const double sz = static_cast<Theoretical1*>(SupplyAnimalPtr(0, j))->GetSize();
			int index = static_cast<int>(floor(sz / bin));
			if (index > 9) index = 9; // The last one at max size and messes up the bin calcuation, so allocate this one specially
			bins[index]++;
		}
		// Now print this months data to the output file
		m_SizeClassesFile << g_date->Date() << '\t' << min << '\t' << max << '\t' << size1;
		for (int i = 0; i < 10; i++) m_SizeClassesFile << '\t' << bins[i];
		m_SizeClassesFile << endl;
	}
	// Set the food for today based on the number of animals
	m_Food = GetLiveArraySize(0);
}
//---------------------------------------------------------------------------
