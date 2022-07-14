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

#define APP_UP_LINK_MAIN
#include "app_up_link.h"

/***** 应用描述 这里输入对应用描述 *****/

//+++++User_Code_define Begin+++++//

//定义步骤枚举
enum{
	APP_UP_LINK_STEP_START ,	//启动步骤
	APP_UP_LINK_STEP_CONNECT,
	APP_UP_LINK_STEP_SEND,
	APP_UP_LINK_STEP_DISCONNECT,
	APP_UP_LINK_STEP_END
};
//-----User_Code_define End-----//
/******************************** 实例主体函数定义 ********************************/

void app_up_link_init(APP_UP_LINK_T *p)
{
//+++++User_Code_app_up_link_init Begin+++++//
	printf("System Start!\r\n4G Moudle Power ON\r\nWatting for 4G Ready...\r\n");
//-----User_Code_app_up_link_init End-----//
}


void app_up_link(APP_UP_LINK_T *p)
{
//+++++User_Code_app_up_link_Entry Begin+++++//
	BRK_2P_VAL_V100_T *pbrk2;
	int i;
//-----User_Code_app_up_link_Entry End-----//
//+++++User_Code_app_up_link_Step Begin+++++//
	switch(p->step)
	{
	//步骤 0 : 启动步骤
	case APP_UP_LINK_STEP_START:
		if(net_4g_sta())
		{
			printf("4G Net Ready!\r\n");
			tcp_connect("\"182.254.214.242\",8965");
			printf("Connect to:\"182.254.214.242\",8965\r\nWatting connect...\r\n");
			p->step=APP_UP_LINK_STEP_CONNECT;
		}
		break;
	case APP_UP_LINK_STEP_CONNECT:
		if(tcp_sta())
		{
			printf("Connected OK\r\n");
			p->step=APP_UP_LINK_STEP_SEND;
		}
		break;
	case APP_UP_LINK_STEP_SEND:
		if(LMSZ(p->tm))						//判断定时器剩余时间是否为0
		{
			pbrk2 = g_app_down_link.slot[0].buf;
			sprintf(p->buf,"Ua=%3.2fV",pbrk2->val.ua);
			printf("Send Data: %s\r\n",p->buf);
			tcp_send(p->buf,strlen(p->buf));
			LMSS(p->tm,60000);				//重置定时器
		}
		//------------------------------------------------------------
		i=tcp_chkrec();		//检查是否有数据接收
		if(i>0)				//有数据
		{
			i=tcp_rd_data(p->buf,sizeof(p->buf)-1);		//接收数据
			printf("Receive Data: %s\r\n",p->buf);
		}
		//------------------------------------------------------------
		break;
	case APP_UP_LINK_STEP_DISCONNECT:
		
		break;
	case APP_UP_LINK_STEP_END:
		break;
	}
//-----User_Code_app_up_link_Step End-----//
//+++++User_Code_app_up_link_Exit Begin+++++//
//-----User_Code_app_up_link_Exit End-----//
}

//-------------------- 1ms中断函数 ----------------------
//需要使用的用func_LTB_add(&g_var.func_ltb_tick,&xxx_tick);在init程序或者OS任务while之前进行注册
//这样1ms的tick中断就会自动运行这个函数，建议普通延时使用left_ms操作，只有实时性要求非常高的代码放在这里
void app_up_link_tick(void)
{
//+++++User_Code_tick Begin+++++//
//-----User_Code_tick End-----//
}
//------------------ 1ms中断函数结束 --------------------
//+++++User_Code_end Begin+++++//
//-----User_Code_end End-----//
