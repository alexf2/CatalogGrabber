/*  Project grab
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    grab.exe Application
    FILE:         grabapp.cpp
    AUTHOR:       


    OVERVIEW
    ========
    Source file for implementation of grabApp (TApplication).      
*/


#include <owl\owlpch.h>
#pragma hdrstop


#include "grabapp.h"
#include "grbtdlgc.h"                        // Definition of client class.
#include "fcont.h"
#include "ddesrv.h"


//{{grabApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(grabApp, TApplication)
//{{grabAppRSP_TBL_BEGIN}}
	 EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{grabAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//////////////////////////////////////////////////////////
// grabApp
// =====
//
grabApp::grabApp () : TApplication("grab")
{

	 // INSERT>> Your constructor code here.
}


grabApp::~grabApp ()
{
	 // INSERT>> Your destructor code here.
}


//////////////////////////////////////////////////////////
// grabApp
// =====
// Application intialization.
//
void grabApp::InitMainWindow ()
{
	 if (nCmdShow != SW_HIDE)
		  nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

	 SDIDecFrame *frame = new SDIDecFrame(0, GetName(), 0, false);
	 frame->SetFlag(wfShrinkToClient);

	 //
	 // Assign ICON w/ this application.
	 //
	 frame->SetIcon(this, IDI_SDIAPPLICATION);

  
	 SetMainWindow(frame);
}


//{{SDIDecFrame Implementation}}


SDIDecFrame::SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection, TModule *module)
	 : TDecoratedFrame(parent, title, clientWnd == 0 ? new grabTDLGClient(0) : clientWnd, trackMenuSelection, module)
{
	 // INSERT>> Your constructor code here.

}


SDIDecFrame::~SDIDecFrame ()
{
	 // INSERT>> Your destructor code here.

}


void ProcessUnattended( char** );

//////////////////////////////////////////////////////////
// grabApp
// ===========
// Menu Help About grab.exe command
void grabApp::CmHelpAbout ()
{
}
int OwlMain (int iCntP, char** pPrms )
{   //GRAB.EXE /c "TCD 4.12 (CD 4/99)"  "Vehicles"  1019 6 1000 1000 "e:\g1.grb" [ ]
	 /*char* arr[] = {"GRAB.EXE",
	 "/i",
	 "TCD 4.12 (CD 4/99)",
	 "TW         14365P",
	 "Информация об изделии", "5150", "~1", "~1", "#", "6", "100", "1000",
	 "e:\\g1.grb", "[", "]" };
	 iCntP = 11;
	 pPrms = arr;*/

	 try {
		  /*if( iCntP < 3 )
			 throw xmsg( "Need 10 params: /key "
				"Patch to control #  NClm  L1  L2  FileName  Left  Right\n"
				"Where key: c - combobox, l - listbox, t - textbox, i - image\n\n"
				"\tID_of_CB - ID of control\n"
				"\tNClm - number of columns\n"
				"\tL1 and L2 - fake cycles for slow speed\n"
				"\tLeft and Right - start and stop symbol of each field (for show empty fields)\n"
				"Example:\n"
				"\t grab.exe /c \"TCD 4.12 (CD 4/99)\" \"Автомобили\" 1023 # 3 100 1000 \"f:\\tt.txt\" [ ]\n" );
			 */
		  if( iCntP > 1 ) ProcessUnattended( pPrms );
		  else
			{
			  //TDDEServer ddeSrv;
			  //ddeSrv.Init();
			  grabApp    app;
			  int iRet = app.Run();
			  //ddeSrv.Close();
			  return iRet;
			}
	 }
	 catch (xmsg& x) {
		  ::MessageBox(0, x.why().c_str(), "Exception", MB_OK);
	 }

	 return -1;
}

void ProcessUnattended( char** p )
 {
	L_Wnd m_lstWnd;
	TWndTrack* pWT;
	char cMode[ 3 ] =
	 {
		p[1][0], toupper(p[1][1]), 0
	 };

	if( strcmp(cMode, "/C") && strcmp(cMode, "/L") && strcmp(cMode, "/T") && strcmp(cMode, "/I") )
	  throw xmsg( "Bad key: need /c or /l, /t, /i" );

	char **pTmp = p; int iPosDiez = 0;
	for( ; *pTmp; ++pTmp, ++iPosDiez )
	  if( **pTmp == '#' ) break;

	if( !pTmp || **pTmp != '#' )
	  throw xmsg( "Bad paramers format: missing '#'" );

	for( int iPrm = 2; iPrm < iPosDiez; ++iPrm )
	 {
		m_lstWnd.AddAtTail( (pWT=new TWndTrack()) );
		if( p[iPrm][0] >= '0' && p[iPrm][0] <= '9' )
		  pWT->m_lID = strtol( p[ iPrm ], NULL, 0 );
		else if( p[iPrm][0] == '~' )
		 pWT->m_bID = 0, pWT->m_lID = strtol( p[ iPrm ] + 1, NULL, 0 );
		else
		  pWT->m_sParentTitle = p[ iPrm ];
	 }


	FindControlWnd( m_lstWnd, NULL );

	iPosDiez++;

	//mstring str;
	//DumpLst( m_lstWnd, str );
	//MessageBox( str.c_str(), "mm", MB_OK );

	TWndTrack& rTarget = *m_lstWnd.PeekTail();

	if( !strcmp(cMode, "/I") )
	  SetFileName( p[iPosDiez + 3] );
	MkSubclass1( rTarget, strtol(p[iPosDiez], NULL, 0), cMode[1] );

	long lW1, lW2;
	lW1 = strtol( p[iPosDiez + 1], NULL, 0 );
	lW2 = strtol( p[iPosDiez + 2], NULL, 0 );
	GoThroughtCBN1( rTarget, lW1, lW2 );

	MkUnSubclass1( rTarget );

	if( strcmp(cMode, "/I") )
	  StoreList( GetContainer1(), p[iPosDiez + 3], p[iPosDiez + 4][0], p[iPosDiez + 5][0] );
	GetContainer1()->Clear();

	if( *cBufErr )
	  ::MessageBox( NULL, cBufErr, "Error", MB_OK );
 }


void  grabApp::InitInstance()
 {
	TApplication::InitInstance();
	Extra1_Init();
	ddeSrv.Init();
 }

int   grabApp::TermInstance( int status )
 {
   ddeSrv.Close();
	Extra1_Destroy();
	return TApplication::TermInstance( status );
 }
