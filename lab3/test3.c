#include <minix/syslib.h>

#include <minix/drivers.h>

#include <minix/driver.h>

#include <minix/com.h>

#include <minix/sysutil.h>

#include "test3.h"

#include "i8042.h"

#include "timer.h"

#include "i8254.h"

#include "kbd.h"

unsigned long global_counter = 0;

unsigned long assembly_handler();

int kbd_test_scan(unsigned short assembly) {

	unsigned long code;

	message msg;

	int r;

	int aux = 0; //helps to check if code has 2 bytes

	int ipc_status;

	unsigned long irq_set_keyboard;

	irq_set_keyboard = kbd_subscribe();

	if (irq_set_keyboard == -1)

	{

		printf("Error on kbd_subscribe() function\n");

		return 1;

	}

	while (code != ESC)

	{

		if ( (r = driver_receive(ANY, &msg, &ipc_status) ) != 0 )

		{

			printf("driver_receive() function failed with: %d \n", r);

			continue;

		}

		if (is_ipc_notify(ipc_status) ) //received notification

		{

			switch (_ENDPOINT_P(msg.m_source))

			{

			case HARDWARE: //hardware interrupt notification

			{

				if(msg.NOTIFY_ARG & irq_set_keyboard)

				{

					if (assembly == 0)

					{

						code = kbd_data_reader(); //C handler

					}

					else if (assembly == 1)

					{

						code = assembly_handler(); //Assembly handler

					}



					if (code == 0xE0 || aux)

					{

						if (aux)

						{

							aux = 0;

							if ( (code & BIT(7)) == BIT(7) )

							{

								printf("Breakcode: 0xE0 0x%02x \n", code);

							}

							else

							{

								printf("Makecode: 0xE0 0x%02x \n", code);

							}

						}

						else

						{

							aux = 1;

						}

					}

					else

					{

						if ( (code & BIT(7)) == BIT(7) )

						{

							printf("Breakcode: 0x%x \n", code);

						}

						else

						{

							printf("Makecode: 0x%x \n", code);

						}

					}

				}

				break;

			}

			default: break; // no other notifications expected

			}

		}

	}

	if (kbd_unsubscribe() == -1)

	{

		printf("Error on kbd_subscribe() function \n");

	}

	return 0;

}

int kbd_test_poll() {

	long code;
	unsigned long val = 0;
	int aux = 0;
	int sys_counter=0;


	printf("Startpoll\n");

	sys_outb(KBC_CMD_REG, READ_BYTE);
	sys_inb(OUT_BUF, &val);
	val = val ^ 0xFE;
	sys_outb(KBC_CMD_REG, WRITE_BYTE);
	sys_outb(OUT_BUF, val);


	while(code != ESC)
	{

		code = kbd_data_reader();
		if(code < 0){
			continue;
		}

		if (code == 0xE0 || aux)
		{
			if (aux)
			{
				aux = 0;
				if ( (code & BIT(7)) == BIT(7) )
				{
					printf("Breakcode: 0xE0 0x%02x \n", code);
				}
				else
				{
					printf("Makecode: 0xE0 0x%02x \n", code);
				}
			}
			else
			{
				aux = 1;
			}

		}
		else
		{
			if ( (code & BIT(7)) == BIT(7) )
			{
				printf("Breakcode: 0x%x \n", code);
			}
			else
			{
				printf("Makecode: 0x%x \n", code);
			}
		}

	}

	printf("sys_inb counter: %ld", sys_counter);

	sys_outb(KBC_CMD_REG, READ_BYTE);
	sys_inb(OUT_BUF, &val);
	val = val | 0x01;
	sys_outb(KBC_CMD_REG, WRITE_BYTE);
	sys_outb(OUT_BUF, val);

	return 0;

}

int kbd_test_timed_scan(unsigned short n) {

	unsigned long code;

	unsigned long irq_set_keyboard;

	unsigned long irq_set_timer;

	int ipc_status;

	int r;

	message msg;

	int aux = 0;

	irq_set_keyboard = kbd_subscribe(); //subscribes keyboard

	if (irq_set_keyboard == -1)

	{

		printf("Error on kbd_subscribe() function\n");

		return 1;

	}

	irq_set_timer = timer_subscribe_int(); //subscribes timer

	if (irq_set_timer == -1)

	{

		printf("Error on time_subcribe_int() function\n");

		return 1;

	}

	while( (code != ESC) &&  ( (global_counter / 60) < n ) )

	{

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )

		{

			printf("driver_receive() function failed with: %d \n", r);

			continue;

		}

		if (is_ipc_notify(ipc_status) ) //received notification

		{

			switch (_ENDPOINT_P(msg.m_source))

			{

			case HARDWARE: //hardware interrupt notification

			{

				if(msg.NOTIFY_ARG & irq_set_timer)

				{

					global_counter++;

					if( (global_counter / 60) >= n )

						printf("Timed Out\n");

				}

				if(msg.NOTIFY_ARG & irq_set_keyboard)

				{


					code = kbd_data_reader();

					if (code == 0xE0 || aux)

					{

						if (aux) {

							aux = 0;

							if ( (code & BIT(7)) == BIT(7) )
							{

								printf("Breakcode: 0xE0 0x%x \n", code);

							} else {

								printf("Makecode: 0xE0 0x%x \n", code);

							}

						} else
						{

							aux = 1;

						}

					} else {

						if ( (code & BIT(7)) == BIT(7) ){

							printf("Breakcode: 0x%x \n", code);

						}

						else

						{

							printf("Makecode: 0x%x \n", code);

						}

					}

					global_counter = 0;


				}

				break;

			}

			default: break;

			}

		}

	}

	if (kbd_unsubscribe() == -1)

	{

		printf("Error on kbd_unsubscribe() function \n");

		return 1;

	}

	if (timer_unsubscribe_int() == -1)

	{

		printf("Error on timer_unsubscribe_int() function \n");

		return 1;

	}

	return 0;

}

