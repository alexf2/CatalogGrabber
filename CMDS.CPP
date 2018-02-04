
//#include "stdafx.h"
#include <owl\owlpch.h>
#include "mstring.h"
#include <stdlib.h>
#pragma hdrstop


#include "cmds.h"



/*TErrorDta* TErrorDta::CreError( DWORD& rSz, LPSTR lp )
 {
	rSz = sizeof(TErrorDta) + strlen(lp);
	TErrorDta* p = (TErrorDta*)calloc( rSz, 1 );
	p->lSize_Err = strlen(lp);
	strcpy( p->cErr, lp );

	p->cHaveErr = 1;

	return p;
 }


TAddWndToPatch* TAddWndToPatch::CreAddWndToPatch( DWORD& rSz, char cMethodUsed, long lIDControl, LPSTR lp )
 {
    TAddWndToPatch* p;
    if( cMethodUsed )
	 {
       rSz = sizeof(TAddWndToPatch) + strlen(lp);
	   p = (TAddWndToPatch*)calloc( rSz, 1 );
	   p->lSize_Ttl = strlen(lp);
	   strcpy( p->cTitle, lp );
	 }
	else 
	 {
	   rSz = sizeof(TAddWndToPatch);
	   p = (TAddWndToPatch*)calloc( rSz, 1 );
	   p->lIDControl = lIDControl;
	 }
	
	return p;
 }*/
