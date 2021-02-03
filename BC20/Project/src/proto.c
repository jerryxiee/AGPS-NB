
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

  /*************************     ϵͳͷ�ļ�����     *************************/


/*************************     ˽��ͷ�ļ�����     *************************/
#include "proto.h"
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

#define CHECK_DEV_PWD(x)		(0 == strncmp((char const *)x, g_ccfg_config.m_dev_pwd, MAX_DEV_PASSWD_SIZE)) // ��֤����

#define CHECK_FLGS(x,y)			(x == s_common_pack->m_flg_2 && y == s_common_pack->m_flg_3)	              // �жϹ������������־λ

#define ASSERT_IP(x)			(x <= 0 || x >= 0xff)				                                          // IP��ַ�Ϸ��Լ��
#define ASSERT_PORT(x)			(x <= 0 || x >= 0xffff)			                                              // �˿ںϷ��Լ��

#define	RE_MUCH_CONFIG			MUCH CONFIG OK

#define RE_NOSIGN_CONFIG(x)		"NoSignal "_STR(x)"MinuteOFF OK"

#define RE_PASSBY_CONFIG(x)		"Bypass "_STR(x)"Hour OK"

#define RE_RESET(x)				"reset "_STR(x)"respond"

	/* NBģ�����յ����ݷ������� */
#define NB_RECV_FLG_STR			"+RECEIVE" 

#define NB_INTO_PSM_STR			"ENTER PSM"
	
	/*************************     ȫ�ֽṹ�嶨��     *************************/


/* Э������ṹ�� */
	typedef struct
	{
		/* ������ʶλ */
		uint8_t m_flg_1;
		uint8_t m_flg_2;
		uint8_t m_flg_3;

		/* ���� */
		uint8_t m_pwd[MAX_DEV_PASSWD_SIZE];
	}st_common_pack;
	/*************************      ȫ�ֱ�������      *************************/

	const char CODING[] = "SHIKEGPSCODING";

	/* ���������ķ��� */
	const char  b_udp_respond[] = {0xEF,0xA3,0xB5,0x3F,0x31,0x30,0x03,0};/*{ 0xff,0xDD,0x02,0x31,0x30,0x03, 0 };*/

	static st_common_pack* s_common_pack = NULL;
	uint8_t server_key[MAX_SERVER_KEY_SIZE] = { 0 };

	static uint8_t s_respond_buf[50] = { 0 };


	/*************************        ��������        *************************/

	/*************************        ����ʵ��        *************************/



		/* ���ɷ�������Կ */
	void p_proto_gen_server_key(void)
	{
		for (uint8_t i = 0; i < STRLEN(DEFAULT_DEV_IDEN_ID); i++)
		{
			uint8_t conv = g_ccfg_config.m_dev_iden_id[i];  // �豸ID����������  =�� cfg.c ����Ĭ������

			conv = ((conv << 4) & 0xF0) | (((conv >> 4) & 0x0F));
			g_run_paramter.m_dev_iden_id[i] = conv ^ CODING[i]; // ��λ������ɼ���ID

			/* ����server key */
			server_key[2 * i] = itohex((g_run_paramter.m_dev_iden_id[i] & 0xf0) >> 4);  // ��ID������Կ
			server_key[2 * i + 1] = itohex(g_run_paramter.m_dev_iden_id[i] & 0x0f);
			// sprintf((char*)server_key + (i*2), "%02X", g_run_paramter.m_dev_iden_id[i]);
		}
	}
	/*
		��ʮ��.��λ���ݺ�δ��λ���ݸ�ʽ
			1.��λ���ݸ�ʽ����
			* MNOPQRSTUVWXYZxxxxxxEFGH IJ11 * ����, E, γ��, N, �ٶ�, ����* ����, ʱ��, ���**
			2.δ��λ���ݸ�ʽ����
			% MNOPQRSTUVWXYZxxxxxxEFGH11 % Cell, , C, , Lac, L, 0.0, 000.000 % 000000, 000000.000 %
	*/
	/**
  * ***********************************************************************
  * @brief	�������ݰ�ͷ��
  *
  *	@param  p_buff: ������
  *	@param  pkg_type: ���ݰ�����
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
			/* GPS ��λ */
			p_buff[0] = '*';
		}
		else {
			p_buff[0] = '%';
		}
		++(*p_index);

		/* ���ܺ�����ID */
		memcpy(p_buff + 1, server_key, sizeof(char) * MAX_SERVER_KEY_SIZE);  // �����ܵ����IDд��������
		(*p_index) += MAX_SERVER_KEY_SIZE;

		/* �豸��� */                 // sprintf(*string,%s,x)  ��x���ַ�������ʽд�뵽string�ַ���
		(*p_index) += sprintf((char*)p_buff + (*p_index), "%s", g_ccfg_config.m_dev_guid);

		/* ��ѹ���ź� */
		// TODO: ��ѹ �ź�δע��
		(*p_index) += sprintf((char*)p_buff + (*p_index), "%u.%uV%02u",
			g_run_paramter.m_dev_voltage / 10, g_run_paramter.m_dev_voltage % 10
			, g_run_paramter.m_nb_stat.m_nbiot_csq);

		if (PKG_DATA == pkg_type)
		{
			/* ���ݰ� */
			(*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "&$*");
		}
		else if (PKG_HEARTBEAT == pkg_type)
		{
			/* �������� */
			if (TRUE == g_run_paramter.m_gps_ok) {
				/* GPS ��λ */
				if (TRUE == g_run_paramter.m_flg_alarm)
				{	/* �о��� */
					(*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "0L*");
				}
				else { (*p_index) += sprintf((char*)p_buff + (*p_index), "%s", "0H*"); }

			}
			else {
				/* ��վ��λ */
				if (TRUE == g_run_paramter.m_flg_alarm)
				{	/* �о��� */
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


		/* ���GPS����״̬����л�վ��λ */
		if (g_run_paramter.m_gps_ok == FALSE)
		{
			/* ��ȡ��վ��λ���� */
		}

		/* �建�� */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		/* �������ݰ���ͷ�� */
		w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_HEARTBEAT);
 
		/* ����GPS״̬���صĶ�λ��Ϣ */
		if (g_run_paramter.m_gps_ok == TRUE)
		{
			/* GPS��λ */
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%s,%c,%s,%c,%s,%s*%s,%s.%03u**",
				g_gps_info.m_lng, g_gps_info.m_lng_EW,     // ��������
				g_gps_info.m_lat, g_gps_info.m_lat_NS,     // γ������
				g_gps_info.m_speed, g_gps_info.m_direct,   // �ٶ��뷽��
				g_gps_info.m_date, g_gps_info.m_time,
				++g_run_paramter.m_pkg_num % 1000);     // ������ʱ��

		//	memset(&g_gps_info, 0x00, sizeof(g_gps_info));
		}
		else
		{
			/* ��վ��λ */
			// w_ptr += sprintf(g_c_m26l_sbuf + w_ptr, "%%");
#if 0
			w_ptr += dec2dms((char*)g_buff.m_send_buff + w_ptr, (char*)g_gps_info.m_lng);        // ����
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, ",E,");                   // ���ȱ��
			w_ptr += dec2dms((char*)g_buff.m_send_buff + w_ptr, (char*)g_gps_info.m_lat);        // γ��
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, ",N,000.0,000.0%%000000,000000.%03u%%%%",
				++g_run_paramter.m_pkg_num % 1000);    //γ�ȱ�ż�����
#endif
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr,
		 "00000.00000,E,0000.00000,N,000.0,000.0%%000000,000000.%03u%%%%",
				++g_run_paramter.m_pkg_num % 1000);    //γ�ȱ�ż�����
		}
		
		/* ���ͻ������ĳ��� */
		g_buff.m_send_index = w_ptr;

		return w_ptr;
	}
	/**
	  * ***********************************************************************
	  * @brief	Э���������
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
	  * @brief	���������
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

		// �����Ƿ��������յ�������
		//  869662032467935: *376B DA2826645360600C273ADD74 FFFFFF 0.0V34 00*00000.0000,E,0000.0000,N,0.00,0.00*000000,000000**
		softuart_printf("creat queryid\r\n");
		/* �建�� */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		g_buff.m_send_buff[0] = '*';

		/**
		* ������Ĭ������F1������375B
		* �°� F2 ����376B ,Ψһ��ʹ��MC20��IMEI��
		*/
		g_buff.m_send_buff[w_ptr++] = '3';
		g_buff.m_send_buff[w_ptr++] = '7';
		g_buff.m_send_buff[w_ptr++] = '6';
		g_buff.m_send_buff[w_ptr++] = 'B';

		for (int i = 0; i < MAX_IOT_ID_SIZE; i++)
		{
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%02X", g_run_paramter.m_iot_id[i]);
		}

		/* �豸��� */
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%s", STR(DEFAULT_DEV_SER_GUID));

		/* ��ѹ���ź� */
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u.%uV%02u",
			g_run_paramter.m_dev_voltage / 10, g_run_paramter.m_dev_voltage % 10
			, g_run_paramter.m_nb_stat.m_nbiot_csq);

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "00*00000.0000,E,0000.0000,N,0.00,0.00*000000,000000**");

		g_buff.m_send_index = w_ptr;

	}

	/**
	  * ***********************************************************************
	  * @brief	IMEI���ȡ����ܣ�����У���루δע��ʱʹ�ã�
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
* @brief	�ظ���ѯ���� 1
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

		/* �������� */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		/* ��һ������ */
		{
			w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_DATA);

			/************** 1-8 �绰����� ****************/
			while (line++ < 8)
			{
				w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
					line, g_ccfg_config.m_tel_list[line - 1]);
			}

			/************** 9 �ϱ�GPSʱ���� *************/
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
				line++, g_ccfg_config.m_tim_gps_rep);


			/************** 10 �ػ���ʱʱ�� ***************/
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
				line++, g_ccfg_config.m_tim_pwr_delay);

			/************** 11 ��ʱ����ʱ���� ***********/
			w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
				line++, g_ccfg_config.m_tim_sleep);

			// p_m26l_send_udp_data(0, g_buff.m_send_buff);
			p_nblc_port_send_data(g_buff.m_send_buff, w_ptr);
		}

	}

	/**
	  * ***********************************************************************
	  * @brief	�ظ����ö��� 2
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

		/* �������� */
		memset(g_buff.m_send_buff, 0x00, sizeof(g_buff.m_send_buff));

		w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_DATA);

		/* ��Ϊ���ݰ���ͷ��������ͬ������ֻҪ��д��ַǰ�ƾͿ��� */

		/************** 12 �豸��� *******************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_dev_guid);

		/************** 13 ʹ�ܶ�ʱ�ϱ� ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_tim_rep ? "55" : "44");

		/************** 14 �豸���� *******************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_dev_pwd);

		/************** 15 ʹ�ܵ�ѹ��� ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_vol_check ? "55" : "44");

		/************** 16 ˮ�����ش�����ֵ ***********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_cnt_sw_trigger);

		/************** 17 �յ�ģʽ/��ͨģʽ **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"44\"$", line++);

		/************** 18 ʹ�ܶ�ʱ�ϱ� ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_tim_rep ? "55" : "44");

		/************** 19 ʹ��ˮ������1 **************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_sw1 ? "55" : "44");

		/************** 20 ʹ��ˮ������2 **************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_en_sw2 ? "55" : "44");

		/************** 21 ��������� *****************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"0\"$", line++);

		/************** 22 �豸IP��ַ���˿ں� *********/
#if 0
		w_ptr += sprintf((char*)g_c_m26l_sbuf + w_ptr, "%u\"%u.%u.%u.%u:%u\"$",
			line++, g_ccfg_config.b_ser_ip[0], g_ccfg_config.b_ser_ip[1],
			g_ccfg_config.b_ser_ip[2], g_ccfg_config.b_ser_ip[3],
			g_ccfg_config.s_ser_port);
#endif
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"", line++);

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%s:%s", SERVER_IP(IP_1, IP_2, IP_3, IP_4), SERVER_PORT(PORT));

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "\"$");

		/************** 23 GPRS��������� *************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_heartbeat);

		/************** 24 ����ʱGPRS����ʱ�� *********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_alm_continue);

		/************** 25 �豸���ID *****************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"", line++);

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "05951234123456");

		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "\"$");

		/************** 26 �豸����ģʽ ***************/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_en_gprs);

		/************** 27 GPRS����ʱ���� ***********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_alm_interval / 60);

		/************** 28 1-5��绰�ϱ���־ **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%c\"$",
			line++, g_ccfg_config.m_en_1to5_rep ? '1' : '0');

		/************** 29 APN�ӿ� **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%s\"$",
			line++, g_ccfg_config.m_dev_APN);
		/************** 30 �������ź���ʱ���� **********/
		w_ptr += sprintf((char*)g_buff.m_send_buff + w_ptr, "%u\"%u\"$",
			line++, g_ccfg_config.m_tim_no_signal_delay);

		// p_m26l_send_udp_data(0, g_buff.m_send_buff);
		p_nblc_port_send_data(g_buff.m_send_buff, w_ptr);
	}

	/**
  * ***********************************************************************
  * @brief	Э���������
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
		/* udp��������λ�� */
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

		/* UDP ������ */
		if (NULL != (udp_rep = strstr(buf, b_udp_respond)))
		{
			softuart_printf("recv the udp respond!\r\n");
			//GPIO_ResetBits(LED_GPS_PORT, LED_GPS_PIN);
			/* ���յ��������� */
			g_run_paramter.m_heartbeat_respond = TURE;
			g_run_paramter.m_udp_transfer.m_flg_sending = FALSE;
			g_run_paramter.m_udp_transfer.m_flg_retry = TRUE;
			g_run_paramter.m_udp_transfer.m_cnt_send_fail = 0;
			++g_run_paramter.m_nb_stat.m_nbiot_recv_cnt;

			//GPIO_ResetBits(LED_REGISTER_PORT, LED_REGISTER_PIN);
			//191121 ˢ��ʱ��
			

			/* �жϷ�������β���Ƿ��и����������� */
			if (len <= sizeof(b_udp_respond))
			{
			//	softuart_printf("have not nother data!\r\n");
				goto out;
			}

			/* ������ǰ�棬�����ں��棬���������������������� */
			if (buf == udp_rep)
			{
			//	softuart_printf("have nother data!\r\n");
				buf += sizeof(b_udp_respond);
			}
		}
#endif

		s_common_pack = (st_common_pack*)buf;

		/* �ж��Ƿ��ǹ������ݰ� */

		softuart_printf("judge is real data\r\n");
		delay_10ms(1);
		if ('*' == s_common_pack->m_flg_1)
		{
			/* ������� */
			if (0 == strncmp(buf, "*#*#", 4))
			{
				/* ������ */

				/* ����*#*# */
				buf += 4;

				/* ��֤���� */
				if (!CHECK_DEV_PWD(buf))
				{
					/* ������� */
					goto out;
				}

				/* �������� */
				buf += 5;

				// much_config_flg = TRUE;
				uint8_t _much_config_num = 0;
				// uint8_t* tmp = NULL;
				// buf = strtok(buf, "*");
				do {
					/* ��ȡ�����̵ı�� */
					_much_config_num = atoi((const char*)buf);  
					softuart_printf("_much_config_num is %d",(int) _much_config_num);
					switch (_much_config_num)
					{
					case 1:
						/* �ֻ����� */
						break;
					case 2:
						/* �ֻ����� */
						break;
					case 3:
						/* �ֻ����� */
						break;
					case 4:
						/* �ֻ����� */
						break;
					case 5:
						/* �ֻ����� */
						break;
					case 6:
						/* �ֻ����� */
						break;
					case 7:
						/* �ֻ����� */
						break;
					case 8:
						/* �ֻ����� */
						break;
					case 9:
						/* xx=01-60����,(ÿ��һ��ʱ���ϱ�GPS����) ��ʱ�ϱ�����ʱ�� */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_GPS_REP && tmp_val <= MAX_TIM_GPS_REP)
						{
							g_ccfg_config.m_tim_gps_rep = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_gps_rep,
								sizeof(g_ccfg_config.m_tim_gps_rep));
							g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep;
						}
						else {
							/* �Ƿ�ֵ */
						}

						break;
					case 10:
						/* xx= 05-60����(��ʱһ��ʱ��ػ�) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_PWR_DELAY && tmp_val <= MAX_TIM_PWR_DELAY)
						{
							g_ccfg_config.m_tim_pwr_delay = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_pwr_delay,
								sizeof(g_ccfg_config.m_tim_pwr_delay));
							g_run_paramter.m_tim_pwr_off_delay = g_ccfg_config.m_tim_pwr_delay * 60;
						}
						else {
							/* �Ƿ�ֵ */

						}

						break;
					case 11:
						/* xx= 001-999Сʱ(��ʱ����) �豸��˯��ʱ�� */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_SLEEP && tmp_val <= MAX_TIM_SLEEP)
						{
							g_ccfg_config.m_tim_sleep = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_sleep,
								sizeof(g_ccfg_config.m_tim_sleep));
							 g_run_paramter.m_tim_sleep = g_ccfg_config.m_tim_sleep;
						}
						else {
							/* �Ƿ�ֵ */

						}

						break;
					case 12:
						softuart_printf("case 12");
						/* xx= 6λ����������� */
						if (TRUE == g_run_paramter.m_flg_en_unreg && TRUE == is_all_num((uint8_t*)buf + 3, 6))
						{
							memcpy(g_ccfg_config.m_dev_guid, buf + 3, 6);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_guid,
								sizeof(g_ccfg_config.m_dev_guid));
							softuart_printf("save the dev_guid");
							g_run_paramter.m_flg_en_unreg = FALSE;
							/* ��ע�Ѿ���ע�� */
						//	p_task_enqueue(TASK_REGISTER, NULL, NULL);
						}
						else {
							/* �Ƿ�ֵ */
						}

						break;
					case 13:
						/* xx=55�� 44,( 55����GPS�����Զ��ϱ�,44�ر�GPS�����Զ��ϱ�) */
						if ('5' == buf[3] && '5' == buf[4])
						{
							/* ʹ���ϱ�GPS���� */
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
						/* �����ڴ˲��ɱ�� */
						break;
					case 15:
						/* xx=55�� 44 (�������ⲿ�����·�͵�ص�ѹ55Ϊ����44Ϊ��) */
						if ('5' == buf[3] && '5' == buf[4])
						{
							/* ʹ���ϱ�GPS���� */
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
						/* xx= 01-09(����ˮ��������������) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= 1 && tmp_val <= 9)
						{
							g_ccfg_config.m_cnt_sw_trigger = tmp_val;
							
							// Ӧ�ͻ�Ҫ�󣬶ϵ粻���档 200622
							/*p_ccfg_write_config((uint8_t*)g_ccfg_config.m_cnt_sw_trigger,
								sizeof(g_ccfg_config.m_cnt_sw_trigger));*/
						}
						else {
							/* �Ƿ�ֵ */

						}
						break;
					case 17:
						/* xx=55�� 44 ( 44Ϊ��ͨģʽ,55Ϊ�յ�ģʽ) */
						break;
					case 18:
						/* xx=55�� 44(55Ϊ������ʱ����,44Ϊ�رն�ʱ����) */
						if ('5' == buf[3] && '5' == buf[4])
						{
							/* ʹ���ϱ�GPS���� */
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
						/* xx=55�� 44��55������һ��ˮ������,44Ϊ�رյ�һ��ˮ�������� */
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
						/* xx=55�� 44��55�����ڶ���ˮ������, 44Ϊ�رյڶ���ˮ�������� */
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
						/* xx=0-4(���������),(0= CMNET,1=CMWAP,2=uninet,3=uniwap) */
						break;
					case 22:
						/* xx=IP��ַ�Ͷ˿ں�,��220.162.13.122:6868 */
#if 0
						sscanf((const char*)buf + 3, "%d.%d.%d.%d:%d",
							g_run_paramter.m_ser_ip, g_run_paramter.m_ser_ip + 1,
							g_run_paramter.m_ser_ip + 2, g_run_paramter.m_ser_ip + 3,
							&g_run_paramter.m_ser_port);

						p_task_enqueue(TASK_SERVER_CFG, NULL, NULL);
#endif
						break;
					case 23:
						/* xx=30-99����(GPRS����ʱ��) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_HEARTBEAT && tmp_val <= MAX_TIM_HEARTBEAT)
						{
							g_ccfg_config.m_tim_heartbeat = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_heartbeat, sizeof(g_ccfg_config.m_tim_heartbeat));
						}
						else {
							/* �Ƿ�ֵ */

						}
						break;
					case 24:
						/* xx=1-9����(ÿ����һ��GPRS��������ʱ��) */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_ALM_CONTINUE && tmp_val <= MAX_TIM_ALM_CONTINUE)
						{
							g_ccfg_config.m_tim_alm_continue = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_alm_continue,
								sizeof(g_ccfg_config.m_tim_alm_continue));

							g_run_paramter.m_tim_alm_continue += g_ccfg_config.m_tim_alm_continue * 60;

						}
						else {
							/* �Ƿ�ֵ */
						}

						break;
					case 25:
						/* xx=14λ�������ʶ���� */
						if (TRUE == is_all_num((uint8_t*)buf + 3, 14))
						{
							memcpy(g_ccfg_config.m_dev_iden_id, buf + 3, 14);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_iden_id,
								sizeof(g_ccfg_config.m_dev_iden_id));

							/* ���ɷ�������key ���ڷ��������� */
							p_proto_gen_server_key();

							/* ע��ɹ�������Ҫ���������� */
						//	p_task_enqueue(TASK_HEARTBEAT, NULL, NULL);
							p_task_enqueue(TASK_SEND_ICCID, NULL, NULL);
							p_task_enqueue(TASK_SEND_FWVER, NULL, NULL);
						}
						else {
							/* �Ƿ�ֵ */
						}

						break;
					case 26:
						/* xx=0��1(���繤��ģʽ,0=GSM,1=GPRS) */
						break;
					case 27:
						/* XX=05-60��,GPRSģʽ��5�����ڱ�����GPS����ʱ�� */
						tmp_val = atoi((const char*)buf + 3);
						if (tmp_val >= MIN_TIM_ALM_INTERVAL && tmp_val <= MAX_TIM_ALM_INTERVAL)
						{
							g_ccfg_config.m_tim_alm_interval = tmp_val;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_alm_interval,
								sizeof(g_ccfg_config.m_tim_alm_interval));
						}
						break;
					case 28:
						/* ��1������3��,�Ƿ�����ϱ�״̬ */
						break;
					default:
						break;
					}

					/* ����һ�� '*' */
					// buf = strtok(NULL, "*");

					/* �ж϶������Ƕ����� */
					if (strstr(buf + 3, "**"))
					{
						p_task_enqueue(TASK_SEND_DATA, STR(RE_MUCH_CONFIG), NULL);
						goto out;
					}

					buf += strlen(buf) + 1;
				} while (*buf);

			}
			else if ('*' == s_common_pack->m_flg_2) {
				/* �޸����� */
				memcpy(g_ccfg_config.m_dev_pwd, buf + 7, 4);
				p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_pwd, sizeof(g_ccfg_config.m_dev_pwd));
				p_task_enqueue(TASK_SEND_DATA, "CODE OK", NULL);
			}
			else {

				/* ���ñ����绰�����״̬�绰���� */
				uint8_t* str1 = NULL, * p_pwd = NULL;
				uint8_t group = 0;
				str1 = (uint8_t*)strtok(buf, "*");
				p_pwd = (uint8_t*)strtok(NULL, "*");
				/* �ж������Ƿ���ȷ */
				if (!CHECK_DEV_PWD(p_pwd))
				{
					/* ���벻��ȷ */
					goto out;
				}
				/* ��ȡ��� */
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
					/* ���ñ����绰�����״̬�绰���� */
					//TODO���ظ�����:	�绰����		CONFIG  OK
					p_task_enqueue(TASK_SEND_DATA, " CONFIG OK", NULL);
					break;
				case 9:
					/* ����IP��ַ�Ͷ˿ں� */
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
					/* ����GPRS������ʱ�� */
					// *xx*0000*10**,xx=30-99��,Ĭ��60��
					tmp_val = atoi((const char*)str1);
					if (tmp_val >= MIN_TIM_HEARTBEAT && tmp_val <= MAX_TIM_HEARTBEAT)
					{
						g_ccfg_config.m_tim_heartbeat = tmp_val;
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_heartbeat, sizeof(g_ccfg_config.m_tim_heartbeat));
						p_task_enqueue(TASK_SEND_DATA, "10CONFIG OK", NULL);
					}
					else {
						/* �Ƿ�ֵ */
					}

					break;
				case 11:
					/* ��������� */
					// *x*0000*11**, x=14λ�����
					if (strlen((char*)str1) == 14) {
						memcpy(g_ccfg_config.m_dev_iden_id, str1, 14);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_iden_id, sizeof(g_ccfg_config.m_dev_iden_id));
						p_task_enqueue(TASK_SEND_DATA, "11CONFIG OK", NULL);
					}
					else {
						/* �Ƿ�ֵ */
					}
					break;
				case 12:
					/* �豸��� */
					// *x*0000*12**, x=6λ����
					if (strlen((char*)str1) == 6 && TRUE == is_all_num(str1, 6)) {
						memcpy(g_ccfg_config.m_dev_guid, str1, 6);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_dev_guid, sizeof(g_ccfg_config.m_dev_guid));

						p_task_enqueue(TASK_SEND_DATA, "12CONFIG OK", NULL);
						p_task_enqueue(TASK_SWITCHGPS, NULL, NULL);  // LZY��ӣ���־�豸��ע��
					}
					else {
						/* �Ƿ�ֵ */
					}
					break;
				case 13:
					/* ������GPRSģʽ��5�����ڱ�����GPS����ʱ��(5-60��) */
					// *xx*0000*13**,xx=05-60��,Ĭ��10��
					tmp_val = atoi((const char*)str1);

					if (tmp_val >= MIN_TIM_ALM_INTERVAL && tmp_val <= MAX_TIM_ALM_INTERVAL) {
						g_ccfg_config.m_tim_alm_interval = atoi((const char*)str1);
						p_ccfg_write_config((uint8_t*)g_ccfg_config.m_tim_alm_interval, sizeof(g_ccfg_config.m_tim_alm_interval));
						p_task_enqueue(TASK_SEND_DATA, "13CONFIG OK", NULL);
					}
					else {
						/* �Ƿ�ֵ */
					}
					break;
				case 14:
					/* ����1����3����ű���״̬�Ƿ�ͨ�ϱ� */
					//  *xx*0000*13**,xx=05-60��,Ĭ��10��
					p_task_enqueue(TASK_SEND_DATA, "14CONFIG OK", NULL);
					break;
				case 29:
					/* ��ȡAPN���� */
					// *APN����*ABCD*29**
					// g_ccfg_config.m_dev_APN;
					break;
				case 30:
					/*�������ź���ʱ�ػ���ʱ��*/
					// *05 * 0000 * 30 * *  ��λ������
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
						/* �Ƿ�ֵ */
					}

					break;
				case 31:
					/*������·״̬������ˮ�����ص�ʱ��*/
					// *05*0000*31**  ��λ��Сʱ
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
						/* �Ƿ�ֵ */

					}

					break;
				default:
					break;
				}
			}
		}
		else
		{
			/* �������ֿ�ͷ���ݰ� */

			/* ��ֹĳ�鱨���绰�ذ� */
			if (0 == strncmp(buf, "0000", 4))
			{
				//TODO���ظ�����:PAUSE ALARM
			}

			if (CHECK_DEV_PWD(s_common_pack->m_pwd))
			{
				switch (s_common_pack->m_flg_1)
				{
				case '1':
					if (CHECK_FLGS('4', '7'))
					{
						/* 147 ɾ��4-7��ı����绰 */
						p_ccfg_del_tel(3);
						p_ccfg_del_tel(4);
						p_ccfg_del_tel(5);
						p_ccfg_del_tel(6);
						p_task_enqueue(TASK_SEND_DATA, "DEL47 OK", NULL);
					}
					else if (CHECK_FLGS('2', '3'))
					{
						/* 123 �رյ�ѹ��� */
						g_ccfg_config.m_en_vol_check = FALSE;
						p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_vol_check, sizeof(g_ccfg_config.m_en_vol_check));
						p_task_enqueue(TASK_SEND_DATA, "NOCHEK OK", NULL);
					}
					else if (CHECK_FLGS('2', '4'))
					{
						/* 124 ��������� */
						g_ccfg_config.m_en_vol_check = TRUE;
						p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_vol_check, sizeof(g_ccfg_config.m_en_vol_check));
						p_task_enqueue(TASK_SEND_DATA, "CHEK OK", NULL);
					}
					break;
				case '3':
					if (CHECK_FLGS('3', '3'))
					{
						/* 333 ÿ��һ��ʱ���Զ��ϱ�GPS���ݹر� */
						g_ccfg_config.m_en_gps_rep = FALSE;
						p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_gps_rep, sizeof(g_ccfg_config.m_en_gps_rep));
						p_task_enqueue(TASK_SWITCHGPS, NULL, NULL);
						p_task_enqueue(TASK_SEND_DATA, "OFF OK", NULL);
					}
					else if ('0' == s_common_pack->m_flg_2)
					{
						/* 30x ����һ�α���������GPRS����ʱ�� x=1-9���� */
						tmp_val = s_common_pack->m_flg_3 - '0';
						if (MIN_TIM_ALM_CONTINUE <= tmp_val && tmp_val <= MAX_TIM_ALM_CONTINUE)
						{
							g_ccfg_config.m_tim_alm_continue = tmp_val;
						//	p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_tim_alm_continue, sizeof(g_ccfg_config.m_tim_alm_continue));
							p_task_enqueue(TASK_SEND_DATA, "CONTINUE OK", NULL);

							g_run_paramter.m_tim_alm_continue = g_ccfg_config.m_tim_alm_continue * 60;
						}
						else {
							/* �Ƿ�ֵ */
						}

					}
					break;
				case '4':
					if ('*' == s_common_pack->m_pwd[4])
					{
						/* GPS��ʱ�ϱ�ָ���绰�� */
						//TODO:�ظ�����:TIME OK
					}
					else
					{
						/* ÿ��һ��ʱ���Զ��ϱ�GPS��������(��������ʱ�ػ�����ʧЧ) */
						tmp_val = (s_common_pack->m_flg_2 - '0') * 10 + (s_common_pack->m_flg_3 - '0');
						if (tmp_val >= MIN_TIM_GPS_REP && tmp_val <= MAX_TIM_GPS_REP)
						{
							g_ccfg_config.m_tim_gps_rep = tmp_val;
							p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_tim_gps_rep, sizeof(g_ccfg_config.m_tim_gps_rep));
							g_run_paramter.m_tim_gps_rep = g_ccfg_config.m_tim_gps_rep * 60;
							p_task_enqueue(TASK_SEND_DATA, "TIME OK", NULL);

							/* ��GPSģ��ر�ʱ����GPSģ�� */
							if (FALSE == g_ccfg_config.m_en_gps_rep)
							{
								g_ccfg_config.m_en_gps_rep = TRUE;
								p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_en_gps_rep, sizeof(g_ccfg_config.m_en_gps_rep));
								p_task_enqueue(TASK_SWITCHGPS, NULL, NULL);
							}
						}
						else {
							/* �Ƿ�ֵ */
						}

					}
					break;
				case '5':
					/* 5xx ����ʱһ��ʱ��ػ����� xx=05-60���� */
					tmp_val = (s_common_pack->m_flg_2 - '0') * 10 + (s_common_pack->m_flg_3 - '0');
					if (tmp_val >= MIN_TIM_PWR_DELAY && tmp_val <= MAX_TIM_PWR_DELAY)
					{
						g_ccfg_config.m_tim_pwr_delay = tmp_val;
						g_run_paramter.m_tim_pwr_off_delay = g_ccfg_config.m_tim_pwr_delay * 60;
						/* ֱ�Ӹ������ߵ���ʱ�� */
						g_run_paramter.m_tim_gps_rep = g_run_paramter.m_tim_pwr_off_delay ;
					
					
						p_task_enqueue(TASK_SEND_DATA, "CLOS OK", NULL);
					}
					else {
						/* �Ƿ�ֵ ��ֵ���� */
					}

					break;
				case '6':
					if (CHECK_FLGS('6', '6'))
					{
						/* ��ѯGPS���� */
						//TODO:�ظ�GPS����.
					}
					else {
						/* ���δ��������������� */
						tmp_val = (s_common_pack->m_flg_2 - '0') * 10 + (s_common_pack->m_flg_3 - '0');
						if (MIN_CNT_SW_TRIGGER <= tmp_val && 9 >= MAX_CNT_SW_TRIGGER)
						{
							g_ccfg_config.m_cnt_sw_trigger = tmp_val;
							// Ӧ�ͻ�Ҫ�󣬱����������粻���� 200622
							//ccfg_write_config((uint8_t*)&g_ccfg_config.m_cnt_sw_trigger, sizeof(g_ccfg_config.m_cnt_sw_trigger));
							p_task_enqueue(TASK_SEND_DATA, "SPRING OK", NULL);
						}
						else {
							/* �Ƿ�ֵ */
						}

					}
					break;
				case '7':
					if (CHECK_FLGS('7', '7'))
					{
						/* ����ʱ�ػ�(����ʱ��Լ2Сʱ��ػ�) */
					//	g_run_paramter.m_tim_pwr_off_delay = 120;
					//	g_run_paramter.m_tim_gps_rep = 120 * 60;
						p_task_enqueue(TASK_SEND_DATA, "DEFER OK", NULL);
					}
					else if ('7' == s_common_pack->m_flg_2)
					{
						/* 77x ����GPRS��������ƴ���(0= CMNET,1=CMWAP,2=uninet,3=uniwap) */
						if (s_common_pack->m_flg_3 <= 3)
						{
							memcpy(g_ccfg_config.m_dev_APN, "CTNB", 4);
							p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_dev_APN, sizeof(g_ccfg_config.m_dev_APN));
							// s_common_pack->m_flg_3;
						}
						else {
							/* �Ƿ�ֵ */
						}
					}
					else if ('6' == s_common_pack->m_flg_2)
					{
						/* 76x ѡ��GPRS��GSM����ģʽ(1=GPRS,0=GSM) */
						//TODO��	�ظ�����:MODE GSM		�ظ����� : MODE GPRS

					}
					break;
				case '8':
					if (CHECK_FLGS('8', '1'))
					{
						/* 881 ��ѯGPS�汾�� */
						p_task_enqueue(TASK_SEND_FWVER, NULL, NULL);

					}
					else if (CHECK_FLGS('8', '8'))
					{
						/* 888 �ػ����� */
						p_task_enqueue(TASK_SEND_DATA, "STOP OK", NULL);
						p_task_enqueue(TASK_SLEEP, NULL, NULL);
						/* ע:
							1.�ڲ�ʡ��ģʽ����ʱ,�˲�������������.
							2.��ʡ��ģʽ����ʱ,����п����Զ��ϱ�GPS����,Ҫ�ȷ�һ��333ABCD�Ķ��Ÿ�����,�û���ֹͣ�Զ��ϱ���Ȼ���ٷ��˲������ţ��Ϳɹػ�
							*/

					}
					else if (CHECK_FLGS('8', '2'))
					{
						/* 882 ��ȡ�豸Ψһ�� */
						p_task_enqueue(TASK_SEND_ID, NULL, NULL);
					}
					break;
				case '9':
					if ('9' == s_common_pack->m_flg_2)
					{
						switch (s_common_pack->m_flg_3)
						{
						case '0':
							/* 990 ��ȡ�豸���������� */
													//pTestSend("GetICCID\r\n");

							p_task_enqueue(TASK_SEND_ICCID, NULL, NULL);
							break;
						case '1':
							/* 991 �յ�GPS���� */
							//TODO��AIR CONDITION OK
							break;
						case '2':
							/* 992 ����Ϊ��ͨGPS���� */
							//TODO��COMMON OK
							break;
						case '3':
							/* 993 ���ùص���ʱ���湦�� */
							g_ccfg_config.m_en_tim_rep = FALSE;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_tim_rep, sizeof(g_ccfg_config.m_en_tim_rep));
							p_task_enqueue(TASK_SEND_DATA, "CLOS TIME REPORT OK", NULL);
							break;
						case '4':
							/* 994 ���ô򿪶�ʱ���湦�� */
							g_ccfg_config.m_en_tim_rep = TRUE;
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_tim_rep, sizeof(g_ccfg_config.m_en_tim_rep));
							p_task_enqueue(TASK_SEND_DATA, "OPEN TIME REPORT OK", NULL);
							break;
						case '5':
							/* 995 ���ÿ�����һ��ˮ�������������صڶ���ˮ������ */
							g_ccfg_config.m_en_sw1 = TRUE;
							g_ccfg_config.m_en_sw2 = FALSE;
							switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
							p_task_enqueue(TASK_SEND_DATA, "1TOUCHON OK$TOUCH OFF OK", NULL);
					
							break;
						case '6':
							/* 996 ���ÿ����ڶ���ˮ�������������ص�һ��ˮ������ */
							g_ccfg_config.m_en_sw1 = FALSE;
							g_ccfg_config.m_en_sw2 = TRUE;
							switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
							p_task_enqueue(TASK_SEND_DATA, "2TOUCH ON OK$TOUCH OFF OK", NULL);

							break;
						case '7':
							/* 997 ���ÿ�������ˮ�������������� */
							g_ccfg_config.m_en_sw1 = TRUE;
							g_ccfg_config.m_en_sw2 = TRUE;
							switch_ctrl(g_ccfg_config.m_en_sw1, g_ccfg_config.m_en_sw2);
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw1, sizeof(g_ccfg_config.m_en_sw1));
							p_ccfg_write_config((uint8_t*)g_ccfg_config.m_en_sw2, sizeof(g_ccfg_config.m_en_sw2));
							p_task_enqueue(TASK_SEND_DATA, "1TOUCH ON  OK$TOUCH ON  OK", NULL);
							break;
						case '9':
							/* 999 ��ѯ���ò��� */
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

			/* ����İ��� */
			/* 9980000 �̶�ģʽ */
			if (NULL != strstr(buf, "998000"))
			{
				/* ��ʽ��SIM�� */
				//TODO: ��ʽ��SIM��
				if (TRUE == g_run_paramter.m_flg_nb_net)
				{
					/* ���������е����� */
					p_task_clear();
					p_task_enqueue(TASK_RE_FORMAT, "FORMAT DONE", NULL);

				}

			}

			/* 2xxx*ABCD,xxx= 001-999Сʱ */
			if ('2' == s_common_pack->m_flg_1
				&& 0 == strncmp(buf + 5, g_ccfg_config.m_dev_pwd, MAX_DEV_PASSWD_SIZE))
			{
				/* ��ʱ�������� */
				tmp_val = atoi((const char*)buf + 1);
				if (tmp_val >= MIN_TIM_SLEEP && tmp_val <= MAX_TIM_SLEEP)
				{
					g_ccfg_config.m_tim_sleep = tmp_val;
					p_ccfg_write_config((uint8_t*)&g_ccfg_config.m_tim_sleep, sizeof(g_ccfg_config.m_tim_sleep));

					/* �����豸�ػ�ʱ�� */
					// g_run_paramter.m_tim_sleep = g_ccfg_config.m_tim_sleep * 2;

					p_task_enqueue(TASK_SEND_DATA, "OPEN OK", NULL);
				}
				else {
					/* �Ƿ�ֵ */

				}

			}
		}

	out:
		return ret_val;
	}

	/**
  * ***********************************************************************
  * @brief	����NB���¼�
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

		/* ����ģ�����PSMģʽ */
		if (NULL != strstr((const char*)g_buff.m_recv_buff, NB_INTO_PSM_STR))  // ���ģ����յ�����psmģʽ
		{
			/* NB ����PSMģʽ ��Ҫ���л��� */
		;
			p_task_enqueue(TASK_WAKE_NB, NULL, NULL);                        // ģ��ͷ��� activer
			g_buff.m_recv_index = 0;
			goto out;
		}

	out:
		return;
	}

	/**
  * ***********************************************************************
  * @brief	�����������͵ķ��Ͱ�
  *
  *	@param  data: ��Ҫ���͵�����
  *	@param  len: ���ݳ���
  *
  * @retval uint16_t: ���������ݰ��Ĵ�С
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

		g_buff.m_flg_send_lock = FALSE;  // ����
		g_buff.m_send_index = 0x00;
		memset(g_buff.m_send_buff, 0, sizeof(uint8_t) * MAX_SEND_BUFF_SIZE);


		/* ���ݰ� ͷ */
		w_ptr = p_proto_creat_pkg_head(g_buff.m_send_buff, &g_buff.m_send_index, PKG_DATA);

		/* ������Ҫ���͵����� */
		memcpy(g_buff.m_send_buff + g_buff.m_send_index, data, sizeof(uint8_t) * len);
		g_buff.m_send_index += len;

		/* ���ݰ� β */
		g_buff.m_send_buff[g_buff.m_send_index++] = '*';
		g_buff.m_send_buff[g_buff.m_send_index++] = '*';


	out:
		return w_ptr;
	}
	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




