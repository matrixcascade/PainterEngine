#include "runtime/PainterEngine_Application.h"
#include "platform/modules/px_time.h"
#include "platform/modules/px_sockethub.h"

#define PX_VISUALOS_OPEN		1
#define PX_VISUALOS_PAUSE		2
#define PX_VISUALOS_CONTINUE	3
#define PX_VISUALOS_CLOSE		4
#define PX_VISUALOS_PING		5
#define PX_VISUALOS_EVENT		6
#ifndef PX_VISUALOS_COOKIE
#define PX_VISUALOS_COOKIE     "0000000000000000"
#endif
PX_SocketHub SocketHub;
px_mutex cache_surface_lock;
volatile px_dword cache_surface_id = 1;
volatile px_bool event_lock = PX_FALSE;
px_byte event_enter_session[16];
PX_Object_Event event_stack[128];
volatile px_int event_stack_count = 0;
volatile px_dword event_lasttime = 0;

px_surface cache_surface;

typedef enum
{
	PX_VIRTUALPS_CLIENT_STATE_IDLE,
	PX_VIRTUALPS_CLIENT_STATE_RUNNING,
	PX_VIRTUALPS_CLIENT_STATE_PAUSE,
}PX_VirtualPS_Client_State;

typedef struct
{
	px_byte*    cache;
	px_int      cachesize;
	px_byte     cookie[16];
	px_memorypool mp;
	px_dword    last_texture_id;
	px_int      x, y;
	px_texture  pre_texture;
	px_texture  cur_texture;
	px_memory   zip_data;
	PX_VirtualPS_Client_State state;
}PX_VisualOS_Client;

#define PX_VisualOS_Instance_Runtime 1024*1024*16

px_void OnSocketAcceptEvent(const px_byte session[16], PX_Object_Event e)
{
	px_byte zero16[16] = { 0 };
	while (event_lock);
	event_lock = PX_TRUE;
	if (PX_memequ(event_enter_session, zero16, 16))
	{
		event_lasttime = 0;
		PX_memcpy(event_enter_session, session, sizeof(event_enter_session));
	}
	else if (!PX_memequ(event_enter_session, session, sizeof(event_enter_session)))
	{
		event_lock = PX_FALSE;
		return;
	}
	event_lasttime = 0;
	if (event_stack_count < PX_COUNTOF(event_stack))
	{
		event_stack[event_stack_count] = e;
		event_stack_count++;
	}
	event_lock = PX_FALSE;
}

PX_SOCKETHUB_CONNECT_CALLBACK_FUNCTION(OnSocketHubConnect)
{
	PX_VisualOS_Client *pNewClient=(PX_VisualOS_Client *)malloc(sizeof(PX_VisualOS_Client));
	if (!pNewClient)
	{
		return;
	}
	memset(pNewClient,0,sizeof(PX_VisualOS_Client));
	pNewClient->cache=(px_byte *)malloc(PX_VisualOS_Instance_Runtime);
	if (!pNewClient->cache)
	{
		free(pNewClient);
		return;
	}
	pNewClient->cachesize= PX_VisualOS_Instance_Runtime;
	pNewClient->mp=MP_Create(pNewClient->cache,pNewClient->cachesize);
	pNewClient->state=PX_VIRTUALPS_CLIENT_STATE_IDLE;
	PX_MemoryInitialize(&pNewClient->mp ,&pNewClient->zip_data);
	PX_SocketHubSetUserPtr(pSocketHub,session,pNewClient);
}

PX_SOCKETHUB_RECEIVE_CALLBACK_FUNCTION(OnSocketHubRecv)
{
	typedef struct
	{
		px_dword opcode;
		px_byte  cookie[16];
	}PX_VisualOS_Packet_Header;
	PX_VisualOS_Client*pClient=(PX_VisualOS_Client *)PX_SocketHubGetUserPtr(pSocketHub,session);
	PX_VisualOS_Packet_Header*pHeader=(PX_VisualOS_Packet_Header*)data;
	
	if (!PX_memequ(pHeader->cookie, PX_VISUALOS_COOKIE,sizeof(pHeader->cookie)))
	{
		PX_SocketHubCloseSession(pSocketHub, session);
		return;
	}
	
	switch (pHeader->opcode)
	{
	case PX_VISUALOS_OPEN:
	{
		typedef struct
		{
			PX_VisualOS_Packet_Header header;
			px_int32 x,y,width,height;
		}PX_VisualOS_Packet_Login;
		if (datasize!=sizeof(PX_VisualOS_Packet_Login))
		{
			return;
		}
		PX_VisualOS_Packet_Login *pLogin=(PX_VisualOS_Packet_Login *)data;
	
		if (pClient->pre_texture.MP)
			PX_TextureFree(&pClient->pre_texture);
		PX_memset(&pClient->pre_texture,0,sizeof(pClient->pre_texture));
		//create texture
		if(!PX_TextureCreate(&pClient->mp, &pClient->pre_texture, pLogin->width, pLogin->height))
		{
			//close session
			PX_SocketHubCloseSession(pSocketHub,session);
			return;
		}
		if (!PX_TextureCreate(&pClient->mp, &pClient->cur_texture, pLogin->width, pLogin->height))
		{
			PX_TextureFree(&pClient->pre_texture);
			PX_memset(&pClient->pre_texture, 0, sizeof(pClient->pre_texture));
			PX_SocketHubCloseSession(pSocketHub, session);
			return;
		}
		pClient->x=pLogin->x;
		pClient->y=pLogin->y;
		pClient->last_texture_id=0;
		pClient->state=PX_VIRTUALPS_CLIENT_STATE_RUNNING;
		
		PX_MemoryClear(&pClient->zip_data);
		printf("Client Open: session:%x offset:%d,%d width:%d height:%d\n", *(px_dword*)session, pLogin->x, pLogin->y, pLogin->width, pLogin->height);
	}
	break;
	case PX_VISUALOS_CLOSE:
	{
		if (pClient->pre_texture.MP)
			PX_TextureFree(&pClient->pre_texture);
		PX_memset(&pClient->pre_texture,0,sizeof(pClient->pre_texture));
		pClient->state=PX_VIRTUALPS_CLIENT_STATE_IDLE;
		pClient->last_texture_id=0;
		PX_MemoryClear(&pClient->zip_data);
		printf("Client Close:\nsession:%x\n", *(px_dword*)session);
	}
	break;
	case PX_VISUALOS_PAUSE:
	{
		if(pClient->state==PX_VIRTUALPS_CLIENT_STATE_RUNNING)
			pClient->state= PX_VIRTUALPS_CLIENT_STATE_PAUSE;
		printf("Client Pause:\nsession:%x\n", *(px_dword*)session);
	}
	break;
	case PX_VISUALOS_CONTINUE:
	{
		if(pClient->state==PX_VIRTUALPS_CLIENT_STATE_PAUSE)
			pClient->state= PX_VIRTUALPS_CLIENT_STATE_RUNNING;
		printf("Client Continue:\nsession:%x\n", *(px_dword*)session);
	}
	break;
	case PX_VISUALOS_PING:
	{
		typedef struct
		{
			PX_VisualOS_Packet_Header header;
			px_dword time;
		}PX_VisualOS_Packet_Ping;
		PX_VisualOS_Packet_Ping *pPing=(PX_VisualOS_Packet_Ping *)data;
		pPing->time=PX_TimeGetTime();
		PX_SocketHubSend(pSocketHub,session,(px_byte *)pPing,sizeof(PX_VisualOS_Packet_Ping));
		printf("Client Ping:session %x\n", *(px_dword*)session);
	}
	break;
	case PX_VISUALOS_EVENT:
	{
		typedef struct
		{
			PX_VisualOS_Packet_Header header;
			px_uint32 event;
			px_int32 x, y, z;
		}PX_VisualOS_Packet_Event;
		PX_VisualOS_Packet_Event *pPacketEvent=(PX_VisualOS_Packet_Event *)data;
		PX_Object_Event e = {0};
		px_dword zero16[4] = { 0 };
		e.Event = pPacketEvent->event;
		PX_Object_Event_SetCursorX(&e, pPacketEvent->x*1.f);
		PX_Object_Event_SetCursorY(&e, pPacketEvent->y*1.f);
		PX_Object_Event_SetCursorZ(&e, pPacketEvent->z*1.f);
		OnSocketAcceptEvent(session, e);
	}
	default:
		break;
	}
}

PX_SOCKETHUB_SEND_CALLBACK_FUNCTION(OnSocketHubSend)
{
	//send callback
	PX_VisualOS_Client* pClient = (PX_VisualOS_Client*)PX_SocketHubGetUserPtr(pSocketHub, session);
	
	if (pClient->state==PX_VIRTUALPS_CLIENT_STATE_RUNNING)
	{
		if(pClient->last_texture_id!=cache_surface_id&&PX_SocketHubGetClientSendCacheSize(pSocketHub,session)==0)
		{
			pClient->last_texture_id = cache_surface_id;
			PX_MutexLock(&cache_surface_lock);
			PX_TextureCover(&pClient->cur_texture, &cache_surface, -pClient->x, -pClient->y,PX_ALIGN_LEFTTOP);
			PX_MutexUnlock(&cache_surface_lock);
		
			PX_MemoryClear(&pClient->zip_data);
			if (PX_TextureDiffZip(&pClient->mp, &pClient->pre_texture, &pClient->cur_texture, &pClient->zip_data))
			{
				PX_TextureCover(&pClient->pre_texture, &pClient->cur_texture, 0, 0, PX_ALIGN_LEFTTOP);
				PX_SocketHubSend(pSocketHub, session, pClient->zip_data.buffer, pClient->zip_data.usedsize);
				printf("Send Frame %d\n",pClient->zip_data.usedsize);
			}
			else
			{
				printf("failed to zip\n");
			}

		}
	}
}

PX_SOCKETHUB_DISCONNECT_CALLBACK_FUNCTION(OnSocketHubDisconnect)
{
	PX_VisualOS_Client *pClient=(PX_VisualOS_Client *)PX_SocketHubGetUserPtr(pSocketHub,session);
	if (pClient)
	{
		free(pClient->cache);
		free(pClient);
	}
	//set to null
	PX_SocketHubSetUserPtr(pSocketHub,session,PX_NULL);
}


int  PX_AudioInitialize(PX_SoundPlay* soundPlay)
{
	return 0;
}

int main()
{
	px_dword timelasttime=0;
	PX_MutexInitialize(&cache_surface_lock);
	if (!PX_ApplicationInitialize(&App,0,0))
	{
		return 0;
	}
	printf("\nPainterEngine VisualOS Server\n");
	if (!PX_SocketHubInitialize(&SocketHub, PX_SocketHub_DEFAULT_TCP_PORT, PX_SocketHub_DEFAULT_WEBSOCKET_PORT, 1024 * 1024 * 8, OnSocketHubConnect, OnSocketHubSend, OnSocketHubRecv, OnSocketHubDisconnect, &App))
	{
		return 0;
	}
	printf("Server Start\n");

	timelasttime=PX_TimeGetTime();
	while (1)
	{
		px_dword now=PX_TimeGetTime();
		px_dword elapsed=now-timelasttime;
		px_int i;
		timelasttime= now;
		while(event_lock);
		event_lock=PX_TRUE;
		
		for(i=0;i<event_stack_count;i++)
		{
			PX_ObjectPostEvent(App.object_root,event_stack[i]);
		}
		event_stack_count=0;
		event_lasttime += elapsed;
		if (event_lasttime>=3000)
		{
			PX_zeromemory(event_enter_session, sizeof(event_enter_session));
		}
		event_lock=PX_FALSE;

		PX_ApplicationUpdate(&App,elapsed);
		PX_ApplicationRender(&App,elapsed);

		PX_MutexLock(&cache_surface_lock);
		if (cache_surface.width!=App.runtime.surface_width|| cache_surface.height!= App.runtime.surface_height)
		{
			if(cache_surface.surfaceBuffer)
				PX_SurfaceFree(&cache_surface);
			if (!PX_SurfaceCreate(&App.runtime.mp, App.runtime.surface_width, App.runtime.surface_height, &cache_surface))
			{
				//outofmemory crash
				return 0;
			}
		}
		//copy render surface to cache surface
		if (cache_surface.surfaceBuffer)
		{
			if(!PX_memequ(cache_surface.surfaceBuffer, App.runtime.RenderSurface.surfaceBuffer, cache_surface.width*cache_surface.height*sizeof(px_color)))
			{
				memcpy(cache_surface.surfaceBuffer, App.runtime.RenderSurface.surfaceBuffer, cache_surface.width*cache_surface.height* sizeof(px_color));
				cache_surface_id++;
			}
		}
		PX_MutexUnlock(&cache_surface_lock);
	}
}