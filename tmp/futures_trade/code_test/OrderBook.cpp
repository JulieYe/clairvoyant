#include "OrderBook.hpp"

void OrderBook::add(int order_id, char side, double price, int size) 
{
    bool bidQ = memcmp(&side, &BID, sizeof(char)) == 0;
    // some safety check here
    //bool askQ = memcmp(&side, &ASK, sizeof(char)) == 0;
    //if (!bidQ && !askQ) return;

    vector<vector<element> > * bookPtr = bidQ ? &bid_book : &ask_book;
    auto it = find_if((*bookPtr).begin(), 
		      (*bookPtr).end(), 
		      [&](vector<element> v) { return fabs(price - get<1>(v[0])) <= EPS; });
    if (it != (*bookPtr).end()) {
	it->push_back(make_tuple(order_id, price, size));
	id_level_map[order_id] = it - (*bookPtr).begin();
    }
    else {
	vector<element> v = {make_tuple(order_id, price, size)};
	(*bookPtr).push_back(move(v));
	// sort the outside vector based on ascending prices for ask, descending prices for bid
	sort((*bookPtr).begin(), 
	     (*bookPtr).end(), 
	     bidQ ? ([&](vector<element> l, vector<element> r) { return get<1>(l[0]) > get<1>(r[0]); }) : 
	     ([&](vector<element> l, vector<element> r) { return get<1>(l[0]) < get<1>(r[0]); }));
	auto it = find_if((*bookPtr).begin(), 
			  (*bookPtr).end(), 
			  [&](vector<element> v) { return fabs(price - get<1>(v[0])) <= EPS; });
	id_level_map[order_id] = it - (*bookPtr).begin();
    }
    id_side_map[order_id] = side;
}

void OrderBook::modify(int order_id, int new_size) { 
    // new_size is not zero
    //if (new_size == 0) return;
    auto it_level = id_level_map.find(order_id);
    auto it_side = id_side_map.find(order_id);
    if (it_level != id_level_map.end() && it_side != id_side_map.end()) { 
	vector<element> * ptr = memcmp(&(it_side->second), &BID, sizeof(char)) == 0 ? &(bid_book[it_level->second]) : &(ask_book[it_level->second]);
	auto it = find_if((*ptr).begin(), 
			  (*ptr).end(), 
			  [&](element e) { return order_id == get<0>(e); });
	(*ptr)[it-(*ptr).begin()] = make_tuple(order_id, get<1>(*it), new_size);
    }
}

void OrderBook::remove(int order_id) { 
    auto it_level = id_level_map.find(order_id);
    auto it_side = id_side_map.find(order_id);
    if (it_level != id_level_map.end() && it_side != id_side_map.end()) { 
	bool bidQ = memcmp(&(it_side->second), &BID, sizeof(char)) == 0;
	vector<element> * ptr = bidQ ? &(bid_book[it_level->second]) : &(ask_book[it_level->second]);
	(*ptr).erase(remove_if((*ptr).begin(), 
			       (*ptr).end(), 
			       [&](element e) { return order_id == get<0>(e); }), 
		     (*ptr).end());
	id_level_map.erase(order_id);
	id_side_map.erase(order_id);
	if (ptr->empty()) { 
	    if (bidQ) { 
		bid_book.erase(bid_book.begin() + it_level->second);
		for (int i = it_level->second + 1; i < bid_book.size() + 1; ++i) {
		    for (auto each : bid_book[i]) {
			id_level_map[get<0>(each)] -= 1;
		    }
		}
	    } else {
		ask_book.erase(ask_book.begin() + it_level->second);
		for (int i = it_level->second + 1; i < ask_book.size() + 1; ++i) {
		    for (auto each : ask_book[i]) {
			id_level_map[get<0>(each)] -= 1;
		    }
		}
	    }
	}
    }
}

double OrderBook::get_price(char side, int level) { 
    bool bidQ = memcmp(&side, &BID, sizeof(char)) == 0;
    if (bidQ && (bid_book.size() >= level)) {
	return get<1>(bid_book[level-1][0]);
    } else if (!bidQ && ask_book.size() >= level) { 
	return get<1>(ask_book[level-1][0]);
    } else {
	return 0.0;
    }
}
 
int OrderBook::get_size(char side, int level) { 
    bool bidQ = memcmp(&side, &BID, sizeof(char)) == 0;
    if (bidQ && bid_book.size() >= level) {
	return accumulate(bid_book[level-1].begin(),
			  bid_book[level-1].end(),
			  0,
			  [&](int size, element e) { return size + get<2>(e); });
    } else if (!bidQ && ask_book.size() >= level) { 
	return accumulate(ask_book[level-1].begin(),
			  ask_book[level-1].end(),
			  0,
			  [&](int size, element e) { return size + get<2>(e); });
    } else {
	return 0;
    }
}


