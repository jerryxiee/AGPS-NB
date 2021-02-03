
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

  /*************************     系统头文件包含     *************************/
#include "stm8l15x.h"
#include <string.h>
#include "stm8l15x_usart.h"
/*************************     私有头文件包含     *************************/

#include "AT.h"


/*************************      导入命名空间      *************************/


/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C" {
#endif 
#endif

	/*************************        全局枚举        *************************/

	/*************************       全局宏定义       *************************/

	/*************************     全局结构体定义     *************************/

	/*************************      全局变量定义      *************************/

	char* const s_at_rbuf = (char*)g_buff.m_recv_buff;


	/*************************        函数声明        *************************/

	/*************************        函数实现        *************************/
	void p_uart_send_string(const char* c)
	{
		/* 发送到字符串结束 */
		while (*c != '\0')
		{    
			/* 发送单字节数据 */  
			USART_SendData8(USART1, *c++);    // 写入一个字符，指针加一   
			while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));  // 等待发送寄存器空 
		}
		delay_10ms(1);
		return;
	}

	/* 格式化数据 */
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
* @brief	等待串口返回数据
*
*	@param  u8 wait_ok_err:     遇到OK/ERROR立即返回
*	@param  u8 wait_char:       遇到特定字符立即返回
*	@param  u16 wait_ms_max:    等待时间
*	@param  u8 need_format:     替换换行符
*
* @retval int:
*
* @attention
* ***********************************************************************
*/

	uint16_t p_at_wait_data(uint8_t wait_ok_err, uint16_t wait_10ms_max, uint8_t need_format)
	{
		uint32_t start_time = GET_10MS_TIMER();  // 10ms时间戳

		while ((GET_10MS_TIMER() - start_time) < wait_10ms_max) // 在给定时间内等待
		{
			if ((strstr(s_at_rbuf, "ERROR") != NULL) && (wait_ok_err == TRUE))
			{
				break;
			}
			if ((strstr(s_at_rbuf, "OK\r\n") != NULL) && (wait_ok_err == TRUE))
			{
				break;
			}

			delay_10ms(1); /* 这条可以去掉？*/
		}

		g_buff.m_flg_recv_lock = TRUE;  // 原来用的宏，我认为错误

		if (need_format == TRUE)
		{
			p_at_format_data(g_buff.m_recv_index);
		}

		return g_buff.m_recv_index;


	}


	/**
	  * ***********************************************************************
	  * @brief	等待两个ok
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
		/* 换行符自动变成0x00 */
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

		/*0x0d替换为0x00 ：拆分为行*/
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
		

			/* g_m261_rbuffer[i] == 0x00 */   // 如果s_at_rbuf[i + 1] != 0x00
			if ((s_at_rbuf[i + 2] != 0x00) && ((i + 2) < max_len))
			{
				return &s_at_rbuf[i + 2];
			}
		}

		/*返回NULL，退出getline流程*/
		return NULL;
	}

	/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
}
#endif 
#endif




