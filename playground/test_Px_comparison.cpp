#include <iostream>
#include <utility>
#include "../src/Px.hpp"

using namespace std;
using namespace simulator;

int main()
{
    Px px1(11, 0.25);
    Px px2(10.25, 0.25);

    cout << px1.getNumTicks() << endl
	 << px2.getNumTicks() << endl
	 << boolalpha << (px1 < px2) << endl;

    swap(px1, px2);

    cout << px1.getNumTicks() << endl
	 << px2.getNumTicks() << endl
	 << boolalpha << (px1 < px2) << endl;

    return 0;
}
