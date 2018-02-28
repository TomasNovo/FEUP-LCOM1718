#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include <minix/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "i8042.h"
#include "timer.h"

static int hook_id_mouse;

int mouse_subscribe() //to subscribe the mouse
{
	unsigned long solution = IRQ_REENABLE | IRQ_EXCLUSIVE;
	hook_id_mouse = MOUSE_IRQ;

	if (sys_irqsetpolicy(MOUSE_IRQ, solution, &hook_id_mouse ) != OK)
	{
		printf("Error on sys_irqsetpolicy() function\n");

		return -1;
	}

	if (sys_irqenable(&hook_id_mouse) != OK)
	{
		printf("Error on sys_irqenable() function\n");

		return -1;
	}
	return BIT(MOUSE_IRQ);
}

int mouse_unsubscribe()  //to unsubscribe the mouse
{
	if (sys_irqdisable(&hook_id_mouse) != OK)
	{
		printf("Error on sys_irqdisable() function\n");
		return -1;
	}

	if (sys_irqrmpolicy(&hook_id_mouse) != OK)
	{
		printf("Error on sys_irqrmpolicy() function\n");
		return -1;
	}
	return 0;

}

unsigned long mouse_data_reader() //handler
{
	unsigned long  data;
	unsigned long stat;
	int try = 5;

	while( try > 0 )
	{
		sys_inb(STAT_REG, &stat);
		if( stat & OBF )
		{
			if (OBF & stat)
			{
				sys_inb(OUT_BUF, &data);
				if ( (stat & (PAR_ERR | TO_ERR)) != 0 )
				{
					return -1;
				}
				else
				{
					tickdelay(micros_to_ticks(WAIT_KBC));
					return data;
				}
			}
		}
		try--;
		tickdelay(micros_to_ticks(WAIT_KBC));
	}
	return 1;
}




int send_kbd(unsigned long command)
{
	unsigned long stat;
	int try = 5;


	while(try > 0)
	{

		sys_inb(STAT_REG, &stat);

		if ((IBF & stat) == 0)
		{
			sys_outb(OUT_BUF, command);
			return 0;
		}

		try--;
		tickdelay(micros_to_ticks(WAIT_KBC));
	}

	return 1;
}



int send_command_kbd(unsigned long command)
{
	unsigned long stat;
	int try = 5;

	while (try > 0)
	{
		sys_inb(STAT_REG, &stat);

		if ((IBF & stat) == 0)
		{
			sys_outb(STAT_REG, command);
			tickdelay(micros_to_ticks(WAIT_KBC));
			return 0;
		}

		tickdelay(micros_to_ticks(WAIT_KBC));

		try--;
	}

	return 1;
}


int send_mouse(unsigned long command)
{
	unsigned long data;
	int try = 5;

	while (try > 0)
	{
		if (send_command_kbd(MOUSE_SEND_COMMAND) == 0)
		{
			if (send_kbd(command) == 0)
			{
				data = mouse_data_reader();

				if (data == ACK) //checks if everything is OK
				{
					return 0;
				}
			}

		}

		try--;
	}


	return 1;
}

void packet_display(unsigned short* packet)
{

	printf("B1=0x%02X ", packet[0]);
	printf("B2=0x%02X ", packet[1]);
	printf("B3=0x%02X ", packet[2]);

	if(packet[0] & BIT(0))
	{
		printf("LB: 1 ");
	}
	else printf("LB: 0 ");

	if(packet[0] & BIT(2))
	{
		printf("MB: 1 ");
	}
	else printf("MB: 0 ");

	if(packet[0] & BIT(1))
	{
		printf("RB: 1 ");
	}
	else printf("RB: 0 ");

	if (packet[0] & BIT(6))
	{
		printf("XOV: 1 ");
	}
	else printf("XOV: 0 ");

	if(packet[0] & BIT(7))
	{
		printf("YOV: 1 ");
	}
	else printf("YOV: 0 ");

	if(packet[0] & BIT(4))
	{
		packet[1] = ~packet[1] & 0xFF;
		packet[1] = packet[1] + 1;
		printf("X: -%3d ", packet[1]);
	}
	else printf("X:  %3d ", packet[1]);


	if (packet[0] & BIT(5))
	{
		packet[2] = ~packet[2] & 0xFF;
		packet[2] = packet[2] + 1;
		printf("Y: -%3d", packet[2]);
	}
	else printf("Y:  %3d", packet[2]);

	printf("\n");
}


int mouse_test_packet(unsigned short cnt)
{

	unsigned long irq_set_mouse;
	irq_set_mouse = mouse_subscribe();

	if (irq_set_mouse == -1)
	{
		printf("Error on mouse_subscribe() function\n");
		return 1;
	}

	unsigned long data;

	if (send_mouse(ENABLE) != 0)  // enable data reporting
	{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	message msg;
	int r;
	int ipc_status;
	size_t npackets = 0;
	size_t packet_index = 0;
	unsigned short packet[3];

	while (npackets < cnt)
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
				if(msg.NOTIFY_ARG & irq_set_mouse)
				{

					sys_inb(OUT_BUF, &data);

					packet[packet_index] = data;

					if(packet[0] & MOUSE_SYNC)
					{
						packet_index++;
					}
					else
					{
						packet_index = 0;
					}

					if(packet_index >= 3) {
						packet_index = 0;
						npackets++;
						packet_display(packet);
					}
				}


			default: break;
			}

			}
		}



	}

	if (send_mouse(DISABLE) != 0)  // disable data reporting
	{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	if (mouse_unsubscribe() == -1)
	{
		printf("Error on mouse_unsubscribe() function \n");
		return 1;
	}

	data = mouse_data_reader();

	return 0;
}

int mouse_test_async(unsigned short idle_time)
{
	unsigned long irq_set_timer;
	irq_set_timer = timer_subscribe_int(); //subscribes timer

	if (irq_set_timer == -1)
	{
		printf("Error on timer_subscribe_int() function\n");
		return 1;
	}

    unsigned long irq_set_mouse;
	irq_set_mouse = mouse_subscribe(); //subscribes mouse

	if (irq_set_mouse == -1)
	{
		printf("Error on mouse_subscribe() function\n");
		return 1;
	}


	unsigned long data;

	if (send_mouse(ENABLE) != 0) //enables data reporting
	{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	message msg;
	int r;
	int ipc_status;
	size_t npackets = 0;
	size_t packet_index = 0;
	unsigned short packet[3];
	unsigned int counter = 0;


	while ( (counter / 60 ) < idle_time)
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
				if (msg.NOTIFY_ARG & irq_set_timer)
					counter = counter + 1;

					if(msg.NOTIFY_ARG & irq_set_mouse)
					{
						sys_inb(OUT_BUF, &data);
						packet[packet_index] = data;
						if(packet[0] & MOUSE_SYNC)
						{
							packet_index++;
						}
						else
						{
							packet_index = 0;
						}
						if(packet_index >= 3)
						{
							packet_index = 0;
							npackets++;
							packet_display(packet);
						}

						counter = 0;
					}
			default: break;
				}
			}
		}
	}

	if (idle_time == (counter/60 ) )
	{
		printf("Timed out !\n");
	}


	if (send_mouse(DISABLE) != 0) //disables data reporting
	{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	if (mouse_unsubscribe() == -1) // unsubscribes mouse
	{
		printf("Error on mouse_unsubscribe() function \n");
		return 1;
	}

	if(timer_unsubscribe_int() == -1) //unsubscribes timer
	{
		printf("Error on timer_unsubscribe_int() function\n");
		return 1;
	}

	data = mouse_data_reader();

	return 0;
}

int mouse_test_remote(unsigned long period, unsigned short cnt)
{
	unsigned long val = 0;
	unsigned long KBC_BYTE;
	size_t npackets = 0;
	size_t packet_index = 0;
	unsigned short packet[3];
	unsigned long temp_byte;
	unsigned long data = mouse_data_reader();
	unsigned int counter = 0;

	int ipc_status;
	int r;
	message msg;

	int time_counter = 0;


	sys_outb(KBC_CMD_REG, ENABLE_MOUSE);

	sys_outb(KBC_CMD_REG, MOUSE_SEND_COMMAND);
	sys_outb(IN_BUF, 0xF0);
	sys_inb(OUT_BUF, &val);

	if(val == ACK)
	{
		sys_outb(KBC_CMD_REG, READ_BYTE );
		sys_inb(OUT_BUF, &KBC_BYTE);

		sys_outb(KBC_CMD_REG, WRITE_BYTE);
		sys_outb(IN_BUF, (KBC_BYTE & 0xfd) );

		 int irq_set_timer = timer_subscribe_int();


		 while (npackets < cnt)
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

		 				if(msg.NOTIFY_ARG & irq_set_timer)
		 				{
		 					timer_int_handler();
		 				}

		 				if ( (time_counter*60) == (period*1000) )
		 				{
		 					sys_outb(KBC_CMD_REG, MOUSE_SEND_COMMAND);
							sys_outb(IN_BUF, 0xEB);
							sys_inb(OUT_BUF, &val);

							if(val == ACK)
							{
								temp_byte = mouse_data_reader();

								packet[packet_index] = data;

								if(packet[0] & MOUSE_SYNC)
								{
									packet_index++;
								}
								else
								{
									packet_index = 0;
								}
								if(packet_index >= 3)
								{
									packet_index = 0;
									npackets++;
									packet_display(packet);
								}

								counter = 0;
							}
							else
								return -1;
		 				}
		 				break;

		 			default:
		 				break;
		 			}
		 		}
		 	}
	}
    else
        return -1;


    sys_outb(KBC_CMD_REG, WRITE_BYTE);
    sys_outb(OUT_BUF, KBC_BYTE );


    return 0;


}


unsigned int l_limit = 0;

void state_machine(char transition) // transition can be D(own), U(p), E(xit)
{
	char state = 'I'; // state can be I(nitial), D(rawing), C(omplete)

	switch(state)
	{

	case 'I':
	{
		if (transition == 'D')
			state = 'D';
	}

	case 'D':
	{
		if (transition == 'E')
		{
			state = 'C';
		}
		else if (transition == 'U')
		{
			state = 'I'; //reset
		}
	}

	case 'C':
	{
		l_limit = 1;
	}

	default: state = 'I';
	}

}


int mouse_test_gesture(short length)
{
	unsigned long irq_set_mouse;
	irq_set_mouse = mouse_subscribe();

	if (irq_set_mouse == -1)
	{
		printf("Error on mouse_subscribe() function\n");
		return 1;
	}

	unsigned long data;

	if (send_mouse(ENABLE) != 0)  // enable data reporting
	{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	message msg;
	int r;
	int ipc_status;
	size_t npackets = 0;
	size_t packet_index = 0;
	unsigned short packet[3];
	unsigned int l_counter = 0;

	while ( l_limit != 1 )
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
				if(msg.NOTIFY_ARG & irq_set_mouse)
				{

					sys_inb(OUT_BUF, &data);

					packet[packet_index] = data;

					if(packet[0] & MOUSE_SYNC)
					{
						packet_index++;
					}
					else
					{
						packet_index = 0;
					}

					if(packet_index >= 3)
					{

						packet_index = 0;
						npackets++;
						packet_display(packet);

						if ((BIT(1) & packet[0]) == 1) //right button is being pressed
						{
							state_machine('D'); // RDown
						}
						else if ((BIT(1) & packet[0]) == 0) //right button released
						{
							state_machine('U'); // RUp
						}
						if (packet[1] > 0)
						{
							if (packet[2] > 0)
							{
								l_counter = l_counter + packet[2];
							}
						}
						else if (packet[1] < 0)
						{
							if (packet[2] < 0)
							{
								l_counter = l_counter - packet[2];
							}
						}
						if (l_counter > length)
						{
							state_machine('E'); //it exits, turns length_limit = 1 and breaks while
						}
					}

				}
			default: break;
			}

			}
		}



	}

	if (send_mouse(DISABLE) != 0)  // disable data reporting
	{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	if (mouse_unsubscribe() == -1) //unsubscribes mouse
	{
		printf("Error on mouse_unsubscribe() function \n");
		return 1;
	}

	data = mouse_data_reader();

	return 0;

}
