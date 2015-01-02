#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include <tuple>
#include <cstring>
#include <iostream>
#include <cmath>

using std::vector;
using std::find_if;
using std::sort;
using std::remove_if;
using std::accumulate;
using std::move;
using std::make_tuple;
using std::tuple;
using std::get;
using std::map;
using std::memcmp;
using std::cout;
using std::endl;
using std::fabs;

class OrderBook { 
private:

    const double EPS = 1e-6;
    char BID = 'B';
    char ASK = 'S';

    typedef tuple<int, double, int> element;
    vector<vector<element> > bid_book;
    vector<vector<element> > ask_book;
    map<int, int> id_level_map;
    map<int, char> id_side_map;

public:

    void add(int order_id, char side, double price, int size);
    void modify(int order_id, int new_size);
    void remove(int order_id);
    double get_price(char side, int level);
    int get_size(char side, int level);
};
