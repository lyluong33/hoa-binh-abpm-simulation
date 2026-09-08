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

/** \file Theoretical2.h
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
#ifndef Theoretical2H
#define Theoretical2H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

class Theoretical2;
class Theoretical2_Population_Manager;
class struct_Theoretical2;

//------------------------------------------------------------------------------
/**
Used for the population manager's list of Theoretical2
*/
//typedef vector<Theoretical2*> TListOfTheoretical2;
//---------------------------------------------------------------------------

/**
Theoretical2 like other ALMaSS animals work using a state/transition concept.
These are the Theoretical2 behavioural states, these need to be altered, but some are here just to show how they should look.
*/
typedef enum
{
      toTheoretical2s_InitialState=0,
      toTheoretical2s_Develop,
      toTheoretical2s_Die
} TTypeOfTheoretical2State;


class Theoretical2 : public TAnimal
{
   /**
   A Theoretical2 must have some simple functionality:
   Inititation
   Movement
   Dying

   Inherits m_Location_x, m_Location_y, m_OurLandscape from TAnimal
   NB All areas are squares of size length X length
   */

protected:
   /** \brief Variable to record current behavioural state */
   TTypeOfTheoretical2State m_CurrentNAState;
   /** \brief This is a time saving pointer to the correct population manager object */
   Theoretical2_Population_Manager*  m_OurPopulationManager;
   double m_data[10000];
public:
   /** \brief Theoretical2 constructor */
   Theoretical2(int p_x, int p_y,Landscape* p_L, Theoretical2_Population_Manager* p_NPM);
   /** \brief Theoretical2 destructor */
   ~Theoretical2();
   /** \brief Behavioural state development */
   TTypeOfTheoretical2State st_Develop( void );
   /** \brief Behavioural state die */
   void st_Die( void );
   /** \brief The BeginStep is the first 'part' of the timestep that an animal can behave in. It is called once per timestep. */
   virtual void BeginStep(void) {} // NB this is not used in the Theoretical2 code
   /** \brief The Step is the second 'part' of the timestep that an animal can behave in. It is called continuously until all animals report that they are 'DONE'. */
   virtual void Step(void);
   /** \brief The EndStep is the third 'part' of the timestep that an animal can behave in. It is called once per timestep. */
   virtual void EndStep(void) {} // NB this is not used in the Theoretical2 code
   /** \brief Used to reinitialise an object */
   void ReInit(struct_Theoretical2* data);
};

#endif
