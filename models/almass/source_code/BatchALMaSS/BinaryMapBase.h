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

#ifndef __BINARYMAPBASE_H
#define __BINARYMAPBASE_H

class BinaryMapBase
{
public:
	BinaryMapBase(unsigned int a_width, unsigned int a_height, unsigned int a_resolution, unsigned int a_noValues);
	~BinaryMapBase();
	uint64 GetValue(unsigned a_x, unsigned a_y);
	void SetValue(unsigned a_x, unsigned a_y, unsigned a_value);
	void ClearValue(unsigned a_x, unsigned a_y);
	void ClearMap();
protected:
	uint64* m_map ;
	unsigned int m_height;
	unsigned int m_width;
	unsigned int m_colourRes;
	unsigned int m_colourScaler;
	unsigned int m_resolution;
	unsigned int m_resolutionscaler;
	unsigned int m_maskbits;
	unsigned int m_maplength;
	uint64 m_mask;
};

#endif
