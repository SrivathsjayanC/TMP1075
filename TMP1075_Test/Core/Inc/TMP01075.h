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
    TMP1075_REG_DEVID   = 0x0F  /**< Device ID Register (Read-Only) */
} TMP1075_Reg_t;
/**
 * @brief  TMP1075 Initialization Structure definition.
 *
 * @details This structure holds the configuration parameters required to initialize
 *          the TMP1075 device. The parameters (except the device address and internal
 *          cache) map directly to the bitfields in the device's Configuration Register (CFG).
 *
 * @param  conv_rate       Specifies the temperature conversion rate (e.g., 27.5ms, 55ms, 110ms, or 220ms).
 * @param  faults          Specifies the number of consecutive out-of-limit faults required to trigger an alert.
 * @param  pol             Specifies the polarity of the ALERT pin (Active Low or Active High).
 * @param  thermostat_mode Specifies the thermostat operation mode (Comparator mode or Interrupt mode).
 * @param  op_mode         Specifies the device operation mode (Continuous Conversion or Shutdown/One-Shot mode).
 * @param  dev_addr        Specifies the I2C device address of the targeted TMP1075 sensor.
 * @param  __config_reg    Internal driver variable. Caches the most significant byte (MSB)
 *                         of the configuration register for internal operations. (Do not modify).
 */
typedef struct
{
	uint8_t conv_rate;
	uint8_t faults;
	uint8_t pol;
	uint8_t thermostat_mode;
	uint8_t op_mode;

	uint16_t dev_addr;

	uint8_t __config_reg;

} TMP1075_Init_t;
/**
 * @brief  TMP1075 Handle Structure definition.
 *
 * @details This structure contains the hardware communication handle, GPIO pin
 *          mappings for the hardware alert functionality, and the initialization
 *          configuration for a specific TMP1075 device instance.
 *
 * @param  hi2c       Pointer to an I2C_HandleTypeDef structure that contains
 *                    the configuration information for the I2C bus communicating with the sensor.
 * @param  ALERT_Port Pointer to the GPIO_TypeDef structure representing the microcontroller
 *                    port connected to the TMP1075 ALERT pin.
 * @param  ALERT_Pin  Specifies the microcontroller GPIO pin connected to the TMP1075 ALERT pin.
 * @param  Init       TMP1075_Init_t structure that contains the initialization
 *                    and configuration parameters for the device.
 */
typedef struct
{
	I2C_HandleTypeDef *hi2c;

	GPIO_TypeDef *ALERT_Port;
	uint16_t ALERT_Pin;

	TMP1075_Init_t Init;

} TMP1075_Handle_t;
/**
 * @defgroup TMP1075_Conversion_Rates TMP1075 Conversion Rates
 * @brief    Macros defining the temperature conversion rate settings.
 * @details  These values specify the frequency at which the TMP1075 updates
 *           the temperature register in continuous conversion mode.
 * @{
 */
#define TMP1075_CONV_RATE_27_5MS  0x00U  /*!< Conversion time of 27.5 milliseconds */
#define TMP1075_CONV_RATE_55MS    0x01U  /*!< Conversion time of 55 milliseconds */
#define TMP1075_CONV_RATE_110MS   0x02U  /*!< Conversion time of 110 milliseconds */
#define TMP1075_CONV_RATE_220MS   0x03U  /*!< Conversion time of 220 milliseconds */
/**
 * @}
 */

/**
 * @defgroup TMP1075_Fault_Queue TMP1075 Fault Queue
 * @brief    Macros defining the fault queue configuration.
 * @details  These values specify the number of consecutive out-of-limit temperature
 *           measurements required to trigger the ALERT pin.
 * @{
 */
#define TMP1075_FAULT_1    0x00U  /*!< 1 consecutive fault required to trigger an alert */
#define TMP1075_FAULT_2    0x01U  /*!< 2 consecutive faults required to trigger an alert */
#define TMP1075_FAULT_3    0x02U  /*!< 3 consecutive faults required to trigger an alert */
#define TMP1075_FAULT_4    0x03U  /*!< 4 consecutive faults required to trigger an alert */
/**
 * @}
 */

/**
 * @defgroup TMP1075_Polarity TMP1075 ALERT Pin Polarity
 * @brief    Macros defining the active state polarity of the ALERT pin.
 * @details  These values configure whether the ALERT pin goes low or high when triggered.
 * @{
 */
#define TMP1075_POL_LOW    0x00U  /*!< ALERT pin is Active Low (default) */
#define TMP1075_POL_HIGH   0x01U  /*!< ALERT pin is Active High */
/**
 * @}
 */

/**
 * @defgroup TMP1075_Thermostat_Mode TMP1075 Thermostat Mode
 * @brief    Macros defining the behavior of the thermostat (ALERT) functionality.
 * @details  Selects between Comparator mode (pin acts like a thermostat) or
 *           Interrupt mode (pin acts as an interrupt flag requiring clearance).
 * @{
 */
#define TMP1075_THERMOSTAT_COMP_MODE  0x00U  /*!< Comparator mode: ALERT asserts/deasserts automatically based on limits */
#define TMP1075_THERMOSTAT_INT_MODE   0x01U  /*!< Interrupt mode: ALERT asserts and requires a register read to clear */
/**
 * @}
 */

/**
 * @defgroup TMP1075_Operation_Mode TMP1075 Operation Mode
 * @brief    Macros defining the power and conversion mode of the device.
 * @details  Configures the device to either measure temperature continuously or
 *           enter a low-power shutdown state where single-shot conversions are possible.
 * @{
 */
#define TMP1075_MODE_CONT_CONV   0x00U  /*!< Continuous conversion mode (normal operation) */
#define TMP1075_MODE_SHUTDOWN    0x01U  /*!< Shutdown mode (low power), enables one-shot conversions */
/**
 * @}
 */

// Internal Macros
#define __TMP1075_SD_POS    0x08U
#define __TMP1075_TM_POS    0x09U
#define __TMP1075_POL_POS   0x0AU
#define __TMP1075_FAULT_POS 0x0BU
#define __TMP1075_RATE_POS  0x0DU
#define __TMP1075_OS_POS    0x0FU
#define __TMP1075_OS_POS_IN_8BITS 0x07U

#define __TMP1075_TRIGGER_0S (uint8_t)(0x01<<__TMP1075_OS_POS_IN_8BITS)

#define __TMP1075_BIT_SHIFT  0x04U

#define __TMP1075_GEN_CALL  0x00U
#define __TMP1075_RESET_COMMAND 0x06U;

#define __TMP1075_TEMP_LSB  0.0625f

HAL_StatusTypeDef TMP1075_Init(TMP1075_Handle_t *htmp);
HAL_StatusTypeDef TMP1075_ReadReg(TMP1075_Handle_t *htmp,TMP1075_Reg_t Tmp1075_Reg,uint16_t *pData);
HAL_StatusTypeDef TMP1075_Reset(TMP1075_Handle_t *htmp);
HAL_StatusTypeDef TMP1075_Trigger_Conv(TMP1075_Handle_t *htmp);
HAL_StatusTypeDef TMP1075_Set_Upper_Limit(TMP1075_Handle_t *htmp, int16_t Val);
HAL_StatusTypeDef TMP1075_Set_Lower_Limit(TMP1075_Handle_t *htmp, int16_t Val);
HAL_StatusTypeDef TMP1075_Get_Temp_C(TMP1075_Handle_t *htmp,float *pData);



#endif /* INC_TMP01075_H_ */
