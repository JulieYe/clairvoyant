#include <numeric>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    string ticker = "CFFEX:IF1305";
    vector<string> tickers;
    tickers.push_back("CFFEX:IF1305");
    tickers.push_back("CFFEX:IF1306");

    cout << accumulate(tickers.begin(),
		       tickers.end(),
		       0,
		       [&](int i, string str) { return str == ticker ? (i + 1) : i; }) 
	 << endl;

    return 0;
}
