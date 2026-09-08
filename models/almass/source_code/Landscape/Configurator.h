//
// Configurator.h
//
/*
*
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
 * Modified by Andrey Chuhutin, April-May 2020
*/

#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

// If defined will compile many methods as inlines for speed.
//#define CONF_INLINES

#include <stdio.h>
#include <functional>
#ifdef __UNIX
#undef max
#endif
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <array>
#include <memory>

//#include <algorithm.h>

// This one *has* to be a define!
// A global configurator variable will not do.
#define CFG_MAX_LINE_LENGTH 512

/* we will call everything from std namespace explicitly*/
//using namespace std;


typedef enum {
  CFG_NONE,
  CFG_INT,
  CFG_FLOAT,
  CFG_BOOL,
  CFG_STRING,
  CFG_ARRAY_INT,
  CFG_ARRAY_DOUBLE,
  CFG_FUNCTION
} CfgType;

typedef enum {
  CFG_CUSTOM,
  CFG_PUBLIC,
  CFG_PRIVATE
} CfgSecureLevel;


/** \brief
* Base class for a configurator entry
*/
class CfgBase {
protected:
    std::string         m_key;
    CfgSecureLevel m_level;
    bool m_rangetest;
    bool m_definedinconfig{false};

 public:
	 CfgBase(const std::string& a_key, CfgSecureLevel a_level);
     CfgBase( const std::string& a_key, CfgSecureLevel a_level ,bool a_definedinconfig);
	 virtual ~CfgBase();

  std::string    getkey  ( ) { return m_key; }
  virtual CfgType gettype ( ) { return CFG_NONE; }
  CfgSecureLevel  getlevel( void ) { return m_level; }
  void set_definedinconfig(bool flag){m_definedinconfig=flag;}
  bool get_definedinconfig() const{return m_definedinconfig;}
  bool get_rangetest() const{return m_rangetest;};
};

/** \brief
* Integer configurator entry class
*/
class CfgInt : public CfgBase
{
protected:


    int m_int;
	int m_max{};
	int m_min{};

 public:
  CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval );
  CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval,  int a_min, int a_max);
  CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval, bool a_definedinconfig);
  CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval, int a_min, int a_max, bool a_definconf);
  int value( ) const { return m_int; }
  void set(int a_newval);
  CfgType gettype( ) override { return CFG_INT; }
  double getmax() const {return m_max;}
  double getmin() const {return m_min;}
};

/** \brief
* Double configurator entry class
*/
class CfgFloat : public CfgBase
{
protected:
    CfgFloat(const std::string a_key, CfgSecureLevel a_level, double a_defval, double a_min, double a_max,
             bool a_definconf);

    double m_float;
	double m_min{};
	double m_max{};

public:
	CfgFloat(const std::string&, CfgSecureLevel a_level, double a_defval);
    CfgFloat(const std::string&, CfgSecureLevel a_level, double a_defval, bool a_definconf);
	/** \brief Constructor with max min checking enabled*/
	CfgFloat(std::string, CfgSecureLevel a_level, double a_defval, double a_min, double a_max);

	double  value() const { return m_float; }
	void set(double a_newval);
	CfgType gettype() override { return CFG_FLOAT; }
    double getmax() const {return m_max;}
    double getmin() const {return m_min;}
};


/** \brief
 * Bool configurator entry class
 *
 *
*/
class CfgBool : public CfgBase
{
protected:
	bool            m_bool;

 public:
  CfgBool( const std::string&, CfgSecureLevel a_level, bool a_defval );
  CfgBool( const std::string&, CfgSecureLevel a_level, bool a_defval, bool a_definconf );

  bool            value( ) const { return m_bool; }
  void            set( bool a_newval ) { m_bool = a_newval; }
  CfgType gettype( ) override { return CFG_BOOL; }
};


/** \brief
* String configurator entry class
*/
class CfgStr : public CfgBase
{
protected:
	std::string          m_string;

 public:
  CfgStr( const std::string& a_key, CfgSecureLevel a_level, const std::string& a_defval );
  CfgStr( const std::string& a_key, CfgSecureLevel a_level, const std::string& a_defval, bool a_definconf );

  char*     value( ) const { return const_cast<char *>(m_string.c_str()); }
  void            set( std::string a_newval ) { m_string = std::move(a_newval); }
  virtual CfgType gettype( void ) { return CFG_STRING; }
};
/** \brief
* Array_Int configurator entry class
*/
class CfgArray_Int : public CfgBase
{
protected:


    int  array_size{0};
    std::vector<int> m_intarray{};
public:

    CfgArray_Int( const std::string& a_name, CfgSecureLevel a_level, int a_numofvals, const std::vector<int>& a_defval );
    CfgArray_Int( const std::string& a_name, CfgSecureLevel a_level, int a_numofvals, const std::vector<int>& a_defval, bool a_definconf);
    CfgArray_Int(const std::string &a_key, CfgSecureLevel a_level, int a_numvals);
    std::vector<int>     value() const { return m_intarray; }
    void            set( std::vector<int> a_newval ) { m_intarray = std::move(a_newval); }
    virtual CfgType gettype( ) { return CFG_ARRAY_INT; }
    int get_array_size(){return array_size;}
    int value(unsigned a_index) { return m_intarray[a_index]; }
};

class CfgArray_Double : public CfgBase
{
protected:
    int  array_size{0};
    std::vector<double> m_doublearray{};
public:

    CfgArray_Double( const std::string& a_name, CfgSecureLevel a_level, int a_numofvals, const std::vector<double>& a_defval );
    CfgArray_Double( const std::string& a_name, CfgSecureLevel a_level, int a_numofvals, const std::vector<double>& a_defval, bool a_definconf);
    CfgArray_Double(const std::string &a_key, CfgSecureLevel a_level, int a_numvals);

    std::vector<double>     value() const { return m_doublearray; }
    void            set( std::vector<double> a_newval ) { m_doublearray = std::move(a_newval); }
    virtual CfgType gettype( ) { return CFG_ARRAY_DOUBLE; }
    int get_array_size(){return array_size;}
    double value(unsigned a_index) { return m_doublearray[a_index]; }
};
/** \brief
 * Function configurator entry class
 * The function pointers are all of type <void ()>.
 * The user is responsible for casting them for usage to proper type.
 * The constructor is responsible for casting them for storage to void() using reinterpret_cast
*/
class CfgFunction : public CfgArray_Double{
protected:
    std::function<void ()> funtocall;
    std::string function_name;
public:
    CfgFunction( std::string a_name, CfgSecureLevel a_level,  int a_numofvals, const double a_defval, std::string a_function_name, std::function<void ()> funtocall );
    CfgFunction( std::string a_name, CfgSecureLevel a_level,  int a_numofvals, const double a_defval, std::string a_function_name );

    CfgFunction( std::string a_name, CfgSecureLevel a_level,  int a_numofvals, const double a_defval);


    std::function<void ()>     value() const { return funtocall; }
    std::string     fun_name() const { return function_name; }
    void            set( std::vector<double> a_newval, std::string fun_name );
    virtual CfgType gettype( ) { return CFG_FUNCTION; }
};

/** \brief
* A class to provide standard parameter entry facilities
*/
class Configurator
{
 protected:
	std::map<std::string, unsigned int> CfgI;
    std::vector<std::shared_ptr<CfgBase>>         CfgVals;

  // Private methods and fields for the configuration file parser.
  unsigned int m_lineno;
  bool ParseCfgLine( std::string a_line );

  //bool LastDoubleQuote( std::string a_rest_of_line );

  // Discreet security check. Returns true if we terminate line
  // parsing early.
  bool SetCfgGatekeeper( const std::string a_method,
			 const std::string a_key,
			 CfgSecureLevel a_level );

  // Helper methods.
  void  ShowIdType( unsigned int a_i );
  std::string ExtractString( std::string a_line ) const;
	bool ParseCommentLine(std::string a_line);
	void  DumpSymbols( const char *a_dumpfile,
	                   CfgSecureLevel a_level );

 public: // for python interface 
    void SetCfgInt(std::string a_key, std::string a_val);
    void SetCfgFloat(std::string a_key, std::string a_val);
    void SetCfgBool(std::string a_key, std::string a_val);
    void SetCfgStr(std::string a_key, std::string a_val);
    void SetCfgArrayInt(std::string a_key, std::string a_val);
    void SetCfgArrayDouble(std::string a_key, std::string a_val);
    void SetCfgArrayIntCsv(std::string a_key, std::string a_val);
    void SetCfgArrayDoubleCsv(std::string a_key, std::string a_val);
    void SetCfgFunction(std::string a_key, std::string a_val);

 public:
  // Write all configuration values with a security level at or below
  // a_level to a_dumpfile in alphabetical order. a_level must
  // be CFG_CUSTOM (user settable) or CFG_PUBLIC (user can see
  // predefined value and the very existence of this parameter).
  void DumpPublicSymbols( const char *a_dumpfile,
			  CfgSecureLevel a_level );

  // Dump *all* configuration values, including the private ones,
  // to a_dumpfile and then calls exit(), noting the event in the error
  // file!! For debugging purposes only. The call to exit() is there in
  // order to try and prevent any use of this call from making it into
  // production code.
  void DumpAllSymbolsAndExit( const char *a_dumpfile );

  // Reads and parses a_cfgfile for configuration values. Unknown
  // or CFG_PRIVATE keys are silently ignored thus preventing snooping
  // on possibly interesting key values. Returns true if reading and
  // parsing was error free.
  bool ReadSymbols( const char *a_cfgfile );

  // You should never use these ones directly, they are
  // called automagically by the CfgBase class con/destructor
  // when needed.
   Configurator( void );
  ~Configurator( void );

  // Please don't use this unless you know what you are doing.
  // Need to be public as it is used by the CfgBase class and friends.
  bool Register( CfgBase* a_cfgval, const std::string& a_key );
  bool Register(std::shared_ptr<CfgBase> a_cfgval, const std::string &a_key);
  void StoreFromConfig(const std::string &name, std::string type, std::string value);


    bool CheckBounds(std::shared_ptr<CfgBase> a_cfgval, CfgBase *b_cfgval);

    std::vector<int> ParseArrayInt(const std::string& a_stringtoparse);

    std::vector<double> ParseArrayDouble(const std::string &a_stringtoparse);
};

std::shared_ptr <Configurator> CreateConfigurator();

extern std::shared_ptr <Configurator> g_cfg;
//extern class Configurator *g_cfg;
#endif // CONFIGURATOR_H



