
#ifndef ANDROID_UDP_H
#define ANDROID_UDP_H

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
	PX_UDP_IP_TYPE_IPV4,
	PX_UDP_IP_TYPE_IPV6
}PX_UDP_IP_TYPE;

typedef struct
{
	unsigned int listenPort;
	unsigned int socket;
	PX_UDP_IP_TYPE type;
}PX_UDP;

typedef struct  
{
	unsigned int port;
	union
	{
		unsigned int ipv4;
		unsigned int ipv6[4];
	};
}PX_UDP_ADDR;

PX_UDP_ADDR PX_UDP_ADDR_IPV4(unsigned int ipv4,unsigned short port);
int PX_UDPListen(PX_UDP *udp,unsigned short listenPort);
int PX_UDPInitialize(PX_UDP *udp,PX_UDP_IP_TYPE type);
int PX_UDPSend(PX_UDP *udp,PX_UDP_ADDR addr,void *buffer,int size);
int PX_UDPReceived(PX_UDP *udp,PX_UDP_ADDR *from_addr,void *buffer,int buffersize,int *readsize);
void PX_UDPFree(PX_UDP *udp);
#endif

