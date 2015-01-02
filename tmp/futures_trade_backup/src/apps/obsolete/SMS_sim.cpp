//
// File:        $ SMS_sim.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/08/19 00:00:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2013 by Zhengzhang Futures Co. Ltd.

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

#include "CFFEX.hpp"
#include "SMS.hpp"
#include "SimulatorCore.hpp"
#include "ContractSpec.hpp"
#include "ConfigSpec.hpp"

using namespace simulator;

int main(int argc, char ** argv)
{
    if(argc < 9) {
	std::cout << "usage:" << std::endl;
	std::cout << "SMS_sim [config] [data_file] [YYYYMMDD] [exchange] [contract] [log_folder] [threshold] [ordersize]" << std::endl;
	return(0);
    }

    string config_filename(argv[1]);
    string filename(argv[2]);
    string date(argv[3]);
    string exchange(argv[4]);
    string contract(argv[5]);
    string log_folder(argv[6]);
    double threshold(strtod(argv[7], NULL));
    int order_size(atoi(argv[8]));


    SimulatorCore sc(date+"T08:59:00.0", date+"T15:16:00.0", 500, "millisecond");
    
    Logger logger(log_folder + "/sms_" + date + "_" + contract + ".log");
    logger.start();

    ConfigReader cr(config_filename);
    string tmpConfig = log_folder + "/sms_" + date + "_" + contract + ".cfg";
    cr.setValue("Exchange", exchange);
    cr.setValue("Contract", contract);
    cr.setValue("SignalThreshold", threshold);
    cr.setValue("OrderSize", order_size);
    cr.writeConfig(tmpConfig);

    CTPEXCH exch(exchange, contract, filename, cr.getDouble("TickSize"), &sc);

    SMS sms(tmpConfig, &sc, &logger);

    //sms.setThreshold(threshold);
    //sms.setOrderSize(order_size);

    sc.registerMatchy(&exch);
    sc.registerStrategy(&sms);
		
    while (sc.next()) {}
    logger.end();
}
