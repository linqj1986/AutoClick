
#include "TProcess.h"
#include <tlhelp32.h>
#include <tchar.h>

#pragma warning(disable:4996)

namespace TFC{
	CTProcess::CTProcess()
	{
	}
	CTProcess::~CTProcess()
	{
	}

    BOOL CTProcess::CompareProcessNameA( LPCSTR pszName, LPCSTR pszNameList )
    {
        if ( strcmp( pszName, pszNameList ) == 0 )
        {
            return TRUE;
        }

        char* pszFind = (char*)strstr( pszNameList, pszName );
        int nNameLen = strlen(pszName);
        while( pszFind )
        {
            int nIndex = pszFind-pszNameList;
            if ( (nIndex == 0 || pszNameList[nIndex-1] == '|')
                && ( pszNameList[nIndex+nNameLen] == '\0' || pszNameList[nIndex+nNameLen] == '|' )
                )
            {
                return TRUE;
            }
            pszFind = strstr( pszFind+strlen(pszName), pszName );
        }
        return FALSE;
    }

	BOOL CTProcess::CompareProcessNameW( LPCWSTR pszName, LPCWSTR pszNameList )
	{
		if ( wcscmp( pszName, pszNameList ) == 0 )
		{
			return TRUE;
		}

		WCHAR* pszFind = (WCHAR*)wcsstr( pszNameList, pszName );
		int nNameLen = wcslen(pszName);
		while( pszFind )
		{
			int nIndex = pszFind-pszNameList;
			if ( (nIndex == 0 || pszNameList[nIndex-1] == L'|')
				&& ( pszNameList[nIndex+nNameLen] == L'\0' || pszNameList[nIndex+nNameLen] == L'|' )
				)
			{
				return TRUE;
			}
			pszFind = wcsstr( pszFind+wcslen(pszName), pszName );
		}
		return FALSE;
	}

	//֧��'|'�ָ������������Ҷ������
	DWORD CTProcess::FindProcessA( LPCSTR pszProcessName )
	{
		DWORD dwRet = 0;
		tagPROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32); 
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if( hProcessSnap != INVALID_HANDLE_VALUE
			&& hProcessSnap != NULL
			)
		{
			BOOL bMore = ::Process32First(hProcessSnap, &pe32);
			while(bMore)
			{
				if ( CompareProcessNameA(pe32.szExeFile, pszProcessName) )
				{
					dwRet = pe32.th32ProcessID;
					break;
				}
				bMore = ::Process32Next(hProcessSnap, &pe32);
			}
			::CloseHandle(hProcessSnap);
		}
		return dwRet;
	}

	DWORD CTProcess::FindProcessW( LPCWSTR pszProcessName )
	{
		DWORD dwRet = 0;
		tagPROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(pe32); 
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if( hProcessSnap != INVALID_HANDLE_VALUE
			&& hProcessSnap != NULL
			)
		{
			BOOL bMore = ::Process32FirstW(hProcessSnap, &pe32);
			while(bMore)
			{
				if ( CompareProcessNameW(pe32.szExeFile, pszProcessName) )
				{
					dwRet = pe32.th32ProcessID;
					break;
				}
				bMore = ::Process32NextW(hProcessSnap, &pe32);
			}
			::CloseHandle(hProcessSnap);
		}
		return dwRet;
	}

	DWORD CTProcess::FindProcess( const DWORD dwProcessID )
	{
		DWORD dwRet = 0;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32); 
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if( hProcessSnap != INVALID_HANDLE_VALUE
			&& hProcessSnap != NULL
			)
		{
			BOOL bMore = ::Process32First(hProcessSnap, &pe32);
			while(bMore)
			{
				if ( pe32.th32ProcessID == dwProcessID )
				{
					dwRet = pe32.th32ProcessID;
					break;
				}
				bMore = ::Process32Next(hProcessSnap, &pe32);
			}
			::CloseHandle(hProcessSnap);
		}
		return dwRet;
	}

	int CTProcess::FindProcessA( LPCSTR pszProcessName, std::vector<DWORD>* pvecRet )
	{
		int nRet = 0;
		tagPROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32); 
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if( hProcessSnap != INVALID_HANDLE_VALUE
			&& hProcessSnap != NULL
			)
		{
			BOOL bMore = ::Process32First(hProcessSnap, &pe32);
			while(bMore)
			{
				if ( CompareProcessNameA(pe32.szExeFile, pszProcessName) )
				{
					pvecRet->push_back(pe32.th32ProcessID);
				}
				bMore = ::Process32Next(hProcessSnap, &pe32);
			}
			::CloseHandle(hProcessSnap);
		}
		return nRet;
	}

	int CTProcess::FindProcessW( LPCWSTR pszProcessName, std::vector<DWORD>* pvecRet )
	{
		int nRet = 0;
		tagPROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(pe32); 
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if( hProcessSnap != INVALID_HANDLE_VALUE
			&& hProcessSnap != NULL
			)
		{
			BOOL bMore = ::Process32FirstW(hProcessSnap, &pe32);
			while(bMore)
			{
				if ( CompareProcessNameW(pe32.szExeFile, pszProcessName) )
				{
					pvecRet->push_back(pe32.th32ProcessID);
				}
				bMore = ::Process32NextW(hProcessSnap, &pe32);
			}
			::CloseHandle(hProcessSnap);
		}
		return nRet;
	}

	DWORD CTProcess::GetParentProcess( const DWORD dwChildProcessID )
	{
		typedef struct tagPROCESS_BASIC_INFORMATION  
		{  
			DWORD ExitStatus;  
			DWORD PebBaseAddress;  
			DWORD AffinityMask;  
			DWORD BasePriority;  
			ULONG UniqueProcessId;  
			ULONG InheritedFromUniqueProcessId;  
		}PROCESS_BASIC_INFORMATION;  
		PROCESS_BASIC_INFORMATION pbi;  

		typedef enum enumSYSTEM_INFORMATION_CLASS  
		{  
			SystemBasicInformation,  
			SystemProcessorInformation,  
			SystemPerformanceInformation,  
			SystemTimeOfDayInformation,  
		}SYSTEM_INFORMATION_CLASS;  

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,dwChildProcessID);  
		if(!hProcess)  
			return 0;  

		typedef LONG (WINAPI *PNTQUERYINFORMATIONPROCESS)(HANDLE,UINT,PVOID,ULONG,PULONG);  
		PNTQUERYINFORMATIONPROCESS  NtQueryInformationProcess = (PNTQUERYINFORMATIONPROCESS)GetProcAddress(GetModuleHandle(_T("ntdll")),("NtQueryInformationProcess"));  

		LONG status = NtQueryInformationProcess(hProcess,SystemBasicInformation,(PVOID)&pbi,sizeof(PROCESS_BASIC_INFORMATION),NULL);  
		DWORD                     dwParentPID = 0;  

		if(!status)
		{
			dwParentPID = pbi.InheritedFromUniqueProcessId;  
		}
		CloseHandle (hProcess);  
		return dwParentPID;
	}

    BOOL CTProcess::CloseProcess( const DWORD dwPid )
    {
        BOOL bRet = FALSE;
		HANDLE hProcess=OpenProcess(PROCESS_TERMINATE,FALSE, dwPid);
        if(hProcess)
		{
            if( TerminateProcess(hProcess,0) )
            {
                bRet = TRUE;
			}
			CloseHandle( hProcess );
		}
        return bRet;
    }

	BOOL CTProcess::CloseProcessA( LPSTR pszProcessName )
	{
		BOOL bRet = TRUE;
		std::vector<DWORD> vecId;
		int nCount = FindProcessA( pszProcessName, &vecId );
		for ( int i = 0; i < vecId.size(); i++ )
		{
			if ( !CloseProcess( vecId[i] ) )
			{
				bRet = FALSE;
			}
		}
		return bRet;
	}

	BOOL CTProcess::CloseProcessW( LPCWSTR pszProcessName )
	{
		BOOL bRet = TRUE;
		std::vector<DWORD> vecId;
		int nCount = FindProcessW( pszProcessName, &vecId );
		for ( int i = 0; i < vecId.size(); i++ )
		{
			if ( !CloseProcess( vecId[i] ) )
			{
				bRet = FALSE;
			}
		}
		return bRet;
	}

	DWORD CTProcess::CreateProcessA( LPSTR lpszCmd, BOOL bIsHide )
	{
		//�����ܵ�������ִ�н��
		DWORD dwRet = 0;
		STARTUPINFOA si = {0};
		si.cb = sizeof(STARTUPINFOA);
		GetStartupInfoA(&si);
		si.wShowWindow = bIsHide ? SW_HIDE : SW_SHOW;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.hStdError = NULL;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ����� 
		si.hStdOutput = NULL;           //�Ѵ������̵ı�׼����ض��򵽹ܵ����� 
		si.hStdInput = NULL;           //�Ѵ������̵ı�׼�����ض��򵽹ܵ����Ex 

		PROCESS_INFORMATION pi = {0};

		DWORD dwCreationFlags = CREATE_NEW_CONSOLE;//�µĽ��̽�ʹ��һ���µĿ���̨�������Ǽ̳и����̵Ŀ���̨(�����ظ����ô˺���ʱ��
		if ( ::CreateProcessA(NULL, lpszCmd, NULL, NULL, TRUE, dwCreationFlags, NULL, NULL, &si, &pi) )
		{
			dwRet = pi.dwProcessId;
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		return dwRet;
	}

	DWORD CTProcess::CreateProcessW( LPWSTR lpszCmd, BOOL bIsHide )
	{
		//�����ܵ�������ִ�н��
		DWORD dwRet = 0;
		STARTUPINFOW si = {0};
		si.cb = sizeof(STARTUPINFOW);
		GetStartupInfoW(&si);
		si.wShowWindow = bIsHide ? SW_HIDE : SW_SHOW;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.hStdError = NULL;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ����� 
		si.hStdOutput = NULL;           //�Ѵ������̵ı�׼����ض��򵽹ܵ����� 
		si.hStdInput = NULL;           //�Ѵ������̵ı�׼�����ض��򵽹ܵ����Ex 

		PROCESS_INFORMATION pi = {0};

		DWORD dwCreationFlags = CREATE_NEW_CONSOLE;//�µĽ��̽�ʹ��һ���µĿ���̨�������Ǽ̳и����̵Ŀ���̨(�����ظ����ô˺���ʱ��
		if ( ::CreateProcessW(NULL, lpszCmd, NULL, NULL, TRUE, dwCreationFlags, NULL, NULL, &si, &pi) )
		{
			dwRet = pi.dwProcessId;
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		return dwRet;
	}

    BOOL CTProcess::CreateProcessA(LPCSTR lpszCmd, DWORD &dwExitCode, char **cResult, DWORD dwMSTime, BOOL bIsHide /*= TRUE*/, LPCSTR lpszCurrentDir /*= NULL*/, BOOL bInheritHandles /*= TRUE*/, BYTE* pbyWritePipe /*= NULL*/, const int nWriteLen /*= 0*/, const DWORD dwPipeBufSize /*= 4096*/)
    {
        //�����ܵ�������ִ�н��
        SECURITY_ATTRIBUTES sa; 
        HANDLE hRead=NULL, hWrite=NULL;
        HANDLE hReadEx = NULL, hWriteEx = NULL;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
        sa.lpSecurityDescriptor = NULL; 
        sa.bInheritHandle = TRUE; 

        if(cResult != NULL)
        {
            if (!CreatePipe(&hRead, &hWrite, &sa, 0)) 
            { 
                return FALSE; 
            }
        }
        if ( pbyWritePipe && nWriteLen > 0 )
        {
            if (!CreatePipe(&hReadEx, &hWriteEx, &sa, 0)) 
            { 
                return FALSE; 
            }
        }

        STARTUPINFOA si = {0};
        si.cb = sizeof(STARTUPINFOA);
        GetStartupInfoA(&si);
        si.wShowWindow = bIsHide ? SW_HIDE : SW_SHOW;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.hStdError = hWrite;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ����� 
        si.hStdOutput = hWrite;           //�Ѵ������̵ı�׼����ض��򵽹ܵ����� 
        si.hStdInput = hReadEx;           //�Ѵ������̵ı�׼�����ض��򵽹ܵ����Ex 


        PROCESS_INFORMATION pi = {0};

        DWORD dwCreationFlags = CREATE_NEW_CONSOLE;//�µĽ��̽�ʹ��һ���µĿ���̨�������Ǽ̳и����̵Ŀ���̨(�����ظ����ô˺���ʱ��
        BOOL bOK = ::CreateProcessA(NULL, (LPSTR)lpszCmd, NULL, NULL, bInheritHandles, dwCreationFlags, NULL, lpszCurrentDir, &si, &pi);
        CloseHandle(hWrite); //�ر������̵Ĺܵ�����
        CloseHandle(hReadEx);//�ر��������Ĺܵ����Ex
        if ( pbyWritePipe && nWriteLen > 0 )
        {
            DWORD dwWrited = 0;
            while( dwWrited < nWriteLen )
            {
                DWORD dwTemp;
                if ( !WriteFile( hWriteEx, &pbyWritePipe[dwWrited], nWriteLen-dwWrited, &dwTemp, NULL ) )
                {
                    break;
                }
                dwWrited += dwTemp;
            }
        }
        CloseHandle( hWriteEx );
        if(dwMSTime==0 || !bOK)
        {
            CloseHandle(hRead); 
        }
        else
        {
            //����Զ�ȴ���������ReadFile�ж��Ƿ����������ȡ����ֵ(������WaitFor��
            if(dwMSTime != INFINITE)
            {
                DWORD dwRes = WaitForSingleObject(pi.hProcess, dwMSTime);
                if(WAIT_TIMEOUT != dwRes)
                {
                    GetExitCodeProcess(pi.hProcess, &dwExitCode);
                }
                else
                {
                    TerminateProcess(pi.hProcess,0);
                }
            }

            if(cResult != NULL)
            {
                //��ȡ�ܵ����
                byte *byteBufs = (BYTE*)calloc( dwPipeBufSize, 1);
                DWORD bytesRead = 0;

                char* pcsRead = NULL;
                DWORD dwReaded = 0;
                while (true) 
                { 
                    //TODO:�������ĳ����� �ػ�����֮��ģ���ReadFile���������˳�����Ҫ����ע��
                    if (!ReadFile(hRead, byteBufs, dwPipeBufSize, &bytesRead,NULL)) 
                        break;

                    if ( bytesRead > 0 )
                    {
                        char* pcsOld = pcsRead;
                        pcsRead = (char*)calloc( dwReaded+bytesRead+1, 1 );
						if ( pcsOld )
						{
							memcpy( pcsRead, pcsOld, dwReaded );
							free( pcsOld );
						}
                        memcpy( &pcsRead[dwReaded], byteBufs, bytesRead );
						dwReaded += bytesRead;
                    }
                }
                if ( pcsRead )
                {
                    *cResult = pcsRead;
                }
                else
                {
                    *cResult = (char*)calloc( 1, sizeof(char) );
                }
				free( byteBufs );
            }
            CloseHandle(hRead); 

            //����Զ�ȴ���������ReadFile�ж��Ƿ����������ȡ����ֵ
            if(dwMSTime == INFINITE)
            {
                if(cResult == NULL)
                {
                    DWORD dwRes = WaitForSingleObject(pi.hProcess, dwMSTime);
                }
                GetExitCodeProcess(pi.hProcess, &dwExitCode);

            }

        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        //ȥ����Ӱ�� "???" Sid: S-1-5-21-361015073-3415420457-1167146809-1001
        if(cResult != NULL)
        {
            char* pcsFind = strstr( *cResult, " Sid: " );
            if ( pcsFind != NULL )
            {
                int nIndexLen = 2;
                char* pcsIndex = strstr( pcsFind, "\r\n" );
                if ( pcsIndex == NULL )
                {
                    pcsIndex = strstr( pcsFind, "\n" );
                    nIndexLen = 1;
                }
                if ( pcsIndex )//ɾ������
                {
                    pcsIndex+=nIndexLen;

                    char* pszOld = *cResult;
                    *cResult = (char*)calloc( strlen(pcsIndex)+1, 1 );
                    memcpy( *cResult, pcsIndex, strlen(pcsIndex) );
                    free( pszOld );
                }
            }
        }
        return bOK;
    }

    BOOL CTProcess::CreateProcessW(LPWSTR lpszCmd, DWORD &dwExitCode, WCHAR **cResult, DWORD dwMSTime, BOOL bIsHide /*= TRUE*/, LPCWSTR lpszCurrentDir /*= NULL*/, BOOL bInheritHandles /*= TRUE*/, BYTE* pbyWritePipe /*= NULL*/, const int nWriteLen /*= 0*/, const DWORD dwPipeBufSize /*= 4096*/)
    {
        //�����ܵ�������ִ�н��
        SECURITY_ATTRIBUTES sa; 
        HANDLE hRead=NULL, hWrite=NULL;
        HANDLE hReadEx = NULL, hWriteEx = NULL;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
        sa.lpSecurityDescriptor = NULL; 
        sa.bInheritHandle = TRUE; 

        if(cResult != NULL)
        {
            if (!CreatePipe(&hRead, &hWrite, &sa, 0)) 
            { 
                return FALSE; 
            }
        }
        if ( pbyWritePipe && nWriteLen > 0 )
        {
            if (!CreatePipe(&hReadEx, &hWriteEx, &sa, 0)) 
            { 
                return FALSE; 
            }
        }

        STARTUPINFOW si = {0};
        si.cb = sizeof(STARTUPINFOW);
        GetStartupInfoW(&si);
        si.wShowWindow = bIsHide ? SW_HIDE : SW_SHOW;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.hStdError = hWrite;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ����� 
        si.hStdOutput = hWrite;           //�Ѵ������̵ı�׼����ض��򵽹ܵ����� 
        si.hStdInput = hReadEx;             //�Ѵ������̵ı�׼�����ض��򵽹ܵ����Ex


        PROCESS_INFORMATION pi = {0};

        DWORD dwCreationFlags = CREATE_NEW_CONSOLE;//�µĽ��̽�ʹ��һ���µĿ���̨�������Ǽ̳и����̵Ŀ���̨(�����ظ����ô˺���ʱ��
        BOOL bOK = ::CreateProcessW(NULL, lpszCmd, NULL, NULL, bInheritHandles, dwCreationFlags, NULL, lpszCurrentDir, &si, &pi);
        CloseHandle(hWrite); //�ر������̵�д���
        CloseHandle( hReadEx );//�ر������̵������

        if ( pbyWritePipe && nWriteLen > 0 )
        {
            DWORD dwWrited = 0;
            while( dwWrited < nWriteLen )
            {
                DWORD dwTemp;
                if ( !WriteFile( hWriteEx, &pbyWritePipe[dwWrited], nWriteLen-dwWrited, &dwTemp, NULL ) )
                {
                    break;
                }
                dwWrited += dwTemp;
            }
        }
        CloseHandle( hWriteEx );

        if(dwMSTime==0 || !bOK)
        {
            CloseHandle(hRead); 
        }
        else
        {
            //����Զ�ȴ���������ReadFile�ж��Ƿ����������ȡ����ֵ(������WaitFor��
            if(dwMSTime != INFINITE)
            {
                DWORD dwRes = WaitForSingleObject(pi.hProcess, dwMSTime);
                if(WAIT_TIMEOUT != dwRes)
                {
                    GetExitCodeProcess(pi.hProcess, &dwExitCode);
                }
                else
                {
                    TerminateProcess(pi.hProcess,0);
                }
            }

            if(cResult != NULL)
            {
                //��ȡ�ܵ����
                byte *byteBufs = (BYTE*)calloc( dwPipeBufSize, 1);
                DWORD bytesRead = 0;

                char* pcsRead = NULL;
                DWORD dwReaded = 0;
                while (true) 
                { 
                    //TODO:�������ĳ����� �ػ�����֮��ģ���ReadFile���������˳�����Ҫ����ע��
                    if (!ReadFile(hRead, byteBufs, dwPipeBufSize, &bytesRead,NULL)) 
                        break;

                    if ( bytesRead > 0 )
                    {
                        char* pcsOld = pcsRead;
                        pcsRead = (char*)calloc( dwReaded+bytesRead+1, 1 );
						if ( pcsOld )
						{
							memcpy( pcsRead, pcsOld, dwReaded );
							free( pcsOld );
						}
                        memcpy( &pcsRead[dwReaded], byteBufs, bytesRead );
						dwReaded += bytesRead;
                    }
                }
                if ( pcsRead )
                {
					BOOL bUtf8 = FALSE;
                    int nLen = MultiByteToWideChar( CP_ACP, 0, pcsRead, strlen(pcsRead)+1, NULL, 0 );
					if ( nLen == 0 )
					{
						nLen = MultiByteToWideChar( CP_UTF8, 0, pcsRead, strlen(pcsRead)+1, NULL, 0 );
						bUtf8 = TRUE;
					}
                    *cResult = (WCHAR*)calloc( nLen+1, sizeof(WCHAR) );
					if ( bUtf8 )
					{
						MultiByteToWideChar( CP_UTF8, 0, pcsRead, strlen(pcsRead)+1, *cResult, nLen );
					}
					else
					{
						MultiByteToWideChar( CP_ACP, 0, pcsRead, strlen(pcsRead)+1, *cResult, nLen );
					}
                    free( pcsRead );
                }
                else
                {
                    *cResult = (WCHAR*)calloc( 1, sizeof(WCHAR) );
                }
				free( byteBufs );
            }
            CloseHandle(hRead); 

            //����Զ�ȴ���������ReadFile�ж��Ƿ����������ȡ����ֵ
            if(dwMSTime == INFINITE)
            {
                if(cResult == NULL)
                {
                    DWORD dwRes = WaitForSingleObject(pi.hProcess, dwMSTime);
                }
                GetExitCodeProcess(pi.hProcess, &dwExitCode);

            }

        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        //ȥ����Ӱ�� "???" Sid: S-1-5-21-361015073-3415420457-1167146809-1001
        if(cResult != NULL)
        {
            WCHAR* pcsFind = wcsstr( *cResult, L" Sid: " );
            if ( pcsFind != NULL )
            {
                int nIndexLen = 2;
                WCHAR* pcsIndex = wcsstr( pcsFind, L"\r\n" );
                if ( pcsIndex == NULL )
                {
                    pcsIndex = wcsstr( pcsFind, L"\n" );
                    nIndexLen = 1;
                }
                if ( pcsIndex )//ɾ������
                {
                    pcsIndex+=nIndexLen;

                    WCHAR* pszOld =*cResult;
                    *cResult = (WCHAR*)calloc( wcslen(pcsIndex)+1, sizeof(WCHAR) );
                    wcscpy( *cResult, pcsIndex );
                    free( pszOld );
                }
            }
        }
        return bOK;
    }


    BOOL CTProcess::CreateProcessAndWaitDoneA(LPSTR lpszCmd, BOOL bIsHide /*= TRUE*/)
    {
        DWORD dwExitCode = 0;
        return CreateProcessA(lpszCmd,dwExitCode,NULL,INFINITE,bIsHide,NULL);
    }

    BOOL CTProcess::CreateProcessAndWaitDoneA(LPSTR lpszCmd, DWORD &dwExitCode, BOOL bIsHide /*= TRUE*/)
    {
        return CreateProcessA(lpszCmd,dwExitCode,NULL,INFINITE,bIsHide,NULL);
    }

    BOOL CTProcess::CreateProcessAndNoWaitA(LPSTR lpszCmd, BOOL bIsHide /*= TRUE*/, LPCSTR lpszCurrentDir /*= NULL*/)
    {
        DWORD dwExitCode=0;
        return CreateProcessA(lpszCmd,dwExitCode,NULL,0,bIsHide,lpszCurrentDir, FALSE);
    }

    BOOL CTProcess::CreateProcessAndWaitTimeA(LPSTR lpszCmd, DWORD &dwExitCode, DWORD dwMSTime, BOOL bIsHide /*= TRUE*/)
    {
        return CreateProcessA(lpszCmd,dwExitCode,NULL,dwMSTime,bIsHide,NULL);
    }
    
    BOOL CTProcess::CreateProcessAndWaitTimeA(LPSTR lpszCmd, std::string &szResult, DWORD dwMSTime, BOOL bIsHide /*= TRUE*/)
    {
        DWORD dwExitCode=0;
        char *cResult = NULL;
        BOOL bRet = CreateProcessA(lpszCmd,dwExitCode,&cResult,dwMSTime,bIsHide,NULL);
        szResult = cResult;
        free(cResult);
        return bRet;
    }


    BOOL CTProcess::CreateProcessAndWaitDoneW(LPWSTR lpszCmd, BOOL bIsHide /*= TRUE*/)
    {
        DWORD dwExitCode = 0;
        return CreateProcessW(lpszCmd,dwExitCode,NULL,INFINITE,bIsHide,NULL);
    }

    BOOL CTProcess::CreateProcessAndWaitDoneW(LPWSTR lpszCmd, DWORD &dwExitCode, BOOL bIsHide /*= TRUE*/)
    {
        return CreateProcessW(lpszCmd,dwExitCode,NULL,INFINITE,bIsHide,NULL);
    }

    BOOL CTProcess::CreateProcessAndNoWaitW(LPWSTR lpszCmd, BOOL bIsHide /*= TRUE*/, LPCWSTR lpszCurrentDir /*= NULL*/)
    {
        DWORD dwExitCode=0;
        return CreateProcessW(lpszCmd,dwExitCode,NULL,0,bIsHide,lpszCurrentDir, FALSE);
    }

    BOOL CTProcess::CreateProcessAndWaitTimeW(LPWSTR lpszCmd, DWORD &dwExitCode, DWORD dwMSTime, BOOL bIsHide /*= TRUE*/)
    {
        return CreateProcessW(lpszCmd,dwExitCode,NULL,dwMSTime,bIsHide,NULL);
    }

    BOOL CTProcess::CreateProcessAndWaitTimeW(LPWSTR lpszCmd, std::wstring &szResult, DWORD dwMSTime, BOOL bIsHide /*= TRUE*/)
    {
        DWORD dwExitCode=0;
        WCHAR *cResult = NULL;
        BOOL bRet = CreateProcessW(lpszCmd,dwExitCode,&cResult,dwMSTime,bIsHide,NULL);
        szResult = cResult;
        free(cResult);
        return bRet;
    }
}
