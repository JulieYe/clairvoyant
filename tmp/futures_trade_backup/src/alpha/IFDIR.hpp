/* 
   Generally manually now. 
   Should be automated generally through regression tool. 
*/

#ifndef IFDIR_HPP
#define IFDIR_HPP

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

    class IFDIR
    {
    public:
	//signal related
	SignalOnPxPredict<		FeatureProd<BookPressure, ShortTermReturn<5000> >,
FeatureProd<BookPressure, ShortTermReturn<30000> >,
FeatureProd<BookPressure, TradeDeviationMA<2> >,
FeatureProd<BookPressure, TradeDeviationMA<10> >,
FeatureProd<BookPressure, SignedVolume<2000> >,
FeatureProd<BookPressure, SignedVolume<60000> >,
FeatureProd<BookPressure, SignedVolume<300000> >,
FeatureProd<ShortTermReturn<500>, ShortTermReturn<5000> >,
FeatureProd<ShortTermReturn<500>, ShortTermReturn<30000> >,
FeatureProd<TradeDeviationMA<2>, TradeDeviationMA<10> >,
FeatureProd<SignedVolume<500>, SignedVolume<2000> >,
FeatureProd<SignedVolume<500>, SignedVolume<60000> >	  > directionSignal_;
    
    IFDIR() {
	// initialize signals
	directionSignal_.registerFeature(PFeature(new FeatureProd<BookPressure, ShortTermReturn<5000> >()),
PFeature(new FeatureProd<BookPressure, ShortTermReturn<30000> >()),
PFeature(new FeatureProd<BookPressure, TradeDeviationMA<2> >()),
PFeature(new FeatureProd<BookPressure, TradeDeviationMA<10> >()),
PFeature(new FeatureProd<BookPressure, SignedVolume<2000> >()),
PFeature(new FeatureProd<BookPressure, SignedVolume<60000> >()),
PFeature(new FeatureProd<BookPressure, SignedVolume<300000> >()),
PFeature(new FeatureProd<ShortTermReturn<500>, ShortTermReturn<5000> >()),
PFeature(new FeatureProd<ShortTermReturn<500>, ShortTermReturn<30000> >()),
PFeature(new FeatureProd<TradeDeviationMA<2>, TradeDeviationMA<10> >()),
PFeature(new FeatureProd<SignedVolume<500>, SignedVolume<2000> >()),
PFeature(new FeatureProd<SignedVolume<500>, SignedVolume<60000> >())
					 );

	// register betas
	directionSignal_.registerBeta(-393.267000319252, -60.9728359621795, 2398.32791188866, -4029.95956653031, 0.000320585147241799, 3.41709651531148e-05, 2.16248212046764e-05, 1661011.7517011, -238296.040090161, 145987114.106634, 5.45729914358296e-06, -3.25708179775333e-07);

	// register bounds
	directionSignal_.getFeatures()[0]->setBound(true, 0.000264);
directionSignal_.getFeatures()[1]->setBound(true, 0.000670);
directionSignal_.getFeatures()[2]->setBound(true, 0.000052);
directionSignal_.getFeatures()[3]->setBound(true, 0.000023);
directionSignal_.getFeatures()[4]->setBound(true, 103.949846);
directionSignal_.getFeatures()[5]->setBound(true, 578.107236);
directionSignal_.getFeatures()[6]->setBound(true, 1394.191372);
directionSignal_.getFeatures()[7]->setBound(true, 0.000000);
directionSignal_.getFeatures()[8]->setBound(true, 0.000000);
directionSignal_.getFeatures()[9]->setBound(true, 0.000000);
directionSignal_.getFeatures()[10]->setBound(true, 92130.890813);
directionSignal_.getFeatures()[11]->setBound(true, 249947.362966);
    }

	~IFDIR()
	{}
	
    };

}

#endif  // IFDIR_HPP