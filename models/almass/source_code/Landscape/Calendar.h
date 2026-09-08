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


#ifndef CALENDAR_H
#define CALENDAR_H

extern class Calendar *g_date;

struct aDate
{
	int year;
	int month;
	int day;
};

class Calendar
{
public:
	// Reset to default values, January 1st, 0000.
	void  Reset(void);
	void  Reset2(void);

	/** The return value for the next three methods
	indicates wether the global Tick() should be called
	for the whole landscape model. Order matters here, so this cannot just
	be done internally within the Calendar class. */
	bool  TickMinute(void);
	bool  TickMinute10(void);
	// Resets minute counter to zero.
	bool  TickHour(void);

	// Advance date by one day. Resets minute and hour counters to zero.
	void  Tick(void);

	long  Date(void) { return m_date; }
	int  DayInYear(void) { return m_day_in_year; }
	// long  DaysInOldYears( void ) { return m_olddays; }
	long  OldDays(void) { return m_olddays; }
	long  GlobalDate(int a_day, int a_month, int a_year);

	int   DayLength(void) { return m_todayslength; }
	int   SunRiseTime(void) { return m_todayssunrise; }
	int   SunSetTime(void) { return m_todayssunset; }
	int   DayLength(int a_day_in_year);
	int   SunRiseTime(int a_day_in_year);
	int   SunSetTime(int a_day_in_year);
	int   GetFirstYear(void) { return m_firstyear; }
	int   GetLastYear(void) { return m_lastyear; }
	int   GetYear(void) { return m_year; }
	int   GetYearNumber(void) { return m_simulationyear; }
	int   GetMonth(void) { return m_month + 1; }
	int   GetMonthRaw(void) { return m_month; }
	int   GetMinute(void) { return m_minutes; }
	int   GetHour(void) { return m_hours; }
	int   GetDayInMonth(void) { return m_day_in_month; }
	double GetDaylightProportion(void){ return m_daylightproportion; }
    bool  JanFirst( void ) { return m_janfirst; }
    bool  MarchFirst( void ) { return m_marchfirst; }
    void  SetFirstYear( int a_year ) { m_firstyear = a_year; }
    void  SetLastYear( int a_year ) { m_lastyear = a_year; }
    bool  ValidDate( int a_day, int a_month );
	// Check the date with ValidDate() before using DayInYear()
    // (or suffer the consequences).
    int   DayInYear( int a_day, int a_month );

    Calendar( void );
	void CreateDaylength(double latitude, double longitude, int timezone, int year=2021);

  private:
    long m_date;
    long m_olddays;
    int  m_firstyear; // Used for resetting the date.
    int  m_lastyear;
    int  m_day_in_month;
    int  m_day_in_year;
    int  m_month;
    int  m_year;
	int  m_simulationyear;
    int  m_todayslength;
	int m_todayssunrise;
	int m_todayssunset;
	int m_dawn;
	int m_dusk;
	double m_daylightproportion;
    int  m_minutes;
    int  m_hours;
    bool m_janfirst;
    bool m_marchfirst;
    
    int m_daylength[365];
	int m_sunrise[365];
	int m_sunset[365];
}; // class Calendar


Calendar* CreateCalendar();

#endif // CALENDAR_H
