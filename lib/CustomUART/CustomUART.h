#ifndef CUSTOMUART_H
#define CUSTOMUART_H

#include <Arduino.h>

//payload if cmd = set_mode
enum UART_MODE_t
{
    IDLE        =   0x00,
    BLE_CONTR   =   0x01,
    ESP_FC      =   0x02,
    NRF24       =   0x03
};

//each mode has different payloads
struct __attribute__((packed)) UART_PAYLOAD_BLE_CONTR
{
   uint8_t keyboard_data[8]; 
   uint8_t mouse_data[5];
};

#define DEFAULT_UART_HEADER 0xAA

class CUSTOM_UART_t
{
private:
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint32_t baudrate;
    SerialUART* refUART = nullptr;

    uint8_t header;
    UART_MODE_t uart_mode;
    uint8_t length;
    uint8_t payload[20];

    void send_data_raw();
    
public:
    CUSTOM_UART_t();
    void set_header(uint8_t head){header = head;}
    void begin(SerialUART* ref, int8_t tx, int8_t rx, uint32_t baud);
    void process_data(UART_MODE_t mode, uint8_t* buffer, uint8_t buffer_length);
};

#endif
