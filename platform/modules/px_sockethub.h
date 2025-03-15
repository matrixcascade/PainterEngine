#ifndef PX_SocketHub_H
#include "platform/modules/px_tcp.h"
#include "platform/modules/px_thread.h"
#include "platform/modules/px_sysmemory.h"
#include "platform/modules/px_time.h"
#include "kernel/PX_Kernel.h"



#define  PX_SocketHub_SESSION_MAX 16
#define  PX_SocketHub_DEFAULT_TCP_PORT		31410
#define  PX_SocketHub_DEFAULT_WEBSOCKET_PORT 31411
typedef enum
{
	PX_SocketHub_TYPE_TCP=0,
	PX_SocketHub_TYPE_WEBSOCKET=1,
}PX_SocketHub_TYPE;

typedef struct
{
	px_byte Session[PX_SocketHub_SESSION_MAX];
}PX_SocketHub_Session;


#define PX_SOCKETHUB_MAX_CONNECTION 32

struct _PX_SocketHub;

typedef struct
{
	px_int   socket;
	PX_Linker linker;
	PX_WebSocket websocket_Instance;
	px_thread recv_thread, send_thread;
	px_mutex  rs_lock;
	px_dword  cache_size;

	px_byte*  recv_cache;
	px_dword  recv_cache_offset;

	px_byte*  send_cache;
	px_dword  send_cache_wcursor;
	px_dword  send_cache_rcursor;

	struct _PX_SocketHub * pSocketHub;
	px_void* userptr;
}PX_Websocket_Instance;

typedef struct
{
	px_int   socket;
	px_thread recv_thread,send_thread;

	px_dword  cache_size;

	px_byte* recv_cache;
	px_dword  recv_cache_offset;
	volatile  px_bool recv_cache_lock;

	px_byte* send_cache;
	volatile px_int  send_cache_wcursor;
	volatile px_int  send_cache_rcursor;
	volatile  px_bool send_end;
	struct _PX_SocketHub* pSocketHub;
	px_void* userptr;
}PX_TCP_Instance;



typedef  px_void(*PX_SocketHub_OnConnectCallback)(struct _PX_SocketHub* pSocketHub,  const px_byte session[16], px_void* ptr);
#define PX_SOCKETHUB_CONNECT_CALLBACK_FUNCTION(name) px_void name(struct _PX_SocketHub* pSocketHub, const px_byte session[16], px_void* ptr)

typedef px_void(*PX_SocketHub_ReceiveCallback)(struct _PX_SocketHub* pSocketHub, const px_byte session[16], const px_byte data[],px_int datasize,px_void *ptr);
#define PX_SOCKETHUB_RECEIVE_CALLBACK_FUNCTION(name) px_void name(PX_SocketHub* pSocketHub,const px_byte session[16],const  px_byte* data, px_int datasize,px_void *ptr)

typedef px_void(*PX_SocketHub_OnSendCallback)(struct _PX_SocketHub* pSocketHub, const px_byte session[16],px_dword elapsed, px_void* ptr);
#define PX_SOCKETHUB_SEND_CALLBACK_FUNCTION(name) px_void name(struct _PX_SocketHub* pSocketHub,const px_byte session[16],px_dword elapsed,px_void *ptr)

typedef px_void(*PX_SocketHub_OnDisconnectCallback)(struct _PX_SocketHub* pSocketHub, const px_byte session[16], px_void* ptr);
#define PX_SOCKETHUB_DISCONNECT_CALLBACK_FUNCTION(name) px_void name(struct _PX_SocketHub* pSocketHub,const px_byte session[16],px_void *ptr)
typedef struct _PX_SocketHub
{
	px_dword cache_size;
	//websocket channel
	PX_TCP websocket_tcp;
	px_thread websocket_accept_thread;
	px_thread websocket_thread;
	volatile px_bool websocket_enable;
	volatile px_bool websocket_connect_lock;
	PX_Websocket_Instance websocket_instance[PX_SOCKETHUB_MAX_CONNECTION];

	//tcp channel
	PX_TCP tcp;
	px_thread tcp_thread;
	volatile px_bool tcp_enable;
	volatile px_bool tcp_connect_lock;
	PX_TCP_Instance tcp_instance[PX_SOCKETHUB_MAX_CONNECTION];

	//callback
	PX_SocketHub_OnConnectCallback connect_callback;
	PX_SocketHub_OnSendCallback send_callback;
	PX_SocketHub_ReceiveCallback recv_callback;
	PX_SocketHub_OnDisconnectCallback disconnect_callback;
	px_void* userptr;
}PX_SocketHub;

px_bool PX_SocketHubInitialize(PX_SocketHub* pSocketHub, \
	px_word tcp_port, px_word websocket_port,\
	px_dword  cache_size,\
	PX_SocketHub_OnConnectCallback connect_callback,\
	PX_SocketHub_OnSendCallback send_callback,\
	PX_SocketHub_ReceiveCallback recv_callback,\
	PX_SocketHub_OnDisconnectCallback disconnect_callback,\
	px_void *ptr);
px_bool PX_SocketHubSend(PX_SocketHub* pServerHub, const  px_byte Session[16],const px_byte* data, px_int send_data_size);
px_bool PX_SocketHubWriteSendCache(PX_SocketHub* pServerHub, const  px_byte Session[16], const px_byte* data, px_int send_data_size);
px_void PX_SocketHubFree(PX_SocketHub* pSocketHub);
px_void PX_SocketHubSetUserPtr(PX_SocketHub* pServerHub, const  px_byte Session[16], px_void* ptr);
px_void* PX_SocketHubGetUserPtr(PX_SocketHub* pServerHub, const  px_byte Session[16]);
px_void PX_SocketHubCloseSession(PX_SocketHub* pServerHub, const  px_byte Session[16]);
px_int  PX_SocketHubGetClientSendCacheSize(PX_SocketHub* pServerHub, const  px_byte Session[16]);
px_int  PX_SocketHubGetClientRecvCacheSize(PX_SocketHub* pServerHub, const  px_byte Session[16]);
px_int PX_SocketHubGetCurrentOnlineCount(PX_SocketHub* pServerHub);

#endif // !PX_SocketHub_H
