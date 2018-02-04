#if !defined(__grbtdlgc_h)              // Sentry, use file only if it's not already included.
#define __grbtdlgc_h

/*  Project grab
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    grab.exe Application
    FILE:         grbtdlgc.h
    AUTHOR:       


    OVERVIEW
    ========
    Class definition for grabTDLGClient (TDialog).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\radiobut.h>
#include <owl\edit.h>
#include <owl\button.h>
#include "grabapp.rh"                  // Definition of all resources.

#include "cmds.h"
#include "FindWnd.h"
#include "extra1.h"


const long SZ_TXT_TTL = 1024;
const long SZ_TXT_NUM = 10;

//{{TDialog = grabTDLGClient}}
struct grabTDLGClientXfer {
//{{grabTDLGClientXFER_DATA}}
	 char    m_edMWnd[ SZ_TXT_TTL ];
	 char    m_edIDCB[ SZ_TXT_NUM ];
	 char    m_edWND2[ SZ_TXT_TTL ];
    char    m_edFName[ SZ_TXT_TTL ];
	 char    m_edClms[ SZ_TXT_NUM ];
	 char    m_edL1[ SZ_TXT_NUM ];
    char    m_edL2[ SZ_TXT_NUM ];
	 char    m_edLeft[ 3 ];
	 char    m_edRight[ 3 ];
    bool    m_radCombo;
    bool    m_radList;
    bool    m_radText;
//{{grabTDLGClientXFER_DATA_END}}
};


class grabTDLGClient : public TDialog {
public:
	 grabTDLGClient (TWindow *parent, TResId resId = IDD_CLIENT, TModule *module = 0);
	 virtual ~grabTDLGClient ();


//{{grabTDLGClientXFER_DEF}}
protected:
	 TButton *pbOK;
	 TButton *pbCancel;
	 TEdit *m_edMWnd;
	 TEdit *m_edIDCB;
	 TEdit *m_edWND2;
	 TEdit *m_edFName;

	 /*void ClearPatch( TClearPatch* );
	 void AddWndToPatch( TAddWndToPatch* );
	 void BuildPatch( TBuildPatch* );
	 void Subclass( TSubclass* );
	 void Extract( TExtract* );
	 void Unsubclass( TUnsubclass* );
	 void StoreInFile( TStoreInFile* );
	 void ClearData( TClearData* );
	 void GetData( TGetData* );*/

    TEdit *m_edClms;
    TEdit *m_edL1;
    TEdit *m_edL2;
    TEdit *m_edLeft;
    TEdit *m_edRight;
    TRadioButton *m_radCombo;
    TRadioButton *m_radList;
    TRadioButton *m_radText;

//{{grabTDLGClientXFER_DEF_END}}

//{{grabTDLGClientRSP_TBL_BEGIN}}
protected:
	 void BNClicked ();
	 void BNClickedCancel ();
	 void BNClicked_Store ();
//{{grabTDLGClientRSP_TBL_END}}

	 LRESULT  EvCopyData( WPARAM wParam, LPARAM lParam );

	 virtual LRESULT WindowProc(uint msg, WPARAM wParam, LPARAM lParam);
	 virtual void PerformCreate(int menuOrId);
	 virtual bool PreProcessMsg(MSG& msg);
    virtual bool Create();


	 L_Wnd m_lstWnd;
    UINT m_uiMsg_GetData;

DECLARE_RESPONSE_TABLE(grabTDLGClient);
};    //{{grabTDLGClient}}


#endif                                      // __grbtdlgc_h sentry.
