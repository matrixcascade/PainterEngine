
#include "windows_tcp.h"

int PX_TCPInitialize(PX_TCP *tcp,PX_TCP_IP_TYPE type)
{
	WORD wVersionRequested; 
	WSADATA wsaData;        
	int err;           
	int nZero=0;
	int nRecvBuf=1024*1024*2;
	int nSendBuf=1024*1024*2;
	int optval=TRUE;
	int imode=1,rev;
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
	

	if ((tcp->socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
	{
		return 0;
	}
	  
	rev=ioctlsocket(tcp->socket,FIONBIO,(u_long *)&imode);  

	
	setsockopt(tcp->socket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	setsockopt(tcp->socket,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

	if(rev == SOCKET_ERROR)  
	{  
		printf("ioctlsocket failed!");  
		closesocket(tcp->socket);  
		WSACleanup();  
		return 0;  
	}  

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
	}
	return ret;
}

int PX_TCPSend(PX_TCP *tcp,void *buffer,int size)
{
	
	int length;
	switch(tcp->type)
	{
	case PX_TCP_IP_TYPE_IPV4:
		{
			while(size>0)
			{
				
				if ((length=send(tcp->socket,(const char *)buffer,size,0))==SOCKET_ERROR)
				{
					return FALSE;
				}
				size-=length;
			}
			return TRUE;
		}
		break;
	case PX_TCP_IP_TYPE_IPV6:
		{
			return FALSE;
		}
		break;
	}
	return 0;
}

int PX_TCPReceived(PX_TCP *tcp,void *buffer,int buffersize,int *readsize)
{
	size_t ReturnSize;
	switch (tcp->type)
	{
	case PX_TCP_IP_TYPE_IPV4:
		{
			int SockAddrSize=sizeof(SOCKADDR);
			if((ReturnSize=recv(tcp->socket,(char *)buffer,buffersize,0))!=SOCKET_ERROR)
			{
				*readsize=ReturnSize;
				return 1;
			}
			else
				return 0;
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

int PX_TCPAccept(PX_TCP *tcp,unsigned int *socket,PX_TCP_ADDR *fromAddr)
{
	SOCKADDR_IN sockaddr_in;
	int len;
	*socket=accept(tcp->socket,(LPSOCKADDR)&sockaddr_in,&len);
	return *socket!=INVALID_SOCKET;
}


void PX_TCPFree(PX_TCP *tcp)
{
	closesocket(tcp->socket);
}


int PX_TCPListen(PX_TCP *tcp,unsigned short listen_Port)
{
	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family=AF_INET;
	sockaddr_in.sin_addr.s_addr=INADDR_ANY;
	sockaddr_in.sin_port=(listen_Port);

	if (bind(tcp->socket,(LPSOCKADDR)&sockaddr_in,sizeof(sockaddr_in))==SOCKET_ERROR)
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

