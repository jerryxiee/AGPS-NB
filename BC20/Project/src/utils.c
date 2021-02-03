/**
  *************************************************************************
  * @file    utils.c
  * @author  Monkey.Huaqian
  * @version V1.0.0
  * @date    2018/05/13 17:24
  * @brief   
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */
  
/*************************     ϵͳͷ�ļ�����     *************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*************************     ˽��ͷ�ļ�����     *************************/
#include "utils.h"

/*************************      ���������ռ�      *************************/


/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C"{ 
#endif 
#endif

/*************************        ȫ��ö��        *************************/

/*************************       ȫ�ֺ궨��       *************************/

/*************************     ȫ�ֽṹ�嶨��     *************************/

/*************************      ȫ�ֱ�������      *************************/

/*************************        ��������        *************************/

/*************************        ����ʵ��        *************************/

/**
  * ***********************************************************************
  * @brief	�ַ����ָ��
  *			
  *	@param  str: 
  *	@param  output: 
  *	@param  ch: 
  *
  * @retval int8_t: 
  *
  * @attention	: none
  * ***********************************************************************
  */
int8_t custom_strtok(char* str, char** output, char ch)
{
	static char* src = NULL;		// src ��һ��ָ�룬��ֵΪ��
	char* tmp = NULL;				// tmp ��һ��ָ�룬��ֵΪ��
	int8_t ret_val = FALSE;

	*output = NULL;				    // *output��һ��ָ�룬��ֵΪ��

	/* �ж��Ƿ����´����ַ��� */
	if (NULL != str)             // ��������strָ�벻Ϊ��
	{
		src = str;              // ��str ��ֵ�� src����ʱsrc�ʹ����strָ��ͬһ����ַ
	}

	/* Դ�ַ������벻Ϊ�� */
	if (NULL == src)
	{
		goto out;
	}

	/* δ�ҵ�Ŀ��ָ��ַ� */
	if ((tmp = strchr(src, ch)) == NULL)    // ��ȡ�����ַ����ָ������׵�ַ�������ȡ����
	{
		/* �ж��Ƿ���β�� */
		if (NULL != src)					// ��ʱ��srcӦ��û�иı䣬��Ȼָ�����ַ������׵�ַ
		{
			*output = src;					// *output��srcָ��ͬһ���ַ
			ret_val = TRUE;
			src = NULL;						// src ����ָ���					
		}
		goto out;							// �˳��������� *outputָ��ָ���һ���ָ����ǰ����ַ���
	}

	*output = src;							
	src = tmp + 1;
	*tmp = 0;

	/* �ж��Ƿ���������ָ��� ",,,," */
	if (0 == **output)
	{
		ret_val = FALSE;
	}
	else {
		ret_val = TRUE;
	}

out:
	return ret_val;
}

uint8_t itohex(uint8_t aHex)
{
	if (/* (aHex >= 0) && */(aHex <= 9))
		aHex += 0x30;
	else if ((aHex >= 10) && (aHex <= 15))//A-F
		aHex += 0x37;
	else aHex = 0xff;
	return aHex;
}

uint8_t hextoi(uint8_t hexch)
{
	if ((hexch >= '0') && (hexch <= '9'))
		return hexch - '0';
	else if ((hexch >= 'A') && (hexch <= 'F'))
		return hexch - 'A' + 10;
	else if ((hexch >= 'a') && (hexch <= 'f'))
		return hexch - 'a' + 10;
	else
		return -1;
}

/**
  * ***********************************************************************
  * @brief	��������Ƿ�Ϊ����
  *			
  *	@param  num: ָ����Ҫ������ݵ�ָ��
  *	@param  size: ��Ҫ�������ݴ�С
  *
  * @retval uint8_t: 
  *
  * @attention	: none
  * ***********************************************************************
  */
uint8_t is_all_num(uint8_t* num, uint8_t size)
{
	for (uint8_t i = 0; i < size; i++)
	{
		if (num[i] <= '0' && num[i] >= '9')
		{
			return FALSE;
		}
	}

	return TRUE;
}




/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
} 
#endif 
#endif


