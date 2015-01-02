#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;

void dateTimeParser()
{
    char year[5];
    char mon[3];
    char mday[3];
    char hour[3];
    char min[3];
    char sec[3];
    char nanoSec[10];

    string s = "2012-10-29T09:15:00.1234";

    strcpy(year, s.substr(0,4).c_str());
    strcpy(mon, s.substr(5,2).c_str());
    strcpy(mday, s.substr(8,2).c_str());
    strcpy(hour, s.substr(11,2).c_str());
    strcpy(min, s.substr(14,2).c_str());
    strcpy(sec, s.substr(17,2).c_str());
    strcpy(nanoSec, s.substr(20).c_str());

    memset(nanoSec + strlen(nanoSec), '0', 9 - strlen(nanoSec));
    memset(nanoSec + 9, '\0', 1);

    string tz = "TZ=Asia/Shanghai";
    putenv(const_cast<char *>(tz.c_str()));
    tzset();
    tm dt;
    dt.tm_year = atoi(year) - 1900;
    dt.tm_mon = atoi(mon) - 1;
    dt.tm_mday = atoi(mday);
    dt.tm_hour = atoi(hour);
    dt.tm_min = atoi(min);
    dt.tm_sec = atoi(sec);

    time_t tv = mktime(&dt);

    cout << asctime(&dt);

    duration<long, nano> numNanoseconds(atol(nanoSec));
    time_point<system_clock, duration<long, nano> > tp(duration<long, nano>(tv * duration<long, nano>::period::den / duration<long, nano>::period::num));
    cout << tp.time_since_epoch().count() << endl;
    tp += numNanoseconds;
    cout << "numNanoseconds = " << numNanoseconds.count() << endl;
    cout << tp.time_since_epoch().count() << endl;

    time_point<system_clock, duration<long, nano> > tp1 = tp + numNanoseconds;

    cout << boolalpha << (tp1 > tp) << endl;
    cout << std::min(tp, tp1).time_since_epoch().count() << endl;

    cout << boolalpha << ((tp1 - tp).count() == 123400000ULL) << endl;
}

int main()
{
    dateTimeParser();

    return 0;
}
