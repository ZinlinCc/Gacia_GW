/************************************Copyright (c)**************************************
                                          

--------------文件信息------------------------------------------------------------------
文 件 名: FUNC_USART.c
创 建 人: Zhoucongwen
email:	  mcuast@qq.com
创建日期: 2019年04月15日
描    述: 串口驱动
---------- 版本信息---------------------------------------------------------------------
 版    本: V1.0
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) C.C. 2019/04/15
All rights reserved
****************************************************************************************/ 

#define FUNC_USART_MAIN
#include "func_usart.h"
#include "string.h"
//--------------------------------------------------------------------------------------
//版本号管理
#define STM32F10X_USART_VER_C 101

	#if STM32F10X_USART_VER_C != STM32F10X_USART_VER_H
		#ERROR "STM32F10X_USART_C_VER ERROR!"
	#endif
//--------------------------------------------------------------------------------------
void usart_1ms_isr(void)
{
	USART_T *pu;
	int i;
	
	pu=&g_USART[0];
	for(i=0;i<USART_N;i++)
	{
		if(pu->huart != 0)
		{
			usart_tm_1mS(pu);
		}
		pu++;
	}
}
//通讯插入到定时器的程序
uint8_t usart_tm_1mS(USART_T *pu)
{
	uint8_t r=0;
	if(pu->tm_1mS!=0 && --pu->tm_1mS==0)
	{
		if(pu->adr==0 || TstB(pu->sta,B_USART_ADR_OK))
		{
			//!禁止继续接收
			//u32_clrb((unsigned long*)(pu->base+USART_CR1),B_USART_CR1_RE);	//禁止接收
			//------------------------------------------------------------
			//usart_dis_receive(pu->huart);			//禁止接收
			//------------------------------------------------------------
			u8_setb(&pu->sta,B_EV_USART_RX);			//置接收标志
			//u8_clrb(&pu->sta,B_USART_ADR_OK);
			r=1;
		}
		else
		{
			pu->r_e=0;		//地址不对,马上启动新的接收
		}
	}
	return(r);
}

//-------------------------------------------------------------------------------
//清零接收指针，切换为RSP85接收状态，允许接收
void usart_rx_rst(USART_T *pu)
{
	pu->r_s=0;
	pu->r_e=0;
	__usart_rs485_receive(pu);	//切换为RSP85接收状态
	if(!(pu->sta & (1<<B_USART_FD)))
	{
		__usart_dis_tc_it(pu);
	}
	__usart_en_receive(pu);
}
//切换为RSP85接收状态，允许接收，不清零指针
void usart_rx_en(USART_T *pu)
{
	__usart_rs485_receive(pu);
	if(!(pu->sta & (1<<B_USART_FD)))
	{
		__usart_dis_tc_it(pu);	//V115 禁止TC中断
	}
	__usart_en_receive(pu);		//允许接收
}
//单独清零接收指针
void usart_rx_clr(USART_T *pu)
{
	pu->r_s=0;
	pu->r_e=0;
	u8_clrb(&(pu->sta),B_EV_USART_RX);
}
//-------------------------------------------------------------------------------
//兼容半双工发送函数
//启动发送函数，调用函数前必须填充好发送缓冲区
void usart_hd_send_start(USART_T *pu,unsigned short n)
{
	if(n!=0)
	{
		u8_setb(&(pu->sta),B_USART_TX_BUSY);
		pu->t_s=0;		//半双工，每次清零
		pu->t_e=n;
		__usart_rs485_send(pu);
		if(!(pu->sta & (1 << B_USART_FD)))
		{
			__usart_dis_receive(pu);		//如果是全双工，就不关接收了
		}
		pu->r_s=0;		//半双工对接收清零
		pu->r_e=0;		//半双工对接收清零
		u8_clrb(&pu->sta,B_EV_USART_RX);
		u8_clrb(&pu->sta,B_USART_REC_1BYTE);		
		__usart_en_txe(pu);	//允许TXE中断
	}
}
//-------------------------------------------------------------------------------


//从接收缓冲区读取数据,最多n个
int usart_buf(USART_T *pu,unsigned char *buf,unsigned short n,unsigned char b_move)
{
	unsigned int head,tail,head_end,rn,len;
	
	if(n==0)
	{
		return(0);
	}
	
	head=pu->r_s;
	tail=pu->r_e;
	len=pu->rx_buf_size;
	
	head_end = len - head;		//求头到最后的字节数
	rn=( tail + head_end ) % len;		//求出目前接收到的字节数
		
	if(rn>0)
	{						//有数据
		if(rn > n)
		{					//数据量大于接收的数据n，只接收n个
			rn=n;
			tail=(head+rn)%len;		//重新计算尾部！防止后面出错
		}
		
		if(tail > head)
		{							//尾比头大，数据在连续区，直接拷贝
			memcpy(buf,pu->rx_buf+head,rn);
		}
		else
		{							//尾比头小，没有头尾相等状态
			//head_end=len-head;		//求头到最后的字节数
			memcpy(buf,pu->rx_buf+head,head_end);	//拷贝头到最后的字节
			memcpy(buf+head_end,pu->rx_buf,rn-head_end);	//拷贝剩下的字节
			
		}
		if(b_move)
		{							//如果删除接收缓冲区的内容，调整头指针
			pu->r_s=(head + rn) % len;
		}
	}
	return(rn);
}

//检查是否接收到一组数据的帧间隔
int usart_chk_frame(USART_T *pu)
{
	unsigned int i;
	i = u8_tscb(&(pu->sta),B_EV_USART_RX);
	if(i)
	{
		u8_clrb(&(pu->sta),B_USART_REC_1BYTE);
		return(1);
	}
	else
	{
		return(0);
	}
}

int usart_chk_recbyte(USART_T *pu)
{
	return(u8_tscb(&(pu->sta),B_USART_REC_1BYTE) !=0);
}

	
//从接收缓冲区读取数据,最多n个	
int usart_recv(USART_T *p,unsigned char *buf,unsigned short n)
{
	return(usart_buf(p,buf,n,1));
}

//从接收缓冲区拷贝数据,最多n个
int usart_copy(USART_T *p,unsigned char *buf,unsigned short n)
{
	return(usart_buf(p,buf,n,0));
}

//求接收缓冲区已经接收的字节数
unsigned int usart_recv_bytes(USART_T *pu)
{
	unsigned int i;
	i=(unsigned int)(pu->r_e) + (unsigned int)(pu->rx_buf_size) - (unsigned int)(pu->r_s);
	return(i % pu->rx_buf_size);
}

//从接收缓冲区删除n个数据
unsigned int usart_rbuf_erase(USART_T *pu,unsigned short n)
{
	unsigned int head,tail,rn,len;
	head=pu->r_s;
	tail=pu->r_e;
	len=pu->rx_buf_size;
	
	rn=(tail + len - head) % len;		//求出目前接收到的字节数
	
	if(n > rn)
	{
		n=rn;
	}
	
	pu->r_s=(head + n) % len;
	return(n);
}

//向通讯缓冲区填入字节，并启动发送
//p:通讯结构体指针
//buf:要发送字节的指针
//n：要发送的字节数
//返回：发送成功的字节数，由于通讯口剩余空间有可能比要发送的字节数小，返回的字节数小于等于n
int usart_send(USART_T *pu,unsigned char *buf,unsigned short n)
{
	unsigned int tn,head,tail,tail_end,len;
	
	if(n>0)
	{
		if(buf == pu->tx_buf)		//通讯发送缓冲区直接作为工作缓冲区
		{
			pu->t_s=0;
			pu->t_e=0;
		}
		
		head=pu->t_s;
		tail=pu->t_e;
		len=pu->tx_buf_size;
		
		
		tail_end = len - tail;						//求尾到最后的空间
		tn=(head + tail_end) % len;		//求出缓冲区剩下的空间
		
		if(tn==0)
		{
			tn=len;		//缓冲区空
		}
		
		tn--;			//区别缓冲区空与缓冲区满，实际空间减少一个
		
		if(n > tn)
		{
			n=tn;		//如果要求发送的内容超过缓冲区剩余的空间，则发送剩余空间大小的字节数
		}
		
		if(n!=0)
		{
			u8_setb(&(pu->sta),B_USART_TX_BUSY);			//切换成忙状态
			__usart_rs485_send(pu);		//RS485端口指向发送状态	
			
			if(buf != pu->tx_buf)
			{
				if(head > tail)
				{
					memcpy(pu->tx_buf+tail,buf,n);			//头大于尾，剩余空间在连续区域
				}
				else
				{
					//tail_end=len-tail;						//求尾到最后的空间
					if(tail_end>=n)
					{
						memcpy(pu->tx_buf+tail,buf,n);		//比n大，这段连续空间直接拷贝
					}
					else
					{
						memcpy(pu->tx_buf+tail,buf,tail_end);		//先拷贝尾到最后的字节数
						memcpy(pu->tx_buf,buf+tail_end,n-tail_end);	//再拷贝剩下的字节数
					}
				}
			}
			pu->t_e=(tail + n) % len;		//调整尾指针

			__usart_en_txe(pu);		//允许TXE中断
		}
		return(n);		//返回发送字节数
	}
	else
	{		
		return(0);
	}
}



//==============================================================================

//配置字符串"数据位，校验位，停止位，半双工/全双工，输出PP或OD，输入上拉或悬空，中断优先级，RS485DPIO，RS485PIN，RS485极性"
//CfgStr:"7/8,n/e/o,1/2,p/o,u/f,0-15,A-F,0-15,P/N"

typedef struct{
	char comm_port;
	char data_bits;
	char parity;
	char stop_bits;
	char hf_mode;
	char tx_port_mode;
	char rx_port_mode;
	char interrupt;
	char RS485_GPIO_BASE;
	char RS485_GPIO_PIN;
	char RS485_PORT_MODE;
	char RS485_GPIO_polarity;
	char str_end;
} USART_CFGSTR_T;


//CfgStr:"1-5,7/8,n/e/o,1/2,h/f,p/o,u/f,0-15,a-g 0,0-f,p/o,+/-"
//pu=usart_open("38n1fpu100p+",115200,256,256);		//全双工，无RS485控制，中断优先级1
//pu=usart_open("28n1hpu2a1p+",115200,256,256);		//半双工，PA1~RS485控制，中断优先级2
USART_T* usart_open(const char *cfg_str,unsigned int ulBaud,unsigned short tx_buf_size,unsigned int rx_buf_size)
{

	int com;
	int i;

	USART_CFGSTR_T CFG;
	
	//------------------------------------------------------------
	
	USART_T *pu=NULL;
	
	if(strlen(cfg_str)!=(sizeof(USART_CFGSTR_T)-1))
	{
		return(NULL);
	}
	
	memcpy(&CFG,cfg_str,sizeof(CFG)-1);
	CFG.str_end=0;
	str2lwr((char *)&CFG);
		
	com=CFG.comm_port - '1';

	if(com>=0 && com<=USART_N)
	{
		pu = &g_USART[com];	
		pu->huart = g_phuart[com].huart;
		pu->com = com;
	}
	else
	{
		return(NULL);
	}
	//--------------------------------------------------------------------------

	//奇偶校验和位长
//	if(CFG.data_bits=='8' && CFG.parity=='n')
//	{
//		pu->UART_WordLength=UART_WORDLENGTH_8B;
//	}
//	else if(CFG.data_bits=='8')
//	{
//		pu->UART_WordLength=UART_WORDLENGTH_9B;
//	}
	
	
	pu->UART_WordLength=8;
	
	if(CFG.parity=='n')
	{
		pu->UART_Parity=USART_PARITY_N;
	}
	else if(CFG.parity=='e')
	{
		pu->UART_Parity=USART_PARITY_E;
	}
	else if(CFG.parity=='o')
	{
		pu->UART_Parity=USART_PARITY_O;
	}

	//--------------------------------------------------------------------------
	//申请接收缓冲区
	pu->rx_buf_size=rx_buf_size;	
	pu->rx_buf=DataPool_Get(rx_buf_size);
	if(pu->rx_buf == NULL)
	{
		return(NULL);
	}
	
	if(CFG.hf_mode=='h')
		{	//半双工，发送缓冲区与接收缓冲区使用同一个空间
		pu->tx_buf=pu->rx_buf;
		pu->tx_buf_size=rx_buf_size;
	}
	else if(CFG.hf_mode=='f')
	{		//全双工，发送缓冲区与接收缓冲区使用独立空间
		pu->tx_buf_size=tx_buf_size;
		pu->tx_buf=DataPool_Get(tx_buf_size);
		pu->sta |= 1<<B_USART_FD;				//全双工标志
		if(pu->tx_buf==NULL)
		{
			return(NULL);
		}
	}
	else
	{
		return(NULL);
	}
	//--------------------------------------------------------------------------
	//配置输入RS485 的GPIO
	if(CFG.RS485_GPIO_BASE=='0')
	{
		pu->RS485_PIN = 0;
		pu->RS485_PORT = RS485_PORT_NULL;
	}
	else if(CFG.RS485_GPIO_BASE >='a' && CFG.RS485_GPIO_BASE <='g')
	{
		pu->RS485_PORT = CFG.RS485_GPIO_BASE - 'a';
		if(CFG.RS485_GPIO_polarity == '+')
		{
			pu->RS485_POL = 1;
		}
		else if(CFG.RS485_GPIO_polarity == '-')
		{
			pu->RS485_POL = 0;
		}
		else
		{
			return(NULL);
		}
		i=hexchar_btye(CFG.RS485_GPIO_PIN);
		if(i>=0)
		{	
			pu->RS485_PIN = i;
		}
		else
		{
			return(NULL);
		}
		if(CFG.RS485_PORT_MODE == 'o')
		{
			pu->RS485_PIN |= USART_RS485_PIN_MODE_OD;
		}
		__usart_rs485_port_init(pu);
		
		pu->sta |= (1 << B_USART_485_DELAY);	//默认延迟
	}
	else
	{
		return(NULL);
	}
	//-------------------------------------------------------------------------
	if(CFG.hf_mode=='f')
	{
		pu->sta |= 1<<B_USART_FD;				//全双工标志
	}
	else if(CFG.hf_mode != 'h')
	{
		return(NULL);
	}
	//-------------------------------------------------------------------------
	pu->baud=ulBaud;
	__usart_msp_init(pu);			//初始化串口
	__usart_rs485_receive(pu);		//初始化后切为接收状态
	//-------------------------------------------------------------------------
	__usart_en_err(pu);							//允许接收错误中断
	__usart_en_receive(pu);						//允许接收中断
	//---------------------------------------------------------------------
	Set_Usart_Tout_1mS(pu,ulBaud);				//设超时门限
	//---------------------------------------------------------------------
	i=hexchar_btye(CFG.interrupt);
	__usart_set_it_pri(pu,i);					//设中断优先级
	//---------------------------------------------------------------------
	return(pu);	
}

//-----------------------------------------------------------------------------------
void Set_Usart_Adr(USART_T *pu,unsigned char adr)
{
	pu->adr=adr;
}


//-----------------------------------------------------------------------------------
#define AutoBaud_Retry_NB	4
uint8_t Usart_Auto_BaudRate(uint8_t comm_ret,USART_T *pu)
{
	uint8_t r=0;

	if(!TstB(pu->baud_sta,B_USART_BAUD_LOCK) && (comm_ret!=0))
	{//波特率没有锁定
		if(comm_ret==USART_REC_OK || comm_ret==USART_REC_FORMATOK)
		{//收到正确的格式包或不采用波特率自适应,锁定波特率
			u8_setb(&(pu->baud_sta),B_USART_BAUD_LOCK);
			Set_Usart_Tout_1mS(pu,BAUD_USE_NB);
			r=1;
		}
		else
		{
			if(++pu->baud_sta>AutoBaud_Retry_NB)
			{
				pu->baud_sta=0;//cnt_1ms+pu->baud_tout;
				if(++(pu->baud_nb)>=BAUD_NB_MAX)
				{
					pu->baud_nb=0;
				}
				__usart_set_baudrate(pu,Tb_BaudRate[pu->baud_nb]);
			}
		}	
	}
	return(r);	
}
//-----------------------------------------------------------------------------------
void Set_Usart_Tout_1mS(USART_T *pu,uint32_t ulBaud)
{
	uint32_t j,k;

	if(ulBaud==BAUD_USE_NB)
	{
		ulBaud=Tb_BaudRate[pu->baud_nb];
	}

	if(TstB(pu->baud_sta,B_USART_BAUD_LOCK))
	{
		//k=2;
		k=3;
		j=(uint32_t)1000*4*11;	//(1/1mS)*4T*11B/BaudRate
	}	
	else
	{
		//k=1;
		k=2;
		j=(uint32_t)1000*3*11;	//(1/1mS)*3T*11B/BaudRate
	}

	j/=ulBaud;		

	if(j==0)
	{
		j++;
	}
	j+=k;	

	pu->tm_1mS_init=j;
}
//----------------------------------------------------------------------------
//从一个串口拷贝接收的内容发送到另一个串口
//pd:目标串口
//ps:源串口
//b_erase:是否删除源串口内容
unsigned int usart_copy_usart(USART_T *pd,USART_T *ps,unsigned char b_erase)
{
	unsigned int d_e,d_len,s_s,s_e,s_len;
	unsigned char *pdb,*psb; 
	unsigned int cnt=0;
	
	d_e=pd->t_e;
	d_len=pd->tx_buf_size;
	pdb=pd->tx_buf;
	s_s=ps->r_s;
	s_e=ps->r_e;
	s_len=ps->rx_buf_size;
	psb=ps->rx_buf;
	
	if(s_s!=s_e)
	{
		u8_setb(&(pd->sta),B_USART_TX_BUSY);			//切换成忙状态

		__usart_rs485_send(pd);			//RS485端口指向发送状态	
		
		while(s_s!=s_e)
		{
			pdb[d_e++]=psb[s_s++];
			s_s %= s_len;
			d_e %= d_len;
			cnt++;
		}
		
		if(b_erase)
		{
			ps->r_s=s_s;
		}
		pd->t_e=d_e;
		
		__usart_en_txe(pd);
	}
	return(cnt);
}




//#include "stdio.h"
//int fputc(int ch, FILE *f)	//要使用printf 必须重定义这个函数
/* FILE is typedef’ d in stdio.h. */ 
//FILE __stdout;
//FILE __stdin;
//int fputc(int ch,FILE *f)
//{
//	UART_WriteByte(UART_DebugInstance, ch);
//	return ch;
//}

//int fgetc(FILE *f)
//{
//    uint16_t ch;
//    while(UART_ReadByte(UART_DebugInstance, &ch));
//    return (ch & 0xFF);
//}


void User_UART_Receive_IT(USART_T *pu, uint32_t errorflags)
{
	
}


//串口中断函数
//void User_HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
void usart_isr(USART_T *pu)
{
	unsigned short chk;
	unsigned char errorflags;
	unsigned short n;
	unsigned char ch;
	unsigned long j;
	
	chk = __usart_isr_chk(pu);
	errorflags = chk >> 8;
	
	//接收中断
	if (chk & USART_ITCHK_RECEIVE)
	{
		//先接收内容
		ch = __usart_receive_byte(pu);
		
		pu->sta |= (1<<B_USART_REC_1BYTE);
		
		//n=缓冲区已经接收的字节数
		n=((unsigned int)(pu->r_e) + (unsigned int)(pu->rx_buf_size) - (unsigned int)(pu->r_s)) % (unsigned int)(pu->rx_buf_size);
			
		//if(n==0 || (i & COMM_STA_ERR_BIT)==0)
		if(errorflags == 0)
		{									//收到第一个字节或者有帧错误，帧错误一般用于波特率自适应
			if((n+1) < pu->rx_buf_size)
			{								//小于缓冲区
				pu->rx_buf[pu->r_e]=ch;		//接收一个字节
				if(TstB(pu->sta,B_USART_CHKADR) && n==0)
				{							//需要判断第一个地址，第一个字节判断一下地址
					if(ch==pu->adr || ch==0)
					{
						u8_setb(&(pu->sta),B_USART_ADR_OK);	//设地址匹配标志
					}
				}
				j=pu->r_e+1;
				j %= (pu->rx_buf_size);			//对尾指针增量
				pu->r_e=j;						//一次性赋值
			}
		pu->tm_1mS=pu->tm_1mS_init;				//设超时寄存器
		}
	}
	
	//发送缓存空中断
	if (chk & USART_ITCHK_TXE)
	{
		unsigned int j;									//发送一个字节
		if(pu->t_s!=pu->t_e) 
		{
			__usart_send_byte(pu, pu->tx_buf[pu->t_s]);	//发送字节

			j=pu->t_s+1;
			j %= (pu->tx_buf_size);
			pu->t_s=j;
		}
		else
		{	//最后一个字节移入发送寄存器
			__usart_dis_txe(pu);
			__usart_en_tc_it(pu);
		}
		return;
	}

	//发送移位寄存器空中断
	if (chk & USART_ITCHK_TC)
	{
		u8_clrb(&(pu->sta),B_USART_TX_BUSY);			//切换成空闲状态
		__usart_rs485_receive(pu);		//物理上发送完成
		__usart_dis_tc_it(pu);
		if(!(pu->sta & (1 << B_USART_FD)))
		{
			__usart_en_receive(pu);
		}
		return;
	}
}


void func_usart_485_delay_set(USART_T *pu,RS485_SEND_DELAY_E en)
{
	if(en)
	{
		pu->sta |= (1 << B_USART_485_DELAY);
	}
	else
	{
		pu->sta &= ~(1 << B_USART_485_DELAY);
	}
}
	
unsigned char usart_chk_send_busy(USART_T *pu)
{
	return(pu->sta & B_USART_TX_BUSY);
}

void func_usart_init(void)
{
	__usart_msp_port_init();
}

