//
// weather.h
//


#ifndef WEATHER_H
#define WEATHER_H

#include <vector>
using namespace std;

extern class Weather *g_weather;

// Below is the data for insolation in MJ per m2 for �dum fitted to 1961-1990
// by Olesen (1991)
const double c_insolation [365] = {
  0.91,
  0.921585,
  0.935138,
  0.950669,
  0.968188,
  0.987706,
  1.009231,
  1.032774,
  1.058346,
  1.085956,
  1.115614,
  1.14733,
  1.181113,
  1.216974,
  1.254921,
  1.294964,
  1.337112,
  1.381373,
  1.427757,
  1.476271,
  1.526923,
  1.57972,
  1.634671,
  1.691781,
  1.751057,
  1.812504,
  1.876128,
  1.941934,
  2.009926,
  2.080107,
  2.15248,
  2.227048,
  2.303812,
  2.382774,
  2.463933,
  2.547289,
  2.63284,
  2.720585,
  2.81052,
  2.902642,
  2.996945,
  3.093425,
  3.192073,
  3.292884,
  3.395847,
  3.500954,
  3.608194,
  3.717555,
  3.829024,
  3.942587,
  4.05823,
  4.175935,
  4.295687,
  4.417465,
  4.541252,
  4.667024,
  4.794762,
  4.924441,
  5.056036,
  5.189522,
  5.324873,
  5.462059,
  5.601051,
  5.741819,
  5.884329,
  6.02855,
  6.174446,
  6.321981,
  6.471118,
  6.621819,
  6.774044,
  6.927753,
  7.082902,
  7.239449,
  7.397349,
  7.556555,
  7.717022,
  7.8787,
  8.041541,
  8.205493,
  8.370505,
  8.536524,
  8.703496,
  8.871365,
  9.040077,
  9.209573,
  9.379796,
  9.550686,
  9.722183,
  9.894227,
  10.06675,
  10.2397,
  10.41301,
  10.58661,
  10.76044,
  10.93443,
  11.10852,
  11.28264,
  11.45671,
  11.63068,
  11.80448,
  11.97802,
  12.15125,
  12.3241,
  12.49648,
  12.66834,
  12.8396,
  13.01019,
  13.18004,
  13.34907,
  13.51721,
  13.6844,
  13.85056,
  14.01561,
  14.17949,
  14.34212,
  14.50344,
  14.66337,
  14.82184,
  14.97877,
  15.13411,
  15.28778,
  15.43971,
  15.58983,
  15.73807,
  15.88437,
  16.02866,
  16.17087,
  16.31093,
  16.44879,
  16.58439,
  16.71764,
  16.8485,
  16.97691,
  17.1028,
  17.22612,
  17.3468,
  17.46479,
  17.58005,
  17.6925,
  17.80211,
  17.90881,
  18.01256,
  18.11332,
  18.21102,
  18.30564,
  18.39712,
  18.48542,
  18.5705,
  18.65233,
  18.73086,
  18.80605,
  18.87788,
  18.94632,
  19.01132,
  19.07286,
  19.13092,
  19.18546,
  19.23647,
  19.28393,
  19.3278,
  19.36809,
  19.40475,
  19.4378,
  19.4672,
  19.49296,
  19.51506,
  19.53349,
  19.54825,
  19.55934,
  19.56676,
  19.5705,
  19.57058,
  19.56698,
  19.55973,
  19.54883,
  19.53428,
  19.51611,
  19.49432,
  19.46893,
  19.43996,
  19.40743,
  19.37136,
  19.33177,
  19.28868,
  19.24213,
  19.19214,
  19.13873,
  19.08195,
  19.02183,
  18.95839,
  18.89168,
  18.82173,
  18.74858,
  18.67228,
  18.59286,
  18.51036,
  18.42484,
  18.33634,
  18.2449,
  18.15058,
  18.05342,
  17.95348,
  17.8508,
  17.74545,
  17.63746,
  17.52691,
  17.41384,
  17.29832,
  17.18039,
  17.06012,
  16.93757,
  16.8128,
  16.68586,
  16.55683,
  16.42576,
  16.29271,
  16.15775,
  16.02094,
  15.88235,
  15.74203,
  15.60006,
  15.4565,
  15.3114,
  15.16485,
  15.0169,
  14.86762,
  14.71707,
  14.56532,
  14.41243,
  14.25848,
  14.10352,
  13.94762,
  13.79084,
  13.63325,
  13.47491,
  13.31588,
  13.15624,
  12.99604,
  12.83534,
  12.6742,
  12.5127,
  12.35088,
  12.18881,
  12.02654,
  11.86415,
  11.70167,
  11.53918,
  11.37673,
  11.21437,
  11.05216,
  10.89016,
  10.72841,
  10.56697,
  10.40589,
  10.24522,
  10.08502,
  9.925323,
  9.766186,
  9.607653,
  9.449772,
  9.292586,
  9.13614,
  8.980476,
  8.825635,
  8.67166,
  8.518588,
  8.366459,
  8.215311,
  8.06518,
  7.916101,
  7.768108,
  7.621236,
  7.475515,
  7.330978,
  7.187655,
  7.045573,
  6.904763,
  6.765249,
  6.62706,
  6.490218,
  6.35475,
  6.220676,
  6.088021,
  5.956804,
  5.827045,
  5.698765,
  5.571981,
  5.44671,
  5.32297,
  5.200776,
  5.080142,
  4.961083,
  4.843613,
  4.727743,
  4.613485,
  4.50085,
  4.38985,
  4.280492,
  4.172788,
  4.066743,
  3.962368,
  3.859668,
  3.758651,
  3.659322,
  3.561687,
  3.465753,
  3.371522,
  3.279,
  3.18819,
  3.099097,
  3.011723,
  2.926071,
  2.842145,
  2.759946,
  2.679477,
  2.600739,
  2.523735,
  2.448466,
  2.374933,
  2.303139,
  2.233084,
  2.16477,
  2.098198,
  2.033369,
  1.970285,
  1.908946,
  1.849355,
  1.791512,
  1.735419,
  1.681077,
  1.628489,
  1.577656,
  1.528581,
  1.481264,
  1.43571,
  1.391919,
  1.349896,
  1.309643,
  1.271164,
  1.234461,
  1.199539,
  1.166401,
  1.135053,
  1.105497,
  1.07774,
  1.051786,
  1.02764,
  1.005309,
  0.984798,
  0.966113,
  0.94926,
  0.934248,
  0.921081,
  0.909769,
  0.900317,
  0.892735,
  0.88703,
  0.88321,
  0.881284,
  0.881261,
  0.883149,
  0.886958,
  0.892696,
  0.900374
};


class Weather
{
  vector<double> m_rain;
  vector<double> m_wind;
  vector<double> m_winddir;
  vector<double> m_winddir8;
  vector<double> m_WindDirRadians;
  vector<double> m_temp;
  vector<double> m_soiltemp;
  vector<double> m_snow;
  vector<double> m_mintemp;
  vector<double> m_maxtemp;
  vector<double> m_relhumidity;
  vector<double> m_soiltemptwilight;
  vector<double> m_radiation;
  vector<double> m_flyinghours;

  //below are the vectors for storing the weather data in hour
  vector<vector<double>> m_rain_h;
  vector<vector<double>> m_wind_h;
  vector<vector<double>> m_winddir_h;
  vector<vector<double>> m_winddir8_h;
  vector<vector<double>> m_temp_h;
  vector<vector<double>> m_radiation_h;
  vector<vector<double>> m_soiltemp_h;
  vector<vector<double>> m_snow_h;
  vector<vector<double>> m_humidity_h;

  //index for today's hourly weather data
  int m_hourly_today_index;

  bool m_fsoiltemp = false;
  bool m_fsnow = false;
  bool m_fminmaxtemp = false;
  bool m_frelhumidity = false;
  bool m_fsoiltemptwilight = false;
  bool m_fradiation = false;
  bool m_fflyinghours = false;



  double m_temptoday;
  double m_raintoday;
  double m_windtoday;
  int m_winddirtoday;
  int m_winddir8today;
  double m_WindDirTodayRadians;
  bool m_snowtoday;
  bool  m_rainingtoday;
  double m_insolation;
  double m_humiditytoday;
  double m_soiltemptoday;
  double m_mintempyesterday;
  double m_maxtempyesterday;
  double m_mintemptoday;
  double m_maxtemptoday;
  double m_mintemptomorrow;
  double m_maxtemptomorrow;
  double m_radiationtoday;
  double m_flyinghourstoday;
  double m_temp_variation{0};


  bool  m_wind_valid;
  bool  m_winddir_valid;
  bool  m_winddir8_valid;
  
  long  m_datemodulus;

  /** \brief The snow depth in cm */
  double m_snowdepth;

  /** \brief Flag for hourly data. */
  bool m_hourly_flag;
  /** \brief Holds the number of days we have weather data for */
  int m_NoDays;
  int m_weather_day;
public:
        Weather( const char* a_weatherdatafile = "default");
       ~Weather( void );

  /** \brief Get today's weather data at the given hour. */
  double GetTempHour(int hour) {return m_temp_h.at(m_hourly_today_index).at(hour);}
  double GetSoilTempHour(int hour) { return m_soiltemp_h.at(m_hourly_today_index).at(hour); }
  double GetWindHour(int hour) {return m_wind_h.at(m_hourly_today_index).at(hour);}
  double GetRainHour(int hour) {return m_rain_h.at(m_hourly_today_index).at(hour);}
  double GetRadiationHour(int hour) {return m_radiation_h.at(m_hourly_today_index).at(hour);}
  double GetDDDegs( long a_date );
  double GetGlobalRadiation(long a_date) { if(m_hourly_flag) return m_radiation.at(a_date % m_datemodulus); return (double)c_insolation[a_date % m_datemodulus]; }
  double GetGlobalRadiation( void )        { return m_insolation; }
  /** \brief Get the temperature on a particular date */
  double GetTemp(long a_date) { return m_temp[a_date % m_datemodulus]; }
  /** \brief Get the current soil temperature */
  double  GetSoilTemp(void) { return m_soiltemptoday; }
  /** \brief Get the soil temperature on a particular date */
  double GetSoilTemp(long a_date) { return m_soiltemp[a_date % m_datemodulus]; }
  /** \brief Get the temperature today */
  double GetTemp(void) { return m_temptoday; }
  /** \brief Get the temperature at the given number of days which is the number of days after today.*/
  double GetTempAfterDays(int days) { return m_temp[(m_weather_day + days) % m_datemodulus]; }
  /** \brief Get the min temperature today */
  double GetMinTemp(void) { return m_mintemptoday; }
  /** \brief Get the max temperature today */
  double GetMaxTemp(void) { return m_maxtemptoday; }
    /** \brief Get the min temperature yesterday */
  double GetMinTempYesterday(void) { return m_mintempyesterday; }
  /** \brief Get the max temperature yesterday */
  double GetMaxTempYesterday(void) { return m_maxtempyesterday; }
    /** \brief Get the min temperature tomorrow */
  double GetMinTempTomorrow(void) { return m_mintemptomorrow; }
  /** \brief Get the max temperature tomorrow */
  double GetMaxTempTomorrow(void) { return m_maxtemptomorrow; }
  /** \brief Get the max temperature today */
  double GetRadiation(void) { return m_radiationtoday; }
  /** \brief Get the max temperature today */
  double GetFlyingHours(void) { return m_flyinghourstoday; }
  /** \brief Get the humidity score today */
  double GetHumidity(void)    { return m_humiditytoday; }
  double GetMeanTemp(long a_date, unsigned int a_period);
  double GetRain(long a_date) { return m_rain[a_date %m_datemodulus]; }
  double GetRain( void )        { return m_raintoday; }
  double GetWind(long a_date) { return m_wind[a_date % m_datemodulus]; }
  double GetWind( void )        { return m_windtoday; }
  /** \brief Four wind directions N = 0, clockwise*/
  int GetWindDirection( void )        { return m_winddirtoday; }
  /** \brief Eight wind directions N = 0, NE = 1, ..., clockwise*/
  int GetWindDirection8( void )        { return m_winddir8today; }
  /** \brief East is 0, anticlockwise*/
  double GetWindDirectionRadians(void) {return m_WindDirTodayRadians;}
  bool  GetSnow( long a_date );
  bool  GetSnow( void )        { return m_snowtoday; }
  /** \brief Get the current snow depth */
  double  GetSnowDepth(void) { return m_snowdepth; }
  bool  Raining( void )        { return m_rainingtoday; }

  bool Get_fsoiltemp(void) { return m_fsoiltemp;  } // if true read, if false calculated 
  bool Get_fsnow(void) { return m_fsnow; } // if true read, if false calculated 
  bool Get_fminmaxtemp(void) { return m_fminmaxtemp; } // if true exists, if false it does not
  bool Get_frelhumidity(void) { return m_frelhumidity; } // if true exists, if false it does not
  bool Get_fsoiltemptwilight(void) { return m_fsoiltemptwilight; } // if true exists, if false it does not
  bool Get_fradiation(void) { return m_fradiation; } // if true exists, if false it does not
  int GetNoDays(void) { return m_NoDays; }

  // Returns total amount of rain from a_date and a_period of days
  // *backwards*! Normally a_date is equal to g_date->Date() when one
  // want to check for rain before doing a crop management step.
  // a_period should be at least one and positive.
  // --FN--, 29/11-2000.
  double GetRainPeriod( long a_date, unsigned int a_period );
  double GetWindPeriod( long a_date, unsigned int a_period );
  double GetTempPeriod( long a_date, unsigned int a_period );
  void  Tick( void );

  void readWeatherFile(int NoDays, ifstream& inFile);
  void readWeatherFileHourly(int NoHours, int NoDays, ifstream& inFile);

protected:
	double DeriveSnowCover(double a_snowdepth, double a_rainfall, double a_temperature);
	double DeriveSoilTemp(double a_temperature);

};

Weather* CreateWeather();

#endif // WEATHER_H






