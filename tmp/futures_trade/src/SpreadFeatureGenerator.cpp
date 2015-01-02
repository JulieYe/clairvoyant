//
// File:        $ SpreadFeatureGenerator.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/10/28 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Feature.hpp"
#include "CFFEX.hpp"
#include "FeatureCollector.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"

using simulator::SignedVolume;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::Px;
using simulator::Logger;
using simulator::Matchy;
using simulator::CFFEX;
using simulator::SimulatorCore;
using simulator::FeatureCollector;
using simulator::PFeature;
using simulator::ContractSpec;
using simulator::ConfigSpec;
using simulator::CTPEXCH;

// using simulator::MidPoint;
using simulator::ImpliedSpreadPrice;
using simulator::ImpliedSpreadPriceSmooth;
// using simulator::ImpliedSpreadBidAskSpread;
using simulator::HighFrequencyVolatilitySpread;
using simulator::HighFrequencyVolatility;
// using simulator::LastTradeSpread;
// using simulator::BookPressure;
// using simulator::BookPressureNL;
// using simulator::BookPressureLog;
// using simulator::ShortTermReturn;
// using simulator::VolumeTrend;
// using simulator::TradeDeviation;
// using simulator::TradeDeviationMA;
// using simulator::SignedVolume;
// using simulator::SignedVolumeTradeVsMid;
// using simulator::SignedVolumeWithBookSizeChg;
// using simulator::Vwap;
// using simulator::VwapMidReturn;
// using simulator::BookSizeChg;
// using simulator::BookSizeChg2;
// using simulator::Volatility;
// using simulator::WindowedBreakthrough;

#include <fstream>
#include <algorithm>

using std::ifstream;
using std::getline;
using std::min;

int main(int argc, char ** argv)
{
    string data_filename_front = argv[1];
    string data_filename_back = argv[2];
    string contractSpec_filename = argv[3];
    string exchange_front = argv[4];
    string exchange_back = argv[5];
    string contract_front = argv[6];
    string contract_back = argv[7];
    string asOfDate = argv[8];
    string feature_filename = argv[9];

    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead_front = exchange_front + ":" + contract_front.substr(0, 2);
    string tickerHead_back = exchange_back + ":" + contract_back.substr(0, 2);
    double tickSize_front, tickSize_back;
    contractSpec.getTickSize(tickerHead_front, tickSize_front);
    contractSpec.getTickSize(tickerHead_back, tickSize_back);
    string startTradingTime, startTradingTime_front, startTradingTime_back;
    contractSpec.getDefaultStartTradingTime(tickerHead_front, startTradingTime_front);
    contractSpec.getDefaultStartTradingTime(tickerHead_back, startTradingTime_back);
    string exitTradingTime, exitTradingTime_front, exitTradingTime_back;
    contractSpec.getDefaultExitTradingTime(tickerHead_front, exitTradingTime_front);
    contractSpec.getDefaultExitTradingTime(tickerHead_back, exitTradingTime_back);

    string scStartDateTime = asOfDate.substr(0,4)+'-'+asOfDate.substr(4,2)+'-'+asOfDate.substr(6,2)+"T"+min(startTradingTime_front,startTradingTime_back)+".000";
    string scStopDateTime = asOfDate.substr(0,4)+'-'+asOfDate.substr(4,2)+'-'+asOfDate.substr(6,2)+"T"+min(exitTradingTime_front,exitTradingTime_back)+".000";

    SimulatorCore sc(scStartDateTime, scStopDateTime, 500, "millisecond");
    Logger logger(feature_filename);
    logger.start();
    CTPEXCH exch_front(exchange_front, contract_front, data_filename_front, tickSize_front, &sc);
    CTPEXCH exch_back(exchange_back, contract_back, data_filename_back, tickSize_back, &sc);
    FeatureCollector fc(exchange_front, contract_front, exchange_back, contract_back, &sc, &logger);

    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,2>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,5>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,10>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,20>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,30>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,40>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,50>()), 0);
    fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,60>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,90>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::mid),4,120>()), 0);
    
    fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 0);

    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::buy)>()), 500);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::sell)>()), 500);

    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 1000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 2000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 3000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 4000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 5000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 10000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 20000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 30000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 40000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 50000);
    // fc.addFeature(PFeature(new ImpliedSpreadPrice<static_cast<int>(BuySellSide::mid)>()), 60000);

    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::buy),4,60>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadPriceSmooth<static_cast<int>(BuySellSide::sell),4,60>()), 0);

    // fc.addFeature(PFeature(new ImpliedSpreadBidAskSpread<static_cast<int>(FrontBackSide::mid)>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadBidAskSpread<static_cast<int>(FrontBackSide::front)>()), 0);
    // fc.addFeature(PFeature(new ImpliedSpreadBidAskSpread<static_cast<int>(FrontBackSide::back)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 2, static_cast<int>(BuySellSide::mid)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 2, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 2, static_cast<int>(BuySellSide::sell)>()), 0);
    fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 2, static_cast<int>(BuySellSide::mid)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 5, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 5, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 10, static_cast<int>(BuySellSide::mid)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 10, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 10, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 20, static_cast<int>(BuySellSide::mid)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 20, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<60, 20, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 10, static_cast<int>(BuySellSide::mid)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 10, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 10, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 30, static_cast<int>(BuySellSide::mid)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 30, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 30, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 60, static_cast<int>(BuySellSide::mid)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 60, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature(new HighFrequencyVolatilitySpread<300, 60, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature(new LastTradeSpread()), 0);
		
    fc.writeHeader();
		
    sc.registerMatchy(&exch_front);
    sc.registerMatchy(&exch_back);
    sc.registerStrategy(&fc);
		
    while (sc.next()) {}
    logger.end();
}
