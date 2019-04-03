
// AutoClick.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CAutoClickApp:
// See AutoClick.cpp for the implementation of this class
//

class CAutoClickApp : public CWinApp
{
public:
	CAutoClickApp();

// Overrides
public:
	CString m_cstrAppMutexName;
	HANDLE m_hMutex;
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CAutoClickApp theApp;