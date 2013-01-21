/* 
* Copyright 2012 NexR
* @author: tim.yang@nexr.com, gigsda@gmail.com
*/ 

#include "win_dependent_util.h"
#include <string.h>
#include <assert.h>
#include "logger.h"
#include "data_type.h"



/////////////////////////////////////////////////////////////////////

//  HMC_CHAR  -> wchar

wchar_t* CharToWChar(const char * pstrSrc)
{
    HMC_INT nLen = strlen(pstrSrc)+1;
	static wchar_t pwstr[1000];
    
#define _CRT_SECURE_NO_WARNINGS 1
  
    mbstowcs(pwstr, pstrSrc, nLen);

    return pwstr;
}

/////////////////////////////////////////////////////////////////////

// wchar -> char
char * WCharToChar(const wchar_t* pwstrSrc)
{
	 
#if !defined _DEBUG
    HMC_INT nLen = 0;
	static char pstr[1000];
    nLen = (wcslen(pwstrSrc) + 1)*2;
 
    WideCharToMultiByte( 949, 0, pwstrSrc, -1, pstr, nLen, NULL, NULL);
#else
	static char pstr[1000];

    HMC_INT nLen = wcslen(pwstrSrc);
	
 
	wcstombs(pstr, pwstrSrc, nLen+1);
#endif

    return pstr;
}
 