
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

  /*************************     系统头文件包含     *************************/


/*************************     私有头文件包含     *************************/
#include "ME3616.h"
#include "common_cfg.h"
#include "proto.h"

/*************************      导入命名空间      *************************/


/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

/*************************        全局枚举        *************************/

/*************************       全局宏定义       *************************/
#define C_M26_MAX_PROC_LINE		20
#define MAX_WAIT_AT_SEC			20
#define C_L620_WAIT_CS_PS_SEC   180   //联网180秒
#define	MAX_WAIT_SIM_SEC		30
#define MAX_WAIT_AGPS_SERC		60  
#define KEEP_LOCAT_SEC			15
#define MAX_UNLOCAT_SEC			120   // 最长未定位等待时间
	

/*************************     全局结构体定义     *************************/

/*************************      全局变量定义      *************************/

	st_gps_info g_gps_info = { 0 };
/*************************        函数声明        *************************/

/*************************        函数实现        *************************/
uint8_t p_nblc_common_func(uint8_t send_string, char* at_cmd, char* match_cmd, uint16_t wait_ms, uint8_t(*proc_cb)(char*), u8 iWait_OK)
{
	
	int data_len = 0x00;
	char* line = NULL;
	uint8_t ret_val = FALSE;
	uint8_t max_line = 0x00;
	uint32_t start_time = GET_10MS_TIMER();
	char* tmp_ok = NULL;
	char str_ok[] = "OK";
/******************************* 发送AT指令，等待返回数据 **********************************/
	

#if (NB_COAP_EN == 1)   // 电信平台


	while ((tmp_ok = strstr(g_buff.m_recv_buff, str_ok)) && ((GET_10MS_TIMER() - start_time) < wait_ms))
	{
		memset(tmp_ok, 0x00, sizeof(str_ok));
		tmp_ok = tmp_ok + sizeof(str_ok);
	}

	if (strstr(g_buff.m_recv_buff, "+QLWDATARECV:") == NULL)    // 先处理服务器数据，在处理其他的返回指令
	{
		/* 清理接收缓冲区 */
		g_buff.m_flg_recv_lock = FALSE;  // 解锁
		g_buff.m_recv_index = 0x00;
		memset(g_buff.m_recv_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);
	}
#endif

	/* 发送命令 */
	if (send_string == TRUE)
	{
		p_uart_send_string("AT+");
		p_uart_send_string(at_cmd);
	}
	p_uart_send_string("\r\n");


	while ((GET_10MS_TIMER() - start_time) < wait_ms)        // 规定时间内，如果收到 ERROR/OK/匹配数据 退出
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

/******************* 发送AT指令，等待返回数据 *****************************/

/******************* 逐行匹配数据,并返回匹配结果给函数调用者 ***************/

	//.m_flg_recv_lock = TRUE; // 锁住串口缓冲区开始处理数据
	p_at_format_data(g_buff.m_recv_index);  // 从头格式化到尾指针
	data_len = g_buff.m_recv_index;

	 
	while ( (  (line= p_at_get_line(line,data_len))!= NULL  ) && ((++max_line) < C_M26_MAX_PROC_LINE))
	{
		/*softuart_printf("line is %s\r\n", line);*/
		if (strlen(line) > 0) 
		{
			if (strstr(line,"OK")&& match_cmd == NULL)       // 一旦匹配到OK,就不匹配其他
			{
			    ret_val = TRUE;
			
			}
			else if (strstr(line, "+QGNSSRD:") != NULL)
			{
				u8 get_data = p_dispose_gps_date(line);
				ret_val = strcmp(match_cmd, "+QGNSSRD:") == 0 ? get_data : ret_val;
			}

#if (NB_COAP_EN == 1)   // 电信平台
			else if (strstr(line, "+QLWDATARECV:") != NULL)    // 内含接收数据处理，M2MCLIRECV：数据
			{
				g_l620_net_timeout = GET_10MS_TIMER();
				u8 get_data = p_nblc_proc_read_data(line);
				ret_val = strcmp(match_cmd, "+QLWDATARECV:") == 0 ? get_data : ret_val;  // 如果期望接收M2MCLIRECV，返回数据处理的结果。
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
				/* 交由主动上传数据函数处理 */
			}

		}
	}
	///* 清理接收缓冲区 */
	//g_buff.m_flg_recv_lock = FALSE;  // 解锁
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
	/******************************* 发送AT指令，等待返回数据 **********************************/


	while ((tmp_ok = strstr(g_buff.m_recv_buff, str_ok)) && ((GET_10MS_TIMER() - start_time) < wait_ms))
	{
		memset(tmp_ok, 0x00, sizeof(str_ok));
		tmp_ok = tmp_ok + sizeof(str_ok);
	}

	while ((GET_10MS_TIMER() - start_time) < wait_ms)        // 规定时间内，如果收到 ERROR/OK/匹配数据 退出
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

		/* 清理接收缓冲区 */
		g_buff.m_flg_recv_lock = FALSE;  // 解锁
		g_buff.m_recv_index = 0x00;
		memset(g_buff.m_recv_buff, 0, sizeof(uint8_t) * MAX_RECV_BUFF_SIZE);

	/* 发送命令 */
	if (send_string == TRUE)
	{
		p_uart_send_string("AT+");
		p_uart_send_string(at_cmd);
	}
	p_uart_send_string("\r\n");


	while ((GET_10MS_TIMER() - start_time) < wait_ms)        // 规定时间内，如果收到 ERROR/OK/匹配数据 退出
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

/******************* 发送AT指令，等待返回数据 *****************************/

/******************* 逐行匹配数据,并返回匹配结果给函数调用者 ***************/

	//.m_flg_recv_lock = TRUE; // 锁住串口缓冲区开始处理数据
	p_at_format_data(g_buff.m_recv_index);  // 从头格式化到尾指针
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
	delay_10ms(6);  // 拉低60ms
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

/* 查SIM卡 */
uint8_t p_nblc_get_cimi(void)
{
	return p_nblc_common_func(TRUE, "CIMI", NULL, 100, p_nblc_proc_cimi, FALSE);
}


static uint8_t p_nblc_proc_csq(char* line)
{
	/* NB使用CESQ,2G使用CSQ, CESQ在24以上 表示信号还可以 */
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
	/* 本地 / 漫游 */
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
  * @brief	获取NB的信号强度
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
	asm("nop"); //一个asm("nop")函数经过示波器测试代表100ns 
	asm("nop");
	asm("nop");
	asm("nop");
}

//---- 毫秒级延时程序----------------------- 
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
	GPIO_ResetBits(BC20_POWER_PORT, BC20_POWER_PIN);   // 拉低
	Delayms(500);
	GPIO_SetBits(BC20_POWER_PORT, BC20_POWER_PIN);  // 拉高
	Delayms(100);


}

/**
  * ***********************************************************************
  * @brief	关闭回显
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

	/* 等待启动完成 */
	for (at_time_out = GET_10MS_TIMER() + MAX_WAIT_AT_SEC * 100; GET_10MS_TIMER() < at_time_out; )
	{

		if (FALSE == p_nblc_communic_test())  // 直到回复OK，才发送其他指令
		{
			continue;
		}
		else
		{
			break;
		}
	}

	/* 开启全功能 */
	p_nblc_common_func(TRUE, "CFUN=1", NULL, 100, NULL, FALSE);

	
	for (sim_time_out = GET_10MS_TIMER() + MAX_WAIT_SIM_SEC * 100; GET_10MS_TIMER() < sim_time_out;)
	{
		delay_10ms(100);
	
		
		//	p_uart_send_string("ATI\r\n");

			
			/* 关闭低功耗模式 */
		if (FALSE == p_nblc_common_func(TRUE, "CPSMS=0", NULL, 100, NULL, TRUE))
		{
			continue;
		}

			/* 查询设备标识 */
		if (FALSE == p_nblc_get_cimi())
		{
			continue;
		}

		/* 读取SIM卡ID */
		if (FALSE == p_nblc_get_iccid())
		{
			continue;
		}

		/* 读取imei */
		if (FALSE == p_nblc_get_imei())
		{
			continue;
		}

		/* 读取电压值 */
		if (FALSE == p_nblc_get_vol())
		{
			continue;
		}

		softuart_printf(" g_run_paramter.m_dev_voltage is %d", g_run_paramter.m_dev_voltage);

		

		/* 查询卡准备好了没 */
		if (FALSE == p_nblc_common_func(TRUE, "CPIN?", "READY", 100, NULL, TRUE))
		{
			continue;
		}
		sim_status = TRUE;

		break;


	}
		

	if (sim_status == TRUE)
	{
		/* 新旧扰码算法各试一次 */
		for (uint8_t i = 0; i < 2; i++)
		{
			/* APN设置 */
		//	p_nblc_common_func(TRUE, "CGDCONT=1,\"IP\",\"CTNB\"", NULL, 100, NULL, FALSE);
			for (uint8_t i = 0; i < C_L620_WAIT_CS_PS_SEC / 6; i++)
			{
				/* 核实模块注册上了没 */
				if (TRUE == p_nblc_get_cereg())
				{
					ca_status = TRUE;
					break;
				}
				p_nblc_get_csq();
				delay_10ms(600); // 查询速度太快会影响IP分配
			}
			/* 网络未注册成功更换扰码算法 */
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
  * @brief	初始化AGPS定位
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
		/* 开启AGPS功能 */
		if (FALSE==p_nblc_common_func(TRUE, "QGNSSAGPS=1", NULL, 100, NULL, TRUE))
		{
			delay_10ms(300); // 等待AGPS开启，下载辅助数据成功
			break;
		}
		/* 开启AGPS功能 */
		if (FALSE==p_nblc_common_func(TRUE, "QGNSSAGPS?", "+QGNSSAGPS: 1", 100, NULL, FALSE))
		{
		
			break;
		//	p_nblc_common_func(TRUE, "QGNSSCMD=0,\"$AIDINFO\"", NULL, 100, NULL, TRUE);
			
			
		}
		 /* 开启GNSS电源 */
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
  * @brief	解析GPS数据 GNRMC
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


	/* UTC 时间，hhmmss（时分秒） */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_time, p, sizeof(g_gps_info.m_time) - 1); 
	}

	/* 定位状态，A=有效定位，V=无效定位 */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		if ('V' == *p)
		{
			/* 非有效定位 */
			g_run_paramter.m_gps_ok = FALSE;
			goto out;
		}
		g_run_paramter.m_gps_ok = TRUE;
	}

	/* 纬度 ddmm.mmmm */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_lat, p, sizeof(g_gps_info.m_lat) - 1);
	}

	/* 纬度半球 N（北半球）或 S（南半球） */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		g_gps_info.m_lat_NS = *p;
	}

	/* 经度 dddmm.mmmm */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_lng, p, sizeof(g_gps_info.m_lng) - 1);
	}

	/* 经度半球 E（东经）或 W（西经） */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		g_gps_info.m_lng_EW = *p;
	}

	/* 地面速率（000.0~999.9 节） */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_speed, p, sizeof(g_gps_info.m_speed) - 1);
	}

	/* 地面航向（000.0~359.9 度） */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_direct, p, sizeof(g_gps_info.m_direct) - 1);
	}

	/* UTC 日期，ddmmyy（日月年） */
	if (TRUE == custom_strtok(NULL, &p, ','))
	{
		memcpy(g_gps_info.m_date, p, sizeof(g_gps_info.m_date) - 1);
	}

out:
	return g_run_paramter.m_gps_ok;
}


/*************************        函数实现        *************************/

/**
  * ***********************************************************************
  * @brief	初始化ME3616模块
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
	/* 初始化开机引脚，复位引脚*/
	GPIO_Init(BC20_POWER_PORT, BC20_POWER_PIN, GPIO_Mode_Out_PP_High_Fast);
	GPIO_Init(BC20_RESET_PORT, BC20_RESET_PIN, GPIO_Mode_Out_PP_High_Fast);

	/* 开机 */
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
	/* 检查是否注册上 */
	if (FALSE == p_nblc_get_cereg())
	{
		return FALSE;
	}
#if (NB_COAP_EN == 1)     


	/* 等待启动完成 */
	for (at_time_out = GET_10MS_TIMER() + MAX_WAIT_AT_SEC * 100; GET_10MS_TIMER() < at_time_out; )
	{
		delay_10ms(100);

		/* 删除场景，否则无法重复设置IP与端口 */
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

		/* 配置上行通道与下行通道 */
		if (!p_nblc_common_func(TRUE,"QLWADDOBJ=19,0,1,\"0\"", NULL, 100, NULL, TURE))
		{
			continue;
		}

		if (!p_nblc_common_func(TRUE,"QLWADDOBJ=19,1,1,\"0\"", NULL, 100, NULL, TURE))
		{
			continue;
		}

		/* 直吐模式注册 */
		if (p_nblc_common_func(TRUE,"QLWOPEN=0","+QLWOBSERVE:", 2000, NULL, FALSE))
		{
			return TURE;
		}
	}
	/* 注销 */
	p_nblc_common_func(TRUE, "QLWCLOSE", "CLOSE OK", 500, NULL, TRUE);
	return FALSE;

#endif
	//p_uart_send_string("AT+M2MCLINEW=117.60.157.137,5683,\"");              // 商用平台   
	//p_uart_send_string("AT+M2MCLINEW=117.27.157.39,7879,\"");               // 时刻NB管理平台
	
	//p_uart_send_string("AT+M2MCLINEW=180.101.147.115,5683,\"");             // 测试平台      
	
	
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

/* 服务器协议处理 */
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

		/* 数据指针偏移到数据位置 */
		s += (strlen(s) + 2);							// before data

		if ('\0' == *s) { break; } // data


		/*
		for (u16 i = 0; i < recv_len; i++)
		{
			b[i] = hextoi(s[i * 2]) << 4;
			b[i] |= hextoi(s[i * 2 + 1]);
		}
		*/

		/* 服务器协议处理 */
		++g_run_paramter.m_nb_stat.m_nbiot_recv_cnt;
		p_proto_dispose_data(s, recv_len);
#endif

		/* 重设需要处理时间 */
		g_run_paramter.m_nbiot_start_proc_time = GET_1S_TIMER();

		return TRUE;
	} while (0);

	return FALSE;
}
/**
  * ***********************************************************************
  * @brief	发送UDP数据
  *
  *	@param  udp_data: 需要发送的数据
  *	@param  len: 数据长度
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


		/* 设置上报的模式为hex(1),接收模式为tex(0)*/
	p_nblc_common_func(TURE, "QLWCFG=\"dataformat\",1,0", NULL, 100, NULL, TRUE);
	
	/* 以直吐模式注册到物联网平台 */



	sprintf(udp_buff, "AT+QLWDATASEND=19,0,0,%d,",len); // 结尾自动加\0
	p_uart_send_string(udp_buff);  // 向电信平台发数据
	
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
	/* 重设需要处理时间 */

	g_run_paramter.m_nb_stat.m_nbiot_send_cnt++;
	g_run_paramter.m_nbiot_start_proc_time = GET_10MS_TIMER();
	       //p_nblc_common_func(FALSE, NULL, NULL, 1000, NULL,FALSE);
	return p_nblc_common_func(FALSE, NULL,NULL, 500, NULL, TRUE);  
#else   
	char tmp[20] = { 0x00 };
	/* 发送命令 */
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

	/* 重设需要处理时间 */
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


	/* 设置上报的模式为hex(1),接收模式为tex(0)*/
	p_uart_send_string("AT+QLWCFG=\"dataformat\",1,0\r\n");
	delay_10ms(2);
	/* 以直吐模式注册到物联网平台 */



	sprintf(udp_buff, "AT+QLWDATASEND=19,0,0,%d,", len); // 结尾自动加\0
	p_uart_send_string(udp_buff);  // 向电信平台发数据

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
	/* 重设需要处理时间 */

	g_run_paramter.m_nb_stat.m_nbiot_send_cnt++;
	g_run_paramter.m_nbiot_start_proc_time = GET_10MS_TIMER();
	//p_nblc_common_func(FALSE, NULL, NULL, 1000, NULL,FALSE);
	return p_wait_ok(100);

}
/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




