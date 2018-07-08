#ifndef __PLC_INSTRUCTION_H__
#define __PLC_INSTRUCTION_H__

#ifdef __cplusplus
extern "C"{
#endif

#define MAX_INS_LEN	( 0x10 + 1 )

#ifdef _WIN32

//����ӳ��
enum e_insMap{
	//insNULL,		//null instruction
	insLD,	
	insLDI,
	insLDP,
	insLDF,

	insAND,
	insANI,
	insANDP,
	insANDF,
	insOR,
	insORI,
	insORP,
	insORF,
	insANB,
	insORB,

	insMPS,
	insMRD,
	insMPP,
	insINV,
	insMEP,
	insMEF,

	insOUT,
	insSET,
	insRST,
	insPLS,
	insPLF,

	insMC,
	insMCR,

	insNOP,
	insEND,

	insSTL,
	insRET,

	insADD,
	insSUB,
	insMUL,
	insDIV,
	insINC,
	insDEC,

	insMOV,
	insCMP,

	insCJ,
	insCJP,

	insSRET,
	insFEND,
	insCALL,
	insP
};

//����PLCָ��ϵͳ
enum e_sinsMap{
	sinsLD = 0x00,	
	sinsLDI = 0x01,
	sinsLDP = 0x0202,
	sinsLDF = 0x0302,

	sinsAND = 0x0c,
	sinsANI = 0x0d,
	sinsANDP = 0x0e,
	sinsANDF = 0x0f,
	sinsOR = 0x06,
	sinsORI = 0x07,
	sinsORP = 0x08,
	sinsORF = 0x09,
	sinsANB = 0x19,
	sinsORB = 0x18,

	sinsMPS = 0x1a,
	sinsMRD = 0x1b,
	sinsMPP = 0x1c,
	sinsINV = 0x14,
	sinsMEP = 0x13,
	sinsMEF = 0x12,

	sinsOUT = 0x20,
	sinsOUTT = 0x2103,
	sinsOUTC = 0x2105,
	sinsSET = 0x23,
	sinsRST = 0x24,
	sinsPLS = 0x25,
	sinsPLF = 0x26,

	sinsMC = 0x2c03,
	sinsMCR = 0x3002,

	sinsNOP,
	sinsEND = 0x34,

	sinsSTL = 0x6c0123,
	sinsRET = 0x6c0124,
	sinsSET1 = 0x2302,
	sinsRST1 = 0x2402,		//T��Cͬһ��

	sinsADD = 0x490728,
	sinsSUB = 0x49072A,
	sinsMUL = 0x49072C,
	sinsDIV = 0x49072E,
	sinsINC = 0x4A0300,
	sinsDEC = 0x4A0302,

	//sinsAND< = 0x40050411;

	sinsMOV = 0x4C0500,
	sinsCMP = 0x480706,

	sinsCJ = 0x4d0300,
	sinsCJP = 0x4d030002,

	sinsSRET = 0x6a0107,
	sinsFEND = 0x33,
	sinsCALL = 0x540301,
	sinsP = 0x3c
};



enum e_elementMap{
	scoilX=0x9c,	//����̵��� 0~160
	scoilY=0x9d,	//����̵��� 0~160
	scoilM=0x90,	//�����̵��� 0~9000
	scoilS=0x98,	//״̬		 0~1000
	scoilT=0xc2,	//��ʱ��		 0~255
	scoilC=0xc5,	//������		 0~255
	scoilD=0xa8,	//���ݼĴ��� 0~9000
	scoilR='R',	//��չ
	scoilP=0xd0,	//ָ��
	scoilN=0xd2,	//Ƕ��
	scoilK=0xe8,	//����

	scoilZ='Z',	//�ڲ��м�̵���		0~255
	scoilL='L',	//�ڲ��м䱣�̵ּ���	0~9000 bit
	scoilU='U'	//�ڲ�16λ
};

#endif

//DSPָ��ϵͳ
enum e_dinsMap{
	//����ָ�� ��8λ
	dinsLD = 0x10,	
	dinsLDI = 0x11,
	dinsLDP = 0x12,
	dinsLDF = 0x13,

	dinsAND = 0x1c,
	dinsANI = 0x1d,
	dinsANDP = 0x1e,
	dinsANDF = 0x1f,
	dinsOR = 0x16,
	dinsORI = 0x17,
	dinsORP = 0x18,
	dinsORF = 0x19,
	dinsANB = 0x29,
	dinsORB = 0x28,

	dinsMPS = 0x2a,
	dinsMRD = 0x2b,
	dinsMPP = 0x2c,
	dinsINV = 0x24,
	dinsMEP = 0x23,
	dinsMEF = 0x22,

	dinsOUT = 0x30,
	dinsSET = 0x31,
	dinsRST = 0x32,
	dinsRSTC = 0x33,
	dinsRSTT = 0x34,
	dinsOUTT = 0x35,
	dinsOUTC = 0x36,
	dinsPLS = 0x37,
	dinsPLF = 0x38,

	dinsMC = 0x41,
	dinsMCR = 0x40,

	dinsNOP = 0x42,
	dinsEND = 0x44,

	dinsSTL = 0x50,
	dinsRET = 0x51,
	dinsSET1 = 0x52,
	dinsRST1 = 0x53,

	//����ָ��
	dinsADD = 0x0100,
	dinsSUB = 0x0101,
	dinsMUL = 0x0102,
	dinsDIV = 0x0103,
	dinsINC = 0x0104,
	dinsDEC = 0x0105,

	dinsMOV = 0x0200,
	dinsCMP = 0x0201,

	dinsSRET = 0x0300,
	dinsFEND = 0x0301,
	dinsCALL = 0x0302,
	dinsCJ = 0x0303,
	dinsCJP = 0x0304,

	dinsP = 0x0400
};

enum e_delementMap{
	dcoilX=0x90,	//����̵���
	dcoilY=0x91,	//����̵���
	dcoilM=0x92,	//�����̵���
	dcoilS=0x93,	//״̬		
	dcoilT=0x94,	//��ʱ��		
	dcoilC=0x95,	//������		
	dcoilD=0x96,	//���ݼĴ��� 
	dcoilP=0x97,
	dcoilDT=0xa0,	//��ʱ������	
	dcoilDC=0xa1,	//����������
	dcoilK=0xf0,	//����
	dcoilDK=0xf1	//����˫�ֽ�
};

#ifdef __cplusplus
}
#endif
#endif	

