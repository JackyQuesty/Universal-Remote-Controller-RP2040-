#include <GlobalVariables.h>

#include "BackendTaskHandle.h"
#include "FrontendTaskHandle.h"

// struct UART_DATA
// {
//   uint16_t UART_key = 0xAA;
//   uint8_t Joyst_RX, Joyst_RY;
//   uint8_t Joyst_LX, Joyst_LY;
// } UART_Data;

SemaphoreHandle_t Core_Data_Mutex = NULL;
GLOBAL_CORE_DATA_DRONE_FC_PROFILE_t Core_Data_Drone_FC;

void check_mutex(SemaphoreHandle_t mutex, const char* mess)
{
  if( (mutex)==NULL )
  {
    while(1)
    {
      Serial.printf("mutex failed: %s\n", mess);
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(OnB_led, OUTPUT);
  digitalWrite(OnB_led,0);

  Core_Data_Mutex = xSemaphoreCreateMutex();
  check_mutex(Core_Data_Mutex, "core data mutex");

  init_core0();
  init_core1();
}

void loop(){vTaskDelay(portMAX_DELAY);}
void loop1(){vTaskDelay(portMAX_DELAY);}

//8: a
//9: x
//10:s 
//11:w
//12:d