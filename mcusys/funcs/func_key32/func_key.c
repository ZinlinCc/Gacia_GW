/************************************Copyright (c)**************************************
                                          

--------------文件信息------------------------------------------------------------------
文 件 名: func_key.c
创 建 人: Zhoucongwen
email:	  mcuast@qq.com
创建日期: 2019年04月15日
描    述: 按键进程
---------- 版本信息---------------------------------------------------------------------
 版    本: V1.0
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/ 
#define FUNC_KEY_MAIN
#include "func_key.h"
#include "includes.h"

//======================================================================================
extern unsigned int func_key_msp_get_input(unsigned int *input);
extern void func_key_msp_init(void);
//======================================================================================
void func_key_init(unsigned char option,unsigned short lk_thre0,unsigned short lk_thre1)
{
	func_key_msp_init();
	g_key.key.option=option;
	g_key.key.lk_thre0=lk_thre0*100;
	g_key.key.lk_thre1=lk_thre1*100;
}

void func_key(void)		//这个程序每10-20mS调用一次
{
	unsigned int input;
	unsigned int key_nb;
	
	if(left_ms(&g_key.tm)==0)
	{
		left_ms_set(&g_key.tm,FUNC_KEY_TM_PERIOD);	//设定下一次延时时间
		//--------------------------------------------------
		//获取按键原始端口信息
		key_nb = func_key_msp_get_input(&input);
		//--------------------------------------------------
		key32_oper(&g_key.key,input,key_nb);	//对按键进行处理，有效按键信息进入队列
		//--------------------------------------------------
	}
}

/*
//键码的使用
void xxxxx(void)
{
	unsigned char key_code;

	key_code=get_key(&g_key);

	switch(key_code)
	{
		case KEY_CODE0:
			//键码0的操作
			break;
		case KEY_CODE1:
			break;
		case KEY_CODE2:
			break;
		case KEY_CODE3:
			break;
		case KEY_CODE4:
			break;
		case KEY_CODE5:
			break;
		case KEY_CODE6:
			break;
		case KEY_CODE7:
			break;
		default:
			break;
	}
}
*/

/*
typedef struct{
	int x0;
	int y0;
	int x1;
	int y1;
	int color;
}DW_LINE_T;


typedef struct{
	u16 x0;
	u16 y0;
	u16 x1;
	u16 y1;
	u16 color;
	u8 mode;
}DW_RECT_T;



int draw_line(unsigned char *buf,DW_LINE_T *p);
*/
