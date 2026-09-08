//
// Croprotation.h
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


#ifndef CROPROTATION_H
#define CROPROTATION_H

#include <vector>

extern class CropRotation *g_rotation;

const int NoFarmTypes = 2;

struct Rotation
{
  TTypesOfVegetation CropNum[ NoFarmTypes ];
  TTypesOfVegetation NewCrop[ NoFarmTypes ][4];
  int Percent[ NoFarmTypes ][4];
};

struct Starter
{
  TTypesOfVegetation CropNum[ NoFarmTypes ];
  int Percent[ NoFarmTypes ];
};

class CropRotation
{
  vector< Rotation* > m_rots;
  vector< Starter* >  m_start;

public:
  TTypesOfVegetation GetNextCrop( int a_farmtype, int a_current_crop );
  TTypesOfVegetation GetFirstCrop( int a_farmtype, bool *a_low_nutrient );
       CropRotation( int a_num_crops );
      ~CropRotation( void );
};

struct ManagementDependency
{
	/** Defines an activity and a condition e.g. activity number X and if true the 
	current activity will not be executed unless X hhas been done. If false the reverse.*/
	unsigned m_activity;
	bool m_condition;
};

#ifdef __NEWCropactivityCODE

/** \brief A class to model crop management activity */
class CropManagementActivity
{
protected:
	/** \brief The activity type based on FarmToDo enum */
	FarmToDo m_activitytype;
	/** \brief The amount or value of the activity e.g 20mg pesticide */
	double m_amount;
	/** \brief The base probability of doing the activity*/
	double m_baseprobability;
	/** \brief The first possible activity date */
	unsigned m_startdate;
	/** \brief The length of time in days that the activity could be carried out */
	unsigned m_activityperiod;
	/** \brief A unique activity number */
	unsigned m_activityID;
	/** \brief An array holding the list of next activities */
	vector<unsigned> m_NextActivities;
	/** \brief An array of probabilities for each next possible action */
	vector<double> m_NextActivitiesProbs;
	/** \brief A list of dependency conditions */
	vector<ManagementDependency>m_dependencies;
	/** \brief Flag to indicate managment is done or not*/
	bool m_finishedornot;
public:
	/** \brief The CropManagementActiviy constructor*/
	CropManagementActivity(FarmToDo a_activitytype,
		double a_amount,
		double a_baseprobability,
		unsigned a_startdate,
		unsigned a_activityperiod,
		unsigned a_activityID,
		vector<unsigned> a_NextActivities,
		vector<double> a_NextActivitiesProbs,
		vector<ManagementDependency>a_dependencies)
	{
		m_finishedornot = false;
		m_activitytype = a_activitytype;
		m_amount = a_amount;
		m_baseprobability = a_baseprobability;
		m_startdate = a_startdate;
		m_activityperiod = a_activityperiod;
		m_activityID  = a_activityID;
		m_NextActivities = a_NextActivities;
		m_NextActivitiesProbs = a_NextActivitiesProbs;
		m_dependencies = a_dependencies;
	}
	/** \brief Execute the managment if possible and return next management */
	unsigned DoManagement();
};
#endif

#endif // CROPROTATION_H

