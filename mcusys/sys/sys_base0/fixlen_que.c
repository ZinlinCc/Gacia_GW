/************************************Copyright (c)**************************************
                                          

--------------文件信息------------------------------------------------------------------
文 件 名: fixlen_que.c
创 建 人: Zhoucongwen
email:	  mcuast@qq.com
创建日期: 2019年04月15日
描    述: 固定长度队列
---------- 版本信息---------------------------------------------------------------------
 版    本: V1.0
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/
#include "fixlen_que.h"
#include "string.h"

//----------------------------------------------------------------------
//版本号管理
#define FIXLEN_QUE_VER_C 100

	#if FIXLEN_QUE_VER_C != FIXLEN_QUE_VER_H
		#ERROR
	#endif
//----------------------------------------------------------------------

//-----------------------------------------------------------------------------
//向缓冲区里写一组定长数据
//成功：返回0
//失败：返回-1
int fixlen_que_Wr(FIXLEN_QUE_T *pr,unsigned char *buf,unsigned short n)
{
	unsigned int i;
	i=Ring_buf_leftbyte(pr);
	if((n+2)>i)
	{
		return(-1);
	}
	
	Ring_Buf_Wr(pr,(unsigned char *)&n,2);
	Ring_Buf_Wr(pr,buf,n);
	return(0);
}
//---------------------------------------------------
//从缓冲区里读一组定长数据
//成功：返回字节数
//失败：返回0
int fixlen_que_Rd(FIXLEN_QUE_T *pr,unsigned char *buf,unsigned short buf_size)
{
	unsigned short i;
	unsigned short n;
	
	i=Ring_buf_byte(pr);
	if(i<3)
	{
		return(0);
	}
	Ring_Buf_Copy(pr,(unsigned char *)&n,2);
	if(i < (n+2))
	{
		return(0);
	}
	if(n > buf_size)
	{
		return(-1);
	}
	Ring_Buf_Erase(pr,2);
	Ring_Buf_Rd(pr,buf,n);
	return(n);
}
//-----------------------------------------------------------------------------
//从缓冲区里检测最近的数据长度
//成功：返回字节数
//失败：返回0
int fixlen_que_firsize(FIXLEN_QUE_T *pr)
{
	unsigned short i;
	unsigned short n;
	
	i=Ring_buf_byte(pr);
	if(i<3)
	{
		return(0);
	}
	Ring_Buf_Copy(pr,(unsigned char *)&n,2);
	if(i < (n+2))
	{
		return(0);
	}
	return(n);
}
//-----------------------------------------------------------------------------

