#include "FrontendTaskHandle.h"
#include "GlobalVariables.h"

#include "lv_conf.h"
#define LGFX_USE_V1
#include <lvgl.h>
#include "ui.h"
#include "screens.h"
#include "vars.h"
#include "tft_lgfx_conf.h"
//===== CORE 1 INTERNAL DECLARATIONS =====

#define DRAW_BUF_SIZE (320 * 240 / 4)
// Force the RP2040 compiler to allocate this array on a clean 64-byte boundary
alignas(64) static uint8_t lv_display_buf[DRAW_BUF_SIZE];

LGFX tft;
GLOBAL_CORE_DATA_DRONE_FC_PROFILE_t Local_Data_Core1;

//===== CORE 1 INTERNAL FUNCTIONS =====

// // --- LVGL Tick Callback ---
static uint32_t my_tick_cb(void)  {return millis();}

// // --- LVGL Display FLush Callback ---
void display_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *pixel_map )
{
  uint32_t start_x = area->x1;
  uint32_t start_y = area->y1;
  uint32_t area_width = lv_area_get_width(area);
  uint32_t area_height = lv_area_get_height(area);

  tft.startWrite();  
  tft.pushImage(start_x, start_y, area_width, area_height, (uint16_t *)pixel_map);
  tft.endWrite();
  
  lv_display_flush_ready(disp);
}

// // --- LVGL Touch Read Callback ---
static void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
  uint16_t x=0,y=0;
  if( tft.getTouch(&x, &y) )
  {
    
    data->state = LV_INDEV_STATE_PRESSED;
    
    Serial.printf("LVGL Touch Coordinates -> X: %d, Y: %d\n", x, y);
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
  data->point.x = x;
  data->point.y = y;


}

int32_t monitor_throttle;
int32_t monitor_yaw;
int32_t monitor_pitch;
int32_t monitor_roll;

void set_var_monitor_throttle(int32_t value) {monitor_throttle = value;}
void set_var_monitor_yaw(int32_t value) {monitor_yaw = value;}
void set_var_monitor_pitch(int32_t value) {monitor_pitch = value;}
void set_var_monitor_roll(int32_t value) {monitor_roll = value;}

int32_t get_var_monitor_throttle() {return monitor_throttle;}
int32_t get_var_monitor_yaw() {return monitor_yaw;}
int32_t get_var_monitor_pitch() {return monitor_pitch;}
int32_t get_var_monitor_roll() {return monitor_roll;}

int32_t monitor_aux1;
int32_t get_var_monitor_aux1() {return monitor_aux1;}
void set_var_monitor_aux1(int32_t value) {monitor_aux1 = value;}

int32_t monitor_aux2;
int32_t get_var_monitor_aux2() {return monitor_aux2;}
void set_var_monitor_aux2(int32_t value) {monitor_aux2 = value;}

int32_t monitor_aux3;
int32_t get_var_monitor_aux3() {return monitor_aux3;}
void set_var_monitor_aux3(int32_t value) {monitor_aux3 = value;}

int32_t monitor_aux4;
int32_t get_var_monitor_aux4() {return monitor_aux4;}
void set_var_monitor_aux4(int32_t value) {monitor_aux4 = value;}

//===== CORE 1 TASK AND SETUP =====

void core1_loop(void*)
{
  uint32_t p_print_time = 0;
  while(1)
  {
    lv_timer_handler();
    ui_tick();
    if( lv_screen_active() == objects.drone_monitor )
    {
      set_var_monitor_throttle(Local_Data_Core1.Throttle);
      set_var_monitor_yaw(Local_Data_Core1.Yaw);
      set_var_monitor_pitch(Local_Data_Core1.Pitch);
      set_var_monitor_roll(Local_Data_Core1.Roll);
      set_var_monitor_aux1(Local_Data_Core1.AUX1==100 ? 0 : 1);
      set_var_monitor_aux2(Local_Data_Core1.AUX2==100 ? 0 : 1);
      set_var_monitor_aux3(Local_Data_Core1.AUX3==100 ? 0 : 1);
      set_var_monitor_aux4(Local_Data_Core1.AUX4==100 ? 0 : 1);
    }
    static uint8_t cnt = 0;
    Serial.printf("%d\n",cnt++);
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void CoreData_Receive(void*)
{
  while(1)
  {
    if( xSemaphoreTake(Core_Data_Mutex, pdMS_TO_TICKS(10))==pdTRUE )
    {
      Local_Data_Core1 = Core_Data_Drone_FC;
      xSemaphoreGive(Core_Data_Mutex);
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

TaskHandle_t Core1_TaskHandle = NULL;
void init_core1()
{
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(0x0);

    uint16_t calib_data[8]={300,300,3850,300,300,3700,3850,3700};
    tft.setTouchCalibrate(calib_data);
    uint16_t x,y;
    while(1)
    {
      if( tft.getTouch(&x, &y) ){
        Serial.printf("LVGL Touch Coordinates -> X: %d, Y: %d\n", x, y);
        delay(1);
      }
    }

    //=== LVGL Display Setup ===
    lv_init();
    lv_tick_set_cb(my_tick_cb);

    lv_display_t *disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565_SWAPPED);

    lv_display_set_buffers(disp, lv_display_buf, NULL, sizeof(lv_display_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, display_flush_cb);

    lv_indev_t * ts = lv_indev_create();
    lv_indev_set_type(ts, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(ts, touch_read_cb);

    lv_lock();
    ui_init();
    lv_unlock();

    // lv_timer_t* DebugInfo_Timer = lv_timer_create(LVGL_Value_Update,5,NULL);  

    uint8_t core1_priority_cnt = 2;
    xTaskCreate(
    core1_loop,
    "Main_LVGL_Task",
    4096,
    NULL,
    core1_priority_cnt--,
    &Core1_TaskHandle
    );

    xTaskCreate(
    CoreData_Receive,
    "CoreData_Receive_Task",
    1024,
    NULL,
    core1_priority_cnt--,
    &Core1_TaskHandle
    );
    vTaskCoreAffinitySet(Core1_TaskHandle, 1<<1);
}