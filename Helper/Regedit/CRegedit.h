/********************************************************************
//  作者:           LiJY
//  CopyRight(c)    2013 创意嘉和 All Rights Reserved 
//  创建时间:       2013/06/19 9:51
//  类描述:          注册表操作类
//  修改时间:       
//  修改目的:       增加对64位注册表操作的问题(本身是32位程序才需要）
//  备注：            WIN7修改注册表信息，需要注意权限问题。
                    参考 http://blog.csdn.net/ljd_1986413/article/details/6248676
*********************************************************************/
#pragma once
#include <string>
using namespace std;

class  CRegedit
{
public:
    CRegedit(void);
    ~CRegedit(void);
    //************************************
    // 其返回值 :ERROR_SUCCESS为操作成功，否则为失败
    //************************************
    static LONG ReadDWORD(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,__out DWORD &dwValue, BOOL bIsVisit64Reg=FALSE);
    static LONG WriteDWORD(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,DWORD dwValue, BOOL bIsVisit64Reg=FALSE);
	static LONG ReadSZ(HKEY HKEY_, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, __out std::wstring &wstrValue, BOOL bIsVisit64Reg = FALSE);
    static LONG WriteSZ(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,LPCTSTR lpszValue, BOOL bIsVisit64Reg=FALSE);

    //设置开机启动项
    static LONG RegisterPcStartup(LPCTSTR lpszRunKeyName, LPCTSTR lpszExeRunCmd);
private:
    //************************************
    // 函数描述 :获取指定注册表位置的值
    // 其返回值 :ERROR_SUCCESS为操作成功，否则为失败（详见winerror.h的ERROR_相关定义)
    //
    // 参数详情 :HKEY HKEY_  ------  主键类型，如HKEY_LOCAL_MACHINE ..
    // 参数详情 :LPCTSTR lpszSubKey  ------  子键地址，如"SoftWare//Knight Studio//Knight"
    // 参数详情 :LPCTSTR lpszValueName  ------  键名称，如"UserName"
    // 参数详情 :__out LPDWORD lpdwType  ------  键值类型，如REG_SZ REG_DWORD
    // 参数详情 :__out LPBYTE lpData  ------  查询结果的缓冲区
    // 参数详情 :__out LPDWORD lpdwcbData  ------  查询结果的缓冲区大小
    //
    // 作者描述 :LiJY 2013/06/19 10:00
    // 其它备注 :
    //************************************
    static LONG Read(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,__out LPDWORD lpdwType,__out LPBYTE lpData,__out LPDWORD lpdwcbData, BOOL bIsVisit64Reg=FALSE);
    static LONG Write(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,DWORD dwType,LPBYTE lpData,DWORD dwcbData, BOOL bIsVisit64Reg=FALSE);
};

