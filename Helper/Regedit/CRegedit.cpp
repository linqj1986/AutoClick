#include "StdAfx.h"
#include "CRegedit.h"

#include <windows.h>
CRegedit::CRegedit(void)
{
}


CRegedit::~CRegedit(void)
{
}

LONG CRegedit::ReadDWORD(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,__out DWORD &dwValue, BOOL bIsVisit64Reg/*=FALSE*/)
{
    DWORD Type = REG_DWORD;
    DWORD len = sizeof(DWORD);
    LONG ret = Read(HKEY_,lpszSubKey,lpszValueName,&Type,(LPBYTE)&dwValue,&len, bIsVisit64Reg);
    if(!ret || Type != REG_DWORD)
        return ret;

    return ret;
}

LONG CRegedit::ReadSZ(HKEY HKEY_, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, __out std::wstring &wstrValue, BOOL bIsVisit64Reg/*=FALSE*/)
{
    DWORD Type = REG_SZ;
    DWORD len=2048;
	TCHAR szValue[2048] = { NULL };
    LONG ret = Read(HKEY_,lpszSubKey,lpszValueName,&Type,(LPBYTE)(szValue),&len, bIsVisit64Reg);
	wstrValue = wstring(szValue);
    if(!ret || Type != REG_SZ)
        return ret;

    return ret;
}

LONG CRegedit::WriteDWORD(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,DWORD dwValue, BOOL bIsVisit64Reg/*=FALSE*/)
{
    return Write(HKEY_,lpszSubKey,lpszValueName,REG_DWORD,(LPBYTE)&dwValue,sizeof(dwValue), bIsVisit64Reg);
}

LONG CRegedit::WriteSZ(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,LPCTSTR lpValue, BOOL bIsVisit64Reg/*=FALSE*/)
{
#ifdef _UNICODE
    return Write(HKEY_,lpszSubKey,lpszValueName,REG_SZ,(LPBYTE)((lpValue)),lstrlen(lpValue)*2, bIsVisit64Reg);
#else
    return Write(HKEY_,lpszSubKey,lpszValueName,REG_SZ,(LPBYTE)((lpValue)),lstrlen(lpValue), bIsVisit64Reg);
#endif
}

LONG CRegedit::Read(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,__out LPDWORD lpdwType,__out LPBYTE lpData,__out LPDWORD lpdwcbData, BOOL bIsVisit64Reg/*=FALSE*/)
{
    HKEY hKey;
    REGSAM samDesired = KEY_READ;
    if(bIsVisit64Reg) samDesired |= KEY_WOW64_64KEY;

    LONG ret = ::RegOpenKeyEx(HKEY_,lpszSubKey,0,samDesired,&hKey);
    if(ret != ERROR_SUCCESS)
    {
        return ret;
    }

    ret = ::RegQueryValueEx(hKey,lpszValueName,NULL,lpdwType,lpData,lpdwcbData);
    if(ret != ERROR_SUCCESS)
    {
        ::RegCloseKey(hKey);
        return ret;
    }

    ::RegCloseKey(hKey);
    return ERROR_SUCCESS;
}

LONG CRegedit::Write(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,DWORD dwType,LPBYTE lpData,DWORD dwcbData, BOOL bIsVisit64Reg/*=FALSE*/)
{
    HKEY hKey;
    REGSAM samDesired = KEY_WRITE;
    if(bIsVisit64Reg) samDesired |= KEY_WOW64_64KEY;
    LONG ret = ::RegOpenKeyEx(HKEY_,lpszSubKey,0,samDesired,&hKey);
    if(ret != ERROR_SUCCESS)
    {
        DWORD dwDisposition;
        ret = ::RegCreateKeyEx(HKEY_, lpszSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
        if (ret != ERROR_SUCCESS)
        {
            ::RegCloseKey(hKey);
            return ret;
        }
    }

    ret = ::RegSetValueEx(hKey,lpszValueName,NULL,dwType,lpData,dwcbData);
    if(ret != ERROR_SUCCESS)
    {
        ::RegCloseKey(hKey);
        return ret;
    }

    ::RegCloseKey(hKey);
    return ERROR_SUCCESS;
}


LONG CRegedit::RegisterPcStartup(LPCTSTR lpszRunKeyName, LPCTSTR lpszExeRunCmd)
{
    HKEY hKey;
    REGSAM samDesired = KEY_WRITE;
    LONG ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),0,samDesired,&hKey);
    if(ret != ERROR_SUCCESS)
    {
        return ret;
    }

    DWORD dwcbData = lstrlen(lpszExeRunCmd)*sizeof(TCHAR);
    ret = ::RegSetValueEx(hKey,lpszRunKeyName,NULL,REG_SZ,(LPBYTE)(lpszExeRunCmd),dwcbData);
    if(ret != ERROR_SUCCESS)
    {
        ::RegCloseKey(hKey);
        return ret;
    }

    ::RegCloseKey(hKey);
    return ERROR_SUCCESS;
}

