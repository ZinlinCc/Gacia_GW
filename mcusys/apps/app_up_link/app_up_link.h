/************************************Copyright (c)**************************************

            ��Ƭ��������֣������ӷ��ӵ��ظ��Ͷ������������ר�������Ĵ�����ҵ��                  

                                    wwww.mcuabc.cn

----------------------------------------�ļ���Ϣ-----------------------------------------
�� �� ��: 
�� �� ��: 
email:	mcusys@qq.com
��������: 
��    ��: 
----------------------------------------�汾��Ϣ-----------------------------------------
 ��    ��: V1.00
 ˵    ��: 
----------------------------------------------------------------------------------------
Copyright(C) xsy,ccmcu,zcw 2019/06/18
All rights reserved
****************************************************************************************/

#ifndef __APP_UP_LINK_H__
#define __APP_UP_LINK_H__

#include "includes.h"

/***** Ӧ������ ���������Ӧ������ *****/

#ifdef APP_UP_LINK_MAIN
	#define APP_UP_LINK_EXT 
#else
	#define APP_UP_LINK_EXT  extern
#endif

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//
//����Ӧ�ñ���
typedef struct{
//+++++User_Code_VarDef Begin+++++//
	unsigned char step;
	time_ms_T tm;
	char buf[1024];
//-----User_Code_VarDef End-----//
//Ӧ����Ҫ�ı���
} APP_UP_LINK_T;

//����Ӧ��ʵ��
APP_UP_LINK_EXT APP_UP_LINK_T g_app_up_link;

//����Ӧ�ú���
void app_up_link_init(APP_UP_LINK_T*);

void app_up_link(APP_UP_LINK_T*);

//+++++User_Code_Methods Begin+++++//
//-----User_Code_Methods End-----//
#endif
