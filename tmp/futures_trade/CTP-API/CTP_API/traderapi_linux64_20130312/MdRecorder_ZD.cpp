#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>
#include <unistd.h>

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"

using namespace std;
using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::duration;
using std::nano;

typedef time_point<system_clock, duration<unsigned long long, nano> > time_stamp;
typedef duration<unsigned long long, nano> duration_nano;

time_stamp getCurrTime()
{
    timespec tp_;
    clock_gettime(CLOCK_REALTIME, &tp_);
    time_stamp tp(duration_nano(tp_.tv_sec * duration_nano::period::den / duration_nano::period::num));
    duration_nano numNanoseconds(tp_.tv_nsec);
    tp += numNanoseconds;
    return tp;
}

string getCurrDate()
{
    time_stamp tp = getCurrTime();
    unsigned long long nanoSecondsEpoch = tp.time_since_epoch().count();

    char nanosec[10];
    sprintf(nanosec, "%09llu", nanoSecondsEpoch % 1000000000ULL);

    time_t tv = nanoSecondsEpoch / 1000000000ULL;
    tm *dt;
    dt = localtime(&tv);
    
    char year[5];
    char mon[3];
    char mday[3];
    sprintf(year, "%d", dt->tm_year + 1900);
    sprintf(mon, "%02d", dt->tm_mon + 1);
    sprintf(mday, "%02d", dt->tm_mday);
    
    char ret[9];
    memcpy(ret, year, 4);
    memcpy(ret + 4, mon, 2);
    memcpy(ret + 6, mday, 2);
    memset(ret + 8, '\0', 1);
    
    return ret;
}

class MdSpi : public CThostFtdcMdSpi
{
private:
    CThostFtdcMdApi * pMdApi_;
    FILE * pFile_;
    char ** pInstrument_;
    int numContracts_;

public:
    MdSpi(CThostFtdcMdApi *pMdApi, FILE *pFile, char **pInstrument, int numContracts) : pMdApi_(pMdApi), pFile_(pFile), pInstrument_(pInstrument), numContracts_(numContracts)
    {
#ifdef DEBUG
	for (int i = 0; i < numContracts_; ++i)
	    cout << pInstrument_[i] << endl;
#endif
    }
    
    ~MdSpi()
    {}

    virtual void OnFrontConnected()
    {
	CThostFtdcReqUserLoginField reqUserLogin;
	strcpy(reqUserLogin.BrokerID, "5600");
	strcpy(reqUserLogin.UserID, "880015");
	strcpy(reqUserLogin.Password, "830817");
	pMdApi_->ReqUserLogin(&reqUserLogin, 0);
    }

    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to log in, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else if (pRspUserLogin != NULL) {
	    // char * Instrument[] = {"IF1212", "IF1301", "IF1303", "IF1306"};
	    pMdApi_->SubscribeMarketData(pInstrument_, numContracts_);
	} else {
	    cout << "Failed to log in: null pointer pRspUserLogin" << endl;
	    exit(-1);
	}
    }

    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to subscribe market data, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else if (pSpecificInstrument != NULL) {
	    cout << "Subscribed " << pSpecificInstrument->InstrumentID << endl;
	} else {
	    cout << "Failed to subscribe market data: null pointer pSpecificInstrument" << endl;
	    exit(-1);
	}
    }

    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
    {
	fprintf(pFile_, "InstrumentID=[%s], UpdateTime=[%s], UpdateMillisec=[%d], LastPrice=[%f], Volume=[%d], BidPrice1=[%f], BidVolume1=[%d], AskPrice1=[%f], AskVolume1=[%d], Epoch=[%llu]\n", pDepthMarketData->InstrumentID, pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec, pDepthMarketData->Volume > 0 ? pDepthMarketData->LastPrice : 0.0, pDepthMarketData->Volume, pDepthMarketData->BidVolume1 > 0 ? pDepthMarketData->BidPrice1 : 0.0, pDepthMarketData->BidVolume1, pDepthMarketData->AskVolume1 > 0 ? pDepthMarketData->AskPrice1 : 0.0, pDepthMarketData->AskVolume1, getCurrTime().time_since_epoch().count());
	fflush(pFile_);
    }

    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	printf("OnRspError:\n");
	printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    }
};

class TraderSpi : public CThostFtdcTraderSpi
{
private:
    CThostFtdcTraderApi * pTraderApi_;
    TThostFtdcBrokerIDType brokerID_;
    TThostFtdcInvestorIDType userID_;
    vector<string> * pContracts_;

public:
    TraderSpi(CThostFtdcTraderApi *pTraderApi, vector<string> *pContracts) : 
	pTraderApi_(pTraderApi), pContracts_(pContracts)
    {}
    
    ~TraderSpi()
    {}
    
    virtual void OnFrontConnected()
    {
	CThostFtdcReqUserLoginField reqUserLogin;
	memset(&reqUserLogin, 0, sizeof(reqUserLogin));
	strcpy(brokerID_, "5600");
	strcpy(reqUserLogin.BrokerID, brokerID_);
	strcpy(userID_, "880015");
	strcpy(reqUserLogin.UserID, userID_);
	strcpy(reqUserLogin.Password, "830817");
	pTraderApi_->ReqUserLogin(&reqUserLogin, 0);
    }

    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to log in, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else if (pRspUserLogin != NULL) {
	    CThostFtdcQrySettlementInfoField settlementInfo;
	    memset(&settlementInfo, 0, sizeof(settlementInfo));
	    strcpy(settlementInfo.BrokerID, brokerID_);
	    strcpy(settlementInfo.InvestorID, userID_);
	    strcpy(settlementInfo.TradingDay, pTraderApi_->GetTradingDay());
	    pTraderApi_->ReqQrySettlementInfo(&settlementInfo, 0);
	} else {
	    cout << "Failed to log in: null pointer pRspUserLogin" << endl;
	    exit(-1);
	}
    }

    void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to query settlement info, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else if (bIsLast) {
	    sleep(1); // have to sleep some time, pretend to confirm settlement information..
	    /* Confirm settlement info. */
	    CThostFtdcSettlementInfoConfirmField settlementInfoConfirm;
	    memset(&settlementInfoConfirm, 0, sizeof(settlementInfoConfirm));
	    strcpy(settlementInfoConfirm.BrokerID, brokerID_);
	    strcpy(settlementInfoConfirm.InvestorID, userID_);
	    pTraderApi_->ReqSettlementInfoConfirm(&settlementInfoConfirm, 0);
	}
    }

    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	if (pRspInfo != NULL && pRspInfo->ErrorID != 0) {
	    OnRspError(pRspInfo, nRequestID, bIsLast);
	    printf("Failed to confirm settlement info, errorcode=%d, errormsg=%s, requestid=%d, chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
	    exit(-1);
	} else {
	    CThostFtdcQryInstrumentField req;
	    memset(&req, 0, sizeof(req));
	    pTraderApi_->ReqQryInstrument(&req, ++nRequestID);
	}
    }

    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	pContracts_->push_back(pInstrument->InstrumentID);
	cout << pInstrument->InstrumentID << ":" 
	     << pInstrument->ExchangeID << ":" 
	     << pInstrument->InstrumentName << ":" 
	     << pInstrument->ExchangeInstID << ":" 
	     << pInstrument->ProductID << ":" 
	     << pInstrument->DeliveryYear << ":" 
	     << pInstrument->DeliveryMonth 
	     << endl;
    }

    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
	printf("OnRspError:\n");
	printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
    }
};

int main(int argc, char ** argv)
{
    vector<string> contracts;
    CThostFtdcTraderApi *pTraderApi = (argc == 1) ? 
	CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/traderapi_linux64/Trader_day_con/") : 
	CThostFtdcTraderApi::CreateFtdcTraderApi("/home/mingyuan/packages/traderapi_linux64/Trader_night_con/");
    TraderSpi traderSpi(pTraderApi, &contracts);
    pTraderApi->RegisterSpi(&traderSpi);
    pTraderApi->SubscribePublicTopic(THOST_TERT_RESUME);
    pTraderApi->SubscribePrivateTopic(THOST_TERT_RESUME);
    string registerFrontAddressPortTrader("tcp://10.21.108.1:41205");
    pTraderApi->RegisterFront(const_cast<char *>(registerFrontAddressPortTrader.c_str()));
    pTraderApi->Init();
    sleep(10); // Wait for login.

    char * pInstrument[contracts.size()];
    for (unsigned i = 0; i < contracts.size(); ++i)
	pInstrument[i] = const_cast<char *>(contracts[i].c_str());

    string fileName;
    if (argc == 1) { 
	fileName = "/home/mingyuan/data/" + getCurrDate() + ".data";
    } else { 
	fileName = "/home/mingyuan/data/" + getCurrDate() + ".night.data";
    }
    FILE * pFile = fopen(fileName.c_str(), "a");
    
    CThostFtdcMdApi *pMdApi = (argc == 1) ? 
	CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/traderapi_linux64/Md_day_con/", false) : 
	CThostFtdcMdApi::CreateFtdcMdApi("/home/mingyuan/packages/traderapi_linux64/Md_night_con/", false);
    MdSpi sh(pMdApi, pFile, pInstrument, contracts.size());
    pMdApi->RegisterSpi(&sh);
    string registerFrontAddressPort("tcp://10.21.108.1:41213");
    pMdApi->RegisterFront(const_cast<char *>(registerFrontAddressPort.c_str()));

    pMdApi->Init();

    pTraderApi->Join();
    pMdApi->Join();

    pMdApi->Release();
    pTraderApi->Release();

    fclose(pFile);

    return 0;
}
