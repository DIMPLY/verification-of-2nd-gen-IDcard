#pragma once


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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
