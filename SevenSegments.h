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

 #define SS_MAXD 18 ///< Maximum number of Displays for RPP
 #define SS_DOFF_CC 0x00 ///< segments code to turn off display in a common cathode
 #define SS_DOFF_CA 0xFF ///< segments code to turn off display in a common anode

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