/**
 * \file SevenSegments.c
 * \brief
 * \author RAVV
 * \version
 * \date
 * \copyright
 */

#include <stdint.h>
#include "SevenSegments.h"
#include "hardware/gpio.h"

void ss_init(ss_config_t *SS, uint8_t NumD, ss_type_t type, uint32_t segMask, uint32_t disMask){
    SS->type = type;
    assert(NumD <= SS_MAXD && "ERROR!!! The number of displays is not supported");
    SS->numD = NumD;       // total number of displays
    assert((segMask|disMask)==(segMask^disMask) && "Control GPIOs overlap with Segment GPIOs");
    SS->segMask = segMask;   // 8 Gpios used to drive display segments in any position a-MSB and p-LSB
    SS->disMask = disMask;   // numD Gpios used to multiplexe displays in any position 0-MSB and NumC-LSB
    SS->display = 0x00000001;   // current display
    SS->enMask = (1<<NumD) - 1;    // Mask with active displays, default all display active
    SS->refFreq = NumD*60;   // Multiplexation frequency, default frequency set to 60*numD
    const uint8_t* ptr;
    if(type==COMMON_ANODE){
        ptr = SS_CODES_CA;
        SS->disOff = SS_DOFF_CA;
    }
    else{
        ptr = SS_CODES_CC;
        SS->disOff = SS_DOFF_CC;
    }



    // SS->segPosArray[8] with GPIO number for each segment 0-a, 1-b, .. 7-p
    uint32_t mask = segMask;
    uint8_t cnt = 0, pos = 0;
    while(mask){
        if(mask & 0x00000001){
            SS->segPosArray[cnt] = pos;
            cnt++;
        }
        pos++;
        mask = mask >> 1;
        assert(cnt<8 && "ERROR!!! There are more than 8 segments");
    }
    assert(cnt<8 && "ERROR!!! There are missing GPIOS for the 8 segments in each display");

    // SS->disPosArray[32] with GPIO number for each display control signal [0]-LSD, ... , [numD-1] - MSD
    mask = disMask;
    cnt =0;
    pos = 0;
    while(mask){
        if(mask & 0x00000001){
            SS->disPosArray[cnt] = pos;
            cnt++;
        }
        pos++;
        mask == mask >> 1;
        assert(cnt<NumD && "There are more control GPIOs than displays to control");
    }
    assert(cnt==NumD-1 && "There are missing GPIOs for controlling each display");

    for(int i=0;i<28;i++){
        uint32_t temp = 0;
        for(int j=0;j<8;j++){
             temp |= ((ptr[i]>>j)&0x00000001)<<(SS->segPosArray[j]);
        }
        SS->bcd2SSDeco[i] = temp;
    }

    for(int i=0;i<NumD;i++){
        SS->array[i] = SS->bcd2SSDeco[0];
    }

    for(int i=0;i<NumD;i++){
        SS->muxSeq[i] = 0x000000001 << SS->disPosArray[i];
    }

    SS->blinkMask = 0x00000000;
    SS->blinkState = false;
    SS->blinkFreq = 1;

    tb_init(&(SS->ssRefreshTB),1000000/SS->refFreq,false);
    tb_init(&(SS->ssBlinkTB),1000000/SS->blinkFreq,false);

    // Initialize GPIOs to drive segments
    gpio_init_mask(SS->segMask);
    gpio_set_dir_masked(SS->segMask,SS->segMask);
    gpio_put_masked(SS->segMask,SS->disOff); 
    for(int i=0;i<8;i++){
        gpio_set_drive_strength(SS->segPosArray[i],GPIO_DRIVE_STRENGTH_12MA);
    }
    // Initialize GPIOs to control displays
    gpio_init_mask(SS->disMask);
    gpio_set_dir_masked(SS->disMask,SS->disMask);
    gpio_put_masked(SS->disMask,0x00000000);
}

void ss_refresh(ss_config_t *SS){

    if(tb_check(&(SS->ssRefreshTB))){                       ///< Refresh displays at the every refresh time base event
        tb_next(&(SS->ssRefreshTB));                        ///< Update refresh time base for next event
        if(tb_check(&(SS->ssBlinkTB))){                     ///< Verify blink period event
            SS->blinkState = !(SS->blinkState);          ///< Update blink state true display on and false display off
            tb_next(&(SS->ssBlinkTB));                      ///< Update blink time base for next event
        }
        uint32_t muxMask = SS->enMask;                      ///< Register in muxMask only the displays on
        if(!SS->blinkState)                                 ///< When blinkState false
            muxMask &= ~(SS->blinkMask);                    ///< Update muxMask erasing from the mask the displays blinking 

        if(muxMask){                                        ///< When there is at least one display to show
            uint8_t cnt = ((SS->display)+1)%(SS->numD);     ///< Compute next display to show
            while(!(muxMask & (0x00000001<<cnt)))           ///< While next display disable
                cnt = (cnt+1)%(SS->numD);                   ///< continue searching for display to refresh
            gpio_put_masked(SS->segMask,SS->disOff);        ///< Turn off segments before moving to new display
            gpio_put_masked(SS->disMask,SS->muxSeq[cnt]);   ///< Next display is now current display
            gpio_put_masked(SS->segMask,SS->array[cnt]);    ///< Write segments value to current display
            SS->display = cnt;
        }
        else{                                               ///< when there are not display to show
            gpio_put_masked(SS->disMask,0x00000000);        ///< Turn off all display
            gpio_put_masked(SS->segMask,SS->disOff);        ///< Let's ensure all segments off
        }
    }
}
