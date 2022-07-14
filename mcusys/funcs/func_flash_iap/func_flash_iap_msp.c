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
#include "includes.h"

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//

#ifndef __FUNC_FLASH_IAP

	#ifdef MCU_AT32F421x
		#define FLASH_SIZE_ADR 0x1ffff7e0
							  
		#define IAP_RDP_Key			((unsigned short)0x00A5)	 	
		#define IAP_FLASH_KEY1		((unsigned int)0x45670123)
		#define IAP_FLASH_KEY2		((unsigned int)0xCDEF89AB)

		//��ȡ��оƬ��Flash��С
		unsigned int __func_flash_iap_get_size(void)
		{
			return(*((unsigned short*)FLASH_SIZE_ADR));
		}

		//��ȡ��оƬ��������С
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
		
		//��ȡ��оƬ��FLASH��ʼ��ַ
		unsigned int __func_flash_iap_get_baseadr(void)
		{
			return(FLASH_BASE);	//��ʼ��ַ
		}


		void __func_flash_iap_unlock(void)
		{
			__disable_irq();	//���ж�
			FLASH_Unlock();
			__enable_irq();		//���ж�
		}

		void __func_flash_iap_lock(void)
		{
			;
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return(0);
		}


		//��flashдһ������
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
		#define IAP_PAGE_ZISE		0x800				// ������С

		//��ȡ��оƬ��Flash��С
		unsigned int __func_flash_iap_get_size(void)
		{
			return(*((unsigned short*)FLASH_SIZE_ADR));
		}

		//��ȡ��оƬ��������С
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
		
		//��ȡ��оƬ��FLASH��ʼ��ַ
		unsigned int __func_flash_iap_get_baseadr(void)
		{
			return(FLASH_BASE);	//��ʼ��ַ
		}


		void __func_flash_iap_unlock(void)
		{
			__disable_irq();	//���ж�
			FLASH_Unlock();
			__enable_irq();		//���ж�
		}

		void __func_flash_iap_lock(void)
		{
			;
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return(0);
		}


		//��flashдһ������
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
		//V101����ҳ����
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
		#define IAP_PAGE_ZISE		0x800				// ������С

		#define IAP_SR_BSY			0x0001				// SR �Ŀ���λ
		#define IAP_SR_EOP			0x0020				// EOP �������

		#define IAP_CR_PG			0x0001				// ���  (1=ѡ���̲���)
		#define IAP_CR_PER			0x0002				// ҳ����(1=ѡ��ҳ��������)	
		#define IAP_CR_MER			0x0008				// ȫ����(ѡ����������û�ҳ)
		#define IAP_CR_OPTPG		0x0010				// ��дѡ���ֽ�(��ѡ���ֽڱ��)
		#define IAP_CR_OPTER		0x0020				// ����ѡ���ֽ�(����ѡ���ֽ�/С��Ϣ��)
		#define IAP_CR_STRT			0x0040				// ��ʼ����
		#define IAP_CR_LOCK			0x0080				// ����(ֻ��д��1��)
		#define IAP_CR_OPTWRE		0x0200				// ����дѡ���ֽ�
		#define IAP_CR_ERRIE		0x0400				// �������״̬�ж�
		#define IAP_CR_EOPIE		0x1000				// �����������ж�	
																			
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
		#define CR_OPTWRE_Set		((unsigned int)0x00000200)	// ����дѡ���ֽ�
		#define CR_OPTWRE_Reset		((unsigned int)0x00001DFF)	// ����дѡ���ֽ�

		#define IAP_RDP				0x1FFFF800			// ��д������RDP


	//	void __func_iap_erase_page(unsigned int adr)
	//	{
	//		FLASH->CR &= CR_PG_Reset;					//�¼���
	//		FLASH->CR |= CR_PER_Set;						  		   
	//		
	//		FLASH->AR  = adr;							// ѡ��Ҫ������ҳ
	//		FLASH->CR |= CR_STRT_Set;
	//		while(FLASH->CR & CR_STRT_Set){;}			//�ȴ�STRTλ���0���¼��룩
	//		while(FLASH->SR & IAP_SR_BSY){;}			// ���æλ	
	//		FLASH->CR &= CR_PER_Reset;		  
	//		while(FLASH->SR & IAP_SR_BSY){;}			// ���æλ	

	//		//�����в��		
	//	}

	//	void __func_flash_erase_page(unsigned int adr)
	//	{
	//		__asm("cpsid i");
	//		FLASH->KEYR = IAP_FLASH_KEY1;				// ����FLASH��д����������
	//		FLASH->KEYR = IAP_FLASH_KEY2;
	//		__asm("cpsie i");
	//		
	//		__func_iap_erase_page(adr);

	//		//�����в��
	//			
	//		FLASH->CR &= CR_PG_Reset;					// ����FLASH��д����������
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
			return(0x8000000);	//��ʼ��ַ
		}


		void __func_flash_iap_unlock(void)
		{
			__disable_irq();	//���ж�
			
			FLASH->KEYR = IAP_FLASH_KEY1;				// ����FLASH��д����������
			FLASH->KEYR = IAP_FLASH_KEY2;

			__enable_irq();		//���ж�
		}

		void __func_flash_iap_lock(void)
		{
			FLASH->CR &= CR_PG_Reset;
			FLASH->CR |= CR_LOCK_Set;					// ����FLASH��д����������
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return((FLASH->SR & IAP_SR_BSY) != 0);
		}


		//��flashдһ������
		unsigned char __func_flash_iap_write_hword(unsigned int adr,unsigned short us)
		{
			FLASH->CR |= CR_PG_Set;
			*(volatile unsigned short*)adr = us;		// ���ַд������			   
			
			while(FLASH->SR & IAP_SR_BSY)
			{
				;			// ���æλ
			}

			if(*(unsigned short*)adr != us)	// ����д�����
			{	
				__func_flash_iap_lock();			
				return IAP_FLASH_FAIL;			
			}
			return(IAP_FLASH_SUCCESS);
		}




		/***************************************************************************************/
		//V101����ҳ����
		void __func_flash_iap_erase_page(unsigned int adr)
		{
			__asm("cpsid i");
			FLASH->KEYR = IAP_FLASH_KEY1;				// ����FLASH��д����������
			FLASH->KEYR = IAP_FLASH_KEY2;
			__asm("cpsie i");
			
			FLASH->CR &= CR_PG_Reset;					//�¼���
			FLASH->CR |= CR_PER_Set;						  		   
			
			FLASH->AR  = adr;							// ѡ��Ҫ������ҳ
			FLASH->CR |= CR_STRT_Set;
			while(FLASH->CR & CR_STRT_Set){;}			//�ȴ�STRTλ���0���¼��룩
			while(FLASH->SR & IAP_SR_BSY){;}			// ���æλ	
			FLASH->CR &= CR_PER_Reset;		  
			while(FLASH->SR & IAP_SR_BSY){;}			// ���æλ	

			//�����в��
				
			FLASH->CR &= CR_PG_Reset;					// ����FLASH��д����������
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
			return(512);	//ÿҳ512�ֽ�
		}

		unsigned int __func_flash_iap_get_baseadr(void)
		{
			return(0x00000000);	//��ʼ��ַ
		}


		void __func_flash_iap_unlock(void)
		{
		//	__disable_irq();	//���ж�
		//	
		//	FLASH->KEYR = IAP_FLASH_KEY1;				// ����FLASH��д����������
		//	FLASH->KEYR = IAP_FLASH_KEY2;

		//	__enable_irq();		//���ж�
		}

		void __func_flash_iap_lock(void)
		{
		//	FLASH->CR &= CR_PG_Reset;
		//	FLASH->CR |= CR_LOCK_Set;					// ����FLASH��д����������
		}

		unsigned char __func_flash_iap_chk_busy(void)
		{
			return(0);
		}


		//��flashдһ������
		unsigned char __func_flash_iap_write_hword(unsigned int adr,unsigned short us)
		{
			HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, adr, us);
			return(IAP_FLASH_SUCCESS);
		}




		/***************************************************************************************/
		//V101����ҳ����
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
