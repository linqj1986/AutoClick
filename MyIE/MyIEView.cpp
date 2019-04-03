//Download by http://www.NewXing.com
// MyIEView.cpp : CMyIEView 类的实现
//

#include "stdafx.h"
#include "MyIE.h"

#include "MyIEDoc.h"
#include "MyIEView.h"
#include ".\myieview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "comutil.h"

// CMyIEView

IMPLEMENT_DYNCREATE(CMyIEView, CHtmlView)

BEGIN_MESSAGE_MAP(CMyIEView, CHtmlView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, CHtmlView::OnFilePrint)
	ON_COMMAND(ID_Fill, OnFill)
END_MESSAGE_MAP()

// CMyIEView 构造/析构

char* KS_ANSI_to_UTF8 (const char* szAnsi)
{
	if (szAnsi == NULL)
		return NULL ;

	_bstr_t   bstrTmp (szAnsi) ;
	int       nLen = ::WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)bstrTmp, -1, NULL, 0, NULL, NULL) ;
	char      * pUTF8 = new char[nLen+1] ;
	ZeroMemory (pUTF8, nLen + 1) ;
	::WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)bstrTmp, -1, pUTF8, nLen, NULL, NULL) ;
	return pUTF8 ;
}

string GetModuleFileDirectory()
{
	char szPath[MAX_PATH] = { NULL };
	GetModuleFileNameA(NULL, szPath, MAX_PATH);
	string strPath = szPath;
	strPath = strPath.substr(0, strPath.rfind('\\') + 1);
	return strPath;
}

string GetUserName()
{
	string strDst;
	string strCfg = GetModuleFileDirectory();
	strCfg += "data.ini";
	int id = GetPrivateProfileIntA("config", "curid", 0, strCfg.c_str());

	char szID[10]={NULL};
	char szBuf[100]={NULL};
	itoa(id,szID,10);
	GetPrivateProfileStringA(szID, "num", "6217002740031623050", szBuf, sizeof(szBuf), strCfg.c_str());
	strDst = szBuf;
	return strDst;
}

CMyIEView::CMyIEView()
{
	// TODO: 在此处添加构造代码

}

CMyIEView::~CMyIEView()
{
}

BOOL CMyIEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CHtmlView::PreCreateWindow(cs);
}

void CMyIEView::OnInitialUpdate()
{
	time=0;
	//wstrUser = ::GetCommandLineW();
	//OutputDebugStringW(wstrUser.c_str());
	CHtmlView::OnInitialUpdate();
	Navigate2(_T("https://ibsbjstar.ccb.com.cn/CCBIS/V6/STY1/CN/login.jsp"),navNoHistory|navNoWriteToCache|navNoReadFromCache,NULL);

	DWORD dwAutoTimerThreadID;
	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)AutoTimerThread, this, NULL, &dwAutoTimerThreadID );
}


// CMyIEView 打印



// CMyIEView 诊断

#ifdef _DEBUG
void CMyIEView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CMyIEView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CMyIEDoc* CMyIEView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyIEDoc)));
	return (CMyIEDoc*)m_pDocument;
}
#endif //_DEBUG

// CMyIEView 消息处理程序

void CMyIEView::OnFill()
{
	// TODO: 在此添加命令处理程序代码

	//DWORD dwAutoTimerThreadID;
	//CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)AutoTimerThread, this, NULL, &dwAutoTimerThreadID );
}

DWORD CMyIEView::AutoTimerThread( LPVOID lParam )
{
	CMyIEView *pThis = (CMyIEView*) lParam;

	if (1)
	{
		Sleep(6000);

		CRect rect;
		pThis->GetWindowRect(&rect);
		POINT p;
		GetCursorPos(&p);
		SetCursorPos(1000+rect.left,240+rect.top);

		// 点击用户名位置
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);

		Sleep(1000);

		// 输入用户名
		string strUser = GetUserName();
		for (int i=0;i<strUser.length();i++)
		{
			int x = strUser[i];
			keybd_event(x,0,0,0);
			keybd_event(x,0,KEYEVENTF_KEYUP,0);
		}
		

		Sleep(1000);

		// 点击密码位置
		SetCursorPos(1000+rect.left,290+rect.top);

		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);

		Sleep(1000);

		// 输入密码
		for (int i=0;i<6;i++)
		{
			keybd_event(65,0,0,0);
			keybd_event(65,0,KEYEVENTF_KEYUP,0);
		}

		Sleep(1000);

		// 按钮位置
		SetCursorPos(1000+rect.left,340+rect.top);
		mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
		mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);

		Sleep(5000);
		pThis->Navigate2("about:blank",navNoHistory|navNoWriteToCache|navNoReadFromCache,NULL);
		//Sleep(2000);
		//pThis->Navigate2(_T("https://ibsbjstar.ccb.com.cn/CCBIS/V6/STY1/CN/login.jsp"),navNoHistory|navNoWriteToCache|navNoReadFromCache,NULL);
		//Sleep(5000);
		ExitProcess(0);
	}
	

	return NULL;
}

