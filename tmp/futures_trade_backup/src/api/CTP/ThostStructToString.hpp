//
// File:        $ ThostStructToString.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/10 16:36:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.


#ifndef THOSTSTRUCTTOSTRING_HPP
#define THOSTSTRUCTTOSTRING_HPP

#include <sstream>
#include <string>

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

using std::stringstream;
using std::string;

namespace simulator {

    string convertStructToString(CThostFtdcDepthMarketDataField *pData, string delimiter);
    string convertStructToString(CThostFtdcTradeField * pTrade, string delimiter);
    string convertStructToString(CThostFtdcOrderField * pOrder, string delimiter);
    string convertStructToString(CThostFtdcInputOrderField * pInput, string delimiter);
    string convertStructToString(CThostFtdcInputOrderActionField * pAction, string delimiter);
    string convertStructToString(CThostFtdcSettlementInfoField * pSettlementInfo, string delimiter);    
    string convertStructToString(CThostFtdcOrderActionField *pAction, string delimiter);
}










#endif 
