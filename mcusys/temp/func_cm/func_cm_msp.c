#include "func_cm.h"
#include "includes.h"
//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//

#ifndef __FUNC_CM_MSP
	
	#define CM_POW_PORT GPIOB
	#define CM_POW_PIN GPIO_Pins_2
	
	#define CM_SW_PORT GPIOC
	#define CM_SW_PIN GPIO_Pins_5
	
	#define CM_RST_PORT GPIOC
	#define CM_RST_PIN GPIO_Pins_4
	
	#define CM_VCC_PORT GPIOC
	#define CM_VCC_PIN GPIO_Pins_3


	void __func_cm_msp_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOC , ENABLE);
		
		GPIO_StructInit(&GPIO_InitStructure);
		
		//≈‰÷√4V_EN
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = CM_POW_PIN;
		GPIO_Init(CM_POW_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_OD;
		GPIO_InitStructure.GPIO_Pins = CM_SW_PIN | CM_RST_PIN;
		GPIO_Init(CM_SW_PORT, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pins = CM_VCC_PIN;
		GPIO_Init(CM_VCC_PORT, &GPIO_InitStructure);
	}

	void __func_cm_pwr_on(void)
	{
		GPIO_WriteBit(CM_POW_PORT, CM_POW_PIN, Bit_SET);
	}
	
	void __func_cm_pwr_off(void)
	{
		GPIO_WriteBit(CM_POW_PORT, CM_POW_PIN, Bit_RESET);
	}
	
	void __func_cm_sw_on(void)
	{
		//GPIO_WriteBit(CM_SW_PORT, CM_SW_PIN, Bit_SET);
		GPIO_WriteBit(CM_SW_PORT, CM_SW_PIN, Bit_RESET);
	}
	
	void __func_cm_sw_off(void)
	{
		//GPIO_WriteBit(CM_SW_PORT, CM_SW_PIN, Bit_RESET);
		GPIO_WriteBit(CM_SW_PORT, CM_SW_PIN, Bit_SET);
	}
	
	void __func_cm_rst_hi(void)
	{
		//GPIO_WriteBit(CM_RST_PORT, CM_RST_PIN, Bit_SET);
		GPIO_WriteBit(CM_RST_PORT, CM_RST_PIN, Bit_RESET);
	}
	
	void __func_cm_rst_lo(void)
	{
		//GPIO_WriteBit(CM_RST_PORT, CM_RST_PIN, Bit_RESET);
		GPIO_WriteBit(CM_RST_PORT, CM_RST_PIN, Bit_SET);
	}
	
	unsigned char __func_cm_read_vcc(void)
	{
		return(GPIO_ReadInputDataBit(CM_VCC_PORT,CM_VCC_PIN));
	}
#endif
//+++++User_Code_end Begin+++++//
//-----User_Code_end End-----//
