#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <fstream.h>
#include <assert.h>
#include <stdio.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "Fcont.h"


void TFetchedStrContainer::EndStoringString( long lNStr )
 {
	/*IT_LStrStr it( m_ls2 );
	for( ; lNStr > 0; ++it, --lNStr );

	if( it.Current()->m_lst.GetItemsInContainer() < m_iN )
	 {
		long lAdd = m_iN - it.Current()->m_lst.GetItemsInContainer();
		for( ; lAdd > 0; --lAdd )
		 {
			TCell *pCell;
			m_siPending.m_lst.AddAtTail( (pCell=new TCell()) );
			pCell->x = m_xLats; pCell->y = m_yLast;
		 }
	 }*/
 }

void TFetchedStrContainer::AddStrSz( LPCSTR lp, LPRECT lpRe, size_t sz )
 {
	long x, y;
	if( !lpRe ) x = y = 0;
	else x = lpRe->left, y = lpRe->top;

	TStrItem* pI = new TStrItem();
	m_ls2.AddAtTail( pI );
	TCell *pCell = new TCell();
	pI->m_lst.AddAtTail( pCell );

	if( !lp ) pCell->m_s = "";
	else pCell->m_s.AssignN( lp, sz );

	pCell->x = x; pCell->y = y;
	m_xLats = x, m_yLast = y;
 }


void TFetchedStrContainer::AddStr( LPCSTR lp, LPRECT lpRe )
 {
	long x, y;
	if( !lpRe ) x = y = 0;
	else x = lpRe->left, y = lpRe->top;

	TStrItem* pI = new TStrItem();
	m_ls2.AddAtTail( pI );
	TCell *pCell = new TCell();
	pI->m_lst.AddAtTail( pCell );

	pCell->m_s = lp ? lp:"";
	pCell->x = x; pCell->y = y;
	m_xLats = x, m_yLast = y;
 }

void TFetchedStrContainer::AddStr( LPCSTR lp, LPRECT lpRe, long lNStr )
 {
	if( lNStr > m_ls2.GetItemsInContainer() - 1 )
	 {
		sprintf( cBufErr, "(AddStr) Out of array bounds in m_ppSIChache: %ld", lNStr );
		return;
	 }

	long x, y;
	if( !lpRe ) x = y = 0;
	else x = lpRe->left, y = lpRe->top;

	//IT_LStrStr it( m_ls2 );
	//for( ; lNStr > 0; ++it, --lNStr );

	PTStrItem pIT = m_ppSIChache[ lNStr ];

	TCell *pCell;
	pIT->m_lst.AddAtTail( (pCell=new TCell()) );
	pCell->m_s = lp ? lp:"";
	pCell->x = x; pCell->y = y;
	m_xLats = x, m_yLast = y;

	/*if( m_siPending.m_lst.GetItemsInContainer() > 0 &&
		 m_siPending.m_lst.PeekTail()->y != y
	  )
	 {
		long lAdd = m_iN - m_siPending.m_lst.GetItemsInContainer();
		for( ; lAdd > 0; --lAdd )
		 {
			TCell *pCell;
			m_siPending.m_lst.AddAtTail( (pCell=new TCell()) );
			pCell->x = m_xLats; pCell->y = m_yLast;
		 }
	 }

	if( m_siPending.m_lst.GetItemsInContainer() >= m_iN )
	 {
		TStrItem* siFnd = m_ls2.Find( &m_siPending );
		if( !siFnd )
		  m_ls2.AddAtTail( new TStrItem(m_siPending) );

		m_siPending.m_lst.Flush( 1 );

	 }

	TCell *pCell;
	m_siPending.m_lst.AddAtTail( (pCell=new TCell()) );
	pCell->m_s = lp ? lp:"";
	pCell->x = x; pCell->y = y;
	m_xLats = x, m_yLast = y;*/
 }

void TFetchedStrContainer::AddStrSz( LPCSTR lp, LPRECT lpRe, long lNStr, size_t sz )
 {
	if( lNStr > m_ls2.GetItemsInContainer() - 1 )
	 {
		sprintf( cBufErr, "(AddStrSz) Out of array bounds in m_ppSIChache: %ld", lNStr );
		return;
	 }

	long x, y;
	if( !lpRe ) x = y = 0;
	else x = lpRe->left, y = lpRe->top;

	//IT_LStrStr it( m_ls2 );
	//for( ; lNStr > 0; ++it, --lNStr );

	PTStrItem pIT = m_ppSIChache[ lNStr ];

	TCell *pCell;
	pIT->m_lst.AddAtTail( (pCell=new TCell()) );
	if( !lp ) pCell->m_s = "";
	else pCell->m_s.AssignN( lp, sz );
	pCell->x = x; pCell->y = y;
	m_xLats = x, m_yLast = y;
 }

void TFetchedStrContainer::Dump( mstring& rS, char cLeft, char cRight )
 {
	 rS = "";

	 for( IT_LStrStr it(m_ls2); it; ++it )
	  {
		 int iCnt = 0;
		 for( IT_LStr it2(it.Current()->m_lst); it2; ++it2, ++iCnt )
		  {
			 //char ccp[128]; sprintf( ccp, "%ld, %ld: ", it2.Current()->x, it2.Current()->y );
			 if( cLeft && cRight )
				rS += (iCnt ? "  ":"") +  mstring(cLeft) + it2.Current()->m_s + mstring(cRight);
			 else if( cLeft )
				rS += (iCnt ? "  ":"") +  mstring(cLeft) + it2.Current()->m_s;
			 else if( cRight )
				rS += (iCnt ? "  ":"") +  it2.Current()->m_s + mstring(cRight);
			 else
				rS += (iCnt ? "  ":"") +  it2.Current()->m_s;
		  }

		 rS += "\n";
	  }
 }

 //   2 - table:  nRows:DWORD, nColumns:DWORD, [sizeStr:WORD, str. bytes, sizeStr:WORD, str. bytes ... ]
LPBYTE TFetchedStrContainer::DumpCompact( LPDWORD lpSz )
 {
	*lpSz = sizeof(DWORD) * 2 + 1;
	IT_LStrStr it( m_ls2 );
	DWORD dwClms = 0;
	for( ; it; ++it )
	  dwClms = max( dwClms, (DWORD)it.Current()->m_lst.GetItemsInContainer() );

	it.Restart();
	for( ; it; ++it )
	 {
		for( IT_LStr it2(it.Current()->m_lst); it2; ++it2 )
		  *lpSz += sizeof(WORD) + it2.Current()->m_s.length();

		*lpSz += sizeof(WORD) * (dwClms - it.Current()->m_lst.GetItemsInContainer());
	 }

	LPBYTE lpRet = (LPBYTE)calloc( *lpSz, 1 );
	if( !lpRet ) return NULL;

	*lpRet = 2;

	LPDWORD lp = (LPDWORD)(lpRet + 1);
	*lp++ = m_ls2.GetItemsInContainer(); //number rows

	it.Restart();
	//*lp++ = m_ls2.GetItemsInContainer() > 0 ? it.Current()->m_lst.GetItemsInContainer():0;
	*lp++ = dwClms;

	LPBYTE lpTmp = (LPBYTE)lp;
	for( ; it; ++it )
	 {
		for( IT_LStr it2(it.Current()->m_lst); it2; ++it2 )
		 {
			LPWORD lpW = (LPWORD)lpTmp;
			*lpW = it2.Current()->m_s.length();
			lpTmp += sizeof(WORD) + *lpW++;
			strncpy( (LPSTR)lpW, it2.Current()->m_s.c_str(), it2.Current()->m_s.length() );
		 }

		for( long l = dwClms - it.Current()->m_lst.GetItemsInContainer(); l > 0; --l, lpTmp += sizeof(WORD) )
		  (*(LPWORD)lpTmp) = 0;

	 }

	return lpRet;
 }

/* HGLOBAL TFetchedStrContainer::GetPackedData()
 {
	 long lSz = 0;
	 for( IT_LStrStr it(m_ls2); it; ++it )
	  {
		 for( IT_LStr it2(it.Current()->m_lst); it2; ++it2 )
			lSz += it2.Current()->m_s.length() + 5;
			//4 byte - len; self data; 1 byte - term. 0
	  }
	 HGLOBAL hg = GlobalAlloc( GMEM_ZEROINIT|GMEM_MOVEABLE, lSz + 4 );
	 LPVOID p = GlobalLock( hg );

	 long *pL = (long*)p;
	 *pL = m_ls2.GetItemsInContainer();
	 LPSTR lp = (LPSTR)p;
	 lp += 4;

	  for( it = IT_LStrStr(m_ls2); it; ++it )
	  {
		 for( IT_LStr it2(it.Current()->m_lst); it2; ++it2 )
		  {
			 pL = (long*)lp;
			 *pL = it2.Current()->m_s.length();
			 lp += 4;
			 strcpy( lp, it2.Current()->m_s.c_str() );
			 lp += it2.Current()->m_s.length() + 1;
		  }
	  }
	  GlobalUnlock( hg );
	  return hg;
 }*/


static int CheckTh( const long& lArg, void* pDta )
 {
	return lArg == (long)pDta;
 }


void TFetchedStrContainer::InsertSpaces()
 {
	TSDoubleListImp<long> lsXClms;
	for( IT_LStrStr it( m_ls2 ); it; ++it )
	  for( IT_LStr it2( it.Current()->m_lst ); it2; ++it2 )
		{
		  long x = it2.Current()->x;
		  long *pI = lsXClms.FirstThat( CheckTh, (void*)x );
		  //if( !pI && x < 2000 ) lsXClms.AddAtHead( x );
		  if( !pI && x < 2000 )
			 lsXClms.Add( x );
		}

	for( it = IT_LStrStr(m_ls2); it; ++it )
	 {
		TSDoubleListIteratorImp<long> itCms( lsXClms );
		TIDoubleListImp<TCell> lstTmp;
		for( IT_LStr it2(it.Current()->m_lst); it2; ++it2, ++itCms )
		{
		  if( it2.Current()->x != itCms.Current() )
			{
			  TCell *pCell = new TCell();
			  //pCell->m_s = lp ? lp:"";
			  pCell->x = itCms.Current(); pCell->y = 0;
			  //m_xLats = x, m_yLast = y;

			  lstTmp.AddAtTail( pCell );
			  ++itCms;
			}
		  lstTmp.AddAtTail( it2.Current() );
		}

		if( lstTmp.GetItemsInContainer() != it.Current()->m_lst.GetItemsInContainer() )
		 {
			it.Current()->m_lst.FlushNoDelete();
			for( TIDoubleListIteratorImp<TCell> it5(lstTmp); it5; ++it5 )
			  it.Current()->m_lst.AddAtTail( it5.Current() );
		 }
	 }


	/*mstring ss;
	for( TSDoubleListIteratorImp<long> it1( lsXClms ); it1; ++it1 )
	 {
		long ll = it1.Current();
		char cb[512];
		sprintf( cb, "%lu ", ll );
		ss += cb;
	 }
	 MessageBox( NULL, ss.c_str(), "ll", MB_OK );*/
 }

void TFetchedStrContainer::InitOn( long lCount )
 {
	Clear();

	m_ppSIChache = new PTStrItem[ lCount ];
	for( long l = 0; lCount > 0; --lCount, ++l )
	  m_ls2.AddAtTail( (m_ppSIChache[l] = new TStrItem()) );
 }

