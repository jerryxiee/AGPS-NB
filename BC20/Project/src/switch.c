
/**
  *************************************************************************
  * @file    switch.c
  * @author  LZY.Huaqian
  * @version V2.0.0
  * @date    2020/03/25
  * @brief
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

  /*************************     ϵͳͷ�ļ�����     *************************/


/*************************     ˽��ͷ�ļ�����     *************************/
#include "switch.h"

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

/**
  * ***********************************************************************
  * @brief	��Ч��ˮ������
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void switch_uninit(void)
	{
		/* �ر��񶯿��ع��ܣ�����Ϊ��������ͣ�ʡ�� */
		GPIO_Init(SWITCH_TWO_PORT, SWITCH_TWO_PIN, GPIO_Mode_Out_PP_Low_Slow);
		GPIO_Init(SWITCH_THREE_PORT, SWITCH_THREE_PIN, GPIO_Mode_Out_PP_Low_Slow);
	}

/**
  * ***********************************************************************
  * @brief	��ʼ��ˮ������
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void p_init_mercury(void)
	{

		/* �������ز���ȫ���ر� */
		if (FALSE == g_ccfg_config.m_en_sw1
			&& FALSE == g_ccfg_config.m_en_sw2)
		{
			g_ccfg_config.m_en_sw1 = TRUE;
			g_ccfg_config.m_en_sw2 = TRUE;
		}

		switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
		}

	/**
	  * ***********************************************************************
	  * @brief	ˮ�����ؿ���
	  *
	  *	@param  s:
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	void switch_ctrl(uint8_t sw1, uint8_t sw2)
	{
		disableInterrupts();

		/* ����2 */
		if (TRUE == sw1)
		{
			/* �� */
			GPIO_Init(SWITCH_TWO_PORT, SWITCH_TWO_PIN, GPIO_Mode_In_FL_IT);  // ����Ϊ��������
			EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);  // �������ж˿�pin2���ţ����ж�2��
		}
		else
		{
			/* �ر� */
			GPIO_Init(SWITCH_TWO_PORT, SWITCH_TWO_PIN, GPIO_Mode_Out_PP_Low_Slow);
		}

		/* ����3 */
		if (TRUE == sw2)
		{
			GPIO_Init(SWITCH_THREE_PORT, SWITCH_THREE_PIN, GPIO_Mode_In_FL_IT);
			EXTI_SetPinSensitivity(EXTI_Pin_3, EXTI_Trigger_Falling);
		}
		else
		{
			GPIO_Init(SWITCH_THREE_PORT, SWITCH_THREE_PIN, GPIO_Mode_Out_PP_Low_Slow);
		}

		enableInterrupts();
	}

	/**
  * ***********************************************************************
  * @brief	�����
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void switch_process(void)
	{
		
		if (g_run_paramter.m_switch_trigger_cnt >=1)   // ������һ�ξͱ���
		{
			softuart_printf("g_run_paramter.m_switch_trigger_cnt is %d \r\n", g_run_paramter.m_switch_trigger_cnt);
			/* ����𶯴��� */
			g_run_paramter.m_switch_trigger_cnt = 0;

			/* ��־���� */
			g_run_paramter.m_flg_alarm = TRUE;

			/* �����豸�Ĺ���ʱ�� */
	
			softuart_printf(" g_run_paramter.m_tim_pwr_off_delay is %d\n", g_run_paramter.m_tim_pwr_off_delay);
				g_run_paramter.m_tim_gps_rep = g_run_paramter.m_tim_pwr_off_delay;
			
		
		//	g_run_paramter.m_tim_gps_rep = 900;

			/* ���µ��α�������ʱ�� */
			g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;

			/* �ж�GPS�Ƿ��д� */
			if (FALSE == g_ccfg_config.m_en_gps_rep)
			{
				g_ccfg_config.m_en_gps_rep = TRUE;
			
			}
		}
	}

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




