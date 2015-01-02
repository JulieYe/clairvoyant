/* 
   Generally manually now. 
   Should be automated generally through regression tool. 
*/

#ifndef IF_HPP
#define IF_HPP

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

    class IF
    {
    public:
	//signal related
	SignalOnPxPredict<		ShortTermReturn<500>,
ShortTermReturn<1000>,
ShortTermReturn<1500>,
ShortTermReturn<2000>,
ShortTermReturn<5000>,
ShortTermReturn<20000>,
ShortTermReturn<30000>,
ShortTermReturn<180000>,
TradeDeviationMA<2>,
TradeDeviationMA<15>,
SignedVolume<500>,
SignedVolume<2000>,
SignedVolume<10000>,
SignedVolume<600000>,
SignedVolumeTradeVsMid<500>,
SignedVolumeTradeVsMid<2000>,
SignedVolumeTradeVsMid<20000>,
SignedVolumeWithBookSizeChg<1500>,
SignedVolumeWithBookSizeChg<20000>,
BookSizeChg<500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<1500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<2000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<5000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg<1000, static_cast<int>(BuySellSide::sell)>,
BookSizeChg<1500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg<2000, static_cast<int>(BuySellSide::sell)>,
BookSizeChg<5000, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<1000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<5000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<1000, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<5000, static_cast<int>(BuySellSide::sell)>,
SweepStrong<-1, 1, 5>	  > directionSignal_;
    
    IF() {
	// initialize signals
	directionSignal_.registerFeature(PFeature(new ShortTermReturn<500>()),
PFeature(new ShortTermReturn<1000>()),
PFeature(new ShortTermReturn<1500>()),
PFeature(new ShortTermReturn<2000>()),
PFeature(new ShortTermReturn<5000>()),
PFeature(new ShortTermReturn<20000>()),
PFeature(new ShortTermReturn<30000>()),
PFeature(new ShortTermReturn<180000>()),
PFeature(new TradeDeviationMA<2>()),
PFeature(new TradeDeviationMA<15>()),
PFeature(new SignedVolume<500>()),
PFeature(new SignedVolume<2000>()),
PFeature(new SignedVolume<10000>()),
PFeature(new SignedVolume<600000>()),
PFeature(new SignedVolumeTradeVsMid<500>()),
PFeature(new SignedVolumeTradeVsMid<2000>()),
PFeature(new SignedVolumeTradeVsMid<20000>()),
PFeature(new SignedVolumeWithBookSizeChg<1500>()),
PFeature(new SignedVolumeWithBookSizeChg<20000>()),
PFeature(new BookSizeChg<500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<1000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<1500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<2000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<5000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg<1000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg<1500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg<2000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg<5000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<1000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<5000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<1000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<5000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new SweepStrong<-1, 1, 5>())
					 );

	// register betas
	directionSignal_.registerBeta(-503.633316379124, -191.342431380622, 89.6596129542034, 56.9033888434847, 71.9048609047076, -11.1699923546755, -8.73393866861908, 1.49961837660947, 677.694912405904, -428.211696391004, 0.000156661565115564, -0.000365028210692501, 1.68687482571019e-05, 1.16112942562858e-06, 0.000217420570422348, -0.000143120993161976, 8.17497912740369e-06, -0.00010476677499768, 2.61865884648961e-06, 0.000202816922369306, 0.000255607070378602, 0.000107646698517184, 0.000150306783196828, 0.00024455238543834, -0.000209780658531888, -0.000181503839205139, -0.000237298969859564, -0.00011218656111021, -0.00018545342961414, 0.00192211470028972, 0.00135093769797463, -0.000175601618792672, -0.00220916922022501, -0.00116646578655872, 0.000149083518212382, 0.00419365554456169);

	// register bounds
	directionSignal_.getFeatures()[0]->setBound(true, 0.000308);
directionSignal_.getFeatures()[1]->setBound(true, 0.000409);
directionSignal_.getFeatures()[2]->setBound(true, 0.000493);
directionSignal_.getFeatures()[3]->setBound(true, 0.000573);
directionSignal_.getFeatures()[4]->setBound(true, 0.000911);
directionSignal_.getFeatures()[5]->setBound(true, 0.001836);
directionSignal_.getFeatures()[6]->setBound(true, 0.002234);
directionSignal_.getFeatures()[7]->setBound(true, 0.005421);
directionSignal_.getFeatures()[8]->setBound(true, 0.000172);
directionSignal_.getFeatures()[9]->setBound(true, 0.000063);
directionSignal_.getFeatures()[10]->setBound(true, 209.255328);
directionSignal_.getFeatures()[11]->setBound(true, 353.739630);
directionSignal_.getFeatures()[12]->setBound(true, 737.678113);
directionSignal_.getFeatures()[13]->setBound(true, 6682.951596);
directionSignal_.getFeatures()[14]->setBound(true, 233.276806);
directionSignal_.getFeatures()[15]->setBound(true, 376.370229);
directionSignal_.getFeatures()[16]->setBound(true, 1161.601524);
directionSignal_.getFeatures()[17]->setBound(true, 308.610190);
directionSignal_.getFeatures()[18]->setBound(true, 3118.587741);
directionSignal_.getFeatures()[19]->setBound(true, 150.613039);
directionSignal_.getFeatures()[20]->setBound(true, 173.424326);
directionSignal_.getFeatures()[21]->setBound(true, 185.387478);
directionSignal_.getFeatures()[22]->setBound(true, 189.830574);
directionSignal_.getFeatures()[23]->setBound(true, 207.663233);
directionSignal_.getFeatures()[24]->setBound(true, 142.782217);
directionSignal_.getFeatures()[25]->setBound(true, 163.344086);
directionSignal_.getFeatures()[26]->setBound(true, 173.142882);
directionSignal_.getFeatures()[27]->setBound(true, 179.037942);
directionSignal_.getFeatures()[28]->setBound(true, 194.590807);
directionSignal_.getFeatures()[29]->setBound(true, 78.051522);
directionSignal_.getFeatures()[30]->setBound(true, 122.464637);
directionSignal_.getFeatures()[31]->setBound(true, 349.644633);
directionSignal_.getFeatures()[32]->setBound(true, 77.426574);
directionSignal_.getFeatures()[33]->setBound(true, 120.869206);
directionSignal_.getFeatures()[34]->setBound(true, 339.523577);
directionSignal_.getFeatures()[35]->setBound(true, 2.299421);
    }

	~IF()
	{}
	
    };

}

#endif  // IF_HPP