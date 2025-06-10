/**
 * \file        TimeH.h
 * \brief       Define some methods to count the time and date as humans do. Uses the RTC module.
 * \details
 * \author      Ricardo Andres Velasquez Velez
 * \version     0.0.1
 * \date        05/27/2025
 * \copyright   Unlicensed 
 */

#ifndef __TIME_4H_H_
#define __TIME_4H_H_
//#define PICO_INCLUDE_RTC_DATETIME

#include <stdint.h>
#include <stdio.h>
#include "hardware/rtc.h"
#include "pico/types.h"
#include "TimeBase.h"

#ifndef PICO_INCLUDE_RTC_DATETIME
typedef struct {
    int16_t year;    ///< 0..4095
    int8_t month;    ///< 1..12, 1 is January
    int8_t day;      ///< 1..28,29,30,31 depending on month
    int8_t dotw;     ///< 0..6, 0 is Sunday
    int8_t hour;     ///< 0..23
    int8_t min;      ///< 0..59
    int8_t sec;      ///< 0..59
} datetime_t;
#endif

typedef enum {T4H_DAILY_ALARM,T4H_WEEKLY_ALARM, T4H_DATE_ALARM} alarm_type_t;
typedef enum {T4H_ALARM_READY,T4H_ALARM_ON, T4H_ALARM_OFF, T4H_ALARM_SUSPENDED} alarm_state_t;
typedef enum {T4H_SUNDAY, T4H_MONDAY, T4H_TUESDAY, T4H_WEDNESDAY, T4H_THURSDAY, T4H_FRIDAY, T4H_SATURDAY} dotw_t;

typedef struct{
    datetime_t date; ///< Current date and time
    datetime_t alarm; ///< Alarm date and time
    alarm_type_t type; ///< Type of the alarm (daily, weekly, date)
    alarm_state_t state; ///< State of the alarm (ready, on, off, suspended)
    time_base_t postTB; ///< Time base for posting updates
    time_base_t refreshTB; ///< Time base for refreshing the display
    uint8_t postPeriod; ///< Post period in minutes
}time_h_t; ///< Time handler data structure

/**
 * \fn void t4h_init(time_h_t * T)
 * \brief Initialize the time handler data structure
 * 
 * \param T Pointer to time handler data structure
 * \details This function initializes the time handler data structure with default values.
 */
void t4h_init(time_h_t * T){
    T->date.year = 2025; // Default year
    T->date.month = 1; // Default month
    T->date.day = 1; // Default day
    T->date.dotw = T4H_SUNDAY; // Default day of the week
    T->date.hour = 0; // Default hour
    T->date.min = 0; // Default minute
    T->date.sec = 0; // Default second

    T->type = T4H_DAILY_ALARM; // Default alarm type
    T->state = T4H_ALARM_OFF; // Alarm is initially off

    tb_init(&T->postTB,300000000,false); // Initialize post time base with a period of 1 second
    tb_init(&T->refreshTB,60000000,false); // Initialize refresh time base with a period of 1 second

    T->postPeriod = 5; // Set post period in minutes
}
/**
 * \fn void t4h_set_time_date(time_h_t * T, uint8_t day, uint8_t month, uint16_t year)
 * \brief Set the date in the time handler
 * 
 * \param T Pointer to time handler data structure
 * \param day Day of the month (1-31)
 * \param month Month of the year (1-12)
 * \param year Year (0-4095)
 */
void t4h_set_time_date(time_h_t * T, uint8_t day, uint8_t month, uint16_t year){
    T->date.day = day;
    T->date.month = month;
    T->date.year = year;
}
/**
 * \fn void t4h_set_time_hour(time_h_t * T, uint8_t hour, uint8_t min)
 * \brief Set the hour in the time handler
 * 
 * \param T Pointer to time handler data structure
 * \param hour Hour of the day (0-23)
 * \param min Minute of the hour (0-59)
 */
void t4h_set_time_hour(time_h_t * T, uint8_t hour, uint8_t min){
    T->date.hour = hour;
    T->date.min = min;
    T->date.sec = 0; // Reset seconds to 0 when setting hour and minute
}

/**
 * \fn void t4h_set_time_dotw(time_h_t * T, dotw_t day)
 * \brief Set the day of the week in the time handler
 * 
 * \param T Pointer to time handler data structure
 * \param day Day of the week (0-6, where 0 is Sunday)
 */
void t4h_set_time_dotw(time_h_t * T, dotw_t day){
    T->date.dotw = day;
}

/**
 * \fn void t4h_set_alarm_date(time_h_t * T, uint8_t day, uint8_t month, uint16_t year)
 * \brief Set the alarm date in the time handler
 * \param T Pointer to time handler data structure
 * \param day Day of the month for the alarm (1-31)
 * \param month Month of the year for the alarm (1-12)
 * \param year Year for the alarm (0-4095)
 * 
 */
void t4h_set_alarm_date(time_h_t * T, uint8_t day, uint8_t month, uint16_t year){
    T->alarm.day = day;
    T->alarm.month = month;
    T->alarm.year = year;
}

/**
 * \fn void t4h_set_alarm_hour(time_h_t * T, uint8_t hour, uint8_t min)
 * \brief Set the alarm hour in the time handler
 * 
 * \param T Pointer to time handler data structure
 * \param hour Hour of the day for the alarm (0-23)
 * \param min Minute of the hour for the alarm (0-59)
 */
void t4h_set_alarm_hour(time_h_t * T, uint8_t hour, uint8_t min){
    T->alarm.hour = hour;
    T->alarm.min = min;
    T->alarm.sec = 0; // Reset seconds to 0 when setting alarm hour and minute
}

/**
 * \fn void t4h_set_alarm_dotw(time_h_t * T, dotw_t day)
 * \brief Set the alarm day of the week in the time handler
 * 
 * \param T Pointer to time handler data structure
 * \param day Day of the week for the alarm (0-6, where 0 is Sunday)
 */
void t4h_set_alarm_dotw(time_h_t * T, dotw_t day){
    T->alarm.dotw = day;
}

/**
 * \fn void t4h_set_alarm_type(time_h_t * T, alarm_type_t type)
 * \brief Set the type of the alarm in the time handler
 * 
 * \param T Pointer to time handler data structure
 * \param type Type of the alarm (T4H_DAILY_ALARM, T4H_WEEKLY_ALARM, T4H_DATE_ALARM)
 */
void t4h_set_alarm_type(time_h_t * T, alarm_type_t type){
    T->type = type;
}

/**
 * \fn void t4h_enable_alarm(time_h_t * T)
 * \brief Enable the alarm in the time handler
 * \param T Pointer to time handler data structure
 * \details This function enables the alarm in the time handler, allowing it to trigger when the current time matches the alarm time.
 * * \note The alarm state will be set to T4H_ALARM_ON.
 */
void t4h_enable_alarm(time_h_t * T){
    T->state = T4H_ALARM_ON;
}

/**
 * \fn void t4h_disable_alarm(time_h_t * T)
 * \brief Disable the alarm in the time handler
 * \param T Pointer to time handler data structure
 * \details This function disables the alarm in the time handler, preventing it from triggering.
 * \note The alarm state will be set to T4H_ALARM_OFF.
 */
void t4h_disable_alarm(time_h_t * T){
    T->state = T4H_ALARM_OFF;
}

/**
 * \fn void t4h_update_rtc_time(time_h_t * T)
 * \brief Update the RTC time with the current time in the time handler
 * \param T Pointer to time handler data structure
 * \details This function updates the RTC with the current time stored in the time handler. 
 * It sets the RTC time to the values specified in the time handler's date and time fields.
 */
void t4h_update_rtc_time(time_h_t * T){
    rtc_set_datetime(&T->date);
}


/**
 * \fn void t4h_update_rtc_alarm(time_h_t * T)
 * \brief Update the RTC alarm with the current alarm in the time handler
 * \param T Pointer to time handler data structure
 * \details This function updates the RTC alarm with the current alarm settings stored in the time handler. 
 * It sets the RTC alarm to the values specified in the time handler's alarm fields.
 */
void t4h_update_rtc_alarm(time_h_t * T){
   // rtc_set_alarm(&T->alarm,);
}

/**
 * \fn void t4h_set_post_period(time_h_t * T, uint32_t period)
 * \brief Set the post period for the time handler
 * \param T Pointer to time handler data structure
 * \param period Post period in minutes
 * \details This function sets the post period for the time handler, which is used to control how often the time handler updates its state.
 */
void t4h_set_post_period(time_h_t * T, uint8_t period){
    T->postPeriod = period;
    T->postTB.delta = period * 1000000 * 60; // Convert minutes to microseconds
}

/**
 * \fn void t4h_start_post(time_h_t * T)
 * \brief Start the post time base in the time handler
 * \param T Pointer to time handler data structure
 * \details This function starts the post time base, allowing the time handler to update its state at the specified post period.
 */
void t4h_start_post(time_h_t * T){
    tb_update(&T->postTB); // Update the post time base to start counting
    tb_enable(&T->postTB); // Enable the post time base
    T->state = T4H_ALARM_SUSPENDED; // Set the alarm state to suspended while post is active
}

/**
 * \fn void t4h_stop_post(time_h_t * T)
 * \brief Stop the post time base in the time handler
 * \param T Pointer to time handler data structure
 * \details This function stops the post time base, preventing the time handler from updating its state.
 */ 
void t4h_stop_post(time_h_t * T){
    tb_disable(&T->postTB); // Disable the post time base
    if(T->type == T4H_DATE_ALARM) {
        T->state = T4H_ALARM_OFF; // Set the alarm state to off after stopping post
        rtc_disable_alarm(); // Disable the RTC alarm to prevent it from triggering
    }
    else if(T->type == T4H_WEEKLY_ALARM) {
        T->state = T4H_ALARM_ON; // Set the alarm state back to on after stopping post
        rtc_enable_alarm(); // Re-enable the RTC alarm
    }
    else if(T->type == T4H_DAILY_ALARM) {
        T->state = T4H_ALARM_ON; // Set the alarm state back to on after stopping post
        rtc_enable_alarm(); // Re-enable the RTC alarm
    }
    else {
        T->state = T4H_ALARM_OFF; // Default case, set the alarm state to off
        rtc_disable_alarm(); // Disable the RTC alarm   
    }
}

/**
 * \fn void t4h_get_date(time_h_t * T, datetime_t * date);
 * \brief Get the current date from the time handler
 * \param T Pointer to time handler data structure
 * \param date Pointer to a datetime_t structure where the current date will be stored
 * \details This function retrieves the current date from the time handler and stores it in the provided datetime_t structure.
 */
void t4h_get_date(time_h_t * T, datetime_t * date){
    date->day = T->date.day;
    date->month = T->date.month;
    date->year = T->date.year;
    date->dotw = T->date.dotw;
    date->hour = T->date.hour;
    date->min = T->date.min;
    date->sec = T->date.sec;
}

/**
 * \fn uint8_t t4h_get_hour(time_h_t * T)
 * \brief Get the current hour from the time handler
 * \param T Pointer to time handler data structure 
 * \returns The current hour (0-23)
 * \details This function retrieves the current hour from the time handler.
 */
uint8_t t4h_get_hour(time_h_t * T){
    return T->date.hour;
}

/**
 * \fn uint8_t t4h_get_minute(time_h_t * T)
 * \brief Get the current minute from the time handler
 * \param T Pointer to time handler data structure
 * \returns The current minute (0-59)
 * \details This function retrieves the current minute from the time handler.
 */
uint8_t t4h_get_minute(time_h_t * T){
    return T->date.min;
}

/**
 * \fn uint8_t t4h_get_second(time_h_t * T)
 * \brief Get the current second from the time handler
 * \param T Pointer to time handler data structure
 * \returns The current second (0-59)
 * \details This function retrieves the current second from the time handler.
 */
uint8_t t4h_get_second(time_h_t * T){
    return T->date.sec;
}

/**
 * \fn uint8_t t4h_get_day(time_h_t * T)
 * \brief Get the current day from the time handler
 * \param T Pointer to time handler data structure
 * \returns The current day of the month (1-31)
 * \details This function retrieves the current day from the time handler.
 */
uint8_t t4h_get_day(time_h_t * T){
    return T->date.day;
}
/**
 * \fn uint8_t t4h_get_dotw(time_h_t * T)
 * \brief Get the current day of the week from the time handler
 * \param T Pointer to time handler data structure
 * \returns The current day of the week (0-6, where 0 is Sunday)
 * \details This function retrieves the current day of the week from the time handler.
 */
uint8_t t4h_get_dotw(time_h_t * T){
    return T->date.dotw;
}

/**
 * \fn uint8_t t4h_get_month(time_h_t * T)
 * \brief Get the current month from the time handler
 * \param T Pointer to time handler data structure
 * \returns The current month (1-12)
 * \details This function retrieves the current month from the time handler.
 */
uint8_t t4h_get_month(time_h_t * T){
    return T->date.month;
}

/**
 * \fn uint16_t t4h_get_year(time_h_t * T)
 * \brief Get the current year from the time handler
 * \param T Pointer to time handler data structure
 * \returns The current year (0-4095)
 * \details This function retrieves the current year from the time handler.
 */
uint16_t t4h_get_year(time_h_t * T);

/**
 * \fn void t4h_refresh_time(time_h_t * T)
 * \brief Refresh the time in the time handler from the RTC
 * \param T Pointer to time handler data structure  
 * \returns true if the time was refreshed, false otherwise
 * \details This function refreshes the time in the time handler by reading the current time from the RTC module.
 * It updates the time handler's date, hour, minute, second, day, month, and year fields with the current RTC values.
 * This method controls the postpond feature through the postTB time base. When the alarm is postponed,
 * the postTB is enabled and will count the postponed period.
 * \note This function should be called in the main loop to keep the time handler's time synchronized with the RTC.
 * Time base refreshTB is used to control how often the time is refreshed.
 */
bool t4h_refresh_time(time_h_t * T){
    if(tb_check(&T->refreshTB)){
        tb_next(&T->refreshTB);
        if(rtc_hw->ints & RTC_INTS_RTC_BITS){
            //rtc_hw->intr |= RTC_INTS_RTC_BITS; // Clear the RTC interrupt

            T->state = T4H_ALARM_READY; // Set the alarm state to ready
        }
        return true; // Time to refresh
    }
    return false; // Not time to refresh yet
}


/**
 * \fn alarm_state_t t4h_get_alarm_state(time_h_t * T)
 * \brief Get the current alarm state from the time handler
 * \param T Pointer to time handler data structure
 * \returns The current alarm state (T4H_ALARM_READY, T4H_ALARM_ON, T4H_ALARM_OFF, T4H_ALARM_SUSPENDED)
 */
alarm_state_t t4h_get_alarm_state(time_h_t * T){
    return T->state; // Return the current alarm state  
}

 #endif