#include <stdio.h>
#include "pico/stdlib.h"
#include "PushButton.h"
#include "SevenSegments.h"
#include "SmartBuzzer.h"
#include "SmartLED.h"


void main(void)
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
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

void testPB(uint8_t numGPIO){
    push_button_t PB;
    pb_init(&PB,0,0,numGPIO);
    printf("TESTING PUSH BUTTON!!!!\n");
    sleep_ms(1000);
    printf("Let's capture an event ONCE.\n\n Please press push button once...\n");
    pb_event_t epb = NONE;
    time_base_t tout;
    tb_init(&tout,5000000,true);
    while(epb==NONE){
        PB.PBProcess(&PB);
        epb = pb_get_event(&PB);
        if(tb_check(&tout))
            break;
    }
    if(epb==ONCE){
        printf("Well done!!! Event detected");
    }
    else{
        printf("We might have a problem!!! ONCE event wasn't detected in the last 5 seconds");
    }
    sleep_ms(1000);

    printf("Let's capture an event TWICE.\n\n Please press push button twice in less than one second...\n");
    epb = NONE;
    tb_init(&tout,5000000,true);
    while(epb==NONE){
        PB.PBProcess(&PB);
        epb = pb_get_event(&PB);
        if(tb_check(&tout))
            break;
    }
    if(epb==TWICE)
    {
        
        printf("Well done!!!  Event detected");
    }
    else{

        printf("We might have a problem!!! TWICE event wasn't detected in the last 5 seconds");
    }
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

void testSSDisplay(uint32_t segMask, uint32_t disMask, uint8_t numD){

}