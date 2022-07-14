#include "includes.h"
#include "main.h"

#define FUNC_KEY_DUMMY (void*)0xffffffff

//+++++User_Code_define Begin+++++//
	#define SW_OK_GPIO_Port GPIOC
	#define SW_OK_Pin GPIO_Pins_7

	typedef struct{
		GPIO_Type *GPIOx;				//定时器，多少时间进行一次按键进程
		uint16_t Pin;				//按键结构体
		uint8_t pol;
	}FUNC_KEY_MSP_T;


	//按键端口配置信息
	//MCUAST103键盘配置
	const FUNC_KEY_MSP_T tb_func_key_msp[]={
		{SW_OK_GPIO_Port,SW_OK_Pin,0},	//Ok
		{NULL,0}
	};


	//读取按键状态函数
	unsigned int func_key_msp_get_input(unsigned int *input)
	{
		unsigned int in=0;
		unsigned int i;
		const FUNC_KEY_MSP_T *pk;
		
		for(i=0;i<32;i++)
		{
			pk=&tb_func_key_msp[i];
			if(pk->GPIOx == NULL)
			{
				break;
			}
			if(pk->GPIOx == FUNC_KEY_DUMMY)
			{
				continue;
			}
			if(GPIO_ReadInputDataBit(pk->GPIOx, pk->Pin)==pk->pol)
			{
				in |= (1<<i);
			}
			pk++;
		}
			
		*input =in ;
				
		return i;
	}

	void func_key_msp_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
		
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_5;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
#define __FUNC_KEY_MSP
//-----User_Code_define End-----//

//----------------------------------------------------------------------------
//以下为范例程序，如果用户自定义，可以在User_Code里面加上自己的代码，
//然后#define __FUNC_KEY_MSP ,使以下默认代码无效
//----------------------------------------------------------------------------
	
#ifndef __FUNC_KEY_MSP
//=========================================================================================================
#if defined (MCU_AT32F413x) || defined (MCU_AT32F415x) || defined (MCU_AT32F403x)
	#define SW_UP_GPIO_Port GPIOC
	#define SW_UP_Pin GPIO_Pins_5
	#define SW_DOWN_GPIO_Port GPIOC
	#define SW_DOWN_Pin GPIO_Pins_10
	#define SW_LEFT_GPIO_Port GPIOC
	#define SW_LEFT_Pin GPIO_Pins_4
	#define SW_RIGHT_GPIO_Port GPIOC
	#define SW_RIGHT_Pin GPIO_Pins_11
	#define SW_CANCEL_GPIO_Port GPIOC
	#define SW_CANCEL_Pin GPIO_Pins_12
	#define SW_OK_GPIO_Port GPIOC
	#define SW_OK_Pin GPIO_Pins_13

	typedef struct{
		GPIO_Type *GPIOx;				//定时器，多少时间进行一次按键进程
		uint16_t Pin;				//按键结构体
		uint8_t pol;
	}FUNC_KEY_MSP_T;


	//按键端口配置信息
	//MCUAST103键盘配置
	const FUNC_KEY_MSP_T tb_func_key_msp[]={
		{SW_UP_GPIO_Port,SW_UP_Pin,0},	//Up
		{SW_DOWN_GPIO_Port,SW_DOWN_Pin,0},	//Down
		{SW_LEFT_GPIO_Port,SW_LEFT_Pin,0},	//Left
		{SW_RIGHT_GPIO_Port,SW_RIGHT_Pin,0},	//Right
		{SW_OK_GPIO_Port,SW_OK_Pin,0},	//Ok
		{SW_CANCEL_GPIO_Port,SW_CANCEL_Pin,0},	//Cancel
		{NULL,0}
	};


	//读取按键状态函数
	unsigned int func_key_msp_get_input(unsigned int *input)
	{
		unsigned int in=0;
		unsigned int i;
		const FUNC_KEY_MSP_T *pk;
		
		for(i=0;i<32;i++)
		{
			pk=&tb_func_key_msp[i];
			if(pk->GPIOx == NULL)
			{
				break;
			}
			if(pk->GPIOx == FUNC_KEY_DUMMY)
			{
				continue;
			}
			if(GPIO_ReadInputDataBit(pk->GPIOx, pk->Pin)==pk->pol)
			{
				in |= (1<<i);
			}
			pk++;
		}
			
		*input =in ;
				
		return i;
	}

	void func_key_msp_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
		
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_4 | GPIO_Pins_5 | GPIO_Pins_10 | GPIO_Pins_11 | GPIO_Pins_12 | GPIO_Pins_13;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
  #endif
//================================================================================================
  #ifdef MCU_AT32F421x
	#define APP_ACT_KEY_PORT GPIOB
	#define APP_ACT_KEY_Pin GPIO_Pins_4
	
	#define APP_ACT_SW_LIM0_PORT GPIOA
	#define APP_ACT_SW_LIM0_PIN GPIO_Pins_8
	
	#define APP_ACT_SW_LIM1_PORT GPIOB
	#define APP_ACT_SW_LIM1_PIN GPIO_Pins_15
	
	#define APP_ACT_HS0_PORT GPIOB
	#define APP_ACT_HS0_PIN GPIO_Pins_13

	#define APP_ACT_HS1_PORT GPIOB
	#define APP_ACT_HS1_PIN GPIO_Pins_2
	
	#define APP_ACT_LK0_PORT GPIOF
	#define APP_ACT_LK0_PIN GPIO_Pins_1

	#define APP_ACT_LK1_PORT GPIOF
	#define APP_ACT_LK1_PIN GPIO_Pins_0
	


	typedef struct{
		GPIO_Type *GPIOx;				//定时器，多少时间进行一次按键进程
		uint16_t Pin;				//按键结构体
		uint8_t pol;
	}FUNC_KEY_MSP_T;


	//按键端口配置信息
	//MCUAST103键盘配置
	const FUNC_KEY_MSP_T tb_func_key_msp[]={
		{APP_ACT_SW_LIM0_PORT,APP_ACT_SW_LIM0_PIN,0},	//Up
		{APP_ACT_SW_LIM1_PORT,APP_ACT_SW_LIM1_PIN,0},	//Down
		{APP_ACT_KEY_PORT,APP_ACT_KEY_Pin,0},	//Ok
		{APP_ACT_HS0_PORT,APP_ACT_HS0_PIN,0},	//Ok
		{APP_ACT_HS1_PORT,APP_ACT_HS1_PIN,0},	//Ok
		{APP_ACT_LK0_PORT,APP_ACT_LK0_PIN,0},	//Ok
		{APP_ACT_LK1_PORT,APP_ACT_LK1_PIN,0},	//Ok
		{NULL,0}
	};


	//读取按键状态函数
	unsigned int func_key_msp_get_input(unsigned int *input)
	{
		unsigned int in=0;
		unsigned int i;
		const FUNC_KEY_MSP_T *pk;
		
		for(i=0;i<32;i++)
		{
			pk=&tb_func_key_msp[i];
			if(pk->GPIOx == NULL)
			{
				break;
			}
			if(pk->GPIOx == FUNC_KEY_DUMMY)
			{
				continue;
			}
			if(GPIO_ReadInputDataBit(pk->GPIOx, pk->Pin)==pk->pol)
			{
				in |= (1<<i);
			}
		}
			
		*input =in ;
				
		return i;
	}

	void func_key_msp_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
		const FUNC_KEY_MSP_T *pk;
		unsigned int i;
		
		RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB | RCC_AHBPERIPH_GPIOC | RCC_AHBPERIPH_GPIOF, ENABLE);
		
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PU;
		
		
		for(i=0;i<32;i++)
		{
			pk=&tb_func_key_msp[i];
			if(pk->GPIOx == NULL)
			{
				break;
			}
			if(pk->GPIOx == FUNC_KEY_DUMMY)
			{
				continue;
			}
			GPIO_InitStructure.GPIO_Pins = pk->Pin;
			GPIO_Init(pk->GPIOx, &GPIO_InitStructure);
		}
	}
  #endif
  /*
  #ifdef MCU_AT32F421x
	#define KEY_GPIO_Port GPIOA
	#define KEY_GPIO_Pin GPIO_Pins_4

	typedef struct{
		GPIO_Type *GPIOx;				//定时器，多少时间进行一次按键进程
		uint16_t Pin;				//按键结构体
		uint8_t pol;
	}FUNC_KEY_MSP_T;


	//按键端口配置信息
	//MCUAST103键盘配置
	const FUNC_KEY_MSP_T tb_func_key_msp[]={
		{FUNC_KEY_DUMMY,0,0},	//Up
		{FUNC_KEY_DUMMY,0,0},	//Down
		{FUNC_KEY_DUMMY,0,0},	//Left
		{FUNC_KEY_DUMMY,0,0},	//Right
		{KEY_GPIO_Port,KEY_GPIO_Pin,0},	//Ok
		{FUNC_KEY_DUMMY,0,0},	//Cancel
		{NULL,0}
	};


	//读取按键状态函数
	unsigned int func_key_msp_get_input(unsigned int *input)
	{
		unsigned int in=0;
		unsigned int i;
		const FUNC_KEY_MSP_T *pk;
		
		for(i=0;i<32;i++)
		{
			pk=&tb_func_key_msp[i];
			if(pk->GPIOx == NULL)
			{
				break;
			}
			if(pk->GPIOx == FUNC_KEY_DUMMY)
			{
				continue;
			}
			if(GPIO_ReadInputDataBit(pk->GPIOx, pk->Pin)==pk->pol)
			{
				in |= (1<<i);
			}
		}
			
		*input =in ;
				
		return i;
	}

	void func_key_msp_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_AHBPERIPH_GPIOA , ENABLE);
		
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Pins = KEY_GPIO_Pin;
		GPIO_InitStructure.GPIO_Pull = GPIO_Pull_PU;
		GPIO_Init(KEY_GPIO_Port, &GPIO_InitStructure);
	}
  #endif
	
	
	
	
	#if defined (MCU_AT32F413x) || defined (MCU_AT32F415x) || defined (MCU_AT32F403x)
	#define SW1_GPIO_Port GPIOC
	#define SW1_Pin GPIO_Pins_4
	#define SW2_GPIO_Port GPIOC
	#define SW2_Pin GPIO_Pins_3
	#define SW3_GPIO_Port GPIOB
	#define SW3_Pin GPIO_Pins_2
	#define SW4_GPIO_Port GPIOC
	#define SW4_Pin GPIO_Pins_5
	#define SW5_GPIO_Port GPIOB
	#define SW5_Pin GPIO_Pins_1
	#define SW6_GPIO_Port GPIOB
	#define SW6_Pin GPIO_Pins_0

	typedef struct{
		GPIO_Type *GPIOx;				//定时器，多少时间进行一次按键进程
		uint16_t Pin;				//按键结构体
		uint8_t pol;
	}FUNC_KEY_MSP_T;


	//按键端口配置信息
	//MCUAST103键盘配置
	const FUNC_KEY_MSP_T tb_func_key_msp[]={
		{SW1_GPIO_Port,SW1_Pin,0},	//Up
		{SW4_GPIO_Port,SW4_Pin,0},	//Down
		{SW2_GPIO_Port,SW2_Pin,0},	//Left
		{SW6_GPIO_Port,SW6_Pin,0},	//Right
		{SW3_GPIO_Port,SW3_Pin,0},	//Ok
		{SW5_GPIO_Port,SW5_Pin,0},	//Cancel
		{NULL,0}
	};


	//读取按键状态函数
	unsigned int func_key_msp_get_input(unsigned int *input)
	{
		unsigned int in=0;
		unsigned int i;
		const FUNC_KEY_MSP_T *pk;
		
		for(i=0;i<32;i++)
		{
			pk=&tb_func_key_msp[i];
			if(pk->GPIOx == NULL)
			{
				break;
			}
			if(pk->GPIOx == FUNC_KEY_DUMMY)
			{
				continue;
			}
			if(GPIO_ReadInputDataBit(pk->GPIOx, pk->Pin)==pk->pol)
			{
				in |= (1<<i);
			}
			pk++;
		}
			
		*input =in ;
				
		return i;
	}

	void func_key_msp_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
		
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_0 | GPIO_Pins_1 | GPIO_Pins_2;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3 | GPIO_Pins_4 | GPIO_Pins_5;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	#endif
	*/
	//============================================================================
	#ifdef MCU_STM32F10x
		typedef struct{
			GPIO_TypeDef *GPIOx;				//定时器，多少时间进行一次按键进程
			uint16_t Pin;				//按键结构体
			uint8_t pol;
		}FUNC_KEY_MSP_T;


		//按键端口配置信息

		//默认键盘表
	//	const FUNC_KEY_MSP_T tb_func_key_msp[]={
	//		{SW0_GPIO_Port,SW0_Pin,0},	//Up
	//		{SW4_GPIO_Port,SW4_Pin,0},	//Down
	//		{SW1_GPIO_Port,SW1_Pin,0},	//Left
	//		{SW5_GPIO_Port,SW5_Pin,0},	//Right
	//		{SW3_GPIO_Port,SW3_Pin,0},	//Ok
	//		{SW2_GPIO_Port,SW2_Pin,1},	//Cancel
	//		{NULL,0}
	//	};
		//MCUAST103键盘配置
		const FUNC_KEY_MSP_T tb_func_key_msp[]={
			{SW1_GPIO_Port,SW1_Pin,0},	//Up
			{SW4_GPIO_Port,SW4_Pin,0},	//Down
			{SW2_GPIO_Port,SW2_Pin,0},	//Left
			{SW6_GPIO_Port,SW6_Pin,0},	//Right
			{SW3_GPIO_Port,SW3_Pin,0},	//Ok
			{SW5_GPIO_Port,SW5_Pin,0},	//Cancel
			{NULL,0}
		};


		//读取按键状态函数
		unsigned int func_key_msp_get_input(unsigned int *input)
		{
			unsigned int in=0;
			unsigned int i;
			const FUNC_KEY_MSP_T *pk;
			
			for(i=0;i<32;i++)
			{
				pk=&tb_func_key_msp[i];
				if(pk->GPIOx == NULL)
				{
					break;
				}
				if(pk->GPIOx == FUNC_KEY_DUMMY)
				{
					continue;
				}
				if(HAL_GPIO_ReadPin(pk->GPIOx,pk->Pin)==pk->pol)
				{
					in |= (1<<i);
				}
				pk++;
			}
				
			*input =in ;
					
			return i;
		}

		void func_key_msp_init(void)
		{
			;
		}
	#endif
#endif
//----------------------------------------------------------------------------
