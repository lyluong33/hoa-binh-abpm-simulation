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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <regex>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream> //gcc/ and clang ignores it, but it is needed for successful compilation in VS
#include <iterator>

#include "../BatchALMaSS/ALMaSS_Setup.h"
#include "Configurator.h"
#include "MapErrorMsg.h"
#include "ALMaSSDefines.h"

#if !(defined __UNIX) & !(defined __BORLANDC__)
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif

//using namespace std;

extern void FloatToDouble(double&, float);

std::shared_ptr <Configurator> g_cfg = nullptr;
//class Configurator *g_cfg = nullptr;
static CfgBool l_cfg_public_warn_on_set("CFG_PUBLIC_WARN_ON_SET",
					CFG_CUSTOM, true );
static CfgBool l_cfg_public_exit_on_set("CFG_PUBLIC_EXIT_ON_SET",
					CFG_CUSTOM, true );


static const std::string CfgSecureStrings[] = {
  "CFG_CUSTOM",
  "CFG_PUBLIC",
  "CFG_PRIVATE"
};


static const std::string CfgTypeStrings[] = {
  "*none*",
  "int",
  "double",
  "bool",
  "string",
  "array int",
  "array double",
  "array int csv",
  "array double csv"
};


CfgBase::CfgBase( const std::string& a_key, CfgSecureLevel a_level )
{
  if ( nullptr == g_cfg ) {
    g_cfg = static_cast<const std::shared_ptr<Configurator>>(new Configurator);// move to c++11
    //  std::shared_ptr <Configurator> g_cfg(new Configurator);
  }

  m_key   = a_key;
  m_level = a_level;
  m_rangetest = false;

}
CfgBase::CfgBase( const std::string& a_key, CfgSecureLevel a_level ,bool a_definedinconfig)
{
    m_key   = a_key;
    m_level = a_level;
    m_rangetest = false;
    m_definedinconfig = a_definedinconfig;
}



CfgBase::~CfgBase( void )
{
  ;
}



CfgInt::CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval) :CfgBase(a_key, a_level)
{
	m_int = a_defval;
    g_cfg->Register( this, a_key );
}

CfgInt::CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval, bool a_definconf) :CfgBase{a_key, a_level,a_definconf}
{
    /** \brief A special constructor to be used for dynamically allocated configs*/
    m_int = a_defval;
}

CfgInt::CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval, int a_min, int a_max) :CfgBase(a_key, a_level)
{
	/** \brief Constructor with max min checking enabled*/
	m_min = a_min;
	m_max = a_max;
	m_int = a_defval;
	m_rangetest = true;
    g_cfg->Register( this, a_key );
}
CfgInt::CfgInt(const std::string& a_key, CfgSecureLevel a_level, int a_defval, int a_min, int a_max, bool a_definconf) :CfgBase(a_key, a_level)
{
    /** \brief Constructor with max min checking enabled*/
    m_min = a_min;
    m_max = a_max;
    m_rangetest = true;
    m_int = a_defval;
    g_cfg->Register( this, a_key );
}

void CfgInt::set(int a_newval) {
	if (m_rangetest) {
		if ((a_newval<m_min) || (a_newval>m_max))
		{
			g_msg->Warn(WARN_FILE, "CfgInt::set Value out of range: ", a_newval);
			g_msg->Warn("Name: ", m_key);
			g_msg->Warn(WARN_FILE, "CfgInt::set Max allowed: ", m_max);
			g_msg->Warn(WARN_FILE, "CfgInt::set Min allowed: ", m_min);
		}
	}
	m_int = a_newval;
}


CfgFloat::CfgFloat(const std::string& a_key, CfgSecureLevel a_level, double a_defval) :CfgBase(a_key, a_level)
{
	m_float = a_defval;
    g_cfg->Register( this, a_key );
}
CfgFloat::CfgFloat(const std::string& a_key, CfgSecureLevel a_level, double a_defval, bool a_definconf) :CfgBase{a_key, a_level,a_definconf}
{
    m_float = a_defval;
}

CfgFloat::CfgFloat(const std::string a_key, CfgSecureLevel a_level, double a_defval, double a_min, double a_max) : CfgBase(a_key, a_level)
{
	m_min = a_min;
	m_max = a_max;
	m_rangetest = true;
	m_float = a_defval;
    g_cfg->Register( this, a_key );
}
CfgFloat::CfgFloat(const std::string a_key, CfgSecureLevel a_level, double a_defval, double a_min, double a_max, bool a_definconf) : CfgBase(a_key, a_level)
{
    m_min = a_min;
    m_max = a_max;
    m_rangetest = true;
    g_cfg->Register( this, a_key );
}

void CfgFloat::set(double a_newval) {
	if (m_rangetest) {
		if ((a_newval<m_min) || (a_newval>m_max))
		{
			g_msg->Warn("CfgFloat::set Value out of range: ", a_newval);
			g_msg->Warn("Name: ", m_key);
			g_msg->Warn("CfgFloat::set Max allowed: ", m_max);
			g_msg->Warn("CfgFloat::set Min allowed: ", m_min);
		}
	}
	m_float = a_newval;
}



CfgBool::CfgBool( const std::string&          a_key,
		  CfgSecureLevel a_level,
		  bool           a_defval )
  :CfgBase( a_key, a_level )
{
  m_bool = a_defval;
  g_cfg->Register( this, a_key );
}
CfgBool::CfgBool( const std::string&          a_key,
                  CfgSecureLevel a_level,
                  bool           a_defval, bool a_definconf )
        :CfgBase{ a_key, a_level , a_definconf}
{
    m_bool = a_defval;
}


CfgStr::CfgStr( const std::string&          a_key,
		CfgSecureLevel a_level,
		const std::string&          a_defval )
  :CfgBase( a_key, a_level )
{
  m_string  = a_defval;
  g_cfg->Register( this, a_key );
}
CfgStr::CfgStr( const std::string&          a_key,
                CfgSecureLevel a_level,
                const std::string&          a_defval, bool a_definconf )
        :CfgBase{ a_key, a_level, a_definconf }
{
    m_string  = a_defval;
}

CfgArray_Int::CfgArray_Int( const std::string&          a_key,
                CfgSecureLevel a_level, int a_numvals,
                const std::vector<int>&          a_defval )
        :CfgBase( a_key, a_level )
{
    array_size = a_numvals;
    m_intarray  = a_defval;
    g_cfg->Register( this, a_key );
}
CfgArray_Int::CfgArray_Int( const std::string&          a_key,
                            CfgSecureLevel a_level, int a_numvals)
        :CfgBase( a_key, a_level )
{
    array_size = a_numvals;

    g_cfg->Register( this, a_key );
}
CfgArray_Int::CfgArray_Int( const std::string&          a_key,
                CfgSecureLevel a_level, int a_numvals,
                const std::vector<int>&          a_defval, bool a_definconf )
        :CfgBase{ a_key, a_level, a_definconf }
{
    array_size = a_numvals;
    m_intarray  = a_defval;
}

CfgArray_Double::CfgArray_Double( const std::string&          a_key,
                            CfgSecureLevel a_level, int a_numvals,
                            const std::vector<double>&          a_defval )
        :CfgBase( a_key, a_level )
{
    array_size = a_numvals;
    m_doublearray  = a_defval;
    g_cfg->Register( this, a_key );
}
CfgArray_Double::CfgArray_Double( const std::string&          a_key,
                            CfgSecureLevel a_level, int a_numvals)
        :CfgBase( a_key, a_level )
{
    array_size = a_numvals;

    g_cfg->Register( this, a_key );
}
CfgArray_Double::CfgArray_Double( const std::string&          a_key,
                            CfgSecureLevel a_level, int a_numvals,
                            const std::vector<double>&          a_defval, bool a_definconf )
        :CfgBase{ a_key, a_level, a_definconf }
{
    array_size = a_numvals;
    m_doublearray  = a_defval;
}



Configurator::Configurator( )
{
	m_lineno = 0;
}



Configurator::~Configurator( )
{
  ;
}


bool Configurator::Register( CfgBase* a_cfgval, const std::string& a_key )
{
  const std::string& l_key = a_key;
  char lineno[ 20 ];

  if ( CfgI.find( l_key ) != CfgI.end() ) {
    // Couldn't register, already exists.
    // will check if it was defined in cfg file
    // if so envoke checking the value method
    // and after correction return true

    if (CfgVals[CfgI.find(l_key)->second]->get_definedinconfig()) {
        sprintf(lineno, "%d", m_lineno);
        /*
        g_msg->Warn( WARN_FILE, "Configurator::Register()\n"
        +a_key+": Registering non-static variable which value was first defined in "
                                "config file. Line: ", "NA");
                                */
        a_cfgval->set_definedinconfig(true);
        return CheckBounds(CfgVals[CfgI.find(l_key)->second], a_cfgval);


    }else{
        // otherwise
        return false;
    }


  }

  unsigned int i   = (int) CfgVals.size();
  CfgI[ l_key ]    = i;
  CfgVals.resize( i+1 );
  // we will define an empty destructor for statically initiated shared pointer

  CfgVals[ i ]    = std::shared_ptr<CfgBase>{std::shared_ptr<CfgBase*>{},a_cfgval};



  return true;
}
bool Configurator::Register( std::shared_ptr<CfgBase> a_cfgval, const std::string& a_key )
{
    /** \brief An overloaded  version of Register for dynamically allocated configs
     *
     * */
    const std::string& l_key = a_key;

    if ( CfgI.find( l_key ) != CfgI.end() ) {
        // Couldn't register, already exists.
        return false;
    }

    unsigned int i   = (int) CfgVals.size();
    CfgI[ l_key ]    = i;
    CfgVals.resize( i+1 );
    // we will define an empty destructor for statically initiated shared pointer

    CfgVals[i ]    = a_cfgval;



    return true;
}


bool Configurator::ReadSymbols( const char *a_cfgfile ) {
	std::ifstream cf_file;
	//char cfgline[ CFG_MAX_LINE_LENGTH ]; //maybe we'll change it to std::string later
    std::string cfgline{};
	cf_file.open(a_cfgfile,std::fstream::in);
	if ( !cf_file.is_open() ) {
		g_msg->Warn( WARN_FILE, "Configurator::ReadSymbols() Unable to open file for reading: ", a_cfgfile );
		exit(1);
	}
	while ( !cf_file.eof()) {
	//	for (unsigned i=0; i< CFG_MAX_LINE_LENGTH; i++) cfgline[i]=' '; // Done to get rid of the rubbish that otherwise messes up the parse
		std::getline(cf_file,cfgline);
		if (ParseCfgLine( cfgline ))
		{
            do
            {
	            m_lineno++;
                std::getline(cf_file, cfgline);
            } while (!ParseCommentLine(cfgline));
		}
		m_lineno++;
	}
	cf_file.close();
	return true;
}



// Caution, modifies original line, and returns a pointer to a
// substring of a_line.
std::string Configurator::ExtractString( std::string a_line ) const
{
  char lineno[ 20 ];

  // scan for the first double quote or end of line.
  std::size_t found_first_quote =a_line.find('"');
  std::size_t found_last_quote =a_line.rfind('"');
  if ((found_first_quote == std::string::npos)||(found_first_quote == found_last_quote)){
      sprintf( lineno, "%d", m_lineno );
      g_msg->Warn( WARN_FILE, "Configurator::ExtractString()\n"
                              "  String not enclosed in double quotes at "
                              "config line ", lineno );
      exit(1);
  }
  // we can get over all this by just using regex
  try{
      std::regex regexp_between_quotes(R"("[^"\\]*(\\.[^"\\]*)*")");

      std::smatch smatch;
      std::string therest_str{};
      std::string extracted_str;
      if (std::regex_search(a_line, smatch, regexp_between_quotes)) {
          if (smatch.size() > 1) {
              extracted_str= smatch.str(0); // return the first matching string
              int endofmatch = int(smatch.position(0)+smatch.length(0));
              if (a_line.length()>endofmatch){
                  therest_str = a_line.erase(smatch.position(0), endofmatch);
              }

          }
          else {
              sprintf(lineno, "%d", m_lineno);
              g_msg->Warn(WARN_FILE, "Configurator::ExtractString()\n"
                                     "  String not enclosed in double quotes at "
                                     "config line ", lineno);
              exit(1);
          }

      }
      if ((!therest_str.empty())&&(therest_str.at(0)!='#')){
          sprintf( lineno, "%d", m_lineno );

          g_msg->Warn( WARN_FILE, "Configurator::ExtractString() "
                                  "Illegal comment at "
                                  "config line ", lineno );
          exit(1);
      }
      /*
       * We will strip the quotes (first and last symbol and return it)
       * */
      return extracted_str.substr(1, extracted_str.size() - 2);
  }
  catch (const std::regex_error& e) {
      sprintf(lineno, "%d", m_lineno);
      std::cout << "regex_error caught: " << e.what() << '\n';
      g_msg->Warn(WARN_FILE, "Configurator::ExtractString()\n"
                             "  Regex error ", lineno);
  }

  return ""; // Compiler happiness


}

/**
 * \brief Reads each line and parses it
 * \param a_line is the line read
 * \return true if the line starts with a long block comment '/*'
 */
bool Configurator::ParseCommentLine(std::string a_line)
/**
 * \brief Reads each line and checks if it is the end of the comment
 * \param a_line is the line read
 * \return true if the line ends with a long block comment end characters ' *''/'
 */
{
    if (a_line.find("*/") != std::string::npos) {
        return true;
    } // NB even if this string is in the middle of a line the rest is ignored, best to use a line only for this
    return false;
}
    bool Configurator::ParseCfgLine(std::string a_line)
{
  std::string l_id  ;
  std::string l_type;
  std::string l_sep ;
  std::string l_val ;
  std::string l_comm;
  char lineno[ 20 ];
  if (a_line.empty()){return false;}// Empty string
  if (a_line.at(0) == '#') { return false; }// Comment line: starts from #
  if ((a_line.at(0) == '/') && (a_line.at(1) == '*'))
  {
	  return true;
  }// Comment block: starts from /*

  //if (a_line.find_first_not_of(" \t\n\v\f\r") != std::string::npos){return;}// Empty line
  if ( std::all_of(a_line.begin(),a_line.end(),isspace)){return false;}// Empty line
  if (a_line.back() == '\r'){a_line.pop_back();} //If there is /r in the end (reading Win file in Unix)
  // consisting only of white spaces or tabs.


  //int l_conv = sscanf( a_line, "%[A-Z_] (%[a-z]) %s", l_id, sizeof(l_id),l_type,sizeof(l_type), l_sep,sizeof(l_sep) );
  std::regex regexp_parsing(R"(^([a-zA-Z_]\w*)\s*(\(.*\))\s*=\s*(.*?)\s*([\#].+\w*?)*$)");
  // This may look complicated for the person who never saw regex but we basically extract 4 groups:
  // (1) alphanumeric variable name
  // (2) in brackets (type)
  // =
  // (3) value can be values or numbers or whatever, (since we allow strings)
  // (4: optional) "#comment" (always after #)

  std::smatch smatch;
  std::string name_str;
  std::string type_str;
  std::string value_str;
  std::string comment_str;
  sprintf( lineno, "%d", m_lineno );
  if (std::regex_search(a_line, smatch, regexp_parsing)) {

      if (smatch.size() > 2) { // comment is optional so there are 3 or 4 groups: zeros element is all matches
          name_str= smatch.str(1);
          type_str = smatch.str(2);
          type_str = type_str.substr(1, type_str.size() - 2); // will remove brackets from around type
          value_str = smatch.str(3);
          comment_str = smatch.str(4);

      }
      else{
          sprintf( lineno, "%d", m_lineno );
          g_msg->Warn( WARN_FILE, "Configurator::ParseCfgLine() "
                                  " Unsuccessful parsing: Syntax error at",
                       lineno );
      }
  }
//int l_conv = sscanf( a_line, "%[A-Z_] (%[a-z]) %s", l_id, l_type, l_sep );





  if ( CfgI.find( name_str ) == CfgI.end() ) {
    // Key doesn't exists among the preloaded global configuration
    // variables. Previously we ignored this quietly. Now we will store it nonetheless
    // in the datastructures
#ifndef _cfgWarningOff

    g_msg->WarnAddInfo( WARN_UNDEF, "Configurator::ParseCfgLine() "
                            " Warning: not statically initialised key ("+name_str+") was found in CFG: line ",
                   lineno );
#endif
    this->StoreFromConfig(name_str, type_str, value_str);

    return false;
  }

  if ( type_str.compare("string")== 0 ) {
    // We are not yet ready to do the assignment.
    // If we really have a string enclosed in non-escaped
    // double quotes at the end of the line, then we need to
    // extract it first from our input.
    SetCfgStr( name_str, ExtractString( value_str+comment_str )); // we send just the 3rd and 4th groups,
                                                    // but this also will do
    return false;
  }

  // Not a string, so extract data value and possible comment.
//  l_conv = sscanf( a_line, "%*[A-Z_] (%*[a-z]) %*s %s %s",
//		   l_val, l_comm );
//
//  if ( l_conv == 2 && l_comm[0] != '#' ) {
//    // Illegal comment at end of line.
//    sprintf( lineno, "%d", m_lineno );
//    g_msg->Warn( WARN_FILE, "Configurator::ParseCfgLine() "
//		 "Syntax error at end of config line ",
//		 lineno );
//    exit(1);
//}
    if ( type_str.compare("double")== 0 ){
        SetCfgFloat( name_str, value_str );
        return false;
    }
    if ( type_str.compare("float")== 0 ){
            SetCfgFloat( name_str, value_str );
            return false;
    }
    if ( type_str.compare("bool")== 0 ){
        SetCfgBool( name_str, value_str );
        return false;
    }
    if ( type_str.compare("int")== 0 ){
        SetCfgInt( name_str, value_str );
        return false;
    }

    if ( type_str.compare("array int")== 0 ){
        SetCfgArrayInt( name_str, value_str );
        return false;
    }
    if ( type_str.compare("array double")== 0 ){
        SetCfgArrayDouble( name_str, value_str );
        return false;
    }

    if ( type_str.compare("array double csv")== 0 ){
        SetCfgArrayDoubleCsv( name_str, value_str );
        return false;
    }

    if ( type_str.compare("array int csv")== 0 ){
        SetCfgArrayIntCsv( name_str, value_str );
        return false;
    }
    if ( type_str.compare("function")== 0 ){
        SetCfgFunction( name_str, value_str );
        return false;
    }
  sprintf( lineno, "%d", m_lineno );
  g_msg->Warn( WARN_FILE, "Configurator::ParseCfgLine() "
	       "Unknown type specifier at config line ",
	       lineno );
  exit(1);
}



void Configurator::ShowIdType( unsigned int a_i )
{
  g_msg->WarnAddInfo( WARN_FILE,
		      "Type for identifier ",
                      (CfgVals[ a_i ].get())->getkey().c_str() );
  g_msg->WarnAddInfo( WARN_FILE, " is (",
		      CfgTypeStrings[ (CfgVals[ a_i ].get())->gettype() ] );
  g_msg->WarnAddInfo( WARN_FILE, ")\n", "" );
}



bool Configurator::SetCfgGatekeeper( const std::string a_method,
				     const std::string /* a_key */,
				     CfgSecureLevel a_level )
{
  if ( a_level == CFG_PRIVATE ) {
    // Attempting to set private config variable. Ignore quietly.
    return true;
  }

  if ( a_level == CFG_PUBLIC &&
       l_cfg_public_warn_on_set.value()) {
    // Attempting to set public config variable. Warn and
    // possibly exit if this is configured.
    char lineno[20];
    sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, a_method, lineno );

    if ( l_cfg_public_exit_on_set.value()) {
      exit(1);
    }
    return true;
  }
  return false;
}



void Configurator::SetCfgInt( std::string a_key, std::string a_val )
{
  int l_val;
  char lineno[20];
  std::string l_key = a_key;

  if ( sscanf( a_val.c_str(), "%d", &l_val ) != 1 ) {
    sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, "Configurator::SetCfgInt() "
		 "Not an integer data value at config line",
		 lineno );
    exit(1);
  }
  // for now we will leave the gatekeeper
  // Check access security.
  unsigned int i         = CfgI[ l_key ];
  if ( SetCfgGatekeeper( "Configurator::SetCfgInt() "
			 "Attempting to set public config variable in line",
			 a_key,
                         (CfgVals[ i ].get())->getlevel()
			 )) {
    return;
  }

  if ( (CfgVals[ i ].get())->gettype() != CFG_INT ) {
    sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, "Configurator::SetCfgInt() "
		 "Non-integer identifier specified at config line",
		 lineno );
    ShowIdType( i );
    exit(1);
  }

  dynamic_cast<CfgInt*>(CfgVals[ i ].get())->set( l_val );
}

void Configurator::SetCfgArrayInt(std::string a_key, std::string a_val)
{
    //int l_val;
    char lineno[20];
    std::string l_key = a_key;

    std::istringstream iss( a_val );

    int number;
    std::vector<int> array_int;
    while ( iss >> number )
        array_int.push_back( number );
    // for now we will leave the gatekeeper
    // Check access security.
    unsigned int i         = CfgI[ l_key ];
    if ( SetCfgGatekeeper( "Configurator::SetCfgInt() "
                           "Attempting to set public config variable in line",
                           a_key,
                           (CfgVals[ i ].get())->getlevel()
    )) {
        return;
    }

    if ( (CfgVals[ i ].get())->gettype() != CFG_ARRAY_INT ) {
        sprintf( lineno, "%d", m_lineno );
        g_msg->Warn( WARN_FILE, "Configurator::SetCfgArrayInt() "
                                "Non-array-integer identifier specified at config line",
                     lineno );
        ShowIdType( i );
        exit(1);
    }
    if (array_int.size()==dynamic_cast<CfgArray_Int*>((CfgVals[ i ].get()))->get_array_size()){
        dynamic_cast<CfgArray_Int*>((CfgVals[ i ].get()))->set( array_int );
    }else{
        sprintf( lineno, "%d", m_lineno );
        g_msg->Warn( WARN_FILE, "Configurator::SetCfgArrayInt() "
                                "Wrong array length at config line",
                     lineno );
    }

}
void Configurator::SetCfgArrayIntCsv(std::string a_key, std::string a_val)
{
    //int l_val;
    char lineno[20];
    std::string l_key = a_key;

    std::istringstream iss( a_val );

    int number;
    std::vector<int> array_int;
    while ( iss >> number )
        array_int.push_back( number );
    // for now we will leave the gatekeeper
    // Check access security.
    unsigned int i         = CfgI[ l_key ];
    if ( SetCfgGatekeeper( "Configurator::SetCfgInt() "
                           "Attempting to set public config variable in line",
                           a_key,
                           (CfgVals[ i ].get())->getlevel()
    )) {
        return;
    }

    if ( (CfgVals[ i ].get())->gettype() != CFG_ARRAY_INT ) {
        sprintf( lineno, "%d", m_lineno );
        g_msg->Warn( WARN_FILE, "Configurator::SetCfgInt() "
                                "Non-array-int identifier specified at config line",
                     lineno );
        ShowIdType( i );
        exit(1);
    }

    dynamic_cast<CfgArray_Int*>((CfgVals[ i ].get()))->set( array_int );
}

void Configurator::SetCfgArrayDoubleCsv(std::string a_key, std::string a_val)
{
    //int l_val;
    char lineno[20];
    std::string l_key = a_key;

    std::istringstream iss( a_val );

    int number;
    std::vector<int> array_int;
    while ( iss >> number )
        array_int.push_back( number );
    // for now we will leave the gatekeeper
    // Check access security.
    unsigned int i         = CfgI[ l_key ];
    if ( SetCfgGatekeeper( "Configurator::SetCfgInt() "
                           "Attempting to set public config variable in line",
                           a_key,
                           (CfgVals[ i ].get())->getlevel()
    )) {
        return;
    }

    if ( (CfgVals[ i ].get())->gettype() != CFG_ARRAY_DOUBLE ) {
        sprintf( lineno, "%d", m_lineno );
        g_msg->Warn( WARN_FILE, "Configurator::SetCfgInt() "
                                "Non-array-double identifier specified at config line",
                     lineno );
        ShowIdType( i );
        exit(1);
    }

    dynamic_cast<CfgArray_Int*>((CfgVals[ i ].get()))->set( array_int );
}


void Configurator::SetCfgArrayDouble(std::string a_key, std::string a_val)
{
    //int l_val;
    char lineno[20];
    std::string l_key = a_key;

    std::istringstream iss( a_val );

    double number;
    std::vector<double> array_double;
    while ( iss >> number )
        array_double.push_back( number );
    // for now we will leave the gatekeeper
    // Check access security.
    unsigned int i         = CfgI[ l_key ];
    if ( SetCfgGatekeeper( "Configurator::SetCfgInt() "
                           "Attempting to set public config variable in line",
                           a_key,
                           (CfgVals[ i ].get())->getlevel()
    )) {
        return;
    }

    if ( (CfgVals[ i ].get())->gettype() != CFG_ARRAY_DOUBLE ) {
        sprintf( lineno, "%d", m_lineno );
        g_msg->Warn( WARN_FILE, "Configurator::SetCfgInt() "
                                "Non-double-array identifier specified at config line",
                     lineno );
        ShowIdType( i );
        exit(1);
    }
    if (array_double.size()==dynamic_cast<CfgArray_Double*>((CfgVals[ i ].get()))->get_array_size()) {
        dynamic_cast<CfgArray_Double *>((CfgVals[i].get()))->set(array_double);
    }
    else{
        sprintf( lineno, "%d", m_lineno );
        g_msg->Warn( WARN_FILE, "Configurator::SetCfgArrayDouble() "
                                "Wrong array length at config line",
                     lineno );
    }
}

void Configurator::SetCfgFunction(std::string a_key, std::string a_val)
{
    //int l_val;
    char lineno[20];
    std::string l_key = a_key;

    std::istringstream iss( a_val );

    int number;
    std::vector<double> array_double;
    while ( iss >> number )
        array_double.push_back( number );
    // for now we will leave the gatekeeper
    // Check access security.
    unsigned int i         = CfgI[ l_key ];
    if ( SetCfgGatekeeper( "Configurator::SetCfgInt() "
                           "Attempting to set public config variable in line",
                           a_key,
                           (CfgVals[ i ].get())->getlevel()
    )) {
        return;
    }

    if ( (CfgVals[ i ].get())->gettype() != CFG_INT ) {
        sprintf( lineno, "%d", m_lineno );
        g_msg->Warn( WARN_FILE, "Configurator::SetCfgInt() "
                                "Non-integer identifier specified at config line",
                     lineno );
        ShowIdType( i );
        exit(1);
    }

    dynamic_cast<CfgArray_Double*>((CfgVals[ i ].get()))->set( array_double );
}

void Configurator::SetCfgBool( std::string a_key, std::string a_val )
{
  char lineno[20];
  std::string l_key = a_key;
  bool   l_val = false;

  if (  a_val.compare( "false" ) == 0 ) {
    ; // l_val defaults to false.
  } else if (  a_val.compare( "true" ) == 0 ) {
    l_val = true;
  } else {
    sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, "Configurator::SetCfgBool() "
		 "Not a boolean data value at config line",
		 lineno );
    exit(1);
  }

  // Check access security.
  unsigned int i         = CfgI[ l_key ];
  if ( SetCfgGatekeeper( "Configurator::SetCfgBool() "
			 "Attempting to set public config variable in line",
			 a_key,
			 (CfgVals[ i ].get())->getlevel()
			 )) {
    return;
  }

  if ( (CfgVals[ i ].get())->gettype() != CFG_BOOL ) {
    sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, "Configurator::SetCfgBool() "
		 "Non-boolean identifier specified at config line",
		 lineno );
    ShowIdType( i );
    exit(1);
  }

  dynamic_cast<CfgBool*>((CfgVals[ i ].get()))->set( l_val );
}



void Configurator::SetCfgFloat( std::string a_key, std::string a_val )
{
  double l_val;
  double f;
  char lineno[20];
  std::string l_key = a_key;

  if ( sscanf( a_val.c_str(), "%lf", &f) != 1 ) {
    sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, "Configurator::SetCfgFloat() "
		 "Not a floating point data value at config line",
		 lineno );
    exit(1);
  }
  //FloatToDouble(l_val,f);
  l_val = f;

  // Check access security.
  unsigned int i         = CfgI[ l_key ];
  if ( SetCfgGatekeeper( "Configurator::SetCfgFloat() "
			 "Attempting to set public config variable in line",
			 a_key,
			 (CfgVals[ i ].get())->getlevel()
			 )) {
    return;
  }

  if ( (CfgVals[ i ].get())->gettype() != CFG_FLOAT ) {
sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, "Configurator::SetCfgFloat() "
		 "Non-floating point identifier specified at config line",
		 lineno );
    ShowIdType( i );
    exit(1);
  }

  dynamic_cast<CfgFloat*>((CfgVals[ i ].get()))->set( l_val );
}



void Configurator::SetCfgStr( std::string a_key, std::string a_val )
{
  char lineno[20];
  std::string l_key = a_key;

  // Check access security.
  unsigned int i         = CfgI[ l_key ];
  if ( SetCfgGatekeeper( "Configurator::SetCfgStr() "
			 "Attempting to set public config variable in line",
			 a_key,
			 (CfgVals[ i ].get())->getlevel()
			 )) {
    return;
  }

  if ( (CfgVals[ i ].get())->gettype() != CFG_STRING ) {
    sprintf( lineno, "%d", m_lineno );
    g_msg->Warn( WARN_FILE, "Configurator::SetCfgStr() "
		 "Non-string identifier specified at config line",
		 lineno );
    ShowIdType( i );
    exit(1);
  }

  dynamic_cast<CfgStr*>((CfgVals[ i ].get()))->set( a_val );
}



void Configurator::DumpPublicSymbols( const char *a_dumpfile,
				      CfgSecureLevel a_level )
{
  if ( a_level > CFG_PUBLIC ) {
    a_level = CFG_PUBLIC;
  }
  DumpSymbols( a_dumpfile, a_level );
}



void Configurator::DumpAllSymbolsAndExit( const char *a_dumpfile )
{
  DumpSymbols( a_dumpfile, CFG_PRIVATE );
  exit(1);
}


void Configurator::DumpSymbols( const char *a_dumpfile,
				CfgSecureLevel a_level )
{
  FILE *l_dumpfile;
  char l_oprefix[ CFG_MAX_LINE_LENGTH ] = {""};
  char l_nprefix[ CFG_MAX_LINE_LENGTH ];
  std::string l_id;
  l_dumpfile=fopen( a_dumpfile, "w" );
  if (!l_dumpfile) {
    g_msg->Warn( WARN_FILE, "Configurator::DumpSymbols() "
		 "Unable to open file for writing:",
		 a_dumpfile );
    exit(1);
  }

  typedef std::map<std::string,unsigned int>::const_iterator MI;

  for ( MI ii = CfgI.begin(); ii != CfgI.end(); ii++ ) {
    unsigned int i = ii->second;

    // Skip 'secret' variables.
    if ( (CfgVals[ i ].get())->getlevel() > a_level ) {
      continue;
    }

    // Weird hack to separate different groups of
    // configuration names.
    std::string rubbish=(CfgVals[ i ].get())->getkey();
    l_id=rubbish.c_str();
    //l_id = (CfgVals[ i ].get())->getkey().c_str();
    sscanf( l_id.c_str(), "%[A-Z]", l_nprefix );
    if ( strcmp( l_oprefix, l_nprefix ) != 0 ) {
      fprintf( l_dumpfile, "\n" );
      strcpy( l_oprefix, l_nprefix );
    }

    fprintf( l_dumpfile, "%s (%s) = ",
	     l_id.c_str(),
	     CfgTypeStrings[ (CfgVals[ i ].get())->gettype() ].c_str()
	     );

    switch( (CfgVals[ i ].get())->gettype() ) {
    case CFG_INT:
      {
	CfgInt* l_p = dynamic_cast<CfgInt*>((CfgVals[ i ].get()));
	fprintf( l_dumpfile, "%d", l_p->value() );
	break;
      }
    case CFG_FLOAT:
      {
	CfgFloat* l_p = dynamic_cast<CfgFloat*>((CfgVals[ i ].get()));
	fprintf( l_dumpfile, "%f", l_p->value() );
	break;
      }
    case CFG_BOOL:
      {
	CfgBool* l_p = dynamic_cast<CfgBool*>((CfgVals[ i ].get()));
	if ( l_p->value() ) {
	  fprintf( l_dumpfile, "true" );
	} else {
	  fprintf( l_dumpfile, "false" );
	}
	break;
      }
    case CFG_STRING:
      {
	CfgStr* l_p = dynamic_cast<CfgStr*>((CfgVals[ i ].get()));
	fprintf( l_dumpfile, "\"%s\"", l_p->value() );
	break;
      }
     case CFG_ARRAY_INT:
         {
         CfgArray_Int *l_p = dynamic_cast<CfgArray_Int *>((CfgVals[i].get()));
         std::ostringstream oss;
         auto mystring= l_p->value();
            if (!mystring.empty())
            {
            // Convert all but the last element to avoid a trailing " "
                std::copy(mystring.begin(), mystring.end()-1,
                std::ostream_iterator<int>(oss, " "));

                // Now add the last element with no delimiter
                oss << mystring.back();
                const char* stringtodump=oss.str().c_str();
                fprintf(l_dumpfile, "%s", stringtodump);
            }

         break;
     }
        case CFG_ARRAY_DOUBLE:
        {
            CfgArray_Double *l_p = dynamic_cast<CfgArray_Double *>((CfgVals[i].get()));
            std::ostringstream oss;
            auto mystring= l_p->value();
            if (!mystring.empty())
            {
                // Convert all but the last element to avoid a trailing " "
                std::copy(mystring.begin(), mystring.end()-1,
                          std::ostream_iterator<double>(oss, " "));

                // Now add the last element with no delimiter
                oss << mystring.back();
                const char* stringtodump=oss.str().c_str();
                fprintf(l_dumpfile, "%s", stringtodump);
            }

            break;
        }
    default:
      {
	char l_errno[20];
	sprintf( l_errno, "%d", (CfgVals[ i ].get())->gettype() );
	g_msg->Warn( WARN_FILE, "Configurator::DumpSymbols() "
		     "Unknown symbol type read:",
		     l_errno );
	exit(1);
      }
    }
    fprintf( l_dumpfile, "  # %s\n",
	     CfgSecureStrings[ (CfgVals[ i ].get())->getlevel() ].c_str()
	     );
  }

}
/*
 * This function runs when the configuration has not been defined during static initialisation
 * Therefore we store what is possible from configurator file assuming the rest
 *
 * */
void Configurator::StoreFromConfig(const std::string& name, std::string type, std::string value) {
    char lineno[20];
    if (type.compare("int") == 0){
        int l_val;
        if ( sscanf( value.c_str(), "%d", &l_val ) != 1 ) {
            sprintf(lineno, "%d", m_lineno);
            g_msg->Warn(WARN_FILE, "Configurator::StoreFromConfig() "
                                   "Not an integer data value at config line",
                        lineno);
            exit(1);
        }

        //will create first an object of CfgInt (will allocate memory for it)
        auto new_object (std::make_shared<CfgInt>( CfgInt ( name,CFG_CUSTOM,l_val, true)));
        // register it in "database" : for dynamically allocated data this function "moves" the ownership of the object
        this->Register(std::move(std::static_pointer_cast<CfgBase>(new_object)) , name );


    } else if (type.compare("bool") == 0){
        bool l_val{false};
        if (  value.compare( "false" ) == 0 ) {
            ; // l_val defaults to false.
        } else if (  value.compare( "true" ) == 0 ) {
            l_val = true;
        } else {
            sprintf( lineno, "%d", m_lineno );
            g_msg->Warn( WARN_FILE, "Configurator::SetCfgBool() "
                                    "Not a boolean data value at config line",
                         lineno );
            exit(1);
        }

        //will create first an object of CfgInt
        auto new_object (std::make_shared<CfgBool>( CfgBool ( name,CFG_CUSTOM,l_val, true)));

        // register it in "database"
        this->Register(std::move(std::static_pointer_cast<CfgBase>(new_object)) , name );


    } else if ((type.compare("double") == 0)||(type.compare("float") == 0)){
        double l_val;
        if ( sscanf( value.c_str(), "%lf", &l_val ) != 1 ) {
            sprintf(lineno, "%d", m_lineno);
            g_msg->Warn(WARN_FILE, "Configurator::StoreFromConfig() "
                                   "Not an double data value at config line",
                        lineno);
            exit(1);
        }

        //will create first an object of CfgFloat
        auto new_object (std::make_shared<CfgFloat>( CfgFloat ( name,CFG_CUSTOM,l_val, true)));

        // "register" it in "database"
        this->Register(std::move(std::static_pointer_cast<CfgBase>(new_object)) , name );


    }
    else if (type.compare("string") == 0) {


        //will create first an object of CfgInt
        auto new_object (std::make_shared<CfgStr>(CfgStr (name, CFG_CUSTOM, ExtractString(value), true)));
        // register it in "database"
        this->Register(std::move(std::static_pointer_cast<CfgBase>(new_object)) , name );

    }
    else if (type.compare("array int") == 0) {
        try {
            std::regex int_regex("(\s*(-*[0-9]+\s+)+\s*)");


            std::smatch smatch;
            std::string therest_str{};
            std::string extracted_str;
            if (std::regex_search(value, smatch, int_regex)) {
                if (smatch.size() < 1) {
                    sprintf(lineno, "%d", m_lineno);
                    g_msg->Warn(WARN_FILE, "Configurator::StoreFromConfig() "
                                           + name + " Unsuccessful parsing: Syntax error at",
                                lineno);
                }
            }
        }
        catch (const std::regex_error& e) {
            sprintf(lineno, "%d", m_lineno);
            std::cout << "regex_error caught: " << e.what() << '\n';
            g_msg->Warn(WARN_FILE, "Configurator::StoreFromConfig()\n"
                                   "  Regex error ", lineno);
        }
        std::vector<int> array_vec=ParseArrayInt(value);
        int a_numvals = int(array_vec.size());

        //will create first an object of CfgInt
        auto new_object (std::make_shared<CfgArray_Int>(CfgArray_Int (name, CFG_CUSTOM, a_numvals, array_vec, true)));
        // register it in "database"
        this->Register(std::move(std::static_pointer_cast<CfgBase>(new_object)) , name );

    }
    else if (type.compare("array double") == 0) {
        try {
            std::regex int_regex("(\s*(-*[0-9.]+\s+)+\s*)");


            std::smatch smatch;
            std::string therest_str{};
            std::string extracted_str;
            if (std::regex_search(value, smatch, int_regex)) {
                if (smatch.size() < 1) {
                    sprintf(lineno, "%d", m_lineno);
                    g_msg->Warn(WARN_FILE, "Configurator::StoreFromConfig() "
                                           + name + " Unsuccessful parsing: Syntax error at",
                                lineno);
                }
            }
        }
        catch (const std::regex_error& e) {
            sprintf(lineno, "%d", m_lineno);
            std::cout << "regex_error caught: " << e.what() << '\n';
            g_msg->Warn(WARN_FILE, "Configurator::StoreFromConfig()\n"
                                   "  Regex error ", lineno);
        }
        std::vector<double> array_vec=ParseArrayDouble(value);
        int a_numvals = int(array_vec.size());

        //will create first an object of CfgInt
        auto new_object (std::make_shared<CfgArray_Double>(CfgArray_Double (name, CFG_CUSTOM, a_numvals, array_vec, true)));
        // register it in "database"
        this->Register(std::move(std::static_pointer_cast<CfgBase>(new_object)) , name );

    }
}
bool Configurator::CheckBounds(std::shared_ptr<CfgBase> a_cfgval, CfgBase* b_cfgval){
    /**\brief Function checks that values in the configfile a_cfgval and in the var b_cfgval are in line
     * */

     if (b_cfgval->gettype()!=a_cfgval->gettype()){
        g_msg->Warn( WARN_FILE, "Configurator::CheckBounds() "
        "Mismatched config variable types",
        "NA" );
        return false;
     }
    switch (b_cfgval->gettype()) {
            case CFG_INT:{
            if ((b_cfgval->get_rangetest())&&(std::dynamic_pointer_cast<CfgInt>(a_cfgval)->value()>dynamic_cast<CfgInt*>(b_cfgval)->getmax()
            ||std::dynamic_pointer_cast<CfgInt>(a_cfgval)->value()<dynamic_cast<CfgInt*>(b_cfgval)->getmin())){
                g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\n Check failed "
                                        +std::dynamic_pointer_cast<CfgInt>(a_cfgval)->getkey()+" Not within the bounds, setting default",
                "NA" );
#ifdef __CFG_RUNNING_DEFAULTS
                std::dynamic_pointer_cast<CfgInt>(a_cfgval)->set(dynamic_cast<CfgInt*>(b_cfgval)->value());
#endif
            }else{
                dynamic_cast<CfgInt*>(b_cfgval)->set(std::dynamic_pointer_cast<CfgInt>(a_cfgval)->value());
                // g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck passed "
                //                         +std::dynamic_pointer_cast<CfgInt>(a_cfgval)->getkey()+": Using config file value "
                //                                " Line: ", "NA");
            }

            };
            break;
            case CFG_FLOAT: {
                if ((b_cfgval->get_rangetest())&&(std::dynamic_pointer_cast<CfgFloat>(a_cfgval)->value() >
                    dynamic_cast<CfgFloat *>(b_cfgval)->getmax()
                    || std::dynamic_pointer_cast<CfgFloat>(a_cfgval)->value() <
                       dynamic_cast<CfgFloat *>(b_cfgval)->getmin())) {
                    g_msg->Warn(WARN_FILE, "Configurator::CheckBounds()\n Check failed"
                                           + std::dynamic_pointer_cast<CfgFloat>(a_cfgval)->getkey() +
                                           "Not within the bounds, setting default", "NA"
                    );
#ifdef __CFG_RUNNING_DEFAULTS
                    std::dynamic_pointer_cast<CfgFloat>(a_cfgval)->set(dynamic_cast<CfgFloat*>(b_cfgval)->value());
#endif
                } else {
                    dynamic_cast<CfgFloat *>(b_cfgval)->set(std::dynamic_pointer_cast<CfgFloat>(a_cfgval)->value());
                    // g_msg->Warn(WARN_FILE, "Configurator::CheckBounds()\nCheck passed "
                    //                        + std::dynamic_pointer_cast<CfgFloat>(a_cfgval)->getkey() +
                    //                        ": Using config file value "
                    //                        "Line: ", "NA");
                }
            }
            break;
            case CFG_BOOL:{
                dynamic_cast<CfgBool*>(b_cfgval)->set(std::dynamic_pointer_cast<CfgBool>(a_cfgval)->value());
                // g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck passed "
                //                         +std::dynamic_pointer_cast<CfgBool>(a_cfgval)->getkey()+": Using config file value "
                //                                                                                "Line: ", "NA");
            }
            break;
            case CFG_STRING:{
                 dynamic_cast<CfgStr*>(b_cfgval)->set(std::dynamic_pointer_cast<CfgStr>(a_cfgval)->value());
                // g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck passed "
                //                         +std::dynamic_pointer_cast<CfgStr>(a_cfgval)->getkey()+": Using config file value "
                //                                                                                "Line: ", "NA");
            }
                break;
                case CFG_ARRAY_INT:{
                    if(std::dynamic_pointer_cast<CfgArray_Int>(a_cfgval)->get_array_size()!=dynamic_cast<CfgArray_Int*>(b_cfgval)->get_array_size())
                        if (dynamic_cast<CfgArray_Int*>(b_cfgval)->value().empty()){
                            g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck failed wrong vector size "
                                                    +std::dynamic_pointer_cast<CfgArray_Int>(a_cfgval)->getkey()+": No in-code value "
                                                                                                           "Line: ", "NA");
                            return false;
                        }
                        else{
                            g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck failed wrong vector size "
                                                    +std::dynamic_pointer_cast<CfgArray_Int>(a_cfgval)->getkey()+": Using in-code value "
                                                                                                                 "Line: ", "NA");
#ifdef __CFG_RUNNING_DEFAULTS
                            std::dynamic_pointer_cast<CfgArray_Int>(a_cfgval)->set(dynamic_cast<CfgArray_Int*>(b_cfgval)->value());
#endif
                    }
                        else{
                            dynamic_cast<CfgArray_Int*>(b_cfgval)->set(std::dynamic_pointer_cast<CfgArray_Int>(a_cfgval)->value());
                            // g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck passed "
                            //                     +std::dynamic_pointer_cast<CfgArray_Int>(a_cfgval)->getkey()+": Using config file value "
                            //                                                                            "Line: ", "NA");
                        }
                }
                break;
                case CFG_ARRAY_DOUBLE:{
                    if(std::dynamic_pointer_cast<CfgArray_Double>(a_cfgval)->get_array_size()!=dynamic_cast<CfgArray_Double*>(b_cfgval)->get_array_size())
                        if (dynamic_cast<CfgArray_Double*>(b_cfgval)->value().empty()){
                            g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck failed wrong vector size "
                                                    +std::dynamic_pointer_cast<CfgArray_Double>(a_cfgval)->getkey()+": No in-code value "
                                                                                                                 "Line: ", "NA");
                            return false;
                        }
                        else{
                            g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck failed wrong vector size "
                                                    +std::dynamic_pointer_cast<CfgArray_Double>(a_cfgval)->getkey()+": Using in-code value "
                                                                                                                 "Line: ", "NA");
#ifdef __CFG_RUNNING_DEFAULTS
                            std::dynamic_pointer_cast<CfgArray_Double>(a_cfgval)->set(dynamic_cast<CfgArray_Double*>(b_cfgval)->value());
#endif
                        }
                    else{
                        dynamic_cast<CfgArray_Double*>(b_cfgval)->set(std::dynamic_pointer_cast<CfgArray_Double>(a_cfgval)->value());
                        // g_msg->Warn( WARN_FILE, "Configurator::CheckBounds()\nCheck passed "
                        //                         +std::dynamic_pointer_cast<CfgArray_Double>(a_cfgval)->getkey()+": Using config file value "
                        //                                                                                      "Line: ", "NA");
                    }
                }
                break;

}
return true;
}

//TODO: The next two can be united using a template
std::vector<int> Configurator::ParseArrayInt(const std::string& a_stringtoparse) {

    std::stringstream iss( a_stringtoparse );

    int number;
    std::vector<int> myNumbers;
    while ( iss >> number )
        myNumbers.push_back( number );
    return myNumbers;
}
std::vector<double> Configurator::ParseArrayDouble(const std::string& a_stringtoparse) {

    std::stringstream iss( a_stringtoparse );

    double number;
    std::vector<double> myNumbers;
    while ( iss >> number )
        myNumbers.push_back( number );
    return myNumbers;
}

std::shared_ptr <Configurator> CreateConfigurator()
{
    if (g_cfg == nullptr)
    {
        g_cfg = static_cast<const std::shared_ptr<Configurator>>(new Configurator());
    }

    return g_cfg;
}
