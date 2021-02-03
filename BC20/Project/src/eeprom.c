/**
  *************************************************************************
  * @file    eeprom.c
  * @author  Monkey.Huaqian
  * @version V1.0.0
  * @date    2018/05/19 18:05
  * @brief   
  *************************************************************************
  * @attention
  *
  * <h2><center>&Copyright(c) 2014-2020 Xiamen HuaQian Studio</center></h2>
  *
  *************************************************************************
  */
  
/*************************     系统头文件包含     *************************/
#include "stm8l15x_flash.h"

/*************************     私有头文件包含     *************************/
#include "eeprom.h"
#include "common_cfg.h"

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
* @brief	初始化EEPROM
*
*	@param  :
*
* @retval void:
*
* @attention	: none
* ***********************************************************************
*/
void p_init_eeprom(void)
{
	/*初始化EEPROM*/
	FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);
	FLASH_WaitForLastOperation(FLASH_MemType_Data);
	FLASH_Unlock(FLASH_MemType_Data);
	while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);
	FLASH_WaitForLastOperation(FLASH_MemType_Data);
}

/**
* ***********************************************************************
* @brief	写入配置
*
*	@param  u8 * cfg_ptr:   g_ccfg_config的配置元素的指针
*	@param  u16 size:       g_ccfg_config的配置元素的大小
*
* @retval void:
*
* @attention
* ***********************************************************************
*/
void p_ccfg_write_config(uint8_t *cfg_ptr, uint16_t size)
{
	uint16_t addr = cfg_ptr - (uint8_t*)&g_ccfg_config + EEPROM_BASE_Addr;

	FLASH_Unlock(FLASH_MemType_Data);    //解锁数据EEPROM存储器

	for (uint16_t i = 0; i < size; i++)
	{
		FLASH_ProgramByte(addr++, *cfg_ptr++);
		FLASH_WaitForLastOperation(FLASH_MemType_Data);
	}

	FLASH_Lock(FLASH_MemType_Data);
}


/**
* ***********************************************************************
* @brief	读取配置
*
*	@param  u8 * cfg_ptr:   g_ccfg_config的配置元素的指针
*	@param  u16 size:       g_ccfg_config的配置元素的大小
*
* @retval void:
*
* @attention
* ***********************************************************************
*/
void p_ccfg_read_config(uint8_t *cfg_ptr, uint16_t size)
{
	uint16_t addr = cfg_ptr - (uint8_t*)&g_ccfg_config + EEPROM_BASE_Addr;

	for (uint16_t i = 0; i < size; i++)
	{
		*cfg_ptr++ = FLASH_ReadByte(addr++);
	}
}
/*************************      C++ 兼容定义      *************************/
#ifdef __cplusplus 
#if __cplusplus 
} 
#endif 
#endif


