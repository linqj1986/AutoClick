#pragma once

#include <xstring>
#include <vector>
using namespace std;

class CStringHelper
{
public:
	CStringHelper();
	virtual ~CStringHelper();
	static wstring GetBinDir();
	static unsigned char ToHex(unsigned char x);
	static std::wstring C2W(const char *pc);
	static std::string W2C(const wchar_t * pw);
	static std::string UTF82ANSI(const char* szUTF8);
	static char* ANSI2UTF8(const char* szAnsi);
	static std::string UrlEncode(const std::string& str);
	static string I2A(int nValue);
	static wstring I2W(int nValue);
	static void SplitString(CString szText, std::vector<CString>&vecString, LPCTSTR lpzsFlag);
};

