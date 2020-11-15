
#ifndef ANDROID_TCP_H
#define ANDROID_TCP_H

//GNU C
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/endian.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

typedef enum
{
	PX_TCP_IP_TYPE_IPV4,
	PX_TCP_IP_TYPE_IPV6
}PX_TCP_IP_TYPE;

typedef struct  
{
	unsigned int port;
	union
	{
		unsigned int ipv4;
		unsigned int ipv6[4];
	};
}PX_TCP_ADDR;

typedef struct
{
	unsigned int listenPort;
	unsigned int socket;
	PX_TCP_IP_TYPE type;
	PX_TCP_ADDR connectAddr;
}PX_TCP;



int PX_TCPListen(PX_TCP *tcp,unsigned short listenPort);
int PX_TCPInitialize(PX_TCP *tcp,PX_TCP_IP_TYPE type);
int PX_TCPConnect(PX_TCP *tcp,PX_TCP_ADDR addr);
int PX_TCPSend(PX_TCP *tcp,void *buffer,int size);
int PX_TCPReceived(PX_TCP *tcp,void *buffer,int buffersize,int *readsize);
int PX_TCPAccept(PX_TCP *tcp,unsigned int *socket,PX_TCP_ADDR *fromAddr);
void PX_TCPFree(PX_TCP *tcp);
#endif

