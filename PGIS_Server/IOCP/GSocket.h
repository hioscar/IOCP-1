#pragma once

/*********************************************************************************
                   条件编译
*********************************************************************************/
#define _G_FUNC_MESSAGELIST_DATA				0

#define _GSOCKET_FUNC_TCP_SERVER				1
#define _GSOCKET_FUNC_TCP_CLIENT				1
#define _GSOCKET_FUNC_UDP_SERVER				0
#define _GSOCKET_FUNC_UDP_CLIENT				1

#define _USE_INTERLOCKED_IN_LIST				0
#define _USE_SINGLY_LINKED_LIST_IN_IODATA_POOL	0

#define _REUSED_SOCKET							0
#define _USE_GPROTOCOL							0
#define _USE_PROCESS_THREAD						0				//是否启用工作线程
#define _RUN_INFO								1

#define _OUTIODATA_INFO							0
#define _OUTWSDATA_INFO							0
#define _OUTIODATA_INFO_COUNT					1				//对使用内存类型进行计数

/*********************************************************************************
                   GSocket定义
*********************************************************************************/
typedef BOOL(*PFN_ON_GSOCK_TRAVERSAL)(const DWORD dwParam, const DWORD dwIndex, const DWORD dwContext);

typedef BOOL(*PFN_ON_GSOCK_FINDDATA_CHAR)(const DWORD dwParam, const DWORD dwIndex, const DWORD dwContext, const char* szData);
typedef BOOL(*PFN_ON_GSOCK_FINDDATA_DWORD)(const DWORD dwParam, const DWORD dwIndex, const DWORD dwContext, const DWORD dwData);
/*********************************************************************************
                  GThread
*********************************************************************************/
typedef enum _GTHREAD_TYPE
{
GTHREAD_TYPE_IOCP,
GTHREAD_TYPE_TCP_SERVER_SERVICE,
GTHREAD_TYPE_TCP_CLIENT_SERVICE,
GTHREAD_TYPE_UDP_SERVER_SERVICE,
GTHREAD_TYPE_UDP_CLIENT_SERVICE,
GTHREAD_TYPE_PROCESS,
GTHREAD_TYPE_DB_SERVICE
}GTHREAD_TYPE;

DWORD GThrd_GetThreadId(DWORD dwWorkerId);
DWORD GThrd_GetRunCount(DWORD dwWorkerId);
DWORD GThrd_GetState(DWORD dwThreadContext);
GTHREAD_TYPE GThrd_GetType(DWORD dwThreadContext);
char* GThrd_GetName(DWORD dwWorkerId);
void GThrd_TraversalThread(DWORD dwParam, PFN_ON_GSOCK_TRAVERSAL pfnOnProc);
DWORD GThrd_GetThreadCount(void);

void* GThrd_GetData(void);
void GThrd_SetData(DWORD dwWorkerId, void * pData);

typedef	void(*PFN_ON_GTHREAD_EVENT)(const DWORD dwThreadContext);
void GThrd_SetEventProc(PFN_ON_GTHREAD_EVENT pfnOnBeginProc, PFN_ON_GTHREAD_EVENT pfnOnEndProc);

/*********************************************************************************
                   GProcThrdessThread
*********************************************************************************/
#if(_USE_PROCESS_THREAD)
DWORD GProcThrd_GetThreadNumber(void);
void GProcThrd_SetThreadNumber(DWORD dwNumber);
#endif

/*********************************************************************************
                   GWkrThrd
*********************************************************************************/
DWORD GWkrThrd_GetNumberOfProcessors(void);
DWORD GWkrThrd_GetWorkerThreadNumberDef(void);
DWORD GWkrThrd_GetWorkerThreadNumber(void);
void GWkrThrd_SetWorkerThreadNumber(DWORD dwNumber);
DWORD GWkrThrd_GetConcurrentThreadNumber(void);
void GWkrThrd_SetConcurrentThreadNumber(DWORD dwNumber);

/*********************************************************************************
                   GHndData
*********************************************************************************/
typedef	void(*PFN_ON_GHND_EVENT)(const DWORD dwClientContext);
typedef	void(*PFN_ON_GHND_DATA_EVENT)(const DWORD dwClientContext, const char *pBuf, const DWORD Bytes);

typedef enum _GHANDLE_TYPE
{
GHND_TYPE_TCP_SVR_LISTEN,
GHND_TYPE_TCP_SVR_CLIENT,
GHND_TYPE_TCP_CLT_CLIENT,
GHND_TYPE_UDP_SVR_LISTEN,
GHND_TYPE_UDP_SVR_CLIENT,
GHND_TYPE_UDP_CLT_CLIENT,
}GHND_TYPE;


typedef enum _GHANDLE_STATE
{
GHND_STATE_DISCONNECT,
GHND_STATE_CONNECTED,
GHND_STATE_ACCEPTING,
GHND_STATE_CONNECTING,
GHND_STATE_LISTENING
}GHND_STATE;

DWORD GHndDat_GetMemBytes(void);
DWORD GHndDat_GetSize(void);
DWORD GHndDat_GetTotal(void);
void GHndDat_SetTotal(DWORD dwTotal);
DWORD GHndDat_GetUsed(void);
GHND_TYPE GHndDat_GetType(DWORD dwClientContext);
GHND_STATE GHndDat_GetState(DWORD dwClientContext);
DWORD GHndDat_GetAddr(DWORD dwClientContext);
DWORD GHndDat_GetPort(DWORD dwClientContext);
DWORD GHndDat_GetTickCountAcitve(DWORD dwClientContext);
DWORD GHndDat_GetOwner(DWORD dwClientContext);

void* GHndDat_GetData(DWORD dwClientContext);
void GHndDat_SetData(DWORD dwClientContext, void* pData);

void GHndDat_SetProcOnHndCreate(PFN_ON_GHND_EVENT pfnOnProc);
void GHndDat_SetProcOnHndDestroy(PFN_ON_GHND_EVENT pfnOnProc);

/*********************************************************************************
                   GIoData
*********************************************************************************/
typedef char* PGIO_BUF;

DWORD GIoDat_GetSize(void);
DWORD GIoDat_GetGBufSize(void);
void GIoDat_SetGBufSize(DWORD dwSize);
DWORD GIoDat_GetMemBytes(void);
DWORD GIoDat_GetTotal(void);
void GIoDat_SetTotal(DWORD dwTotal);
DWORD GIoDat_GetUsed(void);

PGIO_BUF GIoDat_AllocGBuf(void);
void GIoDat_FreeGBuf(PGIO_BUF pGIoBuf);

/*********************************************************************************
                   GUdpClient
*********************************************************************************/

DWORD GUdpClt_GetClientCount(void);

/*********************************************************************************
                   GTcpClient
*********************************************************************************/

DWORD GTcpClt_GetClientCount(void);
void GTcpClt_TraversalClient(DWORD dwParam, DWORD dwFromIndex, DWORD dwCount, PFN_ON_GSOCK_TRAVERSAL pfnOnProc);

void GUdpClt_PostBroadcastBuf(char* pBuf, DWORD dwBytes);
DWORD GTcpClt_CreateClient(char *pszRemoteIp, DWORD dwRemotePort, char *pszLocalIp, void* pOwner);
void GTcpClt_DestroyClient(DWORD dwClientContext);
void GTcpClt_DisconnectClient(DWORD dwClientContext);

/*********************************************************************************
                   GTcpServer
*********************************************************************************/
DWORD GTcpSvr_GetClientCount(void);
DWORD GTcpSvr_GetListenerCount(void);
DWORD GTcpSvr_GetPendingAcceptCount(void);

DWORD GTcpSvr_GetListenerConnectCount(DWORD dwListenerId);
void GTcpSvr_TraversalListener(DWORD dwParam, PFN_ON_GSOCK_TRAVERSAL pfnOnProc);
void GTcpSvr_TraversalClient(DWORD dwParam, DWORD dwFromIndex, DWORD dwCount, PFN_ON_GSOCK_TRAVERSAL pfnOnProc);

void GTcpSvr_PostBroadcastBuf(char* pBuf, DWORD dwBytes, DWORD dwParam, DWORD dwSvrContext, PFN_ON_GSOCK_TRAVERSAL pfnOnProc);	/*not good*/
void GTcpSvr_PostBroadcastBuf_Char(char* pBuf, DWORD dwBytes, DWORD dwParam, DWORD dwSvrContext, char* pSvrBuf, PFN_ON_GSOCK_FINDDATA_CHAR pfnOnProc); /*not good*/
DWORD GTcpSvr_CreateListen(char *lpszLocalIp, DWORD dwLocalPort, void* pOwner);
void GTcpSvr_CloseClient(DWORD dwClientContext);

/*********************************************************************************
                   GCommProt
*********************************************************************************/
BOOL GCommProt_PostSendGBuf(DWORD dwClientContext, PGIO_BUF pGBuf, DWORD dwBytes);
BOOL GCommProt_PostSendBuf(DWORD dwClientContext, char* pBuf, DWORD dwBytes);

/*********************************************************************************
                   GMessageList		cant use
*********************************************************************************/
typedef enum _GHANDLE_MESSAGETYPE
{
	AT_MOST_ONCE,	//至多一次，消息发布完全依赖底层 TCP/IP 网络。会发生消息丢失或重复。这一级别可用于如下情况，环境传感器数据，丢失一次读记录无所谓，因为不久后还会有第二次发送。 
	AT_LEAST_ONCE_SEND,	//至少一次，确保消息到达，但消息重复可能会发生。														消息发送模式[保存发送消息]
	AT_LEAST_ONCE_ASK,	//至少一次，确保消息到达，但消息重复可能会发生。														接收方发送应答，发送方收取应答，删除发送消息。
	EXACTLY_ONCE_SEND,	//只有一次，确保消息到达一次。这一级别可用于如下情况，在计费系统中，消息重复或丢失会导致不正确的结果。	//发送方发送MESSAGEID及消息 【ACTION:保存发送消息】 -SEND- 接收方接收数据 【ACTION:保存接收的数据】
	EXACTLY_ONCE_ASK,																											//接收方发送MESSAGEID	 -ASK-  发送方接收MESSAGEID
	EXACTLY_ONCE_REL,																											//发送方发送MESSAGEID    -REL-	接收方接收MESSAGEID		【ACTION:删除保存接收数据】
	EXACTLY_ONCE_COMP,																											//接收方发送MESSAGEID	-COMP	发送方接收MESSAGEID		【ACTION:删除保存发送数据】																										
}GHND_MESSAGETYPE;

DWORD GMessageList_CreateMessageList(DWORD dwClientContext, GHND_MESSAGETYPE EnumType, DWORD dwResend, const char *szData, void* pOwner);
void GMessageList_DestroyMessageList(DWORD dwClientContext);

/*********************************************************************************
                   GSocket
*********************************************************************************/
DWORD GSock_GetTimeAutoConnect(void);
void GSock_SetTimeAutoConnect(DWORD dwTime);
DWORD GSock_GetTimeIdleOvertime(void);
void GSock_SetTimeIdleOvertime(DWORD dwTime);
DWORD GSock_GetTimeHeartbeat(void);
void GSock_SetTimeHeartbeat(DWORD dwTime);

#if(_GSOCKET_FUNC_TCP_SERVER)
DWORD GSock_GetTimeAcceptOvertime(void);
void GSock_SetTimeAcceptOvertime(DWORD dwTime);
#endif//#if(_GSOCKET_FUNC_TCP_SERVER)

#if(_GSOCKET_FUNC_TCP_SERVER || _GSOCKET_FUNC_UDP_SERVER)
DWORD GSock_GetMaxNumberConnection(void);
void GSock_SetMaxNumberConnection(DWORD dwNumber);
#endif//#if(_GSOCKET_FUNC_TCP_SERVER || _GSOCKET_FUNC_UDP_SERVER)

#if(_GSOCKET_FUNC_TCP_SERVER)
DWORD GSock_GetNumberPostAccept(void);
void GSock_SetNumberPostAccept(DWORD dwNumber);
#endif//#if(_GSOCKET_FUNC_TCP_SERVER)

DWORD GSock_GetNumberPostRecv(void);
void GSock_SetNumberPostRecv(DWORD dwNumber);

#if(_GSOCKET_FUNC_TCP_SERVER)
BOOL GSock_IsZeroByteAccept(void);
void GSock_SetZeroByteAccept(BOOL bIsZeroBuffer);
#endif//#if(_GSOCKET_FUNC_TCP_SERVER

BOOL GSock_IsZeroByteRecvr(void);
void GSock_SetZeroByteRecv(BOOL bIsZeroBuffer);

#if(_GSOCKET_FUNC_TCP_SERVER)
void GSock_SetOnConnectProcTcpSvr(PFN_ON_GHND_DATA_EVENT pfnOnProc);
#endif

#if(_GSOCKET_FUNC_UDP_SERVER)
void GSock_SetOnConnectProcUdpSvr(PFN_ON_GHND_DATA_EVENT pfnOnProc);
#endif

#if(_GSOCKET_FUNC_TCP_SERVER || _GSOCKET_FUNC_UDP_SERVER)
void GSock_SetOnDisconnectProcSvr(PFN_ON_GHND_EVENT pfnOnProc);
void GSock_SetOnReceiveProcSvr(PFN_ON_GHND_DATA_EVENT pfnOnProc);
void GSock_SetOnSendedProcSvr(PFN_ON_GHND_DATA_EVENT pfnOnProc);
void GSock_SetOnSendErrorProcSvr(PFN_ON_GHND_DATA_EVENT pfnOnProc);
void GSock_SetOnConnectionOverflowProc(PFN_ON_GHND_EVENT pfnOnProc);

void GSock_SetOnCreateClientSvrProc(PFN_ON_GHND_EVENT pfnOnProc);
void GSock_SetOnDestroyClientSvrProc(PFN_ON_GHND_EVENT pfnOnProc);
#endif//#if(_GSOCKET_FUNC_TCP_SERVER || _GSOCKET_FUNC_UDP_SERVER)

#if(_GSOCKET_FUNC_TCP_CLIENT || _GSOCKET_FUNC_UDP_CLIENT)
void GSock_SetOnConnectProcClt(PFN_ON_GHND_DATA_EVENT pfnOnProc);
void GSock_SetOnDisconnectProcClt(PFN_ON_GHND_EVENT pfnOnProc);
void GSock_SetOnReceiveProcClt(PFN_ON_GHND_DATA_EVENT pfnOnProc);
void GSock_SetOnSendedProcClt(PFN_ON_GHND_DATA_EVENT pfnOnProc);
void GSock_SetOnSendErrorProcClt(PFN_ON_GHND_DATA_EVENT pfnOnProc);
#endif//#if(_GSOCKET_FUNC_TCP_CLIENT || _GSOCKET_FUNC_UDP_CLIENT)

#if(_GSOCKET_FUNC_TCP_CLIENT)
void GSock_SetOnConnectErrorProc(PFN_ON_GHND_EVENT pfnOnProc);
void GSock_SetOnCreateClientCltProc(PFN_ON_GHND_EVENT pfnOnProc);
void GSock_SetOnDestroyClientCltProc(PFN_ON_GHND_EVENT pfnOnProc);
#endif//#if(_GSOCKET_FUNC_TCP_CLIENT)

void GSock_SetOnIdleOvertimeProc(PFN_ON_GHND_EVENT pfnOnProc);
void GSock_SetOnHeartbeatProc(PFN_ON_GHND_EVENT pfnOnProc);
void GSock_SetUnknownErrorProc(PFN_ON_GHND_EVENT pfnOnProc);

void GSock_CloseClient(DWORD dwClientContext);

void* GSock_WSAGetExtensionFunctionPointer(SOCKET sSocket, GUID ExFuncGuid);
void GSock_GetLocalNetIp(char *szIp, int nSize, BOOL bIsIntnetIp);
void GSock_AddrToIp(char *szIp, int nSize, DWORD dwAddr);

BOOL GSock_IsActive(void);
void GSock_SetSocketSendBufSize(int nSize);
void GSock_SetSocketRecvBufSize(int nSize);
void GSock_SetMtuNodelay(BOOL bNodelay);
void GSock_StartService(void);
void GSock_StopService(void);

char* GSock_GetVersion(void);
DWORD GSock_GetClientCount(void);

