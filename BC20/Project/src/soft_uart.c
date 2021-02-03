
/**
  *************************************************************************
  * @file    soft_uart.c
  * @author  CJC.Huaqian
  * @version V2.0.0
  * @date    2020/03/27
  * @brief
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

/*************************     ϵͳͷ�ļ�����     *************************/
#include<stdio.h>
#include<stdarg.h>
/*************************     ˽��ͷ�ļ�����     *************************/
#include "soft_uart.h"

/*************************      ���������ռ�      *************************/

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

/*************************        ȫ��ö��        *************************/

/*************************       ȫ�ֺ궨��       *************************/

/*************************     ȫ�ֽṹ�嶨��     *************************/

/*************************      ȫ�ֱ�������      *************************/
unsigned char uart_TX3_Buffer[COM_TX1_Lenth] = { 0x00 };
const unsigned char MSK_TAB[9] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0 }; // �����ǽ���λ����һ��8λ�������λ����λȡ��
unsigned char Tx_bit = 0;
unsigned char Tx_data = 0;
COMx_Define COM3 = { 0x00 };
/*************************        ��������        *************************/

/*************************        ����ʵ��        *************************/

/* ��ģ�⴮�ڵĳ�ʼ�� */
void p_softuart_init(void)
{
		/* ʹ��ʱ�� */
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);                       // ������ʱ��2��ʱ��

		/* ģ��IO�ڵĳ�ʼ��  TX -- ��������� */
		//GPIO_Init(RXD_PORT, RXD_PIN, GPIO_Mode_In_PU_IT);
		GPIO_Init(TXD_PORT, TXD_PIN, GPIO_Mode_Out_PP_High_Fast);

		/* ��ʱ����ʼ�� */
	//	TIM2_DeInit();
		TIM2_TimeBaseInit(TIM2_Prescaler_8, TIM2_CounterMode_Up, 210);    
		TIM2_ARRPreloadConfig(ENABLE);
		TIM2_ClearFlag(TIM2_FLAG_Update);                                             // �������жϱ�־λ                                     // ��������ж�
		TIM2_ITConfig(TIM2_IT_Update, ENABLE);

		/* �ر�ȫ���ж� */
		disableInterrupts();
		/* �����ж����ȼ� */
		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_3);      // �������ȼ�
		/* ��ȫ���ж� */
		enableInterrupts();

		TIM2_Cmd(ENABLE);
}



	/* ģ�ⷢ�ͺ��� */
	//   ������һ���ֽ�д�ɷ���һ�����飨�ַ�����
void Softruan_DataBuf_Push(char dat)	                // д�뷢�ͻ��壬ָ��+1
{
	if (COM3.TX_read != COM3.TX_write + 1)
	{
			uart_TX3_Buffer[COM3.TX_write] = dat;	    // װ���ͻ���
			++COM3.TX_write;
			if (COM3.TX_write >= COM_TX1_Lenth)
				COM3.TX_write = 0;
	}

	if (COM3.B_TX_busy == FALSE)		            // ��־����
	{
			COM3.B_TX_busy = TRUE;		                // ��־æ
			Tx_bit = 0xff;                              // ѡ����巽ʽ
			TIM_SEND_ENABLE();			                // TODO: ���������ж�
	}
}

void p_swut_send_timing(void)
{
	if (COM3.B_TX_busy == TRUE)
	{
			/* ��ʱ���жϽ�����busy��־λ */
			switch (Tx_bit)
			{
			case 0:
				clr_Tx;               // ������ʼλ
				break;
			case DATA_LENGTH + 1:
			case DATA_LENGTH + 2:
			case DATA_LENGTH + 3:
			case DATA_LENGTH + 4:
				set_Tx;	             // ����ֹͣλ
				break;

			case 0xff:
				break;              // ��Ҫ����װ������
			default:
			{
				if (Tx_data & MSK_TAB[Tx_bit - 1])
				{
					set_Tx;
				}
				else
				{
					clr_Tx;
				}
			}
			break;
			}
			if (Tx_bit > DATA_LENGTH + STOP_BITS)
			{
				if (COM3.TX_read != COM3.TX_write)
				{
					Tx_data = uart_TX3_Buffer[COM3.TX_read];
					if (++COM3.TX_read >= COM_TX1_Lenth)
					{
						COM3.TX_read = 0;
					}
				}
				else
				{
					COM3.B_TX_busy = FALSE;

				}

				Tx_bit = 0;
			}
			else
			{
				++Tx_bit;
			}

	}

	else
	{
		/* �رն�ʱ�� */
		//TIM2_Cmd(DISABLE);
		TIM_SEND_DISABLE();
	}
}


/* �����Լ���printf���� --- ������������Ĵ����� */
void softuart_string(char *p)
{
	while (*p)
	{
		Softruan_DataBuf_Push(*p++);
	}

}

void softuart_printf(const char *fmt, ...)

{
#ifdef SOFTUART
	va_list ap;
	char string[256];//�����ڲ���չRAM���Ƿ����ⲿRAM�����ܳ����ڲ���չRAM��С(�˴�Ϊ1024)

	va_start(ap, fmt);
	vsprintf(string, fmt, ap);//�˴�Ҳ����ʹ��sprintf�������÷���࣬�Լ��޸ļ��ɣ��˴���ȥ
	softuart_string(string);
	va_end(ap);
#endif // SOFTUART	
}



#if 0
/* ��ʱ��2 --- ����ģ�⴮�ڵķ��� */
//INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
	/* ʱ��100US */
	p_swut_send_timing();                // ��������
	TIM2_ClearFlag(TIM2_FLAG_Update);    // ���
}
#endif
	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




