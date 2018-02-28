#ifndef __RTC_H
#define __RTC_H
#include "test4.h"
#include "i8042.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <minix/types.h>
#include <stdlib.h>
#include <stdio.h>

#define BIT(n) (0x01<<(n))

#define SECONDS 0
#define MINUTES 2
#define HOURS 4
#define DAY 7
#define MONTH 8
#define YEAR 9

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define REG_A 10
#define REG_B 11

#define REG_B_DM BIT(2)

#define UIP BIT(7)

/** @defgroup rtc rtc
 * @{
 */

/**
 * @brief Checks if the values given by the RTC are in BCD, verifying the DM in Register B.
 * @return 1 if the values are in bcd, 0 if they are in binary
 */

int checkBCD();

/**
 * @brief Converts values from bcd to binary.
 * @param bcd value in bcd
 * @return the value in binary
 */

unsigned long BCDconverterToBinary(unsigned long bcd);

/**
 * @brief Read the date
 * param data date
 * @param port port
 * @return 0 if true, -1 if false
 */


int portReader(unsigned long *data, unsigned long port);

/**
 * @brief Write the date
 * param data date
 * @param port port
 * @return 0 if true, -1 if false
 */


int portWriter(unsigned long command,unsigned long  port);

/**
 * @brief Gets the current seconds from the RTC in binary.
 * @return seconds
 */

unsigned long getSeconds();

/**
 * @brief Gets the current minutes from the RTC in binary.
 * @return minutes
 */

unsigned long getMinutes();

/**
 * @brief Gets the current hour from the RTC in binary.
 * @return hours
 */

unsigned long getHours();

/**
 * @brief Gets the current day from the RTC in binary.
 * @return day
 */

unsigned long getDay();

/**
 * @brief Gets the current mounth from the RTC in binary.
 * @return mounth
 */

unsigned long getMonth();

/**
 * @brief Gets the current year from the RTC in binary.
 * @return year
 */

unsigned long getYear();

#endif
