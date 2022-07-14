/************************************Copyright (c)**************************************
                                          
                               温州大学智能电子电器服务中心

--------------文件信息------------------------------------------------------------------
文 件 名: iap.c
创 建 人: ChenChong
email:	  ccmcu@qq.com
创建日期: 2019年04月15日
描    述: 在线编程
---------- 版本信息---------------------------------------------------------------------
 版    本: V1.0
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/   
#include "func_flash_iap.h"


/***************************************************************************************
** 函数名称: IAP_FlashWrite
** 功能描述: IAP扇区烧写函数
** 参    数: w_data		/ 写入数据指针
** 参    数: w_length	/ 写入数据长度
** 参    数: w_addr		/ 写入数据地址
** 返 回 值: 写入结果(TRUE / FALSE)
** 备    注: 扇区方式写入,要求地址和长度都以1024字节对齐       
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
	
	sta_addr = adr;			// 1K字节对齐
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
			__nop();			// 检测忙位
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
