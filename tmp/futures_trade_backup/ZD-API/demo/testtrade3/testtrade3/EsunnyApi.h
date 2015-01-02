//////////////////////////////////////////////////////////////////////////
/// \file EsunnyApi.h
/// \brief 易盛动态库提供对外接口
///  
///  1.	包含四个类，和四个公共函数\n
///  2.	四个类分为两组，交易两个(IEsunnyTradeApi, IEsunnyTradeSpi)，行情两个(IEsunnyQuoteApi, IEsunnyQuoteSpi)\n
///  3.	Api结尾的类为调用函数(发起请求)，Spi结尾的类为回调函数(接收数据)\n
///  4.	公共函数 分别提供：认证，获取版本，构造交易Api，构造行情Api四项功能\n
///  5.	Api认证：易盛公司针对每个Api的开发者提供Api认证码，以便于识别不同的开发者，期货公司通过认证码对应的识别码控制是否允许相应的开发\n
//////////////////////////////////////////////////////////////////////////

#ifndef _ESUNNY_API_
#define _ESUNNY_API_

#include "EsunnyApiStruct.h"

/// \brief 数据接收完成，交易相关操作处理类
///
/// 基于动态库的开发需要重写自己关注的回调函数
/// 函数名称中包含Qry的为查询应答，包含Rtn的为推送数据
class IEsunnyTradeSpi
{
public:
	////////////////////////////////////////
	/// \fn    OnOpen
	/// \brief 交易类IEsunnyTradeApi成功建立与交易服务器连接时的回调函数
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnOpen() = 0;

	////////////////////////////////////////
	/// \fn    OnClose
	/// \brief 交易类IEsunnyTradeApi断开与交易服务器连接时的回调函数
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnClose() = 0;

	////////////////////////////////////////
	/// \fn    OnLogin
	/// \brief 交易类收到登录应答数据返回时回调函数
	/// \param rsp 登录请求的应答
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnLogin(TEsRspField& rsp) = 0;

	////////////////////////////////////////
	/// \fn    OnSetPass
	/// \brief 交易类收到修改密码应答数据返回时回调函数
	/// \param rsp 修改密码请求的应答
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnSetPass(TEsRspField& rsp) = 0;

	////////////////////////////////////////
	/// \fn    OnQryMoney
	/// \brief 收到资金查询应答数据时回调函数
	/// \param rsp 资金查询应答
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryMoney(TEsMoneyRspField& rsp) = 0;
	
	////////////////////////////////////////
	/// \fn    OnRtnMoney
	/// \brief 客户资金发生变化时回调函数
	/// \param rsp 客户资金变化数据
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnRtnMoney(TEsMoneyRspField& rsp) = 0;

    ////////////////////////////////////////
    /// \fn    OnRspOrderInsert
    /// \brief 报单操作应答
    /// \param rsp 报单操作结果，委托编号和报单请求ID在本函数关联
    /// \return void
    ////////////////////////////////////////
	virtual void __cdecl OnRspOrderInsert(TEsOrderInsertRspField& rsp) = 0;

    ////////////////////////////////////////
    /// \fn    OnRspOrderAction
    /// \brief 撤单操作应答
    /// \param rsp 撤单操作结果
    /// \return void
    ////////////////////////////////////////
	virtual void __cdecl OnRspOrderAction(TEsOrderActionRspField& rsp) = 0;

	////////////////////////////////////////
	/// \fn    OnQryOrder
	/// \brief 查询当天委托应答
	/// \param rsp 委托查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryOrder(TEsOrderInsertRspField* rsp, TEsIsLastType islast) = 0;
	
	////////////////////////////////////////
	/// \fn    OnRtnOrder
	/// \brief 委托状态发生变化时回调函数
	/// \param rsp 发生变化的委托信息
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnRtnOrder(TEsOrderInsertRspField& rsp) = 0;
	
	////////////////////////////////////////
	/// \fn    OnQryMatch
	/// \brief 成交查询应答数据返回时回调函数
	/// \param rsp 成交查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryMatch(TEsMatchRspField* rsp, TEsIsLastType islast) = 0;
	
	////////////////////////////////////////
	/// \fn    OnRtnMatch
	/// \brief 成交发生变化时回调函数
	/// \param rsp 发生变化的成交数据
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnRtnMatch(TEsMatchRspField& rsp) = 0;

	////////////////////////////////////////
	/// \fn    OnQryHold
	/// \brief 持仓查询应答数据返回时回调函数
	/// \param rsp 持仓查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryHold(TEsHoldRspField* rsp, TEsIsLastType islast) = 0;
	
	////////////////////////////////////////
	/// \fn    OnRtnHold
	/// \brief 持仓变化时回调函数
	/// \param rsp 发生变化的持仓数据
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnRtnHold(TEsHoldRspField& rsp) = 0;

	////////////////////////////////////////
	/// \fn    OnQryExchangeState
	/// \brief 市场状态查询应答数据返回时回调函数
	/// \param rsp 市场状态查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryExchangeState(TEsExchangeStateRspField* rsp, TEsIsLastType islast) = 0;
	
	////////////////////////////////////////
	/// \fn    OnRtnExchangeState
	/// \brief 市场状态变化时回调函数
	/// \param rsp 市场状态变化信息
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnRtnExchangeState(TEsExchangeStateRspField& rsp) = 0;

	////////////////////////////////////////
	/// \fn    OnQryCommodity
	/// \brief 商品查询应答数据返回时回调函数
	/// \param rsp 商品查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryCommodity(TEsCommodityRspField* rsp, TEsIsLastType islast) = 0;

	////////////////////////////////////////
	/// \fn    OnQryContract
	/// \brief 合约查询应答数据返回时回调函数
	/// \param rsp 合约查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryContract(TEsContractRspField* rsp, TEsIsLastType islast) = 0;
	
	////////////////////////////////////////
	/// \fn    OnQryCmbContract
	/// \brief 组合合约查询应答数据返回时回调函数
	/// \param rsp 组合合约查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryCmbContract(TEsCmbContractRspField* rsp, TEsIsLastType islast) = 0;

	////////////////////////////////////////
	/// \fn    OnQryClient
	/// \brief 交易员下属客户信息查询应答数据返回时回调函数
	/// \param rsp 交易员下属客户信息查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryClient(TEsClientInfoField* rsp, TEsIsLastType islast) = 0;

	////////////////////////////////////////
	/// \fn    OnQryBankClient
	/// \brief 转账客户查询应答数据返回时回调函数
	/// \param rsp 转账客户查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryBankClient(TEsBankClientInfoField* rsp, TEsIsLastType islast) = 0;
	
	////////////////////////////////////////
	/// \fn    OnQryBankCash
	/// \brief 转账流水查询应答数据返回时回调函数
	/// \param rsp 转账流水查询结果
	/// \param islast 数据是否接收完成，TRUE-完成，FALSE-未完成
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQryBankCash(TEsBankCashRspField* rsp, TEsIsLastType islast) = 0;
	
	////////////////////////////////////////
	/// \fn    OnRtnBankCash
	/// \brief 转账流水变化时回调函数
	/// \param rsp 发生变化的转账流水数据
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnRtnBankCash(TEsBankCashRspField& rsp) = 0;
};

/// \brief 交易的接口类
///
/// 基于动态库的开发无需重新实现接口，获取api的实例后，通过该实例调用接口即可
class IEsunnyTradeApi
{
public:
	////////////////////////////////////////
	/// \brief 设置回调数据接口
	/// \param spi 派生自IEsunnyTradeSpi的业务处理类
	/// \return   void
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual void __cdecl SetSpi(IEsunnyTradeSpi* spi) = 0;
	
	////////////////////////////////////////
	/// \fn    Free
	/// \brief 释放当前api会话
	/// \return   void
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual void __cdecl Free() = 0;

	////////////////////////////////////////
	/// \brief 连接交易服务器
	/// \param addr 交易服务器连接地址信息
	/// \return   bool 是否成功建立连接
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual bool __cdecl Open(TEsAddressField& addr) = 0;
	
	////////////////////////////////////////
	/// \fn    Close
	/// \brief 关闭交易服务
	/// \return   void
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual void __cdecl Close() = 0;
	
	////////////////////////////////////////
	/// \fn    IsOpen
	/// \brief 获取当前交易处理实例是否连接
	/// \return   bool TRUE-成功连接, FALSE-未连接
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual bool __cdecl IsOpen() = 0;

	////////////////////////////////////////
	/// \fn    Login
	/// \brief 发起登录请求
	/// \param req 登录信息
	/// \return   int 是否成功发送登录请求，是否登录成功需要在登录对应回调函数获取
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl Login(TEsLoginReqField& req) = 0;
	
	////////////////////////////////////////
	/// \fn    GetTradingDay
	/// \brief 获取日期，登录成功后 方可获取交易日期
	/// \param day 交易日期
	/// \return   void
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual void __cdecl GetTradingDay(TEsDateTimeType day) = 0;
	
	////////////////////////////////////////
	/// \fn    SetPass
	/// \brief 修改个人密码
	/// \param req 密码修改请求信息
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl SetPass(TEsSetPassReqField& req) = 0;

	////////////////////////////////////////
	/// \fn    QryClients
	/// \brief 查询交易员下属客户
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryClients() = 0;
	
	////////////////////////////////////////
	/// \fn    QryBankClients
	/// \brief 查询转账客户
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryBankClients() = 0;
	
	////////////////////////////////////////
	/// \fn    QryBankCash
	/// \brief 查询转账流水
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryBankCash() = 0;
	
	////////////////////////////////////////
	/// \fn    BankTrade
	/// \brief 转账操作
	/// \param req
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl BankTrade(TEsBankCashReqField& req) = 0;

	////////////////////////////////////////
	/// \fn    QryMoney
	/// \brief 查询客户资金
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryMoney() = 0;
	
	////////////////////////////////////////
	/// \fn    QryOrder
	/// \brief 查询客户委托，查询所有数据，流号填0，或者从返回数据中获取当前返回列表的下一个流号
	/// \param streamid
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryOrder(TEsStreamIDType streamid) = 0;
	
	////////////////////////////////////////
	/// \fn    QryMatch
	/// \brief 查询客户成交，查询所有数据，流号填0，或者从返回数据中获取当前返回列表的下一个流号
	/// \param streamid
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryMatch(TEsStreamIDType streamid) = 0;
	
	////////////////////////////////////////
	/// \fn    QryHold
	/// \brief 查询单腿持仓
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryHold() = 0;

    ////////////////////////////////////////
    /// \fn    QryCmbHold
    /// \brief 查询组合持仓
	/// \return int 错误代码，0为成功
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
    ////////////////////////////////////////
	virtual int __cdecl QryCmbHold() = 0;

	////////////////////////////////////////
	/// \fn    QryExchangeState
	/// \brief 查询交易所状态  包括交易所时间
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryExchangeState() = 0;
	
	////////////////////////////////////////
	/// \fn    QryCommodity
	/// \brief 查询交易商品
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryCommodity() = 0;
	
	////////////////////////////////////////
	/// \fn    QryContract
	/// \brief 查询单腿合约
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryContract() = 0;
	
	////////////////////////////////////////
	/// \fn    QryCmbContract
	/// \brief 查询组合合约
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl QryCmbContract() = 0;

	////////////////////////////////////////
	/// \fn    OrderInsert
	/// \brief 报单请求
	/// \param req 报单请求数据
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl OrderInsert(TEsOrderInsertReqField& req) = 0;
	
	////////////////////////////////////////
	/// \fn    OrderAction
	/// \brief 报单操作，目前只支持撤单操作
	/// \param req 报单操作数据
	/// \return   int 请求发送成功为0，否则为错误码
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual int __cdecl OrderAction(TEsOrderActionReqField& req) = 0;
};

/// \brief 数据接收完成，行情相关操作处理类
///
/// 基于动态库的开发需要重写自己关注的回调函数
/// 函数名称中包含Qry的为查询应答，包含Rtn的为推送数据
class IEsunnyQuoteSpi
{
public:
	////////////////////////////////////////
	/// \fn    OnOpen
	/// \brief 行情接口API与交易服务器成功连接时的回调函数
	/// \return   void 
	////////////////////////////////////////
	virtual void __cdecl OnOpen() = 0;

	////////////////////////////////////////
	/// \fn    OnClose
	/// \brief 行情接口API与交易服务器断开连接时的回调函数
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnClose() = 0;

	////////////////////////////////////////
	/// \fn    OnQuote
	/// \brief 单腿行情查询，数据返回时回调函数
	/// \param rsp 单腿行情数据
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnQuote(TEsQuoteRspField& rsp) = 0;
	
	////////////////////////////////////////
	/// \fn    OnCmbQuote
	/// \brief 组合行情数据查询，数据返回时回调函数
	/// \param rsp 组合行情数据
	/// \return   void
	////////////////////////////////////////
	virtual void __cdecl OnCmbQuote(TEsCmbQuoteRspField& rsp) = 0;
};

/// \brief 行情查询的接口类
///
/// 基于动态库的开发无需重新实现接口，获取api的实例后，通过该实例调用接口即可
class IEsunnyQuoteApi
{
public:
    ////////////////////////////////////////
    /// \fn    SetSpi
    /// \brief 设置处理行情数据的实例
    /// \param spi 派生于IEsunnyQuoteSpi类的实例
    /// \return   void
    /// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
    ////////////////////////////////////////
	virtual void __cdecl SetSpi(IEsunnyQuoteSpi* spi) = 0;

	////////////////////////////////////////
	/// \fn    Free
	/// \brief 释放当前API会话
	/// \return   void
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual void __cdecl Free() = 0;

	////////////////////////////////////////
	/// \fn    Open
	/// \brief 建立与交易服务器的连接，交易api登录成功后才能进行行情连接
	/// \param addr 交易服务器的地址信息
	/// \return   bool 是否成功建立连接
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual bool __cdecl Open(TEsAddressField& addr) = 0;

	////////////////////////////////////////
	/// \fn    Close
	/// \brief 关闭与交易服务器的连接
	/// \return   void
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual void __cdecl Close() = 0;
	
	////////////////////////////////////////
	/// \fn    IsOpen
	/// \brief 获取当前行情处理实例是否已经成功连接交易服务器
	/// \return   bool
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual bool __cdecl IsOpen() = 0;

	////////////////////////////////////////
	/// \fn    SubQuote
	/// \brief 行情订阅或退订，当为退订请求时，退订所有已订阅行情数据
	/// \param req 订阅或退订行情的请求数据
	/// \return   bool 是否成功订阅或退订
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual bool __cdecl SubQuote(TEsQuoteReqField& req) = 0;

	////////////////////////////////////////
	/// \fn    GetQuote
	/// \brief 获取单腿行情
	/// \param data 单腿行情数据
	/// \return   bool 是否成功获取行情数据
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual bool __cdecl GetQuote(TEsQuoteRspField& data) = 0;

	////////////////////////////////////////
	/// \fn    GetCmbQuote
	/// \brief 获取组合合约行情
	/// \param data 组合合约行情数据
	/// \return   bool
	/// \note 基于动态库的开发无需重写本函数，通过本类的指针调用本接口即可
	////////////////////////////////////////
	virtual bool __cdecl GetCmbQuote(TEsCmbQuoteRspField& data) = 0;
};

extern "C"
{
	////////////////////////////////////////
	/// \brief 开发商认证函数，针对每个开发商一个认证码
	/// \param cert 开发商认证信息
	/// \param path 日志路径,默认为当前程序目录下
	/// \param logLevel 记录日志级别 0-记录操作类API调用日志 1-记录所有API调用日志，默认为1
	/// \return   int 错误代码
	////////////////////////////////////////
	int __cdecl CertEsunnyApi(TEsCertInfoType cert, TEsLogPathType path = NULL, TEsLogLevel logLevel = 1);

	////////////////////////////////////////
	/// \brief 获取动态库版本
	/// \param version 动态库版本号
	/// \return   void
	////////////////////////////////////////
	void __cdecl GetEsunnyApiVersion(TEsVersionType version);

	////////////////////////////////////////
	/// \fn    CreateEsunnyTradeApi
	/// \brief 创建交易处理API实例，如果认证失败返回NULL
	/// \return   IEsunnyTradeApi* 交易处理API实例的指针
	////////////////////////////////////////
	IEsunnyTradeApi* __cdecl CreateEsunnyTradeApi();

	////////////////////////////////////////
	/// \fn    CreateEsunnyQuoteApi
	/// \brief 创建行情处理API实例，如果认证失败返回NULL
	/// \return   IEsunnyQuoteApi* 行情处理API实例的指针
	////////////////////////////////////////
	IEsunnyQuoteApi* __cdecl CreateEsunnyQuoteApi();
}


#endif