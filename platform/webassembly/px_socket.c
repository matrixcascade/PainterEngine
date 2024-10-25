#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include "platform/modules/px_socket.h"
#include "platform/modules/px_time.h"
#include "platform/modules/px_sysmemory.h"
#include "stdio.h"

typedef struct
{
	EMSCRIPTEN_WEBSOCKET_T socket;
	volatile px_bool recv_lock;

}PX_EMWebSocket_Handle;
static EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData);
static EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent, void* userData);
static EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent, void* userData);
static EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent, void* userData);
static px_bool PX_SocketConnectToServer(PX_Socket* pSocket, const px_char host[], px_word port)
{
	px_char _build_host[256];
	PX_EMWebSocket_Handle* pem = (PX_EMWebSocket_Handle*)pSocket->handler;
	EmscriptenWebSocketCreateAttributes ws_attrs;
	PX_sprintf2(_build_host, 256, "ws://%1:%2", PX_STRINGFORMAT_STRING(host), PX_STRINGFORMAT_INT(port));
	ws_attrs.url = _build_host;
	ws_attrs.protocols = "binary";
	ws_attrs.createOnMainThread = EM_TRUE;
	pem->socket = emscripten_websocket_new(&ws_attrs);
	emscripten_websocket_set_onopen_callback(pem->socket, pSocket, onopen);
	emscripten_websocket_set_onerror_callback(pem->socket, pSocket, onerror);
	emscripten_websocket_set_onclose_callback(pem->socket, pSocket, onclose);
	emscripten_websocket_set_onmessage_callback(pem->socket, pSocket, onmessage);
	printf("connecting to %s:%d:%d\n", host, port,PX_TimeGetTime());
	return PX_TRUE;
}


static EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent* websocketEvent, void* userData)
{
	PX_Socket* psocket = (PX_Socket*)userData;
	PX_EMWebSocket_Handle *pem=(PX_EMWebSocket_Handle *)psocket->handler;
	pem->socket = websocketEvent->socket;
	printf("suceess to connect %s:%d:%d\n", host, port, PX_TimeGetTime());
	psocket->connectcallback(psocket, psocket->userptr);
	psocket->isConnecting=PX_TRUE;
    return EM_TRUE;
}

static EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent* websocketEvent, void* userData)
{
	PX_Socket* psocket = (PX_Socket*)userData;
	PX_EMWebSocket_Handle *pem=(PX_EMWebSocket_Handle *)psocket->handler;
	printf("websocket error\n");
	psocket->isConnecting=PX_FALSE;
    return EM_TRUE;
}

static EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent* websocketEvent, void* userData)
{
	PX_Socket* psocket = (PX_Socket*)userData;
	PX_EMWebSocket_Handle* pem = (PX_EMWebSocket_Handle*)psocket->handler;
	psocket->disconnectcallback(psocket, psocket->userptr);
	emscripten_websocket_close(pem->socket, 1000, "user close");
	psocket->isConnecting=PX_FALSE;
	if(!psocket->exit)
	{
		while (!psocket->enable) PX_Sleep(1);
		PX_SocketConnectToServer(psocket, psocket->host, psocket->port);
	}
	
	return EM_TRUE;
}

static EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent* websocketEvent, void* userData)
{
	PX_Socket* psocket = (PX_Socket*)userData;
	PX_EMWebSocket_Handle* pem = (PX_EMWebSocket_Handle*)psocket->handler;
	px_int recv_size = websocketEvent->numBytes;
	while (pem->recv_lock);
	pem->recv_lock = PX_TRUE;
	if (psocket->recv_buffer_offset + recv_size > psocket->recv_buffer_size)
	{
		//fifo crash
		printf("fifo crash,close connection\n");
		emscripten_websocket_close(pem->socket, 1000, "user close");
		return EM_FALSE;
	}
	else
	{
		memcpy(psocket->precv_buffer + psocket->recv_buffer_offset, websocketEvent->data, recv_size);
		psocket->recv_buffer_offset += recv_size;
	}
	if (psocket->recv_buffer_offset>sizeof(px_dword))
	{
		px_dword packetsize = *(px_dword*)psocket->precv_buffer;
		if (packetsize>psocket->recv_buffer_size-sizeof(px_dword))
		{
			//packet size error
			printf("packet size out of bound!close connection\n");
			emscripten_websocket_close(pem->socket, 1000, "user close");
			return EM_FALSE;
		}
		else if (psocket->recv_buffer_offset >= packetsize + sizeof(px_dword))
		{
			psocket->recvcallback(psocket, psocket->precv_buffer + sizeof(px_dword), packetsize, psocket->userptr);
			psocket->recv_buffer_offset -= packetsize + sizeof(px_dword);
			if (psocket->recv_buffer_offset)
			{
				memmove(psocket->precv_buffer, psocket->precv_buffer + packetsize + sizeof(px_dword), psocket->recv_buffer_offset);
			}
		}
	}
	pem->recv_lock = PX_FALSE;
	return EM_TRUE;
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
	PX_EMWebSocket_Handle* pHandle;
	
	PX_memset(pSocket, 0, sizeof(PX_Socket));
	pSocket->handler = (PX_EMWebSocket_Handle*)malloc(sizeof(PX_EMWebSocket_Handle));
	if (pSocket->handler == PX_NULL)
	{
		return PX_FALSE;
	}
	PX_memset(pSocket->handler, 0, sizeof(PX_EMWebSocket_Handle));
	pHandle = (PX_EMWebSocket_Handle*)pSocket->handler;
	PX_strcpy(pSocket->host, host, sizeof(pSocket->host) );
	pSocket->port = port;

	pSocket->cache_size = cache_size;
	pSocket->sendcallback = send_callback;
	pSocket->recvcallback = recv_callback;
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
	pSocket->send_buffer_wcursor = 0;
	pSocket->send_buffer_rcursor = 0;
	pSocket->enable = PX_TRUE;
	PX_SocketConnectToServer(pSocket, host, port);

	return PX_TRUE;
}


px_bool PX_SocketSend(PX_Socket* pSocket, px_byte* data, px_dword send_data_size)
{
	px_int send_offst=0;
	px_int send_block_size;
	px_bool send_size=PX_FALSE;
	px_int retry = 3;
	PX_EMWebSocket_Handle* pem = (PX_EMWebSocket_Handle*)pSocket->handler;
	while (PX_TRUE)
	{
		EMSCRIPTEN_RESULT result;
		if (send_offst>=send_data_size)
		{
			break;
		}
		if (!send_size)
		{
			result = emscripten_websocket_send_binary(pem->socket, &send_data_size, sizeof(send_data_size));
			if (result) {
				return PX_FALSE;
			}
			send_size = PX_TRUE;
		}

		send_block_size = send_data_size - send_offst;
		if (send_block_size > 8192)
		{
			send_block_size = 8192;
		}
		result = emscripten_websocket_send_binary(pem->socket, data + send_offst, send_block_size);
		if (result)
		{
			if (retry)
			{
				retry--;
				PX_Sleep(5);
				continue;
			}
			else
			{
				return PX_FALSE;
			}
		}
		retry=3;
		send_offst += send_block_size;
	}
	return PX_TRUE;

}
px_void PX_SocketConnect(PX_Socket* pSocket, px_bool connecting)
{
	pSocket->enable = connecting;
	if (!connecting)
	{
		PX_EMWebSocket_Handle* pem = (PX_EMWebSocket_Handle*)pSocket->handler;
		emscripten_websocket_close(pem->socket, 1000, "user close");
	}
}
px_void PX_SocketFree(PX_Socket* pSocket)
{
	PX_EMWebSocket_Handle* pem = (PX_EMWebSocket_Handle*)pSocket->handler;
	printf("free socket:close connection\n");
	emscripten_websocket_close(pem->socket, 1000, "user close");
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
