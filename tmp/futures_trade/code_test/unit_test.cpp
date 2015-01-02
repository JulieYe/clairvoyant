#include "OrderBook.hpp"
#include <iostream>

using namespace::std;

int main() { 
    OrderBook ob;
    ob.add(1, 'B', 45.2, 100);
    ob.modify(1, 50);
    ob.add(2, 'S', 51.4, 200);
    ob.add(3, 'B', 45.1, 100);
    ob.add(4, 'S', 51.2, 300);
    ob.add(5, 'S', 51.2, 200);
    ob.remove(3);
    cout << ob.get_price('B', 1) << endl;
    cout << ob.get_size('B', 1) << endl;
    cout << ob.get_price('S', 1) << endl;
    cout << ob.get_size('S', 1) << endl;

    OrderBook ob2;
    ob2.add(1, 'B', 22.5, 100);
    ob2.add(2, 'S', 37.8, 250);
    ob2.add(3, 'B', 24.7, 150);
    ob2.modify(3, 500);
    ob2.add(4, 'S', 35.1, 250);
    ob2.add(5, 'S', 37.8, 150);
    ob2.remove(3);
    ob2.remove(5);
    ob2.add(3, 'S', 37.8, 150);
    ob2.add(5, 'S', 37.6, 350);
    ob2.add(6, 'B', 24.7, 200);
    ob2.modify(6, 150);
    ob2.add(7, 'S', 35.1, 200);
    ob2.add(8, 'B', 22.5, 350);
    cout << ob2.get_price('B', 1) << endl;
    cout << ob2.get_size('B', 1) << endl;
    cout << ob2.get_price('S', 1) << endl;
    cout << ob2.get_size('S', 1) << endl;
    cout << ob2.get_price('B', 2) << endl;
    cout << ob2.get_size('B', 2) << endl;
    cout << ob2.get_price('S', 2) << endl;
    cout << ob2.get_size('S', 2) << endl;
    cout << ob2.get_price('S', 3) << endl;
    return 0;
}
