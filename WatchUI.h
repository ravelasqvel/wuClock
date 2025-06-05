/**
 * \file WatchUI.h
 * \brief Header file for WatchUI module   
 * This file contains the declarations for the WatchUI module, which provides functions to manage the user interface of a watch application.
 * It includes functions to initialize the UI, update the display, and handle user input.
 * \author Ricardo Andres Velasquez Velez
 * \version 0.0.1  
 * \date 05/29/2025
 * \copyright Unlicensed
 */

#ifndef __WATCH_UI_H_
#define __WATCH_UI_H_

#include <stdint.h>
#include "PushButton.h"
#include "SevenSegments.h"
#include "SmartLED.h"
#include "SmartBuzzer.h"

typedef enum {
    WATCH_UI_STATE_NORMAL,
    WATCH_UI_STATE_SET_TIME,
    WATCH_UI_STATE_SET_ALARM,
    WATCH_UI_STATE_SET_SNOOZE,
    WATCH_UI_STATE_ALARM,
    WATCH_UI_STATE_SHOW_DATE,
    WATCH_UI_STATE_SNOOZE
} watch_ui_state_t;

typedef struct  {
    push_button_t pbSetTime;      ///< Push button for setting time
    push_button_t pbSetAlarm;     ///< Push button for setting alarm
    push_button_t pbPlus;        ///< Push button for incrementing values
    push_button_t pbMinus;       ///< Push button for decrementing values
    push_button_t pbSnooze;      ///< Push button for snoozing alarms
    push_button_t pbShowDate;   ///< Push button for showing date

    smart_led_t ledAlarm;      ///< Smart LED for alarm indication
    smart_led_t ledHourUP;       ///< Smart LED for hour increment indication
    smart_led_t ledHourDOWN;     ///< Smart LED for hour decrement indication
    
    buzzer_t buzzer;        ///< Smart buzzer for audio feedback
    
    ss_config_t ssDisplay;  ///< Seven segment display configuration for showing time and date

} watch_ui_t;

/**
 * \fn void watch_ui_init(void)
 * \brief Initialize the Watch UI module
 * 
 * This function initializes the Watch UI module, setting up the necessary components for the user interface.
 */
void watch_ui_init(watch_ui_t *ui) {
    pb_init(&ui->pbSetTime, 2, 0, 0);      ///< Initialize push button for setting time
    pb_init(&ui->pbSetAlarm, 3, 0, 0);     ///< Initialize push button for setting alarm
    pb_init(&ui->pbPlus, 4, 0, 0);         ///< Initialize push button for incrementing values
    pb_init(&ui->pbMinus, 5, 0, 0);        ///< Initialize push button for decrementing values
    pb_init(&ui->pbSnooze, 6, 0, 0);       ///< Initialize push button for snoozing alarms      
    pb_init(&ui->pbShowDate, 7, 0, 0);     ///< Initialize push button for showing date

    ss_init(&ui->ssDisplay, 4, COMMON_ANODE, 0x000F0F00, 0x0000F000); ///< Initialize seven segment display with 4 digits

    buzzer_init(&ui->buzzer, 20); ///< Initialize smart buzzer on GPIO 8
    sLED_init(&ui->ledAlarm, 21);      ///< Initialize smart LED for alarm indication on GPIO 9
    sLED_init(&ui->ledHourUP, 22);   ///< Initialize smart LED for hour increment indication on GPIO 10
    sLED_init(&ui->ledHourDOWN, 26); ///< Initialize smart LED for hour decrement indication on GPIO 11
}

void watch_ui_process(watch_ui_t *ui, watch_ui_state_t state, pb_event_t *events) {

    ss_refresh(&ui->ssDisplay); ///< Refresh the seven segment display
    events[0] = NONE; ///< set time event
    events[1] = NONE; ///< set alarm event
    events[2] = NONE; ///< plus event
    events[3] = NONE; ///< minus event
    events[4] = NONE; ///< snooze event
    events[5] = NONE; ///< show date event
    switch (state)
    {
    case WATCH_UI_STATE_NORMAL:
        events[0] = pb_get_event(&ui->pbSetTime);      ///< Process push button for setting time
        events[1] = pb_get_event(&ui->pbSetAlarm);     ///< Process push button for setting alarm
        events[4] = pb_get_event(&ui->pbSnooze);       ///< Process push button for snoozing alarms
        events[5] = pb_get_event(&ui->pbShowDate);     ///< Process push button for showing date
        sLED_process_blink(&ui->ledHourUP);  ///< Process smart LED for hour increment indication
        sLED_process_blink(&ui->ledHourDOWN);///< Process smart LED for hour decrement indication
        break;
    case WATCH_UI_STATE_SET_TIME:
        // Handle setting time state
        events[0] = pb_get_event(&ui->pbSetTime);
        events[2] = pb_get_event(&ui->pbPlus);
        events[3] = pb_get_event(&ui->pbMinus);
        break;
    case WATCH_UI_STATE_SET_ALARM:
        // Handle setting time state
        events[1] = pb_get_event(&ui->pbSetAlarm);
        events[2] = pb_get_event(&ui->pbPlus);
        events[3] = pb_get_event(&ui->pbMinus);
        break;
    case WATCH_UI_STATE_SET_SNOOZE:
        // Handle setting snooze state
        events[4] = pb_get_event(&ui->pbSnooze);
        events[2] = pb_get_event(&ui->pbPlus);
        events[3] = pb_get_event(&ui->pbMinus);
        break;
    case WATCH_UI_STATE_ALARM:
        // Handle alarm state
        events[1] = pb_get_event(&ui->pbSetAlarm);     ///< Process push button for setting alarm
        events[4] = pb_get_event(&ui->pbSnooze);       ///< Process push button for snoozing alarms
        sLED_process_blink(&ui->ledHourUP);  ///< Process smart LED for alarm indication
        sLED_process_blink(&ui->ledHourDOWN);///< Process smart LED for hour decrement indication
        buzzer_process_ring(&ui->buzzer);       ///< Process smart buzzer for audio feedback
        break;
    case WATCH_UI_STATE_SNOOZE:
        events[1] = pb_get_event(&ui->pbSetAlarm);     ///< Process push button for setting alarm
        sLED_process_blink(&ui->ledAlarm);  ///< Process smart LED for snooze indication
        sLED_process_blink(&ui->ledHourUP);  ///< Process smart LED for hour increment indication
        sLED_process_blink(&ui->ledHourDOWN);///< Process smart LED for hour decrement indication
        break; // Handle snooze state, if needed
    default:
        break;
    }
}   

#endif