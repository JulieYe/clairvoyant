#include "ObjectFactory.hpp"

namespace simulator {
    MatchyFactory * MatchyFactory::pFactory_ = NULL;

    SimMatchyFactory * SimMatchyFactory::pFactory_ = NULL;

    StrategyFactory * StrategyFactory::pFactory_ = NULL;

    MatchyFactory * MatchyFactory::instance()
    {
	if(pFactory_ == NULL) 
	    pFactory_ = new MatchyFactory();
	return pFactory_;
    }


    SimMatchyFactory * SimMatchyFactory::instance()
    {
	if(pFactory_ == NULL) 
	    pFactory_ = new SimMatchyFactory();
	return pFactory_;
    }

    StrategyFactory * StrategyFactory::instance()
    {
	if(pFactory_ == NULL) 
	    pFactory_ = new StrategyFactory();
	return pFactory_;
    }
    
}
