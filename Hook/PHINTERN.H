/*
  PHINTERN.H - Copyright (c) 1993 James M. Finnegan, All Rights Reserved
*/

// Internal ProcHook stuff, only to be used my modules linked
// to the ProcHook DLL

#ifndef _INC_PHINTERN
#define _INC_PHINTERN

#ifdef __cplusplus
extern "C" {            // No mangling!
#endif


#include <windows.h>
#include "prochook.h"

// Byte pack structures
#pragma pack(1) 

// Signature defines
#define MASTER_SIG  'JF'
#define CHILD_SIG   'CF'

// JMP opcode define
#define JMP_OPCODE  0xEA

// "Pseudo-undocumented" Windows Function
WORD FAR PASCAL AllocCStoDSAlias(WORD);

// Internally used functions
BOOL CheckMasterRecord(LPHOOKMASTER);
BOOL CheckChildRecord (LPHOOKCHILD);



// The following stuff is for PROCINFO.C

// Module Table signature
#define NE_SIGNATURE 'EN'     // 'NE' in LOBYTE/HIBYTE (Intel) format

// Module Table data sructure
typedef struct tagMODTABLE
{
    WORD     wModSig;         // Module signature "NE"
    WORD     wLoadCount;      // Load count
    WORD     npEntryTable;    // Ptr to entry table
    WORD     npNextMod;       // Ptr to next module
    WORD     npDGroup;        // Ptr to DGROUP
    WORD     npFileInfo;      // Module file info
    WORD     wModFlags;       // Module flags
    WORD     wDGroupSeg;      // Logical segment # of DGROUP
    WORD     wInitHeap;       // Initial heap size
    WORD     wInitStack;      // Initial stack size
    DWORD    dwInitCSIP;      // Initial instruction pointer
    DWORD    dwInitSSSP;      // Initial stack pointer
    WORD     wSegCount;       // Segment count
    WORD     wModRefCount;    // Module reference table entry count
    WORD     wNonResNameSize; // Size of non-resident names table on disk
    WORD     npSegTab;        // Ptr to segment table
    WORD     npResourceTab;   // Ptr to resource table
    WORD     npResNameTab;    // Ptr to resident name table
    WORD     npModRefTab;     // Ptr to module reference talbe
    WORD     npImpNameTab;    // Ptr to import names table
    DWORD    dwNonResNameOff; // Non-resident name table offset on disk 
    WORD     wMovEntCount;    // Moveable entry count
    WORD     wSectorAlign;    // Sector alignment
    WORD     wTTF;            // TrueType font
    WORD     wOSflags;        // OS flags
    WORD     wUnknown38;
    WORD     wUnknown3A;
    WORD     wUnknown3C;
    WORD     wExpOSver;       // Expected OS version
}MODTABLE;
    
typedef MODTABLE            *PMODTABLE;
typedef MODTABLE NEAR       *NPMODTABLE;
typedef MODTABLE FAR        *LPMODTABLE;

                                       
// Segment Record data sructure
typedef struct tagSEGREC
{
    WORD    wSelOffset;       // Offset to logical selector on disk
    WORD    wSelLength;       // Size of logical segment     
    WORD    wFlags;           // Flags          
    WORD    wSelSize;         // Initial size of segment    
    WORD    wHandle;          // Handle/Selector to segment
}SEGREC;

typedef SEGREC              *PSEGREC;
typedef SEGREC NEAR         *NPSEGREC;
typedef SEGREC FAR          *LPSEGREC;


// Export Ordinal Bundle data sructure
typedef struct tagBUNDLE
{
    WORD    wFirstOrdinal;    // First Ordinal in bundle
    WORD    wLastOrdinal;     // Last Ordinal in bundle
    WORD    npNextBundle;     // Ptr to next bundle
}BUNDLE;

typedef BUNDLE            *PBUNDLE;
typedef BUNDLE NEAR       *NPBUNDLE;
typedef BUNDLE FAR        *LPBUNDLE;

                                       
// Ordinal Entry data sructure
typedef struct tagENTRY
{
    BYTE    bySegmentType;    // Segment type
    BYTE    byFlags;          // Flags
    BYTE    bySegmentNumber;  // Logical segment number
    WORD    wOffset;          // Offset of entry
}ENTRY;

typedef ENTRY            *PENTRY;
typedef ENTRY NEAR       *NPENTRY;
typedef ENTRY FAR        *LPENTRY;


// Module File Information data sructure
typedef struct tagMODFILE
{
    BYTE  byLength;           // Length of structure
    BYTE  byFlag;             // flags
    WORD  wUnknown;           
    WORD  wDate;              // File Date
    WORD  wTime;              // File Time
    char  szFileName[1];      // DOS qualified filename
}
MODFILE;

typedef MODFILE            *PMODFILE;
typedef MODFILE NEAR       *NPMODFILE;
typedef MODFILE FAR        *LPMODFILE;

// Internally used functions
BOOL  CheckModPtr  (LPMODTABLE);
WORD  HandleToSel  (HANDLE);
LPSTR WalkNameTable(LPSTR, WORD);


// Set packing back to its default
#pragma pack()

#ifdef __cplusplus
}                       // End of extern "C" {
#endif 

#endif  // _INC_PHINTERN
