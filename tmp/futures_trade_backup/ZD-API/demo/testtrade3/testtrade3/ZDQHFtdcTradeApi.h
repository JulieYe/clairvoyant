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
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected(){};
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(){};
		
	///��¼������Ӧ
	virtual void OnRspUserLogin(CZDQHFtdcRspUserLoginField *pRspUserLogin, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CZDQHFtdcUserLogoutField *pUserLogout, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

};


class API_EXPORT CZDQHFtdcTradeApi
{
public:
	///ɾ���ӿڶ�����
	///@remark ����ʹ�ñ��ӿڶ���ʱ,���øú���ɾ���ӿڶ���
	virtual void Release() = 0;
	
	///��ʼ��,���ӷ�����
	///@remark ��ʼ�����л���,ֻ�е��ú�,�ӿڲſ�ʼ����
	///@param pAddr��ǰ�û������ַ��
	///@param cCount����ַ����
	virtual void Init(CZDQHAddressField* pAddr, int cCount, TZDQHSourceType srcType) = 0;
	
	
	///ע��ص��ӿ�
	///@param pSpi �����Իص��ӿ����ʵ��
	virtual void RegisterSpi(CZDQHFtdcTradeSpi *pSpi) = 0;

	///�û���¼����
	///@param pReqUserLoginField �ǳ���Ϣ
	///@param nRequestID ����ID  
	///@remark 
	virtual int ReqUserLogin(CZDQHFtdcReqUserLoginField *pReqUserLoginField, int nRequestID,TZDQHSourceType srcType) = 0;	

	//�ǳ�
	///�ǳ�����
	///@param pReqUserLoginField �ǳ���Ϣ
	///@param nRequestID ����ID  
	///@remark 
	virtual int ReqUserLogout(CZDQHFtdcUserLogoutField *pUserLogout, int nRequestID,TZDQHSourceType srcType) =0;
};

extern "C"
{
	CZDQHFtdcTradeApi* __cdecl CreateFtdcTradeApi(const char *pszFlowPath = "");
}


#endif // !defined(AFX_ZDQHFTDCTRADEAPI_H__54526568_374A_476D_AE38_6D9A7B229906__INCLUDED_)
