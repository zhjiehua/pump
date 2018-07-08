#ifdef __cplusplus
extern "C" {
#endif

#include "Protocol.h"


enum eProtocolType{
	eProtocolNull,
	eProtocolCmd,
	eProtocolStr,
};

typedef struct tagProtocolData{
	Protocol4McuConf conf;
	int nTimeout;
    quint8 revBuf[ MAX_PROTOCL_BUF_SZ ];
	uint16 revIndex;
    quint8 senBuf[ MAX_PROTOCL_BUF_SZ ];
	enum eProtocolType eType;
}ProtocolData,*LPProtocolData;

static ProtocolData g_protocol_mcu = {0};
 
static int Asiic2Hex( mbyte* pstr,uint16 sz,uint32* phex )
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
static int Asiic2Int( mbyte* pstr,uint16 sz,uint32* phex )
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
		else
			return 0;
		pc++;
	}
	return 1;
}


static int int2Ascii(uint16 num, mbyte *aBuf)
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

static mbyte hex2Ascii(mbyte data_hex)
{
	char  ASCII_Data;
	ASCII_Data=data_hex & 0x0F;
	if(ASCII_Data<10) 
		ASCII_Data=ASCII_Data+0x30; //‘0--9’
	else  
		ASCII_Data=ASCII_Data+0x37;       //‘A--F’
	return ASCII_Data;
}

static int API_ProtocolMcuSend( mbyte* pData, uint16 sz )
{
	if( g_protocol_mcu.conf.write )
		g_protocol_mcu.conf.write( pData, sz );

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
	//     if( g_protocol_mcu.conf.write )
	//         g_protocol_mcu.conf.write( sendBuf, ps - sendBuf );

	return 1;
}

void API_McuCmdSend( mbyte type, uint32 cmd, uint32 arg )
{
	mbyte data[PROTOCL_MCU_CMD_SZ];
	mbyte *p = data;
	
	if(type == PROTOCL_CMD)
	{
		*p++ = (cmd | CMDHEAD);//0
		*p++ = (arg >> 21)&0x7f;
		*p++ = (arg >> 14)&0x7f;
		*p++ = (arg >> 7)&0x7f;
		*p++ = (arg >> 0)&0x7f;
	}
	else if(type == PROTOCL_STR)
	{
	
	}
	else 
		return;

	API_ProtocolMcuSend(data, sizeof(data));
}




//static __inline void SendNAK()
//{
//	uint16 nak = NAK;
//	if( g_protocol_mcu.conf.write )
//		g_protocol_mcu.conf.write( &nak, 1 );
//}
//
//static __inline void SendACK()
//{
//	uint16 enq = ACK;
//	if( g_protocol_mcu.conf.write )
//		g_protocol_mcu.conf.write( &enq, 1 );
//}







//需要将quint等改成宏定义;
static void ProcessData()
{
	int i;
	uint32 arg = 0;
	mbyte type = 0;
	uint32 cmd = 0;
	uint32 add = 0;

	if ( g_protocol_mcu.eType == eProtocolStr )
	{
		type = PROTOCL_STR;
		cmd = g_protocol_mcu.revBuf[ 1 ];
		for( i = 3; i < 9; ++i )
		{
			arg <<= 4;
			arg |= ( g_protocol_mcu.revBuf[ i ] - '0' ) & 0x0f;
		}
	}else if ( g_protocol_mcu.eType == eProtocolCmd )
	{
		type = PROTOCL_CMD;
		cmd = g_protocol_mcu.revBuf[ 0 ] & 0x0f;
		arg = g_protocol_mcu.revBuf[ 1 ] & 0x7f;
		arg <<= 7;
		arg |= g_protocol_mcu.revBuf[ 2 ] & 0x7f;
		arg <<= 7;
		arg |= g_protocol_mcu.revBuf[ 3 ] & 0x7f;
		arg <<= 7;
		arg |= g_protocol_mcu.revBuf[ 4 ] & 0x7f;
		//g_protocol_mcu.conf.write( g_protocol_mcu.revBuf, 1 );
	}
	if( type && g_protocol_mcu.conf.process )
		g_protocol_mcu.conf.process( type, cmd, arg ,0);

}


int API_McuProtocol( mbyte* pData, uint16 sz )
{
	while( sz-- )
	{
		if( g_protocol_mcu.revIndex >= MAX_PROTOCL_BUF_SZ )
		{
			g_protocol_mcu.eType = eProtocolNull;
			g_protocol_mcu.revIndex = 0;
		}
		switch( g_protocol_mcu.eType )
		{
		case eProtocolNull:
			if( *pData & CMDHEAD )
			{
				g_protocol_mcu.revIndex = 0;
				g_protocol_mcu.revBuf[ g_protocol_mcu.revIndex++] = *pData;
				g_protocol_mcu.eType = eProtocolCmd;

				//处理答应信息;
				if(g_protocol_mcu.conf.checkCompare )
					g_protocol_mcu.conf.checkCompare(*pData);

			}else if( *pData == 'S' )
			{
				g_protocol_mcu.revIndex = 0;
				g_protocol_mcu.revBuf[ g_protocol_mcu.revIndex++] = *pData;
				g_protocol_mcu.eType = eProtocolStr;
			}
			break;
		case eProtocolStr:
			if( g_protocol_mcu.revIndex < PROTOCL_STR_SZ )
			{
				mbyte bErr = 0;
				switch( g_protocol_mcu.revIndex )
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
					g_protocol_mcu.revBuf[ g_protocol_mcu.revIndex++] = *pData;
					if( g_protocol_mcu.revIndex == PROTOCL_STR_SZ )
					{
						ProcessData();
						g_protocol_mcu.revIndex = 0;
						g_protocol_mcu.eType = eProtocolNull;
					}
				}
			}else
				goto protocol_reset;
			break;
		case eProtocolCmd:
			if( ( *pData & CMDHEAD ) == 0 && g_protocol_mcu.revIndex < PROTOCL_MCU_CMD_SZ )
			{
				g_protocol_mcu.revBuf[ g_protocol_mcu.revIndex++] = *pData;
				if( g_protocol_mcu.revIndex == PROTOCL_MCU_CMD_SZ )
				{
					ProcessData();
					g_protocol_mcu.revIndex = 0;
					g_protocol_mcu.eType = eProtocolNull;
				}
			}else
			{
				//处理答应信息;
				if(g_protocol_mcu.conf.checkCompare )
					g_protocol_mcu.conf.checkCompare(*pData);

				sz++;
				goto protocol_reset;
			}
			break;
		default:
			goto protocol_reset;
		}
		++pData;
		continue;
protocol_reset:
		g_protocol_mcu.revIndex = 0;
		g_protocol_mcu.eType = eProtocolNull;
		continue;
	}
	return 1;//返回1表示已经由该函数识别并处理
}

void API_InitMcuProtocol( void )
{
	memset( &g_protocol_mcu, 0, sizeof( g_protocol_mcu ) );
}

void SetMcuProtocolConf( LPProtocol4McuConf conf, unsigned int uFlag )
{
	if( PROTOCOL_CONF_WRITEFUN & uFlag )
		g_protocol_mcu.conf.write = conf->write;
	if( PROTOCOL_CONF_SETTIMERFUN & uFlag )
		g_protocol_mcu.conf.setTimer = conf->setTimer;
	if( PROTOCOL_CONF_TIMEOUTCOUNT & uFlag )
		g_protocol_mcu.conf.nTimeOutCount = conf->nTimeOutCount;
	if( PROTOCOL_CONF_TIMEOUT & uFlag )
		g_protocol_mcu.conf.lTimerout = conf->lTimerout;
	if( PROTOCOL_CONF_PROCESSFUN & uFlag )
		g_protocol_mcu.conf.process = conf->process;
	if( PROTOCOL_CONF_CHECKFUN & uFlag )
		g_protocol_mcu.conf.checkCompare = conf->checkCompare;
}


#ifdef __cplusplus
}
#endif

