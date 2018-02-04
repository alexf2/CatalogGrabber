/*
  PROCUTIL.C - Copyright (c) 1993 James M. Finnegan, All Rights Reserved
*/
#include <windows.h>
#include <memory.h>
#include "prochook.h"
#include "phintern.h"


// Global pointer to the head of hookmaster
// (defined in PROCHOOK.C)
extern NPHOOKMASTER npHookHead;


/*
GetFirstHMaster - This function copies the first master record into an 
application allocated buffer and returns the "magic cookie" to the live
record in ProcHook's local data segment.

Returns - A pointer to the master or NULL if there are no master records
*/

NPHOOKMASTER _export WINAPI GetFirstHMaster(LPHOOKMASTER lpHookMast)
{
    // Make sure we have at least one master record
    if(CheckMasterRecord(npHookHead) != NULL)
    {
        // Copy the "goods" into the callers buffer
        _fmemcpy(lpHookMast,npHookHead,sizeof(HOOKMASTER));    
    }
    else
    {
        return (NPHOOKMASTER)NULL;
    }

    // Return ProcHook's local pointer
    return npHookHead;
}


/*
GetNextHMaster - This function gets the next master record based on data
in the callers buffer (GetFirstHMaster should be called before this
function!)  It then copies the next master record into the application
allocated buffer and returns the "magic cookie" to the live record in 
ProcHook's local data segment.

Returns - A pointer to the master or NULL if there are no master records
*/

NPHOOKMASTER _export WINAPI GetNextHMaster(LPHOOKMASTER lpHookMast)
{
    NPHOOKMASTER npHookMast;
                                         
                                         
    // Make sure we have a real master record
    if(CheckMasterRecord(lpHookMast) == NULL)
        return (NPHOOKMASTER)NULL;

    // Go to the next master record
    npHookMast=lpHookMast->next;
    
    // Make sure we are not at the end of the master list
    if(CheckMasterRecord(npHookMast) != NULL)
    {
        // Copy the "goods" into the callers buffer
        _fmemcpy(lpHookMast,npHookMast,sizeof(HOOKMASTER));    
    }
    else
    {
        return (NPHOOKMASTER)NULL;
    }
    
    // Return ProcHook's local pointer
    return npHookMast;
}


/*
GetFirstHChild - This function copies the first child record of lpHookMast
into an application allocated buffer and returns the "magic cookie" to 
the live record in ProcHook's local data segment.  lpHookMast can either
be a long pointer to a copy of the master record, or, if HIWORD(lpHookMast)
is 0, LOWORD(lpHookMast) is the live pointer to the master record.

Returns - A pointer to the child or NULL if there are no child records
*/

NPHOOKCHILD _export WINAPI GetFirstHChild(LPHOOKMASTER lpHookMast,
                                          LPHOOKCHILD lpHookChild)
{
    NPHOOKMASTER npHookMast;
    NPHOOKCHILD  npHookChild;
    
    
    // If we were passed a live pointer rather than a copy...
    if(HIWORD(lpHookMast) == 0)
    {
        // Assign near pointer
        npHookMast=(NPHOOKMASTER)LOWORD(lpHookMast);
        
        // Check for validity
        if(CheckMasterRecord(npHookMast) == NULL)
            return (NPHOOKCHILD)NULL;
        
        npHookChild=npHookMast->head;
    }    
    // Make sure we have a real lpHookMast
    else if(CheckMasterRecord(lpHookMast) != NULL)
    {
        // Grab the first child record
        npHookChild=lpHookMast->head;
    }
    else
    {
        return (NPHOOKCHILD)NULL;
    }
    
    // Make sure that we have valid records
    if(CheckChildRecord(npHookChild) != NULL)
    {
        // Copy the "goods" into the callers buffer
        _fmemcpy(lpHookChild,npHookChild,sizeof(HOOKCHILD));
    }
    else
    {
        return (NPHOOKCHILD)NULL;
    }

    // Return ProcHook's local pointer
    return npHookChild;
}


/*
GetNextHChild - This function gets the next child record based on data
in the callers buffer (GetFirstHChild should be called before this function!)
It then copies the next child record into the application allocated
buffer and returns the "magic cookie" to the live record in ProcHook's 
local data segment.

Returns - A pointer to the child or NULL if there are no child records
*/

NPHOOKCHILD _export WINAPI GetNextHChild(LPHOOKCHILD lpHookChild)
{
    NPHOOKCHILD npHookChild;


    // Make sure we have a real child record
    if(CheckChildRecord(lpHookChild) == NULL)
        return (NPHOOKCHILD)NULL;

    // Go to the next child record
    npHookChild=lpHookChild->next;
        
    // Make sure that we are not at the end of the child list
    if(CheckChildRecord(npHookChild) != NULL)
    {
        // Copy the "goods" into the callers buffer
        _fmemcpy(lpHookChild,npHookChild,sizeof(HOOKCHILD));
    }
    else
    {
        return (NPHOOKCHILD)NULL;
    }


    // Return ProcHook's local pointer
    return npHookChild;
}


/*
CheckMasterRecord - This function checks the validity of the master record
by checking the signature member.

Returns: NULL if record is invalid
         ~NULL if record is valid
*/

BOOL CheckMasterRecord(LPHOOKMASTER lpHookMaster)
{
    // If the pointer is NULL, return NULL
    if(lpHookMaster == NULL) 
        return NULL;
    
    // If the signature isn't correct, return NULL.
    if(lpHookMaster->wSig != MASTER_SIG)
        return NULL;
        
    return ~NULL;
}


/*
CheckChildRecord - This function checks the validity of the child record
by checking the signature member.

Returns: NULL if record is invalid
         ~NULL if record is valid
*/

BOOL CheckChildRecord(LPHOOKCHILD lpHookChild)
{
    // If the pointer is NULL, return NULL
    if(lpHookChild == NULL)
        return NULL;
    
    // If the signature isn't correct, return NULL.
    if(lpHookChild->wSig != CHILD_SIG)
        return NULL;
        
    return ~NULL;
}
