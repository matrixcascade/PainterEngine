#include "../../platform/modules/px_socket.h"
#include "../../platform/modules/px_tcp.h"
#include "../../platform/modules/px_udp.h"
#include "../../platform/modules/px_time.h"
#include "../../platform/modules/px_sysmemory.h"
#include "../../platform/modules/px_thread.h"

typedef struct
{
	PX_TCP tcp;
	px_thread send_thread;
	px_thread recv_thread;
	volatile px_bool send_end;
}PX_TCP_Handle;


px_int PX_SocketCircularBufferGetSpaceSize(px_int buffersize, volatile px_int* wCursor, volatile px_dword rCursor)
{
	px_int spacesize, _WCursor = *wCursor;
	px_int _RCursor = rCursor;
	spacesize = _WCursor >= _RCursor ? buffersize - (_WCursor - _RCursor) - 8 : _RCursor - _WCursor - 8;
	return spacesize;
}

static px_bool PX_SocketWriteToCircularBuffer(px_byte* buffer, px_int buffersize, volatile px_int* wCursor, volatile px_dword rCursor, px_void* data, px_int datasize)
{
	px_int spacesize, divsize;
	px_int _WCursor = *wCursor;
	px_int _RCursor = rCursor;

	spacesize = _WCursor >= _RCursor ? buffersize - (_WCursor - _RCursor) - 8 : _RCursor - _WCursor - 8;
	if (datasize > spacesize) return PX_FALSE;
	divsize = buffersize - _WCursor;
	if (divsize > spacesize) divsize = spacesize;

	if (divsize >= datasize)
	{
		PX_memcpy(buffer + _WCursor, data, datasize);
		*wCursor = (_WCursor + datasize) % buffersize;
	}
	else
	{
		PX_memcpy(buffer + _WCursor, data, divsize);
		PX_memcpy(buffer, (px_byte*)data + divsize, datasize - divsize);
		*wCursor = datasize - divsize;
	}
	return PX_TRUE;
}

static px_bool PX_SocketConnectToServer(PX_Socket* pSocket, const px_char host[], px_word port)
{
	PX_TCP_ADDR addr;
	PX_TCP_Handle* pHandle = (PX_TCP_Handle*)pSocket->handler;
	PX_TCP* pTCP = (PX_TCP*)&pHandle->tcp;

	if (!PX_TCPInitialize(pTCP, PX_TCP_IP_TYPE_IPV4))
	{
		return PX_FALSE;
	}
	addr.port = PX_htons(port);
	if (PX_IsValidIPAddress(host))
	{
		addr.ipv4 = PX_inet_addr(host);
	}
	else
	{
		addr.ipv4 = PX_UDPGetHostByName(host, 0x08080808);
		if (addr.ipv4 == 0)
		{
			return PX_FALSE;
		}
	}
	printf("connecting to %s:%d:%d\n", host, port,PX_TimeGetTime());
	if (!PX_TCPConnect((PX_TCP*)pSocket->handler, addr))
	{
		return PX_FALSE;
	}
	printf("suceess to connect %s:%d:%d\n", host, port, PX_TimeGetTime());
	pSocket->recv_buffer_offset = 0;
	pSocket->send_buffer_wcursor = 0;
	pSocket->send_buffer_rcursor = 0;
	return PX_TRUE;
}


px_void PX_SocketThread_Send(px_void *ptr)
{
	PX_Socket* pSocket=(PX_Socket *)ptr;
	PX_TCP_Handle *pHandle=(PX_TCP_Handle *)pSocket->handler;
	PX_TCP* pTCP = (PX_TCP*)&pHandle->tcp;

	while (!pSocket->exit&&!pHandle->send_end)
	{
		px_int sendBlockSize;
		if (!pSocket->enable)
		{
			PX_Sleep(0);
			continue;
		}
		//send callback
		if (pSocket->sendcallback)
			pSocket->sendcallback(pSocket, pSocket->userptr);

		//send cache
		do
		{
			px_int sendsize;
			sendBlockSize=pSocket->send_buffer_wcursor>=pSocket->send_buffer_rcursor?pSocket->send_buffer_wcursor-pSocket->send_buffer_rcursor:pSocket->send_buffer_size-pSocket->send_buffer_rcursor;
			sendsize = PX_TCPSend(pTCP, pSocket->psend_buffer+ pSocket->send_buffer_rcursor, sendBlockSize);
			if (sendsize > 0)
			{
				pSocket->send_buffer_rcursor += sendsize;
				pSocket->send_buffer_rcursor %= pSocket->send_buffer_size;
			}
			else if (sendsize < 0)
			{
				goto DISCONNECT;
			}
			else
			{
				break;
			}
		} while (sendBlockSize);

	}
DISCONNECT:
	return;
}
px_void PX_SocketThread_Recv(px_void *ptr)
{
	PX_Socket* pSocket = (PX_Socket*)ptr;
	PX_TCP_Handle* pHandle = (PX_TCP_Handle*)pSocket->handler;
	PX_TCP* pTCP = (PX_TCP*)&pHandle->tcp;

	while (!pSocket->exit)
	{
		if (!pSocket->enable)
		{
			PX_Sleep(0);
			continue;
		}

		//connect 
		if (!PX_SocketConnectToServer(pSocket, pSocket->host, pSocket->port))
		{
			goto DISCONNECT;
		}

		if (pSocket->connectcallback)
			pSocket->connectcallback(pSocket, pSocket->userptr);

		//create send thread
		pHandle->send_end = PX_FALSE;
		PX_ThreadCreate(&pHandle->send_thread, PX_SocketThread_Send, pSocket);

		pSocket->isConnecting = PX_TRUE;
		while (PX_TRUE)
		{
			if (!pSocket->enable)
			{
				goto DISCONNECT;
			}
			while (PX_TRUE)
			{
				px_int recvsize;
				recvsize = PX_TCPReceived(pTCP, pSocket->precv_buffer + pSocket->recv_buffer_offset, pSocket->recv_buffer_size - pSocket->recv_buffer_offset, -1);

				if (recvsize > 0)
				{

					pSocket->recv_buffer_offset += recvsize;
				}
				else if (recvsize < 0)
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

					if (packsize > pSocket->cache_size)
					{
						goto DISCONNECT;
					}
					if (pSocket->recv_buffer_offset - sizeof(px_dword) >= packsize)
					{
						if (pSocket->recvcallback)
							pSocket->recvcallback(pSocket, pSocket->precv_buffer + sizeof(px_dword), packsize, pSocket->userptr);
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
		pSocket->isConnecting = PX_FALSE;
		if (pSocket->disconnectcallback)
			pSocket->disconnectcallback(pSocket, pSocket->userptr);
		PX_TCPSocketFree(((PX_TCP*)pSocket->handler)->socket);
		pHandle->send_end = PX_TRUE;
		while (pHandle->send_thread.isRun);
		printf("socket closed\nreconnecting....");
	}
}


px_bool PX_SocketInitialize(PX_Socket* pSocket, px_dword cache_size, \
	const px_char host[], px_word port, \
	PX_Socket_ConnectCallback connect_callback, \
	PX_Socket_ReceiveCallback recv_callback, \
	PX_Socket_SendCallback send_callback, \
	PX_Socket_DisconnectCallback disconnect_callback, \
	px_void* userptr
)
{
	PX_TCP_Handle* pHandle;
	PX_memset(pSocket, 0, sizeof(PX_Socket));
	pSocket->handler = (PX_TCP_Handle*)malloc(sizeof(PX_TCP_Handle));
	PX_memset(pSocket->handler, 0, sizeof(PX_TCP_Handle));
	if (pSocket->handler == PX_NULL)
	{
		return PX_FALSE;
	}
	pHandle = (PX_TCP_Handle*)pSocket->handler;
	PX_memset(pHandle, 0, sizeof(PX_TCP_Handle));

	
	pSocket->cache_size = cache_size;
	pSocket->recvcallback = recv_callback;
	pSocket->connectcallback = connect_callback;
	pSocket->sendcallback = send_callback;
	pSocket->disconnectcallback = disconnect_callback;
	pSocket->userptr = userptr;
	PX_strcpy(pSocket->host, host, sizeof(pSocket->host) );
	pSocket->port = port;
	pSocket->isConnecting = PX_FALSE;

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
	pSocket->send_buffer_wcursor= 0;
	pSocket->send_buffer_rcursor = 0;
	pSocket->enable = PX_TRUE;
	PX_ThreadCreate(&pHandle->recv_thread, PX_SocketThread_Recv, pSocket);
	return PX_TRUE;
}



px_bool PX_SocketSend(PX_Socket* pSocket,px_byte* data, px_dword send_data_size)
{
	if(PX_SocketCircularBufferGetSpaceSize(pSocket->send_buffer_size, &pSocket->send_buffer_wcursor, pSocket->send_buffer_rcursor)<send_data_size+sizeof(px_dword))
	{
		return PX_FALSE;
	}
	if(PX_SocketWriteToCircularBuffer(pSocket->psend_buffer, pSocket->send_buffer_size, &pSocket->send_buffer_wcursor, pSocket->send_buffer_rcursor, &send_data_size, sizeof(px_dword)))
		return PX_SocketWriteToCircularBuffer(pSocket->psend_buffer, pSocket->send_buffer_size, &pSocket->send_buffer_wcursor, pSocket->send_buffer_rcursor, data, send_data_size);
	return PX_FALSE;
}

px_void PX_SocketConnect(PX_Socket* pSocket, px_bool connecting)
{
	pSocket->enable = connecting;
}
px_void PX_SocketFree(PX_Socket* pSocket)
{
	PX_TCP_Handle* pHandle = (PX_TCP_Handle*)pSocket->handler;
	PX_TCP* pTCP = (PX_TCP*)&pHandle->tcp;
	PX_TCPFree(pTCP);
	free(pSocket->handler);
	free(pSocket->precv_buffer);
	free(pSocket->psend_buffer);
	pSocket->exit=PX_TRUE;
}

px_void PX_SocketSetUserPtr(PX_Socket* pSocket, px_void* ptr)
{
	pSocket->userInstance = ptr;
}
px_void* PX_SocketGetUserPtr(PX_Socket* pSocket)
{
	return pSocket->userInstance;
}
px_void PX_SocketClose(PX_Socket* pSocket)
{
	pSocket->enable = PX_FALSE;
}
px_bool PX_SocketIsConnecting(PX_Socket* pSocket)
{
	return pSocket->isConnecting;
}
