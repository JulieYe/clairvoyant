//
// File:        $ MarketDataSpi.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/13 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "MarketDataSpi.hpp"

namespace simulator 
{
	
    CSimpleHandler::CSimpleHandler(CThostFtdcMdApi *pUserApi, int numDataToWarmUp) : 
	m_pUserApi(pUserApi),
	numDataToWarmUp_(numDataToWarmUp)
    {}

    CSimpleHandler::~CSimpleHandler()
    {}
	
    void CSimpleHandler::addMatchy(CTPMatchyApi * mc)
    {
	m_numContracts++;
	m_contracts.push_back(mc->getContractId());
	m_exchMap[mc->getContractId()] = mc;
    }
		
    void CSimpleHandler::OnFrontConnected()
    {
#ifdef DEBUG
	cerr << "CSimpleHandler::OnFrontConnected - enter" << endl;
#endif
	CThostFtdcReqUserLoginField reqUserLogin;
	//int scanfRet = 0;
		
	// get BrokerID
	//printf("BrokerID:");
	//scanfRet = scanf("%s", g_chBrokerID);
#ifdef BROKER_ZD
	strcpy(g_chBrokerID, "5600");
#elif defined BROKER_GD
	strcpy(g_chBrokerID, "6000");
#elif defined BROKER_MEY1
	strcpy(g_chBrokerID, "7030");
#elif defined BROKER_HT
	strcpy(g_chBrokerID, "3010");
#elif defined BROKER_MEY3
	strcpy(g_chBrokerID, "7030");
#endif
	strcpy(reqUserLogin.BrokerID, g_chBrokerID);
	//strcpy(reqUserLogin.BrokerID, g_chBrokerID);
		
	// get UserID
	//printf("UserID:");
	//scanfRet = scanf("%s", g_chUserID);
#ifdef BROKER_ZD
	strcpy(g_chUserID, "880015");
#elif defined BROKER_GD
	strcpy(g_chUserID, "05810007");
#elif defined BROKER_MEY1
	strcpy(g_chUserID, "11800182");
	//strcpy(g_chUserID, "11800330");
#elif defined BROKER_HT
	strcpy(g_chUserID, "05718");
#elif defined BROKER_MEY3
	strcpy(g_chUserID, "11800395");
#endif
	strcpy(reqUserLogin.UserID, g_chUserID);
		
	// get Password
	//printf("Password:");
#ifdef BROKER_ZD
	strcpy(reqUserLogin.Password, "123456");
#elif defined BROKER_GD
	strcpy(reqUserLogin.Password, "830817");
#elif defined BROKER_MEY1
	strcpy(reqUserLogin.Password, "666888");
#elif defined BROKER_HT
	strcpy(reqUserLogin.Password, "666888");
#elif defined BROKER_MEY3
	strcpy(reqUserLogin.Password, "721007");
#endif
	//scanfRet = scanf("%s", reqUserLogin.Password);
		
	m_pUserApi->ReqUserLogin(&reqUserLogin, 0);
#ifdef DEBUG
	cerr << "CSimpleHandler::OnFrontConnected - exit" << endl;
#endif
    }
		
    void CSimpleHandler::OnFrontDisconnected(int nReason)
    {
#ifdef DEBUG
	cerr << "CSimpleHandler::OnFrontDisconnected - enter" << endl;
#endif
#ifdef DEBUG
	cerr << "CSimpleHandler::OnFrontDisconnected - exit" << endl;
#endif
    }
	
    void CSimpleHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleHandler::OnRspUserLogin - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to login, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else {
	    // init m_pInstrument;
	    char * m_pInstrument[m_contracts.size()];
	    for (unsigned i = 0; i < m_contracts.size(); ++i)
		m_pInstrument[i] = const_cast<char *>(m_contracts[i].c_str());

	    m_pUserApi->SubscribeMarketData(m_pInstrument, m_numContracts);
	    isReady = true;
	}
#ifdef DEBUG
	cerr << "CSimpleHandler::OnRspUserLogin - exit" << endl;
#endif
    }
		
    void CSimpleHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleHandler::OnRspSubMarketData - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to subscribe market data, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else {
	    cout << "Subscribed " << pSpecificInstrument->InstrumentID << endl;
	    // set the particular matchy to be ready for md
	    string contract(pSpecificInstrument->InstrumentID);
	    m_exchMap[contract]->marketDataReadiness() = true;
	}
#ifdef DEBUG
	cerr << "CSimpleHandler::OnRspSubMarketData - exit" << endl;
#endif
    }
	
    void CSimpleHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
    {
#ifdef PRODUCTION
	// Set time offset here (only set ONCE for THE SimulatorCore).
	if (!setTimeOffsetQ_ && m_exchMap[pDepthMarketData->InstrumentID]->numData() > numDataToWarmUp_) {
	    m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->timeOffset() = duration_nano(m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->getCurrTime().time_since_epoch().count() - convertStringToTimePoint(combineToTimestampString(pDepthMarketData->TradingDay, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec)).time_since_epoch().count());
#ifdef DEBUG
	    cout << "CSimpleHandler::OnRtnDepthMarketData - time offset is " << (m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->timeOffset()).count() << "nano seconds" << endl;
#endif
	    m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->timeOffsetQ() = true;
	    // Trigger TraderSpi.initializeMeta() method here.
	    // for (auto it = m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->getExchanges().begin();
	    // 	 it != m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->getExchanges().end();
	    // 	 ++it) {
	    for (auto & it : m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->getExchanges()) {
		auto pCTPMatchyApi = dynamic_cast<CTPMatchyApi *>(&(*it));
		if (pCTPMatchyApi != NULL) {
		    auto pTraderSpi = dynamic_cast<CSimpleTrader *>(pCTPMatchyApi->traderSpi());
		    if (pTraderSpi != NULL) {
			pTraderSpi->initializeMeta();
			//break;
		    }
		}
	    }
	    setTimeOffsetQ_ = true;
	}
#endif
	string contract(pDepthMarketData->InstrumentID);
	strcpy(pDepthMarketData->ExchangeID, m_exchMap[contract]->getExchangeId().c_str());
	m_exchMap[contract]->push(pDepthMarketData);
	++(m_exchMap[contract]->numData());
    }
	
    void CSimpleHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleHandler::OnRspError - enter" << endl;
#endif
	// TODO: make this into a logger file instead of writing on screen
	cerr << "OnRspError:" << endl;
	cerr << "ErrorCode=[" << pRspInfo->ErrorID << "], "
		  << "ErrorMsg=[" << pRspInfo->ErrorMsg << "]" << endl;
	cerr << "RequestID=[" << nRequestID << "], "
		  << "Chain=[" << bIsLast << endl;
#ifdef DEBUG
	cerr << "CSimpleHandler::OnRspError - exit" << endl;
#endif
    }
	
}

// MARKETDATASPI_CPP
