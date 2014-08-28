// ReadCardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DllValidate.h"
#include "ReadCardDlg.h"
#include "afxdialogex.h"


// ReadCardDlg dialog

IMPLEMENT_DYNAMIC(ReadCardDlg, CDialogEx)

ReadCardDlg::ReadCardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ReadCardDlg::IDD, pParent)
{

}

ReadCardDlg::~ReadCardDlg()
{
}

void ReadCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ReadCardDlg)
	DDX_Control(pDX, IDC_Pic, m_picture);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(ReadCardDlg, CDialogEx)
END_MESSAGE_MAP()


// ReadCardDlg message handlers
