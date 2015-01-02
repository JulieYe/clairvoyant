//
// File:        $ CFFEX.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2014/01/25 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2014 by Zhengzhang Futures Co. Ltd.

#include "CFFEX.hpp"

namespace simulator
{

#ifdef SIMULATION
    /* Parse string of data line and store. */
    void CFFEX::parser(string data, BookUpdate<true> *pBookUpdate, TradeUpdate<true> *pTradeUpdate)
    {
	SimMatchy::parser(data, pBookUpdate, pTradeUpdate);
    }

    CFFEX::CFFEX(string contractId, string fileName, SimulatorCore * simulatorCorePtr)
	: SimMatchy("CFFEX", contractId, 0.2, fileName, simulatorCorePtr)
    {
	if (!init()) {
	    InitException myexception;
	    throw myexception;
	}
    }

    CFFEX::CFFEX(string exchangeId, string contractId, string fileName, double tickSize, SimulatorCore * simulatorCorePtr)
	: SimMatchy(exchangeId, contractId, tickSize, fileName, simulatorCorePtr)
    {
	if (!init()) {
	    InitException myexception;
	    throw myexception;
	}
    }

    CFFEX::CFFEX(string exchangeId, double tickSize) :
	SimMatchy(exchangeId)
    { 
	setTickSize(tickSize);
    }

    void CFFEX::getParamFromConfig(INIReader & iniconfig, string sectionName)
    {
	string contract = iniconfig.Get(sectionName, "Contract", "");
	if(contract == "") {
	    throw;
	}
	setContract(contract);
	string filename = iniconfig.Get(sectionName, "DataFile", "");
	if(filename == "") {
	    throw;
	}
	if(!preLoadData(filename)) {
	    InitException myexception;
	    throw myexception;
	}	    
	if (!init()) {
	    InitException myexception;
	    throw myexception;
	}
    }
	
    CFFEX::~CFFEX()
    {}
#endif

    CTPMatchyApi::CTPMatchyApi(string exchange, string contract, int buyOpenPosition, int sellOpenPosition, SimulatorCore * ptr) :
	Matchy(exchange, contract, ptr) 
    {
	initMeta(buyOpenPosition, sellOpenPosition);
    }

    CTPMatchyApi::CTPMatchyApi(string exchange) :
	Matchy(exchange)
    { }

    void CTPMatchyApi::getParamFromConfig(INIReader & iniconfig, string sectionName)
    {
	string contract = iniconfig.Get(sectionName, "Contract", "");
	if(contract == "") {
	    throw;
	}
	setContract(contract);
	int buyOpenPosition = iniconfig.GetInteger(sectionName, "BuyOpenPosition", 0);
	int sellOpenPosition = iniconfig.GetInteger(sectionName, "SellOpenPosition", 0);
	initMeta(buyOpenPosition, sellOpenPosition);
    }

    void CTPMatchyApi::inputOrderFieldInit(CThostFtdcInputOrderField * pOrder)
    {
	memset(pOrder, 0, sizeof(CThostFtdcInputOrderField));
	strcpy(pOrder->BrokerID, brokerId());
	strcpy(pOrder->InvestorID, investorId());
	strcpy(pOrder->UserID, userId());
	strcpy(pOrder->InstrumentID, getContractId().c_str());
#ifdef ACCOUNT_SPECULATOR
	(pOrder->CombHedgeFlag)[0] = THOST_FTDC_HF_Speculation;
#elif defined ACCOUNT_ARBITRAGER
	(pOrder->CombHedgeFlag)[0] = THOST_FTDC_HF_Arbitrage;
#elif defined ACCOUNT_HEDGER
	(pOrder->CombHedgeFlag)[0] = THOST_FTDC_HF_Hedge;
#endif
	pOrder->VolumeCondition = THOST_FTDC_VC_AV;
	pOrder->MinVolume = 1;
	pOrder->ContingentCondition = THOST_FTDC_CC_Immediately;
	pOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	pOrder->IsAutoSuspend = 0;
	pOrder->UserForceClose = 0;
    }

    void CTPMatchyApi::initMeta(int buyOpenPosition, int sellOpenPosition)
    {
	meta_.tradedVolume_ = 0;
	meta_.numData_ = 0;
	meta_.buyOpenQty_ = buyOpenPosition;	
	meta_.sellOpenQty_ = sellOpenPosition;
	cout << "CTPMatchyApi::initMeta - buyOpenPosition set to " << meta_.buyOpenQty_ 
	     << " for "  << getTicker() << endl;
	cout << "CTPMatchyApi::initMeta - sellOpenPosition set to " << meta_.sellOpenQty_ 
	     << " for "  << getTicker() << endl;
	meta_.buyOpenOrderQty_ = 0;
	meta_.sellOpenOrderQty_ = 0;
	meta_.buyCloseOrderQty_ = 0;
	meta_.sellCloseOrderQty_ = 0;
    }

    void CTPMatchyApi::setOrderId(int orderId)
    {
	CSimpleTrader * traderSpi = dynamic_cast<CSimpleTrader *>(traderSpi_);
	if (traderSpi == 0) {
	    cout << "CTPMatchyApi::setOrderId - failed from exchange " << getTicker() << endl;
	} else {
	    traderSpi->orderId() = orderId;
	}
    }

    void CTPMatchyApi::getOrderId()
    {
	CSimpleTrader * traderSpi = dynamic_cast<CSimpleTrader *>(traderSpi_);
	if (traderSpi == 0) {
	    cout << "CTPMatchyApi::getOrderId - failed from exchange " << getTicker() << endl;
	} else {
	    simulatorCorePtr_->orderId() = traderSpi->orderId();
	}
    }

}

// CFFEX_CPP
