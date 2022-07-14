#ifndef __PRJ_CONFIG_H__
#define __PRJ_CONFIG_H__
/***********************************************************************************
发行前注意事项：
1.修改版本号
2.修改输出文件
3.开看门狗
4.开发行相关其他选项，如ID
***********************************************************************************/
//用户自定义配置后再定义以下常量__PRJ_CONFIG_DFT__,即可取消原先的默认定义
//#define __PRJ_CONFIG_DFT__

//+++++User_Code_ParDefine Begin+++++//
//-----------------------------------------------------------------------------------
#define MCU_AT32F403x
#define MCU_CORE_M4
enum{
		OS_PI_Task_system_hi,
		OS_PI_Task_net_link,
		OS_PI_Task_system_lo,
		OS_PI_Task_net_user,
		OS_PI_Task_MAX
	};
#define OS_CPU_FPU_USED
//-----------------------------------------------------------------------------------
  #ifdef MCU_STM32F10x
    #define SYS_FREQ 72000000			//时钟频率
    #define DATA_POOL_SIZE 8192       //数据池
    #define USART_N 5                 //串口个数
  #endif
  #ifdef MCU_STM32F40x
    #define SYS_FREQ 168000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
  #ifdef MCU_HC32L13x
    #define SYS_FREQ 48000000			//时钟频率
    #define DATA_POOL_SIZE 1024
    #define USART_N 2
  #endif
  #ifdef MCU_CX32L003
    #define SYS_FREQ 24000000			//时钟频率
    #define DATA_POOL_SIZE 1024
    #define USART_N 2
  #endif
  #ifdef MCU_AT32F403x
    #define SYS_FREQ 240000000			//时钟频率
    #define DATA_POOL_SIZE 32768
    #define USART_N 5
  #endif
  #ifdef MCU_AT32F413x
    #define SYS_FREQ 200000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
  #ifdef MCU_AT32F415x
    #define SYS_FREQ 120000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
  #ifdef MCU_AT32F421x
    #define SYS_FREQ 120000000			//时钟频率
    #define DATA_POOL_SIZE 1024
    #define USART_N 2
  #endif
  #ifdef MCU_GD32F303x
    #define SYS_FREQ 120000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
//----------------------------------------------------------------------------------
//液晶显示配置
  #define DISP_LINE_N 4		//4行
  #define DISP_COLUMN_N 16	//16列
  #define DISP_BUF_BYTES (DISP_LINE_N*DISP_COLUMN_N)	//显示缓存字节数
  #define DISP_DOT_BYTE_COLUMN_N 8
#define __PRJ_CONFIG_DFT__
//-----User_Code_ParDefine End-----//

#ifndef __PRJ_CONFIG_DFT__
//-----------------------------------------------------------------------------------
#define MCU_AT32F403x
#define MCU_CORE_M4
enum{
		OS_PI_Task_system_hi,
		OS_PI_Task_net_link,
		OS_PI_Task_system_lo,
		OS_PI_Task_net_user,
		OS_PI_Task_MAX
	};
#define OS_CPU_FPU_USED
//-----------------------------------------------------------------------------------
  #ifdef MCU_STM32F10x
    #define SYS_FREQ 72000000			//时钟频率
    #define DATA_POOL_SIZE 8192       //数据池
    #define USART_N 5                 //串口个数
  #endif
  #ifdef MCU_STM32F40x
    #define SYS_FREQ 168000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
  #ifdef MCU_HC32L13x
    #define SYS_FREQ 48000000			//时钟频率
    #define DATA_POOL_SIZE 1024
    #define USART_N 2
  #endif
  #ifdef MCU_CX32L003
    #define SYS_FREQ 24000000			//时钟频率
    #define DATA_POOL_SIZE 1024
    #define USART_N 2
  #endif
  #ifdef MCU_AT32F403x
    #define SYS_FREQ 200000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
  #ifdef MCU_AT32F413x
    #define SYS_FREQ 200000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
  #ifdef MCU_AT32F415x
    #define SYS_FREQ 120000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
  #ifdef MCU_AT32F421x
    #define SYS_FREQ 120000000			//时钟频率
    #define DATA_POOL_SIZE 1024
    #define USART_N 2
  #endif
  #ifdef MCU_GD32F303x
    #define SYS_FREQ 120000000			//时钟频率
    #define DATA_POOL_SIZE 8192
    #define USART_N 5
  #endif
//----------------------------------------------------------------------------------
//液晶显示配置
  #define DISP_LINE_N 4		//4行
  #define DISP_COLUMN_N 16	//16列
  #define DISP_BUF_BYTES (DISP_LINE_N*DISP_COLUMN_N)	//显示缓存字节数
  #define DISP_DOT_BYTE_COLUMN_N 8
#endif
//----------------------------------------------------------------------------------
//**************  请根据以上例程在以下用户定义中加入项目参数定义 *****************//
#endif
