//
// File:        $ CFFEX.hpp $
//
// Version:     $ Revision: 2.2 $
//
// Modified:    $ Date: 2014/01/22 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef CFFEX_HPP
#define CFFEX_HPP

#include <exception>
#include <utility>
#include <mutex>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <numeric>

using std::exception;
using std::move;
using std::mutex;
using std::lock_guard;
using std::atoi;
using std::sort;
using std::partition_copy;
using std::cout;
using std::cerr;
using std::endl;
using std::accumulate;

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

#include "Matchy.hpp"
#include "Message.hpp"
#include "SimulatorCore.hpp"
#include "Converter.hpp"
#include "OrderCache.hpp"
#include "OrderActionCache.hpp"
#include "OrderInputCache.hpp"
#include "OrderInputActionCache.hpp"
#include "ThostStructToString.hpp"
#include "TraderSpi.hpp"


namespace simulator
{
    class CSimpleTrader;
    class SimulatorCore;
}

using simulator::CSimpleTrader;
using simulator::Matchy;
using simulator::SimMatchy;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::BuySellSide;
using simulator::SimulatorCore;
using simulator::MarketOrderMessage;
using simulator::LimitOrderMessage;
using simulator::CancelOrderMessage;
using simulator::time_stamp;
using simulator::duration_nano;
using simulator::combineToTimestampString;
using simulator::ConverterMessage;
using simulator::ConverterOrder;
using simulator::OrderCache;
using simulator::OrderActionCache;
using simulator::OrderInputCache;
using simulator::OrderInputActionCache;
using simulator::convertStructToString;

namespace simulator
{

#ifdef SIMULATION
    class CFFEX : public SimMatchy
    {
    public:
	CFFEX() = delete;

	CFFEX(CFFEX const &) = delete;

	CFFEX & operator=(CFFEX const &) = delete;

	CFFEX(string, string, SimulatorCore *);

	CFFEX(string, string, string, double, SimulatorCore *);

	virtual ~CFFEX();

	virtual void parser(string, BookUpdate<true> *, TradeUpdate<true> *);

	inline list<BookUpdate<true> > getNextBookUpdate() const { return nextBookUpdate_; }
	inline list<TradeUpdate<true> > getNextTradeUpdate() const { return nextTradeUpdate_; }
	inline BookUpdate<true> getCurrBookUpdate() const { return currBookUpdate_; }
    };

    typedef CFFEX CTPEXCH;

#endif

    class CTPMatchyApi : public Matchy
    {
    protected:
	struct Meta
	{
	    TThostFtdcBrokerIDType brokerId_;
	    TThostFtdcInvestorIDType investorId_;
	    TThostFtdcUserIDType userId_;
	    TThostFtdcFrontIDType frontId_;
	    TThostFtdcSessionIDType sessionId_;
	    TThostFtdcPriceType upperLimitPrice_;
	    TThostFtdcPriceType lowerLimitPrice_;
	    TThostFtdcRatioType longMarginRatio_;
	    TThostFtdcRatioType shortMarginRatio_;
	    int buyOpenQty_;
	    int sellOpenQty_;
	    int buyOpenOrderQty_;
	    int sellOpenOrderQty_;
	    int buyCloseOrderQty_;
	    int sellCloseOrderQty_;
	    int tradedVolume_;
	    int numData_;
	} meta_;

	CThostFtdcTraderApi * traderApi_;
	CThostFtdcTraderSpi * traderSpi_;

	ConverterOrder<CThostFtdcInputOrderField *> orderInsertConverter_;
	ConverterOrder<CThostFtdcInputOrderActionField *> orderActionConverter_;

	// acknowledged order message cache
	OrderCache orderCache_;
	OrderActionCache orderActionCache_;
	OrderInputCache orderInputCache_;
	OrderInputActionCache orderInputActionCache_;

    public:
    	CTPMatchyApi() = delete;
	
    	CTPMatchyApi(CTPMatchyApi const &) = delete;

    	CTPMatchyApi & operator=(CTPMatchyApi const &) = delete;

	CTPMatchyApi(string exchange, string contract, int buyOpenPosition, int sellOpenPosition, SimulatorCore * ptr);
	    
	virtual void push(CThostFtdcDepthMarketDataField *) = 0;
	virtual void push(CThostFtdcInputOrderField * pInput) = 0;
	virtual void push(CThostFtdcInputOrderActionField * pAction) = 0;
	virtual void push(CThostFtdcOrderActionField * pAction) = 0;
	virtual void push(CThostFtdcOrderField * pOrder) = 0;
	virtual void push(CThostFtdcTradeField * pTrade) = 0;
	
	/* Set/get BrokerID. */
	inline TThostFtdcBrokerIDType & brokerId() { return meta_.brokerId_; }

	/* Set/get InvestorID. */
	inline TThostFtdcInvestorIDType & investorId() { return meta_.investorId_; }

	/* Set/get UserID. */
	inline TThostFtdcUserIDType & userId() { return meta_.userId_; }

	/* Set/get FrontID. */
	inline TThostFtdcFrontIDType & frontId() { return meta_.frontId_; }

	/* Set/get SessionID. */
	inline TThostFtdcSessionIDType & sessionId() { return meta_.sessionId_; }

	/* Set/get upper/lower limit price for a contract on given day. */
	inline TThostFtdcPriceType & upperLimitPrice() { return meta_.upperLimitPrice_;	}
	inline TThostFtdcPriceType getUpperLimitPrice() const { return meta_.upperLimitPrice_; }
	inline TThostFtdcPriceType & lowerLimitPrice() { return meta_.lowerLimitPrice_;	}
	inline TThostFtdcPriceType getLowerLimitPrice() const { return meta_.lowerLimitPrice_;	}

	/* Set/get long/short margin ratio for a contract on given day. */
	inline TThostFtdcRatioType & longMarginRatio() { return meta_.longMarginRatio_;	}
	inline TThostFtdcRatioType & shortMarginRatio()	{ return meta_.shortMarginRatio_; }

	/* Check if px is within the lower/upper price limit bands. */
	inline bool isPriceValid(TThostFtdcPriceType px) const { return (px > getLowerLimitPrice()) && (px < getUpperLimitPrice()); }

	/* Set/get total traded volume of a contract on a given day. */
	inline int & tradedVolume() { return meta_.tradedVolume_; }

	/* Set/get number of data messages received on a given day. */
	inline int & numData() { return meta_.numData_; }

	/* Get Meta order informations. */
	virtual int buyOpenQty() const { return meta_.buyOpenQty_; }
	virtual int sellOpenQty() const { return meta_.sellOpenQty_; }
	virtual int buyOpenOrderQty() const { return meta_.buyOpenOrderQty_; }
	virtual int buyCloseOrderQty() const { return meta_.buyCloseOrderQty_; }
	virtual int sellOpenOrderQty() const { return meta_.sellOpenOrderQty_; }
	virtual int sellCloseOrderQty() const { return meta_.sellCloseOrderQty_; }

	/* Access trader api. */
	inline CThostFtdcTraderApi * & traderApi() { return traderApi_; }

	/* Access market data api. */
	inline CThostFtdcTraderSpi * & traderSpi() { return traderSpi_; }

	/* Converter accessors */
	inline ConverterOrder<CThostFtdcInputOrderField *> & orderInsertConverter() { return orderInsertConverter_; }
	inline ConverterOrder<CThostFtdcInputOrderActionField *> & orderActionConverter() { return orderActionConverter_; }

	void inputOrderFieldInit(CThostFtdcInputOrderField *);

	void initMeta(int buyOpenPosition, int sellOpenPosition);

	void setOrderId(int orderId);

	void getOrderId();
    };

#ifdef PRODUCTION
    template<int tickNum, int tickDenum>
    class CFFEX : public CTPMatchyApi
    {
    private:
	static constexpr double tickSize_ = tickNum * 1.0 / tickDenum;

	ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 1, tickNum, tickDenum> messageConverter_;

    public:
    	CFFEX() = delete;
	
    	CFFEX(CFFEX const &) = delete;

    	CFFEX & operator=(CFFEX const &) = delete;

    	CFFEX(string contractId, SimulatorCore * simulatorCorePtr)
	    : CTPMatchyApi("CFFEX", contractId, 0, 0, simulatorCorePtr)
	{}

    	CFFEX(string exchangeId, string contractId, int buyOpenPosition, int sellOpenPosition, SimulatorCore * simulatorCorePtr)
	    : CTPMatchyApi(exchangeId, contractId, buyOpenPosition, sellOpenPosition, simulatorCorePtr)
	{}

	inline ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 1, tickNum, tickDenum> & messageConverter() { return messageConverter_; }

	inline double getTickSize() const { return tickSize_; }

    	virtual ~CFFEX() {}

#ifdef DEBUG
	void printOpenPositionOrderQty()
	{
	    cerr << "Ticker=" << getTicker() << endl 
		 << "Timestamp=" << SimulatorCore::convertTimePointToString(simulatorCorePtr_->getCurrTime()) << endl 
		 << "BuyOpenPosition=" << meta_.buyOpenQty_ << endl 
		 << "SellOpenPosition=" << meta_.sellOpenQty_ << endl 
		 << "BuyOpenOrderQty=" << meta_.buyOpenOrderQty_ << endl 
		 << "BuyCloseOrderQty=" << meta_.buyCloseOrderQty_ << endl 
		 << "SellOpenOrderQty=" << meta_.sellOpenOrderQty_ << endl 
		 << "SellCloseOrderQty=" << meta_.sellCloseOrderQty_ << endl;
	}
#endif

	// Update buy/sell open/close order qty by irreversible CFFEX message.
	void updateBuySellOpenCloseOrder(CThostFtdcOrderField * ptr, bool increaseQ)
	{
	    int side = increaseQ ? 1 : -1;
	    if ((ptr->Direction == THOST_FTDC_D_Buy) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Open)) {
		meta_.buyOpenOrderQty_ += (ptr->VolumeTotal * side);
	    } else if ((ptr->Direction == THOST_FTDC_D_Buy) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Close)) {
		meta_.buyCloseOrderQty_ += (ptr->VolumeTotal * side);
	    } else if ((ptr->Direction == THOST_FTDC_D_Sell) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Open)) {
		meta_.sellOpenOrderQty_ += (ptr->VolumeTotal * side);
	    } else if ((ptr->Direction == THOST_FTDC_D_Sell) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Close)) {
		meta_.sellCloseOrderQty_ += (ptr->VolumeTotal * side);
	    }
#ifdef DEBUG
	    cerr << "CFFEX::updateBuySellOpenCloseOrder - enter" << endl;
	    printOpenPositionOrderQty();
	    cerr << "CFFEX::updateBuySellOpenCloseOrder - exit" << endl;
#endif
	}

	// Decrement buy/sell open/close order qty by trade message.
	void updateBuySellOpenCloseOrder(CThostFtdcOrderField * ptr, int qty)
	{
	    if ((ptr->Direction == THOST_FTDC_D_Buy) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Open)) {
		meta_.buyOpenOrderQty_ -= qty;
	    } else if ((ptr->Direction == THOST_FTDC_D_Buy) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Close)) {
		meta_.buyCloseOrderQty_ -= qty;
	    } else if ((ptr->Direction == THOST_FTDC_D_Sell) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Open)) {
		meta_.sellOpenOrderQty_ -= qty;
	    } else if ((ptr->Direction == THOST_FTDC_D_Sell) && (ptr->CombOffsetFlag[0] == THOST_FTDC_OF_Close)) {
		meta_.sellCloseOrderQty_ -= qty;
	    }
#ifdef DEBUG
	    cerr << "CFFEX::updateBuySellOpenCloseOrder - enter" << endl;
	    printOpenPositionOrderQty();
	    cerr << "CFFEX::updateBuySellOpenCloseOrder - exit" << endl;
#endif
	}

	// Update buy/sell open position by trade message.
	void updateBuySellOpenPosition(CThostFtdcTradeField * pTrade, CThostFtdcOrderField * pOrder)
	{
	    if ((pTrade->Direction == THOST_FTDC_D_Buy) && (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open)) {
		meta_.buyOpenQty_ += pTrade->Volume;
	    } else if ((pTrade->Direction == THOST_FTDC_D_Buy) && (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close)) {
		meta_.sellOpenQty_ -= pTrade->Volume;
	    } else if ((pTrade->Direction == THOST_FTDC_D_Sell) && (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open)) {
		meta_.sellOpenQty_ += pTrade->Volume;
	    } else if ((pTrade->Direction == THOST_FTDC_D_Sell) && (pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close)) {
		meta_.buyOpenQty_ -= pTrade->Volume;
	    }
#ifdef DEBUG
	    cerr << "CFFEX::updateBuySellOpenPosition - enter" << endl;
	    printOpenPositionOrderQty();
	    cerr << "CFFEX::updateBuySellOpenPosition - exit" << endl;
#endif
	}

	TThostFtdcOffsetFlagType calcCombOffsetFlag(BuySellSide side, int qty)
	{
	    if ((side == BuySellSide::buy && meta_.sellOpenQty_ >= (meta_.buyCloseOrderQty_ + qty)) || 
		(side == BuySellSide::sell && meta_.buyOpenQty_ >= (meta_.sellCloseOrderQty_ + qty))) {
#ifdef EXCHANGE_CFFEX
		return THOST_FTDC_OF_Close;
#elif defined EXCHANGE_SHFE
		return THOST_FTDC_OF_CloseToday;
#endif
	    }
	    return THOST_FTDC_OF_Open;
	}

	/* Push CTP market data to CFFEX. */
	void push(CThostFtdcDepthMarketDataField * pData)
	{
	    // If simulator core hasn't set up time offset, do not push the data at all.
	    if (!(simulatorCorePtr_->timeOffsetQ()))
		return;

	    lock_guard<mutex> hold(simulatorCorePtr_->lock_);

	    BookUpdate<true> book;
	    TradeUpdate<true> trade;
	    // Add time offset here.
	    messageConverter_.convert(pData, &book, &trade, simulatorCorePtr_->timeOffset());
	    {
		bookUpdates_.push_back(book);
		tradeUpdates_.push_back(trade);
	    }

	    // Trigger strategy flags here.
	    int i = 0;
	    for (auto & pStrat : simulatorCorePtr_->getStrategies()) { 
		int x = 0;
		auto tickers = pStrat->getTickers();
		for (auto it = tickers.begin();
		     it != tickers.end();
		     ++it) {
		    if ((*it) == book.getTicker()) {
			++x;
		    }
		}
		if (x > 0) {
		    simulatorCorePtr_->setStratUpdTrigger(i, true);
		}
		++i;
	    }
	    getOrderId();
	    simulatorCorePtr_->next();
	}
	
	/* This will be called when order are rejected from OnRspOrderInsert. */
	void push(CThostFtdcInputOrderField * pInput)
	{
	    lock_guard<mutex> hold(simulatorCorePtr_->lock_);

	    OrderCacheType::iterator it;
	    OrderInputCacheType::iterator its;
	    if (pInput->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		RejectedOrderMessage<LimitOrderMessage> rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		messageConverter_.convert(pInput, &rom);
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		rejectedLimitOrderMessages().push_back(rom);
		if (!orderInputCache_.find(atoi(pInput->OrderRef), its) && 
		    orderCache_.find(atoi(pInput->OrderRef), it)) {
		    updateBuySellOpenCloseOrder(&(it->second), false);
		}
	    } else if (pInput->OrderPriceType == THOST_FTDC_OPT_AnyPrice) {
		RejectedOrderMessage<MarketOrderMessage> rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		messageConverter_.convert(pInput, &rom);
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		rejectedMarketOrderMessages().push_back(rom);
		if (!orderInputCache_.find(atoi(pInput->OrderRef), its) && 
		    orderCache_.find(atoi(pInput->OrderRef), it)) {
		    updateBuySellOpenCloseOrder(&(it->second), false);
		}
	    }
	    orderInputCache_.insert(pInput);
	    getOrderId();
	    simulatorCorePtr_->next();
	}

	/* reject message from OnRspOrderAction */
	void push(CThostFtdcInputOrderActionField * pAction)
	{
	    lock_guard<mutex> hold(simulatorCorePtr_->lock_);

	    OrderCacheType::iterator it;
	    OrderInputActionCacheType::iterator its;
	    if (orderCache_.find(atoi(pAction->OrderRef), it)) {
		if ((it->second).OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		    RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > rom;
		    rom.timestamp() = simulatorCorePtr_->getCurrTime();
		    rom.exchangeId() = exchangeId_;
		    rom.contractId() = contractId_;
		    rom.qty() = (it->second).VolumeTotal;
		    rom.side() = ((it->second).Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
		    rom.px() = Px((it->second).LimitPrice, tickSize_);
		    rom.messageId() = messageId_++;
		    messageConverter_.convert(pAction, &rom);
		    rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		    simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		    rejectedCancelLimitOrderMessages().push_back(rom);
		    if (!orderInputActionCache_.find(atoi(pAction->OrderRef), its) && 
			orderCache_.cancelOrderQ(&(it->second)) && 
			!orderCache_.canceledOrderQ(&(it->second))) {
			updateBuySellOpenCloseOrder(&(it->second), true);
			orderCache_.resetActiveUserID(&(it->second));
		    }
		} else if ((it->second).OrderPriceType == THOST_FTDC_OPT_AnyPrice) {
		    RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > rom;
		    rom.timestamp() = simulatorCorePtr_->getCurrTime();
		    rom.exchangeId() = exchangeId_;
		    rom.contractId() = contractId_;
		    rom.qty() = (it->second).VolumeTotal;
		    rom.side() = ((it->second).Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
		    rom.messageId() = messageId_++;
		    messageConverter_.convert(pAction, &rom);
		    rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		    simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		    rejectedCancelMarketOrderMessages().push_back(rom);
		    if (!orderInputActionCache_.find(atoi(pAction->OrderRef), its) && 
			orderCache_.cancelOrderQ(&(it->second)) && 
			!orderCache_.canceledOrderQ(&(it->second))) {
			updateBuySellOpenCloseOrder(&(it->second), true);
			orderCache_.resetActiveUserID(&(it->second));
		    }
		}
	    } else {
		cout << "CFFEX::push(CThostFtdcInputOrderActionField *) - no order found in order cache!! this should not happen!!" << endl;
	    }
	    orderInputActionCache_.insert(pAction);
	    getOrderId();
	    simulatorCorePtr_->next();
	}
	
	/* reject message from OnErrRtnOrderAction */
	void push(CThostFtdcOrderActionField * pAction)
	{
	    lock_guard<mutex> hold(simulatorCorePtr_->lock_);

	    OrderCacheType::iterator it;
	    OrderActionCacheType::iterator its;
	    if (orderCache_.find(atoi(pAction->OrderRef), it)) {
		if ((it->second).OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		    RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > rom;
		    rom.timestamp() = simulatorCorePtr_->getCurrTime();
		    rom.exchangeId() = exchangeId_;
		    rom.contractId() = contractId_;
		    rom.qty() = (it->second).VolumeTotal;
		    rom.side() = ((it->second).Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
		    rom.px() = Px((it->second).LimitPrice, tickSize_);
		    rom.messageId() = messageId_++;
		    messageConverter_.convert(pAction, &rom);
		    rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		    simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		    rejectedCancelLimitOrderMessages().push_back(rom);
		    if (!orderActionCache_.find(atoi(pAction->OrderRef), its) && 
			orderCache_.cancelOrderQ(&(it->second)) && 
			!orderCache_.canceledOrderQ(&(it->second))) {
			updateBuySellOpenCloseOrder(&(it->second), true);
			orderCache_.resetActiveUserID(&(it->second));
		    }
		} else if((it->second).OrderPriceType == THOST_FTDC_OPT_AnyPrice) {
		    RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > rom;
		    rom.timestamp() = simulatorCorePtr_->getCurrTime();
		    rom.exchangeId() = exchangeId_;
		    rom.contractId() = contractId_;
		    rom.qty() = (it->second).VolumeTotal;
		    rom.side() = ((it->second).Direction == THOST_FTDC_D_Buy) ? BuySellSide::buy : BuySellSide::sell;
		    rom.messageId() = messageId_++;
		    messageConverter_.convert(pAction, &rom);
		    rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		    simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		    rejectedCancelMarketOrderMessages().push_back(rom);
		    if (!orderActionCache_.find(atoi(pAction->OrderRef), its) && 
			orderCache_.cancelOrderQ(&(it->second)) && 
			!orderCache_.canceledOrderQ(&(it->second))) {
			updateBuySellOpenCloseOrder(&(it->second), true);
			orderCache_.resetActiveUserID(&(it->second));
		    }
		}
	    } else {
		cout << "CFFEX::push(CThostFtdcOrderActionField *) - no order found in order cache!! this should not happen!!" << endl;
	    }
	    orderActionCache_.insert(pAction);
	    getOrderId();
	    simulatorCorePtr_->next();
	}

	/* Push CTP order messages to CFFEX. */
	void push(CThostFtdcOrderField * pOrder)
	{
	    lock_guard<mutex> hold(simulatorCorePtr_->lock_);

	    OrderCacheType::iterator it;
	    /* HEADSUP */
	    // The critical assumption here is that CFFEX or API ALWAYS sends this acknowledgement FIRST,
	    // otherwise we will miss the acknowledgement and CombOffsetFlag will be messed up afterwards.
	    // We still check if this raw acknowledgement comes first and cout message if not.
	    // !AcknowledgedOrderMessage.acknowledgeCancelQ && LimitOrder
	    if (pOrder->OrderPriceType == THOST_FTDC_OPT_LimitPrice && 
		pOrder->OrderSysID[0] == '\0' && 
		pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted &&
		pOrder->OrderStatus == THOST_FTDC_OST_Unknown && 
		pOrder->VolumeTraded == 0 &&
		(!orderCache_.cancelOrderQ(pOrder))) {
		if (!orderCache_.find(atoi(pOrder->OrderRef), it)) {
		    AcknowledgedOrderMessage aom;
		    aom.timestamp() = simulatorCorePtr_->getCurrTime();
		    aom.exchangeId() = exchangeId_;
		    aom.contractId() = contractId_;
		    aom.messageId() = messageId_++;
		    messageConverter_.convert(pOrder, &aom);
		    aom.stratId() = (aom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		    aom.acknowledgeCancelQ() = false;
		    simulatorCorePtr_->setStratUpdTrigger(aom.stratId(), true);
		    acknowledgedOrderMessages().push_back(aom);
		    updateBuySellOpenCloseOrder(pOrder, true);
		} else {
		    cout << "CFFEX::push(CThostFtdcOrderField *) - limit order acknowledgement message comes late!! this should not happen!!" << endl;
		}
	    }
	    // !AcknowledgedOrderMessage.acknowledgeCancelQ && MarketOrder
	    else if (pOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice && 
		     pOrder->OrderSysID[0] == '\0' && 
		     pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted &&
		     pOrder->OrderStatus == THOST_FTDC_OST_Unknown && 
		     pOrder->VolumeTraded == 0 && 
		     (!orderCache_.cancelOrderQ(pOrder))) {
		if (!orderCache_.find(atoi(pOrder->OrderRef), it)) {
		    AcknowledgedOrderMessage aom;
		    aom.timestamp() = simulatorCorePtr_->getCurrTime();
		    aom.exchangeId() = exchangeId_;
		    aom.contractId() = contractId_;
		    aom.messageId() = messageId_++;
		    messageConverter_.convert(pOrder, &aom);
		    aom.stratId() = (aom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		    aom.acknowledgeCancelQ() = false;
		    simulatorCorePtr_->setStratUpdTrigger(aom.stratId(), true);
		    acknowledgedOrderMessages().push_back(aom);
		    updateBuySellOpenCloseOrder(pOrder, true);
		} else {
		    cout << "CFFEX::push(CThostFtdcOrderField *) - market order acknowledgement message comes late!! this should not happen!!" << endl;
		}
	    }
	    // CanceledOrderMessage<LimitOrderMessage>
	    else if (//pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted &&
		     pOrder->OrderStatus == THOST_FTDC_OST_Canceled &&
		     pOrder->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		CanceledOrderMessage<LimitOrderMessage> com;
		com.timestamp() = simulatorCorePtr_->getCurrTime();
		com.exchangeId() = exchangeId_;
		com.contractId() = contractId_;
		com.messageId() = messageId_++;
		messageConverter_.convert(pOrder, &com);
		com.stratId() = (com.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(com.stratId(), true);
		canceledLimitOrderMessages().push_back(com);
		if (orderCache_.find(atoi(pOrder->OrderRef), it)) {
		    if (!orderCache_.cancelOrderQ(&(it->second))) {
			updateBuySellOpenCloseOrder(pOrder, false);
		    }
		    // orderCache_.erase(atoi(pOrder->OrderRef));
		}
	    }
	    // CanceledOrderMessage<MarketOrderMessage>
	    else if (//pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted &&
		     pOrder->OrderStatus == THOST_FTDC_OST_Canceled &&
		     pOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice) {
		CanceledOrderMessage<MarketOrderMessage> com;
		com.timestamp() = simulatorCorePtr_->getCurrTime();
		com.exchangeId() = exchangeId_;
		com.contractId() = contractId_;
		com.messageId() = messageId_++;
		messageConverter_.convert(pOrder, &com);
		com.stratId() = (com.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(com.stratId(), true);
		canceledMarketOrderMessages().push_back(com);
		if (orderCache_.find(atoi(pOrder->OrderRef), it)) {
		    if (!orderCache_.cancelOrderQ(&(it->second))) {
			updateBuySellOpenCloseOrder(pOrder, false);
		    }
		    // orderCache_.erase(atoi(pOrder->OrderRef));
		}
	    }
	    // RejectedOrderMessage<LimitOrderMessage>
	    else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected && 
		     pOrder->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		RejectedOrderMessage<LimitOrderMessage> rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		messageConverter_.convert(pOrder, &rom);
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		rejectedLimitOrderMessages().push_back(rom);
		if (orderCache_.find(atoi(pOrder->OrderRef), it)) {
		    updateBuySellOpenCloseOrder(pOrder, false);
		    // orderCache_.erase(atoi(pOrder->OrderRef));
		}			    
	    }
	    // RejectedOrderMessage<MarketOrderMessage>
	    else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected && 
		     pOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice) {
		RejectedOrderMessage<MarketOrderMessage> rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		messageConverter_.convert(pOrder, &rom);
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		rejectedMarketOrderMessages().push_back(rom);
		if (orderCache_.find(atoi(pOrder->OrderRef), it)) {
		    updateBuySellOpenCloseOrder(pOrder, false);
		    // orderCache_.erase(atoi(pOrder->OrderRef));
		}			    
	    }
	    // RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage>>
	    else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelRejected && 
		     pOrder->OrderPriceType == THOST_FTDC_OPT_LimitPrice) {
		RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		messageConverter_.convert(pOrder, &rom);
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		rejectedCancelLimitOrderMessages().push_back(rom);
		if (orderCache_.find(atoi(pOrder->OrderRef), it) && 
		    orderCache_.cancelOrderQ(&(it->second)) && 
		    !orderCache_.canceledOrderQ(&(it->second))) {
		    updateBuySellOpenCloseOrder(pOrder, true);
		    orderCache_.resetActiveUserID(&(it->second));
		}
	    }
	    // RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage>>
	    else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelRejected && 
		     pOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice) {
		RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		messageConverter_.convert(pOrder, &rom);
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		simulatorCorePtr_->setStratUpdTrigger(rom.stratId(), true);
		rejectedCancelMarketOrderMessages().push_back(rom);
		if (orderCache_.find(atoi(pOrder->OrderRef), it) && 
		    orderCache_.cancelOrderQ(&(it->second)) && 
		    !orderCache_.canceledOrderQ(&(it->second))) {
		    updateBuySellOpenCloseOrder(pOrder, true);
		    orderCache_.resetActiveUserID(&(it->second));
		}			    
	    }
	    // AcknowledgedOrderMessage.acknowledgeCancelQ
	    // Another edge case: OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted && OrderStatus == THOST_FTDC_OST_Unknown, the OrderSubmitStatus never flips to THOST_FTDC_OSS_Accepted once but got filled.
	    // Too many flags could change between, the hack is as follows:
	    // if the order return message does not fall into any of the above categories, and if ActiveUserID is set,
	    // then it is an acknowledged cancel order message
	    /*
	    else if (((pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted && 
	    	       (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing || pOrder->OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)) ||
	    	      (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted && 
	    	       pOrder->OrderStatus == THOST_FTDC_OST_Unknown)) && 
	    	     orderCache_.cancelOrderQ(pOrder)) {
	    */
	    else if (orderCache_.cancelOrderQ(pOrder)) {
		bool findOrderQ = orderCache_.find(atoi(pOrder->OrderRef), it);
		if (!findOrderQ || !orderCache_.cancelOrderQ(&(it->second))) {
		    AcknowledgedOrderMessage aom;
		    aom.timestamp() = simulatorCorePtr_->getCurrTime();
		    aom.exchangeId() = exchangeId_;
		    aom.contractId() = contractId_;
		    aom.messageId() = messageId_++;
		    messageConverter_.convert(pOrder, &aom);
		    aom.stratId() = (aom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		    aom.acknowledgeCancelQ() = true;
		    simulatorCorePtr_->setStratUpdTrigger(aom.stratId(), true);
		    acknowledgedOrderMessages().push_back(aom);
		    updateBuySellOpenCloseOrder(pOrder, false);
		}
	    }
	    // Other order status change
	    else {
		// in case VolumeTotal get changes
		orderCache_.insert(pOrder);
		return;
	    }
	    orderCache_.insert(pOrder);
	    getOrderId();
	    simulatorCorePtr_->next();
	}

	/* Push CTP trade messages to CFFEX. */
	void push(CThostFtdcTradeField * pTrade)
	{
	    lock_guard<mutex> hold(simulatorCorePtr_->lock_);

	    FilledOrderMessage fom;
	    OrderCacheType::iterator it;
	    fom.timestamp() = simulatorCorePtr_->getCurrTime();
	    fom.exchangeId() = exchangeId_;
	    fom.contractId() = contractId_;
	    fom.messageId() = messageId_++;
	    messageConverter_.convert(pTrade, &fom, &orderCache_);
	    fom.stratId() = (fom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
	    simulatorCorePtr_->setStratUpdTrigger(fom.stratId(), true);
	    filledOrderMessages().push_back(fom);
	    /* ATTN */
	    // We have to reply on the assumption that CFFEX will send acknowledge message before any trade message.
	    // This is important since otherwise our open/close order/qty will be messed up.
	    if (orderCache_.find(atoi(pTrade->OrderRef), it)) {
		updateBuySellOpenPosition(pTrade, &(it->second));
		if (!orderCache_.cancelOrderQ(&(it->second))) {
		    // in the case when cancel is acked but we still got a fill
		    // only need to update OpenPosition, no need to do opencloseorder
		    updateBuySellOpenCloseOrder(&(it->second), fom.getQty());
		}
		// if ((it->second).VolumeTotal == 0) 
		//     orderCache_.erase(atoi(pTrade->OrderRef));
	    }
	    getOrderId();
	    simulatorCorePtr_->next();
	    tradedVolume() += fom.getQty();
	}

	// template<typename MarketOrLimitOrderMessageType>
	// vector<MarketOrLimitOrderMessageType> filterOrderMessagesByOrderId(list<MarketOrLimitOrderMessageType> orderMessages)
	// {
	//     vector<MarketOrLimitOrderMessageType> om_in;
	//     vector<MarketOrLimitOrderMessageType> om_out;
	//     partition_copy(orderMessages.begin(),
	// 		   orderMessages.end(),
	// 		   back_inserter(om_in),
	// 		   back_inserter(om_out),
	// 		   [&](MarketOrLimitOrderMessageType m) { return m.getExchangeId() == exchangeId_ && m.getContractId() == contractId_; });
	//     if (!om_in.empty()) {
	// 	sort(om_in.begin(),
	// 	     om_in.end(),
	// 	     [](MarketOrLimitOrderMessageType l, MarketOrLimitOrderMessageType r) { return l.getOrderId() > r.getOrderId(); });
	//     }
	//     return om_in;
	// }

	virtual bool processLimitOrder(LimitOrderMessage & m) 
	{
	    if (m.getExchangeId() != exchangeId_ || m.getContractId() != contractId_) {
		return false;
	    }

	    CThostFtdcInputOrderField order;
	    inputOrderFieldInit(&order);
	    if (isPriceValid(m.getPx().getPx())) {
		BuySellSide side = orderInsertConverter_.convertLimitOrder(&m, 0, &order);
		// Derive CombOffsetFlag
		order.CombOffsetFlag[0] = calcCombOffsetFlag(side, m.getQty());
		if (actionFlag_ == 3) { 
		    while (traderApi()->ReqOrderInsert(&order, 0)) {}
		} else if (actionFlag_ == 2) { 
		    cout << "Submit Limit Order" << endl;
		    cout << convertStructToString(&order, "\n") << endl;
		}
	    }
	    else {
		rejectedLimitOrderMessages().push_back(RejectedOrderMessage<LimitOrderMessage>(exchangeId_, contractId_, simulatorCorePtr_->getCurrTime(), m.getStratId(), messageId_++, m.getOrderId(), m.getQty(), m.getSide(), m.getPx()));
	    }

	    return true;
	}

	virtual bool processMarketOrder(MarketOrderMessage & m) 
	{
	    if (m.getExchangeId() != exchangeId_ || m.getContractId() != contractId_) {
		return false;
	    }

	    CThostFtdcInputOrderField order;
	    inputOrderFieldInit(&order);
	    BuySellSide side = orderInsertConverter_.convertMarketOrder(&m, 0, &order);
	    // Derive CombOffsetFlag
	    order.CombOffsetFlag[0] = calcCombOffsetFlag(side, m.getQty());
	    if (actionFlag_ == 3) { 
		while (traderApi()->ReqOrderInsert(&order, 0)) {}
	    } else if(actionFlag_ == 2) { 
		cout << "Submit Market Order" << endl;
		cout << convertStructToString(&order, "\n") << endl;
	    }

	    return true;
	}

	// Cancel limit orders.
	virtual bool processCancelLimitOrder(CancelOrderMessage<LimitOrderMessage> & m) 
	{
	    if (m.getExchangeId() != exchangeId_ || m.getContractId() != contractId_) {
		return false;
	    }

	    CThostFtdcInputOrderActionField order;
	    bool findOrderQ;
	    findOrderQ = orderActionConverter_.convertCancelOrder(&m, 0, &order, &orderCache_);
	    // Need OrderSysID to cancel an order,
	    // otherwise CFFEX might not send any feedback 
	    // if CThostFtdcInputOrderActionField is sent without OrderSysID,
	    // then we have a pending cancel limit order which will never go away.
	    if (findOrderQ && order.OrderSysID != '\0') {
		if (actionFlag_ == 3) { 
		    while (traderApi()->ReqOrderAction(&order, 0)) {}
		} else if (actionFlag_ == 2) { 
		    cout << "Cancel Limit Order" << endl;
		    cout << convertStructToString(&order, "\n") << endl;
		}
	    }
	    else {
		RejectedOrderMessage<CancelOrderMessage<LimitOrderMessage> > rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		rom.orderId() = m.getOrderId();
		rom.qty() = m.getQty();
		rom.side() = m.getSide();
		rom.px() = m.getPx();
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		rejectedCancelLimitOrderMessages().push_back(rom);
	    }

	    return true;
	}

	// Cancel market orders.
	virtual bool processCancelMarketOrder(CancelOrderMessage<MarketOrderMessage> & m) 
	{
	    if (m.getExchangeId() != exchangeId_ || m.getContractId() != contractId_) {
		return false;
	    }

	    CThostFtdcInputOrderActionField order;
	    bool findOrderQ;
	    findOrderQ = orderActionConverter_.convertCancelOrder(&m, 0, &order, &orderCache_);
	    if (findOrderQ) {
		if (actionFlag_ == 3) { 
		    while (traderApi()->ReqOrderAction(&order, 0)) {}
		} else if (actionFlag_ == 2) { 
		    cout << "Cancel Market Order" << endl;
		    cout << convertStructToString(&order, "\n") << endl;
		}
	    }
	    else {
		RejectedOrderMessage<CancelOrderMessage<MarketOrderMessage> > rom;
		rom.timestamp() = simulatorCorePtr_->getCurrTime();
		rom.exchangeId() = exchangeId_;
		rom.contractId() = contractId_;
		rom.messageId() = messageId_++;
		rom.orderId() = m.getOrderId();
		rom.qty() = m.getQty();
		rom.side() = m.getSide();
		rom.stratId() = (rom.getOrderId()) % (simulatorCorePtr_->getNumStrategies());
		rejectedCancelMarketOrderMessages().push_back(rom);
	    }

	    return true;
	}

    };

    template<int tickNum, int tickDenum>
    using CTPEXCH = CFFEX<tickNum, tickDenum>;

#endif

}

#endif  // CFFEX_HPP
