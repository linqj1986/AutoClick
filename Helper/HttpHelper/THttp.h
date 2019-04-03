#pragma once
#include <wtypes.h>
#include <string>

namespace TFC{

#ifdef _UNICODE
#define GetUrlContentT                  GetUrlContentW
#define PostUrlContentT                 PostUrlContentW
#else
#define GetUrlContentT                  GetUrlContentA
#define PostUrlContentT                 PostUrlContentA
#endif


	class CTHttp{
	public:
		CTHttp();
		virtual ~CTHttp();

		static BYTE* GetUrlContentA( LPCSTR pszUrl, DWORD &dwResultSize, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );//���ص�BYTE* ��Ҫ�ⲿ�����ͷ�
		static BYTE* GetUrlContentW( LPCWSTR pszUrl, DWORD &dwResultSize, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );//���ص�BYTE* ��Ҫ�ⲿ�����ͷ�

		static BOOL GetUrlContentA( LPCSTR pszUrl, std::string &strRet, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );
		static BOOL GetUrlContentW( LPCWSTR pszUrl, std::string &strRet, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );

		static BYTE* PostUrlContentA( LPCSTR pszUrl, LPCSTR pszPostContent, const int nPostContentLen, DWORD &dwResultSize, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );//���ص�BYTE* ��Ҫ�ⲿ�����ͷ�
		static BYTE* PostUrlContentW( LPCWSTR pszUrl, LPCWSTR pszPostContent, const int nPostContentLen, DWORD &dwResultSize, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );//���ص�BYTE* ��Ҫ�ⲿ�����ͷ�

		static BOOL PostUrlContentA( LPCSTR pszUrl, LPCSTR pszPostContent, const int nPostContentLen, std::string &strRet, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );
		static BOOL PostUrlContentW( LPCWSTR pszUrl, LPCWSTR pszPostContent, const int nPostContentLen, std::string &strRet, const DWORD dwConnTimeOut = 15000, const DWORD dwIoTimeOut = 30000 );
    };

}
