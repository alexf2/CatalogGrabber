/*
  PROCHOOK.H - Copyright (c) 1993 James M. Finnegan, All Rights Reserved
*/

// ProcHook Header.  Used by the ProcHook DLL source files, as well
// as any app using the DLL.

#ifndef _INC_PROCHOOK
#define _INC_PROCHOOK

#ifdef __cplusplus
extern "C" {            // No mangling!
#endif

#include <windows.h>

// Byte pack structures
#pragma pack(1)

typedef struct tagHOOKCHILD
{
    WORD                 wSig;         // Signature
    struct  tagHOOKCHILD near *prev;   // Ptr to previous hook
    struct  tagHOOKCHILD near *next;   // Ptr to next hook
    VOID            near *npHookMaster;// Ptr to master structure
    BYTE                 cBytes[5];    // Bytes of hooked function
    FARPROC              lpfnNewFunc;  // Ptr to New function
}HOOKCHILD;                

typedef HOOKCHILD            *PHOOKCHILD;
typedef HOOKCHILD NEAR       *NPHOOKCHILD;
typedef HOOKCHILD FAR        *LPHOOKCHILD;

typedef struct tagHOOKMASTER
{
    WORD                 wSig;         // Signature
    struct tagHOOKMASTER near *prev;   // Ptr to previous master
    struct tagHOOKMASTER near *next;   // Ptr to next master
    NPHOOKCHILD          head;         // Ptr to head of child struct 
    NPHOOKCHILD          tail;         // Ptr to tail of child struct
    BOOL                 bExclusive;   // Exclusive Hook field
    WORD                 wCount;       // Count of # of hooks to this function
    WORD                 wUnhookCount; // Count of # of unhooks to this function
    WORD                 wDataSel;     // Selector to aliased code selector
    FARPROC              lpfnHookedFunction;// Ptr to original function
}HOOKMASTER;

typedef HOOKMASTER            *PHOOKMASTER;
typedef HOOKMASTER NEAR       *NPHOOKMASTER;
typedef HOOKMASTER FAR        *LPHOOKMASTER;


NPHOOKCHILD  _export WINAPI SetProcAddress (FARPROC, FARPROC, BOOL);
BOOL         _export WINAPI SetProcRelease (NPHOOKCHILD);
BOOL         _export WINAPI ProcHook       (NPHOOKCHILD);
BOOL         _export WINAPI ProcUnhook     (NPHOOKCHILD);

NPHOOKMASTER _export WINAPI GetFirstHMaster(LPHOOKMASTER);
NPHOOKMASTER _export WINAPI GetNextHMaster (LPHOOKMASTER);
NPHOOKCHILD  _export WINAPI GetFirstHChild (LPHOOKMASTER, LPHOOKCHILD);
NPHOOKCHILD  _export WINAPI GetNextHChild  (LPHOOKCHILD);

WORD         _export WINAPI GetOrdFromAddr (HMODULE, FARPROC);
LPSTR        _export WINAPI GetNameFromOrd (HMODULE, WORD);


// Set packing back to its default
#pragma pack()

#ifdef __cplusplus
}                       // End of extern "C" {
#endif

#endif  // _INC_PROCHOOK
