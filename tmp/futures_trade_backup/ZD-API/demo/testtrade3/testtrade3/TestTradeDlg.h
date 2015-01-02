// TestTradeDlg.h : header file
//

#if !defined(AFX_TESTTRADEDLG_H__2183C13C_3E97_4F68_8C4B_1B6194FE6D91__INCLUDED_)
#define AFX_TESTTRADEDLG_H__2183C13C_3E97_4F68_8C4B_1B6194FE6D91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestTradeDlg dialog

class CTradeThread;

class CTestTradeDlg : public CDialog
{
public:
	CTradeThread* m_pTradeThread;
	static CTestTradeDlg* s_pThis;

private:
	void ResetLayOut(int cx, int cy);

// Construction
public:
	CTestTradeDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CTestTradeDlg();

// Dialog Data
	//{{AFX_DATA(CTestTradeDlg)
	enum { IDD = IDD_TESTTRADE_DIALOG };
	CButton	m_chkAnyPrice;
	CButton	m_btnClearLog;
	CListBox	m_listInfo;
	CButton	m_chkHegeFlag;
	CButton	m_btnLogin;
	CEdit	m_editVolume;
	CComboBox	m_comboOffsetFlag;
	CComboBox	m_comboInstrumentID;
	CComboBox	m_comboDirection;
	CEdit	m_editSessionID;
	CEdit	m_editOrderSysID;
	CEdit	m_editOrderRef;
	CEdit	m_editFrontID;
	CEdit	m_editPrice;
	CEdit	m_editPassword;
	CEdit	m_editInvestorID;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestTradeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestTradeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnLogin();
	afx_msg void OnBtnOrder();
	afx_msg void OnBtnOrderAction();
	afx_msg LRESULT OnAddMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnClearLog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCheckAnyPrice();
	virtual void OnOK();
	afx_msg void OnButton1();
	afx_msg void OnButtonSub();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTTRADEDLG_H__2183C13C_3E97_4F68_8C4B_1B6194FE6D91__INCLUDED_)
