#ifndef SIMPLESTRATEGY_HPP
#define SIMPLESTRATEGY_HPP

#include <string>

namespace simulator
{
    class Strategy;
    class SimulatorCore;
    class Logger;
}

using std::string

namespace simulator
{
    struct BookMessage 
    {
	double bid;
	double ask;
	int bidSize;
	int askSize;
    };

    struct TradeMessage
    {
	int cumVolume;
	double lastPrice;
    };
    
    
    class SimpleStrategy : public Strategy
    {
    public:
	SimpleStrategy(const string strat_name, SimulatorCore * simcore_ptr, Logger * logger_ptr);
	virtual ~SimpleStrategy();
	
    public:
	void addContract(string ex, string con, double mult);
	void logStatus();
	virtual void process();
	
	// helpsers
	bool getCurrentBookUpdate(int prodid, BookMessage &bm);
	bool getCurrentTradeUpdate(int prodid, TradeMessage &tm);
	void cancelALimitOrder(int orderId, int prodid);
	void placeAMarketOrder(bool isBuy, int ordersize, int prodid);
	void placeALimitOrder(double price, double tickSize, bool isBuy, int ordersize, int prodid);
    };
    
}


#endif
