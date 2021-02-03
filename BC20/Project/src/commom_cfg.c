
/**
  *************************************************************************
  * @file    commom_cfg.c
  * @author  LZY.Huaqian
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

  /*************************     系统头文件包含     *************************/


/*************************     私有头文件包含     *************************/
#include "common_cfg.h"
#include "eeprom.h"
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
	st_ccfg_config	 g_ccfg_config = { 0 };

	st_run_parameter g_run_paramter = { 0 };

	st_buff			 g_buff = { 0 };

	uint32_t __IO    g_vu32_1s_timer = 0;

	uint32_t __IO	 g_vu32_10ms_timer = 0;

	uint32_t		 g_l620_init_timeout = 0;		// l620模块初始化失败的超时检测

	uint32_t		 g_l620_net_timeout = 0;		// l620模块无法链接服务器的超时检测

	LED_STATUS_TypeDef led_status;




	/*************************        函数声明        *************************/



	/*************************        函数实现        *************************/


/**
  * ***********************************************************************
  * @brief	阻塞延时
  *
  *	@param  ms: 10ms
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void delay_10ms(uint32_t ms)
	{
		ms = GET_10MS_TIMER() + ms;

		while (ms > GET_10MS_TIMER());
	}


	/**
	  * ***********************************************************************
	  * @brief	阻塞延时
	  *
	  *	@param  t: 单位 秒
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	void delay_s(uint8_t s)
	{
		const uint32_t now_time = GET_1S_TIMER() + s;

		while (now_time > GET_1S_TIMER());
	}

/**
************************************************************************
* @brief	清空缓冲区
* *	@param  R_or_T: 选择清空发送缓冲区或者接受缓冲区
*
* @retval void:
* * @attention	: none
* ***********************************************************************
*/
	/* 该函数有误，形参希望的得到一个枚举类型的变量，而不是一个常量！ */
#if 0
void clear_buff(Buff_Type_TypeDef buff_type)
{
	if (buff_type & RX_BUFF)
	{
		g_buff.m_flg_recv_lock = FALSE;  // 解锁
		g_buff.m_recv_index = 0x00;
		memset(g_buff.m_recv_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);
	}

	if (buff_type & TX_BUFF)
	{
		g_buff.m_flg_send_lock = FALSE;  // 解锁
		g_buff.m_send_index = 0x00;
		memset(g_buff.m_send_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);
	}


}
#endif



/* 恢复默认配置 */
/**
* ***********************************************************************
* @brief 格式化设备
*
*	@param  void:
*
* @retval void:
*
* @attention
* ***********************************************************************
*/
void p_ccfg_reset(void)
{

	softuart_printf("CCFG RESET\r\n");
/* 清空电话列表 */
	for (uint8_t i = 0; i < 8; i++)
	{
		memset(g_ccfg_config.m_tel_list[i], 0x00, MAX_TEL_NUMBER_SIZE + 1);
		g_ccfg_config.m_tel_list[i][0] = '#';
	}

	/* 物联网卡APN默认设置为 CMNET*/
	memset(g_ccfg_config.m_dev_APN, 0x00, sizeof(g_ccfg_config.m_dev_APN));
	memcpy(g_ccfg_config.m_dev_APN, STR(DEFAULT_DEV_APN), STRLEN(DEFAULT_DEV_APN));

	memset(g_ccfg_config.m_dev_guid, 0x00, sizeof(g_ccfg_config.m_dev_guid));
	memcpy(g_ccfg_config.m_dev_guid, STR(DEFAULT_DEV_GUID), STRLEN(DEFAULT_DEV_GUID));


	memset(g_ccfg_config.m_dev_iden_id, 0x00, sizeof(g_ccfg_config.m_dev_iden_id));
	memcpy(g_ccfg_config.m_dev_iden_id, STR(DEFAULT_DEV_IDEN_ID), STRLEN(DEFAULT_DEV_IDEN_ID));
	// 05951234123456

	/* 清默认密码 */
	memset(g_ccfg_config.m_dev_pwd, 0x00, sizeof(g_ccfg_config.m_dev_pwd));
	memcpy(g_ccfg_config.m_dev_pwd, STR(DEFAULT_DEV_PWD), STRLEN(DEFAULT_DEV_PWD));

	/* 默认服务器 */
	g_ccfg_config.m_ser_ip[0] = IP_1;
	g_ccfg_config.m_ser_ip[1] = IP_2;
	g_ccfg_config.m_ser_ip[2] = IP_3;
	g_ccfg_config.m_ser_ip[3] = IP_4;
	g_ccfg_config.m_ser_port = PORT;

	g_ccfg_config.m_en_1to5_rep = DEFAULT_1TO_REP;
	g_ccfg_config.m_en_gps_rep = DEFAULT_GPS_REP;
	g_ccfg_config.m_en_tim_rep = DEFAULT_TIME_REP;      // 使能定时上报
	g_ccfg_config.m_en_pwr_off_delay = DEFAULT_PWR_OFF_DELAY; // 使能延时关机
	g_ccfg_config.m_en_vol_check = DEFAULT_VOL_CHECK;     // 使能低压检测
	g_ccfg_config.m_en_sw1 = DEFAULT_SWITCH_1;      // 使能水银开关1
	g_ccfg_config.m_en_sw2 = DEFAULT_SWITCH_2;      // 使能水银开关2

	/* 默认数值配置 *//*用于掉电保存的各个配置参数*/
	g_ccfg_config.m_tim_pwr_delay = DEFAULT_TIM_PWR_DELAY;       // 关机延时时间 30分钟*60秒=1800
	g_ccfg_config.m_tim_sleep = DEFAULT_TIM_SELLP;		   // 定时报告时间间隔,30天*24小时=720
	g_ccfg_config.m_tim_gps_rep = DEFAULT_TIM_GPS_REP;		   // 上报GPS间隔
	g_ccfg_config.m_tim_heartbeat = DEFAULT_TIM_HEARTBEAT;	   // 无报警上报心跳包间隔60s
	g_ccfg_config.m_tim_alm_continue = DEFAULT_TIM_ALM_CONTINUE;    // 触发报警后持续上报时间 60秒*9分钟=540
	g_ccfg_config.m_tim_alm_interval = DEFAULT_TIM_ALM_INTERVAL;    // GPS报警间隔
	g_ccfg_config.m_cnt_sw_trigger = DEFAULT_CNT_SW_TRIGGER;      // 休眠状态下触发报警的水银开关触发次数
	g_ccfg_config.m_tim_no_signal_delay = DEFAULT_TIM_NO_SIGNAL_DELAY; // 2017-2-18默认设置无信号延时休眠时间为0

	/* EEPROM设置有效 */
	g_ccfg_config.m_cfg_verify = 0x12341234;

	/* 默认数据 *//*用于程序运行时的各个配置参数*/
	g_run_paramter.m_tim_pwr_off_delay = DEFAULT_TIM_PWR_DELAY * 60;
	g_run_paramter.m_tim_gps_rep = DEFAULT_TIM_GPS_REP * 60;  
	g_run_paramter.m_tim_heartbeat = DEFAULT_TIM_HEARTBEAT;
	g_run_paramter.m_tim_alm_continue = DEFAULT_TIM_ALM_CONTINUE * 60;

	/* 全部写入 */
	p_ccfg_write_config((uint8_t*)&g_ccfg_config, sizeof(st_ccfg_config));

	/* 生成加密身份ID */
	p_proto_gen_server_key();
}

/**
  * ***********************************************************************
  * @brief	载入配置
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
void p_ccfg_load(void)
{
	/* 清空配置数据 */
	memset(&g_ccfg_config, 0, sizeof(g_ccfg_config));

	/* 载入全部数据 */
	p_ccfg_read_config((uint8_t*)&g_ccfg_config, sizeof(st_ccfg_config));

	/* 检查数据是否正常 */
	if (g_ccfg_config.m_cfg_verify != 0x12341234)
	{
		softuart_printf("load data is not nomal\r\n");
		p_ccfg_reset();                       // 数据不正常，载入默认配置
	}



	/* 检查设置合法性 */
	for (uint8_t i = 0; i < MAX_TEL_LIST; i++)//最多八组电话  0--7 
	{
		if (g_ccfg_config.m_tel_list[i][0] == 0x00)
		{
			g_ccfg_config.m_tel_list[i][0] = '#';
			g_ccfg_config.m_tel_list[i][1] = 0x00;
		}
	}
	/*检查物联网卡设备编号*/
	if (g_ccfg_config.m_dev_guid[0] == 0x00)
	{
		memcpy(g_ccfg_config.m_dev_guid, STR(DEFAULT_DEV_GUID), STRLEN(DEFAULT_DEV_GUID));
	}

	if (g_ccfg_config.m_dev_pwd[0] == 0x00)
	{
		memcpy(g_ccfg_config.m_dev_pwd, STR(DEFAULT_DEV_PWD), STRLEN(DEFAULT_DEV_PWD));
	}

	if (g_ccfg_config.m_dev_iden_id[0] == 0x00)
	{
		memcpy(g_ccfg_config.m_dev_iden_id, STR(DEFAULT_DEV_IDEN_ID), STRLEN(DEFAULT_DEV_IDEN_ID));
	}

	/* 最短一分钟,否则默认半小时（客户要求） */
	if (g_ccfg_config.m_tim_gps_rep < MIN_TIM_GPS_REP)
	{
		g_ccfg_config.m_tim_gps_rep = DEFAULT_TIM_GPS_REP;
	}

	/* 无振动报警状态，延时关机时间，最短一分钟,否则默认半小时（客户要求） */
	if (g_ccfg_config.m_tim_pwr_delay < MIN_TIM_PWR_DELAY)
	{
		g_ccfg_config.m_tim_pwr_delay = DEFAULT_TIM_PWR_DELAY;
	}

	/* ... *//*默认休眠状态下定时上报的时间  30天*24小时=720*/
	if ((g_ccfg_config.m_tim_sleep > MAX_TIM_SLEEP) || (g_ccfg_config.m_tim_sleep == 0))
	{
		g_ccfg_config.m_tim_sleep = DEFAULT_TIM_SELLP;
	}

	/* 最少一次，否则默认3次 */ /*休眠状态下触发报警的水银开关触发次数*/
	if ((g_ccfg_config.m_cnt_sw_trigger < 1) || (g_ccfg_config.m_cnt_sw_trigger > 9))
	{
		g_ccfg_config.m_cnt_sw_trigger = DEFAULT_CNT_SW_TRIGGER;
	}

	/* 最短一分钟，否则一分钟 *//*没有报警状态下心跳包的间隔*/
	if (g_ccfg_config.m_tim_heartbeat < 60)
	{
		g_ccfg_config.m_tim_heartbeat = DEFAULT_TIM_HEARTBEAT;
	}

	/* 最短一分钟，否则9分钟 */ /*触发报警后持续上报时间*/
	if (g_ccfg_config.m_tim_alm_continue < MIN_TIM_ALM_CONTINUE)
	{
		g_ccfg_config.m_tim_alm_continue = DEFAULT_TIM_ALM_CONTINUE;
	}

	/* 最短15秒，否则15秒 */ /*GPS报警间隔*/
	if (g_ccfg_config.m_tim_alm_interval < 15)
	{
		g_ccfg_config.m_tim_alm_interval = DEFAULT_TIM_ALM_INTERVAL;
	}

	/* 生成加密身份ID */
	p_proto_gen_server_key();

	/* 重设计时时间 */
	g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep * 60;
	g_run_paramter.m_tim_pwr_off_delay = g_ccfg_config.m_tim_pwr_delay * 60;
	g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat;
	g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;

	/* 上电默认为报警 */
//	g_run_paramter.m_flg_alarm = TRUE;

	/* 重设任务队列 */
	p_task_clear();

}


/**
  * ***********************************************************************
  * @brief	删除某一组报警电话
  *
  *	@param  x:
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
void p_ccfg_del_tel(uint8_t x)
{
	if (x <= 7) // 0 ~ 7 一共8组
	{
		/* 清除数据 */
		memset(g_ccfg_config.m_tel_list[x], 0x00, sizeof(g_ccfg_config.m_tel_list[0]));
		g_ccfg_config.m_tel_list[x][0] = '#';

		p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tel_list[x], sizeof(g_ccfg_config.m_tel_list[0]));
	}
}

/**
  * ***********************************************************************
  * @brief	设备在开机后所需要执行的事务(包括睡眠唤醒）
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void dev_boot_routine(void)
{
		
	g_run_paramter.m_heartbeat_respond = FALSE;

	/* 开机后，记录L620的开始启动时间，用于超时判断 */
	g_l620_init_timeout = GET_1S_TIMER();
	g_l620_net_timeout = GET_10MS_TIMER();
	//g_run_paramter.m_gps_ok_time = GET_1S_TIMER();
	g_run_paramter.m_gps_ok = FALSE;

	g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat;

	//switch_process();  // 振动检测
	softuart_printf("m_flg_alarm is %d\r\n", g_run_paramter.m_flg_alarm);
	/* 报警与未报警的上报时间不相同 */
	if (TRUE == g_run_paramter.m_flg_alarm)
	{
     	g_run_paramter.m_tim_gps_rep = DEFAULT_TIM_GPS_REP *60 ; // 开机后设置为 15分钟
	}
	else
	{
		g_run_paramter.m_tim_gps_rep = 180;

	}



	/* 更新单次报警持续时间 */
	//g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;
	g_run_paramter.m_tim_alm_continue = 540;
	//	g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep * 60;
	//}

	/* 开机后要清空睡眠时间 */
	g_run_paramter.m_tim_sleep = 0;
	g_run_paramter.m_tim_passby = 0;

	/* 清空低压计数值 */
	g_run_paramter.m_cnt_low_vol = 0;
	g_run_paramter.m_dev_voltage = 33;
	led_status = NB_NET_UNREG;

	/*开机后使能GPS LNA放大器管脚*/
	GPIO_SetBits(GPS_EN_PORT, GPS_EN_PIN);

}

	/**
	  * ***********************************************************************
	  * @brief	设备在关机前后所需要执行的事务(包括睡眠唤醒）
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	void dev_shutdown_routine(void)
	{

#ifndef USE_PSM
		/* 断开网络链接 */

		 //p_nblc_common_func(TRUE, "CIPCLOSE", "ERROR", 100, NULL, FALSE);

		/* 关机所有状态都清空 */
		g_run_paramter.m_flg_nb_net = FALSE;
		g_run_paramter.m_flg_nb_init = FALSE;
		g_run_paramter.m_flg_nb_gps = FALSE;
#endif // USE_PSM

		/* 进入睡眠前清空水银开关触发的次数 */
		g_run_paramter.m_switch_trigger_cnt = 0;

		/* 清楚报警状态 */
		g_run_paramter.m_flg_alarm = FALSE;

		/* 时间戳置零,防止溢出 */
		g_vu32_10ms_timer = 0;
		IWDG_UPDATA_TIME();

		g_run_paramter.m_flg_wake_dev = FALSE;
		g_run_paramter.m_flg_nb_rep = FALSE;

		/*关机前拉低GPS LNA放大器管脚*/
		GPIO_ResetBits(GPS_EN_PORT, GPS_EN_PIN);

	}




	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif






