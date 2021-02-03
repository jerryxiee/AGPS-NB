
/**
  *************************************************************************
  * @file    ME3616.c
  * @author  LZY.Huaqian
  * @version V2.0.0
  * @date    2020/03/19
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
#include "ME3616.h"
#include "common_cfg.h"
#include "proto.h"

/*************************      ���������ռ�      *************************/


/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

/*************************        ȫ��ö��        *************************/

/*************************       ȫ�ֺ궨��       *************************/
#define C_M26_MAX_PROC_LINE		20
#define MAX_WAIT_AT_SEC			20
#define C_L620_WAIT_CS_PS_SEC   180   //����180��
#define	MAX_WAIT_SIM_SEC		30
#define MAX_WAIT_AGPS_SERC		60  
#define KEEP_LOCAT_SEC			15
#define MAX_UNLOCAT_SEC			120   // �δ��λ�ȴ�ʱ��
	

/*************************     ȫ�ֽṹ�嶨��     *************************/

/*************************      ȫ�ֱ�������      *************************/

	st_gps_info g_gps_info = { 0 };
/*************************        ��������        *************************/

/*************************        ����ʵ��        *************************/
uint8_t p_nblc_common_func(uint8_t send_string, char* at_cmd, char* match_cmd, uint16_t wait_ms, uint8_t(*proc_cb)(char*), u8 iWait_OK)
{
	
	int data_len = 0x00;
	char* line = NULL;
	uint8_t ret_val = FALSE;
	uint8_t max_line = 0x00;
	uint32_t start_time = GET_10MS_TIMER();
	char* tmp_ok = NULL;
	char str_ok[] = "OK";
/******************************* ����ATָ��ȴ��������� **********************************/
	

#if (NB_COAP_EN == 1)   // ����ƽ̨


	while ((tmp_ok = strstr(g_buff.m_recv_buff, str_ok)) && ((GET_10MS_TIMER() - start_time) < wait_ms))
	{
		memset(tmp_ok, 0x00, sizeof(str_ok));
		tmp_ok = tmp_ok + sizeof(str_ok);
	}

	if (strstr(g_buff.m_recv_buff, "+QLWDATARECV:") == NULL)    // �ȴ�����������ݣ��ڴ��������ķ���ָ��
	{
		/* ������ջ����� */
		g_buff.m_flg_recv_lock = FALSE;  // ����
		g_buff.m_recv_index = 0x00;
		memset(g_buff.m_recv_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);
	}
#endif

	/* �������� */
	if (send_string == TRUE)
	{
		p_uart_send_string("AT+");
		p_uart_send_string(at_cmd);
	}
	p_uart_send_string("\r\n");


	while ((GET_10MS_TIMER() - start_time) < wait_ms)        // �涨ʱ���ڣ�����յ� ERROR/OK/ƥ������ �˳�
	{
	

		if(strstr(g_buff.m_recv_buff,"ERROR")!=NULL) break;
		
		if( (match_cmd==NULL) ||(iWait_OK==TRUE) )  
		{
			if (strstr(g_buff.m_recv_buff, "OK\r\n") != NULL)
			{
				break;
			}
		}
		else if (strstr(g_buff.m_recv_buff, match_cmd)!= NULL)
		{
			if (iWait_OK == FALSE)
			{		
				break;
			}
		}
		//delay_10ms(1);  
	}
	
	//if (proc_cb!=NULL)
	//{
	//	softuart_printf("BUFF is %s\r\n", g_buff.m_recv_buff);
	//}

/******************* ����ATָ��ȴ��������� *****************************/

/******************* ����ƥ������,������ƥ���������������� ***************/

	//.m_flg_recv_lock = TRUE; // ��ס���ڻ�������ʼ��������
	p_at_format_data(g_buff.m_recv_index);  // ��ͷ��ʽ����βָ��
	data_len = g_buff.m_recv_index;

	 
	while ( (  (line= p_at_get_line(line,data_len))!= NULL  ) && ((++max_line) < C_M26_MAX_PROC_LINE))
	{
		/*softuart_printf("line is %s\r\n", line);*/
		if (strlen(line) > 0) 
		{
			if (strstr(line,"OK")&& match_cmd == NULL)       // һ��ƥ�䵽OK,�Ͳ�ƥ������
			{
			    ret_val = TRUE;
			
			}
			else if (strstr(line, "+QGNSSRD:") != NULL)
			{
				u8 get_data = p_dispose_gps_date(line);
				ret_val = strcmp(match_cmd, "+QGNSSRD:") == 0 ? get_data : ret_val;
			}

#if (NB_COAP_EN == 1)   // ����ƽ̨
			else if (strstr(line, "+QLWDATARECV:") != NULL)    // �ں��������ݴ���M2MCLIRECV������
			{
				g_l620_net_timeout = GET_10MS_TIMER();
				u8 get_data = p_nblc_proc_read_data(line);
				ret_val = strcmp(match_cmd, "+QLWDATARECV:") == 0 ? get_data : ret_val;  // �����������M2MCLIRECV���������ݴ���Ľ����
			}
#endif


			else if ((match_cmd!=NULL) && (strstr(line,match_cmd)!=NULL)) 
			{
				ret_val = TRUE;
			}


			else if (proc_cb!=NULL)
			{
				ret_val = proc_cb(line);
			}
			else
			{
				/* ���������ϴ����ݺ������� */
			}

		}
	}
	///* ������ջ����� */
	//g_buff.m_flg_recv_lock = FALSE;  // ����
	//g_buff.m_recv_index = 0x00;
	//memset(g_buff.m_recv_buff, 0x00, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);

	
	memset(g_buff.m_recv_buff, 0x00, data_len);

	if (g_buff.m_recv_index > data_len)
	{
		memcpy(&g_buff.m_recv_buff[0], &g_buff.m_recv_buff[data_len], g_buff.m_recv_index - data_len);
		g_buff.m_recv_index = g_buff.m_recv_index - data_len;
	}
	else
	{
		g_buff.m_recv_index = 0;
	}


	return ret_val;

}


uint8_t p_wait_ok(uint16_t wait_ms)
{
	uint8_t ret_val = FALSE;
	uint32_t start_time = GET_10MS_TIMER();
	char* tmp_ok = NULL;
	char str_ok[] = "OK";
	/******************************* ����ATָ��ȴ��������� **********************************/


	while ((tmp_ok = strstr(g_buff.m_recv_buff, str_ok)) && ((GET_10MS_TIMER() - start_time) < wait_ms))
	{
		memset(tmp_ok, 0x00, sizeof(str_ok));
		tmp_ok = tmp_ok + sizeof(str_ok);
	}

	while ((GET_10MS_TIMER() - start_time) < wait_ms)        // �涨ʱ���ڣ�����յ� ERROR/OK/ƥ������ �˳�
	{
			if (strstr(g_buff.m_recv_buff, "OK\r\n") != NULL)
			{
				ret_val = TURE;
				break;
			}
	}

	return ret_val;
}

uint8_t p_nblc_report_debug_info(uint8_t send_string, char* at_cmd, char* match_cmd, uint16_t wait_ms, uint8_t(*proc_cb)(char*), u8 iWait_OK)
{
	int data_len = 0x00;
	char* line = NULL;
	uint8_t ret_val = FALSE;
	uint8_t max_line = 0x00;
	uint32_t start_time = GET_10MS_TIMER();

		/* ������ջ����� */
		g_buff.m_flg_recv_lock = FALSE;  // ����
		g_buff.m_recv_index = 0x00;
		memset(g_buff.m_recv_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);

	/* �������� */
	if (send_string == TRUE)
	{
		p_uart_send_string("AT+");
		p_uart_send_string(at_cmd);
	}
	p_uart_send_string("\r\n");


	while ((GET_10MS_TIMER() - start_time) < wait_ms)        // �涨ʱ���ڣ�����յ� ERROR/OK/ƥ������ �˳�
	{


		if (strstr(g_buff.m_recv_buff, "ERROR") != NULL) break;

		if ((match_cmd == NULL) || (iWait_OK == TRUE))
		{
			if (strstr(g_buff.m_recv_buff, "OK\r\n") != NULL)
			{
				break;
			}
		}
		else if (strstr(g_buff.m_recv_buff, match_cmd) != NULL)
		{
			if (iWait_OK == FALSE)
			{
				break;
			}
		}
		//delay_10ms(1);  
	}

	//p_proto_creat_data((uint8_t*)(g_buff.m_recv_buff + 30), 100);
	//p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);

/******************* ����ATָ��ȴ��������� *****************************/

/******************* ����ƥ������,������ƥ���������������� ***************/

	//.m_flg_recv_lock = TRUE; // ��ס���ڻ�������ʼ��������
	p_at_format_data(g_buff.m_recv_index);  // ��ͷ��ʽ����βָ��
	data_len = g_buff.m_recv_index;


	while (((line = p_at_get_line(line, data_len)) != NULL) && ((++max_line) < C_M26_MAX_PROC_LINE))
	{
		
		/*softuart_printf("line is %s\r\n", line);*/
		if (strlen(line) > 0)
		{			
			if (strstr(at_cmd, "$AIDINFO"))
			{
				p_proto_creat_data((uint8_t*)line+45, 100);
				p_nblc_debug_send_data(g_buff.m_send_buff, g_buff.m_send_index);
				line = NULL;
				break;

			}
				p_proto_creat_data((uint8_t*)line,strlen(line));
				p_nblc_debug_send_data(g_buff.m_send_buff, g_buff.m_send_index);

		}
	}


	memset(g_buff.m_recv_buff, 0x00, data_len);

	if (g_buff.m_recv_index > data_len)
	{
		memcpy(&g_buff.m_recv_buff[0], &g_buff.m_recv_buff[data_len], g_buff.m_recv_index - data_len);
		g_buff.m_recv_index = g_buff.m_recv_index - data_len;
	}
	else
	{
		g_buff.m_recv_index = 0;
	}

	return ret_val;

}


void p_BC20_reset(void)
{
	GPIO_ResetBits(BC20_RESET_PORT, BC20_RESET_PIN);
	delay_10ms(6);  // ����60ms
	GPIO_SetBits(BC20_RESET_PORT, BC20_RESET_PIN);


}

uint8_t p_nblc_proc_cimi(char* line)
{
	do
	{
		if ('0' <= *line && *line <= '9')
		{
			memcpy(g_run_paramter.m_sim_imsi, line, sizeof(g_run_paramter.m_sim_imsi));
			return TRUE;
		}
	} while (*line++ != 0x00);

	return FALSE;
}

/* ��SIM�� */
uint8_t p_nblc_get_cimi(void)
{
	return p_nblc_common_func(TRUE, "CIMI", NULL, 100, p_nblc_proc_cimi, FALSE);
}


static uint8_t p_nblc_proc_csq(char* line)
{
	/* NBʹ��CESQ,2Gʹ��CSQ, CESQ��24���� ��ʾ�źŻ����� */
	if (strstr(line, "CESQ:"))
	{
		g_run_paramter.m_nb_stat.m_nbiot_csq = atoi(&line[6]);

		return TRUE;
	}
	return FALSE;
}

static uint8_t p_nblc_proc_vol(char* line)
{
	//+CBC: 0,0,3246
	char* s = NULL;

	if (strstr(line, "+CBC:"))
	{
		s = strtok(line, ":");
		s = strtok(NULL, ",");
		s = strtok(NULL, ",");
		s = strtok(NULL, ",");
		
		g_run_paramter.m_dev_voltage = atoi(s) / 100;
		softuart_printf("vol is %d\r\n", g_run_paramter.m_dev_voltage);
		return TRUE;
	}
	return FALSE;

	
}

uint8_t p_nblc_proc_iccid(char* line)
{
	do
	{
		if ('0' <= *line && *line <= '9')
		{
			memcpy(g_run_paramter.m_sim_iccid, line, sizeof(g_run_paramter.m_sim_iccid) - 1);
			return TRUE;
		}
	} while (*line++ != 0x00);

	return FALSE;
}

uint8_t p_nblc_get_iccid(void)
{
	p_uart_send_string("AT+QCCID\r\n");
	return p_nblc_common_func(FALSE, NULL, NULL, 100, p_nblc_proc_iccid, FALSE);
}

uint8_t p_nblc_proc_imei(char* line)
{
	do
	{
		if ('0' <= *line && *line <= '9')
		{
			memcpy(g_run_paramter.m_imei, line, sizeof(g_run_paramter.m_imei));
			return TRUE;
		}
	} while (*line++ != 0x00);

	return FALSE;
}

uint8_t p_nblc_get_imei(void)
{
	return p_nblc_common_func(TRUE, "CGSN=1", NULL, 100, p_nblc_proc_imei, FALSE);
}


uint8_t p_nblc_get_vol(void)
{
	return  p_nblc_common_func(TRUE, "CBC", NULL, 100, p_nblc_proc_vol, TRUE);

}


uint8_t p_nblc_get_cereg(void)
{
	/* ���� / ���� */
	if (TRUE == p_nblc_common_func(TRUE, "CEREG=0", NULL, 100, NULL, TRUE))
	{
		if (p_nblc_common_func(TRUE, "CEREG?", "+CEREG: 0,1", 100, NULL, FALSE) == FALSE)
			return p_nblc_common_func(TRUE, "CEREG?", "+CEREG: 0,5", 100, NULL, TRUE);
		else
			return p_nblc_common_func(FALSE,NULL,NULL,100,NULL,TRUE);
	}
	return FALSE;
}
/**
  * ***********************************************************************
  * @brief	��ȡNB���ź�ǿ��
  *
  *	@param  :
  *
  * @retval uint8_t:
  *
  * @attention	: none
  * ***********************************************************************
  */
uint8_t p_nblc_get_csq(void)
{
	return p_nblc_common_func(TRUE, "CESQ", NULL, 100, p_nblc_proc_csq, FALSE);
}


void Delayus(void)
{
	asm("nop"); //һ��asm("nop")��������ʾ�������Դ���100ns 
	asm("nop");
	asm("nop");
	asm("nop");
}

//---- ���뼶��ʱ����----------------------- 
void Delayms(unsigned int time)
{
	unsigned int i;
	while (time--)
		for (i = 900; i > 0; i--)
			Delayus();
}

static void p_BC20_boot(void)
{

	Delayms(1000);
	GPIO_ResetBits(BC20_POWER_PORT, BC20_POWER_PIN);   // ����
	Delayms(500);
	GPIO_SetBits(BC20_POWER_PORT, BC20_POWER_PIN);  // ����
	Delayms(100);


}

/**
  * ***********************************************************************
  * @brief	�رջ���
  *
  *	@param  :
  *
  * @retval int8_t:
  *
  * @attention	: none
  * ***********************************************************************
  */
int8_t p_nblc_echo_disable(void)
{
	p_uart_send_string("ATE0\r\n");
	//p_uart_send_string("ATE1\r\n");
	return p_nblc_common_func(NULL, NULL, NULL, 100, NULL, FALSE);
}


uint8_t p_init_BC20(void)
{
	uint32_t at_time_out = 0;
	uint32_t sim_time_out = 0;
	uint8_t sim_status = FALSE;
	uint8_t ca_status = FALSE;

	/* �ȴ�������� */
	for (at_time_out = GET_10MS_TIMER() + MAX_WAIT_AT_SEC * 100; GET_10MS_TIMER() < at_time_out; )
	{

		if (FALSE == p_nblc_communic_test())  // ֱ���ظ�OK���ŷ�������ָ��
		{
			continue;
		}
		else
		{
			break;
		}
	}

	/* ����ȫ���� */
	p_nblc_common_func(TRUE, "CFUN=1", NULL, 100, NULL, FALSE);

	
	for (sim_time_out = GET_10MS_TIMER() + MAX_WAIT_SIM_SEC * 100; GET_10MS_TIMER() < sim_time_out;)
	{
		delay_10ms(100);
	
		
		//	p_uart_send_string("ATI\r\n");

			
			/* �رյ͹���ģʽ */
		if (FALSE == p_nblc_common_func(TRUE, "CPSMS=0", NULL, 100, NULL, TRUE))
		{
			continue;
		}

			/* ��ѯ�豸��ʶ */
		if (FALSE == p_nblc_get_cimi())
		{
			continue;
		}

		/* ��ȡSIM��ID */
		if (FALSE == p_nblc_get_iccid())
		{
			continue;
		}

		/* ��ȡimei */
		if (FALSE == p_nblc_get_imei())
		{
			continue;
		}

		/* ��ȡ��ѹֵ */
		if (FALSE == p_nblc_get_vol())
		{
			continue;
		}

		softuart_printf(" g_run_paramter.m_dev_voltage is %d", g_run_paramter.m_dev_voltage);

		

		/* ��ѯ��׼������û */
		if (FALSE == p_nblc_common_func(TRUE, "CPIN?", "READY", 100, NULL, TRUE))
		{
			continue;
		}
		sim_status = TRUE;

		break;


	}
		

	if (sim_status == TRUE)
	{
		/* �¾������㷨����һ�� */
		for (uint8_t i = 0; i < 2; i++)
		{
			/* APN���� */
		//	p_nblc_common_func(TRUE, "CGDCONT=1,\"IP\",\"CTNB\"", NULL, 100, NULL, FALSE);
			for (uint8_t i = 0; i < C_L620_WAIT_CS_PS_SEC / 6; i++)
			{
				/* ��ʵģ��ע������û */
				if (TRUE == p_nblc_get_cereg())
				{
					ca_status = TRUE;
					break;
				}
				p_nblc_get_csq();
				delay_10ms(600); // ��ѯ�ٶ�̫���Ӱ��IP����
			}
			/* ����δע��ɹ����������㷨 */
			if (ca_status == FALSE)
			{
				//p_BC20_reset();
			}
			else
			{
				break;
			}



		}


	}

	return ca_status;

}
/**
  * ***********************************************************************
  * @brief	��ʼ��AGPS��λ
  *
  *	@param  :
  *
  * @retval uint8_t:
  *
  * @attention	: none
  * ***********************************************************************
  */

uint8_t p_init_AGPS(void)
{
	do 
	{
		delay_10ms(100);
		/* ����AGPS���� */
		if (FALSE==p_nblc_common_func(TRUE, "QGNSSAGPS=1", NULL, 100, NULL, TRUE))
		{
			delay_10ms(300); // �ȴ�AGPS���������ظ������ݳɹ�
			break;
		}
		/* ����AGPS���� */
		if (FALSE==p_nblc_common_func(TRUE, "QGNSSAGPS?", "+QGNSSAGPS: 1", 100, NULL, FALSE))
		{
		
			break;
		//	p_nblc_common_func(TRUE, "QGNSSCMD=0,\"$AIDINFO\"", NULL, 100, NULL, TRUE);
			
			
		}
		 /* ����GNSS��Դ */
		if (p_nblc_common_func(TRUE, "QGNSSC=1", NULL, 100, NULL, TRUE))
		{
			return TRUE;
			
		}

		
	

	} while (0);

	p_nblc_common_func(TRUE, "QGNSSAGPS=0", NULL, 100, NULL, TRUE);
	p_nblc_common_func(TRUE, "QGNSSC=0", NULL, 100, NULL, TRUE);


		return FALSE;
	
}

/**
  * ***********************************************************************
  * @brief	����GPS���� GNRMC
  *
  *	@param  locat:
  *
  * @retval int8_t:
  *
  * @attention	: none
  * ***********************************************************************
  */



uint8_t p_dispose_gps_date(char* locat)
{
	char* p = NULL;

	if (NULL == locat || (p = strstr(locat, "$GNRMC")) == NULL)
	{
		return 0;
	}

	/* $GNRMC */
	custom_strtok(p, &p, ',');


	/* UTC ʱ�䣬hhmmss��ʱ���룩 */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_time, p, sizeof(g_gps_info.m_time) - 1); 
	}

	/* ��λ״̬��A=��Ч��λ��V=��Ч��λ */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		if ('V' == *p)
		{
			/* ����Ч��λ */
			g_run_paramter.m_gps_ok = FALSE;
			goto out;
		}
		g_run_paramter.m_gps_ok = TRUE;
	}

	/* γ�� ddmm.mmmm */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_lat, p, sizeof(g_gps_info.m_lat) - 1);
	}

	/* γ�Ȱ��� N�������򣩻� S���ϰ��� */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		g_gps_info.m_lat_NS = *p;
	}

	/* ���� dddmm.mmmm */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_lng, p, sizeof(g_gps_info.m_lng) - 1);
	}

	/* ���Ȱ��� E���������� W�������� */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		g_gps_info.m_lng_EW = *p;
	}

	/* �������ʣ�000.0~999.9 �ڣ� */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_speed, p, sizeof(g_gps_info.m_speed) - 1);
	}

	/* ���溽��000.0~359.9 �ȣ� */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_direct, p, sizeof(g_gps_info.m_direct) - 1);
	}

	/* UTC ���ڣ�ddmmyy�������꣩ */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_date, p, sizeof(g_gps_info.m_date) - 1);
	}

out:
	return g_run_paramter.m_gps_ok;
}


/*************************        ����ʵ��        *************************/

/**
  * ***********************************************************************
  * @brief	��ʼ��ME3616ģ��
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
void p_init_BC20_GPIO(void)
{
	/* ��ʼ���������ţ���λ����*/
	GPIO_Init(BC20_POWER_PORT, BC20_POWER_PIN, GPIO_Mode_Out_PP_High_Fast);
	GPIO_Init(BC20_RESET_PORT, BC20_RESET_PIN, GPIO_Mode_Out_PP_High_Fast);

	/* ���� */
	p_BC20_boot();
}



uint8_t p_nblc_communic_test(void)
{
	softuart_printf("send AT");
	p_uart_send_string("AT");
	//softuart_printf("send AT");
	return p_nblc_common_func(FALSE, NULL, NULL, 300, NULL, TRUE); 
}

u8 p_nblc_establish_network(void)
{	
	uint32_t at_time_out = 0;
	/* ����Ƿ�ע���� */
	if (FALSE == p_nblc_get_cereg())
	{
		return FALSE;
	}
#if (NB_COAP_EN == 1)     


	/* �ȴ�������� */
	for (at_time_out = GET_10MS_TIMER() + MAX_WAIT_AT_SEC * 100; GET_10MS_TIMER() < at_time_out; )
	{
		delay_10ms(100);

		/* ɾ�������������޷��ظ�����IP��˿� */
		p_nblc_common_func(TRUE, "QLWDEL", NULL, 200, NULL, TURE);

		if (!p_nblc_common_func(TRUE, "QLWSERV=117.60.157.137,5683", NULL, 200, NULL, TURE))
		{
			continue;
		}

		p_uart_send_string("AT+QLWCONF=");
		p_uart_send_string("\"");
		p_uart_send_string(g_run_paramter.m_imei);
		p_uart_send_string("\"");
		p_nblc_common_func(FALSE, NULL, NULL, 100, NULL, TURE);

		/* ��������ͨ��������ͨ�� */
		if (!p_nblc_common_func(TRUE,"QLWADDOBJ=19,0,1,\"0\"", NULL, 100, NULL, TURE))
		{
			continue;
		}

		if (!p_nblc_common_func(TRUE,"QLWADDOBJ=19,1,1,\"0\"", NULL, 100, NULL, TURE))
		{
			continue;
		}

		/* ֱ��ģʽע�� */
		if (p_nblc_common_func(TRUE,"QLWOPEN=0","+QLWOBSERVE:", 2000, NULL, FALSE))
		{
			return TURE;
		}
	}
	/* ע�� */
	p_nblc_common_func(TRUE, "QLWCLOSE", "CLOSE OK", 500, NULL, TRUE);
	return FALSE;

#endif
	//p_uart_send_string("AT+M2MCLINEW=117.60.157.137,5683,\"");              // ����ƽ̨   
	//p_uart_send_string("AT+M2MCLINEW=117.27.157.39,7879,\"");               // ʱ��NB����ƽ̨
	
	//p_uart_send_string("AT+M2MCLINEW=180.101.147.115,5683,\"");             // ����ƽ̨      
	
	
}


uint8_t p_nblc_read_data(void)
{
	//GPIO_ResetBits(GPIOA, GPIO_Pin_2);
#if (NB_COAP_EN == 1)   
	     

	return p_nblc_common_func(FALSE, NULL, "M2MCLIRECV", 30, p_nblc_proc_read_data, TRUE);
#else
	return p_nblc_common_func(TRUE, "CIPRXGET=2,"STR(MAX_RECV_BUFF_SIZE), NULL, 100, p_nblc_proc_read_data, TRUE);
#endif
}

void p_nblc_agps_proc(void)
{

	//if ((GET_1S_TIMER() - g_run_paramter.m_gps_init_time > MAX_UNLOCAT_SEC) && (FALSE == g_run_paramter.m_gps_ok))
	//{

	//		    g_run_paramter.m_flg_nb_gps = FALSE; 
	//}




		p_nblc_common_func(TRUE, "ZGPSR", "ZGPSR:", 100, p_dispose_gps_date, TRUE);
	
}

uint8_t p_nblc_proc_read_data(char* line)
{
	
#if (NB_COAP_EN == 1)   
	
	if (strstr(line, "+QLWDATARECV:") == NULL)
	{
		return FALSE;
	}
	//GPIO_ResetBits(GPIOA, GPIO_Pin_3);
#else 
	if (strstr(line, "2,") == NULL)
	{
		return FALSE;
	}
#endif

	do
	{
		char* s = NULL;
		uint16_t recv_len = 0x00;
#if (NB_COAP_EN == 1)  

		s = strtok(line, ":");
		s = strtok(NULL, ",");
		s = strtok(NULL, ",");
		s = strtok(NULL, ",");
		s = strtok(NULL, ",");
		s = strtok(NULL, ",");

		for (recv_len = 0; ; recv_len++) {
			if (s[recv_len] == '\r')
				break;
			if (s[recv_len] == '\0')
				break;
		}

/* ������Э�鴦�� */
		++g_run_paramter.m_nb_stat.m_nbiot_recv_cnt;
		p_proto_dispose_data(s, recv_len);

#else

		if ((s = strtok(line, ",")) == NULL) { break; } // mode
		if ((s = strtok(NULL, ",")) == NULL) { break; } // REQ_length
		if ((recv_len = atoi(s)) >= 256)
		{
			recv_len = 256;
		}

		if ((s = strtok(NULL, ",")) == NULL) { break; } // CNF_length

		/* ����ָ��ƫ�Ƶ�����λ�� */
		s += (strlen(s) + 2);							// before data

		if ('\0' == *s) { break; } // data


		/*
		for (u16 i = 0; i < recv_len; i++)
		{
			b[i] = hextoi(s[i * 2]) << 4;
			b[i] |= hextoi(s[i * 2 + 1]);
		}
		*/

		/* ������Э�鴦�� */
		++g_run_paramter.m_nb_stat.m_nbiot_recv_cnt;
		p_proto_dispose_data(s, recv_len);
#endif

		/* ������Ҫ����ʱ�� */
		g_run_paramter.m_nbiot_start_proc_time = GET_1S_TIMER();

		return TRUE;
	} while (0);

	return FALSE;
}
/**
  * ***********************************************************************
  * @brief	����UDP����
  *
  *	@param  udp_data: ��Ҫ���͵�����
  *	@param  len: ���ݳ���
  *
  * @retval u8:
  *
  * @attention	: none
  * ***********************************************************************
  */
#if 1
uint8_t p_nblc_port_send_data(uint8_t* udp_data, uint16_t len)
{
#if (NB_COAP_EN == 1)   
	// AT+M2MCLISEND=0100

	uint8_t udp_buff[30] = {0};
	softuart_printf("udp send data!\r\n");
	//AT + QISENDEX = 0, 3, 313233


		/* �����ϱ���ģʽΪhex(1),����ģʽΪtex(0)*/
	p_nblc_common_func(TURE, "QLWCFG=\"dataformat\",1,0", NULL, 100, NULL, TRUE);
	
	/* ��ֱ��ģʽע�ᵽ������ƽ̨ */



	sprintf(udp_buff, "AT+QLWDATASEND=19,0,0,%d,",len); // ��β�Զ���\0
	p_uart_send_string(udp_buff);  // �����ƽ̨������
	
	if (udp_data != NULL)
	{
		for (u16 i = 0; i < len; i++)
		{
		//	softuart_printf("%d", udp_data[i]);
			//p_swut_write2buff(itohex((udp_data[i] >> 4) & 0x0F));
			//p_swut_write2buff(itohex((udp_data[i] >> 0) & 0x0F));

			USART_SendData8(USART1, itohex((udp_data[i] >> 4) & 0x0F));
			while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));

			USART_SendData8(USART1, itohex((udp_data[i] >> 0) & 0x0F));
			while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		}
	}                   
	p_uart_send_string(",0x0000");
	/* ������Ҫ����ʱ�� */

	g_run_paramter.m_nb_stat.m_nbiot_send_cnt++;
	g_run_paramter.m_nbiot_start_proc_time = GET_10MS_TIMER();
	       //p_nblc_common_func(FALSE, NULL, NULL, 1000, NULL,FALSE);
	return p_nblc_common_func(FALSE, NULL,NULL, 500, NULL, TRUE);  
#else   
	char tmp[20] = { 0x00 };
	/* �������� */
	// "AT+CIPSEND=length"
	// CONNECT
	// DATA
	// Send OK
	p_uart_send_string("AT+CIPSEND=");
	if (len < 10)
	{
		tmp[0] = len + '0';
	}
	else if (len < 100)
	{
		tmp[0] = (len / 10) + '0';
		tmp[1] = (len % 10) + '0';
	}
	else
	{
		tmp[0] = (len / 100) + '0';
		tmp[1] = ((len % 100) / 10) + '0';
		tmp[2] = (len % 10) + '0';
	}
	p_uart_send_string(tmp);
	p_uart_send_string("\r\n");
	p_nblc_common_func(FALSE, NULL, ">", 300, NULL, FALSE);

	p_uart_send_string((const char*)udp_data);

	/* ������Ҫ����ʱ�� */
	++g_run_paramter.m_nb_stat.m_nbiot_send_cnt;
	g_run_paramter.m_nbiot_start_proc_time = GET_10MS_TIMER();
	delay_10ms(10);
	return p_nblc_common_func(FALSE, NULL, NULL, 100, NULL, FALSE);
#endif
}
#endif



uint8_t p_nblc_debug_send_data(uint8_t* udp_data, uint16_t len)
{
	uint8_t udp_buff[30] = { 0 };

	/*softuart_printf("udp_data is %s\r\n", udp_data);
	delay_10ms(10);
	softuart_printf("udp_data[len-1] is %c\r\n", udp_data[len - 1]);
	softuart_printf("udp_data[len-2] is %c\r\n", udp_data[len - 2]);
	softuart_printf("len is %d\r\n",len);*/


	/* �����ϱ���ģʽΪhex(1),����ģʽΪtex(0)*/
	p_uart_send_string("AT+QLWCFG=\"dataformat\",1,0\r\n");
	delay_10ms(2);
	/* ��ֱ��ģʽע�ᵽ������ƽ̨ */



	sprintf(udp_buff, "AT+QLWDATASEND=19,0,0,%d,", len); // ��β�Զ���\0
	p_uart_send_string(udp_buff);  // �����ƽ̨������

	if (udp_data != NULL)
	{
		for (u16 i = 0; i < len; i++)
		{
			//	softuart_printf("%d", udp_data[i]);
				//p_swut_write2buff(itohex((udp_data[i] >> 4) & 0x0F));
				//p_swut_write2buff(itohex((udp_data[i] >> 0) & 0x0F));

			USART_SendData8(USART1, itohex((udp_data[i] >> 4) & 0x0F));
			while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));

			USART_SendData8(USART1, itohex((udp_data[i] >> 0) & 0x0F));
			while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		}
	}
	p_uart_send_string(",0x0000\r\n");
	/* ������Ҫ����ʱ�� */

	g_run_paramter.m_nb_stat.m_nbiot_send_cnt++;
	g_run_paramter.m_nbiot_start_proc_time = GET_10MS_TIMER();
	//p_nblc_common_func(FALSE, NULL, NULL, 1000, NULL,FALSE);
	return p_wait_ok(100);

}
/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




