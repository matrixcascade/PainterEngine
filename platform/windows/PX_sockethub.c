#include "../modules/px_sockethub.h"
#include "../modules/px_sysmemory.h"

px_int PX_SocketHubCircularBufferGetSpaceSize(px_int buffersize, volatile px_int wCursor, volatile px_dword rCursor)
{
	px_int spacesize, _WCursor = wCursor;
	px_int _RCursor = rCursor;
	spacesize = _WCursor >= _RCursor ? buffersize - (_WCursor - _RCursor) - 8 : _RCursor - _WCursor - 8;
	return spacesize;
}

px_int PX_SocketHubCircularBufferGetReadySize(px_int buffersize, volatile px_int wCursor, volatile px_dword rCursor)
{
	px_int readysize, _WCursor = wCursor;
	px_int _RCursor = rCursor;
	readysize = _WCursor >= _RCursor ? _WCursor - _RCursor : buffersize - _RCursor;
	return readysize;
}




static px_bool PX_SocketHubWriteToCircularBuffer(px_byte* buffer, px_int buffersize,volatile px_int *wCursor, volatile px_dword rCursor,const px_void* data, px_int datasize)
{
	px_int spacesize, divsize;
	px_int _WCursor = *wCursor;
	px_int _RCursor = rCursor;

	spacesize = _WCursor >= _RCursor ? buffersize - (_WCursor - _RCursor) - 8 : _RCursor - _WCursor - 8;
	if (datasize > spacesize) return PX_FALSE;
	divsize = buffersize - _WCursor;
	if (divsize > spacesize) divsize = spacesize;

	if (divsize >= datasize)
	{
		memcpy(buffer + _WCursor, data, datasize);
		*wCursor = (_WCursor + datasize) % buffersize;
	}
	else
	{
		memcpy(buffer + _WCursor, data, divsize);
		memcpy(buffer, (const px_byte*)data + divsize, datasize - divsize);
		*wCursor = datasize - divsize;
	}
	return PX_TRUE;
}



px_void PX_SocketHubThread_TCP_SendHander(px_void *ptr)
{
	PX_TCP_Instance *pInstance=(PX_TCP_Instance*)ptr;
	px_byte session[16];
	px_dword lasttime;
	px_dword index=PX_ARRAY_INDEX(pInstance->pSocketHub->tcp_instance,pInstance);
	//build session header
	PX_memset(session, 0, sizeof(session));
	session[0] = 1;//tcp
	PX_memcpy(session + 1, &index, sizeof(index));

	
	lasttime = PX_TimeGetTime();

	while (!pInstance->send_end)
	{
		px_dword elpased, now;
		px_int ready_send_data_size;
		now = PX_TimeGetTime();
		elpased = now - lasttime;
		lasttime = now;

		if (pInstance->pSocketHub->send_callback)
		{
			pInstance->pSocketHub->send_callback(pInstance->pSocketHub, session, elpased, pInstance->pSocketHub->userptr);
		}

		//send data

		do
		{
			px_int send_size;
			ready_send_data_size= PX_SocketHubCircularBufferGetReadySize(pInstance->cache_size, pInstance->send_cache_wcursor, pInstance->send_cache_rcursor);
			send_size = PX_TCPSocketSend(pInstance->socket, pInstance->send_cache+ pInstance->send_cache_rcursor, ready_send_data_size);
			if (send_size > 0)
			{
				pInstance->send_cache_rcursor = (pInstance->send_cache_rcursor + send_size) % pInstance->cache_size;
			}
			else if (send_size < 0)
			{
				return;
			}
			else
			{
				break;
			}
		}while(ready_send_data_size);
		PX_Sleep(0);
	}
}

px_void PX_SocketHubThread_TCP_RecvHander(px_void* ptr)
{
	PX_TCP_Instance* pInstance = (PX_TCP_Instance*)ptr;
	px_byte session[16];
	
	px_dword index = PX_ARRAY_INDEX(pInstance->pSocketHub->tcp_instance, pInstance);
	//build session header
	PX_memset(session, 0, sizeof(session));
	session[0] = 1;//tcp
	PX_memcpy(session + 1, &index, sizeof(index));
	

	if (pInstance->pSocketHub->connect_callback)
		pInstance->pSocketHub->connect_callback(pInstance->pSocketHub, session, pInstance->pSocketHub->userptr);

	pInstance->send_end=PX_FALSE;
		PX_ThreadCreate(&pInstance->recv_thread, PX_SocketHubThread_TCP_SendHander, pInstance);

	while (PX_TRUE)
	{
		//recv data
		px_byte* pCache = pInstance->recv_cache;
		px_int recv_size = PX_TCPSocketReceived(pInstance->socket, pCache + pInstance->recv_cache_offset, pInstance->cache_size - pInstance->recv_cache_offset, -1);
		
		if (recv_size > 0)
		{
			pInstance->recv_cache_offset += recv_size;
			while (PX_TRUE)
			{
				px_dword packet_size = *(px_dword*)pCache;
				if (packet_size > pInstance->cache_size - sizeof(px_dword))
				{
					printf("Disconnect session:%d by packet out of bound!\n", PX_crc32(session, 16));
					PX_TCPSocketFree(pInstance->socket);
					return;
				}
				else if (pInstance->recv_cache_offset >= sizeof(px_dword) + packet_size)
				{
					PX_SocketHub* pSocketHub = pInstance->pSocketHub;
					//new packet
					if (pSocketHub->recv_callback)
					{
						pSocketHub->recv_callback(pSocketHub, session, pCache + sizeof(px_dword), packet_size, pSocketHub->userptr);
					}

					//move cache
					memcpy(pCache, pCache + sizeof(px_dword) + packet_size, pInstance->recv_cache_offset - sizeof(px_dword) - packet_size);
					pInstance->recv_cache_offset -= packet_size + sizeof(px_dword);
				}
				else
				{
					break;
				}
			}

		}
		else if (recv_size <= 0)
		{
			printf("Disconnect session:%d by client close\n", PX_crc32(session, 16));
			goto DISCONNECT;
		}
	}
DISCONNECT:
	//disconnect
	PX_TCPSocketFree(pInstance->socket);
	while (pInstance->recv_thread.isRun);
	if (pInstance->pSocketHub->disconnect_callback)
		pInstance->pSocketHub->disconnect_callback(pInstance->pSocketHub, session, pInstance->pSocketHub->userptr);
	pInstance->send_end = PX_TRUE;
	
	while (pInstance->pSocketHub->tcp_connect_lock);
	pInstance->pSocketHub->tcp_connect_lock = PX_TRUE;
	pInstance->socket = 0;
	pInstance->recv_cache_offset = 0;
	pInstance->send_cache_wcursor = 0;
	pInstance->send_cache_rcursor = 0;
	pInstance->pSocketHub->tcp_connect_lock = PX_FALSE;
	printf("session thread terminated:%d \n", PX_crc32(session, 16));
}


px_void PX_SocketHubThread_TCPHander(px_void *ptr)
{
	px_int i;
	PX_SocketHub* pSocketHub=(PX_SocketHub *)ptr;
	while (pSocketHub->tcp_enable)
	{
		PX_TCP_ADDR addr;
		px_uint socket;
		if (PX_TCPAccept(&pSocketHub->tcp,&socket, &addr))
		{
			while(pSocketHub->tcp_connect_lock);
			pSocketHub->tcp_connect_lock=PX_TRUE;
			for  (i = 0; i < PX_SOCKETHUB_MAX_CONNECTION; i++)
			{
				if (pSocketHub->tcp_instance[i].socket==0)
				{
					pSocketHub->tcp_instance[i].socket=socket;
					pSocketHub->tcp_instance[i].recv_cache_offset=0;
					pSocketHub->tcp_instance[i].send_cache_wcursor=0;
					pSocketHub->tcp_instance[i].send_cache_rcursor = 0;
					printf("new tcp connection %d begin s/r thread!\n", i);
					PX_ThreadCreate(&pSocketHub->tcp_instance[i].send_thread, PX_SocketHubThread_TCP_RecvHander, &pSocketHub->tcp_instance[i]);
					break;
				}
			}

			if (i==PX_SOCKETHUB_MAX_CONNECTION)
			{
				PX_TCPSocketFree(socket);
			}

			pSocketHub->tcp_connect_lock=PX_FALSE;
		}
	}
}

px_int PX_WebsocketServerLinkerRead_func(struct _PX_Linker* linker, px_void* data, px_int datasize, px_void* userPtr)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)userPtr;
	return PX_TCPSocketReceived(pInstance->socket, data, datasize, -1);
}
px_int PX_WebsocketServerLinkerWrite_func(struct _PX_Linker* linker, px_void* data, px_int datasize, px_void* userPtr)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)userPtr;
	return PX_TCPSocketSend(pInstance->socket, data, datasize);
}

px_void PX_WebsocketServerLinkerClose_func(struct _PX_Linker* linker,  px_void* userPtr)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)userPtr;
	PX_TCPSocketFree(pInstance->socket);
}

PX_WEBSOCKET_CONNECT_CALLBACK_FUNCTION(PX_WebsocketServerConnectCallback)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)ptr;
	px_byte session[16] = { 0 };
	session[0] = 2;//websocket
	px_dword arrayIndex = PX_ARRAY_INDEX(pInstance->pSocketHub->websocket_instance, pInstance);
	PX_memcpy(session + 1, &arrayIndex, sizeof(arrayIndex));
	printf("new websocket connection session:%d\n", PX_crc32(session, 16));
	if (pInstance->pSocketHub->connect_callback)
	{
		pInstance->pSocketHub->connect_callback(pInstance->pSocketHub, session, pInstance->pSocketHub->userptr);
	}
}

PX_WEBSOCKET_RECEIVED_CALLBACK_FUNCTION(PX_WebsocketServerRecvCallback)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)ptr;
	PX_SocketHub* pSocketHub = pInstance->pSocketHub;
	px_byte* pCache = pInstance->recv_cache;
	px_byte session[16] = { 0 };
	px_dword arrayIndex;
	session[0] = 2;//websocket
	arrayIndex = PX_ARRAY_INDEX(pSocketHub->websocket_instance, pInstance);
	PX_memcpy(session + 1, &arrayIndex, sizeof(arrayIndex));
	if (datasize > 0)
	{
		px_dword packet_size;
		if ((px_dword)datasize < pInstance->cache_size - pInstance->recv_cache_offset)
		{
			PX_memcpy(pCache + pInstance->recv_cache_offset, data, datasize);
			pInstance->recv_cache_offset += datasize;
		}
		else
		{
			printf("Disconnect session:%d by out of cache!\n", PX_crc32(session, 16));
			PX_WebSocketFree(&pInstance->websocket_Instance);
		}
		while (PX_TRUE)
		{
			packet_size = *(px_dword*)pCache;
			if (packet_size > pInstance->cache_size - sizeof(px_dword))
			{
				printf("Disconnect session:%d by packet out of bound!\n", PX_crc32(session, 16));
				PX_WebSocketFree(&pInstance->websocket_Instance);
				break;
			}
			else if (pInstance->recv_cache_offset >= sizeof(px_dword) + packet_size)
			{
				//new packet
				if (pSocketHub->recv_callback)
				{
					PX_MutexLock(&pInstance->rs_lock);
					pSocketHub->recv_callback(pSocketHub, session, pCache + sizeof(px_dword), packet_size, pSocketHub->userptr);
					PX_MutexUnlock(&pInstance->rs_lock);
				}

				//move cache
				PX_memcpy(pCache, pCache + sizeof(px_dword) + packet_size, pInstance->recv_cache_offset - sizeof(px_dword) - packet_size);
				pInstance->recv_cache_offset -= packet_size + sizeof(px_dword);
			}
			else
			{
				break;
			}
		}
		
	}
}

PX_WEBSOCKET_DISCONNECT_CALLBACK_FUNCTION(PX_WebsocketServerDisconnectCallback)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)ptr;
	if (pInstance->pSocketHub->disconnect_callback)
	{
		px_byte session[16] = { 0 };
		px_dword arrayIndex;
		session[0] = 2;//websocket
		arrayIndex = PX_ARRAY_INDEX(pInstance->pSocketHub->websocket_instance, pInstance);
		PX_memcpy(session + 1, &arrayIndex, sizeof(arrayIndex));
		printf("Disconnect websocket session:%d\n", PX_crc32(session, 16));
		PX_MutexLock(&pInstance->rs_lock);
		pInstance->pSocketHub->disconnect_callback(pInstance->pSocketHub, session, pInstance->pSocketHub->userptr);
		PX_MutexUnlock(&pInstance->rs_lock);
	}

}

px_bool PX_SocketHubWriteSendCache(PX_SocketHub* pServerHub, const  px_byte Session[16], const px_byte* data, px_int send_data_size)
{
	px_dword index;
	PX_memcpy(&index, Session + 1, sizeof(index));
	if (index >= PX_COUNTOF(pServerHub->websocket_instance))
	{
		return 0;
	}
	switch (Session[0])
	{
	case 1:
	{
		PX_TCP_Instance* pInstance = &pServerHub->tcp_instance[index];
		if (PX_SocketHubCircularBufferGetSpaceSize(pInstance->cache_size, pInstance->send_cache_wcursor, pInstance->send_cache_rcursor) >= send_data_size)
		{
			return PX_SocketHubWriteToCircularBuffer(pInstance->send_cache, pInstance->cache_size, &pInstance->send_cache_wcursor, pInstance->send_cache_rcursor, data, send_data_size);
		}
		return PX_FALSE;
	}
	break;
	case 2:
	{
		PX_Websocket_Instance* pInstance = &pServerHub->websocket_instance[index];
		if (PX_SocketHubCircularBufferGetSpaceSize(pInstance->cache_size, pInstance->send_cache_wcursor, pInstance->send_cache_rcursor) >= send_data_size)
		{
			return PX_SocketHubWriteToCircularBuffer(pInstance->send_cache, pInstance->cache_size, &pInstance->send_cache_wcursor, pInstance->send_cache_rcursor, data, send_data_size);
		}
		return PX_FALSE;
	}
	break;
	default:
		return PX_FALSE;
		break;
	}
	return PX_FALSE;
}

px_bool PX_SocketHubSend(PX_SocketHub* pServerHub, const  px_byte Session[16],const px_byte* data, px_int send_data_size)
{
	px_dword size = (px_dword)send_data_size;
	if (PX_SocketHubWriteSendCache(pServerHub,Session,(px_byte *)&size, sizeof(size)))
	{
		if (PX_SocketHubWriteSendCache(pServerHub, Session, data, send_data_size))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}


px_void PX_WebsocketServer_linkerthread_send(px_void* ptr)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)ptr;
	PX_SocketHub* pSocketHUb = (PX_SocketHub*)pInstance->pSocketHub;
	px_dword lastupdatetime = PX_TimeGetTime();
	px_byte session[16] = { 0 };
	px_dword arrayIndex;
	session[0] = 2;//websocket
	arrayIndex = PX_ARRAY_INDEX(pSocketHUb->websocket_instance, pInstance);
	PX_memcpy(session + 1, &arrayIndex, sizeof(arrayIndex));

	while (PX_TRUE)
	{
		px_dword now = PX_TimeGetTime();
		px_dword elapsed = now - lastupdatetime;
		lastupdatetime = now;
		px_int send_block_size = sizeof(pInstance->websocket_Instance.send_cache) >> 1;
		px_int ready_send_data_size;
		

		if (pInstance->websocket_Instance.state == PX_WEBSOCKET_STATE_CLOSE)
		{
			break;
		}
		if (pInstance->websocket_Instance.state == PX_WEBSOCKET_STATE_ERROR)
		{
			break;
		}

		if (pInstance->websocket_Instance.state == PX_WEBSOCKET_STATE_CONNECTING)
		{
			if (pInstance->pSocketHub->send_callback)
			{
				PX_MutexLock(&pInstance->rs_lock);
				pInstance->pSocketHub->send_callback(pInstance->pSocketHub, session, elapsed, pInstance->pSocketHub->userptr);
				PX_MutexUnlock(&pInstance->rs_lock);
			}
		}
		
		//move send cache to websocket cache
		
		do
		{
			ready_send_data_size = pInstance->send_cache_wcursor >= pInstance->send_cache_rcursor ? pInstance->send_cache_wcursor - pInstance->send_cache_rcursor : pInstance->cache_size - pInstance->send_cache_rcursor;
			if (ready_send_data_size < send_block_size)
			{
				send_block_size = ready_send_data_size;
			}
			if (send_block_size)
			{
				if (!PX_WebSocketWrite(&pInstance->websocket_Instance, pInstance->send_cache+ pInstance->send_cache_rcursor, send_block_size))
				{
					break;
				}
				pInstance->send_cache_rcursor = (pInstance->send_cache_rcursor + send_block_size) % pInstance->cache_size;
			}
		} while (ready_send_data_size);

		PX_WebSocketUpdate_Send(&pInstance->websocket_Instance);
	}
	printf("websocket send thread terminated:%d\n", PX_crc32(session, 16));
}

px_void PX_WebsocketServer_linkerthread_recv(px_void* ptr)
{
	PX_Websocket_Instance* pInstance = (PX_Websocket_Instance*)ptr;
	PX_SocketHub* pSocketHUb = (PX_SocketHub*)pInstance->pSocketHub;
	px_dword lastupdatetime = PX_TimeGetTime();

	if (!PX_WebSocketListen(&pInstance->websocket_Instance))
	{
		PX_WebSocketFree(&pInstance->websocket_Instance);
		return;
	}
	
	pInstance->recv_cache_offset = 0;
	pInstance->send_cache_wcursor = 0;
	pInstance->send_cache_rcursor = 0;

	
	printf("start websocket recv/send thread session:%d\n", PX_crc32(pInstance->recv_cache, 16));
	PX_ThreadCreate(&pInstance->send_thread, PX_WebsocketServer_linkerthread_send, pInstance);


	while (PX_TRUE)
	{
		PX_WEBSOCKET_HANDER_RETURN ret;
		if (pInstance->websocket_Instance.state == PX_WEBSOCKET_STATE_CLOSE)
		{
			break;
		}
		if (pInstance->websocket_Instance.state == PX_WEBSOCKET_STATE_ERROR)
		{
			break;
		}
		
		ret=	PX_WebSocketUpdate_Recv(&pInstance->websocket_Instance);
		switch (ret)
		{
			case PX_WEBSOCKET_HANDER_RETURN_CONTINUE:
			{
				break;
			}
			case PX_WEBSOCKET_HANDER_RETURN_PROTOCAL_ERROR:
			{
				printf("Disconnect session:%d by protocol error!\n", PX_crc32(pInstance->recv_cache, 16));
				break;
			}
			case PX_WEBSOCKET_HANDER_RETURN_TARGETCLOSE_ERROR:
			{
				printf("Disconnect session:%d by socket close error!\n", PX_crc32(pInstance->recv_cache, 16));
				break;
			}
			case PX_WEBSOCKET_HANDER_RETURN_CACHE_ERROR:
			{
				printf("Disconnect session:%d by cache error!\n", PX_crc32(pInstance->recv_cache, 16));
				break;
			}
			case PX_WEBSOCKET_HANDER_RETURN_DONE:
			{
				break;
			}
			default:
			{
				break;
			}
		}
	}
	while (pInstance->pSocketHub->websocket_connect_lock);
	while (pInstance->send_thread.isRun);
	pInstance->pSocketHub->websocket_connect_lock = PX_TRUE;
	pInstance->socket = 0;
	pInstance->recv_cache_offset = 0;
	pInstance->send_cache_wcursor = 0;
	pInstance->send_cache_rcursor = 0;
	pInstance->pSocketHub->websocket_connect_lock = PX_FALSE;
	pInstance->recv_thread.isRun = PX_FALSE;
	pInstance->send_thread.isRun = PX_FALSE;
	printf("session thread terminated:%d \n", PX_crc32(pInstance->recv_cache, 16));
}


px_void PX_Websocket_thread(px_void* ptr)
{
	PX_SocketHub* pSocketHub = (PX_SocketHub*)ptr;
	px_uint new_socket;
	PX_TCP_ADDR from_addr;
	while (pSocketHub->websocket_enable)
	{
		px_int i = 0;
		if (!PX_TCPAccept(&pSocketHub->websocket_tcp, &new_socket, &from_addr))
			break;
		//new connection
		while (pSocketHub->websocket_connect_lock);
		pSocketHub->websocket_connect_lock = PX_TRUE;
		for (i = 0; i < PX_COUNTOF(pSocketHub->websocket_instance); i++)
		{
			if (!pSocketHub->websocket_instance[i].socket)
			{
				break;
			}
		}
		if (i != PX_COUNTOF(pSocketHub->websocket_instance))
		{
			pSocketHub->websocket_instance[i].socket = new_socket;
			pSocketHub->websocket_instance[i].recv_cache_offset = 0;
			pSocketHub->websocket_instance[i].send_cache_wcursor = 0;
			pSocketHub->websocket_instance[i].recv_thread.isRun = PX_FALSE;
			pSocketHub->websocket_instance[i].send_thread.isRun = PX_FALSE;
			PX_MutexInitialize(&pSocketHub->websocket_instance[i].rs_lock);
			PX_WebSocketInitialize(&pSocketHub->websocket_instance[i].websocket_Instance, &pSocketHub->websocket_instance[i].linker, PX_WebsocketServerConnectCallback, PX_WebsocketServerRecvCallback, PX_WebsocketServerDisconnectCallback, &pSocketHub->websocket_instance[i]);
			printf("new websocket port connection %d begin TCP send/recv thread for websocket!\n", i);
			PX_ThreadCreate(&pSocketHub->websocket_instance[i].recv_thread, PX_WebsocketServer_linkerthread_recv, &pSocketHub->websocket_instance[i]);
		}
		else
		{
			PX_TCPSocketFree(new_socket);
		}
		pSocketHub->websocket_connect_lock = PX_FALSE;
	}
	return;
}



px_bool PX_SocketHubInitialize(PX_SocketHub* pSocketHub, \
	px_word tcp_port, px_word websocket_port, \
	px_dword  cache_size, \
	PX_SocketHub_OnConnectCallback connect_callback, \
	PX_SocketHub_OnSendCallback send_callback, \
	PX_SocketHub_ReceiveCallback recv_callback, \
	PX_SocketHub_OnDisconnectCallback disconnect_callback, \
	px_void* ptr)
{
	px_int i;
	PX_memset(pSocketHub, 0, sizeof(PX_SocketHub));

	pSocketHub->recv_callback=recv_callback;
	pSocketHub->send_callback=send_callback;
	pSocketHub->connect_callback=connect_callback;
	pSocketHub->disconnect_callback=disconnect_callback;
	pSocketHub->userptr=ptr;
	pSocketHub->cache_size=cache_size;


	if (tcp_port != 0)
	{
		pSocketHub->tcp_enable = PX_TRUE;
		if (!PX_TCPInitialize(&pSocketHub->tcp, PX_TCP_IP_TYPE_IPV4))
		{
			return PX_FALSE;
		}
		if (!PX_TCPListen(&pSocketHub->tcp, PX_htons(tcp_port)))
		{
			PX_TCPFree(&pSocketHub->tcp);
			return PX_FALSE;
		}

		for(i=0;i<PX_SOCKETHUB_MAX_CONNECTION;i++)
		{
			//malloc tcp/websocket cache
			pSocketHub->tcp_instance[i].pSocketHub= pSocketHub;
			pSocketHub->tcp_instance[i].cache_size=cache_size;
			pSocketHub->tcp_instance[i].recv_cache=(px_byte*)malloc(cache_size);
			if(pSocketHub->tcp_instance[i].recv_cache==PX_NULL)
			{
				PX_ASSERT();
				return PX_FALSE;
			}
			pSocketHub->tcp_instance[i].recv_cache_offset=0;
			pSocketHub->tcp_instance[i].recv_cache_lock=PX_FALSE;
			pSocketHub->tcp_instance[i].send_cache=(px_byte*)malloc(cache_size);
			if(pSocketHub->tcp_instance[i].send_cache==PX_NULL)
			{
				PX_ASSERT();
				return PX_FALSE;
			}
			pSocketHub->tcp_instance[i].send_cache_wcursor=0;
			pSocketHub->tcp_instance[i].send_cache_rcursor=0;
			pSocketHub->tcp_instance[i].socket=0;
		}

		
		PX_ThreadCreate(&pSocketHub->tcp_thread, PX_SocketHubThread_TCPHander, pSocketHub);
	}
    if (websocket_port != 0)
	{
		pSocketHub->websocket_enable = PX_TRUE;
		if (!PX_TCPInitialize(&pSocketHub->websocket_tcp, PX_TCP_IP_TYPE_IPV4))
		{
			return PX_FALSE;
		}
		if (!PX_TCPListen(&pSocketHub->websocket_tcp, PX_htons(websocket_port)))
		{
			PX_TCPFree(&pSocketHub->websocket_tcp);
			return PX_FALSE;
		}
		for(i=0;i<PX_SOCKETHUB_MAX_CONNECTION;i++)
		{
			//malloc tcp/websocket cache
			pSocketHub->websocket_instance[i].pSocketHub= pSocketHub;
			pSocketHub->websocket_instance[i].cache_size=cache_size;
			pSocketHub->websocket_instance[i].recv_cache=(px_byte*)malloc(cache_size);
			if(pSocketHub->websocket_instance[i].recv_cache==PX_NULL)
			{
				PX_ASSERT();
				return PX_FALSE;
			}
			pSocketHub->websocket_instance[i].recv_cache_offset=0;
			pSocketHub->websocket_instance[i].send_cache=(px_byte*)malloc(cache_size);
			if(pSocketHub->websocket_instance[i].send_cache==PX_NULL)
			{
				PX_ASSERT();
				return PX_FALSE;
			}
			pSocketHub->websocket_instance[i].send_cache_wcursor=0;
			pSocketHub->websocket_instance[i].send_cache_rcursor=0;
			pSocketHub->websocket_instance[i].socket=0;
			PX_LinkerInitialize(&pSocketHub->websocket_instance[i].linker, 0, PX_WebsocketServerLinkerRead_func, PX_WebsocketServerLinkerWrite_func, PX_WebsocketServerLinkerClose_func, &pSocketHub->websocket_instance[i]);
		}

		pSocketHub->connect_callback = connect_callback;
		pSocketHub->recv_callback = recv_callback;
		pSocketHub->disconnect_callback = disconnect_callback;
		pSocketHub->userptr = ptr;
		PX_ThreadCreate(&pSocketHub->websocket_accept_thread, PX_Websocket_thread, pSocketHub);
	}

	return PX_TRUE;
}

px_void PX_SocketHubFree(PX_SocketHub* pSocketHub)
{
	pSocketHub->tcp_enable=PX_FALSE;
	pSocketHub->websocket_enable=PX_FALSE;

}

px_void PX_SocketHubCloseSession(PX_SocketHub* pServerHub, const  px_byte Session[16])
{
	px_dword index;
	PX_memcpy(&index, Session + 1, sizeof(index));
	if(index >= PX_COUNTOF(pServerHub->websocket_instance))
	{
		return;
	}
	switch (Session[0])
	{
	case 1:
	{
		PX_TCPSocketFree(pServerHub->tcp_instance[index].socket);
	}
	break;
	case 2:
	{
		PX_WebSocketFree(&pServerHub->websocket_instance[index].websocket_Instance);
	}
	break;
	default:
		break;
	}

}
px_void PX_SocketHubSetUserPtr(PX_SocketHub* pServerHub, const  px_byte Session[16], px_void* ptr)
{
	px_dword index;
	PX_memcpy(&index, Session + 1, sizeof(index));
	if(index >= PX_COUNTOF(pServerHub->websocket_instance))
	{
		return;
	}
	switch (Session[0])
	{
	case 1:
	{
		pServerHub->tcp_instance[index].userptr=ptr;
	}
	break;
	case 2:
	{
		pServerHub->websocket_instance[index].userptr=ptr;
	}
	break;
	default:
		break;
	}

}
px_void* PX_SocketHubGetUserPtr(PX_SocketHub* pServerHub, const  px_byte Session[16])
{
	px_dword index;
	PX_memcpy(&index, Session + 1, sizeof(index));
	if(index >= PX_COUNTOF(pServerHub->websocket_instance))
	{
		return PX_NULL;
	}
	switch (Session[0])
	{
	case 1:
	{
		return pServerHub->tcp_instance[index].userptr;
	}
	break;
	case 2:
	{
		return pServerHub->websocket_instance[index].userptr;
	}
	break;
	default:
		break;
	}
	return PX_NULL;
}

px_int  PX_SocketHubGetClientSendCacheSize(PX_SocketHub* pServerHub, const  px_byte Session[16])
{
	px_dword index;
	PX_memcpy(&index, Session + 1, sizeof(index));
	if(index >= PX_COUNTOF(pServerHub->websocket_instance))
	{
		return 0;
	}
	switch (Session[0])
	{
	case 1:
	{
		volatile px_int wcursor=pServerHub->tcp_instance[index].send_cache_wcursor;
		volatile px_int rcursor=pServerHub->tcp_instance[index].send_cache_rcursor;
		px_int cache_size=pServerHub->tcp_instance[index].cache_size;
		return PX_SocketHubCircularBufferGetReadySize(cache_size, wcursor, rcursor);
	}
	break;
	case 2:
	{
		px_int wcursor=pServerHub->websocket_instance[index].send_cache_wcursor;
		px_int rcursor=pServerHub->websocket_instance[index].send_cache_rcursor;
		px_int cache_size=pServerHub->websocket_instance[index].cache_size;
		return PX_SocketHubCircularBufferGetReadySize(cache_size, wcursor, rcursor);
	}
	break;
	default:
		break;
	}
	return 0;

}
px_int  PX_SocketHubGetClientRecvCacheSize(PX_SocketHub* pServerHub, const  px_byte Session[16])
{
	px_dword index;
	PX_memcpy(&index, Session + 1, sizeof(index));
	if(index >= PX_COUNTOF(pServerHub->websocket_instance))
	{
		return 0;
	}
	switch (Session[0])
	{
	case 1:
	{
		return pServerHub->tcp_instance[index].recv_cache_offset;
	}
	break;
	case 2:
	{
		return pServerHub->websocket_instance[index].recv_cache_offset;
	}
	break;
	default:
		break;
	}
	return 0;
}

px_int PX_SocketHubGetCurrentOnlineCount(PX_SocketHub* pServerHub)
{
	px_int i;
	px_int count = 0;
	for (i = 0; i < PX_SOCKETHUB_MAX_CONNECTION; i++)
	{
		if (pServerHub->tcp_instance[i].socket || pServerHub->websocket_instance[i].socket)
		{
			count++;
		}
	}
	return count;
}