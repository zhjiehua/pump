#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C"{
#endif


//����ָ��16λ��32λ����
//mbyte���ݻ�������,����һ��mbyte����8λ��������Ҫ�޸Ķ�ȡ�ļ�����

//ֱ��ģ��DSP
#define _QT
#ifdef _QT
#include <stdio.h>
#include <stdlib.h>
#include <QtGlobal>
typedef quint16 uint16;
typedef qint16 int16;
typedef quint32 uint32;
typedef qint32 int32;
typedef short api_res;
typedef quint8 mbyte;
#else
//DSP without 8 bit
#include <stdlib.h>
typedef unsigned int uint16;
typedef int int16;
typedef unsigned long uint32;
typedef long int32;
typedef int api_res;
typedef unsigned int mbyte;
#endif

#define RES_OK		1
#define RES_ERR		-1

#ifdef __cplusplus
}
#endif
#endif
