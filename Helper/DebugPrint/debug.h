/******************************************************************************************
* 
*  FileName:    debug.h
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

#ifndef _COMMON_DEBUG_H_
#define _COMMON_DEBUG_H_

#include <tchar.h>

#define DEBUGVERSION

//打调试的函数
void _DebugPrintA( const char *fmt, ... );
void _DebugPrintW( const WCHAR *fmt, ... );
void _DebugMessageBoxA( const char *fmt, ... );
void _DebugMessageBoxW( const WCHAR *fmt, ... );
void LogToFile(const char *v_szMsg,const char *v_szSufix);

#if defined(_UNICODE)
#define _tDebugPrint			_DebugPrintW
#define _tDebugMessageBox		_DebugMessageBoxW
#else
#define _tDebugPrint			_DebugPrintA
#define _tDebugMessageBox		_DebugMessageBoxA
#endif

#endif
