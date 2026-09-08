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

#include <stdlib.h>
#include <cstdlib>


#include <string>
#include "MapErrorMsg.h"
#include "Calendar.h"

#include "sunset.h"

using namespace std;

class Calendar * g_date = nullptr;

void Calendar::Reset(void)
{
    m_date = 0;
    m_olddays = 0;
    m_day_in_month = 0;
    m_minutes = 0;
    m_hours = 0;
    m_day_in_year = -1; // [0..364]
    m_month = 0; // [0..11]
    m_year = m_firstyear;
    m_simulationyear = 0;
    m_janfirst = true;
    m_marchfirst = false;
}

void Calendar::Reset2(void)
{
    m_day_in_month = 0;
    m_date = -1;
    m_day_in_year = -1; // [0..364]
}

Calendar::Calendar( void )
{
  Reset();
}

void Calendar::CreateDaylength( double latitude, double longitude, int timezone, int year)
{
    SunSet sun;

    sun.setPosition(latitude, longitude, timezone); 
    
    const int m_monthlength[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    float sr, ss;
    int diy = 0;  // day in year 

    for (int i = 1; i <= 12; i++) // months
        for (int j = 1; j <= m_monthlength[i - 1]; j++) // day in month 
        {
            sun.setCurrentDate(year, i, j);
            sr = sun.calcSunrise();
            ss = sun.calcSunset();
            m_sunrise[diy] = sr;
            m_sunset[diy] = ss;
            m_daylength[diy] = int(ss - sr);
            diy++;
        }

}

long Calendar::GlobalDate( int a_day, int a_month, int a_year )
{
  return m_olddays + DayInYear( a_day, a_month) + a_year*365;
}


int Calendar::DayLength( int a_day_in_year )
{
  if ( a_day_in_year<0 || a_day_in_year>364 ) {
    g_msg->Warn( WARN_BUG, "Calendar::DayLength(): Day outside a year!",
	  "" );
    exit(1);
  }
  return m_daylength[ a_day_in_year ];
}

int Calendar::SunRiseTime(int a_day_in_year)
{
    if (a_day_in_year < 0 || a_day_in_year>364) {
        g_msg->Warn(WARN_BUG, "Calendar::DayLength(): Day outside a year!",
            "");
        exit(1);
    }
    return m_sunrise[a_day_in_year];
}

int Calendar::SunSetTime(int a_day_in_year)
{
    if (a_day_in_year < 0 || a_day_in_year>364) {
        g_msg->Warn(WARN_BUG, "Calendar::DayLength(): Day outside a year!",
            "");
        exit(1);
    }
    return m_sunset[a_day_in_year];
}


bool Calendar::ValidDate( int a_day, int a_month )
{
  const int m_monthlength[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if ( a_month < 1  ||
       a_month > 12 ||
       a_day < 1    ||
       a_day > m_monthlength[ a_month - 1 ] ) {
    return false;
  } else {
    return true;
  }
}



int Calendar::DayInYear( int a_day, int a_month )
{
  const int m_monthsum[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

  return m_monthsum[ a_month - 1 ] + a_day - 1;
}



bool Calendar::TickMinute(void)
{
	bool dotick = false;

	if (m_minutes++ > 59) {
		m_minutes = 0;
		dotick = true;
	}
	return dotick;
}

bool Calendar::TickMinute10(void)
{
	bool dotick = false;
	m_minutes += 10;
	if (m_minutes > 59) {
		m_minutes = 0;
		dotick = true;
	}
	return dotick;
}

bool Calendar::TickHour(void)
{
	bool dotick = false;

  if ( ++m_hours > 23 ) {
    m_minutes = 0;
    m_hours   = 0;
  	dotick = true;
  }
  return dotick;
}


void Calendar::Tick( void )
{
  const int m_monthlength[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  const int m_maxmonth = 11;

  m_minutes    = 0;
  m_hours      = 0;
  m_date++;
  m_day_in_year++;
  m_day_in_month++;
  m_janfirst   = false;
  m_marchfirst = false;

  if ( m_day_in_month > m_monthlength[ m_month ])
  {
    m_day_in_month = 1;
    m_month++;
    if ( m_month == 2 )
      // March 1st.
      m_marchfirst = true;
  }

  if ( m_month > m_maxmonth )
  {
    m_month = 0;
    m_year++;
	m_simulationyear++;
	m_olddays += 365;
    m_janfirst = true;
    m_day_in_year = 0;
  }
  m_todayslength = m_daylength[ m_day_in_year ];
  m_todayssunrise = m_sunrise[ m_day_in_year ];
  m_todayssunset = m_sunset[ m_day_in_year ];
  m_daylightproportion = m_daylength[m_day_in_year] / 1440.0;
}


Calendar* CreateCalendar()
{
    if (g_date == NULL)
    {
        g_date = new Calendar();
    }

    return g_date;
}
