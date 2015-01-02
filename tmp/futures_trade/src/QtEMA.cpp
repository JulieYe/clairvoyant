//
// File:        $ QtEMA.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/31 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include "QtEMA.hpp"

namespace simulator
{

    QtEMA::QtEMA(int n, double tau, double t0, double z0, IntrpType i_type)
	: n_(n), tau_(tau), type_(i_type), tn_(t0), ema_(z0), ze_(new double[2*n+2])
    {
	// initialize to 0
	for (int i = 0; i <= n; ++i) {
	    ze_[i] = z0;
	}
    }

    QtEMA::~QtEMA()
    {
	delete [] ze_;
    }

    void QtEMA::calc(double t, double z)
    {
	double alpha = (t-tn_)/tau_;
	double mu = exp(-alpha);
	double nu;
	switch (type_) {
	case IntrpType::Previous: 
	    nu = 1.0; break;
	case IntrpType::Linear:
	    nu = (1.0-mu)/alpha; break;
	case IntrpType::Next:
	    nu = mu; break;
	default:
	    ;
	}

	double * zc = &ze_[n_+1];

	zc[0] = z;
	for (int i = 1; i <= n_; ++i) {
	    zc[i] = mu * ze_[i] + (nu - mu) * ze_[i-1] + (1.0 - nu) * zc[i-1];
	}

	memcpy(static_cast<void *>(ze_), static_cast<void *>(zc), (n_+1)*sizeof(ze_[0]));
    }

    // update with the new observation
    void QtEMA::update(double t, double z)
    {
	calc(t, z);
	set(t, ze_[n_]);
    }

    QtMA::QtMA(int n, double tau, double t0, double z0, QtEMA::IntrpType i_type)
    	: QtEMA(n, 2*tau/(n+1), t0, z0, i_type)
    {}

    void QtMA::update(double t, double z)
    {
    	calc(t, z);

    	double sum = 0.0;
    	for (int i = 1; i <= n_; ++i) {
    	    sum += ze_[i];
    	}

    	set(t, sum/n_);
    }

    QtMNorm::QtMNorm(int n, double p, double tau, double t0, double z0, QtEMA::IntrpType i_type)
    	: QtMA(n, tau, t0, pow(fabs(z0), p), i_type), 
    	  p_(p)
    {}

    void QtMNorm::update(double t, double z)
    {
    	QtMA::update(t, pow(fabs(z), p_));
    	set(t, pow(ema_, 1.0/p_));
    }

    QtMVar::QtMVar(int n, double p, double tau, double t0, double z0, QtEMA::IntrpType i_type)
    	: QtMA(n, tau, t0, 0, i_type),
    	  p_(p),
    	  ma_(n, tau, t0, z0, i_type)
    {}

    void QtMVar::update(double t, double z)
    {
    	ma_.update(t, z);
    	QtMA::update(t, pow(fabs(z-ma_()), p_));
    }

    double QtMVar::stdev()
    {
    	return pow(this->operator()(), 1.0/p_);
    }

    QtEMAdT::QtEMAdT(double tau, double t0, double z0, double gamma, double beta)
    	: gamma_(gamma), beta_(beta), alpha_(1.0/(gamma*(8.0*beta-3.0))),
    	  ema1_(1, alpha_*tau, t0, z0, QtEMA::IntrpType::Next),
    	  ema2_(2, alpha_*tau, t0, z0, QtEMA::IntrpType::Next),
    	  ema4_(4, alpha_*beta_*tau, t0, z0, QtEMA::IntrpType::Next)
    {}

    void QtEMAdT::update(double t, double z)
    {
    	ema1_.update(t, z);
    	ema2_.update(t, z);
    	ema4_.update(t, z);
    }

}

// QTEMA_CPP
