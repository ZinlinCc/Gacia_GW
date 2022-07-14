/************************************Copyright (c)**************************************
                                          
                               ���ݴ�ѧ���ܵ��ӵ�����������

--------------�ļ���Ϣ------------------------------------------------------------------
�� �� ��: iap.c
�� �� ��: ChenChong
email:	  ccmcu@qq.com
��������: 2019��04��15��
��    ��: ���߱��
---------- �汾��Ϣ---------------------------------------------------------------------
 ��    ��: V1.0
 ˵    ��: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/   
#include "func_flash_iap.h"


/***************************************************************************************
** ��������: IAP_FlashWrite
** ��������: IAP������д����
** ��    ��: w_data		/ д������ָ��
** ��    ��: w_length	/ д�����ݳ���
** ��    ��: w_addr		/ д�����ݵ�ַ
** �� �� ֵ: д����(TRUE / FALSE)
** ��    ע: ������ʽд��,Ҫ���ַ�ͳ��ȶ���1024�ֽڶ���       
****************************************************************************************/
unsigned int func_flash_iap_hword_write(unsigned int adr, unsigned int n ,unsigned char* p,unsigned char b_page_erase)
{
	unsigned int sta_addr, end_addr;
	unsigned int page_mask;

	unsigned short i;
	unsigned int end_adr_max;
	unsigned int flash_base_adr;

	flash_base_adr = __func_flash_iap_get_baseadr();
	i=__func_flash_iap_get_size();
	end_adr_max=i*1024 + flash_base_adr;
	page_mask=__func_flash_iap_get_pagesize()-1;
	
	sta_addr = adr;			// 1K�ֽڶ���
	end_addr = sta_addr + n;			
				  
	if((sta_addr < flash_base_adr) || (end_addr >= end_adr_max) || (sta_addr & 0x01) || (n & 0x01))
	{
		return IAP_FLASH_FAIL;
	}
	
	__func_flash_iap_unlock();
	
	while(sta_addr < end_addr)
	{										   
		while(__func_flash_iap_chk_busy())
		{
			__nop();			// ���æλ
		}
		
		
		if(b_page_erase && (sta_addr & page_mask) ==0)
		{
			__func_flash_iap_erase_page(sta_addr);
		}
	 	
		if(*(unsigned short*)sta_addr != *(unsigned short*)p)
		{
			if(!__func_flash_iap_write_hword(sta_addr,*(unsigned short*)p))
			{
				return IAP_FLASH_FAIL;			
			}
		}
		sta_addr +=2;
		p+=2;
	}

	__func_flash_iap_lock();
	
	return (IAP_FLASH_SUCCESS); 	
}	

/***************************************************************************************/


/****************************************************************************************
**  End Of File                                                     
****************************************************************************************/
