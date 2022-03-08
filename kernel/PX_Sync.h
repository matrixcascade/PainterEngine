#ifndef PX_SYNC_H
#define PX_SYNC_H

#include "../core/PX_Core.h"


#define PX_SYNC_SERVER_ID 0
#define PX_SYNC_SERVER_SEND_DURATION   3
#define PX_SYNC_SERVER_SEND_TIMES      3

#define PX_SYNC_CLIENT_SEND_TIMES    3
#define PX_SYNC_DEFAULT_INSTR_ONCE_MAX_SIZE  256
#define PX_SYNC_UNIQUE_ARRAY_SIZE 64
#define PX_SYNC_CACHESIZE 1420
#define PX_SYNC_INSTRS_SIZE 128*1024
#define PX_SYNC_INSTRS_BYTES_SIZE 1024*1024
#define PX_SYNC_CLIENT_SEND_DURATION 3

typedef enum
{
	PX_SYNC_IO_TYPE_CONNECT,
	PX_SYNC_IO_TYPE_CONNECTACK,
	PX_SYNC_IO_TYPE_QUERYSTATE,
	PX_SYNC_IO_TYPE_QUERYSTATEACK,
	PX_SYNC_IO_TYPE_REQUEST,
	PX_SYNC_IO_TYPE_OPCODE,
}PX_SYNC_IO_TYPE;

typedef enum
{
	PX_SYNC_SERVER_STATUS_CONNECT,
	PX_SYNC_SERVER_STATUS_PROCESSING,
	PX_SYNC_SERVER_STATUS_END,
}PX_SYNC_SERVER_STATUS;

typedef enum
{
	PX_SYNC_SERVERCLIENT_STATUS_CONNECT,
	PX_SYNC_SERVERCLIENT_STATUS_PROCESSING,
	PX_SYNC_SERVERCLIENT_STATUS_END,
}PX_SYNC_SERVERCLIENT_STATUS;

typedef enum
{
	PX_SYNC_CLIENT_STATUS_CONNECTING,
	PX_SYNC_CLIENT_STATUS_WAITING,
	PX_SYNC_CLIENT_STATUS_PROCESSING,
	PX_SYNC_CLIENT_STATUS_END,
}PX_SYNC_CLIENT_STATUS;


struct _PX_SyncFrame_Server;
struct _PX_SyncFrame_Client;


typedef struct
{
	px_dword c_id;
	px_dword size;
	px_byte  data[1];
}PX_SyncFrame_InstrStream_Stamp;


typedef struct
{
	px_dword oft;
	px_dword size;
}PX_SyncFrame_InstrStream_StampIndex;

typedef struct
{
	px_dword client_id;
	px_dword server_id;
	px_dword c_id;
	px_dword timeIndexOffset;
	px_dword timeStreamOffset;
	px_dword uniqueQueuewIndex;
	px_dword acceptuniqueQueue[PX_SYNC_UNIQUE_ARRAY_SIZE];
	px_dword unique;
	px_dword sendDurationTick;
	px_dword sendTimes;
	px_dword onceRecvSize;
	px_dword lastsendtime;
	px_dword lastInstrelapsed;
	PX_SYNC_SERVERCLIENT_STATUS status;
	px_int send_cache_instr_size;
	px_byte send_cache_instr_buffer[PX_SYNC_CACHESIZE];
	PX_LinkerData linkerData;
}PX_SyncFrame_Server_Clients;


typedef struct  
{
	px_dword verify_id;
	px_dword type;
	px_dword unique;
	union
	{
		struct  
		{
			px_dword param1;
			px_dword param2;
		};
		px_byte data[1];
	};
}PX_Sync_IO_Packet;


typedef struct _PX_SyncFrame_Server
{
	px_memorypool *mp;
	px_dword time;
	px_dword updateDuration;
	px_dword unique;
	px_dword version;
	px_memory stampsInstrStream;
	px_vector stampsIndexTable;//PX_Sync_Server_StampIndex *
	px_vector clients;//PX_Sync_Server_Clients *
	PX_SYNC_SERVER_STATUS status;
	PX_Linker *linker;
	px_int recv_cache_size;
	px_uint instr_once_maxsize;
	px_byte recv_cache_buffer[PX_SYNC_CACHESIZE];
	px_byte send_cache_buffer[PX_SYNC_CACHESIZE];
	px_int send_times;
}PX_SyncFrameServer;




typedef struct _PX_SyncFrame_Client
{
	px_memorypool *mp;
	px_dword  c_id;
	px_dword  client_id;
	px_dword  server_verify_id;
	px_int   connectCount;
	px_int   connectSumCount;
	px_dword version;
	px_dword time;
	px_dword delayms;
	px_dword sendDurationTick;
	px_dword updateDuration;
	px_dword uniqueQueuewIndex;
	px_dword acceptuniqueQueue[PX_SYNC_UNIQUE_ARRAY_SIZE];
	px_dword unique;
	px_memory stampsInstrStream;//instrments
	px_vector stampsIndexTable;//PX_Sync_Server_StampIndex *
	px_memory Input_InstrStream;
	PX_Linker *linker;
	PX_SYNC_CLIENT_STATUS status;
	px_int send_repeat_times;
	px_byte send_cache_buffer[PX_SYNC_CACHESIZE];
	px_int recv_cache_buffer_size;
	px_byte recv_cache_buffer[PX_SYNC_CACHESIZE];
	px_int send_cache_Instr_size;
	px_byte send_cache_Instr_buffer[PX_SYNC_CACHESIZE];
	px_int send_times;
}PX_SyncFrameClient;




px_bool PX_SyncFrameServerInitialize(PX_SyncFrameServer *sync,px_memorypool *mp,px_dword updateDuration,PX_Linker *linker);
px_void PX_SyncFrameServerSetVersion(PX_SyncFrameServer *sync,px_dword version);
px_bool PX_SyncFrameServerAddClient(PX_SyncFrameServer *sync,px_dword server_verify_id,px_dword client_id,px_dword c_id);
px_void PX_SyncFrameServerSetInstrOnceMaxSize(PX_SyncFrameServer *sync,px_int maxsize);
px_void PX_SyncFrameServerRun(PX_SyncFrameServer *sync);
px_void PX_SyncFrameServerStop(PX_SyncFrameServer *sync);
px_void PX_SyncFrameServerUpdate(PX_SyncFrameServer *sync,px_dword elapsed);
px_void PX_SyncFrameServerFree(PX_SyncFrameServer *sync);
px_int  PX_SyncFrameServerGetReadyFrameCount(PX_SyncFrameServer *sync);
px_uint32 PX_SyncFrameServerSum32(PX_SyncFrameServer *sync);


px_bool PX_SyncFrameClientInitialize(PX_SyncFrameClient *client,px_memorypool *mp,px_dword updateDuration,px_dword server_verify_id,px_dword client_id,PX_Linker *linker);
px_void PX_SyncFrameClientAddInstr(PX_SyncFrameClient *client,px_void *instr,px_int size);
px_void PX_SyncFrameClientSetVersion(PX_SyncFrameClient *sync,px_dword version);
px_void PX_SyncFrameClientUpdate(PX_SyncFrameClient *sync,px_dword elapsed);
px_int  PX_SyncFrameClientGetReadyFrameCount(PX_SyncFrameClient *sync);
px_void PX_SyncFrameClientFree(PX_SyncFrameClient *sync);
px_uint32 PX_SyncFrameClientSum32(PX_SyncFrameClient *sync);


#define PX_SYNCDATA_DATAGRAM_MAX_SIZE			1460
#define PX_SYNCDATA_BLOCK_SIZE					1408
#define PX_SYNCDATA_SENDING_DURATION			20
#define PX_SYNCDATA_QUERY_DURATION				500
#define PX_SYNCDATA_REQUEST_DURATION			100

struct _PX_SyncData_Server;
struct _PX_SyncData_Client;


typedef struct
{
	px_int blockIndex;
}PX_SyncDataBlock;

typedef enum
{
	PX_SYNCDATA_CLIENT_STATUS_QUERY,
	PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZING,
	PX_SYNCDATA_CLIENT_STATUS_SYNCHRONIZED,
	PX_SYNCDATA_STATUS_ERROR,
}PX_SYNCDATA_STATUS;

typedef px_bool (*PX_SyncData_Encode)(px_byte *data,px_int size);
typedef px_bool (*PX_SyncData_Decode)(px_byte *data,px_int size);

typedef struct _PX_SyncData_Server_Client
{
    px_dword     clientID;
	PX_SYNCDATA_STATUS     status;
	px_int		 ProcessIndex;
	px_dword	 send_elapsed;
}PX_SyncData_Server_Client;

typedef struct _PX_SyncData_Server
{
	PX_Linker *linker;
	px_dword serverID;
	px_byte *data;
	px_dword size;

	px_void *user;
	px_vector clients;// vector for PX_SyncData_Server_Client
}PX_SyncDataServer;

typedef struct _PX_SyncData_Client
{
	PX_Linker *linker;

	px_memorypool *mp;
	px_dword	 clientID,serverID;
	
	px_byte *data;
	px_dword datasize;

	px_int blockCount;
	px_int acceptBlock;

	px_void *user;

	PX_SYNCDATA_STATUS status;
	
	px_dword query_elapsed;
	px_dword last_recv_elapsed;
}PX_SyncDataClient;

#define PX_SYNCDATA_OPCODE_QUERY 0x680
#define PX_SYNCDATA_OPCODE_QUERYACK 0x6802
#define PX_SYNCDATA_OPCODE_REQUEST 0x6803
#define PX_SYNCDATA_OPCODE_REQUESTACK 0x6804


typedef struct
{
	px_dword opcode;
	px_dword serverID;
}PX_SyncData_DatagramHeader;

typedef struct
{
	px_dword opcode;
	px_dword serverID;
	px_dword clientID;
	px_dword reserved;
}PX_SyncData_Query;

typedef struct
{
	px_dword opcode;
	px_dword serverID;
	px_dword size;
	px_dword reserved;
}PX_SyncData_QueryAck;

typedef struct
{
	px_dword opcode;
	px_dword serverID;
	px_dword reserved;
	px_dword blockIndex;
}PX_SyncData_Request;

typedef struct  
{
	px_dword opcode;
	px_dword serverID;
	px_dword reserved;
	px_dword blockIndex;
	px_byte data[PX_SYNCDATA_BLOCK_SIZE];
}PX_SyncData_RequestAck;

typedef union
{
	PX_SyncData_DatagramHeader header;
	PX_SyncData_Query query;
	PX_SyncData_QueryAck queryack;
	PX_SyncData_Request request;
	PX_SyncData_RequestAck requestAck;
}PX_SyncData_Datagram;

px_bool PX_SyncDataServerInitialize(PX_SyncDataServer *syncdata_server,px_memorypool *mp,px_dword serverID,PX_Linker *linker);
px_bool PX_SyncDataServerAddClient(PX_SyncDataServer *syncdata_server,px_dword clientID);
px_bool PX_SyncDataServerSetSyncData(PX_SyncDataServer *s,px_void *data,px_dword size);
px_bool PX_SyncDataServerUpdate(PX_SyncDataServer *syncdata_server,px_int elapsed);
px_void PX_SyncDataServerFree(PX_SyncDataServer *syncdata_server);


px_bool PX_SyncDataClientInitialize(PX_SyncDataClient *syncdata_client,px_memorypool *mp,px_dword serverID,px_dword clientID,PX_Linker *linker);
px_bool PX_SyncDataClientIsCompleted(PX_SyncDataClient *syncdata_client);
px_bool PX_SyncDataClientUpdate(PX_SyncDataClient *syncdata_client,px_int elapsed);
px_void PX_SyncDataClientFree(PX_SyncDataClient *syncdata_client);
#endif
