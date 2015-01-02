#include <iostream>
using std::cout;
using std::endl;

#include "../src/Converter.hpp"

using simulator::combineToTimestampString;
using simulator::ConverterMessage;
using simulator::ConverterOrder;

int main()
{
    cout << combineToTimestampString("20121228", "15:57:00", 50) << endl;

    ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 5, 2, 10> myConverter_5level;
    ConverterMessage<CThostFtdcDepthMarketDataField *, true, true, 1, 2, 10> myConverter_1level;

    ConverterOrder<CThostFtdcInputOrderField *> myConverter;

    return 0;
}
