//
// File:        $ FeatureCollector.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/04/19 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "FeatureCollector.hpp"

namespace simulator
{
    // only supports one contract right now!
    FeatureCollector::FeatureCollector(string exchange, string contract, SimulatorCore * simptr, Logger * logptr) :
	Strategy("FeatureCollector", simptr, logptr), 
	featureSampler_(FeatureSampler()), 
	isSpreadQ_(false)
    {
	addStrat(exchange, contract, 0.0);
    }

    FeatureCollector::FeatureCollector(string exchangeFront, string contractFront, string exchangeBack, string contractBack, SimulatorCore * simptr, Logger * logptr) :
	Strategy("FeatureCollector", simptr, logptr), 
	featureSampler_(FeatureSampler()), 
	isSpreadQ_(true)
    {
	addStrat(exchangeFront, contractFront, 0.0);
	addStrat(exchangeBack, contractBack, 0.0);
    }
    
    void FeatureCollector::addFeature(PFeature pfeature, long delayms)
    {
	FeatureInfo finfo;
	finfo.pFeature_ = pfeature;
	finfo.delayNano_ = duration_nano(delayms * 1000000ULL);
	featureSampler_.featureInfoVector().push_back(finfo);
    }
	
    void FeatureCollector::writeHeader()
    {
	featureSampler_.init();
	stringstream ss;
	ss << "ts,datetime";
	for(auto it = featureSampler_.featureInfoVector().begin();
	    it != featureSampler_.featureInfoVector().end();
	    it++) {
	    ss << ',' << ((*it).pFeature_)->getName();
	    if((*it).delayNano_.count() > 0) {
		ss << "_" << (*it).delayNano_.count() / 1000000ULL << "ms";
	    }
	}
	loggerPtr_->push(ss.str());
    }
	
    void FeatureCollector::process()
    {
	if (!isSpreadQ_) {
	    if (bookUpdates_[0].size() > 0 && tradeUpdates_[0].size() > 0) {
		featureSampler_.update(currTime_);
		auto ibu = bookUpdates_[0].begin();
		auto itu = tradeUpdates_[0].begin();
		while (ibu != bookUpdates_[0].end() && itu != tradeUpdates_[0].end()) {
		    featureSampler_.update((*ibu));
		    featureSampler_.update((*itu));
		    featureSampler_.update((*ibu), (*itu));
		    ibu++;
		    itu++;
		}
		featureSampler_.updateData(currTime_);
		string newdata = featureSampler_.flushCompletedVectors(currTime_);
		if (newdata.size() > 1) {
		    newdata = newdata.substr(0,newdata.size() - 1);
		    loggerPtr_->push(newdata);
		}
	    }
	} else {
	    for (int i = 0; i < 2; ++i) {
		if (bookUpdates_[i].size() > 0 && tradeUpdates_[i].size() > 0) {
		    featureSampler_.update(currTime_);
		    auto ibu = bookUpdates_[i].begin();
		    auto itu = tradeUpdates_[i].begin();
		    while (ibu != bookUpdates_[i].end() && itu != tradeUpdates_[i].end()) {
			featureSampler_.update((*ibu), i==0 ? true : false);
			featureSampler_.update((*itu), i==0 ? true : false);
			featureSampler_.update((*ibu), (*itu), i==0 ? true : false);
			ibu++;
			itu++;
		    }
		    featureSampler_.updateData(currTime_);
		    string newdata = featureSampler_.flushCompletedVectors(currTime_);
		    if (newdata.size() > 1) {
			newdata = newdata.substr(0,newdata.size() - 1);
			loggerPtr_->push(newdata);
		    }
		}
	    }
	}
    }

}

// FEATURECOLLECTOR_CPP
