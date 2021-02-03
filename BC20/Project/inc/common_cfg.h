 
/**
  *************************************************************************
  * @file    common_cfg.h
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

  /*************************     ���ظ���������     *************************/
#ifndef __COMMON_CFG_H__
#define __COMMON_CFG_H__

/*************************     ϵͳͷ�ļ�����     *************************/
#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm8l15x_exti.h"
#include "stm8l15x_conf.h"
/*************************     ˽��ͷ�ļ�����     *************************/
#include "AT.h"
#include "ME3616.h"
#include "initial.h"
#include "task_queue.h"
#include "utils.h"
#include "switch.h"
#include "default.h"
#include "proto.h"
#include "soft_uart.h"
#include "stdio.h"
/*************************      ���������ռ�      *************************/

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif
	
/*************************        ȫ��ö��        *************************/
/* ���������� */
typedef enum
{
	TX_BUFF = (uint8_t)0x01,	/* ���ͻ����� */
	RX_BUFF = (uint8_t)0x02		/* ���ջ����� */
}Buff_Type_TypeDef;



typedef enum
{
	NB_NET_UNREG,		// ����δע��
	NB_CSQ_LOW,			// �ź���
	NB_CSQ_HIGH			// �ź�ǿ

}LED_STATUS_TypeDef;


/*************************       ȫ�ֺ궨��       *************************/
/* �Ƿ�ʹ�ÿյ��𶯹����㷨 */
#define USE_AIR_FILTER_ALGO     1

#if (USE_AIR_FILTER_ALGO)
#define USE_AIR_FILTER          1 
#endif //(USE_AIR_FILTER_ALGO)

#define FALSE					0x00
#define TURE					(!FALSE)


#define EEPROM_BASE_Addr            0x1000


/* ME3616��Դ�������� */
#define		ME3616_POWER_PORT		GPIOC
#define		ME3616_POWER_PIN		GPIO_Pin_6
#define		ME3616_RESET_PORT		GPIOC
#define		ME3616_RESET_PIN		GPIO_Pin_4
#define		ME3616_WAKEUP_OUT_PORT  GPIOC
#define		ME3616_WAKEUP_OUT_PIN   GPIO_Pin_5


/* BC20��Դ�������� */
#define     BC20_POWER_PORT			 GPIOC
#define     BC20_POWER_PIN			 GPIO_Pin_6  
#define		BC20_RESET_PORT		     GPIOC
#define		BC20_RESET_PIN		     GPIO_Pin_5
#define     BC20_PSM_PORT			 GPIOC
#define     BC20_PSM_PIN			 GPIO_Pin_4


/* ���ٶȼ����ſ���*/

#define ACCLEROMETER_SCK_PORT	GPIOC
#define ACCLEROMETER_SCK_PIN	GPIO_Pin_1
#define ACCLEROMETER_SDO_PORT	GPIOC
#define ACCLEROMETER_SD0_PIN	GPIO_Pin_0
#define ACCLEROMETER_INIT1_PORT	GPIOD
#define ACCLEROMETER_INIT1_PIN  GPIO_Pin_4
#define ACCLEROMETER_INIT2_PORT	GPIOB
#define ACCLEROMETER_INIT2_PIN	GPIO_Pin_7


/* ME3616����ͨѶ���� */
  #define ME3616_TXD_PORT			GPIOC
  #define ME3616_TXD_PIN			GPIO_Pin_3
  #define ME3616_RXD_PORT			GPIOC
  #define ME3616_RXD_PIN			GPIO_Pin_2

/* BC20 ����ͨѶ���� */
  #define BC20_TXD_PORT				GPIOC
  #define BC20_TXD_PIN				GPIO_Pin_3
  #define BC20_RXD_PORT				GPIOC
  #define BC20_RXD_PIN				GPIO_Pin_2



#define MAX_TEL_LIST                8         // ���绰��������
#define MAX_TEL_NUMBER_SIZE         16        // ���绰���볤��
#define MAX_DEV_PASSWD_SIZE         4         // �豸���볤��
#define MAX_DEV_ID_SIZE             6         // �豸��ų���
#define MAX_UNIQUE_ID_SIZE			6		  // �豸Ψһ�볤��
#define MAX_DEV_IDEN_ID_SIZE        14        // �豸���ID����
#define MAX_SIM_IMSI_SIZE			16		  // IMSI�볤��-�����ƶ��û�ʶ����
#define MAX_SIM_ICCID_SIZE			21		  // ICCID
#define	MAX_IMEI_SIZE				16		  // IMEI��

#define MAX_UNIQUE_ID_SIZE          6         // ΨһID����
#define MAX_IOT_ID_SIZE             12        // IOT ID
#define MAX_DEV_APN_SIZE            30        // ��������APN��󳤶�

#define MAX_RECV_BUFF_SIZE			420		 // ���ܻ�������С
#define MAX_SEND_BUFF_SIZE			156	      // ���ͻ�������С

#define BUFF_RECV					0x01	  // ���ܻ�����
#define	BUFF_SEND					0x02	  // ���ͻ�����
/********************	ADC��ѹ�ɼ�		************************************/
#define		GET_1S_TIMER()			g_vu32_1s_timer
#define		GET_10MS_TIMER()		g_vu32_10ms_timer
/*****************************ˮӡ����*******************************/
/* ˮ������ 2 */
#define		SWITCH_TWO_PORT			GPIOB
#define		SWITCH_TWO_PIN			GPIO_Pin_2
#define		SWITCH_TWO_EXIT_PIN		EXTI_IT_Pin2

/* ˮ������ 3 */
#define		SWITCH_THREE_PORT		GPIOB
#define		SWITCH_THREE_PIN		GPIO_Pin_3
#define		SWITCH_THREE_EXIT_PIN	EXTI_IT_Pin3

/*****************************LED��*******************************/
/* �𶯵� */
#define		LED_SHAKE_PORT			GPIOA
#define		LED_SHAKE_PIN			GPIO_Pin_4
/* ע��״̬�� */
#define		LED_REGISTER_PORT		GPIOA
#define		LED_REGISTER_PIN		GPIO_Pin_2
/* GPS״̬�� */
#define		LED_GPS_PORT			GPIOA
#define		LED_GPS_PIN			    GPIO_Pin_3

/*****************************GPS�Ŵ���*******************************/
#define     GPS_EN_PORT             GPIOA
#define     GPS_EN_PIN              GPIO_Pin_5

#ifdef USE_IWDG
#define IWDG_RELOAD()				IWDG_ReloadCounter()
#define IWDG_UPDATA_TIME()			g_run_paramter.m_IWDG_reload_time = GET_10MS_TIMER()
#else
#define IWDG_RELOAD()				
#define IWDG_UPDATA_TIME()		
#endif // USE_IWDG

/*************************     ȫ�ֽṹ�嶨��     *************************/
typedef struct {
	char		m_tel_list[MAX_TEL_LIST][MAX_TEL_NUMBER_SIZE + 1];   // �绰�����
	char        m_dev_guid[MAX_DEV_ID_SIZE + 1];                     // �豸���
	char        m_dev_pwd[MAX_DEV_PASSWD_SIZE + 1];					 // �豸����
	char        m_dev_iden_id[MAX_DEV_IDEN_ID_SIZE + 1];               // �豸���ID
	char        m_dev_APN[MAX_DEV_APN_SIZE + 1];		             // ��������APN
	uint8_t     m_ser_ip[4];                                         // ������IP
	uint16_t    m_ser_port;                                          // �������˿�

	/* �豸����ʹ�ܱ�־ 8 byte */
	uint8_t		m_en_1to5_rep;
	uint8_t     m_en_gprs;                                      // �豸����ģʽ��GSM��GPRS��
	uint8_t     m_en_gps_rep;                                   // ʹ��GPS��ʱ�ϱ�
	uint8_t     m_en_tim_rep;                                   // ʹ�ܶ�ʱ�ϱ�
	uint8_t     m_en_pwr_off_delay;                             // ʹ�ܹػ���ʱ
	uint8_t     m_en_vol_check;                                 // ʹ�ܵ�ѹ���
	uint8_t     m_en_sw1;                                       // ʹ��ˮ������ 1
	uint8_t     m_en_sw2;                                       // ʹ��ˮ������ 2



	/* �豸������ֵ���� 14 byte */
	uint8_t		m_tim_passby;									// ��·ʱ��
	uint16_t    m_cnt_sw_trigger;                               // ˮ�����ش�����ֵ
	uint16_t    m_tim_pwr_delay;                                // �ػ���ʱʱ��
	uint16_t    m_tim_sleep;									// ��ʱ����ʱ������
	uint16_t    m_tim_gps_rep;                                  // �ϱ�GPSʱ����
	uint16_t    m_tim_heartbeat;                                // GPRS���������
	uint16_t    m_tim_alm_continue;                             // ����ʱGPRS����ʱ��
	uint16_t    m_tim_alm_interval;                             // GPRS�������ʱ��
	uint16_t    m_tim_no_signal_delay;						    // �豸���ź�״̬�µĵȴ�ʱ��


	/* �豸��������У�� 4 byte */
	uint32_t    m_cfg_verify;                                   // EEPROM������Ч��־
}st_ccfg_config;


typedef struct {
	/* ���ܺ�����ID 12 byte */
	char m_dev_iden_id[MAX_DEV_IDEN_ID_SIZE + 1];          // �豸���ID
	char m_sim_imsi[MAX_SIM_IMSI_SIZE + 1];				 // IMSI��-�����ƶ��û�ʶ����
	char m_unique_id[MAX_UNIQUE_ID_SIZE + 1];              // ΨһID
	char m_iot_id[MAX_IOT_ID_SIZE + 1];                    // iot id????
	char m_sim_iccid[MAX_SIM_ICCID_SIZE + 1];              // ICCID: ����������Ϊ20λ
	char m_imei[MAX_IMEI_SIZE + 1];			             // IMEI��

	/* ��ʱIP��ַ�����ڼ���û����õ�IP�ĺϷ��� */
	uint8_t     m_ser_ip[4];                             // ������IP
	uint16_t    m_ser_port;                              // �������˿�

	/* ����ʱ���� */
	uint8_t  m_flg_en_alarm_led : 1;	    // �Ƿ���˸������
	uint8_t  m_flg_en_unreg : 1;			// �豸��ע��ƣ������ж��豸�Ƿ�ע��
	uint8_t  m_flg_alarm : 1;               // �豸�Ƿ񱨾�
	uint8_t  m_gps_ok : 1;					// GPS�Ƿ�λ�ɹ�
	uint8_t  m_flg_wake_dev : 1;            // �Ƿ����豸
	uint8_t  m_flg_nb_init : 1;				// nbģ��״̬
	uint8_t	 m_flg_nb_net : 1;				// nb����״̬
	uint8_t	 m_flg_nb_gps : 1;				// nbGPS״̬
	uint8_t  m_flg_nb_rep : 1;				// �ж��豸�Ƿ�Ϊ��ʱ�ϱ�
	uint8_t  m_heartbeat_respond:1;         // ����������
	uint8_t  m_flag_nb_low_csq : 1;         // �ź�ǿ�ȵ�
	uint8_t	 m_tim_passby;					// ��·ʱ��
	uint8_t  m_cnt_low_vol;					// ��ѹ����
	uint8_t  m_gps_quere_fail_cnt;          // GPS��ѯʧ�ܼ���
	uint16_t m_tim_pwr_off_delay;	        // ��ʱ�ػ�
	uint16_t m_tim_sleep;					// ��ʱ�ϱ����� TIME REPORT
	int16_t m_tim_gps_rep;	                // GPS�����ϱ�ʱ�� ����
	uint16_t m_tim_heartbeat;	            // ������ʱ��
	int16_t m_tim_alm_continue;            // �����ĳ���ʱ��


	struct {
		uint8_t  m_nbiot_init_ok_cnt;
		uint8_t  m_nbiot_init_fail_cnt;
		uint8_t	 m_nbiot_net_abnormal_cnt;
		uint8_t  m_nbiot_send_cnt;
		uint8_t  m_nbiot_recv_cnt;
		uint8_t  m_nbiot_csq;
	}m_nb_stat;

	uint32_t m_nbiot_start_proc_time;

#ifdef USE_IWDG
	uint32_t m_IWDG_reload_time;
#endif // USE_IWDG

#ifdef USE_UDP
	struct
	{
		uint32_t m_send_time;			// ��¼�������ݵ�ʱ��
		st_task  m_task;	            // ��¼���͵�����
		uint8_t m_flg_retry : 1;		// �����Ƿ��ط�
		uint8_t m_flg_sending : 1;		// ���壺δ�յ�����Ϊ�����Ƿ����ڷ���
		uint8_t m_cnt_send_fail : 6;	// ����ʧ��
	}m_udp_transfer;

#endif // USE_UDP

#ifdef USE_AIR_FILTER
	uint8_t air_filtering;              // �Ƿ����ڽ��пյ��˲�����
#endif // USE_AIR_FILTER

	uint8_t  m_dev_voltage;				// �豸��ѹ
	uint16_t m_switch_trigger_cnt;

	uint16_t m_pkg_num;


}st_run_parameter;

typedef struct
{
	uint8_t	m_flg_recv_lock : 1;	// ���ջ�����������
	uint8_t	m_flg_send_lock : 1;	// ���ͻ�����������
	uint16_t m_recv_index;			// �������±�
	uint16_t m_send_index;

	uint8_t m_recv_buff[MAX_RECV_BUFF_SIZE]; // ����������
	uint8_t m_send_buff[MAX_SEND_BUFF_SIZE];
	uint32_t m_recv_timeout;
}st_buff;


/*************************      ȫ�ֱ�������      *************************/


	extern st_ccfg_config	g_ccfg_config;
	extern st_run_parameter g_run_paramter;
	extern uint32_t __IO    g_vu32_1s_timer;
	extern uint32_t	__IO	g_vu32_10ms_timer;
	extern uint32_t			g_l620_init_timeout;
	extern uint32_t			g_l620_net_timeout;
	extern st_buff			g_buff;
	extern LED_STATUS_TypeDef led_status;

/*************************        ��������        *************************/
	extern 	void delay_s(uint8_t s);
	extern void delay_10ms(uint32_t ms);
	extern void p_ccfg_del_tel(uint8_t x);
	extern void dev_boot_routine(void);
	extern void dev_shutdown_routine(void);
	extern void p_ccfg_load(void);
	extern void p_ccfg_reset(void);
	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     ���ظ���������     *************************/
#endif



