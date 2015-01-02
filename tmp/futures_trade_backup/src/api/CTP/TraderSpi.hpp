//
// File:        $ TraderSpi.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/14 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef TRADERSPI_HPP
#define TRADERSPI_HPP

#include <iostream>
#include <unistd.h>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::vector;
using std::find_if;
using std::atoi;

#include "ThostFtdcTraderApi.h"
#include "ThostStructToString.hpp"

#include "CFFEX.hpp"
#include "INIReader.hpp"

namespace simulator
{
    class CTPMatchyApi;
}

using simulator::convertStructToString;
using simulator::CTPMatchyApi;

namespace simulator
{

    class CSimpleTrader : public CThostFtdcTraderSpi
    {
    private:
	CThostFtdcTraderApi * m_pUserApi;

	map<string, CTPMatchyApi *> m_exchMap;
	vector<string> m_contracts;
	int m_numContracts = 0;
	int orderId_ = 0;

	bool isReady = false;
	
	TThostFtdcBrokerIDType g_chBrokerID;
	TThostFtdcInvestorIDType g_chUserID;
	TThostFtdcProductInfoType  g_chUserProductInfo;
	TThostFtdcAuthCodeType g_chAuthCode;
	
    public:
	CSimpleTrader(CThostFtdcTraderApi *pUserApi) ;
	
	~CSimpleTrader()
	{}

	void initializeMeta();
	void initializeMetaPriceLimits();
	
	void addMatchy(CTPMatchyApi *);
	void getParamFromConfig(INIReader &iniconfig, std::string sectionName);

	inline int & orderId() { return orderId_; }

	virtual void OnFrontConnected();
	
	virtual void OnFrontDisconnected(int nReason);
	
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

	virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
    };

}

#endif  // TRADERSPI_HPP
