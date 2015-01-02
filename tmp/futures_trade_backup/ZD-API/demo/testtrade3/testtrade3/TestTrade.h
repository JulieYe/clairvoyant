// TestTrade.h : main header file for the TESTTRADE application
//

#if !defined(AFX_TESTTRADE_H__15D3E2ED_7E71_4DB8_87DB_B8EDEF1DE191__INCLUDED_)
#define AFX_TESTTRADE_H__15D3E2ED_7E71_4DB8_87DB_B8EDEF1DE191__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestTradeApp:
// See TestTrade.cpp for the implementation of this class
//

class CTestTradeApp : public CWinApp
{
public:
	CTestTradeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestTradeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestTradeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTTRADE_H__15D3E2ED_7E71_4DB8_87DB_B8EDEF1DE191__INCLUDED_)
