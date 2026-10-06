https://docs.google.com/spreadsheets/d/1KTAJzv6TCtbVF6Hwy976dH70Ub42ouoYcQ13svK8Ke0/edit?usp=sharing
This sheet above helps in calculating the limit values for LLIM and HLIM registers. (T<sub>HIGH</sub> T<sub>LOW</sub>) Limit

**Test Conditions:**

Board Used: STM32F407G-DISC

I2C MODE - Standard Mode (100KHz)

htmp.Init.dev_addr = 0x48<<1;
htmp.Init.thermostat_mode = TMP1075_THERMOSTAT_INT_MODE;
htmp.Init.conv_rate = TMP1075_CONV_RATE_27_5MS;
htmp.Init.faults = TMP1075_FAULT_1;
htmp.Init.op_mode = TMP1075_MODE_CONT_CONV;
htmp.Init.pol = TMP1075_POL_HIGH;
TMP1075_Init(&htmp);
  
uint16_t val1 = 0x3C0; // 60°C
uint16_t val2 = 0x320; // 50°C
TMP1075_Set_Upper_Limit(&htmp, val1);
TMP1075_Set_Lower_Limit(&htmp, val2);

**Other Configuration Tested**

I2C MODE - Fast Mode (400KHz)

htmp.Init.thermostat_mode = TMP1075_THERMOSTAT_COMP_MODE;

htmp.Init.conv_rate = TMP1075_CONV_RATE_220MS;

htmp.Init.faults = TMP1075_FAULT_4;

htmp.Init.pol = TMP1075_POL_LOW;

htmp.Init.op_mode = TMP1075_MODE_SHUTDOWN;
