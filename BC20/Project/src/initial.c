


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

  /*************************     系统头文件包含     *************************/
#include "stm8l15x_gpio.h"
#include "stm8l15x_clk.h"
#include "stm8l15x_usart.h"
#include "stm8l15x_itc.h"
#include "common_cfg.h"
#include "stm8l15x_tim3.h"
#include "stm8l15x_tim4.h"

/*************************     私有头文件包含     *************************/


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
#if 1
/* 测试单片机是否运转正常 */
	void p_init_gpio(void)
	{
		/* 初始化PA口为高速、推挽、低电平 */
	    GPIO_Init(LED_GPS_PORT, LED_GPS_PIN, GPIO_Mode_Out_PP_High_Fast);
		GPIO_Init(LED_REGISTER_PORT, LED_REGISTER_PIN, GPIO_Mode_Out_PP_High_Fast);
		GPIO_Init(LED_SHAKE_PORT, LED_SHAKE_PIN, GPIO_Mode_Out_PP_High_Fast);
		GPIO_Init(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Mode_In_PU_No_IT);  // 下载角设置为上拉输入
		GPIO_Init(GPS_EN_PORT, GPS_EN_PIN, GPIO_Mode_Out_PP_High_Slow);        //配置GPS LNA放大器高电平

	//	GPIO_Init(GPIOA, GPIO_Pin_1, GPIO_Mode_Out_PP_High_Slow); // 复位引脚初始化为高电平
}
#endif

	/**
  * ***********************************************************************
  * @brief	初始化 系统 时钟
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
		/* CLK寄存器复位 */
		CLK_DeInit();
	
		/* 分频比1,不分频 */
		CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

		/* 使能时钟切换 */
		CLK_SYSCLKSourceSwitchCmd(ENABLE); 
	
		/* 使能内部高速时钟HSI */
		CLK_HSICmd(ENABLE);

		/* 主时钟切换为HSI */
		CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

		/* 等待LSI时钟源稳定 */
		while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI);
		
		
	}

	/**
  * ***********************************************************************
  * @brief	初始化BN220串口
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
		/* 初始化RXD为浮空输入，TXD为开漏输出 */
		GPIO_Init(BC20_RXD_PORT, BC20_RXD_PIN, GPIO_Mode_In_FL_No_IT);  // 为什么要配置成非中断？
		GPIO_Init(BC20_TXD_PORT, BC20_TXD_PIN, GPIO_Mode_Out_OD_HiZ_Fast);  //开漏输出

		/* 打开串口1时钟 peripheral(外围的) */
		CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

		/* 初始化串口1 */
		USART_Init(USART1, BaudRate, USART_WordLength_8b, USART_StopBits_1, \
			USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));

		/* 关闭全局中断 */
		disableInterrupts();

		/* 设置软件中断优先级 */
		ITC_SetSoftwarePriority(USART1_RX_IRQn, ITC_PriorityLevel_2);

		/* 开全局中断 */
		enableInterrupts();

		/* 使能串口1接收中断， 关闭空闲和发送中断*/
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

		/* 使能串口1 */
		USART_Cmd(USART1, ENABLE);
	}

	/**
  * ***********************************************************************
  * @brief	初始化定时器
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
		/* 使能定时器时钟 */
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
		CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);

		CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
		/*
		配置定时器3时基模式(非计数)
		16M/128/1250
		定时器中断10ms
		*/
		TIM3_TimeBaseInit(TIM3_Prescaler_128, TIM3_CounterMode_Up, (uint16_t)1249);
		TIM3_ClearFlag(TIM3_FLAG_Update);  // 清除溢出中断标志位
		TIM3_ITConfig(TIM3_IT_Update, ENABLE); // 使能溢出中断


		/*
		配置定时器4时基模式(LED)
		16M/1024/(155+1) = 100Hz
		中断一次10ms
		*/
		TIM4_TimeBaseInit(TIM4_Prescaler_1024, (uint16_t)155);
		TIM4_ClearFlag(TIM4_FLAG_Update);
		TIM4_ITConfig(TIM4_IT_Update, ENABLE);

		/*
		配置定时器2时基模式(非计数)
		16M/128/1250
		定时器中断10ms
		*/
		TIM3_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Up, (uint16_t)1249);
		TIM3_ClearFlag(TIM2_FLAG_Update);  // 清除溢出中断标志位
		TIM3_ITConfig(TIM2_IT_Update, ENABLE); // 使能溢出中断


		/* 关闭全局中断 */
		disableInterrupts();

		/* 设置中断优先级 */
		ITC_SetSoftwarePriority(TIM3_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_1);
		ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_3);

		ITC_SetSoftwarePriority(TIM2_UPD_OVF_TRG_BRK_IRQn, ITC_PriorityLevel_1);
		
		/* 开全局中断 */
		enableInterrupts();

		/* 使能定时器 */
		TIM4_Cmd(ENABLE);
		TIM3_Cmd(ENABLE);

		TIM2_Cmd(ENABLE);
	}

	/**
  * ***********************************************************************
  * @brief	判断设备是否需要格式化
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

		GPIO_Init(ME3616_RXD_PORT, ME3616_RXD_PIN, GPIO_Mode_Out_PP_High_Fast);  // 接收设置为 高
		GPIO_Init(ME3616_TXD_PORT, ME3616_TXD_PIN, GPIO_Mode_In_FL_No_IT);     // 发送 设置为浮空
		GPIO_SetBits(ME3616_RXD_PORT, ME3616_RXD_PIN);

		delay_10ms(50);

		//GPIO_Init(ME3616_TXD_PORT, ME3616_TXD_PIN, GPIO_Mode_Out_PP_High_Fast);
		if (GPIO_ReadInputDataBit(ME3616_TXD_PORT, ME3616_TXD_PIN))
		{
			/* 关闭控制灯的定时器 */
			TIM4_Cmd(DISABLE);
			//GPIO_ResetBits(LED_NB_PORT, LED_NB_PIN);   // 拉低NB引脚
			GPIO_ResetBits(LED_GPS_PORT, LED_GPS_PIN);                //拉低GPS状态灯
			GPIO_ResetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);      //拉低注册状态灯
			GPIO_ResetBits(LED_SHAKE_PORT, LED_SHAKE_PIN);            //拉低震动灯


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

				/* 灯状态 */
				if (i > 10 && i < 20)
				{
					GPIO_ResetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);      //拉低注册状态灯
				}
				else if (i >= 20) 
				{
					GPIO_ResetBits(LED_SHAKE_PORT, LED_SHAKE_PIN);            //拉低震动灯
				}
			}


			if (j >= 25) 
			{
				GPIO_ResetBits(LED_GPS_PORT, LED_GPS_PIN);          //拉低GPS状态灯
				p_ccfg_reset();                         // 格式化
				p_proto_gen_server_key();               //生成服务器密钥
			}

			GPIO_SetBits(LED_GPS_PORT, LED_GPS_PIN);
			GPIO_SetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);
			GPIO_SetBits(LED_SHAKE_PORT, LED_SHAKE_PIN);

			TIM4_Cmd(ENABLE);
		}


		softuart_printf("before load m_flg_alarm is %d\r\n", g_run_paramter.m_flg_alarm);
		p_ccfg_load();      //载入配置
		softuart_printf("after load m_flg_alarm is %d\r\n", g_run_paramter.m_flg_alarm);
	}

	 /**
  * ***********************************************************************
  * @brief	初始化LED灯
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
		 /* 全部熄灭 */
		 GPIO_Init(LED_SHAKE_PORT, LED_SHAKE_PIN, GPIO_Mode_Out_PP_High_Slow);
		 GPIO_Init(LED_REGISTER_PORT, LED_REGISTER_PIN, GPIO_Mode_Out_PP_High_Slow);
		 GPIO_Init(LED_GPS_PORT, LED_GPS_PIN, GPIO_Mode_Out_PP_High_Slow);
		 //GPIO_Init(LED_NB_PORT, LED_NB_PIN, GPIO_Mode_Out_PP_High_Slow);
	 }

	 void p_init_IWDG(void)
	 {
#ifdef USE_IWDG
		 /* 读取Option byte中独立看门狗设置的数据 */
		 if (FLASH_ReadByte(0x0004808) != 0x02)						// 判断看门狗在休眠状态是否停止
		 {
			 softuart_printf("IWDG IS RUNing\r\n");

			 FLASH->CR2 = 0x80;										// 设置OPT： 对选项字节进行写操作使能
			 FLASH_Unlock(FLASH_MemType_Data);						// flash先解锁
			 /* Wait until Data EEPROM area unlocked flag is set*/
			 while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);

			 FLASH_ProgramByte(0x0004808, 0x02);						// 修改Option byte中的OPT[3]
			 FLASH_WaitForLastOperation(FLASH_MemType_Data);
			 FLASH_Lock(FLASH_MemType_Data);							// flash重新上锁
			 FLASH->CR2 = 0x00;										// 设置OPT： 对选项字节进行写操作恢复禁止
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


	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




