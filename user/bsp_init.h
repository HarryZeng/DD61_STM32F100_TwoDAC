///**
//  ********************************  STM32F10x  *********************************
//  * @文件名     ： bsp_init.h
//  * @作者       ： HarryZeng
//  * @库版本     ： V1.0.0
//  * @文件版本   ： V1.0.0
//  * @日期       ： 2017年10�31日
//  * @摘要       ： 数据处理
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  更新日志:
//  2017-10-31 V1.0.0:初始版本
//  ----------------------------------------------------------------------------*/
///* 包含的头文件 --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/


#ifndef __BSP_INIT_H
#define __BSP_INIT_H
#ifdef __cplusplus
 extern "C" {
#endif
#include  "stm32f10x.h"
#include "project.h"
#include 	"stdbool.h"
#include 	"stdint.h"

	
#define ADC1_DR_ADDRESS    ((uint32_t)0x4001244C)	 
#define DMA_BUFFER_SIZE     4*4	 
	 
void RCC_Configuration(void);
void SMG_GPIO_INIT(void);
void IO_GPIO_INIT(void);
void TIM3_init(void);
	 void TIM4_init(void);
void TIM2_init(void);
void TIM1_Init(void);
void ADC1_Configuration(void);	 
void ADC1_Init(void);
void ADC2_Init(void);
void PVD_init(void);

#ifdef  DAC_OUT_Enable
void DAC_Configuration(void);	
#endif

extern int16_t adc_dma_tab[DMA_BUFFER_SIZE]; 
extern int32_t SA_MaxValue;
extern int32_t SB_MaxValue;
	 
#ifdef __cplusplus
}
#endif
#endif /*__BSP_INIT_H */

