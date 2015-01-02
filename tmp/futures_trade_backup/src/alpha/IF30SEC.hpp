/* 
   Generally manually now. 
   Should be automated generally through regression tool. 
*/

#ifndef IF30SEC_HPP
#define IF30SEC_HPP

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

    class IF30SEC
    {
    public:
	//signal related
	SignalOnPxPredict<		ShortTermReturn<50000>,
ShortTermReturn<60000>,
ShortTermReturn<180000>,
SignedVolume<1500>,
SignedVolume<20000>,
SignedVolume<300000>,
SignedVolume<600000>,
SignedVolumeTradeVsMid<500>,
SignedVolumeTradeVsMid<20000>,
SignedVolumeTradeVsMid<60000>,
SignedVolumeTradeVsMid<300000>,
SignedVolumeWithBookSizeChg<20000>,
SignedVolumeWithBookSizeChg<300000>,
VwapMidReturn<20000>,
BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<5000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<5000, static_cast<int>(BuySellSide::sell)>	  > directionSignal_;
    
    IF30SEC() {
	// initialize signals
	directionSignal_.registerFeature(PFeature(new ShortTermReturn<50000>()),
PFeature(new ShortTermReturn<60000>()),
PFeature(new ShortTermReturn<180000>()),
PFeature(new SignedVolume<1500>()),
PFeature(new SignedVolume<20000>()),
PFeature(new SignedVolume<300000>()),
PFeature(new SignedVolume<600000>()),
PFeature(new SignedVolumeTradeVsMid<500>()),
PFeature(new SignedVolumeTradeVsMid<20000>()),
PFeature(new SignedVolumeTradeVsMid<60000>()),
PFeature(new SignedVolumeTradeVsMid<300000>()),
PFeature(new SignedVolumeWithBookSizeChg<20000>()),
PFeature(new SignedVolumeWithBookSizeChg<300000>()),
PFeature(new VwapMidReturn<20000>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<5000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<5000, static_cast<int>(BuySellSide::sell)>())
					 );

	// register betas
	directionSignal_.registerBeta(127.162359944635, -87.1994881891069, -20.5465756400784, -0.000242410757290862, -4.04341532628481e-05, 3.46288494469194e-05, -2.79054129031112e-05, 0.000251943170611379, -0.000229648484413862, 7.6957456902438e-05, -1.89796146963198e-05, -2.50739725658864e-05, -2.38893205323333e-06, -378.422441430243, 0.00247245248233545, 0.000188160104643925, -0.00244081928553534, -0.000285317264640105);

	// register bounds
	directionSignal_.getFeatures()[0]->setBound(true, 0.005086);
directionSignal_.getFeatures()[1]->setBound(true, 0.005502);
directionSignal_.getFeatures()[2]->setBound(true, 0.008456);
directionSignal_.getFeatures()[3]->setBound(true, 1072.605201);
directionSignal_.getFeatures()[4]->setBound(true, 2481.914837);
directionSignal_.getFeatures()[5]->setBound(true, 6842.861648);
directionSignal_.getFeatures()[6]->setBound(true, 9190.786099);
directionSignal_.getFeatures()[7]->setBound(true, 628.657860);
directionSignal_.getFeatures()[8]->setBound(true, 2138.308685);
directionSignal_.getFeatures()[9]->setBound(true, 3104.421259);
directionSignal_.getFeatures()[10]->setBound(true, 5817.398218);
directionSignal_.getFeatures()[11]->setBound(true, 5500.432789);
directionSignal_.getFeatures()[12]->setBound(true, 51320.159950);
directionSignal_.getFeatures()[13]->setBound(true, 0.001775);
directionSignal_.getFeatures()[14]->setBound(true, 121.577254);
directionSignal_.getFeatures()[15]->setBound(true, 599.681669);
directionSignal_.getFeatures()[16]->setBound(true, 123.223810);
directionSignal_.getFeatures()[17]->setBound(true, 597.990596);
    }

	~IF30SEC()
	{}
	
    };

}

#endif  // IF30SEC_HPP