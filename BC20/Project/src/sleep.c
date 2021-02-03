
/**
  *************************************************************************
  * @file    sleep.c
  * @author  LZY.Huaqian
  * @version V2.0.0
  * @date    2020/03/31
  * @brief
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

  /*************************     ϵͳͷ�ļ�����     *************************/


#include "stm8l15x.h"
#include "stm8l15x_pwr.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_rtc.h"
//#include "stm8l15x_iwdg.h"


/*************************     ˽��ͷ�ļ�����     *************************/
#include "sleep.h"
#include "common_cfg.h"
#include "initial.h"
#include "switch.h"
//#include "soft_uart.h"
//#include "L620.h"
//#include "adc.h"
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
#define		CLOSE_ALL_LED()			p_init_led()
	/*************************      ȫ�ֱ�������      *************************/

	/*************************        ��������        *************************/

	/*************************        ����ʵ��        *************************/

	static void lowlevel_init(void);

	static void clk_init(void);

	static void RTC_init(void);

	static void clear_ITPendingBit(void);

	static void wake_up_GPIO_init(void);

	static void no_use_gpio_init(void);
	static void lowlevel_restore(void);



	/**
	  * ***********************************************************************
	  * @brief	�豸��������
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	void dev_sleep(void)
	{
		uint32_t time = 0;

#ifndef USE_PSM
		/* NB��������ģʽ */
	//	p_nblc_open_sm(TRUE);
#endif // USE_PSM

		/* �ػ�ǰ������ */
		dev_shutdown_routine();

		

		lowlevel_init();

		/* ʹ��ȫ���ж� */
		enableInterrupts();

		/* ����RTC���ѻ����ⲿ�жϻ��ѣ������������ */

		do {

			/* ����ǰ����һ��ι�� */
			IWDG_RELOAD();

			/* δ������ */
			halt();

			g_run_paramter.m_switch_trigger_cnt = 0;

			CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
			TIM3_Cmd(ENABLE);


			/* �ж��Ƿ����񶯻��� */
			for (time = GET_10MS_TIMER() + 14; time > GET_10MS_TIMER();)
			{
				IWDG_RELOAD();
				/* ���140ms */
				if (g_run_paramter.m_switch_trigger_cnt >= g_ccfg_config.m_cnt_sw_trigger)
				{
					g_run_paramter.m_flg_alarm = TRUE;
					g_run_paramter.m_flg_wake_dev = TRUE;
					break;
				}
			}

			if (FALSE == g_run_paramter.m_flg_wake_dev)
			{
				CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);
				TIM3_Cmd(DISABLE);

				/* �رն�ʱ��3 ��ι�� */
				IWDG_RELOAD();
			}

		} while (FALSE == g_run_paramter.m_flg_wake_dev);

	

		/* �����豸 */
		lowlevel_restore();

		/* �������� */
		dev_boot_routine();



	}

	void ME3616_uinit(void)
	{

		
		
		/* ע��UDP */
		p_nblc_common_func(TRUE, "QLWCLOSE", "CLOSE OK", 500, NULL, TRUE);

		/* �ر�GPS��Դ */
		p_nblc_common_func(TRUE, "QGNSSC=0", NULL, 100, NULL, TRUE);


		p_nblc_common_func(TRUE, "CFUN=0", NULL, 100, NULL, TRUE);  // �����������


		 /* ģ��ػ�*/
		p_nblc_common_func(TRUE, "QPOWD=0", NULL, 500, NULL, TRUE);


		/* ���ڷ���ʼ�����رո��ù���   */
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
		USART_Cmd(USART1, DISABLE);

		
		GPIO_Init(BC20_TXD_PORT, BC20_TXD_PIN, GPIO_Mode_In_FL_No_IT);
		GPIO_Init(BC20_RXD_PORT, BC20_RXD_PIN, GPIO_Mode_In_FL_No_IT);



		/* ���Է��֣�����ߣ�����ͣ��������붼��ȸ��մ���������һ��  */
		GPIO_Init(BC20_POWER_PORT, BC20_POWER_PIN, GPIO_Mode_In_FL_No_IT);
		GPIO_Init(BC20_RESET_PORT, BC20_RESET_PIN, GPIO_Mode_In_FL_No_IT);
		GPIO_Init(BC20_PSM_PORT,    BC20_PSM_PIN , GPIO_Mode_In_FL_No_IT);


	}

	///* ����ʼ�����ٶȼ����� */
	//void Accelerometer_uinit(void)
	//{
	//	
	//	GPIO_Init(ACCLEROMETER_SCK_PORT,ACCLEROMETER_SCK_PIN, GPIO_Mode_Out_PP_Low_Slow);
	//	GPIO_Init(ACCLEROMETER_SDO_PORT, ACCLEROMETER_SD0_PIN, GPIO_Mode_Out_PP_Low_Slow);  
	//	GPIO_Init(ACCLEROMETER_INIT1_PORT, ACCLEROMETER_INIT1_PIN, GPIO_Mode_Out_PP_Low_Slow);
	//	GPIO_Init(ACCLEROMETER_INIT2_PORT, ACCLEROMETER_INIT2_PIN, GPIO_Mode_Out_PP_Low_Slow);
	//}
		



	/* δʹ�����ų�ʼ��	 16��*/
	void no_use_gpio_init(void)
	{

		GPIO_Init(GPIOA, GPIO_Pin_0| GPIO_Pin_1, GPIO_Mode_In_PU_No_IT); // NRST,SWIM ��������


		GPIO_Init(GPIOA, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Slow);  // 1
		GPIO_Init(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Slow); // 5
		GPIO_Init(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Slow);  // 4

		GPIO_Init(ACCLEROMETER_SCK_PORT, ACCLEROMETER_SCK_PIN, GPIO_Mode_Out_PP_High_Slow);
		GPIO_Init(ACCLEROMETER_SDO_PORT, ACCLEROMETER_SD0_PIN, GPIO_Mode_Out_PP_High_Slow);
		GPIO_Init(ACCLEROMETER_INIT1_PORT, ACCLEROMETER_INIT1_PIN, GPIO_Mode_Out_PP_Low_Slow);
		GPIO_Init(ACCLEROMETER_INIT2_PORT, ACCLEROMETER_INIT2_PIN, GPIO_Mode_Out_PP_Low_Slow);
	}
	/**
	  * ***********************************************************************
	  * @brief	�弶��ʼ��
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	static void lowlevel_init(void)
	{

		ME3616_uinit();	// 5��
		no_use_gpio_init(); // δʹ�����ų�ʼ�� 16

		/* �ر�����ָʾ�ƣ����䶨ʱ�� */
		TIM4_ClearITPendingBit(TIM4_IT_Update);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);
		TIM4_Cmd(DISABLE);
		CLOSE_ALL_LED();  // 3��   // 3+5+16+2=26  + VSS + VDD =28 



		clk_init();
		RTC_init();

	
		/* �ر�ʱ�����ʱ��3 */
		TIM3_ClearITPendingBit(TIM3_IT_Update);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);
		TIM3_Cmd(DISABLE);

		/* �رն�ʱ��3�� ����ι�� */
		IWDG_RELOAD();



		/* �رն�ʱ��2 */
		TIM2_ClearITPendingBit(TIM2_IT_Update);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);
		TIM2_Cmd(DISABLE);



	
	//	Accelerometer_uinit();


		/* ˮӡ�������ų�ʼ�� */
		wake_up_GPIO_init();
		clear_ITPendingBit();
	}

	

	/**
	  * ***********************************************************************
	  * @brief	�л�ʱ��Դ�����ڲ�����ʱ��ת�����ڲ�����ʱ��
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	static void clk_init(void)
	{
		/*����LSIʱ��*/
		CLK_LSICmd(ENABLE);
		/*ʹ��ʱ��Դѡ��*/
		CLK_SYSCLKSourceSwitchCmd(ENABLE);
		/*ѡ��ʱ��Դ��Ƶ��*/
		CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
		/*�л�ʱ��ԴΪLSI*/
		CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);
		/*�ȴ�LSIʱ��Դ�ȶ�*/
		while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_LSI);

		/*�ر�HSI*/
		CLK_HSICmd(DISABLE);

		/*ʧ�ܿ��ٻ���*/
		CLK_HaltConfig(CLK_Halt_FastWakeup, DISABLE);

		PWR_FastWakeUpCmd(DISABLE);
		PWR_UltraLowPowerCmd(ENABLE);
	}

	/**
	  * ***********************************************************************
	  * @brief	��ʼ��RTCʱ��
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	static void RTC_init(void)
	{
		/*����RTCʱ��ԴΪLSI��64��Ƶ*/
		CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_64);

		/*�ȴ�LSi�ȶ�*/
		while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);

		/*ʹ��RTCʱ��*/
		CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

		/*RTC����ʱ��16��Ƶ*/
		RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);

		/*ʹ�ܻ����ж�*/
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		/*�趨���Ѽ���ֵ*/
		RTC_SetWakeUpCounter(65000);
		//RTC_SetWakeUpCounter(1110);		// 30s
		//RTC_SetWakeUpCounter(44531);//2017-2-22�޸�RTC��ʱʱ��   20s
		//RTC_SetWakeUpCounter(22266);//10min��37�μ���Ϊ1s��һ�μ���Ϊ27.027ms
        // RTC_SetWakeUpCounter(370);//10s����һ��
		/*ʹ��RTC����*/
		RTC_WakeUpCmd(ENABLE);
	}

	/**
	  * ***********************************************************************
	  * @brief	����жϱ�־λ
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	static void clear_ITPendingBit(void)
	{


	}

	/**
	  * ***********************************************************************
	  * @brief	��������ǰ����GPIO
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: ���õĶ˿�����Ϊ���������ģʽ
					  ������������Ϊ���������ģʽ�ض�
					  ������������Ϊ��©�����ģʽ
	  * ***********************************************************************
	  */
	static void wake_up_GPIO_init(void)
	{
		/*���ⲿ�жϱ�־λ*/
		EXTI_ClearITPendingBit(SWITCH_TWO_EXIT_PIN);
		EXTI_ClearITPendingBit(SWITCH_THREE_EXIT_PIN);
	}

	/**
	  * ***********************************************************************
	  * @brief	�弶��ʼ����ԭ
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	static void lowlevel_restore(void)
	{
		RTC_WakeUpCmd(DISABLE);

		p_init_clk();
		p_init_timer();
		p_softuart_init();
	//	p_init_gpio();
	//	p_init_judgle_reset();

		p_init_BC20_uart(9600);
		p_init_BC20_GPIO();

		p_init_mercury();
		dev_boot_routine(); // ����������
		 

		softuart_printf("the g_ccfg_config.m_dev_guid is %s\r\n", g_ccfg_config.m_dev_guid);
#if 0
		p_init_clk();

		/* ��ʼ��LED�� */
		p_init_led();

		p_init_timer();

#ifdef USE_PSM
	//	p_init_gpio_psm_wake();
#else
		p_init_ME3616_GPIO();
#endif


		p_init_mercury();
#endif 
	}

	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




