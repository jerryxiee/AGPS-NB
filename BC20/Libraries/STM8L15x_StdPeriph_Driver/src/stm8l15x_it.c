/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_it.h"
#include "stm8l15x_gpio.h"
#include "stm8l15x_exti.h"
#include "stm8l15x_iwdg.h"
#include "stm8l15x_tim3.h"
#include "stm8l15x_tim4.h"
#include "common_cfg.h"
#include "soft_uart.h"


/** @addtogroup STM8L15x_StdPeriph_Template
  * @{
  */
	
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define GPIO_RESET(port, pin)			port->ODR &= (uint8_t)(~pin)
#define GPIO_TOGGLEBIT(port, pin)       port->ODR ^= pin

/* Private variables ---------------------------------------------------------*/
static __IO uint8_t s_vu8_timer_10ms_index = 0x00;
static __IO uint8_t s_1s_reload_timer      = 0x00;
static __IO uint8_t s_alarm_led_index      = 0x00;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy interrupt routine
  * @par Parameters:
  * None
  * @retval 
  * None
*/
INTERRUPT_HANDLER(NonHandledInterrupt,0)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
#endif

/**
  * @brief TRAP interrupt routine
  * @par Parameters:
  * None
  * @retval 
  * None
*/
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief FLASH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler,1)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler,2)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler,3)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief RTC / CSS_LSE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	if (RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{


		do 
		{
			/* 旁路时间半个小时自减一次 */
			if (g_run_paramter.m_tim_passby
				&& 0 != (--g_run_paramter.m_tim_passby))
			{
				// --g_run_paramter.m_tim_passby;
				break;
			}

			/* 设备睡眠时间 */
			if (g_run_paramter.m_tim_sleep
				&& 0 != (--g_run_paramter.m_tim_sleep))
			{
				// --g_run_paramter.m_tim_sleep;
				break;
			}
			/* 睡眠结束,唤醒设备进行定是上报 */
			g_run_paramter.m_flg_wake_dev = TRUE;
			g_run_paramter.m_flg_nb_rep = TRUE;


			/* 重新装载下次睡眠持续的时间 */
			// g_run_paramter.m_tim_sleep = 2 * g_ccfg_config.m_tim_sleep;

		} while (0);

	}
	RTC_ClearITPendingBit(RTC_IT_WUT);
}
/**
  * @brief External IT PORTE/F and PVD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler,5)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler,6)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PORTD /PORTH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTID_H_IRQHandler,7)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN0 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	
}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */


	/* 水银开关二 */
	if (EXTI_GetITStatus(SWITCH_TWO_EXIT_PIN) != RESET)
	{
		if (GPIO_ReadInputDataBit(SWITCH_TWO_PORT, SWITCH_TWO_PIN) != SET)  // 读取水银1引脚
		{
		//  GPIO_WriteBit(GPIOA, GPIO_Pin_3, RESET);
			g_run_paramter.m_switch_trigger_cnt++;
			g_run_paramter.m_flg_en_alarm_led = TRUE; // 指示报警    
			g_run_paramter.m_flg_alarm = TRUE;
		}
	}

	EXTI_ClearITPendingBit(EXTI_IT_Pin2);
	
}

/**
  * @brief External IT PIN3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI3_IRQHandler,11)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */


	/* 外部中断唤醒后，重新喂狗 */

	/* 水银开关三 */
	if (EXTI_GetITStatus(SWITCH_THREE_EXIT_PIN) != RESET)
	{
		
		if (GPIO_ReadInputDataBit(SWITCH_THREE_PORT, SWITCH_THREE_PIN) != SET)
		{

		//	GPIO_WriteBit(GPIOA, GPIO_Pin_2, RESET);
			//IWDG_ReloadCounter();//外部中断唤醒后，重新喂狗
			g_run_paramter.m_switch_trigger_cnt++;
			g_run_paramter.m_flg_en_alarm_led = TRUE;// 指示报警    
			g_run_paramter.m_flg_alarm = TRUE;
		}
	}
	EXTI_ClearITPendingBit(EXTI_IT_Pin3);
}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler,13)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */

	/*p_swut_start_recv();
	
	EXTI_ClearITPendingBit(EXTI_IT_Pin5);*/
}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler,14)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
	*/
	
}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief LCD /AES Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(LCD_AES_IRQHandler,16)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler,17)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler,18)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	p_swut_send_timing();
	///*清除中断标志*/
	TIM2_ClearITPendingBit(TIM2_IT_Update);

}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler,20)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */

}

/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler,21)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	TIM3_ClearITPendingBit(TIM3_IT_Update);

	/* 1s 时间戳 */
	if (++s_1s_reload_timer >= 100)
	{
		s_1s_reload_timer = 0;
		g_vu32_1s_timer++;
	}

	/* 10ms时间戳 */
	g_vu32_10ms_timer++;



#ifdef USE_IWDG

	/* 100ms 喂一次狗 */
	if (g_vu32_10ms_timer-g_run_paramter.m_IWDG_reload_time < DEFAULT_IWDG_TIME && g_vu32_10ms_timer%10 == 0)
	{
		IWDG_RELOAD();
	}

#endif // USE_IWDG

}
/**
  * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler,22)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler,23)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	//p_swut_recv_timing();
	//TIM1_ClearFlag(TIM1_FLAG_Update);
}
/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler,24)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler,25)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	/* 10ms */
	TIM4_ClearITPendingBit(TIM4_IT_Update);

	s_vu8_timer_10ms_index++;
	if (s_vu8_timer_10ms_index >= 200)
	{
		s_vu8_timer_10ms_index = 0;
	}

	//if (GET_10MS_TIMER() - g_l620_net_timeout > 30000) // 300s无数据，复位
	//{
	//	p_task_enqueue(TASK_SEND_DATA, "NO SEVER DATA", NULL);
	//	p_task_enqueue(TASK_NBREST, NULL, NULL);
	//	g_l620_net_timeout = GET_10MS_TIMER();
	//}
#ifndef SOFTUART
	
	switch (led_status)
	{

	case NB_NET_UNREG:	// 网络未注册，慢闪
		if (0 == s_vu8_timer_10ms_index % 100)
		{
			GPIO_TOGGLEBIT(LED_REGISTER_PORT, LED_REGISTER_PIN);
		}

		break;

	case NB_CSQ_LOW:	// 信号弱，长灭一闪
		if (s_vu8_timer_10ms_index % 100 <= 5)
		{
			GPIO_RESET(LED_REGISTER_PORT, LED_REGISTER_PIN);
		}
		else
		{
			GPIO_SetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);
		}
		break;

	case NB_CSQ_HIGH:	// 信号强,常亮

		
		if ((GET_10MS_TIMER() - g_l620_net_timeout < 200) &(g_l620_net_timeout!=0)) // 5s内有udp反馈包，常亮一灭
		{
			if (0 == s_vu8_timer_10ms_index % 5)  
			{
				GPIO_TOGGLEBIT(LED_REGISTER_PORT, LED_REGISTER_PIN);
			}
			
		}
		else
		{
			GPIO_RESET(LED_REGISTER_PORT, LED_REGISTER_PIN);
		}

		break;
	default:
		break;
	}
#endif // !SOFTUART

	if (0 == s_vu8_timer_10ms_index % (TRUE == g_run_paramter.m_flg_en_unreg ? 5 : 100))
	{
		if (TRUE == g_run_paramter.m_gps_ok)
		{
			GPIO_RESET(LED_GPS_PORT, LED_GPS_PIN);
		}
		else 
		{
			GPIO_TOGGLEBIT(LED_GPS_PORT, LED_GPS_PIN);
		}


	}


	++s_alarm_led_index;

	if (s_alarm_led_index > 200)
	{
		s_alarm_led_index = 0x00;
		g_run_paramter.m_flg_en_alarm_led = FALSE;

		/* 保证闪烁后灯是关闭的 */
	GPIO_Init(LED_SHAKE_PORT, LED_SHAKE_PIN, GPIO_Mode_Out_PP_High_Slow);
	}



	/* 震动灯 */
	if (TRUE == g_run_paramter.m_flg_en_alarm_led
		&& 0 == s_alarm_led_index % 5)
	{
		GPIO_TOGGLEBIT(LED_SHAKE_PORT, LED_SHAKE_PIN);
	}

}
/**
  * @brief SPI1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler,26)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */		
}

/**
  * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler,27)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */

}

/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler,28)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */

	if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
	{
		/*产生了串口1接收中断*/
		static uint8_t gps_index=0;
		u8 data = USART_ReceiveData8(USART1);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		//	USART_ClearFlag(USART1, USART_FLAG_RXNE);

		g_buff.m_recv_timeout = GET_10MS_TIMER();
		
	
			do 
			{
				if (g_buff.m_recv_index >=511)
				{
					break;
                                }
				g_buff.m_recv_buff[g_buff.m_recv_index++] = data;
				//test
				//p_swut_write2buff(data);
			} while (0);
		
		//}

	}
}

/**
  * @brief I2C1 / SPI2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler,29)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/