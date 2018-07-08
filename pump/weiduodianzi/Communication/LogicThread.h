#ifndef LOGICTHREAD_H
#define LOGICTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include "Protocol.h"


#define TIMEOUT_PERIOD 50


#ifdef WIN32
#include "Communication/win_qextserialport.h"
#elif linux
#include "Communication//posix_qextserialport.h"
#endif

#if 0
//����Ϊ����16�ֽ��������ݵ�����;
#define PFC_READ_PRODUCT_ID			0x01//��ȡ��ƷID;
#define PFC_READ_LICENSE_H			0x02//��ȡ���кŸ�λ;
#define PFC_READ_LICENSE_L			0x03//��ȡ���кŵ�λ;
#define PFC_READ_STATUS_1			0x04//��ȡ�����״̬1(��״̬�뵱ǰ����);
#define PFC_READ_STATUS_2			0x05//��ȡ�����״̬2��ʱ�䳣���������Χ);
#define PFC_READ_VERSION			0x06//��ȡ���������汾��;
#define PFC_READ_REF				0x07//��ȡref*;
#define PFC_READ_SIG				0x08//��ȡsig*;

//����Ϊ����ACK��NACK������;
#define PFC_WAVE_LENGTH				0x10//����;
#define PFC_TIME_CONST				0x11//ʱ�䳣��;
#define PFC_OUTPUT_EXTENT			0x12//�����Χ;
#define PFC_SYNC_TIME				0X13//�趨ͬ��ʱ��;
#define PFC_LIGHT_TURN_ON			0x14//����;
#define PFC_LIGHT_TURN_OFF			0x15//�ص�;
#define PFC_WAVE_LEN_INI			0x16//������ʼ��;
#define PFC_AUTO_RESET				0x17//�Զ���������;
#define PFC_SET_AU_FREQ				0x18//�趨Auֵ����Ƶ������;
#define PFC_SET_OUTPUT_EVENT		0x19//�趨����¼�;
#define PFC_LIGHT_SEL				0x20//뮵����ٵ�ѡ��;

//����Ϊ��λ���������͵�����;
#define PFC_READ_AU					0x90//��Auֵ����;
#define PFC_INPUT_EVENT				0x91//���������¼�;
#define PFC_SYS_ERR					0x92//����ϵͳ����;
#define PFC_STAT_CHANGE_1			0x93//�����������뵼�¼���������״̬�仯1(��״̬�뵱ǰ����);
#define PFC_STAT_CHANGE_2			0x94//�����������뵼�¼���������״̬�仯2(ʱ�䳣���������Χ);
#endif

#define PFCC_READ_PRODUCT_ID		0x01//��ȡ��ƷID;
#define PFCC_READ_LICENSE_H			0x02//��ȡ���кŸ�λ;
#define PFCC_READ_LICENSE_L			0x03//��ȡ���кŵ�λ;
#define PFCC_READ_PUMPSTATUS		0x04//��ȡ������״̬;
#define PFCC_READ_PUMPFIX			0x05//��ȡ������У������;
#define PFCC_READ_PUMPTIME			0x06//��ȡ��������ʱ��;


//����Ϊ����ACK��NACK������;
#define PFCC_SET_FLOW				0x10//�趨������;
#define PFCC_SET_FLOWPERCENT		0x11//�趨�������ٷֱ�;
#define PFCC_SYNCTIME				0x12//�趨ͬ��ʱ��;
#define PFCC_MAX_PRESS				0X13//�趨���ѹ��;
#define PFCC_MIN_PRESS				0x14//�趨��Сѹ��;
#define PFCC_PUMPSTART				0x15//����������;
#define PFCC_PUMPSTOP				0x16//��ֹͣ����;
#define PFCC_PRESSCLEAR				0x17//ѹ���Զ���������;
#define PFCC_READ_PRESS				0x18//��ȡѹ�����趨ѹ������Ƶ������
#define PFCC_SET_MODE				0x19//�趨��ģʽ;
#define PFCC_SET_FLOWFIX			0x40//�趨��У����������;

//����Ϊ��λ���������͵�����;
#define PFCC_SEND_PRESS					0x90//��Auֵ����;
#define PFCC_INPUT_EVENT				0x91//���������¼�;
#define PFCC_SYS_ERR					0x92//����ϵͳ����;







#define PFC_SET_FLOW1				0x08//���ó�ϴ����; 
#define PFC_SET_MAXPRESS			0x09//�������ѹ��;
#define PFC_SET_MINPRESS			0x0A//������Сѹ��;
#define PFC_SET_PUMPMOD				0x0C//���ñù���ģʽ;
#define PFC_PRESS_FIX				0x0D//ѹ��У��;
#define PFC_TIME_SYNC				0x0E//ʱ��ͬ������;
#define PFC_PRESS_FIXW				0x0F//ѹ��У׼д��;
#define PFC_FLOW_FIXW				0x0B//����У׼��������;

#define PFC_PUMP					0x0//Pump;
#define PFC_PURGE					0x01//Purge
#define PFC_START					0x0//Start
#define PFC_HOLD					0x05//Hold
#define PFC_STOP					0x02//Stop
#define PFC_READ_PRESS				0x03//��ȡѹ������2;
#define PFC_SET_FLOW2				0x06//���ó�ϴ����2;
#define PFC_DOWNLOAD_PROGRAM		0x07//���س���;



/******************************MUCͨѶ����ʶ��**********************************/
#define MCU_SET_PARAM				0x01//���ò���;
#define MCU_MOTOR_INI				0x02//����ʼ��;
#define MCU_WAVEADD_MOTOR			0x04//�������ӷ���ת�����;
#define MCU_WAVEDEC_MOTOR			0x05//������С����ת�����;
#define MCU_READ_PARAM				0x09//��ȡ����;
#define MCU_READ_VERSION			0x0a//��ȡ�ֿذ汾��;
#define MCU_READ_AU_VAL				0x0b//��ȡAUֵ;
#define MCU_READ_AU_VALB			0x0c//��ȡAUֵ;



struct SendMcuData{
	mbyte type;
	uint32 cmd; 
	uint32 arg;
};



/*�����̨�߼�����;*/
class Worker : public QObject
{

	Q_OBJECT
public:
	Worker(QObject *parent = 0);
	~Worker();

	void startTimer();
	void setPcProtocol(const int &nPro){ m_nPcProtocol = nPro; };
	int getPcProtocol(){return m_nPcProtocol;};

	//! ������ؽӿ�;
	//! ��ȡPCͨѶ�˿�����(���ڻ�����);
	quint32 getConnectPortType(){return m_nConnectPort;}
	//! ��ȡԶ��IP;
	QString getRemoteIP(){return m_strRemoteIp;}
	//! ��ȡԶ�˶˿�;
	quint32 getRemotePort(){return m_nRemotePort;}

	//tcp
	QTcpServer *tcpSocketServer;				//�Žܻ����@2016-06-22����������ͨ��
	QTcpSocket *tcpSocketClientConnection;
	void setupNetworkCommunication();
	//udp
	QUdpSocket *udpSocket;
	void setupMCUCommunication();

private:
	int m_nPcProtocol;							//��λ��ͨѶЭ��ѡ��(0 ��Э�飬 1��Э��);
	QThread m_workerThread;
	QTimer *m_pTimer;								//���ڶ�ʱ��ȡͨѶ���ݵĶ�ʱ��;
	quint8 abc;
	QList< QList<quint32> >sendList4Mcu;			//�����б�;
	QList< QList<quint32> >sendList4Pc;				//�����б�;
	quint32 m_nResendTimeout;						//�ط���ʱ;
	quint32 m_nResendCnt;							//�ط�����;

	//! ��λ����ͨѶ�ӿ�(���ڡ�����);
	quint32 m_nConnectPort;

	//! �����������;
	quint32 m_nLocalPort;

	//! Զ���������;
	quint32 m_nRemotePort;
	QString m_strRemoteIp;


	void setupCommunication();						//����ͨѶ�������Ǵ��ڻ�������;
	void setupSerialCommunication();				//���ô���ͨѶ;

signals:
	void process4Mcu( quint8 type, quint32 cmd, quint32 arg, quint32 add);
	void process4Pc( quint8 type, quint32 cmd, quint32 arg, quint32 add);
	void process4PcClarity( quint8 hID, quint32 hAI, quint32 hPFC, quint32 nVal);
	void communicationError(int reSend);

	

private slots:
	void CmdSend(quint8 type, quint32 cmd, quint32 arg);
	void CmdSend4Mcu(quint8 type, quint32 cmd, quint32 arg);
	void CmdSendClarity(quint32 hAI, quint32 hPFC, quint32 hVal);

	void sendAckClarity(quint8 ack);

public slots:
	void timeoutFunc();
	void processCmd4Pc( mbyte type, uint32 cmd, uint32 arg, uint32 add);				//�Խ��յ������ݽ��д���;
	void processCmd4Clarity(mbyte hID, mbyte hAI, mbyte hPFC, uint32 nVal);		//�Խ��յ������ݽ��д�������ClarityЭ��;
	void processCmd4Mcu( mbyte type, uint32 cmd, uint32 arg, uint32 add);			//�Խ��յ������ݽ��д���;
	void check4Mcu(uint32 cmd);														//У��;

	//�Žܻ����@2016-06-22�����socketͨ��
	//tcp
	void acceptConnection();
	void readClient();
	//udp
	void processPendingDatagrams();

	void l_setConnectPort(int nCon);
	void l_setPcProtocol(int nPro);
	void l_setRemoteIP();
	void l_setRemotePort();
	void l_setLocalIP();
	void l_setLocalPort();
};



#endif // LOGICTHREAD_H
