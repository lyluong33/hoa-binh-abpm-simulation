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

/** \file Theoretical1_Population_Manager.h
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
#ifndef Theoretical1_Population_ManagerH
#define Theoretical1_Population_ManagerH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

class Theoretical1;

//------------------------------------------------------------------------------

/**
\brief
Used for creation of a new Theoretical1 object
*/
class struct_Theoretical1 {
public:
	/** \brief x-coord */
	int m_x;
	/** \brief y-coord */
	int m_y;
	/** \brief species ID */
	int m_species;
	/** \brief Landscape pointer */
	Landscape* L;
	/** \brief Theoretical1_Population_Manager pointer */
	Theoretical1_Population_Manager* NPM;
};

/**
\brief
The class to handle all predator population related matters
*/
class Theoretical1_Population_Manager : public Population_Manager {
public:
	// Methods
	/** \brief Theoretical1_Population_Manager Constructor */
	Theoretical1_Population_Manager(Landscape* L);
	/** \brief Theoretical1_Population_Manager Destructor */
	~Theoretical1_Population_Manager() override;
	/** \brief Method for creating a new individual Theoretical1 */
	void CreateObjects(int a_obType, TAnimal* a_pvo, struct_Theoretical1* a_data, int a_number);
	/** \brief Get the amount of food */
	double GetFood() const { return m_Food; }
	/** \brief Reduce the amount of food */
	void SubtractFood(const double a_food) { m_Food -= a_food; }

protected:
// Attributes
   /** \brief Output file */
	ofstream m_SizeClassesFile;
	/** \brief A member variable to store the current amount of food */
	double m_Food;
	// Methods
   /** \brief  Things to do before anything else at the start of a timestep  */
	virtual void DoFirst();
   /** \brief Things to do before the Step */
   virtual void DoBefore(){}
   /** \brief Things to do before the Step */
   virtual void DoAfter(){}
   /** \brief Things to do after the EndStep */
   virtual void DoLast(){}
};

#endif
