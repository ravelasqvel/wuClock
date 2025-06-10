#include <stdio.h>
#include "pico/stdlib.h"
#include "PushButton.h"
#include "SevenSegments.h"
#include "SmartBuzzer.h"
#include "SmartLED.h"
#include "WatchUI.h"
#include "Time4H.h"


watch_ui_t watchUI;  ///< Global variable for the watch UI
time_h_t timeHandler;  ///< Global variable for the time handler
ui_event_t events;  ///< Array to hold events from push buttons


void (* CurrentState)(void);
void StateSetTime(void);
void StateSetAlarm(void);
void StateSetSnooze(void);
void StateShowDate(void);
void StateNormal(void);
void StateAlarm(void);

void main(void)
{
    stdio_init_all();
    watch_ui_init(&watchUI);  ///< Initialize the watch UI
    t4h_init(&timeHandler);  ///< Initialize the time handler

    CurrentState = StateNormal;

    printf("Hello, world!\n");
        sleep_ms(1000);
    while (true) {
        CurrentState();  // Call the current state function
    }
}



void StateNormal(void){
    t4h_refresh_time(&timeHandler);  ///< Refresh the time handler
    if(t4h_is_time_to_refresh(&timeHandler)){  ///< Check if it's time to refresh the display
        char hour[3];
        char min[3];

        uint8_t h = t4h_get_hour(&timeHandler);  ///< Get the current hour
        uint8_t m = t4h_get_minute(&timeHandler);  ///< Get the current minute

        itoa(h, hour, 10);  ///< Convert hour to string
        itoa(m, min, 10);  ///< Convert minute to string

        ss_set_digit(&watchUI.ssDisplay, 0, hour[0] - '0');  ///< Set the first digit of the display to the first character of the hour
        ss_set_digit(&watchUI.ssDisplay, 1, hour[1] - '0');  ///< Set the second digit of the display to the second character of the hour
        ss_set_digit(&watchUI.ssDisplay, 2, min[0] - '0');  ///< Set the third digit of the display to the first character of the minute
        ss_set_digit(&watchUI.ssDisplay, 3, min[1] - '0');  ///< Set the fourth digit of the display to the second character of the minute
    }
    watch_ui_process(&watchUI, WATCH_UI_STATE_NORMAL, events);  ///< Process the watch UI in normal state

    if(t4h_get_alarm_state(&timeHandler) == T4H_ALARM_READY){  ///< Check if the alarm is on
       CurrentState = StateAlarm;  ///< Change state to alarm state
    }

    if(events.all){
        if(events.BITS.set_time == TWICE){  ///< Check if the set time button was pressed
            CurrentState = StateSetTime;  ///< Change state to set time state
        }
        if(events.BITS.set_alarm == ONCE){  ///< Check if the set alarm button was pressed
            t4h_enable_alarm(&timeHandler);  ///< Enable the alarm in the time handler
        }
        else if(events.BITS.set_alarm == TWICE){  ///< Check if the set alarm button was pressed
            CurrentState = StateSetAlarm;  ///< Change state to set alarm state
        }
        if(events.BITS.snooze == TWICE){  ///< Check if the snooze button was pressed
            CurrentState = StateSetSnooze;  ///< Change state to snooze state
        }
        if(events.BITS.show_date == TWICE){  ///< Check if the show date button was pressed
            CurrentState = StateShowDate;  ///< Change state to show date state
        }
    }
}

void StateAlarm(void){
    t4h_refresh_time(&timeHandler);  ///< Refresh the time handler
    if(t4h_is_time_to_refresh(&timeHandler)){  ///< Check if it's time to refresh the display
        char hour[3];
        char min[3];

        uint8_t h = t4h_get_hour(&timeHandler);  ///< Get the current hour
        uint8_t m = t4h_get_minute(&timeHandler);  ///< Get the current minute

        itoa(h, hour, 10);  ///< Convert hour to string
        itoa(m, min, 10);  ///< Convert minute to string

        ss_set_digit(&watchUI.ssDisplay, 0, hour[0] - '0');  ///< Set the first digit of the display to the first character of the hour
        ss_set_digit(&watchUI.ssDisplay, 1, hour[1] - '0');  ///< Set the second digit of the display to the second character of the hour
        ss_set_digit(&watchUI.ssDisplay, 2, min[0] - '0');  ///< Set the third digit of the display to the first character of the minute
        ss_set_digit(&watchUI.ssDisplay, 3, min[1] - '0');  ///< Set the fourth digit of the display to the second character of the minute
    
        CurrentState = StateNormal;
    }
    watch_ui_process(&watchUI, WATCH_UI_STATE_ALARM, events);  ///< Process the watch UI in normal state
    if(events.all){
        
        if(events.BITS.set_alarm){  ///< Check if the set alarm button was pressed
            t4h_disable_alarm(&timeHandler);  ///< Enable the alarm in the time handler
            CurrentState = StateNormal;  ///< Change state to normal state
        }
        if(events.BITS.snooze){  ///< Check if the snooze button was pressed
            CurrentState = StateSnooze;  ///< Change state to snooze state
        }
    }

}

void StateSetTime(void){}
void StateSetAlarm(void){}
void StateSetSnooze(void){}
void StateShowDate(void){}
void StateSnooze(void){}