#include "px_websocket.h"

static EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) 
{   
    PX_WebsocketEm *em=(PX_WebsocketEm *)userData;
    em->status=PX_WEBSOCKETEM_STATUS_CONNECTING;
    em->socket=websocketEvent->socket;
    return EM_TRUE;
}

static EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData) 
{
    PX_WebsocketEm *em=(PX_WebsocketEm *)userData;
	em->status=PX_WEBSOCKETEM_STATUS_CLOSED;
    return EM_TRUE;
}

static EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData) 
{
    PX_WebsocketEm *em=(PX_WebsocketEm *)userData;
    em->status=PX_WEBSOCKETEM_STATUS_CLOSED;
    return EM_TRUE;
}

static EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) 
{
    PX_WebsocketEm *em=(PX_WebsocketEm *)userData;
    px_int32 header;
	header=websocketEvent->numBytes;

	while (em->recv_lock);
	em->recv_lock = PX_TRUE;
	if (PX_FifoBufferPush(&em->recv_fifo, &header, sizeof(header)))
	{
		if (!PX_FifoBufferPush(&em->recv_fifo,websocketEvent->data,header))
		{
			//fifo full
			PX_MemoryClear(&em->recv_fifo);
		}
	}
	em->recv_lock = PX_FALSE;

    return EM_TRUE;
}


px_bool PX_WebsocketEmInitialize(PX_WebsocketEm *em)
{
    if (!emscripten_websocket_is_supported()) {
        return 0;
    }
	em->status=PX_WEBSOCKETEM_STATUS_CLOSED;
	printf("Websocket Initialized\n");
    return 1;
}

px_bool PX_WebsocketEmConnect(PX_WebsocketEm *em,const px_char host[])
{
    EmscriptenWebSocketCreateAttributes ws_attrs;
	printf("Websocket Connect to %s\n",host);
    PX_memset(em,0,sizeof(PX_WebsocketEm));
    ws_attrs.url=host;
    ws_attrs.protocols="binary";
    ws_attrs.createOnMainThread=EM_TRUE;
    em->status=PX_WEBSOCKETEM_STATUS_WAITING;
    em->socket = emscripten_websocket_new(&ws_attrs);
    emscripten_websocket_set_onopen_callback( em->socket , em, onopen);
    emscripten_websocket_set_onerror_callback( em->socket , em, onerror);
    emscripten_websocket_set_onclose_callback( em->socket , em, onclose);
    emscripten_websocket_set_onmessage_callback( em->socket , em, onmessage);
    em->mp=MP_Create(em->cache,sizeof(em->cache));
    PX_FifoBufferInitialize(&em->mp, &em->recv_fifo);
    return PX_TRUE;
}
px_int PX_WebsocketEmRead(PX_WebsocketEm *em,px_byte *data,px_int data_size)
{
    px_int32 header;

	if (em->recv_fifo.usedsize)
	{
		while (em->recv_lock);
		em->recv_lock = PX_TRUE;
		PX_FifoBufferPop(&em->recv_fifo, &header, sizeof(header));
		if ((px_int)header> em->recv_fifo.usedsize)
		{
			//error
			PX_MemoryClear(&em->recv_fifo);
			return 0;
		}
		else
		{
			if ((px_int)header <= data_size)
			{
				PX_FifoBufferPop(&em->recv_fifo, data, header);
				em->recv_lock = PX_FALSE;
				return header;
			}
			else
			{
				PX_FifoBufferPop(&em->recv_fifo, 0, header);
				em->recv_lock = PX_FALSE;
				return 0;
			}
		}
		em->recv_lock = PX_FALSE;
	}
    return 0;
}

px_bool PX_WebsocketEmWrite(PX_WebsocketEm *em,px_byte *data,px_int data_size)
{
    EMSCRIPTEN_RESULT result=emscripten_websocket_send_binary(em->socket,data,data_size);
    if (result) {
        return PX_FALSE;
    }
    return PX_TRUE;
}

px_void PX_WebsocketEmFree(PX_WebsocketEm *em)
{
    emscripten_websocket_close(em->socket,1000,"user close");
}

