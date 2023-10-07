
#include "../modules/px_udp.h"

#pragma comment(lib,"ws2_32.lib")                                               // Platform of MSVC 
#pragma comment( lib,"winmm.lib" )

#include <Windows.h>															//header 
#include <WinSock.h>
#include <Mmsystem.h>
#include "stdio.h"

int PX_UDPInitialize(PX_UDP *udp,PX_UDP_IP_TYPE type)
{
	WORD wVersionRequested; 
	WSADATA wsaData;        
	int err;           
	int nZero=0;
	int nRecvBuf=1024*1024;
	int nSendBuf=1024*1024;
	int optval=TRUE;
	int imode=1,rev;
	udp->type=type;

	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {

		return 0;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup();
			return 0;
	}
	//Initialize socket
	

	if ((udp->socket=(unsigned int)socket(AF_INET,SOCK_DGRAM,0))==INVALID_SOCKET)
	{
		return 0;
	}
	  
	rev=ioctlsocket(udp->socket,FIONBIO,(u_long *)&imode);  

	
	setsockopt(udp->socket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	setsockopt(udp->socket,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

	if(rev == SOCKET_ERROR)  
	{  
		printf("ioctlsocket failed!");  
		closesocket(udp->socket);  
		WSACleanup();  
		return 0;  
	}  

	if (setsockopt(udp->socket,SOL_SOCKET,SO_BROADCAST,(char *)&optval,sizeof(int))==SOCKET_ERROR)
	{
		closesocket(udp->socket);
		return 0;
	}
	return 1;
}

int PX_UDPSend(PX_UDP *udp,PX_UDP_ADDR addr,void *buffer,int size)
{
	
	int length;
	switch(udp->type)
	{
	case PX_UDP_IP_TYPE_IPV4:
		{
			SOCKADDR_IN to;
			to.sin_family=AF_INET;
			to.sin_addr.s_addr=addr.ipv4;
			to.sin_port=(addr.port);
			while(size>0)
			{
				if ((length=sendto(udp->socket,(const char *)buffer,size,0,(LPSOCKADDR)&to,sizeof(SOCKADDR)))==SOCKET_ERROR)
				{
					return FALSE;
				}
				size-=length;
			}
			return TRUE;
		}
		break;
	case PX_UDP_IP_TYPE_IPV6:
		{
// 			sockaddr_in6 to;
// 			to.sin6_family=AF_INET6;  
// 			to.sin6_port=htons(addr.port);  
// 			px_memcpy(to.sin6_addr,addr.ipv6,sizeof(addr.ipv6));
// 			while(size>0)
// 			{
// 				if ((length=sendto(udp->socket,(const char *)buffer,size,0,(LPSOCKADDR)&to,sizeof(sockaddr_in6)))==SOCKET_ERROR)
// 				{
// 					return FALSE;
// 				}
// 				size-=length;
// 			}
// 			return TRUE;
		}
		break;
	}
	return 0;
}

int PX_UDPReceived(PX_UDP *udp,PX_UDP_ADDR *from_addr,void *buffer,int buffersize,int *readsize)
{
	size_t ReturnSize;
	switch (udp->type)
	{
	case PX_UDP_IP_TYPE_IPV4:
		{
			SOCKADDR_IN in;
			int SockAddrSize=sizeof(SOCKADDR);
			if((ReturnSize=recvfrom(udp->socket,(char *)buffer,buffersize,0,(LPSOCKADDR)&in,&SockAddrSize))!=SOCKET_ERROR)
			{
				from_addr->ipv4=in.sin_addr.s_addr;
				from_addr->port=in.sin_port;
				*readsize=(int)ReturnSize;
				return 1;
			}
			else
				return 0;
		}
		break;
	case PX_UDP_IP_TYPE_IPV6:
		{
// 			sockaddr_in6 in;
// 			int SockAddrSize=sizeof(SOCKADDR);
// 			if((ReturnSize=recvfrom(udp->socket,(char *)buffer,buffersize,0,(LPSOCKADDR)&in,&SockAddrSize))!=SOCKET_ERROR)
// 			{
// 				px_memcpy(from_addr->ipv6,in.sin6_addr,sizeof(from_addr->ipv6));
// 				from_addr->port=in.sin_port;
// 				*readsize=ReturnSize;
// 				return 1;
// 			}
// 			else
// 				return 0;
		}
		break;
	}
	return 0;
}

void PX_UDPFree(PX_UDP *udp)
{
	closesocket(udp->socket);
}


int PX_UDPListen(PX_UDP *udp,unsigned short listen_Port)
{
	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family=AF_INET;
	sockaddr_in.sin_addr.s_addr=INADDR_ANY;
	sockaddr_in.sin_port=(listen_Port);

	if (bind(udp->socket,(LPSOCKADDR)&sockaddr_in,sizeof(sockaddr_in))==SOCKET_ERROR)
	{
		closesocket(udp->socket);
		return 0;
	}
	return 1;
}

PX_UDP_ADDR PX_UDP_ADDR_IPV4(unsigned int ipv4,unsigned short port)
{
	PX_UDP_ADDR addr;
	addr.ipv4=ipv4;
	addr.port=port;
	return addr;
}

unsigned int PX_UDPGetHostByName(const char *host, unsigned int dns_addr)
{
	PX_UDP udp;
	if (PX_UDPInitialize(&udp, PX_UDP_IP_TYPE_IPV4))
	{
		int strl,size,srcindex=0,ptrindex=12;
		int _tryt = 8;
		PX_UDP_ADDR target;
		unsigned char content[1024] = { 0x01,0xEC,0x01,0,0,1,0,0,0,0,0,0,0,0 };
		while (host[srcindex])
		{
			unsigned char* plen = &content[ptrindex];
			*plen = 0;
			ptrindex++;
			while (1)
			{
				if (host[srcindex]==0)
				{
					break;
				}
				else if (host[srcindex] == '.')
				{
					srcindex++;
					break;
				}
				else
				{
					content[ptrindex] = host[srcindex];
					(*plen)++;
				}
				ptrindex++;
				srcindex++;
			}
			if (host[srcindex] == 0)
			{
				break;
			}
		}
		strl = (int)strlen(host);
		content[13 + strl + 1] = 0;
		content[13 + strl + 2] = 1;
		content[13 + strl + 3] = 0;
		content[13 + strl + 4] = 1;
		target.ipv4 = dns_addr;
		target.port = 53<<8;

		while (_tryt--)
		{
			PX_UDPSend(&udp, target, content, 12 + 1 + strl + 1 + 4);
			Sleep(200);
			if (PX_UDPReceived(&udp, &target, content, sizeof(content), &size))
			{
				return (content[size - 1] << 24) + (content[size - 2] << 16) + (content[size - 3] << 8) + (content[size - 4]);
			}
		}
		
	}
	return 0;
}