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

 const uint8_t SS_CODES_CC[] = {
    // abcdefgp
     0b11111100,  // 0 0
     0b01100000,  // 1 1
     0b11011010,  // 2 2
     0b11110010,  // 3 3
     0b01100110,  // 4 4
     0b10110110,  // 5 5 y S
     0b10111110,  // 6 6
     0b11100000,  // 7 7
     0b11111110,  // 8 8
     0b11110110,  // 9 9
     0b11101110,  // 10 A
     0b00111110,  // 11 b
     0b10011100,  // 12 C
     0b01111010,  // 13 d
     0b10011110,  // 14 E
     0b10001110,  // 15 F
     0b11011110,  // 16 g
     0b00101010,  // 17 h
     0b01100000,  // 18 I
     0b11110000,  // 19 J
     0b00011100,  // 20 L
     0b00101010,  // 21 n
     0b00111010,  // 22 o
     0b11001110,  // 23 P
     0b11100110,  // 24 q
     0b00001010,  // 25 r
     0b00011110,  // 26 t
     0b00111000,  // 27 u
     0b01111100   // 28 U
 };

 const uint8_t SS_CODES_CA[] = {
    // abcdefgp
     0b00000011,  // 0 0
     0b10011111,  // 1 1
     0b00100101,  // 2 2
     0b00001101,  // 3 3 CA
     0b10011001,  // 4 4 CA
     0b01001001,  // 5 5 y S CA
     0b01000001,  // 6 6 CA
     0b00011111,  // 7 7 CA
     0b00000001,  // 8 8 CA
     0b00001001,  // 9 9 CA
     0b00010001,  // 10 A CA
     0b11000001,  // 11 b CA
     0b01100011,  // 12 C CA
     0b10000101,  // 13 d CA
     0b01100001,  // 14 E CA
     0b01110001,  // 15 F CA
     0b00100001,  // 16 g CA
     0b11010101,  // 17 h CA
     0b10011111,  // 18 I CA
     0b00001111,  // 19 J CA
     0b11100011,  // 20 L CA
     0b11010101,  // 21 n CA
     0b11000101,  // 22 o CA
     0b00110001,  // 23 P CA
     0b00011001,  // 24 q CA
     0b11110101,  // 25 r CA
     0b11100001,  // 26 t CA
     0b11000111,  // 27 u CA
     0b10000011   // 28 U CA
 };

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
