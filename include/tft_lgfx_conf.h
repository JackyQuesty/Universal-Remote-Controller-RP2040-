#ifndef TFT_LGFX_CONF_H
#define TFT_LGFX_CONF_H

#include <LovyanGFX.hpp>

#define TFT_CS 0
#define TFT_RST 1
#define TFT_DC 2
#define SPI_MOSI 3
#define SPI_MISO 4
#define SPI_SCL 6

#define TOUCH_CS 7
#define TOUCH_IRQ 8

#define TFT_WIDTH 320
#define TFT_HEIGHT 240

class LGFX : public lgfx::LGFX_Device 
{
  lgfx::Panel_ST7789  _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Touch_XPT2046 _touch_instance;

  public:
  LGFX(void)
  {
    { // Configure the SPI bus
      auto cfg = _bus_instance.config();
      
      cfg.spi_host = 0;          // RP2040 SPI0 (or 1 for SPI1)
      cfg.spi_mode = 0;          // SPI Mode 0
      cfg.freq_write = 40*1000000; // 40MHz SPI clock (safe for most displays)
      // cfg.freq_write = 2000000;
      cfg.freq_read  = 16*1000000; 
      cfg.pin_sclk = SPI_SCL;
      cfg.pin_mosi = SPI_MOSI;
      cfg.pin_miso = SPI_MISO;
      cfg.pin_dc   = TFT_DC;

      _bus_instance.config(cfg); 
      _panel_instance.setBus(&_bus_instance); 
    }

    { // Configure the display panel settings
      auto cfg = _panel_instance.config();

      cfg.pin_cs           = TFT_CS;
      cfg.pin_rst          = TFT_RST;
      
      cfg.memory_width     = 240;
      cfg.memory_height    = 320;
      cfg.panel_width      = 240; 
      cfg.panel_height     = 320;
      cfg.offset_x         = 0;
      cfg.offset_y         = 0;
      cfg.bus_shared       =  true;

      _panel_instance.config(cfg);
    }

    { // Configure the Touch Settings
      auto cfg = _touch_instance.config();

      cfg.bus_shared = true;   
      cfg.spi_host   = 0;
      cfg.x_min      = 300;
      cfg.x_max      = 3850;
      cfg.y_min      = 300; 
      cfg.y_max      = 3700;
      cfg.pin_sclk = SPI_SCL;
      cfg.pin_mosi = SPI_MOSI;
      cfg.pin_miso = SPI_MISO;
      cfg.pin_cs     = TOUCH_CS;
      // cfg.freq = 1000000;
      cfg.pin_int    = TOUCH_IRQ;  
      cfg.freq       =2 * 1000000;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance); // Apply configuration
  }
};

#endif