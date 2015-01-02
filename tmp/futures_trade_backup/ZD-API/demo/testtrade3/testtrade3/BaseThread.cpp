#include "stdafx.h"
#include "BaseThread.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////
CBaseThread::CBaseThread(void)
{
	m_pParam = NULL;
	m_hThread = NULL;
	m_dwThreadID = 0;
}


//////////////////////////////////////////////////////////////////////////
CBaseThread::~CBaseThread(void)
{
	DeleteThread();
}


//////////////////////////////////////////////////////////////////////////
void CBaseThread::Release()
{
	delete this;
}


//////////////////////////////////////////////////////////////////////////
HANDLE CBaseThread::GetSafeHandle() const
{
	return m_hThread;
}


//////////////////////////////////////////////////////////////////////////
BOOL CBaseThread::PreStartThread()
{
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
BOOL CBaseThread::CreateThread(LPVOID pParam)
{
	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, (unsigned int (__stdcall*)(void*))ThreadProc, (void*)this, CREATE_SUSPENDED, (unsigned int *)&m_dwThreadID);
	if (m_hThread != NULL)
	{
		m_pParam = pParam;
		if(PreStartThread())
		{
			::ResumeThread(m_hThread);
		}
	}

	return (m_hThread == NULL ? FALSE : TRUE);
}


//////////////////////////////////////////////////////////////////////////
void CBaseThread::DeleteThread()
{
	if (m_hThread == NULL)
	{
		return ;
	}

	if (::WaitForSingleObject(m_hThread, 500) == WAIT_TIMEOUT)
	{
		HANDLE hTmpTrd = m_hThread;
		if (hTmpTrd != NULL)
		{
			::TerminateThread(hTmpTrd, 0);
			::CloseHandle(hTmpTrd);
			hTmpTrd = NULL;
		}
	}

	m_hThread = NULL;
}


//////////////////////////////////////////////////////////////////////////
DWORD CBaseThread::SuspendThread()
{
	return ::SuspendThread(m_hThread);
}


//////////////////////////////////////////////////////////////////////////
DWORD CBaseThread::ResumeThread()
{
	return ::ResumeThread(m_hThread);
}


//////////////////////////////////////////////////////////////////////////
BOOL CBaseThread::RunThread(LPVOID pParam)
{
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
void CBaseThread::ExitThread()
{
}


//////////////////////////////////////////////////////////////////////////
DWORD WINAPI CBaseThread::ThreadProc(LPVOID pParam)
{
	CBaseThread *pThis = (CBaseThread*)pParam;
	while (pThis->RunThread(pThis->m_pParam))
	{
		;
	}

	HANDLE hThread = pThis->GetSafeHandle();
	pThis->m_hThread = NULL;

	if (hThread != NULL)
	{
		::CloseHandle(hThread);
		hThread = NULL;
	}

	pThis->ExitThread();

	return 0;
}