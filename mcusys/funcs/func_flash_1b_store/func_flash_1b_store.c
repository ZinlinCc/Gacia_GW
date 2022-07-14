/************************************Copyright (c)**************************************
                                          
                               ���ݴ�ѧ���ܵ��ӵ�����������

--------------�ļ���Ϣ------------------------------------------------------------------
�� �� ��: flash_1b_store.c
�� �� ��: ChenChong
email:	  ccmcu@qq.com
��������: 2019��04��15��
��    ��: ����Flash��Ϊ������
---------- �汾��Ϣ---------------------------------------------------------------------
 ��    ��: V1.0
 ˵    ��: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/
#define FUNC_FLASH_1B_STORE_MAIN
#include "..\func_flash_iap\func_flash_iap.h"
#include "func_flash_1b_store.h"
//----------------------------------------------------------------------
#define FLASH_1B_STORE_INIT_SUCCESS 1	//��ʼ���ɹ�������ȡ����
#define FLASH_1B_STORE_INIT_NODATA 0		//��ʼ���ɹ���û������
#define FLASH_1B_STORE_INIT_SIZE_ERR -1	//�洢����С���󣬴洢���ߴ��������ݿ������ҳ�ߴ��������
#define FLASH_1B_STORE_INIT_STARTADR_ERR -2	//��ʼ��ַ������ʼ��ַ������ҳ�����
#define FLASH_1B_STORE_INIT_BLOCK_SIZE_ERR -3	//���ݿ�ߴ������Ϊflash�洢����shortΪ��Ԫ�����ݿ������2����

#define FLASH_1B_STORE_CHKSUM_ERR 0xffff

FLASH_1B_STORE_T g_flash_1b_store;


int flash_1b_store_chksum(FLASH_1B_STORE_T *p,unsigned int adr);


//������һ�����ݿ�ĵ�ַ��������ݿ��Խҳ�߽磬��������һҳ����ʼ
unsigned int flash_1b_store_calc_next_adr(FLASH_1B_STORE_T *p,unsigned int adr);

unsigned int flash_1b_store_wr(FLASH_1B_STORE_T *p);
//----------------------------------------------------------------------

//��flash�洢�Ľṹ����г�ʼ��

int func_flash_1b_store_init(FLASH_1B_STORE_T *p,unsigned int flash_store_adr,unsigned int size,void *p_data,const void *p_data_dft,unsigned short data_size,unsigned short lock_time_s,unsigned char b_bak,unsigned char key_en)
{
	unsigned short page_size;
	unsigned int adr;
	unsigned short sn,us;

	//--------------------------------------------------------
	//��оƬIDʶ��ҳ��С
	//page_size=*((unsigned short*)FLASH_SIZE_ADR);
	page_size = __func_flash_iap_get_pagesize();
	
	p->page_size=page_size;				//ÿҳ�ֽ���
	//--------------------------------------------------------
	if(size < page_size)
	{
		return(FLASH_1B_STORE_INIT_SIZE_ERR);	//�洢����С��һҳ������
	}
	p->size=size;
	
	adr=flash_store_adr % page_size;
	if(adr!=0)
	{
		return(FLASH_1B_STORE_INIT_STARTADR_ERR);	//��ʼ��ַ����
	}
	
	if(data_size & 0x01)
	{
		return(FLASH_1B_STORE_INIT_BLOCK_SIZE_ERR);	//���ݿ鳤�Ȳ�Ϊż������
	}
	
	p->start_adr=flash_store_adr;		//�洢��ʼ��ַ
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
		sn=(flash_1b_store_chksum(p,adr));	//����кϷ������ݷ���С��0xffff�����к�
		us=(sn - p->block_sn);
		
		if((short)us >0)
		{							//�кϷ���, �������кſ���ֵ��һ����0000��0xfffd��
			p->block_sn=sn;			//ˢ�����к��뻺��
			p->cur_adr=adr;			//ˢ�µ�ǰ��ַ
			memcpy(p->p_block,(unsigned char *)adr,p->block_size);	//�������
		}
		*/
		sn=*(unsigned short *)(adr+FLASH_1B_STORE_SN_OFS);
		us=(sn - p->block_sn);
		if((short)us >0)
		{							//�кϷ���, �������кſ���ֵ��һ����0000��0xfffd��
			sn=(flash_1b_store_chksum(p,adr));	//����кϷ������ݷ���С��0xffff�����к�
			if(sn != FLASH_1B_STORE_CHKSUM_ERR)
			{
				p->block_sn=sn;			//ˢ�����к��뻺��
				p->cur_adr=adr;			//ˢ�µ�ǰ��ַ
				memcpy(p->p_data,(unsigned char *)adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size);	//�������
				if(p->p_data_bak !=NULL)
				{
					memcpy(p->p_data_bak,p->p_data,p->data_size);
				}
			}
		}
		
		adr=flash_1b_store_calc_next_adr(p,adr);		//��ַ������һ�����ݿ��ַ
		if(adr==p->start_adr)
		{
			break;					//�Ѿ����������洢��
		}
	}
	
	if(p->block_sn == FLASH_1B_STORE_CHKSUM_ERR)
	{	//û���ҵ���ȷ�����ݿ飬Ĭ������
		memcpy(p->p_data,p_data_dft,p->data_size);
		p->key=FLASH_1B_STORE_KEY_CODE;
		return(FLASH_1B_STORE_INIT_NODATA);
	}
	
	return(FLASH_1B_STORE_INIT_SUCCESS);
}
//--------------------------------------------------------------------------------
//������һ�����ݿ�ĵ�ַ
unsigned int flash_1b_store_calc_next_adr(FLASH_1B_STORE_T *p,unsigned int adr)
{
	unsigned int j;
	unsigned short block_size;

	block_size=p->data_size+sizeof(FLASH_1B_STORE_HEAD_T);
	
	adr+=block_size;
			
	j= p->page_size - (adr%p->page_size);	//j=��ҳʣ����ֽ���
	
	if(j < block_size)
	{									//��ҳʣ���ֽ��������ݿ鳤��С���е���һҳ��ʼ
		j=adr-p->start_adr;
		j/=p->page_size;
		adr=p->start_adr+(j+1)*p->page_size;	//�е���һҳ��ʼ
	}
	
	if(adr-p->start_adr >= p->size)
	{
		adr=p->start_adr;	//���������洢����Χ���ص���ʼ
	}
	return(adr);
}
//---------------------------------------------------------------------------------
//���洢�����У��ͺϷ���
int flash_1b_store_chksum(FLASH_1B_STORE_T *p,unsigned int adr)
{
	unsigned short chksum;
	//����SN�����ݿ��У��
	chksum=calc_short_sum((unsigned char *)(adr + 2),p->data_size+2,FLASH_STOER_CHKSUM_ORG);
	if(chksum != *(unsigned short*)adr)
	{
		return(FLASH_1B_STORE_CHKSUM_ERR);		//У�����
	}
	
	return(*(unsigned short*)(adr+2));			//У����ȷ���������к�
}
//---------------------------------------------------------------------------------
//������д��flash
unsigned int flash_1b_store_wr(FLASH_1B_STORE_T *p)
{
	unsigned int adr,j,r=1;
	unsigned short chksum,sn;
	
	adr=flash_1b_store_calc_next_adr(p,p->cur_adr);	//�����һ������д�ĵ�ַ
	
	p->cur_adr=adr;									//���õ�ǰ��ַ
	
	sn=p->block_sn+1;
	if(sn == FLASH_1B_STORE_CHKSUM_ERR)
	{
		sn=0;
	}
	p->block_sn=sn;
	p->head.sn=sn;
	chksum=calc_short_sum((unsigned char *)(&sn),sizeof(sn),FLASH_STOER_CHKSUM_ORG);	//����У���
	chksum=calc_short_sum((unsigned char *)((unsigned int)p->p_data),p->data_size,chksum);	//����У���
	p->head.chksum=chksum;													//�ڻ��������У���

//	�ڽ���ǰ��������
//	if(p->p_data_bak)
//	{
//		memcpy(p->p_data_bak,p->p_data,sizeof(*p->p_data));							//ͬʱ���±�����������
//	}
	
	/*
	if(!(IAP_Flash_Write(adr,p->block_size,p->p_block,IAP_FLASH_ERASEPAGE)))
	{																						//д��ʧ�ܣ��л�����һҳ�ٴ�д��
		j=adr-p->start_adr;
		j/=p->page_size;
		adr=p->start_adr+(j+1)*p->page_size;	//�е���һҳ��ʼ
		if(adr-p->start_adr >= p->size)			//�����������ռ��ַ
		{
			adr=p->start_adr;	//���������洢����Χ���ص���ʼ
		}
		p->cur_adr=adr;									//���õ�ǰ��ַ
		r=IAP_Flash_Write(adr,p->block_size,p->p_block,IAP_FLASH_ERASEPAGE);				//��ҳ�߽��ٴ�д��
	}
	*/
	r=func_flash_iap_hword_write(adr,sizeof(FLASH_1B_STORE_HEAD_T),(unsigned char *)&p->head,IAP_FLASH_ERASEPAGE);
	r&=func_flash_iap_hword_write(adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size,(unsigned char *)p->p_data,IAP_FLASH_ERASEPAGE);
	if(!r)
	{																						//д��ʧ�ܣ��л�����һҳ�ٴ�д��
		j=adr-p->start_adr;
		j/=p->page_size;
		adr=p->start_adr+(j+1)*p->page_size;	//�е���һҳ��ʼ
		if(adr-p->start_adr >= p->size)			//�����������ռ��ַ
		{
			adr=p->start_adr;	//���������洢����Χ���ص���ʼ
		}
		p->cur_adr=adr;									//���õ�ǰ��ַ
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
		{							//�б��ݱȽ���
			if(memcmp(p->p_data,p->p_data_bak,p->data_size)!=0)
			{															//�Ƚϲ���ͬ					
				memcpy(p->p_data_bak,p->p_data,p->data_size);		//�������������������´αȽ�
				i=1;													//��ʱ��д�룬��������ʱ
			}
		}
		
		if(i==0 && memcmp(p->p_data,(unsigned char*)p->cur_adr+sizeof(FLASH_1B_STORE_HEAD_T),p->data_size)!=0)	//�Ƚ���������flash�����ݣ������ͬ��д��
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
