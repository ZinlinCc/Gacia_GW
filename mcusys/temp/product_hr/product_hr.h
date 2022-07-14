#ifndef __PRODUCT_HR_H__
#define __PRODUCT_HR_H__


/***** 应用描述 这里输入对应用描述 *****/

#ifdef PRODUCT_HR_MAIN
	#define PRODUCT_HR_EXT 
#else
	#define PRODUCT_HR_EXT  extern
#endif

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//
	//Modbus HR变量的单位
	#define EPA_DIV 	1000
	#define U_DIV 		100
	#define I_DIV		100
	#define T_DIV		10
	#define FREQ_DIV	100
	#define P_DIV		100
	#define PF_DIV		100
	

	#define BRK_1P_HR_SVER 				0
	#define BRK_1P_HR_CTRL 				2
	#define BRK_1P_HR_HS_STA 			6
	#define BRK_1P_HR_LOCK_STA 			7
	#define BRK_1P_HR_MANUAL_OPEN 		8
	#define BRK_1P_HR_PRO		 		12
	#define BRK_1P_HR_AL		 		13
	#define BRK_1P_HR_ERR		 		14
	
	#define BRK_1P_HR_FREQ		 		17
	#define BRK_1P_HR_TB		 		18
	#define BRK_1P_HR_IL		 		19
	#define BRK_1P_HR_UAL		 		20
	#define BRK_1P_HR_UAH		 		21
	#define BRK_1P_HR_IAL		 		22
	#define BRK_1P_HR_IAH		 		23
	#define BRK_1P_HR_PAL		 		24
	#define BRK_1P_HR_PAH		 		25
	#define BRK_1P_HR_QAL		 		26
	#define BRK_1P_HR_QAH		 		27
	#define BRK_1P_HR_SAL		 		28
	#define BRK_1P_HR_SAH		 		29
	#define BRK_1P_HR_EPAL		 		30
	#define BRK_1P_HR_EPAH		 		31
	#define BRK_1P_HR_FPA		 		34
	#define BRK_1P_HR_T1		 		35
	#define BRK_1P_HR_T2		 		36
	
	#define BRK_1P_HR_SET_EN		 	60
	#define BRK_1P_HR_SET_IN		 	61
	#define BRK_1P_HR_SET_IL_EN		 	62
	#define BRK_1P_HR_SET_IL_THRE	 	63
	#define BRK_1P_HR_SET_IL_AL_THRE 	64
	#define BRK_1P_HR_SET_OC_EN		 	65
	#define BRK_1P_HR_SET_OC_TM		 	66
	#define BRK_1P_HR_SET_UPRO_EN	 	67
	#define BRK_1P_HR_SET_UPRO_TM	 	68
	#define BRK_1P_HR_SET_URST_TM	 	69
	#define BRK_1P_HR_SET_TPRO_EN	 	70
	#define BRK_1P_HR_SIZE			 	71
	
	
	
	
	
	#define BRK_2P_HR_SVER 				0
	#define BRK_2P_HR_CTRL 				2
	#define BRK_2P_HR_HS_STA 			6
	#define BRK_2P_HR_LOCK_STA 			7
	#define BRK_2P_HR_MANUAL_OPEN 		8
	#define BRK_2P_HR_PRO		 		12
	#define BRK_2P_HR_AL		 		13
	#define BRK_2P_HR_ERR		 		14
	
	#define BRK_2P_HR_FREQ		 		17
	#define BRK_2P_HR_TB		 		18
	#define BRK_2P_HR_IL		 		19
	#define BRK_2P_HR_UAL		 		20
	#define BRK_2P_HR_UAH		 		21
	#define BRK_2P_HR_IAL		 		22
	#define BRK_2P_HR_IAH		 		23
	#define BRK_2P_HR_PAL		 		24
	#define BRK_2P_HR_PAH		 		25
	#define BRK_2P_HR_QAL		 		26
	#define BRK_2P_HR_QAH		 		27
	#define BRK_2P_HR_SAL		 		28
	#define BRK_2P_HR_SAH		 		29
	#define BRK_2P_HR_EPAL		 		30
	#define BRK_2P_HR_EPAH		 		31
	#define BRK_2P_HR_FPA		 		34
	#define BRK_2P_HR_T1		 		35
	#define BRK_2P_HR_T2		 		36
	
	#define BRK_2P_HR_SET_EN		 	60
	#define BRK_2P_HR_SET_IN		 	61
	#define BRK_2P_HR_SET_IL_EN		 	62
	#define BRK_2P_HR_SET_IL_THRE	 	63
	#define BRK_2P_HR_SET_IL_AL_THRE 	64
	#define BRK_2P_HR_SET_OC_EN		 	65
	#define BRK_2P_HR_SET_OC_TM		 	66
	#define BRK_2P_HR_SET_UPRO_EN	 	67
	#define BRK_2P_HR_SET_UPRO_TM	 	68
	#define BRK_2P_HR_SET_URST_TM	 	69
	#define BRK_2P_HR_SET_TPRO_EN	 	70
	#define BRK_2P_HR_SIZE			 	71
	
//+++++User_Code_VarDef Begin+++++//
//-----User_Code_VarDef End-----//
#endif
