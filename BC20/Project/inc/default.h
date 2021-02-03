
/**
  *************************************************************************
  * @file    default.h
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

  /*************************     防重复包含定义     *************************/
#ifndef __DEFAULT_H__
#define __DEFAULT_H__

/*************************     系统头文件包含     *************************/


/*************************     私有头文件包含     *************************/

/*************************      导入命名空间      *************************/

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        全局枚举        *************************/

	/*************************       全局宏定义       *************************/
#define _STR(x)						#x
#define STR(x)						_STR(x)
#define STRLEN(x)					(sizeof(_STR(x))-1)
#define ARRAY_SIZE(x)				sizeof(_STR(x))			// 对应的数组至少要有的长度

/* 版本日期 */
#define	VOL							3.6V
#define	DATA						__DATE__
#define _VERSION(data,type)			"version 5.0 " data " " type
#define FW_VERSION(data,type)		_VERSION(data,_STR(type))



/* 服务器配置信息 */
#ifndef SHIKE_SERVER
#define IP_1						117	
#define IP_2						27
#define IP_3						157
#define IP_4						39
#define PORT						7879
#else
#define IP_1						117	
#define IP_2						27
#define IP_3						157
#define IP_4						39
#define PORT						7879
#endif

#define _SERVER_IP(a,b,c,d)			_STR(a##.##b##.##c##.##d)
#define	SERVER_IP(a,b,c,d)			_SERVER_IP(a,b,c,d)
#define _PORT(x)					_STR(x)
#define SERVER_PORT(x)				_PORT(x)

#ifdef USE_UDP
#define UDP_TRANSFER_TIMEOUT		5				// UDP数据包未收到反馈超时时间
#define UDP_MAX_RESENF_CNT			3				// UDP数据包重发次数				
#endif // USE_UDPS

/* 看门狗超时时间 */
#define DEFAULT_IWDG_TIME				12000

/* 设备 默认APN */
#define	DEFAULT_DEV_APN					ctnb

/* 设备 默认GUID 编号 */
#define DEFAULT_DEV_GUID				ABCDEF

/* 设备 默认上报服务器GUID ID */
#define DEFAULT_DEV_SER_GUID			FFFFFF

/* 设备 默认身份ID 最长14 */
#define DEFAULT_DEV_IDEN_ID				12345678901234

/* 设备 默认密码 */
#define DEFAULT_DEV_PWD					0000

/* 使能1-5组电话上报 */
#define DEFAULT_1TO_REP					FALSE

/* 使能GPS定时上报 */
#define DEFAULT_GPS_REP					TRUE	

/* 使能定时上报 */
#define DEFAULT_TIME_REP				TRUE

/* 使能关机延时 */
#define DEFAULT_PWR_OFF_DELAY			TRUE

/* 使能低压检测 */
#define DEFAULT_VOL_CHECK				TRUE

/* 使能水银开关 1 */
#define DEFAULT_SWITCH_1			TRUE

/* 使能水银开关 2*/
#define DEFAULT_SWITCH_2		       TRUE

/* 距关机时间 秒 没有用 */
#define DEFAULT_TIM_PWR_DELAY			15
#define MIN_TIM_PWR_DELAY				5
#define MAX_TIM_PWR_DELAY				60

/* 定时报告周期 小时 */
#define DEFAULT_TIM_SELLP				720
#define MIN_TIM_SLEEP					1
#define MAX_TIM_SLEEP					999

/* 上报GPS的时间 分 */
#define DEFAULT_TIM_GPS_REP				15	// 报警状态，持续工作时长 
#define MIN_TIM_GPS_REP					1
#define MAX_TIM_GPS_REP					60

/* 无报警上报心跳包周期 60s */
#define DEFAULT_TIM_HEARTBEAT			46
#define MIN_TIM_HEARTBEAT				30
#define MAX_TIM_HEARTBEAT				99

/* 触发报警后持续上报时间 秒 */
#define DEFAULT_TIM_ALM_CONTINUE		9
#define MIN_TIM_ALM_CONTINUE			1
#define MAX_TIM_ALM_CONTINUE			9

/* 报警后GPS的上报间隔（秒） */
#define DEFAULT_TIM_ALM_INTERVAL		15	 // 实际上是心跳包的间隔
#define MIN_TIM_ALM_INTERVAL			5
#define MAX_TIM_ALM_INTERVAL			60

/* 休眠状态下触发报警的水银开关触发次数 */
#define DEFAULT_CNT_SW_TRIGGER			3
#define MIN_CNT_SW_TRIGGER				1
#define MAX_CNT_SW_TRIGGER				9

/* 无信号关机延时时间 */
#define DEFAULT_TIM_NO_SIGNAL_DELAY		0
#define MIN_TIM_NO_SIGNAL_DELAY			0
#define MAX_TIM_NO_SIGNAL_DELAY			9

/* 无信号情况下默认的关机时间 */
#define DEFAULT_TIM_NO_SIGN_DEV_OFF		600

/* 盘路时间 */
#define MIN_PASSBY_TIME					1
#define MAX_PASSBY_TIME					48

/* 低压下界 2.1V */
#define DEFAULT_LOW_POWER				21

/* 定位失败重启模块时间 */
#define MAX_UNLOCATE_TIME				600

#define CONRTINUE_LOCATE_TIME           10

/* APN异常检查 */
#define DEV_APN_ASSERT()			while((STRLEN(DEFAULT_DEV_APN) > sizeof(g_ccfg_config.m_dev_APN)))

/* GUID异常检查 */
#define DEV_GUID_ASSERT()			while(STRLEN(DEFAULT_DEV_GUID) > sizeof(g_ccfg_config.m_dev_guid))

/* 默认身份ID 异常检查 */
#define DEV_IDEN_ID_ASSERT()		while(STRLEN(DEFAULT_DEV_IDEN_ID) > sizeof(g_ccfg_config.m_dev_iden_id))

/* 默认密码 异常检查 */
#define DEV_PWD_ASSERT()			while(STRLEN(DEFAULT_DEV_PWD) > sizeof(g_ccfg_config.m_dev_pwd))

/* 设备数组越界异常检查 */
#define ASSERT()					do \
									{\
										DEV_APN_ASSERT();\
										DEV_GUID_ASSERT();\
										DEV_IDEN_ID_ASSERT();\
										DEV_PWD_ASSERT();\
									} while (0)

#define CHECK_NB_TIME				5


/*************************     全局结构体定义     *************************/

/*************************      全局变量定义      *************************/

/*************************        函数声明        *************************/

	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     防重复包含定义     *************************/
#endif



