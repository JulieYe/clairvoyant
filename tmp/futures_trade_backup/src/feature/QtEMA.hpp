//
// File:        $ QtEMA.hpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/03/31 00:00:00 $
//
// ID:          $ fye $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#ifndef QTEMA_HPP
#define QTEMA_HPP

#include <cmath>
#include <cstring>

using std::exp;
using std::pow;
using std::fabs;
using std::memcpy;

namespace simulator
{

    /* n-iterative ema calculator */
    class QtEMA 
    {
    public:
	enum class IntrpType {Previous, Linear, Next};

	QtEMA(int n, double tau, double t0, double z0, IntrpType i_type);
	virtual ~QtEMA();

	virtual void update(double t, double z);

	inline double operator() () const { return(ema_); }
	inline double time() const { return(tn_); }
	inline double tau() const { return(tau_); }

    protected:
	int n_;
	double tau_;
	IntrpType type_;

	double tn_, ema_;
	double * ze_;  // iterative states from index 1 to n.

	void calc(double t, double z);

	inline void set(double tn, double ema) { tn_ = tn; ema_ = ema; }
    };

    /* n-MA calculator */
    class QtMA : public QtEMA
    {
    public:
    	QtMA(int n, double tau, double t0, double z0, QtEMA::IntrpType i_type);
	
    	virtual void update(double t, double z);
    	inline double tau() const { return(tau_ * (n_ + 1) / 2); }
    };

    /* ema calculator for p-degree norm */
    class QtMNorm : public QtMA
    {
    public:
    	QtMNorm(int n, double p, double tau, double t0, double z0, QtEMA::IntrpType i_type);

    	virtual void update(double t, double z);

    private:
    	double p_;
    };

    /* ema calculator for p-degree variance */
    class QtMVar : public QtMA
    {
    public:
    	QtMVar(int n, double p, double tau, double t0, double z0, QtEMA::IntrpType i_type);

    	virtual void update(double t, double z);
    	double stdev();

    private:
    	double p_;
    	QtMA ma_;
    };

    /* EMA differential operator */
    class QtEMAdT
    {
    public:
    	QtEMAdT(double tau, double t0, double z0, double gamma = 1.22208, double beta = 0.65);

    	void update(double t, double z);

    	inline double value() const { return(gamma_*(ema1_() + ema2_() - 2.0*ema4_())); }

    	inline double time() const { return(ema1_.time()); }

    private:
    	double gamma_, beta_, alpha_;
    	QtEMA ema1_, ema2_, ema4_;
    };

}

#endif  // QTEMA_HPP
