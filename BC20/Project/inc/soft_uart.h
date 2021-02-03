
/**
  *************************************************************************
  * @file    soft_uart.h
  * @author  CJC.Huaqian
  * @version V2.0.0
  * @date    2020/03/18
  * @brief
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

  /*************************     ���ظ���������     *************************/
#ifndef __SOFT_UART_H__
#define __SOFT_UART_H__

/*************************     ϵͳͷ�ļ�����     *************************/
#include "stm8l15x.h"
#include "stm8l15x_it.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_itc.h"
#include "stm8l15x_exti.h"
/*************************     ˽��ͷ�ļ�����     *************************/

/*************************      ���������ռ�      *************************/

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

/*************************        ȫ��ö��        *************************/

/*************************       ȫ�ֺ궨��       *************************/
#define	COM_TX1_Lenth			128
#define	DATA_LENGTH				8		
#define STOP_BITS				1
/* ģ�⴮������ */
#define		TXD_PORT		LED_REGISTER_PORT
#define		TXD_PIN			LED_REGISTER_PIN
#define set_Tx  GPIOA->ODR |= LED_REGISTER_PIN;
#define clr_Tx  GPIOA->ODR &= (uint8_t)(~LED_REGISTER_PIN);
#define TIM_SEND_ENABLE()			do{	TIM2->CNTRH = 0; TIM2->CNTRL = 0 ;TIM2->CR1 |= (uint8_t)(TIM_CR1_CEN);  } while (0)
#define TIM_SEND_DISABLE()			do{	TIM2->CR1 &= (uint8_t)(~TIM_CR1_CEN);                               } while (0)
/*************************     ȫ�ֽṹ�嶨��     *************************/

typedef struct
{
	u8	TX_read;		//���Ͷ�ָ��
	u8	TX_write;		//����дָ��
	u8	B_TX_busy;		//æ��־

} COMx_Define;
/*************************      ȫ�ֱ�������      *************************/

/*************************        ��������        *************************/

extern void Softruan_DataBuf_Push(char dat);
extern void p_softuart_init(void);
extern void softuart_string(char *p);
extern void softuart_printf(const char *fmt, ...);
extern void p_swut_send_timing(void);
	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     ���ظ���������     *************************/
#endif



