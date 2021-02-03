


/**
  *************************************************************************
  * @file    main.c
  * @author  LZY.Huaqian
  * @version V2.0.0
  * @date    2018/03/18 11.06
  * @brief
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

  /*************************     ϵͳͷ�ļ�����     *************************/
#include "stm8l15x_gpio.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_usart.h"
#include "stm8l15x_itc.h"
#include "common_cfg.h"
#include "stm8l15x_tim3.h"
#include "stm8l15x_tim4.h"

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

	/*************************     ȫ�ֽṹ�嶨��     *************************/

	/*************************      ȫ�ֱ�������      *************************/

	/*************************        ��������        *************************/

	/*************************        ����ʵ��        *************************/
#if 1
/* ���Ե�Ƭ���Ƿ���ת���� */
	void p_init_gpio(void)
	{
		/* ��ʼ��PA��Ϊ���١����졢�͵�ƽ */
	    GPIO_Init(LED_GPS_PORT, LED_GPS_PIN, GPIO_Mode_Out_PP_High_Fast);
		GPIO_Init(LED_REGISTER_PORT, LED_REGISTER_PIN, GPIO_Mode_Out_PP_High_Fast);
		GPIO_Init(LED_SHAKE_PORT, LED_SHAKE_PIN, GPIO_Mode_Out_PP_High_Fast);
		GPIO_Init(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Mode_In_PU_No_IT);  // ���ؽ�����Ϊ��������
		GPIO_Init(GPS_EN_PORT, GPS_EN_PIN, GPIO_Mode_Out_PP_High_Slow);        //����GPS LNA�Ŵ����ߵ�ƽ

	//	GPIO_Init(GPIOA, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Slow); // ��λ���ų�ʼ��Ϊ�ߵ�ƽ
}
#endif

	/**
  * ***********************************************************************
  * @brief	��ʼ�� ϵͳ ʱ��
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void p_init_clk(void)
	{
		/* CLK�Ĵ�����λ */
		CLK_DeInit();
	
		/* ��Ƶ��1,����Ƶ */
		CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

		/* ʹ��ʱ���л� */
		CLK_SYSCLKSourceSwitchCmd(ENABLE); 
	
		/* ʹ���ڲ�����ʱ��HSI */
		CLK_HSICmd(ENABLE);

		/* ��ʱ���л�ΪHSI */
		CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

		/* �ȴ�LSIʱ��Դ�ȶ� */
		while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI);
		
		
	}

	/**
  * ***********************************************************************
  * @brief	��ʼ��BN220����
  *
  *	@param  BaudRate:
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */

	void p_init_BC20_uart(uint32_t BaudRate)
	{
		/* ��ʼ��RXDΪ�������룬TXDΪ��©��� */
		GPIO_Init(BC20_RXD_PORT, BC20_RXD_PIN, GPIO_Mode_In_FL_No_IT);  // ΪʲôҪ���óɷ��жϣ�
		GPIO_Init(BC20_TXD_PORT, BC20_TXD_PIN, GPIO_Mode_Out_OD_HiZ_Fast);  //��©���

		/* �򿪴���1ʱ�� peripheral(��Χ��) */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

		/* ��ʼ������1 */
		USART_Init(USART1, BaudRate, USART_WordLength_8b, USART_StopBits_1, \
			USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));

		/* �ر�ȫ���ж� */
		disableInterrupts();

		/* ��������ж����ȼ� */
		ITC_SetSoftwarePriority(USART1_RX_IRQn, ITC_PriorityLevel_2);

		/* ��ȫ���ж� */
		enableInterrupts();

		/* ʹ�ܴ���1�����жϣ� �رտ��кͷ����ж�*/
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

		/* ʹ�ܴ���1 */
		USART_Cmd(USART1, ENABLE);
	}

	/**
  * ***********************************************************************
  * @brief	��ʼ����ʱ��
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void p_init_timer(void)
	{
		/* ʹ�ܶ�ʱ��ʱ�� */
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);

		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
		/*
		���ö�ʱ��3ʱ��ģʽ(�Ǽ���)
		16M/128/1250
		��ʱ���ж�10ms
		*/
		TIM3_TimeBaseInit(TIM3_Prescaler_128, TIM3_CounterMode_Up, (uint16_t)1249);
		TIM3_ClearFlag(TIM3_FLAG_Update);  // �������жϱ�־λ
		TIM3_ITConfig(TIM3_IT_Update, ENABLE); // ʹ������ж�


		/*
		���ö�ʱ��4ʱ��ģʽ(LED)
		16M/1024/(155+1) = 100Hz
		�ж�һ��10ms
		*/
		TIM4_TimeBaseInit(TIM4_Prescaler_1024, (uint16_t)155);
		TIM4_ClearFlag(TIM4_FLAG_Update);
		TIM4_ITConfig(TIM4_IT_Update, ENABLE);

		/*
		���ö�ʱ��2ʱ��ģʽ(�Ǽ���)
		16M/128/1250
		��ʱ���ж�10ms
		*/
		TIM3_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Up, (uint16_t)1249);
		TIM3_ClearFlag(TIM2_FLAG_Update);  // �������жϱ�־λ
		TIM3_ITConfig(TIM2_IT_Update, ENABLE); // ʹ������ж�


		/* �ر�ȫ���ж� */
		disableInterrupts();

		/* �����ж����ȼ� */
		ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_1);
		ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_3);

		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_1);
		
		/* ��ȫ���ж� */
		enableInterrupts();

		/* ʹ�ܶ�ʱ�� */
		TIM4_Cmd(ENABLE);
		TIM3_Cmd(ENABLE);

		TIM2_Cmd(ENABLE);
	}

	/**
  * ***********************************************************************
  * @brief	�ж��豸�Ƿ���Ҫ��ʽ��
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void p_init_judgle_reset(void)
	{
		//DISABLE_BN220();
		uint8_t rx_status = 1;
		uint8_t tx_status = 1;
		uint8_t i = 0, j = 0;

		GPIO_Init(ME3616_RXD_PORT, ME3616_RXD_PIN, GPIO_Mode_Out_PP_High_Fast);  // ��������Ϊ ��
		GPIO_Init(ME3616_TXD_PORT, ME3616_TXD_PIN, GPIO_Mode_In_FL_No_IT);     // ���� ����Ϊ����
		GPIO_SetBits(ME3616_RXD_PORT, ME3616_RXD_PIN);

		delay_10ms(50);

		//GPIO_Init(ME3616_TXD_PORT, ME3616_TXD_PIN, GPIO_Mode_Out_PP_High_Fast);
		if (GPIO_ReadInputDataBit(ME3616_TXD_PORT, ME3616_TXD_PIN))
		{
			/* �رտ��ƵƵĶ�ʱ�� */
			TIM4_Cmd(DISABLE);
			//GPIO_ResetBits(LED_NB_PORT, LED_NB_PIN);   // ����NB����
			GPIO_ResetBits(LED_GPS_PORT, LED_GPS_PIN);                //����GPS״̬��
			GPIO_ResetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);      //����ע��״̬��
			GPIO_ResetBits(LED_SHAKE_PORT, LED_SHAKE_PIN);            //�����𶯵�


			for (i = 0; i < 30; ++i)
			{

				if (rx_status)
				{
					GPIO_ResetBits(ME3616_RXD_PORT, ME3616_RXD_PIN);
					rx_status = 0;
				}
				else
				{
					GPIO_SetBits(ME3616_RXD_PORT, ME3616_RXD_PIN);
					rx_status = 1;
				}

				delay_10ms(10);

				if (GPIO_ReadInputDataBit(ME3616_TXD_PORT, ME3616_TXD_PIN))
				{
					tx_status = 1; 
				}
				else
				{
					tx_status = 0;
				}

				softuart_printf("tx_statusis %d\r\n", tx_status);
				softuart_printf("rx_status %d\r\n", rx_status);

				if (rx_status == tx_status)
				{
					softuart_printf("j is %d\r\n", j);
					++j;
				}
				else
				{
					softuart_printf("j is NULL ");
				}

				/* ��״̬ */
				if (i > 10 && i < 20)
				{
					GPIO_ResetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);      //����ע��״̬��
				}
				else if (i >= 20) 
				{
					GPIO_ResetBits(LED_SHAKE_PORT, LED_SHAKE_PIN);            //�����𶯵�
				}
			}


			if (j >= 25) 
			{
				GPIO_ResetBits(LED_GPS_PORT, LED_GPS_PIN);          //����GPS״̬��
				p_ccfg_reset();                         // ��ʽ��
				p_proto_gen_server_key();               //���ɷ�������Կ
			}

			GPIO_SetBits(LED_GPS_PORT, LED_GPS_PIN);
			GPIO_SetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);
			GPIO_SetBits(LED_SHAKE_PORT, LED_SHAKE_PIN);

			TIM4_Cmd(ENABLE);
		}


		softuart_printf("before load m_flg_alarm is %d\r\n", g_run_paramter.m_flg_alarm);
		p_ccfg_load();      //��������
		softuart_printf("after load m_flg_alarm is %d\r\n", g_run_paramter.m_flg_alarm);
	}

	 /**
  * ***********************************************************************
  * @brief	��ʼ��LED��
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	 void p_init_led(void)
	 {
		 /* ȫ��Ϩ�� */
		 GPIO_Init(LED_SHAKE_PORT, LED_SHAKE_PIN, GPIO_Mode_Out_PP_High_Slow);
		 GPIO_Init(LED_REGISTER_PORT, LED_REGISTER_PIN, GPIO_Mode_Out_PP_High_Slow);
		 GPIO_Init(LED_GPS_PORT, LED_GPS_PIN, GPIO_Mode_Out_PP_High_Slow);
		 //GPIO_Init(LED_NB_PORT, LED_NB_PIN, GPIO_Mode_Out_PP_High_Slow);
	 }

	 void p_init_IWDG(void)
	 {
#ifdef USE_IWDG
		 /* ��ȡOption byte�ж������Ź����õ����� */
		 if (FLASH_ReadByte(0x0004808) != 0x02)						// �жϿ��Ź�������״̬�Ƿ�ֹͣ
		 {
			 softuart_printf("IWDG IS RUNing\r\n");

			 FLASH->CR2 = 0x80;										// ����OPT�� ��ѡ���ֽڽ���д����ʹ��
			 FLASH_Unlock(FLASH_MemType_Data);						// flash�Ƚ���
			 /* Wait until Data EEPROM area unlocked flag is set*/
			 while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);

			 FLASH_ProgramByte(0x0004808, 0x02);						// �޸�Option byte�е�OPT[3]
			 FLASH_WaitForLastOperation(FLASH_MemType_Data);
			 FLASH_Lock(FLASH_MemType_Data);							// flash��������
			 FLASH->CR2 = 0x00;										// ����OPT�� ��ѡ���ֽڽ���д�����ָ���ֹ
	    }
		 else
		 {

			 softuart_printf("IWDG IS STOP\r\n");
		
		 }
		 /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
		 IWDG_Enable();

		 /* IWDG timeout equal to 214 ms (the timeout may varies due to LSI frequency
		 dispersion) */
		 /* Enable write access to IWDG_PR and IWDG_RLR registers */
		 IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

		 /* IWDG configuration: IWDG is clocked by LSI = 38KHz */
		 IWDG_SetPrescaler(IWDG_Prescaler_256);

		 /* IWDG timeout equal to 214.7 ms (the timeout may varies due to LSI frequency dispersion) */
		 /* IWDG timeout = (RELOAD_VALUE + 1) * Prescaler / LSI
		 = (254 + 1) * 32 / 38 000
		 = 214.7 ms */
		 IWDG_SetReload(254);

		 /* Reload IWDG counter */
		 IWDG_RELOAD();

#endif

	
}


	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




