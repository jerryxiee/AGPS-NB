
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


/*************************     ˽��ͷ�ļ�����     *************************/

#include"initial.h"
#include"stm8l15x_gpio.h"
#include"common_cfg.h"



/*************************      ���������ռ�      *************************/


/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        ȫ��ö��        *************************/

	/*************************       ȫ�ֺ궨��       *************************/

#ifdef DEBUG_UART
#define DEBUG_TXD_PORT		SWITCH_TWO_PORT
#define DEBUG_TXD_PIN		SWITCH_TWO_PIN

#define DEBUG_RXD_PORT		SWITCH_THREE_PORT
#define DEBUG_RXD_PIN		SWITCH_THREE_PIN
#endif


/*************************     ȫ�ֽṹ�嶨��     *************************/

	/*************************      ȫ�ֱ�������      *************************/
	static uint32_t s_1s_cd = 0;
	char buf[] = "+ZGPSR: 091725.84,2452.48599N,11833.00900E,1.88,-29.9,3,,1.979,1.068,030420,04";
	/*************************        ��������        *************************/

	static void register_process(void);
	static void unregister_process(void);
	/* Private function prototypes -----------------------------------------------*/
	static uint8_t tim_5s_nc_check_net(void);

	

	/*************************        ����ʵ��        *************************/

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
			/* ��ȡNB���������ݣ�ת����DEBUG_TX */
			GPIO_WriteBit(DEBUG_TXD_PORT, DEBUG_TXD_PIN, GPIO_ReadInputDataBit(ME3616_RXD_PORT, ME3616_RXD_PIN)); 

			/* ��ȡ MCU ���������ݣ���ME3616_TX */7
			GPIO_WriteBit(ME3616_TXD_PORT, ME3616_TXD_PIN, GPIO_ReadInputDataBit(DEBUG_RXD_PORT, DEBUG_RXD_PIN));

		}
		
#else

		static uint32_t s_main_1s_cd = 0;

		 p_init_clk();             //��ʼ��ʱ��
		p_init_timer();            //��ʼ����ʱ��
#ifdef SOFTUART
		p_softuart_init();
#endif
		p_init_gpio();             //��ʼ���ܽ�
		p_init_IWDG();             //
		p_init_judgle_reset();     //�ж��豸�Ƿ���Ҫ��ʽ��

		p_init_BC20_uart(9600);    //��ʼ��BN220����
		p_init_BC20_GPIO();
		p_init_mercury();
		dev_boot_routine(); // ����������
		/* �ж��Ƿ���Ҫ��ʽ�� */

		softuart_printf("the g_ccfg_config.m_dev_guid is %s\r\n", g_ccfg_config.m_dev_guid);
		softuart_printf("STR(DEFAULT_DEV_GUID) is %s\r\n", STR(DEFAULT_DEV_GUID));
		// softuart_printf("STR(DEFAULT_DEV_GUID) is %d\r\n", STR(DEFAULT_DEV_GUID));

		 /* �ж��豸�Ƿ�ע�� */
		if (NULL != strstr(g_ccfg_config.m_dev_guid, STR(DEFAULT_DEV_GUID)))
		{
			softuart_printf("DEV is UNreg\r\n");
			/* δע�� */
			g_run_paramter.m_flg_en_unreg = TRUE;
		}                  
		else
		{
			softuart_printf("DEV is reg\r\n");
			/* ��ע�� */
			g_run_paramter.m_flg_en_unreg = FALSE;
			led_status = NB_NET_UNREG;
		}
		softuart_printf("reset\r\n");


		

		while (1)
		{
			/* ��ѭ��ι�� */
			IWDG_UPDATA_TIME();
			
			handle_task_queue();

			/* �ж�L620��ʼ�� */
			if (FALSE == g_run_paramter.m_flg_nb_init)
			{
				if ((g_run_paramter.m_flg_nb_init = p_init_BC20()) == FALSE)
				{

					p_BC20_reset();
					/* ʧ�ܼ��� */
					++g_run_paramter.m_nb_stat.m_nbiot_init_fail_cnt;
				
					if (g_run_paramter.m_nb_stat.m_nbiot_init_fail_cnt >= 3)
					{
						p_task_enqueue(TASK_SLEEP, NULL, NULL);
					}


					continue;
				}
				else
				{
							/* ����δע�� У���� */
			     	g_run_paramter.m_flg_nb_net = FALSE;
					p_proto_gen_iotid();
					p_nblc_get_csq();
					led_status = NB_CSQ_HIGH;
				}
			}


			/* �ж�L620����״̬ */
			if (FALSE == g_run_paramter.m_flg_nb_net)
			{

				if ((g_run_paramter.m_flg_nb_net = p_nblc_establish_network()) == FALSE)
				{
					++g_run_paramter.m_nb_stat.m_nbiot_net_abnormal_cnt;

					/* �ظ�����10��ʧ�ܣ����³�ʼ��NBģ�� */
					if (10 == g_run_paramter.m_nb_stat.m_nbiot_net_abnormal_cnt) {
						g_run_paramter.m_nb_stat.m_nbiot_net_abnormal_cnt = 0;
						g_run_paramter.m_flg_nb_init = FALSE;
					}
					continue;
				}

				else {

					/* ������������,��GPS */
					if (FALSE == g_run_paramter.m_flg_en_unreg
						&& FALSE == g_run_paramter.m_flg_alarm
						&& TRUE == g_run_paramter.m_flg_nb_rep
						&& TRUE == g_ccfg_config.m_en_tim_rep
						)     // ʹ���˶�ʱ�ϱ�
					{

							p_task_enqueue(TASK_TIME_REP, NULL, NULL);

					}
					else {
						/* ���������Ϸ������ݰ�����߽��շ��������ݵĻ��� */
						p_task_enqueue(TASK_HEARTBEAT, NULL, NULL);
					}
				}


			}



			/* �豸�Ѿ�ע�� */
			if (FALSE == g_run_paramter.m_flg_en_unreg)
			{
				//	softuart_printf("rest is reg\r\n");
				register_process();
			}
			else {
				//	softuart_printf("rest is unreg\r\n");
					/* �豸δע�� */
				unregister_process();

			}


			if (s_main_1s_cd != GET_1S_TIMER() && (s_main_1s_cd = GET_1S_TIMER()))
			{

			   if(TRUE == g_run_paramter.m_flg_alarm)  // ����״̬������GPS��λ
			   { 

					/* �ж� GPS��ʼ���Ƿ�ɹ� */
					if (FALSE == g_run_paramter.m_flg_nb_gps)
					{
						g_run_paramter.m_flg_nb_gps = p_init_AGPS();
				    }						
					else
					{   
							/* ��ѯ��λ */
							if (!p_nblc_common_func(TRUE, "QGNSSRD=\"NMEA/RMC\"", NULL, 300, NULL, TRUE))   //��ѯ��λ��Ϣʧ��
							{
								g_run_paramter.m_gps_quere_fail_cnt++;
								/* ����10�β�ѯʧ�ܣ����³�ʼ��AGPS */
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
			   
				   /* ���������·����� */
				   p_nblc_common_func(FALSE, NULL, NULL, 10, NULL, FALSE);
			 
	


			   	if (0 == s_main_1s_cd % 5)
				{
				
				
				/* ���nb������״̬ */
					if (FALSE == tim_5s_nc_check_net())
					{
						led_status = NB_NET_UNREG;
					}
					else
					{
						/* ��ȡNB���ź�ǿ�� */
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
	  * @brief	�豸�Ѿ�ע��
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
	
		/* ����豸�Ƿ񱨾� */
		switch_process();
		
		if (GET_1S_TIMER() != s_1s_cd && (s_1s_cd = GET_1S_TIMER()))
		{


			 softuart_printf("register_process\r\n");
			/* ���������� */
			if (0 >= --g_run_paramter.m_tim_heartbeat)  //g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat=DEFAULT_TIM_HEARTBEAT=60
			{

				
				softuart_printf("TASK_heartbeat enqueue \r\n");
				p_task_enqueue(TASK_HEARTBEAT, NULL, NULL);

				/* �ж��Ƿ��б��� */
				if (TRUE == g_run_paramter.m_flg_alarm)
				{
					g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_alm_interval;
				}
				else {
					g_run_paramter.m_tim_heartbeat = g_ccfg_config.m_tim_heartbeat;
				}
			}

			/* ��������ʱ�� */
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
				/* ����Ƕ�ʱ�ϱ������ҽ��յ����������������������� 2020 0403*/
				p_task_enqueue(TASK_SLEEP, NULL, NULL);
			}

			/* ������GPS���ϱ� */
			if (0 >= --g_run_paramter.m_tim_gps_rep)
			{
				/* ������GPS���ϱ����� */
				p_task_enqueue(TASK_SLEEP, NULL, NULL);
				goto OUT;
			}



			/* ��ѹ��ⲿ�� */
			if (TRUE == g_ccfg_config.m_en_vol_check)
			{
				if (0 == s_1s_cd % 5)
				{
					/* ������ѹ���Ż�ȡ��ص�ѹ */
		//			g_run_paramter.m_dev_voltage = GET_BAT_VALUE();
				}

				if (DEFAULT_LOW_POWER >= g_run_paramter.m_dev_voltage
					&& ++g_run_paramter.m_cnt_low_vol
					&& 3 <= g_run_paramter.m_cnt_low_vol)
				{
					p_task_enqueue(TASK_SEND_DATA, "Low Voltage", NULL);
				}

				/* 2������ռ���ֵ */
				if (0 == s_1s_cd % 120)
				{
					g_run_paramter.m_cnt_low_vol = 0;
				}
			}

		} // 1������

	OUT:

		/* ����Ψһ���� */
		return;
	}



	/**
	  * ***********************************************************************
	  * @brief	�豸��ע�����
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
				/* �������к� */
				p_task_enqueue(TASK_QUERY_ID, NULL, NULL);
			}
		}
	}


	/**
  * ***********************************************************************
  * @brief	���nb������״̬
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
		/* ���NB������״̬ */
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
			/* ����Ͽ� */
			g_run_paramter.m_flg_nb_net = FALSE;

			/* ������������ */
			// p_task_enqueue(TASK_CON_SER, NULL, NULL);

			return FALSE;
		}
	}

	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




