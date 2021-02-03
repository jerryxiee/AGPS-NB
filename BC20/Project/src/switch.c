
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

  /*************************     系统头文件包含     *************************/


/*************************     私有头文件包含     *************************/
#include "switch.h"

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

/*************************        函数声明        *************************/

/*************************        函数实现        *************************/

/**
  * ***********************************************************************
  * @brief	无效化水银开关
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
		/* 关闭振动开关功能，设置为推挽输出低，省电 */
		GPIO_Init(SWITCH_TWO_PORT, SWITCH_TWO_PIN, GPIO_Mode_Out_PP_Low_Slow);
		GPIO_Init(SWITCH_THREE_PORT, SWITCH_THREE_PIN, GPIO_Mode_Out_PP_Low_Slow);
	}

/**
  * ***********************************************************************
  * @brief	初始化水银开关
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

		/* 两个开关不能全部关闭 */
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
	  * @brief	水银开关控制
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

		/* 开关2 */
		if (TRUE == sw1)
		{
			/* 打开 */
			GPIO_Init(SWITCH_TWO_PORT, SWITCH_TWO_PIN, GPIO_Mode_In_FL_IT);  // 设置为浮空输入
			EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);  // 配置所有端口pin2引脚？外中断2？
		}
		else
		{
			/* 关闭 */
			GPIO_Init(SWITCH_TWO_PORT, SWITCH_TWO_PIN, GPIO_Mode_Out_PP_Low_Slow);
		}

		/* 开关3 */
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
  * @brief	检测震动
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
		
		if (g_run_paramter.m_switch_trigger_cnt >=1)   // 工作中一次就报警
		{
			softuart_printf("g_run_paramter.m_switch_trigger_cnt is %d \r\n", g_run_paramter.m_switch_trigger_cnt);
			/* 清空震动次数 */
			g_run_paramter.m_switch_trigger_cnt = 0;

			/* 标志报警 */
			g_run_paramter.m_flg_alarm = TRUE;

			/* 更新设备的工作时间 */
	
			softuart_printf(" g_run_paramter.m_tim_pwr_off_delay is %d\n", g_run_paramter.m_tim_pwr_off_delay);
				g_run_paramter.m_tim_gps_rep = g_run_paramter.m_tim_pwr_off_delay;
			
		
		//	g_run_paramter.m_tim_gps_rep = 900;

			/* 更新单次报警持续时间 */
			g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;

			/* 判断GPS是否有打开 */
			if (FALSE == g_ccfg_config.m_en_gps_rep)
			{
				g_ccfg_config.m_en_gps_rep = TRUE;
			
			}
		}
	}

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




