
/**
  *************************************************************************
  * @file    AT.h
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
#ifndef __PROTO_H__
#define __PROTO_H__

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
/* ���ݰ������� */
typedef enum
{
	PKG_HEARTBEAT = (uint8_t)0x00,  /* ������ */
	PKG_DATA = (uint8_t)0x02,		/* ���� ���Է����������� */
} PKG_Type_TypeDef;
/*************************       ȫ�ֺ궨��       *************************/
#define MAX_SERVER_KEY_SIZE			28		  // ��������֤�볤��
#define TRANSFAR_TIME_OUT			5		  // �������ݳ�ʱʱ�� ����λ��S��

/*************************     ȫ�ֽṹ�嶨��     *************************/




/*************************      ȫ�ֱ�������      *************************/
extern uint8_t			server_key[MAX_SERVER_KEY_SIZE];
/*************************        ��������        *************************/
extern uint16_t p_proto_creat_pkg_head(uint8_t* p_buff, uint16_t* p_index, PKG_Type_TypeDef pkg_type);
	extern void p_proto_gen_server_key(void);
	extern uint16_t p_proto_creat_heartbeat(void);
	extern void p_proto_gen_iotid(void);
	extern void p_proc_listen_event(void);
	extern uint16_t p_proto_creat_data(uint8_t* data, uint8_t len);
	extern void p_proto_reply_setting_2(void);
	extern void p_proto_reply_setting_1(void);
	extern uint8_t p_proto_dispose_data(char* buf, int len);
	extern void p_proc_creat_iot_queryid(void);
	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     ���ظ���������     *************************/
#endif



