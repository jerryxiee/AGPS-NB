
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

/*************************     系统头文件包含     *************************/
#include<stdio.h>
#include<stdarg.h>
/*************************     私有头文件包含     *************************/
#include "soft_uart.h"

/*************************      导入命名空间      *************************/

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

/*************************        全局枚举        *************************/

/*************************       全局宏定义       *************************/

/*************************     全局结构体定义     *************************/

/*************************      全局变量定义      *************************/
unsigned char uart_TX3_Buffer[COM_TX1_Lenth] = { 0x00 };
const unsigned char MSK_TAB[9] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0 }; // 这里是进行位操作一共8位（从最低位进行位取）
unsigned char Tx_bit = 0;
unsigned char Tx_data = 0;
COMx_Define COM3 = { 0x00 };
/*************************        函数声明        *************************/

/*************************        函数实现        *************************/

/* 对模拟串口的初始化 */
void p_softuart_init(void)
{
		/* 使能时钟 */
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);                       // 开启定时器2的时钟

		/* 模拟IO口的初始化  TX -- 推挽输出高 */
		//GPIO_Init(RXD_PORT, RXD_PIN, GPIO_Mode_In_PU_IT);
		GPIO_Init(TXD_PORT, TXD_PIN, GPIO_Mode_Out_PP_High_Fast);

		/* 定时器初始化 */
	//	TIM2_DeInit();
		TIM2_TimeBaseInit(TIM2_Prescaler_8, TIM2_CounterMode_Up, 210);    
		TIM2_ARRPreloadConfig(ENABLE);
		TIM2_ClearFlag(TIM2_FLAG_Update);                                             // 清除溢出中断标志位                                     // 向上溢出中断
		TIM2_ITConfig(TIM2_IT_Update, ENABLE);

		/* 关闭全局中断 */
		disableInterrupts();
		/* 设置中断优先级 */
		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_3);      // 设置优先级
		/* 开全局中断 */
		enableInterrupts();

		TIM2_Cmd(ENABLE);
}



	/* 模拟发送函数 */
	//   将发送一个字节写成发送一个数组（字符串）
void Softruan_DataBuf_Push(char dat)	                // 写入发送缓冲，指针+1
{
	if (COM3.TX_read != COM3.TX_write + 1)
	{
			uart_TX3_Buffer[COM3.TX_write] = dat;	    // 装发送缓冲
			++COM3.TX_write;
			if (COM3.TX_write >= COM_TX1_Lenth)
				COM3.TX_write = 0;
	}

	if (COM3.B_TX_busy == FALSE)		            // 标志空闲
	{
			COM3.B_TX_busy = TRUE;		                // 标志忙
			Tx_bit = 0xff;                              // 选择具体方式
			TIM_SEND_ENABLE();			                // TODO: 触发发送中断
	}
}

void p_swut_send_timing(void)
{
	if (COM3.B_TX_busy == TRUE)
	{
			/* 定时器中断进入检测busy标志位 */
			switch (Tx_bit)
			{
			case 0:
				clr_Tx;               // 发送起始位
				break;
			case DATA_LENGTH + 1:
			case DATA_LENGTH + 2:
			case DATA_LENGTH + 3:
			case DATA_LENGTH + 4:
				set_Tx;	             // 发送停止位
				break;

			case 0xff:
				break;              // 需要重新装载数据
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
		/* 关闭定时器 */
		//TIM2_Cmd(DISABLE);
		TIM_SEND_DISABLE();
	}
}


/* 建立自己的printf函数 --- 可以设置任意的串口上 */
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
	char string[256];//访问内部拓展RAM，非访问外部RAM，不能超过内部拓展RAM大小(此处为1024)

	va_start(ap, fmt);
	vsprintf(string, fmt, ap);//此处也可以使用sprintf函数，用法差不多，稍加修改即可，此处略去
	softuart_string(string);
	va_end(ap);
#endif // SOFTUART	
}



#if 0
/* 定时器2 --- 用于模拟串口的发送 */
//INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler, 20)
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler, 19)
{
	/* 时间100US */
	p_swut_send_timing();                // 发送数据
	TIM2_ClearFlag(TIM2_FLAG_Update);    // 清空
}
#endif
	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




