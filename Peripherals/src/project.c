
///**
//  ********************************  STM32F0x1  *********************************
//  * @�ļ���     �� differentialDC.c
//  * @����       �� HarryZeng
//  * @��汾     �� V1.5.0
//  * @�ļ��汾   �� V1.0.0
//  * @����       �� 2017��05��11��
//  * @ժҪ       �� ���ݴ���
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  ������־:
//  2017-05-11 V1.0.0:��ʼ�汾
//  ----------------------------------------------------------------------------*/
///* ������ͷ�ļ� --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "project.h"
#include "key.h"
#include "display.h"
#include "SelfStudy.h"
#include "menu.h"
#include "flash.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
/*DSP��궨�壺ARM_MATH_CM0*/



uint32_t DealyBaseTime=8;
uint16_t DEL = 4;

int16_t HI = 1000;
int16_t LO = 700;
bool timeflag = 0;
uint8_t LastOUT1=0;
uint8_t RegisterA=0;
uint8_t RegisterB=1;
uint8_t RegisterC=0;
uint8_t OUT1=0;
uint8_t OUT2=0;
uint8_t OUT3=0;
int16_t OUT2_TimerCounter=0;
uint16_t OUT2_Timer=0;
int16_t 	OUT3_TimerCounter=0;
uint16_t OUT3_Timer=0;
uint32_t ADCRawValue=0;
int32_t ADC_Display=0;
int32_t DACOUT = 1000;
uint32_t CPV = 0;

Button_STATUS KEY=ULOC;
uint8_t 		ConfirmShortCircuit=0;
uint32_t   	ShortCircuitCounter = 0;
uint32_t   	ShortCircuitLastTime = 0;
uint8_t 		KeyMappingFlag=0;
uint8_t  		EventFlag=0x00; 
uint8_t 		ShortCircuit=0;
uint32_t 		ShortCircuitTimer=0;
uint8_t  		displayModeONE_FLAG = 0; 
uint8_t 		DisplayModeNo=0;

void SetRegisterA(uint32_t GetADCValue);
void DisplayMODE(void);
void DisplayModeONE(void);
void DisplayModeTWO(void);
void DisplayModeTHIRD(void);
void DisplayModeFour(void);
void ShortCircuitProtection(void);
void SetOUT1Status(void);
void SetOUT2Status(void);
void SetOUT3Status(void);
void ButtonMapping(void);
void DEL_Set(void);
void DisplayModeONE_STD(void);
void DisplayModeONE_AREA(void);
void ResetParameter(void);
void Test_Delay(uint32_t ms);
uint32_t ADCDispalyProcess(uint32_t *ADC_RowValue,uint16_t Length);
uint8_t CheckDust(void);

extern int8_t PERCENTAGE;
extern int16_t ATT100;
extern uint16_t FSV;
extern int32_t SV;
extern uint8_t SelftStudyflag;
extern int8_t DSC;
/*----------------------------------�궨��-------------------------------------*/

uint8_t DustFlag=0;

int32_t 	S[4];
uint8_t 	S_Index=0;
uint8_t 	S_Flag=0;

int32_t 	SX[4];
int32_t 	SX_Final[32];
uint8_t 	SX_Flag;
uint8_t 	SX_Index=0;


int32_t 	S_Final=0; /*S-SET*/
uint8_t 	S_Final_FinishFlag=0;

uint32_t 	S32=0;
uint8_t 	S32_Flag=0;

int32_t 	S_SET=0;
uint32_t 	S_Selft=0;

int16_t 	Threshold=1000;

int16_t 	DX=0;
int16_t 	Last_DX=0;
int16_t 	Min_DX=0;
uint8_t		DX_Flag=1;

uint8_t 	TX_Index=0;
int32_t 	TX=0;
int32_t 	TX_Signal[8];
int16_t 	FX=0;

uint32_t 	Display_Signal[256];
uint32_t 	Display_Signal_Index=0;
uint32_t 	Display_Signal_Flag=0;
uint32_t 	DisplayADCValue_Sum=0;
/*ȥ�������Сֵ��ʣ�µ�������ƽ��*/
uint32_t DeleteMaxAndMinGetAverage(uint32_t *ary,uint8_t Length,uint32_t *Max,uint32_t *Min)
{
		int j;
		uint32_t Sum=0;
		*Max = ary[0];
		*Min = ary[0];

    for (j=1; j<Length; j++)
    {
        if (*Max<ary[j])
            *Max = ary[j];

        if (*Min>ary[j])
            *Min = ary[j];
		}
		
		for (j=0; j<Length; j++)
    {
			Sum += ary[j];
		}
		//return (Sum-*Max-*Min)/(Length-2);
		return (Sum)/(Length);
}
/*���ܺ�*/
void GetSum(uint32_t *SUM,uint32_t *arry,uint8_t arryLength)
{
		int j;
		for (j=0; j<arryLength; j++)
    {
			*SUM += arry[j];
		}
}

/*��ƽ��*/
void GetAverage(uint32_t *Average,uint32_t *arry,uint8_t arryLength)
{
		int j;
		uint32_t sum;
		for (j=0; j<arryLength; j++)
    {
			sum+= arry[j];
			
		}
		*Average  = sum / j;
		
}

/*���㺯��*/
void ClearData(uint32_t *ary,uint8_t Length)
{
		int j;
		for (j=0; j<Length; j++)
    {
			*ary = 0x00;
			ary++;/*ָ����λ*/
		}
}

/*��¼CPV������OUT�����*/
void CPV_SET_OUT(void)
{
		//uint8_t OUT1_LED,OUT2_LED,OUT3_LED;
		if(LastOUT1==0&&OUT1==1)
		{
			CPV++;
			if(CPV>=SV) /*����������ﵽԤ���趨��SV�����㣬OUT2���һ���ߵ�ƽ*/
			{
				OUT2 = 1;
				//CPV = 0;
			}
			if(CPV>=FSV) /*����������ﵽԤ���趨��FSV�����㣬OUT3���һ���ߵ�ƽ*/
			{
				OUT3 = 1;
				CPV = 0;
			}
		}
		LastOUT1 = OUT1;
//		/*��ʾOUT1��OUT2��״̬*/
		SMG_DisplayOUT_STATUS(OUT1,OUT2,OUT3);
}


uint8_t PWMCounter=0;
uint32_t S1024=0;
uint16_t S1024_Index=0;
uint32_t S1024_Sum=0;

#define S1024_Or_S8192  1024
void JudgeDX(void)
{
		/*�жϻҲ�Ӱ��̶�*/
	if(displayModeONE_FLAG ==0)
	{
		if(RegisterA) //STD����Mode
		{
			
			//S1024_Sum = S1024_Sum + S_Final;
			S1024_Sum = S1024_Sum + SX_Final[SX_Index];
			S1024_Index++;
			if(S1024_Index>=S1024_Or_S8192)
			{
				S1024_Index = 0;
				S1024 = S1024_Sum / S1024_Or_S8192; //���S1024
				
				if(DSC)                 //modifiy 20180105
					DX = S_SET - S1024;					//modifiy 20171230
				else
					DX = 0;   
				
				S1024_Sum  = 0;
			}
			if(DX<=-2000) //�����ʣ�����С��-1500ʱ��û�б�����С����
				DX=-2000;
			else if(DX>=2000) 
				DX = 2000;   
//			
//			Last_DX = DX;
//			
//			if(Last_DX<Min_DX) 	/*���ڼ�¼��С��DXֵ*/  
//				Min_DX = DX;
			
				//DustFlag = CheckDust(); /*�Ҳ�������أ�DUST*/
		}
	}
	else if(displayModeONE_FLAG ==1) /*Area Mode*/
	{
		if(RegisterC) 
		{
			//S1024_Sum = S1024_Sum + S_Final;
			S1024_Sum = S1024_Sum + SX_Final[SX_Index];
			S1024_Index++;
			if(S1024_Index>=S1024_Or_S8192)
			{
				S1024_Index = 0;
				S1024 = S1024_Sum / S1024_Or_S8192; //���S1024
				if(DSC)                 //modifiy 20180105
					DX = S_SET - S1024;					//modifiy 20171230
				else
					DX = 0; 
				S1024_Sum  = 0;
			}
			if(DX<=-2000) //�����ʣ�����С��-1500ʱ��û�б�����С����
				DX=-2000;
			else if(DX>=2000) 
				DX = 2000; 
//			
//			Last_DX = DX;
//			
//			if(Last_DX<Min_DX) 	/*���ڼ�¼��С��DXֵ*/  
//				Min_DX = DX;
//			
//				//DustFlag = CheckDust(); /*�Ҳ�������أ�DUST*/
		}
	}
}

uint16_t  RunCounter[100];
uint8_t		Runflag=0;
uint8_t		RunIndex=0;
void DMA1_Channel1_IRQHandler(void)
{
	uint32_t 	SX_Max=0;
	uint32_t 	SX_Min=0;

	uint32_t 	TX_Max=0;
	uint32_t 	TX_Min=0;

	uint32_t 	Dispaly_Max=0;
	uint32_t 	Dispaly_Min=0;
	
	int32_t tempdata;
	
 	/*�ж�DMA��������ж�*/ 
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)                        
	{
		if(PWMCounter<100)
			PWMCounter++;
		S[0] = 4095-adc_dma_tab[0];
		S[1] = 4095-adc_dma_tab[1];
		S[2] = 4095-adc_dma_tab[2];
		S[3] = 4095-adc_dma_tab[3];
		GetSum(&SX[S_Index++],S,4);/*�ĸ�ͨ���ܺ�*/
		S_Flag = 1;
		
		if(S_Index>3)	
		{					
				S_Index = 0;
				/*SX,FX*/
				tempdata = DeleteMaxAndMinGetAverage(SX,4,&SX_Max,&SX_Min);
				SX_Final[SX_Index] = tempdata - 3000;/*��ò�ȥ�������Сֵ����ʣ�����ݵ�ƽ��ֵ,��Ҫ��32��*/
				/*������λ*/
				if(SX_Final[SX_Index]>=9999) 
					SX_Final[SX_Index] = 9999;
				if(SX_Final[SX_Index]<=0) 
					SX_Final[SX_Index] = 0;
					
				/*���S�����ź�*/
				S_Final = SX_Final[SX_Index] + DX;	/*��������ź�ֵ �ź�ֵ����DX���õ���������Ϊ�����ź�ֵ*/
				if(S_Final>=9999) 
					S_Final=9999;
				if(S_Final<=0) 
					S_Final=0;
				S_Final_FinishFlag = 1;
			
								
				if(PWMCounter>50)/*���ڵ���50��PWM���壬�ſ�ʼ����RegisterA*/
					SetRegisterA(S_Final);/*�ж�RegisterA��״̬*/
				if(PWMCounter>80)
					SetOUT1Status();/*���ڵ���50��PWM���壬�ſ�ʼ����OUT���*/
				
				FX = (SX_Max-SX_Min);  /*���FX*/
				ClearData(SX,4);/*����*/
			
				/*TX*/
				GetSum(&TX_Signal[TX_Index++],&S_Final,1);/*�����ܺ�,TX*///20180106
				if(TX_Index>=6)
				{
					TX_Index = 0;
					DeleteMaxAndMinGetAverage(TX_Signal,6,&TX_Max,&TX_Min);
					TX = (TX_Max-TX_Min);/*���TX*/
					//TX = 0;
					ClearData(TX_Signal,8);/*����*/
				}
				
				/*��ʾ���ݼ���*/
				GetSum(&DisplayADCValue_Sum,&S_Final,1);
				Display_Signal_Index++;
				if(Display_Signal_Index>=256)
				{
					ADCRawValue = DisplayADCValue_Sum/256;
					Display_Signal_Index = 0;
					Display_Signal_Flag	=	1;
					DisplayADCValue_Sum = 0;
					
				}

				/*����CPV������OUT���*/
				CPV_SET_OUT();
				/*�жϻҲ�*/
				if(DX_Flag==1)
					JudgeDX();
				/*�ۼ�32������*/
				SX_Index++;
				if(SX_Index>31)
				{
					SX_Index = 0;
					SX_Flag = 1;
					/*��ѧϰ*/
					if(SelftStudyflag)
					{
						DX = 0;
						S1024 = 0;
						//GetAverage(&S_SET,SX_Final,32); /*��ѧϰ�����S-SET*/
						//Threshold = S_SET-80;   /*���·�ֵ*/
						S_SET = S_Final;
						
						if(displayModeONE_FLAG)//����ģʽ
						{
							if(DisplayModeNo==0)
							{
								HI = S_SET*(100-PERCENTAGE)/100;   /*���·�ֵ,=S-SET*(1-1%)=S-SET*0.99=S-SET*99/100*/;
								WriteFlash(HI_FLASH_DATA_ADDRESS,HI);
							}
							else if(DisplayModeNo==1)
							{
								LO =S_SET*(100-PERCENTAGE)/100;   /*���·�ֵ,=S-SET*(1-1%)=S-SET*0.99=S-SET*99/100*/;
								WriteFlash(LO_FLASH_DATA_ADDRESS,LO);
							}
						}
						else    //��׼ģʽ
						{
							Threshold = S_SET*(100-PERCENTAGE)/100;   /*���·�ֵ,=S-SET*(1-1%)=S-SET*0.99=S-SET*99/100*/
							WriteFlash(Threshold_FLASH_DATA_ADDRESS,Threshold);
						}
						SelftStudyflag = 0;
						
						WriteFlash(S_SET_FLASH_DATA_ADDRESS,S_SET);
					}
				}
				/*����ʱ��*/
				RunCounter[RunIndex] = TIM_GetCounter(TIM4);
				if(RunCounter[RunIndex] >=3200)
					Runflag = 1;
				RunIndex++;
		}
	}
	/*���DMA�жϱ�־λ*/	
	DMA_ClearITPendingBit(DMA1_IT_TC1); 
}

/*������ɺ�ADC���ݴ���*/
uint32_t  ADCDispalyProcess(uint32_t *ADC_RowValue,uint16_t Length)
{
	uint32_t DisplayADCValue=0;
	uint32_t SumADCValue=0;
	uint16_t k;
	for(k=0;k<Length;k++)
		SumADCValue += ADC_RowValue[k];
	
	DisplayADCValue = SumADCValue/(k);
	
	return DisplayADCValue;
}


/*����OUT1*/
/********************
*
*�жϳ�RegisterA״̬
*
**********************/
uint8_t GetRegisterAState(uint32_t ADCValue)
{
	uint8_t A;
		
	if(ADCValue>=Threshold)
		A = 1;
	else if(ADCValue<=Threshold-50)
		A = 0;
	return A;
}

/*	������ʾ*/
/********************
*
*��ʾADC��ֵ����
*
**********************/

void GetADCValue(void)
{
		uint32_t 	Dispaly_Max=0;
		uint32_t 	Dispaly_Min=0;
					/*������ʾ*/
			ADC_Display =ADCRawValue;
}


void Main_Function(void)
{
	GetEEPROM();
	ATTSet(ATT100);
	while(1)
	{
		if(0)
		{
			Dust_Display();
			GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);/*һֱ��OUT1����*/
		}
		else
		{

				/*��·����*/
				ShortCircuitProtection();

				while(ConfirmShortCircuit)
				{
						GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,Bit_RESET);
						GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_RESET);/*��������OUT*/ /*���ֶ�·����OUT��������*/
						GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_RESET);/*��������OUT*/ /*���ֶ�·����OUT��������*/
						if((ShortCircuitLastTime - ShortCircuitTimer)>=2000)
						{
								ConfirmShortCircuit = 0;
								ShortCircuitCounter = 0;
								ShortCircuit=0;
						}
				}
				
				/*������ʾģʽ*/
				DisplayMODE();
			
				/*��������*/
				ButtonMapping();
				
				/*OUT2���*/
				SetOUT2Status();
				/*OUT3���*/
				SetOUT3Status();
				
				if(KEY==ULOC)/*�жϰ����Ƿ�����*/
				{
				/*SET��ѧϰģʽ*/
				if((DisplayModeNo==0&&displayModeONE_FLAG==0)||(DisplayModeNo<=1&&displayModeONE_FLAG==1))
					selfstudy();
				/*Mode�˵�ģʽ*/
					menu();
				}
		}
	}
}

/*********************************
�Ҳ�������أ�DUST
**********************************/
uint8_t CheckDust(void)
{
	//if(DX>(S32/2))
	if(DX>=1501)
	{
		return 1;
	}
	else
		return 0;
}

/*******************************
*
*��ʾģʽ�л�
*
*******************************/

void DisplayMODE(void)
{
	if(ModeButton.Status ==Press && (DownButton.Status==Press || SetButton.Status == Press||UpButton.Status==Press))
	{
		
	}
	else
	{
		if(KEY==ULOC)
		{
			if(ModeButton.PressCounter==0)
			{
					DisplayModeNo = 0;
			}
			else if(ModeButton.Effect==PressShort && ModeButton.PressCounter==1&&DownButton.Status==Release&&UpButton.Status==Release)
			{
				DisplayModeNo = 1;
			}
			else if(ModeButton.Effect==PressShort && ModeButton.PressCounter==2&&DownButton.Status==Release&&UpButton.Status==Release)
			{
					DisplayModeNo = 2;
			}
			else if(ModeButton.Effect == PressShort && ModeButton.PressCounter==3 &&DownButton.Status==Release&&UpButton.Status==Release)
			{
					DisplayModeNo = 3;
			}
//			else if(ModeButton.Effect == PressShort && ModeButton.PressCounter==4 &&DownButton.Status==Release&&UpButton.Status==Release)
//			{
//				DisplayModeNo = 4;
//			}
			/********mode ���� ѭ��*******/
			if(ModeButton.Effect == PressShort && ModeButton.PressCounter==3 &&DownButton.Status==Release&&displayModeONE_FLAG==0)  //if need to display Mode_Four,PressCounter=4
			{
					ModeButton.PressCounter = 0;
					DisplayModeNo = 0;
			}
			else if(ModeButton.Effect == PressShort && ModeButton.PressCounter==4 &&DownButton.Status==Release&&displayModeONE_FLAG==1)//if need to display Mode_Four,PressCounter=5
			{
					ModeButton.PressCounter = 0;
					DisplayModeNo = 0;
			}
		}
		else
		{
			while(ModeButton.Status==Press||SetButton.Status==Press||DownButton.Status==Press||UpButton.Status==Press)
			{
				if(ModeButton.Effect==PressLong && UpButton.Status ==Release &&DownButton.Effect ==PressLong && DownButton.Status ==Press)
				{
					if(KEY == ULOC)
							KEY = LOC;
					else
					{
						KEY = ULOC;
						KeyMappingFlag = 0;
					}
					ModeButton.PressCounter = 0;
					while(ModeButton.Effect==PressLong && UpButton.Status ==Release &&DownButton.Effect ==PressLong && DownButton.Status ==Press)
					{
						ButtonMappingDisplay(1);
					}
					WriteFlash(KEY_FLASH_DATA_ADDRESS,KEY);
					ModeButton.Effect=PressNOEffect;
					ModeButton.PressCounter = 0;
					DownButton.PressCounter = 0;
					DownButton.Effect=PressNOEffect;
				}
				else
					ButtonMappingDisplay(1);
			}
		}
		/*��ʾ*/
		if((DisplayModeNo==0&&displayModeONE_FLAG==0)||(DisplayModeNo<=1&&displayModeONE_FLAG==1))
		{
			DisplayModeONE();
		}
		else if((DisplayModeNo==1&&displayModeONE_FLAG==0)||(DisplayModeNo==2&&displayModeONE_FLAG==1))
		{
			DisplayModeTWO();
		}
		else if((DisplayModeNo==2&&displayModeONE_FLAG==0)||(DisplayModeNo==3&&displayModeONE_FLAG==1))
		{
			DisplayModeTHIRD();
		}
		else if((DisplayModeNo==3&&displayModeONE_FLAG==0)||(DisplayModeNo==4&&displayModeONE_FLAG==1))
		{
			DisplayModeFour();
		}
	}
}

/*******************************
*
*��ʾģʽ1
*
*******************************/
int8_t LastDSC;
void DisplayModeONE(void)
{
	if(displayModeONE_FLAG)
	{
		if(DSC)
		{
			LastDSC = DSC;
			DSC = 0;  //����ģʽ�£�DXһֱΪ0
		}
		DisplayModeONE_AREA();
	}
	else
	{
		if(LastDSC)
		{
			LastDSC = 0;
			DSC = 1;
		}
		DisplayModeONE_STD();
	}
}


/*******************************
*
*��ʾģʽ1_DETECT_STD
*
*******************************/
uint32_t tempPress=0;
void DisplayModeONE_STD(void)
{
	static uint8_t lastCounter;
	static int16_t LastThreshold;
	
		/*�������ʾ*/
		SMG_DisplayModeONE(Threshold,ADC_Display);
	
	if(ModeButton.Status==Release && KeyMappingFlag==0 && KEY==ULOC)
	{
		/*Up Button*/
		LastThreshold = Threshold;
		if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
		{
			lastCounter = UpButton.PressCounter;
			UpButton.PressCounter = 0;
			Threshold = Threshold+1;
		}
		else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
		{				/*�����Ű���������ʱ�䳬������ʱ��*/
			UpButton.PressCounter = 0;
			if(UpButton.PressTimer<=KEY_LEVEL_1)
			{
				if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&tempPress == 1)
				{
					Threshold = Threshold+1;
					tempPress = 0;
				}
			}
			else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<=KEY_LEVEL_2)
			{
				if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&tempPress == 1)
				{
						tempPress = 0;
					Threshold = Threshold+2;
				}
			}
			else 
			{
				if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&tempPress == 1)
				{
						tempPress = 0;
					Threshold = Threshold+5;
				}
			}
		}	
		else
		{
			UpButton.Effect = PressShort;
		}
			/*Down Button*/
		if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
		{
			DownButton.PressCounter = 0;
			Threshold = Threshold-1;
		}
		else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
		{
			DownButton.PressCounter = 0;
			if(DownButton.PressTimer<KEY_LEVEL_1)
			{
				if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&tempPress == 1)
				{
					Threshold = Threshold-1;
					tempPress = 0;
				}
			}
			else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
			{
				if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&tempPress == 1)
				{
					Threshold = Threshold-2;
					tempPress = 0;
				}
			}
			else 
			{
				if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&tempPress == 1)
				{
					tempPress = 0;
					Threshold = Threshold-5;
				}
			}
		}
		else
		{
			DownButton.Effect = PressShort;
		}
		if(LastThreshold!=Threshold && DownButton.Status==Release && UpButton.Status==Release)
		{
			WriteFlash(Threshold_FLASH_DATA_ADDRESS,Threshold);
		}
	}
		if(Threshold>=9999)
				Threshold = 9999;
		else if(Threshold<=0)
				Threshold = 0;
}


/*******************************
*
*��ʾģʽ1_DETECT_AREA
*
*******************************/
void DisplayModeONE_AREA(void)
{
			static uint8_t lastCounter;
			static int16_t LastHIValue;
			static int16_t LastLOValue;
			
			/*HI display mode*/
			if(DisplayModeNo==0)
			{
				if(ModeButton.Status==Release && KeyMappingFlag==0 && KEY==ULOC)
				{
					/*Up Button*/
					LastHIValue = HI;
					if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
					{
						lastCounter = UpButton.PressCounter;
						UpButton.PressCounter = 0;
						HI = HI+1;
						if(HI>=9999)
								HI = 9999;
						SMG_DisplayModeONE_Detect_AREA_HI(1,HI,ADC_Display); /*��ʾ��ֵ*/
					}
					else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
					{				/*�����Ű���������ʱ�䳬������ʱ��*/
						UpButton.PressCounter = 0;
						if(UpButton.PressTimer<=KEY_LEVEL_1)
						{
							if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								HI = HI+1;
							}
						}
						else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<=KEY_LEVEL_2)
						{
							if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								HI = HI+2;
							}
						}
						else 
						{
							if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								HI = HI+5;
							}
						}
					if(HI>=9999)
							HI = 9999;
						SMG_DisplayModeONE_Detect_AREA_HI(1,HI,ADC_Display); /*��ʾ��ֵ*/
					}	
//					else
//					{
//						UpButton.Effect = PressShort;
//						SMG_DisplayModeONE_Detect_AREA_HI(timeflag,HI,ADC_Display); /*������ʾHI�뷧ֵ*/
//					}
						/*Down Button*/
					else if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
					{
						DownButton.PressCounter = 0;
						HI = HI-1;
						if(HI<=0)
							HI = 0;
						SMG_DisplayModeONE_Detect_AREA_HI(1,HI,ADC_Display); /*��ʾ��ֵ*/
					}
					else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
					{
						DownButton.PressCounter = 0;
						if(DownButton.PressTimer<KEY_LEVEL_1)
						{
							if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								HI = HI-1;
							}
						}
						else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
						{
							if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								HI = HI-2;
							}
						}
						else 
						{
							if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								HI = HI-5;
							}
						}
						if(HI<=0)
							HI = 0;
						SMG_DisplayModeONE_Detect_AREA_HI(1,HI,ADC_Display); /*��ʾ��ֵ*/
					}
					else
					{
						DownButton.Effect = PressShort;
						SMG_DisplayModeONE_Detect_AREA_HI(timeflag,HI,ADC_Display); /*������ʾHI�뷧ֵ*/
					}
					if(LastHIValue!=HI && DownButton.Status==Release && UpButton.Status==Release)
					{
						WriteFlash(HI_FLASH_DATA_ADDRESS,HI);
					}
				}
			}
			
			/*LO display mode*/
			else if(DisplayModeNo==1)
			{
				if(ModeButton.Status==Release && KeyMappingFlag==0 && KEY==ULOC)
				{
					/*Up Button*/
					LastLOValue = LO;
					if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
					{
						lastCounter = UpButton.PressCounter;
						UpButton.PressCounter = 0;
						LO = LO+1;
						if(LO>=9999)
							LO = 9999;
						SMG_DisplayModeONE_Detect_AREA_LO(1,LO,ADC_Display);/*��ʾ��ֵ*/
					}
					else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
					{				/*�����Ű���������ʱ�䳬������ʱ��*/
						UpButton.PressCounter = 0;
						if(UpButton.PressTimer<=KEY_LEVEL_1)
						{
							if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								LO = LO + 1;
							}
						}
						else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<=KEY_LEVEL_2)
						{
							if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								LO = LO + 2;
							}
						}
						else 
						{
							if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								LO = LO + 5;
							}
						}
						if(LO>=9999)
							LO = 9999;						
						SMG_DisplayModeONE_Detect_AREA_LO(1,LO,ADC_Display);/*��ʾ��ֵ*/
					}	
//					else
//					{
//						UpButton.Effect = PressShort;
//						SMG_DisplayModeONE_Detect_AREA_LO(timeflag,LO,ADC_Display);/*������ʾLO�뷧ֵ*/
//					}
						/*Down Button*/
					else if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
					{
						DownButton.PressCounter = 0;
						LO = LO-1;
						if(LO<=0)
							LO = 0;
						SMG_DisplayModeONE_Detect_AREA_LO(1,LO,ADC_Display);/*��ʾ��ֵ*/
					}
					else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
					{
						DownButton.PressCounter = 0;
						if(DownButton.PressTimer<KEY_LEVEL_1)
						{
							if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								LO = LO - 1;
							}
						}
						else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
						{
							if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								LO = LO - 2;
							}
						}
						else 
						{
							if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&tempPress == 1)
							{
								tempPress = 0;
								LO = LO - 5;
							}
						}
						if(LO<=0)
							LO = 0;
						SMG_DisplayModeONE_Detect_AREA_LO(1,LO,ADC_Display);/*��ʾ��ֵ*/
					}
					else
					{
						DownButton.Effect = PressShort;
						SMG_DisplayModeONE_Detect_AREA_LO(timeflag,LO,ADC_Display);/*������ʾLO�뷧ֵ*/
					}
					if(LastLOValue!=LO && DownButton.Status==Release && UpButton.Status==Release)
					{
						WriteFlash(LO_FLASH_DATA_ADDRESS,LO);
					}
				}
			}
	
	
	
}

/*******************************
*
*��ʾģʽ2
*
*******************************/
void DisplayModeTWO(void)
{
	static uint8_t lastCounter;
		/*�������ʾ*/
		SMG_DisplayModeTWO(RegisterB);
	
		/*Up Button*/
		if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
		{
			lastCounter = UpButton.PressCounter;
			UpButton.PressCounter = 0;
			if(RegisterB==0)
				RegisterB = 1;
			else 
				RegisterB=0;
			WriteFlash(RegisterB_FLASH_DATA_ADDRESS,RegisterB);
		}

		/*Down Button*/
		if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
		{
			DownButton.PressCounter = 0;
			if(RegisterB==0)
				RegisterB = 1;
			else 
				RegisterB=0;
			WriteFlash(RegisterB_FLASH_DATA_ADDRESS,RegisterB);
		}
}

/*******************************
*
*��ʾģʽ3
*
*******************************/
void DisplayModeTHIRD(void)
{
		/*�������ʾ*/
		SMG_DisplayModeTHIRD(CPV);
		/*����Ϊ���������������ֹ��Ӱ�쵽��ʾģʽ4*/
		DownButton.PressCounter = 0;
		UpButton.PressCounter = 0;
}

/*******************************
*
*��ʾģʽ4
*
*******************************/
void DisplayModeFour(void)
{
	DEL_Set();
}

/*******************************
*
*OUT1_DEL Ӧ��ֵ����
*
*******************************/
void DEL_Set(void)
{
		static uint8_t lastCounter;
		uint16_t LastDEL;

			/*��ʾSHOT,��˸*/
			SMG_DisplayMenuTwo_DEL_SET(DEL,0);
	
			/*Up Button*/
			LastDEL = DEL;
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				DEL = DEL+1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
						DEL = DEL+1;
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
						DEL = DEL+1;
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
						DEL = DEL+1;
				}
			}	
			else
			{
				UpButton.Effect = PressShort;
			}	
				/*Down Button*/
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				DownButton.PressCounter = 0;
				DEL = DEL-1;
			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
						DEL = DEL-1;
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
						DEL = DEL-1;
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
						DEL = DEL-1;
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
			}
			if(LastDEL!=DEL && DownButton.Status==Release && UpButton.Status==Release)
			{
				//WriteFlash(DEL_FLASH_DATA_ADDRESS,DEL);
			}
			if(DEL>=500)
				DEL = 500;
			else if(DEL<=4)
					DEL = 4;
	//	}
}

/*******************************
*
*Set RegisterA value
*
*******************************/
void SetRegisterA(uint32_t ADCTestValue)
{
//	TX = 0; //debug
//	DX = 0;
	if(displayModeONE_FLAG) /*AREA Mode*/
	{
		if(ADCTestValue>=LO+TX && ADCTestValue<=HI-TX-80-HI/128)
			RegisterA = 1;
		else if(((ADCTestValue>=(HI+TX))&&(ADCTestValue<=9999))||(ADCTestValue<=(LO-TX-80-LO/128)))	//20180106
			RegisterA = 0;
		
		/*RegisterC*/
		if(ADCTestValue>=HI+TX)
			RegisterC = 1;
		else if(ADCTestValue<=HI-TX-HI/128)
			RegisterC = 0;
	}	
	else  /*STD Mode*/
	{
				if(ADCTestValue>=Threshold+TX)  //20171231
					RegisterA = 1;
				else if(ADCTestValue<=(Threshold-TX-40-Threshold/256))/*20171223*/ //2018-1-5 �޸ĳ�-80,//20180106 �ĳ�-40 /256
					RegisterA = 0;
	}
}


/*******************************
*
*�ж�OUT1�����״̬
*
*******************************/
uint8_t SHOTflag=0;
void SetOUT1Status(void)
{
	if(ShortCircuit!=1)/*���Ƕ�·����������²��ж�OUT1�����*/
	{
		/*ͬ������*/
		OUT1=!(RegisterB^RegisterA);
		if(OUT1_Mode.DelayMode==TOFF)
		{
			//GPIOA->ODR ^= GPIO_Pin_9;
			if(OUT1==0)
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				OUT1_Mode.DelayCounter = 0;
			}
			else
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				OUT1_Mode.DelayCounter=0;
			}
		}
		/*OFFD*/
		else if(OUT1_Mode.DelayMode==OFFD)
		{
			if(OUT1==0)
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				OUT1_Mode.DelayCounter = 0;
			}
			else
			{
				if(OUT1_Mode.DelayCounter>(OUT1_Mode.DelayValue*DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				}
			}
		}
		/*ON_D*/
		else if(OUT1_Mode.DelayMode==ON_D)
		{
			if(OUT1==0)
			{
				if(OUT1_Mode.DelayCounter>(OUT1_Mode.DelayValue*DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				}
			}
			else
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				OUT1_Mode.DelayCounter = 0;
			}
		}
		/*SHOT*/
		else if(OUT1_Mode.DelayMode==SHOT)
		{
			if(OUT1==0 || SHOTflag == 1)
			{
				if(OUT1_Mode.DelayCounter<(OUT1_Mode.DelayValue*DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
					SHOTflag = 1;
				}
				else
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
					SHOTflag = 0;
				}
			}
			else
			{
				OUT1_Mode.DelayCounter = 0;
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
			}
		}
	}
}
/*******************************
*
*�ж�OUT2�����״̬
*
*******************************/
void SetOUT2Status(void)
{
	if(ShortCircuit!=1)/*���Ƕ�·����������²��ж�OUT2�����*/
	{
		if(OUT2)
		{
			GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_SET);/*����*/
		}
	 if(OUT2_TimerCounter >=80)
		{
			OUT2 = 0;
			OUT2_TimerCounter = 0;  /*��ȡ��ǰʱ��*/
			GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_RESET);/*80ms������*/
		}
	}
}

/*******************************
*
*�ж�OUT3�����״̬
*
*******************************/
void SetOUT3Status(void)
{
	if(ShortCircuit!=1)/*���Ƕ�·����������²��ж�OUT2�����*/
	{
		if(OUT3)
		{
			GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_SET);/*����*/
		}
	 if(OUT3_TimerCounter >=160)
		{
			OUT3 = 0;
			OUT3_TimerCounter = 0;  /*��ȡ��ǰʱ��*/
			GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_RESET);/*80ms������*/
		}
	}
}

/*******************************
*
*��·����
*
*******************************/
void ShortCircuitProtection(void)
{
	uint8_t SCState;
	
	/*��ȡSC���ŵ�״̬*/
	if(ShortCircuit!=1)
	{
		SCState = GPIO_ReadInputDataBit(SC_GPIO_Port ,SC_Pin);
		if((BitAction)SCState == Bit_RESET)
		{
			/*����FB_SC*/
			ShortCircuit= 1;
		}
		else
		{
			ShortCircuit = 0;
			ConfirmShortCircuit = 0;
		}
	}
	if(ShortCircuit && ShortCircuitCounter>=100)
	{
		ConfirmShortCircuit=1;
		GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,Bit_RESET);
		GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_RESET);/*��������OUT*/
		GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_RESET);/*��������OUT*/
		ShortCircuitTimer = ShortCircuitLastTime;
	}	
}

/*******************************
*
*��������
*
*******************************/
void ButtonMapping(void)
{
	/*��������*/
	if(ModeButton.Effect==PressLong && UpButton.Status ==Release &&DownButton.Effect ==PressLong && DownButton.Status ==Press)
	{
		if(KEY == ULOC)
				KEY = LOC;
		else
			KEY = ULOC;
		ModeButton.PressCounter = 0;
		while(ModeButton.Effect==PressLong && UpButton.Status ==Release &&DownButton.Effect ==PressLong && DownButton.Status ==Press)
		{
			ButtonMappingDisplay(1);
			KeyMappingFlag = 1;
		}
		WriteFlash(KEY_FLASH_DATA_ADDRESS,KEY);
		ModeButton.Effect=PressNOEffect;
		ModeButton.PressCounter = 0;
		DownButton.PressCounter = 0;
		DownButton.Effect=PressNOEffect;

	}
	/*������ʼ��*/
	else if(ModeButton.Effect==PressLong &&SetButton.Effect ==PressLong && SetButton.Status ==Press &&UpButton.Status ==Release && DownButton.Status ==Release)
	{
		takeoffLED();
		while((ReadButtonStatus(&ModeButton)) == Press && (ReadButtonStatus(&SetButton) == Press))
		{
			EraseFlash();			
			while((ReadButtonStatus(&ModeButton)) == Press && (ReadButtonStatus(&SetButton) == Press))
					ButtonMappingDisplay(2);
		}
	
		ResetParameter();
		
		ModeButton.PressCounter = 0;
		ModeButton.Effect=PressNOEffect;
		DownButton.PressCounter = 0;
		DownButton.Effect=PressNOEffect;
		UpButton.PressCounter = 0;
		UpButton.Effect=PressNOEffect;
		Test_Delay(1000); 
		
	}
	/*����������*/
	else if(ModeButton.Effect==PressLong && UpButton.Status ==Press &&UpButton.Effect ==PressLong && DownButton.Status == Release)
	{
		CPV = 0;
		while(ModeButton.Effect==PressLong && UpButton.Status ==Press &&UpButton.Effect ==PressLong && DownButton.Status == Release)
		{
			ButtonMappingDisplay(3);
		}
		ModeButton.Effect=PressNOEffect;
		ModeButton.PressTimer = 0;
		ModeButton.PressCounter = 0;
		UpButton.PressCounter = 0;
		UpButton.Effect=PressNOEffect;
		Test_Delay(1000); 
	}	
}

 void Test_Delay(uint32_t ms)
{
	uint32_t i;

	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < ms*100; i++);
}
/*******************************
*
*��ȡEEPROM����
*
*******************************/

//	uint32_t _test_send_buf[30];
//	uint32_t _test_recv_buf[30];
//	short _test_size = 30;

//	char statusW=2;
//	char statusR=2;

uint32_t ProgramRUNcounter=0;


void GetEEPROM(void)
{
		
//			//OUT1_Mode.DelayMode 	= *(__IO uint32_t*)(OUT1_Mode_FLASH_DATA_ADDRESS);
//			OUT1_Mode.DelayValue 	= *(__IO uint32_t*)(OUT1_Value_FLASH_DATA_ADDRESS);
//			CSV 									= *(__IO uint32_t*)(CSV_FLASH_DATA_ADDRESS);
//			Threshold 						= *(__IO uint32_t*)(Threshold_FLASH_DATA_ADDRESS);
//			DACOUT 								= *(__IO uint32_t*)(DACOUT_FLASH_DATA_ADDRESS);
//			KEY 									= *(__IO uint32_t*)(KEY_FLASH_DATA_ADDRESS);
//			RegisterB 						= *(__IO uint32_t*)(RegisterB_FLASH_DATA_ADDRESS);
//			DEL 									= *(__IO uint32_t*)(DEL_FLASH_DATA_ADDRESS);
//			HI 										= *(__IO uint32_t*)(HI_FLASH_DATA_ADDRESS);
//			LO 										= *(__IO uint32_t*)(LO_FLASH_DATA_ADDRESS);
//			displayModeONE_FLAG 	= *(__IO uint32_t*)(DETECT_FLASH_DATA_ADDRESS);


			OUT1_Mode.DelayMode 	= ReadFlash(OUT1_Mode_FLASH_DATA_ADDRESS);
			OUT1_Mode.DelayValue 	= ReadFlash(OUT1_Value_FLASH_DATA_ADDRESS);
			SV 										= ReadFlash(SV_FLASH_DATA_ADDRESS);
			Threshold 						= ReadFlash(Threshold_FLASH_DATA_ADDRESS);
			ATT100 								= ReadFlash(ATT100_FLASH_DATA_ADDRESS);
			KEY 									= ReadFlash(KEY_FLASH_DATA_ADDRESS);
			RegisterB 						= ReadFlash(RegisterB_FLASH_DATA_ADDRESS);
			FSV 									= ReadFlash(FSV_FLASH_DATA_ADDRESS);
			HI 										= ReadFlash(HI_FLASH_DATA_ADDRESS);
			LO 										= ReadFlash(LO_FLASH_DATA_ADDRESS);
			displayModeONE_FLAG 	= ReadFlash(DETECT_FLASH_DATA_ADDRESS);
			PERCENTAGE 						= ReadFlash(PERCENTAGE_FLASH_DATA_ADDRESS);
			S_SET 								= ReadFlash(S_SET_FLASH_DATA_ADDRESS);
			DSC 									= ReadFlash(DSC_FLASH_DATA_ADDRESS);
}

/*****************************
*
*��ʼ�����в���
*
****************************/
void ResetParameter(void)
{
		SV = 900;
		Threshold = 1000;
		KEY = ULOC;
		OUT1_Mode.DelayMode = TOFF;
		OUT1_Mode.DelayValue = 10;
		ATT100 = 100;
		DEL = 4;
		RegisterB = 1;
		HI = 1000;
		LO = 700 ;
		displayModeONE_FLAG = 0;
		PERCENTAGE = 1;
		DSC = 1;
		
		WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
		Test_Delay(50); 
		WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
		Test_Delay(50); 
		WriteFlash(SV_FLASH_DATA_ADDRESS,SV);
		Test_Delay(50); 
		WriteFlash(Threshold_FLASH_DATA_ADDRESS,Threshold);
		Test_Delay(50); 
		WriteFlash(ATT100_FLASH_DATA_ADDRESS,ATT100);
		Test_Delay(50); 
		WriteFlash(KEY_FLASH_DATA_ADDRESS,KEY);
		Test_Delay(50); 
		WriteFlash(RegisterB_FLASH_DATA_ADDRESS,RegisterB);
		Test_Delay(50); 
		WriteFlash(FSV_FLASH_DATA_ADDRESS,FSV);
		Test_Delay(50); 
		WriteFlash(HI_FLASH_DATA_ADDRESS,HI);
		Test_Delay(50); 
		WriteFlash(LO_FLASH_DATA_ADDRESS,LO);
		Test_Delay(50); 
		WriteFlash(DETECT_FLASH_DATA_ADDRESS,displayModeONE_FLAG);
		Test_Delay(50);
		WriteFlash(PERCENTAGE_FLASH_DATA_ADDRESS,PERCENTAGE);
		Test_Delay(50);
		WriteFlash(S_SET_FLASH_DATA_ADDRESS,S_SET);
		Test_Delay(50);
		WriteFlash(DSC_FLASH_DATA_ADDRESS,DSC);
		
		ModeButton.Effect=PressNOEffect;
		ModeButton.PressTimer = 0;
		ModeButton.PressCounter = 0;
		SetButton.Effect=PressNOEffect;
		SetButton.PressCounter = 0;

}