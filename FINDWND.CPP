#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <fstream.h>
#include <assert.h>
#include <cstring.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "FindWnd.h"
#include "Fcont.h"


const long BUF_SIZE = 1024;

extern TModule *Module;

static long lCntWnd = 0;
BOOL CALLBACK MyEnumWindowsProc(
	 HWND hwnd,	// handle to parent window
	 LPARAM lParam 	// application-defined value
	)
 {
	if( hwnd == NULL ) return TRUE;

	char cBuf[ BUF_SIZE ];
	long iID;
	*cBuf = 0;

	++lCntWnd;

	RTWndTrack rI = *(PTWndTrack)(LPVOID)lParam;
	if( rI.m_sParentTitle.length() > 0 )
	 {
		GetWindowText( hwnd, cBuf, BUF_SIZE );
		if( rI.m_sParentTitle == cBuf ) rI.m_hw = hwnd;
	 }
	else if( rI.m_bID == 0 && lCntWnd == rI.m_lID )
	 {
		rI.m_hw = hwnd;
		rI.m_sParentTitle = "";
	 }
	else if( rI.m_lID != -1 && (iID=(GetDlgCtrlID(hwnd)&0xFFFF)) == rI.m_lID )
	 {
		rI.m_hw = hwnd;
		GetWindowText( hwnd, cBuf, BUF_SIZE );
		rI.m_sParentTitle = cBuf;
	 }

	return rI.m_hw ? FALSE:TRUE;
 }

static void ThrowCnntFind( RTWndTrack rI )
 {
	if( rI.m_lID == -1 )
	  throw xmsg( ("Can't find window: [" + rI.m_sParentTitle + "]").c_str() );
	else
	 {
		char cBuf[ 64 ];
		sprintf( cBuf, "%lu", rI.m_lID );
		throw xmsg( ("Can't find window: [" + mstring(cBuf) + "]").c_str() );
	 }
 }

void PartSearch( RTWndTrack rI, HWND hwPar ) throw(xmsg)
 {
	if( !hwPar )
	 {
		if( rI.m_sParentTitle.length() < 1 )
		  throw xmsg( "Bad window title: need specify HWND or parent's title" );

		rI.m_hw = FindWindow( NULL, rI.m_sParentTitle.c_str() );
		if( !rI.m_hw ) ThrowCnntFind( rI );
	 }
	else
	 {
		lCntWnd = 0;
		BOOL bRet = EnumChildWindows( hwPar, MyEnumWindowsProc, (LPARAM)(LPVOID)&rI );
		//if( !bRet ) throw xmsg( "Error on finding window: [" + rI.m_sParentTitle + "]" );
		if( !rI.m_hw ) ThrowCnntFind( rI );
	 }
 }

BOOL CALLBACK MyEnumWindowsProc2(
	 HWND hwnd,	// handle to parent window
	 LPARAM lParam 	// application-defined value
	)
 {
	char cBuf[ BUF_SIZE ];
	*cBuf = 0;

	//RTWndTrack rI = *(PTWndTrack)(LPVOID)lParam;
	PTWndTrack *arr = (PTWndTrack*)(LPVOID)lParam;

	lCntWnd = 0;

	if( EnumChildWindows(hwnd, MyEnumWindowsProc, (LPARAM)(LPVOID)arr[1]) )
	 {
		arr[0]->m_hw = hwnd;
		GetWindowText( hwnd, cBuf, BUF_SIZE );
		arr[0]->m_sParentTitle = cBuf;
	 }

	return arr[0]->m_hw ? FALSE:TRUE;
 }

void PartSearchNull( RTWndTrack rI, HWND hwPar, RTWndTrack rI2 ) throw(xmsg)
 {
	PTWndTrack arr[ 2 ] = { &rI, &rI2 };
	BOOL bRet = EnumChildWindows( hwPar, MyEnumWindowsProc2, (LPARAM)(LPVOID)arr );
	//if( !bRet ) throw xmsg( "Error on finding window: [" + rI.m_sParentTitle + "]" );
	if( !rI2.m_hw ) ThrowCnntFind( rI2 );
 }

static void LoadName( RTWndTrack rIt )
 {
	char cBuf[ BUF_SIZE ];
	*cBuf = 0;
	GetClassName( rIt.m_hw, cBuf, BUF_SIZE );
	rIt.m_sClassName = cBuf;
 }

void FindControlWnd( const L_Wnd& rlstPch, HWND hwPar ) throw(xmsg)
 {
	if( rlstPch.IsEmpty() ) return;

	IT_LWnd it( rlstPch );
	for( ; it; it++ )
	 {
		RTWndTrack rIt = *it.Current();

		if( rIt.m_hw )
		 {
			hwPar = rIt.m_hw;
			continue;
		 }

		if( rIt.m_lID == 0 )
		 {
			IT_LWnd it2 = it; it2++;
			RTWndTrack rIt2 = *it2.Current();
			if( it2 )
			 {
				PartSearchNull( rIt, hwPar, rIt2 );
				hwPar = it2.Current()->m_hw;
				if( hwPar ) LoadName( *it2.Current() );
				it++;
			 }
		 }
		else
		 {
			PartSearch( rIt, hwPar );
			hwPar = rIt.m_hw;
		 }

		if( rIt.m_hw ) LoadName( rIt );
	 }
 }

void DumpLst( const L_Wnd& rLW, mstring& rStr )
 {
	rStr = "";
	for( IT_LWnd it( rLW ); it; ++it )
	 {
		RTWndTrack rTr = *it.Current();
		strstream strm;
		strm << "[" << (void*)rTr.m_hw << "] \"" << rTr.m_sParentTitle.c_str() << "\"\t of " <<
		  rTr.m_sClassName.c_str() << " with " << rTr.m_lID << "\n" << '\0';

		char *p = strm.str();
		rStr += p;
		//delete p;
	 }
 }

TWndTrack::~TWndTrack()
 {
 }


TStrItem& TStrItem::operator=( const TStrItem& r )
 {
	m_lst.Flush( 1 );

	for( IT_LStr it( r.m_lst ); it; ++it )
	  m_lst.AddAtTail( new mstring(it.Current()->m_s) );

	return *this;
 }
int TStrItem::operator==( const TStrItem& rI ) const
 {
	if( m_lst.GetItemsInContainer() != rI.m_lst.GetItemsInContainer() ) return false;
	for( IT_LStr it1( m_lst ), it2( rI.m_lst ); it1; ++it1, ++it2 )
	  if( !(it1.Current()->m_s == it2.Current()->m_s) ) return false;

	return true;
 }
int TStrItem::operator<( const TStrItem& rI ) const
 {
	if( m_lst.GetItemsInContainer() < 1 ) return true;
	if( rI.m_lst.GetItemsInContainer() < 1 ) return false;

	return *m_lst.PeekHead() < *rI.m_lst.PeekHead();
 }

void StoreList( TFetchedStrContainer* pC, LPSTR lpName, char cL, char cR ) throw( xmsg )
 {
	 fstream ifsGrabOut;
	 ifsGrabOut.open( lpName, ios::out|ios::trunc );
	 if( !ifsGrabOut ) throw xmsg( ("Can't open output file: " + mstring(lpName)).c_str() );
	 mstring sDmp;
	 pC->Dump( sDmp, cL, cR );
	 ifsGrabOut.write( sDmp.c_str(), sDmp.length() );
	 ifsGrabOut.close();
 }



