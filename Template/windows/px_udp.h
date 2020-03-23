#ifndef PX_UDP_H
#define PX_UDP_H
#include "../../Core/PX_Core.h"

#ifdef _MSC_VER
#pragma comment(lib,"ws2_32.lib")                                               // Platform of MSVC 
#pragma comment( lib,"winmm.lib" )

#include <Windows.h>															//header 
#include <WinSock.h>
#include <strsafe.h>
#include <Mmsystem.h>
#include <assert.h>
#endif

#ifdef __GNUC__																	//GNU C
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#endif

typedef enum
{
	PX_UDP_IP_TYPE_IPV4,
	PX_UDP_IP_TYPE_IPV6
}PX_UDP_IP_TYPE;

typedef struct
{
	px_uint listenPort;
	px_uint socket;
	PX_UDP_IP_TYPE type;
}PX_UDP;

typedef struct  
{
	px_dword port;
	union
	{
		px_dword ipv4;
		px_dword ipv6[4];
	};
}PX_UDP_ADDR;
PX_UDP_ADDR PX_UDP_ADDR_IPV4(px_dword ipv4,px_ushort port);
px_dword PX_UDPaton(px_char *ipaddr);
px_bool PX_UDPListen(PX_UDP *udp,px_ushort listenPort);
px_bool PX_UDPInit(PX_UDP *udp,PX_UDP_IP_TYPE type);
px_bool PX_UDPSend(PX_UDP *udp,PX_UDP_ADDR addr,px_byte *buffer,px_int size);
px_bool PX_UDPReceived(PX_UDP *udp,PX_UDP_ADDR *from_addr,px_byte *buffer,px_int buffersize,px_int *readsize);
px_void PX_UDPFree(PX_UDP *udp);
#endif
