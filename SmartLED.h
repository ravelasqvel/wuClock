/**
 * \file        SmartLED.h
 * \brief       Define some utility methods to control a LED through a GPIO
 * \details
 * \author      Ricardo Andres Velasquez Velez
 * \version     0.0.1
 * \date        05/10/2023
 * \copyright   Unlicensed
 */

#ifndef __SMART_LED_H__
#define __SMART_LED_H__

#include <stdint.h>
#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "TimeBase.h"

typedef struct{
    uint8_t numGPIO;            ///< GPIO to drive the LED
    uint8_t blinkFreq;          ///< Blink frequency in Hz
    uint32_t pulsePeriod;        ///< Pulse period in us
    time_base_t blinkTB;        ///< time base for managing blinking feature
    time_base_t pulseTB;        ///< time base for controling pulse feature
}smart_led_t;

/// @brief Initialize a gpio to drive a LED
/// @param SL Pointer to smart led data structure
void sLED_init(smart_led_t * SL, uint8_t numGPIO){
    SL->numGPIO = numGPIO;
    SL->blinkFreq = 1;
    SL->pulsePeriod = 1000000;
    gpio_init( SL->numGPIO); // gpios for key rows 2,3,4,5
    gpio_set_drive_strength(SL->numGPIO,GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_dir(SL->numGPIO,true); // rows as outputs and cols as inputs
    gpio_put(SL->numGPIO,false);

    tb_init(&SL->blinkTB,1000000/(2*SL->blinkFreq),false);
    tb_init(&SL->pulseTB,1000000,false);
}

/**
 * \fn void sLED_process(smart_led_t * SL)
 * \brief call this method in the state or main loop to process both pulse and blinking features
 * \param SL Pointer to smart led data structure
 */

void sLED_process(smart_led_t * SL){
    if(tb_check(&SL->blinkTB)){                     ///< process blinking
        gpio_xor_mask(0x00000001 << SL->numGPIO);
        tb_next(&SL->blinkTB);
    }
    if(tb_check(&SL->pulseTB)){                     ///< process pulse
        gpio_xor_mask(0x00000001 << SL->numGPIO);
        tb_disable(&SL->pulseTB);
    }
}

/**
 * \fn void sLED_process_blink(smart_led_t * SL)
 * \brief call this method in a FSM state or main loop to process only the blinking feature
 * \param SL Pointer to smart led data structure
 */
void sLED_process_blink(smart_led_t * SL){
    if(tb_check(&SL->blinkTB)){
        gpio_xor_mask(0x00000001 << SL->numGPIO);
        tb_next(&SL->blinkTB);
    }
}
/**
 * \fn void sLED_process_pulse(smart_led_t * SL)
 * \brief call this method in a FSM state or main loop to process only the pulse feature
 * \param SL Pointer to smart led data structure
 */
void sLED_process_pulse(smart_led_t * SL){
    if(tb_check(&SL->pulseTB)){
        gpio_xor_mask(0x00000001 << SL->numGPIO);
        tb_disable(&SL->pulseTB);
    }
}

/**
 * \fn static inline void sLED_on(smart_led_t * SL)
 * \brief call this method to turn ON the LED
 * \param SL Pointer to smart led data structure
 */
static inline void sLED_on(smart_led_t * SL){
    gpio_put(SL->numGPIO,true);
}
/**
 * \fn static inline void sLED_off(smart_led_t * SL)
 * \brief call this method to turn OFF the LED
 * \param SL Pointer to smart led data structure
 */
static inline void sLED_off(smart_led_t * SL){
    gpio_put(SL->numGPIO,false);
}

/**
 * \fn static inline void sLED_toggle(smart_led_t * SL)
 * \brief call this method to toggle the LED state (ON->OFF or OFF->ON)
 * \param SL Pointer to smart LED data structure
 */
static inline void sLED_toggle(smart_led_t * SL){
    gpio_xor_mask(0x00000001 << SL->numGPIO);
}

/**
 * \fn static inline void sLED_pulse(smart_led_t * SL)
 * \brief Call this method to start a light pulse on the LED.
 * \details If LED is ON the light will go OFF for period given by the configured pulse period.
 * If LED is OFF the light will go ON forperiod given by the configured pulse period.
 * \param SL Pointer to smart LED data structure
 */
static inline void sLED_pulse(smart_led_t * SL){
    tb_update(&SL->pulseTB);
    tb_enable(&SL->pulseTB);
    gpio_xor_mask(0x00000001 << SL->numGPIO);
}

/**
 * \fn static inline void sLED_start_blink(smart_led_t * SL)
 * \brief Call this method to make the LED blink with the configured blinking frequency.
 * \param SL Pointer to smart LED data structure
 */
static inline void sLED_start_blink(smart_led_t * SL){
    tb_update(&SL->blinkTB);
    tb_enable(&SL->blinkTB);
}

/**
 * \fn static inline void sLED_stop_blink(smart_led_t * SL, bool value)
 * \brief Call this method to make the LED stop blinking.
 * \param SL Pointer to smart LED data structure
 * \param value final state of the LED after blinking TRUE->ON, FALSE->OFF
 */
static inline void sLED_stop_blink(smart_led_t * SL, bool value){
    tb_disable(&SL->blinkTB);
    gpio_put(SL->numGPIO,value);
}

/**
 * \fn static inline void sLED_set_pulse_period(smart_led_t * SL,uint32_t period)
 * \brief Call this method to setup the pulse period.
 * \param SL Pointer to smart LED data structure
 * \param period New pulse period in us. By default the period is set to 1000000 us.
 */
static inline void sLED_set_pulse_period(smart_led_t * SL,uint32_t period){
    SL->pulsePeriod = period;
    SL->pulseTB.delta = period;
}

/**
 * \fn static inline void sLED_set_blink_freq(smart_led_t * SL,uint32_t freq)
 * \brief Call this method to setup the blink frequency.
 * \param SL Pointer to smart LED data structure
 * \param freq New blink frequency in Hz. By default the frequency is set to 1 Hz.
 */
static inline void sLED_set_blink_freq(smart_led_t * SL,uint32_t freq){
    SL->blinkFreq = freq;
    SL->blinkTB.delta = 1000000/(2*freq);
}

void testLED(uint8_t numGPIO){
    smart_led_t SL;
    sLED_init(&SL,numGPIO);
    printf("TESTING LED!!!!\n");
    sleep_ms(1000);
    printf("Turn ON LED\n");
    sLED_on(&SL);
    sleep_ms(2000);
    printf("Turn OFF LED\n");
    sLED_off(&SL);
    sleep_ms(2000);
    printf("Toggle LED 10 Times\n");
    for(int i=0;i<10;i++){
        sLED_toggle(&SL);
        sleep_ms(1000);
    }
    
    printf("Light ON Pulse 3 seconds\n");
    sLED_off(&SL);
    sLED_set_pulse_period(&SL,3000000);
    sLED_pulse(&SL);
    uint16_t cnt = 0;
    while(cnt<=10000){
        sLED_process_pulse(&SL);
        cnt++;
        printf("%d sec\n",cnt);
        sleep_ms(10);
    }

    printf("Light OFF Pulse 5 seconds\n");
    sLED_on(&SL);
    sLED_set_pulse_period(&SL,5000000);
    sLED_pulse(&SL);
    cnt = 0;
    while(cnt<=10000){
        sLED_process_pulse(&SL);
        cnt++;
        if(!(cnt%1000))
            printf("%d sec\n",cnt);
        sleep_ms(10);
    }

    printf("LED blinking during 10 seconds at 2 Hz, and it finishes ON");
    sLED_off(&SL);
    sLED_set_blink_freq(&SL,2);
    sLED_start_blink(&SL);
    cnt = 0;
    while(cnt<=12000){
        sLED_process_blink(&SL);
        cnt++;
        if(!(cnt%1000))
            printf("%d sec\n",cnt);
        sleep_ms(10);
        if(cnt==10000){
            sLED_stop_blink(&SL,true);
        }
    }

    printf("LED blinking during 5 seconds at 4 Hz, and it finishes OFF");
    sLED_off(&SL);
    sLED_set_blink_freq(&SL,4);
    sLED_start_blink(&SL);
    cnt = 0;
    while(cnt<=10000){
        sLED_process_blink(&SL);
        cnt++;
        if(!(cnt%1000))
            printf("%d sec\n",cnt);
        sleep_ms(10);
        if(cnt==5000){
            sLED_stop_blink(&SL,false);
        }
    }
}

#endif