#include "PX_WebSocket.h"

px_void PX_WebSocketInitialize(px_memorypool* mp, PX_WebSocket* pInstance, PX_Linker* pLinker)
{
	PX_memset(pInstance, 0, sizeof(PX_WebSocket));
	pInstance->mp = mp;
	PX_MemoryInitialize(mp, &pInstance->cache);
	pInstance->plinker = pLinker;
	pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
}

px_void PX_WebSocketUpdate(PX_WebSocket* pInstance)
{
	px_byte readBuffer[64];
	px_int readsize;
	while ((readsize=PX_LinkerRead(pInstance->plinker,readBuffer,sizeof(readBuffer)))>0)
	{
		if(!PX_MemoryCat(&pInstance->cache, readBuffer, readsize))
			pInstance->state = PX_WEBSOCKET_STATE_CLOSE;

		if(pInstance->cache.usedsize> PX_WEBSOCKET_CACHE_MAX_SIZE)
			pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
	}
	if (readsize<0)
	{
		pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
	}
}

px_int PX_WebSocketRead(PX_WebSocket* pInstance, px_byte* data, px_int size)
{

	switch (pInstance->state)
	{
	case PX_WEBSOCKET_STATE_OPEN:
	{
		PX_WebSocketUpdate(pInstance);
		if (PX_HttpCheckContent((const px_char*)pInstance->cache.buffer))
		{
			px_int packetSize = PX_HttpGetPacketSize((const px_char*)pInstance->cache.buffer);
			if (packetSize)
			{
				px_char payload[128] = { 0 };
				if (PX_HttpGetContent((const px_char*)pInstance->cache.buffer, "Sec-WebSocket-Key", payload, sizeof(payload)))
				{

					PX_MemoryRemove(&pInstance->cache, 0, packetSize - 1);
				}
				return PX_FALSE;
			}
		}
	}
	break;
	case PX_WEBSOCKET_STATE_LISTEN:
	{
		PX_WebSocketUpdate(pInstance);
		if (PX_HttpCheckContent((const px_char *)pInstance->cache.buffer))
		{
			px_int packetSize = PX_HttpGetPacketSize((const px_char*)pInstance->cache.buffer);
			if (packetSize)
			{
				px_char payload[128] = { 0 };
				if (PX_HttpGetContent((const px_char*)pInstance->cache.buffer,"Sec-WebSocket-Key",payload,sizeof(payload)))
				{
					px_char response[280] = "HTTP/1.1 101 Switching Protocols\r\nConnection:Upgrade\r\nServer : PainterEngine websocket server\r\nUpgrade : WebSocket\r\nAccess - Control - Allow - Credentials : true\r\nAccess - Control - Allow - Headers : content - type\r\nSec-WebSocket-Accept:";
					PX_SHA1_HASH hash;
					PX_strcat_s(payload, sizeof(payload),"258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
					PX_Sha1Calculate(payload, PX_strlen(payload), &hash);
					PX_Base64Encode(hash.bytes, sizeof(hash.bytes), payload);
					PX_strcat_s(response, sizeof(response), payload);
					PX_strcat_s(response, sizeof(response), "\r\n\r\n");
						
					if (PX_LinkerWrite(pInstance->plinker, response, PX_strlen(response)))
					{
						PX_MemoryRemove(&pInstance->cache, 0, packetSize-1);
						pInstance->state = PX_WEBSOCKET_STATE_CONNECTING;
					}
					else
						pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
				}
				return PX_FALSE;
			}
		}
	}
	break;
	case PX_WEBSOCKET_STATE_CONNECTING:
	{
		px_int32 payloadLen=0;
		px_int maskkey_size = 0;
		px_dword mask;
		px_int headersize = sizeof(PX_WebSocketMessageHeader);
		PX_WebSocketUpdate(pInstance);

		while (PX_TRUE)
		{
			if (pInstance->cache.usedsize >= headersize)
			{
				PX_WebSocketMessageHeader* pheader = (PX_WebSocketMessageHeader*)pInstance->cache.buffer;


				if (pheader->payloadLength == 126)
				{
					if (pInstance->cache.usedsize >= 4)
					{
						payloadLen = *((px_uint16*)(pInstance->cache.buffer + 2));
						if (pheader->mask)
						{
							mask = (*((px_dword*)(pInstance->cache.buffer + 2 + 2)));
							maskkey_size = 4;
						}
						payloadLen = PX_htons(payloadLen);
						headersize += 2;
					}
				}
				else if (pheader->payloadLength == 127)
				{
					if (pInstance->cache.usedsize >= 4)
					{
						payloadLen = (*((px_int32*)(pInstance->cache.buffer + 4)));
						if (pheader->mask)
						{
							mask = (*((px_dword*)(pInstance->cache.buffer + 2 + 8)));
							maskkey_size = 4;
						}
						payloadLen = PX_htonl(payloadLen);
						headersize += 8;
					}
				}
				else
				{
					if (pheader->mask)
					{
						mask = (*((px_dword*)(pInstance->cache.buffer + 2)));
						maskkey_size = 4;
					}
					payloadLen = pheader->payloadLength;
				}


				if (pInstance->cache.usedsize >= payloadLen + headersize + maskkey_size)
				{
					switch (pheader->opcode)
					{
					case 0:
						break;
					case 1:
					case 2:
					{
						if (size >= payloadLen)
						{
							px_int i;
							px_byte* pmask = (px_byte*)&mask;
							PX_memcpy(data, pInstance->cache.buffer + headersize + maskkey_size, payloadLen);
							for (i = 0; i < payloadLen; i++)
							{
								data[i] ^= pmask[i % 4];
							}
							PX_MemoryRemove(&pInstance->cache, 0, headersize + maskkey_size + payloadLen - 1);
							return payloadLen;
						}
						else
						{
							return 0;
						}
					}
					break;
					case 8:
						pInstance->state = PX_WEBSOCKET_STATE_CLOSE;
						break;
					case 9:
						pheader->opcode = 10;
						PX_LinkerWrite(pInstance->plinker, pheader, sizeof(PX_WebSocketMessageHeader));
						break;
					case 10:
						break;
					default:
						break;
					}
					PX_MemoryRemove(&pInstance->cache, 0, payloadLen + (px_int)sizeof(PX_WebSocketMessageHeader) + maskkey_size - 1);
				}
			}
			else
			{
				break;
			}
		}
		
	}
	break;
	case PX_WEBSOCKET_STATE_CLOSE:
	default:
		break;
	}
	return 0;
}

px_bool PX_WebSocketConnet(PX_WebSocket* pInstance)
{
	if (pInstance->state==PX_WEBSOCKET_STATE_CLOSE)
	{
		px_char content[1024] = "GET / HTTP/1.1\r\nHost: painterengine.com\r\nConnection: Upgrade\r\nUpgrade: websocket\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: LNrF70ZAxkZ1qeubkHN7fQ==\r\nSec-WebSocket-Extensions: permessage-deflate; client_max_window_bits\r\n\r\n";
		if (PX_LinkerWrite(pInstance->plinker, content, PX_strlen(content)))
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

px_bool PX_WebSocketClientWrite(PX_WebSocket* pInstance,const px_byte* data, px_int size)
{
	px_byte buffer[128];
	if (pInstance->state!=PX_WEBSOCKET_STATE_CONNECTING)
	{
		return PX_FALSE;
	}
	if (size>65535)
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
		
		PX_LinkerWrite(pInstance->plinker, &header, sizeof(header));
		for (i = 0; i < size; i++)
		{
			buffer[i % sizeof(buffer)] = data[i] ^ 0;
			if (i && (i % sizeof(buffer) == 0))
			{
				PX_LinkerWrite(pInstance->plinker, buffer, sizeof(buffer));
			}
		}
		PX_LinkerWrite(pInstance->plinker, buffer, size % sizeof(buffer));

	}
	else if (size >126)
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
		PX_LinkerWrite(pInstance->plinker, &header, sizeof(header));
		for (i=0;i<size;i++)
		{
			buffer[i % sizeof(buffer)] = data[i] ^ 0;
			if (i&&(i% sizeof(buffer)==0))
			{
				PX_LinkerWrite(pInstance->plinker, buffer, sizeof(buffer));
			}
		}
		PX_LinkerWrite(pInstance->plinker, buffer, size% sizeof(buffer));

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

		PX_LinkerWrite(pInstance->plinker, &header, sizeof(header));
		for (i = 0; i < size; i++)
		{
			buffer[i % sizeof(buffer)] = data[i] ^ 0;
			if (i && (i % sizeof(buffer) == 0))
			{
				PX_LinkerWrite(pInstance->plinker, buffer, sizeof(buffer));
			}
		}
		PX_LinkerWrite(pInstance->plinker, buffer, size % sizeof(buffer));

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

		PX_LinkerWrite(pInstance->plinker, &header, sizeof(header));
		PX_LinkerWrite(pInstance->plinker, (px_void *)data, size);

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
		PX_LinkerWrite(pInstance->plinker, &header, sizeof(header));
		PX_LinkerWrite(pInstance->plinker, (px_void*)data, size);

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
		PX_LinkerWrite(pInstance->plinker, &header, sizeof(header));
		PX_LinkerWrite(pInstance->plinker, (px_void*)data, size);

	}
	return PX_TRUE;
}

px_bool PX_WebSocketWrite(PX_WebSocket* pInstance, const px_byte* data, px_int size)
{
	if (pInstance->isServer)
	{
		PX_WebSocketServerWrite(pInstance, data, size);
	}
	else
	{
		PX_WebSocketClientWrite(pInstance, data, size);
	}
	return PX_TRUE;
}
px_void PX_WebSocketFree(PX_WebSocket* pInstance)
{
	PX_MemoryFree(&pInstance->cache);
}

