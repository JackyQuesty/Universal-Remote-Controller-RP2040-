#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#include <Arduino.h>
#define __FREERTOS 1
#include <FreeRTOS.h>
#include <task.h>

// #define debug_block(i){while(1){delay(200);Serial.printf("debug%d\n",i);}}
// #define debug(i) {Serial.printf("debug%d\n",i);}
// #define LED_DEBUG while(1){delay(200);digitalWrite(25,!digitalRead(25));}

#define I2C_SCL 17
#define I2C_SDA 16
#define PCF_IRQ 18

#define ADS_GND_ADR 0b1001000
#define ADS_VCC_ADR 0b1001001

#define UART_TX 20
#define UART_RX 21

#define OnB_led 25

struct GLOBAL_CORE_DATA_DRONE_FC_PROFILE_t
{
    uint16_t Throttle;
    uint16_t Yaw;
    uint16_t Pitch;
    uint16_t Roll;
    uint16_t AUX1;
    uint16_t AUX2;
    uint16_t AUX3;
    uint16_t AUX4;
};

extern SemaphoreHandle_t Core_Data_Mutex;
extern GLOBAL_CORE_DATA_DRONE_FC_PROFILE_t Core_Data_Drone_FC;

// enum UART_MODE_t
// {
//   UART_DISABLED       = 0x00,
//   UART_MODE_SELECT    = 0x01,
//   UART_BLE_CONTROLLER
// };

// extern UART_MODE_t UART_Mode;

#endif