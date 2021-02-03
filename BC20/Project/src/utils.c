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
  
/*************************     系统头文件包含     *************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*************************     私有头文件包含     *************************/
#include "utils.h"

/*************************      导入命名空间      *************************/


/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
extern "C"{ 
#endif 
#endif

/*************************        全局枚举        *************************/

/*************************       全局宏定义       *************************/

/*************************     全局结构体定义     *************************/

/*************************      全局变量定义      *************************/

/*************************        函数声明        *************************/

/*************************        函数实现        *************************/

/**
  * ***********************************************************************
  * @brief	字符串分割函数
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
	static char* src = NULL;		// src 是一个指针，赋值为空
	char* tmp = NULL;				// tmp 是一个指针，赋值为空
	int8_t ret_val = FALSE;

	*output = NULL;				    // *output是一个指针，赋值为空

	/* 判断是否重新传入字符串 */
	if (NULL != str)             // 如果传入的str指针不为空
	{
		src = str;              // 将str 赋值给 src，此时src和传入的str指向同一个地址
	}

	/* 源字符串必须不为空 */
	if (NULL == src)
	{
		goto out;
	}

	/* 未找到目标分割字符 */
	if ((tmp = strchr(src, ch)) == NULL)    // 获取传入字符串分隔符的首地址，如果获取不到
	{
		/* 判断是否有尾巴 */
		if (NULL != src)					// 此时的src应该没有改变，仍然指向传入字符串的首地址
		{
			*output = src;					// *output与src指向同一块地址
			ret_val = TRUE;
			src = NULL;						// src 重新指向空					
		}
		goto out;							// 退出函数，则 *output指针指向第一个分割符号前面的字符串
	}

	*output = src;							
	src = tmp + 1;
	*tmp = 0;

	/* 判断是否存在连续分隔符 ",,,," */
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
  * @brief	检查数据是否为数字
  *			
  *	@param  num: 指向需要检测数据的指针
  *	@param  size: 需要检测的数据大小
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




/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
} 
#endif 
#endif


