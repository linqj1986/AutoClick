
// AutoClickDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoClick.h"
#include "AutoClickDlg.h"
#include "afxdialogex.h"
#include <tlhelp32.h>
#include <string>
#include "../Helper/Helper.h"

using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ResetData()
{
	string strDst;
	string strCfg = CFileHelper::GetModuleFileDirectory();
	strCfg += "data.ini";
	
	WritePrivateProfileStringA("config", "curid", "0", strCfg.c_str());
	WritePrivateProfileStringA("config", "curtime", "0", strCfg.c_str());
}

int Next()
{
	string strDst;
	string strCfg = CFileHelper::GetModuleFileDirectory();
	strCfg += "data.ini";

	int total = GetPrivateProfileIntA("config", "total", 0, strCfg.c_str());
	int times = GetPrivateProfileIntA("config", "times", 0, strCfg.c_str());
	int curid = GetPrivateProfileIntA("config", "curid", 0, strCfg.c_str());
	int curtime = GetPrivateProfileIntA("config", "curtime", 0, strCfg.c_str());

	if (curtime>times-1)
	{
		if (curid>=total-1)
		{
			//
			return 0;
		} else {
			curid++;
			char szID[10]={NULL};
			itoa(curid,szID,10);
			WritePrivateProfileStringA("config", "curid", szID, strCfg.c_str());

			WritePrivateProfileStringA("config", "curtime", "0", strCfg.c_str());
		}
	} else {
		curtime++;
		char szCurTime[10]={NULL};
		itoa(curtime,szCurTime,10);
		WritePrivateProfileStringA("config", "curtime", szCurTime, strCfg.c_str());
	}
	return 1;
}

void RunProcess(CString cstrPath,CString cstrParam)
{
	if (_taccess(cstrPath, 0) == -1)
	{
		return ;
	}
	string strDir = CFileHelper::GetModuleFileDirectory();
	CString szFileDir = CStringHelper::C2W(strDir.c_str()).c_str();
	CString szFilePath = cstrPath;
	SHELLEXECUTEINFO execinfo;
	memset(&execinfo, 0, sizeof(execinfo)); 
	execinfo.lpFile = szFilePath;
	execinfo.cbSize = sizeof(execinfo);
	execinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	execinfo.nShow = SW_SHOW;
	execinfo.lpParameters = cstrParam;
	execinfo.lpDirectory = szFileDir;

	if (ShellExecuteEx(&execinfo))
	{
		CloseHandle(execinfo.hProcess);
	}
}

void KillExe(const TCHAR* szExeName) 
{ 
	if (szExeName == NULL) 
		return ; 
	int lsr = lstrlen(szExeName); 
	if (lsr == 0) 
		return ; 

	PROCESSENTRY32 my; 
	HANDLE l = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	if (((int)l) != -1) 
	{ 
		my.dwSize = sizeof(my); 
		if (Process32First(l, &my)) 
		{ 
			do 
			{ 
				if (lstrcmpi(szExeName, my.szExeFile + lstrlen(my.szExeFile) - lsr) == 0) 
				{ 
					HANDLE hProcessHandle; 
					hProcessHandle = OpenProcess(0x1F0FFF, true, my.th32ProcessID); 
					if (hProcessHandle != NULL) 
					{ 
						if (TerminateProcess(hProcessHandle, 0) != NULL) 
						{ 
							CloseHandle(hProcessHandle); 
						} 
					} 
					CloseHandle(l); 
					return; 
				} 
			}while (Process32Next(l, &my)); 
		} 
		CloseHandle(l); 
	} 
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAutoClickDlg dialog




CAutoClickDlg::CAutoClickDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoClickDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoClickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_stcText1);
}

BEGIN_MESSAGE_MAP(CAutoClickDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAutoClickDlg message handlers

BOOL CAutoClickDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//
	DWORD dwAutoTimerThreadID;
	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)AutoTimerThread, this, NULL, &dwAutoTimerThreadID );

	//
	string strCfg = CFileHelper::GetModuleFileDirectory();
	strCfg += "data.ini";
	m_nBeginHour = GetPrivateProfileIntA("config", "beginhour", 19, strCfg.c_str());
	m_nBeginMin = GetPrivateProfileIntA("config", "beginmin", 30, strCfg.c_str());

	CString cstrText;
	cstrText.Format(L"自动点击服务每日%d点%d分开启",m_nBeginHour,m_nBeginMin);
	m_stcText1.SetWindowTextW(cstrText);

	//
	this->SetWindowPos(0,0,0,450,350,SWP_NOMOVE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAutoClickDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoClickDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoClickDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAutoClickDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	/*wstring strPath;
	TCHAR szPath[MAX_PATH] = {NULL};
	wcscat(szPath,L"MyIE.exe");
	KillExe(szPath);

	ShellExecute(NULL, L"open", szPath, NULL, NULL,SW_SHOW);*/

	CDialogEx::OnTimer(nIDEvent);
}

DWORD CAutoClickDlg::AutoTimerThread( LPVOID lParam )
{
	CAutoClickDlg *pThis = (CAutoClickDlg*) lParam;
	pThis->m_bIsBegin = FALSE;
	Sleep(2*1000);
	while(1)
	{
		CTime	timeCurrent = CTime::GetCurrentTime();
		int hour = timeCurrent.GetHour();
		int min = timeCurrent.GetMinute();
		if( pThis->m_bIsBegin ||
			((hour == pThis->m_nBeginHour)&&(min ==pThis->m_nBeginMin))
		  )
		{
			//每天指定时间开始任务
			if (Next()>0)
			{
				if (pThis->m_bIsBegin == FALSE)
				{
					LogToFile("任务开始", "log");
				}
				pThis->m_bIsBegin = TRUE;
				TCHAR szPath[MAX_PATH] = {NULL};
				string strDir = CFileHelper::GetModuleFileDirectory();
				wstring wstrPath = CStringHelper::C2W(strDir.c_str());
				wstrPath += L"MyIE.exe";
				KillExe(wstrPath.c_str());
				//ShellExecute(NULL, L"open", szPath, L"6217002740031623054", NULL,SW_SHOW);
				RunProcess(wstrPath.c_str(),L"");
				Sleep(10*1000);
				Sleep(7*1000);
			} else {
				// 任务执行结束
				Sleep(5*1000);
				pThis->m_bIsBegin = FALSE;
				LogToFile("任务结束", "log");
			}
		} else {
			// 任务结束并且时间已过
			ResetData();
			Sleep(20*1000);
		}
		
	}
}