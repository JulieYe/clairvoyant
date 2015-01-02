// this is a file that defines alternative SimMatchy for CFFEX
// basically ignores marketable limit order matching
// might be more realistic sim for CFFEX

#ifndef SIM_CFFEX_ALT_HPP
#define SIM_CFFEX_ALT_HPP

#include "CFFEX.hpp"

namespace simulator
{
#ifdef SIMULATION
    class SimCFFEXAlt : public CFFEX
    {
    public:
	SimCFFEXAlt(string, string, SimulatorCore *);
	
	SimCFFEXAlt(string, string, string, double, SimulatorCore *);
	
	SimCFFEXAlt(string exchange, double tickSize);

	virtual void process(int orderId);

    };



#endif
}

#endif
