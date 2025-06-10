/**
 * \file PushButton.h
 * \brief
 * \author RAVV
 */

#ifndef __PUSH_BUTTON_H
#define __PUSH_BUTTON_H


#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "TimeBase.h"

 typedef enum{NONE, ONCE, TWICE, MORE} pb_event_t;

typedef struct{
    struct{
        uint8_t gpioNum     : 5; ///< GPIO number for the push button
        uint8_t alarmNum    : 2; ///< Timer alarm number associated with the push button
        uint8_t pwmNum      : 3;
        bool eventON        : 1;
        uint16_t eventT_ms  : 16;
        bool debON          : 1;
        uint8_t debT_ms     : 8;
        uint8_t eventCnt    : 3;
    } BITS;
    time_base_t pbTBEvent;
    time_base_t pbTBDebouncer;
    void (*PBProcess) (void * PB);
    pb_event_t PBEvent;
} push_button_t;

/**
 * \fn void pb_init(uint8_t gpioNum, uint8_t alarmNum, uint8_t pwmNum, push_button_t *PB)
 * \brief Initialize a push button to detect pulses in a defined period of time
 * 
 * \param gpioNum GPIO that serves as input to the push button
 * \param alarmNum Timer alarm associated to the push button (used for event period)
 * \param pwmNum PWM slice number associated to the push button (used for debouncer)
 * \param PB Pointer to push button data structure
 */
void pb_init(push_button_t *PB, uint8_t gpioNum, uint8_t alarmNum, uint8_t pwmNum);

/**
 * \fn pb_event_t pb_get_event(push_button_t *PB)
 * \brief
 * \param PB Pointer to push button data structure
 * \returns 
 */
pb_event_t pb_get_event(push_button_t *PB);

/**
 * \fn static inline void pb_clear_event(push_button_t *PB)
 * \brief Call this metod to clear last event
 * \param PB Pointer to push button data structure
 * \returns the event type detected during the event period (NONE, ONCE, TWICE o MORE)
 */
static inline void pb_clear_event(push_button_t *PB){
    PB->BITS.eventCnt = 0;
}

/**
 * \fn static inline void pb_set_event_period(push_button_t *PB, uint16_t T)
 * \brief
 * \param PB Pointer to push button data structure
 * \returns none
 */
static inline void pb_set_event_period(push_button_t *PB, uint16_t T){
    PB->BITS.eventT_ms = T;
}

/**
 * \fn static inline void pb_set_debouncer_period(push_button_t *PB, uint16_t T)
 * \brief Set the debouncer period, the time the signal must remain witout bouncing
 * \param PB Pointer to push button data structure
 * \returns none
 */
static inline void pb_set_debouncer_period(push_button_t *PB, uint16_t T){
    PB->BITS.debT_ms = T;
}

void pb_test(uint8_t numGPIO);

 #endif
