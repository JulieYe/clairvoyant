// stdafx.cpp : source file that includes just the standard includes
//	TestTrade.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
char FRONT_ADDR[MAX_PATH] = "tcp://asp-sim2-front1.financial-trading-platform.com:26205";
TThostFtdcBrokerIDType	BROKER_ID = "2030";
int g_iRequestID = 0;
CThostFtdcTraderApi* g_pUserApi = NULL;
CTradeSpi* g_pUserSpi = NULL;

TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
TThostFtdcOrderRefType	ORDER_REF;	//��������
int g_iOrderRef = 0;

bool g_bReLogin = false;
TThostFtdcInvestorIDType g_szInvestorID = { 0 };
TThostFtdcPasswordType g_szPassword = { 0 };

int g_nEnvType = 0;

//////////////////////////////////////////////////////////////////////////
char g_szLogFileName[MAX_PATH] = "log.txt";
char g_szSeparator[] = "------------------------------------------------------------";

void WriteLog(char* pszMsg, BOOL bSep)
{
	if(pszMsg != NULL && strlen(pszMsg) > 0)
	{
		FILE* fp = fopen(g_szLogFileName, "a");
		if(fp != NULL)
		{
			fprintf(fp, "%s\n", pszMsg);
			//TRACE("%s\n", pszMsg);
			fclose(fp);
			fp = NULL;
		}

		if(bSep)
		{
			WriteSeparator();
		}
	}
}

void WriteSeparator()
{
	FILE* fp = fopen(g_szLogFileName, "a");
	if(fp != NULL)
	{
		fprintf(fp, "%s\n", g_szSeparator);
		TRACE("%s\n", g_szSeparator);
		fclose(fp);
		fp = NULL;
	}
}
