#include "..\ATCMD\ATCMD.h"
#define FUNC_CM_MAIN
#include "FUNC_CM.H"


	#define CM_TOUT_100 10000
	#define CM_TOUT_60000 60000
	#define CM_TOUT_5000 5000
	#define CM_TOUT_CSQ 600000
	#define S_OK_R_N	"OK\r\n"
	#define S_BROM_R_N "M"  //the 不行 BROM\r\n 不行  不要带\r\n  M  F1
	#define S_CONOK_R_N "OK\r\n"// CONNECT OK\r\n   这个还有影响   不可靠
	#define S_UP_R_N	"UPDATE"    //"UPDATE OK\r\n"   
	//#define BC26IMEI     866971030607357   这种方式不可行
	#define BC26REG_AUTO       "+QLWOBSERVE:"
	
	//-----------------------------------------------------------------------------
  //+QLWOBSERVE: 0,19,0,0    object 与平台匹配
  //+QLWDATARECV: 19,1,0,6,000061626364   数据接收
  //时基单位贯穿总线


//================================================================================

//回调函数标志
enum{
	CKBID_NULL,
	CKBID_SW_ON,
	CKBID_ATRESET,
	CKBID_ATCIPSHUT,
	CKBID_ATE0,
	CKBID_ATCPIN,
	CKBID_ATCGREG,
	CKBID_ATCSQ,
	CKBID_ATCGATT,
	CKBID_ATCIPMUX,
	CKBID_ATCIPQSEND,
	CKBID_ATCSTT,
	CKBID_ATCIICR,
	CKBID_ATCIFSR,
	CKBID_ATCIPSTART,
	CKBID_RESET,
	CKBID_CIPCLOSE,
	CKBID_ATCIPSEND_HEAD,
	CKBID_ATCIPSEND_DATA,
};

//硬件开机
const ATCMD_SEND_T cm_hw_init_cmd[]={
	mATCMD_CBK(&__func_cm_pwr_off,0,500),						//关电源500ms，模块掉电
	mATCMD_CBK(&__func_cm_pwr_on,0,100),						//开电源，并延时100ms
	mATCMD_CBK(&__func_cm_sw_on,CKBID_SW_ON,2000),				//模块开机引脚拉低，等待1.8V
	mATCMD_CBK(0,0,500),										//延时500ms
	mATCMD_CBK(&__func_cm_sw_off,0,0),							//模块PWR拉高
	mATCMD_CBKTOUT(0,CKBID_RESET,5000),							//模块等待数据接收间隔达到5秒，就是等待模块初始化发的一串信息都完成了
	mATCMD_END
};

const ATCMD_SEND_T cm_at_init_cmd[]={
	mATCMD_AT("ATE0\r\n",CKBID_ATE0,1000),							//关回显
	mATCMD_AT("AT+CPIN?\r\n",CKBID_ATCPIN,1000),					//查新SIM卡	
	mATCMD_AT("AT+CGREG?\r\n",CKBID_ATCGREG,1000),					//GPRS 网络注册状态
	mATCMD_AT("AT+CSQ\r\n",CKBID_ATCSQ,1000),						//查询信号强度
	mATCMD_AT("AT+CGATT?\r\n",CKBID_ATCGATT,1000),					//GPRS 附着分离
	mATCMD_AT("AT+CIPMUX=1\r\n",CKBID_ATCIPMUX,1000),				//多链接模式
	mATCMD_AT("AT+CIPQSEND=0\r\n",CKBID_ATCIPQSEND,1000),			//慢速发送，等到发送成功
	mATCMD_AT("AT+CSTT\r\n",CKBID_ATCSTT,1000),						//启动任务并设置接入点 APN
	mATCMD_AT("AT+CIICR\r\n",CKBID_ATCIICR,1000),					//激活移动场景
	//mATCMD_AT("AT+CIFSR\r\n",0,100),								//读取本地IP
	mATCMD_AT("AT+CIPSHUT\r\n",CKBID_ATCIPSHUT,1000),				//关移动场景
	mATCMD_END
};



//================================================================================
//读取信号信息

//================================================================================

//================================================================================	
//硬件初始化相关
		

int func_cm_callback(unsigned short id,void *pdata,unsigned short *rec_n);
int func_cm_analyse_data(void *pdata,unsigned short *rec_n);
int func_cm_creat_connect(unsigned char cur_link,void *atbuf, void *ip);
int func_cm_creat_disconnect(unsigned char cur_link,void *atbuf);
int func_cm_creat_send_data_head(unsigned char cur_link,void *atbuf,RING_BUF_T *pr);
int func_cm_creat_send_data(unsigned char cur_link,void *atbuf,unsigned short buf_size,RING_BUF_T *pr);

void func_cm_init(FUNC_CM_T *p,USART_T *pu,USART_T *pm,unsigned char link_n,unsigned short buf_size)		//初始化函数
{
	int i;
	FUNC_CM_LINK_T *pl;
	
	__func_cm_msp_init();
	
	__func_cm_rst_lo();
	__func_cm_sw_off();
	__func_cm_pwr_off();	//模组断电
	
	//--------------------------------------------------------------------------------------------------------------------------------
	//对进程数据结构体初始化
	p->at_send_size=NET_BUF_SIZE;
	p->at_send_buf=(char *)DataPool_Get(p->at_send_size);		//申请AT命令发送缓冲区
	
	p->link_n=link_n;
	pl=&p->link[0];
	for(i=0;i<p->link_n;i++)
	{
		Ring_Buf_Init(&pl->rec_buf,buf_size);
		Ring_Buf_Init(&pl->send_buf,buf_size);
		pl++;
	}
	//---------------------------------------------------------------------------------------------------------------------------------
	atcmd_init(&g_cm.atcmd,pu,pm,&func_cm_callback,&func_cm_analyse_data,NET_BUF_SIZE);								//调用AT命令初始化函数
	//---------------------------------------------------------------------------------------------------------------------------------
}

//=====================================================================================================================================
//CM进程步骤
enum{
	FUNC_CM_STEP_START,
	FUNC_CM_STEP_POWON,
	FUNC_CM_STEP_POWON_WAIT,
	FUNC_CM_STEP_AT_INIT,
	FUNC_CM_STEP_AT_INIT_WAIT,
	FUNC_CM_STEP_CYCLE,
	FUNC_CM_STEP_WAIT_CSQ,
	FUNC_CM_STEP_WAIT_FUNC,
	FUNC_CM_STEP_WAIT_FUNC_SEND_DATA_HEAD,
	FUNC_CM_STEP_ERROR,
};


char test2[2048];

//CM进程
int func_cm_loop(FUNC_CM_T *p)	//进程
{
	int i;
	int j;
	//char *pc;
	//USART_T *pm; //不能只定义不去用呦，会出现指针的呦   你哪里用了呢？
	FUNC_CM_LINK_T *pl;
	
	switch(p->step)//默认为0？
	{
		case FUNC_CM_STEP_START:
			p->step=FUNC_CM_STEP_POWON;
			break;
		case FUNC_CM_STEP_POWON:
			atcmd_start(&p->atcmd,(ATCMD_SEND_T*)cm_hw_init_cmd,countof(cm_hw_init_cmd));	//执行硬件上电初始化
			p->step=FUNC_CM_STEP_POWON_WAIT;
			break;
		case FUNC_CM_STEP_POWON_WAIT:
			i=atcmd_sta(&p->atcmd);
			if(i ==ATCMD_STA_OK)
			{
				p->step=FUNC_CM_STEP_AT_INIT;					//正行完毕进入下一步
			}
			else if(i > ATCMD_STA_OK)
			{
				p->err_step=p->step;
				p->step=FUNC_CM_STEP_ERROR;						//错误
			}
			break;
		case FUNC_CM_STEP_AT_INIT:
			//break;
			atcmd_start(&p->atcmd,(ATCMD_SEND_T*)cm_at_init_cmd,countof(cm_at_init_cmd));	//执行AT指令初始化命令集
			p->step=FUNC_CM_STEP_AT_INIT_WAIT;
			break;
		case FUNC_CM_STEP_AT_INIT_WAIT:
			i=atcmd_sta(&p->atcmd);
			if(i ==ATCMD_STA_OK)
			{
				p->step=FUNC_CM_STEP_CYCLE;				//正确进入下一步
			}
			else if(i > ATCMD_STA_OK)
			{
				p->err_step=p->step;
				p->step=FUNC_CM_STEP_ERROR;						//错误
			}
			break;
		case FUNC_CM_STEP_CYCLE://这里是数据传输的关键
			//----------------------------------------
			//连接循环
			i=p->cur_link;
			i++;
			i %= p->link_n;
			p->cur_link=i;
			//----------------------------------------
			pl=&p->link[i];
			if(pl->func == FUNC_CM_LINK_FUNC_CONNECT)
			{
				func_cm_creat_connect(i,p->at_send_buf,&pl->send_buf);		//确保ip地址长度不超过发送长度
				p->at_send.cmd=p->at_send_buf;
				p->at_send.cmd_len=strlen(p->at_send.cmd);
				p->at_send.cbkid=CKBID_ATCIPSTART;
				p->at_send.tout=2000;
				p->at_send.type=ATCMD_TYPE_AT;
				atcmd_start(&p->atcmd,&p->at_send,1);
				p->cur_func = FUNC_CM_LINK_FUNC_CONNECT;
				p->step=FUNC_CM_STEP_WAIT_FUNC;
				pl->func = FUNC_CM_LINK_FUNC_NULL;
			}
			else if(pl->func == FUNC_CM_LINK_FUNC_DISCONNECT)
			{
				func_cm_creat_disconnect(i,p->at_send_buf);		//确保ip地址长度不超过发送长度
				p->at_send.cmd=p->at_send_buf;
				p->at_send.cmd_len=strlen(p->at_send.cmd);
				p->at_send.cbkid=CKBID_CIPCLOSE;
				p->at_send.tout=5000;
				p->at_send.type=ATCMD_TYPE_AT;
				atcmd_start(&p->atcmd,&p->at_send,1);
				p->cur_func = FUNC_CM_LINK_FUNC_DISCONNECT;
				p->step=FUNC_CM_STEP_WAIT_FUNC;
				pl->func = FUNC_CM_LINK_FUNC_NULL;
			}
			else if(pl->connected)
			{
				if(fixlen_que_firsize(&pl->send_buf))
				{
					func_cm_creat_send_data_head(i,p->at_send_buf,&pl->send_buf);		//确保ip地址长度不超过发送长度
					p->at_send.cmd=p->at_send_buf;
					p->at_send.cmd_len=strlen(p->at_send.cmd);
					p->at_send.cbkid=CKBID_ATCIPSEND_HEAD;
					p->at_send.tout=2000;
					p->at_send.type=ATCMD_TYPE_AT;
					atcmd_start(&p->atcmd,&p->at_send,1);
					p->cur_func = FUNC_CM_LINK_FUNC_SEND_DATA;
					p->step=FUNC_CM_STEP_WAIT_FUNC_SEND_DATA_HEAD;
					pl->func = FUNC_CM_LINK_FUNC_NULL;
				}
			}
			break;
		case FUNC_CM_STEP_WAIT_FUNC_SEND_DATA_HEAD:
			pl=&p->link[p->cur_link];
			i=atcmd_sta(&p->atcmd);
			if(i== ATCMD_STA_OK)
			{
				j=func_cm_creat_send_data(i,p->at_send_buf,p->at_send_size,&pl->send_buf);		//确保ip地址长度不超过发送长度
				p->at_send.cmd=p->at_send_buf;
				p->at_send.cmd_len=j;			//二进制，不能用strlen
				p->at_send.cbkid=CKBID_ATCIPSEND_DATA;
				p->at_send.tout=2000;
				p->at_send.type=ATCMD_TYPE_AT;
				atcmd_start(&p->atcmd,&p->at_send,1);
				p->cur_func = FUNC_CM_LINK_FUNC_SEND_DATA;
				p->step=FUNC_CM_STEP_WAIT_FUNC;
			}
			else if(i > ATCMD_STA_OK)
			{
				p->err_step=p->step;
				p->step=FUNC_CM_STEP_ERROR;
			}
			break;
		case FUNC_CM_STEP_WAIT_FUNC:
			pl=&p->link[p->cur_link];
			i=atcmd_sta(&p->atcmd);
			if(i== ATCMD_STA_OK)
			{
				switch(p->cur_func)
				{
					case FUNC_CM_LINK_FUNC_CONNECT:
						pl->connected=1;
						pl->func=FUNC_CM_LINK_FUNC_NULL;
						break;
					case FUNC_CM_LINK_FUNC_DISCONNECT:
						pl->connected=0;
						pl->func=FUNC_CM_LINK_FUNC_NULL;
						break;
					case FUNC_CM_LINK_FUNC_SEND_DATA:
						pl->func=FUNC_CM_LINK_FUNC_NULL;
						break;
				}
				p->step=FUNC_CM_STEP_CYCLE;
			}
			else if(i > ATCMD_STA_OK)
			{
				p->err_step=p->step;
				p->step=FUNC_CM_STEP_ERROR;
			}
			else if(i==ATCMD_STA_TOUT)
			{
				__nop();
			}
			break;
		default:
		case FUNC_CM_STEP_ERROR:	//超时或错误
			__nop();	//卡在这里，最后需要错误处理
			break;
	}
	
	atcmd(&p->atcmd);
	
	if(__func_cm_read_vcc())
	{
		LED1(0);
	}
	else
	{
		LED1(1);
	}
	
	return(0);
}

unsigned char func_cm_sta(FUNC_CM_T *p)
{
	unsigned char r=0;
	if(p->step == FUNC_CM_STEP_CYCLE)
	{
		r=1;
	}
	return r;
}

int net_4g_sta(void)
{
	return(func_cm_sta(&g_cm));
}

//=================================================================================================
int buf_erase(void *buf_s,int buf_s_n,void *buf_d,int buf_d_n)
{
	int r=0;
	int i;
	unsigned char *p0,*p1;
	
	if((buf_d_n !=0) && (buf_s_n !=0))
	{
		p0 = (unsigned char *)buf_d + buf_d_n;	//指向用掉的字符串后面一个位置
		p1 = (unsigned char *)buf_s + buf_s_n;	//指向所有的字符串后面一个位置
		i=p1-p0;
		
		if(i)
		{
			memcpy(buf_d,p0,i);	//把用掉的字符串后面部分移过来
		}
	}
	r=buf_s_n-buf_d_n;
	//((unsigned char *)buf_s)[r]=0;	//删除后面的字符以后要用0结尾
	return(r);				//返回剩下的字节数
}
//=================================================================================================



//分析服务器主动发回数据检测
int func_cm_analyse_data(void *pdata,unsigned short *rec_n)
{
	char *p0,*p1;
	char *porg=NULL;
	int i,n;
	int used_n;

	char ch[16];
	
	int r=CM_ANALYSE_DATA_RET_NULL;
	
	FUNC_CM_T *pcm = &g_cm;
	FUNC_CM_LINK_T *pl;
	
	while(*rec_n)
	{
		((unsigned char*)pdata)[*rec_n]=0;		//要在数据结尾放字符串结尾标志

		used_n=0;
		porg=strstr(pdata,"\r\n+RECEIVE");
		if(porg)
		{
			r=CM_ANALYSE_DATA_RET_REC_LOCK;
			while(1)
			{
				p0=strstr(porg,",");
				if(p0==NULL)
				{
					break;		//没有找到第一个逗号
				}
				p0++;
				p1=strstr(p0,",");
				if(p1==NULL)
				{
					break;		//没有找到第二个逗号
				}
				i=p1-p0;
				if(i != 1)
				{
					break;		//连接号字符串大于1
				}
				ch[0]=*p0;
				ch[1]=0;		//结束符
				n=asc2int(ch,&i);
				if(!i)
				{
					break;			//转换成数值不成功
				}
				if(n >= pcm->link_n)
				{
					break;			//数值比最大连接数大
				}
				//--------------------------------
				pl=&pcm->link[n];	//定位连接指针
				//--------------------------------
				p0=p1+1;
				p1=strstr(p0,":\r\n");
				if(p1==NULL)
				{
					break;			//没有找到":\r\n"
				}
				i=p1-p0;
				if(i>4)
				{
					break;			//接收字节数大于9999
				}
				memcpy(ch,p0,i);
				ch[i]=0;			//结束符
				
				n=asc2int(ch,&i);	//n=收到的字节数
				
				if(!i)	
				{
					break;			//转换成数值不成功
				}
				p1+=3;				//":\r\n",此时p1指向数据，n是接收到的字节数
				
				i=p1-(char *)pdata;	//求出:到之前的字符长度
				
				i+=2;				//结尾还有一个\r\n
				
				if((i + n) > *rec_n)
				{
					break;			//接收数据量少于实际接收数据
				}
				//------------------------------------------------
				
				//至此，收到一包数据分析成功，写入队列
				if(Ring_buf_leftbyte(&pl->rec_buf) >= (n+2))
				{
					fixlen_que_Wr(&pl->rec_buf,(unsigned char *)p1,n);
				}
				else
				{
					__nop();		//写入缓冲区满，丢弃
				}
				//------------------------------------------------
				i = p1-porg;
				i += (n+2);
				used_n=i;	//用掉的字节数
				break;
			}
		}//if(porg)
		else
		{
			for(i=0;i<LINK_NB;i++)
			{
				sprintf(ch,"\r\n%d, CLOSED\r\n",i);
				
				porg = strstr(pdata,ch);
				if(porg)
				{
					pcm->link[i].connected=0;
					used_n=strlen(ch);
					break;
				}
			}
		}
		if(used_n)
		{
			*rec_n=buf_erase(pdata,*rec_n,porg,used_n);
			r=CM_ANALYSE_DATA_RET_OK;
		}
		else
		{
			break;
		}
		
	
		
	}
	
	return(r);
}





//=================================================================================================
//AT指令返回信息判断回调函数，根据ID与返回的字符串，判断AT指令是否执行成功
int func_cm_callback(unsigned short id,void *pdata,unsigned short *rec_n)
{
	int r=0;	//返回信息
	int used_n=0;
	int i,j;
	char *p0=pdata;
	char *p1;
	unsigned short n=*rec_n;
	
	((unsigned char *)pdata)[*rec_n]=0;		//在接收缓存后面放字符串结尾标志
	
	switch(id)
	{
		case CKBID_NULL:
			break;
		case CKBID_SW_ON:
			if(__func_cm_read_vcc())
			{
				r=1;
			}
			break;
		
		case CKBID_ATCSQ:
//			if(strstr((char *)pdata,"+CSQ")!=NULL)
//			{
//				r=n;
//			}
			p1="OK\r\n";
			goto CKBID_COMM;

		case CKBID_ATRESET:
			p1="RDY\r\n";
			goto CKBID_COMM;
		
		case CKBID_RESET:
			if(n!=0)
			{
				used_n=n;
				r=1;
			}
			break;
		case CKBID_ATCIPSTART:
			p1="CONNECT OK\r\n";
			goto CKBID_COMM;
			
		case CKBID_CIPCLOSE:
			p1="CLOSE OK\r\n";
			goto CKBID_COMM;
			
		case CKBID_ATCIPSEND_HEAD:
			p1="> ";
			goto CKBID_COMM;
			
		case CKBID_ATCIPSEND_DATA:
			p1="SEND OK\r\n";
			goto CKBID_COMM;
//			p0=strstr((char *)pdata,"DATA ACCEPT");
//			if(p0!=NULL)
//			{
//				p0=strstr(p0,"\r\n");
//			}
//			if(p0!=NULL)
//			{
//				i=p0-(char*)pdata;
//				r=i+2;
//			}
//			break;
		case CKBID_ATCIPSHUT:
			p1="\r\nSHUT OK\r\n";
			goto CKBID_COMM;
		
		case CKBID_ATCGATT:
		case CKBID_ATCPIN:
		case CKBID_ATCGREG:
		case CKBID_ATE0:
		case CKBID_ATCIPMUX:
		case CKBID_ATCIPQSEND:
		case CKBID_ATCSTT:
		case CKBID_ATCIICR:
		case CKBID_ATCIFSR:
		default:
			p1="\r\nOK\r\n";
		CKBID_COMM:
			j=strlen(p1);		//特征字符串长度
			p0=strstr((char *)pdata,p1);
			if(p0!=NULL)
			{					//找到特征字符串
				i=p0-(char*)pdata;
				used_n=i+j;			//所有用掉的长度
				p0=pdata;
				r=1;
			}
			break;
	}
	*rec_n=buf_erase(pdata,*rec_n,p0,used_n);
	return(r);
}
//-----------------------------------------------------------------------------------------------
//创建一个TCP连接字符串
int func_cm_creat_connect(unsigned char cur_link,void *atbuf, void *ip)
{
	int i;
	char ch[256];
	i=fixlen_que_firsize(ip);
	if(i>126)
	{
		return(0);
	}
	fixlen_que_Rd(ip,(unsigned char*)ch,i);
	ch[i]=0;
	sprintf(atbuf,"AT+CIPSTART=%d,\"TCP\",%s\r\n",cur_link,ch);
	return(1);
}
//---------------------------------------------------
//创建一个断开TCP连接字符串
int func_cm_creat_disconnect(unsigned char cur_link,void *atbuf)
{
	sprintf(atbuf,"AT+CIPCLOSE=%d\r\n",cur_link);
	return(1);
}
//---------------------------------------------------
//创建一个发送字节头部字符串
int func_cm_creat_send_data_head(unsigned char cur_link,void *atbuf,RING_BUF_T *pr)
{
	int i;
	
	i=fixlen_que_firsize(pr);
	sprintf(atbuf,"AT+CIPSEND=%d,%d\r\n",cur_link,i);
	return(1);
}
//---------------------------------------------------
//创建发送字节
int func_cm_creat_send_data(unsigned char cur_link,void *atbuf,unsigned short buf_size,RING_BUF_T *pr)
{
	return(fixlen_que_Rd(pr,atbuf,buf_size));
}
//=================================================================================================
//--------------------------------- 用户接口函数开始 ----------------------------------
int func_cm_link_connect(FUNC_CM_T *p,unsigned char link,char *ip_port)
{
	FUNC_CM_LINK_T *pl;
	int r=0;
	
	OSSLK;
	while(1)
	{
		if(link >= p->link_n)
		{
			r=-1;		//连接号错误
			break;
		}
		pl=&p->link[link];
		if(pl->connected)
		{
			r=-2;		//已经处于连接状态
			break;
		}
		Ring_Buf_Clr(&pl->send_buf);
		Ring_Buf_Clr(&pl->rec_buf);
		fixlen_que_Wr(&pl->send_buf,(unsigned char *)ip_port,strlen(ip_port));
		pl->func=FUNC_CM_LINK_FUNC_CONNECT;	//这句一定要放在最后
		r=1;
		break;
	}
	OSSULK;
	return(r);
}


int func_cm_link_disconnect(FUNC_CM_T *p,unsigned char link)
{
	FUNC_CM_LINK_T *pl;
	int r=0;
	OSSLK;
	while(1)
	{
		if(link >= p->link_n)
		{
			r=-1;		//连接号错误
			break;
		}
		pl=&p->link[link];
		if(!pl->connected)
		{
			r=-2;		//已经处于断开状态
			break;
		}
		pl->func=FUNC_CM_LINK_FUNC_DISCONNECT;	//这句一定要放在最后
		r=1;
		break;
	}
	OSSULK;
	return(r);
}

int func_cm_link_send(FUNC_CM_T *p,unsigned char link,void *pdata,unsigned short n)
{
	FUNC_CM_LINK_T *pl;
	int i;
	int r=0;
	
	OSSLK;
	while(1)
	{
		if(link >= p->link_n)
		{
			r=-1;		//连接号错误
			break;
		}
	
		pl=&p->link[link];//C讲道理如果是未连接状态，应该运行不到这里，在app_up_link里就倒在APP_UP_LINK_STEP_CONNECT这步了
		if(!pl->connected)
		{
			r=-2;		//未连接状态
			break;
		}
		
		i=Ring_buf_leftbyte(&pl->send_buf);
		if((n+2) > i)
		{
			r=-3;
			break;
		}
		
		fixlen_que_Wr(&pl->send_buf,(unsigned char *)pdata,n);//C把数据放到发送缓冲区里去，然后在func_cm_loop里查看
		pl->func=FUNC_CM_LINK_FUNC_SEND_DATA;					//这句一定要放在最后--C了解,应该是只要放在fixlen_que_Wr后面都行
		r=1;
		break;
	}
	OSSULK;
	return(r);
}




//检测接收缓冲区是否有数据
int func_cm_link_chkrec(FUNC_CM_T *p,unsigned char link)
{
	FUNC_CM_LINK_T *pl;
	int r=0;
	OSSLK;
	while(1)
	{
		if(link >= p->link_n)
		{
			r=-1;		//连接号错误
			break;
		}
	
		pl=&p->link[link];
		
		r=fixlen_que_firsize(&pl->rec_buf);
		break;
	}
	OSSULK;
	return(r);
}



//读取缓冲区数据，若需要读取的数据量大于缓冲区，返回错误
//正确返回读取的字节数
int func_cm_link_rd_data(FUNC_CM_T *p,unsigned char link,void *pdata,unsigned short max_n)
{
	FUNC_CM_LINK_T *pl;
	int r=0;
	int i;
	OSSLK;
	while(1)
	{
		if(link >= p->link_n)
		{
			r=-1;		//连接号错误
			break;
		}
	
		pl=&p->link[link];
		
		i=fixlen_que_firsize(&pl->rec_buf);
		if(i>max_n)
		{
			r=-2;
			break;
		}
		r=fixlen_que_Rd(&pl->rec_buf,pdata,i);
		break;
	}
	OSSULK;
	return(r);
}



//--------------------------------- 用户接口函数结束 ----------------------------------
//=================================================================================================
