//
// File:        $ test_strategy.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/16 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE unit_test_strategy

#include <cstdlib>
#include <iostream>

using std::cout;
using std::boolalpha;
using std::endl;

#include <boost/test/unit_test.hpp>

#include "../Strategy.hpp"
#include "../SimulatorCore.hpp"
#include "../Logger.hpp"

using simulator::Strategy;
using simulator::SimulatorCore;
using simulator::Logger;
using simulator::BuySellSide;
using simulator::Px;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::AcknowledgedOrderMessage;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::CancelOrderMessage;

// tolerance level for testing PnL purpose
double tolerance = 1e-6;

class TestStrategy : public Strategy
{
public:
    TestStrategy(const string strat_name, SimulatorCore * simcore_ptr, Logger * logger_ptr) : Strategy(strat_name, simcore_ptr, logger_ptr)
    {}

    void process()
    {}
};

BOOST_AUTO_TEST_CASE(strategy_one_product)
{
    SimulatorCore sc("2013-01-10T09:15:00.000", "2013-01-10T15:15:00.000", 500, "millisecond");
    Logger logger("tmp/test_strategy.log");
    logger.start();
    TestStrategy strat("strategy_test_one_product", &sc, &logger);
    sc.registerStrategy(&strat);

    /* Send two buy limit orders, 2 sell limit orders, 1 market buy orders
       check limitOrderCounts. */
    /* Check the strat_id of outgoing messages. */
    strat.addStrat("CFFEX", "IF1303", 300);
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.000"), strat.getStratId(), 1, 1, 2, BuySellSide::buy, Px(1210, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.500"), strat.getStratId(), 2, 2, 4, BuySellSide::buy, Px(1210.2, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.500"), strat.getStratId(), 3, 3, 2, BuySellSide::sell, Px(1210.4, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:01.000"), strat.getStratId(), 4, 4, 4, BuySellSide::sell, Px(1210.6, 0.2)));
    strat.marketOrderMessages().push_back(MarketOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.000"), strat.getStratId(), 5, 5, 10, BuySellSide::buy));
    strat.update();
    BOOST_CHECK(strat.getLimitOrderCount(0) == 4);
    BOOST_CHECK(strat.getLimitOrderCount() == 4);
    BOOST_CHECK(sc.limitOrderMessages().front().getStratId() == strat.getStratId());
    BOOST_CHECK(sc.marketOrderMessages().front().getStratId() == strat.getStratId());

    /* Receive the fill message from market buy, 4 acks
       check volume, openposition, openpnl, closedpnl, pnl, fillcounts. */
    /* Check the strat_id of outgoing messages. */
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.300"), strat.getStratId(), 6, 5, 10, BuySellSide::buy, Px(1210.4, 0.2)));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.100"), strat.getStratId(), 7, 1, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.600"), strat.getStratId(), 8, 2, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:01.100"), strat.getStratId(), 9, 3, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.100"), strat.getStratId(), 10, 4, false));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.500"), 11, vector<Px>{Px(1210.2, 0.2)}, vector<int>{12}, vector<Px>{Px(1210.4, 0.2)}, vector<int>{13}));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.500"), 12, Px(1210.4, 0.2), 10, BuySellSide::buy));
    strat.update();
    BOOST_CHECK(strat.getVolume() == 10);
    BOOST_CHECK(strat.getOpenPosition() == 10);
    BOOST_CHECK(abs(strat.getOpenPnL() + 300.0) < tolerance);

    /* Receive fill messages from the two sell limit orders
       check volume, openposition, openpnl, closedpnl, pnl, fillcounts. */
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), strat.getStratId(), 13, 3, 2, BuySellSide::sell, Px(1210.4, 0.2)));
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), strat.getStratId(), 14, 4, 4, BuySellSide::sell, Px(1210.6, 0.2)));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), 15, vector<Px>{Px(1210.6, 0.2)}, vector<int>{12}, vector<Px>{Px(1210.8, 0.2)}, vector<int>{13}));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), 16, Px(1210.4, 0.2), 2, BuySellSide::sell));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.500"), 17, Px(1210.6, 0.2), 4, BuySellSide::sell));
    strat.update();
    BOOST_CHECK(strat.getVolume() == 16);
    BOOST_CHECK(strat.getOpenPosition() == 4);
    BOOST_CHECK(abs(strat.getOpenPnL() - 360.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL() - 240.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL() - 600.0) < tolerance);
    BOOST_CHECK(strat.getFillCount() == 3);

    /* Send two cancel messages for the buy limit orders
       check limitordercounts, marketordercounts, cancelcounts, fillcounts */
    strat.cancelLimitOrderMessages().push_back(CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), strat.getStratId(), 18, 1, 2, BuySellSide::buy, Px(1210, 0.2)));
    strat.cancelLimitOrderMessages().push_back(CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), strat.getStratId(), 19, 2, 4, BuySellSide::buy, Px(1210.2, 0.2)));
    strat.update();
    BOOST_CHECK(strat.getLimitOrderCount() == 4);
    BOOST_CHECK(strat.getMarketOrderCount() == 1);
    BOOST_CHECK(strat.getCancelCount() == 2);
    BOOST_CHECK(strat.getFillCount() == 3);
    BOOST_CHECK(strat.getOpenBuyOrderCount() == 2);
    BOOST_CHECK(strat.getOpenSellOrderCount() == 0);

    /* Receive cancel acks and bookupdates
       check openpnl closedpnl and pnl. */
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.100"), strat.getStratId(), 20, 1, true));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.100"), strat.getStratId(), 21, 2, true));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), 22, vector<Px>{Px(1210.8, 0.2)}, vector<int>{100}, vector<Px>{Px(1211.0, 0.2)}, vector<int>{100}));
    strat.update();
    BOOST_CHECK(abs(strat.getOpenPnL() - 600.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL() - 240.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL() - 840.0) < tolerance);
    
    logger.end();
}

BOOST_AUTO_TEST_CASE(strategy_two_products)
{
    SimulatorCore sc("2013-01-10T09:15:00.000", "2013-01-10T15:15:00.000", 500, "millisecond");
    Logger logger("tmp/test_strategy.log");
    logger.start();
    TestStrategy strat("strategy_test_two_products", &sc, &logger);
    sc.registerStrategy(&strat);

    /* Send two buy limit orders, 2 sell limit orders, 1 market buy orders
       check limitOrderCounts. */
    /* Check the strat_id of outgoing messages. */
    strat.addStrat("CFFEX", "IF1303", 300);
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.000"), strat.getStratId(), 1, 1, 2, BuySellSide::buy, Px(1210, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.500"), strat.getStratId(), 2, 2, 4, BuySellSide::buy, Px(1210.2, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.500"), strat.getStratId(), 3, 3, 2, BuySellSide::sell, Px(1210.4, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:01.000"), strat.getStratId(), 4, 4, 4, BuySellSide::sell, Px(1210.6, 0.2)));
    strat.marketOrderMessages().push_back(MarketOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.000"), strat.getStratId(), 5, 5, 10, BuySellSide::buy));
    strat.addStrat("CFFEX", "IF1306", 400);
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:00.000"), strat.getStratId(), 6, 6, 2, BuySellSide::buy, Px(2210, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:00.500"), strat.getStratId(), 7, 7, 4, BuySellSide::buy, Px(2210.2, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:00.500"), strat.getStratId(), 8, 8, 2, BuySellSide::sell, Px(2210.4, 0.2)));
    strat.limitOrderMessages().push_back(LimitOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:01.000"), strat.getStratId(), 9, 9, 4, BuySellSide::sell, Px(2210.6, 0.2)));
    strat.marketOrderMessages().push_back(MarketOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:02.000"), strat.getStratId(), 10, 10, 10, BuySellSide::buy));
    strat.update();
    BOOST_CHECK(strat.getLimitOrderCount(0) == 4);
    BOOST_CHECK(strat.getLimitOrderCount(1) == 4);
    BOOST_CHECK(strat.getLimitOrderCount() == 8);
    BOOST_CHECK(sc.limitOrderMessages().front().getStratId() == strat.getStratId());
    BOOST_CHECK(sc.marketOrderMessages().front().getStratId() == strat.getStratId());

    /* Receive the fill message from market buy, 4 acks
       check volume, openposition, openpnl, closedpnl, pnl, fillcounts. */
    /* Check the strat_id of outgoing messages. */
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.300"), strat.getStratId(), 11, 5, 10, BuySellSide::buy, Px(1210.4, 0.2)));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.100"), strat.getStratId(), 12, 1, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:00.600"), strat.getStratId(), 13, 2, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:01.100"), strat.getStratId(), 14, 3, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.100"), strat.getStratId(), 15, 4, false));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.500"), 16, vector<Px>{Px(1210.2, 0.2)}, vector<int>{12}, vector<Px>{Px(1210.4, 0.2)}, vector<int>{13}));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:02.500"), 17, Px(1210.4, 0.2), 10, BuySellSide::buy));
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:02.300"), strat.getStratId(), 18, 10, 10, BuySellSide::buy, Px(2210.4, 0.2)));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:00.100"), strat.getStratId(), 19, 6, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:00.600"), strat.getStratId(), 20, 7, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:01.100"), strat.getStratId(), 21, 8, false));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:02.100"), strat.getStratId(), 22, 9, false));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:02.500"), 23, vector<Px>{Px(2210.2, 0.2)}, vector<int>{12}, vector<Px>{Px(2210.4, 0.2)}, vector<int>{13}));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:02.500"), 24, Px(2210.4, 0.2), 10, BuySellSide::buy));
    strat.update();
    BOOST_CHECK(strat.getVolume(0) == 10);
    BOOST_CHECK(strat.getVolume(1) == 10);
    BOOST_CHECK(strat.getVolume() == 20);
    BOOST_CHECK(strat.getOpenPosition(0) == 10);
    BOOST_CHECK(strat.getOpenPosition(1) == 10);
    BOOST_CHECK(strat.getOpenPosition() == 20);
    BOOST_CHECK(abs(strat.getOpenPnL(0) + 300.0) < tolerance);
    BOOST_CHECK(abs(strat.getOpenPnL(1) + 400.0) < tolerance);
    BOOST_CHECK(abs(strat.getOpenPnL() + 700.0) < tolerance);

    /* Receive fill messages from the two sell limit orders
       check volume, openposition, openpnl, closedpnl, pnl, fillcounts. */
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), strat.getStratId(), 25, 3, 2, BuySellSide::sell, Px(1210.4, 0.2)));
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), strat.getStratId(), 26, 4, 4, BuySellSide::sell, Px(1210.6, 0.2)));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), 27, vector<Px>{Px(1210.6, 0.2)}, vector<int>{12}, vector<Px>{Px(1210.8, 0.2)}, vector<int>{13}));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), 28, Px(1210.4, 0.2), 2, BuySellSide::sell));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:05.500"), 29, Px(1210.6, 0.2), 4, BuySellSide::sell));
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), strat.getStratId(), 30, 8, 2, BuySellSide::sell, Px(2210.4, 0.2)));
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), strat.getStratId(), 31, 9, 4, BuySellSide::sell, Px(2210.6, 0.2)));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), 32, vector<Px>{Px(2210.6, 0.2)}, vector<int>{12}, vector<Px>{Px(2210.8, 0.2)}, vector<int>{13}));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:05.000"), 33, Px(2210.4, 0.2), 2, BuySellSide::sell));
    sc.tradeUpdate().push_back(TradeUpdate<true>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:05.500"), 34, Px(2210.6, 0.2), 4, BuySellSide::sell));
    strat.update();
    BOOST_CHECK(strat.getVolume(0) == 16);
    BOOST_CHECK(strat.getVolume(1) == 16);
    BOOST_CHECK(strat.getVolume() == 32);
    BOOST_CHECK(strat.getOpenPosition(0) == 4);
    BOOST_CHECK(strat.getOpenPosition(1) == 4);
    BOOST_CHECK(strat.getOpenPosition() == 8);
    BOOST_CHECK(abs(strat.getOpenPnL(0) - 360.0) < tolerance);
    BOOST_CHECK(abs(strat.getOpenPnL(1) - 480.0) < tolerance);
    BOOST_CHECK(abs(strat.getOpenPnL() - 840.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL(0) - 240.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL(1) - 320.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL() - 560.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL(0) - 600.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL(1) - 800.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL() - 1400.0) < tolerance);
    BOOST_CHECK(strat.getFillCount(0) == 3);
    BOOST_CHECK(strat.getFillCount(1) == 3);
    BOOST_CHECK(strat.getFillCount() == 6);

    /* Send two cancel messages for the buy limit orders
       check limitordercounts, marketordercounts, cancelcounts, fillcounts */
    strat.cancelLimitOrderMessages().push_back(CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), strat.getStratId(), 35, 1, 2, BuySellSide::buy, Px(1210, 0.2)));
    strat.cancelLimitOrderMessages().push_back(CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), strat.getStratId(), 36, 2, 4, BuySellSide::buy, Px(1210.2, 0.2)));
    strat.cancelLimitOrderMessages().push_back(CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), strat.getStratId(), 37, 6, 2, BuySellSide::buy, Px(2210, 0.2)));
    strat.cancelLimitOrderMessages().push_back(CancelOrderMessage<LimitOrderMessage>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), strat.getStratId(), 38, 7, 4, BuySellSide::buy, Px(2210.2, 0.2)));
    strat.update();

    BOOST_CHECK(strat.getLimitOrderCount(0) == 4);
    BOOST_CHECK(strat.getLimitOrderCount(1) == 4);
    BOOST_CHECK(strat.getLimitOrderCount() == 8);
    BOOST_CHECK(strat.getMarketOrderCount(0) == 1);
    BOOST_CHECK(strat.getMarketOrderCount(1) == 1);
    BOOST_CHECK(strat.getMarketOrderCount() == 2);
    BOOST_CHECK(strat.getOpenBuyOrderCount(0) == 2);
    BOOST_CHECK(strat.getOpenBuyOrderCount(1) == 2);
    BOOST_CHECK(strat.getOpenBuyOrderCount() == 4);
    BOOST_CHECK(strat.getOpenSellOrderCount(0) == 0);
    BOOST_CHECK(strat.getOpenSellOrderCount(1) == 0);
    BOOST_CHECK(strat.getOpenSellOrderCount() == 0);
    BOOST_CHECK(strat.getCancelCount(0) == 2);
    BOOST_CHECK(strat.getCancelCount(1) == 2);
    BOOST_CHECK(strat.getCancelCount() == 4);
    BOOST_CHECK(strat.getFillCount(0) == 3);
    BOOST_CHECK(strat.getFillCount(1) == 3);
    BOOST_CHECK(strat.getFillCount() == 6);

    /* Receive cancel acks and bookupdates
       check openpnl closedpnl and pnl. */
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.100"), strat.getStratId(), 39, 1, true));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.100"), strat.getStratId(), 40, 2, true));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), 41, vector<Px>{Px(1210.8, 0.2)}, vector<int>{100}, vector<Px>{Px(1211.0, 0.2)}, vector<int>{100}));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:06.100"), strat.getStratId(), 42, 6, true));
    sc.acknowledgedOrderMessages().push_back(AcknowledgedOrderMessage("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:06.100"), strat.getStratId(), 43, 7, true));
    sc.bookUpdate().push_back(BookUpdate<true>("CFFEX", "IF1306", sc.convertStringToTimePoint("2013-01-10T09:16:06.000"), 44, vector<Px>{Px(2210.8, 0.2)}, vector<int>{100}, vector<Px>{Px(2211.0, 0.2)}, vector<int>{100}));
    strat.update();
    BOOST_CHECK(abs(strat.getOpenPnL(0) - 600.0) < tolerance);
    BOOST_CHECK(abs(strat.getOpenPnL(1) - 800.0) < tolerance);
    BOOST_CHECK(abs(strat.getOpenPnL() - 1400.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL(0) - 240.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL(1) - 320.0) < tolerance);
    BOOST_CHECK(abs(strat.getClosedPnL() - 560.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL(0) - 840.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL(1) - 1120.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL() - 1960.0) < tolerance);

    // check for openPnlRefPxs
    BOOST_CHECK(abs(strat.getOpenPnlRefPxs(0) - 1210.9) < tolerance);
    sc.bookUpdates().clear();
    sc.tradeUpdates().clear();
    sc.filledOrderMessages().clear();
    sc.filledOrderMessages().push_back(FilledOrderMessage("CFFEX", "IF1303", sc.convertStringToTimePoint("2013-01-10T09:16:07.300"), strat.getStratId(), 0, 0, 0, BuySellSide::buy, Px(1211.0, 0.2)));
    strat.update();
    BOOST_CHECK(abs(strat.getOpenPnlRefPxs(0) - 1211.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL(0) - 960.0) < tolerance);
    BOOST_CHECK(abs(strat.getPnL() - 2080.0) < tolerance);
    
    logger.end();
}
