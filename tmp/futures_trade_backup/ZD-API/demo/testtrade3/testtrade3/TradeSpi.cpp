// TradeSpi.cpp: implementation of the CTradeSpi class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestTrade.h"
#include "TradeSpi.h"
#include "TestTradeDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////


CTradeSpi::CTradeSpi()
{
}

CTradeSpi::~CTradeSpi()
{
}

//////////////////////////////////////////////////////////////////////////
// 是否收到成功的响应
bool CTradeSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));	

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
// 是否我的报单回报
bool CTradeSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
			(pOrder->SessionID == SESSION_ID) &&
			(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

//////////////////////////////////////////////////////////////////////////
// 是否正在交易的报单
bool CTradeSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

//////////////////////////////////////////////////////////////////////////
// 当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void CTradeSpi::OnFrontConnected()
{
	WLOG("连接交易服务器成功", TRUE);
	//if(g_bReLogin)
	{
		g_bReLogin = false;
		ReqUserLogin(g_szInvestorID, g_szPassword);
	}
}

//////////////////////////////////////////////////////////////////////////
// 登录
extern CZDQHFtdcMdApi* g_pZDQHApi;
void CTradeSpi::ReqUserLogin(TThostFtdcInvestorIDType szInvestorID1, TThostFtdcPasswordType szPassword1)
{
	strcpy(g_szInvestorID, szInvestorID);
	strcpy(g_szPassword, szPassword);

	CZDQHFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, szInvestorID);
	strcpy(req.Password, szPassword);
	strcpy(req.UserProductInfo, "xiuju");
//	int nResult = g_pZDQHApi->ReqUserLogin(&req, ++g_iRequestID);
	
//	char szLog[256] = { 0 };
//	sprintf(szLog, "发送登录请求, nResult=%d", nResult);
//	WLOG(szLog, TRUE);
}

void CTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szLog[256] = { 0 };
	sprintf(szLog, "%s", "收到登录响应");
	WLOG(szLog, FALSE);

	if(pRspInfo != NULL)
	{
		memset(szLog, 0, sizeof(szLog));
		sprintf(szLog, "nErrorID=%d, szErrorMsg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		strcpy(szLog, "pRspInfo为NULL");	
	}
	WLOG(szLog, FALSE);

	if(pRspInfo != NULL && pRspInfo->ErrorID != 0)
	{
		CString strMsg;
		strMsg.Format("登录失败！\r\n%s", pRspInfo->ErrorMsg);
		::MessageBox(NULL, strMsg, "提示信息", MB_OK|MB_ICONINFORMATION);
	}

	if(bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		int iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		g_iOrderRef = iNextOrderRef;

		CString strTemp;
		strTemp.Format("%d", FRONT_ID);
		CTestTradeDlg::s_pThis->m_editFrontID.SetWindowText(strTemp);

		strTemp.Format("%d", SESSION_ID);
		CTestTradeDlg::s_pThis->m_editSessionID.SetWindowText(strTemp);
		
		memset(szLog, 0, sizeof(szLog));
		sprintf(szLog, "FrontID=%d\nSessionID=%d\nMaxOrderRef=%d", pRspUserLogin->FrontID, pRspUserLogin->SessionID, iNextOrderRef);
		
		WLOG(szLog, TRUE);

		ReqSettlementInfoConfirm(pRspUserLogin->UserID);
	}
	else
	{
		WriteSeparator();
	}
}


//////////////////////////////////////////////////////////////////////////
void CTradeSpi::ReqUserLogout(TThostFtdcInvestorIDType szInvestorID)
{
	CThostFtdcUserLogoutField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, szInvestorID);
	int iResult = g_pUserApi->ReqUserLogout(&req, ++g_iRequestID);
	TRACE("\n---------->>> 发送登出请求, nResult=%d\n", iResult);
}

void CTradeSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TRACE("\n---------->>> 登出响应: ");
	if(pRspInfo != NULL)
	{
		TRACE("nErrorID = %d, szErrorMsg = %s\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		TRACE("\n");
	}
}


//////////////////////////////////////////////////////////////////////////
// 投资者结算结果确认
void CTradeSpi::ReqSettlementInfoConfirm(TThostFtdcInvestorIDType szInvestorID)
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, szInvestorID);
	int nResult = g_pUserApi->ReqSettlementInfoConfirm(&req, ++g_iRequestID);

	char szLog[256] = { 0 };
	sprintf(szLog, "发送投资者结算结果确认, nResult=%d", nResult);
	WLOG(szLog, TRUE);
}

void CTradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szLog[256] = { 0 };
	sprintf(szLog, "%s", "收到投资者结算结果确认响应");
	WLOG(szLog, FALSE);

	if(bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		memset(szLog, 0, sizeof(szLog));
		sprintf(szLog, "nErrorID = %d, szErrorMsg = %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		WLOG(szLog, TRUE);
		::MessageBox(NULL, "  登录成功！     ", "提示信息", MB_OK|MB_ICONINFORMATION);
		//CTestTradeDlg::s_pThis->m_btnLogin.EnableWindow(FALSE);
	}
	else
	{
		WriteSeparator();
	}
}


//////////////////////////////////////////////////////////////////////////
///报单录入请求响应
void CTradeSpi::ReqOrderInsert(TThostFtdcInvestorIDType szInvestorID, TThostFtdcInstrumentIDType szInstrumentID, 
							   DWORD dwDirection, DWORD dwOffsetFlag, int nVolume, double dPrice, BOOL bAnyPrice, int nHegeFlag)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, szInvestorID);
	strcpy(req.InstrumentID, szInstrumentID);
	
	int nOrderRef = GetOrderRef();
	sprintf(req.OrderRef, "%d", nOrderRef);
	
	if(bAnyPrice)
	{
		req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
		req.TimeCondition = THOST_FTDC_TC_IOC;
	}
	else
	{
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice; 
		req.LimitPrice = dPrice;
		req.TimeCondition = THOST_FTDC_TC_GFD;
	}
	
	//req.Direction = THOST_FTDC_D_Buy;
	req.Direction = (char)dwDirection;

	if(g_nEnvType == 0)
	{
		if(dwOffsetFlag == '0')	{
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		}
		else if(dwOffsetFlag == '1'){
			req.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
		}
	}
	else{
		req.CombOffsetFlag[0] = (char)dwOffsetFlag;
	}
	
	if(nHegeFlag == 0){
		req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	}
	else{
		req.CombHedgeFlag[0] = THOST_FTDC_HF_Hedge;
	}

	req.VolumeTotalOriginal = nVolume;
	req.VolumeCondition = THOST_FTDC_VC_AV;
	req.MinVolume = 1;
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	req.IsAutoSuspend = 0;
	req.UserForceClose = 0;
	
	int nResult = g_pUserApi->ReqOrderInsert(&req, ++g_iRequestID);
	
	char* pszMsg = new char[1024];
	memset(pszMsg, 0, 1024);

	sprintf(pszMsg, "●下单请求, 帐号:%s, 合约:%s, 买卖:%c, 开平:%c, 数量:%d, 价格:%.2lf, 套保:%c, 发送:%s", szInvestorID, szInstrumentID, dwDirection, dwOffsetFlag, nVolume, dPrice, req.CombHedgeFlag[0], nResult==0?"成功":"失败");
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);

	char szTemp[256] = { 0 };
	sprintf(szTemp, "发送下单请求: nResult=%d", nResult);
	WLOG(szTemp, FALSE);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "FrontID=%d\nSessionID=%d\nOrderRef=%s", FRONT_ID, SESSION_ID, req.OrderRef);
	WLOG(szTemp, TRUE);
}

void CTradeSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(pRspInfo)
	{
		char* pszMsg = new char[1024];
		memset(pszMsg, 0, 1024);
		sprintf(pszMsg, "○下单错误, 帐号:%s, 合约:%s, 买卖:%c, 开平:%c, 数量:%d, 价格:%.2lf, 套保:%c, 错误:%s", pInputOrder->InvestorID, pInputOrder->InstrumentID, pInputOrder->Direction, pInputOrder->CombOffsetFlag[0], pInputOrder->VolumeTotalOriginal, pInputOrder->LimitPrice, pInputOrder->CombHedgeFlag[0], pRspInfo->ErrorMsg);
		::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);
	}

	char szTemp[256] = { 0 };
	strcpy(szTemp, "下单回报, OnRspOrderInsert");
	WLOG(szTemp, FALSE);

	if(pRspInfo)
	{
		memset(szTemp, 0, sizeof(szTemp));
		sprintf(szTemp, "nErrorID=%d, szErrorMsg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		WLOG(szTemp, FALSE);
	}

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "FrontID=%d\nSessionID=%d\nOrderRef=%s", FRONT_ID, SESSION_ID, pInputOrder->OrderRef);
	WLOG(szTemp, TRUE);
}

void CTradeSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	char* pszMsg = new char[1024];
	memset(pszMsg, 0, 1024);
	CString strOrderSysID = pOrder->OrderSysID;
	strOrderSysID.TrimLeft();
	strOrderSysID.TrimRight();
	if(strOrderSysID.IsEmpty())
	{
		strOrderSysID = "(NULL)";
	}
	sprintf(pszMsg, "☆委托回报, 帐号:%s, 合约:%s, 买卖:%c, 开平:%c, 数量:%d, 价格:%.2lf, 套保:%c, 单号:%s, 状态:%s", pOrder->InvestorID, pOrder->InstrumentID, pOrder->Direction, pOrder->CombOffsetFlag[0], pOrder->VolumeTotalOriginal, pOrder->LimitPrice, pOrder->CombHedgeFlag[0], strOrderSysID, pOrder->StatusMsg);
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);

	char szTemp[256] = { 0 };
	strcpy(szTemp, "收到下单回报, OnRtnOrder");
	WLOG(szTemp, FALSE);
	
	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "FrontID=%d\nSessionID=%d\nOrderRef=%s\n", pOrder->FrontID, pOrder->SessionID, pOrder->OrderRef);
	WLOG(szTemp, FALSE);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "BrokerID=%s\nBrokerOrderSeq=%d\nOrderLocalID=%s\n", pOrder->BrokerID, pOrder->BrokerOrderSeq, pOrder->OrderLocalID);
	WLOG(szTemp, FALSE);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "ExchangeID=%s\nOrderSysID=%s\n", pOrder->ExchangeID, pOrder->OrderSysID);
	WLOG(szTemp, FALSE);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "OrderStatus=%c, StatusMsg=%s", pOrder->OrderStatus, pOrder->StatusMsg);
	WLOG(szTemp, FALSE);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "NotifySequence=%d", pOrder->NotifySequence);
	WLOG(szTemp, TRUE);

	if(pOrder->NotifySequence != 0 && strcmp(pOrder->OrderSysID, "") == 0)
	{
		if(pOrder->OrderStatus != NULL)
		{
			TRACE("%s\n", pOrder->StatusMsg);
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
///报单操作请求响应
void CTradeSpi::ReqOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction)
{
	int nRtn = g_pUserApi->ReqOrderAction(pInputOrderAction, ++g_iRequestID);
	char* pszMsg = new char[1024];
	memset(pszMsg, 0, 1024);

	CString strOrderSysID = pInputOrderAction->OrderSysID;
	strOrderSysID.TrimLeft();
	strOrderSysID.TrimRight();
	sprintf(pszMsg, "■撤单请求, 帐号:%s, 单号:%s, 发送:%s", pInputOrderAction->InvestorID, strOrderSysID, nRtn==0?"成功":"失败");
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);
}

void CTradeSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if(pInputOrderAction && pRspInfo)
	{
		char* pszMsg = new char[1024];
		memset(pszMsg, 0, 1024);
		CString strOrderSysID = pInputOrderAction->OrderSysID;
		strOrderSysID.TrimLeft();
		strOrderSysID.TrimRight();
		if(strOrderSysID.IsEmpty())
		{
			strOrderSysID = "(NULL)";
		}

		sprintf(pszMsg, "□撤单失败, 帐号:%s, 单号:%s, 错误:%s", pInputOrderAction->InvestorID, strOrderSysID, pRspInfo->ErrorMsg);
		::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);
	}
}

//////////////////////////////////////////////////////////////////////////
///错误应答
void CTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TRACE("\n---------->>> OnRspError\n");
}

//////////////////////////////////////////////////////////////////////////
///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
void CTradeSpi::OnFrontDisconnected(int nReason)
{
	g_bReLogin = true;
}

//////////////////////////////////////////////////////////////////////////
///心跳超时警告。当长时间未收到报文时，该方法被调用。
void CTradeSpi::OnHeartBeatWarning(int nTimeLapse)
{

}


//////////////////////////////////////////////////////////////////////////
///成交通知
void CTradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	char* pszMsg = new char[1024];
	memset(pszMsg, 0, 1024);
	CString strOrderSysID = pTrade->OrderSysID;
	strOrderSysID.TrimLeft();
	strOrderSysID.TrimRight();
	sprintf(pszMsg, "★成交回报, 帐号:%s, 合约:%s, 买卖:%c, 开平:%c, 数量:%d, 价格:%.2lf, 套保:%c, 单号:%s", pTrade->InvestorID, pTrade->InstrumentID, pTrade->Direction, pTrade->OffsetFlag, pTrade->Volume, pTrade->Price, pTrade->HedgeFlag, strOrderSysID);
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);

	char szTemp[256] = { 0 };
	strcpy(szTemp, "收到成交回报, OnRtnTrade");
	WLOG(szTemp, FALSE);

	sprintf(szTemp, "OrderRef = %s\n", pTrade->OrderRef);
	WLOG(szTemp, FALSE);

	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "BrokerID=%s\nBrokerOrderSeq=%d\nOrderLocalID=%s\n", pTrade->BrokerID, pTrade->BrokerOrderSeq, pTrade->OrderLocalID);
	WLOG(szTemp, FALSE);


	memset(szTemp, 0, sizeof(szTemp));
	sprintf(szTemp, "ExchangeID=%s\nOrderSysID=%s\nTradeID=%s\n", pTrade->ExchangeID, pTrade->OrderSysID, pTrade->TradeID);
	WLOG(szTemp, TRUE);
}

int CTradeSpi::GetOrderRef()
{
	int nOrderRef = g_iOrderRef;
	g_iOrderRef++;
	return nOrderRef;
}

void CZDQHAPISpi::OnFrontConnected()
{
	char* pMsg = "登录成功" ;
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pMsg);
	if (g_pZDQHApi)
	{
		TZDQHExchangeType exchange[3] ={0};
		strcpy(exchange[0], "CFFEX");
		strcpy(exchange[1], "CFFEX");
		strcpy(exchange[2], "CFFEX");

		char* contract[3] = {0};
		contract[0] = "IF1311";
		contract[1] = "IF1312";
		contract[2] = "IF1403";
		g_pZDQHApi->SubscribeMarketData(exchange,contract,3);
	}
}

void CZDQHAPISpi::OnFrontDisconnected()
{

}


void CZDQHAPISpi::OnRtnDepthMarketData(CZDQHFtdcDepthQuoteDataField *pDepthMarketData)
{
	if (pDepthMarketData)
	{
		try
		{
			if (pDepthMarketData->AskPrice1 > 1000000)
			{
				return;
			}
			SYSTEMTIME t;
			GetLocalTime(&t);


		 LARGE_INTEGER m_liPerfFreq={0};
		 //获取每秒多少CPU Performance Tick 
		 QueryPerformanceFrequency(&m_liPerfFreq); 
 
		 LARGE_INTEGER m_liPerfStart={0};
		 QueryPerformanceCounter(&m_liPerfStart);
	     unsigned long mtime = m_liPerfStart.QuadPart * 1000 / m_liPerfFreq.QuadPart;	

		 char szbuffer[512] ={0};	
		 sprintf(szbuffer, "%02d:%02d:%02d %03d %d  代码：%s  最新价：%.1lf  持仓量:%.0f    成交量:%d  买一价：%10.1lf 买一量:%10d  卖一价：%.1lf  卖一量：%10d",
			t.wHour,t.wMinute,t.wSecond,t.wMilliseconds,mtime,pDepthMarketData->InstrumentID,	pDepthMarketData->LastPrice,pDepthMarketData->OpenInterest,pDepthMarketData->Volume
			,pDepthMarketData->BidPrice1,pDepthMarketData->BidVolume1,pDepthMarketData->AskPrice1,pDepthMarketData->AskVolume1
			);

			WriteLog(szbuffer, false);
			::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)szbuffer);
		}
		catch(...)
		{
		}
		//sprintf(pszMsg, "★成交回报, 帐号:%s, 合约:%s, 买卖:%c, 开平:%c, 数量:%d, 价格:%.2lf, 套保:%c, 单号:%s", pTrade->InvestorID, pTrade->InstrumentID, pTrade->Direction, pTrade->OffsetFlag, pTrade->Volume, pTrade->Price, pTrade->HedgeFlag, strOrderSysID);
	}
}



extern CZDQHFtdcTradeApi* g_pZdQHTradeapi;

void CZDQHTradeAPISpi::OnFrontConnected(TZDQHSourceType srcType)
{
	char* pMsg = "易盛交易连接成功";
	WriteLog(pMsg,false);
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pMsg);
	if (g_pZdQHTradeapi)
	{
		char userID[25]={0};
		char pass[25]={0};

	
		::GetPrivateProfileString("ESUNNY", "user_name", "", userID, sizeof(userID), "./test_config.ini");
		::GetPrivateProfileString("ESUNNY", "password", "", pass, sizeof(pass), "./test_config.ini");

		CZDQHFtdcReqUserLoginField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.UserID, userID);
		strcpy(req.Password, pass);
		strcpy(req.UserProductInfo, "xiuju");

	//	int iret = g_pZdQHTradeapi->ReqUserLogin(&req,0);
	
	}
}

void CZDQHTradeAPISpi::OnFrontDisconnected(TZDQHSourceType srcType)
{
	char* pMsg = "易盛交易连接失败";
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pMsg);
}

void CZDQHTradeAPISpi::OnRspUserLogin(CZDQHFtdcRspUserLoginField *pRspUserLogin, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	
	if (pRspInfo && pRspInfo->ErrorID != 0)
	{
		char szBuffer[256] ={0};
		strcpy(szBuffer, pRspInfo->ErrorMsg);
		WriteLog(szBuffer,false);
		::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)szBuffer);
		return;
	}
	else
	{
		char* pMsg = "易盛交易登录成功";
		::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pMsg);
	}
	char ip[25]={0};
	int port;
	::GetPrivateProfileString("ESUNNY", "hqip", "", ip, sizeof(ip), "./test_config.ini");
	port = ::GetPrivateProfileInt("ESUNNY", "hqport", 0, "./test_config.ini");
	CZDQHAddressField addr;
	memset(&addr,0,sizeof(addr));
	strncpy(addr.Ip, ip, sizeof(addr.Ip));
	addr.Port = port;
//	g_pZDQHApi->Init(&addr,1,'3');

}

