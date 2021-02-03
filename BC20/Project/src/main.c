
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


/*************************     私有头文件包含     *************************/

#include"initial.h"
#include"stm8l15x_gpio.h"
#include"common_cfg.h"



/*************************      导入命名空间      *************************/


/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        全局枚举        *************************/

	/*************************       全局宏定义       *************************/

#ifdef DEBUG_UART
#define DEBUG_TXD_PORT		SWITCH_TWO_PORT
#define DEBUG_TXD_PIN		SWITCH_TWO_PIN

#define DEBUG_RXD_PORT		SWITCH_THREE_PORT
#define DEBUG_RXD_PIN		SWITCH_THREE_PIN
#endif


/*************************     全局结构体定义     *************************/

	/*************************      全局变量定义      *************************/
	static uint32_t s_1s_cd = 0;
	char buf[] = "+ZGPSR: 091725.84,2452.48599N,11833.00900E,1.88,-29.9,3,,1.979,1.068,030420,04";
	/*************************        函数声明        *************************/

	static void register_process(void);
	static void unregister_process(void);
	/* Private function prototypes -----------------------------------------------*/
	static uint8_t tim_5s_nc_check_net(void);

	

	/*************************        函数实现        *************************/

	void main(void)
	{
#ifdef DEBUG_UART
	;   p_init_clk();
	    GPIO_Init(ME3616_RXD_PORT, ME3616_RXD_PIN, GPIO_Mode_In_FL_No_IT); 
	    GPIO_Init(ME3616_TXD_PORT, ME3616_TXD_PIN, GPIO_Mode_Out_PP_High_Fast);
		p_init_ME3616_GPIO();
		GPIO_Init(DEBUG_RXD_PORT, DEBUG_RXD_PIN, GPIO_Mode_In_FL_No_IT);  
		GPIO_Init(DEBUG_TXD_PORT, DEBUG_TXD_PIN, GPIO_Mode_Out_PP_High_Fast);


		GPIO_Init(LED_GPS_PORT, LED_GPS_PIN, GPIO_Mode_Out_PP_Low_Fast);
		GPIO_ResetBits(LED_GPS_PORT, LED_GPS_PIN);


		while (1)
		{
			/* 读取NB发来的数据，转交给DEBUG_TX */
			GPIO_WriteBit(DEBUG_TXD_PORT, DEBUG_TXD_PIN, GPIO_ReadInputDataBit(ME3616_RXD_PORT, ME3616_RXD_PIN)); 

			/* 读取 MCU 发来的数据，给ME3616_TX */7
			GPIO_WriteBit(ME3616_TXD_PORT, ME3616_TXD_PIN, GPIO_ReadInputDataBit(DEBUG_RXD_PORT, DEBUG_RXD_PIN));

		}
		
#else

		static uint32_t s_main_1s_cd = 0;

		 p_init_clk();             //初始化时钟
		p_init_timer();            //初始化定时器
#ifdef SOFTUART
		p_softuart_init();
#endif
		p_init_gpio();             //初始化管脚
		p_init_IWDG();             //
		p_init_judgle_reset();     //判断设备是否需要格式化

		p_init_BC20_uart(9600);    //初始化BN220串口
		p_init_BC20_GPIO();
		p_init_mercury();
		dev_boot_routine(); // 处理开机事务
		/* 判断是否需要格式化 */

		softuart_printf("the g_ccfg_config.m_dev_guid is %s\r\n", g_ccfg_config.m_dev_guid);
		softuart_printf("STR(DEFAULT_DEV_GUID) is %s\r\n", STR(DEFAULT_DEV_GUID));
		// softuart_printf("STR(DEFAULT_DEV_GUID) is %d\r\n", STR(DEFAULT_DEV_GUID));

		 /* 判断设备是否注册 */
		if (NULL != strstr(g_ccfg_config.m_dev_guid, STR(DEFAULT_DEV_GUID)))
		{
			softuart_printf("DEV is UNreg\r\n");
			/* 未注册 */
			g_run_paramter.m_flg_en_unreg = TRUE;
		}                  
		else
		{
			softuart_printf("DEV is reg\r\n");
			/* 已注册 */
			g_run_paramter.m_flg_en_unreg = FALSE;
			led_status = NB_NET_UNREG;
		}
		softuart_printf("reset\r\n");


		

		while (1)
		{
			/* 主循环喂狗 */
			IWDG_UPDATA_TIME();
			
			handle_task_queue();

			/* 判断L620初始化 */
			if (FALSE == g_run_paramter.m_flg_nb_init)
			{
				if ((g_run_paramter.m_flg_nb_init = p_init_BC20()) == FALSE)
				{

					p_BC20_reset();
					/* 失败计数 */
					++g_run_paramter.m_nb_stat.m_nbiot_init_fail_cnt;
				
					if (g_run_paramter.m_nb_stat.m_nbiot_init_fail_cnt >= 3)
					{
						p_task_enqueue(TASK_SLEEP, NULL, NULL);
					}


					continue;
				}
				else
				{
							/* 生成未注册 校验码 */
			     	g_run_paramter.m_flg_nb_net = FALSE;
					p_proto_gen_iotid();
					p_nblc_get_csq();
					led_status = NB_CSQ_HIGH;
				}
			}


			/* 判断L620网络状态 */
			if (FALSE == g_run_paramter.m_flg_nb_net)
			{

				if ((g_run_paramter.m_flg_nb_net = p_nblc_establish_network()) == FALSE)
				{
					++g_run_paramter.m_nb_stat.m_nbiot_net_abnormal_cnt;

					/* 重复联网10次失败，重新初始化NB模块 */
					if (10 == g_run_paramter.m_nb_stat.m_nbiot_net_abnormal_cnt) {
						g_run_paramter.m_nb_stat.m_nbiot_net_abnormal_cnt = 0;
						g_run_paramter.m_flg_nb_init = FALSE;
					}
					continue;
				}

				else {

					/* 网络链接正常,打开GPS */
					if (FALSE == g_run_paramter.m_flg_en_unreg
						&& FALSE == g_run_paramter.m_flg_alarm
						&& TRUE == g_run_paramter.m_flg_nb_rep
						&& TRUE == g_ccfg_config.m_en_tim_rep
						)     // 使能了定时上报
					{

							p_task_enqueue(TASK_TIME_REP, NULL, NULL);

					}
					else {
						/* 开机后马上发送数据包，提高接收服务器数据的机率 */
						p_task_enqueue(TASK_HEARTBEAT, NULL, NULL);
					}
				}


			}



			/* 设备已经注册 */
			if (FALSE == g_run_paramter.m_flg_en_unreg)
			{
				//	softuart_printf("rest is reg\r\n");
				register_process();
			}
			else {
				//	softuart_printf("rest is unreg\r\n");
					/* 设备未注册 */
				unregister_process();

			}


			if (s_main_1s_cd != GET_1S_TIMER() && (s_main_1s_cd = GET_1S_TIMER()))
			{

			   if(TRUE == g_run_paramter.m_flg_alarm)  // 报警状态，进行GPS定位
			   { 

					/* 判断 GPS初始化是否成功 */
					if (FALSE == g_run_paramter.m_flg_nb_gps)
					{
						g_run_paramter.m_flg_nb_gps = p_init_AGPS();
				    }						
					else
					{   
							/* 查询定位 */
							if (!p_nblc_common_func(TRUE, "QGNSSRD=\"NMEA/RMC\"", NULL, 300, NULL, TRUE))   //查询定位信息失败
							{
								g_run_paramter.m_gps_quere_fail_cnt++;
								/* 超过10次查询失败，重新初始化AGPS */
								if (g_run_paramter.m_gps_quere_fail_cnt >= 10)
								{
									g_run_paramter.m_gps_quere_fail_cnt = 0;
									g_run_paramter.m_flg_nb_gps = FALSE;
								}
							}
	
							if (0 == s_main_1s_cd % 30)
							{
								if (g_run_paramter.m_gps_ok == FALSE)
								{
									p_task_enqueue(TASK_DEBUG_INFO, NULL, NULL);
								}
							
							}
					}
			   }
			   
				   /* 检查服务器下发数据 */
				   p_nblc_common_func(FALSE, NULL, NULL, 10, NULL, FALSE);
			 
	


			   	if (0 == s_main_1s_cd % 5)
				{
				
				
				/* 检查nb的网络状态 */
					if (FALSE == tim_5s_nc_check_net())
					{
						led_status = NB_NET_UNREG;
					}
					else
					{
						/* 获取NB的信号强度 */
						p_nblc_get_csq();
						softuart_printf("CESQ:%d\r\n", g_run_paramter.m_nb_stat.m_nbiot_csq);
						if (g_run_paramter.m_nb_stat.m_nbiot_csq < 5)
						{
							led_status = NB_CSQ_LOW;
						}
						else
						{
							led_status = NB_CSQ_HIGH;
						}

					}

				}


			   
	    }


		

			
		}
#endif
	}

	/**
	  * ***********************************************************************
	  * @brief	设备已经注册
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	static void register_process(void)
	{
	
		/* 检测设备是否报警 */
		switch_process();
		
		if (GET_1S_TIMER() != s_1s_cd && (s_1s_cd = GET_1S_TIMER()))
		{


			 softuart_printf("register_process\r\n");
			/* 发送心跳包 */
			if (0 >= --g_run_paramter.m_tim_heartbeat)  //g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat=DEFAULT_TIM_HEARTBEAT=60
			{

				
				softuart_printf("TASK_heartbeat enqueue \r\n");
				p_task_enqueue(TASK_HEARTBEAT, NULL, NULL);

				/* 判断是否有报警 */
				if (TRUE == g_run_paramter.m_flg_alarm)
				{
					g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_alm_interval;
				}
				else {
					g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat;
				}
			}

			/* 持续报警时间 */
			if (TRUE == g_run_paramter.m_flg_alarm
				&& 0 >= --g_run_paramter.m_tim_alm_continue)
			{

				g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;
				g_run_paramter.m_flg_alarm = FALSE;
			}

			softuart_printf("m_tim_gps_rep is %d \r\n", g_run_paramter.m_tim_gps_rep);
			softuart_printf("m_tim_alm_continue is %d \r\n", g_run_paramter.m_tim_alm_continue);

			if ((TRUE == g_run_paramter.m_heartbeat_respond) && (TRUE == g_run_paramter.m_flg_nb_rep)&&(FALSE == g_run_paramter.m_flg_alarm) )
			{
				/* 如果是定时上报，并且接收到服务器反馈包，立即休眠 2020 0403*/
				p_task_enqueue(TASK_SLEEP, NULL, NULL);
			}

			/* 心跳（GPS）上报 */
			if (0 >= --g_run_paramter.m_tim_gps_rep)
			{
				/* 心跳（GPS）上报结束 */
				p_task_enqueue(TASK_SLEEP, NULL, NULL);
				goto OUT;
			}



			/* 低压检测部分 */
			if (TRUE == g_ccfg_config.m_en_vol_check)
			{
				if (0 == s_1s_cd % 5)
				{
					/* 开启低压检测才获取电池电压 */
		//			g_run_paramter.m_dev_voltage = GET_BAT_VALUE();
				}

				if (DEFAULT_LOW_POWER >= g_run_paramter.m_dev_voltage
					&& ++g_run_paramter.m_cnt_low_vol
					&& 3 <= g_run_paramter.m_cnt_low_vol)
				{
					p_task_enqueue(TASK_SEND_DATA, "Low Voltage", NULL);
				}

				/* 2分钟清空计数值 */
				if (0 == s_1s_cd % 120)
				{
					g_run_paramter.m_cnt_low_vol = 0;
				}
			}

		} // 1秒任务

	OUT:

		/* 函数唯一出口 */
		return;
	}



	/**
	  * ***********************************************************************
	  * @brief	设备无注册代码
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	static void unregister_process(void)
	{
	//	softuart_printf("unregister process\r\n");
		if (GET_1S_TIMER() != s_1s_cd && (s_1s_cd = GET_1S_TIMER()))
		{
			if (0 == s_1s_cd % 30)
			{
				softuart_printf("send the task_qyery_ID\r\n");
				/* 请求序列号 */
				p_task_enqueue(TASK_QUERY_ID, NULL, NULL);
			}
		}
	}


	/**
  * ***********************************************************************
  * @brief	检测nb的网络状态
  *
  *	@param  :
  *
  * @retval uint8_t:
  *
  * @attention	: none
  * ***********************************************************************
  */
	static uint8_t tim_5s_nc_check_net(void)
	{
		/* 检查NB的网络状态 */
#if (NB_COAP_EN == 1)     
		if (TRUE == p_nblc_get_cereg())
#else
		if (TRUE == p_nblc_check_net())
#endif          
		{
			g_run_paramter.m_flg_nb_net = TRUE;

			return TRUE;
		}
		else
		{
			/* 网络断开 */
			g_run_paramter.m_flg_nb_net = FALSE;

			/* 重新链接网络 */
			// p_task_enqueue(TASK_CON_SER, NULL, NULL);

			return FALSE;
		}
	}

	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




