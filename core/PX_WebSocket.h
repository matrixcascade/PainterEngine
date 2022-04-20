#ifndef PX_WEBSOCKET_H
#define PX_WEBSOCKET_H
#include "PX_Memory.h"
#include "PX_Http.h"
#include "PX_Linker.h"
#include "PX_Base64.h"
#include "PX_Sha1.h"

#define PX_WEBSOCKET_CACHE_MAX_SIZE (1024*1024)

typedef enum 
{
	PX_WEBSOCKET_STATE_CLOSE=0,
	PX_WEBSOCKET_STATE_LISTEN,
	PX_WEBSOCKET_STATE_OPEN,
	PX_WEBSOCKET_STATE_CONNECTING,
}PX_WEBSOCKET_STATE;
struct _PX_WebSocket;
typedef struct  _PX_WebSocket
{
	PX_Linker *plinker;
	px_memorypool* mp;
	px_memory cache;
	px_bool isServer;
	PX_WEBSOCKET_STATE state;
}PX_WebSocket;

typedef struct
{
  px_word opcode : 4;
  px_word rsv : 3;
  px_word fin : 1;
  px_word payloadLength : 7;
  px_word mask : 1;
}PX_WebSocketMessageHeader;


px_void PX_WebSocketInitialize(px_memorypool* mp, PX_WebSocket *pInstance, PX_Linker *pLinker);
px_int PX_WebSocketRead(PX_WebSocket* pInstance, px_byte* data, px_int size);
px_bool PX_WebSocketConnet(PX_WebSocket* pInstance);
px_bool PX_WebSocketListen(PX_WebSocket* pInstance);
px_bool PX_WebSocketWrite(PX_WebSocket* pInstance, const px_byte* data, px_int size);
px_void PX_WebSocketFree(PX_WebSocket* pInstance);


#endif