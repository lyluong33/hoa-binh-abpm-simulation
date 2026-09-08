//
//CurveClasses.cpp
//
/*
*******************************************************************************************************
Copyright (c) 2014, Christopher John Topping, Aarhus University
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

#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include "CurveClasses.h"

using namespace std;

//************************************************************************************************************
//********************** CurveClass **************************************************************************
//************************************************************************************************************

CurveClass::CurveClass(bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name)
{
	m_parameterMinX = a_MinX;
	m_parameterMaxX = a_MaxX;
	m_reversecurve = a_reversecurve;
	m_name = a_name;
	/** Make space for the y-values. */
	m_values = new double[10000];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CurveClass::~CurveClass()
{
	delete[] m_values;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CurveClass::CalculateCurveValues()
{
	/** We set m_step to be the size of each step on the X-axis, assuming 10000 steps over the range minX to maxX. */
	m_step = (m_parameterMaxX - m_parameterMinX) / 10000.0;
	m_step_inv = 1.0 / m_step;
	for (int i = 0; i<10000; i++)
	{
		double x = m_parameterMinX + (i * m_step);
		/** Calculate the value based on the input paramters. */
		m_values[i] = DoCalc(x);
		if (m_reversecurve) m_values[i] = 1 - m_values[i];

	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

double CurveClass::GetY(double a_X)
{
	/** This is the time sensitive method so we avoid division, rather use multiplication by the inverse of m_step. 
	Here we have a choice if a_X is bigger than maxX - either return the value for maxX or return parameter A (asymptote) */
	if (a_X > m_parameterMaxX) return m_values[9999]; //return m_parameterA;
	/** If a_X is <= minX then we need to return minX */
	if (a_X <= m_parameterMinX) return m_values[0];
	/** Otherwise we have to calculate which index is closest to a_X */
	int index = (int)((a_X - m_parameterMinX)* m_step_inv);
	/** Find and return the correct Y-value */
	return m_values[index];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CurveClass::WriteDataFile(int a_step)
{
	/** Opens a stream file and outputs every a_step data points as x & y, tab separated. */
	string str1 = "CurvePoints" + m_name + ".txt";
	ofstream ofile(str1.c_str(), ios::out);
	for (int i = 0; i<10000; i = i + a_step)
	{
		ofile << m_parameterMinX + m_step*i << '\t' << m_values[i] << endl;
	}
	ofile.close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//************************************************************************************************************
//********************** GompertzCurveClass ******************************************************************
//************************************************************************************************************

GompertzCurveClass::GompertzCurveClass(double a_A, double a_B, double a_C, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name)
																													: CurveClass(a_reversecurve, a_MaxX, a_MinX, a_name)
{
	m_parameterA = a_A;
	m_parameterB = a_B;
	m_parameterC = a_C;
	CalculateCurveValues();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GompertzCurveClass::~GompertzCurveClass()
{
	;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

double GompertzCurveClass::DoCalc(double a_x)
{
	return m_parameterA * exp((exp(a_x * m_parameterC) * m_parameterB));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//************************************************************************************************************
//********************** Polynomial2CurveClass ******************************************************************
//************************************************************************************************************

Polynomial2CurveClass::Polynomial2CurveClass(double a_A, double a_B, double a_C, double a_D, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name)
: CurveClass(a_reversecurve, a_MaxX, a_MinX, a_name)
{
	m_parameterA = a_A;
	m_parameterB = a_B;
	m_parameterC = a_C;
	m_parameterD = a_D;
	CalculateCurveValues();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Polynomial2CurveClass::~Polynomial2CurveClass()
{
	;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

double Polynomial2CurveClass::DoCalc(double a_x)
{
	//  e.g.   (-0.008*C10*C10 + 0.13*C10 + 0.08) * 19.8 
	return (m_parameterA * a_x * a_x + m_parameterB * a_x + m_parameterC) * m_parameterD;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//************************************************************************************************************
//********************** ThresholdCurveClass ******************************************************************
//************************************************************************************************************

ThresholdCurveClass::ThresholdCurveClass(double a_A, double a_B, double a_C, double a_D, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name)
: CurveClass(a_reversecurve, a_MaxX, a_MinX, a_name)
{
	m_parameterA = a_A;
	m_parameterB = a_B;
	m_parameterC = a_C;
	m_parameterD = a_D;
	CalculateCurveValues();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ThresholdCurveClass::~ThresholdCurveClass()
{
	;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

double ThresholdCurveClass::DoCalc(double a_x)
{
	if ((a_x >= m_parameterC) && (a_x <= m_parameterD)) return m_parameterA;
	return m_parameterB;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//************************************************************************************************************
//********************** HollingsDiscCurveClass ******************************************************************
//************************************************************************************************************

HollingsDiscCurveClass::HollingsDiscCurveClass(double a_A, double a_B, bool a_reversecurve,
										       double a_MaxX, double a_MinX, const char* a_name)
	: CurveClass( a_reversecurve, a_MaxX, a_MinX, a_name ) {
	m_parameterA = a_A;
	m_parameterB = a_B;
	CalculateCurveValues();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

HollingsDiscCurveClass::~HollingsDiscCurveClass() {
	;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

double HollingsDiscCurveClass::DoCalc( double a_x ) {
	return ((a_x * m_parameterA) / (1 + m_parameterA * m_parameterB * a_x));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//************************************************************************************************************
//********************** PettiforFeedingTimeCurve ************************************************************
//************************************************************************************************************

PettiforFeedingTimeCurveClass::PettiforFeedingTimeCurveClass(double a_A, double a_B, double a_C, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name)
	: CurveClass(a_reversecurve, a_MaxX, a_MinX, a_name) {
	m_parameterA = a_A;  // Max feeding time
	m_parameterB = a_B;  // Min feeding time
	m_parameterC = a_C;  // Flock size threshold
	CalculateCurveValues();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PettiforFeedingTimeCurveClass::~PettiforFeedingTimeCurveClass() {
	;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

double PettiforFeedingTimeCurveClass::DoCalc(double a_x) {
	if (a_x >= m_parameterC) return m_parameterA;
	else return m_parameterB + a_x * ((m_parameterA - m_parameterB) / m_parameterC);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------