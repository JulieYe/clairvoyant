/////////////////////////////////////////////////////////////////////////
///@system ��һ��������ϵͳ
///@company  �д��ڻ�
///@file ZDQHUserApiStruct.h
///@brief �����˿ͻ��˽ӿ�ҵ�����ݽṹ
///@history 
///20130923	�쵩		�������ļ�
/////////////////////////////////////////////////////////////////////////

#ifndef ZDQHUSERAPISTRUCTH
#define ZDQHUSERAPISTRUCTH

#include "ZDQHUserApiType.h"
#include "EsunnyApiType.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_MD_API_EXPORT
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __declspec(dllimport)
#endif
#else
#define API_EXPORT 
#endif

///�г� CZCE-֣��,DCE-����,SHFE-�Ϻ�,CFFEX-�н�
typedef char TZDQHExchangeType[9];

struct CZDQHAddressField
{
    TEsIpType					Ip;     ///< ���׷�����IP��ַ
    TEsPortType					Port;   ///< ���׷������˿ں�
};

///�û���¼����
struct CZDQHFtdcReqUserLoginField
{
	///������
	TZDQHFtdcDateType	TradingDay;
	///���͹�˾����
	TZDQHFtdcBrokerIDType	BrokerID;
	///�û�����
	TZDQHFtdcUserIDType	UserID;
	///����
	TZDQHFtdcPasswordType	Password;
	///�û��˲�Ʒ��Ϣ
	TZDQHFtdcProductInfoType	UserProductInfo;
	///�ӿڶ˲�Ʒ��Ϣ
	TZDQHFtdcProductInfoType	InterfaceProductInfo;
	///Э����Ϣ
	TZDQHFtdcProtocolInfoType	ProtocolInfo;
	///Mac��ַ
	TZDQHFtdcMacAddressType	MacAddress;
	///��̬����
	TZDQHFtdcPasswordType	OneTimePassword;
	///�ն�IP��ַ
	TZDQHFtdcIPAddressType	ClientIPAddress;
};

///�û���¼Ӧ��
struct CZDQHFtdcRspUserLoginField
{
	///������
	TZDQHFtdcDateType	TradingDay;
	///��¼�ɹ�ʱ��
	TZDQHFtdcTimeType	LoginTime;
	///���͹�˾����
	TZDQHFtdcBrokerIDType	BrokerID;
	///�û�����
	TZDQHFtdcUserIDType	UserID;
	///����ϵͳ����
	TZDQHFtdcSystemNameType	SystemName;
	///ǰ�ñ��
	TZDQHFtdcFrontIDType	FrontID;
	///�Ự���
	TZDQHFtdcSessionIDType	SessionID;
	///��󱨵�����
	TZDQHFtdcOrderRefType	MaxOrderRef;
	///������ʱ��
	TZDQHFtdcTimeType	SHFETime;
	///������ʱ��
	TZDQHFtdcTimeType	DCETime;
	///֣����ʱ��
	TZDQHFtdcTimeType	CZCETime;
	///�н���ʱ��
	TZDQHFtdcTimeType	FFEXTime;
};

///�û��ǳ�����
struct CZDQHFtdcUserLogoutField
{
	///���͹�˾����
	TZDQHFtdcBrokerIDType	BrokerID;
	///�û�����
	TZDQHFtdcUserIDType	UserID;
};

///ָ���ĺ�Լ
struct CZDQHFtdcSpecificInstrumentField
{
	///��Լ����
	TZDQHFtdcInstrumentIDType	InstrumentID;
};



///�������
struct CZDQHFtdcDepthQuoteDataField
{
	///������
	TZDQHFtdcDateType	TradingDay;
	///��Լ����
	TZDQHFtdcInstrumentIDType	InstrumentID;
	///����������
	TZDQHFtdcExchangeIDType	ExchangeID;
	///��Լ�ڽ������Ĵ���
	TZDQHFtdcExchangeInstIDType	ExchangeInstID;
	///���¼�
	TZDQHFtdcPriceType	LastPrice;
	///�ϴν����
	TZDQHFtdcPriceType	PreSettlementPrice;
	///������
	TZDQHFtdcPriceType	PreClosePrice;
	///��ֲ���
	TZDQHFtdcLargeVolumeType	PreOpenInterest;
	///����
	TZDQHFtdcPriceType	OpenPrice;
	///��߼�
	TZDQHFtdcPriceType	HighestPrice;
	///��ͼ�
	TZDQHFtdcPriceType	LowestPrice;
	///����
	TZDQHFtdcVolumeType	Volume;
	///�ɽ����
	TZDQHFtdcMoneyType	Turnover;
	///�ֲ���
	TZDQHFtdcLargeVolumeType	OpenInterest;
	///������
	TZDQHFtdcPriceType	ClosePrice;
	///���ν����
	TZDQHFtdcPriceType	SettlementPrice;
	///��ͣ���
	TZDQHFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TZDQHFtdcPriceType	LowerLimitPrice;
	///����ʵ��
	TZDQHFtdcRatioType	PreDelta;
	///����ʵ��
	TZDQHFtdcRatioType	CurrDelta;
	///����޸�ʱ��
	TZDQHFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TZDQHFtdcMillisecType	UpdateMillisec;
	///�����һ
	TZDQHFtdcPriceType	BidPrice1;
	///������һ
	TZDQHFtdcVolumeType	BidVolume1;
	///������һ
	TZDQHFtdcPriceType	AskPrice1;
	///������һ
	TZDQHFtdcVolumeType	AskVolume1;
	///����۶�
	TZDQHFtdcPriceType	BidPrice2;
	///��������
	TZDQHFtdcVolumeType	BidVolume2;
	///�����۶�
	TZDQHFtdcPriceType	AskPrice2;
	///��������
	TZDQHFtdcVolumeType	AskVolume2;
	///�������
	TZDQHFtdcPriceType	BidPrice3;
	///��������
	TZDQHFtdcVolumeType	BidVolume3;
	///��������
	TZDQHFtdcPriceType	AskPrice3;
	///��������
	TZDQHFtdcVolumeType	AskVolume3;
	///�������
	TZDQHFtdcPriceType	BidPrice4;
	///��������
	TZDQHFtdcVolumeType	BidVolume4;
	///��������
	TZDQHFtdcPriceType	AskPrice4;
	///��������
	TZDQHFtdcVolumeType	AskVolume4;
	///�������
	TZDQHFtdcPriceType	BidPrice5;
	///��������
	TZDQHFtdcVolumeType	BidVolume5;
	///��������
	TZDQHFtdcPriceType	AskPrice5;
	///��������
	TZDQHFtdcVolumeType	AskVolume5;
	///���վ���
	TZDQHFtdcPriceType	AveragePrice;
};

///��Ӧ��Ϣ
struct CZDQHFtdcRspInfoField
{
	///�������
	TZDQHFtdcErrorIDType	ErrorID;
	///������Ϣ
	TZDQHFtdcErrorMsgType	ErrorMsg;
};

#endif

