//
// File:        $ OrderCache.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/16 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "OrderCache.hpp"

namespace simulator
{

    OrderCache::OrderCache()
    {}

    OrderCache::~OrderCache()
    {}

    bool OrderCache::find(int orderId, OrderCacheType::iterator & it)
    {
	it = content_.find(orderId);
	return (it == content_.end()) ? false : true;
    }

    void OrderCache::insert(CThostFtdcOrderField *pOrder)
    {
	content_[atoi(pOrder->OrderRef)] = *pOrder;
    }

    void OrderCache::erase(int orderId)
    {
	content_.erase(orderId);
    }

    bool OrderCache::diffQ(CThostFtdcOrderField * pOrder1, CThostFtdcOrderField * pOrder2)
    {
	if (memcmp(&(pOrder1->BrokerID), &(pOrder2->BrokerID), sizeof(TThostFtdcBrokerIDType)) ||
	    memcmp(&(pOrder1->InvestorID), &(pOrder2->InvestorID), sizeof(TThostFtdcInvestorIDType)) ||
	    memcmp(&(pOrder1->InstrumentID), &(pOrder2->InstrumentID), sizeof(TThostFtdcInstrumentIDType)) ||
	    memcmp(&(pOrder1->OrderRef), &(pOrder2->OrderRef), sizeof(TThostFtdcOrderRefType)) ||
	    memcmp(&(pOrder1->UserID), &(pOrder2->UserID), sizeof(TThostFtdcUserIDType)) ||
	    memcmp(&(pOrder1->OrderPriceType), &(pOrder2->OrderPriceType), sizeof(TThostFtdcOrderPriceTypeType)) ||
	    memcmp(&(pOrder1->Direction), &(pOrder2->Direction), sizeof(TThostFtdcDirectionType)) ||
	    memcmp(&(pOrder1->CombOffsetFlag), &(pOrder2->CombOffsetFlag), sizeof(TThostFtdcCombOffsetFlagType)) ||
	    memcmp(&(pOrder1->CombHedgeFlag), &(pOrder2->CombHedgeFlag), sizeof(TThostFtdcCombHedgeFlagType)) ||
	    memcmp(&(pOrder1->LimitPrice), &(pOrder2->LimitPrice), sizeof(TThostFtdcPriceType)) ||
	    memcmp(&(pOrder1->VolumeTotalOriginal), &(pOrder2->VolumeTotalOriginal), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->TimeCondition), &(pOrder2->TimeCondition), sizeof(TThostFtdcTimeConditionType)) ||
	    memcmp(&(pOrder1->GTDDate), &(pOrder2->GTDDate), sizeof(TThostFtdcDateType)) ||
	    memcmp(&(pOrder1->VolumeCondition), &(pOrder2->VolumeCondition), sizeof(TThostFtdcVolumeConditionType)) ||
	    memcmp(&(pOrder1->MinVolume), &(pOrder2->MinVolume), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->ContingentCondition), &(pOrder2->ContingentCondition), sizeof(TThostFtdcContingentConditionType)) ||
	    memcmp(&(pOrder1->StopPrice), &(pOrder2->StopPrice), sizeof(TThostFtdcPriceType)) ||
	    memcmp(&(pOrder1->ForceCloseReason), &(pOrder2->ForceCloseReason), sizeof(TThostFtdcForceCloseReasonType)) ||
	    memcmp(&(pOrder1->IsAutoSuspend), &(pOrder2->IsAutoSuspend), sizeof(TThostFtdcBoolType)) ||
	    memcmp(&(pOrder1->BusinessUnit), &(pOrder2->BusinessUnit), sizeof(TThostFtdcBusinessUnitType)) ||
	    memcmp(&(pOrder1->RequestID), &(pOrder2->RequestID), sizeof(TThostFtdcRequestIDType)) ||
	    memcmp(&(pOrder1->OrderLocalID), &(pOrder2->OrderLocalID), sizeof(TThostFtdcOrderLocalIDType)) ||
	    memcmp(&(pOrder1->ExchangeID), &(pOrder2->ExchangeID), sizeof(TThostFtdcExchangeIDType)) ||
	    memcmp(&(pOrder1->ParticipantID), &(pOrder2->ParticipantID), sizeof(TThostFtdcParticipantIDType)) ||
	    memcmp(&(pOrder1->ClientID), &(pOrder2->ClientID), sizeof(TThostFtdcClientIDType)) ||
	    memcmp(&(pOrder1->ExchangeInstID), &(pOrder2->ExchangeInstID), sizeof(TThostFtdcExchangeInstIDType)) ||
	    memcmp(&(pOrder1->TraderID), &(pOrder2->TraderID), sizeof(TThostFtdcTraderIDType)) ||
	    memcmp(&(pOrder1->InstallID), &(pOrder2->InstallID), sizeof(TThostFtdcInstallIDType)) ||
	    memcmp(&(pOrder1->OrderSubmitStatus), &(pOrder2->OrderSubmitStatus), sizeof(TThostFtdcOrderSubmitStatusType)) ||
	    memcmp(&(pOrder1->NotifySequence), &(pOrder2->NotifySequence), sizeof(TThostFtdcSequenceNoType)) ||
	    memcmp(&(pOrder1->TradingDay), &(pOrder2->TradingDay), sizeof(TThostFtdcDateType)) ||
	    memcmp(&(pOrder1->SettlementID), &(pOrder2->SettlementID), sizeof(TThostFtdcSettlementIDType)) ||
	    memcmp(&(pOrder1->OrderSysID), &(pOrder2->OrderSysID), sizeof(TThostFtdcOrderSysIDType)) ||
	    memcmp(&(pOrder1->OrderSource), &(pOrder2->OrderSource), sizeof(TThostFtdcOrderSourceType)) ||
	    memcmp(&(pOrder1->OrderStatus), &(pOrder2->OrderStatus), sizeof(TThostFtdcOrderStatusType)) ||
	    memcmp(&(pOrder1->OrderType), &(pOrder2->OrderType), sizeof(TThostFtdcOrderTypeType)) ||
	    memcmp(&(pOrder1->VolumeTraded), &(pOrder2->VolumeTraded), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->VolumeTotal), &(pOrder2->VolumeTotal), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->InsertDate), &(pOrder2->InsertDate), sizeof(TThostFtdcDateType)) ||
	    memcmp(&(pOrder1->InsertTime), &(pOrder2->InsertTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->ActiveTime), &(pOrder2->ActiveTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->SuspendTime), &(pOrder2->SuspendTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->UpdateTime), &(pOrder2->UpdateTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->CancelTime), &(pOrder2->CancelTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->ActiveTraderID), &(pOrder2->ActiveTraderID), sizeof(TThostFtdcTraderIDType)) ||
	    memcmp(&(pOrder1->ClearingPartID), &(pOrder2->ClearingPartID), sizeof(TThostFtdcParticipantIDType)) ||
	    memcmp(&(pOrder1->SequenceNo), &(pOrder2->SequenceNo), sizeof(TThostFtdcSequenceNoType)) ||
	    memcmp(&(pOrder1->FrontID), &(pOrder2->FrontID), sizeof(TThostFtdcFrontIDType)) ||
	    memcmp(&(pOrder1->SessionID), &(pOrder2->SessionID), sizeof(TThostFtdcSessionIDType)) ||
	    memcmp(&(pOrder1->UserProductInfo), &(pOrder2->UserProductInfo), sizeof(TThostFtdcProductInfoType)) ||
	    memcmp(&(pOrder1->StatusMsg), &(pOrder2->StatusMsg), sizeof(TThostFtdcErrorMsgType)) ||
	    memcmp(&(pOrder1->UserForceClose), &(pOrder2->UserForceClose), sizeof(TThostFtdcBoolType)) ||
	    memcmp(&(pOrder1->ActiveUserID), &(pOrder2->ActiveUserID), sizeof(TThostFtdcUserIDType)) ||
	    memcmp(&(pOrder1->BrokerOrderSeq), &(pOrder2->BrokerOrderSeq), sizeof(TThostFtdcSequenceNoType)) ||
	    memcmp(&(pOrder1->RelativeOrderSysID), &(pOrder2->RelativeOrderSysID), sizeof(TThostFtdcOrderSysIDType)) ||
	    memcmp(&(pOrder1->ZCETotalTradedVolume), &(pOrder2->ZCETotalTradedVolume), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->IsSwapOrder), &(pOrder2->IsSwapOrder), sizeof(TThostFtdcBoolType))) {
	    return true;
	} else {
	    return false;
	}
    }

    /*
    bool OrderCache::diffOnlyAtSequenceNoQ(CThostFtdcOrderField * pOrder1, CThostFtdcOrderField * pOrder2)
    {
	if (memcmp(&(pOrder1->BrokerID), &(pOrder2->BrokerID), sizeof(TThostFtdcBrokerIDType)) ||
	    memcmp(&(pOrder1->InvestorID), &(pOrder2->InvestorID), sizeof(TThostFtdcInvestorIDType)) ||
	    memcmp(&(pOrder1->InstrumentID), &(pOrder2->InstrumentID), sizeof(TThostFtdcInstrumentIDType)) ||
	    memcmp(&(pOrder1->OrderRef), &(pOrder2->OrderRef), sizeof(TThostFtdcOrderRefType)) ||
	    memcmp(&(pOrder1->UserID), &(pOrder2->UserID), sizeof(TThostFtdcUserIDType)) ||
	    memcmp(&(pOrder1->OrderPriceType), &(pOrder2->OrderPriceType), sizeof(TThostFtdcOrderPriceTypeType)) ||
	    memcmp(&(pOrder1->Direction), &(pOrder2->Direction), sizeof(TThostFtdcDirectionType)) ||
	    memcmp(&(pOrder1->CombOffsetFlag), &(pOrder2->CombOffsetFlag), sizeof(TThostFtdcCombOffsetFlagType)) ||
	    memcmp(&(pOrder1->CombHedgeFlag), &(pOrder2->CombHedgeFlag), sizeof(TThostFtdcCombHedgeFlagType)) ||
	    memcmp(&(pOrder1->LimitPrice), &(pOrder2->LimitPrice), sizeof(TThostFtdcPriceType)) ||
	    memcmp(&(pOrder1->VolumeTotalOriginal), &(pOrder2->VolumeTotalOriginal), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->TimeCondition), &(pOrder2->TimeCondition), sizeof(TThostFtdcTimeConditionType)) ||
	    memcmp(&(pOrder1->GTDDate), &(pOrder2->GTDDate), sizeof(TThostFtdcDateType)) ||
	    memcmp(&(pOrder1->VolumeCondition), &(pOrder2->VolumeCondition), sizeof(TThostFtdcVolumeConditionType)) ||
	    memcmp(&(pOrder1->MinVolume), &(pOrder2->MinVolume), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->ContingentCondition), &(pOrder2->ContingentCondition), sizeof(TThostFtdcContingentConditionType)) ||
	    memcmp(&(pOrder1->StopPrice), &(pOrder2->StopPrice), sizeof(TThostFtdcPriceType)) ||
	    memcmp(&(pOrder1->ForceCloseReason), &(pOrder2->ForceCloseReason), sizeof(TThostFtdcForceCloseReasonType)) ||
	    memcmp(&(pOrder1->IsAutoSuspend), &(pOrder2->IsAutoSuspend), sizeof(TThostFtdcBoolType)) ||
	    memcmp(&(pOrder1->BusinessUnit), &(pOrder2->BusinessUnit), sizeof(TThostFtdcBusinessUnitType)) ||
	    memcmp(&(pOrder1->RequestID), &(pOrder2->RequestID), sizeof(TThostFtdcRequestIDType)) ||
	    memcmp(&(pOrder1->OrderLocalID), &(pOrder2->OrderLocalID), sizeof(TThostFtdcOrderLocalIDType)) ||
	    memcmp(&(pOrder1->ExchangeID), &(pOrder2->ExchangeID), sizeof(TThostFtdcExchangeIDType)) ||
	    memcmp(&(pOrder1->ParticipantID), &(pOrder2->ParticipantID), sizeof(TThostFtdcParticipantIDType)) ||
	    memcmp(&(pOrder1->ClientID), &(pOrder2->ClientID), sizeof(TThostFtdcClientIDType)) ||
	    memcmp(&(pOrder1->ExchangeInstID), &(pOrder2->ExchangeInstID), sizeof(TThostFtdcExchangeInstIDType)) ||
	    memcmp(&(pOrder1->TraderID), &(pOrder2->TraderID), sizeof(TThostFtdcTraderIDType)) ||
	    memcmp(&(pOrder1->InstallID), &(pOrder2->InstallID), sizeof(TThostFtdcInstallIDType)) ||
	    memcmp(&(pOrder1->OrderSubmitStatus), &(pOrder2->OrderSubmitStatus), sizeof(TThostFtdcOrderSubmitStatusType)) ||
	    memcmp(&(pOrder1->NotifySequence), &(pOrder2->NotifySequence), sizeof(TThostFtdcSequenceNoType)) ||
	    memcmp(&(pOrder1->TradingDay), &(pOrder2->TradingDay), sizeof(TThostFtdcDateType)) ||
	    memcmp(&(pOrder1->SettlementID), &(pOrder2->SettlementID), sizeof(TThostFtdcSettlementIDType)) ||
	    memcmp(&(pOrder1->OrderSysID), &(pOrder2->OrderSysID), sizeof(TThostFtdcOrderSysIDType)) ||
	    memcmp(&(pOrder1->OrderSource), &(pOrder2->OrderSource), sizeof(TThostFtdcOrderSourceType)) ||
	    memcmp(&(pOrder1->OrderStatus), &(pOrder2->OrderStatus), sizeof(TThostFtdcOrderStatusType)) ||
	    memcmp(&(pOrder1->OrderType), &(pOrder2->OrderType), sizeof(TThostFtdcOrderTypeType)) ||
	    memcmp(&(pOrder1->VolumeTraded), &(pOrder2->VolumeTraded), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->VolumeTotal), &(pOrder2->VolumeTotal), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->InsertDate), &(pOrder2->InsertDate), sizeof(TThostFtdcDateType)) ||
	    memcmp(&(pOrder1->InsertTime), &(pOrder2->InsertTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->ActiveTime), &(pOrder2->ActiveTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->SuspendTime), &(pOrder2->SuspendTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->UpdateTime), &(pOrder2->UpdateTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->CancelTime), &(pOrder2->CancelTime), sizeof(TThostFtdcTimeType)) ||
	    memcmp(&(pOrder1->ActiveTraderID), &(pOrder2->ActiveTraderID), sizeof(TThostFtdcTraderIDType)) ||
	    memcmp(&(pOrder1->ClearingPartID), &(pOrder2->ClearingPartID), sizeof(TThostFtdcParticipantIDType)) ||
	    (memcmp(&(pOrder1->SequenceNo), &(pOrder2->SequenceNo), sizeof(TThostFtdcSequenceNoType)) == 0) ||
	    memcmp(&(pOrder1->FrontID), &(pOrder2->FrontID), sizeof(TThostFtdcFrontIDType)) ||
	    memcmp(&(pOrder1->SessionID), &(pOrder2->SessionID), sizeof(TThostFtdcSessionIDType)) ||
	    memcmp(&(pOrder1->UserProductInfo), &(pOrder2->UserProductInfo), sizeof(TThostFtdcProductInfoType)) ||
	    memcmp(&(pOrder1->StatusMsg), &(pOrder2->StatusMsg), sizeof(TThostFtdcErrorMsgType)) ||
	    memcmp(&(pOrder1->UserForceClose), &(pOrder2->UserForceClose), sizeof(TThostFtdcBoolType)) ||
	    memcmp(&(pOrder1->ActiveUserID), &(pOrder2->ActiveUserID), sizeof(TThostFtdcUserIDType)) ||
	    memcmp(&(pOrder1->BrokerOrderSeq), &(pOrder2->BrokerOrderSeq), sizeof(TThostFtdcSequenceNoType)) ||
	    memcmp(&(pOrder1->RelativeOrderSysID), &(pOrder2->RelativeOrderSysID), sizeof(TThostFtdcOrderSysIDType)) ||
	    memcmp(&(pOrder1->ZCETotalTradedVolume), &(pOrder2->ZCETotalTradedVolume), sizeof(TThostFtdcVolumeType)) ||
	    memcmp(&(pOrder1->IsSwapOrder), &(pOrder2->IsSwapOrder), sizeof(TThostFtdcBoolType))) {
	    return false;
	} else {
	    return true;
	}
    }
    */

}

// ORDERCACHE_CPP
