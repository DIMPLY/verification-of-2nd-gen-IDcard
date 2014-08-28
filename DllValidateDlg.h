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
#include "resource.h"		// 主符号
#include <sstream>
#include "afxwin.h"
#include "afxcmn.h"
#include "ReadCardDlg.h"
#include "mysql.h"
#include "buttonset.h"


#pragma comment (lib, "WS2_32.LIB")
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CDllValidateDlg dialog





class CDllValidateDlg : public CDialog
{
// Construction
public:
	CDllValidateDlg(CWnd* pParent = NULL);	// standard constructor

	void getActs(HTREEITEM root);

	char Gender[4];
	char Folk[11];
	char BirthDay[9];
	char Address[71];
	char Agency[31];
	char ExpireStart[9];
	char ExpireEnd[9];
	char pcSAMID[37];
	char pIINSNDN[17];
	TCHAR *ImgString,*JpgBack,*JpgFront;
	char Name[31];
	char Code[19];
	CStringArray NAME,ID,NAMEntckt,IDntckt;
	void WriteAPerson(CString matchName, CString matchID, BOOL ticket, BOOL buy);

	static BOOL Thread;
	CUIntArray AIDarray;
	CUIntArray AID_no_tckt_array;

	void stripCR(TCHAR **BASE64);
	BOOL IsNum(CString &str);

public:
	CButtonSet bset;
	void inputhand();
	void READ();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL PreTranslateMessage(MSG* pMsg);
	enum button_modes 
	{
		IDLE,		//清零用
		INFORM,		//仅通知信息
		MOREIN,		//入场次数超限
		FUZZYNTCKT,		//准备列出模糊匹配未购票者
		EXACTNTCKT,	//姓名证号下无票
		FUZZY,	//准备列出模糊匹配购票者
		NOCUST,		//无该用户询问是否购票
		CANTLOCK,	//锁定时没有选中类别
		READNLOCK,	//未锁定时读卡
//		NOSEL,		//未选择类别时读卡或手动验证
		INPUTNLOCK,	//未锁定时手动验证
		ASKIFWR
	};
	button_modes infoNo;
	int newCust;
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton7();

	CString limitcopy;
	char tempid[200];
	char uid[200];

	// Dialog Data
	//{{AFX_DATA(CDllValidateDlg)
	enum { IDD = IDD_DLLVALIDATE_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllValidateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CDllValidateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNcDestroy();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	int Readcard();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CString getCurActPath(HTREEITEM hItem,int AID);
	void SetAllChildrenSame(HTREEITEM hItem,BOOL bChk);
	void UpdateParentItem(HTREEITEM hItem,BOOL bChk);
	void GotoEnd();
	void ParseValidation(int);
	int Validate();

	static void ReadThreadProc(LPVOID lParam);

	void getInfo(CString matchID,CString matchName);
	ReadCardDlg readcardDlg;
	CButton button;
	CTabCtrl tabs;
	CTreeCtrl ActivitiesTree;
	CListCtrl FuzzyList;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	HICON m_hIcon;

private:
	MYSQL mysql;
	CFont m_font;
public:
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonThis();
	afx_msg void OnBnClickedButtonNone();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLVALIDATEDLG_H__E6E97A76_E87E_4775_95D4_7BE1686E89A5__INCLUDED_)
