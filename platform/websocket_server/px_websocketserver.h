#include "kernel/PX_Kernel.h"
#include "platform/modules/px_tcp.h"
#include "platform/modules/px_thread.h"
#include "platform/modules/px_time.h"

#define PX_WEBSOCKETSERVER_MAX_CONNECTION 64
#define PX_WEBSOCKETSERVER_MAX_RECV_MEMPOOL (1024*1024*2)
#define PX_WEBSOCKETSERVER_MAX_SEND_MEMPOOL (1024*1024)

#define PX_WEBSOCKETSERVER_MAX_INSTANCE_MEMPOOL (PX_WEBSOCKETSERVER_MAX_RECV_MEMPOOL/PX_WEBSOCKETSERVER_MAX_CONNECTION)

typedef enum 
{
	PX_WEBSOCKETSERVER_STATUS_CLOSED=0,
	PX_WEBSOCKETSERVER_STATUS_RUNNING,
	PX_WEBSOCKETSERVER_STATUS_ERROR,
}PX_WEBSOCKETSERVER_STATUS;

typedef struct  
{
	px_dword handle;
	px_dword size;
}PX_WebsocketServer_Data_header;

typedef struct 
{
	volatile px_bool activating;
	px_dword handle;
	px_int socket;
	PX_Linker linker;
	PX_WebSocket websocket_Instance;
	px_dword elapsed;
	px_memorypool mp;
	px_thread thread;
	px_void* pserver;
	px_byte cache[PX_WEBSOCKETSERVER_MAX_INSTANCE_MEMPOOL];
}PX_WebsocketServer_Instance;

typedef struct 
{
	px_dword alloc_handle;
	PX_WebsocketServer_Instance Instances[PX_WEBSOCKETSERVER_MAX_CONNECTION];
	PX_TCP tcpserver;
	px_fifobuffer recv_fifo;
	px_memorypool mp_recv;
	px_byte recv_buffer[PX_WEBSOCKETSERVER_MAX_RECV_MEMPOOL];
	PX_WEBSOCKETSERVER_STATUS status;
	px_thread server_thread;
	volatile px_bool connect_lock,recv_lock;
}PX_WebsocketServer;


px_bool PX_WebsocketServerInitialize(PX_WebsocketServer* pServer, px_word port);
px_int PX_WebsocketServerRead(PX_WebsocketServer* pServer, px_dword* handle, px_byte* data, px_int recv_data_size);
px_bool PX_WebsocketServerWrite(PX_WebsocketServer* pServer,px_dword handle, const px_byte* data, px_int send_data_size);
px_void PX_WebsocketServerFree(PX_WebsocketServer* pServer);


