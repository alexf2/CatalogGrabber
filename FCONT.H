#if !defined(_FCONT_)
#define _FCONT_

#include <windows.h>
#include "mstring.h"
#include <classlib/dlistimp.h>

#pragma hdrstop

#include "findwnd.h"

_CLASSDEF( TFetchedStrContainer )


#define LThStep  3L

class TFetchedStrContainer
 {
public:

	TFetchedStrContainer(): m_iN( 1 )
	 {
		m_xLats = m_yLast = 0; m_ppSIChache = NULL;
	 }
	TFetchedStrContainer( int iN ): m_iN( iN )
	 {
		m_xLats = m_yLast = 0; m_ppSIChache = NULL;
	 }
	~TFetchedStrContainer()
	 {
		if( m_ppSIChache )
		  delete[] m_ppSIChache, m_ppSIChache = NULL;
	 }

	void InitOn( long lCount );
	void EndStoringString( long lNStr );

	void AddStr( LPCSTR lp, LPRECT lpRe, long lNStr );
	void AddStr( LPCSTR lp, LPRECT lpRe );
	void AddStrSz( LPCSTR lp, LPRECT lpRe, long lNStr, size_t sz );
	void AddStrSz( LPCSTR lp, LPRECT lpRe, size_t sz );

	void Clear()
	 {
		m_xLats = m_yLast = 0;
		m_ls2.Flush( 1 );
		delete[] m_ppSIChache, m_ppSIChache = NULL;
	 }
	void Dump( mstring&, char cLeft, char cRight );
	LPBYTE DumpCompact( LPDWORD lpSz );

	void SetDiv( int iD )
	 {
		m_iN = iD;
	 }

	//HGLOBAL GetPackedData();
	void InsertSpaces();

protected:
  int m_iN;
  L_StrStr m_ls2;
  TStrItem m_siPending;
  long m_xLats, m_yLast;

  TStrItem** m_ppSIChache;
 };

extern char cBufErr[ 1024 ];
void StoreList( TFetchedStrContainer*, LPSTR, char cL, char cR ) throw( xmsg );


#endif

