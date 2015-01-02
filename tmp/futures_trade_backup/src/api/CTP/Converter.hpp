//
// File:        $ Converter.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/13 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef CONVERTER_HPP
#define CONVERTER_HPP

#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>

using std::sprintf;
using std::string;
using std::memset;
using std::memcpy;
using std::strcpy;
using std::atoi;

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

#include "Message.hpp"
#include "SimulatorCore.hpp"
#include "OrderCache.hpp"

using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::BuySellSide;
using simulator::SimulatorCore;
using simulator::MarketOrderMessage;
using simulator::LimitOrderMessage;
using simulator::CancelOrderMessage;
using simulator::AcknowledgedOrderMessage;
using simulator::FilledOrderMessage;
using simulator::CanceledOrderMessage;
using simulator::RejectedOrderMessage;
using simulator::time_stamp;
using simulator::duration_nano;
using simulator::OrderCache;
using simulator::OrderCacheType;
using simulator::convertStringToTimePoint;

namespace simulator
{

    string combineToTimestampString(TThostFtdcDateType const, TThostFtdcTimeType const, TThostFtdcMillisecType const);

    template<typename DataFieldPointer, bool isBookSnapshotQ, bool isTradeSnapshotQ, int DEPTHOFBOOK, int TICKNUM, int TICKDENUM>
    class ConverterMessage;

    template<int tickNum, int tickDenum>
    class ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 1, tickNum, tickDenum>
    {
    private:
	static constexpr double tickSize = tickNum * 1.0 / tickDenum;

    public:
	// Convert snapshot of market data.
	static void convert(CThostFtdcDepthMarketDataField *pData, BookUpdate<true> *pBook, TradeUpdate<true> *pTrade, duration_nano timeOffset = 0)
	{
	    string timestamp = combineToTimestampString(pData->TradingDay, pData->UpdateTime, pData->UpdateMillisec);

	    time_stamp tp = convertStringToTimePoint(timestamp);
	    tp += timeOffset;

	    // Update book at top level.
	    pBook->exchangeId() = pData->ExchangeID;

	    pBook->contractId() = pData->InstrumentID;

	    pBook->timestamp() = tp;

	    ++(pBook->messageId());

	    (pBook->bidQty()).push_back(pData->BidVolume1);

	    if (pData->BidVolume1 > 0)
			(pBook->bidPx()).push_back(Px(pData->BidPrice1, tickSize));
	    else
			(pBook->bidPx()).push_back(Px(0.0, tickSize));

	    (pBook->askQty()).push_back(pData->AskVolume1);

	    if (pData->AskVolume1 > 0)
			(pBook->askPx()).push_back(Px(pData->AskPrice1, tickSize));
	    else
			(pBook->askPx()).push_back(Px(0.0, tickSize));

	    // Update trade.
	    pTrade->exchangeId() = pData->ExchangeID;
	    pTrade->contractId() = pData->InstrumentID;
	    pTrade->timestamp() = tp;
	    pTrade->messageId() = pBook->getMessageId();
	    (pTrade->px()).setPx(pData->LastPrice);
	    (pTrade->px()).setTickSize(tickSize);
	    pTrade->qty() = pData->Volume;
	    pTrade->side() = BuySellSide::unknown;
	}

	// Convert completely filled limit order or partially filled market order.
	// Correct CTP-API logic is that OnRtnTrade sends incremental trade messages.
	static void convert(CThostFtdcTradeField * pTrade, FilledOrderMessage * ptr, OrderCache * orderCachePtr)
	{
	    ptr->orderId() = atoi(pTrade->OrderRef);
	    ptr->qty() = pTrade->Volume;
	    ptr->side() = (pTrade->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    ptr->px() = Px(pTrade->Price, tickSize);
	    ptr->qty() = pTrade->Volume;
	}

	/* TODO */
	// Should never be called in PRODUCTION environment as of now
	// Convert completely filled limit order or partially filled limit order.
	static void convert(CThostFtdcOrderField * pOrder, FilledOrderMessage * ptr, OrderCache * orderCachePtr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    ptr->px() = Px(pOrder->LimitPrice, tickSize);
	    OrderCacheType::iterator it;
	    if (orderCachePtr->find(ptr->getOrderId(), it)) {
		ptr->qty() = pOrder->VolumeTraded - (it->second).VolumeTraded;
	    }
	    else {
		ptr->qty() = pOrder->VolumeTraded;
	    }
	}

	// Convert canceled limit order.
	static void convert(CThostFtdcOrderField * pOrder, CanceledOrderMessage<LimitOrderMessage> * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    ptr->px() = Px(pOrder->LimitPrice, tickSize);
	}

	// Convert canceled market order.
	static void convert(CThostFtdcOrderField * pOrder, CanceledOrderMessage<MarketOrderMessage> * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	}

	// Convert rejected limit order.
	static void convert(CThostFtdcOrderField * pOrder, RejectedOrderMessage<LimitOrderMessage> * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    ptr->px() = Px(pOrder->LimitPrice, tickSize);
	}


	static void convert(CThostFtdcInputOrderField * pOrder, RejectedOrderMessage<LimitOrderMessage> * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotalOriginal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    ptr->px() = Px(pOrder->LimitPrice, tickSize);
	}

	// Convert rejected market order.
	static void convert(CThostFtdcOrderField * pOrder, RejectedOrderMessage<MarketOrderMessage> * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	}

	static void convert(CThostFtdcInputOrderField * pOrder, RejectedOrderMessage<MarketOrderMessage> * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotalOriginal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	}

	// Convert rejected cancel limit order.
	static void convert(CThostFtdcOrderField * pOrder, RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    ptr->px() = Px(pOrder->LimitPrice, tickSize);
	}

	static void convert(CThostFtdcOrderActionField * pOrder, RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    //ptr->qty() = pOrder->VolumeTotal;
	    //ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    //ptr->px() = Px(pOrder->LimitPrice, tickSize);
	}


	static void convert(CThostFtdcInputOrderActionField * pOrder, RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    //ptr->qty() = pOrder->VolumeChange;
	    //ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	    //ptr->px() = Px(pOrder->LimitPrice, tickSize);
	}

	// Convert rejected cancel market order.
	static void convert(CThostFtdcOrderField * pOrder, RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    ptr->qty() = pOrder->VolumeTotal;
	    ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	}

	static void convert(CThostFtdcOrderActionField * pOrder, RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    //ptr->qty() = pOrder->VolumeChange;
	    //ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	}


	static void convert(CThostFtdcInputOrderActionField * pOrder, RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	    //ptr->qty() = pOrder->VolumeChange;
	    //ptr->side() = (pOrder->Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
	}

	// Convert acknowledgement of cancel order.
	// Convert acknowledgement of market order or limit order.
	static void convert(CThostFtdcOrderField * pOrder, AcknowledgedOrderMessage * ptr)
	{
	    ptr->orderId() = atoi(pOrder->OrderRef);
	}
    };
    
    template<int tickNum, int tickDenum>
    class ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 5, tickNum, tickDenum>
    {
    private:
	static constexpr double tickSize = tickNum * 1.0 / tickDenum;

    public:
	static void convert(CThostFtdcDepthMarketDataField *pData, BookUpdate<true> *pBook, TradeUpdate<true> *pTrade, duration_nano timeOffset = 0)
	{
	    string timestamp = combineToTimestampString(pData->TradingDay, pData->UpdateTime, pData->UpdateMillisec);
	    time_stamp tp = convertStringToTimePoint(timestamp);
	    tp += timeOffset;

	    // Update book at top 5 levels.
	    pBook->exchangeId() = pData->ExchangeID;
	    pBook->contractId() = pData->InstrumentID;
	    pBook->timestamp() = tp;
	    ++(pBook->messageId());
	    (pBook->bidQty())[1] = pData->BidVolume1;
	    (pBook->bidQty())[2] = pData->BidVolume2;
	    (pBook->bidQty())[3] = pData->BidVolume3;
	    (pBook->bidQty())[4] = pData->BidVolume4;
	    (pBook->bidQty())[5] = pData->BidVolume5;
	    (pBook->bidPx())[1].setTickSize(tickSize);
	    (pBook->bidPx())[2].setTickSize(tickSize);
	    (pBook->bidPx())[3].setTickSize(tickSize);
	    (pBook->bidPx())[4].setTickSize(tickSize);
	    (pBook->bidPx())[5].setTickSize(tickSize);
	    if (pData->BidVolume1 > 0)
		(pBook->bidPx())[1].setPx(pData->BidPrice1);
	    else
		(pBook->bidPx())[1].setPx(0.0);
	    if (pData->BidVolume2 > 0)
		(pBook->bidPx())[2].setPx(pData->BidPrice2);
	    else
		(pBook->bidPx())[2].setPx(0.0);
	    if (pData->BidVolume3 > 0)
		(pBook->bidPx())[3].setPx(pData->BidPrice3);
	    else
		(pBook->bidPx())[3].setPx(0.0);
	    if (pData->BidVolume4 > 0)
		(pBook->bidPx())[4].setPx(pData->BidPrice4);
	    else
		(pBook->bidPx())[4].setPx(0.0);
	    if (pData->BidVolume5 > 0)
		(pBook->bidPx())[5].setPx(pData->BidPrice5);
	    else
		(pBook->bidPx())[5].setPx(0.0);
		
	    (pBook->askQty())[1] = pData->AskVolume1;
	    (pBook->askQty())[2] = pData->AskVolume2;
	    (pBook->askQty())[3] = pData->AskVolume3;
	    (pBook->askQty())[4] = pData->AskVolume4;
	    (pBook->askQty())[5] = pData->AskVolume5;
	    (pBook->askPx())[1].setTickSize(tickSize);
	    (pBook->askPx())[2].setTickSize(tickSize);
	    (pBook->askPx())[3].setTickSize(tickSize);
	    (pBook->askPx())[4].setTickSize(tickSize);
	    (pBook->askPx())[5].setTickSize(tickSize);
	    if (pData->AskVolume1 > 0)
		(pBook->askPx())[1].setPx(pData->AskPrice1);
	    else
		(pBook->askPx())[1].setPx(0.0);
	    if (pData->AskVolume2 > 0)
		(pBook->askPx())[2].setPx(pData->AskPrice2);
	    else
		(pBook->askPx())[2].setPx(0.0);
	    if (pData->AskVolume3 > 0)
		(pBook->askPx())[3].setPx(pData->AskPrice3);
	    else
		(pBook->askPx())[3].setPx(0.0);
	    if (pData->AskVolume4 > 0)
		(pBook->askPx())[4].setPx(pData->AskPrice4);
	    else
		(pBook->askPx())[4].setPx(0.0);
	    if (pData->AskVolume5 > 0)
		(pBook->askPx())[5].setPx(pData->AskPrice5);
	    else
		(pBook->askPx())[5].setPx(0.0);

	    // Update trade.
	    pTrade->exchangeId() = pData->ExchangeID;
	    pTrade->contractId() = pData->InstrumentID;
	    pTrade->timestamp() = tp;
	    pTrade->messageId() = pBook->getMessageId();
	    (pTrade->px()).setPx(pData->LastPrice);
	    (pTrade->px()).setTickSize(tickSize);
	    pTrade->qty() = pData->Volume;
	    pTrade->side() = BuySellSide::unknown;
	}
    };
	
    template<typename POINTER>
    class ConverterOrder;
    
    template<>
    class ConverterOrder<CThostFtdcInputOrderField *>
    {
    // private:
	// TThostFtdcBrokerIDType	brokerId_;
	// TThostFtdcInvestorIDType investorId_;
	// TThostFtdcUserIDType userId_;
	// TThostFtdcFrontIDType frontId_;
	// TThostFtdcSessionIDType sessionId_;

    public:
	// void setBrokerId(const char *brokerId)
	// {
	//     strcpy(brokerId_, brokerId);
	// }

        // void setInvestorId(const char *investorId)
	// {
	//     strcpy(investorId_, investorId);
	// }

	// void setUserId(const char *userId)
	// {
	//     strcpy(userId_, userId);
	// }

	// void setFrontId(TThostFtdcFrontIDType frontId)
	// {
	//     frontId_ = frontId;
	// }

	// void setSessionId(TThostFtdcSessionIDType sessionId)
	// {
	//     sessionId_ = sessionId;
	// }

	BuySellSide convertMarketOrder(MarketOrderMessage *m, TThostFtdcRequestIDType requestId, CThostFtdcInputOrderField *pOrder)
	{
	    //memset(pOrder, 0, sizeof(CThostFtdcInputOrderField));
	    // strcpy(pOrder->BrokerID, brokerId_);
	    // strcpy(pOrder->InvestorID, investorId_);
	    // strcpy(pOrder->InstrumentID, (m->getContractId()).c_str());
	    TThostFtdcOrderRefType orderRef;
	    sprintf(orderRef, "%d", m->getOrderId());
	    strcpy(pOrder->OrderRef, orderRef);
	    // strcpy(pOrder->UserID, investorId_);
	    pOrder->OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	    pOrder->Direction = (m->getSide() == BuySellSide::buy) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	    // Set CombOffsetFlag outside this scope.
	    // (pOrder->CombHedgeFlag)[0] = THOST_FTDC_HF_Speculation;
	    // Skip LimitPrice.
	    //pOrder->LimitPrice = 0.0;
	    pOrder->VolumeTotalOriginal = m->getQty();
	    pOrder->TimeCondition = THOST_FTDC_TC_IOC;
	    // Skip GTDDate.
	    // pOrder->VolumeCondition = THOST_FTDC_VC_AV;
	    // pOrder->MinVolume = 1;
	    // pOrder->ContingentCondition = THOST_FTDC_CC_Immediately;
	    // Skip StopPrice.
	    // pOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	    // pOrder->IsAutoSuspend = 0;
	    // Skip BusinessUnit.
	    pOrder->RequestID = requestId;
	    // pOrder->UserForceClose = 0;

	    return m->getSide();
	}

	BuySellSide convertLimitOrder(LimitOrderMessage *m, TThostFtdcRequestIDType requestId, CThostFtdcInputOrderField *pOrder)
	{
	    //memset(pOrder, 0, sizeof(CThostFtdcInputOrderField));
	    // strcpy(pOrder->BrokerID, brokerId_);
	    // strcpy(pOrder->InvestorID, investorId_);
	    // strcpy(pOrder->InstrumentID, (m->getContractId()).c_str());
	    TThostFtdcOrderRefType orderRef;
	    sprintf(orderRef, "%d", m->getOrderId());
	    strcpy(pOrder->OrderRef, orderRef);
	    // strcpy(pOrder->UserID, investorId_);
	    pOrder->OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	    pOrder->Direction = (m->getSide() == BuySellSide::buy) ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell;
	    // Set CombOffsetFlag outside this scope.
	    // (pOrder->CombHedgeFlag)[0] = THOST_FTDC_HF_Speculation;
	    pOrder->LimitPrice = m->getPx().getPx();
	    pOrder->VolumeTotalOriginal = m->getQty();
	    pOrder->TimeCondition = THOST_FTDC_TC_GFD;
	    // Skip GTDDate.
	    // pOrder->VolumeCondition = THOST_FTDC_VC_AV;
	    // pOrder->MinVolume = 1;
	    // pOrder->ContingentCondition = THOST_FTDC_CC_Immediately;
	    // Skip StopPrice.
	    // pOrder->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	    // pOrder->IsAutoSuspend = 0;
	    // Skip BusinessUnit.
	    pOrder->RequestID = requestId;
	    // pOrder->UserForceClose = 0;

	    return m->getSide();
	}

    };

    template<>
    class ConverterOrder<CThostFtdcInputOrderActionField *>
    {
    public:
	bool convertCancelOrder(CancelOrderMessage<MarketOrderMessage> *m, TThostFtdcRequestIDType requestId, CThostFtdcInputOrderActionField *pOrderAction, OrderCache * orderCachePtr)
	{
	    OrderCacheType::iterator it;
	    if (!orderCachePtr->find(m->getOrderId(), it))
		return false;
	    else {
		memset(pOrderAction, 0, sizeof(CThostFtdcInputOrderActionField));
		strcpy(pOrderAction->BrokerID, (it->second).BrokerID);
		strcpy(pOrderAction->InvestorID, (it->second).InvestorID);
		// Skip OrderActionRef for now.
		strcpy(pOrderAction->OrderRef, (it->second).OrderRef);
		pOrderAction->RequestID = requestId;
		pOrderAction->FrontID = (it->second).FrontID;
		pOrderAction->SessionID = (it->second).SessionID;
		strcpy(pOrderAction->ExchangeID, (it->second).ExchangeID);
		strcpy(pOrderAction->OrderSysID, (it->second).OrderSysID);
		pOrderAction->ActionFlag = THOST_FTDC_AF_Delete;
		// Skip LimitPrice.
		// Skip VolumeChange.
		strcpy(pOrderAction->UserID, (it->second).UserID);
		strcpy(pOrderAction->InstrumentID, (it->second).InstrumentID);
		return true;
	    }
	}

	bool convertCancelOrder(CancelOrderMessage<LimitOrderMessage> *m, TThostFtdcRequestIDType requestId, CThostFtdcInputOrderActionField *pOrderAction, OrderCache * orderCachePtr)
	{
	    OrderCacheType::iterator it;
	    if (!orderCachePtr->find(m->getOrderId(), it))
		return false;
	    else {
		memset(pOrderAction, 0, sizeof(CThostFtdcInputOrderActionField));
		strcpy(pOrderAction->BrokerID, (it->second).BrokerID);
		strcpy(pOrderAction->InvestorID, (it->second).InvestorID);
		// Skip OrderActionRef for now.
		strcpy(pOrderAction->OrderRef, (it->second).OrderRef);
		pOrderAction->RequestID = requestId;
		pOrderAction->FrontID = (it->second).FrontID;
		pOrderAction->SessionID = (it->second).SessionID;
		strcpy(pOrderAction->ExchangeID, (it->second).ExchangeID);
		strcpy(pOrderAction->OrderSysID, (it->second).OrderSysID);
		pOrderAction->ActionFlag = THOST_FTDC_AF_Delete;
		pOrderAction->LimitPrice = (it->second).LimitPrice;
		// Skip VolumeChange.
		strcpy(pOrderAction->UserID, (it->second).UserID);
		strcpy(pOrderAction->InstrumentID, (it->second).InstrumentID);
		return true;
	    }
	}
    };
    
}

#endif  // CONVERTER_HPP
