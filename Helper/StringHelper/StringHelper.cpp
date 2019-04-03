#include "StdAfx.h"
#include "StringHelper.h"


CStringHelper::CStringHelper()
{
}


CStringHelper::~CStringHelper()
{
}


wstring CStringHelper::GetBinDir()
{
	wstring strBinDir = L"";
	TCHAR tszPath[MAX_PATH] = { NULL };
	::GetModuleFileName(AfxGetInstanceHandle(), tszPath, sizeof(tszPath));
	(wcsrchr(tszPath, '\\'))[0] = 0;
	strBinDir = tszPath;
	strBinDir += L"\\";
	return strBinDir;
}

unsigned char CStringHelper::ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}

std::wstring CStringHelper::C2W(const char *pc)
{
	string str = pc;
	int len = 0;
	len = str.length();
	int unicodeLen = ::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0 ); 
	wchar_t * pUnicode; 
	pUnicode = new wchar_t[unicodeLen+1]; 
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t)); 
	::MultiByteToWideChar( CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen ); 
	wstring rt; 
	rt = ( wchar_t* )pUnicode;
	delete pUnicode; 
	return rt; 
}
/*
wstring 转换为 string
*/
std::string CStringHelper::W2C(const wchar_t * pw)
{
	wstring str = pw;
	char* pElementText;
	int iTextLen;
	iTextLen = WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1) );
	::WideCharToMultiByte( CP_ACP,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL );
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

std::string CStringHelper::UTF82ANSI(const char* szUTF8)
{
	int WLength = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, NULL, NULL );  
	LPWSTR pszW = (LPWSTR) _alloca( (WLength + 1) *sizeof(WCHAR) ) ;      
	MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, pszW, WLength );  
	pszW[WLength] = 0;  

	int ALength = WideCharToMultiByte(CP_ACP, 0, pszW, -1, NULL, 0, NULL, NULL);  
	LPSTR pszA = ( LPSTR ) _alloca ( ALength + 1 );  
	WideCharToMultiByte(CP_ACP, 0, pszW, -1, pszA, ALength, NULL, NULL);  
	pszA[ALength] = 0;  

	return pszA;  
}

char* CStringHelper::ANSI2UTF8(const char* szAnsi)
{
	int WLength = MultiByteToWideChar(CP_ACP, 0, szAnsi, -1, NULL, 0);  
	LPWSTR pszW = (LPWSTR) _alloca((WLength+1) * sizeof(WCHAR));  
	MultiByteToWideChar(CP_ACP, 0, szAnsi, -1, pszW, WLength);  

	int ALength = WideCharToMultiByte(CP_UTF8, 0, pszW, -1, NULL, 0, NULL, NULL);  
	LPSTR pszA = (LPSTR)_alloca( ALength + 1);  
	WideCharToMultiByte(CP_UTF8, 0, pszW, -1, pszA, ALength, NULL, NULL);    
	pszA[ALength] = 0;  

	return pszA;  
}

std::string CStringHelper::UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

string CStringHelper::I2A(int nValue)
{
	string strRet;
	char szVal[MAX_PATH] = { NULL };
	itoa(nValue, szVal, 10);
	strRet = szVal;
	return strRet;
}

wstring CStringHelper::I2W(int nValue)
{
	wstring strRet;
	WCHAR szVal[MAX_PATH] = { NULL };
	_itow(nValue, szVal, 10);
	strRet = szVal;
	return strRet;
}

void CStringHelper::SplitString(CString szText, std::vector<CString>&vecString, LPCTSTR lpzsFlag)
{
	vecString.clear();
	if (szText.IsEmpty())
	{
		return;
	}
	int i = 0;
	while (szText.Find(lpzsFlag) != -1)
	{
		//这里没有考虑 lpzsFlaglpzsFlag多个紧连的情况。不过觉得修改的意义不大，由外层处理即可。 2014/03/11 16:47  LiJY
		vecString.push_back(szText.Left(szText.Find(lpzsFlag)));
		szText.Delete(0, szText.Find(lpzsFlag) + lstrlen(lpzsFlag));
		i++;
	}
	vecString.push_back(szText);
}