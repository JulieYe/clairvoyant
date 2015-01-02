//
// File:        $ FeatureCollector.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/19 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef FEATURECOLLECTOR_HPP
#define FEATURECOLLECTOR_HPP

#include <string>
#include <sstream>

using std::string;
using std::stringstream;

#include "Message.hpp"
#include "SimulatorCore.hpp"
#include "FeatureBase.hpp"
#include "FeatureSampler.hpp"
#include "Strategy.hpp"

using simulator::AbstractMessage;
using simulator::BuySellSide;
using simulator::AbstractMessage;
using simulator::LimitOrderMessage;
using simulator::MarketOrderMessage;
using simulator::CancelOrderMessage;
using simulator::AcknowledgedOrderMessage;
using simulator::FilledOrderMessage;
using simulator::CanceledOrderMessage;
using simulator::RejectedOrderMessage;
using simulator::BookUpdate;
using simulator::TradeUpdate;
using simulator::time_stamp;
using simulator::duration_nano;
using simulator::SimulatorCore;
using simulator::Strategy;
using simulator::FeatureSampler;
using simulator::Feature;
using simulator::FeatureInfo;
using simulator::PFeature;

namespace simulator
{
    class FeatureCollector : public Strategy
    {
    protected:
	FeatureSampler featureSampler_;

    private:
	bool isSpreadQ_;

    public:
	FeatureCollector(string exchange, string contract, SimulatorCore * simptr, Logger * logptr);
	FeatureCollector(string exchangeFront, string contractFont, 
			 string exchangeBack, string contractBack,
			 SimulatorCore * simptr, Logger * logptr);
	void addFeature(PFeature pfeature, long delayms);
	void writeHeader();
	void process();
    };
}

#endif  // FEATURECOLLECTOR_HPP
