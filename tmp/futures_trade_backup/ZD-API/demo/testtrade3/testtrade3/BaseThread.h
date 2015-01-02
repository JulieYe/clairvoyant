#pragma once

#include <windows.h>

class CBaseThread
{
public:
	CBaseThread(void);
	virtual ~CBaseThread(void);

	virtual void Release();

public:
	HANDLE GetSafeHandle() const;

	//////////////////////////////////////////////////////////////////////////
	BOOL CreateThread(LPVOID pParam);
	void DeleteThread();
	
	//////////////////////////////////////////////////////////////////////////
	DWORD SuspendThread();
	DWORD ResumeThread();

	//////////////////////////////////////////////////////////////////////////
	virtual BOOL PreStartThread();
	virtual BOOL RunThread(LPVOID pParam);
	virtual void ExitThread();

protected:
	static DWORD WINAPI ThreadProc(LPVOID pParam);
	HANDLE m_hThread;
	DWORD m_dwThreadID;
	void *m_pParam;
};
