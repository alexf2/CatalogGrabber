/*
  PROCINFO.C - Copyright (c) 1993 James M. Finnegan, All Rights Reserved
*/
#include <windows.h>
#include <windowsx.h>
#include <dos.h>
#include <string.h>
#include "prochook.h"
#include "phintern.h"


/*
GetOrdFromAddr - This function gets the ordinal value of the function 
indicated in lpfnFuncPtr from the module indicated by hMod.

Returns: The ordinal value of the funciton if successful, or NULL
         if the funciton does not match any exported entry points.
*/

WORD _export WINAPI GetOrdFromAddr(HMODULE hMod, FARPROC lpfnFuncPtr)
{
    LPMODTABLE lpModTable;
    LPSEGREC   lpSegRec;
    LPBUNDLE   lpBundle;
    LPENTRY    lpEntry;
    WORD       wSegNum,
               wOrdinal;
    BOOL       bFound=FALSE;
    
    
    // Get a ptr to the handle
    lpModTable=(LPMODTABLE)MK_FP(HandleToSel(hMod),0);
    
    // Check pointer's validity
    if(CheckModPtr(lpModTable) == NULL)
        return NULL;
    
    // Assign first segment record
    lpSegRec=MK_FP(FP_SEG(lpModTable),lpModTable->npSegTab);
    
    // Get logical segment number from Module Table.  Step thru all segment
    // entries till we reach the end or find a match.
    for(wSegNum=1; wSegNum <= lpModTable->wSegCount; wSegNum++, lpSegRec++)
    {
        // Do the selector values match?!
        if(HandleToSel(lpSegRec->wHandle) == FP_SEG(lpfnFuncPtr))
        {
            bFound=TRUE;    
            break;
        }    
    }
    
    // If we didn't find the selector in the table, fail.
    if(bFound == FALSE)
        return NULL;
        
    // Get the first bundle
    lpBundle=MK_FP(FP_SEG(lpModTable),lpModTable->npEntryTable);
    
    bFound=FALSE;
    
    // Walk entry table looking for an offset match
    while(FP_OFF(lpBundle) != NULL)
    {
        // Get the first entry record (directly follows the bundle)
        lpEntry =(LPENTRY)(LPBUNDLE)(lpBundle + 1);

        // Go through entries to find a match.  Add 1 to wFirstOrdinal
        // to corretly point to the first ordinal. Oddly, wLastOrdinal is
        // the correct value!
        for(wOrdinal=lpBundle->wFirstOrdinal+1; 
                  wOrdinal <= lpBundle->wLastOrdinal; wOrdinal++, lpEntry++)
        {
            // If the logical selector values match...
            if(lpEntry->bySegmentNumber == wSegNum)
            {
                // If the offsets match, we found it!
                if(lpEntry->wOffset == FP_OFF(lpfnFuncPtr))
                {
                    bFound=TRUE;
                    break;
                }
            }
        }
        
        // If we found a match, drop out.
        if(bFound==TRUE)
            break;
            
        // Increment to the next bundle and search again
        lpBundle=MK_FP(FP_SEG(lpModTable),lpBundle->npNextBundle);
    }
    
    // If we didn't find the ordinal in the table, fail.
    if(bFound == FALSE)
        return NULL;

    // return ordinal
    return wOrdinal;
}


/*
GetNameFromOrd - This function gets the name of the function indicated by 
wOrdinal from the module indicated by hMod.  Remember that a wOrdinal of
0 will return the name of the module.

Returns: A LPSTR to a buffer holding the name of the exported function, or
         NULL if there is no name associated with the ordinal.
*/

LPSTR _export WINAPI GetNameFromOrd(HMODULE hMod, WORD wOrdinal)
{
    LPMODTABLE lpModTable;
    LPMODFILE  lpModFile;
    LPSTR      lpNameTab;
    LPSTR      lpszStr;
    HFILE      hFile;

    
    // Get a ptr to the handle
    lpModTable=(LPMODTABLE)MK_FP(HandleToSel(hMod),0);
    
    // Check pointer's validity
    if(CheckModPtr(lpModTable) == NULL)
        return NULL;

    // Assign resident names ptr
    lpNameTab=MK_FP(FP_SEG(lpModTable),lpModTable->npResNameTab);
    
    // Walk the resident names table for a match
    lpszStr=WalkNameTable(lpNameTab,wOrdinal);
            
    // If there is no match
    if(*lpszStr == NULL)
    {    
        // Allocate buffer for non-resident table
        lpNameTab=GlobalAllocPtr(GMEM_MOVEABLE|GMEM_ZEROINIT,
                                               lpModTable->wNonResNameSize);
        // If GlobalAllocPtr failed, drop out.        
        if(lpNameTab == NULL)
            return NULL;
        
        // Get the fully qualified pathmane of the module
        lpModFile=MK_FP(FP_SEG(lpModTable),lpModTable->npFileInfo);
        
        // Open the module file
        if((hFile=_lopen(lpModFile->szFileName,READ)) != HFILE_ERROR)
        {        
            // Seek to the non-resident names table
            _llseek(hFile,lpModTable->dwNonResNameOff,0);
        
            // Load table into buffer
            _lread(hFile,lpNameTab,lpModTable->wNonResNameSize);
        
            // Close the file
            _lclose(hFile);

            // Walk the non-resident names table for a match
            lpszStr=WalkNameTable(lpNameTab,wOrdinal);
        }
        
        // Free the non-resident names table buffer
        GlobalFreePtr(lpNameTab);
    }
    
    // Return pointer to the string (or NULL)
    return lpszStr;
}


/*
CheckModPtr - This function checks the signature bytes of the module
table to ensure that the pointer we have is really a valid table.

Returns: ~NULL if successful
         NULL if unsuccessful
*/

BOOL CheckModPtr(LPMODTABLE lpModTable)
{
    // If the pointer is NULL, return NULL
    if(lpModTable == NULL)
        return NULL;
    
    if(lpModTable->wModSig != NE_SIGNATURE)
        return NULL;
    else
        return ~NULL;
}


/*
HandleToSel - This function does some bit hacking of a handle to convert it
into a selector.  This has been adapted from Undocumented Windows 
(A. Schulman, et al, Addison Wesley 1992).

Returns: The selector of the passed in handle
*/

WORD HandleToSel(HANDLE hHandle)
{
    static WORD wVers=NULL;


    // Initialize static variable if it was not previously done
    if(wVers==NULL)
        wVers=(WORD)GetVersion();

    // If we are running Windows 3.0,
    // selector = handle - 1
    if(wVers==3)                 
        return((hHandle & 2) == 2) ? hHandle-1 : hHandle;

    // If we are running Windows 3.1 or better,
    // selector = handle + 1
    else                          
        return(hHandle | 1);
}


/*
WalkNameTable - This function walks the buffer indicated by lpNameTable
for a matching ordinal indicated by wOrd.  The strings in the buffer are
in PASCAL format (length, followed by the string, with no NULL terminator).
The associated ordinal is located in the word immediately following the
string.  The buffer can be a pointer to either the resident or 
non-resident names table, since they are both the same format.

Returns: A LPSTR to the buffer holding the name, or NULL.
*/

LPSTR WalkNameTable(LPSTR lpNameTable, WORD wOrd)
{
    static char cBuffer[129];
    LPSTR       lpLocalNameTable;
    WORD        wOrdinal;
    BYTE        byLength;

    
     // Null terminate the string
    cBuffer[0]=NULL;
                                     
    // Assign the names table to a local pointer, so as not to fool with
    // the value of the caller's pointer
    lpLocalNameTable=lpNameTable;

    // The strings are in pascal format followed by the associated 
    // ordinal.  A NULL length indicates the end of the table.
    while(*lpLocalNameTable != NULL)
    {
        // Get the length of the name
        byLength=*(BYTE far *)lpLocalNameTable;
        
        // The ordinal is the word following the name
        wOrdinal=*(WORD far *)(lpLocalNameTable + byLength + 1);

        // If we found the match!
        if(wOrdinal==wOrd)
        {        
            // Copy the string to a local buffer, and NULL terminate it.
            _fstrncpy(cBuffer, lpLocalNameTable+1, byLength);
            cBuffer[byLength] = NULL;
            
            // Drop out of the while loop
            break;
        }
        
        // Move the pointer to point to the next entry (past the ordinal)
        lpLocalNameTable += (byLength + 3);
    }
    return cBuffer;
}
