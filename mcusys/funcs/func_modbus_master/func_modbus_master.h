#ifndef __FUNC_MODBUS_MASTER_H__
#define __FUNC_MODBUS_MASTER_H__
#include "includes.h"

	typedef struct{
		unsigned char slv;
		unsigned short hr_n;
		unsigned short *phr;
	}FUNC_MODBUS_MASTER_SLV_HR_T;
	
//+++++User_Code_define Begin+++++//
//	#ifdef FUNC_MODBUS_MASTER_MAIN
//		//从机HR对照表，一主多从的时候定义每个从机对应的HR
//		const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[]={
//			{1,100,&HR[0][0]},	//从机地址，HR数量，HR指针
//			{2,100,&HR[1][0]},	//从机地址，HR数量，HR指针
//			{3,100,&HR[2][0]},	//从机地址，HR数量，HR指针
//			{0,0,0}
//		};
//		//const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[]={
//		//	{10,100,HR},	//从机地址，HR数量，HR指针
//		//	{0,0,0}
//		//};
//	#else
//		extern const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[];
//		//extern const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[];
//	#endif
	
	#define FUNC_MODBUS_MASTER_CHKBUS_DELAY 10	//判断总线空闲延时10mS-20mS
	#define FUNC_MODBUS_MASTER_SEND_DELAY 10	//发送前延时10mS
	#define FUNC_MODBUS_MASTER_SEND_PERIOD 100	//发送周期100ms,0表示连续发送
	#define FUNC_MODBUS_MASTER_SEND_TOUT 1000	//发送超时100mS
	#define FUNC_MODBUS_MASTER_SEND_RTY 1		//重发1次
	#define FUNC_MODBUS_MASTER_CYC_NB 64			//循环命令队列长度
	#define FUNC_MODBUS_MASTER_INJ_NB 32			//注入命令队列长度
#define __FUNC_MODBUS_MASTER_DEFINE
//-----User_Code_define End-----//
//=============================================================================
//使用主模块需要对以下内容进行定义	
#ifndef __FUNC_MODBUS_MASTER_DEFINE
	#ifdef FUNC_MODBUS_MASTER_MAIN
		//从机HR对照表，一主多从的时候定义每个从机对应的HR
		const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[]={
			{10,100,HR},	//从机地址，HR数量，HR指针
			{0,0,0}
		};
		const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[]={
			{10,100,HR},	//从机地址，HR数量，HR指针
			{0,0,0}
		};
	#else
		extern const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[];
		extern const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[];
	#endif
	#define FUNC_MODBUS_MASTER_CHKBUS_DELAY 10	//判断总线空闲延时10mS-20mS
	#define FUNC_MODBUS_MASTER_SEND_DELAY 10	//发送前延时10mS
	#define FUNC_MODBUS_MASTER_SEND_PERIOD 100	//发送周期100ms,0表示连续发送
	#define FUNC_MODBUS_MASTER_SEND_TOUT 1000	//发送超时100mS
	#define FUNC_MODBUS_MASTER_SEND_RTY 1		//重发1次
	#define FUNC_MODBUS_MASTER_CYC_NB 4			//循环命令队列长度
	#define FUNC_MODBUS_MASTER_INJ_NB 2			//注入命令队列长度
#endif
//=============================================================================
	
	
	#ifdef FUNC_MODBUS_MASTER_MAIN
		#define FUNC_MODBUS_MASTER_EXT
	#else
		#define FUNC_MODBUS_MASTER_EXT extern
	#endif
	
//	typedef struct{
//		unsigned short send_tout;
//		unsigned char send_rty;
//		unsigned char send_delay;
//		unsigned char func_cyc_nb;
//		unsigned char func_inj_nb;
//	}FUNC_MODBUS_MASTER_INIT_T;	

	//typedef void (*mf_func)(void);
				
	typedef struct{
		unsigned char slv;
		unsigned char func;
		unsigned short da_adr;
		unsigned short da_n;
	#ifdef MODBUS_RW_EN
		unsigned short rww_adr;
		unsigned short rww_n;
	#endif
		//mf_func mff;
	}FUNC_MODBUS_MASTER_FUNC_T;
	
	typedef struct{
		unsigned char step;
		unsigned char send_delay;		//发送前延时
		unsigned char send_rty_nb;		//重发次数
		unsigned char send_rty_cnt;		//重发计数
		unsigned char func_cyc_nb;		//循环指令条数
		unsigned char func_cyc_cur;		//当前循环指令
		unsigned char func_inj_h;		//注入指令头
		unsigned char func_inj_t;		//注入指令尾
		unsigned short send_tout;		//发送超时
		unsigned short send_period;		//通讯周期
		unsigned short err_cnt;			//错误计数
		unsigned char cur_func_mode;	//当前操作的是循环指令还是注入指令
		USART_T *pu;					//串口指针
		time_ms_T tm;					//定时器
		time_ms_T tm_period;					//定时器
		const FUNC_MODBUS_MASTER_SLV_HR_T *tb_slv_hr;	//从机HR对应表
		MODBUS_T md;					//MODBUS结构体
		FUNC_MODBUS_MASTER_FUNC_T func_cyc[FUNC_MODBUS_MASTER_CYC_NB];
		FUNC_MODBUS_MASTER_FUNC_T func_inj[FUNC_MODBUS_MASTER_INJ_NB];
	}FUNC_MODBUS_MASTER_T;
	
	FUNC_MODBUS_MASTER_EXT FUNC_MODBUS_MASTER_T g_func_modbus_master0;
	FUNC_MODBUS_MASTER_EXT FUNC_MODBUS_MASTER_T g_func_modbus_master1;
	FUNC_MODBUS_MASTER_EXT FUNC_MODBUS_MASTER_SLV_HR_T g_func_modbus_master0_slv_hr[32];
	
	//FUNC_MODBUS_MASTER_EXT unsigned short HR[MODBUS_SLV_HR_NB];
	
	void func_modbus_master_init(FUNC_MODBUS_MASTER_T *p,USART_T *pu,const FUNC_MODBUS_MASTER_SLV_HR_T *tb_slv_hr);
	void func_modbus_master_exec(FUNC_MODBUS_MASTER_T *p);
	
	typedef enum{
		FUNC_MODBUS_MASTER_FUNC_CYC,
		FUNC_MODBUS_MASTER_FUNC_INJ,
	}FUNC_MODBUS_MASTER_FUNC_E;
	unsigned char func_modbus_master_add_func(FUNC_MODBUS_MASTER_T *p,FUNC_MODBUS_MASTER_FUNC_T *pf,FUNC_MODBUS_MASTER_FUNC_E type);
	void func_modbus_master_clr_func(FUNC_MODBUS_MASTER_T *p,FUNC_MODBUS_MASTER_FUNC_E type);

#endif
