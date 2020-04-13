#ifndef _BMA250_H
#define _BMA250_H 

#include "type1.h" 
#include "stdbool.h" 

extern unsigned char BMA250_Ang_x;
extern unsigned char BMA250_Ang_z;
extern unsigned char BMA250_Ang_x_buf;
extern unsigned char BMA250_Ang_z_buf;

#define BMA250_DEVICE							0x30

#define BMA2x2_CHIP_ID_REG                      0x00
#define BMA2x2_X_AXIS_LSB_REG                   0x02
#define BMA2x2_X_AXIS_MSB_REG                   0x03
#define BMA2x2_Y_AXIS_LSB_REG                   0x04
#define BMA2x2_Y_AXIS_MSB_REG                   0x05
#define BMA2x2_Z_AXIS_LSB_REG                   0x06
#define BMA2x2_Z_AXIS_MSB_REG                   0x07
#define BMA2x2_TEMP_REG							0x08
#define BMA2x2_STAT1_REG						0x09
#define BMA2x2_STAT2_REG						0x0A
#define BMA2x2_STAT_TAP_SLOPE_REG				0x0B
#define BMA2x2_STAT_ORIENT_HIGH_REG				0x0C
#define BMA2x2_STAT_FIFO_REG					0x0E
#define BMA2x2_RANGE_SELECT_REG                 0x0F
#define BMA2x2_BW_SELECT_REG                    0x10
#define BMA2x2_MODE_CTRL_REG                    0x11
#define BMA2x2_LOW_NOISE_CTRL_REG               0x12
#define BMA2x2_DATA_CTRL_REG                    0x13
#define BMA2x2_RST_REG                          0x14
#define BMA2x2_INTR_ENABLE1_REG                 0x16
#define BMA2x2_INTR_ENABLE2_REG                 0x17
#define BMA2x2_INTR_SLOW_NO_MOTION_REG          0x18
#define BMA2x2_INTR1_PAD_SELECT_REG             0x19
#define BMA2x2_INTR_DATA_SELECT_REG             0x1A
#define BMA2x2_INTR2_PAD_SELECT_REG             0x1B
#define BMA2x2_INTR_SOURCE_REG                  0x1E
#define BMA2x2_INTR_SET_REG                     0x20
#define BMA2x2_INTR_CTRL_REG                    0x21
#define BMA2x2_LOW_DURN_REG                     0x22
#define BMA2x2_LOW_THRES_REG                    0x23
#define BMA2x2_LOW_HIGH_HYST_REG                0x24
#define BMA2x2_HIGH_DURN_REG                    0x25
#define BMA2x2_HIGH_THRES_REG                   0x26
#define BMA2x2_SLOPE_DURN_REG                   0x27
#define BMA2x2_SLOPE_THRES_REG                  0x28
#define BMA2x2_SLOW_NO_MOTION_THRES_REG         0x29
#define BMA2x2_TAP_PARAM_REG                    0x2A
#define BMA2x2_TAP_THRES_REG                    0x2B
#define BMA2x2_ORIENT_PARAM_REG                 0x2C
#define BMA2x2_THETA_BLOCK_REG                  0x2D
#define BMA2x2_THETA_FLAT_REG                   0x2E
#define BMA2x2_FLAT_HOLD_TIME_REG               0x2F
#define BMA2x2_FIFO_WML_TRIG                    0x30
#define BMA2x2_SELFTEST_REG                     0x32
#define BMA2x2_EEPROM_CTRL_REG                  0x33
#define BMA2x2_SERIAL_CTRL_REG                  0x34
#define BMA2x2_OFFSET_CTRL_REG                  0x36
#define BMA2x2_OFFSET_PARAMS_REG                0x37
#define BMA2x2_OFFSET_X_AXIS_REG                0x38
#define BMA2x2_OFFSET_Y_AXIS_REG                0x39
#define BMA2x2_OFFSET_Z_AXIS_REG                0x3A
#define BMA2x2_GP0_REG                          0x3B
#define BMA2x2_GP1_REG                          0x3C
#define BMA2x2_FIFO_MODE_REG                    0x3E
#define BMA2x2_FIFO_DATA_OUTPUT_REG             0x3F

#define BMA250_INT_READ()			(HAL_GPIO_ReadPin(BMA250_GPIO_Port,BMA250_GPIO_Pin) ? 1 : 0)		
							 
extern void BMA250_init(void);
extern bool BMA250_Is_LowINT(void);
extern bool BMA250_Is_HigINT(void);
extern bool BMA250_Is_SlopINT(void);
extern unsigned char BMA250_IS_INT(void);

#if DEBUG
bool BMA250_Is_DTapINT(void);
#endif

extern void BMA250_Clear_INT(void);
extern void BM250_GET_Slop(void);
extern void BMA250_Write_Register (u8 reg, u8 data);
extern unsigned char BMA250_Read_Register (unsigned char reg);
	 
#endif
