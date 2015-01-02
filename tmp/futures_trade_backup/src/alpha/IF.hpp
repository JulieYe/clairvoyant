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
ShortTermReturn<2000>,
ShortTermReturn<5000>,
ShortTermReturn<20000>,
ShortTermReturn<30000>,
ShortTermReturn<180000>,
TradeDeviationMA<2>,
TradeDeviationMA<20>,
SignedVolume<500>,
SignedVolume<1500>,
SignedVolume<2000>,
SignedVolume<10000>,
SignedVolumeTradeVsMid<500>,
SignedVolumeTradeVsMid<2000>,
SignedVolumeWithBookSizeChg<1500>,
SignedVolumeWithBookSizeChg<600000>,
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
BookSizeChg2<1500, static_cast<int>(BuySellSide::sell)>,
BookSizeChg2<5000, static_cast<int>(BuySellSide::sell)>,
SweepStrong<-1, 1, 5>	  > directionSignal_;
    
    IF() {
	// initialize signals
	directionSignal_.registerFeature(PFeature(new ShortTermReturn<500>()),
PFeature(new ShortTermReturn<1000>()),
PFeature(new ShortTermReturn<2000>()),
PFeature(new ShortTermReturn<5000>()),
PFeature(new ShortTermReturn<20000>()),
PFeature(new ShortTermReturn<30000>()),
PFeature(new ShortTermReturn<180000>()),
PFeature(new TradeDeviationMA<2>()),
PFeature(new TradeDeviationMA<20>()),
PFeature(new SignedVolume<500>()),
PFeature(new SignedVolume<1500>()),
PFeature(new SignedVolume<2000>()),
PFeature(new SignedVolume<10000>()),
PFeature(new SignedVolumeTradeVsMid<500>()),
PFeature(new SignedVolumeTradeVsMid<2000>()),
PFeature(new SignedVolumeWithBookSizeChg<1500>()),
PFeature(new SignedVolumeWithBookSizeChg<600000>()),
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
PFeature(new BookSizeChg2<1500, static_cast<int>(BuySellSide::sell)>()),
PFeature(new BookSizeChg2<5000, static_cast<int>(BuySellSide::sell)>()),
PFeature(new SweepStrong<-1, 1, 5>())
					 );

	// register betas
	directionSignal_.registerBeta(-447.264218383769, -215.912853925253, 138.984002825932, 40.6340172893915, -9.48647667238995, -11.1775429954002, 2.06797056318438, 676.490784662649, -439.141535147699, 0.000295409997412098, -0.000205228251760059, -0.000253038742695006, 2.51921693912898e-05, 0.000131470540153949, -0.0001021509324866, -7.89863132908228e-05, -9.7569134364414e-08, 0.000240206758098728, 0.000225356344164922, 0.000136067427634006, 0.000143476698367389, 0.000226308581582735, -0.00022619619408666, -0.000266392332733033, -9.16570846885266e-05, -0.000151329626020801, -0.000215103840726847, 0.00187391916398696, 0.00134569484440097, -0.000130894523285301, -0.00194596640983169, -0.00112147526448152, -0.000255162131921712, 0.000185448031124528, 0.00598695358715516);

	// register bounds
	directionSignal_.getFeatures()[0]->setBound(true, 0.000316);
directionSignal_.getFeatures()[1]->setBound(true, 0.000417);
directionSignal_.getFeatures()[2]->setBound(true, 0.000581);
directionSignal_.getFeatures()[3]->setBound(true, 0.000920);
directionSignal_.getFeatures()[4]->setBound(true, 0.001847);
directionSignal_.getFeatures()[5]->setBound(true, 0.002249);
directionSignal_.getFeatures()[6]->setBound(true, 0.005437);
directionSignal_.getFeatures()[7]->setBound(true, 0.000174);
directionSignal_.getFeatures()[8]->setBound(true, 0.000055);
directionSignal_.getFeatures()[9]->setBound(true, 212.492342);
directionSignal_.getFeatures()[10]->setBound(true, 316.443896);
directionSignal_.getFeatures()[11]->setBound(true, 357.270416);
directionSignal_.getFeatures()[12]->setBound(true, 748.618720);
directionSignal_.getFeatures()[13]->setBound(true, 238.203622);
directionSignal_.getFeatures()[14]->setBound(true, 389.917122);
directionSignal_.getFeatures()[15]->setBound(true, 307.317189);
directionSignal_.getFeatures()[16]->setBound(true, 85677.556542);
directionSignal_.getFeatures()[17]->setBound(true, 151.731736);
directionSignal_.getFeatures()[18]->setBound(true, 173.659182);
directionSignal_.getFeatures()[19]->setBound(true, 183.291717);
directionSignal_.getFeatures()[20]->setBound(true, 189.491732);
directionSignal_.getFeatures()[21]->setBound(true, 206.756474);
directionSignal_.getFeatures()[22]->setBound(true, 145.018453);
directionSignal_.getFeatures()[23]->setBound(true, 163.271036);
directionSignal_.getFeatures()[24]->setBound(true, 173.103796);
directionSignal_.getFeatures()[25]->setBound(true, 178.736255);
directionSignal_.getFeatures()[26]->setBound(true, 193.529011);
directionSignal_.getFeatures()[27]->setBound(true, 77.411900);
directionSignal_.getFeatures()[28]->setBound(true, 119.676550);
directionSignal_.getFeatures()[29]->setBound(true, 339.757490);
directionSignal_.getFeatures()[30]->setBound(true, 76.636165);
directionSignal_.getFeatures()[31]->setBound(true, 118.572076);
directionSignal_.getFeatures()[32]->setBound(true, 154.867264);
directionSignal_.getFeatures()[33]->setBound(true, 333.663440);
directionSignal_.getFeatures()[34]->setBound(true, 2.401403);
    }

	~IF()
	{}
	
    };

}

#endif  // IF_HPP