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

#ifndef __APP_DOWN_LINK_H__
#define __APP_DOWN_LINK_H__

#include "includes.h"

/***** 应用描述 这里输入对应用描述 *****/

#ifdef APP_DOWN_LINK_MAIN
	#define APP_DOWN_LINK_EXT 
#else
	#define APP_DOWN_LINK_EXT  extern
#endif

//+++++User_Code_define Begin+++++//
#define LED0(A) GPIO_WriteBit(GPIOB, GPIO_Pins_8, (BitState)(A))		//:GPIO_TogglePin(GPIOB, GPIO_Pins_8)
#define LED0_TOG GPIO_TogglePin(GPIOB, GPIO_Pins_8)
#define LED1(A) GPIO_WriteBit(GPIOB, GPIO_Pins_9, (BitState)(A))
#define LED2(A) GPIO_WriteBit(GPIOB, GPIO_Pins_7, (BitState)(A))

#define DL_SOLT_SIZE 32

#define m_PRODUCT_VER(A,B)		((A<<16) | B)
	
#define PRODUCT_CODE_BRK_1P_V100	m_PRODUCT_VER(1,100)
#define PRODUCT_CODE_BRK_2P_V100	m_PRODUCT_VER(2,100)
#define PRODUCT_CODE_BRK_3P_V100	m_PRODUCT_VER(3,100)
#define PRODUCT_CODE_BRK_4P_V100	m_PRODUCT_VER(4,100)


	
//产品通用结构体
typedef struct{
	unsigned int product_code_s_ver;	//产品代码与软件版本号
	unsigned short hver;			//硬件版本
	char sn[21];					//序列号
	unsigned char b_online;
}PRODUCT_T;


//---------------------------------------------------------
typedef struct{
	PRODUCT_T product;
	//设定值
	struct{
		unsigned char il_en;		//漏电保护使能
		unsigned char oc_en;		//过流保护使能
		unsigned char upro_en;		//高低压保护使能
		unsigned char tpro_en;			//温度保护使能
		unsigned short in;			//额定电流
		unsigned short il_thre;		//漏电保护阈值
		unsigned short oc_tm;		//过流保护时间
		unsigned short upro_tm;		//过压欠压保护时间
		unsigned short urst_tm;		//过压欠压恢复时间
	}set;
	//控制命令
	unsigned short ctrl;
	//状态值
	struct{
		unsigned char hs_sta;			//手柄状态
		unsigned char lock_sta;			//挂锁状态
		unsigned char b_manual_open;	//手工分闸标志
		unsigned short pro;				//保护标志
		unsigned short al;				//告警标志
		unsigned short err;				//错误标志
	}sta;
	//实时测量数值
	struct{
		float freq;
		float tb;
		float il;
		float ua;
		float ia;
		float pa;
		float qa;
		float sa;
		float epa;
		float pfa;
		float t1;
		float t2;
	}val;
}BRK_2P_VAL_V100_T;

//---------------------------------------------------------
typedef struct{
	PRODUCT_T product;
	//设定值
	struct{
		unsigned char il_en;		//漏电保护使能
		unsigned char oc_en;		//过流保护使能
		unsigned char upro_en;		//高低压保护使能
		unsigned char tpro_en;			//温度保护使能
		unsigned short in;			//额定电流
		unsigned short il_thre;		//漏电保护阈值
		unsigned short oc_tm;		//过流保护时间
		unsigned short upro_tm;		//过压欠压保护时间
		unsigned short urst_tm;		//过压欠压恢复时间
	}set;
	//控制命令
	unsigned short ctrl;
	//状态值
	struct{
		unsigned char hs_sta;			//手柄状态
		unsigned char lock_sta;			//挂锁状态
		unsigned char b_manual_open;	//手工分闸标志
		unsigned short pro;				//保护标志
		unsigned short al;				//告警标志
		unsigned short err;				//错误标志
	}sta;
	//实时测量数值
	struct{
		float freq;
		float tb;
		float il;
		float ua;
		float ia;
		float pa;
		float qa;
		float sa;
		float epa;
		float pfa;
		float t1;
		float t2;
	}val;
}BRK_1P_VAL_V100_T;

//---------------------------------------------------------

//下行位置产品描述
typedef struct{
	PRODUCT_T pdt;				//产品描述
	void *buf; 					//数据缓存区
	unsigned short *phr;		//ModbusHR指针
	unsigned short hr_size;
	unsigned short buf_size;
	time_ms_T tm_online;
}DL_SLOT_T;



//-----User_Code_define End-----//
//定义应用变量
typedef struct{
//+++++User_Code_VarDef Begin+++++//
	unsigned char step;
	DL_SLOT_T slot[DL_SOLT_SIZE];
//-----User_Code_VarDef End-----//
//应用需要的变量
} APP_DOWN_LINK_T;

//定义应用实例
APP_DOWN_LINK_EXT APP_DOWN_LINK_T g_app_down_link;

//定义应用函数
void app_down_link_init(APP_DOWN_LINK_T*);

void app_down_link(APP_DOWN_LINK_T*);

//+++++User_Code_Methods Begin+++++//
//-----User_Code_Methods End-----//
#endif
