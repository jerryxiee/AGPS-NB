
/**
  *************************************************************************
  * @file    proto.c
  * @author  LZY.Huaqian
  * @version V2.0.0
  * @date    2020/03/26
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
#include "proto.h"
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

#define CHECK_DEV_PWD(x)		(0 == strncmp((char const *)x, g_ccfg_config.m_dev_pwd, MAX_DEV_PASSWD_SIZE)) // 验证密码

#define CHECK_FLGS(x,y)			(x == s_common_pack->m_flg_2 && y == s_common_pack->m_flg_3)	              // 判断规则包的两个标志位

#define ASSERT_IP(x)			(x <= 0 || x >= 0xff)				                                          // IP地址合法性检查
#define ASSERT_PORT(x)			(x <= 0 || x >= 0xffff)			                                              // 端口合法性检查

#define	RE_MUCH_CONFIG			MUCH CONFIG OK

#define RE_NOSIGN_CONFIG(x)		"NoSignal "_STR(x)"MinuteOFF OK"

#define RE_PASSBY_CONFIG(x)		"Bypass "_STR(x)"Hour OK"

#define RE_RESET(x)				"reset "_STR(x)"respond"

	/* NB模块有收到数据返回数据 */
#define NB_RECV_FLG_STR			"+RECEIVE" 

#define NB_INTO_PSM_STR			"ENTER PSM"
	
	/*************************     全局结构体定义     *************************/


/* 协议解析结构体 */
	typedef struct
	{
		/* 三个标识位 */
		uint8_t m_flg_1;
		uint8_t m_flg_2;
		uint8_t m_flg_3;

		/* 密码 */
		uint8_t m_pwd[MAX_DEV_PASSWD_SIZE];
	}st_common_pack;
	/*************************      全局变量定义      *************************/

	const char CODING[] = "SHIKEGPSCODING";

	/* 对心跳包的反馈 */
	const char  b_udp_respond[] = {0xEF,0xA3,0xB5,0x3F,0x31,0x30,0x03,0};/*{ 0xff,0xDD,0x02,0x31,0x30,0x03, 0 };*/

	static st_common_pack* s_common_pack = NULL;
	uint8_t server_key[MAX_SERVER_KEY_SIZE] = { 0 };

	static uint8_t s_respond_buf[50] = { 0 };


	/*************************        函数声明        *************************/

	/*************************        函数实现        *************************/



		/* 生成服务器密钥 */
	void p_proto_gen_server_key(void)
	{
		for (uint8_t i = 0; i < STRLEN(DEFAULT_DEV_IDEN_ID); i++)
		{
			uint8_t conv = g_ccfg_config.m_dev_iden_id[i];  // 设备ID从哪里来？  =》 cfg.c 载入默认配置

			conv = ((conv << 4) & 0xF0) | (((conv >> 4) & 0x0F));
			g_run_paramter.m_dev_iden_id[i] = conv ^ CODING[i]; // 按位异或生成加密ID

			/* 生成server key */
			server_key[2 * i] = itohex((g_run_paramter.m_dev_iden_id[i] & 0xf0) >> 4);  // 将ID生成密钥
			server_key[2 * i + 1] = itohex(g_run_paramter.m_dev_iden_id[i] & 0x0f);
			// sprintf((char*)server_key + (i*2), "%02X", g_run_paramter.m_dev_iden_id[i]);
		}
	}
	/*
		三十二.定位数据和未定位数据格式
			1.定位数据格式如下
			* MNOPQRSTUVWXYZxxxxxxEFGH IJ11 * 经度, E, 纬度, N, 速度, 航向* 日期, 时间, 序号**
			2.未定位数据格式如下
			% MNOPQRSTUVWXYZxxxxxxEFGH11 % Cell, , C, , Lac, L, 0.0, 000.000 % 000000, 000000.000 %
	*/
	/**
  * ***********************************************************************
  * @brief	构造数据包头部
  *
  *	@param  p_buff: 缓冲区
  *	@param  pkg_type: 数据包类型
  *
  * @retval uint8_t:
  *
  * @attention	: none
  * ***********************************************************************
  */
	uint16_t p_proto_creat_pkg_head(uint8_t* p_buff, uint16_t* p_index, PKG_Type_TypeDef pkg_type)
	{
		// uint8_t w_ptr = 1;

		*p_index = 0;

		if (TRUE == g_run_paramter.m_gps_ok || PKG_DATA == pkg_type)
		{
			/* GPS 定位 */
			p_buff[0] = '*';
		}
		else {
			p_buff[0] = '%';
		}
		++(*p_index);

		/* 加密后的身份ID */
		memcpy(p_buff + 1, server_key, sizeof(char) * MAX_SERVER_KEY_SIZE);  // 将加密的身份ID写到缓冲区
		(*p_index) += MAX_SERVER_KEY_SIZE;

		/* 设备编号 */                 // sprintf(*string,%s,x)  将x以字符串的形式写入到string字符串
		(*p_index) += sprintf((char*)p_buff + (*p_index), "%s", g_ccfg_config.m_dev_guid);

		/* 电压与信号 */
		// TODO: 电压 信号未注明
		(*p_index) += sprintf((char*)p_buff + (*p_index), "%u.%uV%02u",
			g_run_paramter.m_dev_voltage / 10, g_run_paramter.m_dev_voltage % 10
			, g_run_paramter.m_nb_stat.m_nbiot_csq);

		if (PKG_DATA == pkg_type)
		{
			/* 数据包 */
			(*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "&$*");
		}
		else if (PKG_HEARTBEAT == pkg_type)
		{
			/* 报警数据 */
			if (TRUE == g_run_paramter.m_gps_ok) {
				/* GPS 定位 */
				if (TRUE == g_run_paramter.m_flg_alarm)
				{	/* 有警情 */
					(*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "0L*");
				}
				else { (*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "0H*"); }

			}
			else {
				/* 基站定位 */
				if (TRUE == g_run_paramter.m_flg_alarm)
				{	/* 有警情 */
					(*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "1L%");
				}
				else { (*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "1H%"); }
			}
		}



		return (*p_index);
	}


	uint16_t p_proto_creat_heartbeat(void)
	{
		// uint16_t index = 0;
		uint16_t w_ptr = 0;


		/* 如果GPS不在状态则进行基站定位 */
		if (g_run_paramter.m_gps_ok == FALSE)
		{
			/* 获取基站定位数据 */
		}

		/* 清缓存 */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		/* 构建数据包的头部 */
		w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_HEARTBEAT);
 
		/* 根据GPS状态返回的定位信息 */
		if (g_run_paramter.m_gps_ok == TRUE)
		{
			/* GPS定位 */
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%s,%c,%s,%c,%s,%s*%s,%s.%03u**",
				g_gps_info.m_lng, g_gps_info.m_lng_EW,     // 经度与标号
				g_gps_info.m_lat, g_gps_info.m_lat_NS,     // 纬度与标号
				g_gps_info.m_speed, g_gps_info.m_direct,   // 速度与方向
				g_gps_info.m_date, g_gps_info.m_time,
				++g_run_paramter.m_pkg_num % 1000);     // 日期与时间

		//	memset(&g_gps_info, 0x00, sizeof(g_gps_info));
		}
		else
		{
			/* 基站定位 */
			// w_ptr += sprintf(g_c_m26l_sbuf + w_ptr, "%%");
#if 0
			w_ptr += dec2dms((char*)g_buff.m_send_buff + w_ptr, (char*)g_gps_info.m_lng);        // 经度
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, ",E,");                   // 经度标号
			w_ptr += dec2dms((char*)g_buff.m_send_buff + w_ptr, (char*)g_gps_info.m_lat);        // 纬度
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, ",N,000.0,000.0%%000000,000000.%03u%%%%",
				++g_run_paramter.m_pkg_num % 1000);    //纬度标号及其他
#endif
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr,
		 "00000.00000,E,0000.00000,N,000.0,000.0%%000000,000000.%03u%%%%",
				++g_run_paramter.m_pkg_num % 1000);    //纬度标号及其他
		}
		
		/* 发送缓冲区的长度 */
		g_buff.m_send_index = w_ptr;

		return w_ptr;
	}
	/**
	  * ***********************************************************************
	  * @brief	协议解析函数
	  *
	  *	@param  buf:
	  *	@param  len:
	  *
	  * @retval uint8_t:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */

	/**
	  * ***********************************************************************
	  * @brief	构建请求包
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	void p_proc_creat_iot_queryid(void)
	{
		uint16_t w_ptr = 1;

		// 以下是服务器接收到的数据
		//  869662032467935: *376B DA2826645360600C273ADD74 FFFFFF 0.0V34 00*00000.0000,E,0000.0000,N,0.00,0.00*000000,000000**
		softuart_printf("creat queryid\r\n");
		/* 清缓存 */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		g_buff.m_send_buff[0] = '*';

		/**
		* 物联网默认类型F1，密文375B
		* 新版 F2 密文376B ,唯一码使用MC20的IMEI码
		*/
		g_buff.m_send_buff[w_ptr++] = '3';
		g_buff.m_send_buff[w_ptr++] = '7';
		g_buff.m_send_buff[w_ptr++] = '6';
		g_buff.m_send_buff[w_ptr++] = 'B';

		for (int i = 0; i < MAX_IOT_ID_SIZE; i++)
		{
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%02X", g_run_paramter.m_iot_id[i]);
		}

		/* 设备编号 */
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%s", STR(DEFAULT_DEV_SER_GUID));

		/* 电压与信号 */
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u.%uV%02u",
			g_run_paramter.m_dev_voltage / 10, g_run_paramter.m_dev_voltage % 10
			, g_run_paramter.m_nb_stat.m_nbiot_csq);

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "00*00000.0000,E,0000.0000,N,0.00,0.00*000000,000000**");

		g_buff.m_send_index = w_ptr;

	}

	/**
	  * ***********************************************************************
	  * @brief	IMEI码获取后才能，生成校验码（未注册时使用）
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	void p_proto_gen_iotid(void)
	{

		for (u8 i = 0; i < 12; i++)
		{
			u8 conv = g_run_paramter.m_imei[i + 2];

			conv = ((conv << 4) & 0xF0) | (((conv >> 4) & 0x0F));
			g_run_paramter.m_iot_id[i] = conv ^ CODING[i + 2];
		}
	}

	/**
* ***********************************************************************
* @brief	回复查询短信 1
*
*	@param  char * tel:
*
* @retval void:
*
* @attention
* ***********************************************************************
*/
	void p_proto_reply_setting_1(void)
	{
		uint8_t w_ptr = 0;
		uint8_t line = 0;

		/* 先清理缓存 */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		/* 第一条短信 */
		{
			w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_DATA);

			/************** 1-8 电话号码表 ****************/
			while (line++ < 8)
			{
				w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
					line, g_ccfg_config.m_tel_list[line - 1]);
			}

			/************** 9 上报GPS时间间隔 *************/
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
				line++, g_ccfg_config.m_tim_gps_rep);


			/************** 10 关机延时时间 ***************/
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
				line++, g_ccfg_config.m_tim_pwr_delay);

			/************** 11 定时报告时间间隔 ***********/
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
				line++, g_ccfg_config.m_tim_sleep);

			// p_m26l_send_udp_data(0, g_buff.m_send_buff);
			p_nblc_port_send_data(g_buff.m_send_buff, w_ptr);
		}

	}

	/**
	  * ***********************************************************************
	  * @brief	回复设置短信 2
	  *
	  *	@param  :
	  *
	  * @retval void:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	void p_proto_reply_setting_2(void)
	{
		uint8_t w_ptr = 0;
		uint8_t line = 12;

		/* 先清理缓存 */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_DATA);

		/* 因为数据包的头部内容相同，所以只要将写地址前移就可以 */

		/************** 12 设备编号 *******************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_dev_guid);

		/************** 13 使能定时上报 ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_tim_rep ? "55" : "44");

		/************** 14 设备密码 *******************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_dev_pwd);

		/************** 15 使能低压检测 ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_vol_check ? "55" : "44");

		/************** 16 水银开关触发阈值 ***********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_cnt_sw_trigger);

		/************** 17 空调模式/普通模式 **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"44\"$", line++);

		/************** 18 使能定时上报 ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_tim_rep ? "55" : "44");

		/************** 19 使能水银开关1 **************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_sw1 ? "55" : "44");

		/************** 20 使能水银开关2 **************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_sw2 ? "55" : "44");

		/************** 21 接入点名称 *****************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"0\"$", line++);

		/************** 22 设备IP地址及端口号 *********/
#if 0
		w_ptr += sprintf((char*)g_c_m26l_sbuf + w_ptr, "%u\"%u.%u.%u.%u:%u\"$",
			line++, g_ccfg_config.b_ser_ip[0], g_ccfg_config.b_ser_ip[1],
			g_ccfg_config.b_ser_ip[2], g_ccfg_config.b_ser_ip[3],
			g_ccfg_config.s_ser_port);
#endif
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"", line++);

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%s:%s", SERVER_IP(IP_1, IP_2, IP_3, IP_4), SERVER_PORT(PORT));

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "\"$");

		/************** 23 GPRS心跳包间隔 *************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_heartbeat);

		/************** 24 报警时GPRS工作时间 *********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_alm_continue);

		/************** 25 设备身份ID *****************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"", line++);

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "05951234123456");

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "\"$");

		/************** 26 设备工作模式 ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_en_gprs);

		/************** 27 GPRS报警时间间隔 ***********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_alm_interval / 60);

		/************** 28 1-5组电话上报标志 **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%c\"$",
			line++, g_ccfg_config.m_en_1to5_rep ? '1' : '0');

		/************** 29 APN接口 **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_dev_APN);
		/************** 30 开机无信号延时休眠 **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_no_signal_delay);

		// p_m26l_send_udp_data(0, g_buff.m_send_buff);
		p_nblc_port_send_data(g_buff.m_send_buff, w_ptr);
	}

	/**
  * ***********************************************************************
  * @brief	协议解析函数
  *
  *	@param  buf:
  *	@param  len:
  *
  * @retval uint8_t:
  *
  * @attention	: none
  * ***********************************************************************
  */
	uint8_t p_proto_dispose_data(char* buf, int len)
	{
		uint16_t tmp_val = 0;
		uint8_t ret_val = FALSE;
		/* udp反馈数据位置 */
		char* udp_rep = NULL;
	

		//softuart_printf("begin to analysis the server data\r\n");
		//for (uint8_t i = 0; i < len; i++)
		//{
		//	softuart_printf("%X", buf[i]);
		//
		//}
		//for (uint8_t i = 0; i < len; i++)
		//{
		//	softuart_printf("%X", b_udp_respond[i]);
		//}

	//	softuart_printf("strstr(buf, b_udp_respond) is %X", strstr(buf, b_udp_respond));

		if (NULL == buf)
		{
			goto out;
		}

#ifdef USE_UDP

		/* UDP 反馈包 */
		if (NULL != (udp_rep = strstr(buf, b_udp_respond)))
		{
			softuart_printf("recv the udp respond!\r\n");
			//GPIO_ResetBits(LED_GPS_PORT, LED_GPS_PIN);
			/* 接收到到反馈包 */
			g_run_paramter.m_heartbeat_respond = TURE;
			g_run_paramter.m_udp_transfer.m_flg_sending = FALSE;
			g_run_paramter.m_udp_transfer.m_flg_retry = TRUE;
			g_run_paramter.m_udp_transfer.m_cnt_send_fail = 0;
			++g_run_paramter.m_nb_stat.m_nbiot_recv_cnt;

			//GPIO_ResetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);
			//191121 刷新时间
			

			/* 判断反馈包的尾部是否有跟其他的数据 */
			if (len <= sizeof(b_udp_respond))
			{
			//	softuart_printf("have not nother data!\r\n");
				goto out;
			}

			/* 反馈再前面，数据在后面，跳过反馈，继续解析数据 */
			if (buf == udp_rep)
			{
			//	softuart_printf("have nother data!\r\n");
				buf += sizeof(b_udp_respond);
			}
		}
#endif

		s_common_pack = (st_common_pack*)buf;

		/* 判断是否是规则数据包 */

		softuart_printf("judge is real data\r\n");
		delay_10ms(1);
		if ('*' == s_common_pack->m_flg_1)
		{
			/* 特殊包体 */
			if (0 == strncmp(buf, "*#*#", 4))
			{
				/* 多项编程 */

				/* 跳过*#*# */
				buf += 4;

				/* 验证密码 */
				if (!CHECK_DEV_PWD(buf))
				{
					/* 密码错误 */
					goto out;
				}

				/* 跳过密码 */
				buf += 5;

				// much_config_flg = TRUE;
				uint8_t _much_config_num = 0;
				// uint8_t* tmp = NULL;
				// buf = strtok(buf, "*");
				do {
					/* 获取多项编程的编号 */
					_much_config_num = atoi((const char*)buf);  
					softuart_printf("_much_config_num is %d",(int) _much_config_num);
					switch (_much_config_num)
					{
					case 1:
						/* 手机号码 */
						break;
					case 2:
						/* 手机号码 */
						break;
					case 3:
						/* 手机号码 */
						break;
					case 4:
						/* 手机号码 */
						break;
					case 5:
						/* 手机号码 */
						break;
					case 6:
						/* 手机号码 */
						break;
					case 7:
						/* 手机号码 */
						break;
					case 8:
						/* 手机号码 */
						break;
					case 9:
						/* xx=01-60分钟,(每隔一段时间上报GPS数据) 定时上报持续时间 */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_GPS_REP && tmp_val <= MAX_TIM_GPS_REP)
						{
							g_ccfg_config.m_tim_gps_rep = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_gps_rep,
								sizeof(g_ccfg_config.m_tim_gps_rep));
							g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep;
						}
						else {
							/* 非法值 */
						}

						break;
					case 10:
						/* xx= 05-60分钟(延时一段时间关机) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_PWR_DELAY && tmp_val <= MAX_TIM_PWR_DELAY)
						{
							g_ccfg_config.m_tim_pwr_delay = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_pwr_delay,
								sizeof(g_ccfg_config.m_tim_pwr_delay));
							g_run_paramter.m_tim_pwr_off_delay = g_ccfg_config.m_tim_pwr_delay * 60;
						}
						else {
							/* 非法值 */

						}

						break;
					case 11:
						/* xx= 001-999小时(定时报告) 设备的睡眠时间 */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_SLEEP && tmp_val <= MAX_TIM_SLEEP)
						{
							g_ccfg_config.m_tim_sleep = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_sleep,
								sizeof(g_ccfg_config.m_tim_sleep));
							 g_run_paramter.m_tim_sleep = g_ccfg_config.m_tim_sleep;
						}
						else {
							/* 非法值 */

						}

						break;
					case 12:
						softuart_printf("case 12");
						/* xx= 6位数字主机编号 */
						if (TRUE == g_run_paramter.m_flg_en_unreg && TRUE == is_all_num((uint8_t*)buf + 3, 6))
						{
							memcpy(g_ccfg_config.m_dev_guid, buf + 3, 6);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_guid,
								sizeof(g_ccfg_config.m_dev_guid));
							softuart_printf("save the dev_guid");
							g_run_paramter.m_flg_en_unreg = FALSE;
							/* 标注已经被注册 */
						//	p_task_enqueue(TASK_REGISTER, NULL, NULL);
						}
						else {
							/* 非法值 */
						}

						break;
					case 13:
						/* xx=55或 44,( 55开启GPS数据自动上报,44关闭GPS数据自动上报) */
						if ('5' == buf[3] && '5' == buf[4])
						{
							/* 使能上报GPS数据 */
							g_ccfg_config.m_en_gps_rep = TRUE;
						}
						else
						{
							g_ccfg_config.m_en_gps_rep = FALSE;
						}
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_gps_rep,
							sizeof(g_ccfg_config.m_en_gps_rep));
						p_task_enqueue(TASK_SWITCHGPS, NULL, NULL);
						break;
					case 14:
						/* 密码在此不可编程 */
						break;
					case 15:
						/* xx=55或 44 (允许检测外部充电线路和电池低压55为开，44为关) */
						if ('5' == buf[3] && '5' == buf[4])
						{
							/* 使能上报GPS数据 */
							g_ccfg_config.m_en_vol_check = TRUE;
						}
						else
						{
							g_ccfg_config.m_en_vol_check = FALSE;
						}
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_vol_check,
							sizeof(g_ccfg_config.m_en_vol_check));
						break;
					case 16:
						/* xx= 01-09(初次水银触发报警次数) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= 1 && tmp_val <= 9)
						{
							g_ccfg_config.m_cnt_sw_trigger = tmp_val;
							
							// 应客户要求，断电不保存。 200622
							/*p_ccfg_write_config((uint8_t*)g_ccfg_config.m_cnt_sw_trigger,
								sizeof(g_ccfg_config.m_cnt_sw_trigger));*/
						}
						else {
							/* 非法值 */

						}
						break;
					case 17:
						/* xx=55或 44 ( 44为普通模式,55为空调模式) */
						break;
					case 18:
						/* xx=55或 44(55为开启定时报告,44为关闭定时报告) */
						if ('5' == buf[3] && '5' == buf[4])
						{
							/* 使能上报GPS数据 */
							g_ccfg_config.m_en_tim_rep = TRUE;
						}
						else
						{
							g_ccfg_config.m_en_tim_rep = FALSE;
						}
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_tim_rep,
							sizeof(g_ccfg_config.m_en_tim_rep));

						break;
					case 19:
						/* xx=55或 44（55开启第一个水银工作,44为关闭第一个水银工作） */
						if ('4' == buf[3] && TRUE == g_ccfg_config.m_en_sw2)
						{
							g_ccfg_config.m_en_sw1 = FALSE;
							g_ccfg_config.m_en_sw2 = TRUE;

						}
						else {
							g_ccfg_config.m_en_sw1 = TRUE;
						}

						switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
						break;
					case 20:
						/* xx=55或 44（55开启第二个水银工作, 44为关闭第二个水银工作） */
						if ('4' == buf[3] && TRUE == g_ccfg_config.m_en_sw1)
						{
							g_ccfg_config.m_en_sw1 = TRUE;
							g_ccfg_config.m_en_sw2 = FALSE;

						}
						else {
							g_ccfg_config.m_en_sw2 = TRUE;
						}
						switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
						break;
					case 21:
						/* xx=0-4(接入点名称),(0= CMNET,1=CMWAP,2=uninet,3=uniwap) */
						break;
					case 22:
						/* xx=IP地址和端口号,如220.162.13.122:6868 */
#if 0
						sscanf((const char*)buf + 3, "%d.%d.%d.%d:%d",
							g_run_paramter.m_ser_ip, g_run_paramter.m_ser_ip + 1,
							g_run_paramter.m_ser_ip + 2, g_run_paramter.m_ser_ip + 3,
							&g_run_paramter.m_ser_port);

						p_task_enqueue(TASK_SERVER_CFG, NULL, NULL);
#endif
						break;
					case 23:
						/* xx=30-99秒钟(GPRS心跳时间) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_HEARTBEAT && tmp_val <= MAX_TIM_HEARTBEAT)
						{
							g_ccfg_config.m_tim_heartbeat = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_heartbeat, sizeof(g_ccfg_config.m_tim_heartbeat));
						}
						else {
							/* 非法值 */

						}
						break;
					case 24:
						/* xx=1-9分钟(每报警一次GPRS连续工作时间) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_ALM_CONTINUE && tmp_val <= MAX_TIM_ALM_CONTINUE)
						{
							g_ccfg_config.m_tim_alm_continue = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_alm_continue,
								sizeof(g_ccfg_config.m_tim_alm_continue));

							g_run_paramter.m_tim_alm_continue += g_ccfg_config.m_tim_alm_continue * 60;

						}
						else {
							/* 非法值 */
						}

						break;
					case 25:
						/* xx=14位数字身份识别码 */
						if (TRUE == is_all_num((uint8_t*)buf + 3, 14))
						{
							memcpy(g_ccfg_config.m_dev_iden_id, buf + 3, 14);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_iden_id,
								sizeof(g_ccfg_config.m_dev_iden_id));

							/* 生成服务器的key 用于服务器过滤 */
							p_proto_gen_server_key();

							/* 注册成功过后需要发给服务器 */
						//	p_task_enqueue(TASK_HEARTBEAT, NULL, NULL);
							p_task_enqueue(TASK_SEND_ICCID, NULL, NULL);
							p_task_enqueue(TASK_SEND_FWVER, NULL, NULL);
						}
						else {
							/* 非法值 */
						}

						break;
					case 26:
						/* xx=0或1(网络工作模式,0=GSM,1=GPRS) */
						break;
					case 27:
						/* XX=05-60秒,GPRS模式下5分钟内报警发GPS数据时间 */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_ALM_INTERVAL && tmp_val <= MAX_TIM_ALM_INTERVAL)
						{
							g_ccfg_config.m_tim_alm_interval = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_alm_interval,
								sizeof(g_ccfg_config.m_tim_alm_interval));
						}
						break;
					case 28:
						/* 第1组至第3组,是否短信上报状态 */
						break;
					default:
						break;
					}

					/* 跳过一个 '*' */
					// buf = strtok(NULL, "*");

					/* 判断多项编程是都结束 */
					if (strstr(buf + 3, "**"))
					{
						p_task_enqueue(TASK_SEND_DATA, STR(RE_MUCH_CONFIG), NULL);
						goto out;
					}

					buf += strlen(buf) + 1;
				} while (*buf);

			}
			else if ('*' == s_common_pack->m_flg_2) {
				/* 修改密码 */
				memcpy(g_ccfg_config.m_dev_pwd, buf + 7, 4);
				p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_pwd, sizeof(g_ccfg_config.m_dev_pwd));
				p_task_enqueue(TASK_SEND_DATA, "CODE OK", NULL);
			}
			else {

				/* 设置报警电话号码和状态电话号码 */
				uint8_t* str1 = NULL, * p_pwd = NULL;
				uint8_t group = 0;
				str1 = (uint8_t*)strtok(buf, "*");
				p_pwd = (uint8_t*)strtok(NULL, "*");
				/* 判断密码是否正确 */
				if (!CHECK_DEV_PWD(p_pwd))
				{
					/* 密码不正确 */
					goto out;
				}
				/* 获取组号 */
				group = atoi((buf = strtok(NULL, "*")));
				// sscanf(buf, "*%[^*]*%[^*]*%d*")
				switch (group)
				{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					/* 设置报警电话号码和状态电话号码 */
					//TODO：回复短信:	电话号码		CONFIG  OK
					p_task_enqueue(TASK_SEND_DATA, " CONFIG OK", NULL);
					break;
				case 9:
					/* 设置IP地址和端口号 */
					// *220.162.13.122:6868*0000*9**
#if 0
					sscanf((const char*)str1, "%d.%d.%d.%d:%d",
						g_run_paramter.m_ser_ip, g_run_paramter.m_ser_ip + 1,
						g_run_paramter.m_ser_ip + 2, g_run_paramter.m_ser_ip + 3,
						&g_run_paramter.m_ser_port);
#endif
					p_task_enqueue(TASK_SERVER_CFG, NULL, NULL);
					p_task_enqueue(TASK_SEND_DATA, "9CONFIG OK", NULL);
					break;
				case 10:
					/* 设置GPRS心跳包时间 */
					// *xx*0000*10**,xx=30-99秒,默认60秒
					tmp_val = atoi((const char*)str1);
					if (tmp_val >= MIN_TIM_HEARTBEAT && tmp_val <= MAX_TIM_HEARTBEAT)
					{
						g_ccfg_config.m_tim_heartbeat = tmp_val;
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_heartbeat, sizeof(g_ccfg_config.m_tim_heartbeat));
						p_task_enqueue(TASK_SEND_DATA, "10CONFIG OK", NULL);
					}
					else {
						/* 非法值 */
					}

					break;
				case 11:
					/* 设置身份码 */
					// *x*0000*11**, x=14位身份码
					if (strlen((char*)str1) == 14) {
						memcpy(g_ccfg_config.m_dev_iden_id, str1, 14);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_iden_id, sizeof(g_ccfg_config.m_dev_iden_id));
						p_task_enqueue(TASK_SEND_DATA, "11CONFIG OK", NULL);
					}
					else {
						/* 非法值 */
					}
					break;
				case 12:
					/* 设备编号 */
					// *x*0000*12**, x=6位数字
					if (strlen((char*)str1) == 6 && TRUE == is_all_num(str1, 6)) {
						memcpy(g_ccfg_config.m_dev_guid, str1, 6);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_guid, sizeof(g_ccfg_config.m_dev_guid));

						p_task_enqueue(TASK_SEND_DATA, "12CONFIG OK", NULL);
						p_task_enqueue(TASK_SWITCHGPS, NULL, NULL);  // LZY添加，标志设备已注册
					}
					else {
						/* 非法值 */
					}
					break;
				case 13:
					/* 设置在GPRS模式下5分钟内报警发GPS数据时间(5-60秒) */
					// *xx*0000*13**,xx=05-60秒,默认10秒
					tmp_val = atoi((const char*)str1);

					if (tmp_val >= MIN_TIM_ALM_INTERVAL && tmp_val <= MAX_TIM_ALM_INTERVAL) {
						g_ccfg_config.m_tim_alm_interval = atoi((const char*)str1);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_alm_interval, sizeof(g_ccfg_config.m_tim_alm_interval));
						p_task_enqueue(TASK_SEND_DATA, "13CONFIG OK", NULL);
					}
					else {
						/* 非法值 */
					}
					break;
				case 14:
					/* 设置1组至3组短信报警状态是否开通上报 */
					//  *xx*0000*13**,xx=05-60秒,默认10秒
					p_task_enqueue(TASK_SEND_DATA, "14CONFIG OK", NULL);
					break;
				case 29:
					/* 获取APN长度 */
					// *APN名称*ABCD*29**
					// g_ccfg_config.m_dev_APN;
					break;
				case 30:
					/*设置无信号延时关机的时间*/
					// *05 * 0000 * 30 * *  单位：分钟
					tmp_val = atoi((const char*)str1);
					if (tmp_val >= MIN_TIM_NO_SIGNAL_DELAY && tmp_val <= MAX_TIM_NO_SIGNAL_DELAY)
					{
						g_ccfg_config.m_tim_no_signal_delay = tmp_val;
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_no_signal_delay,
							sizeof(g_ccfg_config.m_tim_no_signal_delay));
						memset(s_respond_buf, 0x00, sizeof(s_respond_buf));
						sprintf((char*)s_respond_buf, "NoSignal %dMinuteOFF OK", g_ccfg_config.m_tim_no_signal_delay);
						p_task_enqueue(TASK_SEND_DATA, (char*)s_respond_buf, NULL);
					}
					else {
						/* 非法值 */
					}

					break;
				case 31:
					/*进入旁路状态不处理水银开关的时间*/
					// *05*0000*31**  单位：小时
					tmp_val = atoi((const char*)str1);
					if (tmp_val >= MIN_PASSBY_TIME && tmp_val <= MAX_PASSBY_TIME)
					{
						g_ccfg_config.m_tim_passby = tmp_val;
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_passby,
							sizeof(g_ccfg_config.m_tim_passby));
						memset(s_respond_buf, 0x00, sizeof(s_respond_buf));
						sprintf((char*)s_respond_buf, "Bypass %dHour OK", g_ccfg_config.m_tim_passby);
						p_task_enqueue(TASK_SEND_DATA, (char*)s_respond_buf, NULL);
						p_task_enqueue(TASK_PASSBY_START, (char*)s_respond_buf, NULL);
					}
					else {
						/* 非法值 */

					}

					break;
				default:
					break;
				}
			}
		}
		else
		{
			/* 以下数字开头数据包 */

			/* 禁止某组报警电话重拔 */
			if (0 == strncmp(buf, "0000", 4))
			{
				//TODO：回复短信:PAUSE ALARM
			}

			if (CHECK_DEV_PWD(s_common_pack->m_pwd))
			{
				switch (s_common_pack->m_flg_1)
				{
				case '1':
					if (CHECK_FLGS('4', '7'))
					{
						/* 147 删除4-7组的报警电话 */
						p_ccfg_del_tel(3);
						p_ccfg_del_tel(4);
						p_ccfg_del_tel(5);
						p_ccfg_del_tel(6);
						p_task_enqueue(TASK_SEND_DATA, "DEL47 OK", NULL);
					}
					else if (CHECK_FLGS('2', '3'))
					{
						/* 123 关闭低压检测 */
						g_ccfg_config.m_en_vol_check = FALSE;
						p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_vol_check, sizeof(g_ccfg_config.m_en_vol_check));
						p_task_enqueue(TASK_SEND_DATA, "NOCHEK OK", NULL);
					}
					else if (CHECK_FLGS('2', '4'))
					{
						/* 124 开启充电检测 */
						g_ccfg_config.m_en_vol_check = TRUE;
						p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_vol_check, sizeof(g_ccfg_config.m_en_vol_check));
						p_task_enqueue(TASK_SEND_DATA, "CHEK OK", NULL);
					}
					break;
				case '3':
					if (CHECK_FLGS('3', '3'))
					{
						/* 333 每隔一段时间自动上报GPS数据关闭 */
						g_ccfg_config.m_en_gps_rep = FALSE;
						p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_gps_rep, sizeof(g_ccfg_config.m_en_gps_rep));
						p_task_enqueue(TASK_SWITCHGPS, NULL, NULL);
						p_task_enqueue(TASK_SEND_DATA, "OFF OK", NULL);
					}
					else if ('0' == s_common_pack->m_flg_2)
					{
						/* 30x 触发一次报警连续发GPRS数据时间 x=1-9分钟 */
						tmp_val = s_common_pack->m_flg_3 - '0';
						if (MIN_TIM_ALM_CONTINUE <= tmp_val && tmp_val <= MAX_TIM_ALM_CONTINUE)
						{
							g_ccfg_config.m_tim_alm_continue = tmp_val;
						//	p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_tim_alm_continue, sizeof(g_ccfg_config.m_tim_alm_continue));
							p_task_enqueue(TASK_SEND_DATA, "CONTINUE OK", NULL);

							g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;
						}
						else {
							/* 非法值 */
						}

					}
					break;
				case '4':
					if ('*' == s_common_pack->m_pwd[4])
					{
						/* GPS定时上报指定电话组 */
						//TODO:回复短信:TIME OK
					}
					else
					{
						/* 每隔一段时间自动上报GPS数据设置(启动后延时关机功能失效) */
						tmp_val = (s_common_pack->m_flg_2 - '0') * 10 + (s_common_pack->m_flg_3 - '0');
						if (tmp_val >= MIN_TIM_GPS_REP && tmp_val <= MAX_TIM_GPS_REP)
						{
							g_ccfg_config.m_tim_gps_rep = tmp_val;
							p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_tim_gps_rep, sizeof(g_ccfg_config.m_tim_gps_rep));
							g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep * 60;
							p_task_enqueue(TASK_SEND_DATA, "TIME OK", NULL);

							/* 在GPS模块关闭时，打开GPS模块 */
							if (FALSE == g_ccfg_config.m_en_gps_rep)
							{
								g_ccfg_config.m_en_gps_rep = TRUE;
								p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_gps_rep, sizeof(g_ccfg_config.m_en_gps_rep));
								p_task_enqueue(TASK_SWITCHGPS, NULL, NULL);
							}
						}
						else {
							/* 非法值 */
						}

					}
					break;
				case '5':
					/* 5xx 短延时一段时间关机设置 xx=05-60分钟 */
					tmp_val = (s_common_pack->m_flg_2 - '0') * 10 + (s_common_pack->m_flg_3 - '0');
					if (tmp_val >= MIN_TIM_PWR_DELAY && tmp_val <= MAX_TIM_PWR_DELAY)
					{
						g_ccfg_config.m_tim_pwr_delay = tmp_val;
						g_run_paramter.m_tim_pwr_off_delay = g_ccfg_config.m_tim_pwr_delay * 60;
						/* 直接更新休眠倒数时间 */
						g_run_paramter.m_tim_gps_rep = g_run_paramter.m_tim_pwr_off_delay ;
					
					
						p_task_enqueue(TASK_SEND_DATA, "CLOS OK", NULL);
					}
					else {
						/* 非法值 数值过大 */
					}

					break;
				case '6':
					if (CHECK_FLGS('6', '6'))
					{
						/* 查询GPS数据 */
						//TODO:回复GPS数据.
					}
					else {
						/* 初次触发报警次数设置 */
						tmp_val = (s_common_pack->m_flg_2 - '0') * 10 + (s_common_pack->m_flg_3 - '0');
						if (MIN_CNT_SW_TRIGGER <= tmp_val && 9 >= MAX_CNT_SW_TRIGGER)
						{
							g_ccfg_config.m_cnt_sw_trigger = tmp_val;
							// 应客户要求，报警次数掉电不保存 200622
							//ccfg_write_config((uint8_t*)&g_ccfg_config.m_cnt_sw_trigger, sizeof(g_ccfg_config.m_cnt_sw_trigger));
							p_task_enqueue(TASK_SEND_DATA, "SPRING OK", NULL);
						}
						else {
							/* 非法值 */
						}

					}
					break;
				case '7':
					if (CHECK_FLGS('7', '7'))
					{
						/* 长延时关机(可延时大约2小时后关机) */
					//	g_run_paramter.m_tim_pwr_off_delay = 120;
					//	g_run_paramter.m_tim_gps_rep = 120 * 60;
						p_task_enqueue(TASK_SEND_DATA, "DEFER OK", NULL);
					}
					else if ('7' == s_common_pack->m_flg_2)
					{
						/* 77x 设置GPRS接入点名称代码(0= CMNET,1=CMWAP,2=uninet,3=uniwap) */
						if (s_common_pack->m_flg_3 <= 3)
						{
							memcpy(g_ccfg_config.m_dev_APN, "CTNB", 4);
							p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_dev_APN, sizeof(g_ccfg_config.m_dev_APN));
							// s_common_pack->m_flg_3;
						}
						else {
							/* 非法值 */
						}
					}
					else if ('6' == s_common_pack->m_flg_2)
					{
						/* 76x 选择GPRS或GSM工作模式(1=GPRS,0=GSM) */
						//TODO：	回复短信:MODE GSM		回复短信 : MODE GPRS

					}
					break;
				case '8':
					if (CHECK_FLGS('8', '1'))
					{
						/* 881 查询GPS版本号 */
						p_task_enqueue(TASK_SEND_FWVER, NULL, NULL);

					}
					else if (CHECK_FLGS('8', '8'))
					{
						/* 888 关机操作 */
						p_task_enqueue(TASK_SEND_DATA, "STOP OK", NULL);
						p_task_enqueue(TASK_SLEEP, NULL, NULL);
						/* 注:
							1.在不省电模式工作时,此操作将不起作用.
							2.在省电模式工作时,如果有开在自动上报GPS数据,要先发一条333ABCD的短信给机器,让机器停止自动上报，然后再发此操作短信，就可关机
							*/

					}
					else if (CHECK_FLGS('8', '2'))
					{
						/* 882 获取设备唯一码 */
						p_task_enqueue(TASK_SEND_ID, NULL, NULL);
					}
					break;
				case '9':
					if ('9' == s_common_pack->m_flg_2)
					{
						switch (s_common_pack->m_flg_3)
						{
						case '0':
							/* 990 获取设备物联网卡号 */
													//pTestSend("GetICCID\r\n");

							p_task_enqueue(TASK_SEND_ICCID, NULL, NULL);
							break;
						case '1':
							/* 991 空调GPS程序 */
							//TODO：AIR CONDITION OK
							break;
						case '2':
							/* 992 设置为普通GPS程序 */
							//TODO：COMMON OK
							break;
						case '3':
							/* 993 设置关掉定时报告功能 */
							g_ccfg_config.m_en_tim_rep = FALSE;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_tim_rep, sizeof(g_ccfg_config.m_en_tim_rep));
							p_task_enqueue(TASK_SEND_DATA, "CLOS TIME REPORT OK", NULL);
							break;
						case '4':
							/* 994 设置打开定时报告功能 */
							g_ccfg_config.m_en_tim_rep = TRUE;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_tim_rep, sizeof(g_ccfg_config.m_en_tim_rep));
							p_task_enqueue(TASK_SEND_DATA, "OPEN TIME REPORT OK", NULL);
							break;
						case '5':
							/* 995 设置开启第一个水银触发工作并关第二个水银工作 */
							g_ccfg_config.m_en_sw1 = TRUE;
							g_ccfg_config.m_en_sw2 = FALSE;
							switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
							p_task_enqueue(TASK_SEND_DATA, "1TOUCHON OK$TOUCH OFF OK", NULL);
					
							break;
						case '6':
							/* 996 设置开启第二个水银触发工作并关第一个水银工作 */
							g_ccfg_config.m_en_sw1 = FALSE;
							g_ccfg_config.m_en_sw2 = TRUE;
							switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
							p_task_enqueue(TASK_SEND_DATA, "2TOUCH ON OK$TOUCH OFF OK", NULL);

							break;
						case '7':
							/* 997 设置开启两个水银都允许触发工作 */
							g_ccfg_config.m_en_sw1 = TRUE;
							g_ccfg_config.m_en_sw2 = TRUE;
							switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
							p_task_enqueue(TASK_SEND_DATA, "1TOUCH ON  OK$TOUCH ON  OK", NULL);
							break;
						case '9':
							/* 999 查询设置操作 */
							p_task_enqueue(TASK_GET_SET_1, NULL, NULL);
							p_task_enqueue(TASK_GET_SET_2, NULL, NULL);
							break;
						default:
							break;
						}
					}
					break;
				default:
					break;
				}
			}

			/* 奇葩的包体 */
			/* 9980000 固定模式 */
			if (NULL != strstr(buf, "998000"))
			{
				/* 格式化SIM卡 */
				//TODO: 格式化SIM卡
				if (TRUE == g_run_paramter.m_flg_nb_net)
				{
					/* 清空任务队列的任务 */
					p_task_clear();
					p_task_enqueue(TASK_RE_FORMAT, "FORMAT DONE", NULL);

				}

			}

			/* 2xxx*ABCD,xxx= 001-999小时 */
			if ('2' == s_common_pack->m_flg_1
				&& 0 == strncmp(buf + 5, g_ccfg_config.m_dev_pwd, MAX_DEV_PASSWD_SIZE))
			{
				/* 定时报告设置 */
				tmp_val = atoi((const char*)buf + 1);
				if (tmp_val >= MIN_TIM_SLEEP && tmp_val <= MAX_TIM_SLEEP)
				{
					g_ccfg_config.m_tim_sleep = tmp_val;
					p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_tim_sleep, sizeof(g_ccfg_config.m_tim_sleep));

					/* 更新设备关机时间 */
					// g_run_paramter.m_tim_sleep = g_ccfg_config.m_tim_sleep * 2;

					p_task_enqueue(TASK_SEND_DATA, "OPEN OK", NULL);
				}
				else {
					/* 非法值 */

				}

			}
		}

	out:
		return ret_val;
	}

	/**
  * ***********************************************************************
  * @brief	监听NB的事件
  *
  *	@param  :
  *
  * @retval void:
  *
  * @attention	: none
  * ***********************************************************************
  */
	void p_proc_listen_event(void)
	{

		softuart_printf("P_proc_listen_event!\r\n");
		if (!g_buff.m_recv_index)
		{
			goto out;
		}

		/* 监听模块进入PSM模式 */
		if (NULL != strstr((const char*)g_buff.m_recv_buff, NB_INTO_PSM_STR))  // 如果模块接收到进入psm模式
		{
			/* NB 进入PSM模式 需要进行唤醒 */
		;
			p_task_enqueue(TASK_WAKE_NB, NULL, NULL);                        // 模块就发送 activer
			g_buff.m_recv_index = 0;
			goto out;
		}

	out:
		return;
	}

	/**
  * ***********************************************************************
  * @brief	构建数据类型的发送包
  *
  *	@param  data: 需要发送的数据
  *	@param  len: 数据长度
  *
  * @retval uint16_t: 构建的数据包的大小
  *
  * @attention	: none
  * ***********************************************************************
  */
	uint16_t p_proto_creat_data(uint8_t* data, uint8_t len)
	{
		uint16_t w_ptr = 0;
		if (NULL == data)
		{
			goto out;
		}

	//	clear_buff(TX_BUFF);

		g_buff.m_flg_send_lock = FALSE;  // 解锁
		g_buff.m_send_index = 0x00;
		memset(g_buff.m_send_buff, 0, sizeof(uint8_t) * MAX_SEND_BUFF_SIZE);


		/* 数据包 头 */
		w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_DATA);

		/* 构建需要发送的数据 */
		memcpy(g_buff.m_send_buff + g_buff.m_send_index, data, sizeof(uint8_t) * len);
		g_buff.m_send_index += len;

		/* 数据包 尾 */
		g_buff.m_send_buff[g_buff.m_send_index++] = '*';
		g_buff.m_send_buff[g_buff.m_send_index++] = '*';


	out:
		return w_ptr;
	}
	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




