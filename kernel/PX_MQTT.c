#include "PX_MQTT.h"
px_int PX_MQTT_GetSize(px_byte *sizeBuffer)
{
	px_int multiplier=1;
	px_int value=0;
	px_byte encodeByte;
	px_int i=0;
	do 
	{
		encodeByte=sizeBuffer[i];
		i++;
		value+=(encodeByte&127)*multiplier;
		multiplier*=128;
		if (multiplier>128*128*128)
		{
			return 0;
		}
	} while ((encodeByte&128)!=0);
	return value;
}
px_uint PX_MQTT_GetContentSizeOfSize(px_int size)
{
	px_int len=0;
	do 
	{
		size/=128;
		len++;
	} while (size>0);
	return len;
}
px_uint PX_MQTTCatRemainingLength(px_memory *memory,px_int size)
{
	px_int encode=0;
	px_int len=0;
	do 
	{
		encode=size%128;
		size/=128;
		if(size>0)
		{
			encode|=128;
		}
		if(!PX_MemoryCat(memory,&encode,1)) return 0;
		len++;
	} while (size>0);
	return len;
}
px_bool PX_MQTTCatUTF8String(px_memory *memory,const px_char *pstring)
{
	px_word size=PX_strlen(pstring);
	px_byte sizeEncode[2];
	sizeEncode[0]=(px_byte)((size&0xff00)>>8);
	sizeEncode[1]=(px_byte)((size&0xff));
	if(!PX_MemoryCat(memory,&sizeEncode,2))return PX_FALSE;
	return PX_MemoryCat(memory,pstring,PX_strlen(pstring));
}
px_bool PX_MQTTCatData(px_memory *memory,const px_void *payload,px_int size)
{
	if(!PX_MQTTCatRemainingLength(memory,size)) return PX_FALSE;
	return PX_MemoryCat(memory,payload,size);
}
px_bool PX_MQTTInitialize(PX_MQTT *Mqtt,px_memorypool *buildmp,PX_Linker *linker)
{
	PX_memset(Mqtt,0,sizeof(PX_MQTT));
	Mqtt->linker=linker;
	Mqtt->buildmp=buildmp;
	PX_MemoryInitialize(buildmp,&Mqtt->topic);
	PX_MemoryInitialize(buildmp,&Mqtt->payload);
	return PX_TRUE;
}
PX_MQTT_CONNECT PX_MQTT_WaitConnACK(PX_MQTT *Mqtt)
{
	px_byte readBuffer[4];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_MQTT_CONNECT_ERROR_DISCONNECT;
	}
	if (readSize==4)
	{
		typedef struct  
		{
			px_byte byte1;
			px_byte byte2;
			px_byte sp;
			px_byte ret;
		}PX_MQTT_CONNACK;
		PX_MQTT_CONNACK *pACK=(PX_MQTT_CONNACK *)readBuffer;

		if (pACK->ret==0)
		{
			return PX_MQTT_CONNECT_SUCCEEDED;
		}
		else
		{
			switch(pACK->ret)
			{
			case 0x01:
				{
					return PX_MQTT_CONNECT_ERROR_PROTOCAL_NO_SUPPORT;
				}
				//break;
			case 0x02:
				{
					return PX_MQTT_CONNECT_ERROR_SESSION_ILLEGAL;
				}
				//break;
			case 0x03:
				{
					return PX_MQTT_CONNECT_ERROR_SERVER_CRASH;
				}
				//break;
			case 0x04:
				{
					return PX_MQTT_CONNECT_ERROR_USER_WRONG;
				}
				//break;
			case 0x05:
				{
					return PX_MQTT_CONNECT_ERROR_ILLEGAL;
				}
				//break;
			default:
				return PX_MQTT_CONNECT_ERROR_UNKNOW;
				//break;
			}
		}

	}
	return PX_MQTT_CONNECT_ERROR_UNKNOW;
}
px_bool PX_MQTT_SendConnect(PX_MQTT *Mqtt,PX_MQTT_ConnectDesc connectDesc)
{
	px_memory packbuild;
	px_uint size=0;
	px_int i;
	//Generate Session
	for (i=0;i<PX_COUNTOF(Mqtt->connectionKeySession)-1;i++)
	{
		Mqtt->connectionKeySession[i]='a'+PX_rand()%26;
	}
	Mqtt->connectionKeySession[i]=0;
	
	PX_MemoryInitialize(Mqtt->buildmp,&packbuild);
	//fixed header
	do
	{
		typedef struct
		{
			px_byte flag:4;
			px_byte type:4;
		}PX_MQTT_FixedHeader;

		PX_MQTT_FixedHeader header;
		header.flag=0;
		header.type=PX_MQTT_FIXERHEADER_CONTROLLERTYPE_CONNECT;

		//10 bytes for variable header
		size+=10;

		//payload size
		
		//connectionSession + 2byte size
		size+=PX_MQTT_CONNECTION_SESSION_SIZE+2;

		if (connectDesc.willTopic&&connectDesc.willTopic[0])
		{
			size+=PX_strlen(connectDesc.willTopic)+2;
		}

		if (connectDesc.willContent&&connectDesc.willSize)
		{
			size+=connectDesc.willSize+2;
		}


		if (connectDesc.userName)
		{
			size+=PX_strlen(connectDesc.userName)+2;
		}

		if (connectDesc.password)
		{
			size+=PX_strlen(connectDesc.password)+2;
		}

		
		PX_MemoryCat(&packbuild,&header,sizeof(header));
		PX_MQTTCatRemainingLength(&packbuild,size);

	}while(0);

	//Variable Header

	//Protocol Name 6 bytes
	do 
	{
		typedef struct
		{
			px_byte size_MSB;
			px_byte size_LSB;
			px_byte cMQTT[4];
		}PX_MQTT_VariableHeader;

		PX_MQTT_VariableHeader variableHeader;
		variableHeader.size_MSB=0;
		variableHeader.size_LSB=4;
		PX_memcpy(variableHeader.cMQTT,"MQTT",4);

		PX_MemoryCat(&packbuild,&variableHeader,sizeof(variableHeader));
	} while (0);
	
	//Protocol level byte 1 bytes
	do 
	{
		typedef struct
		{
			px_byte level;
		}PX_MQTT_ProtocalLevel;

		PX_MQTT_ProtocalLevel Protocallevel;
		Protocallevel.level=0x04;

		PX_MemoryCat(&packbuild,&Protocallevel,sizeof(Protocallevel));
	} while (0);

	//Connection flag 1 bytes
	do 
	{
		typedef struct
		{
			px_byte reserved:1;
			px_byte cleanSession:1;
			px_byte Willflag:1;
			px_byte WillQoS:2;
			px_byte WillRetain:1;
			px_byte PasswordFlag:1;
			px_byte UserNameFlag:1;
		}PX_MQTT_ConnectionFlag;

		PX_MQTT_ConnectionFlag ConnectionFlag;
		ConnectionFlag.reserved=0;
		ConnectionFlag.cleanSession=connectDesc.CleanSession;
		if (connectDesc.willSize&&connectDesc.willContent)
		{
			ConnectionFlag.Willflag=1;
		}
		else
		{
			ConnectionFlag.Willflag=0;
		}

		ConnectionFlag.WillQoS=connectDesc.willQoS;
		ConnectionFlag.WillRetain=connectDesc.willRetain;
		
		if (connectDesc.userName)
		{
			ConnectionFlag.UserNameFlag=1;
		}
		else
		{
			ConnectionFlag.UserNameFlag=0;
		}


		if (connectDesc.password)
		{
			ConnectionFlag.PasswordFlag=1;
		}
		else
		{
			ConnectionFlag.PasswordFlag=0;
		}

		PX_MemoryCat(&packbuild,&ConnectionFlag,sizeof(ConnectionFlag));
	} while (0);


	//KeepAlive time 2 Bytes
	do 
	{
		typedef struct
		{
			px_byte Alive_MSB;
			px_byte Alive_LSB;
		}PX_MQTT_ConnectionAlive;

		PX_MQTT_ConnectionAlive ConnectionAlive;

		ConnectionAlive.Alive_MSB=(connectDesc.KeepAliveTime&0xff00)>>8;
		ConnectionAlive.Alive_LSB=(connectDesc.KeepAliveTime&0xff);

		PX_MemoryCat(&packbuild,&ConnectionAlive,sizeof(ConnectionAlive));
	} while (0);

	//ClientID
	do 
	{
		PX_MQTTCatUTF8String(&packbuild,Mqtt->connectionKeySession);
	} while (0);

	//will topic
	do 
	{
		if (connectDesc.willTopic&&connectDesc.willTopic[0])
		{
			PX_MQTTCatUTF8String(&packbuild,connectDesc.willTopic);
		}
		
	} while (0);


	//will content
	do 
	{
		if (connectDesc.willContent)
		{
			px_word willsize=connectDesc.willSize;
			px_byte willsizeEncode[2];
			willsizeEncode[0]=(px_byte)((willsize&0xff00)>>8);
			willsizeEncode[1]=(px_byte)((willsize&0xff));
			PX_MemoryCat(&packbuild,willsizeEncode,2);
			PX_MemoryCat(&packbuild,connectDesc.willContent,willsize);
		}

	} while (0);

	//userName
	do 
	{
		if (connectDesc.userName)
		{
			PX_MQTTCatUTF8String(&packbuild,connectDesc.userName);
		}
	} while (0);

	//password
	do 
	{
		if (connectDesc.password)
		{
			PX_MQTTCatUTF8String(&packbuild,connectDesc.password);
		}
		
	} while (0);
	
	if(!PX_LinkerWrite(Mqtt->linker,packbuild.buffer,packbuild.usedsize))
	{
		PX_MemoryFree(&packbuild);
		return PX_FALSE;
	}
	PX_MemoryFree(&packbuild);
	return PX_TRUE;
}
PX_MQTT_CONNECT PX_MQTTConnect(PX_MQTT *Mqtt,PX_MQTT_ConnectDesc connectDesc)
{
	if (PX_MQTT_SendConnect(Mqtt,connectDesc))
	{
		return PX_MQTT_WaitConnACK(Mqtt);
	}
	return PX_MQTT_CONNECT_ERROR_DISCONNECT;
}
px_bool PX_MQTT_SendPublih(PX_MQTT *Mqtt,PX_MQTT_PublishDesc publishDesc)
{
	px_memory packbuild;
	PX_MemoryInitialize(Mqtt->buildmp,&packbuild);

	//Fixed Header
	do 
	{
		typedef struct
		{
			unsigned int ratain:1;
			unsigned int QoS:2;
			unsigned int DUP:1;
			unsigned int Type:4;
		}PX_MQTT_FixedHeader;
		px_int size=0;
		PX_MQTT_FixedHeader header;

		header.ratain=publishDesc.retain;
		header.DUP=0;
		header.QoS=publishDesc.qosLevel;
		header.Type=0x03;

		PX_MemoryCat(&packbuild,&header,1);

		//variable header
		size+=PX_strlen(publishDesc.Topic)+2;

		//Packet Identifier
		if (publishDesc.qosLevel==PX_MQTT_QOS_LEVEL_1||publishDesc.qosLevel==PX_MQTT_QOS_LEVEL_2)
		{
			size+=2;
		}

		size+=publishDesc.payloadSize;

		PX_MQTTCatRemainingLength(&packbuild,size);

	} while (0);

	//variable header
	do
	{
		PX_MQTTCatUTF8String(&packbuild,publishDesc.Topic);
		if (publishDesc.qosLevel==PX_MQTT_QOS_LEVEL_1||publishDesc.qosLevel==PX_MQTT_QOS_LEVEL_2)
		{
			Mqtt->identify++;
			PX_MemoryCat(&packbuild,&Mqtt->identify,2);
			
		}
	}while(0);

	//payload
	PX_MemoryCat(&packbuild,publishDesc.payload,publishDesc.payloadSize);

	if(!PX_LinkerWrite(Mqtt->linker,packbuild.buffer,packbuild.usedsize))
	{
		PX_MemoryFree(&packbuild);
		return PX_FALSE;
	}
	PX_MemoryFree(&packbuild);
	return PX_TRUE;
}
px_bool PX_MQTT_WaitPUBACK(PX_MQTT *Mqtt)
{
	px_byte readBuffer[4];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_FALSE;
	}
	if (readSize==4)
	{
		typedef struct
		{
			px_byte b1;
			px_byte b2;
			px_word identify;
		}PX_MQTT_PUBACK;
		PX_MQTT_PUBACK *pAck=(PX_MQTT_PUBACK *)readBuffer;
		if (pAck->b1==64&&pAck->b2==2&&pAck->identify==Mqtt->identify)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTT_SendPUBACK(PX_MQTT *Mqtt,px_word identify)
{
	typedef struct
	{
		px_byte b1;
		px_byte b2;
		px_word identify;
	}PX_MQTT_PUBACK;
	PX_MQTT_PUBACK Ack;
	Ack.b1=64;
	Ack.b2=2;
	Ack.identify=identify;

	return PX_LinkerWrite(Mqtt->linker,&Ack,sizeof(Ack));
}
px_bool PX_MQTT_WaitPUBREC(PX_MQTT *Mqtt)
{
	px_byte readBuffer[4];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_FALSE;
	}
	if (readSize==4)
	{
		typedef struct
		{
			px_byte b1;
			px_byte b2;
			px_word identify;
		}PX_MQTT_PUBACK;
		PX_MQTT_PUBACK *pAck=(PX_MQTT_PUBACK *)readBuffer;
		if (pAck->b1==(64+16)&&pAck->b2==2&&pAck->identify==Mqtt->identify)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTT_SendPUBREC(PX_MQTT *Mqtt,px_word identify)
{
	typedef struct
	{
		px_byte b1;
		px_byte b2;
		px_word identify;
	}PX_MQTT_PUBREC;
	PX_MQTT_PUBREC rec;
	rec.b1=(64+16);
	rec.b2=2;
	rec.identify=identify;
	return PX_LinkerWrite(Mqtt->linker,&rec,sizeof(rec));
}
px_bool PX_MQTT_SendPUBREL(PX_MQTT *Mqtt)
{
	typedef struct
	{
		px_byte b1;
		px_byte b2;
		px_word identify;
	}PX_MQTT_PUBREL;

	PX_MQTT_PUBREL pubrel;
	pubrel.b1=64+32+2;
	pubrel.b2=2;
	pubrel.identify=Mqtt->identify;
	if(!PX_LinkerWrite(Mqtt->linker,&pubrel,sizeof(pubrel)))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}
px_bool PX_MQTT_WaitPUBREL(PX_MQTT *Mqtt,px_word identify)
{
	px_byte readBuffer[4];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_FALSE;
	}
	if (readSize==4)
	{
		typedef struct
		{
			px_byte b1;
			px_byte b2;
			px_word identify;
		}PX_MQTT_PUBREL;
		PX_MQTT_PUBREL *pRel=(PX_MQTT_PUBREL *)readBuffer;
		if (pRel->b1==(64+32+2)&&pRel->b2==2&&pRel->identify==identify)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTT_SendPUBCOMP(PX_MQTT *Mqtt,px_word identify)
{
	typedef struct
	{
		px_byte b1;
		px_byte b2;
		px_word identify;
	}PX_MQTT_PUBCOMP;

	PX_MQTT_PUBCOMP pubcomp;
	pubcomp.b1=64+32+16;
	pubcomp.b2=2;
	pubcomp.identify=identify;
	if(!PX_LinkerWrite(Mqtt->linker,&pubcomp,sizeof(pubcomp)))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}
px_bool PX_MQTT_WaitPUBCOMP(PX_MQTT *Mqtt)
{
	px_byte readBuffer[4];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_FALSE;
	}
	if (readSize==4)
	{
		typedef struct
		{
			px_byte b1;
			px_byte b2;
			px_word identify;
		}PX_MQTT_PUBACK;
		PX_MQTT_PUBACK *pAck=(PX_MQTT_PUBACK *)readBuffer;
		if (pAck->b1==(64+32+16)&&pAck->b2==2&&pAck->identify==Mqtt->identify)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTTPublish(PX_MQTT *Mqtt,PX_MQTT_PublishDesc publishDesc)
{
	if(PX_MQTT_SendPublih(Mqtt,publishDesc))
	{
		switch(publishDesc.qosLevel)
		{
		case PX_MQTT_QOS_LEVEL_0:
			{
				return PX_TRUE;
			}
			//break;
		case PX_MQTT_QOS_LEVEL_1:
			{
				if (PX_MQTT_WaitPUBACK(Mqtt))
				{
					return PX_TRUE;
				}
			}
			break;
		case PX_MQTT_QOS_LEVEL_2:
			{
				if (PX_MQTT_WaitPUBREC(Mqtt))
				{
					if (PX_MQTT_SendPUBREL(Mqtt))
					{
						if (PX_MQTT_WaitPUBCOMP(Mqtt))
						{
							return PX_TRUE;
						}
					}
				}
				return PX_FALSE;
			}
			//break;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTT_SendSubscribe(PX_MQTT *Mqtt,PX_MQTT_SubscribeDesc subscribeDesc)
{
	px_memory packbuild;
	PX_MemoryInitialize(Mqtt->buildmp,&packbuild);

	//header
	do 
	{
		
		typedef struct
		{
			px_byte b1;
		}PX_MQTT_FixedHeader;
		px_int size=0;
		PX_MQTT_FixedHeader fixedHeader;
		fixedHeader.b1=128+2;
		
		PX_MemoryCat(&packbuild,&fixedHeader,1);

		//variable
		size+=2;

		//payload

		//topic
		size+=2+PX_strlen(subscribeDesc.Topic);

		//Requested
		size+=1;

		PX_MQTTCatRemainingLength(&packbuild,size);
	} while (0);
	
	//variable header
	do
	{
		Mqtt->identify++;
		PX_MemoryCat(&packbuild,&Mqtt->identify,2);
	}while(0);

	//payload
	
	//topic
	do
	{
		PX_MQTTCatUTF8String(&packbuild,subscribeDesc.Topic);
	}while(0);
	
	//qos
	do 
	{
		px_byte qos=subscribeDesc.qosLevel;
		PX_MemoryCat(&packbuild,&qos,1);
	} while (0);


	if(!PX_LinkerWrite(Mqtt->linker,packbuild.buffer,packbuild.usedsize))
	{
		PX_MemoryFree(&packbuild);
		return PX_FALSE;
	}
	PX_MemoryFree(&packbuild);
	return PX_TRUE;
	
}
px_bool PX_MQTT_WaitSUBACK(PX_MQTT *Mqtt)
{
	px_byte readBuffer[5];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_FALSE;
	}
	if (readSize==5)
	{
		typedef struct
		{
			px_byte b1;
			px_byte remainingLength;
			px_word identify;
			px_byte qos;
		}PX_MQTT_PUBACK;
		PX_MQTT_PUBACK *pAck=(PX_MQTT_PUBACK *)readBuffer;
		if (pAck->b1==(128+16)&&pAck->remainingLength==3&&pAck->identify==Mqtt->identify&&(pAck->qos&0x80)==0)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTTSubscribe(PX_MQTT *Mqtt,PX_MQTT_SubscribeDesc subscribeDesc)
{
	if (PX_MQTT_SendSubscribe(Mqtt,subscribeDesc))
	{
		if (PX_MQTT_WaitSUBACK(Mqtt))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTT_SendUnsubscribe(PX_MQTT *Mqtt,const px_char *Topic)
{
	px_memory packbuild;
	PX_MemoryInitialize(Mqtt->buildmp,&packbuild);

	//header
	do 
	{

		typedef struct
		{
			px_byte b1;
		}PX_MQTT_FixedHeader;
		px_int size=0;
		PX_MQTT_FixedHeader fixedHeader;
		fixedHeader.b1=128+32+2;

		PX_MemoryCat(&packbuild,&fixedHeader,1);

		//variable
		size+=2;

		//payload

		//topic
		size+=2+PX_strlen(Topic);

		PX_MQTTCatRemainingLength(&packbuild,size);
	} while (0);

	//variable header
	do
	{
		Mqtt->identify++;
		PX_MemoryCat(&packbuild,&Mqtt->identify,2);
	}while(0);

	//payload

	//topic
	do
	{
		PX_MQTTCatUTF8String(&packbuild,Topic);
	}while(0);

	if(!PX_LinkerWrite(Mqtt->linker,packbuild.buffer,packbuild.usedsize))
	{
		PX_MemoryFree(&packbuild);
		return PX_FALSE;
	}
	PX_MemoryFree(&packbuild);
	return PX_TRUE;

}
px_bool PX_MQTT_WaitUNSUBACK(PX_MQTT *Mqtt)
{
	px_byte readBuffer[4];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_FALSE;
	}
	if (readSize==4)
	{
		typedef struct
		{
			px_byte b1;
			px_byte remainingLength;
			px_word identify;
		}PX_MQTT_PUBACK;
		PX_MQTT_PUBACK *pAck=(PX_MQTT_PUBACK *)readBuffer;
		if (pAck->b1==(128+32+16)&&pAck->remainingLength==2&&pAck->identify==Mqtt->identify)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTTUnsubscribe(PX_MQTT *Mqtt,const px_char *Topic)
{
	if (PX_MQTT_SendUnsubscribe(Mqtt,Topic))
	{
		if (PX_MQTT_WaitUNSUBACK(Mqtt))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTT_SendPINGREQ(PX_MQTT *Mqtt)
{
	typedef struct  
	{
		px_byte b1;
		px_byte b2;
	}PX_MQTT_PINGREQ;

	PX_MQTT_PINGREQ pingreq;
	pingreq.b1=128+64+16;
	pingreq.b2=0;

	if(!PX_LinkerWrite(Mqtt->linker,&pingreq,2))
	{
		return PX_FALSE;
	}
	return PX_TRUE;
}
px_bool PX_MQTT_WaitPINGRESP(PX_MQTT *Mqtt)
{
	px_byte readBuffer[2];
	px_int readSize=PX_LinkerRead(Mqtt->linker,readBuffer,sizeof(readBuffer));
	if(readSize==0)
	{
		return PX_FALSE;
	}
	if (readSize==2)
	{
		typedef struct
		{
			px_byte b1;
			px_byte remainingLength;
		}PX_MQTT_PINGRESP;
		PX_MQTT_PINGRESP *pAck=(PX_MQTT_PINGRESP *)readBuffer;
		if (pAck->b1==192&&pAck->remainingLength==0)
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_MQTTPingReq(PX_MQTT *Mqtt)
{
	if (PX_MQTT_SendPINGREQ(Mqtt))
	{
		if (PX_MQTT_WaitPINGRESP(Mqtt))
		{
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

px_bool PX_MQTTListen(PX_MQTT *Mqtt)
{
	
	px_word id;
	px_byte rByte;
	px_int size=0;
	PX_MQTT_QOS_LEVEL qos;
	
	if (!PX_LinkerRead(Mqtt->linker,&rByte,1))
	{
		return PX_FALSE;
	}

	//is Publish?
	do 
	{
		typedef struct
		{
			unsigned int ratain:1;
			unsigned int QoS:2;
			unsigned int DUP:1;
			unsigned int Type:4;
		}PX_MQTT_FixedHeader;

		PX_MQTT_FixedHeader *pFixHeader=(PX_MQTT_FixedHeader *)&rByte;

		if (pFixHeader->Type!=3)
		{
			return PX_FALSE;
		}
		qos=(PX_MQTT_QOS_LEVEL)pFixHeader->QoS;
	} while (0);

	//getsize
	do 
	{
		px_int multiplier=1;
		while (PX_TRUE)
		{
			if (!PX_LinkerRead(Mqtt->linker,&rByte,1))
			{
				return PX_FALSE;
			}
			size+=(rByte&127)*multiplier;
			if ((rByte&128)!=0)
			{
				multiplier*=128;
			}
			else
			{
				break;
			}
			if (multiplier>128*128*128)
			{
				return PX_FALSE;
			}
		}
	} while (0);

	//recv Topic
	do 
	{
		px_int utf8Size=0;
		PX_MemoryClear(&Mqtt->topic);
		if (!PX_LinkerRead(Mqtt->linker,&rByte,1))
		{
			return PX_FALSE;
		}
		utf8Size=rByte*256;
		size--;

		if (!PX_LinkerRead(Mqtt->linker,&rByte,1))
		{
			return PX_FALSE;
		}
		utf8Size+=rByte;
		size--;

		while (utf8Size)
		{
			if (!PX_LinkerRead(Mqtt->linker,&rByte,1))
			{
				return PX_FALSE;
			}
			PX_MemoryCat(&Mqtt->topic,&rByte,1);
			utf8Size--;
			size--;
		}
	} while (0);
	PX_MemoryCat(&Mqtt->topic,"",1);

	//recv ID
	do 
	{
		if (!PX_LinkerRead(Mqtt->linker,&id,2))
		{
			return PX_FALSE;
		}
		size-=2;
	} while (0);


	//recv payload
	do 
	{
		px_int rsize,retSize;
		px_byte cache[128];
		PX_MemoryClear(&Mqtt->payload);
		while (size)
		{
			if (size>sizeof(cache))
			{
				rsize=sizeof(cache);
			}
			else
			{
				rsize=size;
			}
			if ((retSize=PX_LinkerRead(Mqtt->linker,cache,rsize))==0)
			{
				return PX_FALSE;
			}
			size-=retSize;
			PX_MemoryCat(&Mqtt->payload,cache,retSize);
		}
	} while (0);

	switch (qos)
	{
	case PX_MQTT_QOS_LEVEL_0:
		return PX_TRUE;
		//break;
	case PX_MQTT_QOS_LEVEL_1:
		if(!PX_MQTT_SendPUBACK(Mqtt,id))return PX_FALSE;
		return PX_TRUE;
		//break;
	case PX_MQTT_QOS_LEVEL_2:
		{
			if (!PX_MQTT_SendPUBREC(Mqtt,id))
			{
				return PX_FALSE;
			}
			if (!PX_MQTT_WaitPUBREL(Mqtt,id))
			{
				return PX_FALSE;
			}
			if (!PX_MQTT_SendPUBCOMP(Mqtt,id))
			{
				return PX_FALSE;
			}
			return PX_TRUE;
		}
		//break;
	}


	return PX_TRUE;
}

px_bool PX_MQTT_VerifyFrame(px_byte *data,px_int size)
{
	px_byte *pdata=data;
	px_int bsize=1;
	px_int datasize=PX_MQTT_GetSize(data+1);
	if (datasize==0)
	{
		return PX_FALSE;
	}
	while ((*pdata)&0x80)
	{
		bsize++;
		pdata++;
	}
	return size>=(bsize+datasize+1);
}


px_void PX_MQTTFree(PX_MQTT *Mqtt)
{
	PX_MemoryFree(&Mqtt->payload);
	PX_MemoryFree(&Mqtt->topic);
}

