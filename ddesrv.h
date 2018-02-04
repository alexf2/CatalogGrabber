#if !defined(_DDE_SRV_)
#define _DDE_SRV_

#include <windows.h>
#include <windowsx.h>
#include <ddeml.h>
#include <dde.h>
//#include <classlib/dlistimp.h>

#pragma hdrstop

_CLASSDEF( TDDEServer )

class TDDEServer
 {
public:
	TDDEServer()
	 {
		m_bInit = false;
	 }
	virtual ~TDDEServer();

	void Init();
	void Close();

	static LPCSTR m_lpSERVER;
	static LPCSTR m_lpTOPIC;
	static LPCSTR m_lpITEM1;
	static LPCSTR m_lpITEM2;

private:
	bool m_bInit;

 };



#endif

