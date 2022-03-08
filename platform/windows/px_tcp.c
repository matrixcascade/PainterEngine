
#include "../modules/px_tcp.h"

#pragma comment(lib,"ws2_32.lib")                                               // Platform of MSVC 
#pragma comment( lib,"winmm.lib" )

#include <Windows.h>															//header 
#include <WinSock.h>
#include <Mmsystem.h>
#include "stdio.h"


int PX_TCPInitialize(PX_TCP *tcp,PX_TCP_IP_TYPE type)
{
	WORD wVersionRequested; 
	WSADATA wsaData;        
	int err;           
	int nZero=0;
	int nRecvBuf=1024*1024*2;
	int nSendBuf=1024*1024*2;
	int optval=TRUE;

	tcp->type=type;

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

	if ((tcp->socket=(unsigned int)socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{
		return 0;
	}
	  	
	setsockopt(tcp->socket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	setsockopt(tcp->socket,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

	return 1;
}

int PX_TCPConnect(PX_TCP *tcp,PX_TCP_ADDR addr)
{
	SOCKADDR_IN to;
	int ret;
	to.sin_family=AF_INET;
	to.sin_addr.s_addr=addr.ipv4;
	to.sin_port=(addr.port);
	
	ret=connect(tcp->socket,(LPSOCKADDR)&to,sizeof(to));
	if (ret==0)
	{
		tcp->connectAddr=addr;
		return 1;
	}
	return 0;
}

int PX_TCPSend(PX_TCP *tcp,void *buffer,int size)
{
	char *sendBuffer=(char *)buffer;
	int length;
	switch(tcp->type)
	{
	case PX_TCP_IP_TYPE_IPV4:
		{
			do
			{
				if ((length=send(tcp->socket,(const char *)sendBuffer,size,0))==SOCKET_ERROR)
				{
					int error=WSAGetLastError();
					return 0;
				}
				sendBuffer+=length;
				size-=length;
			}while(size>0);
			return 1;
		}
		break;
	case PX_TCP_IP_TYPE_IPV6:
		{
			return 0;
		}
		break;
	}
	return 0;
}

int PX_TCPReceived(PX_TCP *tcp,void *buffer,int buffersize,int timeout)
{
	size_t ReturnSize;
	if (timeout>0)
	{
		int ret = setsockopt(tcp->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
		if (ret != 0)
		{
			return 0;
		}
	}
	 

	switch (tcp->type)
	{
	case PX_TCP_IP_TYPE_IPV4:
		{
			int SockAddrSize=sizeof(SOCKADDR);
			if((ReturnSize=recv(tcp->socket,(char *)buffer,buffersize,0))!=SOCKET_ERROR)
			{
				return (int)ReturnSize;
			}
			else
			{
				int error=WSAGetLastError();
				return 0;
			}
		}
		break;
	case PX_TCP_IP_TYPE_IPV6:
		{
			return 0;
		}
		break;
	}
	return 0;
}

int PX_TCPSocketReceived(unsigned int socket, void* buffer, int buffersize, int timeout)
{
	int ReturnSize;
	int SockAddrSize = sizeof(SOCKADDR);

	if (timeout > 0)
	{
		int ret = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
		if (ret != 0)
		{
			return 0;
		}
	}

	
	ReturnSize= recv(socket, (char*)buffer, buffersize, 0);
	if (ReturnSize==-1)
	{
		int error = WSAGetLastError();
		if (error == 10060)
			return 0;
	}
	return ReturnSize;
}

int PX_TCPSocketSend(unsigned int socket, void* buffer, int size)
{
	char* sendBuffer = (char*)buffer;
	int length;
	int sendsize = size;
	do
	{
		if ((length = send(socket, (const char*)sendBuffer, size, 0)) == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return 0;
		}
		sendBuffer += length;
		size -= length;
	} while (size > 0);
	return sendsize;
}

int PX_TCPAccept(PX_TCP *tcp,unsigned int *socket,PX_TCP_ADDR *fromAddr)
{
	DWORD lasterror;
	SOCKADDR_IN sockaddr_in;
	int len=sizeof(SOCKADDR);
	*socket=(unsigned int)accept((SOCKET)(tcp->socket),(LPSOCKADDR)&sockaddr_in,&len);
	lasterror=WSAGetLastError();
	return *socket!=INVALID_SOCKET;
}


int PX_TCPIsConnecting(PX_TCP *tcp)
{
	char b;
	int err,ret;
	int timeout=1;
	setsockopt(tcp->socket,SOL_SOCKET,SO_RCVTIMEO,(const char *)&timeout,sizeof(timeout));
	ret=recv(tcp->socket,&b,1,MSG_PEEK);
	if ((err=WSAGetLastError())==10060)
	{
		return 1;
	}
	return ret>0;
}

int PX_TCPKeepConnect(PX_TCP *tcp)
{
	if (!PX_TCPIsConnecting(tcp))
	{
		return PX_TCPConnect(tcp,tcp->connectAddr);
	}
	return 1;
}

int PX_TCPRecvCacheSize(PX_TCP *tcp)
{
	unsigned long bytesToRecv;
	if (ioctlsocket(tcp->socket, FIONREAD, &bytesToRecv) == 0)
	{
		return bytesToRecv;
	}
	return 0;
}

int PX_TCPReConnect(PX_TCP *tcp)
{
	return PX_TCPConnect(tcp,tcp->connectAddr);
}

void PX_TCPFree(PX_TCP *tcp)
{
	closesocket(tcp->socket);
}
void PX_TCPSocketFree(unsigned int socket)
{
	closesocket(socket);
}


int PX_TCPListen(PX_TCP *tcp,unsigned short listen_Port)
{
	SOCKADDR_IN sockaddr_in;
	memset(&sockaddr_in,0,sizeof(SOCKADDR_IN));
	sockaddr_in.sin_family=AF_INET;
	sockaddr_in.sin_addr.s_addr=INADDR_ANY;
	sockaddr_in.sin_port=(listen_Port);

	if (bind(tcp->socket,(LPSOCKADDR)&sockaddr_in,sizeof(sockaddr_in))!=0)
	{
		closesocket(tcp->socket);
		return 0;
	}

	if (listen(tcp->socket,5)!=0)
	{
		closesocket(tcp->socket);
		return 0;
	}
	return 1;
}

PX_TCP_ADDR PX_TCP_ADDR_IPV4(unsigned int ipv4,unsigned short port)
{
	PX_TCP_ADDR addr;
	addr.ipv4=ipv4;
	addr.port=port;
	return addr;
}

