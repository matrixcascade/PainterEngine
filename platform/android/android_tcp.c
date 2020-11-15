
#include "android_tcp.h"

int PX_TCPInitialize(PX_TCP *tcp,PX_TCP_IP_TYPE type)
{
	int err;           
	int nZero=0;
	int nRecvBuf=1024*1024*2;
	int nSendBuf=1024*1024*2;
	int optval=1;
	int imode=1,rev;
	tcp->type=type;

	if ((tcp->socket=socket(AF_INET,SOCK_DGRAM,IPPROTO_TCP))==-1)
	{
        err = errno;
		return 0;
	}
	  
	rev=ioctl(tcp->socket,FIONBIO,(u_long *)&imode);
	setsockopt(tcp->socket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
	setsockopt(tcp->socket,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

	if(rev == -1)
	{
        close(tcp->socket);
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
					return 0;
				}
				size-=length;
			}
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
    close(tcp->socket);
}


int PX_TCPListen(PX_TCP *tcp,unsigned short listen_Port)
{
    struct sockaddr_in addrin;
    addrin.sin_family=AF_INET;
    addrin.sin_addr.s_addr=INADDR_ANY;
    addrin.sin_port=(listen_Port);

	if (bind(tcp->socket,(struct sockaddr *)&addrin,sizeof(struct sockaddr))==-1)
	{
        close(tcp->socket);
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
