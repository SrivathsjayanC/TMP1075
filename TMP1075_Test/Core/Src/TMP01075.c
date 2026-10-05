/*
 * TMP01075.c
 *
 *  Created on: Oct 5, 2026
 *      Author: SRIVATHSJAYAN
 */
#include "TMP01075.h"

HAL_StatusTypeDef TMP1075_Init(TMP1075_Handle_t *htmp)
{
	if(htmp->hi2c == NULL || htmp->Init.dev_addr == 0)
	{
		return HAL_ERROR;
	}
	if(htmp->Init.thermostat_mode > TMP1075_THERMOSTAT_INT_MODE ||
			htmp->Init.conv_rate > TMP1075_CONV_RATE_220MS ||
			htmp->Init.faults > TMP1075_FAULT_4 ||
			htmp->Init.op_mode > TMP1075_MODE_SHUTDOWN ||
			htmp->Init.pol > TMP1075_POL_HIGH)
	{
		return HAL_ERROR;
	}
	uint16_t reg;
	HAL_StatusTypeDef status;
	reg = ((htmp->Init.op_mode << __TMP1075_SD_POS) |
			(htmp->Init.alert_mode << __TMP1075_TM_POS) |
			(htmp->Init.pol << __TMP1075_POL_POS) |
			(htmp->Init.faults << __TMP1075_FAULT_POS) |
			(htmp->Init.conv_rate << __TMP1075_RATE_POS));

	status = TMP1075_Reset(htmp);
	if(status != HAL_OK)
	{
		return status;
	}
	status = TMP1075_WriteReg(htmp, TMP1075_REG_CFG, reg);
	if(status != HAL_OK)
	{
		return status;
	}
	htmp->Init.__config_reg[0] = (uint8_t)(reg>>8U);
	htmp->Init.__config_reg[1] = (uint8_t)reg;

	return status;
}
HAL_StatusTypeDef TMP1075_WriteReg(TMP1075_Handle_t *htmp,TMP1075_Reg_t Reg,uint16_t Data)
{
	uint8_t tx[2];

	tx[0] = (uint8_t)(Data>>8U);
	tx[1] = (uint8_t)Data;

	return HAL_I2C_Mem_Write(htmp->hi2c, htmp->Init.dev_addr, Reg, 1, tx, 2, TMP1075_I2C_DELAY);
}
HAL_StatusTypeDef TMP1075_ReadReg(TMP1075_Handle_t *htmp,TMP1075_Reg_t Tmp1075_Reg,uint16_t *pData)
{
	if(htmp->hi2c == NULL || htmp->Init.dev_addr == 0 || pData == NULL)
	{
		return HAL_ERROR;
	}
	HAL_StatusTypeDef status;
	uint8_t rx[2];

	status = HAL_I2C_Mem_Read(htmp->hi2c, htmp->Init.dev_addr, Tmp1075_Reg, 1, rx, 2, TMP1075_I2C_DELAY);
	if(status != HAL_OK)
	{
		return status;
	}

	*pData = (uint16_t)((uint16_t)rx[0]<<8 | (uint16_t)rx[1]);

	return status;
}
HAL_StatusTypeDef TMP1075_Reset(TMP1075_Handle_t *htmp)
{
	if(htmp->hi2c == NULL)
	{
		return HAL_ERROR;
	}
	uint8_t tx[2];
	tx[0] = __TMP1075_GEN_CALL;
	tx[1] = __TMP1075_RESET_COMMAND;

	return HAL_I2C_Master_Transmit(htmp->hi2c,tx[0],&tx[1], 1, TMP1075_I2C_DELAY);
}
HAL_StatusTypeDef TMP1075_Trigger_Conv(TMP1075_Handle_t *htmp)
{
	if(htmp->Init.dev_addr == 0 || htmp->hi2c == NULL)
	{
		return HAL_ERROR;
	}
	htmp->Init.__config_reg[0] |= __TMP1075_TRIGGER_0S;
	return HAL_I2C_Mem_Write(htmp->hi2c, htmp->Init.dev_addr, TMP1075_REG_CFG, 1, (htmp->Init.__config_reg), 2, TMP1075_I2C_DELAY);
}

HAL_StatusTypeDef TMP1075_Set_Upper_Limit(TMP1075_Handle_t *htmp, int16_t Val)
{
	if(htmp->Init.dev_addr == 0 || htmp->hi2c == NULL || Val > 0x0FFF)
	{
		return HAL_ERROR;
	}
	uint16_t limit;
	limit = Val << __TMP1075_BIT_SHIFT;
	return TMP1075_WriteReg(htmp,TMP1075_REG_THIGH, limit);
}
HAL_StatusTypeDef TMP1075_Set_Lower_Limit(TMP1075_Handle_t *htmp, int16_t Val)
{
	if(htmp->Init.dev_addr == 0 || htmp->hi2c == NULL || Val > 0x0FFF)
	{
		return HAL_ERROR;
	}
	uint16_t limit;
	limit = Val << __TMP1075_BIT_SHIFT;
	return TMP1075_WriteReg(htmp,TMP1075_REG_TLOW, limit);
}
HAL_StatusTypeDef TMP1075_Get_Temp_C(TMP1075_Handle_t *htmp,float *pData)
{
	if(htmp->Init.dev_addr == 0 || htmp->hi2c == NULL || pData == NULL)
	{
		return HAL_ERROR;
	}
	HAL_StatusTypeDef status;
	uint16_t temp_raw;
	status = TMP1075_ReadReg(htmp, TMP1075_REG_TEMP, &temp_raw);

	*pData = (float)((temp_raw>>__TMP1075_BIT_SHIFT)*__TMP1075_TEMP_LSB);

	return status;
}
