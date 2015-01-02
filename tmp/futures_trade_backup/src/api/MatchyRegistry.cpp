#include "MatchyRegistry.hpp"

#include "CFFEX.hpp"
#include "SimCFFEXAlt.hpp"

namespace simulator
{
    void registerAllMatchies()
    {
#ifdef PRODUCTION       
	// active contract in IF
	MatchyFactory::instance()->registerByName("CFFEX_IF1", new CTPEXCH<1, 5>("CFFEX"));
	// second active contract in IF
	MatchyFactory::instance()->registerByName("CFFEX_IF2", new CTPEXCH<1, 5>("CFFEX"));
#endif
    }
    
    void registerAllSimMatchies()
    {
#ifdef SIMULATION
	SimMatchyFactory::instance()->registerByName("CFFEX_IF1", new CTPEXCH("CFFEX", 0.2));
	SimMatchyFactory::instance()->registerByName("CFFEX_IF2", new CTPEXCH("CFFEX", 0.2));
	SimMatchyFactory::instance()->registerByName("CFFEXAlt_IF1", new SimCFFEXAlt("CFFEX", 0.2));
	SimMatchyFactory::instance()->registerByName("CFFEXAlt_IF2", new SimCFFEXAlt("CFFEX", 0.2));
#endif
    }
}
