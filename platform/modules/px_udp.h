
#ifndef PX_UDP_H
#define PX_UDP_H

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

int PX_UDPListen(PX_UDP *udp,unsigned short listenPort);
int PX_UDPInitialize(PX_UDP *udp,PX_UDP_IP_TYPE type);
int PX_UDPSend(PX_UDP *udp,PX_UDP_ADDR addr,void *buffer,int size);
int PX_UDPReceived(PX_UDP *udp,PX_UDP_ADDR *from_addr,void *buffer,int buffersize,int *readsize);
void PX_UDPFree(PX_UDP *udp);
#endif
