#ifndef __func_flash_iap_H__
#define __func_flash_iap_H__
/**********************************************************/
//版本号管理

/***************************************************************************************
** 函数名称: IAP_FlashWrite
** 功能描述: IAP扇区烧写函数
** 参    数: w_data		/ 写入数据指针
** 参    数: w_length	/ 写入数据长度
** 参    数: w_addr		/ 写入数据地址
** 返 回 值: 写入结果(TRUE / FALSE)
** 备    注: 扇区写入方式,要求地址和长度都以1024字节对齐       
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
