/**
 * \file        SmartBuzzer.h
 * \brief       Define some utility methods to control a LED through a GPIO
 * \details
 * \author      Ricardo Andres Velasquez Velez
 * \version     0.0.1
 * \date        05/10/2023
 * \copyright   Unlicensed
 */

#ifndef __SMART_BUZZER_H__
#define __SMART_BUZZER_H__

#include <stdint.h>
#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "TimeBase.h"

typedef struct{
    uint8_t numGPIO;            ///< GPIO to drive the LED
    uint8_t ringFreq;          ///< Blink frequency in Hz
    uint32_t beepPeriod;        ///< Pulse period in us
    time_base_t ringTB;        ///< time base for managing blinking feature
    time_base_t beepTB;        ///< time base for controling pulse feature
}buzzer_t;

/**
 * \fn void buzzer_init(buzzer_t * B, uint8_t numGPIO)
 * \brief Initialize the buzzer data struct: members, GPIO, time bases 
 * \param B Pointer to the buzzer data structure
 * \param numGPIO GPIO used to drive the buzzer
 */

void buzzer_init(buzzer_t * B, uint8_t numGPIO){
    B->numGPIO = numGPIO;                                           ///< Needed for all GPIO operations
    B->ringFreq = 1;                                                ///< Ring frequency is set to the default value of 2Hz
    B->beepPeriod = 1000000;                                        ///< Beep period is set to the default value of 1000000 us (1 s)
    gpio_init( B->numGPIO);                                         ///< Initialize GPIO
    gpio_set_drive_strength(B->numGPIO,GPIO_DRIVE_STRENGTH_12MA);   ///< Set driving current to the maximum value
    gpio_set_dir(B->numGPIO,true);                                  ///< Configure the GPIO to output direction
    gpio_put(B->numGPIO,false);                                     ///< Write 0 to the GPIO

    tb_init(&B->ringTB,1000000/(2*B->ringFreq),false);              ///< Initialize the time base for Ring feature
    tb_init(&B->beepTB,1000000,false);                              ///< Initialize the time base for Beep feature
}

/**
 * \fn void buzzer_process(buzzer_t * B)
 * \brief call this method in the state or main loop to process both Beep and Ringing features
 * \param B Pointer to the buzzer data structure
 */

void BED_process(buzzer_t * B){
    if(tb_check(&B->ringTB)){                     ///< process ringing
        gpio_xor_mask(0x00000001 << B->numGPIO);
        tb_next(&B->ringTB);
    }
    if(tb_check(&B->beepTB)){                     ///< process beep
        gpio_put(B->numGPIO, false);
        tb_disable(&B->beepTB);
    }
}

/**
 * \fn void buzzer_process_ring(buzzer_t * B)
 * \brief call this method in a FSM state or main loop to process only the ringing feature
 * \param B Pointer to the buzzer data structure
 */
void buzzer_process_ring(buzzer_t * B){
    if(tb_check(&B->ringTB)){
        gpio_xor_mask(0x00000001 << B->numGPIO);
        tb_next(&B->ringTB);
    }
}

/**
 * \fn void buzzer_process_beep(buzzer_t * B)
 * \brief call this method in a FSM state or main loop to process only the beep feature
 * \param B Pointer to the buzzer data structure
 */
void buzzer_process_beep(buzzer_t * B){
    if(tb_check(&B->beepTB)){
        gpio_put(B->numGPIO, false);
        tb_disable(&B->beepTB);
    }
}

/**
 * \fn static inline void buzzer_on(buzzer_t * B)
 * \brief call this method to turn ON the buzzer sound
 * \param B Pointer to the buzzer data structure
 */
static inline void buzzer_on(buzzer_t * B){
    gpio_put(B->numGPIO,true);
}
/**
 * \fn static inline void buzzer_off(buzzer_t * B)
 * \brief call this method to turn OFF the buzzer sound
 * \param B Pointer to the buzzer data structure
 */
static inline void buzzer_off(buzzer_t * B){
    gpio_put(B->numGPIO,false);
}

/**
 * \fn static inline void buzzer_beep_on(buzzer_t * B)
 * \brief Call this method to produce a short sound with a duration of beepPeriod.
 * \param B Pointer to the buzzer data structure
 */
static inline void buzzer_beep(buzzer_t * B){
    tb_update(&B->beepTB);
    tb_enable(&B->beepTB);
    gpio_put(B->numGPIO,true);
}

/**
 * \fn static inline void buzzer_start_ring(buzzer_t * B)
 * \brief Call this method to make the buzzer produce short beeps continously with frequency ringFreq
 * \param B Pointer to the buzzer data structure
 */
static inline void buzzer_start_ring(buzzer_t * B){
    tb_update(&B->ringTB);
    tb_enable(&B->ringTB);
    gpio_put(B->numGPIO,true);
}

/**
 * \fn static inline void buzzer_stop_ring(buzzer_t * B)
 * \brief Call this method to make the buzzer stop ringing.
 * \param B Pointer to the buzzer data structure
 */
static inline void buzzer_stop_ring(buzzer_t * B){
    tb_disable(&B->ringTB);
    gpio_put(B->numGPIO,false);
}

/**
 * \fn static inline void buzzer_set_beep_period(buzzer_t * B,uint32_t period)
 * \brief Call this method to setup the pulse period.
 * \param B Pointer to the buzzer data structure
 * \param period New pulse period in us. By default the period is set to 1000000 us.
 */
static inline void buzzer_set_beep_period(buzzer_t * B,uint32_t period){
    B->beepPeriod = period;
    B->beepTB.delta = period;
}

/**
 * \fn static inline void buzzer_set_ring_freq(buzzer_t * B,uint32_t freq)
 * \brief Call this method to setup the ring frequency.
 * \param B Pointer to the buzzer data structure
 * \param freq New ring frequency in Hz. By default the frequency is set to 1 Hz.
 */
static inline void buzzer_set_ring_freq(buzzer_t * B,uint32_t freq){
    B->ringFreq = freq;
    B->ringTB.delta = 1000000/(2*freq);
}

void testBuzzer(uint8_t numGPIO){
    buzzer_t B;
    buzzer_init(&B,numGPIO);
    printf("TESTING BUZZZER!!!\n");
    sleep_ms(1000);
    printf("Turn ON buzzer\n");
    buzzer_on(&B);
    sleep_ms(2000);
    printf("Turn OFF buzzer\n");
    buzzer_off(&B);
    sleep_ms(2000);
        
    printf("Buzzer Beep 0.5 seconds\n");
    buzzer_set_beep_period(&B,500000);
    buzzer_beep(&B);
    uint16_t cnt = 0;
    while(cnt<=2000){
        buzzer_process_beep(&B);
        cnt++;
        printf("%d sec\n",cnt);
        sleep_ms(10);
    }

    printf("Buzzer Beep 3 seconds\n");
    buzzer_set_beep_period(&B,3000000);
    buzzer_beep(&B);
    cnt = 0;
    while(cnt<=5000){
        buzzer_process_beep(&B);
        cnt++;
        printf("%d sec\n",cnt);
        sleep_ms(10);
    }

    printf("Buzzer rings during 10 seconds at 2 Hz");
    buzzer_set_ring_freq(&B,2);
    buzzer_start_ring(&B);
    cnt = 0;
    while(cnt<=12000){
        buzzer_process_ring(&B);
        cnt++;
        if(!(cnt%1000))
            printf("%d sec\n",cnt);
        sleep_ms(10);
        if(cnt==10000){
            buzzer_stop_ring(&B);
        }
    }
}

#endif