/////////////////////////////////////////////////////////////////////////
///@system 新一代交易所系统
///@company  中大期货
///@file ZDQHUserApiStruct.h
///@brief 定义了客户端接口业务数据结构
///@history 
///20130923	徐旦		创建该文件
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

///市场 CZCE-郑州,DCE-大连,SHFE-上海,CFFEX-中金
typedef char TZDQHExchangeType[9];

struct CZDQHAddressField
{
    TEsIpType					Ip;     ///< 交易服务器IP地址
    TEsPortType					Port;   ///< 交易服务器端口号
};

///用户登录请求
struct CZDQHFtdcReqUserLoginField
{
	///交易日
	TZDQHFtdcDateType	TradingDay;
	///经纪公司代码
	TZDQHFtdcBrokerIDType	BrokerID;
	///用户代码
	TZDQHFtdcUserIDType	UserID;
	///密码
	TZDQHFtdcPasswordType	Password;
	///用户端产品信息
	TZDQHFtdcProductInfoType	UserProductInfo;
	///接口端产品信息
	TZDQHFtdcProductInfoType	InterfaceProductInfo;
	///协议信息
	TZDQHFtdcProtocolInfoType	ProtocolInfo;
	///Mac地址
	TZDQHFtdcMacAddressType	MacAddress;
	///动态密码
	TZDQHFtdcPasswordType	OneTimePassword;
	///终端IP地址
	TZDQHFtdcIPAddressType	ClientIPAddress;
};

///用户登录应答
struct CZDQHFtdcRspUserLoginField
{
	///交易日
	TZDQHFtdcDateType	TradingDay;
	///登录成功时间
	TZDQHFtdcTimeType	LoginTime;
	///经纪公司代码
	TZDQHFtdcBrokerIDType	BrokerID;
	///用户代码
	TZDQHFtdcUserIDType	UserID;
	///交易系统名称
	TZDQHFtdcSystemNameType	SystemName;
	///前置编号
	TZDQHFtdcFrontIDType	FrontID;
	///会话编号
	TZDQHFtdcSessionIDType	SessionID;
	///最大报单引用
	TZDQHFtdcOrderRefType	MaxOrderRef;
	///上期所时间
	TZDQHFtdcTimeType	SHFETime;
	///大商所时间
	TZDQHFtdcTimeType	DCETime;
	///郑商所时间
	TZDQHFtdcTimeType	CZCETime;
	///中金所时间
	TZDQHFtdcTimeType	FFEXTime;
};

///用户登出请求
struct CZDQHFtdcUserLogoutField
{
	///经纪公司代码
	TZDQHFtdcBrokerIDType	BrokerID;
	///用户代码
	TZDQHFtdcUserIDType	UserID;
};

///指定的合约
struct CZDQHFtdcSpecificInstrumentField
{
	///合约代码
	TZDQHFtdcInstrumentIDType	InstrumentID;
};



///深度行情
struct CZDQHFtdcDepthQuoteDataField
{
	///交易日
	TZDQHFtdcDateType	TradingDay;
	///合约代码
	TZDQHFtdcInstrumentIDType	InstrumentID;
	///交易所代码
	TZDQHFtdcExchangeIDType	ExchangeID;
	///合约在交易所的代码
	TZDQHFtdcExchangeInstIDType	ExchangeInstID;
	///最新价
	TZDQHFtdcPriceType	LastPrice;
	///上次结算价
	TZDQHFtdcPriceType	PreSettlementPrice;
	///昨收盘
	TZDQHFtdcPriceType	PreClosePrice;
	///昨持仓量
	TZDQHFtdcLargeVolumeType	PreOpenInterest;
	///今开盘
	TZDQHFtdcPriceType	OpenPrice;
	///最高价
	TZDQHFtdcPriceType	HighestPrice;
	///最低价
	TZDQHFtdcPriceType	LowestPrice;
	///数量
	TZDQHFtdcVolumeType	Volume;
	///成交金额
	TZDQHFtdcMoneyType	Turnover;
	///持仓量
	TZDQHFtdcLargeVolumeType	OpenInterest;
	///今收盘
	TZDQHFtdcPriceType	ClosePrice;
	///本次结算价
	TZDQHFtdcPriceType	SettlementPrice;
	///涨停板价
	TZDQHFtdcPriceType	UpperLimitPrice;
	///跌停板价
	TZDQHFtdcPriceType	LowerLimitPrice;
	///昨虚实度
	TZDQHFtdcRatioType	PreDelta;
	///今虚实度
	TZDQHFtdcRatioType	CurrDelta;
	///最后修改时间
	TZDQHFtdcTimeType	UpdateTime;
	///最后修改毫秒
	TZDQHFtdcMillisecType	UpdateMillisec;
	///申买价一
	TZDQHFtdcPriceType	BidPrice1;
	///申买量一
	TZDQHFtdcVolumeType	BidVolume1;
	///申卖价一
	TZDQHFtdcPriceType	AskPrice1;
	///申卖量一
	TZDQHFtdcVolumeType	AskVolume1;
	///申买价二
	TZDQHFtdcPriceType	BidPrice2;
	///申买量二
	TZDQHFtdcVolumeType	BidVolume2;
	///申卖价二
	TZDQHFtdcPriceType	AskPrice2;
	///申卖量二
	TZDQHFtdcVolumeType	AskVolume2;
	///申买价三
	TZDQHFtdcPriceType	BidPrice3;
	///申买量三
	TZDQHFtdcVolumeType	BidVolume3;
	///申卖价三
	TZDQHFtdcPriceType	AskPrice3;
	///申卖量三
	TZDQHFtdcVolumeType	AskVolume3;
	///申买价四
	TZDQHFtdcPriceType	BidPrice4;
	///申买量四
	TZDQHFtdcVolumeType	BidVolume4;
	///申卖价四
	TZDQHFtdcPriceType	AskPrice4;
	///申卖量四
	TZDQHFtdcVolumeType	AskVolume4;
	///申买价五
	TZDQHFtdcPriceType	BidPrice5;
	///申买量五
	TZDQHFtdcVolumeType	BidVolume5;
	///申卖价五
	TZDQHFtdcPriceType	AskPrice5;
	///申卖量五
	TZDQHFtdcVolumeType	AskVolume5;
	///当日均价
	TZDQHFtdcPriceType	AveragePrice;
};

///响应信息
struct CZDQHFtdcRspInfoField
{
	///错误代码
	TZDQHFtdcErrorIDType	ErrorID;
	///错误信息
	TZDQHFtdcErrorMsgType	ErrorMsg;
};

#endif

