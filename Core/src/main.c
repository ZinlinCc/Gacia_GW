/*    mcusys_main();

  ******************************************************************************
  * File   : Templates/main.c 
  * Version: V1.2.5
  * Date   : 2020-10-16
  * Brief  : Main program body
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "includes.h"

/** @addtogroup AT32F413_StdPeriph_Templates
  * @{
  */

/** @addtogroup Template
  * @{
  */

/* Private define ------------------------------------------------------------*/

/* Extern variables ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

extern void mcusys_main_init(void);
extern void mcusys_main(void);

__weak void func_iap_app_init(void){;}

/**
  * @brief  Main Function.
  * @param  None
  * @retval None
  */
int main(void)
{
	func_iap_app_init();
	
	__disable_irq();
	
	Target_ResetInit();
	
	mcusys_main_init();
	
	__enable_irq();
	
	while(1)
	{
		mcusys_main();
	}
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */
