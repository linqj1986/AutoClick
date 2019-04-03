/******************************************************************************************
* 
*  FileName:    debug.cpp
*
*  Author:      linzm
*
*  Date:        2010-5-18
*
*  Description: 调试信息
*
*  History:     
*
*******************************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include "debug.h"

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4996)

//#define LENLIMIT 32766
#define LENLIMIT 65534
#define DEBUG_APPIDA "Ga:"
#define DEBUG_APPIDW L"Ga:"
void _DebugPrintA( const char *fmt, ... )
{
#ifdef DEBUGVERSION
    va_list args;//modify by wxg20140923:解决调试信息长度大于512B的部分无法显示的问题；
	int iSize=512;
	char* pBuf = (char*)malloc(iSize+1);
	memset(pBuf, 0, (iSize+1)*sizeof(char));
	int iFlagLen=strlen(DEBUG_APPIDA);
	while(true)
	{
		va_start(args, fmt);
		strcat_s(pBuf, iSize + 1, DEBUG_APPIDA);
		int iRet = _vsnprintf(pBuf+iFlagLen, iSize-iFlagLen, fmt, args);
		va_end(args);
		if((iRet > -1 && iRet < iSize-iFlagLen) || iSize >= LENLIMIT )
		{
			break;
		}
		if(iSize > LENLIMIT/2)
		{		
			iSize = LENLIMIT;
		}
		else
			iSize *= 2;
		pBuf = (char*)realloc(pBuf,iSize+1);
		if(pBuf == NULL)
		{
			OutputDebugStringA("_DebugPrintA:realloc failed!\n");
			return;
		}
		memset(pBuf,0,(iSize+1)*sizeof(char));
	}
    OutputDebugStringA(pBuf);
	free(pBuf);

#endif

}

void _DebugPrintW( const WCHAR *fmt, ... )
{
#ifdef DEBUGVERSION
// 	va_list args;
// 	WCHAR szbuf[512];
// 
// 	memset(szbuf, 0, sizeof(szbuf));
// 	va_start(args, fmt);
// 	_vsnwprintf(szbuf, sizeof(szbuf)/sizeof(WCHAR) - 1, fmt, args);
// 	va_end(args);
// 
// 	OutputDebugStringW(szbuf);

	va_list args;//modify by wxg20140923:解决调试信息长度大于512B的部分无法显示的问题；
	int iSize=512;
	WCHAR* pBuf = (WCHAR*)malloc((iSize+1)*sizeof(WCHAR));
	memset(pBuf, 0, (iSize+1)*sizeof(WCHAR));
	int iFlagLen=_tcslen(DEBUG_APPIDW);
	while(true)
	{
		va_start(args, fmt);
		_tcscat(pBuf,DEBUG_APPIDW);
		int iRet = _vsnwprintf(pBuf+iFlagLen, iSize-iFlagLen*sizeof(WCHAR), fmt, args);
		va_end(args);
		if((iRet > -1 && iRet < iSize-iFlagLen*sizeof(WCHAR)) || iSize >= LENLIMIT/2-1)
		{
			break;
		}

		if(iSize > LENLIMIT/4)
		{		
			iSize = LENLIMIT/2-1;
		}
		else
			iSize *= 2;
		pBuf = (WCHAR*)realloc(pBuf,(iSize+1)*sizeof(WCHAR));
		if(pBuf == NULL)
		{
			OutputDebugStringW(L"_DebugPrintW:realloc failed!\n");
			return;
		}
		memset(pBuf,0,(iSize+1)*sizeof(WCHAR));
	}
	OutputDebugStringW(pBuf);
	free(pBuf);
#endif
}

void _DebugMessageBoxA( const char *fmt, ... )
{
#ifdef DEBUGVERSION
    va_list args;
    char szbuf[512];

    memset(szbuf, 0, sizeof(szbuf));
    va_start(args, fmt);
    _vsnprintf(szbuf, sizeof(szbuf)/sizeof(char) - 1, fmt, args);
    va_end(args);

    MessageBoxA( NULL, szbuf, "调试信息", MB_OK);
#endif
}


void _DebugMessageBoxW( const WCHAR *fmt, ... )
{
#ifdef DEBUGVERSION
	va_list args;
	WCHAR szbuf[512];

	memset(szbuf, 0, sizeof(szbuf));
	va_start(args, fmt);
	_vsnwprintf(szbuf, sizeof(szbuf)/sizeof(WCHAR) - 1, fmt, args);
	va_end(args);

	MessageBoxW( NULL, szbuf, L"调试信息", MB_OK);
#endif
}

//------------------------------------------------------
//			保存信息到文件中，自动按日保存，生成新文件
//------------------------------------------------------
void LogToFile(const char *v_szMsg,const char *v_szSufix)
{
	/*char szPath[MAX_PATH] = { NULL };
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	std::string strPath = szPath;
	strPath = strPath.substr(0, strPath.rfind('\\') + 1);
	strPath += "\\log";*/
	char	szFile[2*MAX_PATH]={NULL};
	GetModuleFileNameA( AfxGetInstanceHandle(), szFile, MAX_PATH );
	int		iLen = strlen( szFile );
	int i=0;
	for( i=iLen-1; i>=0; i-- )
	{
		if( '\\'==szFile[i] ) break;
	}
	szFile[i] = '\0';
	strcat( szFile, "\\log" );
	_mkdir( szFile );

	char szShortTime[MAX_PATH] = { NULL };
	char szLongTime[MAX_PATH] = { NULL };
	struct tm *ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	sprintf(szShortTime, "%d-%d-%d", ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday);
	sprintf(szLongTime, "%d-%d-%d %d:%d:%d", ptr->tm_year + 1900, ptr->tm_mon + 1, ptr->tm_mday
		, ptr->tm_hour, ptr->tm_min, ptr->tm_sec);

	strcat( szFile, "\\" );
	strcat(szFile, szShortTime);
	_mkdir( szFile );

	strcat( szFile, "\\" );

	/*char szID[MAX_PATH]={NULL};
	int nId = GetCurrentProcessId();
	_itoa(nId,szID,10);
	strcat( szFile, szID);
	strcat( szFile, "_");*/

	strcat( szFile, v_szSufix);
	strcat( szFile, ".log");

	FILE*	fp = fopen( szFile, "a+" );
	if( NULL!=fp )
	{
		fprintf(fp, "(%s)%s\n", szLongTime, v_szMsg);
		fclose( fp );
	}
}