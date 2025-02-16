#ifndef PX_WEBSOCKET_H
#define PX_WEBSOCKET_H
#include "PX_Memory.h"
#include "PX_Http.h"
#include "PX_Linker.h"
#include "PX_Base64.h"
#include "PX_Sha1.h"

#define PX_WEBSOCKET_CACHE_MAX_SIZE (1024*64)

typedef enum 
{
	PX_WEBSOCKET_STATE_CLOSE=0,
	PX_WEBSOCKET_STATE_LISTEN,
	PX_WEBSOCKET_STATE_OPEN,
	PX_WEBSOCKET_STATE_CONNECTING,
	PX_WEBSOCKET_STATE_ERROR
}PX_WEBSOCKET_STATE;
struct _PX_WebSocket;
typedef  px_void(*PX_PX_WebSocket_OnConnectCallback)(struct _PX_WebSocket* pSocket,px_void* ptr);
#define PX_WEBSOCKET_CONNECT_CALLBACK_FUNCTION(name) px_void name(struct _PX_WebSocket* pSocket,px_void* ptr)

typedef px_void(*PX_PX_WebSocket_ReceiveCallback)(struct _PX_WebSocket* pSocket,const px_byte data[], px_int datasize, px_void* ptr);
#define PX_WEBSOCKET_RECEIVED_CALLBACK_FUNCTION(name) px_void name(PX_WebSocket* pSocket,const  px_byte* data, px_int datasize,px_void *ptr)


typedef px_void(*PX_PX_WebSocket_OnDisconnectCallback)(struct _PX_WebSocket* pSocket, px_void* ptr);
#define PX_WEBSOCKET_DISCONNECT_CALLBACK_FUNCTION(name) px_void name(struct _PX_WebSocket* pSocket,px_void* ptr)

typedef struct  _PX_WebSocket
{
	PX_Linker *plinker;
	px_byte  recv_cache[PX_WEBSOCKET_CACHE_MAX_SIZE];
	px_dword recv_cache_offset;
	px_byte  send_cache[PX_WEBSOCKET_CACHE_MAX_SIZE];
	PX_ATOMIC px_int send_cache_wcursor, send_cache_rcursor;
	px_bool isServer;
	PX_ATOMIC PX_WEBSOCKET_STATE state;
	PX_PX_WebSocket_OnConnectCallback connect_callback;
	PX_PX_WebSocket_ReceiveCallback recv_callback;
	PX_PX_WebSocket_OnDisconnectCallback disconnect_callback;
	px_void* userptr;
}PX_WebSocket;

typedef struct
{
  px_word opcode : 4;
  px_word rsv : 3;
  px_word fin : 1;
  px_word payloadLength : 7;
  px_word mask : 1;
}PX_WebSocketMessageHeader;


px_void PX_WebSocketInitialize(PX_WebSocket *pInstance, PX_Linker *pLinker,\
	PX_PX_WebSocket_OnConnectCallback connect_callback,\
	PX_PX_WebSocket_ReceiveCallback recv_callback,\
	PX_PX_WebSocket_OnDisconnectCallback disconnect_callback,\
	px_void* ptr);
px_bool PX_WebSocketConnect(PX_WebSocket* pInstance);
px_bool PX_WebSocketListen(PX_WebSocket* pInstance);
px_void PX_WebSocketUpdate_Send(PX_WebSocket* pInstance);
px_void PX_WebSocketUpdate_Recv(PX_WebSocket* pInstance);
px_bool PX_WebSocketWrite(PX_WebSocket* pInstance, const px_byte* data, px_int size);
px_bool PX_WebSocketIsAlive(PX_WebSocket* pInstance);
px_void PX_WebSocketFree(PX_WebSocket* pInstance);


#endif