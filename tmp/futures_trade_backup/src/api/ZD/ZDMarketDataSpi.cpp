//
// File:        $ ZDMarketDataSpi.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/25 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#include "ZDMarketDataSpi.hpp"

using std::memset;
using std::memcpy;
using std::strcpy;

namespace simulator 
{
	
    CSimpleHandlerZD::CSimpleHandlerZD(CZDQHFtdcMdApi *pUserApi, int numDataToWarmUp) : 
	m_pUserApi(pUserApi),
	numDataToWarmUp_(numDataToWarmUp)
    {
    }

    CSimpleHandlerZD::~CSimpleHandlerZD()
    {}
	
    void CSimpleHandlerZD::addMatchy(CTPMatchyApi * mc)
    {
	m_numContracts++;
	m_contracts.push_back(mc->getContractId());
	m_exchanges.push_back(mc->getExchangeId());
	m_exchMap[mc->getContractId()] = mc;
    }

    void CSimpleHandlerZD::getParamFromConfig(INIReader &iniconfig, std::string sectionName)
    {
	std::string broker = iniconfig.Get(sectionName, "Broker", "");
	// if (broker == "ZD") {
	//     strcpy(g_chBrokerID, "5600");
	//     strcpy(g_chUserID, "880015");
	//     password_ = "123456";
	// } else if (broker == "GD") {
	//     strcpy(g_chBrokerID, "6000");
	//     strcpy(g_chUserID, "05810007");
	//     password_ = "830817";
	// } else {
	//     std::cout << "Unknown Broker" << std::endl;
	//     throw;
	// }

    }
		

    void CSimpleHandlerZD::OnRspUnSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnRspUnSubMarketData - enter" << endl;
#endif
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnRspUnSubMarketData - exit" << endl;
#endif

    }

    void CSimpleHandlerZD::OnFrontConnected()
    {
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnFrontConnected - enter" << endl;
#endif
	// init m_pInstrument;
	char * m_pInstrument[m_contracts.size()];
	TZDQHExchangeType m_pExchange[m_contracts.size()];
	for (unsigned i = 0; i < m_contracts.size(); ++i) {
	    m_pInstrument[i] = const_cast<char *>(m_contracts[i].c_str());
	    strcpy(m_pExchange[i], m_exchanges[i].c_str());
	}
	
	m_pUserApi->SubscribeMarketData(m_pExchange, m_pInstrument, m_numContracts);
	isReady = true;
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnFrontConnected - exit" << endl;
#endif
    }
		
    void CSimpleHandlerZD::OnFrontDisconnected()
    {
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnFrontDisconnected - enter" << endl;
#endif
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnFrontDisconnected - exit" << endl;
#endif
    }
	
		
    void CSimpleHandlerZD::OnRspSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnRspSubMarketData - enter" << endl;
#endif
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    printf("Failed to subscribe market data, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else {
	    cout << "Subscribed " << pSpecificInstrument->InstrumentID << endl;
	    // set the particular matchy to be ready for md
	    string contract(pSpecificInstrument->InstrumentID);
	    m_exchMap[contract]->marketDataReadiness() = true;
	}
#ifdef DEBUG
	cerr << "CSimpleHandlerZD::OnRspSubMarketData - exit" << endl;
#endif
    }
	
    void CSimpleHandlerZD::OnRtnDepthMarketData(CZDQHFtdcDepthQuoteDataField *pDepthMarketData)
    {
#ifdef PRODUCTION
	// Set time offset here (only set ONCE for THE SimulatorCore).
	if (!setTimeOffsetQ_ && m_exchMap[pDepthMarketData->InstrumentID]->numData() > numDataToWarmUp_) {
	    m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->timeOffset() = duration_nano(m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->getCurrTime().time_since_epoch().count() - convertStringToTimePoint(combineToTimestampString(pDepthMarketData->TradingDay, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec)).time_since_epoch().count());
#ifdef DEBUG
	    cout << "CSimpleHandlerZD::OnRtnDepthMarketData - time offset is " << (m_exchMap[pDepthMarketData->InstrumentID]->getSimulatorCorePtr()->timeOffset()).count() << "nano seconds" << endl;
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
	convertToCTPMsg(pDepthMarketData);
	m_exchMap[contract]->push(&m_ctpDataMsgCache);
	++(m_exchMap[contract]->numData());
    }
	

    void CSimpleHandlerZD::convertToCTPMsg(CZDQHFtdcDepthQuoteDataField *pDepthMarketData)
    {
	strcpy(m_ctpDataMsgCache.TradingDay, pDepthMarketData->TradingDay);
	strcpy(m_ctpDataMsgCache.InstrumentID, pDepthMarketData->InstrumentID);
	strcpy(m_ctpDataMsgCache.ExchangeID, pDepthMarketData->ExchangeID);
	strcpy(m_ctpDataMsgCache.ExchangeInstID, pDepthMarketData->ExchangeInstID);
	m_ctpDataMsgCache.LastPrice = pDepthMarketData->LastPrice;
	m_ctpDataMsgCache.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
	m_ctpDataMsgCache.PreClosePrice = pDepthMarketData->PreClosePrice;
	m_ctpDataMsgCache.PreOpenInterest = pDepthMarketData->PreOpenInterest;
	m_ctpDataMsgCache.OpenPrice = pDepthMarketData->OpenPrice;
	m_ctpDataMsgCache.HighestPrice = pDepthMarketData->HighestPrice;
	m_ctpDataMsgCache.LowestPrice = pDepthMarketData->LowestPrice;
	m_ctpDataMsgCache.Volume = pDepthMarketData->Volume;
	m_ctpDataMsgCache.Turnover = pDepthMarketData->Turnover;
	m_ctpDataMsgCache.OpenInterest = pDepthMarketData->OpenInterest;
	m_ctpDataMsgCache.ClosePrice = pDepthMarketData->ClosePrice;
	m_ctpDataMsgCache.SettlementPrice = pDepthMarketData->SettlementPrice;
	m_ctpDataMsgCache.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
	m_ctpDataMsgCache.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
	m_ctpDataMsgCache.PreDelta  = pDepthMarketData->PreDelta;
	m_ctpDataMsgCache.CurrDelta = pDepthMarketData->CurrDelta;
	strcpy(m_ctpDataMsgCache.UpdateTime, pDepthMarketData->UpdateTime);
	m_ctpDataMsgCache.UpdateMillisec  = pDepthMarketData->UpdateMillisec;
	m_ctpDataMsgCache.BidPrice1  = pDepthMarketData->BidPrice1;
	m_ctpDataMsgCache.BidVolume1  = pDepthMarketData->BidVolume1;
	m_ctpDataMsgCache.AskPrice1  = pDepthMarketData->AskPrice1;
	m_ctpDataMsgCache.AskVolume1 = pDepthMarketData->AskVolume1;
	m_ctpDataMsgCache.BidPrice2 = pDepthMarketData->BidPrice2;
	m_ctpDataMsgCache.BidVolume2  = pDepthMarketData->BidVolume2;
	m_ctpDataMsgCache.AskPrice2 = pDepthMarketData->AskPrice2;
	m_ctpDataMsgCache.AskVolume2 = pDepthMarketData->AskVolume2;
	m_ctpDataMsgCache.BidPrice3 = pDepthMarketData->BidPrice3;
	m_ctpDataMsgCache.BidVolume3 = pDepthMarketData->BidVolume3;
	m_ctpDataMsgCache.AskPrice3 = pDepthMarketData->AskPrice3;
	m_ctpDataMsgCache.AskVolume3 = pDepthMarketData->AskVolume3;
	m_ctpDataMsgCache.BidPrice4 = pDepthMarketData->BidPrice4;
	m_ctpDataMsgCache.BidVolume4 = pDepthMarketData->BidVolume4;
	m_ctpDataMsgCache.AskPrice4 = pDepthMarketData->AskPrice4;
	m_ctpDataMsgCache.AskVolume4 = pDepthMarketData->AskVolume4;
	m_ctpDataMsgCache.BidPrice5 = pDepthMarketData->BidPrice5;
	m_ctpDataMsgCache.BidVolume5 = pDepthMarketData->BidVolume5;
	m_ctpDataMsgCache.AskPrice5 = pDepthMarketData->AskPrice5;
	m_ctpDataMsgCache.AskVolume5 = pDepthMarketData->AskVolume5;
	m_ctpDataMsgCache.AveragePrice = pDepthMarketData->AveragePrice;
    }

	
}

// ZDMARKETDATASPI_CPP
