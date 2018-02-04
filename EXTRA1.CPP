#include <assert.h>
#include <windows.h>
#include <owl\owlpch.h>

#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <fstream.h>


#pragma hdrstop

#include "extra1.h"
#include "Fcont.h"
#include "HOOK\prochook.h"

//static mstring msText;

extern TModule *Module;

static NPHOOKCHILD npHookChild_DrawText;
static FARPROC fpMy_DrawTxtInst = NULL;


static NPHOOKCHILD npHookChild_BeginPaint;
static FARPROC fpMy_BeginPaintInst = NULL;

static NPHOOKCHILD npHookChild_GetClipBox;
static FARPROC fpMy_GetClipBoxInst = NULL;

static PWndProc POldWP = NULL;
static FARPROC fpInst = NULL;

static TFetchedStrContainer fscCont;
static HDC hdcHook = NULL;

static long lActiveItem = -1;

char cMode = 'C';//combobox
//C, L (list box), T, I, M (multiselect list box), W (window with text)

static BOOL bCheckHDC = TRUE;
static UINT uiMsgThrough = WM_DRAWITEM;//for combobox

static LPSTR lpFileName = NULL; //used in /i mode

void SetFileName( LPSTR lp )
 {
	//::MessageBox( NULL, lp, "kk", MB_OK );
	lpFileName = lp;
 }

char cBufErr[ 1024 ];


//static void GetProcs( FARPROC& fpOld, FARPROC& fpNew )

TFetchedStrContainer* GetContainer1()
 {
	return &fscCont;
 }

HDC FAR PASCAL __export New_BeginPaint(
  HWND hwnd,
  LPPAINTSTRUCT lpPaint

)
 {
	ProcUnhook( npHookChild_BeginPaint );

	HDC hdc = BeginPaint( hwnd, lpPaint );
	SelectClipRgn( hdc, NULL );
	lpPaint->rcPaint.left = lpPaint->rcPaint.top = 0;
	lpPaint->rcPaint.right = lpPaint->rcPaint.bottom = 10000;

	ProcHook( npHookChild_BeginPaint );

	return hdc;
 }

int FAR PASCAL __export New_GetClipBox( HDC hdc, LPRECT lpRc )
 {
	ProcUnhook( npHookChild_GetClipBox );
	int iRet = GetClipBox( hdc, lpRc );
	ProcHook( npHookChild_GetClipBox );

	//SelectClipRgn( hdc, NULL );

	if( cMode != 'C' )
	  lpRc->left = lpRc->top = 0,
	  lpRc->right = lpRc->bottom = 32700;
	  //lpRc->right = lpRc->bottom = 30000;
	else return iRet;


	return SIMPLEREGION;
 }

int FAR PASCAL __export New_StretchDIBits(
	 HDC  hdc,	// handle of device context
	 int  XDest,	// x-coordinate of upper-left corner of dest. rect.
	 int  YDest,	// y-coordinate of upper-left corner of dest. rect.
	 int  nDestWidth,	// width of destination rectangle
	 int  nDestHeight,	// height of destination rectangle
	 int  XSrc,	// x-coordinate of upper-left corner of source rect.
	 int  YSrc,	// y-coordinate of upper-left corner of source rect.
	 int  nSrcWidth,	// width of source rectangle
	 int  nSrcHeight,	// height of source rectangle
	 void  *lpBits,	// address of bitmap bits
	 BITMAPINFO *  lpBitsInfo,	// address of bitmap data
	 UINT  iUsage,	// usage
	 DWORD  dwRop 	// raster operation code
	)
 {
	int iRet;

  ProcUnhook( npHookChild_DrawText );

  //::MessageBox( NULL, "ppp", "drw", MB_OK );
  //for( int k = 0; k < 112; ++k )
	 //MessageBeep( -1 );


  iRet = StretchDIBits(
	 hdc,	// handle of device context
	 XDest,	// x-coordinate of upper-left corner of dest. rect.
	 YDest,	// y-coordinate of upper-left corner of dest. rect.
	 nDestWidth,	// width of destination rectangle
	 nDestHeight,	// height of destination rectangle
	 XSrc,	// x-coordinate of upper-left corner of source rect.
	 YSrc,	// y-coordinate of upper-left corner of source rect.
	 nSrcWidth,	// width of source rectangle
	 nSrcHeight,	// height of source rectangle
	 lpBits,	// address of bitmap bits
	 lpBitsInfo,	// address of bitmap data
	 iUsage,	// usage
	 dwRop );

  /*char cc[ 1024 ];
  sprintf( cc, "Planes = %u, BitCount = %u, Compr = %lu, ClrUsed = %lu, "
	 "ClrImp = %lu; ImgSize = %lu", lpBitsInfo->bmiHeader.biPlanes,
	 lpBitsInfo->bmiHeader.biBitCount,
	 lpBitsInfo->bmiHeader.biCompression,
	 lpBitsInfo->bmiHeader.biClrUsed,
	 lpBitsInfo->bmiHeader.biClrImportant,
	 lpBitsInfo->bmiHeader.biSizeImage );

  ::MessageBox( NULL, cc, "ppp", MB_OK );*/
  //lpFileName

  int iNumClr;
  switch( lpBitsInfo->bmiHeader.biBitCount )
	{
	  case 1:
		 iNumClr = 2; break;
	  case 4:
		 iNumClr = 16; break;
	  case 8:
		 iNumClr = 256; break;
	  case 24:
		 iNumClr = 0; break;
	  default:
		strcpy( cBufErr, "Not supported color resolution" );
		return iRet;
	}
  DWORD dwHdrSz = sizeof(BITMAPINFO) + sizeof(RGBQUAD)*iNumClr - (iNumClr ? sizeof(RGBQUAD):0);


  /*char cN[ 1024 ];
  static int iCntCN = 0;
  sprintf( cN, "%s%u%s", "g:\\ii", iCntCN++, ".bmp" );*/
  HFILE hf = _lcreat( lpFileName, 0 );

  if( hf == HFILE_ERROR )
	{
	  sprintf( cBufErr, "Cann't create file: %s", lpFileName );
	  return iRet;
	}

  BITMAPFILEHEADER bh; memset( &bh, 0, sizeof(BITMAPFILEHEADER) );
  bh.bfType = 0x4D42;
  bh.bfSize = sizeof(BITMAPFILEHEADER) + dwHdrSz + lpBitsInfo->bmiHeader.biSizeImage;
  bh.bfOffBits = sizeof(BITMAPFILEHEADER) + dwHdrSz;

  if( _lwrite(hf, &bh, sizeof(bh)) != sizeof(bh) )
	{
	  _lclose( hf );
	  sprintf( cBufErr, "Error write to file: %s", lpFileName );
	  return iRet;
	}

  if( _lwrite(hf, lpBitsInfo, dwHdrSz) != dwHdrSz )
	{
	  _lclose( hf );
	  sprintf( cBufErr, "Error write to file: %s", lpFileName );
	  return iRet;
	}

  if( _lwrite(hf, lpBits, lpBitsInfo->bmiHeader.biSizeImage) != lpBitsInfo->bmiHeader.biSizeImage )
	{
	  _lclose( hf );
	  sprintf( cBufErr, "Error write to file: %s", lpFileName );
	  return iRet;
	}

  _lclose( hf );

  ProcHook( npHookChild_DrawText );

  return iRet;
 }


BOOL FAR PASCAL __export New_TextOut(

	 HDC hdc,	// handle of device context
	 int nXStart,	// x-coordinate of starting position
	 int nYStart,	// y-coordinate of starting position
	 LPSTR lp,	// address of string
	 int cbString 	// number of characters in string
	)
 {
	int iRet;
	ProcUnhook( npHookChild_DrawText );

//sss += "tt \n";
//sss += lp;

	 if( lActiveItem != -1 && ((bCheckHDC && hdcHook == hdc) || !bCheckHDC) )
	  {

			 if( cbString != -1 )
			  {
				 /*LPSTR lpS = (LPSTR)malloc( cbString + 1 );
				 strncpy( lpS, lp, cbString ); lpS[ cbString ] = 0;*/

				 RECT rr = {nXStart, nYStart, nXStart + 1, nYStart + 1};
				 fscCont.AddStrSz( lp, &rr, cbString );
				 //free( lpS );
			  }
			 else
			  {
				 RECT rr = {nXStart, nYStart, nXStart + 1, nYStart + 1};
				 fscCont.AddStr( lp, &rr );
			  }

	  }

	 iRet = TextOut( hdc, nXStart, nYStart, lp, cbString );

	 ProcHook( npHookChild_DrawText );

	 return iRet;
 }

int FAR PASCAL __export New_DrawText( HDC hDC, LPCSTR lp, int nCnt, LPRECT pR, UINT fmt )
 {
	 int iRet;

	 ProcUnhook( npHookChild_DrawText );

	 //::MessageBox( NULL, lp, "drw", MB_OK );

//sss += "tt ";
//sss += lp;

	 if( lActiveItem != -1 && ((bCheckHDC && hdcHook == hDC) || !bCheckHDC) )
	  {
		 if( !(pR->left > pR->right || pR->left > 2000) )
		  {
			 if( nCnt != -1 )
			  {
				 /*LPSTR lpS = (LPSTR)malloc( nCnt + 1 );
				 strncpy( lpS, lp, nCnt ); lpS[ nCnt ] = 0;*/

				 //fscCont.AddStr( lpS, pR, lActiveItem );
				 fscCont.AddStrSz( lp, pR, lActiveItem, nCnt );
				 //free( lpS );
			  }
			 else
				fscCont.AddStr( lp, pR, lActiveItem );
		  }
	  }

	 iRet = DrawText( hDC, lp, nCnt, pR, fmt );

	 ProcHook( npHookChild_DrawText );

	 return iRet;
 }

static void GetProcs( FARPROC& fpOld, FARPROC& fpNew, UINT* uiMsgTh, BOOL* bChkHDC )
 {
	switch( cMode )
	 {
		case 'C': //combobox
		  fpOld = (FARPROC)DrawText, fpNew = (FARPROC)New_DrawText;
		  if( uiMsgTh ) *uiMsgTh = WM_DRAWITEM;
		  if( bChkHDC ) *bChkHDC = TRUE;
		  break;

		case 'L': //listbox
		  //fpOld = (FARPROC)ExtTextOut, fpNew = (FARPROC)New_ExtTextOut;
		  //fpOld = (FARPROC)TextOut, fpNew = (FARPROC)New_TextOut;
		  fpOld = (FARPROC)DrawText, fpNew = (FARPROC)New_DrawText;
		  if( uiMsgTh ) *uiMsgTh = 0x407; //the WM_USER + 7
		  if( bChkHDC ) *bChkHDC = FALSE;
		  break;

		case 'M': //multiselect listbox
		  fpOld = (FARPROC)DrawText, fpNew = (FARPROC)New_DrawText;
		  if( uiMsgTh ) *uiMsgTh = WM_PAINT;
		  if( bChkHDC ) *bChkHDC = FALSE;
		  break;

		case 'T': //textbox
		  fpOld = (FARPROC)DrawText, fpNew = (FARPROC)New_DrawText;
		  if( uiMsgTh ) *uiMsgTh = WM_PAINT;
		  if( bChkHDC ) *bChkHDC = TRUE;
		  break;

		case 'W': //window with text
		  fpOld = (FARPROC)TextOut, fpNew = (FARPROC)New_TextOut;
		  if( uiMsgTh ) *uiMsgTh = WM_PAINT;
		  if( bChkHDC ) *bChkHDC = FALSE;
		  break;

		case 'I': //image
		  fpOld = (FARPROC)StretchDIBits, fpNew = (FARPROC)New_StretchDIBits;

		  if( uiMsgTh ) *uiMsgTh = WM_PAINT;
		  if( bChkHDC ) *bChkHDC = FALSE;
		  break;

		default:
		  fpOld = NULL, fpNew = NULL;
		  break;
	 }
 }



 LRESULT FAR PASCAL __export MyWindowProc(
	 HWND hwnd,	// handle of window
	 UINT uMsg,	// message identifier
	 WPARAM wParam,	// first message parameter
	 LPARAM lParam 	// second message parameter
	)
 {
	static bool bLock = false;


/*static char cc[ 1024 ];
sprintf( cc, "[%X] ", (int)uMsg );
sss += cc;

return POldWP( hwnd, uMsg, wParam, lParam );*/

//if( uMsg == WM_PAINT ) SelectClipRgn( (HDC)wParam, NULL );

	if( uMsg == uiMsgThrough && !bLock )
	//if( !bLock )
	 {
		bLock = true;
		if( uMsg == WM_DRAWITEM )
		  hdcHook = ((LPDRAWITEMSTRUCT)lParam)->hDC;
		else if( uMsg == WM_PAINT )
		  hdcHook = (HDC)wParam;
		else
		  hdcHook = NULL;


		LRESULT lRes;

		FARPROC fpOld, fpNew;
		GetProcs( fpOld, fpNew, NULL, NULL );
		npHookChild_DrawText = SetProcAddress( fpOld, fpMy_DrawTxtInst, FALSE );

		//------------
		//npHookChild_BeginPaint = SetProcAddress( (FARPROC)BeginPaint, fpMy_BeginPaintInst, FALSE );
		npHookChild_GetClipBox = SetProcAddress( (FARPROC)GetClipBox, fpMy_GetClipBoxInst, FALSE );


		lRes = POldWP( hwnd, uMsg, wParam, lParam );

		SetProcRelease( npHookChild_GetClipBox );
		//SetProcRelease( npHookChild_BeginPaint );
		SetProcRelease( npHookChild_DrawText );

		hdcHook = NULL;
		bLock = false;


		return lRes;
	 }
	else
	  return POldWP( hwnd, uMsg, wParam, lParam );
 }


void MkSubclass1( RTWndTrack rI, int iDiv, char cMode_ /*c - combobox OR l - listbox*/ )  throw(xmsg)
 {
	*cBufErr = 0; //clear error

	assert( rI.m_hw );

	fscCont.Clear();
	fscCont.SetDiv( iDiv );

	cMode = cMode_;
	lActiveItem = -1;

	if( cMode_ == 'T' ) return;

	FARPROC fpOld, fpNew;
	GetProcs( fpOld, fpNew, &uiMsgThrough, &bCheckHDC );

	if( POldWP ) throw xmsg( "Hook is busy" );
	//fpInst = MakeProcInstance( (FARPROC)MyWindowProc, Module->GetInstance() );
	fpMy_DrawTxtInst = MakeProcInstance( fpNew, Module->GetInstance() );
	POldWP = (PWndProc)SetWindowLong( rI.m_hw, GWL_WNDPROC, (LONG)(FARPROC)fpInst );

	//----------
	//fpMy_BeginPaintInst = MakeProcInstance( (FARPROC)New_BeginPaint, Module->GetInstance() );
	//fpMy_GetClipBoxInst = MakeProcInstance( (FARPROC)New_GetClipBox, Module->GetInstance() );
 }

void MkUnSubclass1( RTWndTrack rI )  throw(xmsg)
 {
	assert( rI.m_hw );

	if( cMode == 'T' ) return;

	if( !POldWP ) throw xmsg( "Hook is null" );
	SetWindowLong( rI.m_hw, GWL_WNDPROC, (LONG)(FARPROC)POldWP ), POldWP = NULL;
	FreeProcInstance( fpMy_DrawTxtInst ), fpMy_DrawTxtInst = NULL;
	//FreeProcInstance( fpInst ), fpInst = NULL;

	//FreeProcInstance( fpMy_BeginPaintInst ), fpMy_BeginPaintInst = NULL;
	//FreeProcInstance( fpMy_GetClipBoxInst ), fpMy_GetClipBoxInst = NULL;

	//fscCont.EndStoring();
 }

void GoThroughtCBN1( RTWndTrack rI, long lW1, long lW2 )  throw(xmsg)
 {
	assert( rI.m_hw );

	lActiveItem = -1;

	switch( cMode )
	 {
		case 'C':
		 {
			LRESULT lCnt;

			lCnt = SendMessage( rI.m_hw, CB_GETCOUNT, 0, 0 );
			fscCont.InitOn( lCnt );
			SendMessage( rI.m_hw, CB_SHOWDROPDOWN, (WPARAM)TRUE, 0 );

			for( long l = 0; l < lCnt; ++l )
			 {
				lActiveItem = -1;
				SendMessage( rI.m_hw, CB_SETCURSEL, (WPARAM)-1, 0 ),
				SendMessage( rI.m_hw, CB_SETCURSEL, (WPARAM)l, 0 ),
				SendMessage( rI.m_hw, CB_SETCURSEL, (WPARAM)-1, 0 );
				lActiveItem = l;

				SendMessage( rI.m_hw, CB_SETCURSEL, (WPARAM)l, 0 );

				fscCont.EndStoringString( lActiveItem );

				for( long ll = 0; ll < lW1; ++ll )
				 {
					for( long ll2 = 0; ll2 < lW2; ++ll2 )
					 {
						int a = 12;
						a = a * a + ll;
					 }
				 }
			 }
			lActiveItem = -1;
			SendMessage( rI.m_hw, CB_SHOWDROPDOWN, (WPARAM)FALSE, 0 );

			fscCont.InsertSpaces();
		 };
		break;

		case 'L':
		 {
			LRESULT lCnt;

			lCnt = SendMessage( rI.m_hw, LB_GETCOUNT, 0, 0 );
			fscCont.InitOn( lCnt );

			for( long l = 0; l < lCnt; ++l )
			 {
				lActiveItem = -1;
				SendMessage( rI.m_hw, LB_SETCURSEL, (WPARAM)-1, 0 ),
				SendMessage( rI.m_hw, LB_SETCURSEL, (WPARAM)l, 0 ),
				SendMessage( rI.m_hw, LB_SETCURSEL, (WPARAM)-1, 0 );
				lActiveItem = l;

				SendMessage( rI.m_hw, LB_SETCURSEL, (WPARAM)l, 0 );

				fscCont.EndStoringString( lActiveItem );

				for( long ll = 0; ll < lW1; ++ll )
				 {
					for( long ll2 = 0; ll2 < lW2; ++ll2 )
					 {
						int a = 12;
						a = a * a + ll;
					 }
				 }
			 }
			lActiveItem = -1;

			fscCont.InsertSpaces();
		 };
		break;

		case 'M':
		 {
			LRESULT lCnt;

			lCnt = SendMessage( rI.m_hw, LB_GETCOUNT, 0, 0 );
			fscCont.InitOn( lCnt );


			if( lCnt > 0 )
			  SendMessage( rI.m_hw, LB_SELITEMRANGE, (WPARAM)FALSE, MAKELPARAM((WORD)0, (WORD)(lCnt - 1)) );

			for( long l = 0; l < lCnt; ++l )
			 {
				lActiveItem = -1;
				//SendMessage( rI.m_hw, LB_SELITEMRANGE, (WPARAM)FALSE, MAKELPARAM((WORD)l, (WORD)l) ),
				SendMessage( rI.m_hw, LB_SELITEMRANGE, (WPARAM)TRUE, MAKELPARAM((WORD)l, (WORD)(l)) ),
				SendMessage( rI.m_hw, LB_SELITEMRANGE, (WPARAM)FALSE, MAKELPARAM((WORD)l, (WORD)(l)) );
				SendMessage( rI.m_hw, LB_SETTOPINDEX, (WPARAM)l, 0 );
				UpdateWindow( rI.m_hw );

				lActiveItem = l;
				SendMessage( rI.m_hw, LB_SELITEMRANGE, (WPARAM)TRUE, MAKELPARAM((WORD)l, (WORD)(l)) );
				UpdateWindow( rI.m_hw );

				/*for( long ll = 0; ll < lW1; ++ll )
				 {
					for( long ll2 = 0; ll2 < lW2; ++ll2 )
					 {
						int a = 12;
						a = a * a + ll;
					 }
				 }*/

				lActiveItem = -1;
				SendMessage( rI.m_hw, LB_SELITEMRANGE, (WPARAM)FALSE, MAKELPARAM((WORD)l, (WORD)(l)) );

				fscCont.EndStoringString( lActiveItem );

				for( long ll = 0; ll < lW1; ++ll )
				 {
					for( long ll2 = 0; ll2 < lW2; ++ll2 )
					 {
						int a = 12;
						a = a * a + ll;
					 }
				 }
			 }
			lActiveItem = -1;

			fscCont.InsertSpaces();

			/*mstring bbb;
			fscCont.Dump( bbb, '[', ']' );
			::MessageBox( NULL, bbb.c_str(), "kk", MB_OK );*/
		 };
		break;

		case 'T':
		 {
			fscCont.InitOn( 1 );

			unsigned long sz = SendMessage( rI.m_hw, WM_GETTEXTLENGTH, 0, 0 );
			//if( sz < 1 || sz > 0x10000 ) sz = 65535;
         sz = 20 * 1024;
			LPSTR lpB = (LPSTR)calloc( ++sz, 1 );
			*lpB = 0;

			LRESULT lCnt = SendMessage( rI.m_hw, WM_GETTEXT, sz, (LPARAM)lpB );

			lActiveItem = 0;
			RECT r = {0, 0, 10, 10 };
			fscCont.AddStr( lpB, &r, lActiveItem );
			fscCont.EndStoringString( lActiveItem );
			free( lpB );
			//fscCont.InsertSpaces();
			lActiveItem = -1;
		 };
		break;

		case 'W':
		 {
			fscCont.InitOn( 1 );
//::MessageBox( NULL, "ppp2", "drw2", MB_OK );

			InvalidateRect( rI.m_hw, NULL, FALSE );
			lActiveItem = 0;
			UpdateWindow( rI.m_hw );

			/*RECT r = {0, 0, 10, 10 };
			fscCont.AddStr( msText.c_str(), &r, lActiveItem );
			msText = "";
			fscCont.EndStoringString( lActiveItem );*/

			//fscCont.InsertSpaces();
			lActiveItem = -1;
		 };
		break;

		case 'I':
		 {
			InvalidateRect( rI.m_hw, NULL, TRUE );
			UpdateWindow( rI.m_hw );
		 };
		break;
	 };

/*fstream fss( "g:\\fff.txt", ios::out );
fss << sss.c_str();
fss.flush();
fss.close();*/

 }

void Extra1_Init()
 {
	//HMODULE hh = Module->GetInstance();
	fpMy_GetClipBoxInst = MakeProcInstance( (FARPROC)New_GetClipBox, Module->GetInstance() );
	fpInst = MakeProcInstance( (FARPROC)MyWindowProc, Module->GetInstance() );
	fpMy_BeginPaintInst = MakeProcInstance( (FARPROC)New_BeginPaint, Module->GetInstance() );
 }

void Extra1_Destroy()
 {
	if( fpMy_GetClipBoxInst )
	  FreeProcInstance( fpMy_GetClipBoxInst ), fpMy_GetClipBoxInst = NULL;
	if( fpInst )
	  FreeProcInstance( fpInst ), fpInst = NULL;
	if( fpMy_BeginPaintInst )
	  FreeProcInstance( fpMy_BeginPaintInst ), fpMy_BeginPaintInst = NULL;
 }

