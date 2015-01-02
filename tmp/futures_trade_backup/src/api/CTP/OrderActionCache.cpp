//
// File:        $ OrderActionCache.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/14 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "OrderActionCache.hpp"

namespace simulator
{

    OrderActionCache::OrderActionCache()
    {}

    OrderActionCache::~OrderActionCache()
    {}

    bool OrderActionCache::find(int orderId, OrderActionCacheType::iterator & it)
    {
	it = content_.find(orderId);
	return (it == content_.end()) ? false : true;
    }

    void OrderActionCache::insert(CThostFtdcOrderActionField *pOrderAction)
    {
	content_[atoi(pOrderAction->OrderRef)] = *pOrderAction;
    }

    void OrderActionCache::erase(int orderId)
    {
	content_.erase(orderId);
    }

    bool OrderActionCache::diffQ(CThostFtdcOrderActionField * p1, CThostFtdcOrderActionField * p2)
    {
	if (memcmp(&(p1->BrokerID), &(p2->BrokerID), sizeof(TThostFtdcBrokerIDType)) ||
	    memcmp(&(p1->InvestorID), &(p2->InvestorID), sizeof(TThostFtdcInvestorIDType)) ||
	    memcmp(&(p1->OrderActionRef), &(p2->OrderActionRef), sizeof(TThostFtdcOrderActionRefType)) ||
	    memcmp(&(p1->OrderRef), &(p2->OrderRef), sizeof(TThostFtdcOrderRefType)) ||
	    memcmp(&(p1->RequestID), &(p2->RequestID), sizeof(TThostFtdcRequestIDType)) ||
	    memcmp(&(p1->FrontID), &(p2->FrontID), sizeof(TThostFtdcFrontIDType)) ||
	    memcmp(&(p1->SessionID), &(p2->SessionID), sizeof(TThostFtdcSessionIDType)) ||
	    memcmp(&(p1->ExchangeID), &(p2->ExchangeID), sizeof(TThostFtdcExchangeIDType)) ||
	    memcmp(&(p1->OrderSysID), &(p2->OrderSysID), sizeof(TThostFtdcOrderSysIDType)) ||
	    memcmp(&(p1->ActionFlag), &(p2->ActionFlag), sizeof(TThostFtdcActionFlagType)) ||
	    memcmp(&(p1->LimitPrice), &(p2->LimitPrice), sizeof(TThostFtdcPriceType)) ||
	    memcmp(&(p1->VolumeChange), &(p2->VolumeChange), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(p1->ActionDate), &(p2->ActionDate), sizeof(TThostFtdcDateType)) ||
	    memcmp(&(p1->ActionTime), &(p2->ActionTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(p1->TraderID), &(p2->TraderID), sizeof(TThostFtdcTraderIDType)) ||
	    memcmp(&(p1->InstallID), &(p2->InstallID), sizeof(TThostFtdcInstallIDType)) ||
	    memcmp(&(p1->OrderLocalID), &(p2->OrderLocalID), sizeof(TThostFtdcOrderLocalIDType)) ||
	    memcmp(&(p1->ActionLocalID), &(p2->ActionLocalID), sizeof(TThostFtdcOrderLocalIDType)) ||
	    memcmp(&(p1->ParticipantID), &(p2->ParticipantID), sizeof(TThostFtdcParticipantIDType)) ||
	    memcmp(&(p1->ClientID), &(p2->ClientID), sizeof(TThostFtdcClientIDType)) ||
	    memcmp(&(p1->BusinessUnit), &(p2->BusinessUnit), sizeof(TThostFtdcBusinessUnitType)) ||
	    memcmp(&(p1->OrderActionStatus), &(p2->OrderActionStatus), sizeof(TThostFtdcOrderActionStatusType)) ||
	    memcmp(&(p1->UserID), &(p2->UserID), sizeof(TThostFtdcUserIDType)) ||
	    memcmp(&(p1->StatusMsg), &(p2->StatusMsg), sizeof(TThostFtdcErrorMsgType)) ||
	    memcmp(&(p1->InstrumentID), &(p2->InstrumentID), sizeof(TThostFtdcInstrumentIDType))) {
	    return true;
	} else {
	    return false;
	}
    }

}

// ORDERACTIONCACHE_CPP
