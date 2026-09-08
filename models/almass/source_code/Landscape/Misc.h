#ifndef __Misc_H
#define __Misc_H
/*
*******************************************************************************************************
Copyright (c) 2013, Christopher John Topping, Aarhus University
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

/** \brief ALMaSS_MathFuncs constructor */
class ALMaSS_MathFuncs {
	/**
	* This class is designed to hold general math functions frequently used in the ALMaSS code.
	* For example distance calculations using either Pythagoras or the approximation of Pythagoras
	*/
public:
	/** \brief ALMaSS_MathFuncs constructor */
	ALMaSS_MathFuncs() {
		;
	}
	/** \brief ALMaSS_MathFuncs deconstructor @todo Lars: not sure this is needed here*/
	virtual ~ALMaSS_MathFuncs() {
		;
	};
	/** \brief Calculate distance using Pythagoras */
	int CalcDistPythagoras( int a_x, int a_y, int a_x1, int a_y1 );

	/** \brief Calculate distance using the Pythagoras approximation*/
	int CalcDistPythagorasApprox( int a_x, int a_y, int a_x1, int a_y1 );
};
#endif