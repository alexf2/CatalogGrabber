#if !defined(_CMDSRM_)
#define _CMDSRM_

#pragma pack(1)
struct TCmdBase
 {
	long lIDObj; //instance of hook
   HWND m_hwResponse;
 };

struct TClearPatch: public TCmdBase
 {
 };

struct TAddWndToPatch: public TCmdBase
 {
	char cMethodUsed; //0 - ID, 1 - title

	long lIDControl;

	long lSize_Ttl; //number of bytes in cTitle without '0'
	char cTitle[1];

	//static TAddWndToPatch* CreAddWndToPatch( DWORD& rSz, char cMethodUsed, long lIDControl, LPSTR lp );
 };

struct TBuildPatch: public TCmdBase
 {
 };

struct TSubclass: public TCmdBase
 {
	long lNumberClms;
 };

struct TExtract: public TCmdBase
 {
	long lW1, lW2;
 };

struct TUnsubclass: public TCmdBase
 {
 };

struct TStoreInFile: public TCmdBase
 {
	long lSize_FName; //number of bytes in cFileName without '0'
	char cFileName[1];
 };

struct TClearData: public TCmdBase
 {
 };

struct TGetData: public TCmdBase
 {
 };

struct TErrorDta: public TCmdBase
 {
	char cHaveErr; // 0 - no error, 1 - error

	long lSize_Err; //number of bytes in cErr without '0'
	char cErr[1];

	//static TErrorDta* CreError( DWORD& rSz, LPSTR );
 };

 struct TGetData_Response: public TCmdBase
 {
 };


#pragma pack()

enum RmCmds {
  CM_ClearPatch,
  CM_AddWndToPatch,
  CM_BuildPatch,
  CM_Subclass,
  CM_Extract,
  CM_Unsubclass,
  CM_StoreInFile,
  CM_ClearData,
  CM_GetData,

//Response
  CM_Error,
  CM_GetData_Response
 };

#endif
