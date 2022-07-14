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

#define USER_CTRL_MAIN
#include "user_ctrl.h"

#include "includes.h"

/***** 应用描述 这里输入对应用描述 *****/

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//


int tcp_connect(char *ip_port)
{
	return func_cm_link_connect(&g_cm,0,ip_port);
}

int tcp_disconnect(void)
{
	return func_cm_link_disconnect(&g_cm,0);
}

int tcp_send(void *pdata,unsigned short n)
{
	return func_cm_link_send(&g_cm,0,pdata,n);
}

int tcp_chkrec(void)
{
	return(func_cm_link_chkrec(&g_cm,0));
}

int tcp_rd_data(void *pdata,unsigned short max_n)
{
	return func_cm_link_rd_data(&g_cm,0,pdata,max_n);
}


int tcp_sta(void)
{
	return(g_cm.link[0].connected);
}

//+++++User_Code_end Begin+++++//
//-----User_Code_end End-----//
