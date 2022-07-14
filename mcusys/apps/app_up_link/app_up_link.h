/************************************Copyright (c)**************************************

            单片机编程助手，帮您从繁杂的重复劳动中脱离出来，专心做您的创新与业务                  

                                    wwww.mcuabc.cn

----------------------------------------文件信息-----------------------------------------
文 件 名: 
创 建 人: 
email:	mcusys@qq.com
创建日期: 
描    述: 
----------------------------------------版本信息-----------------------------------------
 版    本: V1.00
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) xsy,ccmcu,zcw 2019/06/18
All rights reserved
****************************************************************************************/

#ifndef __APP_UP_LINK_H__
#define __APP_UP_LINK_H__

#include "includes.h"

/***** 应用描述 这里输入对应用描述 *****/

#ifdef APP_UP_LINK_MAIN
	#define APP_UP_LINK_EXT 
#else
	#define APP_UP_LINK_EXT  extern
#endif

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//
//定义应用变量
typedef struct{
//+++++User_Code_VarDef Begin+++++//
	unsigned char step;
	time_ms_T tm;
	char buf[1024];
//-----User_Code_VarDef End-----//
//应用需要的变量
} APP_UP_LINK_T;

//定义应用实例
APP_UP_LINK_EXT APP_UP_LINK_T g_app_up_link;

//定义应用函数
void app_up_link_init(APP_UP_LINK_T*);

void app_up_link(APP_UP_LINK_T*);

//+++++User_Code_Methods Begin+++++//
//-----User_Code_Methods End-----//
#endif
