
/**
  *************************************************************************
  * @file    task_queue.c
  * @author  LZY.Huaqian
  * @version V2.0.0
  * @date    2020/03/23
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
#include "task_queue.h"
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
#define			TASK_QUEUE_FULL		(g_task_queue.m_task_begin == (g_task_queue.m_task_end+1))
/*************************     ȫ�ֽṹ�嶨��     *************************/

/*************************      ȫ�ֱ�������      *************************/
	st_task_queue g_task_queue = { (Task_Queue_TypeDef)0 };

/*************************        ��������        *************************/

/*************************        ����ʵ��        *************************/



/**
  * ***********************************************************************
  * @brief	������
  *
  *	@param  task_id:
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
/* �ж϶������Ƿ���������� */
uint8_t p_task_simple(Task_Queue_TypeDef task_id)
{
	for (uint8_t i = g_task_queue.m_task_begin;i!=g_task_queue.m_task_end;i++)
	{
		if (task_id == g_task_queue.m_task_list[i].m_task_id)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**
  * ***********************************************************************
  * @brief	�����������
  *
  *	@param  operate: �������
  *	@param  data1:
  *	@param  data2:
  *
  * @retval void:
  *
  * @attention	: �������� ���ڼ�������
  * ***********************************************************************
  */
void p_task_enqueue(Task_Queue_TypeDef task_id, char* data1, char* data2)
{
	/* �ж϶����Ƿ�Ϊ�� */
	if (TASK_QUEUE_FULL || p_task_simple(task_id))
	{
		return;
	}
	g_task_queue.m_task_list[g_task_queue.m_task_end].m_task_id = task_id;
	g_task_queue.m_task_list[g_task_queue.m_task_end].m_data1 = data1;
	g_task_queue.m_task_list[g_task_queue.m_task_end].m_data2 = data2;
	g_task_queue.m_task_end++;
}

/**
  * ***********************************************************************
  * @brief	��ȡ�����еĵ�һ������
  *
  *	@param  task:
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
void p_task_dequeue(st_task* task)
{
	if (g_task_queue.m_task_begin == g_task_queue.m_task_end)
	{
		return;
	}

	/* �ж��Ƿ���Ҫȡ������ */
	if (NULL != task)
	{
		task->m_task_id = g_task_queue.m_task_list[g_task_queue.m_task_begin].m_task_id;
		task->m_data1 = g_task_queue.m_task_list[g_task_queue.m_task_begin].m_data1;
		task->m_data2 = g_task_queue.m_task_list[g_task_queue.m_task_begin].m_data2;
	}

	++g_task_queue.m_task_begin;
}

/**
  * ***********************************************************************
  * @brief	�ж�����������Ƿ�Ϊ��
  *
  *	@param  :
  *
  * @retval bool:
  *
  * @attention	: none
  * ***********************************************************************
  */
int8_t p_task_queue_isempty(void)
{
	return (g_task_queue.m_task_begin == g_task_queue.m_task_end);
}
/**
  * ***********************************************************************
  * @brief	��ȡ��������еĵ�һ��Ԫ��
  *
  *	@param  data1:
  *	@param  data2:
  *
  * @retval u8:
  *
  * @attention	: �������ж϶���Ϊ����
  * ***********************************************************************
  */
Task_Queue_TypeDef p_task_queue_head(char** data1, char** data2)
{
	if (data1) *data1 = g_task_queue.m_task_list[g_task_queue.m_task_begin].m_data1;
	if (data2) *data2 = g_task_queue.m_task_list[g_task_queue.m_task_begin].m_data2;
	return g_task_queue.m_task_list[g_task_queue.m_task_begin].m_task_id;
}
/**
  * ***********************************************************************
  * @brief	����������
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
void p_task_clear(void)
{
	g_task_queue.m_task_begin = g_task_queue.m_task_end;
#ifdef USE_UDP
	g_run_paramter.m_udp_transfer.m_flg_sending = FALSE;
	g_run_paramter.m_udp_transfer.m_flg_retry = FALSE;
#endif // USE_UDP

}

/**
* ***********************************************************************
* @brief	������������е�����
*
*	@param  :
*
* @retval void:
*
* @attention	: none
* ***********************************************************************
*/
void handle_task_queue(void)
{
	/* δʹ�ñ�־λ */
	struct
	{
		uint8_t m_task_finish;   // �����Ƿ����
		st_task m_task;			 // ��¼��ǰ����
	}s_now_task;

	uint8_t tmp_arr[30] = { 0 };

	/* �ж϶����Ƿ�Ϊ�� */
	while (FALSE == p_task_queue_isempty())
	{


		//softuart_printf("Task is not empty!\r\n");

		for (uint8_t i= g_task_queue.m_task_begin;i<= g_task_queue.m_task_end;i++)
		{
			softuart_printf("%3d", g_task_queue.m_task_list[i].m_task_id);	
		}

#ifdef 0
		/* �ж��Ƿ������������ͨѶ���� */
		/* �ж��Ƿ����ڷ������� */
		if (((~p_task_queue_head(NULL, NULL)) & TASK_ID_LOCAL_MASK))    // �����Զ������
		{
			softuart_printf("remote Task\r\n");
			/* Զ������ */
			if (FALSE == g_run_paramter.m_udp_transfer.m_flg_sending)   //  ���û�����ڷ��͵�����
			{
				softuart_printf("udp_transfer flg is FLASE\r\n");
				/* �������������Զ������ */
				p_task_dequeue(&s_now_task.m_task);                     // ����
				memcpy(&g_run_paramter.m_udp_transfer.m_task, &s_now_task.m_task, sizeof(st_task));   // ��������������
				g_run_paramter.m_udp_transfer.m_send_time = GET_1S_TIMER();                           // ���·���ʱ��
				g_run_paramter.m_udp_transfer.m_flg_sending = TRUE;                                   // ��־λ��1
			}
			else if (TRUE == g_run_paramter.m_udp_transfer.m_flg_retry)     //�����Ҫ�ط�
			{
				softuart_printf("upd  retry \r\n");
				memcpy(&s_now_task.m_task, &g_run_paramter.m_udp_transfer.m_task, sizeof(st_task));
				g_run_paramter.m_udp_transfer.m_send_time = GET_1S_TIMER();
				g_run_paramter.m_udp_transfer.m_flg_sending = TRUE;           //�����������ڷ���
				g_run_paramter.m_udp_transfer.m_flg_retry = FALSE;            //�ط���־λ���                                         
			}
			else 
			{

				if (g_run_paramter.m_udp_transfer.m_send_time > UDP_TRANSFER_TIMEOUT)      //������ͳ�ʱ�������·���
				{
					g_run_paramter.m_udp_transfer.m_flg_sending = FALSE;
					g_run_paramter.m_udp_transfer.m_flg_retry = TURE;
				}

				break;
			}
		}
		else
		{
			softuart_printf("remote or local Task\r\n");

			/* �������������Զ������ */
			p_task_dequeue(&s_now_task.m_task);     //��ȡ��������е�һ������
		}
#else
		/* TCP */
		p_task_dequeue(&s_now_task.m_task);         //��ȡ��������е�һ������

#endif // USE_UDP

		softuart_printf("s_now_task.m_task.m_task_id is %d\r\n", s_now_task.m_task.m_task_id);
		switch (s_now_task.m_task.m_task_id)
		{
		case TASK_SLEEP:            //˯������
			softuart_printf("task sleep\r\n");
			/* �������� */
			/* ����˯��ǰ������������ */
			g_run_paramter.m_tim_sleep = g_ccfg_config.m_tim_sleep * 2;

			p_task_clear();
			/* �������� */
			dev_sleep();
			break;
		


		case TASK_NBREST:           //NB��λ
			g_run_paramter.m_flg_nb_init = FALSE;
			g_run_paramter.m_flg_nb_net = FALSE;
			g_run_paramter.m_flg_nb_gps = FALSE;
			
			break;

		case TASK_DEBUG_INFO:       //����

		
			p_nblc_report_debug_info(TRUE, "QGNSSRD=\"NMEA/GSV\"", NULL, 100, NULL, TRUE);
		//	delay_10ms(100);
			p_nblc_report_debug_info(TRUE, "QGNSSCMD=0,\"$AIDINFO\"", "*", 100, NULL, FALSE);

			break;



		case TASK_CON_SER:          //TCP���ӷ�����
			softuart_printf("task con ser\r\n");
			/* �����������TCP���� */
			p_nblc_establish_network();
			break;

		case TASK_HEARTBEAT:        //����������
			softuart_printf("task creat heartbeat\r\n");
			/* ���������� */
			p_proto_creat_heartbeat();
		    g_run_paramter.m_flg_nb_net=p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_TIME_REP:         //���Ͷ�ʱ�ϱ�
			softuart_printf("task creat time report\r\n");
			/* ���Ͷ�ʱ�ϱ����� */
			p_proto_creat_data("TIME REPORT.", 12);
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;
		case TASK_PROCESS_GPS:      //����GPS����
			/* ����GPS���� */
			/*g_gps_info.m_gps_ok = TURE;
			sprintf(g_buff.m_recv_buff, "%s", "$GNRMC,093158.01,A,2452.50016,N,11833.03100,E,4.907,,270320,,,A,V*14");
			p_nblc_common_func(NULL, NULL, "$GNRMC", 100, p_dispose_gps_date, TRUE);*/

			break;
		case TASK_GET_SET_1:        //�ظ�������Ϣ1
			p_proto_reply_setting_1();
			break;

		case TASK_GET_SET_2:        //�ظ�������Ϣ2
			p_proto_reply_setting_2();
			break;

		case TASK_SEND_DATA:        //ͨ��TCP��������
			softuart_printf("task send data\r\n");
			p_proto_creat_data((uint8_t*)s_now_task.m_task.m_data1, strlen((char*)s_now_task.m_task.m_data1));
			//.m_flg_nb_net=p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			softuart_printf("send success? :%d\r\n", p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index));
			break;

		case TASK_SEND_ID:           //����Ψһ��
			softuart_printf("task send id\r\n");

			memset(tmp_arr, 0, sizeof(tmp_arr));
			strcat((char*)tmp_arr, "UniqueID:F2");
			strncat((char*)tmp_arr, (char*)g_run_paramter.m_imei + 2, 12);
			p_proto_creat_data(tmp_arr, strlen((char*)tmp_arr));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_SEND_ICCID:         //����ICCID
			softuart_printf("task send iccid\r\n");
			//pTestSend("SendICCID\r\n");

			memset(tmp_arr, 0, sizeof(tmp_arr));
			strcat((char*)tmp_arr, "ICCID:");
			strcat((char*)tmp_arr, (char*)g_run_paramter.m_sim_iccid);
			p_proto_creat_data(tmp_arr, strlen((char*)tmp_arr));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;
			//��������
		case TASK_QUERY_ID:           //���������������к� 7
			/* �������ID */
		//	softuart_printf("creat query!\r\n");
			p_proc_creat_iot_queryid();
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_SEND_FWVER:         //���Ͱ汾��Ϣ
			softuart_printf("task send fwver\r\n");
			/* ���Ͱ汾��Ϣ */
			p_proto_creat_data((uint8_t*)FW_VERSION(DATA, VOL), strlen((char*)FW_VERSION(DATA, VOL)));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_SERVER_CFG:         //������������Ϣ����
			softuart_printf("task serber cfg\r\n");
			/* �ж��û����õķ�������Ϣ�Ƿ���ȷ */
			// TODO: �ж��Ƕ���������
			memcpy(g_ccfg_config.m_ser_ip, g_run_paramter.m_ser_ip, sizeof(g_ccfg_config.m_ser_ip));
			memcpy(&g_ccfg_config.m_ser_port, &g_run_paramter.m_ser_port, sizeof(g_ccfg_config.m_ser_port));
			p_ccfg_write_config((uint8_t*)g_ccfg_config.m_ser_ip, sizeof(g_ccfg_config.m_ser_ip));
			p_ccfg_write_config((uint8_t*)g_ccfg_config.m_ser_port, sizeof(g_ccfg_config.m_ser_port));
			break;

		case TASK_RE_FORMAT:          //������ʽ���ɹ�
			softuart_printf("task re format\r\n");
			p_proto_creat_data((uint8_t*)s_now_task.m_task.m_data1, strlen((char*)s_now_task.m_task.m_data1));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			g_run_paramter.m_flg_en_unreg = TRUE;
			p_ccfg_reset();
			break;

		case TASK_REGISTER:            //�豸��ע��󣬵�һ������
			softuart_printf("task register\r\n");
			g_run_paramter.m_flg_en_unreg = FALSE;
			p_init_mercury();
			break;

		case TASK_SWITCHGPS:           //GPSģ�����
			softuart_printf("task switgps\r\n");
			if (TRUE == g_ccfg_config.m_en_gps_rep)
			{
			//	ENABLE_BN220();
			}
			else {
			//	DISABLE_BN220();
			}
			break;

		case TASK_RECV_DATA:           //��������
			/* ����Ƿ���������Ҫ���� */
			softuart_printf("task recv data\r\n");
			p_nblc_read_data();
			break;

		case TASK_WAKE_NB:             //����NB
			softuart_printf("task wake nb\r\n");
			p_nblc_port_send_data("alive", 5);
			break;

		case TASK_PASSBY_START:        //��ʼ��·
			softuart_printf("task passby start\r\n");
			g_run_paramter.m_tim_passby = 2 * g_ccfg_config.m_tim_passby;
			/* ����˯��ǰ������������ */
			p_task_clear();

			/* ��ʱ��·ʱ�ر�����ˮӡ���� */
			switch_ctrl(FALSE, FALSE);

			/* �������� */
			dev_sleep();
			break;
		default:
			break;
		}
	}

}

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif