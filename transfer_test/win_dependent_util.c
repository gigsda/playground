
#include "win_dependent_util.h"
#include <string.h>
#include <assert.h>
#include "logger.h"


/////////////////////////////////////////////////////////////////////

//  char -> wchar

wchar_t* CharToWChar(const char* pstrSrc)
{
    int nLen = strlen(pstrSrc)+1;
	static wchar_t pwstr[1000];
     
    mbstowcs(pwstr, pstrSrc, nLen);

    return pwstr;
}

/////////////////////////////////////////////////////////////////////

// wchar -> char
char* WCharToChar(const wchar_t* pwstrSrc)
{
	 
#if !defined _DEBUG
    int nLen = 0;
	static char pstr[1000];
    nLen = (wcslen(pwstrSrc) + 1)*2;
 
    WideCharToMultiByte( 949, 0, pwstrSrc, -1, pstr, nLen, NULL, NULL);
#else
	static char pstr[1000];

    int nLen = wcslen(pwstrSrc);
	wcstombs(pstr, pwstrSrc, nLen+1);
#endif

    return pstr;
}
 