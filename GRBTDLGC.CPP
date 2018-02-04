/*  Project grab

	 Copyright © 1994. All Rights Reserved.

	 SUBSYSTEM:    grab.exe Application
	 FILE:         grbtdlgc.cpp
	 AUTHOR:


	 OVERVIEW
	 ========
	 Source file for implementation of grabTDLGClient (TDialog).
*/

#include <windows.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "grabapp.h"
#include "grbtdlgc.h"
#include "fcont.h"


typedef struct tagCOPYDATASTRUCT {
	 DWORD dwData;
	 DWORD cbData;
	 void* lpData;
} COPYDATASTRUCT, *PCOPYDATASTRUCT;


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(grabTDLGClient, TDialog)
//{{grabTDLGClientRSP_TBL_BEGIN}}
	 EV_BN_CLICKED(ID_SEARCH_CB, BNClicked),
	 EV_BN_CLICKED(ID_STORE, BNClicked_Store),
//{{grabTDLGClientRSP_TBL_END}}
	 EV_MESSAGE(0x004A /*WM_COPYDATA*/, EvCopyData),
END_RESPONSE_TABLE;


//{{grabTDLGClient Implementation}}


//////////////////////////////////////////////////////////
// grabTDLGClient
// ==========
// Construction/Destruction handling.
static grabTDLGClientXfer grabTDLGClientData;

grabTDLGClient::grabTDLGClient (TWindow *parent, TResId resId, TModule *module)
	 : TDialog(parent, resId, module)
{
//{{grabTDLGClientXFER_USE}}

	 m_uiMsg_GetData = RegisterWindowMessage( "FinalGetdata" );

	 pbOK = new TButton(this, ID_SEARCH_CB);
	 pbCancel = new TButton(this, ID_STORE);
	 m_edMWnd = new TEdit(this, IDC_EDIT_MWIND, SZ_TXT_TTL);
	 m_edIDCB = new TEdit(this, IDC_EDIT_IDCB, SZ_TXT_NUM);
	 m_edWND2 = new TEdit(this, IDC_EDIT_WND2, SZ_TXT_TTL);
	 m_edFName = new TEdit(this, IDC_EDIT_FNAME, SZ_TXT_TTL);

	 m_edClms = new TEdit(this, IDC_EDIT_CLMS, SZ_TXT_NUM);
	 m_edL1 = new TEdit(this, IDC_EDIT_L1, SZ_TXT_NUM);
	 m_edL2 = new TEdit(this, IDC_EDIT_L2, SZ_TXT_NUM);

	 m_edLeft = new TEdit(this, IDC_EDIT_LEFT, 3);
	 m_edRight = new TEdit(this, IDC_EDIT_RIGHT, 3);

	 m_radCombo = new TRadioButton(this, IDC_RADIO_Combo, 0);
	 m_radList = new TRadioButton(this, IDC_RADIO_List, 0);
	 m_radText = new TRadioButton(this, IDC_RADIO_Text, 0);

	 strcpy( grabTDLGClientData.m_edMWnd, "TCD 4.12 (CD 4/99)" );
	 strcpy( grabTDLGClientData.m_edWND2, "Автомобили" );
	 strcpy( grabTDLGClientData.m_edIDCB, "1023" );
	 strcpy( grabTDLGClientData.m_edFName, "E:\\grab.txt" );

	 strcpy( grabTDLGClientData.m_edClms, "3" );
	 strcpy( grabTDLGClientData.m_edL1, "10000" );
	 strcpy( grabTDLGClientData.m_edL2, "100" );

	 strcpy( grabTDLGClientData.m_edLeft, "[" );
	 strcpy( grabTDLGClientData.m_edRight, "]" );

	 grabTDLGClientData.m_radCombo = true;
	 grabTDLGClientData.m_radList = false;
	 grabTDLGClientData.m_radText = false;


	 SetTransferBuffer(&grabTDLGClientData);
//{{grabTDLGClientXFER_USE_END}}

	 // INSERT>> Your constructor code here.
}


grabTDLGClient::~grabTDLGClient ()
{
	 Destroy();

	 // INSERT>> Your destructor code here.
}

void grabTDLGClient::BNClicked ()
 {
	char cBuf[ SZ_TXT_TTL + 1 ];

	m_lstWnd.AddAtTail( new TWndTrack() );
	TWndTrack* pWT = m_lstWnd.PeekTail();
	//pWT->m_sParentTitle = "TCD 4.12 (CD 4/99)";
	*cBuf = 0;
	m_edMWnd->GetText( cBuf, SZ_TXT_TTL );
	pWT->m_sParentTitle = cBuf;

	m_lstWnd.AddAtTail( new TWndTrack() );
	pWT = m_lstWnd.PeekTail();
	*cBuf = 0;
	m_edWND2->GetText( cBuf, SZ_TXT_TTL );
	pWT->m_sParentTitle = cBuf;
	//pWT->m_sParentTitle = "Автомобили";

	m_lstWnd.AddAtTail( new TWndTrack() );
	pWT = m_lstWnd.PeekTail();
	*cBuf = 0;
	m_edIDCB->GetText( cBuf, SZ_TXT_NUM );
	pWT->m_lID = strtol( cBuf, NULL, 0 );

	try {
	  FindControlWnd( m_lstWnd, NULL );
	 }
	catch( xmsg& e )
	 {
		MessageBox( e.why().c_str(), "Error", MB_OK|MB_ICONEXCLAMATION );
	 }

	//mstring str;
	//DumpLst( m_lstWnd, str );
	//MessageBox( str.c_str(), "mm", MB_OK );

	TWndTrack& rTarget = *m_lstWnd.PeekTail();


	//if( m_radCombo->GetCheck() == BF_CHECKED || m_radList->GetCheck() == BF_CHECKED )

	try {
		*cBuf = 0;
		m_edClms->GetText( cBuf, SZ_TXT_NUM );
		MkSubclass1( rTarget, strtol(cBuf, NULL, 0),
		  m_radCombo->GetCheck() == BF_CHECKED ? 'C':(m_radList->GetCheck() == BF_CHECKED ? 'L':'T') );
		long lW1, lW2;
		*cBuf = 0;
		m_edL1->GetText( cBuf, SZ_TXT_NUM );
		lW1 = strtol( cBuf, NULL, 0 );
		*cBuf = 0;
		m_edL2->GetText( cBuf, SZ_TXT_NUM );
		lW2 = strtol( cBuf, NULL, 0 );
		GoThroughtCBN1( rTarget, lW1, lW2 );
		MkUnSubclass1( rTarget );
	 }
	catch( xmsg& e )
	 {
		MessageBox( e.why().c_str(), "Error", MB_OK|MB_ICONEXCLAMATION );
	 }

	 m_lstWnd.Flush( 1 );

 }

void grabTDLGClient::BNClickedCancel ()
 {

 }

void grabTDLGClient::BNClicked_Store ()
{
  try {
	 char cBuf[ SZ_TXT_TTL + 1 ];
	 m_edFName->GetText( cBuf, SZ_TXT_TTL );
	 char cBL[3], cBR[3];

	 m_edLeft->GetText( cBL, 2 );
	 m_edRight->GetText( cBR, 2 );

	 StoreList( GetContainer1(), cBuf, cBL[0], cBR[0] );
	 GetContainer1()->Clear();
  }
  catch( xmsg& e )
	{
	  MessageBox( e.why().c_str(), "Error", MB_OK|MB_ICONEXCLAMATION );
	}
}



LRESULT grabTDLGClient::EvCopyData( WPARAM wParam, LPARAM lParam )
 {
	/*PCOPYDATASTRUCT pDts = (PCOPYDATASTRUCT)lParam;
	try {
		switch( pDts->dwData )
		 {
			case CM_ClearPatch:
			  ClearPatch( (TClearPatch*)pDts->lpData );
			  break;
			case CM_AddWndToPatch:
			  AddWndToPatch( (TAddWndToPatch*)pDts->lpData );
			  break;
			case CM_BuildPatch:
			  BuildPatch( (TBuildPatch*)pDts->lpData );
			  break;
			case CM_Subclass:
			  Subclass( (TSubclass*)pDts->lpData );
			  break;
			case CM_Extract:
			  Extract( (TExtract*)pDts->lpData );
			  break;
			case CM_Unsubclass:
			  Unsubclass( (TUnsubclass*)pDts->lpData );
			  break;
			case CM_StoreInFile:
			  StoreInFile( (TStoreInFile*)pDts->lpData );
			  break;
			case CM_ClearData:
			  ClearData( (TClearData*)pDts->lpData );
			  break;
			case CM_GetData:
			  GetData( (TGetData*)pDts->lpData );
			  break;
		 };
	 }
	catch( xmsg& e )
	 {
		//MessageBox( e.why().c_str(), "Error", MB_OK|MB_ICONEXCLAMATION );
		if( ((TCmdBase*)pDts)->m_hwResponse )
		 {
			COPYDATASTRUCT cd;
			cd.dwData = CM_Error;
			cd.lpData = TErrorDta::CreError( cd.cbData, (LPSTR)e.why().c_str() );
			::SendMessage( ((TCmdBase*)pDts)->m_hwResponse, 0x004A,
			  (WPARAM)HWindow, (LPARAM)(LPVOID)&cd );
			free( cd.lpData );
		 }
		return TRUE;
	 }

	if( ((TCmdBase*)pDts)->m_hwResponse )
	 {
		COPYDATASTRUCT cd;
		cd.dwData = CM_Error;
		cd.cbData = sizeof(TErrorDta);
		cd.lpData = calloc( cd.cbData, 1 );
		::SendMessage( ((TCmdBase*)pDts)->m_hwResponse, 0x004A,
		  (WPARAM)HWindow, (LPARAM)(LPVOID)&cd );
		free( cd.lpData );
	 }*/

	return TRUE;
 }

/* void grabTDLGClient::ClearPatch( TClearPatch* p )
  {
	 switch( p->lIDObj )
	  {
		 case 1:
			m_lstWnd.Flush( 1 );
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
  }
 void grabTDLGClient::AddWndToPatch( TAddWndToPatch* p )
  {
	 TWndTrack* pWT = NULL;
	 switch( p->lIDObj )
	  {
		 case 1:
			m_lstWnd.AddAtTail( new TWndTrack() );
			pWT = m_lstWnd.PeekTail();
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
	 if( pWT )
	  {
		 if( p->cMethodUsed ) pWT->m_sParentTitle = p->cTitle;
		 else pWT->m_lID = p->lIDControl;

		 MessageBox( p->cTitle, "yy", MB_OK );
	  }
  }
 void grabTDLGClient::BuildPatch( TBuildPatch* p )
  {
	 switch( p->lIDObj )
	  {
		 case 1:
			FindControlWnd( m_lstWnd, NULL );
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
  }
 void grabTDLGClient::Subclass( TSubclass* p )
  {
	 TWndTrack& rTarget = *m_lstWnd.PeekTail();
	 switch( p->lIDObj )
	  {
		 case 1:
			MkSubclass1( rTarget, p->lNumberClms );
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
  }
 void grabTDLGClient::Extract( TExtract* p )
  {
	 TWndTrack& rTarget = *m_lstWnd.PeekTail();
	 switch( p->lIDObj )
	  {
		 case 1:
			GoThroughtCBN1( rTarget, p->lW1, p->lW2 );
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
  }
 void grabTDLGClient::Unsubclass( TUnsubclass* p )
  {
	 TWndTrack& rTarget = *m_lstWnd.PeekTail();
	 switch( p->lIDObj )
	  {
		 case 1:
			MkUnSubclass1( rTarget );
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
  }
 void grabTDLGClient::StoreInFile( TStoreInFile* p )
  {
	 switch( p->lIDObj )
	  {
		 case 1:
			StoreList( GetContainer1(), p->cFileName, '[', ']' );
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
  }
 void grabTDLGClient::ClearData( TClearData* p )
  {
	 switch( p->lIDObj )
	  {
		 case 1:
			GetContainer1()->Clear();
			break;
		 default:
			throw xmsg( "Object not implemented" );
	  }
  }
 void grabTDLGClient::GetData( TGetData* p )
  {
  }

*/

 bool grabTDLGClient::Create()
 {
	bool bRes = TDialog::Create();
	return bRes;
 }


LRESULT grabTDLGClient::WindowProc( uint msg, WPARAM wParam, LPARAM lParam )
 {
	/*if( msg == m_uiMsg_GetData )
	 {
		HGLOBAL hg = GetContainer1()->GetPackedData();
		GetContainer1()->Clear();
		return (LRESULT)hg;
	 }*/

	return TDialog::WindowProc( msg, wParam, lParam );
 }

void grabTDLGClient::PerformCreate( int menuOrId )
 {
	TDialog::PerformCreate( menuOrId );
 }
bool grabTDLGClient::PreProcessMsg( MSG& msg )
 {
	return TDialog::PreProcessMsg( msg );
 }



