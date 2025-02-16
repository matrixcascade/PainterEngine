#include "../modules/px_visualos.h"



#define PX_VISUALOS_OPEN		1
#define PX_VISUALOS_PAUSE		2
#define PX_VISUALOS_CONTINUE	3
#define PX_VISUALOS_CLOSE		4
#define PX_VISUALOS_PING		5
#define PX_VISUALOS_EVENT		6
#define PX_VISUALOS_GETINFO		8

typedef struct
{ 
	px_byte VisualOS_Runtime[16 * 1024 * 1024];
	px_memorypool mp;
	px_int x;
	px_int y;
	volatile px_int surface_width;
	volatile px_int surface_height;
	px_surface pre_surface;
	px_mutex surface_lock;
	px_surface cur_surface;
	px_int _3s_frameCount,frameCount;
	px_dword _3s_elapsed,elapsed;
	px_float avg_fps;
	PX_Socket socket;
	px_byte cookie[16];
	PX_Object_VisualOS_init init_func;
}PX_VisualOS_Client;

PX_SOCKET_CONNECT_CALLBACK_FUNCTION(PX_VisualOSOnSocketConnect){}


PX_SOCKET_RECEIVE_CALLBACK_FUNCTION(PX_VisualOSOnSocketReceive)
{
	PX_Object* pObject = (PX_Object*)ptr;
	PX_VisualOS_Client* pClient = (PX_VisualOS_Client*)PX_ObjectGetDescByType(pObject, 0x24101601);
	px_abi abi;
	const px_char* ptype;
	
	PX_AbiCreateStaticReader(&abi, data, datasize);
	ptype=PX_AbiGet_string(&abi,"type");
	if (!ptype)
	{
		return;
	}
	if (PX_strequ(ptype,"frame"))
	{
		px_int* pframewidth;
		px_int* pframeheight;
		px_byte* framedata;
		px_int framedatasize;
		pframewidth = PX_AbiGet_int(&abi, "width");
		pframeheight = PX_AbiGet_int(&abi, "height");

		if (!pframewidth || !pframeheight)
		{
			return;
		}
		framedata = PX_AbiGet_data(&abi, "data",&framedatasize);
		if (!framedata)
		{
			return;
		}
		if (pClient->surface_width==0)
		{
			pClient->surface_width = *pframewidth;
		}
		if (pClient->surface_height == 0)
		{
			pClient->surface_height = *pframeheight;
		}

		if (pClient->pre_surface.width!= pClient->surface_width || pClient->pre_surface.height != pClient->surface_height)
		{
			if (pClient->pre_surface.MP)
			{
				PX_TextureFree(&pClient->pre_surface);
				PX_memset(&pClient->pre_surface, 0, sizeof(pClient->pre_surface));
			}
			if(!PX_SurfaceCreate(&pClient->mp, pClient->surface_width, pClient->surface_height, &pClient->pre_surface))
			{
				return;
			}
			PX_SurfaceClearAll(&pClient->pre_surface, PX_COLOR(0, 0, 0, 0));
		}
		if (pClient->cur_surface.width != pClient->surface_width || pClient->cur_surface.height != pClient->surface_height)
		{
			if (pClient->cur_surface.MP)
			{
				PX_TextureFree(&pClient->cur_surface);
				PX_memset(&pClient->cur_surface, 0, sizeof(pClient->cur_surface));
			}
			if(!PX_SurfaceCreate(&pClient->mp, pClient->surface_width, pClient->surface_height, &pClient->cur_surface))
			{
				return;
			}
			PX_SurfaceClearAll(&pClient->cur_surface, PX_COLOR(0, 0, 0, 0));
		}
		pObject->Width = (px_float)pClient->surface_width;
		pObject->Height = (px_float)pClient->surface_height;

		PX_MutexLock(&pClient->surface_lock);
		if (!PX_TextureDiffUnzip(&pClient->mp, framedata, framedatasize, &pClient->pre_surface, &pClient->cur_surface))
			printf("unzip error\n");
		else
			PX_TextureCover(&pClient->pre_surface, &pClient->cur_surface, 0, 0, PX_ALIGN_LEFTTOP);
		PX_MutexUnlock(&pClient->surface_lock);
	}
	else if(PX_strequ(ptype, "init"))
	{
		PX_Object_Event e = { 0 };
		px_int* pframewidth;
		px_int* pframeheight;
		pframewidth = PX_AbiGet_int(&abi, "width");
		pframeheight = PX_AbiGet_int(&abi, "height");
		if (!pframewidth || !pframeheight)
		{
			return;
		}
		pClient->init_func(pObject, *pframewidth, *pframeheight);
	}
}

PX_SOCKET_DISCONNECT_CALLBACK_FUNCTION(PX_VisualOSOnSocketDisconnect){}

PX_OBJECT_EVENT_FUNCTION(PX_VisualOSOnEvent)
{
	px_float x, y, w, h;
	PX_VisualOS_Client* pClient = (PX_VisualOS_Client*)PX_ObjectGetDescByType(pObject, 0x24101601);
	typedef struct
	{
		px_dword opcode;
		px_byte session[16];
	}PX_VisualOS_Packet_Header;
	typedef struct
	{
		PX_VisualOS_Packet_Header header;
		px_uint32 event;
		px_int32 x, y, z;
	}PX_VisualOS_Packet_Event;
	PX_VisualOS_Packet_Event packet;

	if (!pClient->socket.isConnecting) return;
	PX_OBJECT_INHERIT_CODE(pObject,x,y,w,h);
	packet.header.opcode = PX_VISUALOS_EVENT;
	memcpy(packet.header.session, pClient->cookie, sizeof(packet.header.session));
	
	packet.event=e.Event;
	packet.x= (px_int)(PX_Object_Event_GetCursorX(e)-x);
	packet.y= (px_int)(PX_Object_Event_GetCursorY(e)-y);
	packet.z= (px_int)(PX_Object_Event_GetCursorZ(e));
	PX_SocketSend(&pClient->socket, (px_byte*)&packet, sizeof(packet));
}

PX_OBJECT_RENDER_FUNCTION(PX_VisualOSOnRender)
{
	px_float x, y, w, h;
	PX_VisualOS_Client* pClient = (PX_VisualOS_Client*)PX_ObjectGetDescByType(pObject, 0x24101601);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	if (pClient->socket.isConnecting)
	{
		PX_MutexLock(&pClient->surface_lock);
		PX_SurfaceCover(psurface, &pClient->cur_surface, (px_int)x, (px_int)y, PX_ALIGN_LEFTTOP);
		PX_MutexUnlock(&pClient->surface_lock);
		pClient->frameCount++;
		pClient->_3s_frameCount++;
	}

	pClient->elapsed += elapsed;
	pClient->_3s_elapsed += elapsed;
	if (pClient->_3s_elapsed>=3000)
	{
		pClient->avg_fps = (px_float)pClient->_3s_frameCount / (px_float)pClient->_3s_elapsed * 1000;
		pClient->_3s_elapsed = 0;
		pClient->_3s_frameCount = 0;
	}
}

PX_OBJECT_FREE_FUNCTION(PX_VisualOSOnFree)
{
	PX_VisualOS_Client* pClient = (PX_VisualOS_Client*)PX_ObjectGetDescByType(pObject, 0x24101601);
	PX_SocketClose(&pClient->socket);
}

PX_Object* PX_Object_VisualOSAttachObject(PX_Object* pObject, px_int attachIndex)
{
	px_memorypool* mp = pObject->mp;
	PX_VisualOS_Client* pdesc;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_VisualOS_Client*)PX_ObjectCreateDesc(pObject, attachIndex, 0x24101601, 0, PX_VisualOSOnRender, 0,0, sizeof(PX_VisualOS_Client));
	PX_ASSERTIF(pdesc == PX_NULL);
	PX_MutexInitialize(&pdesc->surface_lock);
	pdesc->mp = PX_MemorypoolCreate(pdesc->VisualOS_Runtime, sizeof(pdesc->VisualOS_Runtime));
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_VisualOSOnEvent, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_VisualOSOnEvent, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORRDOWN, PX_VisualOSOnEvent, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORRUP, PX_VisualOSOnEvent, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_VisualOSOnEvent, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_VisualOSOnEvent, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_KEYDOWN, PX_VisualOSOnEvent, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_KEYUP, PX_VisualOSOnEvent, 0);
	return pObject;
}


PX_Object* PX_Object_VisualOSCreate(px_memorypool* mp, PX_Object* Parent, const px_char host[], px_word port,const px_byte cookie[16], px_float x, px_float y, PX_Object_VisualOS_init init)
{
	PX_Object* pObject;
	PX_VisualOS_Client* pClient;
	pObject = PX_ObjectCreate(mp, Parent, x, y, 0, 0, 0, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_VisualOSAttachObject(pObject, 0))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	pClient = (PX_VisualOS_Client*)PX_ObjectGetDescByType(pObject, 0x24101601);
	memcpy(pClient->cookie, cookie, sizeof(pClient->cookie));
	pClient->init_func = init;
	if(!PX_SocketInitialize(&pClient->socket, 8 * 1024 * 1024, host, port, PX_VisualOSOnSocketConnect, PX_VisualOSOnSocketReceive, PX_NULL, PX_VisualOSOnSocketDisconnect, pObject))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

px_bool PX_Object_VisualOSLogin(PX_Object* pObject, px_int target_x, px_int target_y, px_int width, px_int height)
{
	typedef struct
	{
		px_dword opcode;
		px_byte session[16];
	}PX_Socket_Packet_Header;
	typedef struct
	{
		PX_Socket_Packet_Header header;
		px_int32 x, y, width, height;
	}PX_Socket_Packet_Login;
	PX_VisualOS_Client* pClient = (PX_VisualOS_Client*)PX_ObjectGetDescByType(pObject, 0x24101601);
	PX_Socket_Packet_Login login;
	login.header.opcode = PX_VISUALOS_OPEN;
	memcpy(login.header.session, pClient->cookie, sizeof(login.header.session));
	login.x = target_x;
	login.y = target_y;
	login.width = width;
	login.height = height;
	return PX_SocketSend(&pClient->socket, (px_byte*)&login, sizeof(login));
}

