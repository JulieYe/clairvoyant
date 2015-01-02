// TestTradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestTrade.h"
#include "TestTradeDlg.h"

#include "TradeSpi.h"
#include "TradeThread.h"
#include "ZDQHMdApi.h"
#include "ZDQHFtdcTradeApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "thosttraderapi.lib")
#pragma comment(lib, "ZDQHMdUserAPI.lib")
#pragma comment(lib, "EsunnyApi.lib")
#pragma comment(lib, "USTPtraderapi.lib")

CTestTradeDlg* CTestTradeDlg::s_pThis = NULL;

CZDQHFtdcMdApi* g_pZDQHApi = NULL;
CZDQHFtdcTradeApi* g_pZdQHTradeapi = NULL;
CUstpFtdcTraderApi *g_puserapi = NULL;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestTradeDlg dialog

CTestTradeDlg::CTestTradeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestTradeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestTradeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_pUserApi = NULL;
	g_pUserSpi = NULL;
	m_pTradeThread = NULL;
	s_pThis = this;
}

CTestTradeDlg::~CTestTradeDlg()
{
	if(m_pTradeThread != NULL)
	{
		m_pTradeThread->Release();
		m_pTradeThread = NULL;
	}

	if(g_pUserApi != NULL)
	{
		g_pUserApi->Release();
		g_pUserApi = NULL;
	}

	if(g_pUserSpi != NULL)
	{
		delete g_pUserSpi;
		g_pUserSpi = NULL;
	}
}

void CTestTradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestTradeDlg)
	DDX_Control(pDX, IDC_CHECK_ANY_PRICE, m_chkAnyPrice);
	DDX_Control(pDX, IDC_BTN_CLEAR_LOG, m_btnClearLog);
	DDX_Control(pDX, IDC_LIST_INFO, m_listInfo);
	DDX_Control(pDX, IDC_CHECK_HEGE_FLAG, m_chkHegeFlag);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_btnLogin);
	DDX_Control(pDX, IDC_EDIT_VOLUME, m_editVolume);
	DDX_Control(pDX, IDC_COMBO_OFFSET_FLAG, m_comboOffsetFlag);
	DDX_Control(pDX, IDC_COMBO_INSTRUMENTID, m_comboInstrumentID);
	DDX_Control(pDX, IDC_COMBO_DIRECTION, m_comboDirection);
	DDX_Control(pDX, IDC_EDIT_SESSION_ID, m_editSessionID);
	DDX_Control(pDX, IDC_EDIT_ORDER_SYS_ID, m_editOrderSysID);
	DDX_Control(pDX, IDC_EDIT_ORDER_REF, m_editOrderRef);
	DDX_Control(pDX, IDC_EDIT_FRONT_ID, m_editFrontID);
	DDX_Control(pDX, IDC_EDIT_PRICE, m_editPrice);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_INVESTORID, m_editInvestorID);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestTradeDlg, CDialog)
	//{{AFX_MSG_MAP(CTestTradeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOGIN, OnBtnLogin)
	ON_BN_CLICKED(IDC_BTN_ORDER, OnBtnOrder)
	ON_BN_CLICKED(IDC_BTN_ORDER_ACTION, OnBtnOrderAction)
	ON_MESSAGE(UM_ADD_MSG, OnAddMessage)
	ON_BN_CLICKED(IDC_BTN_CLEAR_LOG, OnBtnClearLog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_ANY_PRICE, OnCheckAnyPrice)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButtonSub)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestTradeDlg message handlers

BOOL CTestTradeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	::GetPrivateProfileString("serv_cfg", "front_address", "tcp://asp-sim2-front1.financial-trading-platform.com:26205", FRONT_ADDR, sizeof(FRONT_ADDR), "./test_config.ini");
	::GetPrivateProfileString("serv_cfg", "broker_id", "2030", BROKER_ID, sizeof(BROKER_ID), "./test_config.ini");

	g_nEnvType = GetPrivateProfileInt("serv_cfg", ("env_type"), 0, "./test_config.ini")!=0;

	m_listInfo.SendMessage(LB_SETHORIZONTALEXTENT, 1000, 0);

	if(m_pTradeThread != NULL)
	{
		m_pTradeThread->Release();
		m_pTradeThread = NULL;
	}

	FILE* fp = fopen(g_szLogFileName, "w");
	if(fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}

	char szInvestorID[13] = { 0 }, szPassword[41] = { 0 };
	::GetPrivateProfileString("user_info", "user_name", "352222", g_szInvestorID, sizeof(g_szInvestorID), "./test_config.ini");
	::GetPrivateProfileString("user_info", "password", "888888", g_szPassword, sizeof(g_szPassword), "./test_config.ini");
	m_editInvestorID.SetWindowText(szInvestorID);
	m_editPassword.SetWindowText(szPassword);
	
	char szInstrumentCode[31], szInstrumentID[31];
	for(int i = 1; i <= 4; i++)
	{
		memset(szInstrumentID, 0, sizeof(szInstrumentID));
		memset(szInstrumentCode, 0, sizeof(szInstrumentCode));
		sprintf(szInstrumentCode, "iid%d", i);

		::GetPrivateProfileString("instrument", szInstrumentCode, "", szInstrumentID, sizeof(szInstrumentID), "./test_config.ini");
		if(strcmp(szInstrumentID, "") != 0)
		{
			m_comboInstrumentID.AddString(szInstrumentID);
		}
	}
	if(m_comboInstrumentID.GetCount() > 0)
	{
		m_comboInstrumentID.SetCurSel(0);
	}

	m_comboDirection.AddString("买入");
	m_comboDirection.AddString("卖出");
	m_comboDirection.SetItemData(0, '0');
	m_comboDirection.SetItemData(1, '1');
	m_comboDirection.SetCurSel(0);

	m_comboOffsetFlag.AddString("开仓");
	m_comboOffsetFlag.AddString("平仓");
	m_comboOffsetFlag.SetItemData(0, '0');
	m_comboOffsetFlag.SetItemData(1, '1');
	m_comboOffsetFlag.SetCurSel(0);

	m_editVolume.SetWindowText("1");
	m_editPrice.SetWindowText("2850.0");

	CRect rcClient;
	GetClientRect(&rcClient);
	int cx = rcClient.Width();
	int cy = rcClient.Height();
//	ResetLayOut(cx, cy);
	
	//m_pTradeThread = new CTradeThread;
	//m_pTradeThread->CreateThread(NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestTradeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestTradeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestTradeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestTradeDlg::OnBtnLogin() 
{
	if( g_pZDQHApi == NULL)
	{
		
		char ip[25]={0};
		int port;

		//创建行情api
		g_pZDQHApi = CreateFtdcMdApi("");
		
		
		//注册回调类
		CZDQHAPISpi* pSpi = new CZDQHAPISpi();
		g_pZDQHApi->RegisterSpi(pSpi);

		g_pZDQHApi->Init();

	

	}
}

LRESULT CTestTradeDlg::OnAddMessage(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;
	char* pMsg = (char*)lParam;
	if(pMsg != NULL)
	{
		int nIndex = m_listInfo.AddString(pMsg);
	
		pMsg = NULL;

		if(nIndex >= 0)
		{
			m_listInfo.SetCurSel(nIndex);
			m_listInfo.SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
		}
	}
	return 0;
}
int g_order=140543;
char g_szOrder[25]={0};
void CTestTradeDlg::OnBtnOrder() 
{
	CUstpFtdcInputOrderField ord;
	memset(&ord,0,sizeof(CUstpFtdcInputOrderField));
	strcpy(ord.InvestorID, "000000222355");
	strcpy(ord.InstrumentID, "IF1310");
	ord.OrderPriceType = '2';
	ord.Direction = '0';
	ord.OffsetFlag = '0';
	ord.HedgeFlag = '1';
	ord.LimitPrice = 2440;
	ord.Volume = 1;
	ord.TimeCondition = '1';	
	strcpy(ord.BrokerID,"0213");
	strcpy(ord.UserID,"000000222355");
	strcpy(ord.ExchangeID,"CFFEX");
	ord.VolumeCondition='1';
	ord.ForceCloseReason='0';
	sprintf(ord.UserOrderLocalID, "%12d",++g_order);	
	if (g_puserapi)
		g_puserapi->ReqOrderInsert(&ord,++g_order);
	return ;
	// TODO: Add your control notification handler code here
	double dPrice;
	int nVolume;
	CString strInvestorID, strInstrumentID, strPrice, strVolume;
	DWORD dwDirection, dwOffsetFlag, dwStart = 0, dwStop = 0;
	int nHegeFlag = 0;
	BOOL bAnyPrice = FALSE;

	m_editInvestorID.GetWindowText(strInvestorID);
	m_comboInstrumentID.GetWindowText(strInstrumentID);
	dwDirection = m_comboDirection.GetItemData(m_comboDirection.GetCurSel());
	dwOffsetFlag = m_comboDirection.GetItemData(m_comboOffsetFlag.GetCurSel());
	m_editVolume.GetWindowText(strVolume);
	if(m_chkAnyPrice.GetCheck() == 0){
		m_editPrice.GetWindowText(strPrice);
		dPrice = atof(strPrice);
		bAnyPrice = FALSE;
	}
	else{
		dPrice = 0.0;
		bAnyPrice = TRUE;
	}
	nHegeFlag = m_chkHegeFlag.GetCheck();

	TThostFtdcInvestorIDType szInvestorID;
	TThostFtdcInstrumentIDType szInstrumentID;
	strcpy(szInvestorID, strInvestorID);
	strcpy(szInstrumentID, strInstrumentID);
	
	
	nVolume = atoi(strVolume);
	if(strInvestorID.IsEmpty() == FALSE || strPrice.IsEmpty() == FALSE)
	{
		//g_pUserSpi->ReqOrderInsert(szInvestorID, szInstrumentID, dwDirection, dwOffsetFlag, nVolume, dPrice, bAnyPrice, nHegeFlag);

		//////////////////////////////////////////////////////////////////////////
		/*
		dwStart = GetTickCount();
		for(int i = 0; i < 10000; i++)
		{
			dPrice += 0.2;
			if(dPrice > 2900)
			{
				dPrice = 2850;
			}
			g_pUserSpi->ReqOrderInsert(szInvestorID, szInstrumentID, dwDirection, dwOffsetFlag, nVolume, dPrice);
		}
		dwStop = GetTickCount();
		CString strMsg;
		strMsg.Format("%d", dwStop - dwStart);
		MessageBox(strMsg);
		*/
		//////////////////////////////////////////////////////////////////////////
	}	
	else
	{
		MessageBox("用户名或密码为空！ ",  "提示信息", MB_OK|MB_ICONINFORMATION);
	}
}

void CTestTradeDlg::OnBtnOrderAction() 
{
	return;
	// TODO: Add your control notification handler code here
	CString strUserID;
	CString strFrontID, strSessionID, strOrderSysID, strOrderRef; 
	m_editInvestorID.GetWindowText(strUserID);
	m_editFrontID.GetWindowText(strFrontID);
	m_editSessionID.GetWindowText(strSessionID);
	m_editOrderRef.GetWindowText(strOrderRef);
	m_editOrderSysID.GetWindowText(strOrderSysID);
	
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, strUserID);
	strcpy(req.UserID, strUserID);
	
	req.FrontID = atoi(strFrontID);
	req.SessionID = atoi(strSessionID);
	strcpy(req.OrderRef, strOrderRef);

	if(g_nEnvType == 0)
	{
		strcpy(req.ExchangeID, "SHFE");
	}
	else
	{
		strcpy(req.ExchangeID, "CFFEX");
	}
	sprintf(req.OrderSysID, "%12s", strOrderSysID);

	req.ActionFlag = THOST_FTDC_AF_Delete;

	g_pUserSpi->ReqOrderAction(&req);
}

void CTestTradeDlg::OnBtnClearLog() 
{
	// TODO: Add your control notification handler code here
	m_listInfo.ResetContent();
}

void CTestTradeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	ResetLayOut(cx, cy);
}

void CTestTradeDlg::ResetLayOut(int cx, int cy)
{
	int nLeft = 400;
	int cxNew = cx - nLeft - 10;
	int cyNew = cy - 20;

	if(m_listInfo.GetSafeHwnd())
	{
		m_listInfo.SetWindowPos(NULL, nLeft, 10, cxNew, cyNew, SWP_NOACTIVATE);
	}
}

void CTestTradeDlg::OnCheckAnyPrice() 
{
	// TODO: Add your control notification handler code here
	if(m_chkAnyPrice.GetCheck() == 0)
	{
		m_editPrice.EnableWindow(TRUE);	
	}
	else
	{
		m_editPrice.EnableWindow(FALSE);	
	}
}

void CTestTradeDlg::OnOK() 
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}

void CTestTradeDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here


	CThostFtdcUserLogoutField req;

	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, g_szInvestorID);
	g_pUserApi->ReqUserLogout(&req, ++g_iRequestID);
	
}

void CTestTradeDlg::OnButtonSub() 
{
	// TODO: Add your control notification handler code here
	if (g_pZDQHApi)
	{
		TZDQHExchangeType exchange = {0};
		strcpy(exchange, "CFFEX");
		char* contract = "IF1310";
		g_pZDQHApi->SubscribeMarketData(&exchange,&contract,1);
	}
	
}

void CTestTradeDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	if (g_pZDQHApi)
	{
		TZDQHExchangeType exchange = {0};
		strcpy(exchange, "CFFEX");
		char* contract = "IF1310";
		g_pZDQHApi->UnSubscribeMarketData(&exchange,&contract,1);
	}
}
