#ifndef PX_SOCKET_H
#define PX_SOCKET_H
#include "kernel/PX_Kernel.h"

struct _PX_Socket;
typedef px_void(*PX_Socket_ConnectCallback)(struct _PX_Socket* pSocket, px_void* ptr);
#define PX_SOCKET_CONNECT_CALLBACK_FUNCTION(name) px_void name(PX_Socket* pSocket, px_void* ptr)
typedef px_void(*PX_Socket_ReceiveCallback)(struct _PX_Socket* pSocket, px_byte* data, px_int datasize,px_void *ptr);
#define PX_SOCKET_RECEIVE_CALLBACK_FUNCTION(name) px_void name(PX_Socket* pSocket, px_byte* data, px_int datasize,px_void *ptr)
typedef  px_void(*PX_Socket_SendCallback)(struct _PX_Socket* pSocket, px_void* ptr);
#define PX_SOCKET_SEND_CALLBACK_FUNCTION(name) px_void name(PX_Socket* pSocket, px_void* ptr)
typedef  px_void(*PX_Socket_DisconnectCallback)(struct _PX_Socket* pSocket, px_void* ptr);
#define PX_SOCKET_DISCONNECT_CALLBACK_FUNCTION(name) px_void name(PX_Socket* pSocket, px_void* ptr)


typedef struct _PX_Socket
{
	px_void* handler;
	px_dword cache_size;
	px_char host[128];
	px_word port;
	px_byte* precv_buffer;
	px_int recv_buffer_size;
	px_int recv_buffer_offset;
	px_byte* psend_buffer;
	px_int send_buffer_size;
	volatile px_int send_buffer_wcursor, send_buffer_rcursor;
	PX_Socket_ConnectCallback connectcallback;
	PX_Socket_ReceiveCallback recvcallback;
	PX_Socket_SendCallback sendcallback;
	PX_Socket_DisconnectCallback disconnectcallback;
	px_void* userptr;
	px_void* userInstance;
	px_bool isConnecting;
	volatile px_bool enable;
	volatile px_bool exit;
}PX_Socket;


px_bool PX_SocketInitialize(PX_Socket* pSocket, px_dword cache_size, \
	const px_char host[], px_word port, \
	PX_Socket_ConnectCallback connect_callback, \
	PX_Socket_ReceiveCallback recv_callback, \
	PX_Socket_SendCallback send_callback, \
	PX_Socket_DisconnectCallback disconnect_callback, \
	px_void* userptr
);
px_bool PX_SocketSend(PX_Socket* pSocket, px_byte* data, px_dword send_data_size);
px_void PX_SocketConnect(PX_Socket* pSocket,px_bool connecting);
px_void PX_SocketSetUserPtr(PX_Socket* pSocket, px_void* ptr);
px_void *PX_SocketGetUserPtr(PX_Socket* pSocket);
px_bool PX_SocketIsConnecting(PX_Socket* pSocket);
px_void PX_SocketClose(PX_Socket* pSocket);
px_void PX_SocketFree(PX_Socket* pSocket);
#endif // !PX_SOCKET_H
