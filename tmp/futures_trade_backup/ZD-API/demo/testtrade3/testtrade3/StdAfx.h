// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__CC947C24_EAE4_4F8E_BD8B_A2A68D00230A__INCLUDED_)
#define AFX_STDAFX_H__CC947C24_EAE4_4F8E_BD8B_A2A68D00230A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define UM_ADD_MSG (WM_USER + 1)

#define __ENABLED_LOG__
#ifdef __ENABLED_LOG__
#define WLOG(m, s) WriteLog(m, s);
#else
#define WLOG(m,s) ;
#endif

//////////////////////////////////////////////////////////////////////////
#include "ThostFtdcTraderApi.h"
#include "TradeSpi.h"

//////////////////////////////////////////////////////////////////////////
extern char FRONT_ADDR[MAX_PATH];
extern TThostFtdcBrokerIDType BROKER_ID;
extern int g_iRequestID;

extern CThostFtdcTraderApi*	g_pUserApi;
extern CTradeSpi*	g_pUserSpi;

extern TThostFtdcFrontIDType	FRONT_ID;	//前置编号
extern TThostFtdcSessionIDType	SESSION_ID;	//会话编号
extern TThostFtdcOrderRefType	ORDER_REF;	//报单引用
extern int g_iOrderRef;

extern bool g_bReLogin;
extern TThostFtdcInvestorIDType g_szInvestorID;
extern TThostFtdcPasswordType g_szPassword;


extern char g_szLogFileName[];

extern int g_nEnvType;

void WriteLog(char* pszMsg, BOOL bSep);
void WriteSeparator();


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__CC947C24_EAE4_4F8E_BD8B_A2A68D00230A__INCLUDED_)
