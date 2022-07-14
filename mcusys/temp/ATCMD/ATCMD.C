#define ATCMD_MAIN
#include "ATCMD.H"

#include "string.h"
#include "..\sys\sys_base0\utility.h"

//char test1[4096];
//unsigned int test_n;

int atcmd_init(ATCMD_T *p,USART_T *pu,USART_T *pm,Func_CBK_T cbk,Func_cm_analyse_data_T als_data,unsigned short rec_buf_size)	//初始化
{
	p->pu=pu;								//主串口指针
	p->pm=pm;								//监视串口指针
	if(pm)
	{
		p->option |= ATCMD_OPTION_MONI;
	}
	p->rty_n=1;							//重发次数
	p->sta=ATCMD_STA_STOP;
	
	p->cbk = cbk;
	p->als_data = als_data;
	p->rec_buf = (unsigned char *)DataPool_Get(rec_buf_size);
	p->rec_buf_size = rec_buf_size;
	return(0);
}

void atcmd_start(ATCMD_T *p,ATCMD_SEND_T *cmds,unsigned short n)		//复位并启用运行一串AT指令
{
	p->cur_cmd=0;
	p->sta=ATCMD_STA_PROCESS;
	p->step=0;
	p->pcmd=cmds;
	p->cmd_n=n;
}


unsigned char atcmd_sta(ATCMD_T *p)		//查询状态
{
	return(p->sta);
}


int atcmd(ATCMD_T *p)			//AT指令进程,返回状态
{
	char *buf;
	unsigned int send_n=0;
	unsigned int n=0;
	int i,cbr;
	ATCMD_SEND_T *pcmd;//借助中间变量指针
	USART_T *pu=p->pu;
	USART_T *pm=p->pm;
	
	
	//if(usart_chk_frame(pu))	//判断是否接收到一帧数据
	n=usart_recv_bytes(pu);
	if(n)
	{
		//n=usart_recv_bytes(pu);
//		if(p->option & ATCMD_OPTION_MONI && p->pm)
//		{
//			usart_copy_usart(pm,pu,0);//将接收到的数据发送至PC
//		}
		
		if((n+p->rec_n+1) < p->rec_buf_size)
		{			
			usart_recv(pu,&p->rec_buf[p->rec_n],n);
			
			if(p->option & ATCMD_OPTION_MONI && p->pm)
			{
				usart_send(pm,&p->rec_buf[p->rec_n],n);//将接收到的数据发送至PC
			}
			//----------------------------------------------
			//测试代码
//			memcpy(&test1[test_n],&p->rec_buf[p->rec_n],n);
//			test_n+=n;
//			memcpy(&test1[test_n],"    ",4);
//			test_n+=4;
			//----------------------------------------------
			
			p->rec_n+=n;
			p->rec_buf[p->rec_n]=0;	//强制给接收数据结尾放置字符串结束符
		}
		else
		{	//溢出，清空接收缓冲区
			p->rec_n=0;
			usart_rx_clr(pu);
		}
	}
	//--------------------------------------------------------------------
	//监视口发来数据
	if(p->option & ATCMD_OPTION_MONI && pm)//如果存在监视口，同时转发监视
	{
		if(usart_chk_frame(pm))	//判断是否接收到一帧数据
		{
			usart_copy_usart(pm,pm,0);
			usart_copy_usart(pu,pm,1);
		}
	}
	//--------------------------------------------------------------------
	if(p->rec_n!=0)
	{
		i=p->als_data(p->rec_buf,&p->rec_n);	//分析一下接收的数据是否含有服务器主动发来的数据，如果有i!=0
		//RECEIVE过程中先等待receive结束再给AT回调函数检测
		
		if(i==CM_ANALYSE_DATA_RET_OK)
		{
			p->rec_lock=0;
		}
		else if(i==CM_ANALYSE_DATA_RET_REC_LOCK)
		{
			if(!p->rec_lock)
			{
				p->rec_lock=1;
				LMSS(p->rec_tout,10000);
			}
			else if(LMSZ(p->rec_tout))
			{
				p->rec_lock=0;
				p->rec_n=0;					//2秒没有接收完成，删除数据
			}
		}
	}
		
	
	if(p->sta==ATCMD_STA_PROCESS)
	{
		pcmd=&p->pcmd[p->cur_cmd];	//读取命令
		switch(p->step)//会不会串起来啊
		{
			case ATCMD_START:
				if(p->cur_cmd >= p->cmd_n)
				{
					p->sta=ATCMD_STA_OK;			//全部执行完毕
					break;
				}
				p->rty_cnt=0;            				//复位数据重发计数
			case ATCMD_STEP_GET_CMD:
				if(p->rty_cnt > p->rty_n)
				{
					p->sta=ATCMD_STA_ERR;			//全部执行完毕
					p->step = ATCMD_STEP_ERR;			//重发次数超限
				}
				else
				{
					p->rty_cnt++;
					left_ms_set(&p->tout,pcmd->tout);	//设超时
					if(pcmd->type==ATCMD_TYPE_AT)		//AT指令
					{
						buf=(char *)pcmd->cmd;
						send_n=pcmd->cmd_len;			//0
						p->step=ATCMD_STEP_WAIT_RSP;
					}
					else if(pcmd->type==ATCMD_TYPE_CBK || pcmd->type==ATCMD_TYPE_CBK_TOUT)	//函数调用指令||函数调用指令等待超时指令，一般用于模块开机
					{
						if(pcmd->cmd != NULL)
						{
							(*(Function_T)(pcmd->cmd))();	//如果函数指针是空，就什么也不干
						}
						p->step=ATCMD_STEP_WAIT_RSP;
					}
					else
					{
						p->sta=ATCMD_STA_OK;			//全部执行完毕
					}
				}
				break;
			case ATCMD_STEP_WAIT_RSP:
				if(left_ms(&p->tout)==0)
				{											//超时时间到
					if(pcmd->cbkid == 0 || pcmd->type ==ATCMD_TYPE_CBK_TOUT)		//无回调函数标志，超时就是延时，直接下一条指令||超时函数也进入下一步
					{
						p->cur_cmd++;							
						p->step=ATCMD_START;
					}
					else
					{
						p->step=ATCMD_STEP_GET_CMD;			//超时重发	
					}
				}
				else
				{														//没超时
					cbr=ATCMD_CBK_RSP_NULL;
					if(!p->rec_lock)
					{
						if((p->rec_n != 0) || pcmd->type==ATCMD_TYPE_CBK)		//有收到数据，或者是函数命令
						{
							cbr=p->cbk(pcmd->cbkid,p->rec_buf,&p->rec_n);	//调用回调函数，cbkid=0也是调用一下函数
						}
					}
					
					if(cbr > 0)
					{	
						if(pcmd->type==ATCMD_TYPE_CBK_TOUT)			//如果是超时函数，有数据就复位超时
						{
							left_ms_set(&p->tout,pcmd->tout);
						}
						else
						{
							p->cur_cmd++;							//返回确认，下一条指令
							p->step=ATCMD_START;
						}
					}
					else if(cbr < 0 )				//错误，重发
					{											
						p->step=ATCMD_STEP_GET_CMD;
					}
				}
				break;
			case ATCMD_STEP_OK:
				break;
			case ATCMD_STEP_ERR:
				break;
		}
		if(send_n>0)
		{			
			usart_send(pu,(unsigned char *)buf,send_n);//发送AT指令的
			if(p->option & ATCMD_OPTION_MONI && pm)//如果存在监视口，同时转发监视
			{
				usart_send(pm,(unsigned char *)buf,send_n);//PC查看需要发送的东西
			}
		}
	}//if(p->sta==ATCMD_STA_PROCESS)
	
	return(p->sta);
}




