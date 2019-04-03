#include "StdAfx.h"
#include "THttp.h"
#include <wininet.h>
#include <stdio.h>
#pragma comment(lib,"wininet.lib")

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable:4996)

namespace TFC{
	CTHttp::CTHttp()
	{
	}
	CTHttp::~CTHttp()
	{
	}

	/*
	功能：GET 方式与web通讯 多字节版
	参数1：in	URL链接
	参数2：out	返回的数据长度
	参数3：in	链接超时时间
	参数4：in	读写超时时间
	返回值：返回的内容 注意释放内存
	*/
	BYTE* CTHttp::GetUrlContentA( LPCSTR pszUrl, DWORD &dwResultSize, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		BYTE* pbyRet = NULL;
		HINTERNET hNet = NULL;
		HINTERNET hNetFile = NULL;
		dwResultSize = 0;
		hNet = InternetOpenA( "Mozilla/4.0 (compatible)", 0, NULL, 0, 0 );
		if ( hNet )
		{
			if ( InternetSetOptionA( hNet, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID)&dwConnTimeOut, sizeof(dwConnTimeOut) )
				&& InternetSetOptionA( hNet, INTERNET_OPTION_SEND_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
				&& InternetSetOptionA( hNet, INTERNET_OPTION_RECEIVE_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
				)
			{
				hNetFile = InternetOpenUrlA( hNet, pszUrl, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE, 0 );
				if ( hNetFile )
				{
					DWORD dwStatus = 0;
					DWORD dwSize = sizeof( dwStatus );
					HttpQueryInfoA( hNetFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwSize, 0 );
					if ( dwStatus == 200  )
					{
						//获取报文大小
						char szBuf[MAX_PATH];
						DWORD dwBufSize = sizeof( szBuf );
						memset( szBuf, 0, sizeof(szBuf) );
						HttpQueryInfoA( hNetFile, HTTP_QUERY_CONTENT_LENGTH, szBuf, &dwBufSize, 0 );
						if (dwBufSize > 0)
						{
							int nBufLen = atoi(szBuf);
							pbyRet = (BYTE*)calloc(nBufLen + 1, sizeof(BYTE));
							if ( pbyRet )
							{
								if (!InternetReadFile(hNetFile, pbyRet, nBufLen, &dwResultSize))
								{
									dwResultSize = 0;
									free( pbyRet );
									pbyRet = NULL;
								}
							}
						}
					}
					InternetCloseHandle( hNetFile );
				}
			}
			InternetCloseHandle( hNet );
		}
		return pbyRet;
	}

	/*
	功能：GET 方式与web通讯 宽字节版
	参数1：in	URL链接
	参数2：out	返回的数据长度
	参数3：in	链接超时时间
	参数4：in	读写超时时间
	返回值：返回的内容 注意释放内存
	*/
	BYTE* CTHttp::GetUrlContentW( LPCWSTR pszUrl, DWORD &dwResultSize, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		BYTE* pbyRet = NULL;
		HINTERNET hNet = NULL;
		HINTERNET hNetFile = NULL;
		dwResultSize = 0;
		hNet = InternetOpenW( L"Mozilla/4.0 (compatible)", 0, NULL, 0, 0 );
		if ( hNet )
		{
			if ( InternetSetOptionW( hNet, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID)&dwConnTimeOut, sizeof(dwConnTimeOut) )
				&& InternetSetOptionW( hNet, INTERNET_OPTION_SEND_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
				&& InternetSetOptionW( hNet, INTERNET_OPTION_RECEIVE_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
				)
			{
				hNetFile = InternetOpenUrlW( hNet, pszUrl, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE, 0 );
				if ( hNetFile )
				{
					DWORD dwStatus = 0;
					DWORD dwSize = sizeof( dwStatus );
					HttpQueryInfoW( hNetFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwSize, 0 );
					if ( dwStatus == 200  )
					{
						//获取服务器文件大小
						WCHAR szBuf[MAX_PATH];
						DWORD dwBufSize = sizeof( szBuf );
						memset( szBuf, 0, sizeof(szBuf) );
						HttpQueryInfoW( hNetFile, HTTP_QUERY_CONTENT_LENGTH, szBuf, &dwBufSize, 0 );
						if (dwBufSize > 0)
						{
							pbyRet = (BYTE*)calloc(dwBufSize + 1, sizeof(BYTE));
							if ( pbyRet )
							{
								if (!InternetReadFile(hNetFile, pbyRet, dwBufSize, &dwResultSize))
								{
									dwResultSize = 0;
									free( pbyRet );
									pbyRet = NULL;
								}
							}
						}
					}
					InternetCloseHandle( hNetFile );
				}
			}
			InternetCloseHandle( hNet );
		}
		return pbyRet;
	}

	BOOL CTHttp::GetUrlContentA( LPCSTR pszUrl, std::string &strRet, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		BOOL bRet = FALSE;
		DWORD dwResultLen = 0;
		BYTE* pbyRet = GetUrlContentA( pszUrl, dwResultLen, dwConnTimeOut, dwIoTimeOut );
		if ( pbyRet )
		{
			bRet = TRUE;
			strRet = (char*)pbyRet;
			free(pbyRet);
		}
		return bRet;
	}

	BOOL CTHttp::GetUrlContentW( LPCWSTR pszUrl, std::string &strRet, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		BOOL bRet = FALSE;
		DWORD dwResultLen = 0;
		BYTE* pbyRet = GetUrlContentW( pszUrl, dwResultLen, dwConnTimeOut, dwIoTimeOut );
		if ( pbyRet )
		{
			bRet = TRUE;
			strRet = (char*)pbyRet;
			free(pbyRet);
		}
		return bRet;
	}

	/*
	功能：POST 方式与web通讯 多字节版 http://www.baidu.com?data=xxxxxxxx
	参数1：in	URL链接  如 www.baidu.com
	参数2：in	发送的数据 data=xxxxxxxx
	参数3：in	链接的长度
	参数4：out	返回的内容长度
	参数5：in	链接超时时间
	参数6：in	读写超时时间
	返回值：返回的内容 注意释放内存
	*/
	BYTE* CTHttp::PostUrlContentA( LPCSTR pszUrl, LPCSTR pszPostContent, const int nPostContentLen, DWORD &dwResultSize, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		OutputDebugStringA(pszUrl);
		BYTE* pbyRet = NULL;
		const char szContentType[] = "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n";
		//const char szContentType[] = "Content-Type: application/json\r\n";
		LPCSTR lpAcceptType[2] = { "application/json, text/javascript, */*; q=0.01", NULL };
		//检测http头
		const char *szHead[] = {
			"http://",
			"https://",
		};
		int nHeadLen = 0;
		BOOL bFind = FALSE;
		for ( int i = 0; i < _countof(szHead); i++ )
		{
			nHeadLen = strlen(szHead[i]);
			if ( strncmp( pszUrl, szHead[i], nHeadLen ) == 0 )
			{
				bFind = TRUE;
				break;
			}
		}

		if ( bFind )
		{
			//截断服务器地址和地址之后的请求字串
			const char* pszRequest = strstr( &pszUrl[nHeadLen], "/" );
			if ( pszRequest )
			{
				char szServerAddr[MAX_PATH];
				memset( szServerAddr, 0, sizeof(szServerAddr) );
				strncpy( szServerAddr, pszUrl, pszRequest-pszUrl );

				//提取端口号
				USHORT usPort = 80;
				
				char* pszPort = strstr( &szServerAddr[nHeadLen], ":" );
				if ( pszPort )
				{
					usPort = atoi( &pszPort[1] );
					pszPort[0] = 0;
				}
				//开始连接
				HINTERNET hNet = InternetOpenA( "Mozilla/5.0 (Windows NT 6.1; Win64; x64)", 0, NULL, 0, 0 );
				if ( hNet )
				{
					if ( InternetSetOptionA( hNet, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID)&dwConnTimeOut, sizeof(dwConnTimeOut) )
						&& InternetSetOptionA( hNet, INTERNET_OPTION_SEND_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
						&& InternetSetOptionA( hNet, INTERNET_OPTION_RECEIVE_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
						)
					{
						HINTERNET hConnect = InternetConnectA( hNet, &szServerAddr[nHeadLen], usPort, NULL, NULL, INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0 );
						if ( hConnect )
						{
							HINTERNET hRequest = HttpOpenRequestA( hConnect, "POST", pszRequest, HTTP_VERSIONA, "", lpAcceptType, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT, 0 );
							if ( hRequest )
							{
								char szHeaders[MAX_PATH];
								memset(szHeaders, 0, sizeof(szHeaders));
								sprintf(szHeaders, "Host: %s\r\n", &szServerAddr[nHeadLen]);
								HttpAddRequestHeadersA(hConnect, szHeaders, strlen(szHeaders), HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

								if ( HttpSendRequestA( hRequest, szContentType, strlen(szContentType), (LPVOID)pszPostContent, nPostContentLen ) )
								{

									DWORD dwStatus = 0;
									DWORD dwSize = sizeof(dwStatus);
									HttpQueryInfo( hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwSize, 0 );
									if ( dwStatus == 200 )
									{
										dwResultSize = 0;
										DWORD dwTemp;
										BYTE szBuf[1024];
										BYTE* pbyOld = NULL;;
										while( InternetReadFile( hRequest, szBuf, sizeof(szBuf), &dwTemp ) )
										{
											if ( dwTemp > 0 )
											{
												pbyOld = pbyRet;
												pbyRet = (BYTE*)calloc( dwResultSize+dwTemp+1, sizeof(BYTE) );
												if ( pbyOld )
												{
													memcpy( pbyRet, pbyOld, dwResultSize );
													free( pbyOld );
													pbyOld = NULL;
												}
												memcpy( &pbyRet[dwResultSize], szBuf, dwTemp );
												dwResultSize += dwTemp;
											}
											else
											{
												break;
											}
										}
									}
								}

								DWORD dwCode = GetLastError();
								InternetCloseHandle( hRequest );
								hRequest = NULL;
								//_tDebugPrint(L"HttpSendRequestA code=%d", dwCode);
							}

							InternetCloseHandle( hConnect );
							hConnect = NULL;
						}
						else
						{
							OutputDebugStringA("InternetConnectA 失败");
						}
					}

					InternetCloseHandle( hNet );
					hNet = NULL;
				}
				else
				{
					
				}
			}
			else
			{
				OutputDebugStringA("pszRequest not found");
			}
		}
		else
		{
			OutputDebugStringA("http not found");
		}
		return pbyRet;
	}

	/*
	功能：POST 方式与web通讯 宽字符版 http://www.baidu.com?data=xxxxxxxx
	参数1：in	URL链接  如 www.baidu.com
	参数2：in	发送的数据 data=xxxxxxxx
	参数3：in	链接的长度
	参数4：out	返回的内容长度
	参数5：in	链接超时时间
	参数6：in	读写超时时间
	返回值：返回的内容 注意释放内存
	*/
	BYTE* CTHttp::PostUrlContentW( LPCWSTR pszUrl, LPCWSTR pszPostContent, const int nPostContentLen, DWORD &dwResultSize, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		BYTE* pbyRet = NULL;
		const WCHAR szContentType[] = L"Content-Type: application/x-www-form-urlencoded\r\n";
		LPCWSTR lpAcceptType[2] = { L"*/*", NULL };
		//检测http头
		const WCHAR *szHead[] = {
			L"http://",
			L"https://",
		};
		int nHeadLen = 0;
		BOOL bFind = FALSE;
		for ( int i = 0; i < _countof(szHead); i++ )
		{
			nHeadLen = wcslen(szHead[i]);
			if ( wcsncmp( pszUrl, szHead[i], nHeadLen ) == 0 )
			{
				bFind = TRUE;
				break;
			}
		}

		if ( bFind )
		{
			//截断服务器地址和地址之后的请求字串
			const WCHAR* pszRequest = wcsstr( &pszUrl[nHeadLen], L"/" );
			if ( pszRequest )
			{
				WCHAR szServerAddr[MAX_PATH];
				memset( szServerAddr, 0, sizeof(szServerAddr) );
				wcsncpy( szServerAddr, pszUrl, pszRequest-pszUrl );

				//提取端口号
				USHORT usPort = 80;

				WCHAR* pszPort = wcsstr( szServerAddr, L":" );
				if ( pszPort )
				{
					usPort = _wtoi( &pszPort[1] );
					pszPort[0] = 0;
				}
				//开始连接
				HINTERNET hNet = InternetOpenW( L"Mozilla/4.0 (compatible)", 0, NULL, 0, 0 );
				if ( hNet )
				{
					if ( InternetSetOptionW( hNet, INTERNET_OPTION_CONNECT_TIMEOUT, (LPVOID)&dwConnTimeOut, sizeof(dwConnTimeOut) )
						&& InternetSetOptionW( hNet, INTERNET_OPTION_SEND_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
						&& InternetSetOptionW( hNet, INTERNET_OPTION_RECEIVE_TIMEOUT, (LPVOID)&dwIoTimeOut, sizeof(dwIoTimeOut) )
						)
					{
						HINTERNET hConnect = InternetConnectW( hNet, &szServerAddr[nHeadLen], usPort, NULL, NULL, INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0 );
						if ( hConnect )
						{
							HINTERNET hRequest = HttpOpenRequestW( hConnect, L"POST", pszRequest, HTTP_VERSIONW, L"", lpAcceptType, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT, 0 );
							if ( hRequest )
							{
								WCHAR szHeaders[MAX_PATH];
								memset( szHeaders, 0, sizeof(szHeaders) );
								swprintf( szHeaders, L"Host: %s\r\n", &szServerAddr[nHeadLen] );
								HttpAddRequestHeadersW( hConnect, szHeaders, wcslen(szHeaders), HTTP_ADDREQ_FLAG_ADD );
								if ( HttpSendRequestW( hRequest, szContentType, wcslen(szContentType), (LPVOID)pszPostContent, nPostContentLen ) )
								{

									DWORD dwStatus = 0;
									DWORD dwSize = sizeof(dwStatus);
									HttpQueryInfo( hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwSize, 0 );
									if ( dwStatus == 200 )
									{
										dwResultSize = 0;
										DWORD dwTemp;
										BYTE szBuf[1024];
										BYTE* pbyOld = NULL;;
										while( InternetReadFile( hRequest, szBuf, sizeof(szBuf), &dwTemp ) )
										{
											if ( dwTemp > 0 )
											{
												pbyOld = pbyRet;
												pbyRet = (BYTE*)calloc( dwResultSize+dwTemp+1, sizeof(BYTE) );
												if ( pbyOld )
												{
													memcpy( pbyRet, pbyOld, dwResultSize );
													free( pbyOld );
													pbyOld = NULL;
												}
												memcpy( &pbyRet[dwResultSize], szBuf, dwTemp );
												dwResultSize += dwTemp;
											}
											else
											{
												break;
											}
										}
									}
								}
								DWORD dwCode = GetLastError();
								InternetCloseHandle( hRequest );
								hRequest = NULL;
							}

							InternetCloseHandle( hConnect );
							hConnect = NULL;
						}
					}

					InternetCloseHandle( hNet );
					hNet = NULL;
				}
			}
		}
		return pbyRet;
	}

	BOOL CTHttp::PostUrlContentA( LPCSTR pszUrl, LPCSTR pszPostContent, const int nPostContentLen, std::string &strRet, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		strRet = "";
		BOOL bRet = FALSE;
		DWORD dwResultSize = 0;
		BYTE* pbyRet = PostUrlContentA( pszUrl, pszPostContent, nPostContentLen, dwResultSize, dwConnTimeOut, dwIoTimeOut );
		if ( pbyRet )
		{
			bRet = TRUE;
			strRet = (char*)pbyRet;
			free( pbyRet );
		}
		return bRet;
	}

	BOOL CTHttp::PostUrlContentW( LPCWSTR pszUrl, LPCWSTR pszPostContent, const int nPostContentLen, std::string &strRet, const DWORD dwConnTimeOut, const DWORD dwIoTimeOut )
	{
		BOOL bRet = FALSE;
		DWORD dwResultSize = 0;
		BYTE* pbyRet = PostUrlContentW( pszUrl, pszPostContent, nPostContentLen, dwResultSize, dwConnTimeOut, dwIoTimeOut );
		if ( pbyRet )
		{
			bRet = TRUE;
			strRet = (char*)pbyRet;
			free(pbyRet);
		}
		return bRet;
	}
}
