#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include "timer.h"

int counter = 0;
int hook_id = 0;
int ipc_status = 0;

int timer_get_conf(unsigned char timer, unsigned char *st);

int timer_set_frequency(unsigned char timer, unsigned long freq) {

	if (timer < 0 || timer > 2)
		{
			printf("Error. Timer must be equal to 0, 1 or 2.\n" );
			return 1;
		}


	unsigned char st;

	if(timer_get_conf(timer, &st) != OK)
	{
		printf("Error on time_get_conf() function\n");
		return 1;
	}


	unsigned long word = TIMER_BCD |TIMER_SQR_WAVE | TIMER_LSB_MSB ;
	unsigned long timer_LSB = TIMER_FREQ / freq;
	unsigned long timer_MSB = timer_LSB >> 8;



	switch(timer)
	{

	    case 0:
	{

		if (sys_outb(TIMER_CTRL, word | TIMER_0))
		{
			printf("Error on sys_outb() function\n");
			return 1;
		}

		if (timer == 0)
		{
			sys_outb(TIMER_0, timer_LSB);
			sys_outb(TIMER_0, timer_MSB);
			return 0;
		}


	}

	    case 1:
	    {
	    	if (sys_outb(TIMER_CTRL, word | TIMER_1))
	    			{
	    				printf("Error on sys_outb() function\n");
	    				return 1;
	    			}


	    	if ( timer == 1)
	    	{
	    		sys_outb(TIMER_1, timer_LSB);
	    		sys_outb(TIMER_1, timer_MSB);
	    		return 0;

	    	}
	    }

	    case 2:
	    {
	    	if (sys_outb(TIMER_CTRL, word | TIMER_2))
	    			{
	    				printf("Error on sys_outb() function\n");
	    				return 1;
	    			}



	    	if (timer == 2)
	    	{
	    		sys_outb(TIMER_2, timer_LSB);
	    		sys_outb(TIMER_2, timer_MSB);
	    		return 0;
	    	}
	    }

	    default: return 1;

	}

	return 0;
}


int timer_subscribe_int(void ) {

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

	return BIT(hook_id);

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

void timer_int_handler() {

	counter = counter + 1;

}

int timer_get_conf(unsigned char timer, unsigned char *st) {
	
	long unsigned int reader = ( TIMER_RB_CMD | TIMER_RB_COUNT_ ) ;
    long unsigned int timernumber ;

	switch(timer)
	{
		case 0:
			if (timer == 0)
			{
				timernumber = TIMER_RB_SEL(0);
				reader = reader | timernumber;
				sys_outb(TIMER_CTRL, reader);
				sys_inb(TIMER_0, &reader);
				*st = (unsigned char) reader;
				return 0;
			}

		case 1:
			if (timer == 1)
			{
				timernumber = TIMER_RB_SEL(1);
				reader = reader | timernumber;
				sys_outb(TIMER_CTRL, reader);
				sys_inb(TIMER_1, &reader);
				*st = (unsigned char) reader;
				return 0;
			}
		case 2:
		if (timer == 2)
		{
		   timernumber = TIMER_RB_SEL(2);
		   reader = reader | timernumber;
		   sys_outb(TIMER_CTRL, reader);
		   sys_inb(TIMER_2, &reader);
		   *st = (unsigned char) reader;
		   return 0;
		}

		else return 1;

	}

	return 0;
}

int timer_display_conf(unsigned char conf) {
	

    if (conf & TIMER_SEL2 ) //BIT 7

                    {

                            printf("Output Value: 1\n");

                    }

                    else

                    {

                            printf("Output Value: 0\n");

                    }

                    if (conf & TIMER_SEL1 ) // BIT 6

                                    {

                                            printf("Null count: 1\n");

                                    }

                                    else

                                    {

                                            printf("Null count: 0\n");

                                    }

                    if ((conf & TIMER_LSB_MSB) == TIMER_MSB)

                            {

                                    printf("Type of acess: MSB\n");

                            }

                            else if ((conf & TIMER_LSB_MSB) == TIMER_LSB)

                            {

                                    printf("Type of acess: LSB\n");

                            }

                            else if ((conf & TIMER_LSB_MSB) == TIMER_LSB_MSB)

                            {

                                    printf("Type of acess: LSB followed by MSB\n");

                            }

                    if(conf & TIMER_RB_SEL(2) == OK)

                    {

                            if ((conf & TIMER_RATE_GEN) == OK)

                            {

                                    if((conf & TIMER_RB_SEL(0)) == OK)

                                    {

                                            printf ("Operating mode: Interrupt on terminal count\n");

                                    }

                                    else

                                    {

                                            printf("Operating mode: Hardware retriggerable one-shot\n");

                                    }

                            }

                            else if ((conf & TIMER_RB_SEL(0)) == OK)

                            {

                                    printf("Operating mode: Square wave mode\n");

                            }

                            else

                            {

                                    printf("Operating mode: Rate generator\n");

                            }

                    }

                    else if (conf & TIMER_RB_SEL(0) == OK)

                    {

                            printf("Operating mode: Rate generator\n");

                    }

                    else if ((conf & TIMER_RATE_GEN) == OK)

                    {

                            if (conf & TIMER_RB_SEL(0) != OK)

                            {

                                    printf ("Operating mode: Hardware triggered strobe (retriggerable)\n");

                            }

                            else

                            {

                                    printf("Operating mode: Software triggered strobe\n");

                            }

                    }

                    else

                    {

                            printf("Operating mode: Square wave mode\n");

                    }

            if (conf & BIT(0))

                    {

                            printf("Counting mode: BCD\n");

                    }

                    else

                    {

                            printf("Counting mode: Binary\n");

                    }

               return 0;

}

int timer_test_time_base(unsigned long freq) {
	
	if (timer_set_frequency(0,freq) != OK)
			{
			printf("Error on timer_set_frequency() function\n");
			return 1;
			}
	else
		{
			timer_set_frequency(0, freq);
		}
	return 0;
}

int timer_test_int(unsigned long time) {
	
	int r = 0;
	message msg;

	if (timer_subscribe_int() == -1)
	{
		printf("Error on timer_subscribe_int() function\n");
		return -1;
	}

	if (timer_unsubscribe_int() == -1)
	{
		printf("Error on timer_unsubscribe_int() function()\n");
	    return -1;
	}

	int irq_set = timer_subscribe_int();

	while (counter/60 < time)

	{
		if ((driver_receive(ANY, &msg, &ipc_status)) != OK)
			{
			printf("Error on driver_receive() function, return value = %d \n",r);
			continue;
			}



		if (is_ipc_notify(ipc_status))
		{

			switch(_ENDPOINT_P(msg.m_source))
			{

			case HARDWARE:

			{

				if(msg.NOTIFY_ARG & irq_set)
				{
					timer_int_handler();
					if(counter % 60 == 0)
					{

						printf("Counter = %d seconds\n", counter/60);

					}

				}
				break;

			}

			default: break;


			}


		}

		else
		{


		}



	}





	return 0;
}

int timer_test_config(unsigned char timer) {
	
	unsigned char conf;

    if(timer_get_conf(timer, &conf) != OK)
    	return 1;

    if (timer_display_conf(conf))
    	return 1;

	return 0;
}
