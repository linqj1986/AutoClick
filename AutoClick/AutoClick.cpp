
// AutoClick.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AutoClick.h"
#include "AutoClickDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  MUTEX_NAME   L"CCB|AutoClick|Tool"
#define  WINDOW_NAME   L"MyIE"

// CAutoClickApp

BEGIN_MESSAGE_MAP(CAutoClickApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoClickApp construction

CAutoClickApp::CAutoClickApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAutoClickApp object

CAutoClickApp theApp;


// CAutoClickApp initialization

BOOL CAutoClickApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 创建进程互斥量(根据模拟器类型和小号ID)
	m_cstrAppMutexName.Format( TEXT("%s_%d"), MUTEX_NAME,0 );
	m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, m_cstrAppMutexName);
	if( m_hMutex != NULL )
	{
		// 保留旧程序，本新程序则退出
		MessageBoxA(NULL,"进程已存在","AutoClick提示",MB_OK);
		return FALSE;
	}
	m_hMutex = CreateMutex(NULL,TRUE, m_cstrAppMutexName);

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CAutoClickDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

