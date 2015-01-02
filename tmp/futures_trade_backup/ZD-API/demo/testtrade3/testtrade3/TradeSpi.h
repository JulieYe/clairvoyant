// TradeSpi.h: interface for the CTradeSpi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADESPI_H__4ECD7342_6379_45E0_B8EC_342AAFE02D93__INCLUDED_)
#define AFX_TRADESPI_H__4ECD7342_6379_45E0_B8EC_342AAFE02D93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZDQHMdApi.h"
#include "ZDQHUserApiStruct.h"
#include "ZDQHFtdcTradeApi.h"
#include "USTPFtdcTraderApi.h"

class CTradeSpi : public CThostFtdcTraderSpi
{
public:
	CTradeSpi();
	virtual ~CTradeSpi();

	//当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///登出请求响应
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
	///报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);
	
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	
	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	
public:
	///用户登录请求
	void ReqUserLogin(TThostFtdcInvestorIDType szInvestorID, TThostFtdcPasswordType szPassword);

	///用户登出请求
	void ReqUserLogout(TThostFtdcInvestorIDType szInvestorID);

	///投资者结算结果确认
	void ReqSettlementInfoConfirm(TThostFtdcInvestorIDType szInvestorID);
	
	///报单录入请求
	void ReqOrderInsert(TThostFtdcInvestorIDType szInvestorID, TThostFtdcInstrumentIDType szInstrumentID, 
							   DWORD dwDirection, DWORD dwOffsetFlag, int nVolumen,double dPrice,  BOOL bAnyPrice, int hHegeFlag);
	///报单操作请求
	
	void ReqOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction);
	
	// 是否收到成功的响应
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	
	// 是否我的报单回报
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	
	// 是否正在交易的报单
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);

	int GetOrderRef();

public:
	TThostFtdcInvestorIDType	szInvestorID;
	TThostFtdcPasswordType		szPassword;
};

class CZDQHAPISpi : public CZDQHFtdcMdSpi
{
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected();
			///订阅行情应答
	virtual void OnRspSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///深度行情通知
	virtual void OnRtnDepthMarketData(CZDQHFtdcDepthQuoteDataField *pDepthMarketData);
};

class CZDQHTradeAPISpi : public CZDQHFtdcTradeSpi
{
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected(TZDQHSourceType srcType);
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(TZDQHSourceType srcType);
		
	///登录请求响应
	virtual void OnRspUserLogin(CZDQHFtdcRspUserLoginField *pRspUserLogin, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///登出请求响应
	virtual void OnRspUserLogout(CZDQHFtdcUserLogoutField *pUserLogout, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

};


#endif // !defined(AFX_TRADESPI_H__4ECD7342_6379_45E0_B8EC_342AAFE02D93__INCLUDED_)
