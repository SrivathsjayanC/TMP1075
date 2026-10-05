/*
 * TMP01075.h
 *
 *  Created on: Oct 5, 2026
 *      Author: SRIVATHSJAYAN
 */

#ifndef INC_TMP01075_H_
#define INC_TMP01075_H_

#include "main.h"

#define TMP1075_I2C_DELAY HAL_MAX_DELAY

typedef enum {
    TMP1075_REG_TEMP    = 0x00, /**< Temperature Register (Read-Only) */
    TMP1075_REG_CFG     = 0x01, /**< Configuration Register (Read/Write) */
    TMP1075_REG_TLOW    = 0x02, /**< Temperature Low Limit Register (Read/Write) */
    TMP1075_REG_THIGH   = 0x03, /**< Temperature High Limit Register (Read/Write) */
    TMP1075_REG_DEVID   = 0x0F  /**< Device ID Register (Read-Only, Not available on TMP1075N) */
} TMP1075_Reg_t;

typedef struct
{
	uint8_t conv_rate;
	uint8_t faults;
	uint8_t pol;
	uint8_t thermostat_mode;
	uint8_t op_mode;

	uint16_t dev_addr;

	uint8_t __config_reg[2];

} TMP1075_Init_t;
typedef struct
{
	I2C_HandleTypeDef *hi2c;

	GPIO_TypeDef *ALERT_Port;
	uint16_t ALERT_Pin;

	TMP1075_Init_t Init;

} TMP1075_Handle_t;
//@conv_rate
#define TMP1075_CONV_RATE_27_5MS  0x00U
#define TMP1075_CONV_RATE_55MS    0x01U
#define TMP1075_CONV_RATE_110MS   0x02U
#define TMP1075_CONV_RATE_220MS   0x03U
//@faults
#define TMP1075_FAULT_1    0x00U
#define TMP1075_FAULT_2    0x01U
#define TMP1075_FAULT_3    0x02U
#define TMP1075_FAULT_4    0x03U
//@pol
#define TMP1075_POL_LOW    0x00U
#define TMP1075_POL_HIGH   0x01U
//@alert_mode
#define TMP1075_THERMOSTAT_COMP_MODE  0x00U
#define TMP1075_THERMOSTAT_INT_MODE   0x01U
//@op_mode
#define TMP1075_MODE_CONT_CONV   0x00U
#define TMP1075_MODE_SHUTDOWN   0x01U

#define __TMP1075_SD_POS    0x08U
#define __TMP1075_TM_POS    0x09U
#define __TMP1075_POL_POS   0x0AU
#define __TMP1075_FAULT_POS 0x0BU
#define __TMP1075_RATE_POS  0x0DU
#define __TMP1075_OS_POS    0x0FU

#define __TMP1075_TRIGGER_0S (uint8_t)(0x01<<__TMP1075_SD_POS)

#define __TMP1075_BIT_SHIFT  0x04U

#define __TMP1075_GEN_CALL  0x00U
#define __TMP1075_RESET_COMMAND 0x06U;

#define __TMP1075_TEMP_LSB  0.0625f

HAL_StatusTypeDef TMP1075_Init(TMP1075_Handle_t *htmp);
HAL_StatusTypeDef TMP1075_WriteReg(TMP1075_Handle_t *htmp,TMP1075_Reg_t Reg,uint16_t Data);
HAL_StatusTypeDef TMP1075_ReadReg(TMP1075_Handle_t *htmp,TMP1075_Reg_t Tmp1075_Reg,uint16_t *pData);
HAL_StatusTypeDef TMP1075_Reset(TMP1075_Handle_t *htmp);
HAL_StatusTypeDef TMP1075_Trigger_Conv(TMP1075_Handle_t *htmp);
HAL_StatusTypeDef TMP1075_Set_Upper_Limit(TMP1075_Handle_t *htmp, int16_t Val);
HAL_StatusTypeDef TMP1075_Set_Lower_Limit(TMP1075_Handle_t *htmp, int16_t Val);
HAL_StatusTypeDef TMP1075_Get_Temp_C(TMP1075_Handle_t *htmp,float *pData);



#endif /* INC_TMP01075_H_ */
