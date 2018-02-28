
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include "i8254.h"
#include "kbd.h"
#include "i8042.h"

static int hook_id_kbd;

int kbd_data_reader() //handler

{
	unsigned long data;
	unsigned long stat;

	sys_inb(STAT_REG, &stat);

	int try = 0;
	while( try++ < 3 )

	{
		if( stat & OBF )
		{
			sys_inb(OUT_BUF, &data);
			if ( (stat & (PAR_ERR | TO_ERR)) == 0 )
			{
				return data;
			}
			else
			{
				return -1;
			}
		}
		tickdelay(micros_to_ticks(WAIT_KBC));
	}
	return -2;
}

int kbd_subscribe(void) // to subscribe the keyboard;

{

	unsigned long solution = IRQ_REENABLE | IRQ_EXCLUSIVE;

	hook_id_kbd = KEYBOARD_IRQ;

	if (sys_irqsetpolicy(KEYBOARD_IRQ, solution ,&hook_id_kbd) != OK)

	{

		printf("Error on sys_irqsetpolicy() function\n");

		return -1;

	}

	if (sys_irqenable(&hook_id_kbd) != OK)

	{

		printf("Error on sys_irqenable() function\n");

		return -1;

	}

	return BIT(KEYBOARD_IRQ);

}

int kbd_unsubscribe() // to unsubscribe the keyboard

{

	if (sys_irqdisable(&hook_id_kbd) != OK) {

		printf("Error on sys_irqdisable() function\n");

		return -1;

	}

	if (sys_irqrmpolicy(&hook_id_kbd) != OK) {

		printf("Error on sys_irqrmpolicy() function\n");

		return -1;

	}

	return 0;

}
