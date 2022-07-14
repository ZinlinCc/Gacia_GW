#define CCOS_GLOBALS
#include "CCOS.H"
#include "prj_config.h"
//----------------------------------------------------------------------
//�汾�Ź���
#define CCOS_VER_C 204

	#if CCOS_VER_C != CCOS_VER_H
		#ERROR
	#endif
//----------------------------------------------------------------------

//=============================================================================
//��ʼ������ϵͳ
void OSInit(void)
{
	int i;
	OS_I_DisAll();
	
	
	
#ifdef OSTimeSlices_EN
	
	i=OS_TASK_TimeSlices;
	OSSys.OSTimeSlices_Pri=i;		//����ʱ��Ƭ���ȵ���ʼ���ȼ�
	OSSys.OSTimeSlices_PriCur=i;	//����ʱ��Ƭ���ȵĵ�ǰ���ȼ�
	OSSys.OSTimeSlices_Tbl= ((OSRdy_T)(-1)) << i;	//��ʱ��Ƭ������������ǼǱ��Ա���������ж������Ƿ�����ʱ��Ƭ��������
	
#endif
	
	OSSys.OSRunning=0;							//���б�־����
	OSSys.OSRdytbl=mPri_Tbl(OS_TASK_MAX);		//�������ʼ����Idle������Զ����״̬
	OSSys.OSPrioCur=OS_IDLE_PRI;				//��ǰ������Ϊϵͳ����
	OSSys.TCB_Len=sizeof(OS_Tcb[0]);			//������ƿ鳤�ȣ����ڻ�����ʶ�𳤶�
	
	for(i=0;i<OS_TASK_MAX;i++)
	{
		OS_Tcb[i].Stk=0;						//��ջ��0��ʾû�и�����
	}
	
	//����ϵͳ��̨Idle����	
	OS_Tcb[OS_TASK_MAX].Stk=OSTaskStkInit(OS_TaskIdle, &OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE-1]);	//��ʼIdle�������ջ
}
//=============================================================================
//��������
//����������������ָ�룬���ȼ�
//���أ�����������ɹ�������������ȼ������ȼ���ͻ
OSErr_T OSTaskCreate (void (*task)(void), OSPri_T Pri, OSStk_T *Stk_Top, OSStk_Size_T stk_init_size, OSTaskOpt_Par_T O_Par)
{
	struct OS_TCB *p;
	unsigned int i;
	OSStk_T *pt;
	
	if(Pri>=OS_TASK_MAX)
	{
		return(OS_ERR_CREATE_TASK_MAX);			//�������������
	}
	if(OS_Tcb[Pri]. Stk!=0)
	{
		return(OS_ERR_CREATE_TASK_PRI);			//���ȼ���ͻ
	}
	
	pt = Stk_Top;
	for(i=0;i<stk_init_size;i++)
	{
		*pt-- = 0xffffffff;
	}
	
	p=&OS_Tcb[Pri];								//��ʼ����Ӧ�Ŀ��ƿ�
#ifdef OSTimeSlices_EN								
	if(O_Par & OS_TaskOpt_TimeSlices)			//���8λѡ������
	{											//ʱ��Ƭ��������
		p->TimeSlices=O_Par & OS_TaskPar_Mask;	//ʱ��Ƭ����
	}
#endif
	
	p->Stk=OSTaskStkInit(task, Stk_Top);		//��ʼ�������ջ
	p->EventCnt=0;								//�¼�����
	p->Delay=0;									//��ʱ����
	OS_SetRdy(Pri);								//�������
	
	return(OS_ERR_CREATE_TASK_OK);				//���سɹ�
}
//=============================================================================
//�������ȼ��������
void OS_SetRdy(OSPri_T pri)
{
	OSRdy_T i;
	i=mPri_Tbl(pri);	//��Ѱ������ȼ�ʹ�ü���ǰ�����ָ����������λ��־������ȼ�
	OSSys.OSRdytbl |= i;
}
//=============================================================================
//����Ӧ���ȼ�������
void OS_ClrRdy(OSPri_T pri)
{
	OSRdy_T i;
	i=~mPri_Tbl(pri);	//��Ѱ������ȼ�ʹ�ü���ǰ�����ָ����������λ��־������ȼ�
	OSSys.OSRdytbl &= i;
#ifdef OSCHK_RunTime_EN
	OS_Tcb[pri].RunTime=0;	//�Ӿ����������ͬʱ������Ч��������ʱ��
#endif
}
//=============================================================================
#ifdef OS_ChkLowRdy_EN
	//����Ƿ���ڵ����ȼ�������������
	//���رȵ�ǰ�������ȼ��͵ľ�����Idle����λ����
	OSRdy_T OS_ChkLowRdy(void)
	{
		OSRdy_T i;
		
		i = ((OSRdy_T)(-1) - OSRDY_H) << (OSSys.OSPrioCur);			//0xfffffffe << (OSSys.OSPrioCur);
		i &= ~(mPri_Tbl(OS_IDLE_PRI));								//ȥ��Idle����
		i &= OSSys.OSRdytbl;
		return(i);
	}
#endif
//=============================================================================
//��ʼ����ջ
//�쳣����ʱ��CPU�Զ����ΰ�xPSR, PC, LR, R12�Լ�R3�\R0��Ӳ���Զ�ѹ���ʵ��Ķ�ջ��
//ģ����������Ѿ���������,ѹ��16���Ĵ���
#define CCOS_STK_INIT_DATA

OSStk_T *OSTaskStkInit (void(*task)(void), OSStk_T  *Stk_Top)
{
    OSStk_T *stk;
                                                                        
    stk = Stk_Top;                          //  װ��ջ��
	
#if defined(MCU_CORE_M0)
	*(stk)  = (OSStk_T)0x01000000L;           /* xPSR                                                   */
    *(--stk)  = (OSStk_T)task;                  /* Entry Point (PC)                                       */
	#ifdef CCOS_STK_INIT_DATA
		*(--stk)  = (OSStk_T)0xefefefefL;           /* LR                                                     */
		*(--stk)  = (OSStk_T)0x12121212L;           /* R12                                                    */
		*(--stk)  = (OSStk_T)0x03030303L;           /* R3                                                     */
		*(--stk)  = (OSStk_T)0x02020202L;           /* R2                                                     */
		*(--stk)  = (OSStk_T)0x01010101L;           /* R1                                                     */
		*(--stk)  = (OSStk_T)0x00000000L;           /* R0 : argument                                          */

												   /* Remaining registers saved on process stack             */
		*(--stk)  = (OSStk_T)0x07070707L;           /* R7                                                     */
		*(--stk)  = (OSStk_T)0x06060606L;           /* R6                                                     */
		*(--stk)  = (OSStk_T)0x05050505L;           /* R5                                                     */
		*(--stk)  = (OSStk_T)0x04040404L;           /* R4                                                     */
		*(--stk)  = (OSStk_T)0x11111111L;           /* R11                                                    */
		*(--stk)  = (OSStk_T)0x10101010L;           /* R10                                                    */
		*(--stk)  = (OSStk_T)0x09090909L;           /* R9                                                     */
		*(--stk)  = (OSStk_T)0x08080808L;           /* R8                                                     */
	#else
		stk -= 14;
	#endif
#endif

#if defined(MCU_CORE_M3) || defined(MCU_CORE_M4)

	#ifdef OS_CPU_FPU_USED
	  #ifdef CCOS_STK_INIT_DATA
		*(stk) = (unsigned int)0x00000000L; //No Name Register  
		*(--stk) = (unsigned int)0x00001000L; //FPSCR
		*(--stk) = (unsigned int)0x00000015L; //s15
		*(--stk) = (unsigned int)0x00000014L; //s14
		*(--stk) = (unsigned int)0x00000013L; //s13
		*(--stk) = (unsigned int)0x00000012L; //s12
		*(--stk) = (unsigned int)0x00000011L; //s11
		*(--stk) = (unsigned int)0x00000010L; //s10
		*(--stk) = (unsigned int)0x00000009L; //s9
		*(--stk) = (unsigned int)0x00000008L; //s8
		*(--stk) = (unsigned int)0x00000007L; //s7
		*(--stk) = (unsigned int)0x00000006L; //s6
		*(--stk) = (unsigned int)0x00000005L; //s5
		*(--stk) = (unsigned int)0x00000004L; //s4
		*(--stk) = (unsigned int)0x00000003L; //s3
		*(--stk) = (unsigned int)0x00000002L; //s2
		*(--stk) = (unsigned int)0x00000001L; //s1
		*(--stk) = (unsigned int)0x00000000L; //s0
	  #else //#ifdef CCOS_STK_INIT_DATA
		stk -= (18-1);
	  #endif //#ifdef CCOS_STK_INIT_DATA
		// Registers stacked as if auto-saved on exception
		*(--stk)    = (unsigned int)0x01000000L;             // xPSR
		*(--stk)  = (unsigned int)task;                    // Entry Point 
	  #ifdef CCOS_STK_INIT_DATA	
		*(--stk)  = (unsigned int)0xFFFFFFFEL;	//OS_TaskReturn;           // R14 (LR) (init value will cause fault if ever used)
		*(--stk)  = (unsigned int)0x12121212L;          // R12
		*(--stk)  = (unsigned int)0x03030303L;          // R3
		*(--stk)  = (unsigned int)0x02020202L;          // R2
		*(--stk)  = (unsigned int)0x01010101L;          // R1
		*(--stk)  = (unsigned int)0x00000000L;			//p_arg;                   // R0 : argument

		
		*(--stk) = (unsigned int)0x00000031L; //s31
		*(--stk) = (unsigned int)0x00000030L; //s30
		*(--stk) = (unsigned int)0x00000029L; //s29
		*(--stk) = (unsigned int)0x00000028L; //s28
		*(--stk) = (unsigned int)0x00000027L; //s27
		*(--stk) = (unsigned int)0x00000026L; //s26	
		*(--stk) = (unsigned int)0x00000025L; //s25
		*(--stk) = (unsigned int)0x00000024L; //s24
		*(--stk) = (unsigned int)0x00000023L; //s23
		*(--stk) = (unsigned int)0x00000022L; //s22
		*(--stk) = (unsigned int)0x00000021L; //s21
		*(--stk) = (unsigned int)0x00000020L; //s20
		*(--stk) = (unsigned int)0x00000019L; //s19
		*(--stk) = (unsigned int)0x00000018L; //s18
		*(--stk) = (unsigned int)0x00000017L; //s17
		*(--stk) = (unsigned int)0x00000016L; //s16
			
		// Remaining registers saved on process stack
		*(--stk)  = (unsigned int)0x11111111L;             // R11
		*(--stk)  = (unsigned int)0x10101010L;             // R10
		*(--stk)  = (unsigned int)0x09090909L;             // R9
		*(--stk)  = (unsigned int)0x08080808L;             // R8
		*(--stk)  = (unsigned int)0x07070707L;             // R7
		*(--stk)  = (unsigned int)0x06060606L;             // R6
		*(--stk)  = (unsigned int)0x05050505L;             // R5
		*(--stk)  = (unsigned int)0x04040404L;             // R4
	  #else //#ifdef CCOS_STK_INIT_DATA	
		stk -= (14+16);	//S16-S31,R0-R12��R14��LR������һ�β��ع���ѹջ����
	  #endif //#ifdef CCOS_STK_INIT_DATA	
	#else  //#ifdef OS_CPU_FPU_USED

		//  ģ����쳣���Զ��ѼĴ���ѹջ
		*(stk)   = (OSStk_T)0x01000000L;     //  xPSR                        */ 
		*(--stk) = (OSStk_T)task;            //�ָ���PCֵ���״ξ����������
		
	  #ifdef CCOS_STK_INIT_DATA
		*(--stk) = (OSStk_T)0xFFFFFFFEL;     //  R14 (LR) ���ʹ�þͻ����fault�쳣
																		   
		*(--stk) = (OSStk_T)0x12121212L;     //R12
		*(--stk) = (OSStk_T)0x03030303L;     //R3
		*(--stk) = (OSStk_T)0x02020202L;     //R2
		*(--stk) = (OSStk_T)0x01010101L;     //R1
		*(--stk) = (OSStk_T)0;               //R0 : argument  �������
											 //Remaining registers saved on
											 //process stack
											 //ʣ�µļĴ������浽��ջ

		*(--stk) = (OSStk_T)0x11111111L;     //R11
		*(--stk) = (OSStk_T)0x10101010L;     //R10
		*(--stk) = (OSStk_T)0x09090909L;     //R9
		*(--stk) = (OSStk_T)0x08080808L;     //R8
		*(--stk) = (OSStk_T)0x07070707L;     //R7
		*(--stk) = (OSStk_T)0x06060606L;     //R6
		*(--stk) = (OSStk_T)0x05050505L;     //R5
		*(--stk) = (OSStk_T)0x04040404L;     //R4
	  #else
		stk -= (14);		//R0-R12��R14��LR������һ�β��ع���ѹջ����
	  #endif
	#endif //#ifdef OS_CPU_FPU_USED
#endif	//#if defined(MCU_CORE_M3) || defined(MCU_CORE_M4)


	
	return(stk);
}
//=============================================================================
//���������л�
void  OSSchedLock (void)
{
	if(OSSys.OSLockNesting<OSLockNesting_MAX)                 //Ƕ�׼���	
	{
		OSSys.OSLockNesting++;
	}
}
//=============================================================================
//���������л�
void  OSSchedUnlock (void)
{
	if(OSSys.OSLockNesting !=0)
	{
		OSSys.OSLockNesting--;
	}
	if(OSSys.OSLockNesting==0)
	{
		OS_Sched();	//���������л������������µĸ����ȼ�����
	}
}
//=============================================================================
void  OSTimeDly (OSTick_T Ticks)
{
	struct OS_TCB *p;
#ifdef OSTimeSlices_EN
	OSDelay_VT *vp;
#endif
	
    if (Ticks > 0) 
	{
        p=&OS_Tcb[OSSys.OSPrioCur];
		OS_ENTER_CRI_S();
		p->Delay=Ticks;			//����ʱ��
		
	#ifdef OSTimeSlices_EN
		if(OSSys.OSPrioCur < OSSys.OSTimeSlices_Pri)
		{								//����ʽ����
			OS_ClrRdy(OSSys.OSPrioCur);	//�Ӿ�����ȥ��
			OS_EXIT_CRI_S();
			OS_Sched();
		}
		else
		{								//ʱ��Ƭ����
			OS_EXIT_CRI_S();			//����NOP��CPSIE I�ᱻ�����Ż���
			vp=&p->Delay;				//������CPSIE IҲ���ᱻ�����Ż���
			while(*vp);					//��ѭ���ȴ�
		}		
	#else
		OS_ClrRdy(OSSys.OSPrioCur);	//�Ӿ�����ȥ��
        OS_EXIT_CRI_S();
        OS_Sched();             	//�л�����
	#endif
		
		
    }
}
//=============================================================================
//1mSʱ�ӵδ�����ÿ�����ƿ����ʱ���ʱ���ж��Ƿ����
//���ж��������
void  OSTimeTick_Int (void)
{
	int i;
	struct OS_TCB *p;

    if (OSSys.OSRunning) 
	{
		p=OS_Tcb;
		//===========================================================================================
		//����ʽ����ʱ���жϿ�ʼ
	#ifdef OSTimeSlices_EN
		for(i=0;i<OSSys.OSTimeSlices_Pri;i++)
	#else
		for(i=0;i<OS_TASK_MAX;i++)
	#endif
		{
			OS_I_DisAll();		//���жϳ������棬�߽�����϶�=0�������ñ߽���뺯���Լ��ٹ��ж�ʱ��
			if(p->Tout != 0)
			{
				p->Tout--;
			}
			if(p->Delay != 0)
			{
				if(--p->Delay==0)
				{						//��ʱʱ�䵽���þ�����
					p->Event_trig=OS_TIMEOUT;
					OS_SetRdy(i);
				}
			}
		//-----------------------------------------
		#ifdef OSCHK_RunTime_EN
			else if(i==OSSys.OSPrioCur)
			{							//��ǰ�����������У�����ʱ��+1
				if(p->RunTime < RunTime_MAX)
				{
					p->RunTime++;
				//---------------------------------
				#ifdef OSCHK_RunTimeMax_EN
					if(p->RunTime > p->RunTime_Max)
					{
						p->RunTime_Max=p->RunTime;
					}
				#endif
				//---------------------------------
				}
			}
		#endif
		//-----------------------------------------
			OS_I_EnAll();		//���жϳ������棬�߽�����϶�=0�������ñ߽���뺯���Լ��ٹ��ж�ʱ��
			p++;
		}
	//����ʽ����ʱ���жϽ���
	//===========================================================================================
	
	//===========================================================================================
	//ʱ��Ƭ��������ʱ���жϿ�ʼ
	#ifdef OSTimeSlices_EN
		for(;i<OS_TASK_MAX;i++)
		{
			OS_I_DisAll();		//���жϳ������棬�߽�����϶�=0�������ñ߽���뺯���Լ��ٹ��ж�ʱ��
			if(p->Delay != 0)
			{
				p->Delay--;
			}
			
			if(i==OSSys.OSPrioCur)
			{							//��ǰ�����������У�����ʱ��+1
				if(p->RunTime < RunTime_MAX)
				{
					p->RunTime++;
				//---------------------------------
				#ifdef OSCHK_RunTimeMax_EN
					if(p->RunTime > p->RunTime_Max)
					{
						p->RunTime_Max=p->RunTime;
					}
				#endif
				//---------------------------------	
					if(p->RunTime >= p->TimeSlices)	//��ǰ��Ч����ʱ�䳬��Ԥ��ʱ��Ƭ�¼����л�����һ��ʱ��Ƭ
					{
						OS_ClrRdy(i);							//�����ǰ�ľ���״̬
						if(++OSSys.OSTimeSlices_PriCur >= OS_TASK_MAX)
						{										//ʱ��Ƭ���������ѭ��+1
							OSSys.OSTimeSlices_PriCur = OSSys.OSTimeSlices_Pri;
						}
						OS_SetRdy(OSSys.OSTimeSlices_PriCur);	//��һ��ʱ��Ƭ�������
					}
				}
			}
	
	
			OS_I_EnAll();		//���жϳ������棬�߽�����϶�=0�������ñ߽���뺯���Լ��ٹ��ж�ʱ��
			p++;
		}
	//ʱ��Ƭ��������ʱ���жϽ���
	//===========================================================================================
	#endif
    }
}
//=============================================================================
//�����Ӧ���ȼ����¼�
void OSEventPost (OSPri_T pri)
{
	struct OS_TCB *p;
	int i=0;						//V2.03
	
	
	OS_ENTER_CRI_S();
	
	p=&OS_Tcb[pri];
	
	if(p->Opt & OSTcbOpt_WEvent)	//2.01 ���ӵȴ�״̬�Ա�����������ʱ�б�ʱ����
	{
		p->Opt &= ~OSTcbOpt_WEvent;
	#ifdef OSTimeSlices_EN
		if(pri < OSSys.OSTimeSlices_Pri)
		{
			OS_SetRdy(pri);			//����ʱ��Ƭ��������Ӧ�������
		}
	#else
		OS_SetRdy(pri);				//����Ӧ�������
	#endif
		
		
		p->Delay=0;					//��ʱ������
		p->Event_trig=OS_NML;		//�¼���ʽ����
		i=1;						//V2.03
	}
#ifdef OSTimeSlices_EN
	if(i==0 || pri > OSSys.OSPrioHighRdy || pri==OSSys.OSPrioCur || pri >= OSSys.OSTimeSlices_Pri)	//��ǰ���ȼ����¼���������ȼ��ߣ�
		//�������ڵ�ǰ�����ȼ����У���������ʱ��Ƭ���������¼�+1
#else
	if(i==0 || pri>OSSys.OSPrioHighRdy || pri==OSSys.OSPrioCur)	//��ǰ���ȼ����¼���������ȼ��ߣ��������ڵ�ǰ�����ȼ����У��¼�+1
#endif
	{									
		if(p->EventCnt < OSEvent_MAX)
		{
			p->EventCnt++;
		#ifdef OS_CHK_Event_Max_EN
			if(p->EventCnt>p->EventCnt_Max)
			{
				p->EventCnt_Max=p->EventCnt;
			}
		#endif
		}	               
	}
	OS_EXIT_CRI_S();
	
	OS_Sched();						//�л�����
}
//=============================================================================
//�ȴ��¼��������Լ�,�����趨��ʱʱ��
OSErr_T  OSEventPend (OSTick_T timeout)
{
	struct OS_TCB *p;
#ifdef OSTimeSlices_EN
	OSDelay_VT *vp_delay;
	OSEventCnt_VT *vp_event;
#endif	
	p=&OS_Tcb[OSSys.OSPrioCur];
	
	OS_ENTER_CRI_S();
	if(p->EventCnt != 0)
	{										//�����¼�����û�д����ֱ꣬����Ӧһ���¼����¼�-1
		p->EventCnt--;
		p->Delay=0;
		p->Event_trig=OS_NML;			//V2.03
		OS_EXIT_CRI_S();
		return(OS_NML);					//�¼���ʽ����
	}
	else
	{												//û�д�������¼�
		p->Delay=timeout;							//�賬ʱʱ��
		
		p->Opt |= OSTcbOpt_WEvent;					//2.01 ���ӵȴ�״̬�Ա�����������������ʱ�б��¼����
	#ifdef OSTimeSlices_EN
		if(OSSys.OSPrioCur < OSSys.OSTimeSlices_Pri)
		{												//����ʱ��Ƭ�л�����
			OS_ClrRdy(OSSys.OSPrioCur);					//���������
			OS_EXIT_CRI_S();
			OS_Sched();									//�л�����
		}
		else
		{												//ʱ��Ƭ������������ѭ���ȴ�
			OS_EXIT_CRI_S();
			vp_event=&p->EventCnt;   
			vp_delay=&p->Delay;
			while((*vp_event==0) || (*vp_delay != 0))
			{
				;
			}
			if(p->Event_trig==OS_NML)					//�����¼�����¼���-1
			{
				if(p->EventCnt!=0)
				{
					OS_ENTER_CRI_S();
					p->EventCnt--;
					OS_EXIT_CRI_S();
				}
			}
		}
	#else
		OS_ClrRdy(OSSys.OSPrioCur);					//���������
		OS_EXIT_CRI_S();
		OS_Sched();									//�л�����
	#endif
		
		return(OS_Tcb[OSSys.OSPrioCur].Event_trig);	//����¼���Ӧ״̬
	}
}
//=============================================================================
//����¼�������
void OSClear_Event(OSPri_T Pri)
{
	OS_ENTER_CRI_S();
	OS_Tcb[OSSys.OSPrioCur].EventCnt=0;
	OS_EXIT_CRI_S();
}
//=============================================================================
//��������
void  OS_TaskIdle (void)
{
    while(1) 
	{
        OSTaskIdleHook();	//�û��Զ��庯��
    }
}
//=============================================================================
