/* 
   Generally manually now. 
   Should be automated generally through regression tool. 
*/

#ifndef RB_HPP
#define RB_HPP

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
using simulator::SignedVolume;
using simulator::SignedVolumeTradeVsMid;
using simulator::SignedVolumeWithBookSizeChg;
using simulator::BookPressure;
using simulator::BookPressureNL;
using simulator::BookPressureLog;
using simulator::ShortTermReturn;

namespace simulator
{

    class RB
    {
    public:
	//signal related
	SignalOnPxPredict<		BookPressureNL<1, 3>,
ShortTermReturn<500>,
ShortTermReturn<2000>,
ShortTermReturn<5000>,
ShortTermReturn<20000>,
TradeDeviationMA<2>,
TradeDeviationMA<80>,
SignedVolume<1000>,
SignedVolumeTradeVsMid<500>,
SignedVolumeTradeVsMid<20000>,
SignedVolumeWithBookSizeChg<500>,
SignedVolumeWithBookSizeChg<2000>,
VwapMidReturn<1500>,
BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<1500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<1500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<1500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<2000, static_cast<int>(BuySellSide::sell)>	  > directionSignal_;
    
    RB() {
	// initialize signals
	directionSignal_.registerFeature(PFeature(new BookPressureNL<1, 3>()),
PFeature(new ShortTermReturn<500>()),
PFeature(new ShortTermReturn<2000>()),
PFeature(new ShortTermReturn<5000>()),
PFeature(new ShortTermReturn<20000>()),
PFeature(new TradeDeviationMA<2>()),
PFeature(new TradeDeviationMA<80>()),
PFeature(new SignedVolume<1000>()),
PFeature(new SignedVolumeTradeVsMid<500>()),
PFeature(new SignedVolumeTradeVsMid<20000>()),
PFeature(new SignedVolumeWithBookSizeChg<500>()),
PFeature(new SignedVolumeWithBookSizeChg<2000>()),
PFeature(new VwapMidReturn<1500>()),
PFeature(new BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<1500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<1500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<1500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<2000, static_cast<int>(BuySellSide::sell)>())
					 );

	// register betas
	directionSignal_.registerBeta(1.19905666863276, -191.018897350047, -136.06689765825, -120.81995915996, 30.9487702896664, 181.765175581973, -457.799254243351, -7.41647976934215e-05, 5.63341743824848e-05, 3.48313733876748e-06, 0.000533889883350184, -2.52283824989401e-05, -82.2502605351785, 8.47372704708284e-05, -7.65004779077525e-05, 0.0002428007151736, 0.000279654284920695, -0.000268914955994343, -0.000144447067144712, -0.000103723655814326);

	// register bounds
	directionSignal_.getFeatures()[0]->setBound(true, 0.618684);
directionSignal_.getFeatures()[1]->setBound(true, 0.000361);
directionSignal_.getFeatures()[2]->setBound(true, 0.000656);
directionSignal_.getFeatures()[3]->setBound(true, 0.000995);
directionSignal_.getFeatures()[4]->setBound(true, 0.001907);
directionSignal_.getFeatures()[5]->setBound(true, 0.000448);
directionSignal_.getFeatures()[6]->setBound(true, 0.000136);
directionSignal_.getFeatures()[7]->setBound(true, 1536.587811);
directionSignal_.getFeatures()[8]->setBound(true, 1785.230883);
directionSignal_.getFeatures()[9]->setBound(true, 22745.488274);
directionSignal_.getFeatures()[10]->setBound(true, 566.219907);
directionSignal_.getFeatures()[11]->setBound(true, 2337.762363);
directionSignal_.getFeatures()[12]->setBound(true, 0.000589);
directionSignal_.getFeatures()[13]->setBound(true, 1495.320633);
directionSignal_.getFeatures()[14]->setBound(true, 1680.980275);
directionSignal_.getFeatures()[15]->setBound(true, 512.004440);
directionSignal_.getFeatures()[16]->setBound(true, 1023.932764);
directionSignal_.getFeatures()[17]->setBound(true, 515.350500);
directionSignal_.getFeatures()[18]->setBound(true, 1023.460533);
directionSignal_.getFeatures()[19]->setBound(true, 1233.534971);
    }

	~RB()
	{}
	
    };

}

#endif  // RB_HPP