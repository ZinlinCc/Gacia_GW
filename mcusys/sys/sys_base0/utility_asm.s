	THUMB


	;PRESERVE8
	
	
	
	; Exported functions
	EXPORT	_U32_SetB
	EXPORT	_U16_SetB
	EXPORT	_U8_SetB
		
	EXPORT	_U32_ClrB
	EXPORT	_U16_ClrB
	EXPORT	_U8_ClrB
		
	EXPORT	_U32_TogB
	EXPORT	_U16_TogB
	EXPORT	_U8_TogB
	
	EXPORT	_U32_TscB
	EXPORT	_U16_TscB
	EXPORT	_U8_TscB
	
	EXPORT	_short_xch_hl

;------------------------------------------------------------------------------
	AREA ||i.U32_SetB||, CODE, READONLY, ALIGN=1

;void set_bit_i(void *x,u8 b);

_U32_SetB PROC
		
		MOVS    R3,#1
		LSLS    R3,R3,R1
		
		MRS     R1,PRIMASK
		CPSID	I		
		LDR    	R2,[R0]
		ORRS  	R2,R2,R3
		STR  	R2,[R0]
		MSR     PRIMASK,R1
		BX    	LR
		ENDP

	AREA ||i.U16_SetB||, CODE, READONLY, ALIGN=1

_U16_SetB PROC
		MOVS    R3,#1
		LSLS    R3,R3,R1
		
		MRS     R1,PRIMASK
		CPSID	I
		
		LDRH   	R2,[R0]
		ORRS  	R2,R2,R3
		STRH  	R2,[R0]
		MSR     PRIMASK,R1
		BX    	LR
		ENDP
			
	AREA ||i.U8_SetB||, CODE, READONLY, ALIGN=1
		
_U8_SetB PROC
		MOVS    R3,#1
		LSLS    R3,R3,R1
		
		MRS     R1,PRIMASK
		CPSID	I
		
		LDRB   	R2,[R0]
		ORRS  	R2,R2,R3
		STRB  	R2,[R0]
		MSR     PRIMASK,R1
		BX    	LR
		
        ENDP
			
;------------------------------------------------------------------------------
		AREA ||i.U32_ClrB||, CODE, READONLY, ALIGN=1
;void clr_bit_i(void *x,u8 b);

_U32_ClrB PROC
		MOVS    R3,#1
		LSLS	R3,R3,R1
U32_ClrB_COMM
		MRS     R1,PRIMASK
		CPSID	I
		
		LDR     R2,[R0]
		BICS    R2,R2,R3
		STR     R2,[R0]
		MSR     PRIMASK,R1
		BX      LR
		ENDP
			
		AREA ||i.U16_ClrB||, CODE, READONLY, ALIGN=1

_U16_ClrB PROC
		MOVS    R3,#1
		LSLS		R3,R3,R1
U16_ClrB_COMM
		MRS     R1,PRIMASK
		CPSID	I
		
		LDRH    R2,[R0]
		BICS    R2,R2,R3
		STRH    R2,[R0]
		MSR     PRIMASK,R1
		BX      LR
		ENDP

	AREA ||i.U8_ClrB||, CODE, READONLY, ALIGN=1

_U8_ClrB PROC
		MOVS    R3,#1
		LSLS	R3,R3,R1
U8_ClrB_COMM
		MRS     R1,PRIMASK
		CPSID	I
		
		LDRB    R2,[R0]
		BICS    R2,R2,R3
		STRB    R2,[R0]
		MSR     PRIMASK,R1
		BX      LR

        ENDP
;------------------------------------------------------------------------------
		AREA ||i.U32_CplB||, CODE, READONLY, ALIGN=1

;void cpl_bit_i(void *x,u8 b);
_U32_TogB PROC
		MOVS	R3,#1
		LSLS 	R3,R3,R1
		MRS     R1,PRIMASK
		CPSID	I
		LDR  	R2,[R0]
		EORS 	R2,R2,R3
		STR  	R2,[R0]
		MSR     PRIMASK,R1
		BX   	LR
		ENDP
			
	AREA ||i.U16_CplB||, CODE, READONLY, ALIGN=1

_U16_TogB PROC
		MOVS	R3,#1
		LSLS 	R3,R3,R1
		MRS     R1,PRIMASK
		CPSID	I
		LDRH  	R2,[R0]
		EORS 	R2,R2,R3
		STRH  	R2,[R0]
		MSR     PRIMASK,R1
		BX   	LR
		ENDP
			
	AREA ||i.U8_CplB||, CODE, READONLY, ALIGN=1

_U8_TogB PROC
		MOVS	R3,#1
		LSLS 	R3,R3,R1
		MRS     R1,PRIMASK
		CPSID	I
		LDRB  	R2,[R0]
		EORS 	R2,R2,R3
		STRB  	R2,[R0]
		MSR     PRIMASK,R1
		BX   	LR
        ENDP
;------------------------------------------------------------------------------

;	AREA ||i.tst_bit||, CODE, READONLY, ALIGN=1
;
;tst_bit PROC
;		LDR     R0,[R0]
;		MOV	    R3,#1
;		LSL	    R3,R3,R1
;		AND	    R0,R0,R3
;		CBZ		R0,tst_bit_ret
;		MOV		R0,#1
;tst_bit_ret
;		BX LR
		
;		ENDP
;------------------------------------------------------------------------------			
			
	AREA ||i.U32_TscB||, CODE, READONLY, ALIGN=1
		

_U32_TscB	PROC
		MOVS	R3,#1
		LSLS	R3,R3,R1
		
		MOV		R2,R0
		LDR		R0,[R2]
		ANDS	R0,R0,R3
		BEQ		TSC_RET	;如果是0直接返回
		
		MOV		R0,R2
		PUSH	{LR}
		BL		U32_ClrB_COMM
		MOVS	R0,#1
		POP		{PC}
		ENDP
		
		AREA ||i.U16_TscB||, CODE, READONLY, ALIGN=1
		
_U16_TscB 	PROC
		MOVS	R3,#1
		LSLS	R3,R3,R1
		
		MOV		R2,R0
		LDR		R0,[R2]
		ANDS	R0,R0,R3
		BEQ		TSC_RET	;如果是0直接返回
		
		MOV		R0,R2
		PUSH	{LR}
		BL		U16_ClrB_COMM
		MOVS	R0,#1
		POP		{PC}
		ENDP
			
	AREA ||i.U8_TscB||, CODE, READONLY, ALIGN=1
_U8_TscB	PROC
		MOVS	R3,#1
		LSLS	R3,R3,R1
		
		MOV		R2,R0
		LDRB	R0,[R2]
		ANDS	R0,R0,R3
		BEQ		TSC_RET	;如果是0直接返回
		
		MOV		R0,R2
		PUSH	{LR}
		BL		U8_ClrB_COMM
		MOVS	R0,#1
		POP		{PC}
TSC_RET
		BX	LR
		
		ENDP

;------------------------------------------------------------------------------
		AREA ||i.short_xch_hl||, CODE, READONLY, ALIGN=1

_short_xch_hl PROC
		REV16 r0, r0
		BX lr
		
		ENDP
;-------------------------------------------------------------------------------
		END
			