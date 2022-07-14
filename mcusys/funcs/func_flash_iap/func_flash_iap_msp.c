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
#include "includes.h"

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//

#ifndef __FUNC_FLASH_IAP

	#ifdef MCU_AT32F421x
		#define FLASH_SIZE_ADR 0x1ffff7e0
							  
		#define IAP_RDP_Key			((unsigned short)0x00A5)	 	
		#define IAP_FLASH_KEY1		((unsigned int)0x45670123)
		#define IAP_FLASH_KEY2		((unsigned int)0xCDEF89AB)

		//读取本芯片的Flash大小
		unsigned int __func_flash_iap_get_size(void)
		{
			return(*((unsigned short*)FLASH_SIZE_ADR));
		}

		//读取本芯片的扇区大小
		unsigned int __func_flash_iap_get_pagesize(void)
		{
			unsigned int i;
			
			i=__func_flash_iap_get_size();
			if(i>128)
			{
				i=0x800;
			}
			else
			{
				i=0x400;
			}
			return(i);
		}
		
		//读取本芯片的FLASH起始地址
		unsigned int __func_flash_iap_get_baseadr(void)
		{
			return(FLASH_BASE);	//起始地址
		}


		void __func_flash_iap_unlock(void)
		{
			__disable_irq();	//关中断
			FLASH_Unlock();
			__enable_irq();		//开中断
		}

		void __func_flash_iap_lock(void)
		{
			;
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return(0);
		}


		//向flash写一个半字
		unsigned char __func_flash_iap_write_hword(unsigned int adr,unsigned short us)
		{
			//return FLASH_ProgramHalfWord(adr,us);
			if(*(unsigned short *)adr == 0xffff)
			{
				return (FLASH_ProgramHalfWord(adr,us)==FLASH_PRC_DONE);
			}
			else
			{
				return(0);
			}
		}


		void __func_flash_iap_erase_page(unsigned int adr)
		{
			FLASH_ErasePage(adr);
		}
	#endif

	#if defined (MCU_AT32F413x) || defined (MCU_AT32F415x) || defined (MCU_AT32F403x)
		#define FLASH_SIZE_ADR 0x1ffff7e0
							  
		#define IAP_RDP_Key			((unsigned short)0x00A5)	 	
		#define IAP_FLASH_KEY1		((unsigned int)0x45670123)
		#define IAP_FLASH_KEY2		((unsigned int)0xCDEF89AB)
		#define IAP_PAGE_ZISE		0x800				// 扇区大小

		//读取本芯片的Flash大小
		unsigned int __func_flash_iap_get_size(void)
		{
			return(*((unsigned short*)FLASH_SIZE_ADR));
		}

		//读取本芯片的扇区大小
		unsigned int __func_flash_iap_get_pagesize(void)
		{
			unsigned int i;
			
			i=__func_flash_iap_get_size();
			if(i>128)
			{
				i=0x800;
			}
			else
			{
				i=0x400;
			}
			return(i);
		}
		
		//读取本芯片的FLASH起始地址
		unsigned int __func_flash_iap_get_baseadr(void)
		{
			return(FLASH_BASE);	//起始地址
		}


		void __func_flash_iap_unlock(void)
		{
			__disable_irq();	//关中断
			FLASH_Unlock();
			__enable_irq();		//开中断
		}

		void __func_flash_iap_lock(void)
		{
			;
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return(0);
		}


		//向flash写一个半字
		unsigned char __func_flash_iap_write_hword(unsigned int adr,unsigned short us)
		{
			if(*(unsigned short *)adr == 0xffff)
			{
				return (FLASH_ProgramHalfWord(adr,us)==FLASH_PRC_DONE);
			}
			else
			{
				return(0);
			}
		}




		/***************************************************************************************/
		//V101加入页擦除
		void __func_flash_iap_erase_page(unsigned int adr)
		{
			FLASH_ErasePage(adr);
		}
	#endif

	#ifdef MCU_STM32F10x
		#include "func_flash_iap.h"
		#include "stm32f103xe.h"


		#define FLASH_SIZE_ADR 0x1ffff7e0
							  
		#define IAP_RDP_Key			((unsigned short)0x00A5)	 	
		#define IAP_FLASH_KEY1		((unsigned int)0x45670123)
		#define IAP_FLASH_KEY2		((unsigned int)0xCDEF89AB)
		#define IAP_PAGE_ZISE		0x800				// 扇区大小

		#define IAP_SR_BSY			0x0001				// SR 的空闲位
		#define IAP_SR_EOP			0x0020				// EOP 操作完成

		#define IAP_CR_PG			0x0001				// 编程  (1=选择编程操作)
		#define IAP_CR_PER			0x0002				// 页擦除(1=选择页擦除操作)	
		#define IAP_CR_MER			0x0008				// 全擦除(选择擦除所有用户页)
		#define IAP_CR_OPTPG		0x0010				// 烧写选择字节(对选择字节编程)
		#define IAP_CR_OPTER		0x0020				// 擦除选择字节(擦除选择字节/小信息块)
		#define IAP_CR_STRT			0x0040				// 开始擦除
		#define IAP_CR_LOCK			0x0080				// 锁定(只能写’1’)
		#define IAP_CR_OPTWRE		0x0200				// 允许写选择字节
		#define IAP_CR_ERRIE		0x0400				// 允许错误状态中断
		#define IAP_CR_EOPIE		0x1000				// 允许操作完成中断	
																			
		#define CR_PG_Set           ((unsigned int)0x00000001)
		#define CR_PG_Reset         ((unsigned int)0x00001FFE)
		#define CR_PER_Set          ((unsigned int)0x00000002)
		#define CR_PER_Reset        ((unsigned int)0x00001FFD)
		#define CR_MER_Set          ((unsigned int)0x00000004)
		#define CR_MER_Reset        ((unsigned int)0x00001FFB)
		#define CR_OPTPG_Set        ((unsigned int)0x00000010)
		#define CR_OPTPG_Reset      ((unsigned int)0x00001FEF)
		#define CR_OPTER_Set        ((unsigned int)0x00000020)
		#define CR_OPTER_Reset      ((unsigned int)0x00001FDF)
		#define CR_STRT_Set         ((unsigned int)0x00000040)							 
		#define CR_LOCK_Set         ((unsigned int)0x00000080) 
		#define CR_OPTWRE_Set		((unsigned int)0x00000200)	// 允许写选择字节
		#define CR_OPTWRE_Reset		((unsigned int)0x00001DFF)	// 允许写选择字节

		#define IAP_RDP				0x1FFFF800			// 读写保护字RDP


	//	void __func_iap_erase_page(unsigned int adr)
	//	{
	//		FLASH->CR &= CR_PG_Reset;					//新加入
	//		FLASH->CR |= CR_PER_Set;						  		   
	//		
	//		FLASH->AR  = adr;							// 选择要擦除的页
	//		FLASH->CR |= CR_STRT_Set;
	//		while(FLASH->CR & CR_STRT_Set){;}			//等待STRT位变成0（新加入）
	//		while(FLASH->SR & IAP_SR_BSY){;}			// 检测忙位	
	//		FLASH->CR &= CR_PER_Reset;		  
	//		while(FLASH->SR & IAP_SR_BSY){;}			// 检测忙位	

	//		//不进行查空		
	//	}

	//	void __func_flash_erase_page(unsigned int adr)
	//	{
	//		__asm("cpsid i");
	//		FLASH->KEYR = IAP_FLASH_KEY1;				// 解锁FLASH编写擦除控制器
	//		FLASH->KEYR = IAP_FLASH_KEY2;
	//		__asm("cpsie i");
	//		
	//		__func_iap_erase_page(adr);

	//		//不进行查空
	//			
	//		FLASH->CR &= CR_PG_Reset;					// 锁定FLASH编写擦除控制器
	//		FLASH->CR |= CR_LOCK_Set;
	//		
	//	}

		unsigned int __func_flash_iap_get_size(void)
		{
			return(*((unsigned short*)FLASH_SIZE_ADR));
		}

		unsigned int __func_flash_iap_get_pagesize(void)
		{
			unsigned int i;
			
			i=__func_flash_iap_get_size();
			if(i>128)
			{
				//i=0x7ff;
				i=0x800;
			}
			else
			{
				//i=0x3ff;
				i=0x400;
			}
			return(i);
		}
		
		unsigned int __func_flash_iap_get_baseadr(void)
		{
			return(0x8000000);	//起始地址
		}


		void __func_flash_iap_unlock(void)
		{
			__disable_irq();	//关中断
			
			FLASH->KEYR = IAP_FLASH_KEY1;				// 解锁FLASH编写擦除控制器
			FLASH->KEYR = IAP_FLASH_KEY2;

			__enable_irq();		//开中断
		}

		void __func_flash_iap_lock(void)
		{
			FLASH->CR &= CR_PG_Reset;
			FLASH->CR |= CR_LOCK_Set;					// 锁定FLASH编写擦除控制器
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return((FLASH->SR & IAP_SR_BSY) != 0);
		}


		//想flash写一个半字
		unsigned char __func_flash_iap_write_hword(unsigned int adr,unsigned short us)
		{
			FLASH->CR |= CR_PG_Set;
			*(volatile unsigned short*)adr = us;		// 向地址写入数据			   
			
			while(FLASH->SR & IAP_SR_BSY)
			{
				;			// 检测忙位
			}

			if(*(unsigned short*)adr != us)	// 核验写入的字
			{	
				__func_flash_iap_lock();			
				return IAP_FLASH_FAIL;			
			}
			return(IAP_FLASH_SUCCESS);
		}




		/***************************************************************************************/
		//V101加入页擦除
		void __func_flash_iap_erase_page(unsigned int adr)
		{
			__asm("cpsid i");
			FLASH->KEYR = IAP_FLASH_KEY1;				// 解锁FLASH编写擦除控制器
			FLASH->KEYR = IAP_FLASH_KEY2;
			__asm("cpsie i");
			
			FLASH->CR &= CR_PG_Reset;					//新加入
			FLASH->CR |= CR_PER_Set;						  		   
			
			FLASH->AR  = adr;							// 选择要擦除的页
			FLASH->CR |= CR_STRT_Set;
			while(FLASH->CR & CR_STRT_Set){;}			//等待STRT位变成0（新加入）
			while(FLASH->SR & IAP_SR_BSY){;}			// 检测忙位	
			FLASH->CR &= CR_PER_Reset;		  
			while(FLASH->SR & IAP_SR_BSY){;}			// 检测忙位	

			//不进行查空
				
			FLASH->CR &= CR_PG_Reset;					// 锁定FLASH编写擦除控制器
			FLASH->CR |= CR_LOCK_Set;
		}
	#endif

	#ifdef MCU_CX32L003
		unsigned int __func_flash_iap_get_size(void)
		{
			return(64);		//64K
		}

		unsigned int __func_flash_iap_get_pagesize(void)
		{
			return(512);	//每页512字节
		}

		unsigned int __func_flash_iap_get_baseadr(void)
		{
			return(0x00000000);	//起始地址
		}


		void __func_flash_iap_unlock(void)
		{
		//	__disable_irq();	//关中断
		//	
		//	FLASH->KEYR = IAP_FLASH_KEY1;				// 解锁FLASH编写擦除控制器
		//	FLASH->KEYR = IAP_FLASH_KEY2;

		//	__enable_irq();		//开中断
		}

		void __func_flash_iap_lock(void)
		{
		//	FLASH->CR &= CR_PG_Reset;
		//	FLASH->CR |= CR_LOCK_Set;					// 锁定FLASH编写擦除控制器
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return(0);
		}


		//想flash写一个半字
		unsigned char __func_flash_iap_write_hword(unsigned int adr,unsigned short us)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, adr, us);
			return(IAP_FLASH_SUCCESS);
		}




		/***************************************************************************************/
		//V101加入页擦除
		void __func_flash_iap_erase_page(unsigned int adr)
		{
			FLASH_EraseInitTypeDef 	sFlashEraseInit;
			uint32_t uiErrorPage;

			
			sFlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			sFlashEraseInit.PageAddress = adr;	//FLASH_PROGRAM_ADDRESS_START;
			sFlashEraseInit.NbPages	= 1;		//(FLASH_PROGRAM_ADDRESS_END - FLASH_PROGRAM_ADDRESS_START)/FLASH_PAGE_SIZE + 1;
			if(HAL_FLASH_Erase(&sFlashEraseInit, &uiErrorPage) != HAL_OK)
			{
				;		
			}
		}
	#endif
#endif
/****************************************************************************************
**  End Of File                                                     
****************************************************************************************/
