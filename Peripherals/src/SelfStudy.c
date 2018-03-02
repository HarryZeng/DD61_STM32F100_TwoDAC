
///**
//  ********************************  STM32F0x1  *********************************
//  * @文件名     ： SelftStudy.c
//  * @作者       ： HarryZeng
//  * @库版本     ： V1.5.0
//  * @文件版本   ： V1.0.0
//  * @日期       ： 2017年11月24日
//  * @摘要       ： 数据处理
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  更新日志:
//  2017-11-24 V1.0.0:初始版本
//  ----------------------------------------------------------------------------*/
///* 包含的头文件 --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "project.h"
#include "key.h"
#include "SelfStudy.h"
#include "flash.h"
#include "display.h"
#include "stm32f10x_tim.h"

/*第一次SET按键按下处理函数*/
uint8_t  ATTcalibration(void) ;

/*第二次SET按键按下处理函数*/
void GetMAXADCValue(void);

uint8_t SelftStudyflag=0;
uint8_t ATTcalibrationFlag=0;
uint8_t 	selfDisplayEndFlay=0;
uint32_t  SET1ADC_Value=0;

extern int16_t adc_dma_tab[4];
extern  uint8_t DX_Flag;
void selfstudy(void)
{
	uint8_t OUT1_STATUS,OUT2_STATUS,OUT3_STATUS;
	

	if(SetButton.Status == Press && ModeButton.Status==Press)
	{
		
	}
	else
	{
			/*第一次进入SET模式*/
			while(SetButton.Status == Press )     //只要在显示模式下第一次按下SET按键
			{	
				DX_Flag = 0;
					/*保持OUT1的状态*/
					OUT1_STATUS = GPIO_ReadInputDataBit(OUT1_GPIO_Port,OUT1_Pin);/*获取当前的OUT1状态*/
				GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,(BitAction)OUT1_STATUS);/*保持着OUT1状态*/
					OUT2_STATUS = GPIO_ReadInputDataBit(OUT2_GPIO_Port,OUT2_Pin);/*获取当前的OUT2状态*/
				GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,(BitAction)OUT2_STATUS);/*保持着OUT1状态*/
//					OUT3_STATUS = GPIO_ReadInputDataBit(OUT3_GPIO_Port,OUT3_Pin);/*获取当前的OUT3状态*/
//				GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,(BitAction)OUT3_STATUS);/*保持着OUT1状态*/
				
				/*按着按键3秒内*/
				SMG_DisplaySET_Step_1_Mode(0,Threshold);
				if(SetButton.Status == Release) 
				{
					DX_Flag = 1;
					break;
				}
				//Test_Delay(1000);
		//		/*这里要显示SET1*/
		//		if(EventFlag&Blink500msFlag) 
		//		{
		//			EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
		//			SMG_DisplaySET_Step_1_Mode(0,ADCValue);
		//		}
				//EventFlag = EventFlag &(~ADVtimeFlag);
				//GetADCValue(&SET1ADC_Value);
				
	
						
					/*3秒到了,闪烁提醒*/
					//if(EventFlag&Blink500msFlag) 
					//{
					//	EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
						//if(EventFlag&ADVtimeFlag) //按下的3秒内，一直定时采集ADC值
						//{
							//EventFlag = EventFlag &(~ADVtimeFlag);  //清楚标志位
							//GetADCValue(&SET1ADC_Value);		//定时ADC采样
							
					//	}
							
					//}
					else if(SetButton.Effect == PressLong && SetButton.Status==Press ) /*按键达到3秒后，第一次进入自学习，等待第二次按下SET 3秒*/
					{	
						DX_Flag = 0;
						ATTcalibration();
						
						/*3秒到了，并释放了按键*/
						
						/*一直等待第二次SET的按下*
						**************************/
						SetButton.LastCounter = SetButton.PressCounter;
						SetButton.Effect = PressShort;
						UpButton.PressCounter=0;
						UpButton.Effect=PressNOEffect;
						DownButton.PressCounter=0;
						DownButton.Effect=PressNOEffect;
						ModeButton.PressCounter=0;
						ModeButton.Effect=PressNOEffect;
						//selfDisplayEndFlay =0;
						//GetMAXADCValue();
						while(SetButton.Status==Press)
						{
							DX_Flag = 1;
							SMG_DisplaySET_Step_1_Mode(2,Threshold);
							SetButton.Effect = PressShort;
						}
					}
				
			}
	}
}


///*在一系列的ADCvalue中寻找最大的ADV MAX*/
//uint32_t 			ADCMAX=0;
//uint32_t 		NewThreshold=0;
//uint32_t 		TempMaxADCValue=0;
//void GetMAXADCValue(void)
//{
//		uint32_t 		TempADCValue=0;
////		static uint8_t lastCounter;
//	  TempMaxADCValue =0;
//		while(selfDisplayEndFlay==0)     //第二次按下SET按键
//		{
//			SMG_DisplaySET_Step_1_Mode(0,ADCValue);
//			while(SetButton.Status == Press )
//			{
//				//if(EventFlag&ADVtimeFlag) //按下的3秒内，一直定时采集ADC值
//				//{
//				//	EventFlag = EventFlag &(~ADVtimeFlag);  //清楚标志位
//					//GetADCValue(&TempADCValue);					//定时ADC采样
//					
//					if(TempADCValue>=TempMaxADCValue)
//							TempMaxADCValue = TempADCValue;
//				//}
//				/*这里要显示SET2*/
//				if(EventFlag&Blink500msFlag) 
//				{
//					EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
//					SMG_DisplaySET_Step_2_Mode(0,ADCValue,0);
//				}
//				
//				while(SetButton.Effect == PressLong) /*按下按键已经超过时间，而且没有释放，闪烁提醒*/
//				{	/*3秒到了,闪烁提醒*/
//					if(EventFlag&Blink500msFlag) 
//					{
//						EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
//						SMG_DisplaySET_Step_2_Mode(1,0,0);     
//					}				
//					while(SetButton.Effect == PressLong && SetButton.Status == Release) /*按键达到3秒后，结束第二次SET按键*/
//					{		/*3秒到了，并释放了按键*/
//					//	arm_max_f32(TempADCValue_f[1],i,&ADCMAX,&MAX_Index);  /*获得最大的ADCIN值*/
//						
//						ADCMAX = TempMaxADCValue;
//						NewThreshold = (CalibrateADCValue+ADCMAX)/2;				/*获得本次自学习的OUT1阈值*/
//						
//						GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,(BitAction)GPIO_ReadInputDataBit(OUT1_GPIO_Port,OUT1_Pin));
//						GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,(BitAction)GPIO_ReadInputDataBit(OUT2_GPIO_Port,OUT2_Pin));
//						
////							while(1)
////							{
////								SMG_DisplaySET_Step_2_Mode(2,ADCValue,NewThreshold);
//								
////								
////													/*Up Button*/
////							if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
////							{
////								lastCounter = UpButton.PressCounter;
////								UpButton.PressCounter = 0;
////								NewThreshold = NewThreshold+1;
////							}
////							else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
////							{				/*还按着按键，并且时间超过长按时间*/
////								UpButton.PressCounter = 0;
////								if(UpButton.PressTimer<1000)
////								{
////									if(UpButton.PressTimer%40==0)
////										NewThreshold = NewThreshold+1;
////								}
////								else if(UpButton.PressTimer>1000&&UpButton.PressTimer<3000)
////								{
////									if(UpButton.PressTimer%10==0)
////										NewThreshold = NewThreshold+1;
////								}
////								else 
////								{
////									if(UpButton.PressTimer%2==0)
////										NewThreshold = NewThreshold+1;
////								}
////							}	
////							else
////							{
////								UpButton.Effect = PressShort;
////							}
////								/*Down Button*/
////							if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
////							{
////								DownButton.PressCounter = 0;
////								NewThreshold = NewThreshold-1;
////							}
////							else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
////							{
////								DownButton.PressCounter = 0;
////								if(DownButton.PressTimer<1000)
////								{
////									if(DownButton.PressTimer%40==0)
////										NewThreshold = NewThreshold-1;
////								}
////								else if(DownButton.PressTimer>1000&&DownButton.PressTimer<3000)
////								{
////									if(DownButton.PressTimer%10==0)
////										NewThreshold = NewThreshold-1;
////								}
////								else 
////								{
////									if(DownButton.PressTimer%2==0)
////										NewThreshold = NewThreshold-1;
////								}
////							}
////							else
////							{
////								DownButton.Effect = PressShort;
////							}
////							if(NewThreshold>4000)
////									NewThreshold = 0;
////							else if(NewThreshold<=0)
////									NewThreshold = 4000;
//								
////								if(SetButton.Effect == PressShort)	/*再一次按下SET键，退出SET自学习模式*/
////								{
//									Threshold = NewThreshold;
//									selfDisplayEndFlay = 1;
//									SetButton.PressCounter = 0;					/*清楚按键次数*/
//									SetButton.Status = Release;					/*释放按键*/
//									SetButton.Effect = PressNOEffect;
//	
//									WriteFlash(Threshold_FLASH_DATA_ADDRESS,NewThreshold);
//									WriteFlash(DACOUT_FLASH_DATA_ADDRESS,DACOUT);

//								//	break;															/*结束SET自学习模式*/
//							//	}
//							}
//					}
//				}
//			}
//}

/*ADCIN的数据调零*/

extern uint8_t S_Final_FinishFlag;
extern uint32_t 	S[4];
extern uint32_t 	S_Final;


/*
	6us    	25us

		y							x
	384			->			1
	1600		->			100
公式转换得出约为：y=12x+372
*/
void ATTSet(uint16_t ATTvalue)
{
	uint16_t TimerValue=0;
	
	TimerValue=12*ATTvalue+372;
	
	TIM_SetCompare2(TIM2,TimerValue);
	
}

/*获取四个ADC通道采样后，求平均的值*/
uint8_t  JudgeSvalue(uint32_t *S_Value)
{
	uint8_t flag=0x00;
		if(S_Value[0]>4000) flag |= 0x01;/*0000 0001*/
	else flag &= 0xFE;/*1111 1110*/
	
		if(S_Value[1]>4000) flag |= 0x02;/*0000 0010*/
	else flag &= 0xFD;/*1111 1101*/
	
		if(S_Value[2]>4000) flag |= 0x04;/*0000 0100*/
	else flag &= 0xFB;/*1111 1011*/
	
		if(S_Value[3]>4000) flag |= 0x08;/*0000 1000*/
	else flag &= 0xF7;/*1111 0111*/
	
	if(flag==0x0f)
		return 1;
	else 
		return 0;
}


uint8_t  ATTcalibration(void) 
{
		
		SelftStudyflag = 1;
			/*等待DMA对ADC采样才完成*/
		while(SelftStudyflag)
		{
			
		}
		return 0;
}

