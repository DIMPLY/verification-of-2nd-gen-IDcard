#pragma once
#include "afxwin.h"

#define WM_INPUT_HAND WM_USER+1001
// InputDlg dialog

class InputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputDlg)

public:
	InputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~InputDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit NameInput;
	CEdit IDInput;
	afx_msg void OnBnClickedButton1();
	BOOL PreTranslateMessage(MSG* pMsg);
};
