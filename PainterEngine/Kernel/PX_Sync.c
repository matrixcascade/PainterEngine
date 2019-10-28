#include "PX_Sync.h"

#ifdef PX_DEBUG_MODE
#include "stdio.h"
#define PX_SYNC_LOG  printf
#else
#define PX_SYNC_LOG
#endif

typedef struct  
{
	px_dword param[4];
}PX_Sync_IO_Info;

px_bool PX_SyncFrameServerInit(PX_SyncFrame_Server *sync,px_memorypool *mp,px_dword updateDuration,px_syncframe_server_read read,px_syncframe_server_write write)
{
	sync->mp=mp;
	sync->read=read;
	sync->write=write;
	sync->time=0;
	sync->updateDuration=updateDuration;
	sync->status=PX_SYNC_SERVER_STATUS_END;
	
	sync->unique=0;
	PX_VectorInit(mp,&sync->clients,sizeof(PX_SyncFrame_Server_Clients),32);
	PX_VectorInit(mp,&sync->stampsIndexTable,sizeof(PX_SyncFrame_Server_StampIndex),PX_SYNC_INSTRS_SIZE);
	PX_MemoryInit(mp,&sync->stampsInstrStream);
	return PX_MemoryResize(&sync->stampsInstrStream,PX_SYNC_INSTRS_BYTES_SIZE);
}
static px_bool PX_SyncFrameServer_Write(PX_SyncFrame_Server *sync_server,PX_SyncFrame_Server_Clients *pClient,px_void *data,px_uint size)
{
	PX_Sync_IO_Packet *packet=(PX_Sync_IO_Packet *)data;
	packet->verify_id=pClient->client_verify_id;
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
			if (pClient->verify_id==packet->verify_id)
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
			if(pClient->status==PX_SYNC_SERVERCLIENT_STATUS_CONNECT)
			{
				pClient->status=PX_SYNC_SERVERCLIENT_STATUS_PROCESSING;
				pClient->timeIndexOffset=0;
				pClient->timeStreamOffset;
				pClient->port=port;
				pClient->sendDurationTick=PX_SYNC_SERVER_SEND_DURATION;
				PX_SYNC_LOG("connected\n");
			}

			send_packet=(PX_Sync_IO_Packet *)(sync_server->send_cache_buffer);
			send_packet->verify_id=0;
			send_packet->type=PX_SYNC_IO_TYPE_ACK;
			send_packet->unique=0;
			send_packet->param1=0;
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
			PX_SyncFrame_Server_StampIndex newStamp;
			newStamp.oft=sync_server->stampsInstrStream.usedsize;
			newStamp.size=0;
			PX_SYNC_LOG("Time thunk %d sum32 %d\n",sync_server->stampsIndexTable.size,PX_SyncFrameServerSum32(sync_server));

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
			

			switch (recv_packet->type)
			{
			case PX_SYNC_IO_TYPE_CONNECT:
				{
					send_packet=(PX_Sync_IO_Packet *)(sync_server->send_cache_buffer);
					send_packet->verify_id=0;
					send_packet->type=PX_SYNC_IO_TYPE_ACK;
					send_packet->unique=0;
					send_packet->param1=0;
					send_packet->param2=0;
					pClient->port=port;
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
					PX_SyncFrame_Server_StampIndex *plastStamp=PX_VECTORLAST(PX_SyncFrame_Server_StampIndex,&sync_server->stampsIndexTable);

					//check size
					if (sync_server->recv_cache_size<(px_int)((PX_STRUCT_OFFSET(PX_Sync_IO_Packet,data)+sizeof(px_dword)*2)))
					{
						break;
					}

					//check size
					if (pClient->onceRecvSize+sync_server->recv_cache_size>PX_SYNC_SERVER_ONCE_INSTR_LEN)
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


					   if (reserve_size>=sizeof(PX_SyncFrame_Server_StampIndex)+sizeof(px_dword)*2)
					   {
						   if (sync_server->stampsIndexTable.size-pClient->timeIndexOffset>1) //last stamp is not ready
						   {
							   windexTableSize=(sync_server->stampsIndexTable.size-pClient->timeIndexOffset-1);

							   if (windexTableSize>(reserve_size-sizeof(px_dword)*2)/sizeof(PX_SyncFrame_Server_StampIndex))
							   {
								   windexTableSize=(reserve_size-sizeof(px_dword)*2)/sizeof(PX_SyncFrame_Server_StampIndex);
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
								   PX_memcpy(pClient->send_cache_instr_buffer+(sizeof(sync_server->recv_cache_buffer)-reserve_size),(px_byte *)sync_server->stampsIndexTable.data+pClient->timeIndexOffset*sizeof(PX_SyncFrame_Server_StampIndex),windexTableSize*sizeof(PX_SyncFrame_Server_StampIndex));
								   reserve_size-=(windexTableSize*sizeof(PX_SyncFrame_Server_StampIndex));
								   datasize+=(windexTableSize*sizeof(PX_SyncFrame_Server_StampIndex));
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
						   PX_SYNC_LOG("id %d Update ms %d\n",pClient->c_id,sync_server->time-pClient->lastsendtime);
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
px_bool PX_SyncFrameServerAddClient(PX_SyncFrame_Server *sync,px_dword verify_id,px_dword client_verify_id,px_dword c_id)
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
	client.verify_id=verify_id;
	client.client_verify_id=client_verify_id;
	client.send_cache_instr_size=0;
	client.onceRecvSize=0;
	client.lastsendtime=0;
	return PX_VectorPushback(&sync->clients,&client);
}

px_bool PX_SyncFrameClientInit(PX_SyncFrame_Client *client,px_memorypool *mp,px_dword updateDuration,PX_Sync_Port serverport,px_dword server_verify_id,px_dword verify_id,px_syncframe_client_read read,px_syncframe_client_write write)
{
	px_int i;
	client->serverport=serverport;
	client->mp=mp;
	client->read=read;
	client->write=write;
	client->server_verify_id=server_verify_id;
	client->verify_id=verify_id;
	client->updateDuration=updateDuration;
	client->time=0;
	client->unique=0;
	client->delayms=0;
	

	for(i=0;i<PX_SYNC_UNIQUE_ARRAY_SIZE;i++)
		client->acceptuniqueQueue[i]=0;
	client->uniqueQueuewIndex=0;
	client->status=PX_SYNC_CLIENT_STATUS_CONNECT;
	client->send_cache_Instr_size=0;
	client->send_repeat_times=0;
	PX_VectorInit(mp,&client->stampsIndexTable,sizeof(PX_SyncFrame_Server_StampIndex),PX_SYNC_INSTRS_SIZE);
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
			if (packet->verify_id!=client->verify_id)
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
static px_void PX_SyncFrame_ClientHandle_StatusConnet(PX_SyncFrame_Client *client,px_dword elpased)
{
	//read 
	PX_Sync_IO_Packet *packet;

	while (PX_SyncFrameClient_Read(client))
	{
		PX_Sync_IO_Packet *packet=(PX_Sync_IO_Packet *)(client->recv_cache_buffer);
		if (packet->type==PX_SYNC_IO_TYPE_ACK)
		{
			client->status=PX_SYNC_CLIENT_STATUS_PROCESSING;
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
static px_void PX_SyncFrame_ClientHandle_StatusProcess(PX_SyncFrame_Client *client,px_dword elpased)
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
			PX_SyncFrame_Server_StampIndex *pStamps;
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
			if (reserve_size>=sizeof(PX_SyncFrame_Server_StampIndex)+sizeof(px_dword)*2)
			{
				stampStartIndex=*(px_dword *)(recv_packet->data+sizeof(px_dword)*2+streamSize);
				stampSize=*(px_dword *)(recv_packet->data+sizeof(px_dword)*2+streamSize+sizeof(px_dword));
				pStamps=(PX_SyncFrame_Server_StampIndex *)(recv_packet->data+sizeof(px_dword)*2+streamSize+sizeof(px_dword)*2);
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
			PX_SYNC_LOG("Sync %d delay %d ms sum32:%d\n",client->stampsIndexTable.size,client->time-client->delayms,PX_SyncFrameClientSum32(client));
			if (client->time-client->delayms>100)
			{
				PX_SYNC_LOG("");
			}
			client->delayms=client->time;

		}
	}

	
	
}
px_void PX_SyncFrameClientUpdate(PX_SyncFrame_Client *client,px_dword elpased)
{
	switch (client->status)
	{
	case PX_SYNC_CLIENT_STATUS_CONNECT:
		{
			PX_SyncFrame_ClientHandle_StatusConnet(client,elpased);
		}
		break;
	case PX_SYNC_CLIENT_STATUS_PROCESSING:
		{
			PX_SyncFrame_ClientHandle_StatusProcess(client,elpased);
		}
		break;
	}
}


px_void PX_SyncFrameClientAddInstr(PX_SyncFrame_Client *client,px_void *instr,px_int size)
{
	PX_MemoryCat(&client->Input_InstrStream,instr,size);
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


px_bool PX_SyncDataServerInit(px_memorypool *mp,px_dword pid,PX_SyncData_Server *syncdata_server,px_int  max_Concurrency,px_syncdata_server_write write,px_syncdata_server_read read,PX_SyncData_Encode encode,PX_SyncData_Decode decode)
{
	syncdata_server->alloc_datasize=0;
	syncdata_server->blockCount=0;
	if(!PX_VectorInit(mp,&syncdata_server->clients,sizeof(PX_SyncData_Server_Client),max_Concurrency)) return PX_FALSE;
	syncdata_server->CRC32=0;
	syncdata_server->data=PX_NULL;
	syncdata_server->decode=decode;
	syncdata_server->encode=encode;
	syncdata_server->max_Concurrency=max_Concurrency;
	syncdata_server->mp=mp;
	syncdata_server->p_id=pid;
	syncdata_server->read=read;
	syncdata_server->size=0;
	syncdata_server->write=write;
	return PX_TRUE;
}



px_bool PX_SyncDataServerSetSyncData(PX_SyncData_Server *s,px_byte *data,px_dword size)
{
	px_int i,j;
	PX_SyncData_Server_Client *pClient=PX_NULL;

	for (i=0;i<s->clients.size;i++)
	{
		pClient=PX_VECTORAT(PX_SyncData_Server_Client,&s->clients,i);
		for(j=0;j<PX_SYNCDATA_CACHE_SIZE;j++)
			pClient->sendingQueue[j]=PX_SYNCDATA_INVALID_QUERY_INDEX;
	}

	s->size=size;
	s->blockCount=size%PX_SYNCDATA_BLOCK_SIZE?size/PX_SYNCDATA_BLOCK_SIZE+1:size/PX_SYNCDATA_BLOCK_SIZE;
	if (s->alloc_datasize<size)
	{
		if(s->data)
			MP_Free(s->mp,s->data);

		s->data=(px_byte *)MP_Malloc(s->mp,size);
		if(!s->data) return PX_FALSE;
		s->alloc_datasize=size;
	}

	if (!s->data)
	{
		return PX_FALSE;
	}

	PX_memcpy(s->data,data,size);
	s->CRC32=PX_crc32(data,size);
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

		if(s->decode)
			if(s->decode(data,*readsize))
				return PX_TRUE;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_SyncDataServer_WriteBlock(PX_SyncData_Server *s,PX_Sync_Port port,px_byte *data,px_int bufferSize)
{
	px_byte sdata[PX_SYNCDATA_DATAGRAM_MAX_SIZE];

	if (bufferSize>PX_SYNCDATA_DATAGRAM_MAX_SIZE)
	{
		return PX_FALSE;
	}
	if (s->encode)
	{
		PX_memcpy(sdata,data,bufferSize);
		s->encode(sdata,bufferSize);
		if (s->write(s,port,sdata,bufferSize))
		{
			return PX_TRUE;
		}
	}
	else
	{
		if (s->write(s,port,data,bufferSize))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

PX_SyncData_Datagram R_datagram;
PX_SyncData_Datagram W_datagram;

px_bool PX_SyncDataServerUpdate(PX_SyncData_Server *s,px_int elpased)
{
	PX_Sync_Port port;
	px_int readSize,i;
	PX_SyncData_Server_Client *pClient=PX_NULL;

	PX_memset(&port,0,sizeof(port));

	if (PX_SyncDataServer_ReadBlock(s,&port,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
	{
		if (R_datagram.header.p_id==s->p_id)
		{
			if (R_datagram.header.opcode==PX_SYNCDATA_OPCODE_QUERY)
			{
				if (s->clients.size>s->max_Concurrency)
				{
					goto SEND_PROCESS;
				}
				//searching data
				for (i=0;i<s->clients.size;i++)
				{
					pClient=PX_VECTORAT(PX_SyncData_Server_Client,&s->clients,i);
					if (pClient->id==R_datagram.query.p_clientid)
					{
						if (PX_memequ(&pClient->port,&port,sizeof(port)))
						{
							pClient->last_request_elpased=0;
						}
						else
						{
							PX_memcpy(&pClient->port,&port,sizeof(port));
						}
						break;
					}
				}

				if (i=!s->clients.size)
				{
					W_datagram.response.opcode=PX_SYNCDATA_OPCODE_RESPONSE;
					W_datagram.response.p_id=s->p_id;
					W_datagram.response.CRC32=s->CRC32;
					W_datagram.response.size=s->size;
					if(!PX_SyncDataServer_WriteBlock(s,port,(px_byte *)&W_datagram.response,sizeof(W_datagram.response)))
					{
						goto SEND_PROCESS;
					}
				}
			}

			if (R_datagram.header.opcode==PX_SYNCDATA_OPCODE_REQUEST)
			{
				if (R_datagram.request.CRC32==s->CRC32)
				{
					for (i=0;i<s->clients.size;i++)
					{
						pClient=PX_VECTORAT(PX_SyncData_Server_Client,&s->clients,i);

						if (PX_memequ(&pClient->port,&port,sizeof(port)))
						{
							int j=0;
							pClient->last_request_elpased=0;
							pClient->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZED;
							for(i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
							{
								if (R_datagram.request.block[i]!=PX_SYNCDATA_INVALID_QUERY_INDEX)
								{
									pClient->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING;
									break;
								}
							}
							if (i!=PX_SYNCDATA_CACHE_SIZE)
							{
								for (i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
								{
									if (R_datagram.request.block[i]>=0&&(px_int)R_datagram.request.block[i]<s->blockCount)
									{
										pClient->sendingQueue[j++]=R_datagram.request.block[i];
									}
								}
							}
							break;
						}
					}
				}
				else
				{
					W_datagram.response.opcode=PX_SYNCDATA_OPCODE_RESPONSE;
					W_datagram.response.p_id=s->p_id;
					W_datagram.response.CRC32=s->CRC32;
					W_datagram.response.size=s->size;
					if(!PX_SyncDataServer_WriteBlock(s,port,(px_byte *)&W_datagram.response,sizeof(W_datagram.response)))
					{
						goto SEND_PROCESS;
					}

				}
			}

		}
	}

	//sending
	SEND_PROCESS:
	for (i=0;i<s->clients.size;i++)
	{
		pClient->last_request_elpased+=elpased;
	

		if(pClient->send_elpased>=PX_SYNCDATA_SENDING_DURATION)
		{
			pClient->send_elpased=0;
			for (i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
			{
				if (pClient->queueindex>=PX_SYNCDATA_CACHE_SIZE)
				{
					pClient->queueindex=0;
				}

				while(!pClient->sendingQueue[pClient->queueindex])
				{
					pClient->queueindex++;
				}

				if (pClient->sendingQueue[pClient->queueindex]>=0&&pClient->sendingQueue[pClient->queueindex]<s->blockCount&&elpased)
				{
					

					if(pClient->sendingQueue[pClient->queueindex]!=s->blockCount-1)
						PX_memcpy(W_datagram.data.data,s->data+pClient->sendingQueue[pClient->queueindex]*PX_SYNCDATA_BLOCK_SIZE,PX_SYNCDATA_BLOCK_SIZE);
					else
						PX_memcpy(W_datagram.data.data,s->data+pClient->sendingQueue[pClient->queueindex]*PX_SYNCDATA_BLOCK_SIZE,s->size-PX_SYNCDATA_BLOCK_SIZE*(s->blockCount-1));

					//printf("send block %d\n",trans->sendingQueue[trans->queueindex]);
					pClient->sendingQueue[pClient->queueindex]=PX_SYNCDATA_INVALID_QUERY_INDEX;
					W_datagram.data.opcode=PX_SYNCDATA_OPCODE_DATAGRAM;
					W_datagram.data.CRC32=s->CRC32;
					W_datagram.data.p_id=s->p_id;
					W_datagram.data.blockIndex=pClient->sendingQueue[pClient->queueindex];
					PX_SyncDataServer_WriteBlock(s,pClient->port,(px_byte *)&W_datagram.data,sizeof(W_datagram.data));
					pClient->queueindex++;
					break;
				}

			}
		}
		else
		{
			pClient->send_elpased+=elpased;
		}
	}
	return PX_TRUE;
}

px_void PX_SyncDataServerFree(PX_SyncData_Server *syncdata_server)
{
	PX_VectorFree(&syncdata_server->clients);
}


px_bool PX_SyncDataClientInit(px_memorypool *mp,px_dword pid,px_dword server_client_pid,PX_SyncData_Client *syncdata_client,PX_Sync_Port server_port,px_syncdata_client_write write,px_syncdata_client_read read,PX_SyncData_Encode encode,PX_SyncData_Decode decode)
{
	px_int i;
	syncdata_client->alloc_datasize=0;
	syncdata_client->blockCount=0;
	syncdata_client->data=PX_NULL;
	syncdata_client->decode=decode;
	syncdata_client->encode=encode;
	syncdata_client->mp=mp;
	syncdata_client->offset=0;
	syncdata_client->CRC32=0;
	syncdata_client->p_id=pid;
	syncdata_client->read=read;
	syncdata_client->write=write;
	syncdata_client->elpased=0;
	syncdata_client->last_recv_elpased;
	syncdata_client->size=0;
	syncdata_client->p_server_client_id=server_client_pid;
	syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_QUERY;
	for(i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
	{
		syncdata_client->requestBlock[i].blockIndex=PX_SYNCDATA_INVALID_QUERY_INDEX;
	}
    return PX_TRUE;
}

px_bool PX_SyncDataClientDoSync(PX_SyncData_Client *syncdata_client)
{
	syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_QUERY;
	return PX_TRUE;
}

px_bool PX_SyncDataClient_ReadBlock(PX_SyncData_Client *s,PX_Sync_Port *port,px_byte *data,px_int bufferSize,px_int *readsize)
{
	if (s->read(s,port,data,bufferSize,readsize))
	{
		if (*readsize>bufferSize)
		{
			return PX_FALSE;
		}

		if(s->decode)
			if(s->decode(data,*readsize))
				return PX_TRUE;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_SyncDataClient_WriteBlock(PX_SyncData_Client *s,PX_Sync_Port port,px_byte *data,px_int bufferSize)
{
	px_byte sdata[PX_SYNCDATA_DATAGRAM_MAX_SIZE];

	if (bufferSize>PX_SYNCDATA_DATAGRAM_MAX_SIZE)
	{
		return PX_FALSE;
	}
	if (s->encode)
	{
		PX_memcpy(sdata,data,bufferSize);
		s->encode(sdata,bufferSize);
		if (s->write(s,port,sdata,bufferSize))
		{
			return PX_TRUE;
		}
	}
	else
	{
		if (s->write(s,port,data,bufferSize))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_SyncDataClientResetData(PX_SyncData_Client *syncdata_client,px_uint size,px_uint32 CRC32)
{
	px_int i;
	if (syncdata_client->alloc_datasize<size)
	{
		MP_Free(syncdata_client->mp,syncdata_client->data);
		syncdata_client->data=(px_byte *)MP_Malloc(syncdata_client->mp,size);
		if (!syncdata_client->data)
		{
			syncdata_client->data=PX_NULL;
			syncdata_client->alloc_datasize=0;
			PX_ERROR("Sync data alloc memory error!");
			return PX_FALSE;
		}
		syncdata_client->alloc_datasize=size;
	}

	syncdata_client->size=size;
	syncdata_client->blockCount=size%PX_SYNCDATA_BLOCK_SIZE?size/PX_SYNCDATA_BLOCK_SIZE+1:size/PX_SYNCDATA_BLOCK_SIZE;
	syncdata_client->offset=0;
	syncdata_client->CRC32=CRC32;
	syncdata_client->acceptBlock=0;

	for(i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
	{
		syncdata_client->requestBlock[i].blockIndex=PX_SYNCDATA_INVALID_QUERY_INDEX;
	}
	return PX_TRUE;
}

px_bool PX_SyncDataClientUpdate(PX_SyncData_Client *syncdata_client,px_int elpased)
{
	PX_Sync_Port r_port;
	px_int readSize,i;

	syncdata_client->last_recv_elpased+=elpased;

	switch (syncdata_client->status)
	{
	case PX_SYNCDATA_CLIENT_STATUS_QUERY:	
		if (syncdata_client->elpased>PX_SYNCDATA_QUERY_DURATION)
		{
			syncdata_client->elpased=0;
			W_datagram.query.opcode=PX_SYNCDATA_OPCODE_QUERY;
			W_datagram.query.CRC32=syncdata_client->CRC32;
			W_datagram.query.p_id=syncdata_client->p_id;
			W_datagram.query.p_clientid=syncdata_client->p_server_client_id;
			PX_SyncDataClient_WriteBlock(syncdata_client,syncdata_client->port,(px_byte *)&W_datagram.query,sizeof(W_datagram.query));
		}
		else
		{
			syncdata_client->elpased+=elpased;
		}

		PX_memset(&r_port,0,sizeof(r_port));

		if (PX_SyncDataClient_ReadBlock(syncdata_client,&r_port,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
		{
			if (R_datagram.header.p_id==syncdata_client->p_id)
			{
				if (R_datagram.header.opcode==PX_SYNCDATA_OPCODE_RESPONSE)
				{
					syncdata_client->last_recv_elpased=0;
					if (!PX_SyncDataClientResetData(syncdata_client,R_datagram.response.size,R_datagram.response.CRC32))
					{
						return PX_FALSE;
					}
					syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING;
				}
			}
			
		}
		break;
	case PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING:
		syncdata_client->last_recv_elpased+=elpased;

		if (PX_SyncDataClient_ReadBlock(syncdata_client,&r_port,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
		{
			if (R_datagram.data.p_id==syncdata_client->p_id&&R_datagram.header.opcode==PX_SYNCDATA_OPCODE_DATAGRAM)
			{
				syncdata_client->last_recv_elpased=0;

				if (R_datagram.data.CRC32!=syncdata_client->CRC32)
				{
					break;
				}
				for (i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
				{
					if (syncdata_client->requestBlock[i].blockIndex==R_datagram.data.blockIndex)
					{
						//copy data to buffer
						//printf("accept block %d\n",R_datagram.data.blockIndex);
						syncdata_client->last_recv_elpased=0;
						PX_memcpy(syncdata_client->data+R_datagram.data.blockIndex*PX_SYNCDATA_BLOCK_SIZE,R_datagram.data.data,PX_SYNCDATA_BLOCK_SIZE);

						if (syncdata_client->offset<syncdata_client->size)
						{
							syncdata_client->requestBlock[i].blockIndex=syncdata_client->offset/PX_SYNCDATA_BLOCK_SIZE;
							syncdata_client->offset+=PX_SYNCDATA_BLOCK_SIZE;
						}
						else
						{
							syncdata_client->acceptBlock++;
							syncdata_client->requestBlock[i].blockIndex=PX_SYNCDATA_INVALID_QUERY_INDEX;
						}
						break;
					}
				}
			}

			if (R_datagram.data.p_id==syncdata_client->p_id&&R_datagram.header.opcode==PX_SYNCDATA_OPCODE_RESPONSE)
			{
				if (R_datagram.response.CRC32!=syncdata_client->CRC32)
				{
					syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_QUERY;
					break;
				}
			}
		}

		if (syncdata_client->acceptBlock<syncdata_client->blockCount)
		{
			if (syncdata_client->elpased>PX_SYNCDATA_REQUEST_DURATION)
			{
				px_int i,o=0;
				PX_SyncData_Request request;
				syncdata_client->elpased=0;
				for (i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
				{
					if (syncdata_client->requestBlock[i].blockIndex!=0)
					{
						request.block[o++]=syncdata_client->requestBlock[i].blockIndex;
					}
					else
					{
						request.block[o++]=PX_SYNCDATA_INVALID_QUERY_INDEX;
					}
				}
				request.p_id=syncdata_client->p_id;
				request.opcode=PX_SYNCDATA_OPCODE_REQUEST;
				request.CRC32=syncdata_client->CRC32;
				PX_SyncDataClient_WriteBlock(syncdata_client,syncdata_client->port,(px_byte *)&request,sizeof(request));
			}
			else
			{
				syncdata_client->elpased+=elpased;
			}
		}
		else
		{
			syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZED;

		}
		break;
	case PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZED:

		if (PX_SyncDataClient_ReadBlock(syncdata_client,&r_port,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
		{
			syncdata_client->last_recv_elpased=0;
			if (R_datagram.data.p_id==syncdata_client->p_id&&R_datagram.header.opcode==PX_SYNCDATA_OPCODE_RESPONSE)
			{
				if (R_datagram.response.CRC32!=syncdata_client->CRC32)
				{
					syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_QUERY;
					break;
				}
			}
		}

		if (syncdata_client->elpased>PX_SYNCDATA_CHECK_DURATION)
		{
			px_int i;
			PX_SyncData_Request request;
			syncdata_client->elpased=0;
			for (i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
			{
				request.block[i]=PX_SYNCDATA_INVALID_QUERY_INDEX;
			}
			request.p_id=syncdata_client->p_id;
			request.opcode=PX_SYNCDATA_OPCODE_REQUEST;
			request.CRC32=syncdata_client->CRC32;
			PX_SyncDataClient_WriteBlock(syncdata_client,syncdata_client->port,(px_byte *)&request,sizeof(request));
		}
		else
		{
			syncdata_client->elpased+=elpased;
		}
		break;
	}
	return PX_TRUE;
}

px_void PX_SyncDataClientFree(PX_SyncData_Client *syncdata_client)
{
	if (syncdata_client->data)
	{
		px_int i;
		syncdata_client->alloc_datasize=0;
		syncdata_client->blockCount=0;
		syncdata_client->data=PX_NULL;
		syncdata_client->offset=0;
		syncdata_client->CRC32=0;
		syncdata_client->elpased=0;
		syncdata_client->last_recv_elpased;
		syncdata_client->size=0;
		syncdata_client->status=PX_SYNCDATA_CLIENT_STATUS_QUERY;
		for(i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
		{
			syncdata_client->requestBlock[i].blockIndex=PX_SYNCDATA_INVALID_QUERY_INDEX;
		}

		MP_Free(syncdata_client->mp,syncdata_client->data);
		syncdata_client->data=PX_NULL;
		syncdata_client->alloc_datasize=0;
	}
}

px_bool PX_SyncDataServerAddClient(PX_SyncData_Server *syncdata_server,px_dword p_id)
{
	px_int i;
	PX_SyncData_Server_Client newClient;
	newClient.id=p_id;
	newClient.last_request_elpased=0;
	PX_memset(&newClient.port,0,sizeof(newClient.port));
	newClient.queueindex=0;
	newClient.send_elpased=0;
	newClient.status=PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING;
	for(i=0;i<PX_SYNCDATA_CACHE_SIZE;i++)
	newClient.sendingQueue[i]=PX_SYNCDATA_INVALID_QUERY_INDEX;
	return PX_VectorPushback(&syncdata_server->clients,&newClient);
}


