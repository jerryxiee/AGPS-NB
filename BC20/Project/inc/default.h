
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

  /*************************     ���ظ���������     *************************/
#ifndef __DEFAULT_H__
#define __DEFAULT_H__

/*************************     ϵͳͷ�ļ�����     *************************/


/*************************     ˽��ͷ�ļ�����     *************************/

/*************************      ���������ռ�      *************************/

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        ȫ��ö��        *************************/

	/*************************       ȫ�ֺ궨��       *************************/
#define _STR(x)						#x
#define STR(x)						_STR(x)
#define STRLEN(x)					(sizeof(_STR(x))-1)
#define ARRAY_SIZE(x)				sizeof(_STR(x))			// ��Ӧ����������Ҫ�еĳ���

/* �汾���� */
#define	VOL							3.6V
#define	DATA						__DATE__
#define _VERSION(data,type)			"version 5.0 " data " " type
#define FW_VERSION(data,type)		_VERSION(data,_STR(type))



/* ������������Ϣ */
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
#define UDP_TRANSFER_TIMEOUT		5				// UDP���ݰ�δ�յ�������ʱʱ��
#define UDP_MAX_RESENF_CNT			3				// UDP���ݰ��ط�����				
#endif // USE_UDPS

/* ���Ź���ʱʱ�� */
#define DEFAULT_IWDG_TIME				12000

/* �豸 Ĭ��APN */
#define	DEFAULT_DEV_APN					ctnb

/* �豸 Ĭ��GUID ��� */
#define DEFAULT_DEV_GUID				ABCDEF

/* �豸 Ĭ���ϱ�������GUID ID */
#define DEFAULT_DEV_SER_GUID			FFFFFF

/* �豸 Ĭ�����ID �14 */
#define DEFAULT_DEV_IDEN_ID				12345678901234

/* �豸 Ĭ������ */
#define DEFAULT_DEV_PWD					0000

/* ʹ��1-5��绰�ϱ� */
#define DEFAULT_1TO_REP					FALSE

/* ʹ��GPS��ʱ�ϱ� */
#define DEFAULT_GPS_REP					TRUE	

/* ʹ�ܶ�ʱ�ϱ� */
#define DEFAULT_TIME_REP				TRUE

/* ʹ�ܹػ���ʱ */
#define DEFAULT_PWR_OFF_DELAY			TRUE

/* ʹ�ܵ�ѹ��� */
#define DEFAULT_VOL_CHECK				TRUE

/* ʹ��ˮ������ 1 */
#define DEFAULT_SWITCH_1			TRUE

/* ʹ��ˮ������ 2*/
#define DEFAULT_SWITCH_2		       TRUE

/* ��ػ�ʱ�� �� û���� */
#define DEFAULT_TIM_PWR_DELAY			15
#define MIN_TIM_PWR_DELAY				5
#define MAX_TIM_PWR_DELAY				60

/* ��ʱ�������� Сʱ */
#define DEFAULT_TIM_SELLP				720
#define MIN_TIM_SLEEP					1
#define MAX_TIM_SLEEP					999

/* �ϱ�GPS��ʱ�� �� */
#define DEFAULT_TIM_GPS_REP				15	// ����״̬����������ʱ�� 
#define MIN_TIM_GPS_REP					1
#define MAX_TIM_GPS_REP					60

/* �ޱ����ϱ����������� 60s */
#define DEFAULT_TIM_HEARTBEAT			46
#define MIN_TIM_HEARTBEAT				30
#define MAX_TIM_HEARTBEAT				99

/* ��������������ϱ�ʱ�� �� */
#define DEFAULT_TIM_ALM_CONTINUE		9
#define MIN_TIM_ALM_CONTINUE			1
#define MAX_TIM_ALM_CONTINUE			9

/* ������GPS���ϱ�������룩 */
#define DEFAULT_TIM_ALM_INTERVAL		15	 // ʵ�������������ļ��
#define MIN_TIM_ALM_INTERVAL			5
#define MAX_TIM_ALM_INTERVAL			60

/* ����״̬�´���������ˮ�����ش������� */
#define DEFAULT_CNT_SW_TRIGGER			3
#define MIN_CNT_SW_TRIGGER				1
#define MAX_CNT_SW_TRIGGER				9

/* ���źŹػ���ʱʱ�� */
#define DEFAULT_TIM_NO_SIGNAL_DELAY		0
#define MIN_TIM_NO_SIGNAL_DELAY			0
#define MAX_TIM_NO_SIGNAL_DELAY			9

/* ���ź������Ĭ�ϵĹػ�ʱ�� */
#define DEFAULT_TIM_NO_SIGN_DEV_OFF		600

/* ��·ʱ�� */
#define MIN_PASSBY_TIME					1
#define MAX_PASSBY_TIME					48

/* ��ѹ�½� 2.1V */
#define DEFAULT_LOW_POWER				21

/* ��λʧ������ģ��ʱ�� */
#define MAX_UNLOCATE_TIME				600

#define CONRTINUE_LOCATE_TIME           10

/* APN�쳣��� */
#define DEV_APN_ASSERT()			while((STRLEN(DEFAULT_DEV_APN) > sizeof(g_ccfg_config.m_dev_APN)))

/* GUID�쳣��� */
#define DEV_GUID_ASSERT()			while(STRLEN(DEFAULT_DEV_GUID) > sizeof(g_ccfg_config.m_dev_guid))

/* Ĭ�����ID �쳣��� */
#define DEV_IDEN_ID_ASSERT()		while(STRLEN(DEFAULT_DEV_IDEN_ID) > sizeof(g_ccfg_config.m_dev_iden_id))

/* Ĭ������ �쳣��� */
#define DEV_PWD_ASSERT()			while(STRLEN(DEFAULT_DEV_PWD) > sizeof(g_ccfg_config.m_dev_pwd))

/* �豸����Խ���쳣��� */
#define ASSERT()					do \
									{\
										DEV_APN_ASSERT();\
										DEV_GUID_ASSERT();\
										DEV_IDEN_ID_ASSERT();\
										DEV_PWD_ASSERT();\
									} while (0)

#define CHECK_NB_TIME				5


/*************************     ȫ�ֽṹ�嶨��     *************************/

/*************************      ȫ�ֱ�������      *************************/

/*************************        ��������        *************************/

	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     ���ظ���������     *************************/
#endif



