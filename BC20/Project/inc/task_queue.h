
/**
  *************************************************************************
  * @file    task_queue.h
  * @author  LZY.Huaqian
  * @version V2.0.0
  * @date    2020/03/23
  * @brief
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */

  /*************************     防重复包含定义     *************************/
#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__

/*************************     系统头文件包含     *************************/


/*************************     私有头文件包含     *************************/
#include "stm8l15x.h"
#include "task_queue.h"
#include "sleep.h"
/*************************      导入命名空间      *************************/

/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        全局枚举        *************************/
	/* 任务ID */
	typedef enum 
	{
		/* 服务器通讯任务 */
		TASK_NULL = 0x00,
		TASK_HEARTBEAT,				// 发送心跳包
		TASK_GET_SET_1,             // 回复配置信息1
		TASK_GET_SET_2,				// 回复配置信息2
		TASK_TIME_REP,              // 发送定时上报
		TASK_TCP_TIME_REP,          // GPS网络上报
		TASK_SEND_DATA,             // 通过TCP发送数据
		TASK_SEND_DATA_NMEA,
		TASK_QUERY_ID,              // 物联网卡请求序列号 7
		TASK_SEND_ID,			    // 发送唯一码
		TASK_SEND_ICCID,			// 发送ICCID
		TASK_SEND_FWVER,			// 发送版本信息
		TASK_RE_FORMAT,				// 反馈格式化成功

		/* 本地任务 */
		TASK_SLEEP = 0x80,			// 设备休眠任务
		TASK_CON_SER = 0x81,        // TCP连接服务器
		TASK_PROCESS_GPS,           // 处理GPS数据
		TASK_CLOSE_TCP,             // 关闭TCP连接
		TASK_SERVER_CFG,			// 服务器配置信息更改
		TASK_PASSBY_START,			// 开始旁路
		TASK_REGISTER,				// 设备被注册后，第一个任务
		TASK_SWITCHGPS,				// GPS模块控制
		TASK_RECV_DATA,				// 接收数据
		TASK_WAKE_NB,				// 唤醒NB
		TASK_NBREST,                // NB复位
		TASK_DEBUG_INFO,
		/*测试任务*/
		LED1_ON,
		LED2_ON,
		LED3_ON,
		LED1_OFF,
		LED2_OFF,
		LED3_OFF,
	}Task_Queue_TypeDef;

	/*************************       全局宏定义       *************************/
#define MAX_DEV_RUN_LIST            16        // 设备操作列表长度
#define TASK_ID_LOCAL_MASK			0x80	  // 任务的掩码 用来判断是都是本地任务

/*************************     全局结构体定义     *************************/
	typedef struct {
		Task_Queue_TypeDef    m_task_id;
		char* m_data1;
		char* m_data2;
	}st_task;

	typedef struct {
		st_task      m_task_list[MAX_DEV_RUN_LIST];
		uint8_t      m_task_begin : 4;
		uint8_t      m_task_end : 4;
	}st_task_queue;


/*************************      全局变量定义      *************************/
	extern st_task_queue g_task_queue;

/*************************        函数声明        *************************/


    void handle_task_queue(void);

	void p_task_enqueue(Task_Queue_TypeDef task_id, char* data1, char* data2);

	void p_task_dequeue(st_task* task);

	int8_t p_task_queue_isempty(void);

	Task_Queue_TypeDef p_task_queue_head(char** data1, char** data2);

	void p_task_clear(void);

	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     防重复包含定义     *************************/
#endif



