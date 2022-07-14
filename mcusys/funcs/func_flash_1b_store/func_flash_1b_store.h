#ifndef __FLASH_1B_STORE_H__
#define __FLASH_1B_STORE_H__
//--------------------------------------------------------
//�汾�Ź���
#define FLASH_1B_STORE_VER_H 200
//--------------------------------------------------------
#include "..\..\sys\sys_base0\utility.h"
#include "..\..\sys\sys_base0\chksums.h"
#include "string.h"

#ifdef FUNC_FLASH_1B_STORE_MAIN
	#define FUNC_FLASH_1B_STORE_EXT
#else
	#define FUNC_FLASH_1B_STORE_EXT extern
#endif

	
//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//
#ifndef __FUNC_FLASH_STORE_DEFINE
	
#if defined (MCU_AT32F413x) || defined (MCU_AT32F415x) || defined (MCU_AT32F403x)
	#define FLASH_STORE_SIZE	(2048)			//����2ҳ��Ϊ�洢�����Զ�ҳ
	#define FLASH_STORE_START_ADR (0x8000000+256*1024 - FLASH_STORE_SIZE*2)	//256KоƬ������3-4ҳ
	#define FLASH_LOCK_TM 3							//���ݱ仯�Ժ�3���ڲ��ٱ仯��ִ��һ�α���
	#define FLASH_STORE_RAMBAK 0
	#define FLASH_STORE_KEY_EN 1					//����д��Ŵ��־�����������1�����Ϊ�˼����ϳ�����0
#endif

#if defined (MCU_AT32F421x)	
	#define FLASH_STORE_SIZE	(1024)			//����2ҳ��Ϊ�洢�����Զ�ҳ
	#define FLASH_STORE_START_ADR (0x8000000+64*1024 - FLASH_STORE_SIZE*2)	//256KоƬ������3-4ҳ
	#define FLASH_LOCK_TM 3							//���ݱ仯�Ժ�3���ڲ��ٱ仯��ִ��һ�α���
	#define FLASH_STORE_RAMBAK 0
	#define FLASH_STORE_KEY_EN 1					//����д��Ŵ��־�����������1�����Ϊ�˼����ϳ�����0
#endif
	
#ifdef MCU_CX32L003
	#define FLASH_STORE_SIZE	(512)			//����2ҳ��Ϊ�洢�����Զ�ҳ
	#define FLASH_STORE_START_ADR (0x8000000+256*1024 - FLASH_STORE_SIZE*2)	//256KоƬ������3-4ҳ
	#define FLASH_LOCK_TM 3							//���ݱ仯�Ժ�3���ڲ��ٱ仯��ִ��һ�α���
	#define FLASH_STORE_RAMBAK 0
	#define FLASH_STORE_KEY_EN 1					//����д��Ŵ��־�����������1�����Ϊ�˼����ϳ�����0
#endif

typedef struct {
		unsigned int abc;
		unsigned char txt_disp_test;
		unsigned int password;
	}FSD_T;
	FUNC_FLASH_1B_STORE_EXT FSD_T g_fsd;
	
	#ifdef FUNC_FLASH_1B_STORE_MAIN
		const FSD_T g_fsd_dft={
			0,
			0,
			12345
		};
	#else
		extern const FSD_T g_fsd_dft;
	#endif
	
	
#endif



/*
�洢ģʽ��Flash��nҳ�洢��ÿҳ�ڴ洢���ɸ����飬��һҳ�洢����ʱ�򣬴洢����һҳ����ɾ��ǰһҳ������
ÿ��������BLOCK_START_ID(short)��ͷ��������У����(short),������������
ÿ���ϵ��ȳ�ʼ�����ҳ����һ����Ч���ݵ�index����������
��û������������Ĭ�����ݲ�ɾ��ȫ��ҳ
���ݿ�ṹ
��ʼ��ID��CHKSUM��������

ʹ�÷�����
	//------------------------------------------------------------------------------------------------
	enum{
		CHZ_FDA_STA_ID,			//�������ID
		CHZ_FDA_STA_ID_NB
	};
	
	typedef struct{					//���洢�Ľṹ�嶨�壬ע�����word����
		unsigned char s_lock;
		unsigned char sw;
	}CHZ_FDA_STA_T;
	
	
	PR_CHZ_EXT CHZ_FDA_STA_T g_chz_fda_sta;		//�������ʵ��
	#define  0					
	
	PR_CHZ_EXT FLASH_1B_STORE_DATA_INDEX_T g_chz_flash_data_index[CHZ_FDA_STA_ID_NB];		//��������
	PR_CHZ_EXT FLASH_1B_STORE_T g_chz_flash_1b_store;							//����洢���ṹ��
	
	
	#define CHZ_FLASH_1B_STORE_ADR 0x801C000								//������flash�������ʼ��ַ
	#define CHZ_FLASH_1B_STORE_SIZE 0x3000									//����洢flash�Ĵ�С

	//------------------------------------------------------------------------------------------------
	
	m_flash_data_index_reg(g_chz_flash_data_index[CHZ_FDA_STA_ID],g_chz_fda_sta);	//�ѱ���ע�ᵽ����

	//flash�洢��ʼ��
	if(m_flash_1b_store_init(&g_chz_flash_1b_store,CHZ_FLASH_1B_STORE_ADR,CHZ_FLASH_1B_STORE_SIZE,g_chz_flash_data_index) != FLASH_1B_STORE_INIT_SUCCESS)
	{																		//��ȡ���ɹ���д��Ĭ������
		flash_1b_store_wr(&g_chz_flash_1b_store,CHZ_FDA_STA_ID);
	}
	else
	{
		g_chz_state.s_lock=g_chz_fda_sta.s_lock;							//��ȡ�ɹ�����Ϣ����
		g_chz_state.sw=g_chz_fda_sta.sw;
	}

*/


#define FLASH_1B_STORE_NOBAK	0
#define FLASH_1B_STORE_USEBAK	1


#define FLASH_STOER_CHKSUM_ORG 0x435A		//У������ʼ����


//����������ʼ����

typedef struct{
	unsigned short chksum;
	unsigned short sn;
}FLASH_1B_STORE_HEAD_T;

#define FLASH_1B_STORE_CHKSUM_OFS 0
#define FLASH_1B_STORE_SN_OFS 2
#define FLASH_1B_STORE_KEY_CODE 0x2589
//-----------------------------------------------------------------


typedef struct {
	unsigned int start_adr;			//flash�洢��ʼ��ַ
	unsigned int size;				//����flash�洢������
	unsigned int cur_adr;			//��ǰ��ַ
	unsigned short key_en;
	unsigned short page_size;		//ÿҳ����
	unsigned short data_size;		//���ݿ鳤��
	unsigned short block_sn;		//���ݿ����к�
	unsigned short lock_time_s;		//����ʱ��
	unsigned short key;
	//-------------------------
	FLASH_1B_STORE_HEAD_T head;		//���ݿ�ͷ��
	void *p_data;					//���ݿ������ָ��
	void *p_data_bak;				//�������ݿ�ָ�룬���ڱȽ��Ƿ�仯
	time_ms_T tm;					//��ʱ��������Ƶ��д��
}FLASH_1B_STORE_T;


FUNC_FLASH_1B_STORE_EXT FLASH_1B_STORE_T g_func_flash_1b_store;

int func_flash_1b_store_init(FLASH_1B_STORE_T *p,unsigned int flash_store_adr,unsigned int size,void *p_data,const void *p_data_dft,unsigned short data_size,unsigned short lock_time_s,unsigned char b_bak,unsigned char key_en);

unsigned int func_flash_1b_store(FLASH_1B_STORE_T *p);
//-----------------------------------------------------------------
//+++++User_Code_define1 Begin+++++//
//-----User_Code_define1 End-----//
#endif
