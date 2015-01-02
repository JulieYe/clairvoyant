/* 
   Generally manually now. 
   Should be automated generally through regression tool. 
*/

#ifndef BREAKOUT_HPP
#define BREAKOUT_HPP

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
using simulator::TradeBreakOut;

namespace simulator
{
    
    class BreakOut
    {
    public:
	//signal related
	SignalOnPxPredict<TradeBreakOut> directionSignal_;
	
	BreakOut() {
	}

	BreakOut(int numSec, int ntick, int minCount) {
	    setParam(numSec, ntick, minCount);
	}

	void setParam(int numSec, int ntick, int minCount)  {
	    // initialize signals
	    directionSignal_.registerFeature(PFeature(new TradeBreakOut(numSec, ntick, minCount)));
	    // register betas
	    directionSignal_.registerBeta(1.0);
	}

    };

}

#endif  // BreakOut_HPP
