//
// weather.cpp
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

#include <cstdlib>
#include <cstdio>

#include "ls.h"

//extern void FloatToDouble(double&,float);

// First year of weather data used from datafile. Ignored if 0.
static CfgInt l_weather_starting_year("WEATHER_STARTING_YEAR",
				      CFG_CUSTOM, 0);
static CfgStr l_map_weather_file("MAP_WEATHER_FILE", CFG_CUSTOM, "weather.pre");
static CfgBool l_weather_minmaxtemp("WEATHER_MINMAXTEMP", CFG_CUSTOM, false);
static CfgBool l_weather_radiation("WEATHER_RADIATION", CFG_CUSTOM, false);
static CfgBool l_weather_soiltemp("WEATHER_SOILTEMP", CFG_CUSTOM, false);
static CfgBool l_weather_snowcover("WEATHER_SNOWCOVER", CFG_CUSTOM, false);
static CfgBool l_weather_relhumidity("WEATHER_RELHUMIDITY", CFG_CUSTOM, false);
static CfgBool l_weather_soiltemptwilight("WEATHER_SOILTEMPTWILIGHT", CFG_CUSTOM, false);
static CfgBool l_weather_flyinghours("WEATHER_FLYINGHOURS", CFG_CUSTOM, false);

class Weather     *g_weather;

const int WindDirections[12][100] = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

void Weather::Tick( void ) {
	/** The complicated calculation of m_weather_day is due to the potential to run off either end of the rain data, especially since this is called on Day0 */
	m_weather_day = (g_date->Date() + m_datemodulus) % m_datemodulus;

	//update index for today's index
	m_hourly_today_index = m_weather_day;
	
	if (m_snow[m_weather_day] > 0.0) {
		m_snowtoday = true; // Means snow cover not new snow
	}
	else {
		m_snowtoday = false; 
	}

	
	m_temptoday = m_temp[ m_weather_day ];
	m_raintoday = m_rain[ m_weather_day ];
	m_windtoday = m_wind[ m_weather_day ];
	m_soiltemptoday = m_soiltemp[m_weather_day];
	if(m_fminmaxtemp && m_weather_day - 1 < 0){
		m_mintempyesterday = m_mintemp [m_weather_day];
		m_maxtempyesterday = m_maxtemp [m_weather_day];
	}else{
		m_mintempyesterday = m_mintemptoday;
		m_maxtempyesterday = m_maxtemptoday;
	}

	if (m_fminmaxtemp){
		m_mintemptomorrow = m_mintemp [(g_date->Date() + 1 + m_datemodulus) % m_datemodulus];
		m_maxtemptomorrow = m_maxtemp [(g_date->Date() + 1 + m_datemodulus) % m_datemodulus];
	}else{
		m_mintemptomorrow = m_temp[ (g_date->Date() + 1 + m_datemodulus) % m_datemodulus] - m_temp_variation;
		m_maxtemptomorrow = m_temp[ (g_date->Date() + 1 + m_datemodulus) % m_datemodulus] + m_temp_variation;
	}

    if (m_fminmaxtemp){
        m_mintemptoday = m_mintemp [m_weather_day];
        m_maxtemptoday = m_maxtemp [m_weather_day];
    }
    else{
        m_mintemptoday = m_temptoday - m_temp_variation;
        m_maxtemptoday = m_temptoday + m_temp_variation;
    }
	if (m_fradiation) {
		m_radiationtoday = m_radiation[m_weather_day];
	}
	if (m_fflyinghours) {
		m_flyinghourstoday = m_flyinghours[m_weather_day];
	}
	if (m_raintoday > 0.1)
		m_rainingtoday = true;
	else
		m_rainingtoday = false;
	
	if (m_hourly_flag){ //new hourly weather, no need to calculate humidity and wind direction
		m_humiditytoday = m_relhumidity [m_weather_day];
		m_WindDirTodayRadians = m_WindDirRadians [m_weather_day];
		m_winddirtoday = m_winddir [m_weather_day];
		m_winddir8today = m_winddir8 [m_weather_day];
		m_insolation = m_radiation [m_weather_day];
	}

	else //old daily weather 
	{/**
	* Humidity calculation. This is based on an assumption that humidity is relative to rainfall in the days before.
	* The humidity score is the mean of rainfall over the last 5 days assuming 50% loss each day. Therefore rainfall of 10,0,0,2,0 mm in the previous 5 days would
	* give a humidity of 0.625+0+0+1+0 = 1.625/5 = 0.325
	* But we also need to calculate temperature into this. Lets assume <10 degrees has no effect, 10-20 and this figure is halved, 20-40 and it is half again.
	*/
	m_insolation = c_insolation[ g_date->DayInYear() ];
	double rainscale = 0;
	for (int i = 0; i < 5; i++) {
		/** datemodulus is added to prevent negative overun on the rain or temperature data array, important since this is called on Day0 */
		rainscale += GetRain( (m_weather_day + m_datemodulus) - (i + 1) ) * pow( 0.5, i );
	}
	double temp = GetTempPeriod( m_weather_day + m_datemodulus - 1, 5 ) / 5.0;
	if (temp > 10.0) rainscale *= 0.5;
	if (temp > 20.0) rainscale *= 0.5;
	if (temp > 30.0) rainscale *= 0.5;
	m_humiditytoday = rainscale;
	/** Wind directions is based on probabilities. Can also use the #WindDirections array which allows monthly probabilities */
	double chance = g_rand_uni_fnc();
	if (chance < 0.1) m_winddirtoday = 2; // South
	else if (chance < 0.3) m_winddirtoday = 1; // East
	else if (chance < 0.5) m_winddirtoday = 0; // North
	else m_winddirtoday = 3; // West
	

	if (chance < 0.12) m_winddir8today = 0; // North
	else if (chance < 0.24) m_winddir8today = 1; // NE
	else if (chance < 0.35) m_winddir8today = 2; // E
	else if (chance < 0.42) m_winddir8today = 3; // SE
	else if (chance < 0.48) m_winddir8today = 4; // S
	else if (chance < 0.57) m_winddir8today = 5; // SW
	else if (chance < 0.86) m_winddir8today = 6; // W
	else m_winddir8today = 7; // NW
	}



}



double Weather::GetMeanTemp( long a_date, unsigned int a_period )
{
  double sum = 0.0;

  for ( unsigned int i=0; i<a_period; i++ )
    sum += GetTemp( a_date - i );

  return sum/(double)a_period;
}



bool  Weather::GetSnow( long a_date )
{
  if ( a_date == g_date->Date() ) {
    return m_snowtoday;
  }

  int m_weather_day = a_date % m_datemodulus;
  bool snow = false;

  if ( m_temp[ m_weather_day ] < 0 &&
       m_rain[ m_weather_day ] > 1 &&
       g_random_fnc(100) < 50 ) {
    snow = true;
  }

  //  if ( ((dayinyear<90) || (dayinyear>330)) && (random(4)==1) )
  //  snow = true;

  return snow;
}



double Weather::GetDDDegs( long a_date )
{
  double temp = m_temp[ a_date%m_datemodulus ];
  if ( temp < 0.0 ) {
    temp = 0.0;
  }

  return temp;
}



Weather::Weather( const char* a_weatherdatafile )
{
  int NoDays;
  CfgFloat l_weather_temp_variation("WEATHER_TEMP_VARIATION", CFG_CUSTOM, 7);
  m_temp_variation = l_weather_temp_variation.value();
  ifstream inFile;
  std::string filename;
  
  if (strcmp(a_weatherdatafile, "default") == 0)
	  filename = l_map_weather_file.value();
  else
	  filename = a_weatherdatafile;

  std::cout << "Reading Weather File " << filename << "\n";
  inFile.open(filename);
  if (!inFile) {
    g_msg->Warn(WARN_FILE, "Weather::Weather(): Unable to open file",
		filename );
    exit(1);
  }

  string buf;
  std::getline(inFile,buf);
  try 
  {
	  NoDays = std::stoi(buf);
  }
  catch (const std::invalid_argument& ia) {
	  // cannot conver to int, means I am reading the header line
	  std::getline(inFile, buf);
	  try
	  {
		  NoDays = std::stoi(buf);
	  }
	  catch (const std::invalid_argument& ia) {
		  g_msg->Warn(WARN_FILE, "Weather::Weather(): File format error, expected (optional: header line and) the total number of file entries.","");
		  exit(1);
	  }
  }

  if (NoDays == 2) { // 2 means weather file version 2 which is hourly data


	  //the hourly weather data file, get the number of data point
	  int NoHours;
	  std::getline(inFile, buf);
	  NoHours = std::stoi(buf);
	  if(NoHours <0){
		  g_msg->Warn(WARN_FILE, "Weather::Weather(): File format error, expected total number of file entries, got ",
		  NoHours);
	  	  exit(1);
	  }
	  NoDays = NoHours/24; // We have 24 hours for each day.
	  std::getline(inFile, buf); //read the header line and throw them away
	  readWeatherFileHourly(NoHours, NoDays, inFile);
  }

  else{
	  readWeatherFile(NoDays, inFile);
	  m_NoDays = NoDays;
  }  
  
  Tick();
}

void Weather::readWeatherFileHourly(int NoHours, int NoDays, ifstream& inFile){
  m_hourly_flag = true;
  int  Hour, Day, Month, Year, FirstYear=0, LastYear=0;
  double Temp, Rain, Wind, Winddir, Radiation, SoilTemp, SnowCover, minTemp, maxTemp, soilTempTwilight, relHumidity;

  // these are used to calculate the average value (except the rain) for a day
  double sum_temp = 0.0, sum_rain = 0.0, sum_wind = 0.0, sum_winddir = 0.0, sum_winddir8 = 0.0, sum_soiltemp = 0.0, sum_snow = 0.0, sum_humidity = 0.0, sum_radiation = 0.0;
  m_rain.resize(NoDays);
  m_wind.resize(NoDays);
  m_winddir.resize(NoDays);
  m_winddir8.resize(NoDays);
  m_WindDirRadians.resize(NoDays);
  m_temp.resize(NoDays);
  m_soiltemp.resize(NoDays);
  m_snow.resize(NoDays);
  m_mintemp.resize(NoDays);
  m_maxtemp.resize(NoDays);
  m_relhumidity.resize(NoDays);
  m_soiltemptwilight.resize(NoDays);
  m_radiation.resize(NoDays);


  //initialise the vectors for storing the hourly weather data
  m_temp_h.resize(NoDays);
  m_rain_h.resize(NoDays);
  m_wind_h.resize(NoDays);
  m_winddir_h.resize(NoDays);
  m_winddir8_h.resize(NoDays);
  m_radiation_h.resize(NoDays);
  m_soiltemp_h.resize(NoDays);
  m_snow_h.resize(NoDays);
  m_humidity_h.resize(NoDays);

  for(int i = 0; i < NoDays; i++){
	  m_temp_h.at(i).resize(24);
	  m_rain_h.at(i).resize(24);
	  m_wind_h.at(i).resize(24);
	  m_winddir_h.at(i).resize(24);
	  m_winddir8_h.at(i).resize(24);
	  m_radiation_h.at(i).resize(24);
	  m_soiltemp_h.at(i).resize(24);
  	  m_snow_h.at(i).resize(24);
      m_humidity_h.at(i).resize(24);
  }

  m_fsoiltemp = true;
  m_fsnow = true;
  m_frelhumidity = true;
  m_fsoiltemptwilight = true;
  m_fminmaxtemp = true;


  bool storing = false; // Whether we are storing weather data.
  unsigned int index = 0;

  g_date->SetLastYear( 0 );

  int hour_counter = -1;


	
  for ( int i=0; i<NoHours; i++) {
      inFile >> Year >> Month >> Day >> Hour >> Temp >> Wind >> Winddir >>  Rain >> SoilTemp >> SnowCover >> relHumidity  >> Radiation;


    if ( Month == 2 && Day == 29 ) {
      // Skip leap days.
      continue;
    }

    if ( Month == 1 && Day == 1 && Hour == 0 && !storing &&
	 (
	  l_weather_starting_year.value() == 0 ||
	  l_weather_starting_year.value() == Year
	  )
	 ) {
      // Commence storing of data from Jan. 1st of the first
      // year requested.
      storing = true;
      g_date->SetFirstYear( Year );
      FirstYear = Year;
      g_date->Reset();
    }


    if ( storing ) 
	{
      hour_counter += 1;
	  if (hour_counter == 24)
	  {
	    hour_counter = 0;
	  }

	  if (hour_counter == 0){
		  sum_rain = 0.0;
		  sum_temp = 0.0;
		  sum_radiation = 0.0;
		  sum_wind = 0.0;
		  sum_winddir = 0.0;

		  sum_soiltemp = 0.0;
		  sum_snow = 0.0;
		  sum_humidity = 0.0;
		  maxTemp = -9999;
		  minTemp = 9999;
	  }

	
	  m_rain_h.at(index).at(hour_counter) = Rain;
	  sum_rain += Rain;
	  m_wind_h.at(index).at(hour_counter) = Wind;
	  sum_wind += Wind;
	  m_winddir_h.at(index).at(hour_counter) = Winddir;
	  sum_winddir += Winddir;

	  m_temp_h.at(index).at(hour_counter) = Temp;
	  sum_temp += Temp;
	  if(Temp > maxTemp) maxTemp = Temp;
	  if(Temp < minTemp) minTemp = Temp;
	  m_radiation_h.at(index).at(hour_counter) = Radiation;
	  sum_radiation += Radiation*3600; /**\note cover it to J for daily */
	  m_soiltemp_h.at(index).at(hour_counter) = SoilTemp;
	  sum_soiltemp += SoilTemp;
	  /**
		 * @todo we need to consider what to use for soil temperature for twilight, for now it is the soil temperature at 5am.
		 * 
		 */
	  if(hour_counter == 5){
		m_soiltemptwilight[index] = SoilTemp;
	  }

	  m_snow_h.at(index).at(hour_counter) = SnowCover;
	  sum_snow += SnowCover;
	  m_humidity_h.at(index).at(hour_counter) = relHumidity;
	  sum_humidity += relHumidity;


	//process the daily data
	  if(hour_counter == 23)
	  {
		m_rain[ index ] = sum_rain;
		
		m_wind[ index ] = sum_wind/24.0;
		/**
		 * @todo we need to consider what to use for wind direction, for now it is the averay in 24 hours.
		 * 
		 */
		m_WindDirRadians[index] = sum_winddir/24.0;
		if (m_WindDirRadians[index]>=M_PI*5/4 && m_WindDirRadians[index]<M_PI*7/4) m_winddir[index] = 2; // South
		else if (m_WindDirRadians[index]>=M_PI*3/4 && m_WindDirRadians[index]<M_PI*5/4) m_winddir[index] = 3; // West
		else if (m_WindDirRadians[index]>=M_PI/4 && m_WindDirRadians[index]<M_PI*3/4) m_winddir[index] = 0; // North
		else m_winddir[index] = 1; // east

		if (m_WindDirRadians[index]>=M_PI*13/8 && m_WindDirRadians[index]<M_PI*15/8) m_winddir8[index] = 3; // South East
		else if (m_WindDirRadians[index]>=M_PI*11/8 && m_WindDirRadians[index]<M_PI*13/8) m_winddir8[index] = 4; // South
		else if (m_WindDirRadians[index]>=M_PI*9/8 && m_WindDirRadians[index]<M_PI*11/8) m_winddir8[index] = 5; // South west
		else if (m_WindDirRadians[index]>=M_PI*7/8 && m_WindDirRadians[index]<M_PI*9/8) m_winddir8[index] = 6; // West
		else if (m_WindDirRadians[index]>=M_PI*5/8 && m_WindDirRadians[index]<M_PI*7/8) m_winddir8[index] = 7; // North west
		else if (m_WindDirRadians[index]>=M_PI*3/8 && m_WindDirRadians[index]<M_PI*5/8) m_winddir8[index] = 0; // North
		else if (m_WindDirRadians[index]>=M_PI*1/8 && m_WindDirRadians[index]<M_PI*3/8) m_winddir8[index] = 1; // North east
		else m_winddir8[index] = 2; // east


		


		m_temp[ index ] = sum_temp/24.0;
		m_soiltemp[index] = sum_soiltemp/24.0; 
		m_radiation[index] = sum_radiation/1000000;
		/**
		 * @todo we need to consider what to use for snow depth, for now it is the averay in 24 hours.
		 * 
		 */
		m_snow[index] = sum_snow/24.0; 
		m_mintemp[index] = minTemp;
		m_maxtemp[index] = maxTemp;
		m_relhumidity[index] = sum_humidity/24.0;


		index++;
		if ( (Month == 12) && (Day == 31) && (storing) ) {
        	cout << "LastYear: " << Year << "\n";
      		// Found yet another full year worth of weather data.
      		g_date->SetLastYear( Year );
      		LastYear = Year;
    	} 
	  }
    }

    //cout << "i: " << i << " index: " << index << " Month: " << Month << " Year: " << Year <<"\n";

  }

  // Did we find at least one full year worth of data?
  if ( g_date->GetLastYear() == 0 ) {
    // Nope...
    g_msg->Warn(WARN_FILE, "Weather::Weather(): Weather data file did",
		"not contain at least one year of data!" );
    exit(1);
  }
  //fclose( ifile );
  m_datemodulus = (LastYear - FirstYear + 1)*365;
  m_snowdepth = 0;
}

void Weather::readWeatherFile(int NoDays, ifstream& inFile){
  m_hourly_flag = false;
  int  Day, Month, Year, FirstYear=0, LastYear=0;
  double Temp, Rain, Wind, SoilTemp, SnowCover, minTemp, maxTemp, soilTempTwilight, relHumidity, Radiation, FlyingHours;
  m_rain.resize(NoDays);
  m_wind.resize(NoDays);
  m_temp.resize(NoDays);
  m_soiltemp.resize(NoDays); // when absent, this is calculated 
  m_snow.resize(NoDays); // when absent, this is calculated 
  if (l_weather_soiltemp.value() == true)
  {
	   m_fsoiltemp = true;
  }
  if (l_weather_snowcover.value() == true)
  {
	  m_fsnow = true;
  }
  if (l_weather_relhumidity.value() == true)
  {
	  m_relhumidity.resize(NoDays);
	  m_frelhumidity = true;
  }
  if (l_weather_soiltemptwilight.value() == true)
  {
	  m_soiltemptwilight.resize(NoDays);
	  m_fsoiltemptwilight = true;
  }
  if (l_weather_minmaxtemp.value() == true)
  {
	  m_mintemp.resize(NoDays);
	  m_maxtemp.resize(NoDays);
	  m_fminmaxtemp = true;
  }
  if (l_weather_radiation.value() == true)
  {
	  m_radiation.resize(NoDays);
	  m_fradiation = true;
  }
  if (l_weather_flyinghours.value() == true)
  {
	  m_flyinghours.resize(NoDays);
	  m_fflyinghours = true;
  }
  

  bool storing = false; // Whether we are storing weather data.
  unsigned int index = 0;

  g_date->SetLastYear( 0 );


  for ( int i=0; i<NoDays; i++) {

	  // Year	Month	Day	 AirTemp  Wind	Rain	[MinAirTemp MaxAirTemp]	[SoilTemp]	[SnowCover]	[RelHumidity]	[SoilTempTwilight] [Radiation]

      inFile >> Year >> Month >> Day >> Temp >> Wind >> Rain;
	  if ( m_fminmaxtemp == true )
		  inFile >> minTemp >> maxTemp;
	  if ( m_fsoiltemp == true )
		  inFile >> SoilTemp;
	  if ( m_fsnow == true )
		  inFile >> SnowCover;
	  if ( m_frelhumidity == true )
		  inFile >> relHumidity;
	  if ( m_fsoiltemptwilight == true )
		  inFile >> soilTempTwilight;
	  if (m_fradiation == true)
		  inFile >> Radiation;
	  if (m_fflyinghours == true)
		  inFile >> FlyingHours;

    if ( Month == 2 && Day == 29 ) {
      // Skip leap days.
      continue;
    }

    if ( Month == 1 && Day == 1 && !storing &&
	 (
	  l_weather_starting_year.value() == 0 ||
	  l_weather_starting_year.value() == Year
	  )
	 ) {
      // Commence storing of data from Jan. 1st of the first
      // year requested.
      storing = true;
      g_date->SetFirstYear( Year );
      FirstYear = Year;
      g_date->Reset();
    }


    if ( storing ) 
	{
      
	  m_rain[ index ] = Rain;
      
	  m_wind[ index ] = Wind;

	  m_temp[ index ] = Temp;

	  if (m_fsoiltemp == true)
	  {
		  m_soiltemp[index] = SoilTemp; 
	  }
	  else 
	  {
		  m_soiltemp[index] = DeriveSoilTemp(Temp);
	  }

	  double snowdepth = 0;
	  if (index > 0) 
	  {
		  snowdepth = m_snow[index - 1];
	  }
	  if (m_fsnow == true)
	  {
		  m_snow[index] = SnowCover;
	  }
	  else
	  {
		  m_snow[index] = DeriveSnowCover(snowdepth, Rain, Temp);
	  }

	  if (m_fminmaxtemp == true)
	  {
		  m_mintemp[index] = minTemp;
		  m_maxtemp[index] = maxTemp;
	  }

	  if (m_frelhumidity == true)
	  {
		  m_relhumidity[index] = relHumidity;
	  }

	  if (m_fsoiltemptwilight == true)
	  {
		  m_soiltemptwilight[index] = soilTempTwilight;
	  }
	  if (m_fradiation == true)
	  {
		  m_radiation[index] = Radiation;
	  }
	  if (m_fflyinghours == true)
	  {
		  m_flyinghours[index] = FlyingHours;
	  }
	  index++;
    }

    //cout << "i: " << i << " index: " << index << " Month: " << Month << " Year: " << Year <<"\n";

    if ( (Month == 12) && (Day == 31) && (storing) ) {
        cout << "LastYear: " << Year << "\n";
      // Found yet another full year worth of weather data.
      g_date->SetLastYear( Year );
      LastYear = Year;
    }
  }

  // Did we find at least one full year worth of data?
  if ( g_date->GetLastYear() == 0 ) {
    // Nope...
    g_msg->Warn(WARN_FILE, "Weather::Weather(): Weather data file did",
		"not contain at least one year of data!" );
    exit(1);
  }
  //fclose( ifile );
  m_datemodulus = (LastYear - FirstYear + 1)*365;
  m_snowdepth = 0;
}

Weather::~Weather( void )
{
}

double Weather::DeriveSoilTemp(double a_airtemp)
{
	return a_airtemp - 0; // ***ELA*** here we need to add the conversion equation using config variables
}

double Weather::DeriveSnowCover(double a_snowdepth, double a_rainfall, double a_temperature)
{
	double snowtempthreshold = -1.0;
	if (a_snowdepth > 0.0 && a_temperature < snowtempthreshold) {
		a_snowdepth -= 0.1;  // We decay the snow depth by 0.1 cm per day when temperatures are sub zero
	}
	else if (a_snowdepth > 0.0) // If temperatures are above 0.0, we decay snow 1 cm per degree C
	{
		a_snowdepth -= a_temperature;
	}
	if (a_temperature < snowtempthreshold && a_rainfall > 0.0) {
		a_snowdepth += a_rainfall;  // rain is in mm and snow in cm, so we get the conversion for free.
	}
	if (a_temperature > snowtempthreshold && a_rainfall > 0.0) {
		a_snowdepth -= a_rainfall;  // rain is in mm and snow in cm, so we get the conversion for free.
	}
	if (a_snowdepth < 0.0) a_snowdepth = 0.0;
	return a_snowdepth;
}

double Weather::GetRainPeriod( long a_date, unsigned int a_period )
{
  double sum = 0.0;

  for ( unsigned int i=0; i<a_period; i++ )
    sum += GetRain( a_date - i );
  return sum;
}

double Weather::GetWindPeriod( long a_date, unsigned int a_period )
{
  double sum = 0.0;

  for ( unsigned int i=0; i<a_period; i++ )
    sum += GetWind( a_date - i );

  return sum;
}

double Weather::GetTempPeriod( long a_date, unsigned int a_period )
{
	/**
	* Sums the temperature for the period from a_date back a_period days. 
	* \param [in] a_date the day to start summing degrees
	* \param [in] a_period the number of days períod to sum
	* \return The sum of day degrees
	*/
  double sum = 0.0;

  for ( unsigned int i=0; i<a_period; i++ )
    sum += GetTemp( a_date - i );

  return sum;
}

Weather* CreateWeather()
{
	if (g_weather == NULL)
	{
		g_weather = new Weather();
	}

	return g_weather;
}
