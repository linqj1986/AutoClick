/********************************************************************
//  ����:           LiJY
//  CopyRight(c)    2013 ����κ� All Rights Reserved 
//  ����ʱ��:       2013/06/19 9:51
//  ������:          ע��������
//  �޸�ʱ��:       
//  �޸�Ŀ��:       ���Ӷ�64λע������������(������32λ�������Ҫ��
//  ��ע��            WIN7�޸�ע�����Ϣ����Ҫע��Ȩ�����⡣
                    �ο� http://blog.csdn.net/ljd_1986413/article/details/6248676
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
    // �䷵��ֵ :ERROR_SUCCESSΪ�����ɹ�������Ϊʧ��
    //************************************
    static LONG ReadDWORD(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,__out DWORD &dwValue, BOOL bIsVisit64Reg=FALSE);
    static LONG WriteDWORD(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,DWORD dwValue, BOOL bIsVisit64Reg=FALSE);
	static LONG ReadSZ(HKEY HKEY_, LPCTSTR lpszSubKey, LPCTSTR lpszValueName, __out std::wstring &wstrValue, BOOL bIsVisit64Reg = FALSE);
    static LONG WriteSZ(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,LPCTSTR lpszValue, BOOL bIsVisit64Reg=FALSE);

    //���ÿ���������
    static LONG RegisterPcStartup(LPCTSTR lpszRunKeyName, LPCTSTR lpszExeRunCmd);
private:
    //************************************
    // �������� :��ȡָ��ע���λ�õ�ֵ
    // �䷵��ֵ :ERROR_SUCCESSΪ�����ɹ�������Ϊʧ�ܣ����winerror.h��ERROR_��ض���)
    //
    // �������� :HKEY HKEY_  ------  �������ͣ���HKEY_LOCAL_MACHINE ..
    // �������� :LPCTSTR lpszSubKey  ------  �Ӽ���ַ����"SoftWare//Knight Studio//Knight"
    // �������� :LPCTSTR lpszValueName  ------  �����ƣ���"UserName"
    // �������� :__out LPDWORD lpdwType  ------  ��ֵ���ͣ���REG_SZ REG_DWORD
    // �������� :__out LPBYTE lpData  ------  ��ѯ����Ļ�����
    // �������� :__out LPDWORD lpdwcbData  ------  ��ѯ����Ļ�������С
    //
    // �������� :LiJY 2013/06/19 10:00
    // ������ע :
    //************************************
    static LONG Read(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,__out LPDWORD lpdwType,__out LPBYTE lpData,__out LPDWORD lpdwcbData, BOOL bIsVisit64Reg=FALSE);
    static LONG Write(HKEY HKEY_,LPCTSTR lpszSubKey,LPCTSTR lpszValueName,DWORD dwType,LPBYTE lpData,DWORD dwcbData, BOOL bIsVisit64Reg=FALSE);
};

