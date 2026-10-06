/*
 * TMP01075.c
 *
 *  Created on: Oct 5, 2026
 *      Author: SRIVATHSJAYAN
 */
#include "TMP01075.h"

static HAL_StatusTypeDef TMP1075_WriteReg(TMP1075_Handle_t *htmp,TMP1075_Reg_t Reg,uint16_t Data);
/**
 * @brief  Initializes the TMP1075 temperature sensor with the specified configuration.
 *
 * @details This function validates the user-defined initialization parameters in the handle,
 *          performs a soft reset on the TMP1075 device, and writes the configured settings
 *          to the device's Configuration Register (CFG). It also caches the MSB of the
 *          configuration register within the handle for future driver use.
 *
 * @param  htmp Pointer to a TMP1075_Handle_t structure that contains
 *              the I2C handle, device address, and initialization parameters
 *              for the TMP1075 device.
 *
 * @retval HAL_StatusTypeDef HAL status:
 *         - HAL_OK:      Initialization was successful.
 *         - HAL_ERROR:   Invalid parameter(s), NULL I2C handle, invalid device address,
 *                        or an I2C communication failure occurred during initialization.
 *         - HAL_BUSY:    The I2C peripheral is busy and cannot process the reset or write requests.
 *         - HAL_TIMEOUT: An I2C timeout occurred during the reset or configuration register write.
 */
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
			(htmp->Init.thermostat_mode << __TMP1075_TM_POS) |
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
	htmp->Init.__config_reg = (uint8_t)(reg>>8U);

	return status;
}
/**
 * @brief  Writes a 16-bit data value to a specific TMP1075 register.
 *
 * @details This internal helper function separates the 16-bit data into two
 *          8-bit bytes (MSB first, LSB second) and transmits them via I2C
 *          using the STM32 HAL memory write API.
 *
 * @param  htmp Pointer to a TMP1075_Handle_t structure that contains
 *              the configuration information and I2C handle for the TMP1075 device.
 * @param  Reg  The target register address to write to (of type TMP1075_Reg_t).
 * @param  Data The 16-bit data to be written into the specified register.
 *
 * @retval HAL_StatusTypeDef HAL status of the I2C transaction:
 *         - HAL_OK:      Data was written successfully.
 *         - HAL_ERROR:   An error occurred during the I2C transmission (e.g., NACK or parameter error).
 *         - HAL_BUSY:    The I2C peripheral is currently locked and cannot accept a new request.
 *         - HAL_TIMEOUT: The I2C transaction exceeded the specified maximum delay (TMP1075_I2C_DELAY).
 */
static HAL_StatusTypeDef TMP1075_WriteReg(TMP1075_Handle_t *htmp,TMP1075_Reg_t Reg,uint16_t Data)
{
	uint8_t tx[2];

	tx[0] = (uint8_t)(Data>>8U);
	tx[1] = (uint8_t)Data;

	return HAL_I2C_Mem_Write(htmp->hi2c, htmp->Init.dev_addr, Reg, 1, tx, 2, TMP1075_I2C_DELAY);
}
/**
 * @brief  Reads a 16-bit data value from a specific TMP1075 register.
 *
 * @details This function validates the input pointers and device address,
 *          performs an I2C memory read to receive 2 bytes of data, and
 *          combines the received bytes (MSB first, LSB second) into the
 *          provided 16-bit output variable.
 *
 * @param  htmp        Pointer to a TMP1075_Handle_t structure that contains
 *                     the configuration information and I2C handle for the device.
 * @param  Tmp1075_Reg The target register address to read from (of type TMP1075_Reg_t).
 * @param  pData       Pointer to a uint16_t variable where the read data will be stored.
 *
 * @retval HAL_StatusTypeDef HAL status of the I2C transaction:
 *         - HAL_OK:      Data was read successfully.
 *         - HAL_ERROR:   Invalid parameter(s) (e.g., NULL pointer or address of 0), NACK,
 *                        or an I2C communication error occurred.
 *         - HAL_BUSY:    The I2C peripheral is currently locked and cannot accept a new request.
 *         - HAL_TIMEOUT: The I2C read transaction exceeded the specified maximum delay (TMP1075_I2C_DELAY).
 */
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
/**
 * @brief  Performs a software reset on the TMP1075 device.
 *
 * @details This function validates the I2C handle and transmits an I2C General
 *          Call Reset command. This resets the TMP1075's internal registers to
 *          their power-up default values.
 *
 * @param  htmp Pointer to a TMP1075_Handle_t structure that contains
 *              the configuration information and I2C handle for the device.
 *
 * @retval HAL_StatusTypeDef HAL status of the I2C transaction:
 *         - HAL_OK:      The reset command was transmitted successfully.
 *         - HAL_ERROR:   Invalid parameter (e.g., NULL I2C handle) or an I2C
 *                        communication error occurred (e.g., NACK).
 *         - HAL_BUSY:    The I2C peripheral is currently locked and cannot accept a new request.
 *         - HAL_TIMEOUT: The I2C transmit transaction exceeded the specified maximum delay (TMP1075_I2C_DELAY).
 */
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
/**
 * @brief  Triggers a single-shot temperature conversion on the TMP1075 device.
 *
 * @details This function validates the I2C handle and device address, sets the
 *          one-shot trigger bit in the cached configuration register, and transmits
 *          it via a 1-byte I2C memory write to the device's Configuration Register (CFG).
 *          This is typically used when the device is operating in shutdown mode.
 *
 * @param  htmp Pointer to a TMP1075_Handle_t structure that contains
 *              the configuration information and I2C handle for the device.
 *
 * @retval HAL_StatusTypeDef HAL status of the I2C transaction:
 *         - HAL_OK:      The single-shot conversion trigger was transmitted successfully.
 *         - HAL_ERROR:   Invalid parameter(s) (e.g., NULL I2C handle or address of 0), NACK,
 *                        or an I2C communication error occurred.
 *         - HAL_BUSY:    The I2C peripheral is currently locked and cannot accept a new request.
 *         - HAL_TIMEOUT: The I2C write transaction exceeded the specified maximum delay (TMP1075_I2C_DELAY).
 */
HAL_StatusTypeDef TMP1075_Trigger_Conv(TMP1075_Handle_t *htmp)
{
	if(htmp->Init.dev_addr == 0 || htmp->hi2c == NULL)
	{
		return HAL_ERROR;
	}
	htmp->Init.__config_reg |= __TMP1075_TRIGGER_0S;
	return HAL_I2C_Mem_Write(htmp->hi2c, htmp->Init.dev_addr, TMP1075_REG_CFG, 1, (htmp->Init.__config_reg), 1, TMP1075_I2C_DELAY);
}
/**
 * @brief  Sets the upper temperature limit (T_HIGH) for the TMP1075 device.
 *
 * @details This function validates the I2C handle, device address, and ensures
 *          that the provided limit value does not exceed the 12-bit maximum (0x0FFF).
 *          It then bit-shifts the value to align with the device's data format
 *          and writes it to the T_HIGH register. This threshold is utilized by
 *          the device's thermostat and alert functionalities.
 *
 * @param  htmp Pointer to a TMP1075_Handle_t structure that contains
 *              the configuration information and I2C handle for the device.
 * @param  Val  The 12-bit temperature value (maximum 0x0FFF) to be set as the
 *              upper limit threshold.
 *
 * @retval HAL_StatusTypeDef HAL status of the I2C transaction:
 *         - HAL_OK:      The upper temperature limit was written successfully.
 *         - HAL_ERROR:   Invalid parameter(s) (e.g., NULL I2C handle, address of 0,
 *                        or value exceeds 0x0FFF), NACK, or an I2C communication error.
 *         - HAL_BUSY:    The I2C peripheral is currently locked and cannot accept a new request.
 *         - HAL_TIMEOUT: The I2C write transaction exceeded the specified maximum delay.
 */
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
/**
 * @brief  Sets the lower temperature limit (T_LOW) for the TMP1075 device.
 *
 * @details This function validates the I2C handle, device address, and ensures
 *          that the provided limit value does not exceed the 12-bit maximum (0x0FFF).
 *          It then bit-shifts the value to align with the device's data format
 *          and writes it to the T_LOW register. This threshold is utilized by
 *          the device's thermostat and alert functionalities.
 *
 * @param  htmp Pointer to a TMP1075_Handle_t structure that contains
 *              the configuration information and I2C handle for the device.
 * @param  Val  The 12-bit temperature value (maximum 0x0FFF) to be set as the
 *              lower limit threshold.
 *
 * @retval HAL_StatusTypeDef HAL status of the I2C transaction:
 *         - HAL_OK:      The lower temperature limit was written successfully.
 *         - HAL_ERROR:   Invalid parameter(s) (e.g., NULL I2C handle, address of 0,
 *                        or value exceeds 0x0FFF), NACK, or an I2C communication error.
 *         - HAL_BUSY:    The I2C peripheral is currently locked and cannot accept a new request.
 *         - HAL_TIMEOUT: The I2C write transaction exceeded the specified maximum delay.
 */
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
/**
 * @brief  Reads the current temperature from the TMP1075 device and converts it to Celsius.
 *
 * @details This function validates the input pointers and device address, reads the
 *          raw 16-bit temperature data from the TMP1075 temperature register (TEMP),
 *          shifts the raw data to extract the valid 12-bit result, and applies the
 *          LSB conversion factor to calculate the actual temperature in degrees Celsius
 *          as a floating-point value.
 *
 * @param  htmp  Pointer to a TMP1075_Handle_t structure that contains
 *               the configuration information and I2C handle for the device.
 * @param  pData Pointer to a float variable where the calculated temperature
 *               (in degrees Celsius) will be stored.
 *
 * @retval HAL_StatusTypeDef HAL status of the I2C transaction:
 *         - HAL_OK:      The temperature was read and converted successfully.
 *         - HAL_ERROR:   Invalid parameter(s) (e.g., NULL pointer or address of 0), NACK,
 *                        or an I2C communication error occurred during the read operation.
 *         - HAL_BUSY:    The I2C peripheral is currently locked and cannot accept a new request.
 *         - HAL_TIMEOUT: The I2C read transaction exceeded the specified maximum delay.
 */
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
