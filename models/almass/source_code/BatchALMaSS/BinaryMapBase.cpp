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

#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "../ALMaSSDefines.h"
#include "../BatchALMaSS/BinaryMapBase.h"


BinaryMapBase::BinaryMapBase(unsigned int a_width, unsigned int a_height, unsigned int a_resolution, unsigned int a_noValues)
{
	// Assign member variables
	m_width = a_width;
	m_height = a_height;
	m_colourRes = a_noValues;
	m_resolution = a_resolution;
	// 2 = 1, 4 = 2, 8 = 3 etc..
	m_resolutionscaler = 0;
	unsigned int n=m_resolution;
	do {
		n = n >> 1;
		m_resolutionscaler++;
	} while (n>0);
	m_resolutionscaler--;
	m_colourScaler = 0;
	n=m_colourRes;
	do {
		n = n >> 1;
		m_colourScaler++;
	} while (n>0);
	m_colourScaler--;

	// The total number of 64bit integers needed is height x width / 64 (rounded up);
	// But we need to take the resolution into account too - hence:
	// ( (height x width)/(resolution*resolution) ) / 64, then add one in case there is a fractional part
	// Then we also need to have the size of the colour resolution calculated in here.
	m_maplength = ( ( ( m_width * m_height * m_colourScaler ) / ( m_resolution * m_resolution )) / 64 ) + 1;
	// Need to adjust m_width & m_height to match resolution
	m_width = m_width >> m_resolutionscaler;
	m_height = m_height >> m_resolutionscaler;
	m_map = new uint64[m_maplength];

	// create the mask
	m_mask = m_colourRes-1;
	m_maskbits = m_colourScaler -1;
	ClearMap();
}

BinaryMapBase::~BinaryMapBase()
{
	;
}

void BinaryMapBase::ClearMap()
{
	for (unsigned int i = 0; i < m_maplength; i++) m_map[i]=0;
}

uint64 BinaryMapBase::GetValue(unsigned a_x, unsigned a_y)
{
	// To find an x/y co-ordinate:
	// We need to find the 64-bit int that we are in, then the bit or bytes we need
	unsigned int ref = ( ( ( a_y >> m_resolutionscaler ) * m_width ) + ( a_x >> m_resolutionscaler ) ) * m_colourScaler;
	// ref holds the first bit of information we need, i.e. the location within m_map in terms of element number.
	// Next we need to figure out which uint64 this is in.....
	unsigned int index = ref >> 6; // ....so divide by 64
	// get the value from this location to a temporary variable
	uint64 returnvalue = m_map[index];
	// identify the bits we are interested in
	uint64 bits = ( ref & 63 );
	// Our mask is sized based on the colours represented 1 - binary, 2 - 4, 4 - 16 , 8 - 256 (NB only powers of 2 are allowed).
	// The mask is created in the constructor.
	// So we need to shift our return value and mask it.
	returnvalue = ( returnvalue >> bits ) & m_mask;
	return returnvalue;
}

void BinaryMapBase::SetValue(unsigned a_x, unsigned a_y, unsigned a_value)
{
	// To find an x/y co-ordinate:
	// We need to find the 64-bit int that we are in, then the bit or bytes we need
	unsigned int ref = ( ( ( a_y >> m_resolutionscaler ) * m_width ) + ( a_x >> m_resolutionscaler ) ) * m_colourScaler;
	// ref holds the first bit of information we need, i.e. the location within m_map in terms of element number.
	// Next we need to figure out which uint64 this is in.....
	unsigned int index = ref >> 6; // ....so divide by 64
	// get the value from this location to a temporary variable
	uint64 newvalue = m_map[index];
	// identify the bits we are interested in
	uint64 bits = ( ref & 63 );
	// Our mask is sized based on the colours represented 1 - binary, 2 - 4, 4 - 16 , 8 - 256 (NB only powers of 2 are allowed).
	// The mask is created in the constructor.
	// So we need to shift our mask to cover the correct bits.
	uint64 mask = ~( m_mask << bits );
	// Blank out any current values
	newvalue = newvalue & mask;
	// Pop our new value in
	uint64 value = a_value;
	newvalue = newvalue | ( value  << bits);
	m_map[index] = newvalue;
}

/**
* Avoids passing one parameter compared to SetValue(unsigned, unsigned, unsigned)
*/
void BinaryMapBase::ClearValue(unsigned a_x, unsigned a_y)
{
	// To find an x/y co-ordinate:
	// We need to find the 64-bit int that we are in, then the bit or bytes we need
	unsigned int ref = ( ( ( a_y >> m_resolutionscaler ) * m_width ) + ( a_x >> m_resolutionscaler ) ) * m_colourScaler;
	// ref holds the first bit of information we need, i.e. the location within m_map in terms of element number.
	// Next we need to figure out which uint64 this is in.....
	unsigned int index = ref >> 6; // ....so divide by 64
	// get the value from this location to a temporary variable
	uint64 newvalue = m_map[index];
	// identify the bits we are interested in
	uint64 bits = ( ref & 63 );
	// Our mask is sized based on the colours represented 1 - binary, 2 - 4, 4 - 16 , 8 - 256 (NB only powers of 2 are allowed).
	// The mask is created in the constructor.
	// So we need to shift our mask to cover the correct bits.
	uint64 mask = ~( m_mask << bits );
	// Blank out any current values
	newvalue = newvalue & mask;
	m_map[index] = newvalue;
	;
}

