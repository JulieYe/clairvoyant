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
// �Ƿ��յ��ɹ�����Ӧ
bool CTradeSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));	

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
// �Ƿ��ҵı����ر�
bool CTradeSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
			(pOrder->SessionID == SESSION_ID) &&
			(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

//////////////////////////////////////////////////////////////////////////
// �Ƿ����ڽ��׵ı���
bool CTradeSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

//////////////////////////////////////////////////////////////////////////
// ���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
void CTradeSpi::OnFrontConnected()
{
	WLOG("���ӽ��׷������ɹ�", TRUE);
	//if(g_bReLogin)
	{
		g_bReLogin = false;
		ReqUserLogin(g_szInvestorID, g_szPassword);
	}
}

//////////////////////////////////////////////////////////////////////////
// ��¼
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
//	sprintf(szLog, "���͵�¼����, nResult=%d", nResult);
//	WLOG(szLog, TRUE);
}

void CTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szLog[256] = { 0 };
	sprintf(szLog, "%s", "�յ���¼��Ӧ");
	WLOG(szLog, FALSE);

	if(pRspInfo != NULL)
	{
		memset(szLog, 0, sizeof(szLog));
		sprintf(szLog, "nErrorID=%d, szErrorMsg=%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
	else
	{
		strcpy(szLog, "pRspInfoΪNULL");	
	}
	WLOG(szLog, FALSE);

	if(pRspInfo != NULL && pRspInfo->ErrorID != 0)
	{
		CString strMsg;
		strMsg.Format("��¼ʧ�ܣ�\r\n%s", pRspInfo->ErrorMsg);
		::MessageBox(NULL, strMsg, "��ʾ��Ϣ", MB_OK|MB_ICONINFORMATION);
	}

	if(bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// ����Ự����
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
	TRACE("\n---------->>> ���͵ǳ�����, nResult=%d\n", iResult);
}

void CTradeSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TRACE("\n---------->>> �ǳ���Ӧ: ");
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
// Ͷ���߽�����ȷ��
void CTradeSpi::ReqSettlementInfoConfirm(TThostFtdcInvestorIDType szInvestorID)
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, szInvestorID);
	int nResult = g_pUserApi->ReqSettlementInfoConfirm(&req, ++g_iRequestID);

	char szLog[256] = { 0 };
	sprintf(szLog, "����Ͷ���߽�����ȷ��, nResult=%d", nResult);
	WLOG(szLog, TRUE);
}

void CTradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	char szLog[256] = { 0 };
	sprintf(szLog, "%s", "�յ�Ͷ���߽�����ȷ����Ӧ");
	WLOG(szLog, FALSE);

	if(bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		memset(szLog, 0, sizeof(szLog));
		sprintf(szLog, "nErrorID = %d, szErrorMsg = %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		WLOG(szLog, TRUE);
		::MessageBox(NULL, "  ��¼�ɹ���     ", "��ʾ��Ϣ", MB_OK|MB_ICONINFORMATION);
		//CTestTradeDlg::s_pThis->m_btnLogin.EnableWindow(FALSE);
	}
	else
	{
		WriteSeparator();
	}
}


//////////////////////////////////////////////////////////////////////////
///����¼��������Ӧ
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

	sprintf(pszMsg, "���µ�����, �ʺ�:%s, ��Լ:%s, ����:%c, ��ƽ:%c, ����:%d, �۸�:%.2lf, �ױ�:%c, ����:%s", szInvestorID, szInstrumentID, dwDirection, dwOffsetFlag, nVolume, dPrice, req.CombHedgeFlag[0], nResult==0?"�ɹ�":"ʧ��");
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);

	char szTemp[256] = { 0 };
	sprintf(szTemp, "�����µ�����: nResult=%d", nResult);
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
		sprintf(pszMsg, "���µ�����, �ʺ�:%s, ��Լ:%s, ����:%c, ��ƽ:%c, ����:%d, �۸�:%.2lf, �ױ�:%c, ����:%s", pInputOrder->InvestorID, pInputOrder->InstrumentID, pInputOrder->Direction, pInputOrder->CombOffsetFlag[0], pInputOrder->VolumeTotalOriginal, pInputOrder->LimitPrice, pInputOrder->CombHedgeFlag[0], pRspInfo->ErrorMsg);
		::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);
	}

	char szTemp[256] = { 0 };
	strcpy(szTemp, "�µ��ر�, OnRspOrderInsert");
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
	sprintf(pszMsg, "��ί�лر�, �ʺ�:%s, ��Լ:%s, ����:%c, ��ƽ:%c, ����:%d, �۸�:%.2lf, �ױ�:%c, ����:%s, ״̬:%s", pOrder->InvestorID, pOrder->InstrumentID, pOrder->Direction, pOrder->CombOffsetFlag[0], pOrder->VolumeTotalOriginal, pOrder->LimitPrice, pOrder->CombHedgeFlag[0], strOrderSysID, pOrder->StatusMsg);
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);

	char szTemp[256] = { 0 };
	strcpy(szTemp, "�յ��µ��ر�, OnRtnOrder");
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
///��������������Ӧ
void CTradeSpi::ReqOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction)
{
	int nRtn = g_pUserApi->ReqOrderAction(pInputOrderAction, ++g_iRequestID);
	char* pszMsg = new char[1024];
	memset(pszMsg, 0, 1024);

	CString strOrderSysID = pInputOrderAction->OrderSysID;
	strOrderSysID.TrimLeft();
	strOrderSysID.TrimRight();
	sprintf(pszMsg, "����������, �ʺ�:%s, ����:%s, ����:%s", pInputOrderAction->InvestorID, strOrderSysID, nRtn==0?"�ɹ�":"ʧ��");
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

		sprintf(pszMsg, "������ʧ��, �ʺ�:%s, ����:%s, ����:%s", pInputOrderAction->InvestorID, strOrderSysID, pRspInfo->ErrorMsg);
		::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);
	}
}

//////////////////////////////////////////////////////////////////////////
///����Ӧ��
void CTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TRACE("\n---------->>> OnRspError\n");
}

//////////////////////////////////////////////////////////////////////////
///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
void CTradeSpi::OnFrontDisconnected(int nReason)
{
	g_bReLogin = true;
}

//////////////////////////////////////////////////////////////////////////
///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
void CTradeSpi::OnHeartBeatWarning(int nTimeLapse)
{

}


//////////////////////////////////////////////////////////////////////////
///�ɽ�֪ͨ
void CTradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	char* pszMsg = new char[1024];
	memset(pszMsg, 0, 1024);
	CString strOrderSysID = pTrade->OrderSysID;
	strOrderSysID.TrimLeft();
	strOrderSysID.TrimRight();
	sprintf(pszMsg, "��ɽ��ر�, �ʺ�:%s, ��Լ:%s, ����:%c, ��ƽ:%c, ����:%d, �۸�:%.2lf, �ױ�:%c, ����:%s", pTrade->InvestorID, pTrade->InstrumentID, pTrade->Direction, pTrade->OffsetFlag, pTrade->Volume, pTrade->Price, pTrade->HedgeFlag, strOrderSysID);
	::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)pszMsg);

	char szTemp[256] = { 0 };
	strcpy(szTemp, "�յ��ɽ��ر�, OnRtnTrade");
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
	char* pMsg = "��¼�ɹ�" ;
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
		 //��ȡÿ�����CPU Performance Tick 
		 QueryPerformanceFrequency(&m_liPerfFreq); 
 
		 LARGE_INTEGER m_liPerfStart={0};
		 QueryPerformanceCounter(&m_liPerfStart);
	     unsigned long mtime = m_liPerfStart.QuadPart * 1000 / m_liPerfFreq.QuadPart;	

		 char szbuffer[512] ={0};	
		 sprintf(szbuffer, "%02d:%02d:%02d %03d %d  ���룺%s  ���¼ۣ�%.1lf  �ֲ���:%.0f    �ɽ���:%d  ��һ�ۣ�%10.1lf ��һ��:%10d  ��һ�ۣ�%.1lf  ��һ����%10d",
			t.wHour,t.wMinute,t.wSecond,t.wMilliseconds,mtime,pDepthMarketData->InstrumentID,	pDepthMarketData->LastPrice,pDepthMarketData->OpenInterest,pDepthMarketData->Volume
			,pDepthMarketData->BidPrice1,pDepthMarketData->BidVolume1,pDepthMarketData->AskPrice1,pDepthMarketData->AskVolume1
			);

			WriteLog(szbuffer, false);
			::PostMessage(CTestTradeDlg::s_pThis->GetSafeHwnd(), UM_ADD_MSG, NULL, (LPARAM)szbuffer);
		}
		catch(...)
		{
		}
		//sprintf(pszMsg, "��ɽ��ر�, �ʺ�:%s, ��Լ:%s, ����:%c, ��ƽ:%c, ����:%d, �۸�:%.2lf, �ױ�:%c, ����:%s", pTrade->InvestorID, pTrade->InstrumentID, pTrade->Direction, pTrade->OffsetFlag, pTrade->Volume, pTrade->Price, pTrade->HedgeFlag, strOrderSysID);
	}
}



extern CZDQHFtdcTradeApi* g_pZdQHTradeapi;

void CZDQHTradeAPISpi::OnFrontConnected(TZDQHSourceType srcType)
{
	char* pMsg = "��ʢ�������ӳɹ�";
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
	char* pMsg = "��ʢ��������ʧ��";
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
		char* pMsg = "��ʢ���׵�¼�ɹ�";
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

