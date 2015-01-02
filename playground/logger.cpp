#include "../src/Logger.hpp"
#include <boost/lexical_cast.hpp>
#include <thread>

#include "Timer.h"

using simulator::Logger;

void test_lockless()
{
    Timer t;	

    Logger logger("test.log");
    logger.start();

    t.start();

    for(int ii=0; ii<10000; ii++) {
	logger.push("this is a message");
    }

    t.stop();
    std::cout << "our logger Performance: " << t.micro_seconds() / 1004 << std::endl;

    logger.end();
}

int main() 
{
    Timer::Init();

    test_lockless();

    return 0;
}



