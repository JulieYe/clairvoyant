#include <ctime>
#include <cstdlib>

#include <iostream>

using namespace std;

int main()
{
    char tz[] = "TZ=Asia/Shanghai";
    putenv(const_cast<char *>(tz));
    tzset();

    return 0;
}
