#include "PX_TransmissionProtocal.h"

px_bool PX_TransmissionInit(px_memorypool *mp, PX_Transmission *T,px_dword bridge_id, PX_TransmissionProtocal_Interface_Write write, PX_TransmissionProtocal_Interface_Read read, PX_TransmissionProtocal_Interface_Encode encode, PX_TransmissionProtocal_Interface_Decode decode)
{
	T->decode=decode;
	T->encode=encode;
	T->offset=0;
	T->read=read;
	T->write=write;
	T->mp=mp;
	T->size=0;
	T->p_id=bridge_id;
	T->p_lastid=0;
	T->data=PX_NULL;
	T->alloc_datasize=0;
	T->sendingduration=0;
	T->queueindex=0;
	T->connectTry=0;
	T->max_recv_size=-1;
	PX_memset(T->sendingQueue,0,sizeof(T->sendingQueue));
	T->status=PX_TRANSMISSION_STATUS_STANDBY;
	return PX_TRUE;
}

px_bool PX_TransmissionSend(PX_Transmission *trans,px_byte *data,px_dword size)
{
	if (trans->status!=PX_TRANSMISSION_STATUS_STANDBY)
	{
		return PX_FALSE;
	}
	
	do
	{
		trans->p_id=PX_rand();
	}while(trans->p_id==trans->p_lastid);

	trans->size=size;
	trans->blockCount=size%PX_TRANSMISSION_BLOCK_SIZE?size/PX_TRANSMISSION_BLOCK_SIZE+1:size/PX_TRANSMISSION_BLOCK_SIZE;
	trans->elpased=0;
	trans->acceptBlock=0;
	trans->queueindex=0;
	if (trans->alloc_datasize<size)
	{
		if(trans->data)
		MP_Free(trans->mp,trans->data);

		trans->data=(px_byte *)MP_Malloc(trans->mp,size);
		if(!trans->data) return PX_FALSE;
		trans->alloc_datasize=size;
	}

	PX_memset(trans->requestBlock,0,sizeof(trans->requestBlock));
	
	if (!trans->data)
	{
		return PX_FALSE;
	}

	PX_memcpy(trans->data,data,size);
	trans->connectTry=0;
	trans->status=PX_TRANSMISSION_STATUS_CONNECT;
	trans->elpased=PX_TRANSMISSION_CONNECT_DURATION;
	return PX_TRUE;
}

px_bool PX_TransmissionReadBlock(PX_Transmission *trans,px_byte *data,px_int bufferSize,px_int *readsize)
{
	if (trans->read(data,bufferSize,readsize))
	{
		if (*readsize>bufferSize)
		{
			return PX_FALSE;
		}

		if(trans->decode)
		if(trans->decode(data,*readsize))
			return PX_TRUE;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_TransmissionWriteBlock(PX_Transmission *trans,px_byte *data,px_int bufferSize)
{
	px_byte sdata[PX_TRANSMISSION_DATAGRAM_MAX_SIZE];

	if (bufferSize>PX_TRANSMISSION_DATAGRAM_MAX_SIZE)
	{
		return PX_FALSE;
	}
	if (trans->encode)
	{
		PX_memcpy(sdata,data,bufferSize);
		trans->encode(sdata,bufferSize);
		if (trans->write(sdata,bufferSize))
		{
				return PX_TRUE;
		}
	}
	else
	{
		if (trans->write(data,bufferSize))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_TransmissionRequest(PX_Transmission *trans)
{
	PX_Transmission_Request request;
	px_int i,o=0;
	//printf("request ");
	for (i=0;i<PX_TRANSMISSION_CACHE_SIZE;i++)
	{
		if (trans->requestBlock[i].blockIndex!=0)
		{
			//printf("%d ",trans->requestBlock[i].blockIndex);
			request.block[o++]=trans->requestBlock[i].blockIndex;
		}
		else
		{
			request.block[o++]=0;
		}
	}
	request.p_id=trans->p_id;
	request.opcode=PX_TRANSMISSION_OPCODE_REQUEST;
	//printf("\n");
   return PX_TransmissionWriteBlock(trans,(px_byte *)&request,sizeof(request));
}

PX_Transmission_Datagram R_datagram;
PX_Transmission_Datagram W_datagram;


px_bool PX_TransmissionUpdate(PX_Transmission *trans,px_int elpased)
{
	px_int i,j;
	px_int readSize;
	px_bool allZero;
	px_bool onecConnect=PX_TRUE;
	switch(trans->status)
	{
	case PX_TRANSMISSION_STATUS_STANDBY:
		if (PX_TransmissionReadBlock(trans,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
		{

			if (readSize!=sizeof(R_datagram.connect))
			{
				break;
			}

			if (R_datagram.header.opcode==PX_TRANSMISSION_OPCODE_CONNECT&&R_datagram.header.p_id!=trans->p_lastid)
			{
				if (trans->max_recv_size>0&&R_datagram.connect.size>(px_dword)trans->max_recv_size)
				{
					break;
				}
				trans->status=PX_TRANSMISSION_STATUS_RECEIVING;
				trans->p_lastid=R_datagram.header.p_id;
				trans->size=R_datagram.connect.size;
				trans->elpased=0;
				trans->blockCount=R_datagram.connect.size%PX_TRANSMISSION_BLOCK_SIZE?R_datagram.connect.size/PX_TRANSMISSION_BLOCK_SIZE+1:R_datagram.connect.size/PX_TRANSMISSION_BLOCK_SIZE;
				trans->offset=0;

				if (trans->p_id==0)
				{
					trans->p_id=R_datagram.connect.p_id;
				}
				else
				{
					if(trans->p_id!=R_datagram.connect.p_id)
						break;
				}
				

				if (trans->alloc_datasize<R_datagram.connect.size)
				{
					if (trans->data)
					{
						MP_Free(trans->mp,trans->data);
					}

					trans->data=(px_byte *)MP_Malloc(trans->mp,trans->blockCount*PX_TRANSMISSION_BLOCK_SIZE);
					
					if(!trans->data) 
					{
						trans->status=PX_TRANSMISSION_STATUS_ERROR;
						return PX_FALSE;
					}
					trans->alloc_datasize=trans->blockCount*PX_TRANSMISSION_BLOCK_SIZE;
				}
				
				if (!trans->data)
				{
					return PX_FALSE;
				}
				
				//Copy first block
				if(trans->size<=PX_TRANSMISSION_BLOCK_SIZE)
				{
					PX_memcpy(trans->data,R_datagram.connect.data,trans->size);
					trans->offset+=trans->size;
				}
				else
				{
					PX_memcpy(trans->data,R_datagram.connect.data,PX_TRANSMISSION_BLOCK_SIZE);
					trans->offset+=PX_TRANSMISSION_BLOCK_SIZE;
				}

				//initialize request blocks
				for (i=0;i<PX_TRANSMISSION_CACHE_SIZE;i++)
				{
					if (trans->offset>=trans->size)
					{
						break;
					}
					else
					{
						trans->requestBlock[i].blockIndex=trans->offset/PX_TRANSMISSION_BLOCK_SIZE;
						trans->offset+=PX_TRANSMISSION_BLOCK_SIZE;
					}
				}
				PX_memset(trans->sendingQueue,0,sizeof(trans->sendingQueue));
				trans->status=PX_TRANSMISSION_STATUS_RECEIVING;
				trans->recvElpased=0;

				goto RECV_PROCESS;
			}
		}
		break;
	case PX_TRANSMISSION_STATUS_CONNECT:

		if (PX_TransmissionReadBlock(trans,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
		{
			if (R_datagram.header.p_id==trans->p_id)
			{
				if (R_datagram.header.opcode==PX_TRANSMISSION_OPCODE_DONE)
				{
					trans->status=PX_TRANSMISSION_STATUS_DONE;

					W_datagram.ack.opcode=PX_TRANSMISSION_OPCODE_DONE;
					W_datagram.ack.p_id=trans->p_id;

					if(!PX_TransmissionWriteBlock(trans,(px_byte *)&W_datagram.ack,sizeof(W_datagram.ack)))
					{
						trans->status=PX_TRANSMISSION_STATUS_ERROR;
						return PX_FALSE;
					}
					return PX_TRUE;
				}
				else if(R_datagram.header.opcode==PX_TRANSMISSION_OPCODE_REQUEST)
				{
					if (trans->size>PX_TRANSMISSION_BLOCK_SIZE)
					{
						trans->status=PX_TRANSMISSION_STATUS_SENDING;
						trans->elpased=0;
						
						trans->queueindex=0;
						goto SENDING_PROCESS;
					}
				}
			}
			
		}

		if (trans->elpased>=PX_TRANSMISSION_CONNECT_DURATION)
		{
			trans->elpased=0;

			if (trans->connectTry>=PX_TRANSMISSION_CONNECT_TRY)
			{
				trans->status=PX_TRANSMISSION_STATUS_ERROR;
				return PX_FALSE;
			}

			W_datagram.connect.p_id=trans->p_id;
			W_datagram.connect.opcode=PX_TRANSMISSION_OPCODE_CONNECT;
			W_datagram.connect.size=trans->size;
			if(trans->size<PX_TRANSMISSION_BLOCK_SIZE)
				PX_memcpy(W_datagram.connect.data,trans->data,trans->size);
			else
				PX_memcpy(W_datagram.connect.data,trans->data,PX_TRANSMISSION_BLOCK_SIZE);

			trans->connectTry++;
			if(!PX_TransmissionWriteBlock(trans,(px_byte *)&W_datagram.connect,sizeof(W_datagram.connect)))
			{
				trans->status=PX_TRANSMISSION_STATUS_ERROR;
				return PX_FALSE;
			}
		}
		else
		{
			trans->elpased+=elpased;
		}

		break;
	case PX_TRANSMISSION_STATUS_RECEIVING:
		{
			onecConnect=PX_FALSE;

			if (trans->recvElpased>PX_TRANSMISSION_RECVING_TIMEOUT)
			{
				trans->status=PX_TRANSMISSION_STATUS_ERROR;
				return PX_FALSE;
			}
			else
			{
				trans->recvElpased+=elpased;
			}

			//data recv
			if (PX_TransmissionReadBlock(trans,(px_byte *)&R_datagram.data,sizeof(R_datagram.data),&readSize))
			{
				if (R_datagram.data.p_id==trans->p_id&&R_datagram.header.opcode==PX_TRANSMISSION_OPCODE_DATAGRAM)
				{
					for (i=0;i<PX_TRANSMISSION_CACHE_SIZE;i++)
					{
						if (trans->requestBlock[i].blockIndex==R_datagram.data.blockIndex)
						{
							//copy data to buffer
							//printf("accept block %d\n",R_datagram.data.blockIndex);
							trans->recvElpased=0;
							PX_memcpy(trans->data+R_datagram.data.blockIndex*PX_TRANSMISSION_BLOCK_SIZE,R_datagram.data.data,PX_TRANSMISSION_BLOCK_SIZE);

							if (trans->offset<trans->size)
							{
								trans->requestBlock[i].blockIndex=trans->offset/PX_TRANSMISSION_BLOCK_SIZE;
								trans->offset+=PX_TRANSMISSION_BLOCK_SIZE;
							}
							else
							{
								trans->acceptBlock++;
								trans->requestBlock[i].blockIndex=0;
							}
							break;
						}
					}
				}
			}
			
			//date requests
			RECV_PROCESS:
			allZero=PX_TRUE;
			for (i=0;i<PX_TRANSMISSION_CACHE_SIZE;i++)
			{
				if (trans->requestBlock[i].blockIndex!=0)
				{
					allZero=PX_FALSE;
					break;
				}
			}
			
			
			if (allZero)
			{
				if(onecConnect)
				{
					trans->elpased=0;
					trans->status=PX_TRANSMISSION_STATUS_DONE;

					W_datagram.ack.opcode=PX_TRANSMISSION_OPCODE_DONE;
					W_datagram.ack.p_id=trans->p_id;

					if(!PX_TransmissionWriteBlock(trans,(px_byte *)&W_datagram.ack,sizeof(W_datagram.ack)))
					{
						trans->status=PX_TRANSMISSION_STATUS_ERROR;
						return PX_FALSE;
					}
				}
				else
				{
					//printf("Wait for close\n");
					trans->elpased=0;
					trans->status=PX_TRANSMISSION_STATUS_WAITFORCLOSE;
				}
				
			}
			else
			{
				if (trans->elpased>PX_TRANSMISSION_REQUEST_DURATION)
				{
					trans->elpased=0;
					
					if(!PX_TransmissionRequest(trans))
					{
						trans->status=PX_TRANSMISSION_STATUS_ERROR;
						return PX_FALSE;
					}
				}
				else
				{
					trans->elpased+=elpased;
				}
				
			}
			
		}
		break;
	case PX_TRANSMISSION_STATUS_WAITFORCLOSE:
		{
			if (trans->elpased>PX_TRANSMISSION_CLOSETIME)
			{
				//printf("timeout close\n");
				trans->status=PX_TRANSMISSION_STATUS_DONE;
			}
			else
			{
				trans->elpased+=elpased;
			}
			if(elpased)
			{
				W_datagram.ack.opcode=PX_TRANSMISSION_OPCODE_DONE;
				W_datagram.ack.p_id=trans->p_id;

				if(!PX_TransmissionWriteBlock(trans,(px_byte *)&W_datagram.ack,sizeof(W_datagram.ack)))
				{
					trans->status=PX_TRANSMISSION_STATUS_ERROR;
					return PX_FALSE;
				}
			}
			if (PX_TransmissionReadBlock(trans,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
			{
				if (R_datagram.header.p_id==trans->p_id)
				{
					if (R_datagram.header.opcode==PX_TRANSMISSION_OPCODE_DONE)
					{
						//printf("accept close\n");
						trans->status=PX_TRANSMISSION_STATUS_DONE;
					}
				}
			}
		}
		break;
	case PX_TRANSMISSION_STATUS_SENDING:
		{
			trans->elpased+=elpased;

			if (trans->elpased>=PX_TRANSMISSION_SENDING_TIMEOUT)
			{
				trans->status=PX_TRANSMISSION_STATUS_ERROR;
				return PX_FALSE;
			}


			if (PX_TransmissionReadBlock(trans,(px_byte *)&R_datagram,sizeof(R_datagram),&readSize))
			{
				SENDING_PROCESS:
				if (R_datagram.header.p_id==trans->p_id)
				{
					if (R_datagram.header.opcode==PX_TRANSMISSION_OPCODE_DONE)
					{
						//printf("send done\n");

						W_datagram.ack.opcode=PX_TRANSMISSION_OPCODE_DONE;
						W_datagram.ack.p_id=trans->p_id;

						if(!PX_TransmissionWriteBlock(trans,(px_byte *)&W_datagram.ack,sizeof(W_datagram.ack)))
						{
							trans->status=PX_TRANSMISSION_STATUS_ERROR;
							return PX_FALSE;
						}

						trans->status=PX_TRANSMISSION_STATUS_DONE;
						break;
					}
					if (R_datagram.header.opcode==PX_TRANSMISSION_OPCODE_REQUEST)
					{
						trans->elpased=0;
						W_datagram.data.p_id=trans->p_id;
						W_datagram.data.opcode=PX_TRANSMISSION_OPCODE_DATAGRAM;
						j=0;
						for (i=0;i<PX_TRANSMISSION_CACHE_SIZE;i++)
						{
							if (R_datagram.request.block[i]&&(px_int)R_datagram.request.block[i]<trans->blockCount)
							{
								trans->sendingQueue[j++]=R_datagram.request.block[i];
							}
						}

					}
					
				}
			}

			if(trans->sendingduration>=PX_TRANSMISSION_SENDING_DURATION)
			{
				trans->sendingduration=0;
				for (i=0;i<PX_TRANSMISSION_CACHE_SIZE;i++)
				{
					if (trans->queueindex>=PX_TRANSMISSION_CACHE_SIZE)
					{
						trans->queueindex=0;
					}
					while(!trans->sendingQueue[trans->queueindex])
					{
						trans->queueindex++;
					}
					if (trans->sendingQueue[trans->queueindex]<trans->blockCount&&elpased)
					{
						W_datagram.data.blockIndex=trans->sendingQueue[trans->queueindex];
						if(trans->sendingQueue[trans->queueindex]!=trans->blockCount-1)
							PX_memcpy(W_datagram.data.data,trans->data+trans->sendingQueue[trans->queueindex]*PX_TRANSMISSION_BLOCK_SIZE,PX_TRANSMISSION_BLOCK_SIZE);
						else
							PX_memcpy(W_datagram.data.data,trans->data+trans->sendingQueue[trans->queueindex]*PX_TRANSMISSION_BLOCK_SIZE,trans->size-PX_TRANSMISSION_BLOCK_SIZE*(trans->blockCount-1));

						//printf("send block %d\n",trans->sendingQueue[trans->queueindex]);
						trans->sendingQueue[trans->queueindex]=0;
						PX_TransmissionWriteBlock(trans,(px_byte *)&W_datagram.data,sizeof(W_datagram.data));
						trans->queueindex++;
						break;
					}

				}
			}
			else
			{
				trans->sendingduration+=elpased;
			}

		}
		break;
	case PX_TRANSMISSION_STATUS_DONE:
	default:
		break;
	}
	return PX_TRUE;
}

px_void PX_TransmissionCompleted(PX_Transmission *trans)
{
	trans->offset=0;
	trans->size=0;
	trans->acceptBlock=0;
	trans->sendingduration=0;
	trans->queueindex=0;
	trans->status=PX_TRANSMISSION_STATUS_STANDBY;
	PX_memset(trans->requestBlock,0,sizeof(trans->requestBlock));
	PX_memset(trans->sendingQueue,0,sizeof(trans->sendingQueue));
}

px_void PX_TransmissionFree(PX_Transmission *trans)
{
	if(trans->data)
		MP_Free(trans->mp,trans->data);
}

px_bool PX_TransmissionRecv(PX_Transmission *trans,px_byte *data,px_dword size)
{
	
	if (trans->status==PX_TRANSMISSION_STATUS_DONE)
	{
		if (size<trans->size)
		{
			return PX_FALSE;
		}
		trans->acceptBlock=0;
		PX_memcpy(data,trans->data,trans->size);
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_TramsmissionSetMaxRecvSize(PX_Transmission *trans,px_int size)
{
	trans->max_recv_size=size;
}
