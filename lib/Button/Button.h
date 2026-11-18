#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

enum BUTTON_STATE_t
{
    BUTTON_IDLE,
    BUTTON_PRESSED,
    BUTTON_RELEASED,
    BUTTON_CLICKED,
    BUTTON_HOLDING
};

class Button {
private:
    uint8_t p_raw_state=0;
    uint8_t stable_state=0, p_stable_state=0;

    uint32_t p_debounce_time=0;
    uint8_t debounceDelay;
    
    BUTTON_STATE_t button_state = BUTTON_IDLE;
    uint32_t p_holding_time = 0;
    uint16_t holding_threshold_time = 1000;
    uint8_t toggle_state = 0;

    void debounce(const uint8_t current_state)
    {
        p_stable_state = stable_state;
        if( current_state == p_raw_state )
            {if( millis()-p_debounce_time >= debounceDelay)
                stable_state = current_state;}
        else
            p_debounce_time = millis();
        p_raw_state = current_state; 
    }
public:
    Button(unsigned long debounce = 10) :
        debounceDelay(debounce)
        {}

    uint8_t isPressed() {return (button_state==BUTTON_PRESSED);}
    uint8_t isReleased() {return (button_state==BUTTON_IDLE);}
    uint8_t isClicked() {return (button_state==BUTTON_CLICKED);}
    uint8_t isHolding() {return (button_state==BUTTON_HOLDING);}
    uint8_t isToggle(uint8_t On_Logic= 1){return (toggle_state==On_Logic);}

    void update(const uint8_t current_state) {
        debounce(current_state);
        
        if( stable_state )
        {
            switch(button_state)
            {
                case BUTTON_IDLE:
                    button_state = BUTTON_PRESSED;
                    p_holding_time = millis();
                    break;
                case BUTTON_PRESSED:
                    if( millis() - p_holding_time >= holding_threshold_time )
                        button_state = BUTTON_HOLDING;
                    break;
                default:break;
            }
        }
        else
        {
            switch (button_state)
            {
            case BUTTON_PRESSED:
                button_state = BUTTON_CLICKED;
                break;
            
            default:
                button_state = BUTTON_IDLE;
                break;
            }
        }

        if( button_state == BUTTON_CLICKED )
            toggle_state = !toggle_state;
    }
};

#endif
