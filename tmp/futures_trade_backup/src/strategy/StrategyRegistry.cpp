#include "StrategyRegistry.hpp"
#include "VVDSM.hpp"
#include "LLS.hpp"
#include "VVDSM2.hpp"
//alpha
#include "IF.hpp"
#include "IF30SEC.hpp"
#include "IFDIR.hpp"

namespace simulator
{
    void registerAllStrategies()
    {

	// register VVDSM strategy
	StrategyFactory::instance()->registerByName("VVDSM_IF", new VVDSM<IF>("CFFEX", 300.0, 0.2));
	
	// register LLS strategy
	StrategyFactory::instance()->registerByName("LLS_IFIF", new LLS("CFFEX", 300.0, 0.2, "CFFEX", 0.2));
	
	// register VVDSM2 strategy
	StrategyFactory::instance()->registerByName("VVDSM2_IF", new VVDSM2<IFDIR, IF30SEC>("CFFEX", 300.0, 0.2));


    }

}
