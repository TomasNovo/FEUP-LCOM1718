#include "rtc.h"

int portReader(unsigned long *data, unsigned long port)
{	if (sys_inb(port,data))
			{
			return -1;
			}
return 0;
}

int portWriter(unsigned long command,unsigned long  port)
{
	if (sys_outb(port,command))
		{
		return -1;
		}
	return 0;
}

int checkBCD()
{
	unsigned long regb = 0;
	sys_outb(RTC_ADDR_REG,REG_B);
	sys_inb(RTC_DATA_REG, &regb);

	if (!(REG_B_DM & regb))
		return 1;
	else
		return 0;
}

unsigned long BCDconverterToBinary(unsigned long bcd)
{
	unsigned long binary;
	binary = ((bcd) & 0x0F) + 10 * (((bcd) & 0xF0) >> 4);
	return binary;
}

unsigned long getSeconds()
{

	unsigned long read;
	unsigned long bit;

	unsigned long year;

	int try = 5;
	bit =SECONDS;

	while (try > 0) {
		portWriter(REG_A, RTC_ADDR_REG);
		portReader(&read, RTC_DATA_REG);

		if (!(UIP & read)) {
			portWriter(bit, RTC_ADDR_REG);
			portReader(&read, RTC_DATA_REG);

			break;
		}

		tickdelay(micros_to_ticks(WAIT_KBC));

		try--;

	}
	if (checkBCD()) {
		year = BCDconverterToBinary(read);
	}

	return year;
}

unsigned long getMinutes()
{
	unsigned long read;
	unsigned long bit;

	unsigned long year;

	int try = 5;
	bit = MINUTES;

	while (try > 0) {
		portWriter(REG_A, RTC_ADDR_REG);
		portReader(&read, RTC_DATA_REG);

		if (!(UIP & read)) {
			portWriter(bit, RTC_ADDR_REG);
			portReader(&read, RTC_DATA_REG);

			break;
		}

		tickdelay(micros_to_ticks(WAIT_KBC));

		try--;

	}
	if (checkBCD()) {
		year = BCDconverterToBinary(read);
	}

	return year;
}


unsigned long getHours()
{
	unsigned long read;
	unsigned long bit;

	unsigned long year;

	int try = 5;
	bit = HOURS;

	while (try > 0) {
		portWriter(REG_A, RTC_ADDR_REG);
		portReader(&read, RTC_DATA_REG);

		if (!(UIP & read)) {
			portWriter(bit, RTC_ADDR_REG);
			portReader(&read, RTC_DATA_REG);

			break;
		}

		tickdelay(micros_to_ticks(WAIT_KBC));

		try--;

	}
	if (checkBCD()) {
		year = BCDconverterToBinary(read);
	}

	return year;
}

unsigned long getDay()
{
	unsigned long read;
	unsigned long bit;

	unsigned long year;

	int try = 5;
	bit = DAY;

	while (try > 0) {
		portWriter(REG_A, RTC_ADDR_REG);
		portReader(&read, RTC_DATA_REG);

		if (!(UIP & read)) {
			portWriter(bit, RTC_ADDR_REG);
			portReader(&read, RTC_DATA_REG);

			break;
		}

		tickdelay(micros_to_ticks(WAIT_KBC));

		try--;

	}
	if(checkBCD()){
			year = BCDconverterToBinary(read);
		}


	return year;
}

unsigned long getMonth()
{
	unsigned long read;
	unsigned long bit;

	unsigned long year;

	int try = 5;
	bit = MONTH;

	while (try > 0) {
		portWriter(REG_A, RTC_ADDR_REG);
		portReader(&read, RTC_DATA_REG);

		if (!(UIP & read)) {
			portWriter(bit, RTC_ADDR_REG);
			portReader(&read, RTC_DATA_REG);

			break;
		}

		tickdelay(micros_to_ticks(WAIT_KBC));

		try--;

	}
	if(checkBCD()){
			year = BCDconverterToBinary(read);
		}


	return year;
}



unsigned long getYear()
{
unsigned long read;
unsigned long bit;

unsigned long year;

int try = 5;
bit = YEAR;

while (try > 0) {
	portWriter(REG_A, RTC_ADDR_REG);
	portReader(&read, RTC_DATA_REG);

	if (!(UIP & read)) {
		portWriter(bit, RTC_ADDR_REG);
		portReader(&read, RTC_DATA_REG);

		break;
	}

	tickdelay(micros_to_ticks(WAIT_KBC));

	try--;

}
if(checkBCD()){
		year = BCDconverterToBinary(read);
	}


return year;
}

