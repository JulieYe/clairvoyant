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

	//���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	
	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
	///����¼��������Ӧ
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///��������������Ӧ
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);
	
	///������ʱ���档����ʱ��δ�յ�����ʱ���÷��������á�
	virtual void OnHeartBeatWarning(int nTimeLapse);
	
	///����֪ͨ
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	
	///�ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	
public:
	///�û���¼����
	void ReqUserLogin(TThostFtdcInvestorIDType szInvestorID, TThostFtdcPasswordType szPassword);

	///�û��ǳ�����
	void ReqUserLogout(TThostFtdcInvestorIDType szInvestorID);

	///Ͷ���߽�����ȷ��
	void ReqSettlementInfoConfirm(TThostFtdcInvestorIDType szInvestorID);
	
	///����¼������
	void ReqOrderInsert(TThostFtdcInvestorIDType szInvestorID, TThostFtdcInstrumentIDType szInstrumentID, 
							   DWORD dwDirection, DWORD dwOffsetFlag, int nVolumen,double dPrice,  BOOL bAnyPrice, int hHegeFlag);
	///������������
	
	void ReqOrderAction(CThostFtdcInputOrderActionField* pInputOrderAction);
	
	// �Ƿ��յ��ɹ�����Ӧ
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	
	// �Ƿ��ҵı����ر�
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	
	// �Ƿ����ڽ��׵ı���
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);

	int GetOrderRef();

public:
	TThostFtdcInvestorIDType	szInvestorID;
	TThostFtdcPasswordType		szPassword;
};

class CZDQHAPISpi : public CZDQHFtdcMdSpi
{
public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected();
			///��������Ӧ��
	virtual void OnRspSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData(CZDQHFtdcSpecificInstrumentField *pSpecificInstrument, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CZDQHFtdcDepthQuoteDataField *pDepthMarketData);
};

class CZDQHTradeAPISpi : public CZDQHFtdcTradeSpi
{
public:
	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected(TZDQHSourceType srcType);
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(TZDQHSourceType srcType);
		
	///��¼������Ӧ
	virtual void OnRspUserLogin(CZDQHFtdcRspUserLoginField *pRspUserLogin, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�ǳ�������Ӧ
	virtual void OnRspUserLogout(CZDQHFtdcUserLogoutField *pUserLogout, CZDQHFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

};


#endif // !defined(AFX_TRADESPI_H__4ECD7342_6379_45E0_B8EC_342AAFE02D93__INCLUDED_)
