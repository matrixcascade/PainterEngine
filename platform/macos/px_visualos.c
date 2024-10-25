#include "../modules/px_visualos.h"



#define PX_VISUALOS_OPEN		1
#define PX_VISUALOS_PAUSE		2
#define PX_VISUALOS_CONTINUE	3
#define PX_VISUALOS_CLOSE		4
#define PX_VISUALOS_PING		5
#define PX_VISUALOS_EVENT		6

typedef struct
{ 
	px_byte VisualOS_Runtime[16 * 1024 * 1024];
	px_memorypool mp;
	px_int x;
	px_int y;
	px_int surface_width;
	px_int surface_height;
	px_surface pre_surface;
	volatile px_bool surface_lock;
	px_surface cur_surface;
	px_int _3s_frameCount,frameCount;
	px_dword _3s_elapsed,elapsed;
	px_float avg_fps;
	PX_Socket socket;
	px_byte cookie[16];
}PX_VisualOS_Client;

PX_SOCKET_CONNECT_CALLBACK_FUNCTION(PX_VisualOSOnSocketConnect)
{
	PX_VisualOS_Client* pNewClient = (PX_VisualOS_Client*)ptr;
	pNewClient->mp = PX_MemorypoolCreate(pNewClient->VisualOS_Runtime, sizeof(pNewClient->VisualOS_Runtime));
	PX_SurfaceCreate(&pNewClient->mp, pNewClient->surface_width, pNewClient->surface_height, &pNewClient->cur_surface);
	PX_SurfaceCreate(&pNewClient->mp, pNewClient->surface_width, pNewClient->surface_height, &pNewClient->pre_surface);
	//login
	do
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

		PX_Socket_Packet_Login login;
		login.header.opcode = PX_VISUALOS_OPEN;
		memcpy(login.header.session, pNewClient->cookie, sizeof(login.header.session));
		login.x = pNewClient->x;
		login.y = pNewClient->y;
		login.width = pNewClient->surface_width;
		login.height = pNewClient->surface_height;
		PX_SocketSend(pSocket, (px_byte*)&login, sizeof(login));
	} while (0);
}


PX_SOCKET_RECEIVE_CALLBACK_FUNCTION(PX_VisualOSOnSocketReceive)
{
	PX_VisualOS_Client* pClient = (PX_VisualOS_Client*)ptr;
	while (pClient->surface_lock);
	pClient->surface_lock = PX_TRUE;
	PX_TextureDiffUnzip(&pClient->mp, data, datasize, &pClient->pre_surface, &pClient->cur_surface);
	PX_TextureCover(&pClient->pre_surface, &pClient->cur_surface, 0, 0, PX_ALIGN_LEFTTOP);
	pClient->surface_lock = PX_FALSE;
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
		while (pClient->surface_lock);
		pClient->surface_lock = PX_TRUE;
		PX_SurfaceCover(psurface, &pClient->cur_surface, (px_int)x, (px_int)y, PX_ALIGN_LEFTTOP);
		pClient->surface_lock = PX_FALSE;
		pClient->frameCount++;
		pClient->_3s_frameCount++;
	}
	else
	{
		if(pClient->frameCount==0)
			PX_FontModuleDrawText(psurface,0,(px_int)(x+w/2), (px_int)(y+h/2), PX_ALIGN_CENTER, "Connecting...", PX_COLOR(255, 128, 0, 192));
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

PX_Object* PX_Object_VisualOSAttachObject(PX_Object* pObject, px_int attachIndex, px_float x, px_float y, px_float target_x, px_float target_y, px_float width, px_float height)
{
	px_memorypool* mp = pObject->mp;
	PX_VisualOS_Client* pdesc;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_VisualOS_Client*)PX_ObjectCreateDesc(pObject, attachIndex, 0x24101601, 0, PX_VisualOSOnRender, 0,0, sizeof(PX_VisualOS_Client));
	PX_ASSERTIF(pdesc == PX_NULL);
	pdesc->x = (px_int)target_x;
	pdesc->y = (px_int)target_y;
	pdesc->surface_width = (px_int)width;
	pdesc->surface_height = (px_int)height;


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


PX_Object* PX_Object_VisualOSCreate(px_memorypool* mp, PX_Object* Parent, const px_char host[], px_word port,const px_byte cookie[16], px_float x, px_float y, px_float target_x, px_float target_y, px_float width, px_float height)
{
	PX_Object* pObject;
	PX_VisualOS_Client* pClient;
	pObject = PX_ObjectCreate(mp, Parent, x, y, 0, width, height, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_VisualOSAttachObject(pObject, 0, x, y,target_x,target_y, width, height))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	pClient = (PX_VisualOS_Client*)PX_ObjectGetDescByType(pObject, 0x24101601);
	memcpy(pClient->cookie, cookie, sizeof(pClient->cookie));
	PX_SocketInitialize(&pClient->socket, 8 * 1024 * 1024, host, port, PX_VisualOSOnSocketConnect, PX_VisualOSOnSocketReceive, PX_NULL, PX_VisualOSOnSocketDisconnect, pClient);
	return pObject;
}