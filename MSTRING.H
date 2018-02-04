#if !defined(_MSTRING_)
#define _MSTRING_

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strstrea.h>
#pragma hdrstop

class mstring
 {

friend mstring operator+( const mstring& rS1, const mstring& rS2 );
friend mstring operator+( const mstring& rS1, LPCSTR lp );
friend mstring operator+( LPCSTR lp, const mstring& rS2 );
//friend strstream& operator<<( strstream& rStrm, const mstring& rS );

public:
  mstring(): m_lp( strdup("") )
	{
	}

  mstring( LPCSTR lp )
	{
	  m_lp = NULL;
	  InternalDup( lp );
	}
  mstring( LPCSTR lp, size_t sz )
	{
	  m_lp = NULL;
	  InternalDup( lp, sz );
	}
  mstring( char c )
	{
	  m_lp = (LPSTR)malloc( 2 );
	  *m_lp = c; m_lp[ 1 ] = 0;
	}
  ~mstring()
	{
	  InternalClear();
	}

  mstring( const mstring& rS )
	{
	  m_lp = NULL;
	  InternalDup( rS.m_lp );
	}

  mstring& operator=( LPCSTR lp )
	{
	  InternalDup( lp );
	  return *this;
	}

  mstring& operator=( const mstring& rS )
	{
	  if( (LPVOID)this != (LPVOID)&rS )
		 InternalDup( rS.m_lp );
	  return *this;
	}

  void AssignN( LPCSTR lp, size_t sz )
	{
     InternalDup( lp, sz );
	}

  int IsEmpty() const
	{
	  return *m_lp == 0;
	}

  size_t length() const
	{
	  return strlen( m_lp );
	}

  int operator<( const mstring& rS )
	{
		if( !m_lp ) return 1;
		if( !rS.m_lp ) return 0;
		return strcmp( m_lp, rS.m_lp ) < 0 ? 1:0;
	}
	int operator==( const mstring& rS )
	{
		if( !m_lp || !rS.m_lp ) return (LPVOID)m_lp == (LPVOID)rS.m_lp;

		return strcmp( m_lp, rS.m_lp ) == 0;
	}

	mstring& operator+=( const mstring& rS )
	 {
		return this->operator+=( rS.m_lp );
	 }

	mstring& operator+=( char c )
	 {
		size_t sz;
		LPSTR lp = (LPSTR)malloc( (sz = strlen(m_lp)) + 2 );
		strcpy( lp, m_lp );
		lp[ sz ] = c, lp[ sz + 1 ] = 0;
		free( m_lp ), m_lp = lp;

		return *this;
	 }

	mstring& operator+=( LPCSTR lp )
	 {
		if( lp ) InternalCat( lp );
		return *this;
	 }

	LPCSTR c_str() const
	 {
		return m_lp;
	 }

	strstream& operator<<( strstream& rStrm )
	 {
		rStrm.operator<<( m_lp );
		return rStrm;
	 }

protected:
  LPSTR m_lp;

  void InternalCat( LPCSTR lp )
	{
	  size_t sz;
	  LPSTR lpT = (LPSTR)malloc( (sz=strlen(m_lp)) + strlen(lp) + 1 );
	  strcpy( lpT, m_lp );
	  strcpy( lpT + sz, lp );
	  free( m_lp );
	  m_lp = lpT;
	}

  void InternalDup( LPCSTR lp )
	{
	  InternalClear();
	  if( lp ) m_lp = strdup(lp);
	}
  void InternalDup( LPCSTR lp, size_t sz )
	{
	  InternalClear();
	  if( lp ) m_lp = (LPSTR)calloc( sz + 1, 1 ), strncpy( m_lp, lp, sz );
	}
  void InternalClear()
	{
	  if( m_lp )
		 free( m_lp ), m_lp = NULL;
	}
 };

mstring operator+( const mstring& rS1, const mstring& rS2 );

inline mstring operator+( const mstring& rS1, LPCSTR lp )
 {
	return operator+( rS1, mstring(lp) );
 }

inline mstring operator+( LPCSTR lp, const mstring& rS2 )
 {
	return operator+( mstring(lp), rS2 );
 }


#endif
