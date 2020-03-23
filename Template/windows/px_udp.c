#include "px_udp.h"

px_bool PX_UDPInit(PX_UDP *udp,PX_UDP_IP_TYPE type)
{
	WORD wVersionRequested; 
	WSADATA wsaData;        
	int err;           
	int nZero=0;
	int nRecvBuf=1024*1024*2;
	int nSendBuf=1024*1024*2;
	px_bool optval=TRUE;
	int imode=1,rev;
	udp->type=type;

	wVersionRequested = MAKEWORD( 1, 1 );


	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {

		return PX_FALSE;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup();
			return PX_FALSE;
	}
	//Initialize socket
	

	if ((udp->socket=socket(AF_INET,SOCK_DGRAM,0))==INVALID_SOCKET)
	{
		return PX_FALSE;
	}
	  
	rev=ioctlsocket(udp->socket,FIONBIO,(u_long *)&imode);  

	
	setsockopt(udp->socket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	setsockopt(udp->socket,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

	if(rev == SOCKET_ERROR)  
	{  
		printf("ioctlsocket failed!");  
		closesocket(udp->socket);  
		WSACleanup();  
		return PX_FALSE;  
	}  

	if (setsockopt(udp->socket,SOL_SOCKET,SO_BROADCAST,(char *)&optval,sizeof(px_bool))==SOCKET_ERROR)
	{
		closesocket(udp->socket);
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_bool PX_UDPSend(PX_UDP *udp,PX_UDP_ADDR addr,px_byte *buffer,px_int size)
{
	
	px_int length;
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
	return PX_FALSE;
}

px_bool PX_UDPReceived(PX_UDP *udp,PX_UDP_ADDR *from_addr,px_byte *buffer,px_int buffersize,px_int *readsize)
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
				*readsize=ReturnSize;
				return PX_TRUE;
			}
			else
				return PX_FALSE;
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
// 				return PX_TRUE;
// 			}
// 			else
// 				return PX_FALSE;
		}
		break;
	}
	return PX_FALSE;
}

px_void PX_UDPFree(PX_UDP *udp)
{
	closesocket(udp->socket);
}

px_dword PX_UDPaton(px_char *ipaddr)
{
	return inet_addr(ipaddr);
}

px_bool PX_UDPListen(PX_UDP *udp,px_ushort listen_Port)
{
	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family=AF_INET;
	sockaddr_in.sin_addr.s_addr=INADDR_ANY;
	sockaddr_in.sin_port=(listen_Port);

	if (bind(udp->socket,(LPSOCKADDR)&sockaddr_in,sizeof(sockaddr_in))==SOCKET_ERROR)
	{
		closesocket(udp->socket);
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_UDP_ADDR PX_UDP_ADDR_IPV4(px_dword ipv4,px_ushort port)
{
	PX_UDP_ADDR addr;
	addr.ipv4=ipv4;
	addr.port=port;
	return addr;
}
