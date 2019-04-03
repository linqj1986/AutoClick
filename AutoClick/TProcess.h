#pragma once

#include <wtypes.h>
#include <vector>
#include <string>

#ifndef TFC_LIB
#ifdef _DEBUG
#pragma comment(lib, "tfcProcessD.lib")
#else
#pragma comment(lib, "tfcProcess.lib")
#endif
#endif

namespace TFC{

#ifdef _UNICODE
#define FindProcessT					FindProcessW
#define CloseProcessT					CloseProcessW
#define CreateProcessT                  CreateProcessW
#define CreateProcessAndWaitDoneT       CreateProcessAndWaitDoneW
#define CreateProcessAndNoWaitT         CreateProcessAndNoWaitW
#define CreateProcessAndWaitTimeT       CreateProcessAndWaitTimeW
#else
#define FindProcessT					FindProcessA
#define CloseProcessT					CloseProcessA
#define CreateProcessT                  CreateProcessA
#define CreateProcessAndWaitDoneT       CreateProcessAndWaitDoneA
#define CreateProcessAndNoWaitT         CreateProcessAndNoWaitA
#define CreateProcessAndWaitTimeT       CreateProcessAndWaitTimeA
#endif


	class CTProcess{
	public:
		CTProcess();
		virtual ~CTProcess();

		static BOOL CompareProcessNameA( LPCSTR pszName, LPCSTR pszNameList );
		static BOOL CompareProcessNameW( LPCWSTR pszName, LPCWSTR pszNameList );

		//支持'|'分隔进程名，查找多个进程
		static DWORD FindProcessA( LPCSTR pszProcessName );
		static DWORD FindProcessW( LPCWSTR pszProcessName );

		static int FindProcessA( LPCSTR pszProcessName, std::vector<DWORD>* pvecRet );
		static int FindProcessW( LPCWSTR pszProcessName, std::vector<DWORD>* pvecRet );

		static DWORD FindProcess( const DWORD dwProcessID );

		static DWORD GetParentProcess( const DWORD dwChildProcessID );

        static BOOL CloseProcess( const DWORD dwPid );

		static BOOL CloseProcessA( LPSTR pszProcessName );
		static BOOL CloseProcessW( LPCWSTR pszProcessName );

		static DWORD CreateProcessA( LPSTR lpszCmd, BOOL bIsHide = FALSE );
		static DWORD CreateProcessW( LPWSTR lpszCmd, BOOL bIsHide = FALSE );

        static BOOL CreateProcessA(LPCSTR lpszCmd, DWORD &dwExitCode, char **cResult, DWORD dwMSTime, BOOL bIsHide = TRUE, LPCSTR lpszCurrentDir = NULL, BOOL bInheritHandles = TRUE, BYTE* pbyWritePipe = NULL, const int nWriteLen = 0, const DWORD dwPipeBufSize = 4096);
        static BOOL CreateProcessW(LPWSTR lpszCmd, DWORD &dwExitCode, WCHAR **cResult, DWORD dwMSTime, BOOL bIsHide = TRUE, LPCWSTR lpszCurrentDir = NULL, BOOL bInheritHandles = TRUE, BYTE* pbyWritePipe = NULL, const int nWriteLen = 0, const DWORD dwPipeBufSize = 4096);

        static BOOL CreateProcessAndWaitDoneA(LPSTR lpszCmd, BOOL bIsHide = TRUE);
        static BOOL CreateProcessAndWaitDoneW(LPWSTR lpszCmd, BOOL bIsHide = TRUE);

        static BOOL CreateProcessAndWaitDoneA(LPSTR lpszCmd, DWORD &dwExitCode, BOOL bIsHide = TRUE);
        static BOOL CreateProcessAndWaitDoneW(LPWSTR lpszCmd, DWORD &dwExitCode, BOOL bIsHide = TRUE);

        static BOOL CreateProcessAndNoWaitA(LPSTR lpszCmd, BOOL bIsHide = TRUE, LPCSTR lpszCurrentDir = NULL);
        static BOOL CreateProcessAndNoWaitW(LPWSTR lpszCmd, BOOL bIsHide = TRUE, LPCWSTR lpszCurrentDir = NULL);

        static BOOL CreateProcessAndWaitTimeA(LPSTR lpszCmd, DWORD &dwExitCode, DWORD dwMSTime, BOOL bIsHide = TRUE);
        static BOOL CreateProcessAndWaitTimeW(LPWSTR lpszCmd, DWORD &dwExitCode, DWORD dwMSTime, BOOL bIsHide = TRUE);

        static BOOL CreateProcessAndWaitTimeA(LPSTR lpszCmd, std::string &szResult, DWORD dwMSTime, BOOL bIsHide = TRUE);
        static BOOL CreateProcessAndWaitTimeW(LPWSTR lpszCmd, std::wstring &szResult, DWORD dwMSTime, BOOL bIsHide = TRUE);
    };

}
