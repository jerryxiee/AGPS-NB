
/**
  *************************************************************************
  * @file    AT.c
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

  /*************************     ϵͳͷ�ļ�����     *************************/
#include "stm8l15x.h"
#include <string.h>
#include "stm8l15x_usart.h"
/*************************     ˽��ͷ�ļ�����     *************************/

#include "AT.h"


/*************************      ���������ռ�      *************************/


/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        ȫ��ö��        *************************/

	/*************************       ȫ�ֺ궨��       *************************/

	/*************************     ȫ�ֽṹ�嶨��     *************************/

	/*************************      ȫ�ֱ�������      *************************/

	char* const s_at_rbuf = (char*)g_buff.m_recv_buff;


	/*************************        ��������        *************************/

	/*************************        ����ʵ��        *************************/
	void p_uart_send_string(const char* c)
	{
		/* ���͵��ַ������� */
		while (*c != '\0')
		{    
			/* ���͵��ֽ����� */  
			USART_SendData8(USART1, *c++);    // д��һ���ַ���ָ���һ   
			while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));  // �ȴ����ͼĴ����� 
		}
		delay_10ms(1);
		return;
	}

	/* ��ʽ������ */
	void p_at_format_data(u16 max_len)
	{
		for (u16 i = 0; i < max_len; i++)
		{
			if ((g_buff.m_recv_buff[i] == 0x0d)||
				(g_buff.m_recv_buff[i] == 0x0a))
			{
				g_buff.m_recv_buff[i] = 0x00;
			}

		}
		return;
	}
#if 0

	void p_at_clear_recv_buffer(void)
	{
		clear_buff(RX_BUFF);
	}

#endif
	/**
* ***********************************************************************
* @brief	�ȴ����ڷ�������
*
*	@param  u8 wait_ok_err:     ����OK/ERROR��������
*	@param  u8 wait_char:       �����ض��ַ���������
*	@param  u16 wait_ms_max:    �ȴ�ʱ��
*	@param  u8 need_format:     �滻���з�
*
* @retval int:
*
* @attention
* ***********************************************************************
*/

	uint16_t p_at_wait_data(uint8_t wait_ok_err, uint16_t wait_10ms_max, uint8_t need_format)
	{
		uint32_t start_time = GET_10MS_TIMER();  // 10msʱ���

		while ((GET_10MS_TIMER() - start_time) < wait_10ms_max) // �ڸ���ʱ���ڵȴ�
		{
			if ((strstr(s_at_rbuf, "ERROR") != NULL) && (wait_ok_err == TRUE))
			{
				break;
			}
			if ((strstr(s_at_rbuf, "OK\r\n") != NULL) && (wait_ok_err == TRUE))
			{
				break;
			}

			delay_10ms(1); /* ��������ȥ����*/
		}

		g_buff.m_flg_recv_lock = TRUE;  // ԭ���õĺ꣬����Ϊ����

		if (need_format == TRUE)
		{
			p_at_format_data(g_buff.m_recv_index);
		}

		return g_buff.m_recv_index;


	}


	/**
	  * ***********************************************************************
	  * @brief	�ȴ�����ok
	  *
	  *	@param  wait_10ms_max:
	  *
	  * @retval u16:
	  *
	  * @attention	: none
	  * ***********************************************************************
	  */
	u16 p_at_wait_2_ok(u16 wait_10ms_max)
	{
		u32 start_time = GET_10MS_TIMER();
		u8 ok_cnt = 0;
		char* p_buf = (char*)s_at_rbuf;
		/* ���з��Զ����0x00 */
		while ((GET_10MS_TIMER() - start_time) < wait_10ms_max)
		{
			if ((strstr(p_buf, "ERROR") != NULL))
			{
				break;
			}

			if ((p_buf = strstr(p_buf, "OK\r\n")) != NULL)
			{
				if (0 == ok_cnt)
				{
					p_buf += 2;
					++ok_cnt;
					continue;
				}

				break;
			}

			delay_10ms(1);
		}

		g_buff.m_flg_recv_lock = TRUE;

		/*0x0d�滻Ϊ0x00 �����Ϊ��*/
		p_at_format_data(g_buff.m_recv_index);

		return g_buff.m_recv_index;
	}
	
	char* p_at_get_line(char* line, u32 max_len)
	{
		if (line == NULL)  
		{
			return s_at_rbuf; 
		}
		else if ((line < s_at_rbuf) || (line >= &s_at_rbuf[max_len]))  
		{
			return NULL;
		}

		for (u32 i = (line - s_at_rbuf); i < max_len; i++)   
		{
			if (s_at_rbuf[i] != 0x00)  
			{
				continue;
			}
			if (s_at_rbuf[i+1] != 0x00)
			{
				continue;
			}
		

			/* g_m261_rbuffer[i] == 0x00 */   // ���s_at_rbuf[i + 1] != 0x00
			if ((s_at_rbuf[i + 2] != 0x00) && ((i + 2) < max_len))
			{
				return &s_at_rbuf[i + 2];
			}
		}

		/*����NULL���˳�getline����*/
		return NULL;
	}

	/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




