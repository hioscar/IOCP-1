#include "stdafx.h"

#include "GLog.h"
#include "GMemory.h"
#include "GWorkerThread.h"
#include "GThread.h"
#include "GSocketInside.h"
#include "GSocket.h"
#include "GProcesserThread.h"

/*********************************************************************************
                   变量定义和初始化
*********************************************************************************/
HANDLE								hGWkrThrdCompletionPort = 0;
PGTHREAD							pGWkrThrdWorkerAddr = NULL;
DWORD								dwGWkrThrdWorkerThreadNumber = 0;
DWORD								dwGWkrThrdConcurrentThreadNumber = 0;
BOOL								bGWkrThrdIsActive = FALSE;

/*********************************************************************************
                   工作线程
*********************************************************************************/

void GWkrThrdWorkerThread(PGTHREAD pWorker)
{
	BOOL bResult;
	DWORD dwBytes;
	PGIOCP_COMPLETION_KEY pCompletionKey;
	LPOVERLAPPED pOverlapped;

	for(;;)
	{
		#if(_RUN_INFO)
		pWorker->dwState = GTHREAD_STATE_SLEEP;
		#endif

		bResult = GetQueuedCompletionStatus(hGWkrThrdCompletionPort, &dwBytes, (PULONG_PTR)&pCompletionKey, &pOverlapped, INFINITE);		

		if(pWorker->bIsShutdown)
			break;
		if(!pOverlapped)
			continue;		

		#if(_USE_PROCESS_THREAD)

		#if(_RUN_INFO)
		pWorker->dwState = GTHREAD_STATE_WORKING1;
		#endif

		PostQueuedCompletionStatus(hGProcThrdThreadCompletionPort, (0x80000000 * (bResult == 0)) | dwBytes, (DWORD)pCompletionKey, (LPOVERLAPPED)pOverlapped);

		#else

		if(bResult)
		{
			#if(_RUN_INFO)
			pWorker->dwState = GTHREAD_STATE_WORKING1;
			#endif

			pCompletionKey->pfnOnIocpOper(dwBytes, pCompletionKey, pOverlapped);
		}else
		{
			#if(_RUN_INFO)
			pWorker->dwState = GTHREAD_STATE_WORKING2;
			#endif

			pCompletionKey->pfnOnIocpError(pCompletionKey, pOverlapped);
		}

		#endif

		#if(_RUN_INFO)
		pWorker->dwRunCount++;
		#endif
	}
}

/*********************************************************************************
                   完成端口模块功能函数
*********************************************************************************/
DWORD GWkrThrd_GetNumberOfProcessors(void)
{
	SYSTEM_INFO SystemInfo;
	
	GetSystemInfo(&SystemInfo);	
	return(SystemInfo.dwNumberOfProcessors);
}

DWORD GWkrThrd_GetWorkerThreadNumber(void)
{
	return(dwGWkrThrdWorkerThreadNumber);
}

DWORD GWkrThrd_GetWorkerThreadNumberDef(void)
{
	return(GWkrThrd_GetNumberOfProcessors() * 2 + 2);
}

void GWkrThrd_SetWorkerThreadNumber(DWORD dwNumber)
{
	if(bGWkrThrdIsActive)
		return;
	dwGWkrThrdWorkerThreadNumber = dwNumber;
}

DWORD GWkrThrd_GetConcurrentThreadNumber(void)
{
	return(dwGWkrThrdConcurrentThreadNumber);
}

void GWkrThrd_SetConcurrentThreadNumber(DWORD dwNumber)
{
	if(bGWkrThrdIsActive)
		return;
	dwGWkrThrdConcurrentThreadNumber = dwNumber;
}

void GWkrThrd_Create(void)
{
	if(bGWkrThrdIsActive)
		return;

	DWORD dwNP = GWkrThrd_GetNumberOfProcessors();
	
	if(!dwGWkrThrdWorkerThreadNumber)
		dwGWkrThrdWorkerThreadNumber = dwNP * 2 + 2;

	if(dwGWkrThrdWorkerThreadNumber < dwNP)
		dwGWkrThrdWorkerThreadNumber = dwNP;

	if(dwGWkrThrdConcurrentThreadNumber > dwGWkrThrdWorkerThreadNumber)
		dwGWkrThrdConcurrentThreadNumber = dwGWkrThrdWorkerThreadNumber;

	hGWkrThrdCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, dwGWkrThrdConcurrentThreadNumber);
	if(NULL == hGWkrThrdCompletionPort)
	{
		GLog_Write("GWkrThrd_Create：完成端口句柄创建失败");
		return;
	}
	
	pGWkrThrdWorkerAddr = (PGTHREAD)GMem_Alloc(dwGWkrThrdWorkerThreadNumber * sizeof(GTHREAD));
	if(!pGWkrThrdWorkerAddr)
	{
		dwGWkrThrdWorkerThreadNumber = 0;
		CloseHandle(hGWkrThrdCompletionPort);
		GLog_Write("GWkrThrd_Create：从GMem分配工作者内存失败");
		return;
	}

	DWORD i;
 	PGTHREAD pWorker;
	
	pWorker = pGWkrThrdWorkerAddr;
	for(i = 0; i < dwGWkrThrdWorkerThreadNumber; i++)
	{
		GThrd_CreateThread(pWorker, GTHREAD_TYPE_IOCP, "工作者", &GWkrThrdWorkerThread);
		if(!pWorker->dwThreadId)
		{
			GLog_Write("GWkrThrd_Create：创建工作者线程失败");
			return;
		}
		pWorker = pWorker + 1;
	}
	bGWkrThrdIsActive = TRUE;		
}

void GWkrThrd_Destroy(void)
{
	bGWkrThrdIsActive = FALSE;
	if(!pGWkrThrdWorkerAddr)
		return;

	PGTHREAD pWorker;

	DWORD i;

	pWorker = pGWkrThrdWorkerAddr;
	for(i = 0; i < dwGWkrThrdWorkerThreadNumber; i++)
	{
		pWorker->bIsShutdown = TRUE;
		pWorker = pWorker + 1;
	}
	
	for(i = 0; i < dwGWkrThrdWorkerThreadNumber; i++)
		PostQueuedCompletionStatus(hGWkrThrdCompletionPort, 0, 0, NULL);
	
	pWorker = pGWkrThrdWorkerAddr;
	for(i = 0; i < dwGWkrThrdWorkerThreadNumber; i++)
	{
		GThrd_DestroyThread(pWorker);
		pWorker = pWorker + 1;
	}

	GMem_Free(pGWkrThrdWorkerAddr);
	CloseHandle(hGWkrThrdCompletionPort);
	hGWkrThrdCompletionPort = 0;
	dwGWkrThrdWorkerThreadNumber = 0;
	pGWkrThrdWorkerAddr = NULL;
}
  
  