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

#include "includes.h"

#include "main.h"

//+++++User_Code_define Begin+++++//
USART_T *pu_printf;
// 重定义printf
int fputc(int ch, FILE *fp)
{
	// while(ST_USART_CMD.t_s!=ST_USART_CMD.t_e);		//采用中断发送，取消这句
	if (pu_printf)
	{
		usart_send(pu_printf, (unsigned char *)&ch, 1);
	}
	return ch;
}

//-----User_Code_define End-----//
__align(8) OSStk_T Task_system_hi_Stk[128];
__align(8) OSStk_T Task_net_link_Stk[256];
__align(8) OSStk_T Task_system_lo_Stk[256];
__align(8) OSStk_T Task_net_user_Stk[256];
//__align(8) unsigned int Task_net_user_Stk[256];
void mcusys_main_init(void)
{
	OSErr_T err = 0;
	//+++++User_Code_init_var Begin+++++//
	USART_T *pu;
	USART_T *pm;

	//-----User_Code_init_var End-----//
	//------------------ 系统功能初始化 ------------------

	//+++++User_Code_ccos_init_before Begin+++++//
	// printf("Ver:V%2.2f\r\n",1.00);	//在没有进入操作系统之前运行printf可以使系统控制寄存器control=0x04,开启FPU，在勾选microlib的情况下，这样操作可以在OS启用FPU
	//-----User_Code_ccos_init_before End-----//
	OS_I_DisAll();
	OSInit();
	//+++++User_Code_ccos_init_after Begin+++++//
	//-----User_Code_ccos_init_after End-----//

	delay_us_set_k(1000);
	//------------------ 功能模块库 --------------------

	//+++++User_Code_func_usart_init_before Begin+++++//
	//-----User_Code_func_usart_init_before End-----//
	func_usart_init();
	//+++++User_Code_func_usart_init_after Begin+++++//
	//-----User_Code_func_usart_init_after End-----//

	//+++++User_Code_func_key32_init_before Begin+++++//
	//-----User_Code_func_key32_init_before End-----//
	func_key_init(FUNC_KEY_OPTION, FUNC_KEY_LONG0, FUNC_KEY_LONG1);
	//+++++User_Code_func_key32_init_after Begin+++++//
	//-----User_Code_func_key32_init_after End-----//

	//+++++User_Code_func_flash_1b_store_init_before Begin+++++//
	//-----User_Code_func_flash_1b_store_init_before End-----//
	func_flash_1b_store_init(&g_func_flash_1b_store, FLASH_STORE_START_ADR, FLASH_STORE_SIZE, &g_fsd, &g_fsd_dft, sizeof(g_fsd), FLASH_LOCK_TM, FLASH_STORE_RAMBAK, FLASH_STORE_KEY_EN);
	//+++++User_Code_func_flash_1b_store_init_after Begin+++++//
	//-----User_Code_func_flash_1b_store_init_after End-----//

	//+++++User_Code_func_modbus_master_init_before Begin+++++//
	pu = usart_open("18n1hpu2abp+", 115200, 256, 256); //半双工，PA1~RS485控制，中断优先级2
	// pu=usart_open("28n1hpu2a1p+",115200,256,256);		//半双工，PA1~RS485控制，中断优先级2
	//-----User_Code_func_modbus_master_init_before End-----//
	// func_modbus_master_init(&g_func_modbus_master0,pu,master0_tb_slv_hr);
	func_modbus_master_init(&g_func_modbus_master0, pu, g_func_modbus_master0_slv_hr);
	//+++++User_Code_func_modbus_master_init_after Begin+++++//
	//--------------------------------------------------
	//对通讯模块初始化
	// pu_printf=usart_open("48n1hpu200p+",115200,NET_BUF_SIZE,NET_BUF_SIZE);		//半双工，PA15~RS485控制，中断优先级2
	pu_printf = usart_open("28n1hpu2a1p+", 115200, NET_BUF_SIZE, NET_BUF_SIZE); //半双工，PA15~RS485控制，中断优先级2
	printf("Ver:V%2.2f \r\n", 1.00);												//在没有进入操作系统之前运行printf可以使系统控制寄存器control=0x04,开启FPU，在勾选microlib的情况下，这样操作可以在OS启用FPU

	//C我的板子得用串口5，陈老师的板子要改成串口3
	pu = usart_open("58n1fpu200p+", 115200, NET_BUF_SIZE, NET_BUF_SIZE); //全双工，无RS485控制，中断优先级2
	pm = usart_open("48n1fpu200p+", 115200, NET_BUF_SIZE, NET_BUF_SIZE); //半双工，PA15~RS485控制，中断优先级2
	// pm=usart_open("18n1hpu2abp+",115200,NET_BUF_SIZE,NET_BUF_SIZE);		//半双工，PA15~RS485控制，中断优先级2

	func_cm_init(&g_cm, pu, pm, LINK_NB, NET_BUF_SIZE);
	//-----User_Code_func_modbus_master_init_after End-----//

	//----------- 用户应用init函数 -----------

	//+++++User_Code_app_down_linkinit_before Begin+++++//
	//-----User_Code_app_down_linkinit_before End-----//
	app_down_link_init(&g_app_down_link);
	//+++++User_Code_app_down_linkinit_after Begin+++++//
	//-----User_Code_app_down_linkinit_after End-----//

	//+++++User_Code_app_up_linkinit_before Begin+++++//
	//-----User_Code_app_up_linkinit_before End-----//
	app_up_link_init(&g_app_up_link);
	//+++++User_Code_app_up_linkinit_after Begin+++++//
	//-----User_Code_app_up_linkinit_after End-----//

	//----------- OS init函数 -----------
	//创建任务，参数依次为：任务函数指针，任务优先级，堆栈顶部指针，堆栈大小，传入参数？（最后一个不确定哎0.0）
	//任务创建成功则返回0
	err |= OSTaskCreate(Task_system_hi, OS_PI_Task_system_hi, (OSStk_T *)&LastMember(Task_system_hi_Stk), countof(Task_system_hi_Stk), OS_TaskNoPar);
	printf("Task_system_hi---Task to create success!\r\n");
	err |= OSTaskCreate(Task_net_link, OS_PI_Task_net_link, (OSStk_T *)&LastMember(Task_net_link_Stk), countof(Task_net_link_Stk), OS_TaskNoPar);
	printf("Task_net_link---Task to create success!\r\n");
	err |= OSTaskCreate(Task_system_lo, OS_PI_Task_system_lo, (OSStk_T *)&LastMember(Task_system_lo_Stk), countof(Task_system_lo_Stk), OS_TaskNoPar);
	printf("Task_system_lo---Task to create success!\r\n");
	err |= OSTaskCreate(Task_net_user, OS_PI_Task_net_user, (OSStk_T *)&LastMember(Task_net_user_Stk), countof(Task_net_user_Stk), OS_TaskNoPar);
	printf("Task_net_user---Task to create success!\r\n");

	while (err);
	printf("Gateway initialization succeeded!\r\n");
	OSStartTask();

	//+++++User_Code_init Begin+++++//
	//-----User_Code_init End-----//
}

void Task_system_hi(void)
{
	//+++++User_Code_Task_system_hi_define Begin+++++//
	//-----User_Code_Task_system_hi_define End-----//
	while (1)
	{
		OSEventPend(0);
		//+++++User_Code_Task_system_hi_0 Begin+++++//
		//-----User_Code_Task_system_hi_0 End-----//
		//+++++User_Code_Task_system_hi_1 Begin+++++//
		//-----User_Code_Task_system_hi_1 End-----//
	}
}

void Task_net_link(void)
{
	//+++++User_Code_Task_net_link_define Begin+++++//
	//-----User_Code_Task_net_link_define End-----//
	while (1)
	{
		//+++++User_Code_Task_net_link_0 Begin+++++//
		//-----User_Code_Task_net_link_0 End-----//
		//+++++User_Code_Task_net_link_1 Begin+++++//
		func_cm_loop(&g_cm);
		//-----User_Code_Task_net_link_1 End-----//
		OSTimeDly(10);
	}
}

void Task_system_lo(void)
{
	//+++++User_Code_Task_system_lo_define Begin+++++//
	//-----User_Code_Task_system_lo_define End-----//
	while (1)
	{
		//+++++User_Code_Task_system_lo_0 Begin+++++//
		//-----User_Code_Task_system_lo_0 End-----//

		//+++++User_Code_func_key32_exec_before Begin+++++//
		//-----User_Code_func_key32_exec_before End-----//
		func_key();
		//+++++User_Code_func_key32_exec_after Begin+++++//
		//-----User_Code_func_key32_exec_after End-----//

		//+++++User_Code_func_flash_1b_store_exec_before Begin+++++//
		//-----User_Code_func_flash_1b_store_exec_before End-----//
		func_flash_1b_store(&g_func_flash_1b_store);
		//+++++User_Code_func_flash_1b_store_exec_after Begin+++++//
		//-----User_Code_func_flash_1b_store_exec_after End-----//

		//+++++User_Code_func_modbus_master_exec_before Begin+++++//
		//-----User_Code_func_modbus_master_exec_before End-----//
		func_modbus_master_exec(&g_func_modbus_master0);
		//+++++User_Code_func_modbus_master_exec_after Begin+++++//
		//-----User_Code_func_modbus_master_exec_after End-----//

		//+++++User_Code_app_down_linkexec_before Begin+++++//
		//-----User_Code_app_down_linkexec_before End-----//
		app_down_link(&g_app_down_link);
		//+++++User_Code_app_down_linkexec_after Begin+++++//
		//-----User_Code_app_down_linkexec_after End-----//

		//+++++User_Code_Task_system_lo_1 Begin+++++//
		//-----User_Code_Task_system_lo_1 End-----//
		OSTimeDly(10);
	}
}

void Task_net_user(void)
{
	//+++++User_Code_Task_net_user_define Begin+++++//
	//-----User_Code_Task_net_user_define End-----//
	while (1)
	{
		//+++++User_Code_Task_net_user_0 Begin+++++//
		//-----User_Code_Task_net_user_0 End-----//

		//+++++User_Code_app_up_linkexec_before Begin+++++//
		//-----User_Code_app_up_linkexec_before End-----//
		app_up_link(&g_app_up_link);
		//+++++User_Code_app_up_linkexec_after Begin+++++//
		//-----User_Code_app_up_linkexec_after End-----//

		//+++++User_Code_Task_net_user_1 Begin+++++//
		//-----User_Code_Task_net_user_1 End-----//
		// OSTimeDly(0);
	}
}

void mcusys_main(void)
{
	//+++++User_Code_main_var Begin+++++//
	//-----User_Code_main_var End-----//
	//------------------ 系统功能执行函数 ------------------
	//------------------ 系统功能执行函数 ------------------
	//------------------ 功能模块库 --------------------
	//----------- 用户应用运行函数 -----------

	//+++++User_Code_exec Begin+++++//
	//-----User_Code_exec End-----//
}
