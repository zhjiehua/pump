#ifdef __cplusplus

extern "C" {
#endif

#include "Protocol.h"
//#include <QDebug>

int bAutoAck = 0;
int bAutoRead = 0;

//设备码，不同泵型对应不同的设备码;
uint32 g_MachineCode = 0;


enum eProtocolType{
	eProtocolNull,
	eProtocolCmd,
	eProtocolStr,
};

typedef struct tagProtocolData{
	ProtocolConf conf;
	int nTimeout;
    quint8 revBuf[ MAX_PROTOCL_BUF_SZ ];
	uint16 revIndex;
    quint8 senBuf[ MAX_PROTOCL_BUF_SZ ];
	enum eProtocolType eType;
	mbyte bESCGet;
	int nPumpType;//0:10ml 1:50ml 2:100ml 3:150ml 4:250ml 5:300ml 6:500ml 7:800ml 8:1000ml 9:2000ml 10:3000ml
	//0:10ml 1:50ml 2:100ml 3:250ml 4:500ml 5:1000ml 6:3000ml
	
}ProtocolData,*LPProtocolData;

static ProtocolData g_protocol = {0};
 
int Asiic2Hex( mbyte* pstr,uint16 sz,uint32* phex )
{
	mbyte* pc,*pEnd;
	*phex = 0;
	pc = pstr;
	pEnd = pstr + sz;
	while( pc < pEnd )
	{
		*phex <<= 4;
		if( *pc >= 'A' && *pc <= 'F' )
			*phex |= *pc - 55;
		else if( *pc >= '0' && *pc <= '9' )
			*phex |= *pc - 48;
		else
			return 0;
		pc++;
	}
	return 1;
}

//ascii码转10进制;
int Asiic2Int( mbyte* pstr,uint16 sz,uint32* phex )
{
	mbyte* pc,*pEnd;
	*phex = 0;
	pc = pstr;
	pEnd = pstr + sz;
	*phex = 0;
	while( pc < pEnd )
	{
		*phex *= 10;
		if( *pc >= 'A' && *pc <= 'F' )
			*phex += *pc - 55;
		else if( *pc >= '0' && *pc <= '9' )
			*phex += *pc - 48;
		else if(*pc == 0x20)
			*phex = 0;
		else
			return 0;
		pc++;
	}
	return 1;
}


int int2Ascii(uint16 num, mbyte *aBuf)
{
	mbyte *p = aBuf;
	mbyte bai = num / 100;
	mbyte shi = num % 100 / 10;
	mbyte ge = num % 100 % 10;
	*p++ = bai+0x30;
	*p++ = shi+0x30;
	*p++ = ge+0x30;
	return 3;
}

//mbyte hex2Ascii(mbyte data_hex)
//{
//	char  ASCII_Data;
//	ASCII_Data=data_hex & 0x0F;
//	if(ASCII_Data<10) 
//		ASCII_Data=ASCII_Data+0x30; //‘0--9’
//	else  
//		ASCII_Data=ASCII_Data+0x37;       //‘A--F’
//	return ASCII_Data;
//}

mbyte hex2Ascii(uint32 data_hex, mbyte *data, int len, int bsp)
{
	char  ASCII_Data;
	int i;
	for (i = 0; i < len; i++)
	{
		ASCII_Data=data_hex & 0x0F;
		if(ASCII_Data<10) 
		{
			ASCII_Data=ASCII_Data+0x30; //‘0--9’
		}
		else  
			ASCII_Data=ASCII_Data+0x37;       //‘A--F’
		data[len-1-i] = ASCII_Data;
		data_hex = data_hex>>4;
	}
	if(bsp)//前置0改为0x20;
	{
		for (i=0; i<len; i++)
		{
			if(data[i] != 0x30)
			{
				return 0;
			}
			else
			{
				data[i]=0x20;
			}
		}
	}


	return 0;
}


void API_SetPumpType(int pumpType)
{
	if(pumpType <0 || pumpType > 10)
		return;

	g_protocol.nPumpType = pumpType;
}


void API_CmdSend( mbyte type, uint32 cmd, uint32 arg )
{
	mbyte data[4];
	mbyte *p = data;
	
	if(type == PROTOCL_CMD)
	{
		*p++ = (cmd | CMDHEAD);//0
		*p++ = (arg >> 14)&0x3f;
		*p++ = (arg >> 7)&0x7f;
		*p++ = (arg >> 0)&0x7f;
	}
	else if(type == PROTOCL_STR)
	{
	
	}
	else 
		return;

	API_ProtocolSend(data, sizeof(data));
}




void SendNAK()
{
	quint8 nak = NAK;
	if( g_protocol.conf.write )
		g_protocol.conf.write( &nak, 1 );
}

void SendACK()
{
	quint8 enq = ACK;
   /* if(bAutoAck==0)
        return;*/
	if( g_protocol.conf.write )
		g_protocol.conf.write( &enq, 1 );
}




int API_ProtocolSend( mbyte* pData, uint16 sz )
{
	if( g_protocol.conf.write )
		g_protocol.conf.write( pData, sz );

//     mbyte* p;
//     mbyte* pEnd;
//     mbyte sendBuf[MAX_PROTOCL_BUF_SZ];
//     mbyte* ps = sendBuf;
//     mbyte check;
// 
//     check = STX;
//     *ps++ = STX;
//     pEnd = pData + sz;
//     p = pData;
//     while( p < pEnd )
//     {
//         check += *p;
//         *ps++ = *p++;
//     }
// 	int2Ascii(check, ps);
//     ps += 3;
//     *ps++ = ETX;
//     if( g_protocol.conf.write )
//         g_protocol.conf.write( sendBuf, ps - sendBuf );

    return 1;
}


void API_CmdSendClarity(uint32 hAI, uint32 hPFC, uint32 hVal)
{
	mbyte data[16];
	unsigned char check = 0;
	int i = 0;
	data[0] = STX;
	data[15] = ETX;
	hex2Ascii(g_MachineCode, &data[1], 2, 0);
	hex2Ascii(AI, &data[3], 1, 0);
	hex2Ascii(hPFC, &data[4], 2, 0);
	hex2Ascii(hVal, &data[6], 6, 1);
	//12
	for (i = 0; i < 12; i++)
	{
		check += data[i];
	}
	int2Ascii(check, &data[12]);
	API_ProtocolSend(data, sizeof(data));
}




//需要将quint等改成宏定义;
static void ProcessData(int flowcmd)
{
	int i;
	uint32 arg = 0;
	mbyte type = 0;
	uint32 cmd = 0;
	uint32 add = 0;//附加参数;

	if ( g_protocol.eType == eProtocolStr )
	{
		type = PROTOCL_STR;
		cmd = g_protocol.revBuf[ 1 ];
		for( i = 3; i < 9; ++i )
		{
			arg <<= 4;
			arg |= ( g_protocol.revBuf[ i ] - '0' ) & 0x0f;
		}
	}else if ( g_protocol.eType == eProtocolCmd )
	{
		type = PROTOCL_CMD;
		if(flowcmd)
		{
			//0-10看g_protocol.nPumpType的定义;
			switch(g_protocol.nPumpType)
			{
			case 0:
			case 1:
			case 10:
				{
					cmd = g_protocol.revBuf[ 0 ] & 0x0f;
					add = g_protocol.revBuf[ 1 ] & 0x7f;
					arg = g_protocol.revBuf[ 2 ] & 0x7f;
					arg <<= 7;
					arg |= g_protocol.revBuf[ 3 ] & 0x7f;
					arg <<= 7;
					arg |= g_protocol.revBuf[ 4 ] & 0x7f;
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				{
					cmd = g_protocol.revBuf[ 0 ] & 0x0f;
					add = g_protocol.revBuf[ 1 ] & 0x7f;
					arg = g_protocol.revBuf[ 2 ] & 0x7f;
					arg <<= 7;
					arg |= g_protocol.revBuf[ 3 ] & 0x7f;
					
				}
				break;
			default:
				return;
			}
			//发送回复命令;
			g_protocol.conf.write( g_protocol.revBuf, 1 );
		}
		else
		{
			cmd = g_protocol.revBuf[ 0 ] & 0x0f;
			arg = g_protocol.revBuf[ 1 ] & 0x3f;
			arg <<= 7;
			arg |= g_protocol.revBuf[ 2 ] & 0x7f;
			arg <<= 7;
			arg |= g_protocol.revBuf[ 3 ] & 0x7f;

			if(cmd != 3)
				g_protocol.conf.write( g_protocol.revBuf, 1 );
		}
		
	}

	//张杰华调试添加@2016-06-26
	//if(cmd == 0x08)
	//{
	//	static uint32 last_arg = 0;
	//	if(last_arg != arg)
	//	{
	//		quint8 i = 0;
	//		last_arg = arg;
	//		printf("\n==========\n");
	//		for(i=0;i<5;i++)
	//			printf("g_protocol.revBuf[%d] = %#X\n", i, g_protocol.revBuf[i]);
	//		printf("arg = %d\n=========\n", arg);
	//	}
	//}

	if( type && g_protocol.conf.process )
		g_protocol.conf.process( type, cmd, arg , add);

}


//需要将quint等改成宏定义;
void ProcessData4Clarity()
{
	//检测校验和;
	quint8 senBuf[ MAX_PROTOCL_BUF_SZ ];
	mbyte hID;
	mbyte hAI;
	mbyte hPFC;
	uint32 nVal;
	mbyte sz;
	uint32 hData;
	uint16 dataGet;
	quint8* pData;
	quint8* pc;
	int i;
	uint32 recvCheckSum;
	mbyte checksum = STX;			//需要加上STX;

	//CRC校验;
	for ( i =0; i < g_protocol.revIndex - 3; i++ )
		checksum += g_protocol.revBuf[i];

	//变换ASCii码表示的CRC码转换为十进制;
	Asiic2Int(&g_protocol.revBuf[g_protocol.revIndex - 3], 3, &recvCheckSum);
	if((recvCheckSum & 0xff) != checksum)
	{
		SendNAK();
	}
	else
	{
		pc = g_protocol.revBuf;
		//id;
		Asiic2Hex(pc, 2, &hData);
		hID = hData & 0xff;
		pc += 2;
		//ai;
		Asiic2Hex(pc, 1, &hData);
		hAI = hData & 0xff;
		pc += 1;
		//pfc;
		Asiic2Hex(pc, 2, &hData);
		hPFC = hData & 0xff;
		pc +=2;
		//value;
		Asiic2Int(pc, 6, &nVal);

		if(g_protocol.conf.processClarity)
			g_protocol.conf.processClarity(hID, hAI, hPFC, nVal);
	}


}




int API_Protocol( mbyte* pData, uint16 sz )
{
	static int flowcmd = 0;
	while( sz-- )
	{
		if( g_protocol.revIndex >= MAX_PROTOCL_BUF_SZ )
		{
			g_protocol.eType = eProtocolNull;
			g_protocol.revIndex = 0;
		}
		switch( g_protocol.eType )
		{
		case eProtocolNull:
			if( *pData & CMDHEAD )
			{
				g_protocol.revIndex = 0;
				g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
				g_protocol.eType = eProtocolCmd;
				if(*pData == 0x88)
				{
					flowcmd = 1;//5/4个字节;
				}

			}else if( *pData == 'S' )
			{
				g_protocol.revIndex = 0;
				g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
				g_protocol.eType = eProtocolStr;
			}
			break;
		case eProtocolStr:
			if( g_protocol.revIndex < PROTOCL_STR_SZ )
			{
				mbyte bErr = 0;
				switch( g_protocol.revIndex )
				{
				case 1:
					if( *pData != 'M' && *pData != 'm' )
						bErr = 1;
					break;
				case 2:
					if( *pData != ' ' )
						bErr = 1;
					break;
				case 9:
					if( *pData != '\r' )
						bErr = 1;
					break;
				case 10:
					if( *pData != 0x7f )
						bErr = 1;
					break;
				}
				if( bErr )
					goto protocol_reset;
				else
				{
					g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
					if( g_protocol.revIndex == PROTOCL_STR_SZ )
					{
						ProcessData(0);
						g_protocol.revIndex = 0;
						g_protocol.eType = eProtocolNull;
					}
				}
			}else
			{
				goto protocol_reset;
			}
			break;
		case eProtocolCmd:
			/*if( ( *pData & CMDHEAD ) == 0 && g_protocol.revIndex < PROTOCL_CMD_SZ )
			{
			g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
			if( g_protocol.revIndex == PROTOCL_CMD_SZ )
			{
			ProcessData(0);
			g_protocol.revIndex = 0;
			g_protocol.eType = eProtocolNull;
			}
			}else
			goto protocol_reset;*/
			if(flowcmd)
			{
				int addLen = 0;//额外长度，不同流速命令，协议长度不同;
				switch(g_protocol.nPumpType)
				{
				case 0:
				case 1:
				case 10:
					{
						addLen = 1;
					}
					break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
					{
						addLen = 0;
					}
					break;
				default:
					break;
				}
				if( ( *pData & CMDHEAD ) == 0 && g_protocol.revIndex < PROTOCL_CMD_SZ+addLen )
				{
					g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
					if( g_protocol.revIndex == PROTOCL_CMD_SZ+addLen )
					{
						ProcessData(1);
						g_protocol.revIndex = 0;
						flowcmd = 0;
						g_protocol.eType = eProtocolNull;
					}
				}else
				{
					sz++;
					goto protocol_reset;
				}
			}
			else
			{
				if( ( *pData & CMDHEAD ) == 0 && g_protocol.revIndex < PROTOCL_CMD_SZ )
				{
					g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
					if( g_protocol.revIndex == PROTOCL_CMD_SZ )
					{
						ProcessData(0);
						g_protocol.revIndex = 0;
						g_protocol.eType = eProtocolNull;
					}
				}else
				{
					sz++;
					goto protocol_reset;
				}
			}
			break;
		default:
			goto protocol_reset;
		}
		++pData;
		continue;
protocol_reset:
		g_protocol.revIndex = 0;
		g_protocol.eType = eProtocolNull;
		continue;
	}
	return 1;//返回1表示已经由该函数识别并处理
}


//Clarity协议解析;
int API_ClarityProtocol( mbyte* pData, uint16 sz )
{
	static int stxFlag = 0;
	while( sz-- )
	{
		if( g_protocol.revIndex >= MAX_PROTOCL_BUF_SZ )
		{
			SendNAK();
			g_protocol.revIndex = 0;
		}else
		{
			switch( *pData )
			{
			case STX:
				if( g_protocol.bESCGet )	//数据;
				{
					g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
					g_protocol.bESCGet = 0;
				}else						//STX
				{
					stxFlag = 1;
					g_protocol.revIndex = 0;
				}

				break;
			case ESC:
				if( g_protocol.bESCGet )
				{
					g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
					g_protocol.bESCGet = 0;
				}else
					g_protocol.bESCGet = 1;
				break;
			case ETX:
				if( g_protocol.bESCGet )
				{
					g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
					g_protocol.bESCGet = 0;
				}else
				{
					if(stxFlag)
					{
						stxFlag = 0;
						ProcessData4Clarity();
					}
					else
						g_protocol.revIndex = 0;

				}
				break;
			default:
				g_protocol.revBuf[ g_protocol.revIndex++] = *pData;
				break;
			}
		}
		++pData;
	}
	return 1;//返回1表示已经由该函数识别并处理
}




















void API_InitProtocol( void )
{
	memset( &g_protocol, 0, sizeof( g_protocol ) );
}

void SetProtocolConf( LPProtocolConf conf, unsigned int uFlag )
{
	if( PROTOCOL_CONF_WRITEFUN & uFlag )
		g_protocol.conf.write = conf->write;
	if( PROTOCOL_CONF_SETTIMERFUN & uFlag )
		g_protocol.conf.setTimer = conf->setTimer;
	if( PROTOCOL_CONF_TIMEOUTCOUNT & uFlag )
		g_protocol.conf.nTimeOutCount = conf->nTimeOutCount;
	if( PROTOCOL_CONF_TIMEOUT & uFlag )
		g_protocol.conf.lTimerout = conf->lTimerout;
	if( PROTOCOL_CONF_PROCESSFUN & uFlag )
		g_protocol.conf.process = conf->process;
	if( PROTOCOL_CONF_PROCESSFUNCLARITY & uFlag )
		g_protocol.conf.processClarity = conf->processClarity;
}


#ifdef __cplusplus
}
#endif

