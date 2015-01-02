//////////////////////////////////////////////////////////////////////////
/// \file EsunnyApiType.h
/// \brief 易盛动态库所用到的基本数据类型
/// 
///  业务处理中所用到的数据结构都是由基本数据类型组合而成的
//////////////////////////////////////////////////////////////////////////

#ifndef _ESUNNY_API_TYPE_
#define _ESUNNY_API_TYPE_

///认证信息
typedef char TEsCertInfoType[201];
///日志路径
typedef char TEsLogPathType[1024];
///API日志级别 0-记录所有API调用日志 1-记录操作类API调用日志
typedef int  TEsLogLevel;

///版本信息
typedef char TEsVersionType[51];

///ip地址类型
/*! \var typedef char TEsIpType[21];
 \brief ip地址类型
*/
typedef char TEsIpType[21];
///端口类型
typedef int	 TEsPortType;

///错误号 0-成功
typedef int	 TEsErrorCodeType;
///错误信息 
typedef char TEsErrorInfoType[51];
///是否查询结束
typedef bool TEsIsLastType;

///日期时间类型  yyyy-MM-dd HH:MM:SS
typedef char TEsDateTimeType[21];
///日期类型 yyyy-MM-dd
typedef char TEsDateType[11];
///时间类型 HH:MM:SS
typedef char TEsTimeType[11];

///会员号
typedef char TEsMemberNoType[21];
///席位号
typedef char TEsPosNoType[21];

///登录类型 'C'-单客户,'T'-交易员
typedef char TEsLoginTypeType;
///登录号
typedef char TEsLoginNoType[21];
///客户号
typedef char TEsClientNoType[21];
///客户姓名
typedef char TEsClientNameType[21];
///密码
typedef char TEsPasswordType[21];

///数据流号 标识委托和成交数据的先后顺序，以及查询数据的起始位置
typedef int	 TEsStreamIDType;
///请求编号 客户端生成,首次应答返回, 后续状态变更通过委托号辨认
typedef int	 TEsRequestIndexType;
///委托号  所有报入 交易服务器的报单  均生成委托号
typedef char TEsOrderNoType[21];
///本地号  通过交易服务器 报入交易所的报单  均生成本地号
typedef char TEsLocalNoType[21];
///系统号  报入交易所的报单 交易所系统生成 系统号
typedef char TEsSystemNoType[21];

///成交号
typedef char TEsMatchNoType[21];

///委托类型 0-限价,1-市价
typedef int	 TEsOrderTypeType;


/// \brief 委托状态 0-无效单,1-新单,2-已排队,3-完全成交,4-撤单请求,5-已撤单,6-埋单,7-埋单删除,8-已发送,
///9-待撤,10-部分成交,11-非交易时间,12-资金不足,13-交易关闭,14-指令失败,15-撤单失败,16-系统撤单,
///17-触发单,18-触发单删除,19-挂起,20-激活,21-已受理,22-自动单,23-自动单删除
typedef int	 TEsOrderStateType; 

///组合类型 0-单腿,1-跨期,2-跨品种
typedef int	 TEsCmbTypeType;

///市场 '1'-郑州,'2'-大连,'3'-上海,'5'-中金
typedef char TEsExchangeType;
///商品代码  例如：白糖 SR
typedef char TEsCommodityType[21];
///商品中文名称
typedef char TEsCommodityNameType[21];
///商品每手乘数
typedef int	TEsCommodityDotType;
///商品最小变动价
typedef double TEsCommodityTickType;
///合约代码  例如：白糖 SR011
typedef char TEsContractType[21];

///合约中文名称
typedef char TEsContractNameType[21];

///买卖方向 '1'-买入,'3'-卖出
typedef char TEsDirectType;
///开平	'1'-开仓,'2'-平今(上期所平今),'3'-平仓(上期所平昨)
typedef char TEsOffsetType;
///投保	'1'-投机,'3'-保值
typedef char TEsHedgeType;

///交易数量
typedef int	 TEsTradeVolType;
///交易价格
typedef double TEsTradePriceType;

///资金
typedef double TEsMoneyValueType;

/// \brief 市场状态 '0'-系统初始化中,'1'-系统就绪，可以登录,'2'-开盘竞价入单阶段
/// '3'-开盘集合竞价阶段（停止入单）, '4'-连续交易阶段（开市）,'5'-连续交易暂停阶段
/// '6'-开盘竞价入单暂停阶段,'7'-收市（闭市）,'8'-断开与交易所连接
typedef char TEsExchangeStateType;

///行情价格
typedef double TEsQuotePriceType;
///行情数量
typedef int TEsQuoteVolType;
///订阅行情类型  'S'-订阅,'C'-取消
typedef char TEsSubQuoteType;

///转账分组号
typedef char TEsBankGroupNoType[21];
///银行标识 '1'-工行,'2'-农行分行,'3'-农行总行,'4'-建行,'5'-交行
typedef char TEsBankType;
///银行帐号
typedef char TEsBankClientNoType[21];
///身份证号
typedef char TEsPersonIdType[21];
///客户姓名
typedef char TEsPersonNameType[21];
///转账状态 'Y'-允许,'N'-禁止
typedef char TEsBankClientStateType;
///转账方向 'B'-银行转期货 'F'-期货转银行
typedef char TEsBankTradeTypeType;
///转账金额
typedef double TEsBankMoneyType;
///发起方向 'B'-银行发起 'F'-期货发起
typedef char TEsBankPackFlagType;
///交易类型 1-银行转期货，2-期货转银行，11-冲正银行转期货，12-冲正期货转银行，99-线路检查
typedef int	TEsBankTrTradeTypeType;
///转账流水号
typedef char TEsBankSerNo[11];
///转账返回码
typedef char TEsBankRetCode[5];

///持仓标识
typedef char TEsHoldKey[81];


#endif