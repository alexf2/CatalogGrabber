#if !defined(_FIND_WND_)
#define _FIND_WND_

#include <windows.h>
#include "mstring.h"
#include <classlib/dlistimp.h>

#pragma hdrstop

_CLASSDEF( TWndTrack )
_CLASSDEF( TFetchedStrContainer )
_CLASSDEF( TStrItem )
_CLASSDEF( TCell )


struct TWndTrack
 {
	TWndTrack()
	 {
		m_lID = -1;
		m_hw = NULL;
		m_bID = 1;
	 }
	virtual ~TWndTrack();
	TWndTrack( const TWndTrack& rTr )
	 {
		this->operator=( rTr );
	 }
	TWndTrack& operator=( const TWndTrack& r )
	 {
		m_sParentTitle = r.m_sParentTitle;
		m_sClassName = r.m_sClassName;
		//m_sTitle = r.m_sTitle;
		m_lID = r.m_lID;
		m_hw = r.m_hw;
		m_bID = r.m_bID;
		return *this;
	 }

	int operator==( const TWndTrack& rI ) const
	 {
		return m_sParentTitle == rI.m_sParentTitle && m_lID == rI.m_lID &&
		  m_sClassName == rI.m_sClassName && m_bID == rI.m_bID;
	 }

	int operator<( const TWndTrack& rI ) const
	 {
		return m_sParentTitle < rI.m_sParentTitle;// && m_sTitle < rI.m_sTitle;
	 }

	mstring m_sParentTitle;
	mstring m_sClassName;
	//mstring m_sTitle;
	long m_lID; //if m_bID == 1 then ID else number in list
	HWND m_hw;
	BYTE m_bID;
 };

//, TStandardAllocator
class TOwned_DLstWnd: public TIDoubleListImp<TWndTrack>
 {
public:
	void Flush( int del = 0 )
	 {
		TIDoubleListImp<TWndTrack>::Flush( 1 );
	 }
 };
typedef TOwned_DLstWnd L_Wnd;
typedef TIDoubleListIteratorImp<TWndTrack> IT_LWnd;

void DumpLst( const L_Wnd&, mstring& );
void FindControlWnd( const L_Wnd& rlstPch, HWND hwPar ) throw(xmsg);

/*void MkSubclass( RTWndTrack rI, int iDiv )  throw(xmsg);
void MkUnSubclass( RTWndTrack rI )  throw(xmsg);
void GoThroughtCBN( RTWndTrack rI, mstring sFileName )  throw(xmsg);*/

typedef LRESULT CALLBACK (*PWndProc)(
	 HWND hwnd,
	 UINT uMsg,
	 WPARAM wParam,
	 LPARAM lParam
	);

class TCell
 {
public:
  TCell()
	{
	  x = y = 0;
	}
  ~TCell()
	{
	}

  TCell( const TCell& rTr )
	 {
		this->operator=( rTr );
	 }
  TCell& operator=( const TCell& r )
	{
	  m_s = r.m_s; x = r.x; y = r.y;
	}

  int operator==( const TCell& rI ) const
	{
	  return m_s == rI.m_s;
	}
  int operator<( const TCell& rI ) const
	{
	  if( y < rI.y ) return y < rI.y;
	  return x < rI.x;
	}

  mstring m_s;
  long x, y; //left, top on DrawText
 };


class TOwned_DLstStr: public TIDoubleListImp<TCell>
 {
public:
	void Flush( int del = 0 )
	 {
		TIDoubleListImp<TCell>::Flush( 1 );
	 }
	void FlushNoDelete()
	 {
      TIDoubleListImp<TCell>::Flush( 0 );
	 }
 };
typedef TOwned_DLstStr L_Str;
typedef TIDoubleListIteratorImp<TCell> IT_LStr;



class TStrItem
 {

friend class TFetchedStrContainer;

public:
  TStrItem()
	{
	}
  ~TStrItem()
	{
	}

  TStrItem( const TStrItem& rTr )
	 {
		this->operator=( rTr );
	 }
  TStrItem& operator=( const TStrItem& r );

  int operator==( const TStrItem& rI ) const;
  int operator<( const TStrItem& rI ) const;

protected:
  L_Str m_lst;
 };

class TOwned_DLstStrStr: public TIDoubleListImp<TStrItem>
 {
public:
	void Flush( int del = 0 )
	 {
		TIDoubleListImp<TStrItem>::Flush( 1 );
	 }
 };
typedef TOwned_DLstStrStr L_StrStr;
typedef TIDoubleListIteratorImp<TStrItem> IT_LStrStr;


#endif


