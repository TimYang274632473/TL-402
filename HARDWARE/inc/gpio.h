#ifndef __GPIO_H
#define __GPIO_H

#include "includes.h"
#include "stm32l0xx_hal.h"

#define CHK_VIN_Pin 		GPIO_PIN_13
#define CHK_VIN_GPIO_Port 	GPIOC

#define ZDMD_Pin 			GPIO_PIN_0
#define ZDMD_GPIO_Port 		GPIOH

#define WIFI_Pin 			GPIO_PIN_1
#define WIFI_GPIO_Port 		GPIOH

#define TX_WIFI_Pin			GPIO_PIN_0
#define TX_WIFI_GPIO_Port	GPIOA

#define RX_WIFI_Pin 		GPIO_PIN_1
#define RX_WIFI_GPIO_Port 	GPIOA

#define TX_GPS_Pin			GPIO_PIN_2
#define TX_GPS_Port		 	GPIOA

#define RX_GPS_Pin 			GPIO_PIN_3
#define RX_GPS_GPIO_Port 	GPIOA

#define CS_W25_Pin 			GPIO_PIN_4
#define CS_W25_GPIO_Port 	GPIOA

#define SCLK_W25_Pin 		GPIO_PIN_5
#define SCLK_W25_GPIO_Port 	GPIOA

#define MISO_W25_Pin 		GPIO_PIN_6
#define MISO_W25_GPIO_Port 	GPIOA

#define MOSI_W25_Pin 		GPIO_PIN_7
#define MOSI_W25_GPIO_Port 	GPIOA

#define KEY_SOS_GPIO_Pin 	GPIO_PIN_0
#define KEY_SOS_GPIO_Port	GPIOB

#define CHK_CH_Pin 			GPIO_PIN_1
#define CHK_CH_GPIO_Port 	GPIOB
									 
#define BMA250_GPIO_Pin		GPIO_PIN_2
#define BMA250_GPIO_Port	GPIOB	

#define TX_GSM_Pin 			GPIO_PIN_10
#define TX_GSM_GPIO_Port 	GPIOB

#define RX_GSM_Pin 			GPIO_PIN_11
#define RX_GSM_GPIO_Port 	GPIOB

#define GPS_LED_Pin			GPIO_PIN_12
#define GPS_LED_GPIO_Port	GPIOB

#define WORK_LED_Pin		GPIO_PIN_13
#define WORK_LED_GPIO_Port	GPIOB

#define GSM_LED_Pin			GPIO_PIN_14
#define GSM_LED_GPIO_Port	GPIOB

#define GPRS_LED_Pin		GPIO_PIN_15
#define GPRS_LED_GPIO_Port	GPIOB

#define WM8960_EN_Pin  		GPIO_PIN_8
#define WM8960_EN_GPIO_Port GPIOA

#define TX_PC_Pin 			GPIO_PIN_9
#define TX_PC_GPIO_Port 	GPIOA

#define RX_PC_Pin 			GPIO_PIN_10
#define RX_PC_GPIO_Port 	GPIOA

#define GSMON_Pin			GPIO_PIN_11
#define GSMON_GPIO_Port		GPIOA

#define PWM_AUDIO_GPIO_Pin	GPIO_PIN_12	
#define PWM_AUDIO_GPIO_Port	GPIOA

#define HL_DTR_Pin 			GPIO_PIN_15
#define HL_DTR_GPIO_Port 	GPIOA

#define KEY_CHKPWR_GPIO_Pin	 GPIO_PIN_3
#define KEY_CHKPWR_GPIO_Port GPIOB
										 
#define KEY_CALL1_GPIO_Pin 	GPIO_PIN_4
#define KEY_CALL1_GPIO_Port GPIOB

#define KEY_CALL2_GPIO_Pin 	GPIO_PIN_5
#define KEY_CALL2_GPIO_Port GPIOB
									 
#define IIC_SCL_Pin			GPIO_PIN_6	
#define IIC_SCL_GPIO		GPIOB	

#define IIC_SDA_Pin			GPIO_PIN_7	
#define IIC_SDA_GPIO		GPIOB

#define GPS_VCON_Pin 		GPIO_PIN_8
#define GPS_VCON_GPIO_Port 	GPIOB

#define PWRON_Pin 			GPIO_PIN_9
#define PWRON_GPIO_Port 	GPIOB

extern void power_off(void);
extern void MX_GPIO_Init(void);
extern void motor_on(void);
extern void motor_off(void);
extern void ZDMD_ON(void);
extern void Speak_Amp_On(void);
extern void Speak_Amp_Off(void);
extern void GPS_Power_On(void);
extern void GPS_Force_On(void);
extern void GPS_Power_Off(void);
extern void GPS_Force_Off(void);
extern void GSM_Wakeup(void);
extern void GSM_Sleep(void);
extern void GSMON_H(void);
extern void GSMON_L(void);

extern void WORK_LED_On(void);
extern void WORK_LED_Off(void);
extern void GPRS_LED_On(void);
extern void GPRS_LED_Off(void);
extern void GPS_LED_On(void);
extern void GPS_LED_Off(void);
extern void GSM_LED_On(void);
extern void GSM_LED_Off(void);
extern void WORK_LED_R(void);
extern void GPRS_LED_R(void);
extern void GPS_LED_R(void);
extern void GSM_LED_R(void);

extern GPIO_PinState USBinsert_chk(void);
extern GPIO_PinState PWRkey_chk(void);
extern GPIO_PinState CHK_CH(void);

#endif

