#include <cstdlib>
#include <cstdio>
#include <ctime>

#include <string>
#include <list>
#include <iostream>
#include <chrono>
#include <regex>
#include <exception>
#include "../src/SimulatorCore.hpp"

using std::string;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::duration;
using std::nano;
using std::list;
using std::regex;
using std::smatch;
using std::regex_match;
using std::cout;
using std::endl;
using std::boolalpha;
using std::exception;
using simulator::SimulatorCore;

int main()
{
    string tz = "TZ=Asia/Shanghai";
    putenv(const_cast<char *>(tz.c_str()));
    tzset();
    tm datetime;
    datetime.tm_sec = 0;
    datetime.tm_min = 15;
    datetime.tm_hour = 9;
    datetime.tm_mday = 29;
    datetime.tm_mon = 10 - 1;
    datetime.tm_year = 2012 - 1900;
    
    time_t tv = mktime(&datetime);

    cout << asctime(&datetime);
    cout << "Epoch since 1900: " << tv << endl;

    duration<long, nano> two_nanosecond(2);
    cout << duration<long, nano>::period::den << endl;
    cout << duration<long, nano>::period::num << endl;

    time_point<system_clock, duration<long, nano> > tp(duration<long, nano>(tv * duration<long, nano>::period::den / duration<long, nano>::period::num));
    cout << tp.time_since_epoch().count() << endl;
    
    cout << SimulatorCore::convertTimePointToString(tp) << endl;

    time_point<system_clock, duration<long, nano> > tp1 = tp + two_nanosecond;
    cout << tp1.time_since_epoch().count() << endl;
    cout << SimulatorCore::convertTimePointToString(tp1) << endl;


    cout << boolalpha << (tp < tp1) << endl;
    cout << (tp1 - tp).count() << endl;

    cout << SimulatorCore::convertStringToTimePoint("2012-08-15T09:14:00.250").time_since_epoch().count() << endl;
    cout << SimulatorCore::convertStringToTimePoint("2012-08-15T09:15:00.250").time_since_epoch().count() << endl;

    cout << (tp1 > tp) << endl;

    return 0;
}
