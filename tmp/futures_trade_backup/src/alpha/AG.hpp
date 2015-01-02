/* 
   Generally manually now. 
   Should be automated generally through regression tool. 
*/

#ifndef AG_HPP
#define AG_HPP

#include "Signal.hpp"
#include "Feature.hpp"
#include "Px.hpp"

using simulator::SignalOnPxPredict;
using simulator::PFeature;
using simulator::Px;
using simulator::BookSizeChg;
using simulator::BookSizeChg2;
using simulator::VwapMidReturn;
using simulator::TradeDeviationMA;
using simulator::VolumeTrend;
using simulator::Volatility;
using simulator::HighFrequencyVolatility;
using simulator::SignedVolume;
using simulator::SignedVolumeTradeVsMid;
using simulator::SignedVolumeWithBookSizeChg;
using simulator::BookPressure;
using simulator::BookPressureNL;
using simulator::BookPressureLog;
using simulator::ShortTermReturn;
using simulator::WindowedBreakthrough;
using simulator::EMAWindowedBreakthrough;
using simulator::TradeBreakOut;
using simulator::SweepWeak;
using simulator::SweepStrong;

namespace simulator
{

    class AG
    {
    public:
	//signal related
	SignalOnPxPredict<		ShortTermReturn<500>,
ShortTermReturn<20000>,
TradeDeviationMA<2>,
SignedVolume<500>,
SignedVolumeTradeVsMid<500>,
SignedVolumeWithBookSizeChg<500>,
SignedVolumeWithBookSizeChg<2000>,
BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<2000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<1500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<1500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<1000, static_cast<int>(BuySellSide::sell)>,
BookPressure	  > directionSignal_;
    
    AG() {
	// initialize signals
	directionSignal_.registerFeature(PFeature(new ShortTermReturn<500>()),
PFeature(new ShortTermReturn<20000>()),
PFeature(new TradeDeviationMA<2>()),
PFeature(new SignedVolume<500>()),
PFeature(new SignedVolumeTradeVsMid<500>()),
PFeature(new SignedVolumeWithBookSizeChg<500>()),
PFeature(new SignedVolumeWithBookSizeChg<2000>()),
PFeature(new BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<2000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<1500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<1500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<1000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookPressure())
					 );

	// register betas
	directionSignal_.registerBeta(-720.76079295775, 17.2467864346836, 257.452494963897, -0.000539492501530875, 0.000222626188719661, 0.00208565206762342, -0.000196706148511705, 0.000271475809717827, 0.00015647624007823, -0.000323981630740141, 0.00167818520924002, 0.000448704895819808, -0.00131129646321052, -0.000753734253905948, 0.43510073384991);

	// register bounds
	directionSignal_.getFeatures()[0]->setBound(true, 0.000385);
directionSignal_.getFeatures()[1]->setBound(true, 0.002077);
directionSignal_.getFeatures()[2]->setBound(true, 0.000450);
directionSignal_.getFeatures()[3]->setBound(true, 472.404092);
directionSignal_.getFeatures()[4]->setBound(true, 642.219442);
directionSignal_.getFeatures()[5]->setBound(true, 205.220632);
directionSignal_.getFeatures()[6]->setBound(true, 866.965890);
directionSignal_.getFeatures()[7]->setBound(true, 497.360721);
directionSignal_.getFeatures()[8]->setBound(true, 596.870112);
directionSignal_.getFeatures()[9]->setBound(true, 539.864415);
directionSignal_.getFeatures()[10]->setBound(true, 197.662816);
directionSignal_.getFeatures()[11]->setBound(true, 387.668963);
directionSignal_.getFeatures()[12]->setBound(true, 204.956545);
directionSignal_.getFeatures()[13]->setBound(true, 322.978577);
directionSignal_.getFeatures()[14]->setBound(true, 1.329860);
    }

	~AG()
	{}
	
    };

}

#endif  // AG_HPP