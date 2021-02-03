
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

  /*************************     ϵͳͷ�ļ�����     *************************/


/*************************     ˽��ͷ�ļ�����     *************************/
#include "common_cfg.h"
#include "eeprom.h"
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
	st_ccfg_config	 g_ccfg_config = { 0 };

	st_run_parameter g_run_paramter = { 0 };

	st_buff			 g_buff = { 0 };

	uint32_t __IO    g_vu32_1s_timer = 0;

	uint32_t __IO	 g_vu32_10ms_timer = 0;

	uint32_t		 g_l620_init_timeout = 0;		// l620ģ���ʼ��ʧ�ܵĳ�ʱ���

	uint32_t		 g_l620_net_timeout = 0;		// l620ģ���޷����ӷ������ĳ�ʱ���

	LED_STATUS_TypeDef led_status;




	/*************************        ��������        *************************/



	/*************************        ����ʵ��        *************************/


/**
  * ***********************************************************************
  * @brief	������ʱ
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
	  * @brief	������ʱ
	  *
	  *	@param  t: ��λ ��
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
* @brief	��ջ�����
* *	@param  R_or_T: ѡ����շ��ͻ��������߽��ܻ�����
*
* @retval void:
* * @attention	: none
* ***********************************************************************
*/
	/* �ú��������β�ϣ���ĵõ�һ��ö�����͵ı�����������һ�������� */
#if 0
void clear_buff(Buff_Type_TypeDef buff_type)
{
	if (buff_type & RX_BUFF)
	{
		g_buff.m_flg_recv_lock = FALSE;  // ����
		g_buff.m_recv_index = 0x00;
		memset(g_buff.m_recv_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);
	}

	if (buff_type & TX_BUFF)
	{
		g_buff.m_flg_send_lock = FALSE;  // ����
		g_buff.m_send_index = 0x00;
		memset(g_buff.m_send_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);
	}


}
#endif



/* �ָ�Ĭ������ */
/**
* ***********************************************************************
* @brief ��ʽ���豸
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
/* ��յ绰�б� */
	for (uint8_t i = 0; i < 8; i++)
	{
		memset(g_ccfg_config.m_tel_list[i], 0x00, MAX_TEL_NUMBER_SIZE + 1);
		g_ccfg_config.m_tel_list[i][0] = '#';
	}

	/* ��������APNĬ������Ϊ CMNET*/
	memset(g_ccfg_config.m_dev_APN, 0x00, sizeof(g_ccfg_config.m_dev_APN));
	memcpy(g_ccfg_config.m_dev_APN, STR(DEFAULT_DEV_APN), STRLEN(DEFAULT_DEV_APN));

	memset(g_ccfg_config.m_dev_guid, 0x00, sizeof(g_ccfg_config.m_dev_guid));
	memcpy(g_ccfg_config.m_dev_guid, STR(DEFAULT_DEV_GUID), STRLEN(DEFAULT_DEV_GUID));


	memset(g_ccfg_config.m_dev_iden_id, 0x00, sizeof(g_ccfg_config.m_dev_iden_id));
	memcpy(g_ccfg_config.m_dev_iden_id, STR(DEFAULT_DEV_IDEN_ID), STRLEN(DEFAULT_DEV_IDEN_ID));
	// 05951234123456

	/* ��Ĭ������ */
	memset(g_ccfg_config.m_dev_pwd, 0x00, sizeof(g_ccfg_config.m_dev_pwd));
	memcpy(g_ccfg_config.m_dev_pwd, STR(DEFAULT_DEV_PWD), STRLEN(DEFAULT_DEV_PWD));

	/* Ĭ�Ϸ����� */
	g_ccfg_config.m_ser_ip[0] = IP_1;
	g_ccfg_config.m_ser_ip[1] = IP_2;
	g_ccfg_config.m_ser_ip[2] = IP_3;
	g_ccfg_config.m_ser_ip[3] = IP_4;
	g_ccfg_config.m_ser_port = PORT;

	g_ccfg_config.m_en_1to5_rep = DEFAULT_1TO_REP;
	g_ccfg_config.m_en_gps_rep = DEFAULT_GPS_REP;
	g_ccfg_config.m_en_tim_rep = DEFAULT_TIME_REP;      // ʹ�ܶ�ʱ�ϱ�
	g_ccfg_config.m_en_pwr_off_delay = DEFAULT_PWR_OFF_DELAY; // ʹ����ʱ�ػ�
	g_ccfg_config.m_en_vol_check = DEFAULT_VOL_CHECK;     // ʹ�ܵ�ѹ���
	g_ccfg_config.m_en_sw1 = DEFAULT_SWITCH_1;      // ʹ��ˮ������1
	g_ccfg_config.m_en_sw2 = DEFAULT_SWITCH_2;      // ʹ��ˮ������2

	/* Ĭ����ֵ���� *//*���ڵ��籣��ĸ������ò���*/
	g_ccfg_config.m_tim_pwr_delay = DEFAULT_TIM_PWR_DELAY;       // �ػ���ʱʱ�� 30����*60��=1800
	g_ccfg_config.m_tim_sleep = DEFAULT_TIM_SELLP;		   // ��ʱ����ʱ����,30��*24Сʱ=720
	g_ccfg_config.m_tim_gps_rep = DEFAULT_TIM_GPS_REP;		   // �ϱ�GPS���
	g_ccfg_config.m_tim_heartbeat = DEFAULT_TIM_HEARTBEAT;	   // �ޱ����ϱ����������60s
	g_ccfg_config.m_tim_alm_continue = DEFAULT_TIM_ALM_CONTINUE;    // ��������������ϱ�ʱ�� 60��*9����=540
	g_ccfg_config.m_tim_alm_interval = DEFAULT_TIM_ALM_INTERVAL;    // GPS�������
	g_ccfg_config.m_cnt_sw_trigger = DEFAULT_CNT_SW_TRIGGER;      // ����״̬�´���������ˮ�����ش�������
	g_ccfg_config.m_tim_no_signal_delay = DEFAULT_TIM_NO_SIGNAL_DELAY; // 2017-2-18Ĭ���������ź���ʱ����ʱ��Ϊ0

	/* EEPROM������Ч */
	g_ccfg_config.m_cfg_verify = 0x12341234;

	/* Ĭ������ *//*���ڳ�������ʱ�ĸ������ò���*/
	g_run_paramter.m_tim_pwr_off_delay = DEFAULT_TIM_PWR_DELAY * 60;
	g_run_paramter.m_tim_gps_rep = DEFAULT_TIM_GPS_REP * 60;  
	g_run_paramter.m_tim_heartbeat = DEFAULT_TIM_HEARTBEAT;
	g_run_paramter.m_tim_alm_continue = DEFAULT_TIM_ALM_CONTINUE * 60;

	/* ȫ��д�� */
	p_ccfg_write_config((uint8_t*)&g_ccfg_config, sizeof(st_ccfg_config));

	/* ���ɼ������ID */
	p_proto_gen_server_key();
}

/**
  * ***********************************************************************
  * @brief	��������
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
	/* ����������� */
	memset(&g_ccfg_config, 0, sizeof(g_ccfg_config));

	/* ����ȫ������ */
	p_ccfg_read_config((uint8_t*)&g_ccfg_config, sizeof(st_ccfg_config));

	/* ��������Ƿ����� */
	if (g_ccfg_config.m_cfg_verify != 0x12341234)
	{
		softuart_printf("load data is not nomal\r\n");
		p_ccfg_reset();                       // ���ݲ�����������Ĭ������
	}



	/* ������úϷ��� */
	for (uint8_t i = 0; i < MAX_TEL_LIST; i++)//������绰  0--7 
	{
		if (g_ccfg_config.m_tel_list[i][0] == 0x00)
		{
			g_ccfg_config.m_tel_list[i][0] = '#';
			g_ccfg_config.m_tel_list[i][1] = 0x00;
		}
	}
	/*������������豸���*/
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

	/* ���һ����,����Ĭ�ϰ�Сʱ���ͻ�Ҫ�� */
	if (g_ccfg_config.m_tim_gps_rep < MIN_TIM_GPS_REP)
	{
		g_ccfg_config.m_tim_gps_rep = DEFAULT_TIM_GPS_REP;
	}

	/* ���񶯱���״̬����ʱ�ػ�ʱ�䣬���һ����,����Ĭ�ϰ�Сʱ���ͻ�Ҫ�� */
	if (g_ccfg_config.m_tim_pwr_delay < MIN_TIM_PWR_DELAY)
	{
		g_ccfg_config.m_tim_pwr_delay = DEFAULT_TIM_PWR_DELAY;
	}

	/* ... *//*Ĭ������״̬�¶�ʱ�ϱ���ʱ��  30��*24Сʱ=720*/
	if ((g_ccfg_config.m_tim_sleep > MAX_TIM_SLEEP) || (g_ccfg_config.m_tim_sleep == 0))
	{
		g_ccfg_config.m_tim_sleep = DEFAULT_TIM_SELLP;
	}

	/* ����һ�Σ�����Ĭ��3�� */ /*����״̬�´���������ˮ�����ش�������*/
	if ((g_ccfg_config.m_cnt_sw_trigger < 1) || (g_ccfg_config.m_cnt_sw_trigger > 9))
	{
		g_ccfg_config.m_cnt_sw_trigger = DEFAULT_CNT_SW_TRIGGER;
	}

	/* ���һ���ӣ�����һ���� *//*û�б���״̬���������ļ��*/
	if (g_ccfg_config.m_tim_heartbeat < 60)
	{
		g_ccfg_config.m_tim_heartbeat = DEFAULT_TIM_HEARTBEAT;
	}

	/* ���һ���ӣ�����9���� */ /*��������������ϱ�ʱ��*/
	if (g_ccfg_config.m_tim_alm_continue < MIN_TIM_ALM_CONTINUE)
	{
		g_ccfg_config.m_tim_alm_continue = DEFAULT_TIM_ALM_CONTINUE;
	}

	/* ���15�룬����15�� */ /*GPS�������*/
	if (g_ccfg_config.m_tim_alm_interval < 15)
	{
		g_ccfg_config.m_tim_alm_interval = DEFAULT_TIM_ALM_INTERVAL;
	}

	/* ���ɼ������ID */
	p_proto_gen_server_key();

	/* �����ʱʱ�� */
	g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep * 60;
	g_run_paramter.m_tim_pwr_off_delay = g_ccfg_config.m_tim_pwr_delay * 60;
	g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat;
	g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;

	/* �ϵ�Ĭ��Ϊ���� */
//	g_run_paramter.m_flg_alarm = TRUE;

	/* ����������� */
	p_task_clear();

}


/**
  * ***********************************************************************
  * @brief	ɾ��ĳһ�鱨���绰
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
	if (x <= 7) // 0 ~ 7 һ��8��
	{
		/* ������� */
		memset(g_ccfg_config.m_tel_list[x], 0x00, sizeof(g_ccfg_config.m_tel_list[0]));
		g_ccfg_config.m_tel_list[x][0] = '#';

		p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tel_list[x], sizeof(g_ccfg_config.m_tel_list[0]));
	}
}

/**
  * ***********************************************************************
  * @brief	�豸�ڿ���������Ҫִ�е�����(����˯�߻��ѣ�
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

	/* �����󣬼�¼L620�Ŀ�ʼ����ʱ�䣬���ڳ�ʱ�ж� */
	g_l620_init_timeout = GET_1S_TIMER();
	g_l620_net_timeout = GET_10MS_TIMER();
	//g_run_paramter.m_gps_ok_time = GET_1S_TIMER();
	g_run_paramter.m_gps_ok = FALSE;

	g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat;

	//switch_process();  // �񶯼��
	softuart_printf("m_flg_alarm is %d\r\n", g_run_paramter.m_flg_alarm);
	/* ������δ�������ϱ�ʱ�䲻��ͬ */
	if (TRUE == g_run_paramter.m_flg_alarm)
	{
     	g_run_paramter.m_tim_gps_rep = DEFAULT_TIM_GPS_REP *60 ; // ����������Ϊ 15����
	}
	else
	{
		g_run_paramter.m_tim_gps_rep = 180;

	}



	/* ���µ��α�������ʱ�� */
	//g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;
	g_run_paramter.m_tim_alm_continue = 540;
	//	g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep * 60;
	//}

	/* ������Ҫ���˯��ʱ�� */
	g_run_paramter.m_tim_sleep = 0;
	g_run_paramter.m_tim_passby = 0;

	/* ��յ�ѹ����ֵ */
	g_run_paramter.m_cnt_low_vol = 0;
	g_run_paramter.m_dev_voltage = 33;
	led_status = NB_NET_UNREG;

	/*������ʹ��GPS LNA�Ŵ����ܽ�*/
	GPIO_SetBits(GPS_EN_PORT, GPS_EN_PIN);

}

	/**
	  * ***********************************************************************
	  * @brief	�豸�ڹػ�ǰ������Ҫִ�е�����(����˯�߻��ѣ�
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
		/* �Ͽ��������� */

		 //p_nblc_common_func(TRUE, "CIPCLOSE", "ERROR", 100, NULL, FALSE);

		/* �ػ�����״̬����� */
		g_run_paramter.m_flg_nb_net = FALSE;
		g_run_paramter.m_flg_nb_init = FALSE;
		g_run_paramter.m_flg_nb_gps = FALSE;
#endif // USE_PSM

		/* ����˯��ǰ���ˮ�����ش����Ĵ��� */
		g_run_paramter.m_switch_trigger_cnt = 0;

		/* �������״̬ */
		g_run_paramter.m_flg_alarm = FALSE;

		/* ʱ�������,��ֹ��� */
		g_vu32_10ms_timer = 0;
		IWDG_UPDATA_TIME();

		g_run_paramter.m_flg_wake_dev = FALSE;
		g_run_paramter.m_flg_nb_rep = FALSE;

		/*�ػ�ǰ����GPS LNA�Ŵ����ܽ�*/
		GPIO_ResetBits(GPS_EN_PORT, GPS_EN_PIN);

	}




	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif






