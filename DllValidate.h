// DllValidate.h : main header file for the DLLVALIDATE application
//

#if !defined(AFX_DLLVALIDATE_H__5E6F20D0_F97C_47A8_9279_E30413FDD9DD__INCLUDED_)
#define AFX_DLLVALIDATE_H__5E6F20D0_F97C_47A8_9279_E30413FDD9DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllValidateApp:
// See DllValidate.cpp for the implementation of this class
//

class CDllValidateApp : public CWinApp
{
public:
	CDllValidateApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllValidateApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDllValidateApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLVALIDATE_H__5E6F20D0_F97C_47A8_9279_E30413FDD9DD__INCLUDED_)
