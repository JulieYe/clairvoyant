#include <atomic>
#include <iostream>
using namespace std;

int main()
{
    atomic_long x(0);
    long y = x.fetch_add(2);
    long z = x++;
    cout << "x is " << x.load() << endl;
    cout << "y is " << y << endl;
    cout << "z is " << z << endl;

    return 0;
}
