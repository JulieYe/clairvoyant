// TradeThread.cpp: implementation of the CTradeThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestTrade.h"
#include "TradeThread.h"
#include "TradeSpi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CTradeThread::CTradeThread()
{
}

CTradeThread::~CTradeThread()
{
	DeleteThread();
}

void CTradeThread::Release()
{
	delete this;
}

//////////////////////////////////////////////////////////////////////////
BOOL CreateFolder(const CString& strFolder)
{	
	if(strFolder.IsEmpty())
	{
		return FALSE;
	}
	
	BOOL bRtn = FALSE;
	DWORD dwFileAttrib = ::GetFileAttributes(strFolder);
	if(-1 == dwFileAttrib)
	{
		bRtn = ::CreateDirectory(strFolder, NULL);
	}
	else if(FILE_ATTRIBUTE_DIRECTORY == dwFileAttrib)
	{
		bRtn = TRUE;
	}
	return bRtn;
}
//////////////////////////////////////////////////////////////////////////

BOOL CTradeThread::RunThread(LPVOID pParam)
{
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
	g_pUserSpi = new CTradeSpi();

	CString strUserID = "353911";
	CreateFolder(strUserID);
	strUserID += "\\";
	
	g_pUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi(strUserID);
	g_pUserApi->RegisterSpi((CThostFtdcTraderSpi*)g_pUserSpi);		
	g_pUserApi->SubscribePublicTopic(THOST_TERT_RESUME);			
	g_pUserApi->SubscribePrivateTopic(THOST_TERT_RESUME);				
	g_pUserApi->RegisterFront(FRONT_ADDR);
	g_pUserApi->Init();
	//g_pUserApi->Join();

	return CBaseThread::RunThread(pParam);
}

void CTradeThread::DeleteThread()
{
}
