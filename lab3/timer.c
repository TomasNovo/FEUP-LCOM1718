#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include "timer.h"

int hook_id = 0;

int timer_subscribe_int(void ) {

	int temp = hook_id;
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK)
	{
		printf("Error on sys_irqsetpolicy() function\n");
		return -1;
	}

	if (sys_irqenable(&hook_id) != OK)
	{
		printf("Error on sys_irqenable() function\n");
		return -1;
	}

	return BIT(temp);

}


int timer_unsubscribe_int() {

	if (sys_irqdisable(&hook_id) != OK) {
			printf("Error on sys_irqdisable() function\n");
			return -1;
		}

	if (sys_irqrmpolicy(&hook_id) != OK) {
			printf("Error on sys_irqrmpolicy() function\n");
			return -1;
		}

	return 0;
}
