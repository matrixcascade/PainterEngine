#ifndef PX_TransmissionProtocal_H
#define PX_TransmissionProtocal_H
#include "../Core/PX_Core.h"

#define PX_TRANSMISSION_DATAGRAM_MAX_SIZE 1460
#define PX_TRANSMISSION_BLOCK_SIZE       1408
#define PX_TRANSMISSION_CACHE_SIZE		 32
#define PX_TRANSMISSION_TIMEOUT			 100
#define PX_TRANSMISSION_CONNECT_TRY      16
#define PX_TRANSMISSION_CONNECT_DURATION 500
#define PX_TRANSMISSION_SENDING_DURATION 20
#define PX_TRANSMISSION_SENDING_TIMEOUT  10000
#define PX_TRANSMISSION_RECVING_TIMEOUT  10000
#define PX_TRANSMISSION_REQUEST_DURATION 200
#define PX_TRANSMISSION_CLOSETIME        500

typedef px_bool (*PX_TransmissionProtocal_Interface_Write)(px_byte *data,px_int size);
typedef px_bool (*PX_TransmissionProtocal_Interface_Read)(px_byte *data,px_int bufferSize,px_int *readsize);
typedef px_bool (*PX_TransmissionProtocal_Interface_Encode)(px_byte *data,px_int size);
typedef px_bool (*PX_TransmissionProtocal_Interface_Decode)(px_byte *data,px_int size);

typedef struct
{
	px_int blockIndex;
}PX_TransmissionBlock;

typedef enum
{
	PX_TRANSMISSION_STATUS_CONNECT,
	PX_TRANSMISSION_STATUS_SENDING,
	PX_TRANSMISSION_STATUS_RECEIVING,
	PX_TRANSMISSION_STATUS_WAITFORCLOSE,
	PX_TRANSMISSION_STATUS_DONE,
	PX_TRANSMISSION_STATUS_ERROR,
	PX_TRANSMISSION_STATUS_STANDBY,
}PX_TRANSMISSION_STATUS;


typedef struct _PX_Transmission
{
	PX_TransmissionProtocal_Interface_Write write;
	PX_TransmissionProtocal_Interface_Read read;
	PX_TransmissionProtocal_Interface_Encode encode;
	PX_TransmissionProtocal_Interface_Decode decode;
	px_dword p_id;
	px_dword p_lastid;
	px_byte *data;
	px_dword alloc_datasize;
	px_dword size;
	px_dword offset;
	px_memorypool *mp;
	PX_TRANSMISSION_STATUS status;
	PX_TransmissionBlock requestBlock[PX_TRANSMISSION_CACHE_SIZE];
	px_int sendingQueue[PX_TRANSMISSION_CACHE_SIZE];
	px_int connectTry;
	px_int blockCount;
	px_int acceptBlock;
	px_uint queueindex;
	px_dword sendingduration;
	px_dword recvElpased;
	px_dword elpased;
	px_int  max_recv_size;

}PX_Transmission;

#define PX_TRANSMISSION_OPCODE_CONNECT 0x6601
#define PX_TRANSMISSION_OPCODE_DONE 0x6602
#define PX_TRANSMISSION_OPCODE_REQUEST 0x6603
#define PX_TRANSMISSION_OPCODE_DATAGRAM 0x6604

typedef struct
{
	px_dword opcode;
	px_dword p_id;
}PX_Transmission_DatagramHeader;

typedef struct
{
	px_dword opcode;
	px_dword p_id;
	px_dword size;
	px_byte data[PX_TRANSMISSION_BLOCK_SIZE];
}PX_Transmission_Connect;

typedef struct
{
	px_dword opcode;
	px_dword p_id;
}PX_Transmission_Done;

typedef struct
{
	px_dword opcode;
	px_dword p_id;
	px_dword block[PX_TRANSMISSION_CACHE_SIZE];
}PX_Transmission_Request;

typedef struct  
{
	px_dword opcode;
	px_dword p_id;
	px_dword blockIndex;
	px_byte data[PX_TRANSMISSION_BLOCK_SIZE];
}PX_Transmission_dataDatagram;


typedef union
{
	PX_Transmission_DatagramHeader header;
	PX_Transmission_Connect connect;
	PX_Transmission_Done ack;
	PX_Transmission_Request request;
	PX_Transmission_dataDatagram data;
}PX_Transmission_Datagram;

px_bool PX_TransmissionInit(px_memorypool *mp,PX_Transmission *Ts,px_dword bridge_id,PX_TransmissionProtocal_Interface_Write write,PX_TransmissionProtocal_Interface_Read read,PX_TransmissionProtocal_Interface_Encode encode,PX_TransmissionProtocal_Interface_Decode decode);
px_bool PX_TransmissionSend(PX_Transmission *trans,px_byte *data,px_dword size);
px_bool PX_TransmissionRecv(PX_Transmission *trans,px_byte *data,px_dword size);
px_void PX_TramsmissionSetMaxRecvSize(PX_Transmission *trans,px_int size);
px_bool PX_TransmissionUpdate(PX_Transmission *trans,px_int elpased);
px_void PX_TransmissionCompleted(PX_Transmission *trans);
px_void PX_TransmissionFree(PX_Transmission *trans);
#endif
