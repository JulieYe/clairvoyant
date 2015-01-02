//
// File:        $ test_message.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/10 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_message
#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <vector>
#include <string.h>

#include "../Message.hpp"
#include "../SimulatorCore.hpp"

using simulator::Px;
using simulator::AbstractMessage;
using simulator::BuySellSide;
using simulator::AbstractMessage;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::CancelOrderMessage;
using simulator::AcknowledgedOrderMessage;
using simulator::FilledOrderMessage;
using simulator::CanceledOrderMessage;
using simulator::RejectedOrderMessage;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::SimulatorCore;
using simulator::duration_nano;
using simulator::time_stamp;
using simulator::ImpliedSpreadTopBookUpdate;

// according to unit.test.to.be.written.message
// Mostly make sure the constructors works for various message classes.

// 1. AbstractMessage
BOOST_AUTO_TEST_CASE(abstractmessage_constructor_basic_methods)
{
    AbstractMessage mes0();
    AbstractMessage mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10);
    AbstractMessage mes2(mes1);
    AbstractMessage mes3 = mes1;
    AbstractMessage mes4 = AbstractMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10);
    AbstractMessage mes5(AbstractMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
  
}

// 2. MarketOrderMessage inherits from AbstractMessage, with two more data members: qty_ and side_
BOOST_AUTO_TEST_CASE(marketordermessage_constructor_basic_methods)
{
    MarketOrderMessage mes0();
    MarketOrderMessage mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    MarketOrderMessage mes2(mes1);
    MarketOrderMessage mes3 = mes1;
    MarketOrderMessage mes4 = MarketOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    MarketOrderMessage mes5(MarketOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
}

// 3. LimitOrderMessage inherits from MarketOrderMessage, with one more data member: px_
BOOST_AUTO_TEST_CASE(limitordermessage_constructor_basic_methods)
{
    LimitOrderMessage mes0();
    LimitOrderMessage mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    LimitOrderMessage mes2(mes1);
    LimitOrderMessage mes3 = mes1;
    LimitOrderMessage mes4 = LimitOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    LimitOrderMessage mes5(LimitOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1)));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy &&
		mes1.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy &&
		mes2.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy &&
		mes3.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy &&
		mes4.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy &&
		mes5.getPx() == Px(100.3, 0.1));

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
    mes1.px() = Px(200.3, 0.1);
    BOOST_CHECK(mes1.getPx() == Px(200.3, 0.1));
}

// 4. CancelOrderMessage<LimitOrderMessage> inherits from LimitOrderMessage
BOOST_AUTO_TEST_CASE(cancellimitordermessage_constructor_basic_methods)
{
    CancelOrderMessage<LimitOrderMessage> mes0();
    CancelOrderMessage<LimitOrderMessage> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    CancelOrderMessage<LimitOrderMessage> mes2(mes1);
    CancelOrderMessage<LimitOrderMessage> mes3 = mes1;
    CancelOrderMessage<LimitOrderMessage> mes4 = CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    CancelOrderMessage<LimitOrderMessage> mes5(CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1)));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy &&
		mes1.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy &&
		mes2.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy &&
		mes3.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy &&
		mes4.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy &&
		mes5.getPx() == Px(100.3, 0.1));

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
    mes1.px() = Px(200.3, 0.1);
    BOOST_CHECK(mes1.getPx() == Px(200.3, 0.1));
}


// 5. CancelOrderMessage<MarketOrderMessage> inherits from MarketOrderMessage
BOOST_AUTO_TEST_CASE(cancelmarketordermessage_constructor_basic_methods)
{
    CancelOrderMessage<MarketOrderMessage> mes0();
    CancelOrderMessage<MarketOrderMessage> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    CancelOrderMessage<MarketOrderMessage> mes2(mes1);
    CancelOrderMessage<MarketOrderMessage> mes3 = mes1;
    CancelOrderMessage<MarketOrderMessage> mes4 = CancelOrderMessage<MarketOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    CancelOrderMessage<MarketOrderMessage> mes5(CancelOrderMessage<MarketOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
}

// 6. CanceledOrderMessage<LimitOrderMessage> inherits from LimitOrderMessage
BOOST_AUTO_TEST_CASE(canceledlimitordermessage_constructor_basic_methods)
{
    CanceledOrderMessage<LimitOrderMessage> mes0();
    CanceledOrderMessage<LimitOrderMessage> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    CanceledOrderMessage<LimitOrderMessage> mes2(mes1);
    CanceledOrderMessage<LimitOrderMessage> mes3 = mes1;
    CanceledOrderMessage<LimitOrderMessage> mes4 = CanceledOrderMessage<LimitOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    CanceledOrderMessage<LimitOrderMessage> mes5(CanceledOrderMessage<LimitOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1)));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy &&
		mes1.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy &&
		mes2.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy &&
		mes3.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy &&
		mes4.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy &&
		mes5.getPx() == Px(100.3, 0.1));

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
    mes1.px() = Px(200.3, 0.1);
    BOOST_CHECK(mes1.getPx() == Px(200.3, 0.1));
}

// 7. CanceledOrderMessage<MarketOrderMessage> inherits from MarketOrderMessage
BOOST_AUTO_TEST_CASE(canceledmarketordermessage_constructor_basic_methods)
{
    CanceledOrderMessage<MarketOrderMessage> mes0();
    CanceledOrderMessage<MarketOrderMessage> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    CanceledOrderMessage<MarketOrderMessage> mes2(mes1);
    CanceledOrderMessage<MarketOrderMessage> mes3 = mes1;
    CanceledOrderMessage<MarketOrderMessage> mes4 = CanceledOrderMessage<MarketOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    CanceledOrderMessage<MarketOrderMessage> mes5(CanceledOrderMessage<MarketOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
}

// 8. FilledOrderMessage inherits from LimitOrderMessage
BOOST_AUTO_TEST_CASE(filledordermessage_constructor_basic_methods)
{
    FilledOrderMessage mes0();
    FilledOrderMessage mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    FilledOrderMessage mes2(mes1);
    FilledOrderMessage mes3 = mes1;
    FilledOrderMessage mes4 = FilledOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    FilledOrderMessage mes5(FilledOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1)));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy &&
		mes1.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy &&
		mes2.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy &&
		mes3.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy &&
		mes4.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy &&
		mes5.getPx() == Px(100.3, 0.1));

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
    mes1.px() = Px(200.3, 0.1);
    BOOST_CHECK(mes1.getPx() == Px(200.3, 0.1));
}

// 9. AcknowledgedOrderMessage inherits from AbstractMessage
BOOST_AUTO_TEST_CASE(acknowledgeordermessage_constructor_basic_methods)
{
    AcknowledgedOrderMessage mes0();
    AcknowledgedOrderMessage mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, false);
    AcknowledgedOrderMessage mes2(mes1);
    AcknowledgedOrderMessage mes3 = mes1;
    AcknowledgedOrderMessage mes4 = AcknowledgedOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, false);
    AcknowledgedOrderMessage mes5(AcknowledgedOrderMessage("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, true));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getAcknowledgeCancelQ() == false);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10 &&
		mes2.getAcknowledgeCancelQ() == false);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10 &&
		mes3.getAcknowledgeCancelQ() == false);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10 &&
		mes4.getAcknowledgeCancelQ() == false);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10 &&
		mes5.getAcknowledgeCancelQ() == true);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.acknowledgeCancelQ() = true;
    BOOST_CHECK(mes1.getAcknowledgeCancelQ());
}

// 10. RejectedOrderMessage<LimitOrderMessage> inherits from LimitOrderMessage
BOOST_AUTO_TEST_CASE(rejectedlimitordermessage_constructor_basic_methods)
{
    RejectedOrderMessage<LimitOrderMessage> mes0();
    RejectedOrderMessage<LimitOrderMessage> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    RejectedOrderMessage<LimitOrderMessage> mes2(mes1);
    RejectedOrderMessage<LimitOrderMessage> mes3 = mes1;
    RejectedOrderMessage<LimitOrderMessage> mes4 = RejectedOrderMessage<LimitOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1));
    RejectedOrderMessage<LimitOrderMessage> mes5(RejectedOrderMessage<LimitOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy, Px(100.3, 0.1)));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy &&
		mes1.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy &&
		mes2.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy &&
		mes3.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy &&
		mes4.getPx() == Px(100.3, 0.1));
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy &&
		mes5.getPx() == Px(100.3, 0.1));

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
    mes1.px() = Px(200.3, 0.1);
    BOOST_CHECK(mes1.getPx() == Px(200.3, 0.1));
}

// 11. RejectedOrderMessage<MarketOrderMessage> inherits from MarketOrderMessage
BOOST_AUTO_TEST_CASE(rejectedmarketordermessage_constructor_basic_methods)
{
    RejectedOrderMessage<MarketOrderMessage> mes0();
    RejectedOrderMessage<MarketOrderMessage> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    RejectedOrderMessage<MarketOrderMessage> mes2(mes1);
    RejectedOrderMessage<MarketOrderMessage> mes3 = mes1;
    RejectedOrderMessage<MarketOrderMessage> mes4 = RejectedOrderMessage<MarketOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy);
    RejectedOrderMessage<MarketOrderMessage> mes5(RejectedOrderMessage<MarketOrderMessage>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 1, 100, 10, 101, simulator::BuySellSide::buy));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getStratId() == 1 &&
		mes1.getMessageId() == 100 &&
		mes1.getOrderId() == 10 &&
		mes1.getQty() == 101 &&
		mes1.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getStratId() == 1 &&
		mes2.getMessageId() == 100 &&
		mes2.getOrderId() == 10&&
		mes2.getQty() == 101 &&
		mes2.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getStratId() == 1 &&
		mes3.getMessageId() == 100 &&
		mes3.getOrderId() == 10&&
		mes3.getQty() == 101 &&
		mes3.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getStratId() == 1 &&
		mes4.getMessageId() == 100 &&
		mes4.getOrderId() == 10&&
		mes4.getQty() == 101 &&
		mes4.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getStratId() == 1 &&
		mes5.getMessageId() == 100 &&
		mes5.getOrderId() == 10&&
		mes5.getQty() == 101 &&
		mes5.getSide() == simulator::BuySellSide::buy);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.stratId() = 100;
    BOOST_CHECK(mes1.getStratId() == 100);
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.orderId() = 5500;
    BOOST_CHECK(mes1.getOrderId() == 5500);
    mes1.qty() = 202;
    BOOST_CHECK(mes1.getQty() == 202);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
    // strcpy(mes1.exchangeIdPtr(), "ThisExchange");
    // BOOST_CHECK(strcmp(mes1.exchangeIdPtr(), "ThisExchange")==0);
    // strcpy(mes1.orderSysIdPtr(), "ThisOrderId");
    // BOOST_CHECK(strcmp(mes1.orderSysIdPtr(), "ThisOrderId")==0);
}

// 12. BookUpdate<isSnapshotQ = true> inherits from AbstractMessage, with four more data members: list of bid px's, list of ask px's, list of bid qty's, list of ask qty's, test getMidPx method
BOOST_AUTO_TEST_CASE(bookupdate_snapshot_constructor_basic_methods)
{
    std::vector<Px> bid;
    std::vector<Px> ask;
    std::vector<int> bidsize;
    std::vector<int> asksize;
    for(int ii=0; ii< 4; ii++) {
	bid.push_back(Px(100.0-ii*0.1, 0.1));
	ask.push_back(Px(100.2+ii*0.1, 0.1));
	bidsize.push_back(std::rand() % 100 + 1);
	asksize.push_back(std::rand() % 100 + 1);
    }
  
    double midprice;
  
    BookUpdate<true> mes0("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100);
    BookUpdate<true> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100, bid, bidsize, ask, asksize);
    BookUpdate<true> mes2(mes1);
    BookUpdate<true> mes3 = mes1;
    BookUpdate<true> mes4 = BookUpdate<true>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100, bid, bidsize, ask, asksize);
    BookUpdate<true> mes5("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100, bid, bidsize, ask, asksize);

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getMessageId() == 100 &&
		mes1.getMidPx(&midprice) &&
		std::abs(midprice -  100.1) < 1.0e-5);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getMessageId() == 100 &&
		mes2.getMidPx(&midprice) &&
		std::abs(midprice -  100.1) < 1.0e-5);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getMessageId() == 100 &&
		mes3.getMidPx(&midprice) &&
		std::abs(midprice -  100.1) < 1.0e-5);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getMessageId() == 100 &&
		mes4.getMidPx(&midprice) &&
		std::abs(midprice -  100.1) < 1.0e-5);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getMessageId() == 100 &&
		mes5.getMidPx(&midprice) &&
		std::abs(midprice -  100.1) < 1.0e-5);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.bidPx()[0] = Px(100.1, 0.1);
    BOOST_CHECK(mes1.getMidPx(&midprice) && std::abs(midprice - 100.15) < 1.0e-5);
    mes1.askQty()[2] = 10001;
    BOOST_CHECK(mes1.getAskQty()[2] == 10001);
    mes1.bidPx().clear();
    BOOST_CHECK(!mes1.getMidPx(&midprice));
}

// 13. TradeUpdate<isSnapshotQ = true> inherits from AbstractMessage, with three more data members: px_, qty_, side_
BOOST_AUTO_TEST_CASE(tradeupdate_snapshot_constructor_basic_methods)
{
    TradeUpdate<true> mes0("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100);
    TradeUpdate<true> mes1("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100, Px(100.1, 0.1), 10, simulator::BuySellSide::buy);
    TradeUpdate<true> mes2(mes1);
    TradeUpdate<true> mes3 = mes1;
    TradeUpdate<true> mes4 = TradeUpdate<true>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100, Px(100.1, 0.1), 10, simulator::BuySellSide::buy);
    TradeUpdate<true> mes5(TradeUpdate<true>("CFFEX", "IF12", SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250"), 100, Px(100.1, 0.1), 10, simulator::BuySellSide::buy));

    BOOST_CHECK(mes1.getExchangeId() == "CFFEX" &&
		mes1.getContractId() == "IF12" &&
		mes1.getTicker() == "CFFEX:IF12" &&
		mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes1.getMessageId() == 100 &&
		mes1.getPx() == Px(100.1, 0.1) &&
		mes1.getQty() == 10 &&
		mes1.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes2.getExchangeId() == "CFFEX" &&
		mes2.getContractId() == "IF12" &&
		mes2.getTicker() == "CFFEX:IF12" &&
		mes2.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes2.getMessageId() == 100 &&
		mes2.getPx() == Px(100.1, 0.1) &&
		mes2.getQty() == 10 &&
		mes2.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes3.getExchangeId() == "CFFEX" &&
		mes3.getContractId() == "IF12" &&
		mes3.getTicker() == "CFFEX:IF12" &&
		mes3.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes3.getMessageId() == 100 &&
		mes3.getPx() == Px(100.1, 0.1) &&
		mes3.getQty() == 10 &&
		mes3.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes4.getExchangeId() == "CFFEX" &&
		mes4.getContractId() == "IF12" &&
		mes4.getTicker() == "CFFEX:IF12" &&
		mes4.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes4.getMessageId() == 100 &&
		mes4.getPx() == Px(100.1, 0.1) &&
		mes4.getQty() == 10 &&
		mes4.getSide() == simulator::BuySellSide::buy);
    BOOST_CHECK(mes5.getExchangeId() == "CFFEX" &&
		mes5.getContractId() == "IF12" &&
		mes5.getTicker() == "CFFEX:IF12" &&
		mes5.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250") &&
		mes5.getMessageId() == 100 &&
		mes5.getPx() == Px(100.1, 0.1) &&
		mes5.getQty() == 10 &&
		mes5.getSide() == simulator::BuySellSide::buy);

    // test set methods
    mes1.exchangeId() = "CFFEX2";
    BOOST_CHECK(mes1.getExchangeId() == "CFFEX2");
    mes1.contractId() = "IF13";
    BOOST_CHECK(mes1.getContractId() == "IF13");
    mes1.timestamp() = SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250");
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:00.250"));
    mes1.addLatency(duration_nano(1000000000L));
    BOOST_CHECK(mes1.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:17:01.250"));
    mes1.messageId() = 1000;
    BOOST_CHECK(mes1.getMessageId() == 1000);
    mes1.px() = Px(201, 0.1);
    BOOST_CHECK(mes1.getPx() == Px(201, 0.1));
    mes1.qty() = 1;
    BOOST_CHECK(mes1.getQty() == 1);
    mes1.side() = simulator::BuySellSide::sell;
    BOOST_CHECK(mes1.getSide() == simulator::BuySellSide::sell);
}

// 14. ImpliedSpreadTopBookUpdate is the implied top book update of the spread between two contracts.
BOOST_AUTO_TEST_CASE(impliedspreadtopbookupdate_basic_methods)
{
    std::vector<Px> bid;
    std::vector<Px> ask;
    std::vector<int> bidsize;
    std::vector<int> asksize;
    for(int ii=0; ii< 4; ii++) {
	bid.push_back(Px(100.0-ii*0.1, 0.1));
	ask.push_back(Px(100.2+ii*0.1, 0.1));
	bidsize.push_back(ii + 5);
	asksize.push_back(ii + 6);
    }
    BookUpdate<true> front("CFFEX", "IF1209", SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:00.500"), 0, bid, bidsize, ask, asksize);

    bid.clear();
    ask.clear();
    bidsize.clear();
    asksize.clear();
    for(int ii=0; ii< 4; ii++) {
	bid.push_back(Px(100.5-ii*0.1, 0.1));
	ask.push_back(Px(101.2+ii*0.1, 0.1));
	bidsize.push_back(ii + 3);
	asksize.push_back(ii + 4);
    }
    BookUpdate<true> back("CFFEX", "IF1210", SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:00.500"), 0, bid, bidsize, ask, asksize);

    ImpliedSpreadTopBookUpdate<true> spreadTopBook(front, back);
    BOOST_CHECK(spreadTopBook.getTicker() == "CFFEX:IF1210-CFFEX:IF1209");
    BOOST_CHECK(spreadTopBook.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:00.500"));
    BOOST_CHECK(spreadTopBook.syncQ());

    spreadTopBook.imply();
    BOOST_CHECK(spreadTopBook.getBidPx() == Px(0.3, 0.1));
    BOOST_CHECK(spreadTopBook.getBidQty() == 3);
    BOOST_CHECK(spreadTopBook.getAskPx() == Px(1.2, 0.1));
    BOOST_CHECK(spreadTopBook.getAskQty() == 4);

    bid.clear();
    ask.clear();
    bidsize.clear();
    asksize.clear();
    for(int ii=0; ii< 4; ii++) {
	bid.push_back(Px(100.6-ii*0.1, 0.1));
	ask.push_back(Px(101.0+ii*0.1, 0.1));
	bidsize.push_back(ii + 1);
	asksize.push_back(ii + 2);
    }
    back = BookUpdate<true>("CFFEX", "IF1210", SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:01.500"), 0, bid, bidsize, ask, asksize);
    spreadTopBook.update(back, false);
    BOOST_CHECK(spreadTopBook.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:01.500"));
    spreadTopBook.imply();
    BOOST_CHECK(spreadTopBook.getBidPx() == Px(0.4, 0.1));
    BOOST_CHECK(spreadTopBook.getBidQty() == 1);
    BOOST_CHECK(spreadTopBook.getAskPx() == Px(1.0, 0.1));
    BOOST_CHECK(spreadTopBook.getAskQty() == 2);
    BOOST_CHECK(!spreadTopBook.syncQ());
}

// 14. ImpliedSpreadTopBookUpdate is the implied top book update of the spread between two contracts.
BOOST_AUTO_TEST_CASE(impliedspreadtopbookupdate_basic_methods_default_constructor)
{
    ImpliedSpreadTopBookUpdate<true> spreadTopBook;

    std::vector<Px> bid;
    std::vector<Px> ask;
    std::vector<int> bidsize;
    std::vector<int> asksize;
    for(int ii=0; ii< 4; ii++) {
	bid.push_back(Px(100.0-ii*0.1, 0.1));
	ask.push_back(Px(100.2+ii*0.1, 0.1));
	bidsize.push_back(ii + 5);
	asksize.push_back(ii + 6);
    }
    BookUpdate<true> front("CFFEX", "IF1209", SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:00.500"), 0, bid, bidsize, ask, asksize);

    bid.clear();
    ask.clear();
    bidsize.clear();
    asksize.clear();
    for(int ii=0; ii< 4; ii++) {
	bid.push_back(Px(100.5-ii*0.1, 0.1));
	ask.push_back(Px(101.2+ii*0.1, 0.1));
	bidsize.push_back(ii + 3);
	asksize.push_back(ii + 4);
    }
    BookUpdate<true> back("CFFEX", "IF1210", SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:00.500"), 0, bid, bidsize, ask, asksize);

    spreadTopBook.update(front, true);
    spreadTopBook.update(back, false);
    BOOST_CHECK(spreadTopBook.getTicker() == "CFFEX:IF1210-CFFEX:IF1209");
    BOOST_CHECK(spreadTopBook.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:00.500"));
    BOOST_CHECK(spreadTopBook.syncQ());

    spreadTopBook.imply();
    BOOST_CHECK(spreadTopBook.getBidPx() == Px(0.3, 0.1));
    BOOST_CHECK(spreadTopBook.getBidQty() == 3);
    BOOST_CHECK(spreadTopBook.getAskPx() == Px(1.2, 0.1));
    BOOST_CHECK(spreadTopBook.getAskQty() == 4);

    bid.clear();
    ask.clear();
    bidsize.clear();
    asksize.clear();
    for(int ii=0; ii< 4; ii++) {
	bid.push_back(Px(100.6-ii*0.1, 0.1));
	ask.push_back(Px(101.0+ii*0.1, 0.1));
	bidsize.push_back(ii + 1);
	asksize.push_back(ii + 2);
    }
    back = BookUpdate<true>("CFFEX", "IF1210", SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:01.500"), 0, bid, bidsize, ask, asksize);
    spreadTopBook.update(back, false);
    BOOST_CHECK(spreadTopBook.getTimestamp() == SimulatorCore::convertStringToTimePoint("2012-08-15T09:16:01.500"));
    spreadTopBook.imply();
    BOOST_CHECK(spreadTopBook.getBidPx() == Px(0.4, 0.1));
    BOOST_CHECK(spreadTopBook.getBidQty() == 1);
    BOOST_CHECK(spreadTopBook.getAskPx() == Px(1.0, 0.1));
    BOOST_CHECK(spreadTopBook.getAskQty() == 2);
    BOOST_CHECK(!spreadTopBook.syncQ());
}
