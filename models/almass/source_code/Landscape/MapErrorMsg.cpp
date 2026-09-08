//
// maperrormsg.cpp
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

using namespace std;

#include <cstdio>
#include <time.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>
#include "MapErrorMsg.h"

MapErrorMsg * g_msg = NULL;

MapErrorMsg::MapErrorMsg()
{
	m_warnfile = "ErrorFile.txt";
}

void MapErrorMsg::SetFilename(string a_warnfile)
{
	m_warnfile = a_warnfile;
}

std::string MapErrorMsg::GetFilename()
{
	return m_warnfile;
}

void MapErrorMsg::SetWarnLevel( MapErrorState a_level )
{
  if ( a_level > WARN_BUG && a_level <= WARN_ALL )
    m_level = a_level;
  else {
    m_level = WARN_ALL;
    Warn( WARN_BUG, "MapErrorMsg::SetWarnLevel(): Illegal error level!", "");
    exit(1);
  }
}

void MapErrorMsg::Warn(MapErrorState a_level,
	std::string a_msg1,
	std::string a_msg2)
{
	FILE * EFile;
	time_t aclock;
	tm* newtime;
	time(&aclock);   // Get time in seconds.
	newtime = localtime(&aclock);   // Convert time to struct tm form.

	// Print local time as a string
	EFile = fopen(m_warnfile.c_str(), "a+");
	if (!EFile) {
		fprintf(stderr, "MapErrorMsg::Warn(): Unable to open file"
			" for error messages: %s\n", m_warnfile.c_str());
		exit(1);
	}

	fprintf(EFile, "%s Level %d *****\n%s %s\n",
		asctime(newtime), a_level, a_msg1.c_str(), a_msg2.c_str());
	fflush(EFile);
	fclose(EFile);
}

void MapErrorMsg::Warn(MapErrorState a_level, std::string a_msg1, int a_msg2)
{
	FILE * EFile;
	time_t aclock;
	tm* newtime;
	time(&aclock);   // Get time in seconds.
	newtime = localtime(&aclock);   // Convert time to struct tm form.

	// Print local time as a string
	EFile = fopen(m_warnfile.c_str(), "a+");
	if (!EFile) {
		fprintf(stderr, "MapErrorMsg::Warn(): Unable to open file"
			" for error messages: %s\n", m_warnfile.c_str());
		exit(1);
	}

	fprintf(EFile, "%s Level %d *****\n%s %d\n",
		asctime(newtime), a_level, a_msg1.c_str(), a_msg2);
	fflush(EFile);
	fclose(EFile);
}

void MapErrorMsg::Warn(std::string a_msg1, std::string a_msg2)
{
	FILE * EFile;
	time_t aclock;
	tm* newtime;
	time( &aclock );   // Get time in seconds.
	newtime=localtime( &aclock );   // Convert time to struct tm form.
   // Print local time as a string.

   EFile=fopen(m_warnfile.c_str(), "a+" );
	if ( !EFile ) {
    fprintf(stderr, "MapErrorMsg::Warn(): Unable to open file"
	    " for error messages: %s\n", m_warnfile.c_str() );
    exit(1);
  }

  fprintf( EFile, "%s *****\n%s %s\n",
	   asctime(newtime), a_msg1.c_str(), a_msg2.c_str() );
  fflush( EFile );
  fclose( EFile );
}

void MapErrorMsg::Warn(  std::string a_msg1, double a_num )
{
	FILE * EFile;
	time_t aclock;
	tm* newtime;
	time( &aclock );   // Get time in seconds.
	newtime=localtime( &aclock );   // Convert time to struct tm form.
   // Print local time as a string.

   EFile=fopen(m_warnfile.c_str(), "a+" );
	if ( !EFile ) {
    fprintf(stderr, "MapErrorMsg::Warn(): Unable to open file"
	    " for error messages: %s\n", m_warnfile.c_str() );
    exit(1);
  }

  fprintf( EFile, "%s *****\n%s %g\n",
	   asctime(newtime), a_msg1.c_str(), a_num );
  fflush( EFile );
  fclose( EFile );
  exit(0);
}

void MapErrorMsg::WarnAddInfo(MapErrorState a_level, std::string a_add1, std::string a_add2)
{
	FILE * EFile;
	time_t aclock;
	tm* newtime;
	time(&aclock);   // Get time in seconds.
	newtime = localtime(&aclock);   // Convert time to struct tm form.
   // Print local time as a string.

	if (a_level > m_level)	return;
	EFile = fopen(m_warnfile.c_str(), "a+");
	if (!EFile) {
		fprintf(stderr, "MapErrorMsg::Warn(): Unable to open file"
			" for error messages: %s\n", m_warnfile.c_str());
		exit(1);
	}
	fprintf(EFile, "%s *****\t%s%s\n", asctime(newtime), a_add1.c_str(), a_add2.c_str());
	fflush(EFile);
	fclose(EFile);
}

void MapErrorMsg::WarnAddInfo(MapErrorState a_level, std::string a_add1, double a_num)
{
	if (a_level > m_level)	return;
	ofstream *EFile;
	time_t aclock;
	tm* newtime;
	time(&aclock);   // Get time in seconds.
	newtime = localtime(&aclock);   // Convert time to struct tm form.
   // Print local time as a string.

	EFile = new ofstream(m_warnfile.c_str(), ios::app);
	if (!EFile->is_open()) {
		fprintf(stderr, "MapErrorMsg::Warn(): Unable to open file"
			" for error messages: %s\n", m_warnfile.c_str());
		exit(1);
	}
	(*EFile) << asctime(newtime)<<" *****\t"<<a_add1.c_str() << a_num << endl;
	EFile->close();
	delete EFile;
}

MapErrorMsg * CreateErrorMsg()
{
	if (g_msg == NULL)
	{ 
		g_msg = new MapErrorMsg();
	}

	return g_msg;
}


