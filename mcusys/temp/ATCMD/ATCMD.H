#ifndef __ATCMD_H__
#define __ATCMD_H__

	#ifdef ATCMD_MAIN
		#define ATCMD_EXT
	#else
		#define ATCMD_EXT extern
	#endif
	
	#include "..\funcs\func_usart\func_usart.h"
		
	#define ATCMD_TYPE_NULL	0
	#define ATCMD_TYPE_AT	1
	#define ATCMD_TYPE_CBK	2
	#define ATCMD_TYPE_CBK_TOUT	3
		
	#define ATCMD_CBK_RSP_NULL 0
	
	
	
	#define mATCMD_AT(A,B,C)	{(char*)A,ATCMD_TYPE_AT,B,sizeof(A)-1,C}
	#define mATCMD_CBK(A,B,C)	{A,ATCMD_TYPE_CBK,B,0,C}
	#define mATCMD_CBKTOUT(A,B,C)	{A,ATCMD_TYPE_CBK_TOUT,B,0,C}
	#define mATCMD_END	{0,ATCMD_TYPE_NULL,0,0,0}
	
	#define mATCMD_REC(A,B)	{(char*)A,B}
	//---------------------------------------------------------------------
	
	#define CM_ANALYSE_DATA_RET_NULL 0
	#define CM_ANALYSE_DATA_RET_OK 1
	#define CM_ANALYSE_DATA_RET_REC_LOCK 2
	
	typedef int (*Func_CBK_T)(unsigned short cbkid, void *p, unsigned short *rec_n);
	typedef int (*Func_cm_analyse_data_T)(void *pdata,unsigned short *rec_n);
	
	
	
	//主动发送AT命令信息结构体
	typedef struct{
		void *cmd;					//命令字符串/执行函数
		unsigned char type;			//类型0：函数命令，1：AT命令
		unsigned char cbkid;		//回调函数标识
		unsigned short cmd_len;		//命令长度
		unsigned int tout;			//超时时间/延时时间
	}ATCMD_SEND_T;
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	
	//ATCMD 当前状态状态
	#define ATCMD_STA_STOP		0
	#define ATCMD_STA_PROCESS	1
	#define ATCMD_STA_OK		2
	#define ATCMD_STA_ERR		3
	#define ATCMD_STA_TOUT		4
	#define ATCMD_STA_OK_INIT	5
	
	
	#define ATCMD_OPTION_MONI	0x01
	
	//ATCMD 步骤枚举
	enum{
		ATCMD_START,
		ATCMD_STEP_GET_CMD,
		ATCMD_STEP_WAIT_RSP,
		ATCMD_STEP_CBK,
		ATCMD_STEP_CBK_RSP,
		ATCMD_STEP_OK,
		ATCMD_STEP_ERR,
	};

	typedef struct{
		USART_T *pu;					//串口指针
		USART_T *pm;					//监视串口指针
		ATCMD_SEND_T *pcmd;				//发送命令指针集
		Func_cm_analyse_data_T als_data;//数据分析回调函数
		Func_CBK_T cbk;					//回调函数
		time_ms_T tout;					//超时定时器
		time_ms_T rec_tout;				//接收状态超时
		unsigned char cur_cmd;			//当前命令号或出错的命令号
		unsigned char cmd_n;			//总共及条指令
		unsigned char *rec_buf;
		unsigned short rec_n;
		unsigned short rec_buf_size;
		
		unsigned char rec_lock;			//接收状态
		unsigned char rty_n;			//重发次数
		unsigned char rty_cnt;			//重发计数
		unsigned char sta;				//状态，0:停止，1:正在运行，2：完成全部指令，3:错误，4:超时
		unsigned char step;				//内部状态机步骤
		unsigned char option;			//选项，B0:监视使能
	}ATCMD_T;
	 
	int atcmd_init(ATCMD_T *p,USART_T *pu,USART_T *pm,Func_CBK_T cbk,Func_cm_analyse_data_T als_data,unsigned short ren_buf_size);	//初始化
	void atcmd_start(ATCMD_T *p,ATCMD_SEND_T *cmds,unsigned short n);		//复位并启用运行一串AT指令
	unsigned char atcmd_sta(ATCMD_T *p);		//查询状态
	int atcmd(ATCMD_T *p);			//AT指令进程,返回状态
	

#endif
