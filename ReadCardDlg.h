#pragma once
#define WM_INPUT_HAND WM_USER+1001


// ReadCardDlg dialog

class ReadCardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ReadCardDlg)

public:
	ReadCardDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ReadCardDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };
	CStatic	m_picture;
	CEdit NameInput;
	CEdit IDInput;
	afx_msg void OnBnClickedButton1();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	BOOL PreTranslateMessage(MSG* pMsg);

};
