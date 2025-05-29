/**
 * \file SevenSegments.h
 * \brief
 * \author RAVV
 * \date
 * \version
 * \todo Dimerization using PWM signals to drive the LED segments
 */



 #ifndef __SEVEN_SEGMENTS_H
 #define __SEVEN_SEGMENTS_H

 #include "TimeBase.h"
 #include "hardware/gpio.h"
 #include <stdint.h>

 #define SS_MAXD 18 ///< Maximum number of Displays for RPP
 #define SS_DOFF_CC 0x00 ///< segments code to turn off display in a common cathode
 #define SS_DOFF_CA 0xFF ///< segments code to turn off display in a common anode



typedef enum{COMMON_CATHODE, COMMON_ANODE} ss_type_t;

/**
 * \brief Status and control information for a visualizer compose of multiple seven segment displays
 * 
 */
typedef struct{ 
    ss_type_t type;             ///< Type of display: common anode or common cathode
    uint8_t numD;               ///< total number of displays
    uint8_t display;            ///< current display being multiplexed
    uint32_t segMask;           ///< 8 Gpios used to drive display segments in any position
    uint32_t disMask;           ///< numD Gpios used to multiplexe displays in any position
    uint32_t disOff;            ///< Segment value to turn display OFF
    uint32_t enMask;            ///< Mask with active displays
    uint16_t refFreq;           ///< Multiplexation frequency, default frequency set to 60*numD
    uint8_t array[32];          ///< Array with the segment code for each display [0]-LSD, ... , [numD-1] - MSD
    uint8_t segPosArray[8];     ///< Array with GPIO number for each segment 0-a, 1-b, .. 7-p
    uint8_t disPosArray[32];    ///< Array with GPIO number for each display control signal [0]-LSD, ... , [numD-1] - MSD 
    uint32_t bcd2SSDeco[32];    ///< Array with 7 segment codes in user provided segment positions
    uint32_t muxSeq[32];        ///< Array with 7 segment codes in user provided segment positions
    uint16_t blinkFreq;         ///< Blink Frequency for displays with blinking activated
    uint32_t blinkMask;         ///< Mask to enable blinking feature in the asociate display
    bool blinkState;            ///< Blink state, true if display ON and false if display OFF
    time_base_t ssRefreshTB;    ///< Time base for multiplexing
    time_base_t ssBlinkTB;      ///< Time base for blinking
}ss_config_t;

/**
 * \fn void ss_init(ss_config_t *SS, uint8_t NumD, ss_type_t type, uint32_t segMask, uint32_t disMask)
 * \brief Initialize the seven segments data structure
 * \param SS        pointer to seven segments displays data structure
 * \param NumD      total number of seven segment displays
 * \param type      Display type: common anode (COMMON_ANODE) or common cathode (COMMON_CATHODE)
 * \param segMask   32 bit mask indicating the 8 GPIOs used for segments (p-MSB, ... , a-LSB)
 * \param disMask   32 bit mask indicating the NumD GPIOS for controlling/multiplexing the displays 
 */
void ss_init(ss_config_t *SS, uint8_t NumD, ss_type_t type, uint32_t segMask, uint32_t disMask);

/**
 * \fn void ss_refresh(ss_config_t *SS)
 * \brief Execute the seven segment display refreshing/multiplexing at a constant frequency
 * \param SS        pointer to seven segments displays data structure
 */
void ss_refresh(ss_config_t *SS);

/**
 * \fn static inline void ss_start_refresh(ss_config_t *SS)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 */
static inline void ss_start_refresh(ss_config_t *SS){
    tb_update(&SS->ssRefreshTB);
    tb_enable(&SS->ssRefreshTB);
}

/**
 * \fn static inline void ss_stop_refresh(ss_config_t *SS)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 */
static inline void ss_stop_refresh(ss_config_t *SS){
    tb_disable(&SS->ssRefreshTB);
}

/**
 * \fn static inline void ss_turn_off(ss_config_t *SS)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 */
static inline void ss_turn_off(ss_config_t *SS){
    tb_disable(&SS->ssRefreshTB);
    gpio_put_masked(SS->disMask,0x00000000);        ///< Turn off all display
    gpio_put_masked(SS->segMask,SS->disOff);        ///< Let's ensure all segments off
}

/**
 * \fn static inline void ss_turn_on(ss_config_t *SS)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 */
static inline void ss_turn_on(ss_config_t *SS){
    tb_update(&SS->ssRefreshTB);
    tb_enable(&SS->ssRefreshTB);
    SS->enMask = SS->disMask;
    SS->blinkMask &= ~ SS->disMask;
}

/**
 * \fn static inline void ss_update_value(ss_config_t *SS, uint8_t digit, uint8_t value)
 * \brief
 * \param SS        Pointer to seven segments displays data structure
 * \param digit     Digit number from right to left
 * \param value     Value to show on the respective digit 
 */
static inline void ss_update_value(ss_config_t *SS, uint8_t digit, uint8_t value){
    assert((digit < SS->numD) && "ERROR!!! the digit isn't configured");
    assert(value<32 && "Error!!! Value not valid");
    SS->array[digit] = SS->bcd2SSDeco[value];
}

/**
 * \fn static inline void ss_set_blink_mask(ss_config_t *SS, uint32_t mask)
 * \brief
 * \param SS        Pointer to seven segments displays data structure
 * \param mask      Bit mask with ones on the positions of the digits that should blink
 */
static inline void ss_set_blink_mask(ss_config_t *SS, uint32_t mask){
    assert(!(mask>>SS->numD)&& "ERROR!!! One or more digits in the mask don't configured");
    SS->blinkMask = mask;
}

/**
 * \fn static inline void ss_set_blink(ss_config_t *SS, uint8_t digit)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 * \param digit     Number from right to left of the digit that will set to blink
 */
static inline void ss_set_blink(ss_config_t *SS, uint8_t digit){
    assert((digit < SS->numD) && "ERROR!!! the digit isn't configured");
    SS->blinkMask |= (1<<digit);
}

/**
 * \fn static inline void ss_set_refresh_freq(ss_config_t *SS, uint16_t freq)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 * \param freq      Multiplexing frequency
 */
static inline void ss_set_refresh_freq(ss_config_t *SS, uint16_t freq){
    SS->refFreq = freq;
    SS->ssRefreshTB.delta = 1000000/freq;
}

/**
 * \fn static inline void ss_set_blink_freq(ss_config_t *SS, uint16_t freq)
 * \brief
 * \param SS        Pointer to seven segments displays data structure
 * \param freq      Blinking frequenct
 */
static inline void ss_set_blink_freq(ss_config_t *SS, uint16_t freq){
    SS->blinkFreq = freq;
    SS->ssBlinkTB.delta = 1000000/freq;
}

/**
 * \fn static inline void ss_enable_display_mask(ss_config_t *SS, uint32_t mask)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 * \param mask      Bit mask with ones on the position of the digits that will be enabled
 */
static inline void ss_enable_display_mask(ss_config_t *SS, uint32_t mask){
    assert(!(((1 << SS->numD)-1) ^ mask)&& "ERROR!!! One or more digits in the mask don't configured");
    SS->enMask = mask;
}

/**
 * \fn static inline void ss_enable_display(ss_config_t *SS, uint8_t digit)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 * \param digit     Number from right to left of the digit that must be enabled
 */
static inline void ss_enable_display(ss_config_t *SS, uint8_t digit){
    assert((digit < SS->numD) && "ERROR!!! the digit isn't configured");
    SS->blinkMask |= (1<<digit);
}

/**
 * \fn static inline void ss_disable_display_mask(ss_config_t *SS, uint32_t mask)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 * \param mask      Bit mask with ones on the position of the digits that will be disabled
 */
static inline void ss_disable_display_mask(ss_config_t *SS, uint32_t mask){
    assert(!(((1 << SS->numD)-1) ^ mask)&& "ERROR!!! One or more digits in the mask don't configured");
    SS->enMask = mask;
}

/**
 * \fn static inline void ss_disable_display(ss_config_t *SS, uint8_t digit)
 * \brief
 * \param SS        pointer to seven segments displays data structure
 * \param digit     Number from right to left of the digit that must be enabled
 */
static inline void ss_disable_display(ss_config_t *SS, uint8_t digit){
    assert((digit < SS->numD) && "ERROR!!! the digit isn't configured");
    SS->blinkMask |= (1<<digit);
}

 #endif