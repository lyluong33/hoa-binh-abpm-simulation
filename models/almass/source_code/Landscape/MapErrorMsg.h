//
// MapErrorMsg.h
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

#ifndef MAPERRORMSG_H
#define MAPERRORMSG_H

extern class MapErrorMsg *g_msg;

typedef enum {
  WARN_BUG,
  WARN_FATAL,
  WARN_UNDEF,
  WARN_FILE,
  WARN_MSG,
  WARN_TRIVIAL,
  WARN_ALL
} MapErrorState;

class MapErrorMsg
{
public:

	MapErrorState m_level;
	std::string  m_warnfile;

public:
	void   Warn(MapErrorState a_level, std::string a_msg1, std::string a_msg2);
	void   Warn(MapErrorState a_level, std::string a_msg1, int);
	void   Warn(std::string a_msg1, std::string a_msg2);
	void   Warn(std::string a_msg1, double a_num);
	void   WarnAddInfo(MapErrorState a_level, std::string a_add1, std::string a_add2);
	void   WarnAddInfo(MapErrorState a_level, std::string a_add1, double a_num);
	void   SetWarnLevel(MapErrorState a_level);
	void   SetFilename(std::string  a_warnfile);
	std::string GetFilename();
	MapErrorMsg();
};

MapErrorMsg * CreateErrorMsg();

//#define __UNIX__
#ifdef __UNIX__
// #define AnsiString char *
// typedef char * AnsiString
#endif // __UNIX__

#endif // MAPERRORMSG_H

