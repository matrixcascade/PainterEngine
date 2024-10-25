#include "platform/modules/px_socket.h"
#include "platform/modules/px_tcp.h"
#include "platform/modules/px_udp.h"
#include "platform/modules/px_sysmemory.h"
#include "px_emwebsocket.h"

px_bool PX_SocketInitialize(PX_Socket* pSocket, PX_Socket_ReceivedCallback readcallback, px_void* userptr,px_dword cache_size)
{
	PX_memset(pSocket, 0, sizeof(PX_Socket));
	pSocket->handler = (PX_WebsocketEm*)malloc(sizeof(PX_WebsocketEm));
	if (pSocket->handler == PX_NULL)
	{
		return PX_FALSE;
	}
	if (!PX_WebsocketEmInitialize((PX_TCP*)pSocket->handler))
	{
		free(pSocket->handler);
		return PX_FALSE;
	}
	pSocket->state= PX_SOCKET_STATE_OPENING;
	pSocket->readcallback = readcallback;
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
	px_char em_host[128]={0};
	if (pSocket->state!= PX_SOCKET_STATE_OPENING)
	{
		pSocket->state = PX_SOCKET_STATE_ERROR;
		return PX_FALSE;
	}
	PX_sprintf2(em_host,"ws://%1:%2",PX_STRINGFORMAT_STRING(host),PX_STRINGFORMAT_INT(port));
	if (!PX_WebsocketEmConnect((PX_TCP*)pSocket->handler, addr))
	{
		pSocket->state = PX_SOCKET_STATE_ERROR;
		return PX_FALSE;
	}
	pSocket->state = PX_SOCKET_STATE_CONNECTING;
	return PX_TRUE;
}


px_bool PX_SocketWrite(PX_Socket* pSocket,const px_byte* data, px_dword send_data_size)
{
	//write size
	if (pSocket->state != PX_SOCKET_STATE_CONNECTING)
	{
		PX_ASSERT();
		return PX_FALSE;
	}
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
	PX_TCPFree((PX_TCP*)pSocket->handler);
	free(pSocket->handler);
}

px_void PX_SocketUpdate(PX_Socket* pSocket)
{
	//send cache
	if (pSocket->state == PX_SOCKET_STATE_CONNECTING)
	{
		while (pSocket->send_buffer_offset)
		{
			px_int sendsize;
			px_int sendblocksize = 512;
			if(pSocket->send_buffer_offset <sendblocksize)
			{
				sendblocksize=pSocket->send_buffer_offset;
			}
			sendsize = PX_TCPSend((PX_TCP*)pSocket->handler, pSocket->psend_buffer, sendblocksize);
			if (sendsize > 0)
			{
				if (sendsize == pSocket->send_buffer_offset)
				{
					pSocket->send_buffer_offset = 0;
				}
				else
				{
					PX_memcpy(pSocket->psend_buffer, pSocket->psend_buffer + sendsize, pSocket->send_buffer_offset - sendsize);
					pSocket->send_buffer_offset -= sendsize;
				}
			}
			else if(sendsize==0)
			{
				break;
			}
			else
			{
				pSocket->state = PX_SOCKET_STATE_ERROR;
				break;
			}
		}

		while (pSocket->recv_buffer_offset>=sizeof(px_dword))
		{
			px_dword packsize=*(px_dword*)pSocket->precv_buffer;
			if (pSocket->recv_buffer_offset-sizeof(px_dword)>packsize)
			{
				if(pSocket->readcallback)
					pSocket->readcallback(pSocket,pSocket->precv_buffer+sizeof(px_dword),packsize,pSocket->userptr);
				PX_memcpy(pSocket->precv_buffer,pSocket->precv_buffer+sizeof(px_dword)+packsize,pSocket->recv_buffer_offset-sizeof(px_dword)-packsize);
				pSocket->recv_buffer_offset-=sizeof(px_dword)+packsize;
			}
			else
			{
				break;
			}
		}
	}
}
