;******************************** Copyright (c) ***************************************
;COS_VER_ASM_204
;**************************************************************************************
		IMPORT  OSSys				;ϵͳ������
		IMPORT	OS_Tcb				;������ƿ�
;--------------------------------------------------
		EXPORT	OS_ENTER_CRITICAL
		EXPORT	OS_EXIT_CRITICAL
			
		EXPORT	OSIntEnter
		EXPORT	OSIntExit
		
		EXPORT  OSStartTask
        
		EXPORT	OS_Sched
		            
        EXPORT  PendSV_Handler		;�ж����
;***************************************************************************************        	

REG_PendSV_PRI    	EQU     0xE000ED22  ; PEND���ȼ��Ĵ�����ַ���ֽ���ʽ
NVIC_PendSV_PRI 	EQU     0xFF        ; ����ж�������ȼ�

REG_ICSR		   	EQU     0xE000ED04  ; �жϿ��ƼĴ���
PENDSVSET_BIT	  	EQU     0x10000000  ; BIT28�Ǵ�������жϵ�ֵ.

;REG_SysTick_PRI    EQU     0xE000ED23  ; SysTick���ȼ��Ĵ�����ַ���ֽ���ʽ
;NVIC_SysTick_PRI   EQU		0xFF        ; ��ͣ�0xff

;***************************************************************************************
;		ϵͳ���Ʊ�������
Rdytbl_OFS			EQU		0			;������
PrioCur_OFS			EQU		4
PrioHighRdy_OFS		EQU		5
CriNesting_OFS		EQU		6
IntNesting_OFS		EQU		7
LockNesting_OFS		EQU		8
Running_OFS			EQU		9
TCB_Len_OFS			EQU		10
;					EQU		11
;---------------------------------------------------------------------------------------
;		���ƿ��������
TCB_Stk_OFS			EQU		0
;***************************************************************************************
		PRESERVE8 
			
        THUMB 

		AREA ||i.OS_ENTER_CRITICAL||, CODE, READONLY, ALIGN=3
;/***************************************************************************************
;* ��������: OS_ENTER_CRITICAL
;*
;* ��������: �����ٽ��� 
;*            
;* ��    ��: None
;*
;* �� �� ֵ: None
;*         
;* ����  ��: �³�
;* ��  ����: 2016��10��15��
;*----------------------------------------------------------------------------------------
;*****************************************************************************************/ 

OS_ENTER_CRITICAL PROC
		      
		LDR 	R1, =OSSys	    

		CPSID   I                       	;���ж�
		
		LDRB 	R0, [R1,#CriNesting_OFS]	;OSCriNesting++
		ADD  	R0, R0, #1
		STRB 	R0, [R1,#CriNesting_OFS]	; 
		
  		BX LR
		
		ENDP
;/***************************************************************************************
;* ��������: OS_EXIT_CRITICAL
;*
;* ��������: �˳��ٽ��� 
;*            
;* ��    ��: None
;*
;* �� �� ֵ: None
;*         
;* ����  ��: �³�
;* ��  ����: 2016��10��15��
;*----------------------------------------------------------------------------------------
;*****************************************************************************************/

OS_EXIT_CRITICAL	PROC
		LDR     R1, =OSSys     
		LDRB    R0, [R1,#CriNesting_OFS]	;OSCriNesting--
		SUBS    R0, R0, #1					;�Ѿ��ı��˱�־λ
        STRB    R0, [R1,#CriNesting_OFS]	;��Ӱ���־λ
		;CMP     R0,  #0			        	; if OSCriNesting=0,enable                                 
		MSREQ  PRIMASK, R0					;���CriNesting=0��ִ��PRIMASK=0�൱��"CPSIE	I"
	    
		BX LR
		NOP
		ENDP

		AREA    |.text|, CODE, READONLY
;/**************************************************************************************
;//�����жϣ�Ƕ�׼�1


OSIntEnter	PROC
		;PUSH	{R0-R2}						;�����жϳ���R0-R3�Ѿ��Զ�ѹջ
		LDR     R1, =OSSys
		LDRB	R0,[R1,#Running_OFS]
		CBZ		R0,OSIntEnter_EXIT
		
		;MRS     R2,PRIMASK					;�����ж�����
		CPSID	I							;�����е�����жϿ϶��ǿ��ģ����ж�
		
		LDRB	R0,[R1,#IntNesting_OFS]
		ADD		R0,R0,#1
		STRB	R0,[R1,#IntNesting_OFS]
		
		CPSIE	I
		;MSR     PRIMASK, R2				;�ָ��ж���������
OSIntEnter_EXIT
		;POP		{R0-R2}
		BX		LR
		
		ENDP
;/**************************************************************************************
;* ��������: OSIntExit,OS_Sched,OS_Task_Sw,OSStartTask
;*
;* ��������: �����л�         
;*
;* ��    ��: None
;*
;* �� �� ֵ: None
;*         
;* ����  ��: �³�
;* ��  ����: 2016��10��15��
;/**************************************************************************************
;����ʼ����
OSStartTask		PROC
		CPSID	I							;���ж�
		LDR		R0, =REG_PendSV_PRI     	;�� PendSV�ж� ���ȼ�
        LDR		R1, =NVIC_PendSV_PRI
        STRB	R1, [R0]
		
		LDR     R1, =OSSys
		MOV		R0,#0

        MSR     PSP, R0                 	;�� PSP = 0 �����л�����ʶ���״��л�
        STRB	R0,[R1,#IntNesting_OFS]		;OSIntNesting=0
		STRB	R0,[R1,#LockNesting_OFS]	;OSLockNesting=0;
		SUB		R0,R0,#1
		STRB	R0,[R1,#PrioCur_OFS]		;OSPrioCur=0xFF,�״��л��������ȼ���ͬЧ��
        MOV     R0, #1
        STRB    R0, [R1,#Running_OFS]		;OSRunning=1

		B		OS_Sched					;�л�����������������ȼ�
;-------------------------------------------------------------------------------
OSIntExit
		LDR     R1, =OSSys
		LDRB	R0,[R1,#Running_OFS]
		CBZ		R0,OSIntExit_EXIT
		
		CPSID	I	;�����е�����жϿ϶��ǿ��ģ����ж�
		
		LDRB	R0,[R1,#IntNesting_OFS]		;OSIntNesting--
		SUB		R0,R0,#1
		STRB	R0,[R1,#IntNesting_OFS]
		
		B		OS_Sched_Chk_IntNesting
;--------------------------------------------------------------------------------
OS_Sched
		LDR		R1,=OSSys					;ϵͳ�����ṹ��ָ�룬�����������ƫ������ȡ
		CPSID	I
		LDRB	R0,[R1,#IntNesting_OFS]
OS_Sched_Chk_IntNesting
		CBNZ	R0,OS_Sched_Exit
		
		LDRB	R0,[R1,#LockNesting_OFS]
		CBNZ	R0,OS_Sched_Exit
		;------------------------------------------------------
		LDR		R0,[R1,#Rdytbl_OFS]			;������,�������µ���߾������ȼ�
		RBIT	R0,R0						;��λ��ת
		CLZ		R0,R0						;��ǰ�������
		STRB	R0,[R1,#PrioHighRdy_OFS]	;��������ȼ���������
		;------------------------------------------------------
		LDRB	R2,[R1,#PrioCur_OFS]		;ȡ��ǰ�����������ȼ�
		CMP		R0,R2
		BEQ		OS_Sched_Exit				;�����ȣ����л�

OS_Task_Sw
        LDR     R0, =REG_ICSR		      	;����PendSV�쳣
        LDR     R1, =PENDSVSET_BIT
        STR     R1, [R0]
OS_Sched_Exit
OSIntExit_EXIT
		CPSIE	I
        BX      LR
        
		ENDP
;/**************************************************************************************
;* ��������: PendSV_Handler
;*
;* ��������: PendSV_Handler �����л��ж�
;*
;* ��    ��: None
;*
;* �� �� ֵ: None
;*         
;* ����  ��: �³�
;* ��  ����: 2016��10��15��
;*--------------------------------------------------------------------------------------
;***************************************************************************************/

;�쳣����ʱ��CPU�Զ����ΰ�xPSR, PC, LR, R12�Լ�R3�\R0��Ӳ���Զ�ѹ���ʵ��Ķ�ջ��

PendSV_Handler	PROC
		LDR		R1,=OSSys					;ϵͳ�����ṹ��ָ�룬�����������ƫ������ȡ
		LDRB	R2,[R1,#TCB_Len_OFS]		;���ƿ鳤��
		LDR		R3,=OS_Tcb					;���ƿ���ָ��

		CPSID   I							;���ж�
		
		MRS     R0, PSP                 	;PSP�ǵ�ǰ�̵߳Ķ�ջָ��
        CBZ		R0, OSPendSV_Chg_SP			;��һ�ε��ã�����ҪѹջR4-R11
;----------------------------------------
		STMDB	R0!, {R4-R11}				;��R4-R11����R0Ϊ��ַ�ĵ�Ԫ,�൱��PUSH{R4-R11}
		LDRB	R4,	[R1,#PrioCur_OFS]
		MLA		R5,R2,R4,R3					;R5 = OSPrioCur * TCB_LENGTH + OS_Tcb
        STR     R0, [R5,#TCB_Stk_OFS]   	;�����л���ȥ���̶߳�ջָ�룬OSTCBHighRdy->OSTCBStkPtr = PSP
;-------------------------------------------------------------------------------------------
OSPendSV_Chg_SP
        LDRB    R4, [R1,#PrioHighRdy_OFS] 	;R4=�µ������
		STRB	R4,	[R1,#PrioCur_OFS] 		;�Ѹ����ȼ��Ÿ�����ǰ�����ȼ��� OSPrioCur = OSPrioHighRdy
		MLA		R5,R2,R4,R3					;R5 = OSPrioCur * TCB_LENGTH + OS_Tcb
											;R5ָ�����̵߳�����
        LDR     R0, [R5,#TCB_Stk_OFS]   	;��ȡ������Ķ�ջָ��; PSP = OSTCBHighRdy->OSTCBStkPtr;
		
		LDMIA   R0!, {R4-R11}          		;�൱��POP{R4-R11}

        MSR     PSP, R0                 	;װ���µ�PSP  load PSP with new process SP
        ORR     LR, LR, #0x04           	;ָʾ�жϷ���ʹ��PSP��ջ ensure exception return uses process stack

		CPSIE	I
        BX      LR                      	;�˳��ж�,�л���������

        ENDP

		ALIGN

;****************************************************************************************
;*  End Of File                                                     
;****************************************************************************************				
        END
        
        
