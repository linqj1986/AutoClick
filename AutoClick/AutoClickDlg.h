
// AutoClickDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CAutoClickDlg dialog
class CAutoClickDlg : public CDialogEx
{
// Construction
public:
	CAutoClickDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AUTOCLICK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	BOOL m_bIsBegin;
	int m_nBeginHour;
	int m_nBeginMin;
	static DWORD WINAPI AutoTimerThread( LPVOID lParam );
public:
	CStatic m_stcText1;
};
