#ifndef __FUNC_CM_H__
#define __FUNC_CM_H__
#include "includes.h"

	typedef struct{
		unsigned char slv;
		unsigned short hr_n;
		unsigned short *phr;
	}FUNC_CM_SLV_HR_T;
	
//+++++User_Code_define Begin+++++//
	#define OSSLK OSSchedLock()
	#define OSSULK OSSchedUnlock()
	
	#define NET_BUF_SIZE (2048+20)  //数据大小
	#define LINK_NB 2

#define __FUNC_CM_DEFINE
//-----User_Code_define End-----//
//=============================================================================
//使用主模块需要对以下内容进行定义	
#ifndef __FUNC_CM_DEFINE
	#define NET_BUF_SIZE (1048+20)  //数据大小
	#define LINK_NB 2
#endif
//=============================================================================
	
	
	#ifdef FUNC_CM_MAIN
		#define FUNC_CM_EXT
	#else
		#define FUNC_CM_EXT extern
	#endif
	
	
	#define FUNC_CM_LINK_FUNC_NULL			0
	#define FUNC_CM_LINK_FUNC_SEND_DATA		1
	#define FUNC_CM_LINK_FUNC_CONNECT		2
	#define FUNC_CM_LINK_FUNC_DISCONNECT	3
	
	//连接变量结构体
	typedef struct{
		unsigned char connected;		//连接状态
		unsigned char func;
		RING_BUF_T send_buf;
		RING_BUF_T rec_buf;
	}FUNC_CM_LINK_T;
	
	//通讯模块变量结构体
	typedef struct{
		unsigned char step;			//步骤
		unsigned char err_step;
		time_ms_T tout;				//通用定时器
		time_ms_T csq_tout;			//CSQ的定时周期
		ATCMD_T atcmd;				//定义AT赤岭命令函数变量实例
		ATCMD_SEND_T at_send;		//定义一个AT命令结构体变量，用于动态发送命令
		char CSGN[16];				//用于保存IMEI
		char CIMI[16];				//用于接收卡号
		char CSQ[4];				//用于保存CSQ
		unsigned char link_n;		//有几个连接
		unsigned char cur_link;		//当前连接
		unsigned char cur_func;		//当前指令
		FUNC_CM_LINK_T link[2];		//两个链接
		
		char *at_send_buf;			//做成最后要向模组发送的字符串缓冲区
		unsigned short at_send_size;
		unsigned short at_send_n;	
	}FUNC_CM_T;
	
	
	
	FUNC_CM_EXT FUNC_CM_T g_cm;
	

	void func_cm_init(FUNC_CM_T *p,USART_T *pu,USART_T *pm,unsigned char link_n,unsigned short buf_size);		//初始化函数
	int func_cm_loop(FUNC_CM_T *p);	//进程
	void func_cm_reset(FUNC_CM_T *p);
	
	//------------------------------------------------------------------------------------
	//外部调用接口函数
	int func_cm_link_connect(FUNC_CM_T *p,unsigned char link,char *ip_port);
	int func_cm_link_disconnect(FUNC_CM_T *p,unsigned char link);
	int func_cm_link_send(FUNC_CM_T *p,unsigned char link,void *pdata,unsigned short n);
	
	unsigned char func_cm_sta(FUNC_CM_T *p);
	//检测接收缓冲区是否有数据
	int func_cm_link_chkrec(FUNC_CM_T *p,unsigned char link);
	//读取缓冲区数据，若需要读取的数据量大于缓冲区，返回错误
	//正确返回读取的字节数
	int func_cm_link_rd_data(FUNC_CM_T *p,unsigned char link,void *pdata,unsigned short max_n);
	//-------------------------------------------------------------------------------------
	//硬件配置函数
	void __func_cm_msp_init(void);
	void __func_cm_pwr_on(void);
	void __func_cm_pwr_off(void);
	void __func_cm_sw_on(void);
	void __func_cm_sw_off(void);
	void __func_cm_rst_hi(void);
	void __func_cm_rst_lo(void);
	unsigned char __func_cm_read_vcc(void);
	//-------------------------------------------------------------------------------------
#endif
