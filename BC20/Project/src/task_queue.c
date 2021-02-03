
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

  /*************************     系统头文件包含     *************************/


/*************************     私有头文件包含     *************************/

#include "common_cfg.h"
#include "task_queue.h"
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
#define			TASK_QUEUE_FULL		(g_task_queue.m_task_begin == (g_task_queue.m_task_end+1))
/*************************     全局结构体定义     *************************/

/*************************      全局变量定义      *************************/
	st_task_queue g_task_queue = { (Task_Queue_TypeDef)0 };

/*************************        函数声明        *************************/

/*************************        函数实现        *************************/



/**
  * ***********************************************************************
  * @brief	任务单例
  *
  *	@param  task_id:
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
/* 判断队列中是否有这个任务 */
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
  * @brief	将任务入队列
  *
  *	@param  operate: 任务操作
  *	@param  data1:
  *	@param  data2:
  *
  * @retval void:
  *
  * @attention	: 队列满后 不在加入任务
  * ***********************************************************************
  */
void p_task_enqueue(Task_Queue_TypeDef task_id, char* data1, char* data2)
{
	/* 判断队列是否为满 */
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
  * @brief	获取队列中的第一个任务
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

	/* 判断是否需要取出任务 */
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
  * @brief	判断任务队列中是否为空
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
  * @brief	获取任务队列中的第一个元素
  *
  *	@param  data1:
  *	@param  data2:
  *
  * @retval u8:
  *
  * @attention	: 必须先判断队列为不空
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
  * @brief	清空任务队列
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
* @brief	处理任务队列中的任务
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
	/* 未使用标志位 */
	struct
	{
		uint8_t m_task_finish;   // 任务是否完成
		st_task m_task;			 // 记录当前任务
	}s_now_task;

	uint8_t tmp_arr[30] = { 0 };

	/* 判断队列是否为空 */
	while (FALSE == p_task_queue_isempty())
	{


		//softuart_printf("Task is not empty!\r\n");

		for (uint8_t i= g_task_queue.m_task_begin;i<= g_task_queue.m_task_end;i++)
		{
			softuart_printf("%3d", g_task_queue.m_task_list[i].m_task_id);	
		}

#ifdef 0
		/* 判断是否是与服务器的通讯任务 */
		/* 判断是否是在发送任务 */
		if (((~p_task_queue_head(NULL, NULL)) & TASK_ID_LOCAL_MASK))    // 如果是远程任务
		{
			softuart_printf("remote Task\r\n");
			/* 远程任务 */
			if (FALSE == g_run_paramter.m_udp_transfer.m_flg_sending)   //  如果没有正在发送的任务
			{
				softuart_printf("udp_transfer flg is FLASE\r\n");
				/* 本地任务或其他远程任务 */
				p_task_dequeue(&s_now_task.m_task);                     // 出队
				memcpy(&g_run_paramter.m_udp_transfer.m_task, &s_now_task.m_task, sizeof(st_task));   // 拷贝到发送任务
				g_run_paramter.m_udp_transfer.m_send_time = GET_1S_TIMER();                           // 更新发送时间
				g_run_paramter.m_udp_transfer.m_flg_sending = TRUE;                                   // 标志位置1
			}
			else if (TRUE == g_run_paramter.m_udp_transfer.m_flg_retry)     //如果需要重发
			{
				softuart_printf("upd  retry \r\n");
				memcpy(&s_now_task.m_task, &g_run_paramter.m_udp_transfer.m_task, sizeof(st_task));
				g_run_paramter.m_udp_transfer.m_send_time = GET_1S_TIMER();
				g_run_paramter.m_udp_transfer.m_flg_sending = TRUE;           //设置数据正在发送
				g_run_paramter.m_udp_transfer.m_flg_retry = FALSE;            //重发标志位清除                                         
			}
			else 
			{

				if (g_run_paramter.m_udp_transfer.m_send_time > UDP_TRANSFER_TIMEOUT)      //如果发送超时，则重新发送
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

			/* 本地任务或其他远程任务 */
			p_task_dequeue(&s_now_task.m_task);     //获取任务队列中第一个任务
		}
#else
		/* TCP */
		p_task_dequeue(&s_now_task.m_task);         //获取任务队列中第一个任务

#endif // USE_UDP

		softuart_printf("s_now_task.m_task.m_task_id is %d\r\n", s_now_task.m_task.m_task_id);
		switch (s_now_task.m_task.m_task_id)
		{
		case TASK_SLEEP:            //睡眠任务
			softuart_printf("task sleep\r\n");
			/* 休眠任务 */
			/* 进入睡眠前先清空任务队列 */
			g_run_paramter.m_tim_sleep = g_ccfg_config.m_tim_sleep * 2;

			p_task_clear();
			/* 进入休眠 */
			dev_sleep();
			break;
		


		case TASK_NBREST:           //NB复位
			g_run_paramter.m_flg_nb_init = FALSE;
			g_run_paramter.m_flg_nb_net = FALSE;
			g_run_paramter.m_flg_nb_gps = FALSE;
			
			break;

		case TASK_DEBUG_INFO:       //调试

		
			p_nblc_report_debug_info(TRUE, "QGNSSRD=\"NMEA/GSV\"", NULL, 100, NULL, TRUE);
		//	delay_10ms(100);
			p_nblc_report_debug_info(TRUE, "QGNSSCMD=0,\"$AIDINFO\"", "*", 100, NULL, FALSE);

			break;



		case TASK_CON_SER:          //TCP连接服务器
			softuart_printf("task con ser\r\n");
			/* 与服务器创建TCP链接 */
			p_nblc_establish_network();
			break;

		case TASK_HEARTBEAT:        //发送心跳包
			softuart_printf("task creat heartbeat\r\n");
			/* 发送心跳包 */
			p_proto_creat_heartbeat();
		    g_run_paramter.m_flg_nb_net=p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_TIME_REP:         //发送定时上报
			softuart_printf("task creat time report\r\n");
			/* 发送定时上报数据 */
			p_proto_creat_data("TIME REPORT.", 12);
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;
		case TASK_PROCESS_GPS:      //处理GPS数据
			/* 解析GPS数据 */
			/*g_gps_info.m_gps_ok = TURE;
			sprintf(g_buff.m_recv_buff, "%s", "$GNRMC,093158.01,A,2452.50016,N,11833.03100,E,4.907,,270320,,,A,V*14");
			p_nblc_common_func(NULL, NULL, "$GNRMC", 100, p_dispose_gps_date, TRUE);*/

			break;
		case TASK_GET_SET_1:        //回复配置信息1
			p_proto_reply_setting_1();
			break;

		case TASK_GET_SET_2:        //回复配置信息2
			p_proto_reply_setting_2();
			break;

		case TASK_SEND_DATA:        //通过TCP发送数据
			softuart_printf("task send data\r\n");
			p_proto_creat_data((uint8_t*)s_now_task.m_task.m_data1, strlen((char*)s_now_task.m_task.m_data1));
			//.m_flg_nb_net=p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			softuart_printf("send success? :%d\r\n", p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index));
			break;

		case TASK_SEND_ID:           //发送唯一码
			softuart_printf("task send id\r\n");

			memset(tmp_arr, 0, sizeof(tmp_arr));
			strcat((char*)tmp_arr, "UniqueID:F2");
			strncat((char*)tmp_arr, (char*)g_run_paramter.m_imei + 2, 12);
			p_proto_creat_data(tmp_arr, strlen((char*)tmp_arr));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_SEND_ICCID:         //发送ICCID
			softuart_printf("task send iccid\r\n");
			//pTestSend("SendICCID\r\n");

			memset(tmp_arr, 0, sizeof(tmp_arr));
			strcat((char*)tmp_arr, "ICCID:");
			strcat((char*)tmp_arr, (char*)g_run_paramter.m_sim_iccid);
			p_proto_creat_data(tmp_arr, strlen((char*)tmp_arr));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;
			//看到这里
		case TASK_QUERY_ID:           //物联网卡请求序列号 7
			/* 请求身份ID */
		//	softuart_printf("creat query!\r\n");
			p_proc_creat_iot_queryid();
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_SEND_FWVER:         //发送版本信息
			softuart_printf("task send fwver\r\n");
			/* 发送版本信息 */
			p_proto_creat_data((uint8_t*)FW_VERSION(DATA, VOL), strlen((char*)FW_VERSION(DATA, VOL)));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			break;

		case TASK_SERVER_CFG:         //服务器配置信息更改
			softuart_printf("task serber cfg\r\n");
			/* 判断用户配置的服务器信息是否正确 */
			// TODO: 判断是都正常链接
			memcpy(g_ccfg_config.m_ser_ip, g_run_paramter.m_ser_ip, sizeof(g_ccfg_config.m_ser_ip));
			memcpy(&g_ccfg_config.m_ser_port, &g_run_paramter.m_ser_port, sizeof(g_ccfg_config.m_ser_port));
			p_ccfg_write_config((uint8_t*)g_ccfg_config.m_ser_ip, sizeof(g_ccfg_config.m_ser_ip));
			p_ccfg_write_config((uint8_t*)g_ccfg_config.m_ser_port, sizeof(g_ccfg_config.m_ser_port));
			break;

		case TASK_RE_FORMAT:          //反馈格式化成功
			softuart_printf("task re format\r\n");
			p_proto_creat_data((uint8_t*)s_now_task.m_task.m_data1, strlen((char*)s_now_task.m_task.m_data1));
			p_nblc_port_send_data(g_buff.m_send_buff, g_buff.m_send_index);
			g_run_paramter.m_flg_en_unreg = TRUE;
			p_ccfg_reset();
			break;

		case TASK_REGISTER:            //设备被注册后，第一个任务
			softuart_printf("task register\r\n");
			g_run_paramter.m_flg_en_unreg = FALSE;
			p_init_mercury();
			break;

		case TASK_SWITCHGPS:           //GPS模块控制
			softuart_printf("task switgps\r\n");
			if (TRUE == g_ccfg_config.m_en_gps_rep)
			{
			//	ENABLE_BN220();
			}
			else {
			//	DISABLE_BN220();
			}
			break;

		case TASK_RECV_DATA:           //接收数据
			/* 检查是否有数据需要处理 */
			softuart_printf("task recv data\r\n");
			p_nblc_read_data();
			break;

		case TASK_WAKE_NB:             //唤醒NB
			softuart_printf("task wake nb\r\n");
			p_nblc_port_send_data("alive", 5);
			break;

		case TASK_PASSBY_START:        //开始旁路
			softuart_printf("task passby start\r\n");
			g_run_paramter.m_tim_passby = 2 * g_ccfg_config.m_tim_passby;
			/* 进入睡眠前先清空任务队列 */
			p_task_clear();

			/* 临时盘路时关闭两个水印开关 */
			switch_ctrl(FALSE, FALSE);

			/* 进入休眠 */
			dev_sleep();
			break;
		default:
			break;
		}
	}

}

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif