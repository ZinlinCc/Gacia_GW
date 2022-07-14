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
//		//�ӻ�HR���ձ�һ����ӵ�ʱ����ÿ���ӻ���Ӧ��HR
//		const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[]={
//			{1,100,&HR[0][0]},	//�ӻ���ַ��HR������HRָ��
//			{2,100,&HR[1][0]},	//�ӻ���ַ��HR������HRָ��
//			{3,100,&HR[2][0]},	//�ӻ���ַ��HR������HRָ��
//			{0,0,0}
//		};
//		//const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[]={
//		//	{10,100,HR},	//�ӻ���ַ��HR������HRָ��
//		//	{0,0,0}
//		//};
//	#else
//		extern const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[];
//		//extern const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[];
//	#endif
	
	#define FUNC_MODBUS_MASTER_CHKBUS_DELAY 10	//�ж����߿�����ʱ10mS-20mS
	#define FUNC_MODBUS_MASTER_SEND_DELAY 10	//����ǰ��ʱ10mS
	#define FUNC_MODBUS_MASTER_SEND_PERIOD 100	//��������100ms,0��ʾ��������
	#define FUNC_MODBUS_MASTER_SEND_TOUT 1000	//���ͳ�ʱ100mS
	#define FUNC_MODBUS_MASTER_SEND_RTY 1		//�ط�1��
	#define FUNC_MODBUS_MASTER_CYC_NB 64			//ѭ��������г���
	#define FUNC_MODBUS_MASTER_INJ_NB 32			//ע��������г���
#define __FUNC_MODBUS_MASTER_DEFINE
//-----User_Code_define End-----//
//=============================================================================
//ʹ����ģ����Ҫ���������ݽ��ж���	
#ifndef __FUNC_MODBUS_MASTER_DEFINE
	#ifdef FUNC_MODBUS_MASTER_MAIN
		//�ӻ�HR���ձ�һ����ӵ�ʱ����ÿ���ӻ���Ӧ��HR
		const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[]={
			{10,100,HR},	//�ӻ���ַ��HR������HRָ��
			{0,0,0}
		};
		const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[]={
			{10,100,HR},	//�ӻ���ַ��HR������HRָ��
			{0,0,0}
		};
	#else
		extern const FUNC_MODBUS_MASTER_SLV_HR_T master0_tb_slv_hr[];
		extern const FUNC_MODBUS_MASTER_SLV_HR_T master1_tb_slv_hr[];
	#endif
	#define FUNC_MODBUS_MASTER_CHKBUS_DELAY 10	//�ж����߿�����ʱ10mS-20mS
	#define FUNC_MODBUS_MASTER_SEND_DELAY 10	//����ǰ��ʱ10mS
	#define FUNC_MODBUS_MASTER_SEND_PERIOD 100	//��������100ms,0��ʾ��������
	#define FUNC_MODBUS_MASTER_SEND_TOUT 1000	//���ͳ�ʱ100mS
	#define FUNC_MODBUS_MASTER_SEND_RTY 1		//�ط�1��
	#define FUNC_MODBUS_MASTER_CYC_NB 4			//ѭ��������г���
	#define FUNC_MODBUS_MASTER_INJ_NB 2			//ע��������г���
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
		unsigned char send_delay;		//����ǰ��ʱ
		unsigned char send_rty_nb;		//�ط�����
		unsigned char send_rty_cnt;		//�ط�����
		unsigned char func_cyc_nb;		//ѭ��ָ������
		unsigned char func_cyc_cur;		//��ǰѭ��ָ��
		unsigned char func_inj_h;		//ע��ָ��ͷ
		unsigned char func_inj_t;		//ע��ָ��β
		unsigned short send_tout;		//���ͳ�ʱ
		unsigned short send_period;		//ͨѶ����
		unsigned short err_cnt;			//�������
		unsigned char cur_func_mode;	//��ǰ��������ѭ��ָ���ע��ָ��
		USART_T *pu;					//����ָ��
		time_ms_T tm;					//��ʱ��
		time_ms_T tm_period;					//��ʱ��
		const FUNC_MODBUS_MASTER_SLV_HR_T *tb_slv_hr;	//�ӻ�HR��Ӧ��
		MODBUS_T md;					//MODBUS�ṹ��
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
