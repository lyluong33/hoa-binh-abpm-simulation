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

/** \file Theoretical2_Population_Manager.cpp
\brief <B>The main source code for all predator lifestage and population manager classes</B>
*/
/**
Version of 1 July 2022 \n
By Chris J. Topping \n
*
* This is a minimal example of an ALMaSS agent-based model. It does not consider space therefore can be run with a dummy landscape.
* The model considers only the effects of large scale mortality
*/
/******************************************************************************************************/

#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../ALMaSSDefines.h"
#include "../Landscape/ls.h"
#include "../BatchALMaSS/PopulationManager.h"
#include "../Theoretical/Theoretical2.h"
#include "../Theoretical/Theoretical2_Population_Manager.h"

//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------

Theoretical2_Population_Manager::Theoretical2_Population_Manager(Landscape* L) : Population_Manager(L, 1)
{
	m_is_paralleled = true;
    // Load List of Animal Classes
	m_ListNames[0] = "Theoretical2";
	m_ListNameLength = 1;
	m_SimulationName = "Theoretical2";
	// Create some animals by creating a data structure and passing this to CreateObjects below
	int temp_thread_num = omp_get_max_threads();
	cout<<"Thread used: "<<temp_thread_num<<endl;
    int start_num_in_thread = (10000 / temp_thread_num);
    #pragma omp parallel
	{
		struct_Theoretical2* sp;
		sp = new struct_Theoretical2;
		sp->NPM = this;
		sp->L = m_TheLandscape; // Not needed for this simulation, but a standard part of the ALMaSS TAnimal class.
		for (int i=0; i< start_num_in_thread; i++) // Assume we start with 10000 individuals
		{
			sp->x = g_random_fnc(SimW); // We assigning a location, but for this version of the model it is not actually used.
			sp->y = g_random_fnc(SimH);
			CreateObjects(0,NULL,sp,1); // 0 = our Theoretical2 life stage, there is only one
			//IncLiveArraySize(0); // Recoded a new live indiviual. Commented out since it is handled in the CreateObjects function.
		}
		delete sp;
	}
	// Normally would be an input paramter, but for demonstration this choice is hard coded.
	BeforeStepActions[0] = 4; // 4 = do nothing, 0 = randomise
	//BeforeStepActions[0] = 0; // 4 = do nothing, 0 = randomise
}

//---------------------------------------------------------------------------

Theoretical2_Population_Manager::~Theoretical2_Population_Manager(void)
{
}
//---------------------------------------------------------------------------

void Theoretical2_Population_Manager::CreateObjects(int ob_type,
           TAnimal * ,struct_Theoretical2 * data, int number)
{
   Theoretical2*  new_Theoretical2;
   for (int i = 0; i < number; i++)
	{
		new_Theoretical2 = new Theoretical2(data->x, data->y, data->L, data->NPM);
		PushIndividual(ob_type, new_Theoretical2);
	}
 
}
//---------------------------------------------------------------------------

void Theoretical2_Population_Manager::DoFirst()
{		
	

	//std::cout<<"population: "<<current_num<<std::endl;
	//kill 1000
	int temp_thread_num = omp_get_max_threads();
	int kill_num_in_thread = 1000 / temp_thread_num;
	#pragma omp parallel
	{
		int temp_killed_number = 0;
		for (auto it = TheSubArrays[0][omp_get_thread_num()]->begin(); it != TheSubArrays[0][omp_get_thread_num()]->end(); ++it){ 
			temp_killed_number++;
			dynamic_cast<Theoretical2*>(*it)->st_Die(); 
			if(temp_killed_number >= kill_num_in_thread){
				break;
			}
		}
	}

	//new born 1040
    int new_num_in_thread = (1040 / temp_thread_num);
	#pragma omp parallel
	{
		struct_Theoretical2* sp;
		sp = new struct_Theoretical2;
		sp->NPM = this;
		sp->L = m_TheLandscape; // Not needed for this simulation, but a standard part of the ALMaSS TAnimal class.
		for (int i = 0; i < new_num_in_thread; i++) {
			sp->x = g_random_fnc(SimW); // We assigning a location, but for this version of the model it is not actually used.
			sp->y = g_random_fnc(SimH);
			CreateObjects(0, nullptr, sp, 1);//
		}
		delete sp;
	}

	PartitionLiveDead(0);
}
//---------------------------------------------------------------------------
