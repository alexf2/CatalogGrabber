#if !defined(__grabapp_h)              // Sentry, use file only if it's not already included.
#define __grabapp_h

/*  Project grab
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    grab.exe Application
    FILE:         grabapp.h
    AUTHOR:       


    OVERVIEW
    ========
    Class definition for grabApp (TApplication).      
*/


#include <owl\owlpch.h>
#pragma hdrstop


#include "grabapp.rh"            // Definition of all resources.
#include "ddesrv.h"


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = SDIDecFrame}}
class SDIDecFrame : public TDecoratedFrame {
public:
    SDIDecFrame (TWindow *parent, const char far *title, TWindow *clientWnd, bool trackMenuSelection = false, TModule *module = 0);
    ~SDIDecFrame ();
};    //{{SDIDecFrame}}


//{{TApplication = grabApp}}
class grabApp : public TApplication {
private:

public:
    grabApp ();
    virtual ~grabApp ();

//{{grabAppVIRTUAL_BEGIN}}
public:
    virtual void InitMainWindow();
//{{grabAppVIRTUAL_END}}

//{{grabAppRSP_TBL_BEGIN}}
protected:
	 void CmHelpAbout ();

	 virtual void  InitInstance();
	 virtual int   TermInstance( int status );
//{{grabAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(grabApp);

private:
  TDDEServer ddeSrv;
};    //{{grabApp}}


#endif                                      // __grabapp_h sentry.
