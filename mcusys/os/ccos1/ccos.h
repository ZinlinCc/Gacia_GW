/******************************************************************************
CCOS��һ������Ƕ��ʽ����ϵͳ���������ȼ�����ʽ����ϵͳ
CCOS 2.00�汾 
ʹ��ʱ�û��ļ�ֻ�����CCOS.H����CCOS_CFG.H����������Ӧ�����ã��ڹ����������CCOS.C,CCOS_CPU.S����


�ļ�:CCOS.C,CCOS.H,CCOS_CPU.S,CCOS_CFG.H


�������ܣ�
1.���Դ���1-32�����ȼ�����
2.���д���ʱ���¼���������
3.������ʱ����
4.���������л���������������
5.���в�ѯ�Ƿ��е����ȼ�����������ܣ���ѡ��
6.���м�����Ч��������ʱ�䣬�����ֵ�Ĺ��ܣ���ѡ��
7.���м����¼�����������������ܣ���ѡ��
V2.00
8.����ʱ��Ƭ���������ܣ���ѡ��
V2.01
���ӿ����ֵ�ѡ���ֽڣ�������ȴ�ʱ��״̬��������������ʱ�б��¼����
V2.02
����OS_ETE,OS_ETT����
V2.03
���ƿ����ӳ�ʱ��ʱ��
V2.04
���ӵ���������
V2.05
OSStk_T ����8�ֽڶ���
V2.06
����M4����������µĹ���
******************************************************************************/
#ifndef __CCOS_H__
#define __CCOS_H__

//----------------------------------------------------------------------
//�汾�Ź���
#define CCOS_VER_H 204
//----------------------------------------------------------------------

#ifdef   CCOS_GLOBALS
	#define  OS_EXT
#else
	#define  OS_EXT  extern
#endif
#include "CCOS_CFG.H"
/*****************************************************************************/
//����Ԥ���巽�㲻ͬ��CPUϵͳ����ֲ
typedef __align(8) unsigned int   	OSStk_T;		//��ջ���� V2.05��ջ�������8�ֽڶ��룬������ЩC�Ŀ�ִ�в�����
typedef unsigned int	OSStk_Size_T;	//��ջ��С����
typedef unsigned int	OSRdy_T;		//����������

typedef unsigned short	OSTick_T;		//�δ�ʱ������
typedef unsigned short	OSDelay_T;		//�δ�ʱ������,��������Ϊ����ʱ��Ƭ�����ж��¼�ʱ���ᱻ�������Ż���
typedef volatile unsigned short	OSDelay_VT;		//�δ�ʱ������,��������Ϊ����ʱ��Ƭ�����ж��¼�ʱ���ᱻ�������Ż���


//---------------------
typedef unsigned char	OSRunTime_T;	//��Ч��������ʱ������
#define RunTime_MAX	255
//---------------------
typedef unsigned char	OSRun_T;		//���б�־
typedef unsigned char	OSPri_T;		//���ȼ�������
	
typedef unsigned short	OSSysOpt_Par_T;	//ϵͳѡ������ ѡ��+����
typedef unsigned int	OSTaskOpt_Par_T;	//����ѡ������ ѡ��+����	

typedef unsigned char	OSIntNest_T;	//�ж�Ƕ����

typedef unsigned char	OSCriticalNest_T;	//����߽�Ƕ������

typedef unsigned char	OSLockNest_T;	//��������Ƕ������

typedef unsigned char	OSErr_T;		//��������

typedef unsigned char	OSTcbOpt_T;		//���ƿ�ѡ��

//---------------------
typedef unsigned char	OSEventCnt_T;	//���ƿ��¼���������,��������Ϊ����ʱ��Ƭ�����ж��¼�ʱ���ᱻ�������Ż���
typedef volatile unsigned char	OSEventCnt_VT;	//���ƿ��¼���������,��������Ϊ����ʱ��Ƭ�����ж��¼�ʱ���ᱻ�������Ż���
#define OSEvent_MAX 255
//---------------------
typedef unsigned char	OSEventTrig_T;	//���ƿ��¼���������
typedef unsigned char	OSTcb_Len_T;	//���ƿ��¼���������
typedef unsigned char	OSTask_Max_T;	//���ƿ��¼���������
#define OSRDY_H		((OSRdy_T)0x01)		//�������������ȼ�
#define mPri_Tbl(A)	(OSRDY_H << A)
/*****************************************************************************/

#ifdef OSTimeSlices_EN
	#ifndef OSCHK_RunTime_EN
		#define OSCHK_RunTime_EN		//���ʱ��Ƭ����ʹ�ܣ����������Ч��������ʱ�����ʹ��
	#endif
#endif

#ifdef OSCHK_RunTimeMax_EN
	#ifndef OSCHK_RunTime_EN
		#define OSCHK_RunTime_EN		//������������Ч��������ʱ�����ֵʹ�ܣ����������Ч��������ʱ�����ʹ��
	#endif
#endif

/*****************************************************************************/
#define OS_IDLE_PRI OS_TASK_MAX	//�������ž���Idle����
//-----------------------------------------------------------------------------
#define OS_TRUE 1							//����
#define OS_FALSE 0							//ֹͣ

#define OS_NML 0							//OSEventPend�����¼�����
#define OS_TIMEOUT 1						//OSEventPend������ʱ����

#define OS_ERR_CREATE_TASK_OK  	0			//��������ɹ�
#define OS_ERR_CREATE_TASK_MAX 	1			//�������������
#define OS_ERR_CREATE_TASK_PRI 	2			//���ȼ���ͻ
//=============================================================================

//��������ѡ���
#define OS_TaskOpt_LSL	8
#define OS_TaskPar_Mask	0xff
#define OS_TaskNoPar 0
#define OS_TaskOpt_TimeSlices		(0x01 << OS_TaskOpt_LSL)		//ʱ��Ƭ��������ѡ���ʾ���Ǹ�ʱ��Ƭ���ȵ�����

//ϵͳ����ȫ�ֱ���
//-----------------------------------------------------------------------------
OS_EXT  OSStk_T	OSTaskIdleStk[OS_TASK_IDLE_STK_SIZE];	//��̨�����ջ����
//-----------------------------------------------------------------------------

#define OSTcbOpt_WEvent 0x01		//���ڵȴ��¼�

//���ƿ�ṹ��
struct OS_TCB {
	OSStk_T *Stk;					//4	#0	��ջָ��
	
	OSDelay_T Delay;				//2	�ӳ�ʱ��
	OSRunTime_T RunTime;			//1	��Ч��������ʱ�䣬�Ӿ�����ɾ��ʱ����
	OSRunTime_T RunTime_Max;		//1	��Ч��������ʱ�����ֵ
	
	OSTcbOpt_T Opt;					//1 ѡ��
	OSEventCnt_T EventCnt;			//1	�¼�������
	OSEventTrig_T Event_trig;		//1	�¼����񴥷�ԭ��	
	OSEventCnt_T EventCnt_Max;		//1	�¼����������ֵ
	
	OSDelay_T Tout;					//2	��������ʱ����	V2.03
	
#ifdef OSTimeSlices_EN
	OSRunTime_T TimeSlices;			//1 ʱ��Ƭ����ʱ��ռ�õ�ʱ��Ƭ
	unsigned char rsv0;
	unsigned char rsv1;
	unsigned char rsv2;
#endif
};
OS_EXT struct OS_TCB OS_Tcb[OS_TASK_MAX+1];	//������ƿ�,���һ��Ϊ��̨IDLE������ƿ�

#define OSLockNesting_MAX 255

//ϵͳ����
struct OS_Sys{
	OSRdy_T OSRdytbl;					//4	#0	������
	
	OSPri_T OSPrioCur;					//1	#4	��ǰ���ȼ�
	OSPri_T OSPrioHighRdy;				//1	#5	���ݾ�������������������ȼ�
	OSCriticalNest_T OSCriNesting;		//1	#6	����߽����
	OSIntNest_T OSIntNesting;			//1	#7	�ж�Ƕ����
	
	OSLockNest_T OSLockNesting;			//1	#8	��������Ƕ��������ʱȡ0��1
	OSRun_T OSRunning;					//1	#9	����ָʾ
	OSTcb_Len_T TCB_Len;				//1	#10	���ƿ�ṹ��Ĵ�С
	unsigned char rsv0;					//1 #11 ��ʱԤ��
	
	//���±������ڻ���ļ���ʹ�ã�����Ҫ�ڻ���ļ���������
#ifdef OSTimeSlices_EN
	OSRdy_T OSTimeSlices_Tbl;			//ʱ��Ƭ�����
	OSPri_T OSTimeSlices_Pri;			//ʱ��Ƭ����ǰ����ִ�е�����
	OSPri_T OSTimeSlices_PriCur;		//ʱ��Ƭ�����е�ǰ���������
#endif
};

OS_EXT struct OS_Sys OSSys;

//=============================================================================
//�ⲿ���ú���
extern void OSInit(void);		//����ϵͳ��ʼ��
//��������,����������ָ�룬��ջ����ָ�룬���ȼ���ѡ��+����
extern OSErr_T OSTaskCreate (void (*task)(void), OSPri_T Pri, OSStk_T *Stk_Top, OSStk_Size_T stk_init_size, OSTaskOpt_Par_T O_Par); 
extern void OSStartTask(void);			//��������ϵͳ
//-----------------------------------------------------------------------------
extern OSErr_T OSEventPend (OSTick_T timeout);	//�ȴ��¼�������
extern void OSEventPost (OSPri_T ev);				//�����¼������ж������л�
extern void OSClear_Event(OSPri_T Pri);		//����¼�������
#define OSClear_MyEvent() OSClear_Event(OSSys.OSPrioCur)
extern void OSTimeDly (OSTick_T ticks);			//��ʱ����

#define OS_ETE (OS_Tcb[OSSys.OSPrioCur].Event_trig==OS_NML)		//�����¼�����
#define OS_ETT (OS_Tcb[OSSys.OSPrioCur].Event_trig==OS_TIMEOUT)	//��ʱ����
//-----------------------------------------------------------------------------
#define OS_TOUT (OS_Tcb[OSSys.OSPrioCur].Tout==0)		//����ʱ����������	V2.03
#define OS_SetTout(A) OS_Tcb[OSSys.OSPrioCur].Tout=A	//������ʱ������	V2.03
#define OS_ClrTout() OS_Tcb[OSSys.OSPrioCur].Tout=0	//������ʱ������	V2.03
//-----------------------------------------------------------------------------
//�û�ʹ�����жϹ��õĿ�����ʱ������ʹ�����º����������������жϣ���������ɶ�ʹ��
//�������ٽ���߽籣���Ժ�Ĵ��룬�Լ����жϱ��ӳٵ�ʱ��
//���ֻ��ʹ�������Ŀ�����������ʹ��OSSchedLock()��OSSchedUnlock()������
//����ֻ�������л����ܱ��ӳ٣�����Ӱ���ж�
extern void OS_ENTER_CRITICAL(void);	//����߽磬����Ƕ�׹���
extern void OS_EXIT_CRITICAL(void);		//�˳��߽磬����Ƕ�׹���
extern void OSSchedLock (void);			//��������
extern void OSSchedUnlock (void);		//���������һ�
//-----------------------------------------------------------------------------
#ifdef OS_ChkLowRdy_EN
	OSRdy_T OS_ChkLowRdy(void);				//����Ƿ���ڵ����ȼ�������������
#endif
//-----------------------------------------------------------------------------
//�жϳ������
extern void  OSTimeTick_Int (void);		//1mS�δ���
extern void  OSIntEnter (void);			//�����жϴ���
extern void  OSIntExit (void);			//�˳��жϴ������ж������л�
//-----------------------------------------------------------------------------
//�ڲ�����
//���ڳ�����û��Ƕ��ʹ��OS_ENTER_CRITICAL��OS_EXIT_CRITICAL����˿��Լ򵥶�������������
#define OS_ENTER_CRI_S() 		__asm("cpsid i")	//__asm("nop;cpsid i")	//����߽�
#define OS_EXIT_CRI_S()			__asm("cpsie i")	//__asm("cpsie i;nop")	//�˳��߽�,����NOP�������Щʱ��ᱻ�Ż�û��
#define OS_I_DisAll()			__asm("cpsid i")	//__asm("nop;cpsid i")
#define OS_I_EnAll()			__asm("cpsie i")	//__asm("cpsie i;nop")

extern OSStk_T *OSTaskStkInit (void(*task)(void), OSStk_T  *ptos);	//��ջ��ʼ��

extern void OS_SetRdy(OSPri_T pri);		//���������������
extern void OS_ClrRdy(OSPri_T pri);		//���������������
extern void OS_Sched (void);			//�����л�

extern void OS_TaskIdle (void);			//ϵͳ��̨����
/*************************************************************************************/
#endif


