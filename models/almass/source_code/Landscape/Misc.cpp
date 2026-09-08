//
// misc.cpp
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

#define _CRT_SECURE_NO_DEPRECATE

#include "string.h"
#include "ls.h"
#include "Map_cfg.h"

extern CfgBool cfg_dumpvegjan;
extern CfgStr cfg_dumpvegjanfile;
extern CfgBool cfg_dumpvegjune;
extern CfgStr cfg_dumpvegjunefile;

//------------------------------------------------------------------------------
/** \brief Calculate distance using Pythagoras */
int ALMaSS_MathFuncs::CalcDistPythagoras( int a_x, int a_y, int a_x1, int a_y1 ) {
	int dx = a_x - a_x1;
	int dy = a_y - a_y1;
	int dist = (int)sqrt( (double)(dx*dx + dy*dy) );
	return dist;
}
/** \brief Calculate distance using the Pythagoras approximation*/
int ALMaSS_MathFuncs::CalcDistPythagorasApprox( int a_x, int a_y, int a_x1, int a_y1 ) {
	/**
	The approximation to Pythagorus is accurate to around 6%
	*/
	int dx = abs( a_x - a_x1 );
	int dy = abs( a_y - a_y1 );
	dx++;  //To avoid division by zero
	dy++;
	int dist;
	if (dx>dy) {
		dist = dx + (dy * dy) / (2 * dx);
	}
	else dist = dy + (dx * dx) / (2 * dy);
	return dist;
}


