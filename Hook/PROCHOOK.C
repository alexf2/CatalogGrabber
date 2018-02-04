/*
  PROCHOOK.C - Copyright (c) 1993 James M. Finnegan, All Rights Reserved
*/
#include <windows.h>
#include <dos.h>
#include <memory.h>
#pragma hdrstop

#include "phintern.h"
#include "prochook.h"

// Global pointers to the head and tail of hookmaster
NPHOOKMASTER npHookHead = NULL;
NPHOOKMASTER npHookTail = NULL;


/*
Boilerplate!
*/

int WINAPI LibMain(HANDLE hInstance, WORD wDataSeg, WORD wHeapSize,
							  LPSTR lpszCmdLine)
{
	 if(wHeapSize > 0)
		  UnlockData(0);

	 return 1;
}


/*
SetProcAddress - This function handles all of the linked list management,
checks to see if the procedure has been hooked before, and sets up the
hook to our function.

Returns - A pointer to the NPHOOKCHILD of our function or NULL
			 upon failure.
*/

NPHOOKCHILD _export WINAPI SetProcAddress(FARPROC lpfnOrigFunc,
													 FARPROC lpfnNewFunc, BOOL bExclusive)
{
	 NPHOOKMASTER npHookMast;       // Ptr to hook master record
	 NPHOOKCHILD  npHookChild;      // Ptr to child record
	 WORD         wCodeSel;         // pointer to code selector
    BYTE         byJmp=JMP_OPCODE; // JMP instruction opcode


    npHookMast=npHookHead;  // assign local ptr as head
        
    // Allocate the child record
    if((npHookChild=(NPHOOKCHILD)LocalAlloc(LPTR,sizeof(HOOKCHILD))) == NULL)
        return (NPHOOKCHILD)NULL;

    // Look for a previous master record that hooks the function that we 
    // want to hook.  Drop out with npHookMast == NULL if there is no record
    while(npHookMast != NULL)
    {
        // Compare FARPROC pointers
        if(lpfnOrigFunc == npHookMast->lpfnHookedFunction)
            break;           // Drop out if found

        // increment to next master record
        npHookMast=npHookMast->next;
    }
        
    // If we got to the end of the master list without finding an entry 
    if(npHookMast == NULL)
    {
        // Check to see if someone else is already hooking this function
        // by comparing the first byte in the function to JMP
        if((_fmemcmp(lpfnOrigFunc,&byJmp,1)) == NULL)
        {
            // Someone else beat us to it!  So, bail out.
            LocalFree((HLOCAL)npHookChild);  // Free the child record
            return (NPHOOKCHILD)NULL;
		  }

        // Create a hookmaster for this function at the end of the list
        if((npHookMast=(NPHOOKMASTER)LocalAlloc(LPTR,sizeof(HOOKMASTER))) == NULL)
        {
            // We're out of local heap space, bail.
            LocalFree((HLOCAL)npHookChild);  // Free the child record
            return (NPHOOKCHILD)NULL;
        }

        // Save the pointer to the function we are hooking
        npHookMast->lpfnHookedFunction=lpfnOrigFunc;

        // Set child record count
        npHookMast->wCount=0;

        // Set Unhook count
        npHookMast->wUnhookCount=0;

        // Set exclusive switch to the parameter passed in
        npHookMast->bExclusive=bExclusive;

        // Assign signature for sanity checking
        npHookMast->wSig=MASTER_SIG;
        
        // Assign previous to old tail. Remember that npHookTail is NULL
        // if this is the first master record.
        npHookMast->prev=npHookTail; 
        
        // If there was a previous master, link it's next pointer to us.
		  if(npHookMast->prev != NULL)
            npHookMast->prev->next=npHookMast;
        
        // Point to NULL indicating the end of the linked list
        npHookMast->next=NULL;        

        npHookMast->head=NULL;        // Assign child struct ptr to NULL
        npHookMast->tail=NULL;        // Assign child struct ptr to NULL
                                 
        // Make us the master tail pointer
        npHookTail=npHookMast;        

        // if this is the first master block, make us the head 
        // pointer as well
        if(npHookHead == NULL)
				npHookHead=npHookMast;
        
        // Fix the Code Selector to the Original function
        wCodeSel=FP_SEG(lpfnOrigFunc);
        GlobalFix(wCodeSel);
    
        // Alias Selector as data
        npHookMast->wDataSel=AllocCStoDSAlias(wCodeSel);
    }
    else   // We have previously hooked this function. So...
    {
        // Is this hook exclusive, or are we trying to make it exclusive
        // now?!  OR is the Unhook count > 0 ?  If so, we shouldn't hook.
        if( (npHookMast->bExclusive == TRUE) || (bExclusive == TRUE) ||
            (npHookMast->wUnhookCount > 0) )
		  {
            // Bail out.
            LocalFree((HLOCAL)npHookChild);  // Free the child record
            return (NPHOOKCHILD)NULL;
        }    
    }
    
    /*
      At this point, we are done with npHookMast.  It is set up in the
      linked list and is pointing to the Master record of the function
      that we want to hook.  We will now start setting up the child
      record (allocated above) by hooking it to the end of the child
      linked list and assigning its values.
    */
    
	 // Hook the new child structure at the end of the child linked list.
    // Note that npHookChild->prev will be assigned NULL if this is the
    // first child record.
    npHookChild->prev=npHookMast->tail;
    npHookChild->next=NULL;

    // If there was a previous child record, link its next ptr to us.
    if(npHookChild->prev != NULL)
        npHookChild->prev->next=npHookChild;
    
    // If this is a new master record, make us the child head.
    if(npHookMast->head == NULL)    
        npHookMast->head=npHookChild; 
        
    // Make us the Child tail pointer
	 npHookMast->tail=npHookChild;
        
    // Increment wCount to reflect another hook
    npHookMast->wCount++;
    
    // Backlink us to the master record
    npHookChild->npHookMaster=npHookMast;

    // Assign signature for sanity checking
    npHookChild->wSig=CHILD_SIG;
    
    // Assign pointer to new function
    npHookChild->lpfnNewFunc=lpfnNewFunc;
    
    // Call ProcHook to hook the original function to the new function.
	 ProcHook(npHookChild);
    // Since this is a new hook, don't reflect a "rehook"
    npHookMast->wUnhookCount++;

    return npHookChild;
}


/*
SetProcRelease - This function permanently unhooks the function refernced
by npHookChild and "glues" the remaining structures together.

Returns: NULL if successful
         ~NULL if unsuccessful
*/

BOOL _export WINAPI SetProcRelease(NPHOOKCHILD npHookChild)
{
    NPHOOKMASTER npHookMast; // Ptr to hook master record
    WORD         wCodeSel;   // pointer to code selector

    
    // If the record isn't valid, bail out.
    if(CheckChildRecord(npHookChild) == NULL)
		  return ~NULL;

    // Get the master header record
    npHookMast=npHookChild->npHookMaster;
    
    // If any children are unhooked, it is unsafe to continue. Bail out.
	 if(npHookMast->wUnhookCount > 0)
		  return ~NULL;

    // Check for the existence of a previous and next record and "glue"
    // them together, if necessary
    
    if(npHookChild->prev != NULL)
    {
        // If there is a prevoius record, make its next pointer point
        // to our next pointer.
        npHookChild->prev->next=npHookChild->next;
    }
    else
    {
        // If there is no previous record, then we are the head record
		  // referenced in our master pointer.  So set our next pointer to
        // the head.  This will be NULL if there is no next record.
        npHookMast->head=npHookChild->next; 
    }

    if(npHookChild->next != NULL)
    {
        // If there is a next record, make its previous pointer point
        // to our previous pointer.
        npHookChild->next->prev=npHookChild->prev;

        // Also, make its saved bytes ours.  That way, when it calls
        // ProcUnhook, it won't be pointing to us anymore!
        _fmemcpy(npHookChild->next->cBytes,npHookChild->cBytes,5);
    }
	 else
    {
        // If there is no next record, then we are the tail record
        // referenced in our master pointer.  So set our previous pointer
        // to the tail.  This will be NULL if there is no previous record.
        npHookMast->tail=npHookChild->prev;     

        // Since we are the tail record, simply unhook us.
        ProcUnhook(npHookChild);
        // Since this hook is permenently deleted, don't reflect a "unhook"
        npHookMast->wUnhookCount--;
    }

    // Decrement wCount to reflect a hook being deleted
    npHookMast->wCount--;

    // Clear the memory area to invalidate its contents
    _fmemset(npHookChild,NULL,sizeof(HOOKCHILD));
    
    // Delete the child record
    LocalFree((HLOCAL)npHookChild);

    // If there are no child records, we don't need the master anymore!
    if((npHookMast->head == NULL) && (npHookMast->tail == NULL))
    {
        // Check for the existence of a previous and next master and "glue"
        // them together, if necessary
    
        if(npHookMast->prev != NULL)
        {
				// If there is a prevoius master, make its next pointer point
            // to our next master.
            npHookMast->prev->next=npHookMast->next;
        }
        else
        {
            // If there is no previous master, then we are the head master
            // referenced in the global pointer.  So set our next pointer to
            // the head.  This will be NULL if there is no next record.
            npHookHead=npHookMast->next; 
        }

        if(npHookMast->next != NULL)
        {
            // If there is a next master, make its previous pointer point
				// to our previous pointer.
            npHookMast->next->prev=npHookMast->prev;
        }
        else
        {
            // If there is no next master, then we are the tail master
            // referenced in our global pointer.  So set our previous pointer
            // to the tail.  This will be NULL if there is no previous master.
            npHookTail=npHookMast->prev;     
        }

        // Free Data Selector allocated by AllocCStoDSAlias
        FreeSelector(npHookMast->wDataSel);
    
        // Unfix code segment (decrement lock count)
		  wCodeSel=FP_SEG(npHookMast->lpfnHookedFunction);
        GlobalUnfix(wCodeSel);
    
        // Clear the memory area to invalidate its contents
        _fmemset(npHookMast,NULL,sizeof(HOOKMASTER));
    
        // Delete the master record
        LocalFree((HLOCAL)npHookMast);
    }
    
    return NULL;
}


/*
ProcHook - This function saves the first five bytes of the function
refernced by npHookChild and replaces the five bytes with a far jump
into the function we defined (lpfnNewFunc).

Returns: NULL if successful
         ~NULL if unsuccessful
*/

BOOL _export WINAPI ProcHook(NPHOOKCHILD npHookChild)
{
    NPHOOKMASTER npHookMast;       // Ptr to hook master record
    BYTE FAR     *lpJmpPtr;        // Ptr to the beg. of function to hook
    BYTE         wJmp=JMP_OPCODE;  // JMP instruction opcode
    
    
	 // If the record isn't valid, bail out.
    if(CheckChildRecord(npHookChild) == NULL)
		  return ~NULL;

    // Get the master header record
    npHookMast=npHookChild->npHookMaster;
    
    // Create an offset into the selector
    lpJmpPtr=MK_FP(npHookMast->wDataSel,FP_OFF(npHookMast->lpfnHookedFunction));
    
    // Read and save 5 bytes of the function
    _fmemcpy(npHookChild->cBytes,lpJmpPtr,5);
    
    // Change the first 5 bytes to JMP 1234:5678 (EA 78 56 34 12)
    _fmemcpy(lpJmpPtr++,&wJmp,1);
	 _fmemcpy(lpJmpPtr,  &npHookChild->lpfnNewFunc,4);

    // Decrement the unhook count
    npHookMast->wUnhookCount--;

    return NULL;
}


/*
ProcUnhook - This function restores the first five bytes of the function
refernced by npHookChild, putting them back to the way they were before
ProcHook Got its hands on them!

Returns: NULL if successful
			~NULL if unsuccessful
*/

BOOL _export WINAPI ProcUnhook(NPHOOKCHILD npHookChild)
{
    BYTE FAR     *lpJmpPtr;       // Ptr to the beg. of function to unhook
    NPHOOKMASTER npHookMast;      // Ptr to hook master record
    
    
    // If the record isn't valid, bail out.
    if(CheckChildRecord(npHookChild) == NULL)
		  return ~NULL;

    // Get the master header record
    npHookMast=npHookChild->npHookMaster;

	 // Create an offset into the selector
	 lpJmpPtr=MK_FP(npHookMast->wDataSel,FP_OFF(npHookMast->lpfnHookedFunction));

	 // Restore the 5 bytes of the function
	 _fmemcpy(lpJmpPtr,npHookChild->cBytes,5);

	 // Increment the unhook count
	 npHookMast->wUnhookCount++;

	 return NULL;
}


/*
WEP - This function satisfies Windows' eccentricies.  Because of Windows 3.0
wierdness, don't do anything here!
*/

int _export WINAPI WEP(int t)
{
	 return 1;
}
