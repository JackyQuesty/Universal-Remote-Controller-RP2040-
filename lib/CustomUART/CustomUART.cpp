#include "CustomUART.h"

CUSTOM_UART_t::CUSTOM_UART_t(){};

void CUSTOM_UART_t::send_data_raw()
{
    uint8_t send_buffer[3+length];
    send_buffer[0] = header;
    send_buffer[1] = (uint8_t)uart_mode;
    send_buffer[2] = length;
    for(uint8_t i = 3; i<3+length; i++)
        send_buffer[i] = payload[i-3];

    (*refUART).write(send_buffer, 3+length);
}

void CUSTOM_UART_t::begin(SerialUART* ref, int8_t tx=-1, int8_t rx=-1, uint32_t baud=9600)
{
    if( tx<0 || rx<0 )  {Serial.println("Serial init failed");return;}

    tx_pin = tx;
    rx_pin = rx;
    baudrate = baud;
    refUART = ref;

    (*refUART).setTX(tx);
    (*refUART).setRX(rx);
    (*refUART).begin(baud);

    header = DEFAULT_UART_HEADER;
    uart_mode = IDLE;
    length = 1;
    memset(payload, 0, 20);
    send_data_raw();
}

void CUSTOM_UART_t::process_data(UART_MODE_t mode, uint8_t* buffer, uint8_t buffer_length)
{
    if( buffer_length<=0 )
    {
        Serial.printf("not suitable buffer length\n");
        return;
    }

    if( uart_mode != mode )
    {
        //its a new mode, change
        uart_mode = mode;
        length = buffer_length;
        memset(payload, 0, length);
    }
    else    //no mode changed
        for(int i = 0; i<length; i++)
            payload[i] = buffer[i];
    
    send_data_raw();
}