//
// File:        $ MarketDataSpi.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/25 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef MARKETDATASPI_HPP
#define MARKETDATASPI_HPP

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

#include "ThostFtdcMdApi.h"

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
	
    class CSimpleHandler : public CThostFtdcMdSpi
    {
    private:
	CThostFtdcMdApi * m_pUserApi;
	map<string, CTPMatchyApi *> m_exchMap;
	vector<string> m_contracts;
	int m_numContracts = 0;
	    
	bool isReady = false;
	    
	TThostFtdcBrokerIDType g_chBrokerID;
	TThostFtdcUserIDType g_chUserID;

	string password_;

	bool setTimeOffsetQ_ = false;
	int numDataToWarmUp_ = 0;
		
    public:

	CSimpleHandler(CThostFtdcMdApi *pUserApi, int numDataToWarmUp);
	    
	~CSimpleHandler();
		
	void addMatchy(CTPMatchyApi *);

	void getParamFromConfig(INIReader &iniconfig, std::string sectionName);
	    
	virtual void OnFrontConnected();		
	virtual void OnFrontDisconnected(int nReason);
	    
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	    
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	    
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
	    
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    };
	
}

#endif  // MARKETDATASPI_HPP

