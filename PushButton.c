/**
 * \file PushButton.c
 * \brief
 * \author RAVV
 * \date 05/22/2025
**/

#include <stdint.h>
#include "PushButton.h"
#include "TimeBase.h"
#include "hardware/gpio.h"

void PBCatchEventFSM(void *ptr);
void PBDebounceFSM1(void *ptr);
void PBDebounceFSM2(void *ptr); 
void PBDebounceFSM3(void *ptr);
void PBCatchEventNFSM(void *ptr);

void pb_init(push_button_t *PB, uint8_t gpioNum, uint8_t alarmNum, uint8_t pwmNum){
    PB->BITS.alarmNum = alarmNum;
    PB->BITS.debON = false;
    PB->BITS.debT_ms = 20;
    PB->BITS.eventCnt = 0;
    PB->BITS.eventT_ms = 1000;
    PB->BITS.gpioNum = gpioNum;
    PB->BITS.pwmNum = pwmNum;
    tb_init(&(PB->pbTBDebouncer), 20000, false);
    tb_init(&(PB->pbTBEvent),1000000,false);
    PB->PBProcess = PBCatchEventFSM;
    PB->PBEvent = NONE;

    gpio_init(gpioNum);
    gpio_set_dir(gpioNum,false);
    gpio_set_function(gpioNum,GPIO_FUNC_SIO);
    gpio_set_pulls(gpioNum,false,true);
    gpio_set_input_enabled(gpioNum,true);
    gpio_set_input_hysteresis_enabled(gpioNum,true);
}


pb_event_t pb_get_event(push_button_t *PB){
    switch (PB->BITS.eventCnt)
    {
    case 0:
        PB->PBEvent = NONE;
        break;
    case 1:
        PB->PBEvent = ONCE;
        break;
    case 2:
        PB->PBEvent = TWICE;
        break;
    default:
        PB->PBEvent = MORE;
        break;
    }
    return PB->PBEvent;
}



/**
 * \fn
 * \brief
 * \param PB
 * \returns 
 */
void PBCatchEventFSM(void *ptr){
    push_button_t *PB = (push_button_t *)ptr;
    bool gpioValue = gpio_get(PB->BITS.gpioNum);
    if(gpioValue){
        PB->BITS.debON = true;
        PB->BITS.eventON = true;
        tb_update(&(PB->pbTBEvent));
        tb_update(&(PB->pbTBDebouncer));
        tb_enable(&(PB->pbTBEvent));
        tb_enable(&(PB->pbTBDebouncer));
        PB->BITS.eventCnt += 1;
        PB->PBProcess = PBDebounceFSM1;
        gpio_acknowledge_irq(PB->BITS.gpioNum,GPIO_IRQ_EDGE_RISE);
    }
}

/**
 * \fn
 * \brief
 * \param PB
 * \returns 
 */
void PBDebounceFSM1(void *ptr){
    push_button_t *PB = (push_button_t *)ptr;
    if(tb_check(&(PB->pbTBDebouncer))){
        uint32_t gpioEvent = gpio_get_irq_event_mask(PB->BITS.gpioNum);
        gpio_acknowledge_irq(PB->BITS.gpioNum,GPIO_IRQ_EDGE_RISE);
        if(!gpioEvent){
            PB->PBProcess = PBDebounceFSM2;
        }
        else{
            tb_update(&(PB->pbTBDebouncer));
            tb_enable(&(PB->pbTBDebouncer));
        }
    }
}

/**
 * \fn
 * \brief
 * \param PB
 * \returns 
 */
void PBDebounceFSM2(void *ptr){
    push_button_t *PB = (push_button_t *)ptr;
    bool gpioValue = gpio_get(PB->BITS.gpioNum);
    if(!gpioValue){
        PB->PBProcess = PBDebounceFSM3;
        tb_update(&(PB->pbTBDebouncer));
        tb_enable(&(PB->pbTBDebouncer));
    }
}

/**
 * \fn
 * \brief
 * \param PB
 * \returns 
 */
void PBDebounceFSM3(void *ptr){
    push_button_t *PB = (push_button_t *)ptr;
    if(tb_check(&(PB->pbTBDebouncer))){
        uint32_t gpioEvent = gpio_get_irq_event_mask(PB->BITS.gpioNum);
        gpio_acknowledge_irq(PB->BITS.gpioNum,GPIO_IRQ_EDGE_RISE);
        if(!gpioEvent){
            PB->PBProcess = PBCatchEventNFSM;
            PB->BITS.debON = false;
        }
        else{
            tb_update(&(PB->pbTBDebouncer));
            tb_enable(&(PB->pbTBDebouncer));
        }
    }
}

/**
 * \fn
 * \brief
 * \param PB
 * \returns 
 */
void PBCatchEventNFSM(void *ptr){
    push_button_t *PB = (push_button_t *)ptr;
    if(tb_check(&(PB->pbTBEvent))){
        PB->BITS.eventON = false;
        PB->PBProcess = PBCatchEventFSM;
    }
    else{
        bool gpioValue = gpio_get(PB->BITS.gpioNum);
        if(gpioValue){
            PB->BITS.debON = true;
            tb_update(&(PB->pbTBDebouncer));
            tb_enable(&(PB->pbTBDebouncer));
            PB->BITS.eventCnt += 1;
            PB->PBProcess = PBDebounceFSM1;
            gpio_acknowledge_irq(PB->BITS.gpioNum,GPIO_IRQ_EDGE_RISE);
        }
    }
}