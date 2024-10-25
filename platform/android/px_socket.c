#include "platform/modules/px_socket.h"
#include "platform/modules/px_tcp.h"
#include "platform/modules/px_udp.h"
#include "platform/modules/px_sysmemory.h"
#include "platform/modules/px_thread.h"

typedef struct
{
	PX_TCP tcp;
	px_thread thread;
}PX_TCP_Handle;

px_void PX_SockeThread(PX_Socket* pSocket)
{
	PX_TCP_Handle *pHandle=(PX_TCP_Handle *)pSocket->handler;
	PX_TCP* pTCP = (PX_TCP*)&pHandle->tcp;
	//connect 
	pSocket->connectcallback(pSocket, pSocket->userptr);
	
	while (PX_TRUE)
	{
		//send cache
		while (pSocket->send_buffer_offset)
		{
			static px_int index;
			static px_int sum;
			px_int sendsize;
			px_int sendblocksize = 512;
			if (pSocket->send_buffer_offset < sendblocksize)
			{
				sendblocksize = pSocket->send_buffer_offset;
			}
			sendsize = PX_TCPSend(pTCP, pSocket->psend_buffer, sendblocksize);
			if (sendsize > 0)
			{
				if (sendsize == pSocket->send_buffer_offset)
				{
					pSocket->send_buffer_offset = 0;
				}
				else
				{
					memcpy(pSocket->psend_buffer, pSocket->psend_buffer + sendsize, pSocket->send_buffer_offset - sendsize);
					pSocket->send_buffer_offset -= sendsize;
				}
			}
			else if (sendsize < 0)
			{
				goto DISCONNECT;
			}
			else
			{
				break;
			}
		}

		while (PX_TRUE)
		{
			px_int recvsize;
			recvsize = PX_TCPReceived(pTCP, pSocket->precv_buffer + pSocket->recv_buffer_offset, pSocket->recv_buffer_size - pSocket->recv_buffer_offset,1);
			if (recvsize > 0)
			{
				pSocket->recv_buffer_offset += recvsize;
			}
			else if(recvsize<0)
			{
				goto DISCONNECT;
			}
			else
			{
				break;
			}

			if (pSocket->recv_buffer_offset >= sizeof(px_dword))
			{
				px_dword packsize = *(px_dword*)pSocket->precv_buffer;
				if(packsize>pSocket->cache_size)
				{
					goto DISCONNECT;
				}
				if (pSocket->recv_buffer_offset - sizeof(px_dword) >= packsize)
				{
					if (pSocket->readcallback)
						pSocket->readcallback(pSocket, pSocket->precv_buffer + sizeof(px_dword), packsize, pSocket->userptr);
					memcpy(pSocket->precv_buffer, pSocket->precv_buffer + sizeof(px_dword) + packsize, pSocket->recv_buffer_offset - sizeof(px_dword) - packsize);
					pSocket->recv_buffer_offset -= sizeof(px_dword) + packsize;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
DISCONNECT:
	pSocket->disconnectcallback(pSocket, pSocket->userptr);
	PX_TCPFree((PX_TCP*)pSocket->handler);

}


px_bool PX_SocketInitialize(PX_Socket* pSocket, px_dword cache_size, \
	PX_Socket_ConnectCallback connect_callback, PX_Socket_ReceiveCallback recv_callback, PX_Socket_DisconnectCallback disconnect_callback, \
	px_void* userptr)
{
	PX_TCP_Handle* pHandle;
	PX_TCP* pTCP;
	PX_memset(pSocket, 0, sizeof(PX_Socket));
	pSocket->handler = (PX_TCP_Handle*)malloc(sizeof(PX_TCP_Handle));
	PX_memset(pSocket->handler, 0, sizeof(PX_TCP_Handle));
	if (pSocket->handler == PX_NULL)
	{
		return PX_FALSE;
	}
	pHandle = (PX_TCP_Handle*)pSocket->handler;
	pTCP = (PX_TCP*)&pHandle->tcp;
	if (!PX_TCPInitialize(pTCP, PX_TCP_IP_TYPE_IPV4))
	{
		free(pSocket->handler);
		return PX_FALSE;
	}
	pSocket->cache_size = cache_size;
	pSocket->readcallback = recv_callback;
	pSocket->connectcallback = connect_callback;
	pSocket->disconnectcallback = disconnect_callback;
	pSocket->userptr = userptr;

	pSocket->precv_buffer = malloc(cache_size);
	if (pSocket->precv_buffer == PX_NULL)
	{
		free(pSocket->handler);
		return PX_FALSE;
	}
	pSocket->recv_buffer_size = cache_size;
	pSocket->recv_buffer_offset = 0;

	pSocket->psend_buffer = malloc(cache_size);
	if (pSocket->psend_buffer == PX_NULL)
	{
		free(pSocket->handler);
		free(pSocket->precv_buffer);
		return PX_FALSE;
	}
	pSocket->send_buffer_size = cache_size;
	pSocket->send_buffer_offset = 0;

	return PX_TRUE;
}

px_bool PX_SocketConnect(PX_Socket* pSocket,const px_byte host[],px_word port)
{
	PX_TCP_ADDR addr;
	PX_TCP_Handle* pHandle = (PX_TCP_Handle*)pSocket->handler;
	PX_TCP* pTCP = (PX_TCP*)&pHandle->tcp;

	addr.port=PX_htons(port);
	if (PX_IsValidIPAddress(host))
	{
		addr.ipv4=PX_inet_addr(host);
	}
	else
	{
		addr.ipv4=PX_UDPGetHostByName(host,0x08080808);
		if (addr.ipv4==0)
		{
			return PX_FALSE;
		}
	}
	if (!PX_TCPConnect((PX_TCP*)pSocket->handler, addr))
	{
		return PX_FALSE;
	}
	pSocket->recv_buffer_offset = 0;
	pSocket->send_buffer_offset = 0;
	PX_ThreadCreate(&pHandle->thread, PX_SockeThread, pSocket);
	return PX_TRUE;
}


px_bool PX_SocketSend(PX_Socket* pSocket,const px_byte* data, px_dword send_data_size)
{
	//push to send buffer
	if (pSocket->send_buffer_offset + send_data_size + sizeof(px_dword) > pSocket->send_buffer_size)
	{
		return PX_FALSE;
	}
	*(px_dword*)(pSocket->psend_buffer + pSocket->send_buffer_offset) = send_data_size;
	pSocket->send_buffer_offset += sizeof(px_dword);
	PX_memcpy(pSocket->psend_buffer + pSocket->send_buffer_offset, data, send_data_size);
	pSocket->send_buffer_offset += send_data_size;
	return PX_TRUE;
}
px_void PX_SocketFree(PX_Socket* pSocket)
{
	PX_TCP_Handle* pHandle = (PX_TCP_Handle*)pSocket->handler;
	PX_TCP* pTCP = (PX_TCP*)&pHandle->tcp;
	PX_TCPFree(pTCP);
	free(pSocket->handler);
	free(pSocket->precv_buffer);
	free(pSocket->psend_buffer);

}

