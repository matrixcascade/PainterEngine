#include "platform/modules/px_httpserver.h"
px_bool PX_HttpServer_Response404(PX_HttpServer_Request* pdesc)
{
	px_byte header[64] = { 0 };
	px_memory response_data;
	px_int offset=0;
	PX_MemoryInitialize(&pdesc->mp, &response_data);
	PX_sprintf1(header, sizeof(header), "HTTP/1.1 404 Not Found\r\nContent-Length:%1\r\n\r\n", PX_STRINGFORMAT_INT(0));
	PX_MemoryCat(&response_data, header, PX_strlen(header));
	while (PX_TRUE)
	{
		px_int s;
		px_int ssize = response_data.usedsize - offset;
		if (ssize == 0)
		{
			break;
		}
		if (ssize > 1024)
		{
			ssize = 1024;
		}
		s = PX_TCPSocketSend(pdesc->socket, response_data.buffer + offset, ssize);
		if (s <= 0)
		{
			break;
		}
		offset += s;
	}
	PX_MemoryFree(&response_data);
	return PX_TRUE;
}


px_bool PX_HttpServer_ResponseData(PX_HttpServer_Request* pdesc, px_byte* data, px_int size)
{
	px_byte header[64] = { 0 };
	px_int offset = 0;
	px_memory response_data;
	PX_MemoryInitialize( &pdesc->mp, &response_data);
	PX_sprintf1(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Length:%1\r\n\r\n", PX_STRINGFORMAT_INT(size));
	PX_MemoryCat(&response_data, header, PX_strlen(header));
	PX_MemoryCat(&response_data, data, size);
	while (PX_TRUE)
	{
		px_int s;
		px_int ssize = response_data.usedsize - offset;
		if (ssize==0)
		{
			break;
		}
		if (ssize>1024)
		{
			ssize = 1024;
		}
		s =  PX_TCPSocketSend(pdesc->socket, response_data.buffer + offset, ssize);
		if (s <= 0)
		{
			break;
		}
		offset += s;
	}
	PX_MemoryFree(&response_data);
	return PX_TRUE;
}

px_bool PX_HttpServer_ResponseFile(PX_HttpServer_Request* prequest, const px_char path[])
{
	const px_char* loadpath;
	px_int i;
	PX_HttpServer* pdesc = prequest->pserver;
	if (PX_strstr(path,"index.htm")|| PX_strstr(path, "index.html")|| PX_strstr(path, "default.html"))
	{
		loadpath = "/";
	}
	else
	{
		loadpath = path;
	}
	for (i = 0; i < pdesc->abi_data.size; i++)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pdesc->abi_data, i);
		const px_char* pfilename = PX_AbiGet_string(pabi, "filename");
		if (!pfilename)
		{
			continue;
		}
		if (PX_strequ(pfilename, loadpath))
		{
			px_byte* pdata;
			px_int size;
			pdata=PX_AbiGet_data(pabi, "data", &size);
			if (pdata)
			{
				return PX_HttpServer_ResponseData(prequest, pdata, size);
			}
		}
	}
	return PX_HttpServer_Response404(prequest);
}

px_void PX_HttpServer_RequestThread(px_void* ptr)
{
	PX_HttpServer_Request* pdesc = (PX_HttpServer_Request*)ptr;
	px_int search_index=0;
	px_char path[256] = { 0 };

	while (PX_TRUE)
	{
		px_int size;
		px_byte cache[1024] = {0};
		size = PX_TCPSocketReceived(pdesc->socket, cache, sizeof(cache), 3000);
		if (size > 0)
		{
			if (!PX_MemoryCat(&pdesc->request_data, cache, size))
				goto _END;

			if (PX_HttpContentIsComplete(pdesc->request_data.buffer, pdesc->request_data.usedsize))
			{
				break;
			}
		}
		else
		{
			goto _END;
		}
	}
	if (!PX_HttpCheckContent(pdesc->request_data.buffer))
	{
		goto _END;
	}

	if (!PX_HttpGetRequestPath(pdesc->request_data.buffer, pdesc->request_data.usedsize, path, sizeof(path)))
	{
		goto _END;
	}

	if (!PX_HttpServer_ResponseFile(pdesc, path))
	{
		goto _END;
	}

_END:
	PX_TCPSocketFree(pdesc->socket);
	pdesc->activating = PX_FALSE;
}

px_void PX_HttpServer_ListenThread(px_void* ptr)
{
	PX_HttpServer* pServer = (PX_HttpServer*)ptr;
	while (PX_TRUE)
	{
		px_int socket;
		PX_TCP_ADDR addr;
		if (PX_TCPAccept(&pServer->tcp, &socket, &addr))
		{
			px_int i;
			for (i = 0; i < PX_COUNTOF(pServer->request); i++)
			{
				if (!pServer->request[i].activating) 
				{
					PX_HttpServer_Request* pdesc = &pServer->request[i];
					PX_memset(pdesc, 0, sizeof(PX_HttpServer_Request));
					pServer->request[i].activating = PX_TRUE;
					pdesc->pserver = pServer;
					pdesc->socket = socket;
					pdesc->mp = MP_Create(pdesc->cache, sizeof(pdesc->cache));
					PX_MemoryInitialize(&pdesc->mp, &pdesc->request_data);
					PX_ThreadCreate(&pdesc->thread, PX_HttpServer_RequestThread, pdesc);
					break;
				}
			}
			
		}
	}
	
}

px_bool PX_HttpServer_Initialize(PX_HttpServer* httpserver, px_int port)
{
	if (!PX_TCPInitialize(&httpserver->tcp, PX_TCP_IP_TYPE_IPV4))
	{
		return PX_FALSE;
	}
	if (!PX_TCPListen(&httpserver->tcp, PX_htons(port)))
	{
		return PX_FALSE;
	}
	httpserver->mp = MP_Create(httpserver->cache, sizeof(httpserver->cache));
	PX_VectorInitialize(&httpserver->mp ,&httpserver->abi_data, sizeof(px_abi), 16);
	PX_ThreadCreate(&httpserver->thread, PX_HttpServer_ListenThread, httpserver);
	return PX_TRUE;
	
}


px_bool PX_HttpServer_AddFile(PX_HttpServer* httpserver, const px_char path[],px_byte *data,px_int datasize)
{
	px_abi abi;
	PX_AbiCreateDynamicWriter(&abi, &httpserver->mp);
	if(!PX_AbiSet_string(&abi, "filename", path))
	{
		PX_AbiFree(&abi);
		return PX_FALSE;
	}
	if (!PX_AbiSet_data(&abi, "data", data, datasize))
	{
		PX_AbiFree(&abi);
		return PX_FALSE;
	}
	if(!PX_VectorPushback(&httpserver->abi_data, &abi))
	{
		PX_AbiFree(&abi);
		return PX_FALSE;
	}
	return PX_TRUE;

}


px_void PX_HttpServer_Free(PX_TCP* tcp)
{
	PX_TCPFree(tcp);
}


