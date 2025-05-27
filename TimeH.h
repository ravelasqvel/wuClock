/**
 * \file        TimeH.h
 * \brief       Define some methods to count the time and date as humans do. Uses the RTC module.
 * \details
 * \author      Ricardo Andres Velasquez Velez
 * \version     0.0.1
 * \date        05/27/2025
 * \copyright   Unlicensed 
 */

#ifndef __TIME_H_H_
#define __TIME_H_H_
#define PICO_INCLUDE_RTC_DATETIME

#include <stdint.h>
#include "hardware/rtc.h"
#include "pico/types.h"
#include "TimeBase.h"

typedef struct {
    int16_t year;    ///< 0..4095
    int8_t month;    ///< 1..12, 1 is January
    int8_t day;      ///< 1..28,29,30,31 depending on month
    int8_t dotw;     ///< 0..6, 0 is Sunday
    int8_t hour;     ///< 0..23
    int8_t min;      ///< 0..59
    int8_t sec;      ///< 0..59
} datetime_t;

typedef enum {T4H_DAILY_ALARM,T4H_WEEKLY_ALARM, T4H_DATE_ALARM} alarm_type_t;
typedef enum {T4H_ALARM_ON, T4H_ALARM_OFF, T4H_ALARM_SUSPENDED} alarm_state_t;
typedef enum {T4H_SUNDAY, T4H_MONDAY, T4H_TUESDAY, T4H_WEDNESDAY, T4H_THURSDAY, T4H_FRIDAY, T4H_SATURDAY} dotw_t;

typedef struct{
    datetime_t date;
    datetime_t alarm;
    alarm_type_t type;
    alarm_state_t state;
    time_base_t postTB;
    uint32_t postPeriod;
}time_h_t;\

void t4h_init(time_h_t * T);
void t4h_set_time_date(time_h_t * T, uint8_t day, uint8_t month, uint16_t year);
void t4h_set_time_hour(time_h_t * T, uint8_t hour, uint8_t min);
void t4h_set_time_dotw(time_h_t * T, dotw_t day);
void t4h_set_alarm_date(time_h_t * T, uint8_t day, uint8_t month, uint16_t year);
void t4h_set_alarm_hour(time_h_t * T, uint8_t hour, uint8_t min);
void t4h_set_alarm_dotw(time_h_t * T, dotw_t day);
void t4h_set_alarm_type(time_h_t * T, alarm_type_t type);



 #endif