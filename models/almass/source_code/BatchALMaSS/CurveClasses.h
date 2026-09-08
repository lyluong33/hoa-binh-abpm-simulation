//
//CurveClasses.h
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

#ifndef CurveClasses_H
#define CurveClasses_H

using namespace std;

/**
\brief Provides a flexible curve class which provides a quick way to calculate return values for any given x-vale with a resolution of 10,000 units on the x-axis.
*/
class CurveClass
{
public:
	CurveClass(bool a_reversecurve, double a_MaxX,double a_MinX, const char* a_name);
	virtual ~CurveClass();
	virtual double GetY(double a_X);
	virtual void WriteDataFile( int a_step );
protected:
	/** \brief A parameter for a the maximum value of X we consider */
	double m_parameterMaxX;
	/** \brief A parameter for a the minimum value of X we consider */
	double m_parameterMinX;
	/** \brief The size of each step on the X-axis */
	double m_step;
	/** \brief The inverse of m_step */
	double m_step_inv;
	/** \brief The values of y we store for each X */
	double* m_values;
	/** \brief A scaler for the x-values */
	//double m_scaler;
	/** A name to identify the curve */
	string m_name;
	/** \brief If true the values fall from 1 to zero, otherwise its zero to 1 */
	bool m_reversecurve;

	/** \brief fills in the curve for 10,000 values from MinX to MaxX */
	virtual void CalculateCurveValues();
	/** \brief The specific calulation of y for a given x */
	virtual double DoCalc(double /* a_x */) { return 0;  }

};


/** \brief A Gompertz curve class */
class GompertzCurveClass : public CurveClass
{
public:
	GompertzCurveClass(double a_A, double a_B, double a_C, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name);
	virtual ~GompertzCurveClass();
protected:
	/** The Gompertz curve takes three parameters and we also specify a X-axis range */
	/** \brief Parameter A */
	double m_parameterA;
	/** \brief Parameter B */
	double m_parameterB;
	/** \brief Parameter C */
	double m_parameterC;

	/** \brief The specific calulation of y for a given x */
	virtual double DoCalc(double a_x);
};

/** \brief A polynomial curve class */
class Polynomial2CurveClass : public CurveClass
{
public:
	Polynomial2CurveClass(double a_A, double a_B, double a_C, double a_D, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name);
	virtual ~Polynomial2CurveClass();
protected:
	/** The scaled polynomial curve takes four parameters and we also specify a X-axis range. \n
	* \n
	* The form is (ParameterA*x*x + ParameterB*x + ParameterC) * ParameterD\n
	*/
	/** \brief Parameter A */
	double m_parameterA;
	/** \brief Parameter B */
	double m_parameterB;
	/** \brief Parameter C */
	double m_parameterC;
	/** \brief Parameter D */
	double m_parameterD;

	/** \brief The specific calulation of y for a given x */
	virtual double DoCalc(double a_x);
};

/** \brief A rectangular threshold curve class */
class ThresholdCurveClass : public CurveClass
{
public:
	ThresholdCurveClass(double a_A, double a_B, double a_C, double a_D, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name);
	virtual ~ThresholdCurveClass();
protected:
	/** The threshold curve takes four parameters and we also specify a X-axis range. \n
	* \n
	* The form is if xmax <= x >= xmin y = a else y = b
	*/
	/** \brief Parameter A is the return if between xmin & xmax */
	double m_parameterA;
	/** \brief Parameter B is the return if not between xmin & xmax*/
	double m_parameterB;
	/** \brief Parameter C is xmin*/
	double m_parameterC;
	/** \brief Parameter D is the xmax*/
	double m_parameterD;

	/** \brief The specific calulation of y for a given x */
	virtual double DoCalc(double a_x);
};
/** \brief A Hollings Disc curve class - type II functional response */
class HollingsDiscCurveClass : public CurveClass {
public:
	HollingsDiscCurveClass( double a_A, double a_B, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name );
	virtual ~HollingsDiscCurveClass();
protected:
	/** Hollings disc curve takes two parameters */
	/** \brief Parameter A */
	double m_parameterA;
	/** \brief Parameter B */
	double m_parameterB;
	/** \brief The specific calulation of y for a given x */
	virtual double DoCalc( double a_x );
};
/** \brief A curve for feeding time derived from Pettifor et al (2000). Journal of Applied Ecology 37: 103-135.
*/
class PettiforFeedingTimeCurveClass : public CurveClass {
public:
	PettiforFeedingTimeCurveClass(double a_A, double a_B, double a_C, bool a_reversecurve, double a_MaxX, double a_MinX, const char* a_name);
	virtual ~PettiforFeedingTimeCurveClass();
protected:
	/** \brief Parameter A is the maximum feeding time */
	double m_parameterA;
	/** \brief Parameter B is the minimum feeding time*/
	double m_parameterB;
	/** \brief Parameter C is the threshold floksize above which feeding time remains at maximum.*/
	double m_parameterC;
	/** \brief The specific calulation of y for a given x */
	virtual double DoCalc(double a_x);
};

#endif