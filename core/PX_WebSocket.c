#include "PX_WebSocket.h"

px_void PX_WebSocketInitialize(PX_WebSocket* pInstance, PX_Linker* pLinker, \
	PX_PX_WebSocket_OnConnectCallback connect_callback, \
	PX_PX_WebSocket_ReceiveCallback recv_callback, \
	PX_PX_WebSocket_OnDisconnectCallback disconnect_callback, \
	px_void* ptr)
{
	PX_memset(pInstance, 0, sizeof(PX_WebSocket));
	
	pInstance->plinker = pLinker;
	pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
	pInstance->recv_cache_offset = 0;
	pInstance->send_cache_wcursor = 0;
	pInstance->connect_callback = connect_callback;
	pInstance->recv_callback = recv_callback;
	pInstance->disconnect_callback = disconnect_callback;
	pInstance->userptr = ptr;
}

static px_bool PX_WebSocketWriteToCircularBuffer(px_byte* buffer, px_int buffersize, PX_ATOMIC px_int* wCursor, PX_ATOMIC px_dword rCursor, px_void* data, px_int datasize)
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
		PX_memcpy(buffer + _WCursor, data, datasize);
		*wCursor = (_WCursor + datasize) % buffersize;
	}
	else
	{
		PX_memcpy(buffer + _WCursor, data, divsize);
		PX_memcpy(buffer, (px_byte*)data + divsize, datasize - divsize);
		*wCursor = datasize - divsize;
	}
	return PX_TRUE;
}


px_bool PX_WebSocketWriteSendCache(PX_WebSocket* pInstance, px_byte* data, px_int size)
{
	return PX_WebSocketWriteToCircularBuffer(pInstance->send_cache, PX_WEBSOCKET_CACHE_MAX_SIZE, &pInstance->send_cache_wcursor, pInstance->send_cache_rcursor, data, size);
}

px_int PX_WebSocketGetSendCacheSpaceSize(PX_WebSocket* pInstance)
{
	return pInstance->send_cache_wcursor >= pInstance->send_cache_rcursor ? PX_WEBSOCKET_CACHE_MAX_SIZE - (pInstance->send_cache_wcursor - pInstance->send_cache_rcursor) - 8 : pInstance->send_cache_rcursor - pInstance->send_cache_wcursor - 8;
}


px_bool PX_WebSocketClientWrite(PX_WebSocket* pInstance, const px_byte* data, px_int size)
{
	px_byte buffer[128];
	if (pInstance->state != PX_WEBSOCKET_STATE_CONNECTING)
	{
		return PX_FALSE;
	}
	if (size > 65535)
	{
		typedef struct
		{
			px_word opcode : 4;
			px_word rsv : 3;
			px_word fin : 1;
			px_word payloadLength : 7;
			px_word mask : 1;
			px_dword dummy;
			px_dword payloadTrueSize;
			px_word masker0;
			px_word masker1;
		}PX_WebSocketHeader;
		px_int i;
		PX_WebSocketHeader header;
		PX_memset(&header, 0, sizeof(header));
		header.fin = 1;
		header.opcode = 2;
		header.payloadLength = 127;
		header.mask = 1;
		header.payloadTrueSize = PX_htonl(size);
		if (PX_WebSocketGetSendCacheSpaceSize(pInstance)< sizeof(header)+ size)
		{
			return PX_FALSE;
		}

		if (!PX_WebSocketWriteSendCache(pInstance, (px_byte*)&header, sizeof(header)))
		{
			return PX_FALSE;
		}
		for (i = 0; i < size; i++)
		{
			buffer[i % sizeof(buffer)] = data[i] ^ 0;
			if (i && (i % sizeof(buffer) == 0))
			{
				if (!PX_WebSocketWriteSendCache(pInstance, buffer, sizeof(buffer)))
				{
					return PX_FALSE;
				}
			}
		}
		if (!PX_WebSocketWriteSendCache(pInstance, buffer, size % sizeof(buffer)))
		{
			return PX_FALSE;
		}

	}
	else if (size > 126)
	{
		typedef struct
		{
			px_word opcode : 4;
			px_word rsv : 3;
			px_word fin : 1;
			px_word payloadLength : 7;
			px_word mask : 1;
			px_word payloadTrueSize;
			px_word masker0;
			px_word masker1;
		}PX_WebSocketHeader;
		px_int i;
		PX_WebSocketHeader header;
		PX_memset(&header, 0, sizeof(header));
		header.fin = 1;
		header.opcode = 2;
		header.payloadLength = 126;
		header.payloadTrueSize = PX_htons(size);
		header.mask = 1;
		if (PX_WebSocketGetSendCacheSpaceSize(pInstance) < sizeof(header) + size)
		{
			return PX_FALSE;
		}
		if (!PX_WebSocketWriteSendCache(pInstance, (px_byte*)&header, sizeof(header)))
		{
			return PX_FALSE;
		}
		for (i = 0; i < size; i++)
		{
			buffer[i % sizeof(buffer)] = data[i] ^ 0;
			if (i && (i % sizeof(buffer) == 0))
			{
				if (!PX_WebSocketWriteSendCache(pInstance, buffer, sizeof(buffer)))
				{
					return PX_FALSE;
				}
			}
		}
		if (!PX_WebSocketWriteSendCache(pInstance, buffer, size % sizeof(buffer)))
		{
			return PX_FALSE;
		}

	}
	else
	{
		typedef struct
		{
			px_word opcode : 4;
			px_word rsv : 3;
			px_word fin : 1;
			px_word payloadLength : 7;
			px_word mask : 1;
			px_word masker0;
			px_word masker1;
		}PX_WebSocketHeader;
		px_int i;
		PX_WebSocketHeader header;
		PX_memset(&header, 0, sizeof(header));
		header.fin = 1;
		header.opcode = 2;
		header.payloadLength = size;
		header.mask = 1;
		if (PX_WebSocketGetSendCacheSpaceSize(pInstance) < sizeof(header) + size)
		{
			return PX_FALSE;
		}

		if (!PX_WebSocketWriteSendCache(pInstance, (px_byte*)&header, sizeof(header)))
		{
			return PX_FALSE;
		}
		for (i = 0; i < size; i++)
		{
			buffer[i % sizeof(buffer)] = data[i] ^ 0;
			if (i && (i % sizeof(buffer) == 0))
			{
				if (!PX_WebSocketWriteSendCache(pInstance, buffer, sizeof(buffer)))
				{
					return PX_FALSE;
				}
			}
		}
		if (!PX_WebSocketWriteSendCache(pInstance, buffer, size % sizeof(buffer)))
		{
			return PX_FALSE;
		}

	}
	return PX_TRUE;
}
px_bool PX_WebSocketServerWrite(PX_WebSocket* pInstance, const px_byte* data, px_int size)
{
	if (pInstance->state != PX_WEBSOCKET_STATE_CONNECTING)
	{
		return PX_FALSE;
	}
	if (size > 65535)
	{
		typedef struct
		{
			px_word opcode : 4;
			px_word rsv : 3;
			px_word fin : 1;
			px_word payloadLength : 7;
			px_word mask : 1;
			px_dword dummy;
			px_dword payloadTrueSize;
		}PX_WebSocketHeader;

		PX_WebSocketHeader header;
		PX_memset(&header, 0, sizeof(header));
		header.fin = 1;
		header.opcode = 2;
		header.payloadLength = 127;
		header.payloadTrueSize = PX_htonl(size);
		if (PX_WebSocketGetSendCacheSpaceSize(pInstance) < sizeof(header) + size)
		{
			return PX_FALSE;
		}

		if (!PX_WebSocketWriteSendCache(pInstance, (px_byte*)&header, sizeof(header)))
		{
			return PX_FALSE;
		}
		if (!PX_WebSocketWriteSendCache(pInstance, (px_void*)data, size))
		{
			return PX_FALSE;
		}

	}
	else if (size > 126)
	{
		typedef struct
		{
			px_word opcode : 4;
			px_word rsv : 3;
			px_word fin : 1;
			px_word payloadLength : 7;
			px_word mask : 1;
			px_word payloadTrueSize;
		}PX_WebSocketHeader;

		PX_WebSocketHeader header;
		PX_memset(&header, 0, sizeof(header));
		header.fin = 1;
		header.opcode = 2;
		header.payloadLength = 126;
		header.payloadTrueSize = PX_htons(size);
		if (PX_WebSocketGetSendCacheSpaceSize(pInstance) < sizeof(header) + size)
		{
			return PX_FALSE;
		}
		if (!PX_WebSocketWriteSendCache(pInstance, (px_byte*)&header, sizeof(header)))
		{
			return PX_FALSE;
		}
		if (!PX_WebSocketWriteSendCache(pInstance, (px_void*)data, size))
		{
			return PX_FALSE;
		}

	}
	else
	{
		typedef struct
		{
			px_word opcode : 4;
			px_word rsv : 3;
			px_word fin : 1;
			px_word payloadLength : 7;
			px_word mask : 1;
		}PX_WebSocketHeader;

		PX_WebSocketHeader header;
		PX_memset(&header, 0, sizeof(header));
		header.fin = 1;
		header.opcode = 2;
		header.payloadLength = size;
		if (PX_WebSocketGetSendCacheSpaceSize(pInstance) < sizeof(header) + size)
		{
			return PX_FALSE;
		}
		if (!PX_WebSocketWriteSendCache(pInstance, (px_byte*)&header, sizeof(header)))
		{
			return PX_FALSE;
		}
		if (!PX_WebSocketWriteSendCache(pInstance, (px_void*)data, size))
		{
			return PX_FALSE;
		}

	}
	return PX_TRUE;
}

px_bool PX_WebSocketWrite(PX_WebSocket* pInstance, const px_byte* data, px_int size)
{
	if (pInstance->isServer)
	{
		return PX_WebSocketServerWrite(pInstance, data, size);
	}
	else
	{
		return PX_WebSocketClientWrite(pInstance, data, size);
	}
	return PX_TRUE;
}

px_void PX_WebSocketUpdate_Send(PX_WebSocket* pInstance)
{
	px_int sendblocksize=0;
	do
	{
		px_int sendsize;
		sendblocksize = pInstance->send_cache_wcursor >= pInstance->send_cache_rcursor ? pInstance->send_cache_wcursor - pInstance->send_cache_rcursor : PX_WEBSOCKET_CACHE_MAX_SIZE - pInstance->send_cache_rcursor;

		if (sendblocksize > 0)
		{
			sendsize = PX_LinkerWrite(pInstance->plinker, pInstance->send_cache + pInstance->send_cache_rcursor, sendblocksize);
			pInstance->send_cache_rcursor += sendsize;
			pInstance->send_cache_rcursor %= PX_WEBSOCKET_CACHE_MAX_SIZE;
			if (sendsize<=0)
			{
				return;
			}
		}
	}while(sendblocksize);
}

typedef enum
{
	PX_WEBSOCKET_HANDER_RETURN_CONTINUE,
	PX_WEBSOCKET_HANDER_RETURN_ERROR,
	PX_WEBSOCKET_HANDER_RETURN_DONE,
}PX_WEBSOCKET_HANDER_RETURN;

static PX_WEBSOCKET_HANDER_RETURN PX_WebSocketUpdate_RecvHandleData(PX_WebSocket* pInstance)
{
	switch (pInstance->state)
	{
	case PX_WEBSOCKET_STATE_OPEN:
	{
		if (PX_HttpCheckContent((const px_char*)pInstance->recv_cache))
		{
			px_dword packetSize = PX_HttpGetHttpHeaderSize((const px_char*)pInstance->recv_cache);
			if (packetSize < pInstance->recv_cache_offset)
			{
				px_char payload[128] = { 0 };
				if (PX_HttpGetContent((const px_char*)pInstance->recv_cache, "Sec-WebSocket-Key", payload, sizeof(payload)))
				{
					//move cache
					PX_memcpy(pInstance->recv_cache, pInstance->recv_cache + packetSize, pInstance->recv_cache_offset - packetSize);
					pInstance->recv_cache_offset -= packetSize;
				}
				return PX_WEBSOCKET_HANDER_RETURN_CONTINUE;
			}
		}
	}
	break;
	case PX_WEBSOCKET_STATE_LISTEN:
	{
		if (PX_HttpCheckContent((const px_char*)pInstance->recv_cache))
		{
			px_int packetSize = PX_HttpGetHttpHeaderSize((const px_char*)pInstance->recv_cache);
			if (packetSize)
			{
				px_char payload[128] = { 0 };
				if (PX_HttpGetContent((const px_char*)pInstance->recv_cache, "Sec-WebSocket-Key", payload, sizeof(payload)))
				{
					px_char response[280] = "HTTP/1.1 101 Switching Protocols\r\nConnection:Upgrade\r\n\
Upgrade : websocket\r\n\
Access - Control - Allow - Credentials : true\r\nAccess - Control - Allow - Headers : content - type\r\n\
Sec-WebSocket-Protocol : binary\r\n\
Sec-WebSocket-Accept:";
					PX_SHA1_HASH hash;
					PX_strcat_s(payload, sizeof(payload), "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
					PX_Sha1Calculate(payload, PX_strlen(payload), &hash);
					PX_Base64Encode(hash.bytes, sizeof(hash.bytes), payload);
					PX_strcat_s(response, sizeof(response), payload);
					PX_strcat_s(response, sizeof(response), "\r\n\r\n");

					if (PX_WebSocketWriteSendCache(pInstance, (px_byte*)response, PX_strlen(response)))
					{
						//PX_MemoryRemove(&pInstance->cache, 0, packetSize-1);
						//move cache
						PX_memcpy(pInstance->recv_cache, pInstance->recv_cache + packetSize, pInstance->recv_cache_offset - packetSize);
						pInstance->recv_cache_offset -= packetSize;
						if (pInstance->connect_callback)
						{
							pInstance->connect_callback(pInstance, pInstance->userptr);
						}
						pInstance->state = PX_WEBSOCKET_STATE_CONNECTING;
						
					}
					else
					{
						return PX_WEBSOCKET_HANDER_RETURN_ERROR;
					}
				}
				return PX_WEBSOCKET_HANDER_RETURN_CONTINUE;
			}
			else
			{
				return PX_WEBSOCKET_HANDER_RETURN_ERROR;
			}
		}
	}
	break;
	case PX_WEBSOCKET_STATE_CONNECTING:
	{
		px_int32 payloadLen = 0;
		px_int maskkey_size = 0;
		px_dword mask;
		px_dword headersize = sizeof(PX_WebSocketMessageHeader);

		while (PX_TRUE)
		{
			if (pInstance->recv_cache_offset >= headersize)
			{
				PX_WebSocketMessageHeader* pheader = (PX_WebSocketMessageHeader*)pInstance->recv_cache;

				if (pheader->payloadLength == 126)
				{
					if (pInstance->recv_cache_offset >= 4)
					{
						payloadLen = *((px_uint16*)(pInstance->recv_cache + 2));
						if (pheader->mask)
						{
							mask = (*((px_dword*)(pInstance->recv_cache + 2 + 2)));
							maskkey_size = 4;
						}
						payloadLen = PX_htons(payloadLen);
						headersize += 2;
					}
					else
					{
						return PX_WEBSOCKET_HANDER_RETURN_DONE;
					}
				}
				else if (pheader->payloadLength == 127)
				{
					if (pInstance->recv_cache_offset >= 4)
					{
						payloadLen = (*((px_int32*)(pInstance->recv_cache + 4)));
						if (pheader->mask)
						{
							mask = (*((px_dword*)(pInstance->recv_cache + 2 + 8)));
							maskkey_size = 4;
						}
						payloadLen = PX_htonl(payloadLen);
						headersize += 8;
					}
					else
					{
						return PX_WEBSOCKET_HANDER_RETURN_DONE;
					}
				}
				else
				{
					if (pheader->mask)
					{
						mask = (*((px_dword*)(pInstance->recv_cache + 2)));
						maskkey_size = 4;
					}
					payloadLen = pheader->payloadLength;
				}

				if (payloadLen > sizeof(pInstance->recv_cache))
				{
					return PX_WEBSOCKET_HANDER_RETURN_ERROR;
				}
				else if (sizeof(pInstance->recv_cache) < payloadLen + headersize + maskkey_size)
				{
					return PX_WEBSOCKET_HANDER_RETURN_ERROR;
				}
				else if (pInstance->recv_cache_offset >= payloadLen + headersize + maskkey_size)
				{
					switch (pheader->opcode)
					{
					case 0:
						break;
					case 1:
					case 2://text or binary
					{
						px_byte* pdata;
						px_int i;
						px_byte* pmask = (px_byte*)&mask;
						pdata = pInstance->recv_cache + headersize + maskkey_size;
						for (i = 0; i < payloadLen; i++)
						{
							pdata[i] ^= pmask[i % 4];
						}
						if (pInstance->recv_callback)
						{
							pInstance->recv_callback(pInstance, pdata, payloadLen, pInstance->userptr);
						}

						//move cache
						PX_memcpy(pInstance->recv_cache, pInstance->recv_cache + headersize + maskkey_size + payloadLen, pInstance->recv_cache_offset - headersize - maskkey_size - payloadLen);
						pInstance->recv_cache_offset -= headersize + maskkey_size + payloadLen;
						return PX_WEBSOCKET_HANDER_RETURN_CONTINUE;

					}
					break;
					case 8://close
						return PX_WEBSOCKET_HANDER_RETURN_DONE;
						break;
					case 9://ping
						pheader->opcode = 10;
						if (!PX_WebSocketWriteSendCache(pInstance, (px_byte*)pheader, sizeof(PX_WebSocketMessageHeader)))
						{
							//out of cache
							return PX_WEBSOCKET_HANDER_RETURN_ERROR;
						}
						break;
					case 10://pong
						break;
					default:
						break;
					}

					//move cache
					PX_memcpy(pInstance->recv_cache, pInstance->recv_cache + payloadLen + (px_int)sizeof(PX_WebSocketMessageHeader) + maskkey_size, pInstance->recv_cache_offset - payloadLen - (px_int)sizeof(PX_WebSocketMessageHeader) - maskkey_size);
					pInstance->recv_cache_offset -= payloadLen + (px_int)sizeof(PX_WebSocketMessageHeader) + maskkey_size;
				}
				else
				{
					return PX_WEBSOCKET_HANDER_RETURN_DONE;
				}
			}
			else
			{
				return PX_WEBSOCKET_HANDER_RETURN_DONE;
			}
		}

	}
	break;
	case PX_WEBSOCKET_STATE_CLOSE:
	default:
		return PX_WEBSOCKET_HANDER_RETURN_DONE;
	}
	return PX_WEBSOCKET_HANDER_RETURN_DONE;
}

px_void PX_WebSocketUpdate_Recv(PX_WebSocket* pInstance)
{
	while (PX_TRUE)
	{
		px_int readsize;
		px_int spacesize = sizeof(pInstance->recv_cache) - pInstance->recv_cache_offset;
		px_byte* pDataPtr = pInstance->recv_cache + pInstance->recv_cache_offset;
		if (spacesize == 0)
		{
			break;
		}
		readsize = PX_LinkerRead(pInstance->plinker, pDataPtr, spacesize);
		if (readsize > 0)
		{
			pInstance->recv_cache_offset += readsize;
			do
			{
				PX_WEBSOCKET_HANDER_RETURN ret = PX_WebSocketUpdate_RecvHandleData(pInstance);
				if (ret == PX_WEBSOCKET_HANDER_RETURN_DONE)
				{
					break;
				}
				else if (ret == PX_WEBSOCKET_HANDER_RETURN_ERROR)
				{
					goto DISCONNECT;
				}
				else if (ret == PX_WEBSOCKET_HANDER_RETURN_CONTINUE)
				{
					continue;
				}
				else
				{
					break;
				}
			} while (PX_TRUE);
			
		}
		else if (readsize <= 0)
		{
			goto DISCONNECT;
		}
	}

	return;
DISCONNECT:
	pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
	PX_LinkerClose(pInstance->plinker);
	if (pInstance->disconnect_callback)
	{
		pInstance->disconnect_callback(pInstance, pInstance->userptr);
	}
	
	
	return;
}




px_bool PX_WebSocketConnect(PX_WebSocket* pInstance)
{
	if (pInstance->state==PX_WEBSOCKET_STATE_CLOSE)
	{
		px_char content[] = "GET / HTTP/1.1\r\nHost: painterengine.com\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: LNrF70ZAxkZ1qeubkHN7fQ==\r\nSec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\r\n\r\n";
		if (PX_WebSocketWriteSendCache(pInstance, (px_byte *)content, PX_strlen(content)))
		{
			pInstance->state = PX_WEBSOCKET_STATE_OPEN;
			pInstance->isServer = PX_FALSE;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
	
}

px_bool PX_WebSocketListen(PX_WebSocket* pInstance)
{
	if (pInstance->state == PX_WEBSOCKET_STATE_CLOSE)
	{
			pInstance->state = PX_WEBSOCKET_STATE_LISTEN;
			pInstance->isServer = PX_TRUE;
			return PX_TRUE;
	}
	return PX_FALSE;
}


px_void PX_WebSocketFree(PX_WebSocket* pInstance)
{
	PX_LinkerClose(pInstance->plinker);
	pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
}

px_bool PX_WebSocketIsAlive(PX_WebSocket* pInstance)
{
	return pInstance->state != PX_WEBSOCKET_STATE_CLOSE;
}