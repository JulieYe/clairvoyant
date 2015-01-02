//////////////////////////////////////////////////////////////////////////
/// \file EsunnyApiStruct.h
/// \brief 易盛动态库业务处理所用到的数据域
/// 
///  业务处理中所用到的数据域都是由基本数据类型组合而成的，与交易服务器之间
///  的通信报文需要填充这些数据域或者从这些数据域中取出信息
//////////////////////////////////////////////////////////////////////////
#ifndef _ESUNNY_API_STRUCT_
#define _ESUNNY_API_STRUCT_

#include "EsunnyApiType.h"

/// \brief 连接到交易服务器IP、端口信息域
struct TEsAddressField
{
    TEsIpType					Ip;     ///< 交易服务器IP地址
    TEsPortType					Port;   ///< 交易服务器端口号
};

///错误应答域
struct TEsRspField
{
    TEsErrorCodeType			ErrorCode; ///< 错误码，为0表示成功
    TEsErrorInfoType			ErrorInfo; ///< 错误详细描述
};

///登录请求域
struct TEsLoginReqField
{
    TEsLoginTypeType			LoginType; ///< 'C'-单客户,'T'-交易员
    TEsLoginNoType				LoginNo;   ///< 登录号
    TEsPasswordType				Password;  ///< 登录密码
};

///修改密码请求域
struct TEsSetPassReqField
{
    TEsPasswordType				OldPassword; ///< 旧密码
    TEsPasswordType				NewPassword; ///< 新密码
};

///交易员下属客户信息查询应答域
struct TEsClientInfoField
{
    TEsClientNoType				ClientNo;   ///< 客户号
    TEsClientNameType			ClientName; ///< 客户姓名
};

///客户银行账号信息域
struct TEsBankClientInfoField
{
    TEsBankGroupNoType			GroupNo;        ///< 转账分组号
    TEsClientNoType				ClientNo;       ///< 客户号
    TEsBankType					BankId;         ///< 银行标识
    TEsBankClientNoType			BankClientNo;   ///< 银行帐号
    TEsPersonIdType				PersonId;       ///< 身份证号
    TEsPersonNameType			PersonName;     ///< 客户姓名
    TEsBankClientStateType		State;          ///< 转账状态
};

///转账操作请求域
struct TEsBankCashReqField
{
    TEsBankClientInfoField		BankClient;             ///< 客户银行账号信息
    TEsPasswordType				BankPassword;			///< 银行密码
    TEsPasswordType				FuturePassword;			///< 期货密码
    TEsBankTradeTypeType		TradeType;				///< 转账类型
    TEsBankMoneyType			Money;					///< 转账金额
};

///转账操作响应域
struct TEsBankCashRspField
{
    TEsClientNoType				ClientNo;				///< 客户号
    TEsBankType					BankId;					///< 银行标识
    TEsBankClientNoType			BankClientNo;			///< 银行帐号
    TEsBankPackFlagType			PackFlag;				///< 发起类型
    TEsBankTrTradeTypeType		TrTradeType;			///< 交易类型
    TEsBankSerNo				BankSerNo;				///< 银行流水号
    TEsBankSerNo				FutureSerNo;			///< 期货流水号
    TEsBankMoneyType			Money;					///< 转账金额
    TEsClientNoType				OperNo;					///< 转账操作人
    TEsDateTimeType				DateTime;				///< 操作时间
    TEsBankRetCode				RetCode;				///< 返回码，在ErrorCode.dat文件中描述
};

///查询资金应答域
struct TEsMoneyRspField
{
    TEsClientNoType				ClientNo;			///< 客户号
    TEsMoneyValueType			YAvailable;			///< 昨可用资金
    TEsMoneyValueType			YMoney;				///< 昨账面资金
    TEsMoneyValueType			YBalance;			///< 昨资金权益
    TEsMoneyValueType			CashOut;			///< 出金
    TEsMoneyValueType			CashIn;				///< 入金
    TEsMoneyValueType			Fee;				///< 手续费
    TEsMoneyValueType			Deposit;			///< 保证金
    TEsMoneyValueType			Frozen;				///< 冻结保证金
    TEsMoneyValueType			CoverProfit;		///< 平仓盈亏(逐笔)
    TEsMoneyValueType			FloatProfit;		///< 持仓盈亏(逐笔)
    TEsMoneyValueType			DayCoverProfit;		///< 平仓盈亏(盯市)
    TEsMoneyValueType			DayFloatProfit;		///< 持仓盈亏(盯市)
    TEsMoneyValueType			TAvailable;			///< 今可用资金
    TEsMoneyValueType			TMoney;				///< 今账面资金
    TEsMoneyValueType			TBalance;			///< 今资金权益
};

///客户报单请求域
struct TEsOrderInsertReqField
{
    TEsClientNoType				ClientNo;			///< 客户号
    TEsRequestIndexType			RequestID;          ///< 请求编号, 详见TEsRequestIndexType定义
    TEsOrderTypeType			OrderType;          ///< 委托类型
    TEsExchangeType				Exchange;           ///< 交易所
    TEsCmbTypeType				CmbType;            ///< 组合类型
    TEsContractType				Contract1;          ///< 合约1代码
    TEsContractType				Contract2;          ///< 合约2代码
    TEsDirectType				Direct;             ///< 买卖标识
    TEsOffsetType				Offset;             ///< 开平仓标识
    TEsHedgeType				Hedge;              ///< 投保标识
    TEsTradePriceType			OrderPrice;         ///< 交易价格
    TEsTradeVolType				OrderVol;           ///< 交易数量
};

///客户报单应答域
struct TEsOrderInsertRspField
{
    TEsClientNoType				ClientNo;   ///< 客户号
    TEsOrderInsertReqField		ReqData;    ///< 客户报单请求域
    TEsRspField					RspHead;    ///< 错误域
    TEsOrderStateType			OrderState; ///< 委托状态，详见TEsOrderStateType定义
    TEsOrderNoType				OrderNo;    ///< 委托号
    TEsLocalNoType				LocalNo;    ///< 本地号
    TEsSystemNoType				SystemNo;   ///< 系统号
    TEsMemberNoType				MemberNo;   ///< 会员号
    TEsPosNoType				PosNo;      ///< 席位号
    TEsTradePriceType			MatchPrice; ///< 暂时无用
    TEsTradeVolType				MatchVol;   ///< 成交数量
    TEsDateTimeType				InsertTime; ///< 报单录入时间
    TEsDateTimeType				DeleteTime; ///< 报单删除时间
    TEsClientNoType				InsertNo;   ///< 下单客户号
    TEsClientNoType				DeleteNo;   ///< 撤单客户号
    TEsStreamIDType				StreamID;   ///< 数据流号
    TEsErrorInfoType			OrderEcho;  ///< 上级系统错误信息提示
};

///客户报单操作域，目前只支持撤单操作
struct TEsOrderActionReqField
{
    TEsClientNoType				ClientNo;   ///< 客户号
    TEsExchangeType				Exchange;   ///< 交易所
    TEsContractType				Contract;   ///< 合约代码
    TEsOrderNoType				OrderNo;    ///< 委托号
    TEsSystemNoType				SystemNo;   ///< 系统号
};

///客户撤单操作应答域
struct TEsOrderActionRspField
{
    TEsClientNoType				ClientNo;   ///< 客户号
    TEsOrderActionReqField		ReqData;	///< 撤单请求数据
    TEsRspField					RspHead;	///< 错误域
    TEsOrderStateType			OrderState;	///< 委托状态
    TEsDirectType				Direct;		///< 买卖方向
    TEsOffsetType				Offset;		///< 开平仓标志
    TEsHedgeType				Hedge;		///< 投机保值标志
    TEsLocalNoType				LocalNo;	///< 委托的服务器本地编号
    TEsTradePriceType           OrderPrice;	///< 委托价格
    TEsDateTimeType				InsertTime;	///< 录入时间
    TEsDateTimeType				DeleteTime;	///< 删除时间
    TEsClientNoType				InsertNo;	///< 录单人编号
    TEsClientNoType				DeleteNo;	///< 删单人编号
    TEsStreamIDType				StreamID;	///< 本包数据流编号
};

///成交查询应答域
struct TEsMatchRspField
{
    TEsClientNoType				ClientNo;   ///< 客户号
    TEsLocalNoType				LocalNo;    ///< 本地号
    TEsSystemNoType				SystemNo;   ///< 系统号
    TEsMatchNoType				MatchNo;    ///< 成交号
    TEsExchangeType				Exchange;   ///< 交易所
    TEsContractType				Contract;   ///< 合约代码
    TEsDirectType				Direct;     ///< 买卖方向
    TEsOffsetType				Offset;     ///< 开平仓标识
    TEsHedgeType				Hedge;      ///< 投保标识
    TEsTradePriceType			MatchPrice; ///< 成交价格
    TEsTradeVolType				MatchVol;   ///< 成交数量
    TEsTimeType					MatchTime;  ///< 成交时间
    TEsMoneyValueType			Fee;        ///< 手续费
    TEsStreamIDType				StreamID;   ///< 数据流号
};

/// 查询客户持仓应答域
struct TEsHoldRspField
{
    TEsClientNoType				ClientNo;   ///< 客户号
    TEsMatchNoType				MatchNo;    ///< 成交号
    TEsMatchNoType				CmbMatchNo; ///< 组合合约成交号
    TEsExchangeType				Exchange;   ///< 交易所
    TEsCmbTypeType				CmbType;    ///< 组合类型
    TEsContractType				Contract1;  ///< 合约1代码
    TEsContractType				Contract2;  ///< 合约2代码
    TEsDirectType				Direct;     ///< 买卖方向
    TEsHedgeType				Hedge;      ///< 投保标识
    TEsTradePriceType			HoldPrice;  ///< 持仓价格
    TEsTradeVolType				HoldVol;    ///< 持仓数量
    TEsDateType					HoldDate;   ///< 持仓时间
    TEsHoldKey                  HoldKey;    ///< 持仓唯一标识，收到应答中重复记录以最后一条为准
};

///查询交易所状态应答域
struct TEsExchangeStateRspField
{
    TEsExchangeType				Exchange;           ///< 交易所
    TEsExchangeStateType		ExchangeState;      ///< 市场状态，详见TEsExchangeStateType定义
    TEsDateTimeType				ExchangeDateTime;   ///< 交易所系统时间
};

///商品查询应答域
struct TEsCommodityRspField
{
    TEsExchangeType				Exchange;   ///< 交易所
    TEsCommodityType			Commodity;  ///< 商品代码
    TEsCommodityNameType		Name;       ///< 商品名称
    TEsCommodityDotType			Dot;        ///< 商品每手乘数
    TEsCommodityTickType		Tick;	    ///< 商品最小变动价	
};

///合约查询应答域
struct TEsContractRspField
{
    TEsExchangeType				Exchange;       ///< 交易所
    TEsContractType				Contract;       ///< 合约代码
    TEsContractNameType			Name;           ///< 合约中文名称
    TEsDateType					BeginTradeDay;  ///< 开始交易日
    TEsDateType					EndTradeDay;    ///< 结束交易日
    TEsDateType					DeliveryDay;    ///< 交割日
};

///组合合约查询应答域
struct TEsCmbContractRspField
{
    TEsExchangeType				Exchange;   ///<交易所
    TEsCmbTypeType				CmbType;    ///<组合类型
    TEsContractType				Contract1;  ///<合约1代码
    TEsContractType				Contract2;  ///<合约2代码
};

///订阅行情请求域
struct TEsQuoteReqField
{
    TEsExchangeType				Exchange;   ///<交易所
    TEsCmbTypeType				CmbType;    ///<组合类型
    TEsContractType				Contract1;  ///<合约1代码
    TEsContractType				Contract2;  ///<合约2代码
    TEsSubQuoteType				SubQuote;   ///<订阅取消标识
};

///合约行情信息域
struct TEsQuoteRspField
{
    TEsExchangeType				Exchange;       ///<交易所
    TEsContractType				Contract;       ///<合约代码

    TEsQuotePriceType			YClose;			///<昨收盘
    TEsQuotePriceType			YSettle;		///<昨结算
    TEsQuotePriceType			Open;			///<开盘价
    TEsQuotePriceType			High;			///<最高价
    TEsQuotePriceType			Low;			///<最低价
    TEsQuotePriceType			New;			///<最新价
    TEsQuotePriceType			Settle;			///<结算价
    TEsQuotePriceType			AvgPrice;		///<平均价
    TEsQuoteVolType				Volume;			///<成交量
    TEsQuoteVolType				Amount;			///<持仓量

    TEsQuotePriceType			Buy;			///<申买价
    TEsQuoteVolType				BuyVol;			///<申买量
    TEsQuotePriceType			Sell;			///<申卖价
    TEsQuoteVolType				SellVol;		///<申卖量				

    TEsQuotePriceType			LimitUp;		///<涨停板
    TEsQuotePriceType			LimitDown;		///<跌停板
};

///组合合约行情信息域
struct TEsCmbQuoteRspField
{
    TEsExchangeType				Exchange;   ///<交易所
    TEsCmbTypeType				CmbType;    ///<组合类型
    TEsContractType				Contract1;  ///<合约1代码
    TEsContractType				Contract2;  ///<合约2代码

    TEsQuotePriceType			Buy;			///<申买价，郑州大连交易所发布本字段
    TEsQuoteVolType				BuyVol;			///<申买量，郑州大连交易所发布本字段
    TEsQuotePriceType			Sell;			///<申卖价，郑州大连交易所发布本字段
    TEsQuoteVolType				SellVol;		///<申卖量，郑州大连交易所发布本字段

    TEsQuoteVolType				AllBuyVol;		///<总买量，郑州交易所发布本字段
    TEsQuoteVolType				AllSellVol;		///<总卖量，郑州交易所发布本字段

    TEsQuotePriceType			DeriveBuy;			///<申买价，非交易所发布，易盛根据组合行情计算
    TEsQuoteVolType				DeriveBuyVol;		///<申买量，非交易所发布，易盛根据组合行情计算
    TEsQuotePriceType			DeriveSell;			///<申卖价，非交易所发布，易盛根据组合行情计算
    TEsQuoteVolType				DeriveSellVol;		///<申卖量，非交易所发布，易盛根据组合行情计算

};

#endif