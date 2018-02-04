#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <fstream.h>
#include <assert.h>
#include <cstring.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "ddesrv.h"
#include "extra1.h"
#include "fcont.h"

static HSZ hszServer = NULL;
static HSZ hszTopic  = NULL;
static HSZ hszItem1  = NULL;
static HSZ hszItem2  = NULL;

static DWORD dwInst = 0;
static FARPROC lpDdeProc = NULL;

static HCONV hConvApp1 = NULL;
//static HCONV hConvApp2 = NULL;

static long lW1 = 1, lW2 = 1;
static L_Wnd m_lstWnd;


static LPBYTE GetResults( LPDWORD lpSz )
 {
	 LPBYTE lpRet = NULL;
	 //lpRet[0] - result type:
	 //	0 - error message (zero terminated string)
	 //   1 - text box content (zero terminated string)
	 //   2 - table:  nRows:DWORD, nColumns:DWORD, [sizeStr:WORD, str. bytes, sizeStr:WORD, str. bytes ... ]
	 //   3 - HWND
	 if( *cBufErr )
	  {
		 *lpSz = strlen( cBufErr ) + 2;
		 lpRet = (LPBYTE)calloc( *lpSz, 1 );
		 lpRet++; // lpRet[0]
		 strcpy( lpRet, cBufErr );
		 --lpRet;
	  }
	 else if( cMode == 'T' )
	  {
		 mstring sDmp;
		 GetContainer1()->Dump( sDmp, 0, 0 );
		 *lpSz = sDmp.length() + 2;
		 lpRet = (LPBYTE)calloc( *lpSz, 1 );
		 *lpRet++ = 1;
		 strcpy( lpRet, sDmp.c_str() );
		 --lpRet;
	  }
	 else if( cMode == 'C' || cMode == 'L' || cMode == 'M' || cMode == 'W' )
	  {
		 lpRet = GetContainer1()->DumpCompact( lpSz );
	  }
	 else if( cMode == 'I' )
	  {
		 LPCSTR lpMsg = "<OK>";
		 *lpSz = strlen( lpMsg ) + 2;
		 lpRet = (LPBYTE)calloc( *lpSz, 1 );
		 *lpRet++ = 1;
		 strcpy( lpRet, lpMsg );
		 --lpRet;
	  }
	 else
	  {
		 LPCSTR lpMsg = "Dump not implemented";
		 *lpSz = strlen( lpMsg ) + 2;
		 lpRet = (LPBYTE)calloc( *lpSz, 1 );
		 *lpRet++ = 1;
		 strcpy( lpRet, lpMsg );
		 --lpRet;
	  }
	 //C, L, T, I, E

	 return lpRet;
 }

LPBYTE GetWindowM( LPDWORD lpSz )
 {
	if( m_lstWnd.GetItemsInContainer() < 1 )
	 {
		strcpy( cBufErr, "No windows found" );
		return NULL;
	 }

	LPBYTE lpRet = NULL;

	*lpSz = sizeof(DWORD) + 1;
	lpRet = (LPBYTE)calloc( *lpSz, 1 );
	*lpRet++ = 3; //HWND

	TWndTrack& rTarget = *m_lstWnd.PeekTail();
	*(LPDWORD)lpRet = (DWORD)rTarget.m_hw;

	/*char hh[ 128 ]; sprintf( hh, "%u", (unsigned)rTarget.m_hw );
	MessageBox( NULL, hh, "ll", MB_OK );*/

	return lpRet - 1;
 }

static void Execute2( LPSTR arr[], int iCnt )
 {
	/*mstring mm;
	for( int ii = 0; ii < iCnt; ++ii )
	  mm += mstring("'") + arr[ ii ] + mstring("'") + mstring("\n");
	MessageBox( NULL, mm.c_str(), "MM", MB_OK );*/

	char c = toupper( arr[0][0] );
	if( c != 'D' && c != 'F' && c != 'E' && m_lstWnd.GetItemsInContainer() < 1 )
	 {
		strcpy( cBufErr, "No window captured on server" );
		return;
	 }

	cMode = c;

	switch( c )
	 {
		case 'F': //find window
		 {
			if( iCnt < 2 )
			 {
				strcpy( cBufErr, "Bad number parameters" );
				return;
			 }
			int iSkip = 0;
			for( int i = 1; i < iCnt; ++i )
			  if( arr[i][0] == '#' && (i == 1 || arr[i - 1][0] == '#') ) iSkip++;

			if( m_lstWnd.GetItemsInContainer() < iSkip )
			 {
				strcpy( cBufErr, "Cann't skip: too short list of windows" );
				return;
			 }

			if( iSkip == 0 ) m_lstWnd.Flush( 1 );
			else
			 {
				for( i = m_lstWnd.GetItemsInContainer() - iSkip; i > 0; --i )
				  m_lstWnd.DetachAtTail( 1 );
			 }


			for( i = iSkip + 1; i < iCnt; ++i )
			 {
				TWndTrack* pWT;

				m_lstWnd.AddAtTail( (pWT=new TWndTrack()) );
				if( arr[i][0] >= '0' && arr[i][0] <= '9' )
				  pWT->m_lID = strtol( arr[i], NULL, 0 );
				else if( arr[i][0] == '~' )
				 pWT->m_bID = 0, pWT->m_lID = strtol( arr[i] + 1, NULL, 0 );
				else
				  pWT->m_sParentTitle = arr[i];
			 }

			 FindControlWnd( m_lstWnd, NULL );
		 }
		break;

		case 'C':
		case 'L':
		case 'T':
		case 'W':
		case 'M':
		  {
			 if( iCnt != 2 )
			  {
				 strcpy( cBufErr, "Bad number parameters: NClm need" );
				 return;
			  }

			 GetContainer1()->Clear();

			 TWndTrack& rTarget = *m_lstWnd.PeekTail();
			 MkSubclass1( rTarget, strtol(arr[1], NULL, 0), c );
			 GoThroughtCBN1( rTarget, lW1, lW2 );
			 MkUnSubclass1( rTarget );
		  }
		 break;

		case 'I': //grab image
		  {
			if( iCnt != 2 )
			 {
				strcpy( cBufErr, "Bad number parameters: File Name need" );
				return;
			 }

			 GetContainer1()->Clear();

			 TWndTrack& rTarget = *m_lstWnd.PeekTail();
			 SetFileName( arr[1] );
			 MkSubclass1( rTarget, 1, c );
			 GoThroughtCBN1( rTarget, lW1, lW2 );
			 MkUnSubclass1( rTarget );
		  }
		 break;

		case 'D': //set delay
			{
			  if( iCnt != 3 )
				{
				  strcpy( cBufErr, "Bad number parameters: L1 L2  need" );
				  return;
				}
			  lW1 = strtol( arr[1], NULL, 0 );
			  lW2 = strtol( arr[2], NULL, 0 );
			}
		  break;

		case 'E': //exit
			{
			  if( iCnt != 1 )
				{
				  strcpy( cBufErr, "Bad number parameters: no parameters needed" );
				  return;
				}
			  PostQuitMessage( 0 );
			}
		  break;

		default:
		  strcpy( cBufErr, "Bad command - need C, L, T, I, D" );
		  break;
	 }
 }

// find window: f # # "Wnd1 cap" "Wnd2 cap" 1299 ~1 ~2
//		Where: # - empty window (keep old window)
//		       ~ - prefix for relative position

// set delay:   d L1 L2
// combobox:    c NClm
// listbox:     l NClm
// m-s listbox: m NClm
// textbox:     t
// image:       i "file name"
// exit:        e
// get window:  g
#define MAX_CMDS 50
static void ExecuteCommand( LPBYTE lpDta )//zero terminated string
 {
	*cBufErr = 0;

	LPSTR acParams[ MAX_CMDS ];
	memset( acParams, 0, sizeof(LPSTR) * MAX_CMDS );

	int iParamCount = 0;
	LPSTR lp1 = (LPSTR)lpDta;
	LPSTR lp2 = lp1;
	bool bFlIn = false;
	for( ; 1; ++lp2 )
	 {
		if( *lp2 == '\"' ) { bFlIn = !bFlIn; if( bFlIn ) lp1 = lp2; continue; }
		else if( *lp2 == ' ' )
		 {
			if( bFlIn ) continue;
			if( *lp1 == ' ' ) continue;
		 }
		else if( *lp2 != 0 ) { if( *lp1 == ' ' ) lp1 = lp2; continue; }

		if( *lp2 == 0 && *lp1 == ' ' ) break;

		if( *lp1 == '\"' ) ++lp1;
		LPSTR lp3 = (*(lp2 - 1) == '\"' ? lp2 - 2:lp2 - 1);
		int iSz = lp3 < lp1 ? 1:(lp3 - lp1 + 2);
		acParams[ iParamCount ] = (LPSTR)calloc( iSz, 1 );
		if( iSz > 1 ) memcpy( acParams[iParamCount], lp1, iSz - 1 );
		iParamCount++;

		if( iParamCount > MAX_CMDS )
		 {
			strcpy( cBufErr, "Too many parameters" );
			return;
		 }

		lp1 = lp2;

		if( *lp2 == 0 ) break;
	 }

	if( iParamCount < 1 )
	 {
		strcpy( cBufErr, "Empty command string" );
		return;
	 }

	try {
	  Execute2( acParams, iParamCount );
	 }
	catch( xmsg& x ) {
		strcpy( cBufErr, x.why().c_str() );
	 }

	for( int i = 0; acParams[i] && i < MAX_CMDS; ++i )
	  free( acParams[i] );
 }

HDDEDATA EXPENTRY _export DDEServerCallback( WORD wType, WORD wFmt, HCONV hConv,
  HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2 )
 {
	switch( wType )
	 {
		case XTYP_CONNECT:
		  if( hsz2 == hszServer && hsz1 == hszTopic )
			 return (HDDEDATA)TRUE;
		  else return (HDDEDATA)FALSE;

		case XTYP_REQUEST:
			{
			  if( hszTopic != hsz1 )
				{
				  strcpy( cBufErr, "Bad topic or item of DDE server" );
				  return DDE_FNOTPROCESSED;
				}

			  DWORD dwSz;
			  LPBYTE lpDta;

			  if( hsz2 == hszItem2 ) //result
				{
				  lpDta = GetResults( &dwSz );
				  if( !lpDta )
					{
					  strcpy( cBufErr, "No memory" );
					  return DDE_FNOTPROCESSED;
					}
				  GetContainer1()->Clear();
				}
			  else if( hsz2 == hszItem1 ) //window
				{
				  lpDta = GetWindowM( &dwSz );
				  if( !lpDta ) return DDE_FNOTPROCESSED;
				}
			  else
				{
				  strcpy( cBufErr, "No such item" );
				  return DDE_FNOTPROCESSED;
				}

			  hData = DdeCreateDataHandle( dwInst, lpDta, dwSz, 0, hszItem2, CF_TEXT, 0 );
			  free( lpDta );
			  return hData;
			}
		  break;

		case XTYP_EXECUTE:
		  {
			  if( hszTopic != hsz1 )
				{
				  strcpy( cBufErr, "Bad topic of DDE server" );
				  return DDE_FNOTPROCESSED;
				}

			  DWORD dwDtaLen;
			  LPBYTE lpDta = DdeAccessData( hData, &dwDtaLen );
			  if( !lpDta )
				{
				  strcpy( cBufErr, "DDE server cann't access to command" );
				  return DDE_FNOTPROCESSED;
				}
			  try {
				  ExecuteCommand( lpDta );
				  DdeUnaccessData( hData );
				  return (HDDEDATA)DDE_FACK;
				}
			  catch( xmsg& x ) {
				 strcpy( cBufErr, x.why().c_str() );
				 DdeUnaccessData( hData );
				 return (HDDEDATA)DDE_FACK;
				}
			}
		  break;

		case XTYP_POKE:
		  break;

		case XTYP_CONNECT_CONFIRM:
		  hConvApp1 = hConv;
		  break;

		case XTYP_DISCONNECT:
		  hConvApp1 = NULL;
		  break;

		case XTYP_ERROR:
		  break;
	 }
	return NULL;
 }


LPCSTR TDDEServer::m_lpSERVER = "GRABBER";
LPCSTR TDDEServer::m_lpTOPIC  = "MAIN";
LPCSTR TDDEServer::m_lpITEM1  = "WINDOW";
LPCSTR TDDEServer::m_lpITEM2  = "RESULT";


TDDEServer::~TDDEServer()
 {
	//Close();
 }

void TDDEServer::Init()
 {
 
	lpDdeProc = MakeProcInstance( (FARPROC)DDEServerCallback, Module->GetInstance() );
	if( DdeInitialize((LPDWORD)&dwInst, (PFNCALLBACK)lpDdeProc, APPCLASS_STANDARD, 0) != DMLERR_NO_ERROR )
	 {
		FreeProcInstance( lpDdeProc ), lpDdeProc = NULL;
		throw xmsg( "Cann't initialize DDE" );
	 }

	hszServer = DdeCreateStringHandle( dwInst, TDDEServer::m_lpSERVER, CP_WINANSI );
	hszTopic  = DdeCreateStringHandle( dwInst, TDDEServer::m_lpTOPIC, CP_WINANSI );
	hszItem1  = DdeCreateStringHandle( dwInst, TDDEServer::m_lpITEM1, CP_WINANSI );
	hszItem2  = DdeCreateStringHandle( dwInst, TDDEServer::m_lpITEM2, CP_WINANSI );

	if( !DdeNameService(dwInst, hszServer, (HSZ)NULL, DNS_REGISTER) )
	 {
		DdeFreeStringHandle( dwInst, hszServer );
		DdeFreeStringHandle( dwInst, hszTopic );
		DdeFreeStringHandle( dwInst, hszItem1 );
		DdeFreeStringHandle( dwInst, hszItem2 );
		hszServer = hszTopic = hszItem1 = hszItem2 = NULL;

		DdeUninitialize( dwInst ), dwInst = 0;
		FreeProcInstance( lpDdeProc ), lpDdeProc = NULL;

		throw xmsg( "Cann't register DDE server" );
	 }

	m_bInit = true;
 }
void TDDEServer::Close()
 {
	if( !m_bInit ) return;

	if( hConvApp1 ) DdeDisconnect( hConvApp1 ), hConvApp1 = NULL;

	if( hszServer )
	 {
		DdeNameService( dwInst, hszServer, (HSZ)NULL, DNS_UNREGISTER );

		DdeFreeStringHandle( dwInst, hszServer );
		DdeFreeStringHandle( dwInst, hszTopic );
		DdeFreeStringHandle( dwInst, hszItem1 );
		DdeFreeStringHandle( dwInst, hszItem2 );
		hszServer = hszTopic = hszItem1 = hszItem2 = NULL;
	 }

   DdeUninitialize( dwInst ), dwInst = 0;
	FreeProcInstance( lpDdeProc ), lpDdeProc = NULL;

	m_bInit = false;
 }

