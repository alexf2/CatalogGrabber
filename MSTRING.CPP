#include "mstring.h"

mstring operator+( const mstring& rS1, const mstring& rS2 )
 {
	if( rS1.IsEmpty() && rS2.IsEmpty() ) return mstring();
	if( rS1.IsEmpty() )  return rS2;
	if( rS2.IsEmpty() ) return rS1;

	size_t sz1;
	LPSTR lp = (LPSTR)malloc( (sz1=rS1.length()) + rS2.length() + 1 );
	strcpy( lp, rS1.m_lp );
	strcpy( lp + sz1, rS2.m_lp );

	return lp;
 }
