/************************************Copyright (c)**************************************

            单片机编程助手，帮您从繁杂的重复劳动中脱离出来，专心做您的创新与业务                  

                                    wwww.mcuabc.cn

----------------------------------------文件信息-----------------------------------------
文 件 名: 
创 建 人: 
email:	mcusys@qq.com
创建日期: 
描    述: 
----------------------------------------版本信息-----------------------------------------
 版    本: V1.00
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) xsy,ccmcu,zcw 2019/06/18
All rights reserved
****************************************************************************************/

#define APP_DOWN_LINK_MAIN
#include "app_down_link.h"

/***** 应用描述 这里输入对应用描述 *****/

//+++++User_Code_define Begin+++++//
BRK_2P_VAL_V100_T *brk0;

typedef struct {
	unsigned short HR[100];
}HR_TEST_T;
HR_TEST_T *brk_hr0;
//-----User_Code_define End-----//
/******************************** 实例主体函数定义 ********************************/
void app_down_link_init(APP_DOWN_LINK_T *p)
{
//+++++User_Code_app_down_link_init Begin+++++//
	unsigned int i,j;
	PRODUCT_T *pdt;
	DL_SLOT_T *psl;
	FUNC_MODBUS_MASTER_FUNC_T mf;
	
	//配置产品类型
	//------------------------------------
	//手工添加产品，2P断路器
	psl=&p->slot[0];
	pdt=&psl->pdt;
	pdt->product_code_s_ver=PRODUCT_CODE_BRK_2P_V100;
	pdt->hver=100;
	strcpy(pdt->sn,"12345");
	
	//------------------------------------
	//根据产品类型申请内存
	j=0;
	for(i=0;i<DL_SOLT_SIZE;i++)
	{
		psl=&p->slot[i];
		pdt=&psl->pdt;
		switch(pdt->product_code_s_ver)
		{
			case PRODUCT_CODE_BRK_2P_V100:
				psl->hr_size=BRK_2P_HR_SIZE;
				psl->buf_size=sizeof(BRK_2P_VAL_V100_T);
				break;
			case PRODUCT_CODE_BRK_1P_V100:
				psl->hr_size=BRK_2P_HR_SIZE;
				psl->buf_size=sizeof(BRK_2P_VAL_V100_T);
				break;
			default:
				j=1;
				break;
		}
		
		if(j)
		{
			break;
		}
		else
		{
			//---------------------------------------------------------------------------------
			psl->phr=DataPool_Get_Align(psl->hr_size*2,DPGA_HWORD);		//申请HR空间
			psl->buf=DataPool_Get_Align(psl->buf_size,DPGA_WORD);		//申请变量结构体空间
			//memcpy(psl->buf,&psl->pdt,sizeof(PRODUCT_T));				//拷贝产品信息到变量结构体
			
			brk0 = psl->buf;
			brk_hr0 = (HR_TEST_T*)psl->phr;
			//---------------------------------------------------------------------------------
			g_func_modbus_master0_slv_hr[i].hr_n=psl->hr_size;			//填写主模式HR与SLV关联表
			g_func_modbus_master0_slv_hr[i].phr=psl->phr;
			g_func_modbus_master0_slv_hr[i].slv=i+1;
			//---------------------------------------------------------------------------------
			mf.slv=g_func_modbus_master0_slv_hr[i].slv;					//添加规则循环命令
			mf.func=MD_RD_HR;
			mf.da_adr=0;
			mf.da_n=psl->hr_size;
			func_modbus_master_add_func(&g_func_modbus_master0,&mf,FUNC_MODBUS_MASTER_FUNC_CYC);
			//---------------------------------------------------------------------------------
			
		}
	}
//-----User_Code_app_down_link_init End-----//
}
void app_down_link(APP_DOWN_LINK_T *p)
{
//+++++User_Code_app_down_link_Entry Begin+++++//
//-----User_Code_app_down_link_Entry End-----//
//+++++User_Code_app_down_link_Step Begin+++++//
//-----User_Code_app_down_link_Step End-----//
//+++++User_Code_app_down_link_Exit Begin+++++//
//-----User_Code_app_down_link_Exit End-----//
}

//-------------------- 1ms中断函数 ----------------------
//需要使用的用func_LTB_add(&g_var.func_ltb_tick,&xxx_tick);在init程序或者OS任务while之前进行注册
//这样1ms的tick中断就会自动运行这个函数，建议普通延时使用left_ms操作，只有实时性要求非常高的代码放在这里
void app_down_link_tick(void)
{
//+++++User_Code_tick Begin+++++//
//-----User_Code_tick End-----//
}
//------------------ 1ms中断函数结束 --------------------
//+++++User_Code_end Begin+++++//
int BRK_2P_V100_HR_BUF(void *buf,unsigned short *phr);
int BRK_1P_V100_HR_BUF(void *buf, unsigned short *phr);
#define SOLT_ON_LINE_TOUT 3000



//一帧modbus通讯成功
void func_modbus_master_success(MODBUS_T *p)
{
	int i,err=0;

	PRODUCT_T *pdt;
	DL_SLOT_T *psl;
	i=p->slv-1;	//得到从机号,广播未处理？
	
	psl=&g_app_down_link.slot[i];
	LMSS(psl->tm_online,SOLT_ON_LINE_TOUT);			//成功的通讯，复位超时定时器
	pdt=&psl->pdt;
	pdt->b_online=1;
	
	if(p->da_adr == 0 && p->da_n == psl->hr_size)
	{												//成功进行一帧读取通讯
		switch(pdt->product_code_s_ver)
		{
			default:
				err=1;
				break;
			case PRODUCT_CODE_BRK_2P_V100:
				BRK_2P_V100_HR_BUF(psl->buf,psl->phr);
				break;
			case PRODUCT_CODE_BRK_1P_V100:
				BRK_2P_V100_HR_BUF(psl->buf,psl->phr);
				break;
		}
		if(!err)
		{
			memcpy(psl->buf,&psl->pdt,sizeof(PRODUCT_T));				//拷贝产品信息到变量结构体
		}
	}
}

#define mHR2_INT(A,B) (A[B] | (A[B+1]<<16))


int BRK_2P_V100_HR_BUF(void *buf, unsigned short *phr)
{
	BRK_2P_VAL_V100_T *pv=buf;
	float f;
	//-----------------------------------------------
	//控制
	pv->ctrl = phr[BRK_2P_HR_CTRL];
	//-----------------------------------------------
	//设定
	pv->set.il_en = phr[BRK_2P_HR_SET_IL_EN];
	pv->set.il_thre = phr[BRK_2P_HR_SET_IL_THRE];
	pv->set.in = phr[BRK_2P_HR_SET_IN];
	pv->set.oc_en = phr[BRK_2P_HR_SET_OC_EN];
	pv->set.oc_tm = phr[BRK_2P_HR_SET_OC_TM];
	pv->set.tpro_en = phr[BRK_2P_HR_SET_TPRO_EN];
	pv->set.upro_en = phr[BRK_2P_HR_SET_UPRO_EN];
	pv->set.upro_tm = phr[BRK_2P_HR_SET_UPRO_TM];
	pv->set.urst_tm = phr[BRK_2P_HR_SET_URST_TM];
	//-----------------------------------------------
	//状态
	pv->sta.al = phr[BRK_2P_HR_AL];
	pv->sta.b_manual_open = phr[BRK_2P_HR_MANUAL_OPEN];
	pv->sta.err = phr[BRK_2P_HR_ERR];
	pv->sta.hs_sta = phr[BRK_2P_HR_HS_STA];
	pv->sta.lock_sta = phr[BRK_2P_HR_LOCK_STA];
	pv->sta.pro = phr[BRK_2P_HR_PRO];
	//-----------------------------------------------
	//实时数值

	//f = mHR2_INT(phr,BRK_2P_HR_EPAL);
	//fff = f/EPA_DIV;


	f = mHR2_INT(phr,BRK_2P_HR_EPAL);
	pv->val.epa = f/EPA_DIV;

	f = phr[BRK_2P_HR_FREQ];
	pv->val.freq = f/FREQ_DIV;
	
	f = mHR2_INT(phr,BRK_2P_HR_IAL);
	pv->val.ia = f/I_DIV;
	
	pv->val.il = phr[BRK_2P_HR_IL];
	
	f = mHR2_INT(phr,BRK_2P_HR_PAL);
	pv->val.pa = f/P_DIV;
	
	f = phr[BRK_2P_HR_FPA];
	pv->val.pfa = f/PF_DIV;
	
	f = mHR2_INT(phr,BRK_2P_HR_QAL);
	pv->val.qa = f/P_DIV;
	
	f = mHR2_INT(phr,BRK_2P_HR_SAL);
	pv->val.sa = f/P_DIV;
	
	f = (short)phr[BRK_2P_HR_T1];
	pv->val.t1 = f/T_DIV;
	
	f = (short)phr[BRK_2P_HR_T2];
	pv->val.t2 = f/T_DIV;
	
	f = (short)phr[BRK_2P_HR_TB];
	pv->val.tb = f/T_DIV;
	
	f = mHR2_INT(phr,BRK_2P_HR_UAL);
	pv->val.ua=f/U_DIV;
	
	//---------------------------------------------------
	
	return(1);
}


int BRK_1P_V100_HR_BUF(void *buf, unsigned short *phr)
{
	BRK_1P_VAL_V100_T *pv=buf;
	float f;
	//-----------------------------------------------
	//控制
	pv->ctrl = phr[BRK_1P_HR_CTRL];
	//-----------------------------------------------
	//设定
	pv->set.il_en = phr[BRK_1P_HR_SET_IL_EN];
	pv->set.il_thre = phr[BRK_1P_HR_SET_IL_THRE];
	pv->set.in = phr[BRK_1P_HR_SET_IN];
	pv->set.oc_en = phr[BRK_1P_HR_SET_OC_EN];
	pv->set.oc_tm = phr[BRK_1P_HR_SET_OC_TM];
	pv->set.tpro_en = phr[BRK_1P_HR_SET_TPRO_EN];
	pv->set.upro_en = phr[BRK_1P_HR_SET_UPRO_EN];
	pv->set.upro_tm = phr[BRK_1P_HR_SET_UPRO_TM];
	pv->set.urst_tm = phr[BRK_1P_HR_SET_URST_TM];
	//-----------------------------------------------
	//状态
	pv->sta.al = phr[BRK_1P_HR_AL];
	pv->sta.b_manual_open = phr[BRK_1P_HR_MANUAL_OPEN];
	pv->sta.err = phr[BRK_1P_HR_ERR];
	pv->sta.hs_sta = phr[BRK_1P_HR_HS_STA];
	pv->sta.lock_sta = phr[BRK_1P_HR_LOCK_STA];
	pv->sta.pro = phr[BRK_1P_HR_PRO];
	//-----------------------------------------------
	//实时数值

	//f = mHR2_INT(phr,BRK_1P_HR_EPAL);
	//fff = f/EPA_DIV;


	f = mHR2_INT(phr,BRK_1P_HR_EPAL);
	pv->val.epa = f/EPA_DIV;

	f = phr[BRK_1P_HR_FREQ];
	pv->val.freq = f/FREQ_DIV;
	
	f = mHR2_INT(phr,BRK_1P_HR_IAL);
	pv->val.ia = f/I_DIV;
	
	pv->val.il = phr[BRK_1P_HR_IL];
	
	f = mHR2_INT(phr,BRK_1P_HR_PAL);
	pv->val.pa = f/P_DIV;
	
	f = phr[BRK_1P_HR_FPA];
	pv->val.pfa = f/PF_DIV;
	
	f = mHR2_INT(phr,BRK_1P_HR_QAL);
	pv->val.qa = f/P_DIV;
	
	f = mHR2_INT(phr,BRK_1P_HR_SAL);
	pv->val.sa = f/P_DIV;
	
	f = (short)phr[BRK_1P_HR_T1];
	pv->val.t1 = f/T_DIV;
	
	f = (short)phr[BRK_1P_HR_T2];
	pv->val.t2 = f/T_DIV;
	
	f = (short)phr[BRK_1P_HR_TB];
	pv->val.tb = f/T_DIV;
	
	f = mHR2_INT(phr,BRK_1P_HR_UAL);
	pv->val.ua=f/U_DIV;
	
	//---------------------------------------------------
	
	return(1);
}

//-----User_Code_end End-----//
