// ZDQHFtdcTradeApi.h: interface for the CZDQHFtdcTradeApi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZDQHFTDCTRADEAPI_H__54526568_374A_476D_AE38_6D9A7B229906__INCLUDED_)
#define AFX_ZDQHFTDCTRADEAPI_H__54526568_374A_476D_AE38_6D9A7B229906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ZDQHUserApiStruct.h"
class CZDQHFtdcTradeSpi
{
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected(){};
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(){};
		
	///登录请求响应
	virtual void OnRspUserLogin(CZDQHFtdcRspUserLoginField *pRspUserLogin, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///登出请求响应
	virtual void OnRspUserLogout(CZDQHFtdcUserLogoutField *pUserLogout, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

};


class API_EXPORT CZDQHFtdcTradeApi
{
public:
	///删除接口对象本身
	///@remark 不再使用本接口对象时,调用该函数删除接口对象
	virtual void Release() = 0;
	
	///初始化,连接服务器
	///@remark 初始化运行环境,只有调用后,接口才开始工作
	///@param pAddr：前置机网络地址。
	///@param cCount：地址个数
	virtual void Init(CZDQHAddressField* pAddr, int cCount, TZDQHSourceType srcType) = 0;
	
	
	///注册回调接口
	///@param pSpi 派生自回调接口类的实例
	virtual void RegisterSpi(CZDQHFtdcTradeSpi *pSpi) = 0;

	///用户登录请求
	///@param pReqUserLoginField 登出信息
	///@param nRequestID 请求ID  
	///@remark 
	virtual int ReqUserLogin(CZDQHFtdcReqUserLoginField *pReqUserLoginField, int nRequestID,TZDQHSourceType srcType) = 0;	

	//登出
	///登出请求
	///@param pReqUserLoginField 登出信息
	///@param nRequestID 请求ID  
	///@remark 
	virtual int ReqUserLogout(CZDQHFtdcUserLogoutField *pUserLogout, int nRequestID,TZDQHSourceType srcType) =0;
};

extern "C"
{
	CZDQHFtdcTradeApi* __cdecl CreateFtdcTradeApi(const char *pszFlowPath = "");
}


#endif // !defined(AFX_ZDQHFTDCTRADEAPI_H__54526568_374A_476D_AE38_6D9A7B229906__INCLUDED_)
