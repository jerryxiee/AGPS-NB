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
  
/*************************     ϵͳͷ�ļ�����     *************************/
#include "stm8l15x_flash.h"

/*************************     ˽��ͷ�ļ�����     *************************/
#include "eeprom.h"
#include "common_cfg.h"

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
* @brief	��ʼ��EEPROM
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
	/*��ʼ��EEPROM*/
	FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);
	FLASH_WaitForLastOperation(FLASH_MemType_Data);
	FLASH_Unlock(FLASH_MemType_Data);
	while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);
	FLASH_WaitForLastOperation(FLASH_MemType_Data);
}

/**
* ***********************************************************************
* @brief	д������
*
*	@param  u8 * cfg_ptr:   g_ccfg_config������Ԫ�ص�ָ��
*	@param  u16 size:       g_ccfg_config������Ԫ�صĴ�С
*
* @retval void:
*
* @attention
* ***********************************************************************
*/
void p_ccfg_write_config(uint8_t *cfg_ptr, uint16_t size)
{
	uint16_t addr = cfg_ptr - (uint8_t*)&g_ccfg_config + EEPROM_BASE_Addr;

	FLASH_Unlock(FLASH_MemType_Data);    //��������EEPROM�洢��

	for (uint16_t i = 0; i < size; i++)
	{
		FLASH_ProgramByte(addr++, *cfg_ptr++);
		FLASH_WaitForLastOperation(FLASH_MemType_Data);
	}

	FLASH_Lock(FLASH_MemType_Data);
}


/**
* ***********************************************************************
* @brief	��ȡ����
*
*	@param  u8 * cfg_ptr:   g_ccfg_config������Ԫ�ص�ָ��
*	@param  u16 size:       g_ccfg_config������Ԫ�صĴ�С
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
/*************************      C++ ���ݶ���      *************************/
#ifdef __cplusplus 
#if __cplusplus 
} 
#endif 
#endif


