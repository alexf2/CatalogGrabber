#if !defined(_EXTRA1_)
#define _EXTRA1_

#include "findwnd.h"

void MkSubclass1( RTWndTrack rI, int iDiv, char cMode /*c - combobox OR l - listbox*/ )  throw(xmsg);
void MkUnSubclass1( RTWndTrack rI )  throw(xmsg);

void GoThroughtCBN1( RTWndTrack rI, long lW1, long lW2 )  throw(xmsg);

TFetchedStrContainer* GetContainer1();

void SetFileName( LPSTR );

extern char cBufErr[ 1024 ];
extern char cMode;//combobox
//C, L, T, I

void Extra1_Init();
void Extra1_Destroy();

#endif

