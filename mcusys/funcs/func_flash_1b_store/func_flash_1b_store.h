#ifndef __FLASH_1B_STORE_H__
#define __FLASH_1B_STORE_H__
//--------------------------------------------------------
//版本号管理
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
	#define FLASH_STORE_SIZE	(2048)			//分配2页作为存储，可以多页
	#define FLASH_STORE_START_ADR (0x8000000+256*1024 - FLASH_STORE_SIZE*2)	//256K芯片，倒数3-4页
	#define FLASH_LOCK_TM 3							//数据变化以后，3秒内不再变化，执行一次保存
	#define FLASH_STORE_RAMBAK 0
	#define FLASH_STORE_KEY_EN 1					//数据写入放错标志，建议这个设1，如果为了兼容老程序，设0
#endif

#if defined (MCU_AT32F421x)	
	#define FLASH_STORE_SIZE	(1024)			//分配2页作为存储，可以多页
	#define FLASH_STORE_START_ADR (0x8000000+64*1024 - FLASH_STORE_SIZE*2)	//256K芯片，倒数3-4页
	#define FLASH_LOCK_TM 3							//数据变化以后，3秒内不再变化，执行一次保存
	#define FLASH_STORE_RAMBAK 0
	#define FLASH_STORE_KEY_EN 1					//数据写入放错标志，建议这个设1，如果为了兼容老程序，设0
#endif
	
#ifdef MCU_CX32L003
	#define FLASH_STORE_SIZE	(512)			//分配2页作为存储，可以多页
	#define FLASH_STORE_START_ADR (0x8000000+256*1024 - FLASH_STORE_SIZE*2)	//256K芯片，倒数3-4页
	#define FLASH_LOCK_TM 3							//数据变化以后，3秒内不再变化，执行一次保存
	#define FLASH_STORE_RAMBAK 0
	#define FLASH_STORE_KEY_EN 1					//数据写入放错标志，建议这个设1，如果为了兼容老程序，设0
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
存储模式：Flash分n页存储，每页内存储若干个区块，当一页存储满的时候，存储到下一页，并删除前一页的内容
每个区块以BLOCK_START_ID(short)开头，接着是校验码(short),再下面是数据
每次上电先初始化并找出最后一组有效数据的index，载入数据
若没有数据则载入默认数据并删除全部页
数据块结构
起始码ID，CHKSUM，数据体

使用范例：
	//------------------------------------------------------------------------------------------------
	enum{
		CHZ_FDA_STA_ID,			//定义变量ID
		CHZ_FDA_STA_ID_NB
	};
	
	typedef struct{					//待存储的结构体定义，注意必须word对齐
		unsigned char s_lock;
		unsigned char sw;
	}CHZ_FDA_STA_T;
	
	
	PR_CHZ_EXT CHZ_FDA_STA_T g_chz_fda_sta;		//定义变量实体
	#define  0					
	
	PR_CHZ_EXT FLASH_1B_STORE_DATA_INDEX_T g_chz_flash_data_index[CHZ_FDA_STA_ID_NB];		//定义索引
	PR_CHZ_EXT FLASH_1B_STORE_T g_chz_flash_1b_store;							//定义存储主结构体
	
	
	#define CHZ_FLASH_1B_STORE_ADR 0x801C000								//定义在flash里面的起始地址
	#define CHZ_FLASH_1B_STORE_SIZE 0x3000									//定义存储flash的大小

	//------------------------------------------------------------------------------------------------
	
	m_flash_data_index_reg(g_chz_flash_data_index[CHZ_FDA_STA_ID],g_chz_fda_sta);	//把变量注册到索引

	//flash存储初始化
	if(m_flash_1b_store_init(&g_chz_flash_1b_store,CHZ_FLASH_1B_STORE_ADR,CHZ_FLASH_1B_STORE_SIZE,g_chz_flash_data_index) != FLASH_1B_STORE_INIT_SUCCESS)
	{																		//读取不成功，写入默认数据
		flash_1b_store_wr(&g_chz_flash_1b_store,CHZ_FDA_STA_ID);
	}
	else
	{
		g_chz_state.s_lock=g_chz_fda_sta.s_lock;							//读取成功的信息处理
		g_chz_state.sw=g_chz_fda_sta.sw;
	}

*/


#define FLASH_1B_STORE_NOBAK	0
#define FLASH_1B_STORE_USEBAK	1


#define FLASH_STOER_CHKSUM_ORG 0x435A		//校验码起始数据


//变量索引初始化宏

typedef struct{
	unsigned short chksum;
	unsigned short sn;
}FLASH_1B_STORE_HEAD_T;

#define FLASH_1B_STORE_CHKSUM_OFS 0
#define FLASH_1B_STORE_SN_OFS 2
#define FLASH_1B_STORE_KEY_CODE 0x2589
//-----------------------------------------------------------------


typedef struct {
	unsigned int start_adr;			//flash存储起始地址
	unsigned int size;				//用于flash存储总容量
	unsigned int cur_adr;			//当前地址
	unsigned short key_en;
	unsigned short page_size;		//每页长度
	unsigned short data_size;		//数据块长度
	unsigned short block_sn;		//数据块序列号
	unsigned short lock_time_s;		//锁定时间
	unsigned short key;
	//-------------------------
	FLASH_1B_STORE_HEAD_T head;		//数据块头部
	void *p_data;					//数据块变量的指针
	void *p_data_bak;				//备份数据块指针，用于比较是否变化
	time_ms_T tm;					//定时器，避免频繁写入
}FLASH_1B_STORE_T;


FUNC_FLASH_1B_STORE_EXT FLASH_1B_STORE_T g_func_flash_1b_store;

int func_flash_1b_store_init(FLASH_1B_STORE_T *p,unsigned int flash_store_adr,unsigned int size,void *p_data,const void *p_data_dft,unsigned short data_size,unsigned short lock_time_s,unsigned char b_bak,unsigned char key_en);

unsigned int func_flash_1b_store(FLASH_1B_STORE_T *p);
//-----------------------------------------------------------------
//+++++User_Code_define1 Begin+++++//
//-----User_Code_define1 End-----//
#endif
