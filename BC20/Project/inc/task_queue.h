
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

  /*************************     ���ظ���������     *************************/
#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__

/*************************     ϵͳͷ�ļ�����     *************************/


/*************************     ˽��ͷ�ļ�����     *************************/
#include "stm8l15x.h"
#include "task_queue.h"
#include "sleep.h"
/*************************      ���������ռ�      *************************/

/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        ȫ��ö��        *************************/
	/* ����ID */
	typedef enum 
	{
		/* ������ͨѶ���� */
		TASK_NULL = 0x00,
		TASK_HEARTBEAT,				// ����������
		TASK_GET_SET_1,             // �ظ�������Ϣ1
		TASK_GET_SET_2,				// �ظ�������Ϣ2
		TASK_TIME_REP,              // ���Ͷ�ʱ�ϱ�
		TASK_TCP_TIME_REP,          // GPS�����ϱ�
		TASK_SEND_DATA,             // ͨ��TCP��������
		TASK_SEND_DATA_NMEA,
		TASK_QUERY_ID,              // ���������������к� 7
		TASK_SEND_ID,			    // ����Ψһ��
		TASK_SEND_ICCID,			// ����ICCID
		TASK_SEND_FWVER,			// ���Ͱ汾��Ϣ
		TASK_RE_FORMAT,				// ������ʽ���ɹ�

		/* �������� */
		TASK_SLEEP = 0x80,			// �豸��������
		TASK_CON_SER = 0x81,        // TCP���ӷ�����
		TASK_PROCESS_GPS,           // ����GPS����
		TASK_CLOSE_TCP,             // �ر�TCP����
		TASK_SERVER_CFG,			// ������������Ϣ����
		TASK_PASSBY_START,			// ��ʼ��·
		TASK_REGISTER,				// �豸��ע��󣬵�һ������
		TASK_SWITCHGPS,				// GPSģ�����
		TASK_RECV_DATA,				// ��������
		TASK_WAKE_NB,				// ����NB
		TASK_NBREST,                // NB��λ
		TASK_DEBUG_INFO,
		/*��������*/
		LED1_ON,
		LED2_ON,
		LED3_ON,
		LED1_OFF,
		LED2_OFF,
		LED3_OFF,
	}Task_Queue_TypeDef;

	/*************************       ȫ�ֺ궨��       *************************/
#define MAX_DEV_RUN_LIST            16        // �豸�����б���
#define TASK_ID_LOCAL_MASK			0x80	  // ��������� �����ж��Ƕ��Ǳ�������

/*************************     ȫ�ֽṹ�嶨��     *************************/
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


/*************************      ȫ�ֱ�������      *************************/
	extern st_task_queue g_task_queue;

/*************************        ��������        *************************/


    void handle_task_queue(void);

	void p_task_enqueue(Task_Queue_TypeDef task_id, char* data1, char* data2);

	void p_task_dequeue(st_task* task);

	int8_t p_task_queue_isempty(void);

	Task_Queue_TypeDef p_task_queue_head(char** data1, char** data2);

	void p_task_clear(void);

	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif

/*************************     ���ظ���������     *************************/
#endif



