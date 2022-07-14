#include "includes.h"

//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//

//----------------------------------------------------------------------------
//以下为范例程序，如果用户自定义，可以在User_Code里面加上自己的代码，
//然后#define __FUNC_USART_MSP ,使以下默认代码无效
//----------------------------------------------------------------------------


#ifndef __FUNC_USART_MSP
/***************************************************************************************/
	#ifdef MCU_AT32F421x
	#define GPIO_P(A) ((GPIO_Type*)((uint32_t)(A) * 0x400 + GPIOA_BASE))

	//关联串口实例与串口指针表
	const PHUART_T g_phuart[2]={
		{USART1,USART1_IRQn,USART1},
		{USART2,USART2_IRQn,USART2}
	};

	
	void USART1_IRQHandler(void)
	{
		usart_isr(&g_USART[0]);	
	}
	void USART2_IRQHandler(void)
	{
		usart_isr(&g_USART[1]);	
	}
	
//--------------------------------------------------------------------------------------
	//配置所有的TX，RX，RS485端口
	void __usart_msp_port_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
	
		RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_GPIOA,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1, ENABLE);
		
		
		// Connect PXx to USART2_Tx 
		GPIO_PinAFConfig(GPIOA, GPIO_PinsSource2, GPIO_AF_1);
		// Connect PXx to USART2_Rx 
		GPIO_PinAFConfig(GPIOA, GPIO_PinsSource3, GPIO_AF_1);
		
		// Connect PXx to USART1_Tx 
		GPIO_PinAFConfig(GPIOA, GPIO_PinsSource9, GPIO_AF_1);
		// Connect PXx to USART1_Rx 
		GPIO_PinAFConfig(GPIOA, GPIO_PinsSource10, GPIO_AF_1);

		// Configure USART1 Tx/Rx amd USART2 Tx/Rx
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2 | GPIO_Pins_3 | GPIO_Pins_9 | GPIO_Pins_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
		GPIO_InitStructure.GPIO_OutType = GPIO_OutType_PP;
		GPIO_InitStructure.GPIO_Pull = GPIO_Pull_NOPULL;
		GPIO_Init(GPIOA, &GPIO_InitStructure);  
		
		//--------------------------------------------------------------------------------
	}
	
	void __usart_rs485_port_init(USART_T *pu)
	{
		GPIO_InitType GPIO_InitStructure;
		//配置RS485口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OutType=GPIO_OutType_PP;
		GPIO_InitStructure.GPIO_Pull=GPIO_Pull_PU;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = (1 << pu->RS485_PIN);
		GPIO_Init(GPIO_P(pu->RS485_PORT), &GPIO_InitStructure);
	}
	
	
	//--------------------------------------------------------------------------------------
	void __usart_msp_init(USART_T *pu)
	{
		USART_InitType USART_InitStruct;
		USART_Type *USARTx;
				
		USART_StructInit(&USART_InitStruct);
		USART_InitStruct.USART_BaudRate=pu->baud;
		USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
		USART_InitStruct.USART_Mode= USART_Mode_Tx | USART_Mode_Rx;
		
		if(pu->UART_Parity == USART_PARITY_E)
		{
			USART_InitStruct.USART_Parity = USART_Parity_Even;
			USART_InitStruct.USART_WordLength = USART_WordLength_9b;
		}
		else if(pu->UART_Parity == USART_PARITY_O)
		{
			USART_InitStruct.USART_Parity = USART_Parity_Odd;
			USART_InitStruct.USART_WordLength = USART_WordLength_9b;
		}
		else
		{
			USART_InitStruct.USART_Parity = USART_Parity_No;
			USART_InitStruct.USART_WordLength = USART_WordLength_8b;
		}
		
		USART_InitStruct.USART_StopBits = USART_StopBits_1;
		
		USARTx = pu->huart;
		
		USART_Init(USARTx, &USART_InitStruct);
		
		USART_INTConfig(USARTx,USART_INT_RDNE,ENABLE);
		
		USART_Cmd(USARTx,ENABLE);		
	}
			
			
	//切换RS485引脚到发送状态
	void __usart_rs485_send(USART_T *pu)
	{
		if(pu->RS485_PORT != RS485_PORT_NULL)
		{
			GPIO_WriteBit(GPIO_P(pu->RS485_PORT), (1 << pu->RS485_PIN),(BitState)(pu->RS485_POL));
			if(pu->sta & (1<<B_USART_485_DELAY))
			{
				delay_us_set_k(FUNC_USART_485_DELAY_US);
			}
		}
		
		//if(!(pu->sta & USART_CFG_FD))
		if(!(pu->sta & (1 << B_USART_FD)))
		{
			__usart_dis_receive(pu);
		}
	}

	void __usart_rs485_receive(USART_T *pu)
	{
		if(pu->RS485_PORT != RS485_PORT_NULL)
		{
			GPIO_WriteBit(GPIO_P(pu->RS485_PORT), (1 << pu->RS485_PIN),(BitState)(!pu->RS485_POL));
		}
	}

	//禁止接收使能
	void __usart_dis_receive(USART_T *pu)
	{
		USART_Type *USARTx;	
		USARTx = pu->huart;
		
		USARTx->CTRL1 &= ~USART_CTRL1_REN;
	}


	//允许接收
	void __usart_en_receive(USART_T *pu)
	{
		USART_Type *USARTx;	
		USARTx = pu->huart;
		
		USARTx->CTRL1 |= USART_CTRL1_REN;
	}

	//允许TC中断 发送
	void __usart_en_tc_it(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TRAC,ENABLE);
	}

			//禁止TC中断
	void __usart_dis_tc_it(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TRAC,DISABLE);
	}


	//允许发送缓存空中断
	void __usart_en_txe(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TDE,ENABLE);

	}

	//允许发送缓存空中断
	void __usart_dis_txe(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TDE,DISABLE);
	}


	//允许接收错误中断
	void __usart_en_err(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_PERR,ENABLE);
		USART_INTConfig(pu->huart,USART_INT_FERR,ENABLE);
	}

	//设置中断优先级并且使能中断
	void __usart_set_it_pri(USART_T *pu, unsigned char pri)
	{
		NVIC_InitType NVIC_InitStructure;	//定义中断配置结构体 
		
		NVIC_InitStructure.NVIC_IRQChannel = g_phuart[pu->com].IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pri;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}

	//发送一个字节
	void __usart_send_byte(USART_T *pu, unsigned char ch)
	{	
		USART_SendData(pu->huart,ch);
	}

	//接收一个字节
	unsigned char __usart_receive_byte(USART_T *pu)
	{
		return  USART_ReceiveData(pu->huart);
	}
			//检查中断寄存器，高8位式错误信息，低8位是中断信息
	unsigned short __usart_isr_chk(USART_T *pu)
	{
		USART_Type *UASRTx = pu->huart;
		unsigned short chk;
		
		uint32_t isrflags   = UASRTx->STS;
		uint32_t cr1its     = UASRTx->CTRL1;
		uint32_t cr3its     = UASRTx->CTRL3;
		uint32_t errorflags = 0x00U;
		//uint32_t dmarequest = 0x00U;
		
		//USART_T *pu;
		//pu = phuart_sch_pu(huart);		//通过串口实例反向搜索用户串口指针

		// If no error occurs
		errorflags = (isrflags & (uint32_t)(USART_STS_PERR | USART_STS_FERR | USART_STS_ORERR | USART_STS_NERR));
		
		chk = errorflags;
		chk <<= 8;

		// UART in mode Receiver -------------------------------------------------
		if (((isrflags & USART_STS_RDNE) != RESET) && ((cr1its & USART_CTRL1_RDNEIEN) != RESET))
		{
			chk |= USART_ITCHK_RECEIVE;	//全双工通讯接收完继续判断是否有发送中断
		}
		
		/* UART in mode Transmitter ------------------------------------------------*/
		if (((isrflags & USART_STS_TDE) != RESET) && ((cr1its & USART_CTRL1_TDEIEN) != RESET))
		{
			chk |= USART_ITCHK_TXE;			//发送一个字节
		}

		/* UART in mode Transmitter end --------------------------------------------*/
		if (((isrflags & USART_STS_TRAC) != RESET) && ((cr1its & USART_CTRL1_TRACIEN) != RESET))
		{
			chk |= USART_ITCHK_TC;			//物理上发送完成
		}
		return(chk);
	}

			//直接设置波特率
	void __usart_set_baudrate(USART_T *pu,uint32_t BaudRate)
	{
	//------------------------------
	//设定波特率  
	//------------------------------
	//设定帧间隔超时
		
		Set_Usart_Tout_1mS(pu,BaudRate);
	//------------------------------
	}
	#endif
	/***********************************************************************************************/
	#if defined (MCU_AT32F413x) || defined (MCU_AT32F415x) || defined (MCU_AT32F403x)
	
	#define GPIO_P(A) ((GPIO_Type*)((uint32_t)(A) * 0x400 + GPIOA_BASE))

	//关联串口实例与串口指针表
	const PHUART_T g_phuart[5]={
		{USART1,USART1_IRQn,USART1},
		{USART2,USART2_IRQn,USART2},
		{USART3,USART3_IRQn,USART3},
		{UART4,UART4_IRQn,UART4},
		{UART5,UART5_IRQn,UART5}
	};

	
	void USART1_IRQHandler(void)
	{
		usart_isr(&g_USART[0]);	
	}
	void USART2_IRQHandler(void)
	{
		usart_isr(&g_USART[1]);	
	}
	void USART3_IRQHandler(void)
	{
		usart_isr(&g_USART[2]);	
	}
	void UART4_IRQHandler(void)
	{
		usart_isr(&g_USART[3]);	
	}
	void UART5_IRQHandler(void)
	{
		usart_isr(&g_USART[4]);	
	}

//--------------------------------------------------------------------------------------
	//配置所有的TX，RX，RS485端口
	void __usart_msp_port_init(void)
	{
		GPIO_InitType GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1 | RCC_APB2PERIPH_GPIOA | RCC_APB2PERIPH_GPIOB | RCC_APB2PERIPH_GPIOC | RCC_APB2PERIPH_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2 | RCC_APB1PERIPH_USART3 | RCC_APB1PERIPH_UART4 | RCC_APB1PERIPH_UART5 , ENABLE);
		
		GPIO_StructInit(&GPIO_InitStructure);
		
		//配置RX1口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		
		//配置TX1口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		//配置RX2口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		
		//配置TX2口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		
		//配置RX3口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		
		//配置TX3口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		//--------------------------------------------------------------------------------
		
		//配置RX4口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		
		//配置TX4口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		//--------------------------------------------------------------------------------
		
		//配置RX5口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		
		//配置TX5口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = GPIO_Pins_12;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		//--------------------------------------------------------------------------------
		
		
	}
	
	void __usart_rs485_port_init(USART_T *pu)
	{
		GPIO_InitType GPIO_InitStructure;
		//配置RS485口
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
		GPIO_InitStructure.GPIO_MaxSpeed=GPIO_MaxSpeed_2MHz;
		GPIO_InitStructure.GPIO_Pins = (1 << pu->RS485_PIN);
		GPIO_Init(GPIO_P(pu->RS485_PORT), &GPIO_InitStructure);
	}
	
	
	//--------------------------------------------------------------------------------------
	void __usart_msp_init(USART_T *pu)
	{
		USART_InitType USART_InitStruct;
		USART_Type *USARTx;
				
		USART_StructInit(&USART_InitStruct);
		USART_InitStruct.USART_BaudRate=pu->baud;
		USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
		USART_InitStruct.USART_Mode= USART_Mode_Tx | USART_Mode_Rx;
		
		if(pu->UART_Parity == USART_PARITY_E)
		{
			USART_InitStruct.USART_Parity = USART_Parity_Even;
			USART_InitStruct.USART_WordLength = USART_WordLength_9b;
		}
		else if(pu->UART_Parity == USART_PARITY_O)
		{
			USART_InitStruct.USART_Parity = USART_Parity_Odd;
			USART_InitStruct.USART_WordLength = USART_WordLength_9b;
		}
		else
		{
			USART_InitStruct.USART_Parity = USART_Parity_No;
			USART_InitStruct.USART_WordLength = USART_WordLength_8b;
		}
		
		USART_InitStruct.USART_StopBits = USART_StopBits_1;
		
		USARTx = pu->huart;
		
		USART_Init(USARTx, &USART_InitStruct);
		
		USART_INTConfig(USARTx,USART_INT_RDNE,ENABLE);
		
		USART_Cmd(USARTx,ENABLE);		
	}
			
			
	//切换RS485引脚到发送状态
	void __usart_rs485_send(USART_T *pu)
	{
		if(pu->RS485_PORT != RS485_PORT_NULL)
		{
			GPIO_WriteBit(GPIO_P(pu->RS485_PORT), (1 << pu->RS485_PIN),(BitState)(pu->RS485_POL));
			if(pu->sta & (1<<B_USART_485_DELAY))
			{
				delay_us_set_k(FUNC_USART_485_DELAY_US);
			}
		}
		
		//if(!(pu->sta & USART_CFG_FD))
		if(!(pu->sta & (1 << B_USART_FD)))
		{
			__usart_dis_receive(pu);
		}
	}

	void __usart_rs485_receive(USART_T *pu)
	{
		if(pu->RS485_PORT != RS485_PORT_NULL)
		{
			GPIO_WriteBit(GPIO_P(pu->RS485_PORT), (1 << pu->RS485_PIN),(BitState)(!pu->RS485_POL));
		}
	}

	//禁止接收使能
	void __usart_dis_receive(USART_T *pu)
	{
		USART_Type *USARTx;	
		USARTx = pu->huart;
		
		USARTx->CTRL1 &= ~USART_CTRL1_REN;
	}


	//允许接收
	void __usart_en_receive(USART_T *pu)
	{
		USART_Type *USARTx;	
		USARTx = pu->huart;
		
		USARTx->CTRL1 |= USART_CTRL1_REN;
	}

	//允许TC中断 发送
	void __usart_en_tc_it(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TRAC,ENABLE);
	}

			//禁止TC中断
	void __usart_dis_tc_it(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TRAC,DISABLE);
	}


	//允许发送缓存空中断
	void __usart_en_txe(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TDE,ENABLE);

	}

	//允许发送缓存空中断
	void __usart_dis_txe(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_TDE,DISABLE);
	}


	//允许接收错误中断
	void __usart_en_err(USART_T *pu)
	{
		USART_INTConfig(pu->huart,USART_INT_PERR,ENABLE);
		USART_INTConfig(pu->huart,USART_INT_FERR,ENABLE);
	}

	//设置中断优先级并且使能中断
	void __usart_set_it_pri(USART_T *pu, unsigned char pri)
	{
		NVIC_InitType NVIC_InitStructure;	//定义中断配置结构体 
		
		NVIC_InitStructure.NVIC_IRQChannel = g_phuart[pu->com].IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = pri;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}

	//发送一个字节
	void __usart_send_byte(USART_T *pu, unsigned char ch)
	{	
		USART_SendData(pu->huart,ch);
	}

	//接收一个字节
	unsigned char __usart_receive_byte(USART_T *pu)
	{
		return  USART_ReceiveData(pu->huart);
	}
			//检查中断寄存器，高8位式错误信息，低8位是中断信息
	unsigned short __usart_isr_chk(USART_T *pu)
	{
		USART_Type *UASRTx = pu->huart;
		unsigned short chk;
		
		uint32_t isrflags   = UASRTx->STS;
		uint32_t cr1its     = UASRTx->CTRL1;
		uint32_t cr3its     = UASRTx->CTRL3;
		uint32_t errorflags = 0x00U;
		//uint32_t dmarequest = 0x00U;
		
		//USART_T *pu;
		//pu = phuart_sch_pu(huart);		//通过串口实例反向搜索用户串口指针

		// If no error occurs
		errorflags = (isrflags & (uint32_t)(USART_STS_PERR | USART_STS_FERR | USART_STS_ORERR | USART_STS_NERR));
		
		chk = errorflags;
		chk <<= 8;

		// UART in mode Receiver -------------------------------------------------
		if (((isrflags & USART_STS_RDNE) != RESET) && ((cr1its & USART_CTRL1_RDNEIEN) != RESET))
		{
			chk |= USART_ITCHK_RECEIVE;	//全双工通讯接收完继续判断是否有发送中断
		}
		
		/* UART in mode Transmitter ------------------------------------------------*/
		if (((isrflags & USART_STS_TDE) != RESET) && ((cr1its & USART_CTRL1_TDEIEN) != RESET))
		{
			chk |= USART_ITCHK_TXE;			//发送一个字节
		}

		/* UART in mode Transmitter end --------------------------------------------*/
		if (((isrflags & USART_STS_TRAC) != RESET) && ((cr1its & USART_CTRL1_TRACIEN) != RESET))
		{
			chk |= USART_ITCHK_TC;			//物理上发送完成
		}
		return(chk);
	}

			//直接设置波特率
	void __usart_set_baudrate(USART_T *pu,uint32_t BaudRate)
	{
	//------------------------------
	//设定波特率  
	//------------------------------
	//设定帧间隔超时
		
		Set_Usart_Tout_1mS(pu,BaudRate);
	//------------------------------
	}
	
	#endif
	//--------------------------- STM32F10x系列驱动函数 -------------------------------------
	#ifdef MCU_STM32F10x

		//对串口实例进行弱定义，强定义在cube初始化的时候进行
		__WEAK UART_HandleTypeDef huart1;
		__WEAK UART_HandleTypeDef huart2;
		__WEAK UART_HandleTypeDef huart3;
		__WEAK UART_HandleTypeDef huart4;
		__WEAK UART_HandleTypeDef huart5;


		//关联串口实例与串口指针表
		const PHUART_T g_phuart[5]={
			{&huart1,USART1_IRQn,USART1},
			{&huart2,USART2_IRQn,USART2},
			{&huart3,USART3_IRQn,USART3},
			{&huart4,UART4_IRQn,UART4},
			{&huart5,UART5_IRQn,UART5}
		};


		//--------------------------------------------------------------------------------------
		void __usart_msp_init(USART_T *pu)
		{
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_RXNE);	//允许接收中断
		}
		
		void __usart_msp_port_init(void)
		{
		}
	
		void __usart_rs485_port_init(USART_T *pu)
		{
		}
		
		//切换RS485引脚到发送状态
		void __usart_rs485_send(USART_T *pu)
		{
			GPIO_TypeDef *gp;
			if(pu->RS485_PORT != RS485_PORT_NULL)
			{
				gp=(GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE)*pu->RS485_PORT);
				HAL_GPIO_WritePin(gp,(1 << pu->RS485_PIN),(GPIO_PinState)(pu->RS485_POL));
			}
			//if(!(pu->sta & USART_CFG_FD))
			if(!(pu->sta & (1 << B_USART_FD)))
			{
				__usart_dis_receive(pu);
			}
		}

		void __usart_rs485_receive(USART_T *pu)
		{
			GPIO_TypeDef *gp;
			if(pu->RS485_PORT != RS485_PORT_NULL)
			{
				gp=(GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE)*pu->RS485_PORT);
				HAL_GPIO_WritePin(gp,(1 << pu->RS485_PIN),(GPIO_PinState)(!pu->RS485_POL));
			}
		}

		//禁止接收
		void __usart_dis_receive(USART_T *pu)
		{
			//__HAL_UART_DISABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_RXNE);
			((UART_HandleTypeDef *)(pu->huart))->Instance->CR1 &= ~USART_CR1_RE;
		}


		//允许接收
		void __usart_en_receive(USART_T *pu)
		{
			//__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_RXNE);	//允许接收
			((UART_HandleTypeDef *)(pu->huart))->Instance->CR1 |= USART_CR1_RE;
		}


		//允许TC中断
		void __usart_en_tc_it(USART_T *pu)
		{
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TC);	
		}

		//禁止TC中断
		void __usart_dis_tc_it(USART_T *pu)
		{
			__HAL_UART_DISABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TC);	
		}


		//允许发送缓存空中断
		void __usart_en_txe(USART_T *pu)
		{
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TXE);
		}

		//允许发送缓存空中断
		void __usart_dis_txe(USART_T *pu)
		{
			__HAL_UART_DISABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TXE);
		}


		//允许接收错误中断
		void __usart_en_err(USART_T *pu)
		{
			// Enable the UART Parity Error Interrupt 
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_PE);

			// Enable the UART Error Interrupt: (Frame error, noise error, overrun error) 
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_ERR);
		}

		//设置中断优先级并且使能中断
		void __usart_set_it_pri(USART_T *pu, unsigned char pri)
		{
			HAL_NVIC_SetPriority((IRQn_Type)g_phuart[pu->com].IRQn, pri, 0);
			HAL_NVIC_EnableIRQ((IRQn_Type)g_phuart[pu->com].IRQn);
		}

		//发送一个字节
		void __usart_send_byte(USART_T *pu, unsigned char ch)
		{
			((UART_HandleTypeDef *)(pu->huart))->Instance->DR = pu->tx_buf[pu->t_s];	//发送字节
		}

		//接收一个字节
		unsigned char __usart_receive_byte(USART_T *pu)
		{
			//(uint8_t)(pu->huart->Instance->DR & (uint8_t)0x00FF);
			return(((UART_HandleTypeDef *)(pu->huart))->Instance->DR & (uint8_t)0x00FF);	//接收字节
		}

		//检查中断寄存器，高8位式错误信息，低8位是中断信息
		unsigned short __usart_isr_chk(USART_T *pu)
		{
			UART_HandleTypeDef *huart = pu->huart;
			unsigned short chk;
			
			uint32_t isrflags   = READ_REG(huart->Instance->SR);
			uint32_t cr1its     = READ_REG(huart->Instance->CR1);
			uint32_t cr3its     = READ_REG(huart->Instance->CR3);
			uint32_t errorflags = 0x00U;
			//uint32_t dmarequest = 0x00U;
			
			//USART_T *pu;
			//pu = phuart_sch_pu(huart);		//通过串口实例反向搜索用户串口指针

			// If no error occurs
			errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
			
			chk = errorflags;
			chk <<= 8;

			// UART in mode Receiver -------------------------------------------------
			if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
			{
				chk |= USART_ITCHK_RECEIVE;	//全双工通讯接收完继续判断是否有发送中断
			}
			
			/* UART in mode Transmitter ------------------------------------------------*/
			if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
			{
				chk |= USART_ITCHK_TXE;			//发送一个字节
			}

			/* UART in mode Transmitter end --------------------------------------------*/
			if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
			{
				chk |= USART_ITCHK_TC;			//物理上发送完成
			}
			return(chk);
		}

		//直接设置波特率
		void __usart_set_baudrate(USART_T *pu,uint32_t BaudRate)
		{
			//------------------------------
			//设定波特率
			//------------------------------
			//设定帧间隔超时
			Set_Usart_Tout_1mS(pu,BaudRate);
			//------------------------------
		}
		//================================================================================
		//中断函数
		void USART1_IRQHandler(void)
		{
			usart_isr(&g_USART[0]);	
		}

		void USART2_IRQHandler(void)
		{
			usart_isr(&g_USART[1]);	
		}

		void USART3_IRQHandler(void)
		{
			usart_isr(&g_USART[2]);	
		}

		#if USART_N > 3
		void UART4_IRQHandler(void)
		{
			usart_isr(&g_USART[3]);	
		}

		void UART5_IRQHandler(void)
		{
			usart_isr(&g_USART[4]);	
		}
		#endif //#if USART_N > 3
	#endif	//#ifdef MCU_STM32F10x
	
	/*********************************************************************************************/
	//CX32L003 驱动函数
		#ifdef MCU_CX32L003
		static const uint8_t ParityTable256[256] = 
			{
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0, 
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1, 	
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1,	
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0, 	
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1,		
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0, 	
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0, 
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1,	
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1,	
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0, 
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0, 
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1,
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0, 
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1,
			1, 0,	0, 1,	0, 1,	1, 0,	0, 1,	1, 0,	1, 0,	0, 1,
			0, 1,	1, 0,	1, 0,	0, 1,	1, 0,	0, 1,	0, 1,	1, 0	
			};
		//--------------------------------------------------------------------------------------
		//对串口实例进行弱定义，强定义在cube初始化的时候进行
		UART_HandleTypeDef huart0;//huart1的话会报在func_usrat_msp.o和log.o中重复定义
		UART_HandleTypeDef huart1;
		//关联串口实例与串口指针表
		const PHUART_T g_phuart[2]={
			{&huart0,UART0_IRQn,((UART_TypeDef *)UART0_BASE)},//定义了两个串口，串口号，中断号，串口基地址
			{&huart1,UART1_IRQn,((UART_TypeDef *)UART1_BASE)}
		};
		
		//--------------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------------
		//对通讯端口的配置，每个工程都需要重新修改，如果有多个串口都一起在这里配置
		void __usart_msp_port_init(void)
		{
			GPIO_InitTypeDef GPIO_InitStruct = {0};
			
			__HAL_RCC_UART0_CLK_ENABLE();				//打开串口使能
			
			__HAL_RCC_GPIOA_CLK_ENABLE();				//打开相应的端口使能

			//配置TX端口
			GPIO_InitStruct.Pin = GPIO_PIN_1;
			GPIO_InitStruct.Mode = GPIO_MODE_AF;
			GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;	//推挽
			GPIO_InitStruct.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;//去抖动不使能
			GPIO_InitStruct.SlewRate = GPIO_SLEW_RATE_HIGH;
			GPIO_InitStruct.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Alternate = GPIO_AF5_UART0_RXD;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			
			//配置RX端口
			GPIO_InitStruct.Pin = GPIO_PIN_2;
			GPIO_InitStruct.Mode = GPIO_MODE_AF;
			GPIO_InitStruct.Alternate = GPIO_AF5_UART0_TXD;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		}
		//--------------------------------------------------------------------------------------
		//--------------------------------------------------------------------------------------
		
		void __usart_rs485_port_init(USART_T *pu)
		{
			GPIO_InitTypeDef GPIO_InitStruct = {0};
			GPIO_TypeDef  * GPIOx;
			
			//对RS485端口进行初始化
			switch(pu->RS485_PORT)
			{
			case 0:
				__HAL_RCC_GPIOA_CLK_ENABLE();
				GPIOx = GPIOA;
				break;
			case 1:
				__HAL_RCC_GPIOB_CLK_ENABLE();
				GPIOx = GPIOB;
				break;
			case 2:
				__HAL_RCC_GPIOC_CLK_ENABLE();
				GPIOx = GPIOC;
				break;
			case 3:
				__HAL_RCC_GPIOD_CLK_ENABLE();
				GPIOx = GPIOD;
				break;
			}
			
			if(pu->RS485_PORT != RS485_PORT_NULL)
			{
				GPIO_InitStruct.Pin = (1 << pu->RS485_PIN);
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
				GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;	//推挽
				GPIO_InitStruct.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;//去抖动不使能
				GPIO_InitStruct.SlewRate = GPIO_SLEW_RATE_LOW;
				GPIO_InitStruct.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Alternate = 0;
				HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
			}
		}
				
		void __usart_msp_init(USART_T *pu)
		{
			//UART_MspInit(pu);		 //用这个做芯片端口驱动，不在对HAL_UART_MspInit(huart);进行强定义了
			
			// Check the UART handle allocation
			UART_HandleTypeDef *UartxHandle=pu->huart;//定义了一个临时的串口参数，指向了传递进来的串口
			
			UartxHandle->Instance = g_phuart[pu->com].UART_x;//串口地址
			UartxHandle->Init.BaudRate = pu->baud;	//pu->baud 波特率
			UartxHandle->Init.BaudDouble = UART_BAUDDOUBLE_ENABLE;	//无波特率自适应？
			//UartxHandle->Init.WordLength = pu->UART_WordLength;//数据长度？
			if(pu->UART_Parity == USART_PARITY_N)
			{
				UartxHandle->Init.WordLength = UART_WORDLENGTH_8B;//数据长度？
			}
			else
			{
				UartxHandle->Init.WordLength = UART_WORDLENGTH_9B;//数据长度？
			}
			UartxHandle->Init.Parity = pu->UART_Parity;//校验位
			UartxHandle->Init.Mode = UART_MODE_TX_RX;//收发模式。
			HAL_UART_Init(UartxHandle);			//与硬件相关的配置初始化
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_RXNE);
		}
				
		
		//切换RS485引脚到发送状态
		void __usart_rs485_send(USART_T *pu)
		{
			GPIO_TypeDef *gp;
			if(pu->RS485_PORT != RS485_PORT_NULL)
			{
				gp=(GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE)*pu->RS485_PORT);
				HAL_GPIO_WritePin(gp,(1 << pu->RS485_PIN),(GPIO_PinState)(pu->RS485_POL));
			}
			//if(!(pu->sta & USART_CFG_FD))
			if(!(pu->sta & (1 << B_USART_FD)))
			{
				__usart_dis_receive(pu);
			}
		}

		void __usart_rs485_receive(USART_T *pu)
		{
			GPIO_TypeDef *gp;
			if(pu->RS485_PORT != RS485_PORT_NULL)
			{
				gp=(GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE - GPIOA_BASE)*pu->RS485_PORT);
				HAL_GPIO_WritePin(gp,(1 << pu->RS485_PIN),(GPIO_PinState)(!pu->RS485_POL));
			}
		}

		//禁止接收中断
		void __usart_dis_receive(USART_T *pu)
		{
			//__HAL_UART_DISABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_RXNE);//RXNE->RIEN
			((UART_HandleTypeDef *)pu->huart)->Instance->SCON &= ~UART_SCON_REN;
			
		}


		//允许接收中断
		void __usart_en_receive(USART_T *pu)
		{
			//__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_RXNE);
			((UART_HandleTypeDef *)pu->huart)->Instance->SCON |= UART_SCON_REN;
		}


		//允许TC中断
		void __usart_en_tc_it(USART_T *pu)
		{
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TC);	//TC->TIEN
		}

		//禁止TC中断
		void __usart_dis_tc_it(USART_T *pu)
		{
			__HAL_UART_DISABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TC);	
		}
		

		//允许发送缓存空中断
		void __usart_en_txe(USART_T *pu)//利用这个函数去发送信息这里
		{
			unsigned int j;
			
			__usart_send_byte(pu,pu->tx_buf[pu->t_s]);//发送一个字符来置位标志位
			j=pu->t_s+1;
			j %= (pu->tx_buf_size);
			pu->t_s=j;
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TC);	//TC->TIEN
		}

		//禁止发送缓存空中断
		void __usart_dis_txe(USART_T *pu)
		{
			__HAL_UART_DISABLE_IT((UART_HandleTypeDef *)(pu->huart), UART_IT_TC);	
		}


		//允许接收错误中断
		void __usart_en_err(USART_T *pu)
		{
			__HAL_UART_ENABLE_IT((UART_HandleTypeDef *)(pu->huart), LPUART_IT_TE);//TE->TEEN
		}

		//设置中断优先级并且使能中断
		void __usart_set_it_pri(USART_T *pu, unsigned char pri)
		{
			HAL_NVIC_SetPriority((IRQn_Type)g_phuart[pu->com].IRQn, pri);
			HAL_NVIC_EnableIRQ((IRQn_Type)g_phuart[pu->com].IRQn);//在总中断里使能串口中断
		}

		//发送一个字节
		void __usart_send_byte(USART_T *pu, unsigned char ch)
		{
			UART_HandleTypeDef *UartxHandle=pu->huart;
			if(UartxHandle->Init.Parity == UART_PARITY_EVEN)
			{
				MODIFY_REG(UartxHandle->Instance->SCON, UART_SCON_TB8, ((ParityTable256[ch] & 0x01) << UART_SCON_TB8_Pos));
			}
			else if(UartxHandle->Init.Parity == UART_PARITY_ODD)
			{
				MODIFY_REG(UartxHandle->Instance->SCON, UART_SCON_TB8, (((~ParityTable256[ch]) & 0x01) << UART_SCON_TB8_Pos));
			}
			((UART_HandleTypeDef *)(pu->huart))->Instance->SBUF = ch;	//发送字节
		
		}

		//接收一个字节
		unsigned char __usart_receive_byte(USART_T *pu)
		{
			return(((UART_HandleTypeDef *)(pu->huart))->Instance->SBUF & (uint8_t)0x00FF);	//接收字节
		}

		//检查中断寄存器，高8位是错误信息，低8位是中断信息//没有错误信息，尝试只查低三位
		unsigned short __usart_isr_chk(USART_T *pu)
		{
			UART_HandleTypeDef *huart = pu->huart;
			unsigned short chk;
			uint32_t errorflags = 0x00U;//没有的话，先设成没错看看
			uint32_t sconits     = READ_REG(huart->Instance->INTSR);//读取UART标志位寄存器
			huart->Instance->INTCLR = 0x00000007;//清除标志位,不知是否需要

			chk = errorflags;
			chk <<= 8;

			// UART in mode Receiver -------------------------------------------------
			if ((sconits & 0x00000001) != RESET)//RXNE->R1->0 bit
			{
				chk |= USART_ITCHK_RECEIVE;	//全双工通讯接收完继续判断是否有发送中断
			}
//			
//			/* UART in mode Transmitter ------------------------------------------------*/
//			if ((sconits & USART_CR1_TXEIE) != RESET)
//			{
//				chk |= USART_ITCHK_TXE;			//发送一个字节
//			}

			/* UART in mode Transmitter end --------------------------------------------*/
			if ((sconits & 0x00000002) != RESET)//TC->T1->1 bit
			{
				if(pu->t_s != pu->t_e)
				{
					chk |= USART_ITCHK_TXE;			//不是最后一个字节
				}
				else
				{
					chk |= USART_ITCHK_TC;			//最后一个字节物理上发送完成
				}
			}
			return(chk);
		}

		//直接设置波特率
		void __usart_set_baudrate(USART_T *pu,uint32_t BaudRate)
 		{
			//------------------------------
			//设定波特率
			UART_HandleTypeDef *huart = pu->huart;
			huart->Init.BaudRate=BaudRate;
			/* Check the parameters */
			assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
			huart->Instance->BAUDCR = (((((huart->Init.BaudDouble >> UART_SCON_DBAUD_Pos)+1)*HAL_RCC_GetPCLKFreq())/(32*(huart->Init.BaudRate))-1) & UART_BAUDCR_BRG) | UART_BAUDCR_SELF_BRG;			
			//------------------------------
			//设定帧间隔超时
			Set_Usart_Tout_1mS(pu,BaudRate);
			//------------------------------
		}
		//================================================================================
		//中断函数
		
		void UART0_IRQHandler(void)
		{
			usart_isr(&g_USART[0]);	
		}
		
		void UART1_IRQHandler(void)
		{
			usart_isr(&g_USART[1]);	
		}
	#endif	//#ifdef MCU_CX32L003
//=============================================================================================
	#ifdef MCU_HC32L13x
	
	#include "ddl.h"

	//关联串口实例与串口指针表
	const PHUART_T g_phuart[2]={
		{M0P_UART0,UART0_IRQn,((void *)M0P_UART0)},
		{M0P_UART1,UART1_IRQn,((void *)M0P_UART1)}
	};

	void UART1_IRQHandler(void)
	{
		usart_isr(&g_USART[1]);	
	}
	void UART0_IRQHandler(void)
	{
		usart_isr(&g_USART[0]);	
	}

//--------------------------------------------------------------------------------------
	//定义tx和rx的IO口
	typedef struct{
		en_sysctrl_peripheral_gate_t PERIPHERAL_GATE;
		en_gpio_port_t  UARTx_TX_PORT;
		en_gpio_pin_t UARTx_TX_PIN;
		en_gpio_af_t UARTx_TX_ALTERNATE_AFn;
		en_gpio_port_t  UARTx_RX_PORT;
		en_gpio_pin_t UARTx_RX_PIN;
		en_gpio_af_t UARTx_RX_ALTERNATE_AFn;
	}USART_MSP_T;

	//每次修改硬件，填写对应的TX,RX的pin脚
	const USART_MSP_T usart_msp[2]={
		{	//UART0
			SysctrlPeripheralUart0,
			GpioPortB,
			GpioPin6,
			GpioAf2,
			GpioPortB,
			GpioPin7,
			GpioAf2
		},
		{	//UART1
			SysctrlPeripheralUart1,
			GpioPortA,
			GpioPin2,
			GpioAf2,
			GpioPortA,
			GpioPin3,
			GpioAf2
		}
	};
	
	
	void __usart_msp_port_init(void)
	{
		;
	}
	
	void __usart_rs485_port_init(USART_T *pu)
	{
		;
	}
	
	void UART_PortInit(USART_T *pu)
	{
		stc_gpio_config_t stcGpioCfg;
		
		DDL_ZERO_STRUCT(stcGpioCfg);
		
		Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 	//打开所有的口

		Sysctrl_SetPeripheralGate(usart_msp[pu->com].PERIPHERAL_GATE,TRUE);	//使能uart模块时钟
		

		stcGpioCfg.enDir = GpioDirOut;
		stcGpioCfg.enDrv = GpioDrvH;
		stcGpioCfg.enPuPd = GpioNoPuPd;
		stcGpioCfg.enOD = GpioOdDisable;
		stcGpioCfg.enCtrlMode = GpioAHB;

		Gpio_Init(usart_msp[pu->com].UARTx_TX_PORT,usart_msp[pu->com].UARTx_TX_PIN,&stcGpioCfg);
		Gpio_SetAfMode(usart_msp[pu->com].UARTx_TX_PORT,usart_msp[pu->com].UARTx_TX_PIN,usart_msp[pu->com].UARTx_TX_ALTERNATE_AFn);//TX

		stcGpioCfg.enDir = GpioDirIn;
		Gpio_Init(usart_msp[pu->com].UARTx_RX_PORT,usart_msp[pu->com].UARTx_RX_PIN,&stcGpioCfg);
		Gpio_SetAfMode(usart_msp[pu->com].UARTx_RX_PORT,usart_msp[pu->com].UARTx_RX_PIN,usart_msp[pu->com].UARTx_RX_ALTERNATE_AFn);//RX
		
		//-----------------------------------------------------------------------------------------------
		//对RS485端口进行初始化			
		if(pu->RS485_PORT != RS485_PORT_NULL)
		{
			stcGpioCfg.enDir = GpioDirOut;
			stcGpioCfg.enDrv = GpioDrvH;
			stcGpioCfg.enPuPd = GpioNoPuPd;
			stcGpioCfg.enOD = GpioOdDisable;
			stcGpioCfg.enCtrlMode = GpioAHB;
			
			Gpio_Init((en_gpio_port_t)(pu->RS485_PORT * 0x40),(en_gpio_pin_t)(pu->RS485_PIN),&stcGpioCfg);
		}
		//--------------------------------------------------------------------
	}

	//--------------------------------------------------------------------------------------
	void __usart_msp_init(USART_T *pu)
	{
		uint16_t u16Scnt = 0;
		stc_uart_config_t  stcConfig;
		stc_uart_irq_cb_t stcUartIrqCb;
		stc_uart_multimode_t stcMulti;
		stc_uart_baud_t stcBaud;
		
		en_uart_mmdorck_t enTb8;
		
		
		DDL_ZERO_STRUCT(stcConfig);
		DDL_ZERO_STRUCT(stcUartIrqCb);
		DDL_ZERO_STRUCT(stcMulti);
		DDL_ZERO_STRUCT(stcBaud);
		
		//Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);//使能GPIO模块时钟
		//Sysctrl_SetPeripheralGate(RS485_SYS_CLOCK,TRUE);//使能uart模块时钟
		UART_PortInit(pu);//串口端口初始化
		
		stcUartIrqCb.pfnRxIrqCb   = NULL;//中断回调函数入口地址
		stcUartIrqCb.pfnTxIrqCb   = NULL;
		stcUartIrqCb.pfnRxFEIrqCb = NULL;
		stcUartIrqCb.pfnPEIrqCb   = NULL;
		stcUartIrqCb.pfnCtsIrqCb  = NULL;
		stcConfig.pstcIrqCb = &stcUartIrqCb;
		stcConfig.bTouchNvic = TRUE;
	  
	//	if(TRUE == stcConfig.bTouchNvic)
	//	{
	//		EnableNvic((IRQn_Type)g_phuart[pu->com].IRQn,IrqLevel3,TRUE);
	//	}
		EnableNvic((IRQn_Type)g_phuart[pu->com].IRQn,IrqLevel3,TRUE);	//允许NVIC中断
		
		if(pu->UART_Parity == USART_PARITY_N)
		{
			stcConfig.enRunMode = UartMode1;//模式1
		}
		else
		{
			stcConfig.enRunMode = UartMode3;//模式1
		}
		
		stcConfig.enStopBit = Uart1bit;  //1bit停止位
		
		stcMulti.enMulti_mode = UartNormal;//正常工作模式
		
		Uart_SetMultiMode(pu->com,&stcMulti);//多主机单独配置
		
		//enTb8 = UartDataOrAddr;//无校验
		enTb8 = (en_uart_mmdorck_t)pu->UART_Parity;	// 设校验
		
		Uart_SetMMDOrCk(pu->com,enTb8);
		
		Uart_Init(pu->com, &stcConfig);//串口初始化
		
		Uart_SetClkDiv(pu->com,Uart8Or16Div);//采样分频
		stcBaud.u32Pclk = Sysctrl_GetPClkFreq();
		stcBaud.enRunMode = UartMode1;
		stcBaud.u32Baud = pu->baud;
		u16Scnt = Uart_CalScnt(pu->com,&stcBaud);//波特率计算
		Uart_SetBaud(pu->com,u16Scnt);//波特率设置
		
		Uart_ClrStatus(pu->com,UartRC);//清接收请求
		Uart_EnableIrq(pu->com,UartRxIrq);//使能串口中断  
		Uart_EnableFunc(pu->com,UartRx);//使能收发 	
	}
			
			
	//切换RS485引脚到发送状态
	void __usart_rs485_send(USART_T *pu)
	{
	//	RS485_SEND_EN;
		//Gpio_WriteOutputIO(RS485_EN_PORT,RS485_EN_PIN,1);
		if(pu->RS485_PORT != RS485_PORT_NULL)
		{
			Gpio_WriteOutputIO((en_gpio_port_t)(pu->RS485_PORT * 0x40), (en_gpio_pin_t)pu->RS485_PIN,pu->RS485_POL);
		}
		
		//if(!(pu->sta & USART_CFG_FD))
		if(!(pu->sta & (1 << B_USART_FD)))
		{
			__usart_dis_receive(pu);
		}
	}

	void __usart_rs485_receive(USART_T *pu)
	{
	//	RS485_REC_EN;
		//Gpio_WriteOutputIO(RS485_EN_PORT,RS485_EN_PIN,0);
		if(pu->RS485_PORT != RS485_PORT_NULL)
		{
			Gpio_WriteOutputIO((en_gpio_port_t)(pu->RS485_PORT * 0x40),(en_gpio_pin_t)pu->RS485_PIN,!pu->RS485_POL);
		}
	}

	//禁止接收使能
	void __usart_dis_receive(USART_T *pu)
	{
		//Uart_DisableIrq(RS485_WORK_CH,UartRxIrq);
		//Uart_DisableIrq(pu->com,UartRxIrq);
		Uart_DisableFunc(pu->com,UartRx);
	}


	//允许接收
	void __usart_en_receive(USART_T *pu)
	{
		//Uart_EnableIrq(pu->com,UartRxIrq);
		Uart_EnableFunc(pu->com,UartRx);
	}

	//允许TC中断 发送
	void __usart_en_tc_it(USART_T *pu)
	{
		Uart_EnableIrq(pu->com,UartTxIrq);
	}

			//禁止TC中断
	void __usart_dis_tc_it(USART_T *pu)
	{
		Uart_DisableIrq(pu->com,UartTxIrq);

	}


	//允许发送缓存空中断
	void __usart_en_txe(USART_T *pu)
	{
		Uart_EnableIrq(pu->com,UartTxEIrq);

	}

	//允许发送缓存空中断
	void __usart_dis_txe(USART_T *pu)
	{
		Uart_DisableIrq(pu->com,UartTxEIrq);

	}


	//允许接收错误中断
	void __usart_en_err(USART_T *pu)
	{
		
	// Enable the UART Parity Error Interrupt 
		Uart_EnableIrq(pu->com,UartPEIrq);

	// Enable the UART Error Interrupt: (Frame error, noise error, overrun error) 
		Uart_EnableIrq(pu->com,UartFEIrq);
	}

	//设置中断优先级并且使能中断
	void __usart_set_it_pri(USART_T *pu, unsigned char pri)
	{
		EnableNvic((IRQn_Type)g_phuart[pu->com].IRQn,(en_irq_level_t)pri,TRUE);
	}

	//发送一个字节
	void __usart_send_byte(USART_T *pu, unsigned char ch)
	{	
		//M0P_UART0->SBUF = pu->tx_buf[pu->t_s];
		((M0P_UART_TypeDef *)(pu->huart))->SBUF=ch;
	}

	//接收一个字节
	unsigned char __usart_receive_byte(USART_T *pu)
	{
		return  Uart_ReceiveData(pu->com);
	}
			//检查中断寄存器，高8位式错误信息，低8位是中断信息
	unsigned short __usart_isr_chk(USART_T *pu)
	{

		unsigned short chk;
		uint32_t errorflags = 0x00U;
		
		M0P_UART_TypeDef *pm0p_uart=(M0P_UART_TypeDef *)pu->huart;
		stc_uart_scon_field_t scon_f;	
		scon_f = pm0p_uart->SCON_f;

		errorflags = (pm0p_uart->ISR_f.FE << 1) | (pm0p_uart->ISR_f.PE);
		pm0p_uart->ICR_f.FECF = 0;
		pm0p_uart->ICR_f.PECF = 0;
		chk  = errorflags;
		chk <<= 8;
		// UART in mode Receiver -------------------------------------------------
		if (scon_f.RCIE && pm0p_uart->ISR_f.RC)
		{
			chk |= USART_ITCHK_RECEIVE;	//全双工通讯接收完继续判断是否有发送中断
			pm0p_uart->ICR_f.RCCF = 0;//清除接收中断标志位
		}
		

		/* UART in mode Transmitter ------------------------------------------------*/
		if ( scon_f.TXEIE && pm0p_uart->ISR_f.TXE)
		{
			chk |= USART_ITCHK_TXE;			//发送一个字节	
		}

		/* UART in mode Transmitter end --------------------------------------------*/
		//if (scon_f.TCIE && pm0p_uart->ISR_f.TC)
		if (pm0p_uart->ISR_f.TC)	//不判断是否允许中断
		{
			chk |= USART_ITCHK_TC;			//物理上发送完成
			pm0p_uart->ICR_f.TCCF = 0;//清除发送中断标志位
		}
		
		return(chk);
	}

			//直接设置波特率
	void __usart_set_baudrate(USART_T *pu,uint32_t BaudRate)
	{
	//------------------------------
	//设定波特率  
	//------------------------------
	//设定帧间隔超时
		
		Set_Usart_Tout_1mS(pu,BaudRate);
	//------------------------------
	}
	#endif
	
#endif	//#ifndef __FUNC_OLED_MSP
