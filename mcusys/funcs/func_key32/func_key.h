#ifndef __FUNC_KEY_H__
#define __FUNC_KEY_H__

	#ifdef FUNC_KEY_MAIN
	 	#define FUNC_KEY_EXT
	#else
		#define FUNC_KEY_EXT extern
	#endif
	
#include "includes.h" 
#include "key.h"

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//		

#ifndef __FUNC_KEY_DFT
	#define FUNC_KEY_OPTION 	KEY_OPTION_SHORT_NEG
	#define FUNC_KEY_LONG0 		5
	#define FUNC_KEY_LONG1 		10
#endif
		
	#define m_KEY_CODE_BIT(A) (1<<(A-1))		//宏定义，把按键码转换成输入位
	
	#define FUNC_KEY_TM_PERIOD 10	//10ms一次按键计算
	
	typedef struct{
		time_ms_T tm;				//定时器，多少时间进行一次按键进程
		KEY32_T key;				//按键结构体
	}FUNC_KEY_T;
	
	FUNC_KEY_EXT FUNC_KEY_T g_key;	//定义按键全局变量
	void func_key_init(unsigned char option,unsigned short lk_thre0,unsigned short lk_thre1);
	void func_key_msp_init(void);
	void func_key(void);

#endif
