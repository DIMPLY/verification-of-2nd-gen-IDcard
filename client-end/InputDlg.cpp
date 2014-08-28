// InputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DllValidate.h"
#include "InputDlg.h"
#include "afxdialogex.h"


// InputDlg dialog

IMPLEMENT_DYNAMIC(InputDlg, CDialogEx)

InputDlg::InputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(InputDlg::IDD, pParent)
{

}

InputDlg::~InputDlg()
{
}

void InputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, NameInput);
	DDX_Control(pDX, IDC_EDIT2, IDInput);
}


BEGIN_MESSAGE_MAP(InputDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &InputDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// InputDlg message handlers


void InputDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	extern CDllValidateApp theApp;
	::SendMessage(AfxGetMainWnd()->m_hWnd,WM_INPUT_HAND,0,0);
}

BOOL InputDlg::PreTranslateMessage(MSG* pMsg)
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
