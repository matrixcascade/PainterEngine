#ifndef PX_HTTPSERVER_H
#define PX_HTTPSERVER_H
#include "px_time.h"
#include "px_tcp.h"
#include "px_thread.h"
#include "px_sysmemory.h"
#include "../../kernel/PX_Kernel.h"

#define PX_HTTPSERVER_MAX_REQUEST 64
typedef struct
{
	volatile px_bool activating;
	px_memorypool mp;
	px_void* pserver;
	px_int socket;
	px_byte cache[1024 * 1024];
	px_memory request_data;
	px_thread thread;
}PX_HttpServer_Request;


typedef struct
{
	px_memorypool mp;
	PX_TCP tcp;
	px_byte cache[32 * 1024 * 1024];
	px_vector abi_data;
	px_thread thread;
	PX_HttpServer_Request request[PX_HTTPSERVER_MAX_REQUEST];
}PX_HttpServer;



px_bool PX_HttpServer_Initialize(PX_HttpServer* httpserver, px_int port);
px_bool PX_HttpServer_AddFile(PX_HttpServer* httpserver, const px_char path[], px_byte* data, px_int datasize);
#endif // !PX_HTTPSERVER_H
