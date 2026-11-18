#include "BackendTaskHandle.h"
#include "GlobalVariables.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <PCF8575.h>
#include "CustomUART.h"
#include "Button.h"

//===== CORE 0 INTERNAL DECLARATIONS =====

#pragma pack(push,1)
struct Gamepad_t
{
  uint8_t isUp=0, isDown=0, isLeft=0, isRight=0, isCenter=0;
};
#pragma pack(pop)

volatile bool PCF_INT_Flag = 0;
uint16_t Button_Array = 0xFFF;

#define Josyti_Processed_Val_max 127
#define Josyti_Processed_Val_min 1
class Joystick_t
{
private:
  Adafruit_ADS1015 &AnalogReader;

  int8_t x,y;
  uint8_t is_inverted_x=0, is_inverted_y=0;

  uint8_t raw_y, raw_x;
  uint8_t raw_x_threshold = 3, raw_y_threshold = 3;
  uint8_t raw_x_center, raw_y_center;
  uint8_t raw_x_min = 15, raw_x_max = 150;
  uint8_t raw_y_min = 15, raw_y_max = 150;
  bool have_throttle = 0;
public:
  Joystick_t(Adafruit_ADS1015 &ref): AnalogReader(ref){}

  //raw value: raw_min -> -raw_threshold raw_center +raw_threshold -> raw_max => -127->+127 with 0 centered
  void readX()
  {
    raw_x = AnalogReader.readADC_SingleEnded(1)/10;

    if( raw_x > raw_x_max ) raw_x_max = raw_x;
    else if(raw_x < raw_x_min) raw_x_min = raw_x;

    //converting
    if( raw_x > raw_x_center + raw_x_threshold )
    {
      x = map(raw_x, raw_x_center + raw_x_threshold, raw_x_max, Josyti_Processed_Val_min, Josyti_Processed_Val_max);
    }
    else if( raw_x < raw_x_center - raw_x_threshold )
    {
      x = map(raw_x, raw_x_center - raw_x_threshold, raw_x_min, -Josyti_Processed_Val_min, -Josyti_Processed_Val_max);
    }
    else
      x = 0;

    if( is_inverted_x ) x*=(-1);
  }

  void readY()
  {
    raw_y = AnalogReader.readADC_SingleEnded(0)/10;

    if( raw_y > raw_y_max ) raw_y_max = raw_y;
    else if(raw_y < raw_y_min) raw_y_min = raw_y;

    // converting
    if( !have_throttle )
    {
      if( raw_y > raw_y_center + raw_y_threshold )
      {
        y = map(raw_y, raw_y_center + raw_y_threshold, raw_y_max, Josyti_Processed_Val_min, Josyti_Processed_Val_max);
      }
      else if( raw_y < raw_y_center - raw_y_threshold )
      {
        y = map(raw_y, raw_y_center - raw_y_threshold, raw_y_min, -Josyti_Processed_Val_min, -Josyti_Processed_Val_max);
      }
      else
        y = 0;
    }
    else
      y = map(raw_y, raw_y_min, raw_y_max, 0, 127);

    if( is_inverted_y ) y*=(-1);
  }
  
  void center_calib()
  {
    uint16_t sum_raw_x=0;
    for(int i=0; i<100; i++)
    {
      sum_raw_x += AnalogReader.readADC_SingleEnded(1)/10;
    }
    sum_raw_x/=100;
    raw_x_center  = sum_raw_x;

    uint16_t sum_raw_y=0;
    for(int i=0; i<100; i++)
    {
      sum_raw_y += AnalogReader.readADC_SingleEnded(0)/10;
    }
    sum_raw_y/=100;
    raw_y_center  = sum_raw_y;
  }

  int8_t getX() {return x;}
  int8_t getY() {return y;}
  int8_t get_rawX() {return raw_x;}
  int8_t get_rawY() {return raw_y;}
  void set_inverted_x(uint8_t inverted = 1) {is_inverted_x = inverted;}
  void set_inverted_y(uint8_t inverted = 1) {is_inverted_y = inverted;}
  void set_FC_profile() {have_throttle =1;}
};

Adafruit_ADS1015 AnalogReaderR, AnalogReaderL;
PCF8575 Gamepad(0x20);

Joystick_t JoystL(AnalogReaderL), JoystR(AnalogReaderR);
Gamepad_t GamepadL, GamepadR;

CUSTOM_UART_t myUART;
uint8_t data_buffer[20];
uint8_t data_length = 1;
UART_MODE_t current_protocol_mode = ESP_FC;

Button UpL, DownL, LeftL, RightL, CenterL;
Button UpR, DownR, LeftR, RightR, CenterR;

struct BLE_UART_DATA_t
{
  uint8_t TX_Mode = 0xBE;  //BLE mode

  uint8_t keyboard_data[8];
  uint8_t mouse_data[5];  

} BLE_UART_Data;

GLOBAL_CORE_DATA_DRONE_FC_PROFILE_t Local_Data_Core0;

//===== CORE 0 INTERNAL FUNCTIONS =====

void PCF_INT_Cb() {PCF_INT_Flag = 1;}
void Inputs_Setup()
{
  // === I2C setup ===
  Wire.setSCL(I2C_SCL);
  Wire.setSDA(I2C_SDA);
  Wire.setClock(400*1000);
  Wire.begin();

  // === AnalogReader Setup ===
  while( !AnalogReaderL.begin(ADS_GND_ADR, &Wire))
  {
    digitalWrite(OnB_led, !digitalRead(OnB_led));
    Serial.println("ADS1015 L failed");
    delay(500);
  }

  AnalogReaderL.setGain(GAIN_ONE);
  AnalogReaderL.setDataRate(RATE_ADS1015_3300SPS);
  JoystL.center_calib();
  JoystL.set_FC_profile();

  while( !AnalogReaderR.begin(ADS_VCC_ADR, &Wire))
  {
    digitalWrite(OnB_led, !digitalRead(OnB_led));
    Serial.println("ADS1015 R failed");
    delay(500);
  }

  AnalogReaderR.setGain(GAIN_ONE);
  AnalogReaderR.setDataRate(RATE_ADS1015_3300SPS);
  JoystR.center_calib();
  JoystR.set_inverted_y(1);
  JoystR.set_inverted_x(1);

  // === Gamepad Setup ===
  Gamepad.begin();
  while( !Gamepad.isConnected())
  {
    digitalWrite(OnB_led, !digitalRead(OnB_led));
    Serial.println("PCF1115 failed");
    delay(500);
  }
  digitalWrite(OnB_led, 0);

  pinMode(PCF_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PCF_IRQ),PCF_INT_Cb,FALLING);
}
void Serialprint()
{
  static uint32_t p_time = 0;
  if( millis()-p_time>=20 )
  {
    p_time = millis();
    Serial.printf("===== Print Info =====\n");
    Serial.printf("JoystL:\nx: %d\ny:%d\n",JoystL.getX(),JoystL.getY());
    Serial.printf("JoystR:\nx: %d\ny:%d\n",JoystR.getX(),JoystR.getY());

    // Serial.printf("Gamepad: ");
    // for(int i=0; i<16; i++) {Serial.printf("%d:%d-", i,Button_Array>>i&1);}
    Serial.printf("running on core %d\n", get_core_num());
    Serial.printf("\n=====================\n");
  }
}
void BLE_Controller()
{
  //sending keyboard
  memset(BLE_UART_Data.keyboard_data, 0, 8);

  uint8_t array_id =2;  //have reserved and modifier on first two bytes
  BLE_UART_Data.keyboard_data[0] = BLE_UART_Data.keyboard_data[1] = 0;
  int8_t  Lx=JoystL.getX(), Ly=JoystL.getY();
  if( Lx>30 )
    BLE_UART_Data.keyboard_data[array_id++]=0x07; //d pressed
  if( Lx<-30 )
    BLE_UART_Data.keyboard_data[array_id++]=0x04; //a pressed
  if( Ly>30 )
    BLE_UART_Data.keyboard_data[array_id++]=0x1a; //w pressed
  if( Ly<-30 )
    BLE_UART_Data.keyboard_data[array_id++]=0x16; //s pressed

  if( GamepadR.isCenter )
    BLE_UART_Data.keyboard_data[array_id++]=0x2C; //space pressed

  if( GamepadL.isLeft )
    BLE_UART_Data.keyboard_data[array_id++]=0x14; //q pressed
  if( GamepadL.isRight )
    BLE_UART_Data.keyboard_data[array_id++]=0x08; //e pressed
  // if( !(Button_Array>>8&1) )
  //   BLE_UART_Data.keyboard_data[array_id++]=0x04;  //a pressed
  // if( !(Button_Array>>9&1) )
  //   BLE_UART_Data.keyboard_data[array_id++]=0x1b;  //x pressedw
  // // if( !(Button_Array>>10&1) )
  // //   BLE_UART_Data.keyboard_data[array_id++]=0x16;  //s pressed
  // if( !(Button_Array>>11&1) )
  //   BLE_UART_Data.keyboard_data[array_id++]=0x1a;  //w pressed
  // if( !(Button_Array>>12&1) )
  //   BLE_UART_Data.keyboard_data[array_id++]=0x07;  

  //sending mouse
  memset(BLE_UART_Data.mouse_data, 0, 5);

  uint8_t MouseButton = 0;
  static const int8_t max_speed = 7;

  if( GamepadR.isLeft ) //left clicked
    MouseButton |= 1;
  if( GamepadR.isRight )//right clicked
    MouseButton |=2;

  //x movement
  int8_t MouseX = JoystR.getX();
  if( MouseX>0 )
    MouseX = map(MouseX, 1, 127, 1, max_speed);
  else if( MouseX<0 )
    MouseX = map(MouseX, -1, -127, -1, -max_speed);

  //y movement
  int8_t MouseY = JoystR.getY();
  if( MouseY>0 )
    MouseY = map(MouseY, 1, 127, -max_speed, -1);
  else if( MouseY<0 )
    MouseY = map(MouseY, -1, -127, 1, max_speed);

  // Serial.printf("Mouse: %d - x: %d - y: %d\n", MouseButton, MouseX, MouseY);

  BLE_UART_Data.mouse_data[0] = MouseButton;
  BLE_UART_Data.mouse_data[1] = static_cast<uint8_t>(MouseX);
  BLE_UART_Data.mouse_data[2] = static_cast<uint8_t>(MouseY);
  BLE_UART_Data.mouse_data[3] = BLE_UART_Data.mouse_data[4] = 0;

  Serial2.write((uint8_t*)&BLE_UART_Data,14);
}

uint8_t JDC(int8_t joyst_val) //Convert Joystick value -127 -> 127 to drone Joystick value(uint8_t) 100->200
{
  if( !(joyst_val >= -127 && joyst_val <= 127) )  return 150;
  if( joyst_val == 0 )
    return 150;
  
  if( joyst_val>0 )
    return constrain(map(joyst_val, Josyti_Processed_Val_min, Josyti_Processed_Val_max, 151, 200),100, 200);
  return constrain(map(joyst_val, -Josyti_Processed_Val_min, -Josyti_Processed_Val_max, 149, 100),100, 200);
}
void UART_ESP_FC_Handle()
{
  UpL.update(GamepadL.isUp);
  DownL.update(GamepadL.isDown);
  UpR.update(GamepadR.isUp);
  DownR.update(GamepadR.isDown);

  data_length = 8;
  data_buffer[0] = JDC(JoystR.getX()); //roll
  data_buffer[1] = JDC(JoystR.getY()); //pitch
  data_buffer[2] = map(JoystL.getY(), 0, Josyti_Processed_Val_max, 100, 200); //throttle
  data_buffer[3] = JDC(JoystL.getX()); //yaw
  data_buffer[4] = map(UpL.isToggle(), 0, 1, 100, 200); //AUX1
  data_buffer[5] = map(DownL.isToggle(),0,1,100,200);  //AUX2
  data_buffer[6] = map(UpR.isToggle(), 0, 1, 100, 200); //AUX3
  data_buffer[7] = map(DownR.isToggle(), 0, 1, 100, 200); //AUX4

  Local_Data_Core0.Roll = data_buffer[0]*10;
  Local_Data_Core0.Pitch = data_buffer[1]*10;
  Local_Data_Core0.Throttle = data_buffer[2]*10;
  Local_Data_Core0.Yaw = data_buffer[3]*10;
  Local_Data_Core0.AUX1 = data_buffer[4];
  Local_Data_Core0.AUX2 = data_buffer[5];
  Local_Data_Core0.AUX3 = data_buffer[6];
  Local_Data_Core0.AUX4 = data_buffer[7];
}

//===== CORE 0 TASK AND SETUP =====

void task_InputRead(void*)
{
    uint16_t t =0;
    while(1)
    {
        //Read Joystick
        // digitalWrite(OnB_led, !digitalRead(OnB_led));
        JoystR.readX();
        JoystR.readY();

        JoystL.readX();
        JoystL.readY();

        // Serial.printf("raw LY: %d\n", AnalogReaderL.readADC_SingleEnded(0)/10);

        // // Read Gamepad
        if( PCF_INT_Flag )
        {
            PCF_INT_Flag = 0;
            Button_Array = Gamepad.read16();

            GamepadL.isLeft = !(Button_Array>>0&1);
            GamepadL.isCenter=!(Button_Array>>1&1);
            GamepadL.isDown = !(Button_Array>>2&1);
            GamepadL.isUp =   !(Button_Array>>3&1);
            GamepadL.isRight= !(Button_Array>>4&1);

            GamepadR.isLeft = !(Button_Array>>11&1);
            GamepadR.isCenter=!(Button_Array>>12&1);
            GamepadR.isDown = !(Button_Array>>13&1);
            GamepadR.isUp =   !(Button_Array>>14&1);
            GamepadR.isRight= !(Button_Array>>15&1);
        }

        // Serialprint();
        // static uint8_t cnt = 0;
        // Serial.printf("reading input cnt: %d\n", cnt++);

        //run at 100Hz
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_UART_Handle(void*)
{
  memset(data_buffer, 0, 20);
  while(1)
  {     
    switch (current_protocol_mode)
    {      
    case BLE_CONTR:
      data_length = 13;
      
      break;

    case ESP_FC:
      UART_ESP_FC_Handle();
      // static uint32_t p_print_time = 0;
      // if( millis() - p_print_time >=20 )  {p_print_time = millis(); Serial.printf("throt: %d\n", data_buffer[2]);}
      break;

    case IDLE:
      data_length = 1;
      data_buffer[0] = 9;
    }

    myUART.process_data(current_protocol_mode, data_buffer, data_length);
    //run at 100Hz
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void CoreData_Send(void*)
{
  while(1)
  {
    if( xSemaphoreTake(Core_Data_Mutex, pdMS_TO_TICKS(5))==pdTRUE )
    {
      Core_Data_Drone_FC = Local_Data_Core0;
      xSemaphoreGive(Core_Data_Mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

TaskHandle_t Core0_TaskHandle = NULL;
void init_core0()
{
    myUART.begin(&Serial2, UART_TX, UART_RX, 115200);
    Inputs_Setup();

    uint8_t core0_priority_cnt = 3;

    xTaskCreate(
    task_UART_Handle,
    "UART_Handle_task",
    2048,
    NULL,
    core0_priority_cnt--,
    &Core0_TaskHandle
    );

    xTaskCreate(
    task_InputRead,
    "Input_Read_task",
    2048,
    NULL,
    core0_priority_cnt--,
    &Core0_TaskHandle
    );

    xTaskCreate(
    CoreData_Send,
    "CoreData_Send_Task",
    2048,
    NULL,
    core0_priority_cnt--,
    &Core0_TaskHandle
    );

    vTaskCoreAffinitySet(Core0_TaskHandle, 1<<0);
}
