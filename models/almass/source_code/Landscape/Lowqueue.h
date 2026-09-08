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
/**
\file
\brief
<B>lowqueue.h This file contains the headers for the farm event queue classes</B> \n
*/
/**
\file
 by Frank Nikolaisen \n
 Version of June 2003 \n
 \n
 Doxygen formatted comments in July 2008 \n
*/
//
// lowqueue.h
//
/*

Copyright (c) 2003, National Environmental Research Institute, Denmark (NERI)

All rights reserved.


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

*) Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
*) Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
*) Neither the name of the NERI nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef LOWQUEUE_H
#define LOWQUEUE_H

#include <queue>
using namespace std;

/**
\brief
Used in event handling
*/
template <class ELEMTYPE>
struct LowPriPair
{
  typedef  LowPriPair value_type;
  typedef  LowPriPair size_type;

  ELEMTYPE m_element;
  long m_pri;

  LowPriPair()
  {
  }

  LowPriPair( const ELEMTYPE& a_element, long a_priority )
  {
    m_element = a_element;
    m_pri = a_priority;
  }

  bool operator< (const LowPriPair& a_another_pri_pair) const
  {
    return m_pri > a_another_pri_pair.m_pri;
  }
};


/**
\brief
Used in event handling
*/
template < class ELEMTYPE >
class LowPriority : public priority_queue< LowPriPair< ELEMTYPE > >
{
public:
  LowPriority() : priority_queue <LowPriPair< ELEMTYPE> >()
  {}

  void Push( ELEMTYPE a_element, long a_priority )
  {
    this->push( LowPriPair<ELEMTYPE>( a_element, a_priority ));
  }

  void Pop()
  {
    this->pop();
  }

  bool Empty( void )
  {
    return this->empty();
  }

  LowPriPair<ELEMTYPE> Bottom( void )
  {
    return this->top();
  }
};

#endif // LOWQUEUE_H
