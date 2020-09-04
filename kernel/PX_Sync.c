#include "PX_Sync.h"

#ifdef PX_DEBUG_MODE
#include "stdio.h"
#define PX_SYNC_LOG printf
#else
#define PX_SYNC_LOG
#endif

typedef struct  
{
	px_dword param[4];
}PX_Sync_IO_Info;

px_bool PX_SyncFrameServerInit(PX_SyncFrame_Server *sync,px_memorypool *mp,px_dword updateDuration,px_syncframe_server_read read,px_syncframe_server_write write,px_void *user)
{
	sync->mp=mp;
	sync->read=read;
	sync->write=write;
	sync->time=0;
	sync->updateDuration=updateDuration;
	sync->status=PX_SYNC_SERVER_STATUS_END;
	sync->user=user;
	sync->instr_once_maxsize=PX_SYNC_DEFAULT_INSTR_ONCE_MAX_SIZE;
	sync->unique=0;
	sync->version=0;
	PX_VectorInit(mp,&sync->clients,sizeof(PX_SyncFrame_Server_Clients),32);
	PX_VectorInit(mp,&sync->stampsIndexTable,sizeof(PX_SyncFrame_InstrStream_StampIndex),PX_SYNC_INSTRS_SIZE);
	PX_MemoryInit(mp,&sync->stampsInstrStream);
	return PX_MemoryResize(&sync->stampsInstrStream,PX_SYNC_INSTRS_BYTES_SIZE);
}

px_void PX_SyncFrameServerSetVersion(PX_SyncFrame_Server *sync,px_dword version)
{
	sync->version=version;
}

static px_bool PX_SyncFrameServer_Write(PX_SyncFrame_Server *sync_server,PX_SyncFrame_Server_Clients *pClient,px_void *data,px_uint size)
{
	PX_Sync_IO_Packet *packet=(PX_Sync_IO_Packet *)data;
	packet->verify_id=pClient->client_id;
	return (sync_server->write(sync_server,pClient->port,data,size));
}
static px_bool PX_SyncFrameServer_Read(PX_SyncFrame_Server *server,PX_Sync_Port *port,PX_SyncFrame_Server_Clients **ppclient)
{
	PX_SyncFrame_Server_Clients *pClient=PX_NULL;
	PX_Sync_IO_Packet *packet=PX_NULL;
	
__RE_RECV:

	while (server->read(server,port,server->recv_cache_buffer,sizeof(server->recv_cache_buffer),&server->recv_cache_size))
	{
		int i;
		packet=(PX_Sync_IO_Packet *)(server->recv_cache_buffer);
		for (i=0;i<server->clients.size;i++)
		{
			pClient=PX_VECTORAT(PX_SyncFrame_Server_Clients,&server->clients,i);
			if (pClient->server_id==packet->verify_id)
			{
				if(packet->unique==0)
				{
					*ppclient=pClient;
					return PX_TRUE;
				}
				else
				{
					px_int j;
					for (j=0;j<PX_SYNC_UNIQUE_ARRAY_SIZE;j++)
					{
						if (packet->unique==pClient->acceptuniqueQueue[j])
						{
							goto __RE_RECV;
						}
					}
					if (pClient->uniqueQueuewIndex>=PX_SYNC_UNIQUE_ARRAY_SIZE)
					{
						pClient->uniqueQueuewIndex=0;
					}
					pClient->acceptuniqueQueue[pClient->uniqueQueuewIndex]=packet->unique;
					pClient->uniqueQueuewIndex++;
					*ppclient=pClient;
					return PX_TRUE;
				}

			}
		}
		
	}
	return PX_FALSE;
}

static px_void PX_SyncFrameServerHandle_StatusConnect(PX_SyncFrame_Server *sync_server,px_dword elpased)
{
	PX_SyncFrame_Server_Clients *pClient=PX_NULL;
	PX_Sync_IO_Packet *recv_packet,*send_packet;
	PX_Sync_Port port;
	px_int i;

	PX_memset(&port,0,sizeof(PX_Sync_Port));

	while (PX_SyncFrameServer_Read(sync_server,&port,&pClient))
	{
		recv_packet=(PX_Sync_IO_Packet *)(sync_server->recv_cache_buffer);

		if (recv_packet->type==PX_SYNC_IO_TYPE_CONNECT)
		{
			if (recv_packet->param1==sync_server->version)
			{
				if(pClient->status==PX_SYNC_SERVERCLIENT_STATUS_CONNECT)
				{
					pClient->status=PX_SYNC_SERVERCLIENT_STATUS_PROCESSING;
					pClient->timeIndexOffset=0;
					pClient->timeStreamOffset=0;
					pClient->port=port;
					pClient->sendDurationTick=PX_SYNC_SERVER_SEND_DURATION;
				}
			}
			
			send_packet=(PX_Sync_IO_Packet *)(sync_server->send_cache_buffer);
			send_packet->verify_id=0;
			send_packet->type=PX_SYNC_IO_TYPE_CONNECTACK;
			send_packet->unique=0;
			send_packet->param1=pClient->c_id;
			send_packet->param2=0;
			port=pClient->port;
			PX_SyncFrameServer_Write(sync_server,pClient,send_packet,sizeof(PX_Sync_IO_Packet));

			for(i=0;i<sync_server->clients.size;i++)
			{
				pClient=PX_VECTORAT(PX_SyncFrame_Server_Clients,&sync_server->clients,i);
				if (pClient->status!=PX_SYNC_SERVERCLIENT_STATUS_PROCESSING)
				{
					break;
				}
			}

			if (i>=sync_server->clients.size)
			{
				sync_server->status=PX_SYNC_SERVER_STATUS_PROCESSING;
				return;
			}
		}
		else if(recv_packet->type==PX_SYNC_IO_TYPE_QUERYSTATE)
		{
			px_int count=0;
			for(i=0;i<sync_server->clients.size;i++)
			{
				PX_SyncFrame_Server_Clients *pEnumClient=PX_VECTORAT(PX_SyncFrame_Server_Clients,&sync_server->clients,i);
				if (pEnumClient->status==PX_SYNC_SERVERCLIENT_STATUS_PROCESSING)
				{
					count++;
				}
			}

			send_packet=(PX_Sync_IO_Packet *)(sync_server->send_cache_buffer);
			send_packet->verify_id=0;
			send_packet->type=PX_SYNC_IO_TYPE_QUERYSTATEACK;
			send_packet->unique=0;
			send_packet->param1=count;
			send_packet->param2=sync_server->clients.size;
			port=pClient->port;
			PX_SyncFrameServer_Write(sync_server,pClient,send_packet,sizeof(PX_Sync_IO_Packet));
		}
	}
}


static px_void PX_SyncFrameServerHandle_StatusProcess(PX_SyncFrame_Server *sync_server,px_dword updateelpased)
{
	px_int i,datasize=0,dataoffset;
	PX_SyncFrame_Server_Clients *pClient=PX_NULL;
	PX_Sync_IO_Packet *send_packet,*recv_packet;
	PX_Sync_Port port;
	px_dword elpased;
	PX_memset(&port,0,sizeof(PX_Sync_Port));
	sync_server->time+=updateelpased;

	for (i=0;i<sync_server->clients.size;i++)
	{
		pClient=PX_VECTORAT(PX_SyncFrame_Server_Clients,&sync_server->clients,i);
		pClient->lastInstrElpased+=updateelpased;
	}

	do
	{
		if (updateelpased>=PX_SYNC_SERVER_SEND_DURATION)
		{
			elpased=PX_SYNC_SERVER_SEND_DURATION;
			updateelpased-=elpased;
		}
		else
		{
			elpased=updateelpased;
			updateelpased=0;
		}


		//nop timestamp
		if ((sync_server->time+elpased)/sync_server->updateDuration>=(px_dword)sync_server->stampsIndexTable.size)
		{
			PX_SyncFrame_InstrStream_StampIndex newStamp;
			newStamp.oft=sync_server->stampsInstrStream.usedsize;
			newStamp.size=0;
			//PX_SYNC_LOG("Time thunk %d sum32 %d\n",sync_server->stampsIndexTable.size,PX_SyncFrameServerSum32(sync_server));

			PX_VectorPushback(&sync_server->stampsIndexTable,&newStamp);
			for (i=0;i<sync_server->clients.size;i++)
			{
				pClient=PX_VECTORAT(PX_SyncFrame_Server_Clients,&sync_server->clients,i);
				pClient->onceRecvSize=0;
			}
		}

		while (PX_SyncFrameServer_Read(sync_server,&port,&pClient))
		{
			recv_packet=(PX_Sync_IO_Packet *)(sync_server->recv_cache_buffer);
			pClient->lastInstrElpased=0;

			switch (recv_packet->type)
			{
			case PX_SYNC_IO_TYPE_CONNECT:
				{
					if (recv_packet->param1==sync_server->version)
					{
						send_packet=(PX_Sync_IO_Packet *)(sync_server->send_cache_buffer);
						send_packet->verify_id=0;
						send_packet->type=PX_SYNC_IO_TYPE_CONNECTACK;
						send_packet->unique=0;
						send_packet->param1=pClient->c_id;;
						send_packet->param2=0;
						pClient->port=port;
						PX_SyncFrameServer_Write(sync_server,pClient,send_packet,sizeof(PX_Sync_IO_Packet));
					}
				}
				break;
			case PX_SYNC_IO_TYPE_QUERYSTATE:
				{
					send_packet=(PX_Sync_IO_Packet *)(sync_server->send_cache_buffer);
					send_packet->verify_id=0;
					send_packet->type=PX_SYNC_IO_TYPE_QUERYSTATEACK;
					send_packet->unique=0;
					send_packet->param1=sync_server->clients.size;
					send_packet->param2=sync_server->clients.size;
					port=pClient->port;
					PX_SyncFrameServer_Write(sync_server,pClient,send_packet,sizeof(PX_Sync_IO_Packet));
				}
				break;
			case PX_SYNC_IO_TYPE_REQUEST:
				{
					pClient->timeIndexOffset=recv_packet->param1;
					pClient->timeStreamOffset=recv_packet->param2;
					pClient->sendTimes=0;
					pClient->sendDurationTick=PX_SYNC_SERVER_SEND_DURATION;
				}
				break;
			case PX_SYNC_IO_TYPE_OPCODE:
				{
					px_int max_size=0;
					PX_SyncFrame_InstrStream_StampIndex *plastStamp=PX_VECTORLAST(PX_SyncFrame_InstrStream_StampIndex,&sync_server->stampsIndexTable);

					//check size
					if (sync_server->recv_cache_size<(px_int)((PX_STRUCT_OFFSET(PX_Sync_IO_Packet,data)+sizeof(px_dword)*2)))
					{
						break;
					}

					//check size
					if (pClient->onceRecvSize+sync_server->recv_cache_size>sync_server->instr_once_maxsize)
					{
						break;
					}
					pClient->onceRecvSize+=sync_server->recv_cache_size;
					datasize=sync_server->recv_cache_size-(px_int)PX_STRUCT_OFFSET(PX_Sync_IO_Packet,data);

					PX_MemoryCat(&sync_server->stampsInstrStream,&pClient->c_id,sizeof(pClient->c_id));//id 
					PX_MemoryCat(&sync_server->stampsInstrStream,&datasize,sizeof(px_dword));//size
					PX_MemoryCat(&sync_server->stampsInstrStream,&recv_packet->data,datasize);

					plastStamp->size+=sizeof(pClient->c_id)+sizeof(px_dword)+datasize;

				}
				break;
			}
		}

		//send instrument

		if(sync_server->stampsIndexTable.size)
		{
			for (i=0;i<sync_server->clients.size;i++)
			{
				pClient=PX_VECTORAT(PX_SyncFrame_Server_Clients,&sync_server->clients,i);

				if (((px_int)pClient->timeIndexOffset>=sync_server->stampsIndexTable.size)&&((px_int)pClient->timeStreamOffset>=sync_server->stampsInstrStream.usedsize))
				{
					continue;
				}
				
				if (pClient->sendDurationTick>=PX_SYNC_SERVER_SEND_DURATION)
				{
				   pClient->sendDurationTick=0;

				   if (pClient->sendTimes<=0)
				   {
					   px_uint reserve_size;
					   px_dword stream_size=0,indexTableSize=0;
					   px_dword wstream_size=0,windexTableSize=0;
		

					   send_packet=(PX_Sync_IO_Packet *)(pClient->send_cache_instr_buffer);
					   PX_memset(pClient->send_cache_instr_buffer,0,sizeof(pClient->send_cache_instr_buffer));


					   send_packet->type=PX_SYNC_IO_TYPE_OPCODE;
					   send_packet->verify_id=0;

					   reserve_size=sizeof(pClient->send_cache_instr_buffer)- (px_int)PX_STRUCT_OFFSET(PX_Sync_IO_Packet,data);

					   wstream_size=sync_server->stampsInstrStream.usedsize-pClient->timeStreamOffset;

					   if (wstream_size>reserve_size-sizeof(pClient->timeStreamOffset)-sizeof(wstream_size))//<--- start offset dword and size dword
					   {
						   wstream_size=reserve_size-sizeof(pClient->timeStreamOffset)-sizeof(wstream_size);
					   }

					   datasize=0;
					   dataoffset=0;
					   
					   //stream start-offset 4bytes
					   PX_memcpy(pClient->send_cache_instr_buffer+(sizeof(sync_server->recv_cache_buffer)-reserve_size),&pClient->timeStreamOffset,sizeof(pClient->timeStreamOffset));
					   reserve_size-=sizeof(pClient->timeStreamOffset);
					   //stream size 4bytes
					   PX_memcpy(pClient->send_cache_instr_buffer+(sizeof(sync_server->recv_cache_buffer)-reserve_size),&wstream_size,sizeof(wstream_size));
					   reserve_size-=sizeof(wstream_size);

					   //Instr stream 
					   if (wstream_size>reserve_size){wstream_size=reserve_size;}
					   PX_memcpy(pClient->send_cache_instr_buffer+(sizeof(sync_server->recv_cache_buffer)-reserve_size),sync_server->stampsInstrStream.buffer+pClient->timeStreamOffset,wstream_size);
					   datasize+=wstream_size;
					   reserve_size-=wstream_size;


					   if (reserve_size>=sizeof(PX_SyncFrame_InstrStream_StampIndex)+sizeof(px_dword)*2)
					   {
						   if (sync_server->stampsIndexTable.size-pClient->timeIndexOffset>1) //last stamp is not ready
						   {
							   windexTableSize=(sync_server->stampsIndexTable.size-pClient->timeIndexOffset-1);

							   if (windexTableSize>(reserve_size-sizeof(px_dword)*2)/sizeof(PX_SyncFrame_InstrStream_StampIndex))
							   {
								   windexTableSize=(reserve_size-sizeof(px_dword)*2)/sizeof(PX_SyncFrame_InstrStream_StampIndex);
							   }
							   if (windexTableSize)
							   {
								   //Index start-offset
								   PX_memcpy(pClient->send_cache_instr_buffer+(sizeof(sync_server->recv_cache_buffer)-reserve_size),&pClient->timeIndexOffset,sizeof(pClient->timeIndexOffset));
								   reserve_size-=sizeof(pClient->timeIndexOffset);
								   //Index Size
								   PX_memcpy(pClient->send_cache_instr_buffer+(sizeof(sync_server->recv_cache_buffer)-reserve_size),&windexTableSize,sizeof(windexTableSize));
								   reserve_size-=sizeof(windexTableSize);
								   //data
								   PX_memcpy(pClient->send_cache_instr_buffer+(sizeof(sync_server->recv_cache_buffer)-reserve_size),(px_byte *)sync_server->stampsIndexTable.data+pClient->timeIndexOffset*sizeof(PX_SyncFrame_InstrStream_StampIndex),windexTableSize*sizeof(PX_SyncFrame_InstrStream_StampIndex));
								   reserve_size-=(windexTableSize*sizeof(PX_SyncFrame_InstrStream_StampIndex));
								   datasize+=(windexTableSize*sizeof(PX_SyncFrame_InstrStream_StampIndex));
							   }
						   }
					   }
					 

					   pClient->timeStreamOffset+=wstream_size;
					   pClient->timeIndexOffset+=windexTableSize;
					   pClient->send_cache_instr_size=sizeof(sync_server->recv_cache_buffer)-reserve_size;
					   //send cache
					   
					   if(datasize)
					   {
						   //alloc unique
						   send_packet->unique=(++sync_server->unique);
						   if (send_packet->unique==0)
						   {
							   send_packet->unique=1;
						   }
						  // PX_SYNC_LOG("id %d Update ms %d\n",pClient->c_id,sync_server->time-pClient->lastsendtime);
						   pClient->lastsendtime=sync_server->time;
						   PX_SyncFrameServer_Write(sync_server,pClient,pClient->send_cache_instr_buffer,pClient->send_cache_instr_size);
						   //////////////////////////////////////////////////////////////////////////
						   pClient->sendTimes=PX_SYNC_SERVER_SEND_TIMES;
						   //////////////////////////////////////////////////////////////////////////	
					   }
					   else
					   {
						   pClient->send_cache_instr_size=0;
						   pClient->sendTimes=0;
					   }

				   }
				   else
				   {
					   pClient->sendTimes--;
					    send_packet=(PX_Sync_IO_Packet *)(pClient->send_cache_instr_buffer);
					   if (pClient->send_cache_instr_size)
					   {
						PX_SyncFrameServer_Write(sync_server,pClient,pClient->send_cache_instr_buffer,pClient->send_cache_instr_size);
					   }
				   }
				}
				else
				{
					pClient->sendDurationTick+=elpased;
				}

			}
		}
		
	}while (updateelpased);
}
px_void PX_SyncFrameServerUpdate(PX_SyncFrame_Server *sync_server,px_dword elpased)
{
	switch(sync_server->status)
	{
	case PX_SYNC_SERVER_STATUS_CONNECT:
		{
			PX_SyncFrameServerHandle_StatusConnect(sync_server,elpased);
		}
		break;
	case PX_SYNC_SERVER_STATUS_PROCESSING:
		{
			PX_SyncFrameServerHandle_StatusProcess(sync_server,elpased);
		}
		break;
	case PX_SYNC_SERVER_STATUS_END:
		break;
	default:
		break;

	}
}
px_bool PX_SyncFrameServerAddClient(PX_SyncFrame_Server *sync,px_dword server_verify_id,px_dword client_id,px_dword c_id)
{
	PX_SyncFrame_Server_Clients client;
	px_int i;
	PX_memset(client.port.byte_atom,0,sizeof(client.port.byte_atom));
	client.timeIndexOffset=0;
	client.timeStreamOffset=0;
	client.sendDurationTick=PX_SYNC_SERVER_SEND_DURATION;
	client.status=PX_SYNC_SERVERCLIENT_STATUS_CONNECT;
	for(i=0;i<PX_SYNC_UNIQUE_ARRAY_SIZE;i++)
	client.acceptuniqueQueue[i]=0;
	client.uniqueQueuewIndex=0;
	client.sendTimes=0;
	client.c_id=c_id;
	client.server_id=server_verify_id;
	client.client_id=client_id;
	client.send_cache_instr_size=0;
	client.onceRecvSize=0;
	client.lastsendtime=0;
	client.lastInstrElpased=0;
	return PX_VectorPushback(&sync->clients,&client);
}


px_void PX_SyncFrameServerSetInstrOnceMaxSize(PX_SyncFrame_Server *sync,px_int maxsize)
{
	sync->instr_once_maxsize=maxsize;
}

px_bool PX_SyncFrameClientInit(PX_SyncFrame_Client *client,px_memorypool *mp,px_dword updateDuration,PX_Sync_Port serverport,px_dword server_verify_id,px_dword client_id,px_syncframe_client_read read,px_syncframe_client_write write,px_void *user)
{
	px_int i;
	client->serverport=serverport;
	client->mp=mp;
	client->read=read;
	client->write=write;
	client->server_verify_id=server_verify_id;
	client->client_id=client_id;
	client->updateDuration=updateDuration;
	client->time=0;
	client->unique=0;
	client->delayms=0;
	client->user=user;
	client->c_id=0;
	client->connectCount=0;
	client->connectSumCount=0;
	client->version=0;

	for(i=0;i<PX_SYNC_UNIQUE_ARRAY_SIZE;i++)
		client->acceptuniqueQueue[i]=0;
	client->uniqueQueuewIndex=0;
	client->status=PX_SYNC_CLIENT_STATUS_CONNECTING;
	client->send_cache_Instr_size=0;
	client->send_repeat_times=0;
	PX_VectorInit(mp,&client->stampsIndexTable,sizeof(PX_SyncFrame_InstrStream_StampIndex),PX_SYNC_INSTRS_SIZE);
	PX_MemoryInit(mp,&client->Input_InstrStream);
	PX_MemoryInit(mp,&client->stampsInstrStream);
	return PX_MemoryResize(&client->stampsInstrStream,2*PX_SYNC_INSTRS_SIZE);

}
static px_bool PX_SyncFrameClient_Read(PX_SyncFrame_Client *client)
{
	PX_Sync_Port port;
	px_bool repeat;
	PX_memset(&port,0,sizeof(PX_Sync_Port));
	while (client->read(client,&port,client->recv_cache_buffer,sizeof(client->recv_cache_buffer),&client->recv_cache_buffer_size))
	{
		if (PX_memequ(port.byte_atom,client->serverport.byte_atom,sizeof(port.byte_atom)))
		{
			PX_Sync_IO_Packet *packet=(PX_Sync_IO_Packet *)(client->recv_cache_buffer);
			if (packet->verify_id!=client->client_id)
			{
				return PX_FALSE;
			}

			if(packet->unique==0)
			{
				return PX_TRUE;
			}
			else
			{
				px_int j;
				repeat=PX_FALSE;
				for (j=0;j<PX_SYNC_UNIQUE_ARRAY_SIZE;j++)
				{
					if (packet->unique==client->acceptuniqueQueue[j])
					{
						repeat=PX_TRUE;
						break;
					}
				}
				if (repeat)
				{
					continue;
				}
				if (client->uniqueQueuewIndex>=PX_SYNC_UNIQUE_ARRAY_SIZE)
				{
					client->uniqueQueuewIndex=0;
				}
				client->acceptuniqueQueue[client->uniqueQueuewIndex]=packet->unique;
				client->uniqueQueuewIndex++;
				return PX_TRUE;
			}
			
		}
		else
		{
			continue;
		}
	}
	return PX_FALSE;
}
static px_bool PX_SyncFrameClient_Write(PX_SyncFrame_Client *client,px_void *data,px_uint size)
{
	PX_Sync_IO_Packet *packet=(PX_Sync_IO_Packet *)(data);
	if (!data)
	{
		return PX_FALSE;
	}
	packet->verify_id=client->server_verify_id;
	return (client->write(client,client->serverport,data,size));
}

static px_void PX_SyncFrame_ClientHandle_StatusConneting(PX_SyncFrame_Client *client,px_dword elpased)
{
	//read 
	PX_Sync_IO_Packet *packet;

	while (PX_SyncFrameClient_Read(client))
	{
		PX_Sync_IO_Packet *packet=(PX_Sync_IO_Packet *)(client->recv_cache_buffer);
		if (packet->type==PX_SYNC_IO_TYPE_CONNECTACK)
		{
			client->status=PX_SYNC_CLIENT_STATUS_WAITING;
			client->c_id=packet->param1;
			while (PX_SyncFrameClient_Read(client));//clear buffer
			return;
		}
	}

	if (elpased)
	{
		packet=(PX_Sync_IO_Packet *)(client->send_cache_buffer);
		packet->verify_id=0;
		packet->type=PX_SYNC_IO_TYPE_CONNECT;
		packet->unique=0;
		packet->param1=client->version;
		packet->param2=0;
		PX_SyncFrameClient_Write(client,packet,sizeof(PX_Sync_IO_Packet));
	}
	
}

static px_void PX_SyncFrame_ClientHandle_StatusWaiting(PX_SyncFrame_Client *client,px_dword elpased)
{
	//read 
	PX_Sync_IO_Packet *packet;

	while (PX_SyncFrameClient_Read(client))
	{
		PX_Sync_IO_Packet *packet=(PX_Sync_IO_Packet *)(client->recv_cache_buffer);
		if (packet->type==PX_SYNC_IO_TYPE_QUERYSTATEACK)
		{
			client->connectCount=packet->param1;
			client->connectSumCount=packet->param2;
			while (PX_SyncFrameClient_Read(client));//clear buffer
			if (client->connectCount>=client->connectSumCount)
			{
				client->status=PX_SYNC_CLIENT_STATUS_PROCESSING;
			}
			return;
		}
	}

	if (elpased)
	{
		packet=(PX_Sync_IO_Packet *)(client->send_cache_buffer);
		packet->verify_id=0;
		packet->type=PX_SYNC_IO_TYPE_QUERYSTATE;
		packet->unique=0;
		packet->param1=0;
		packet->param2=0;
		PX_SyncFrameClient_Write(client,packet,sizeof(PX_Sync_IO_Packet));
	}

}

static px_void PX_SyncFrame_ClientHandle_Request(PX_SyncFrame_Client *client)
{
	PX_Sync_IO_Packet *packet;

	packet=(PX_Sync_IO_Packet *)(client->send_cache_buffer);

	packet->unique=0;
	packet->type=PX_SYNC_IO_TYPE_REQUEST;
	packet->verify_id=0;
	packet->param1=client->stampsIndexTable.size;
	packet->param2=client->stampsInstrStream.usedsize;

	PX_SyncFrameClient_Write(client,packet,sizeof(PX_Sync_IO_Packet));

}

static px_void PX_SyncFrame_ClientHandle_StatusProcessing(PX_SyncFrame_Client *client,px_dword elpased)
{
	PX_Sync_IO_Packet *recv_packet,*send_packet;
	px_int i;
	client->time+=elpased;
	//write
	//update send cache
	if (client->send_repeat_times==0)
	{
		//read instr to send_repeat_times

		if (client->Input_InstrStream.usedsize)
		{
			px_int sendsize=0;
			
			send_packet=(PX_Sync_IO_Packet *)(client->send_cache_Instr_buffer);
			send_packet->type=PX_SYNC_IO_TYPE_OPCODE;
			send_packet->verify_id=0;
			if (client->unique==0)
			{
				client->unique=1;
			}
			send_packet->unique=(client->unique++);
			
			sendsize+=sizeof(recv_packet->type)+sizeof(recv_packet->verify_id);
			
			if((px_uint)client->Input_InstrStream.usedsize<sizeof(client->send_cache_Instr_buffer)- (px_int)PX_STRUCT_OFFSET(PX_Sync_IO_Packet,data))
			{
				client->send_cache_Instr_size=client->Input_InstrStream.usedsize+ (px_int)PX_STRUCT_OFFSET(PX_Sync_IO_Packet,data);
				PX_memcpy(send_packet->data,client->Input_InstrStream.buffer,client->Input_InstrStream.usedsize);
			}
			else
			{
				PX_MemoryClear(&client->Input_InstrStream);
				client->send_cache_Instr_size=0;
			}
			PX_MemoryClear(&client->Input_InstrStream);

			client->send_repeat_times=PX_SYNC_CLIENT_SEND_TIMES-1;
			
			
			PX_SyncFrameClient_Write(client,client->send_cache_Instr_buffer,client->send_cache_Instr_size);
		}
	}
	else
	{
		
		if (client->sendDurationTick+elpased>=PX_SYNC_CLIENT_SEND_DURATION)
		{
			client->sendDurationTick=0;
			client->send_repeat_times--;
			if(client->send_cache_Instr_size)
			{
				PX_SyncFrameClient_Write(client,client->send_cache_Instr_buffer,client->send_cache_Instr_size);
			}
		}
		else
		{
			client->sendDurationTick+=elpased;
		}
	}





	//recv
	while (PX_SyncFrameClient_Read(client))
	{
		recv_packet=(PX_Sync_IO_Packet *)(client->recv_cache_buffer);

		if (recv_packet->type==PX_SYNC_IO_TYPE_OPCODE)
		{
			px_int streamStart,streamSize,stampStartIndex,stampSize,reserve_size;
			px_byte *streamdata;
			PX_SyncFrame_InstrStream_StampIndex *pStamps;
			streamStart=*(px_dword *)recv_packet->data;
			streamSize=*(px_dword *)(recv_packet->data+sizeof(px_dword));
			streamdata=recv_packet->data+sizeof(px_dword)*2;
			if (streamStart>client->stampsInstrStream.usedsize)
			{
				PX_SyncFrame_ClientHandle_Request(client);
				return;
			}

			if (streamStart<client->stampsInstrStream.usedsize)
			{
				continue;
			}


			if (streamSize)
			{
				PX_MemoryCat(&client->stampsInstrStream,streamdata,streamSize);
			}
			reserve_size=client->recv_cache_buffer_size-streamSize-sizeof(px_dword)*2- (px_int)PX_STRUCT_OFFSET(PX_Sync_IO_Packet,data);
			if (reserve_size>=sizeof(PX_SyncFrame_InstrStream_StampIndex)+sizeof(px_dword)*2)
			{
				stampStartIndex=*(px_dword *)(recv_packet->data+sizeof(px_dword)*2+streamSize);
				stampSize=*(px_dword *)(recv_packet->data+sizeof(px_dword)*2+streamSize+sizeof(px_dword));
				pStamps=(PX_SyncFrame_InstrStream_StampIndex *)(recv_packet->data+sizeof(px_dword)*2+streamSize+sizeof(px_dword)*2);
			}
			else
			{
				continue;
			}

			if (stampStartIndex>client->stampsIndexTable.size)
			{
				PX_SyncFrame_ClientHandle_Request(client);
				return;
			}

			if (stampStartIndex<client->stampsIndexTable.size)
			{
				continue;
			}

		
			for(i=0;i<stampSize;i++)
			{
				PX_VectorPushback(&client->stampsIndexTable,&pStamps[i]);
			}
			//PX_SYNC_LOG("Sync %d delay %d ms sum32:%d\n",client->stampsIndexTable.size,client->time-client->delayms,PX_SyncFrameClientSum32(client));
			client->delayms=client->time;
		}
	}

	
	
}
px_void PX_SyncFrameClientUpdate(PX_SyncFrame_Client *client,px_dword elpased)
{
	switch (client->status)
	{
	case PX_SYNC_CLIENT_STATUS_CONNECTING:
		{
			PX_SyncFrame_ClientHandle_StatusConneting(client,elpased);
		}
		break;
	case PX_SYNC_CLIENT_STATUS_WAITING:
		{
			PX_SyncFrame_ClientHandle_StatusWaiting(client,elpased);
		}
		break;
	case PX_SYNC_CLIENT_STATUS_PROCESSING:
		{
			PX_SyncFrame_ClientHandle_StatusProcessing(client,elpased);
		}
		break;
		case PX_SYNC_CLIENT_STATUS_END:
			break;
	}
}


px_int PX_SyncFrameClientGetReadyFrameCount(PX_SyncFrame_Client *sync)
{
	return sync->stampsIndexTable.size;
}

px_void PX_SyncFrameClientAddInstr(PX_SyncFrame_Client *client,px_void *instr,px_int size)
{
	if (client->status==PX_SYNC_CLIENT_STATUS_PROCESSING)
	{
		PX_MemoryCat(&client->Input_InstrStream,instr,size);
	}
	
}


px_void PX_SyncFrameClientSetVersion(PX_SyncFrame_Client *sync,px_dword version)
{
	sync->version=version;
}

px_void PX_SyncFrameServerStop(PX_SyncFrame_Server *sync)
{
	sync->status=PX_SYNC_SERVER_STATUS_END;
}
px_void PX_SyncFrameServerRun(PX_SyncFrame_Server *sync)
{
	sync->status=PX_SYNC_SERVER_STATUS_CONNECT;
}
px_void PX_SyncFrameServerFree(PX_SyncFrame_Server *sync)
{
	PX_VectorFree(&sync->clients);
	PX_VectorFree(&sync->stampsIndexTable);
	PX_MemoryFree(&sync->stampsInstrStream);
}

px_int PX_SyncFrameServerGetReadyFrameCount(PX_SyncFrame_Server *sync)
{
	return sync->stampsIndexTable.size-1;
}

px_void PX_SyncFrameClientFree(PX_SyncFrame_Client *sync)
{
	PX_VectorFree(&sync->stampsIndexTable);
	PX_MemoryFree(&sync->stampsInstrStream);
	PX_MemoryFree(&sync->Input_InstrStream);
}




px_uint32 PX_SyncFrameServerSum32(PX_SyncFrame_Server *sync)
{
	px_uint32 sum=0;
	sum+=PX_sum32(sync->stampsIndexTable.data,sync->stampsIndexTable.nodesize*sync->stampsIndexTable.size);
	sum+=PX_sum32(sync->stampsInstrStream.buffer,sync->stampsInstrStream.usedsize);
	return sum;
}

px_uint32 PX_SyncFrameClientSum32(PX_SyncFrame_Client *sync)
{
	px_uint32 sum=0;
	sum+=PX_sum32(sync->stampsIndexTable.data,sync->stampsIndexTable.nodesize*sync->stampsIndexTable.size);
	sum+=PX_sum32(sync->stampsInstrStream.buffer,sync->stampsInstrStream.usedsize);
	return sum;
}



//////////////////////////////////////////////////////////////////////////
//SyncData

static px_int PX_SyncDataCalculateBlockCount(px_int size)
{
	return size%PX_SYNCDATA_BLOCK_SIZE?size/PX_SYNCDATA_BLOCK_SIZE+1:size/PX_SYNCDATA_BLOCK_SIZE;
}

px_bool PX_SyncDataServerInit(PX_SyncData_Server *syncdata_server,px_memorypool *mp,px_dword serverID,px_syncdata_server_read read,px_syncdata_server_write write,px_void *user)
{
	PX_memset(syncdata_server,0,sizeof(PX_SyncData_Server));
	syncdata_server->serverID=serverID;
	syncdata_server->read=read;
	syncdata_server->write=write;
	syncdata_server->user=user;
	if(!PX_VectorInit(mp,&syncdata_server->clients,sizeof(PX_SyncData_Server_Client),16)) return PX_FALSE;
	return PX_TRUE;
}



px_bool PX_SyncDataServerSetSyncData(PX_SyncData_Server *s,px_void *data,px_dword size)
{
	s->data=(px_byte *)data;
	s->size=size;

	return PX_TRUE;
}

px_bool PX_SyncDataServer_ReadBlock(PX_SyncData_Server *s,PX_Sync_Port *port,px_byte *data,px_int bufferSize,px_int *readsize)
{
	if (s->read(s,port,data,bufferSize,readsize))
	{
		if (*readsize>bufferSize)
		{
			return PX_FALSE;
		}

		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_SyncDataServer_WriteBlock(PX_SyncData_Server *s,PX_Sync_Port port,px_byte *data,px_int bufferSize)
{
	if (bufferSize>PX_SYNCDATA_DATAGRAM_MAX_SIZE)
	{
		return PX_FALSE;
	}
	if (s->write(s,port,data,bufferSize))
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}



px_bool PX_SyncDataServerUpdate(PX_SyncData_Server *s,px_int elpased)
{
	PX_Sync_Port port;
	px_int readSize,i;
	PX_SyncData_Server_Client *pClient=PX_NULL;
	PX_SyncData_Datagram R_datagram;

	PX_memset(&port,0,sizeof(port));

	while (PX_SyncDataServer_ReadBlock(s,&port,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
	{
		if (R_datagram.header.serverID==s->serverID)
		{
			if (R_datagram.header.opcode==PX_SYNCDATA_OPCODE_QUERY)
			{
				if (s->clients.size>s->clients.size)
				{
					continue;
				}
				//searching data
				for (i=0;i<s->clients.size;i++)
				{
					pClient=PX_VECTORAT(PX_SyncData_Server_Client,&s->clients,i);
					if (pClient->clientID==R_datagram.query.clientID)
					{
						PX_SyncData_QueryAck queryAck;
						if (!PX_memequ(&pClient->port,&port,sizeof(port)))
						{
							PX_memcpy(&pClient->port,&port,sizeof(port));
						}
						queryAck.opcode=PX_SYNCDATA_OPCODE_QUERYACK;
						queryAck.serverID=s->serverID;
						queryAck.reserved=0;
						queryAck.size=s->size;
						PX_SyncDataServer_WriteBlock(s,port,(px_byte *)&queryAck,sizeof(queryAck));
						break;
					}
				}
			}

			if (R_datagram.header.opcode==PX_SYNCDATA_OPCODE_REQUEST)
			{
				for (i=0;i<s->clients.size;i++)
				{
					pClient=PX_VECTORAT(PX_SyncData_Server_Client,&s->clients,i);

					if (PX_memequ(&pClient->port,&port,sizeof(port)))
					{
						int j=0;
						if ((px_int)R_datagram.request.blockIndex<PX_SyncDataCalculateBlockCount(s->size))
						{
							pClient->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING;
							pClient->ProcessIndex=R_datagram.request.blockIndex;
						}
						else
						{
							pClient->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZED;
							pClient->ProcessIndex=R_datagram.request.blockIndex;
						}
						
					}
				}
			}

		}
	}

	for (i=0;i<s->clients.size;i++)
	{
		pClient=PX_VECTORAT(PX_SyncData_Server_Client,&s->clients,i);
		if (pClient->status!=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING)
		{
			continue;
		}
		if (pClient->ProcessIndex<PX_SyncDataCalculateBlockCount(s->size))
		{
			if(pClient->send_elpased>=PX_SYNCDATA_SENDING_DURATION)
			{
				px_int size;
				PX_SyncData_RequestAck QuestAck;
				pClient->send_elpased=0;
				QuestAck.blockIndex=pClient->ProcessIndex;
				QuestAck.opcode=PX_SYNCDATA_OPCODE_REQUESTACK;
				QuestAck.reserved=0;
				QuestAck.serverID=s->serverID;
				size=s->size-pClient->ProcessIndex*PX_SYNCDATA_BLOCK_SIZE;
				size=(size>PX_SYNCDATA_BLOCK_SIZE?PX_SYNCDATA_BLOCK_SIZE:size);
				PX_memcpy(QuestAck.data,s->data+pClient->ProcessIndex*PX_SYNCDATA_BLOCK_SIZE,size);
				PX_SyncDataServer_WriteBlock(s,pClient->port,(px_byte *)&QuestAck,sizeof(QuestAck));
				pClient->ProcessIndex++;
			}
			else
			{
				pClient->send_elpased+=elpased;
			}
		}		
	}
	return PX_TRUE;
}

px_bool PX_SyncDataServerAddClient(PX_SyncData_Server *syncdata_server,px_dword clientID)
{
	PX_SyncData_Server_Client newClient;
	PX_memset(&newClient,0,sizeof(newClient));
	newClient.ProcessIndex=0;
	newClient.send_elpased=0;
	newClient.status=PX_SYNCDATA_CLIENT_STATUS_QUERY;
	newClient.clientID=clientID;
	return PX_VectorPushback(&syncdata_server->clients,&newClient);
}

px_void PX_SyncDataServerFree(PX_SyncData_Server *syncdata_server)
{
	PX_VectorFree(&syncdata_server->clients);
}

px_bool PX_SyncDataClientInit(PX_SyncData_Client *syncdata_client,px_memorypool *mp,px_dword serverID,px_dword clientID,PX_Sync_Port server_port,px_syncdata_client_read read,px_syncdata_client_write write,px_void *user)
{
	PX_memset(syncdata_client,0,sizeof(PX_SyncData_Client));
	syncdata_client->clientID=clientID;
	syncdata_client->mp=mp;
	syncdata_client->port=server_port;
	syncdata_client->read=read;
	syncdata_client->serverID=serverID;
	syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_QUERY;
	syncdata_client->user=user;
	syncdata_client->write=write;
	return PX_TRUE;
}



px_bool PX_SyncDataClientIsCompleted(PX_SyncData_Client *syncdata_client)
{
	return (syncdata_client->status==PX_SYNC_SERVERCLIENT_STATUS_END);
}

px_bool PX_SyncDataClient_ReadBlock(PX_SyncData_Client *s,PX_Sync_Port *port,px_byte *data,px_int bufferSize,px_int *readsize)
{
	if (s->read(s,port,data,bufferSize,readsize))
	{
		if (*readsize>bufferSize)
		{
			return PX_FALSE;
		}
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_SyncDataClient_WriteBlock(PX_SyncData_Client *s,PX_Sync_Port port,px_byte *data,px_int bufferSize)
{
	if (bufferSize>PX_SYNCDATA_DATAGRAM_MAX_SIZE)
	{
		return PX_FALSE;
	}
	if (s->write(s,port,data,bufferSize))
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_SyncDataClientResetData(PX_SyncData_Client *syncdata_client,px_uint size)
{
	if (syncdata_client->data)
	{
		MP_Free(syncdata_client->mp,syncdata_client->data);
	}
	syncdata_client->data=(px_byte *)MP_Malloc(syncdata_client->mp,size);
	syncdata_client->datasize=size;
	syncdata_client->blockCount=PX_SyncDataCalculateBlockCount(size);
	syncdata_client->acceptBlock=0;

	return syncdata_client->data!=PX_NULL;
}

px_bool PX_SyncDataClientUpdate(PX_SyncData_Client *syncdata_client,px_int elpased)
{
	PX_Sync_Port port;
	px_int readSize;
	PX_SyncData_Datagram R_datagram;

	syncdata_client->last_recv_elpased+=elpased;

	switch (syncdata_client->status)
	{
	case PX_SYNCDATA_CLIENT_STATUS_QUERY:	
		{
			if (syncdata_client->query_elpased>PX_SYNCDATA_QUERY_DURATION)
			{
				PX_SyncData_Query query;
				syncdata_client->query_elpased=0;
				query.opcode=PX_SYNCDATA_OPCODE_QUERY;
				query.reserved=0;
				query.serverID=syncdata_client->serverID;
				query.clientID=syncdata_client->clientID;
				PX_SyncDataClient_WriteBlock(syncdata_client,syncdata_client->port,(px_byte *)&query,sizeof(query));
			}
			else
			{
				syncdata_client->query_elpased+=elpased;
			}

			if (PX_SyncDataClient_ReadBlock(syncdata_client,&port,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
			{
				if (R_datagram.header.serverID==syncdata_client->serverID)
				{
					if (R_datagram.header.opcode==PX_SYNCDATA_OPCODE_QUERYACK)
					{
						syncdata_client->last_recv_elpased=0;
						if (!PX_SyncDataClientResetData(syncdata_client,R_datagram.queryack.size))
						{
							return PX_FALSE;
						}
						syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING;
					}
				}

			}
		}
		break;
	case PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING:
		{
			syncdata_client->last_recv_elpased+=elpased;

			if (PX_SyncDataClient_ReadBlock(syncdata_client,&port,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
			{
				if (R_datagram.requestAck.serverID==syncdata_client->serverID&&R_datagram.header.opcode==PX_SYNCDATA_OPCODE_REQUESTACK)
				{
					px_int size;
					px_int index=R_datagram.requestAck.blockIndex;
					if (index==syncdata_client->acceptBlock)
					{
						syncdata_client->last_recv_elpased=0;
						syncdata_client->query_elpased=0;
						size=syncdata_client->datasize-index*PX_SYNCDATA_BLOCK_SIZE;
						size=(size>PX_SYNCDATA_BLOCK_SIZE?PX_SYNCDATA_BLOCK_SIZE:size);
						PX_memcpy(syncdata_client->data+index*PX_SYNCDATA_BLOCK_SIZE,R_datagram.requestAck.data,size);
						syncdata_client->acceptBlock++;
					}
					else
					{
						syncdata_client->query_elpased=PX_SYNCDATA_REQUEST_DURATION;
					}

				}
			}

			if (syncdata_client->acceptBlock<syncdata_client->blockCount)
			{
				if (syncdata_client->query_elpased>=PX_SYNCDATA_REQUEST_DURATION)
				{
					px_int o=0;
					PX_SyncData_Request request;
					syncdata_client->query_elpased=0;
					request.blockIndex=syncdata_client->acceptBlock;
					request.serverID=syncdata_client->serverID;
					request.opcode=PX_SYNCDATA_OPCODE_REQUEST;
					request.reserved=0;
					PX_SyncDataClient_WriteBlock(syncdata_client,syncdata_client->port,(px_byte *)&request,sizeof(request));
				}
				else
				{
					syncdata_client->query_elpased+=elpased;
				}
			}
			else
			{
				syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZED;

			}
		}
		break;
	case PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZED:
		break;
		case PX_SYNCDATA_STATUS_ERROR:
			break;
	}
	return PX_TRUE;
}

px_void PX_SyncDataClientFree(PX_SyncData_Client *syncdata_client)
{
	if (syncdata_client->data)
	{
		MP_Free(syncdata_client->mp,syncdata_client->data);
		syncdata_client->data=PX_NULL;
	}
}




