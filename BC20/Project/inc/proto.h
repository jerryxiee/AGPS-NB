
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

  /*************************     防重复包含定义     *************************/
#ifndef __PROTO_H__
#define __PROTO_H__

/*************************     系统头文件包含     *************************/


/*************************     私有头文件包含     *************************/
#include "common_cfg.h"
/*************************      导入命名空间      *************************/

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

/*************************        全局枚举        *************************/
/* 数据包的类型 */
typedef enum
{
	PKG_HEARTBEAT = (uint8_t)0x00,  /* 心跳包 */
	PKG_DATA = (uint8_t)0x02,		/* 数据 可以发送任意数据 */
} PKG_Type_TypeDef;
/*************************       全局宏定义       *************************/
#define MAX_SERVER_KEY_SIZE			28		  // 服务器验证码长度
#define TRANSFAR_TIME_OUT			5		  // 传输数据超时时间 （单位：S）

/*************************     全局结构体定义     *************************/




/*************************      全局变量定义      *************************/
extern uint8_t			server_key[MAX_SERVER_KEY_SIZE];
/*************************        函数声明        *************************/
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
	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     防重复包含定义     *************************/
#endif



