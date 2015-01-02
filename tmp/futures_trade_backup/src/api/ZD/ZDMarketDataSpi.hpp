//
// File:        $ ZDMarketDataSpi.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/25 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef ZDMARKETDATASPI_HPP
#define ZDMARKETDATASPI_HPP

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::map;
using std::vector;

//#include "ThostFtdcMdApi.h"
#include "ZDQHMdApi.h"
#include "ZDQHUserApiStruct.h"
#include "ThostFtdcUserApiStruct.h"

#include "CFFEX.hpp"
#include "Message.hpp"
#include "Converter.hpp"
#include "TraderSpi.hpp"
#include "INIReader.hpp"

using simulator::convertStringToTimePoint;
using simulator::combineToTimestampString;
using simulator::CSimpleTrader;

namespace simulator 
{
	
    class CSimpleHandlerZD : public CZDQHFtdcMdSpi
    {
    private:
	CZDQHFtdcMdApi * m_pUserApi;
	map<string, CTPMatchyApi *> m_exchMap;
	vector<string> m_contracts;
	vector<string> m_exchanges;
	int m_numContracts = 0;
	    
	bool isReady = false;
	    
	// TThostFtdcBrokerIDType g_chBrokerID;
	// TThostFtdcUserIDType g_chUserID;

	// string password_;

	bool setTimeOffsetQ_ = false;
	int numDataToWarmUp_ = 0;

	CThostFtdcDepthMarketDataField  m_ctpDataMsgCache;

    public:

	CSimpleHandlerZD(CZDQHFtdcMdApi *pUserApi, int numDataToWarmUp);
	    
	~CSimpleHandlerZD();
		
	void addMatchy(CTPMatchyApi *);

	void getParamFromConfig(INIReader &iniconfig, std::string sectionName);
	    
	virtual void OnFrontConnected();		
	virtual void OnFrontDisconnected();
	
	virtual void OnRspSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	virtual void OnRspUnSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnDepthMarketData(CZDQHFtdcDepthQuoteDataField *pDepthMarketData);
	
    private:
	void convertToCTPMsg(CZDQHFtdcDepthQuoteDataField *pDepthMarketData);
    };
	
}

#endif  // ZDMARKETDATASPI_HPP

