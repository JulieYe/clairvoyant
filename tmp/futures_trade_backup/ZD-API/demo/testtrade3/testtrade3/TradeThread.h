// TradeThread.h: interface for the CTradeThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRADETHREAD_H__4318B45B_83DF_4E45_A4FE_D3625751A4C6__INCLUDED_)
#define AFX_TRADETHREAD_H__4318B45B_83DF_4E45_A4FE_D3625751A4C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseThread.h"

BOOL CreateFolder(const CString& strFolder);

class CTradeThread : public CBaseThread  
{
public:
	CTradeThread();
	virtual ~CTradeThread();
	
public:
	virtual void Release();
	virtual BOOL RunThread(LPVOID pParam);
	virtual void DeleteThread();

};

#endif // !defined(AFX_TRADETHREAD_H__4318B45B_83DF_4E45_A4FE_D3625751A4C6__INCLUDED_)
