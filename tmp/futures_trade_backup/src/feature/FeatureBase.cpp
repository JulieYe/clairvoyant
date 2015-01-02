//
// File:        $ Feature.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/11/24 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "FeatureBase.hpp"

namespace simulator
{
    Feature::Feature() :
	featureName_("feature"),
	value_(0.0),
	badValue_(0.0),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}
    
    Feature::Feature(string featureName) :
	featureName_(featureName),
	value_(0.0),
	badValue_(0.0),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}

    Feature::Feature(string featureName, double initValue) :
	featureName_(featureName),
	value_(initValue),
	badValue_(0.0),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}
    
    Feature::Feature(string featureName, double initValue, double badValue) :
	featureName_(featureName),
	value_(initValue),
	badValue_(badValue),
	isGood_(false),
	useBound_(false),
	upperBound_(1e6),
	lowerBound_(-1e6)
    {}

    Feature::~Feature()
    {}

    void Feature::setBound(bool useBound, double bound)
    {
	useBound_ = useBound;
	upperBound_ = std::abs(bound);
	lowerBound_ = -1.0 * upperBound_;
    }

}

// FEATUREBASE_CPP
