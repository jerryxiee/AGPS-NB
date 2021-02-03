
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

  /*************************     系统头文件包含     *************************/


#include "stm8l15x.h"
#include "stm8l15x_pwr.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_rtc.h"
//#include "stm8l15x_iwdg.h"


/*************************     私有头文件包含     *************************/
#include "sleep.h"
#include "common_cfg.h"
#include "initial.h"
#include "switch.h"
//#include "soft_uart.h"
//#include "L620.h"
//#include "adc.h"
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
#define		CLOSE_ALL_LED()			p_init_led()
	/*************************      全局变量定义      *************************/

	/*************************        函数声明        *************************/

	/*************************        函数实现        *************************/

	static void lowlevel_init(void);

	static void clk_init(void);

	static void RTC_init(void);

	static void clear_ITPendingBit(void);

	static void wake_up_GPIO_init(void);

	static void no_use_gpio_init(void);
	static void lowlevel_restore(void);



	/**
	  * ***********************************************************************
	  * @brief	设备进入休眠
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
		/* NB进入休眠模式 */
	//	p_nblc_open_sm(TRUE);
#endif // USE_PSM

		/* 关机前的事务 */
		dev_shutdown_routine();

		

		lowlevel_init();

		/* 使能全局中断 */
		enableInterrupts();

		/* 除非RTC唤醒或者外部中断唤醒，否则继续休眠 */

		do {

			/* 休眠前进行一次喂狗 */
			IWDG_RELOAD();

			/* 未被唤醒 */
			halt();

			g_run_paramter.m_switch_trigger_cnt = 0;

			CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
			TIM3_Cmd(ENABLE);


			/* 判断是否是振动唤醒 */
			for (time = GET_10MS_TIMER() + 14; time > GET_10MS_TIMER();)
			{
				IWDG_RELOAD();
				/* 检测140ms */
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

				/* 关闭定时器3 后喂狗 */
				IWDG_RELOAD();
			}

		} while (FALSE == g_run_paramter.m_flg_wake_dev);

	

		/* 唤醒设备 */
		lowlevel_restore();

		/* 开机事务 */
		dev_boot_routine();



	}

	void ME3616_uinit(void)
	{

		
		
		/* 注销UDP */
		p_nblc_common_func(TRUE, "QLWCLOSE", "CLOSE OK", 500, NULL, TRUE);

		/* 关闭GPS电源 */
		p_nblc_common_func(TRUE, "QGNSSC=0", NULL, 100, NULL, TRUE);


		p_nblc_common_func(TRUE, "CFUN=0", NULL, 100, NULL, TRUE);  // 保存网络参数


		 /* 模块关机*/
		p_nblc_common_func(TRUE, "QPOWD=0", NULL, 500, NULL, TRUE);


		/* 串口反初始化，关闭复用功能   */
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
		USART_Cmd(USART1, DISABLE);

		
		GPIO_Init(BC20_TXD_PORT, BC20_TXD_PIN, GPIO_Mode_In_FL_No_IT);
		GPIO_Init(BC20_RXD_PORT, BC20_RXD_PIN, GPIO_Mode_In_FL_No_IT);



		/* 测试发现，推挽高，推挽低，上拉输入都会比浮空待机电流高一点  */
		GPIO_Init(BC20_POWER_PORT, BC20_POWER_PIN, GPIO_Mode_In_FL_No_IT);
		GPIO_Init(BC20_RESET_PORT, BC20_RESET_PIN, GPIO_Mode_In_FL_No_IT);
		GPIO_Init(BC20_PSM_PORT,    BC20_PSM_PIN , GPIO_Mode_In_FL_No_IT);


	}

	///* 反初始化加速度计引脚 */
	//void Accelerometer_uinit(void)
	//{
	//	
	//	GPIO_Init(ACCLEROMETER_SCK_PORT,ACCLEROMETER_SCK_PIN, GPIO_Mode_Out_PP_Low_Slow);
	//	GPIO_Init(ACCLEROMETER_SDO_PORT, ACCLEROMETER_SD0_PIN, GPIO_Mode_Out_PP_Low_Slow);  
	//	GPIO_Init(ACCLEROMETER_INIT1_PORT, ACCLEROMETER_INIT1_PIN, GPIO_Mode_Out_PP_Low_Slow);
	//	GPIO_Init(ACCLEROMETER_INIT2_PORT, ACCLEROMETER_INIT2_PIN, GPIO_Mode_Out_PP_Low_Slow);
	//}
		



	/* 未使用引脚初始化	 16个*/
	void no_use_gpio_init(void)
	{

		GPIO_Init(GPIOA, GPIO_Pin_0| GPIO_Pin_1, GPIO_Mode_In_PU_No_IT); // NRST,SWIM 上拉输入


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
	  * @brief	板级初始化
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

		ME3616_uinit();	// 5个
		no_use_gpio_init(); // 未使用引脚初始化 16

		/* 关闭所有指示灯，及其定时器 */
		TIM4_ClearITPendingBit(TIM4_IT_Update);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, DISABLE);
		TIM4_Cmd(DISABLE);
		CLOSE_ALL_LED();  // 3个   // 3+5+16+2=26  + VSS + VDD =28 



		clk_init();
		RTC_init();

	
		/* 关闭时间戳定时器3 */
		TIM3_ClearITPendingBit(TIM3_IT_Update);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, DISABLE);
		TIM3_Cmd(DISABLE);

		/* 关闭定时器3后 重新喂狗 */
		IWDG_RELOAD();



		/* 关闭定时器2 */
		TIM2_ClearITPendingBit(TIM2_IT_Update);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, DISABLE);
		TIM2_Cmd(DISABLE);



	
	//	Accelerometer_uinit();


		/* 水印开关引脚初始化 */
		wake_up_GPIO_init();
		clear_ITPendingBit();
	}

	

	/**
	  * ***********************************************************************
	  * @brief	切换时钟源，由内部高速时钟转化成内部低速时钟
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
		/*开启LSI时钟*/
		CLK_LSICmd(ENABLE);
		/*使能时钟源选择*/
		CLK_SYSCLKSourceSwitchCmd(ENABLE);
		/*选择时钟源分频比*/
		CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
		/*切换时钟源为LSI*/
		CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);
		/*等待LSI时钟源稳定*/
		while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_LSI);

		/*关闭HSI*/
		CLK_HSICmd(DISABLE);

		/*失能快速唤醒*/
		CLK_HaltConfig(CLK_Halt_FastWakeup, DISABLE);

		PWR_FastWakeUpCmd(DISABLE);
		PWR_UltraLowPowerCmd(ENABLE);
	}

	/**
	  * ***********************************************************************
	  * @brief	初始化RTC时钟
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
		/*设置RTC时钟源为LSI，64分频*/
		CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_64);

		/*等待LSi稳定*/
		while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);

		/*使能RTC时钟*/
		CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

		/*RTC唤醒时钟16分频*/
		RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);

		/*使能唤醒中断*/
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		/*设定唤醒计数值*/
		RTC_SetWakeUpCounter(65000);
		//RTC_SetWakeUpCounter(1110);		// 30s
		//RTC_SetWakeUpCounter(44531);//2017-2-22修改RTC计时时间   20s
		//RTC_SetWakeUpCounter(22266);//10min，37次计数为1s，一次计数为27.027ms
        // RTC_SetWakeUpCounter(370);//10s唤醒一次
		/*使能RTC唤醒*/
		RTC_WakeUpCmd(ENABLE);
	}

	/**
	  * ***********************************************************************
	  * @brief	清楚中断标志位
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
	  * @brief	进入休眠前配置GPIO
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: 无用的端口配置为推完输出低模式
					  开关引脚配置为推挽输出高模式关断
					  串口引脚配置为开漏输出高模式
	  * ***********************************************************************
	  */
	static void wake_up_GPIO_init(void)
	{
		/*清外部中断标志位*/
		EXTI_ClearITPendingBit(SWITCH_TWO_EXIT_PIN);
		EXTI_ClearITPendingBit(SWITCH_THREE_EXIT_PIN);
	}

	/**
	  * ***********************************************************************
	  * @brief	板级初始化还原
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
		dev_boot_routine(); // 处理开机事务
		 

		softuart_printf("the g_ccfg_config.m_dev_guid is %s\r\n", g_ccfg_config.m_dev_guid);
#if 0
		p_init_clk();

		/* 初始化LED灯 */
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

	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




