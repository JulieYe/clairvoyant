#include <iostream>

using namespace std;

constexpr int operator"" _add(int x)
{
    cout << x << endl;
    return x;
}

int main()
{
    auto i = 5_add;
    cout << i << endl;

    return 0;
}
