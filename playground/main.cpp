#include <functional>
#include <iostream>
#include <thread>

#include "Timer.h"

int repeat_count = 10000;

#include "Logger.hpp"
void test_lockless()
{
    Timer t;	

    auto logFile = createLogFile("log.txt");
    LocklessLogger locklessLogger(*logFile);

    std::thread loggingThread(std::ref(locklessLogger));

    t.start();

    for (int i = 0; i < repeat_count; ++i) {
	locklessLogger.push("this is a message");
    }

    t.stop();
    std::cout << "test_lockless Performance: " << t.micro_seconds() / 1004 << std::endl;

    locklessLogger.finalize();
    loggingThread.join();
}

int main(int argc, char** argv)
{
	Timer::Init();

	test_lockless();

	return 0;
}

