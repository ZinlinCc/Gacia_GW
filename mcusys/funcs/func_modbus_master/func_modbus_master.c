#define FUNC_MODBUS_MASTER_MAIN
#include "func_modbus_master.h"

enum {
	FUNC_MODBUS_MASTER_STEP_CHECK_FRAME,
	FUNC_MODBUS_MASTER_STEP_SEND
};




void func_modbus_master_init(FUNC_MODBUS_MASTER_T *p,USART_T *pu,const FUNC_MODBUS_MASTER_SLV_HR_T *tb_slv_hr)
{
	p->pu=pu;
	p->send_delay=FUNC_MODBUS_MASTER_SEND_DELAY;
	p->send_rty_nb=FUNC_MODBUS_MASTER_SEND_RTY;
	p->send_tout=FUNC_MODBUS_MASTER_SEND_TOUT;
	p->send_period=FUNC_MODBUS_MASTER_SEND_PERIOD;
	p->tb_slv_hr=tb_slv_hr;
}

unsigned char func_modbus_master_add_func(FUNC_MODBUS_MASTER_T *p,FUNC_MODBUS_MASTER_FUNC_T *pf,FUNC_MODBUS_MASTER_FUNC_E type)
{
	unsigned char r=0;
	unsigned char ch;
	if(type == FUNC_MODBUS_MASTER_FUNC_CYC)
	{
		if(p->func_cyc_nb < FUNC_MODBUS_MASTER_CYC_NB)
		{
			p->func_cyc[p->func_cyc_nb]=*pf;
			p->func_cyc_nb++;
			r=1;
		}
	}
	else
	{
		ch = (p->func_inj_t +1)%FUNC_MODBUS_MASTER_INJ_NB;
		if(ch!=p->func_inj_h)
		{
			p->func_inj[p->func_inj_t]=*pf;
			p->func_inj_t=ch;
			r=1;
		}
	}
	return(r);
}
void func_modbus_master_clr_func(FUNC_MODBUS_MASTER_T *p,FUNC_MODBUS_MASTER_FUNC_E type)
{
	if(type == FUNC_MODBUS_MASTER_FUNC_CYC)
	{
		p->func_cyc_nb=0;
		p->func_cyc_cur=0;
	}
	else
	{
		p->func_inj_t=p->func_inj_h=0;	
	}
}


__weak void func_modbus_master_after_rec(FUNC_MODBUS_MASTER_T *p)
{
	return;
}

__weak void func_modbus_master_before_send(FUNC_MODBUS_MASTER_T *p)
{
	return;
}


static unsigned char get_func(FUNC_MODBUS_MASTER_T *p)
{
	FUNC_MODBUS_MASTER_FUNC_T *pf=NULL;
	int i,r=0;
	
	if(p->func_inj_h != p->func_inj_t)
	{
		pf=&p->func_inj[p->func_inj_h];
		p->func_inj_h++;
		p->func_inj_h %= FUNC_MODBUS_MASTER_INJ_NB;
		p->cur_func_mode = FUNC_MODBUS_MASTER_FUNC_INJ;
	}
	else
	{
		if(p->func_cyc_nb !=0)
		{
			pf=&p->func_cyc[p->func_cyc_cur];
			p->func_cyc_cur++;
			p->func_cyc_cur %= p->func_cyc_nb;
		}
		p->cur_func_mode = FUNC_MODBUS_MASTER_FUNC_CYC;
	}
	
	if(pf)
	{
		for(i=0;;i++)
		{	//利用SLV查找对应的HR与HR数量
			if(p->tb_slv_hr[i].slv==pf->slv)
			{
				r=1;
				break;
			}
			if(p->tb_slv_hr[i].slv==0)
			{
				break;
			}
		}
		if(r)
		{
			p->md.phr=p->tb_slv_hr[i].phr;
			p->md.hr_n=p->tb_slv_hr[i].hr_n;
			p->md.func=pf->func;
			p->md.slv=pf->slv;
			p->md.da_adr=pf->da_adr;
			p->md.da_n=pf->da_n;
		#ifdef MODBUS_RW_EN
			p->md.rww_adr=pf->rww_adr;
			p->md.rww_n=pf->rww_n;
		#endif
//			if(pf->mff)
//			{
//				pf->mff();
//			}
		}
	}	
	return(r);
}

enum{
	FUNC_MODBUS_MASTER_STEP_CHK_PERIOD,
	FUNC_MODBUS_MASTER_STEP_CHK_REC,
	FUNC_MODBUS_MASTER_STEP_RESEND,
};


__weak void func_modbus_master_success(MODBUS_T *p)
{
	(void)p;
}

void func_modbus_master_exec(FUNC_MODBUS_MASTER_T *p)
{
	int i;
	
	switch(p->step)
	{
		case FUNC_MODBUS_MASTER_STEP_CHK_PERIOD:
			if(left_ms(&p->tm)==0)
			{
				if(usart_chk_recbyte(p->pu))
				{
					i= MyRnd() % 10;		//取0-9随机数
					i += FUNC_MODBUS_MASTER_CHKBUS_DELAY;
					left_ms_set(&p->tm,i);		//如果期间收到其他接收，缓10mS再操作
				}
				else if(left_ms(&p->tm_period)==0)
				{
					left_ms_set(&p->tm_period,p->send_period);
					if(get_func(p))
					{
						i = modbus_master_send(&p->md,p->pu->tx_buf);
						if(i>0)
						{
							p->send_rty_cnt = 0;
							usart_hd_send_start(p->pu,i);
							left_ms_set(&p->tm,p->send_tout);
							p->step=FUNC_MODBUS_MASTER_STEP_CHK_REC;
						}
					}
				}
			}
			break;
		case FUNC_MODBUS_MASTER_STEP_CHK_REC:
			if(usart_chk_frame(p->pu))
			{
				if((p->cur_func_mode == FUNC_MODBUS_MASTER_FUNC_CYC) && (p->func_inj_h != p->func_inj_t))
				{	//当前执行命令是循环命令且发现有注入命令，直接抛弃这次通讯任务
					left_ms_set(&p->tm,p->send_delay);
					p->step=FUNC_MODBUS_MASTER_STEP_CHK_PERIOD;
				}
				else if(modbus_master_rec(&p->md,p->pu->rx_buf,p->pu->r_e)==MODBUS_SUCCESS)
				{
					left_ms_set(&p->tm,p->send_delay);
					p->step=FUNC_MODBUS_MASTER_STEP_CHK_PERIOD;
					//一次成功的通讯
					func_modbus_master_success(&p->md);
				}
				else
				{
					i= MyRnd() % 10;		//取0-9随机数
					i += FUNC_MODBUS_MASTER_CHKBUS_DELAY;
					left_ms_set(&p->tm,i);		//如果期间收到其他接收，缓10mS再操作
					p->step=FUNC_MODBUS_MASTER_STEP_RESEND;
				}
			}
			else if(left_ms(&p->tm)==0)
			{
				i= MyRnd() % 10;		//取0-9随机数
				i += FUNC_MODBUS_MASTER_CHKBUS_DELAY;
				left_ms_set(&p->tm,i);		//如果期间收到其他接收，缓10mS再操作
				p->step=FUNC_MODBUS_MASTER_STEP_RESEND;
			}
			break;
		case FUNC_MODBUS_MASTER_STEP_RESEND:
			if(left_ms(&p->tm)==0)
			{
				if(usart_chk_recbyte(p->pu))
				{
					i= MyRnd() % 10;		//取0-9随机数
					i += FUNC_MODBUS_MASTER_CHKBUS_DELAY;
					left_ms_set(&p->tm,i);		//如果期间收到其他接收，缓10mS再操作
				}
				else
				{
					if(p->send_rty_cnt >= p->send_rty_nb)
					{
						p->step=FUNC_MODBUS_MASTER_STEP_CHK_PERIOD;
					}
					else
					{
						
						p->send_rty_cnt ++;
						p->err_cnt++;
						i = modbus_master_send(&p->md,p->pu->tx_buf);
						if(i>0)
						{
							left_ms_set(&p->tm_period,p->send_period);
							usart_hd_send_start(p->pu,i);
							left_ms_set(&p->tm,p->send_tout);
							p->step=FUNC_MODBUS_MASTER_STEP_CHK_REC;
						}
						else
						{
							p->step = FUNC_MODBUS_MASTER_STEP_CHK_PERIOD;
						}
					}
				}
			}
			break;
	}
}



