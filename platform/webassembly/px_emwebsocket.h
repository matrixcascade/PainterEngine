#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <kernel/PX_Kernel.h>
#include "stdio.h"

typedef enum
{
    PX_WEBSOCKETEM_STATUS_WAITING,
    PX_WEBSOCKETEM_STATUS_CONNECTING,
    PX_WEBSOCKETEM_STATUS_CLOSED,
}PX_WEBSOCKETEM_STATUS;


typedef struct 
{
    PX_WEBSOCKETEM_STATUS  status;
    px_memorypool mp;
    px_fifobuffer recv_fifo;
    EMSCRIPTEN_WEBSOCKET_T socket;
    px_byte cache[1024*1024];//1M for recv
    volatile px_bool recv_lock;
}PX_WebsocketEm;


px_bool PX_WebsocketEmInitialize(PX_WebsocketEm *em);
px_bool PX_WebsocketEmConnect(PX_WebsocketEm *em,const px_char host[]);
px_int PX_WebsocketEmRead(PX_WebsocketEm *em,px_byte *data,px_int data_size);
px_bool PX_WebsocketEmWrite(PX_WebsocketEm *em,px_byte *data,px_int data_size);
px_void PX_WebsocketEmFree(PX_WebsocketEm *em);