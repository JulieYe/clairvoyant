//
// File:        $ FeatureGenerator.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/06 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "Feature.hpp"
#include "CFFEX.hpp"
#include "FeatureCollector.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"

#include <fstream>

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

using simulator::MidPoint;
using simulator::BookPressure;
using simulator::BookPressureNL;
using simulator::BookPressureLog;
using simulator::ShortTermReturn;
using simulator::VolumeTrend;
using simulator::TradeDeviation;
using simulator::TradeDeviationMA;
using simulator::SignedVolume;
using simulator::SignedVolumeTradeVsMid;
using simulator::SignedVolumeWithBookSizeChg;
using simulator::Vwap;
using simulator::VwapMidReturn;
using simulator::BookSizeChg;
using simulator::BookSizeChg2;
using simulator::Volatility;
using simulator::WindowedBreakthrough;
using simulator::EMAWindowedBreakthrough;
using simulator::WindowedBreakBand;
using simulator::TradeBreakOut;

using std::ifstream;
using std::getline;

int main(int argc, char ** argv)
{
    string data_filename = argv[1];
    string contractSpec_filename = argv[2];
    string exchange = argv[3];
    string contract = argv[4];
    string asOfDate = argv[5];
    string feature_filename = argv[6];

    ContractSpec contractSpec(contractSpec_filename);
    string tickerHead = exchange + ":" + contract.substr(0, 2);
    double tickSize;
    contractSpec.getTickSize(tickerHead, tickSize);
    string startTradingTime;
    contractSpec.getDefaultStartTradingTime(tickerHead, startTradingTime);
    string exitTradingTime;
    contractSpec.getDefaultExitTradingTime(tickerHead, exitTradingTime);

    string scStartDateTime = asOfDate.substr(0,4)+'-'+asOfDate.substr(4,2)+'-'+asOfDate.substr(6,2)+"T"+startTradingTime+".000";
    string scStopDateTime = asOfDate.substr(0,4)+'-'+asOfDate.substr(4,2)+'-'+asOfDate.substr(6,2)+"T"+exitTradingTime+".000";

    SimulatorCore sc(scStartDateTime, scStopDateTime, 500, "millisecond");
    Logger logger(feature_filename);
    logger.start();
    CTPEXCH exch(exchange, contract, data_filename, tickSize, &sc);
    FeatureCollector fc(exchange, contract, &sc, &logger);

    constexpr int numPeriodsPerYear = 252 * 4.5 * 60 * 60 * 2;

    fc.addFeature(PFeature(new MidPoint()), 0);
    fc.addFeature(PFeature(new MidPoint()), 2000);
    fc.addFeature(PFeature(new MidPoint()), 5000);
    fc.addFeature(PFeature(new MidPoint()), 10000);
    fc.addFeature(PFeature(new MidPoint()), 20000);
    fc.addFeature(PFeature(new MidPoint()), 30000);
    fc.addFeature(PFeature(new MidPoint()), 40000);
    fc.addFeature(PFeature(new MidPoint()), 50000);
    fc.addFeature(PFeature(new MidPoint()), 60000);

    fc.addFeature(PFeature(new TradeBreakOut(300, 2, 3)), 0);
    fc.addFeature(PFeature(new TradeBreakOut(300, 2, 5)), 0);
    fc.addFeature(PFeature(new TradeBreakOut(600, 2, 3)), 0);
    fc.addFeature(PFeature(new TradeBreakOut(600, 2, 5)), 0);

    // fc.addFeature(PFeature(new WindowedBreakthrough<60>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakthrough<300>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakthrough<600>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakthrough<1200>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakthrough<1800>()), 0);

    // fc.addFeature(PFeature(new EMAWindowedBreakthrough<60, 30>()), 0);
    // fc.addFeature(PFeature(new EMAWindowedBreakthrough<300, 150>()), 0);
    // fc.addFeature(PFeature(new EMAWindowedBreakthrough<600, 300>()), 0);
    // fc.addFeature(PFeature(new EMAWindowedBreakthrough<1200, 600>()), 0);
    // fc.addFeature(PFeature(new EMAWindowedBreakthrough<1800, 900>()), 0);

    // fc.addFeature(PFeature(new WindowedBreakBand<60, 10>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakBand<300, 50>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakBand<600, 50>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakBand<1200, 50>()), 0);
    // fc.addFeature(PFeature(new WindowedBreakBand<1800, 50>()), 0);

    // fc.addFeature(PFeature( new BookPressure()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<2,1>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<3,1>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<4,1>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<5,1>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<6,1>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<1,2>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<1,3>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<1,4>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<1,5>()), 0);
    // fc.addFeature(PFeature( new BookPressureNL<1,6>()), 0);
    // fc.addFeature(PFeature( new BookPressureLog()), 0);

    // fc.addFeature(PFeature( new ShortTermReturn<500>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<1000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<1500>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<2000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<5000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<10000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<20000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<30000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<50000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<60000>()), 0);
    // fc.addFeature(PFeature( new ShortTermReturn<180000>()), 0);

    // fc.addFeature(PFeature( new TradeDeviationMA<2>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<4>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<6>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<8>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<10>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<15>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<20>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<40>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<60>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<80>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<120>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<360>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<720>()), 0);
    // fc.addFeature(PFeature( new TradeDeviationMA<1440>()), 0);

    // fc.addFeature(PFeature( new SignedVolume<500>()), 0);
    // fc.addFeature(PFeature( new SignedVolume<1000>()), 0);
    // fc.addFeature(PFeature( new SignedVolume<1500>()), 0);
    // fc.addFeature(PFeature( new SignedVolume<2000>()), 0);
    // fc.addFeature(PFeature( new SignedVolume<10000>()), 0);
    // fc.addFeature(PFeature( new SignedVolume<20000>()), 0);
    // fc.addFeature(PFeature( new SignedVolume<360000>()), 0);
    // fc.addFeature(PFeature( new SignedVolume<720000>()), 0);

    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<500>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<1000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<1500>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<2000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<10000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<20000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<360000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeTradeVsMid<720000>()), 0);

    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<500>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<1000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<1500>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<2000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<10000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<20000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<360000>()), 0);
    // fc.addFeature(PFeature( new SignedVolumeWithBookSizeChg<720000>()), 0);

    // fc.addFeature(PFeature( new VwapMidReturn<1500>()), 0);
    // fc.addFeature(PFeature( new VwapMidReturn<20000>()), 0);
    // fc.addFeature(PFeature( new VwapMidReturn<50000>()), 0);

    // fc.addFeature(PFeature( new BookSizeChg<500, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<1500, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<2000, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<5000, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<500, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<1000, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<1500, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<2000, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg<5000, static_cast<int>(BuySellSide::sell)>()) , 0);

    // fc.addFeature(PFeature( new BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<1000, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<1500, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<2000, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<5000, static_cast<int>(BuySellSide::buy)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<1000, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<1500, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<2000, static_cast<int>(BuySellSide::sell)>()), 0);
    // fc.addFeature(PFeature( new BookSizeChg2<5000, static_cast<int>(BuySellSide::sell)>()) , 0);

    // fc.addFeature(PFeature(new VolumeTrend<1, 5>()), 0);
    // fc.addFeature(PFeature(new VolumeTrend<2, 20>()), 0);

    // fc.addFeature(PFeature(new Volatility<20, numPeriodsPerYear>()), 0);
    // fc.addFeature(PFeature(new Volatility<60, numPeriodsPerYear>()), 0);
    // fc.addFeature(PFeature(new Volatility<80, numPeriodsPerYear>()), 0);
    // fc.addFeature(PFeature(new Volatility<120, numPeriodsPerYear>()), 0);
		
    fc.writeHeader();
		
    sc.registerMatchy(&exch);
    sc.registerStrategy(&fc);
		
    while (sc.next()) {}
    logger.end();
}
