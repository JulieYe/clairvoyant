/* 
   Generally manually now. 
   Should be automated generally through regression tool. 
*/

#ifndef RU_HPP
#define RU_HPP

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

    class RU
    {
    public:
	//signal related
	SignalOnPxPredict<		BookPressureNL<1, 4>,
ShortTermReturn<500>,
ShortTermReturn<1000>,
ShortTermReturn<5000>,
ShortTermReturn<20000>,
ShortTermReturn<50000>,
ShortTermReturn<180000>,
TradeDeviationMA<2>,
TradeDeviationMA<120>,
TradeDeviationMA<1440>,
SignedVolume<1000>,
SignedVolume<2000>,
SignedVolume<10000>,
SignedVolume<360000>,
SignedVolumeTradeVsMid<500>,
SignedVolumeTradeVsMid<2000>,
SignedVolumeTradeVsMid<20000>,
SignedVolumeTradeVsMid<360000>,
SignedVolumeTradeVsMid<720000>,
SignedVolumeWithBookSizeChg<500>,
SignedVolumeWithBookSizeChg<2000>,
SignedVolumeWithBookSizeChg<10000>,
SignedVolumeWithBookSizeChg<20000>,
SignedVolumeWithBookSizeChg<360000>,
SignedVolumeWithBookSizeChg<720000>,
VwapMidReturn<1500>,
VwapMidReturn<20000>,
BookSizeChg<1500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg<2000, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<1500, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<5000, static_cast<int>(BuySellSide::buy)>,
BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<1000, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<2000, static_cast<int>(BuySellSide::sell)>,
TradeBreakOut,
TradeBreakOut,
TradeBreakOut,
TradeBreakOut,
TradeBreakOut,
TradeBreakOut	  > directionSignal_;
    
    RU() {
	// initialize signals
	directionSignal_.registerFeature(PFeature(new BookPressureNL<1, 4>()),
PFeature(new ShortTermReturn<500>()),
PFeature(new ShortTermReturn<1000>()),
PFeature(new ShortTermReturn<5000>()),
PFeature(new ShortTermReturn<20000>()),
PFeature(new ShortTermReturn<50000>()),
PFeature(new ShortTermReturn<180000>()),
PFeature(new TradeDeviationMA<2>()),
PFeature(new TradeDeviationMA<120>()),
PFeature(new TradeDeviationMA<1440>()),
PFeature(new SignedVolume<1000>()),
PFeature(new SignedVolume<2000>()),
PFeature(new SignedVolume<10000>()),
PFeature(new SignedVolume<360000>()),
PFeature(new SignedVolumeTradeVsMid<500>()),
PFeature(new SignedVolumeTradeVsMid<2000>()),
PFeature(new SignedVolumeTradeVsMid<20000>()),
PFeature(new SignedVolumeTradeVsMid<360000>()),
PFeature(new SignedVolumeTradeVsMid<720000>()),
PFeature(new SignedVolumeWithBookSizeChg<500>()),
PFeature(new SignedVolumeWithBookSizeChg<2000>()),
PFeature(new SignedVolumeWithBookSizeChg<10000>()),
PFeature(new SignedVolumeWithBookSizeChg<20000>()),
PFeature(new SignedVolumeWithBookSizeChg<360000>()),
PFeature(new SignedVolumeWithBookSizeChg<720000>()),
PFeature(new VwapMidReturn<1500>()),
PFeature(new VwapMidReturn<20000>()),
PFeature(new BookSizeChg<1500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg<2000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<1500, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<5000, static_cast<int>(BuySellSide::buy)>()),
PFeature(new BookSizeChg2<500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<1000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<2000, static_cast<int>(BuySellSide::sell)>()),
					 PFeature(new TradeBreakOut(60,1,2)),
					 PFeature(new TradeBreakOut(60,2,5)),
					 PFeature(new TradeBreakOut(120,2,3)),
					 PFeature(new TradeBreakOut(120,2,5)),
					 PFeature(new TradeBreakOut(300,1,2)),
					 PFeature(new TradeBreakOut(300,1,5))
					 );

	// register betas
	directionSignal_.registerBeta(6.7777030768613, -4232.87954837575, -1750.54487424672, -392.735656798728, -353.759117645874, -424.024917865134, 115.262480084596, 3207.42705878011, -4047.33964135729, 10434.7374797211, -0.0055592134678293, -0.001154957732867, -0.000370264642639497, -0.000148251599390406, 0.00400093348944143, 0.000701668926452742, 0.000305977833351903, 0.000102980801243244, -7.09807186160507e-05, 0.0319057086166572, -0.00257371764761666, -0.000664774736351566, 0.000391623981520395, 2.049597834685e-05, -1.00094331372615e-05, -985.925976003158, 960.676275690244, 0.00361361317521295, -0.00250798035061881, 0.0327718089091488, 0.0132072738413368, 0.00346801922927213, -0.0146004864832663, -0.0170067392845204, -0.0105904031667543, 1.27069051716339, 308.106754559945, 42.9720705786374, -191.662185455181, 4.10426645104527, -5.23518664460304);

	// register bounds
	directionSignal_.getFeatures()[0]->setBound(true, 0.501343);
directionSignal_.getFeatures()[1]->setBound(true, 0.000420);
directionSignal_.getFeatures()[2]->setBound(true, 0.000551);
directionSignal_.getFeatures()[3]->setBound(true, 0.001168);
directionSignal_.getFeatures()[4]->setBound(true, 0.002300);
directionSignal_.getFeatures()[5]->setBound(true, 0.003586);
directionSignal_.getFeatures()[6]->setBound(true, 0.006654);
directionSignal_.getFeatures()[7]->setBound(true, 0.000427);
directionSignal_.getFeatures()[8]->setBound(true, 0.000088);
directionSignal_.getFeatures()[9]->setBound(true, 0.000029);
directionSignal_.getFeatures()[10]->setBound(true, 211.165546);
directionSignal_.getFeatures()[11]->setBound(true, 285.085395);
directionSignal_.getFeatures()[12]->setBound(true, 596.711390);
directionSignal_.getFeatures()[13]->setBound(true, 4109.324254);
directionSignal_.getFeatures()[14]->setBound(true, 208.343722);
directionSignal_.getFeatures()[15]->setBound(true, 344.211692);
directionSignal_.getFeatures()[16]->setBound(true, 3103.202496);
directionSignal_.getFeatures()[17]->setBound(true, 13704.816588);
directionSignal_.getFeatures()[18]->setBound(true, 19415.305473);
directionSignal_.getFeatures()[19]->setBound(true, 61.431685);
directionSignal_.getFeatures()[20]->setBound(true, 252.620113);
directionSignal_.getFeatures()[21]->setBound(true, 2297.784532);
directionSignal_.getFeatures()[22]->setBound(true, 3470.054595);
directionSignal_.getFeatures()[23]->setBound(true, 32878.396520);
directionSignal_.getFeatures()[24]->setBound(true, 60505.490419);
directionSignal_.getFeatures()[25]->setBound(true, 0.000570);
directionSignal_.getFeatures()[26]->setBound(true, 0.001227);
directionSignal_.getFeatures()[27]->setBound(true, 173.406568);
directionSignal_.getFeatures()[28]->setBound(true, 185.261214);
directionSignal_.getFeatures()[29]->setBound(true, 62.632588);
directionSignal_.getFeatures()[30]->setBound(true, 117.928894);
directionSignal_.getFeatures()[31]->setBound(true, 241.275154);
directionSignal_.getFeatures()[32]->setBound(true, 64.181111);
directionSignal_.getFeatures()[33]->setBound(true, 95.143159);
directionSignal_.getFeatures()[34]->setBound(true, 146.408857);
directionSignal_.getFeatures()[35]->setBound(true, 0.522337);
directionSignal_.getFeatures()[36]->setBound(true, 0.031328);
directionSignal_.getFeatures()[37]->setBound(true, 0.071209);
directionSignal_.getFeatures()[38]->setBound(true, 0.042418);
directionSignal_.getFeatures()[39]->setBound(true, 0.326319);
directionSignal_.getFeatures()[40]->setBound(true, 0.261168);
    }

	~RU()
	{}
	
    };

}

#endif  // RU_HPP
