#ifndef __func_flash_iap_H__
#define __func_flash_iap_H__
/**********************************************************/
//�汾�Ź���

/***************************************************************************************
** ��������: IAP_FlashWrite
** ��������: IAP������д����
** ��    ��: w_data		/ д������ָ��
** ��    ��: w_length	/ д�����ݳ���
** ��    ��: w_addr		/ д�����ݵ�ַ
** �� �� ֵ: д����(TRUE / FALSE)
** ��    ע: ����д�뷽ʽ,Ҫ���ַ�ͳ��ȶ���1024�ֽڶ���       
****************************************************************************************/
#define IAP_FLASH_ERASEPAGE 1
#define IAP_FLASH_NOTERASEPAGE 0

#define IAP_FLASH_SUCCESS 1
#define IAP_FLASH_FAIL 0


unsigned int func_flash_iap_hword_write(unsigned int adr, unsigned int n ,unsigned char* p,unsigned char b_page_erase);

unsigned int __func_flash_iap_get_baseadr(void);
unsigned int __func_flash_iap_get_size(void);
unsigned int __func_flash_iap_get_pagesize(void);
void __func_flash_iap_erase_page(unsigned int);
void __func_flash_iap_unlock(void);
unsigned char __func_flash_iap_chk_busy(void);
unsigned char __func_flash_iap_write_hword(unsigned int adr,unsigned short us);
void __func_flash_iap_lock(void);
void __func_flash_iap_erase_page(unsigned int adr);
void __func_flash_erase_page(unsigned int adr);


#endif /* __IAP_H */					  
/****************************************************************************************
**  End Of File                                                     
****************************************************************************************/
