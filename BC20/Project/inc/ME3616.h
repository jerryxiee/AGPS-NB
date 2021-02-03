
/**
  *************************************************************************
  * @file    ME2616.h
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

  /*************************     ���ظ���������     *************************/
#ifndef __ME3616_H__
#define __ME3616_H__

/*************************     ϵͳͷ�ļ�����     *************************/


/*************************     ˽��ͷ�ļ�����     *************************/

#include "common_cfg.h"
/*************************      ���������ռ�      *************************/

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        ȫ��ö��        *************************/

	/*************************       ȫ�ֺ궨��       *************************/
#define	NB_COAP_EN		1
#define EXPANSION		5
#define DEBUG_DATA_SIZE 100
/*************************     ȫ�ֽṹ�嶨��     *************************/
	typedef struct
	{
		uint8_t m_time[7+ EXPANSION];
		uint8_t m_date[10+ EXPANSION];      // ������ʱ����
		uint8_t m_lat[13+ EXPANSION];       // GPSγ��
		uint8_t m_lat_NS;
		uint8_t m_lng[13+ EXPANSION];       // GPS����
		uint8_t m_lng_EW;
		uint8_t m_speed[10];     // ����
		uint8_t m_direct[10];    // ����
	}st_gps_info;



/*************************      ȫ�ֱ�������      *************************/
	extern st_gps_info g_gps_info ;



/*************************        ��������        *************************/
extern uint8_t p_nblc_common_func(uint8_t send_string, char* at_cmd, char* match_cmd, uint16_t wait_ms, uint8_t(*proc_cb)(char*), u8 iWait_OK);
extern uint8_t p_nblc_report_debug_info(uint8_t send_string, char* at_cmd, char* match_cmd, uint16_t wait_ms, uint8_t(*proc_cb)(char*), u8 iWait_OK);
extern uint8_t p_init_BC20(void);
extern void p_init_BC20_GPIO(void);
extern uint8_t p_nblc_communic_test(void);
extern uint8_t p_init_AGPS(void);
extern uint8_t p_nblc_read_data(void);
extern void p_nblc_agps_proc(void);
extern u8 p_nblc_UDP_connect_net(void);
extern uint8_t p_dispose_gps_date(char* locat);
extern uint8_t p_nblc_proc_read_data(char* line);
extern uint8_t p_nblc_port_send_data(uint8_t* udp_data, uint16_t len);
extern uint8_t p_nblc_debug_send_data(uint8_t* udp_data, uint16_t len);
extern uint8_t p_nblc_get_csq(void);
extern uint8_t p_nblc_get_cereg(void);

extern u8 p_nblc_establish_network(void);
/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     ���ظ���������     *************************/
#endif



