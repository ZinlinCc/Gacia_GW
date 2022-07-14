/************************************Copyright (c)**************************************
                                          

--------------文件信息------------------------------------------------------------------
文 件 名: key.c
创 建 人: Zhoucongwen
email:	  mcuast@qq.com
创建日期: 2019年04月15日
描    述: 按键处理
---------- 版本信息---------------------------------------------------------------------
 版    本: V1.0
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/ 
#include "key.h"

//每10-20mS执行一次，新的键码送入缓冲区
unsigned char key32_oper(KEY32_T *p,unsigned int key_input,unsigned char key_nb)
{
	unsigned int i,j,k,key_input_last,sta,sta_last;
	unsigned char r,kc;
	
	r=0;
	sta=p->sta;				//频繁使用的变量，采用动态变量过度效率更高
	sta_last=sta;			//保存上次实际值
	
	key_input_last=p->input_last;	//取出上次状态
	
	i = key_input & key_input_last;		//新状态与上次状态进行与操作，剩下的'1'表示两次都是'1'
	
	sta |= i;						//确认1的位加载到实际状态
	
	i = key_input | key_input_last;		//新状态与上次状态进行与操作，剩下的'0'表示两次都是'0'
	
	sta &= i;						//确认0的位加载到实际状态
	
	p->input_last = key_input;		//更新最后一次输入值
	
	
	
	if(p->option & KEY_OPTION_SHORT_NEG)
	{
		j = (sta ^ sta_last) & p->sta;		//按键释放有效模式，j里面的内容表示从1跳变为0的位
	}
	else
	{
		j = (sta ^ sta_last) & sta;		//常规模式，j里面的内容表示从0跳变为1的位
	}
	
	p->sta = sta;					//存回防抖后的状态

	k=0x01;				//用于测试的位
	for(i=0;i<key_nb;i++)	//循环32次，测试每一位
	{
		//-------------------------------------------------------------------------------------
		//长键判断
		kc=i+1;
		if(sta & k)
		{
			if(p->cnt[i] < KEY32_CNT_MAX)			//按键一直按下，计数器增量
			{
				if(++p->cnt[i] == KEY32_LONG_THRE)
				{
					if(!(p->option & KEY_OPTION_SHORT_NEG))
					{
						if(add_key32_code(&p->key_code,(kc+KEY32_CODE_LONGMARK)))
						{
							r=1;	//有新长按键
						}
					}
				}
			}
			else
			{
				if(kc == KEY_CODE_UP || kc == KEY_CODE_DOWN)
				{
					p->cnt[i] = KEY32_LONG_THRE;
					if(add_key32_code(&p->key_code,kc))
					{
						r=1;	//有新按键
					}
				}
			}
		}
		else if(!(p->option & KEY_OPTION_SHORT_NEG))
		{
			p->cnt[i]=0;		//按键释放，计数清零
		}
		//-------------------------------------------------------------------------------------
		//短键判断
		if(j & k)
		{
			if((p->option & KEY_OPTION_SHORT_NEG))
			{	
				if(p->cnt[i] >= p->lk_thre0)
				{
					kc+=KEY32_CODE_LONGMARK;
				}
				if(p->cnt[i] >= p->lk_thre1)
				{
					kc+=KEY32_CODE_LONGMARK1;
				}
				p->cnt[i]=0;
			}
		
			if(add_key32_code(&p->key_code,(kc)))
			{
				r=1;	//有新按键
			}
			else
			{
				break;	//缓冲区已满直接退出
			}
		}
		k <<= 1;	//测试位左移一位
		//-------------------------------------------------------------------------------------
	}
	return(r);
}

//从按键缓冲区里面获取一个键码，成功返回非零，无键码返回0
unsigned char get_key32_code(KEY_CODE_T *p)
{
	unsigned char key_code=0;
	if(p->code_tail != p->code_head)
	{
		key_code=p->code[p->code_head];	//缓冲区有键码
		p->code_head = ++p->code_head & KEY32_CODE_POINT_MASK;	//调整头指针
	}
	return(key_code);		//返回键码
}

//从按键缓冲区里面获取一个键码，成功返回非零，无键码返回0
unsigned char add_key32_code(KEY_CODE_T *p,unsigned char key_code)
{
	unsigned char temp;
	unsigned char r=0;
	
	temp = (p->code_tail+1) & KEY32_CODE_POINT_MASK;	//尾指针循环增量
	if(temp != p->code_head)
	{
		p->code[p->code_tail]=key_code;	//有新的键从0跳变到1，加入一个键码到缓冲区，键值>0
		p->code_tail=temp;	//刷新尾指针
		r=1;
	}
		
	return(r);		//返回键码
}

void key32_init(KEY32_T *p,unsigned char option,unsigned short lk_thre0,unsigned short lk_thre1)
{
	p->option = option;
	p->lk_thre0 = lk_thre0;
	p->lk_thre1 = lk_thre1;
}

