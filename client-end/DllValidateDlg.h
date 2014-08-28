// DllValidateDlg.h : header file
//

#if !defined(AFX_DLLVALIDATEDLG_H__E6E97A76_E87E_4775_95D4_7BE1686E89A5__INCLUDED_)
#define AFX_DLLVALIDATEDLG_H__E6E97A76_E87E_4775_95D4_7BE1686E89A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxinet.h>
#include <iostream>
#include <winsock2.h>
#include <stdlib.h>
#include <String.h>
#include "resource.h"		// Ö÷·ûºÅ
#include <sstream>
#include "afxwin.h"
#include "afxcmn.h"
#include "ReadCardDlg.h"
#include "InputDlg.h"

#pragma comment (lib, "WS2_32.LIB")
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDllValidateDlg dialog

class CDllValidateDlg : public CDialog
{
// Construction
public:
	CDllValidateDlg(CWnd* pParent = NULL);	// standard constructor
	static string MultiByteToUtf8( const char* pszMultiByte );
	static string Utf8ToMultiByte(string pszUtf8);
	void getActs(HTREEITEM root);
	static int AID;
	char Name[31];
	char Code[19];
	static CString IDinfo_post;
	void stripCR(TCHAR **BASE64);
	CStringArray NAME,ID;
	BOOL IsNum(CString &str);
	static BOOL Thread;
	struct PASSdata
	{
		CString matchName;
		CString matchID;
		int ticket;
	};
// Dialog Data
	//{{AFX_DATA(CDllValidateDlg)
	enum { IDD = IDD_DLLVALIDATE_DIALOG };
	//}}AFX_DATA
	static BOOL PostHttpPage(CString &result,CString &PageName,CString &postData);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllValidateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDllValidateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	int OnReadcard();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CString getCurActPath(HTREEITEM hItem);
	void GotoEnd();
	void ValidateAndParse(CString);
	void updateAndPass(CString,CString,int);
	static void ValidateThread(LPVOID lParam);
	static void ReadThreadProc(LPVOID lParam);
	static void UPndPASSThread(LPVOID lParam);
	static void newCustomerThread(LPVOID lParam);
	static void getInfoThread(LPVOID lParam);
	ReadCardDlg readcardDlg;
	InputDlg inputDlg;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl ActivitiesTree;
	void inputhand();
	void READ();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl FuzzyList;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	CTabCtrl tabs;
	afx_msg void OnTcnSelchangeTab3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL PreTranslateMessage(MSG* pMsg);
private:
	CBrush m_brush;
	CFont m_font;
public:
	afx_msg void OnBnClickedButton6();
	CButton button;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLVALIDATEDLG_H__E6E97A76_E87E_4775_95D4_7BE1686E89A5__INCLUDED_)
