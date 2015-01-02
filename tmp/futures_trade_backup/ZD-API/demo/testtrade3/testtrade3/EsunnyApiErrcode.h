//////////////////////////////////////////////////////////////////////////
/// \file EsunnyApiErrcode.h
/// \brief 易盛动态库返回的错误代码
/// 
///  调用接口函数返回值的具体意义
//////////////////////////////////////////////////////////////////////////

#ifndef _ESUNNY_API_INNER_ERROR_CODE_H_
#define _ESUNNY_API_INNER_ERROR_CODE_H_

#define ES_ERRCODE_SUCCESS              0       ///< 操作成功
#define ES_ERRCODE_NEWWORK_FAILED       -1      ///< 网络断开或故障
#define ES_ERRCODE_INVALID_LOGINTYPE    -2      ///< 不是有效的登陆类型
#define ES_ERRCODE_IN_QUERYING          -3      ///< 正在查询，请稍后重试
#define ES_ERRCODE_LOGTYPE_NOT_SUPPORT  -4      ///< 该用户类型不支持本操作
#define ES_ERRCODE_BANK_TRADE_TYPE_ERR  -5      ///< 转账类型不正确
#define ES_ERRCODE_EXCEED_MAX_REQ       -6      ///< 超过每秒最大请求数量
#define ES_ERRCODE_CERT_CODE_ERR        -7      ///< 认证码错误或过期
#define ES_ERRCODE_OPEN_API_LOG_ERR     -8      ///< 打开API日志文件失败

#endif
