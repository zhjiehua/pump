#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "Device.h"
#include "plc_instruction.h"

#define MAX_PROTOCL_BUF_SZ	50
#define PROTOCL_CMD_SZ		4
#define PROTOCL_MCU_CMD_SZ  5
#define PROTOCL_STR_SZ		11

#define PROTOCL_CMD			1
#define PROTOCL_STR			2
#define PROTOCL_LOCAL_USE_CHANGE_PUMPTYPE   3

#define STX		0x21
#define ETX		0x0a
#define ESC		0x10

#define ENQ		0x05
#define ACK		0x23
#define NAK		0x24
#define WAIT	0x25

//#define ID		0x13
#define AI		0x0

#define CMDHEAD 0x80




#define CMD_READ		'0'
#define CMD_WRITE		'1'
#define CMD_FORCEON		'7'
#define CMD_FORCEOFF	'8'

#define CMD_PRO_READ	'a'
#define CMD_PRO_WRITE	'b'

#define CMDLEN_COIL		16
#define CMDLEN_PRO		18

#define MONITOR_ADDR    8040
#define MONITORID_ADDR  1024

typedef qint16 (*pComWrite)( quint8* data, uint16 sz );
typedef void (*pTimerOut)( void* pData );
typedef void (*pSetTimer)( int nID, unsigned long time, void* pData, pTimerOut callback );
typedef void (*pProcess)( mbyte type, uint32 cmd, uint32 arg , uint32 add);
typedef void (*pProcess4Clarity)( mbyte hID, mbyte hAI, mbyte hPFC, uint32 nVal );
typedef void (*pCheckCompare)(uint32 cmd);

typedef struct tagProtocolConf{
	pComWrite write;
	pSetTimer setTimer;
	pProcess process;
	pProcess4Clarity processClarity;
	int nTimeOutCount;
	unsigned long lTimerout;
}ProtocolConf,*LPProtocolConf;

#define PROTOCOL_CONF_WRITEFUN		0x01
#define PROTOCOL_CONF_SETTIMERFUN	0x02
#define PROTOCOL_CONF_TIMEOUTCOUNT	0x04
#define PROTOCOL_CONF_TIMEOUT		0x08
#define PROTOCOL_CONF_PROCESSFUN	0x10
#define PROTOCOL_CONF_CHECKFUN		0x20
#define PROTOCOL_CONF_PROCESSFUNCLARITY		0x40

int API_Protocol( mbyte* pData, uint16 sz );
void SendNAK();
void SendACK();
int API_ClarityProtocol( mbyte* pData, uint16 sz );
void API_InitProtocol( void );
void SetProtocolConf( LPProtocolConf conf, unsigned int uFlag );
int API_ProtocolSend( mbyte* pData, uint16 sz );
void API_CmdSend(mbyte type, uint32 cmd, uint32 arg);
void API_SetPumpType(int pumpType);//设置泵的类型，类型不同,旧上位机协议流速命令长度不同;

void API_CmdSendClarity(uint32 hAI, uint32 hPFC, uint32 hVal);


#define API_ReadData(x,y,z) 1
int API_WriteData(mbyte coil, uint16 addr, uint16 dataGet);
int API_WriteBit(mbyte coil, uint16 addr, uint16 dataGet);
#define API_IsPLCRuning() 1
#define API_ProGramRead(x,y) 1
#define API_ProGramWrite(x,y) 1

/***************************MCU**********************************/
typedef struct tagProtocol4McuConf{
	pComWrite write;
	pSetTimer setTimer;
	pProcess process;
	pCheckCompare checkCompare;
	int nTimeOutCount;
	unsigned long lTimerout;
}Protocol4McuConf,*LPProtocol4McuConf;

int API_McuProtocol( mbyte* pData, uint16 sz );
void SetMcuProtocolConf( LPProtocol4McuConf conf, unsigned int uFlag );
void API_McuCmdSend( mbyte type, uint32 cmd, uint32 arg );

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __PROTOCOL_H__ */
