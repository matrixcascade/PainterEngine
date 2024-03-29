#include "px_websocketserver.h"

px_int PX_WebsocketServerLinkerRead_func(struct _PX_Linker* linker, px_void* data, px_int datasize, px_void* userPtr)
{
	PX_WebsocketServer_Instance* pInstance = (PX_WebsocketServer_Instance *)userPtr;
	return PX_TCPSocketReceived(pInstance->socket, data, datasize,  1);
}
px_int PX_WebsocketServerLinkerWrite_func(struct _PX_Linker* linker, px_void* data, px_int datasize, px_void* userPtr)
{
	PX_WebsocketServer_Instance* pInstance = (PX_WebsocketServer_Instance*)userPtr;
	return PX_TCPSocketSend(pInstance->socket, data, datasize);
}

px_void PX_WebsocketServer_linkerthread(px_void* ptr)
{
	PX_WebsocketServer_Instance* pInstance = (PX_WebsocketServer_Instance*)ptr;
	PX_WebsocketServer* pServer = (PX_WebsocketServer*)pInstance->pserver;
	px_byte cache[1024];
	px_dword lastupdatetime = PX_TimeGetTime();
	PX_WebSocketListen(&pInstance->websocket_Instance);
	while (PX_TRUE)
	{
		px_dword now = PX_TimeGetTime();
		px_dword elapsed =  now- lastupdatetime;
		px_int size= PX_WebSocketRead(&pInstance->websocket_Instance, cache, sizeof(cache));
		lastupdatetime = now;
		pInstance->elapsed += elapsed;
		if (size)
		{
			PX_WebsocketServer_Data_header header;
			header.handle = pInstance->handle;
			header.size = size;

			while (pServer->recv_lock);
			pServer->recv_lock = PX_TRUE;
			if (PX_FifoBufferPush(&pServer->recv_fifo, &header, sizeof(header)))
			{
				if (!PX_FifoBufferPush(&pServer->recv_fifo,cache,size))
				{
					//fifo full
					PX_MemoryClear(&pServer->recv_fifo);
				}
			}
			pServer->recv_lock = PX_FALSE;
		}

		if (pInstance->elapsed>=6000)
		{
			if (pInstance->websocket_Instance.state!=PX_WEBSOCKET_STATE_CONNECTING)
			{
				break;
			}
		}

		if(pInstance->websocket_Instance.state== PX_WEBSOCKET_STATE_CLOSE)
		{
			break;
		}
	}

	PX_WebSocketFree(&pInstance->websocket_Instance);
	PX_TCPSocketFree(pInstance->socket);
	while (pServer->connect_lock);
	pServer->connect_lock = PX_TRUE;
	pInstance->activating = PX_FALSE;
	pServer->connect_lock = PX_FALSE;
}


px_void PX_WebsocketServer_thread(px_void* ptr)
{
	PX_WebsocketServer* pServer = (PX_WebsocketServer*)ptr;
	while (pServer->status==PX_WEBSOCKETSERVER_STATUS_RUNNING)
	{
		px_uint new_socket;
		PX_TCP_ADDR from_addr;
		if (PX_TCPAccept(&pServer->tcpserver, &new_socket, &from_addr))
		{
			px_int i = 0;
			//new connection
			while (pServer->connect_lock);
			pServer->connect_lock = PX_TRUE;
			for (i = 0; i < PX_COUNTOF(pServer->Instances); i++)
			{
				if (!pServer->Instances[i].activating)
				{
					break;
				}
			}
			if (i!= PX_COUNTOF(pServer->Instances))
			{
				PX_memset(&pServer->Instances[i], 0, sizeof(pServer->Instances[i]));
				pServer->Instances[i].activating = PX_TRUE;
				pServer->Instances[i].elapsed = 0;
				pServer->Instances[i].pserver = pServer;
				pServer->Instances[i].socket = new_socket;
				pServer->Instances[i].handle = pServer->alloc_handle++;
				pServer->Instances[i].mp = MP_Create(pServer->Instances[i].cache, sizeof(pServer->Instances[i].cache));
				PX_LinkerInitialize(&pServer->Instances[i].linker, 0, PX_WebsocketServerLinkerRead_func, PX_WebsocketServerLinkerWrite_func, 0, &pServer->Instances[i]);
				PX_WebSocketInitialize(&pServer->Instances[i].mp, &pServer->Instances[i].websocket_Instance, &pServer->Instances[i].linker);
				PX_ThreadCreate(&pServer->Instances[i].thread, PX_WebsocketServer_linkerthread, &pServer->Instances[i]);
			}
			else
			{
				PX_TCPSocketFree(new_socket);
			}
			pServer->connect_lock = PX_FALSE;
		}
	}
	return;
}

px_bool PX_WebsocketServerInitialize(PX_WebsocketServer* pServer,px_word port)
{
	PX_memset(pServer, 0, sizeof(PX_WebsocketServer));
	pServer->mp_recv = MP_Create(pServer->recv_buffer, sizeof(pServer->recv_buffer));

	PX_FifoBufferInitialize(&pServer->mp_recv, &pServer->recv_fifo);

	
	if (!PX_TCPInitialize(&pServer->tcpserver, PX_TCP_IP_TYPE_IPV4))
	{
		pServer->status = PX_WEBSOCKETSERVER_STATUS_ERROR;
		return PX_FALSE;
	}
	if (!PX_TCPListen(&pServer->tcpserver, PX_htons(port)))
	{
		PX_TCPFree(&pServer->tcpserver);
		pServer->status = PX_WEBSOCKETSERVER_STATUS_ERROR;
		return PX_FALSE;
	}
	pServer->status = PX_WEBSOCKETSERVER_STATUS_RUNNING;
	PX_ThreadCreate(&pServer->server_thread, PX_WebsocketServer_thread, pServer);
	return PX_TRUE;
}

px_int PX_WebsocketServerRead(PX_WebsocketServer* pServer, px_dword* handle, px_byte* data, px_int recv_data_size)
{
	PX_WebsocketServer_Data_header header;

	if (pServer->recv_fifo.usedsize)
	{
		while (pServer->recv_lock);
		pServer->recv_lock = PX_TRUE;
		PX_FifoBufferPop(&pServer->recv_fifo, &header, sizeof(header));
		if ((px_int)header.size > pServer->recv_fifo.usedsize)
		{
			//error
			PX_MemoryClear(&pServer->recv_fifo);
			return 0;
		}
		else
		{
			if ((px_int)header.size <= recv_data_size)
			{
				PX_FifoBufferPop(&pServer->recv_fifo, data, header.size);
				pServer->recv_lock = PX_FALSE;
				*handle = header.handle;
				return header.size;
			}
			else
			{
				PX_FifoBufferPop(&pServer->recv_fifo, 0, header.size);
				pServer->recv_lock = PX_FALSE;
				return 0;
			}
		}
		pServer->recv_lock = PX_FALSE;
	}
	
	return 0;
}

px_bool PX_WebsocketServerWrite(PX_WebsocketServer* pServer, px_dword handle, const px_byte* data, px_int send_data_size)
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pServer->Instances);i++)
	{
		if (pServer->Instances[i].activating)
		{
			if (pServer->Instances[i].handle==handle)
			{
				PX_WebSocketWrite(&pServer->Instances[i].websocket_Instance, data, send_data_size);
				return PX_TRUE;
			}
		}
	}
	return PX_FALSE;
}

px_void PX_WebsocketServerFree(PX_WebsocketServer* pServer)
{
	px_int i;
	pServer->status = PX_WEBSOCKETSERVER_STATUS_CLOSED;
	PX_TCPFree(&pServer->tcpserver);
	for (i=0;i<PX_COUNTOF(pServer->Instances);i++)
	{
		if (pServer->Instances[i].activating)
		{
			PX_TCPSocketFree(pServer->Instances[i].socket);
		}
	}

	for (i = 0; i < PX_COUNTOF(pServer->Instances); i++)
	{
		while (pServer->Instances[i].activating);
	}
}

