// ReadCardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DllValidate.h"
#include "ReadCardDlg.h"
#include "afxdialogex.h"
#include "DllValidateDlg.h"


// ReadCardDlg dialog

IMPLEMENT_DYNAMIC(ReadCardDlg, CDialogEx)


ReadCardDlg::ReadCardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ReadCardDlg::IDD, pParent)
{

}


void ReadCardDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	extern CDllValidateApp theApp;
	//((CDllValidateDlg *)GetParent())->;//SetFocus();
	::PostMessage(AfxGetMainWnd()->m_hWnd,WM_INPUT_HAND,0,0);
}

BOOL ReadCardDlg::PreTranslateMessage(MSG* pMsg)
{
 // TODO: Add your specialized code here and/or call the base class
 if(pMsg->message==WM_KEYDOWN) 
 { 
  switch(pMsg->wParam)  
  {  
  case VK_RETURN:   //ÆÁ±ÎEnter 
	OnBnClickedButton1();
	return true;
  case VK_ESCAPE:   //ÆÁ±ÎEsc  
   return true;  
  } 
 }
 return CDialog::PreTranslateMessage(pMsg);
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
	DDX_Control(pDX, IDC_EDIT1, NameInput);
	DDX_Control(pDX, IDC_EDIT2, IDInput);
}


BEGIN_MESSAGE_MAP(ReadCardDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &ReadCardDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// ReadCardDlg message handlers
