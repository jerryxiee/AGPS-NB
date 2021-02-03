 
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

  /*************************     防重复包含定义     *************************/
#ifndef __COMMON_CFG_H__
#define __COMMON_CFG_H__

/*************************     系统头文件包含     *************************/
#include "stm8l15x.h"
#include "stm8l15x_conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm8l15x_exti.h"
#include "stm8l15x_conf.h"
/*************************     私有头文件包含     *************************/
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
/*************************      导入命名空间      *************************/

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif
	
/*************************        全局枚举        *************************/
/* 缓冲区类型 */
typedef enum
{
	TX_BUFF = (uint8_t)0x01,	/* 发送缓冲区 */
	RX_BUFF = (uint8_t)0x02		/* 接收缓冲区 */
}Buff_Type_TypeDef;



typedef enum
{
	NB_NET_UNREG,		// 网络未注册
	NB_CSQ_LOW,			// 信号弱
	NB_CSQ_HIGH			// 信号强

}LED_STATUS_TypeDef;


/*************************       全局宏定义       *************************/
/* 是否使用空调震动过滤算法 */
#define USE_AIR_FILTER_ALGO     1

#if (USE_AIR_FILTER_ALGO)
#define USE_AIR_FILTER          1 
#endif //(USE_AIR_FILTER_ALGO)

#define FALSE					0x00
#define TURE					(!FALSE)


#define EEPROM_BASE_Addr            0x1000


/* ME3616电源控制引脚 */
#define		ME3616_POWER_PORT		GPIOC
#define		ME3616_POWER_PIN		GPIO_Pin_6
#define		ME3616_RESET_PORT		GPIOC
#define		ME3616_RESET_PIN		GPIO_Pin_4
#define		ME3616_WAKEUP_OUT_PORT  GPIOC
#define		ME3616_WAKEUP_OUT_PIN   GPIO_Pin_5


/* BC20电源控制引脚 */
#define     BC20_POWER_PORT			 GPIOC
#define     BC20_POWER_PIN			 GPIO_Pin_6  
#define		BC20_RESET_PORT		     GPIOC
#define		BC20_RESET_PIN		     GPIO_Pin_5
#define     BC20_PSM_PORT			 GPIOC
#define     BC20_PSM_PIN			 GPIO_Pin_4


/* 加速度计引脚控制*/

#define ACCLEROMETER_SCK_PORT	GPIOC
#define ACCLEROMETER_SCK_PIN	GPIO_Pin_1
#define ACCLEROMETER_SDO_PORT	GPIOC
#define ACCLEROMETER_SD0_PIN	GPIO_Pin_0
#define ACCLEROMETER_INIT1_PORT	GPIOD
#define ACCLEROMETER_INIT1_PIN  GPIO_Pin_4
#define ACCLEROMETER_INIT2_PORT	GPIOB
#define ACCLEROMETER_INIT2_PIN	GPIO_Pin_7


/* ME3616串口通讯引脚 */
  #define ME3616_TXD_PORT			GPIOC
  #define ME3616_TXD_PIN			GPIO_Pin_3
  #define ME3616_RXD_PORT			GPIOC
  #define ME3616_RXD_PIN			GPIO_Pin_2

/* BC20 串口通讯引脚 */
  #define BC20_TXD_PORT				GPIOC
  #define BC20_TXD_PIN				GPIO_Pin_3
  #define BC20_RXD_PORT				GPIOC
  #define BC20_RXD_PIN				GPIO_Pin_2



#define MAX_TEL_LIST                8         // 最大电话号码组数
#define MAX_TEL_NUMBER_SIZE         16        // 最大电话号码长度
#define MAX_DEV_PASSWD_SIZE         4         // 设备密码长度
#define MAX_DEV_ID_SIZE             6         // 设备编号长度
#define MAX_UNIQUE_ID_SIZE			6		  // 设备唯一码长度
#define MAX_DEV_IDEN_ID_SIZE        14        // 设备身份ID长度
#define MAX_SIM_IMSI_SIZE			16		  // IMSI码长度-国际移动用户识别码
#define MAX_SIM_ICCID_SIZE			21		  // ICCID
#define	MAX_IMEI_SIZE				16		  // IMEI码

#define MAX_UNIQUE_ID_SIZE          6         // 唯一ID长度
#define MAX_IOT_ID_SIZE             12        // IOT ID
#define MAX_DEV_APN_SIZE            30        // 物联网卡APN最大长度

#define MAX_RECV_BUFF_SIZE			420		 // 接受缓冲区大小
#define MAX_SEND_BUFF_SIZE			156	      // 发送缓冲区大小

#define BUFF_RECV					0x01	  // 接受缓冲区
#define	BUFF_SEND					0x02	  // 发送缓冲区
/********************	ADC电压采集		************************************/
#define		GET_1S_TIMER()			g_vu32_1s_timer
#define		GET_10MS_TIMER()		g_vu32_10ms_timer
/*****************************水印开关*******************************/
/* 水银开关 2 */
#define		SWITCH_TWO_PORT			GPIOB
#define		SWITCH_TWO_PIN			GPIO_Pin_2
#define		SWITCH_TWO_EXIT_PIN		EXTI_IT_Pin2

/* 水银开关 3 */
#define		SWITCH_THREE_PORT		GPIOB
#define		SWITCH_THREE_PIN		GPIO_Pin_3
#define		SWITCH_THREE_EXIT_PIN	EXTI_IT_Pin3

/*****************************LED灯*******************************/
/* 震动灯 */
#define		LED_SHAKE_PORT			GPIOA
#define		LED_SHAKE_PIN			GPIO_Pin_4
/* 注册状态灯 */
#define		LED_REGISTER_PORT		GPIOA
#define		LED_REGISTER_PIN		GPIO_Pin_2
/* GPS状态灯 */
#define		LED_GPS_PORT			GPIOA
#define		LED_GPS_PIN			    GPIO_Pin_3

/*****************************GPS放大器*******************************/
#define     GPS_EN_PORT             GPIOA
#define     GPS_EN_PIN              GPIO_Pin_5

#ifdef USE_IWDG
#define IWDG_RELOAD()				IWDG_ReloadCounter()
#define IWDG_UPDATA_TIME()			g_run_paramter.m_IWDG_reload_time = GET_10MS_TIMER()
#else
#define IWDG_RELOAD()				
#define IWDG_UPDATA_TIME()		
#endif // USE_IWDG

/*************************     全局结构体定义     *************************/
typedef struct {
	char		m_tel_list[MAX_TEL_LIST][MAX_TEL_NUMBER_SIZE + 1];   // 电话号码表
	char        m_dev_guid[MAX_DEV_ID_SIZE + 1];                     // 设备编号
	char        m_dev_pwd[MAX_DEV_PASSWD_SIZE + 1];					 // 设备密码
	char        m_dev_iden_id[MAX_DEV_IDEN_ID_SIZE + 1];               // 设备身份ID
	char        m_dev_APN[MAX_DEV_APN_SIZE + 1];		             // 物联网卡APN
	uint8_t     m_ser_ip[4];                                         // 服务器IP
	uint16_t    m_ser_port;                                          // 服务器端口

	/* 设备各项使能标志 8 byte */
	uint8_t		m_en_1to5_rep;
	uint8_t     m_en_gprs;                                      // 设备工作模式（GSM、GPRS）
	uint8_t     m_en_gps_rep;                                   // 使能GPS定时上报
	uint8_t     m_en_tim_rep;                                   // 使能定时上报
	uint8_t     m_en_pwr_off_delay;                             // 使能关机延时
	uint8_t     m_en_vol_check;                                 // 使能低压检测
	uint8_t     m_en_sw1;                                       // 使能水银开关 1
	uint8_t     m_en_sw2;                                       // 使能水银开关 2



	/* 设备各项数值设置 14 byte */
	uint8_t		m_tim_passby;									// 旁路时间
	uint16_t    m_cnt_sw_trigger;                               // 水银开关触发阈值
	uint16_t    m_tim_pwr_delay;                                // 关机延时时间
	uint16_t    m_tim_sleep;									// 定时报告时间周期
	uint16_t    m_tim_gps_rep;                                  // 上报GPS时间间隔
	uint16_t    m_tim_heartbeat;                                // GPRS心跳包间隔
	uint16_t    m_tim_alm_continue;                             // 报警时GPRS工作时间
	uint16_t    m_tim_alm_interval;                             // GPRS报警间隔时间
	uint16_t    m_tim_no_signal_delay;						    // 设备无信号状态下的等待时间


	/* 设备参数设置校验 4 byte */
	uint32_t    m_cfg_verify;                                   // EEPROM设置有效标志
}st_ccfg_config;


typedef struct {
	/* 加密后的身份ID 12 byte */
	char m_dev_iden_id[MAX_DEV_IDEN_ID_SIZE + 1];          // 设备身份ID
	char m_sim_imsi[MAX_SIM_IMSI_SIZE + 1];				 // IMSI码-国际移动用户识别码
	char m_unique_id[MAX_UNIQUE_ID_SIZE + 1];              // 唯一ID
	char m_iot_id[MAX_IOT_ID_SIZE + 1];                    // iot id????
	char m_sim_iccid[MAX_SIM_ICCID_SIZE + 1];              // ICCID: 物联网卡号为20位
	char m_imei[MAX_IMEI_SIZE + 1];			             // IMEI码

	/* 临时IP地址，用于检测用户设置的IP的合法性 */
	uint8_t     m_ser_ip[4];                             // 服务器IP
	uint16_t    m_ser_port;                              // 服务器端口

	/* 运行时参数 */
	uint8_t  m_flg_en_alarm_led : 1;	    // 是否闪烁报警灯
	uint8_t  m_flg_en_unreg : 1;			// 设备无注册灯，用于判断设备是否注册
	uint8_t  m_flg_alarm : 1;               // 设备是否报警
	uint8_t  m_gps_ok : 1;					// GPS是否定位成功
	uint8_t  m_flg_wake_dev : 1;            // 是否唤醒设备
	uint8_t  m_flg_nb_init : 1;				// nb模块状态
	uint8_t	 m_flg_nb_net : 1;				// nb网络状态
	uint8_t	 m_flg_nb_gps : 1;				// nbGPS状态
	uint8_t  m_flg_nb_rep : 1;				// 判断设备是否为定时上报
	uint8_t  m_heartbeat_respond:1;         // 心跳包反馈
	uint8_t  m_flag_nb_low_csq : 1;         // 信号强度低
	uint8_t	 m_tim_passby;					// 旁路时间
	uint8_t  m_cnt_low_vol;					// 低压计数
	uint8_t  m_gps_quere_fail_cnt;          // GPS查询失败计数
	uint16_t m_tim_pwr_off_delay;	        // 延时关机
	uint16_t m_tim_sleep;					// 定时上报周期 TIME REPORT
	int16_t m_tim_gps_rep;	                // GPS数据上报时间 心跳
	uint16_t m_tim_heartbeat;	            // 心跳的时间
	int16_t m_tim_alm_continue;            // 报警的持续时间


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
		uint32_t m_send_time;			// 记录发送数据的时间
		st_task  m_task;	            // 记录发送的任务
		uint8_t m_flg_retry : 1;		// 发送是否重发
		uint8_t m_flg_sending : 1;		// 定义：未收到反馈为数据是否正在发送
		uint8_t m_cnt_send_fail : 6;	// 发送失败
	}m_udp_transfer;

#endif // USE_UDP

#ifdef USE_AIR_FILTER
	uint8_t air_filtering;              // 是否正在进行空调滤波操作
#endif // USE_AIR_FILTER

	uint8_t  m_dev_voltage;				// 设备电压
	uint16_t m_switch_trigger_cnt;

	uint16_t m_pkg_num;


}st_run_parameter;

typedef struct
{
	uint8_t	m_flg_recv_lock : 1;	// 接收缓冲区互斥锁
	uint8_t	m_flg_send_lock : 1;	// 发送缓冲区互斥锁
	uint16_t m_recv_index;			// 接收区下标
	uint16_t m_send_index;

	uint8_t m_recv_buff[MAX_RECV_BUFF_SIZE]; // 缓冲区数组
	uint8_t m_send_buff[MAX_SEND_BUFF_SIZE];
	uint32_t m_recv_timeout;
}st_buff;


/*************************      全局变量定义      *************************/


	extern st_ccfg_config	g_ccfg_config;
	extern st_run_parameter g_run_paramter;
	extern uint32_t __IO    g_vu32_1s_timer;
	extern uint32_t	__IO	g_vu32_10ms_timer;
	extern uint32_t			g_l620_init_timeout;
	extern uint32_t			g_l620_net_timeout;
	extern st_buff			g_buff;
	extern LED_STATUS_TypeDef led_status;

/*************************        函数声明        *************************/
	extern 	void delay_s(uint8_t s);
	extern void delay_10ms(uint32_t ms);
	extern void p_ccfg_del_tel(uint8_t x);
	extern void dev_boot_routine(void);
	extern void dev_shutdown_routine(void);
	extern void p_ccfg_load(void);
	extern void p_ccfg_reset(void);
	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     防重复包含定义     *************************/
#endif



