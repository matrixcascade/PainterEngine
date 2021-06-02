//////////////////////////////////////////////////////////////////////////
//MQTT 3.1.1 for PainterEngine 
//code by:DBinary 2020-12-28
//////////////////////////////////////////////////////////////////////////

#ifndef PX_MQTT_H
#define PX_MQTT_H

#include "../core/PX_Core.h"

typedef enum
{
	PX_MQTT_QOS_LEVEL_0,
	PX_MQTT_QOS_LEVEL_1,
	PX_MQTT_QOS_LEVEL_2,
}PX_MQTT_QOS_LEVEL;

#define PX_MQTT_DEFAULT_TIMEOUT 2000
#define PX_MQTT_CONNECTION_SESSION_SIZE 32

#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_RESERVED0	0
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_CONNECT		1
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_CONNACK		2
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_PUBLISH		3
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_PUBACK		4
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_PUBREC		5
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_PUBREL		6
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_PUBCOMP		7
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_SUBSCRIBE	8
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_SUBACK		9	
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_UNSUBSCRIBE  10
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_UNSUBACK     11
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_PINGREQ		12
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_PINGRESP     13
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_DISCONNECT   14
#define PX_MQTT_FIXERHEADER_CONTROLLERTYPE_RESERVED1	15

typedef enum
{
	PX_MQTT_STATUS_DISCONNECT,
	PX_MQTT_STATUS_CONNECTING,
}PX_MQTT_STATUS;

typedef enum
{
	PX_MQTT_CONNECT_SUCCEEDED,
	PX_MQTT_CONNECT_ERROR_UNKNOW,
	PX_MQTT_CONNECT_ERROR_DISCONNECT,
	PX_MQTT_CONNECT_ERROR_PROTOCAL_NO_SUPPORT,
	PX_MQTT_CONNECT_ERROR_SESSION_ILLEGAL,
	PX_MQTT_CONNECT_ERROR_SERVER_CRASH,
	PX_MQTT_CONNECT_ERROR_USER_WRONG,
	PX_MQTT_CONNECT_ERROR_ILLEGAL,
}PX_MQTT_CONNECT;

struct _PX_MQTT
{
	px_memorypool *buildmp;
	px_dword sessionKey;
	PX_Linker *linker;
	px_dword identify;
	px_memory topic;
	px_memory payload;
	px_char connectionKeySession[PX_MQTT_CONNECTION_SESSION_SIZE+1];
};
typedef struct _PX_MQTT PX_MQTT;

typedef struct  
{
	const px_char *userName;
	const px_char *password;
	const px_char *willTopic;
	const px_void *willContent;
	px_uint willSize;
	px_bool willRetain;
	px_bool CleanSession;
	PX_MQTT_QOS_LEVEL willQoS;
	px_word KeepAliveTime;//seconds
}PX_MQTT_ConnectDesc;


typedef struct  
{
	PX_MQTT_QOS_LEVEL qosLevel;
	px_bool retain;
	const px_char *Topic;
	const px_void *payload;
	px_int payloadSize;
}PX_MQTT_PublishDesc;

typedef struct  
{
	PX_MQTT_QOS_LEVEL qosLevel;
	const px_char *Topic;
}PX_MQTT_SubscribeDesc;

px_bool PX_MQTTInitialize(PX_MQTT *Mqtt,px_memorypool *mp,PX_Linker *linker);
PX_MQTT_CONNECT PX_MQTTConnect(PX_MQTT *Mqtt,PX_MQTT_ConnectDesc connectDesc);
px_bool PX_MQTTPublish(PX_MQTT *Mqtt,PX_MQTT_PublishDesc publishDesc);
px_bool PX_MQTTSubscribe(PX_MQTT *Mqtt,PX_MQTT_SubscribeDesc subscribeDesc);
px_bool PX_MQTTUnsubscribe(PX_MQTT *Mqtt,const px_char *Topic);
px_bool PX_MQTTPingReq(PX_MQTT *Mqtt);
px_bool PX_MQTTListen(PX_MQTT *Mqtt);
px_void PX_MQTTFree(PX_MQTT *Mqtt);
#endif

