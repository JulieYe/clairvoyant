//
// File:        $ SimMain.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/11/30 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;

#include "SimulatorCore.hpp"
#include "ObjectFactory.hpp"
#include "StrategyRegistry.hpp"
#include "MatchyRegistry.hpp"
#include "INIReader.hpp"
#include "Logger.hpp"

using namespace simulator;

int main(int argc, char ** argv)
{
    if(argc < 2) {
	std::cout << "usage: SimMain config.cfg" << std::endl;
	return(0);
    }
    string config(argv[1]);
    INIReader iniconfig(config);

    registerAllSimMatchies();
    registerAllStrategies();
    
    // reading main section
    string dateStr = iniconfig.Get("Main", "Date", "");
    string startTimeStr = iniconfig.Get("Main", "StartTime", "08:59");
    string endTimeStr = iniconfig.Get("Main", "EndTime", "15:16");
    int interval = iniconfig.GetInteger("Main", "Interval", 500);
    string timeUnit = iniconfig.Get("Main", "TimeUnit", "millisecond");
    string matchiesStr = iniconfig.Get("Main", "Matchy", "");
    string strategiesStr = iniconfig.Get("Main", "Strategy", "");

    SimulatorCore sc(dateStr + "T" + startTimeStr + ":00.0", dateStr + "T" + endTimeStr + ":00.0",
		     interval, timeUnit);
    

    // reading Matchy sections
    stringstream ss(matchiesStr);
    string matchyName;
    while(ss.good()) {
	getline(ss, matchyName, ',');
	std::cout << "creating Matchy " << matchyName << std::endl;
	SimMatchy * pMatchy = SimMatchyFactory::instance()->getByName(matchyName);
	pMatchy->setSimulatorCore(&sc);
	pMatchy->getParamFromConfig(iniconfig, matchyName);
	sc.registerMatchy(pMatchy);
    }
    
    // reading strategy sections
    stringstream sss(strategiesStr);
    string strategyName;
    vector<Logger *> loggerVec;
    while(sss.good()) {
	getline(sss, strategyName, ',');
	std::cout << "creating Strategy " << strategyName << std::endl;
	Strategy * pStrategy = StrategyFactory::instance()->getByName(strategyName);
	pStrategy->setSimulatorCore(&sc);
	pStrategy->getParamFromConfig(iniconfig, strategyName);
	sc.registerStrategy(pStrategy);
	string loggerFolder = iniconfig.Get(strategyName, "LoggerOutputFolder", "");
	if(loggerFolder != "") {
	    string loggerFile = loggerFolder + "/sim_" + dateStr + "_" + strategyName + ".log";
	    Logger * pLogger = new Logger(loggerFile);
	    pLogger->start();
	    pStrategy->setLogger(pLogger);
	    loggerVec.push_back(pLogger);
	}
    }
    
    // run through data
    while (sc.next()) { }
    
    // release logger
    for(vector<Logger *>::iterator it = loggerVec.begin();
	it != loggerVec.end(); it++) {
	(*it)->end();
	delete (*it);
    }

    // write final out put
    string output = iniconfig.Get("Main", "OutputFile", "");
    if(output != "") {
	ofstream ff;
	ff.open(output);
	stringstream ssss(strategiesStr);
	bool headerWritten = false;
	while(ssss.good()) {
	    getline(ssss, strategyName, ',');
	    Strategy *pStrategy = StrategyFactory::instance()->getByName(strategyName);
	    if(!headerWritten)  {
		ff << pStrategy->getStrategyStatusHeader(",date,strategy") << std::endl;
		headerWritten = true;
	    }
	    ff << pStrategy->getStrategyStatus(","+dateStr+","+strategyName) << std::endl;
	}
	ff.close();
    }
    std::cout << "done" << std::endl;
}
