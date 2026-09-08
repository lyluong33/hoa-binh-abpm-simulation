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

/** \file Theoretical1.h
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
//---------------------------------------------------------------------------

#ifndef Theoretical1H
#define Theoretical1H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

class Theoretical1;
class Theoretical1_Population_Manager;

//------------------------------------------------------------------------------
/**
Used for the population manager's list of Theoretical1
*/
//typedef vector<Theoretical1*> TListOfTheoretical1;
//---------------------------------------------------------------------------

/**
Theoretical1 like other ALMaSS animals work using a state/transition concept.
These are the Theoretical1 behavioural states, these need to be altered, but some are here just to show how they should look.
*/
using TTypeOfTheoretical1State = enum {
	toTheoretical1s_InitialState = 0,
	toTheoretical1s_Develop,
	toTheoretical1s_Die
};

class Theoretical1 : public TAnimal {
	/**
	A Theoretical1 must have some simple functionality:
	Inititation
	Movement
	Dying
 
	Inherits m_Location_x, m_Location_y, m_OurLandscape from TAnimal
	NB All areas are squares of size length X length
	*/

protected:
	/** \brief Variable to record current behavioural state */
	TTypeOfTheoretical1State m_CurrentNAState;
	/** \brief A size class */
	double m_Size;
	/** \brief This is a time saving pointer to the correct population manager object */
	Theoretical1_Population_Manager* m_OurPopulationManager;

public:
	/** \brief Theoretical1 constructor */
	Theoretical1(int a_x, int a_y, Landscape* a_l_ptr, Theoretical1_Population_Manager* a_NPM_ptr);
	/** \brief Theoretical1 destructor */
	~Theoretical1() override;
	/** \brief Behavioural state development */
	TTypeOfTheoretical1State st_Develop(void);
	/** \brief The BeginStep is the first 'part' of the timestep that an animal can behave in. It is called once per timestep. */
	void BeginStep(void) override {
	} // NB this is not used in the Theoretical1 code
	/** \brief The Step is the second 'part' of the timestep that an animal can behave in. It is called continuously until all animals report that they are 'DONE'. */
	void Step(void) override;
	/** \brief The EndStep is the third 'part' of the timestep that an animal can behave in. It is called once per timestep. */
	void EndStep(void) override {
	} // NB this is not used in the Theoretical1 code
	/** \brief A typical interface function - this one returns the size */
	double GetSize() { return m_Size; }
	/** \brief A typical interface function - this one sets the size */
	void SetSize(unsigned a_sz) { m_Size = a_sz; }
};

#endif
