//
// File:        $ TraderSpi.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/05/14 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "TraderSpi.hpp"

namespace simulator
{
    CSimpleTrader::CSimpleTrader(CThostFtdcTraderApi *pUserApi) :
	m_pUserApi(pUserApi) 
    {}

    void CSimpleTrader::addMatchy(CTPMatchyApi * pm)
    {
	m_numContracts++;
	m_contracts.push_back(pm->getContractId());
	m_exchMap[pm->getContractId()] = pm;
    }

    void CSimpleTrader::initializeMeta()
    {
	initializeMetaPriceLimits();
    }


    void CSimpleTrader::getParamFromConfig(INIReader &iniconfig, std::string sectionName)
    {
	std::string broker = iniconfig.Get(sectionName, "Broker", "");
	if(broker == "ZD") {
	    strcpy(g_chBrokerID, "5600");
	    strcpy(g_chUserID, "880015");
	} else if (broker == "GD") {
	    strcpy(g_chBrokerID, "6000");
	    strcpy(g_chUserID, "05810007");
	} else {
	    std::cout << "Unknown Broker" << std::endl;
	    throw;
	}

    }

    void CSimpleTrader::initializeMetaPriceLimits()
    {
	CThostFtdcQryDepthMarketDataField qryDepthMarketData;
	for (auto it = m_exchMap.begin(); it != m_exchMap.end(); ++it) {
	    strcpy(qryDepthMarketData.InstrumentID, (it->second->getContractId()).c_str());
	    //m_pUserApi->ReqQryDepthMarketData(&qryDepthMarketData, 0);
	    cout << "CSimpleTrader::initializeMetaPriceLimits " << m_pUserApi->ReqQryDepthMarketData(&qryDepthMarketData, 0) << endl;
	    sleep(2);
	}
    }

    void CSimpleTrader::OnFrontConnected()
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnFrontConnected - enter" << endl;
#endif
	CThostFtdcReqUserLoginField reqUserLogin;
	memset(&reqUserLogin, 0, sizeof(reqUserLogin));
	
	strcpy(reqUserLogin.BrokerID, g_chBrokerID);
	
	strcpy(reqUserLogin.UserID, g_chUserID);
	
	// get Password
	printf("Password:");
	scanf("%s", reqUserLogin.Password);
	
	m_pUserApi->ReqUserLogin(&reqUserLogin, 0);
#ifdef DEBUG
	cerr << "CSimpleTrader::OnFrontConnected - exit" << endl;
#endif
    }

    void CSimpleTrader::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspAuthenticate - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to authenticate, errorcode=%d, errormsg=%s, requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	}
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspAuthenticate - enter" << endl;
#endif
    }
    
    void CSimpleTrader::OnFrontDisconnected(int nReason)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnFrontDisconnected - enter" << endl;
	cerr << "CSimpleTrader::OnFrontDisconnected - enter" << endl;
#endif
    }

    void CSimpleTrader::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspUserLogin - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to log in, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else if (pRspUserLogin != NULL) {
	    for (int ii = 0; ii < m_numContracts; ++ii) {
		if (pRspUserLogin->BrokerID != NULL) {
		    strcpy(m_exchMap[m_contracts[ii]]->brokerId(), pRspUserLogin->BrokerID);
		}
		if (pRspUserLogin->UserID != NULL) {
		    strcpy(m_exchMap[m_contracts[ii]]->investorId(), pRspUserLogin->UserID);
		}
		if (pRspUserLogin->UserID != NULL) {
		    strcpy(m_exchMap[m_contracts[ii]]->userId(), pRspUserLogin->UserID);
		}
		m_exchMap[m_contracts[ii]]->frontId() = pRspUserLogin->FrontID;
		m_exchMap[m_contracts[ii]]->sessionId() = pRspUserLogin->SessionID;
	    }
#ifdef DEBUG
	    cout << "MaxOrderRef starts with " << pRspUserLogin->MaxOrderRef << endl;
	    cout << "FrontID: " << pRspUserLogin->FrontID << ", SessionID: " << pRspUserLogin->SessionID << endl;
#endif
	    /* Initialize strategy class orderId. */
	    int num_strat = m_exchMap[m_contracts[0]]->getSimulatorCorePtr()->getNumStrategies();
	    // int orderId_start_global = (atoi(pRspUserLogin->MaxOrderRef) / num_strat + 1) * num_strat;
	    // for (auto & pStrategy : m_exchMap[m_contracts[0]]->getSimulatorCorePtr()->getStrategies())
	    // 	pStrategy->initOrderId(orderId_start_global);
	    orderId_ = (atoi(pRspUserLogin->MaxOrderRef) / num_strat + 1) * num_strat;

	    /* Confirm settlement info. */
	    CThostFtdcQrySettlementInfoField settlementInfo;
	    memset(&settlementInfo, 0, sizeof(settlementInfo));
#ifdef DEBUG
	    cout << "GetTradingDay() = " << m_pUserApi->GetTradingDay() << endl;
#endif
	    strcpy(settlementInfo.BrokerID, g_chBrokerID);
	    strcpy(settlementInfo.InvestorID, g_chUserID);
	    strcpy(settlementInfo.TradingDay, m_pUserApi->GetTradingDay());
	    
	    m_pUserApi->ReqQrySettlementInfo(&settlementInfo, 0);
	} else {
	    cout << "Failed to log in: null pointer pRspUserLogin" << endl;
	    exit(-1);
	}
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspUserLogin - exit" << endl;
#endif
    }

    void CSimpleTrader::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspQrySettlementInfo - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to query settlement info, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else if (bIsLast) {
	    int isconfirm;
	    cout << "Is Settlement Info Correct? (1 - yes; 0 - no)" << endl;
	    cin >> isconfirm;
	    if (isconfirm) {
		/* Confirm settlement info. */
		CThostFtdcSettlementInfoConfirmField settlementInfoConfirm;
		memset(&settlementInfoConfirm, 0, sizeof(settlementInfoConfirm));
		
		strcpy(settlementInfoConfirm.BrokerID, g_chBrokerID);
		strcpy(settlementInfoConfirm.InvestorID, g_chUserID);
		
		m_pUserApi->ReqSettlementInfoConfirm(&settlementInfoConfirm, 0);
	    } else {
		cout << "Call Broker!!!!!!!!!!" << endl;
		cout << "Exiting... " << endl;
		exit(-1);
	    }
	}
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspQrySettlementInfo - exit" << endl;
#endif
    }

    void CSimpleTrader::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspSettlementInfoConfirm - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to confirm settlement info, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else {
	    isReady=true;

	    int action = -1;
	    while (action < 0 || action > 3 || action == 1) {
		cout << "What should we do next? \n";
		cout << "0 - exit\n";
		cout << "1 - live simulation (not implemented yet)\n";
		cout << "2 - debug (print messages to exchange but not actually submitting orders)\n";
		cout << "3 - let's trade and become billionaires!!!!\n" << endl;;
		cin >> action;
		if (action == 0) {
		    cout << "exiting..." << endl;
		    exit(-1);
		} else if (action== 1) {
		    cout << "not implemented yet, will repeate menu" << endl;
		    //exit(-1);
		} else if (action== 2) {
		    // notify all matchy that we do debug
		    // set isready to be true for all registered matchy
		    for (int ii = 0; ii<m_numContracts; ii++) {
			m_exchMap[m_contracts[ii]]->traderReadiness() = true;
			m_exchMap[m_contracts[ii]]->actionFlag() = 2;
		    }
		} else if (action== 3) {
		    cout << "yeah!!! Can you confirm who you are again?" << endl;
		    string who;
		    cin >> who;
		    if (who == "mingyuan" || who == "fei") {
			cout << "OK, master " << who << ", I will start trading right away." << endl;
			// notify all mathcy that we trade
			// set is ready to be true for all registered matchy
			for (int ii = 0; ii<m_numContracts; ii++) {
			    m_exchMap[m_contracts[ii]]->traderReadiness() = true;
			    m_exchMap[m_contracts[ii]]->actionFlag() = 3;
			}
			cout << "Becasue you compile with DEBUG option, you will see massive amount of messages!" << endl;
		    } else {
			cout << "wait.. who the hell are you!" << endl;
			action = -1;
		    }
		}
	    }
	}
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspSettlementInfoConfirm - exit" << endl;
#endif
    }

    void CSimpleTrader::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspQryDepthMarketData - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to query depth market data, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else if (pDepthMarketData != NULL) {
	    auto it = find_if(m_exchMap.begin(), 
			      m_exchMap.end(), 
			      [&] (decltype(*m_exchMap.begin()) p) { return (p.second->getExchangeId() == pDepthMarketData->ExchangeID) && (p.second->getContractId() == pDepthMarketData->InstrumentID); });
	    if (it != m_exchMap.end()) {
		it->second->upperLimitPrice() = pDepthMarketData->UpperLimitPrice;
		it->second->lowerLimitPrice() = pDepthMarketData->LowerLimitPrice;
#ifdef DEBUG
		cout << it->second->getExchangeId() << "'s " << it->second->getContractId() 
			  << "'s " << "lower and upper limit prices are " 
			  << it->second->lowerLimitPrice() << " and " 
			  << it->second->upperLimitPrice() << endl;
#endif
	    }
	} else {
	    cout << "Failed to query depth market data: null pointer pDepthMarketData" << endl;
	    exit(-1);
	}
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspQryDepthMarketData - exit" << endl;
#endif
    }
    
    void CSimpleTrader::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspError - enter" << endl;
#endif
	// TODO: make this into a logger file instead of writing on screen
	cerr << "OnRspError:" << endl;
	cerr << "ErrorCode=[" << pRspInfo->ErrorID << "], "
		  << "ErrorMsg=[" << pRspInfo->ErrorMsg << "]" << endl;
	cerr << "RequestID=[" << nRequestID << "], "
		  << "Chain=[" << bIsLast << endl;
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspError - exit" << endl;
#endif
    }

    void CSimpleTrader::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspUserLogout - enter" << endl;
	cerr << "CSimpleTrader::OnRspUserLogout - exit" << endl;
#endif
    }

    void CSimpleTrader::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspOrderInsert - enter " << endl;
#endif
	// send reject
	if (pInputOrder != NULL) {
	    string contract(pInputOrder->InstrumentID);
	    m_exchMap[contract]->push(pInputOrder);
	}
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	}
#ifdef DEBUG
	if (pInputOrder != NULL)
	    cerr << convertStructToString(pInputOrder, "\n") << endl;
	cerr << "CSimpleTrader::OnRspOrderInsert - exit " << endl;
#endif
    }
    
    void CSimpleTrader::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRspOrderAction - enter" << endl;
#endif
	// send reject
	if (pInputOrderAction != NULL) {
	    string contract(pInputOrderAction->InstrumentID);
	    m_exchMap[contract]->push(pInputOrderAction);
	}
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	}	    
#ifdef DEBUG
	if (pInputOrderAction != NULL) 
	    cerr << convertStructToString(pInputOrderAction, "\n") << endl;
	cerr << "CSimpleTrader::OnRspOrderAction - exit" << endl;
#endif
    }	

    void CSimpleTrader::OnRtnOrder(CThostFtdcOrderField *pOrder)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRtnOrder - enter" << endl;
#endif
	if (pOrder != NULL) {
	    string contract(pOrder->InstrumentID);
	    if (m_exchMap.find(contract) != m_exchMap.end()) {
		m_exchMap[contract]->push(pOrder);
	    }
	}
#ifdef DEBUG
	if (pOrder != NULL) 
	    cerr << convertStructToString(pOrder, "\n") << endl;
	cerr << "CSimpleTrader::OnRtnOrder - exit" << endl;
#endif
    }
    
    void CSimpleTrader::OnRtnTrade(CThostFtdcTradeField *pTrade)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnRtnTrade - enter" << endl;
#endif
	if (pTrade != NULL) {
	    string contract(pTrade->InstrumentID);
	    if (m_exchMap.find(contract) != m_exchMap.end()) {
		m_exchMap[contract]->push(pTrade);
	    }
	}
#ifdef DEBUG
	if (pTrade != NULL) 
	    cerr << convertStructToString(pTrade, "\n") << endl;
	cerr << "CSimpleTrader::OnRtnTrade - exit" << endl;
#endif
    }

    void CSimpleTrader::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnErrRtnOrderInsert - enter" << endl;
#endif
	// send reject
	if (pInputOrder != NULL) {
	    string contract(pInputOrder->InstrumentID);
	    m_exchMap[contract]->push(pInputOrder);
	}
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, 0, true);
	}
#ifdef DEBUG
	if (pInputOrder != NULL) 
	    cerr << convertStructToString(pInputOrder, "\n") << endl;
	cerr << "CSimpleTrader::OnErrRtnOrderInsert - exit" << endl;
#endif
    }

    void CSimpleTrader::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
    {
#ifdef DEBUG
	cerr << "CSimpleTrader::OnErrRtnOrderAction - enter" << endl;
#endif
	// send reject
	if (pOrderAction != NULL) {
	    string contract(pOrderAction->InstrumentID);
	    m_exchMap[contract]->push(pOrderAction);
	}
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, 2, true);
	}
#ifdef DEBUG
	if (pOrderAction != NULL) 
	    cerr << convertStructToString(pOrderAction, "\n") << endl;
	cerr << "CSimpleTrader::OnErrRtnOrderAction - exit" << endl;
#endif
    }

}

// TRADERSPI_CPP
