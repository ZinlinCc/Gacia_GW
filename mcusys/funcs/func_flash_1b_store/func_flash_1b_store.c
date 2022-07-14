/************************************Copyright (c)**************************************
                                          
                               温州大学智能电子电器服务中心

--------------文件信息------------------------------------------------------------------
文 件 名: flash_1b_store.c
创 建 人: ChenChong
email:	  ccmcu@qq.com
创建日期: 2019年04月15日
描    述: 利用Flash作为存贮器
---------- 版本信息---------------------------------------------------------------------
 版    本: V1.0
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/
#define FUNC_FLASH_1B_STORE_MAIN
#include "..\func_flash_iap\func_flash_iap.h"
#include "func_flash_1b_store.h"
//----------------------------------------------------------------------
#define FLASH_1B_STORE_INIT_SUCCESS 1	//初始化成功，并读取数据
#define FLASH_1B_STORE_INIT_NODATA 0		//初始化成功，没有数据
#define FLASH_1B_STORE_INIT_SIZE_ERR -1	//存储器大小错误，存储器尺寸必须比数据块大且是页尺寸的整数倍
#define FLASH_1B_STORE_INIT_STARTADR_ERR -2	//起始地址错误，起始地址必须是页的起点
#define FLASH_1B_STORE_INIT_BLOCK_SIZE_ERR -3	//数据块尺寸错误，因为flash存储是以short为单元，数据块必须是2对齐

#define FLASH_1B_STORE_CHKSUM_ERR 0xffff

FLASH_1B_STORE_T g_flash_1b_store;


int flash_1b_store_chksum(FLASH_1B_STORE_T *p,unsigned int adr);


//计算下一个数据块的地址，如果数据块跨越页边界，则跳至下一页的起始
unsigned int flash_1b_store_calc_next_adr(FLASH_1B_STORE_T *p,unsigned int adr);

unsigned int flash_1b_store_wr(FLASH_1B_STORE_T *p);
//----------------------------------------------------------------------

//对flash存储的结构体进行初始化

int func_flash_1b_store_init(FLASH_1B_STORE_T *p,unsigned int flash_store_adr,unsigned int size,void *p_data,const void *p_data_dft,unsigned short data_size,unsigned short lock_time_s,unsigned char b_bak,unsigned char key_en)
{
	unsigned short page_size;
	unsigned int adr;
	unsigned short sn,us;

	//--------------------------------------------------------
	//读芯片ID识别页大小
	//page_size=*((unsigned short*)FLASH_SIZE_ADR);
	page_size = __func_flash_iap_get_pagesize();
	
	p->page_size=page_size;				//每页字节数
	//--------------------------------------------------------
	if(size < page_size)
	{
		return(FLASH_1B_STORE_INIT_SIZE_ERR);	//存储容量小于一页，错误
	}
	p->size=size;
	
	adr=flash_store_adr % page_size;
	if(adr!=0)
	{
		return(FLASH_1B_STORE_INIT_STARTADR_ERR);	//起始地址错误
	}
	
	if(data_size & 0x01)
	{
		return(FLASH_1B_STORE_INIT_BLOCK_SIZE_ERR);	//数据块长度不为偶数错误
	}
	
	p->start_adr=flash_store_adr;		//存储初始地址
	p->cur_adr=flash_store_adr;
	p->p_data=p_data;
	p->data_size=data_size;
	p->lock_time_s=lock_time_s;
	p->block_sn=FLASH_1B_STORE_CHKSUM_ERR;
	p->key_en=key_en;
	
	
	if(b_bak)
	{
		p->p_data_bak=DataPool_Get(data_size);
	}
	else
	{
		p->p_data_bak=NULL;
	}
	
	adr=flash_store_adr;
	//----------------------------------------------------------------------------
	while(1)
	{
		/*
		sn=(flash_1b_store_chksum(p,adr));	//如果有合法的数据返回小于0xffff的序列号
		us=(sn - p->block_sn);
		
		if((short)us >0)
		{							//有合法的, 并且序列号靠后（值不一定大，0000比0xfffd大）
			p->block_sn=sn;			//刷新序列号与缓存
			p->cur_adr=adr;			//刷新当前地址
			memcpy(p->p_block,(unsigned char *)adr,p->block_size);	//载入变量
		}
		*/
		sn=*(unsigned short *)(adr+FLASH_1B_STORE_SN_OFS);
		us=(sn - p->block_sn);
		if((short)us >0)
		{							//有合法的, 并且序列号靠后（值不一定大，0000比0xfffd大）
			sn=(flash_1b_store_chksum(p,adr));	//如果有合法的数据返回小于0xffff的序列号
			if(sn != FLASH_1B_STORE_CHKSUM_ERR)
			{
				p->block_sn=sn;			//刷新序列号与缓存
				p->cur_adr=adr;			//刷新当前地址
				memcpy(p->p_data,(unsigned char *)adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size);	//载入变量
				if(p->p_data_bak !=NULL)
				{
					memcpy(p->p_data_bak,p->p_data,p->data_size);
				}
			}
		}
		
		adr=flash_1b_store_calc_next_adr(p,adr);		//地址调成下一个数据块地址
		if(adr==p->start_adr)
		{
			break;					//已经遍历整个存储区
		}
	}
	
	if(p->block_sn == FLASH_1B_STORE_CHKSUM_ERR)
	{	//没有找到正确的数据块，默认数据
		memcpy(p->p_data,p_data_dft,p->data_size);
		p->key=FLASH_1B_STORE_KEY_CODE;
		return(FLASH_1B_STORE_INIT_NODATA);
	}
	
	return(FLASH_1B_STORE_INIT_SUCCESS);
}
//--------------------------------------------------------------------------------
//计算下一个数据块的地址
unsigned int flash_1b_store_calc_next_adr(FLASH_1B_STORE_T *p,unsigned int adr)
{
	unsigned int j;
	unsigned short block_size;

	block_size=p->data_size+sizeof(FLASH_1B_STORE_HEAD_T);
	
	adr+=block_size;
			
	j= p->page_size - (adr%p->page_size);	//j=本页剩余的字节数
	
	if(j < block_size)
	{									//本页剩余字节数比数据块长度小，切到下一页开始
		j=adr-p->start_adr;
		j/=p->page_size;
		adr=p->start_adr+(j+1)*p->page_size;	//切到下一页开始
	}
	
	if(adr-p->start_adr >= p->size)
	{
		adr=p->start_adr;	//超过整个存储区范围，回到起始
	}
	return(adr);
}
//---------------------------------------------------------------------------------
//检测存储区块的校验和合法性
int flash_1b_store_chksum(FLASH_1B_STORE_T *p,unsigned int adr)
{
	unsigned short chksum;
	//计算SN与数据块的校验
	chksum=calc_short_sum((unsigned char *)(adr + 2),p->data_size+2,FLASH_STOER_CHKSUM_ORG);
	if(chksum != *(unsigned short*)adr)
	{
		return(FLASH_1B_STORE_CHKSUM_ERR);		//校验错误
	}
	
	return(*(unsigned short*)(adr+2));			//校验正确，返回序列号
}
//---------------------------------------------------------------------------------
//把数据写入flash
unsigned int flash_1b_store_wr(FLASH_1B_STORE_T *p)
{
	unsigned int adr,j,r=1;
	unsigned short chksum,sn;
	
	adr=flash_1b_store_calc_next_adr(p,p->cur_adr);	//获得下一个可以写的地址
	
	p->cur_adr=adr;									//设置当前地址
	
	sn=p->block_sn+1;
	if(sn == FLASH_1B_STORE_CHKSUM_ERR)
	{
		sn=0;
	}
	p->block_sn=sn;
	p->head.sn=sn;
	chksum=calc_short_sum((unsigned char *)(&sn),sizeof(sn),FLASH_STOER_CHKSUM_ORG);	//计算校验和
	chksum=calc_short_sum((unsigned char *)((unsigned int)p->p_data),p->data_size,chksum);	//计算校验和
	p->head.chksum=chksum;													//在缓存里加上校验和

//	在进入前拷贝过了
//	if(p->p_data_bak)
//	{
//		memcpy(p->p_data_bak,p->p_data,sizeof(*p->p_data));							//同时更新备份区的数据
//	}
	
	/*
	if(!(IAP_Flash_Write(adr,p->block_size,p->p_block,IAP_FLASH_ERASEPAGE)))
	{																						//写入失败，切换到下一页再次写入
		j=adr-p->start_adr;
		j/=p->page_size;
		adr=p->start_adr+(j+1)*p->page_size;	//切到下一页开始
		if(adr-p->start_adr >= p->size)			//超出了整个空间地址
		{
			adr=p->start_adr;	//超过整个存储区范围，回到起始
		}
		p->cur_adr=adr;									//设置当前地址
		r=IAP_Flash_Write(adr,p->block_size,p->p_block,IAP_FLASH_ERASEPAGE);				//在页边界再次写入
	}
	*/
	r=func_flash_iap_hword_write(adr,sizeof(FLASH_1B_STORE_HEAD_T),(unsigned char *)&p->head,IAP_FLASH_ERASEPAGE);
	r&=func_flash_iap_hword_write(adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size,(unsigned char *)p->p_data,IAP_FLASH_ERASEPAGE);
	if(!r)
	{																						//写入失败，切换到下一页再次写入
		j=adr-p->start_adr;
		j/=p->page_size;
		adr=p->start_adr+(j+1)*p->page_size;	//切到下一页开始
		if(adr-p->start_adr >= p->size)			//超出了整个空间地址
		{
			adr=p->start_adr;	//超过整个存储区范围，回到起始
		}
		p->cur_adr=adr;									//设置当前地址
		r=func_flash_iap_hword_write(adr,sizeof(FLASH_1B_STORE_HEAD_T),(unsigned char *)&p->head,IAP_FLASH_ERASEPAGE);
		r&=func_flash_iap_hword_write(adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size,(unsigned char *)p->p_data,IAP_FLASH_ERASEPAGE);
	}
	return(r);
}

unsigned int func_flash_1b_store(FLASH_1B_STORE_T *p)
{
	unsigned int r=0,i=0;
	
	if(left_ms(&p->tm)==0)
	{
		if(p->p_data_bak != NULL)
		{							//有备份比较区
			if(memcmp(p->p_data,p->p_data_bak,p->data_size)!=0)
			{															//比较不相同					
				memcpy(p->p_data_bak,p->p_data,p->data_size);		//拷贝至备份区，用于下次比较
				i=1;													//暂时不写入，重新设延时
			}
		}
		
		if(i==0 && memcmp(p->p_data,(unsigned char*)p->cur_adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size)!=0)	//比较数据区与flash的内容，如果不同，写入
		{
			if((!p->key_en) || (p->key==FLASH_1B_STORE_KEY_CODE))
			{
				flash_1b_store_wr(p);
				p->key=0;
				r=1;
			}
			else
			{
				memcpy(p->p_data,(unsigned char*)p->cur_adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size);
			}
		}
		left_ms_set(&p->tm,p->lock_time_s*1000);
	}
	return(r);
}


//==============================================================================================================================================
