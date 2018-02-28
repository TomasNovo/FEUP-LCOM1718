#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "video_gr.h"
#include "smiley.h"
#include "i8042.h"
#include "i8254.h"
#include "bitmap.h"
#include "kbd.h"
#include "timer.h"
#include "menu.h"
#include "smiley.h"
#include "rtc.h"

void entering()
{

	Bitmap* one = loadBitmap("/home/lcom/svn/proj/doc/one.bmp");

	Bitmap* two = loadBitmap("/home/lcom/svn/proj/doc/two.bmp");

	Bitmap* three = loadBitmap("/home/lcom/svn/proj/doc/three.bmp");

	Bitmap* four = loadBitmap("/home/lcom/svn/proj/doc/four.bmp");

	int ipc_status, r = 0;
	message msg;

	unsigned long irq_set_timer;
	unsigned long irq_set_keyboard;

	int counter = 0;
	unsigned long code;

	unsigned int i = 4;

	irq_set_timer = timer_subscribe_int(); //subscribes timer
	if (irq_set_timer == -1) {
		printf("Error on time_subcribe_int() function\n");
		return 1;
	}

	irq_set_keyboard = kbd_subscribe(); //subscribes keyboard
	if (irq_set_keyboard == -1) {
		printf("Error on kbd_subscribe() function\n");
		return 1;
	}


		while(code != 0x001C) //enter key
		{
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
			{
				printf("driver_receive() function failed with: %d \n", r);
				continue;
			}


			if (is_ipc_notify(ipc_status)) //received notification
				switch (_ENDPOINT_P(msg.m_source))
				{
				case HARDWARE: //hardware interrupt notification
				{
					if (msg.NOTIFY_ARG & irq_set_keyboard)
					{
			          code = kbd_data_reader();
					}
					if (msg.NOTIFY_ARG & irq_set_timer)
					{
						counter++;

						if (counter % 60 == 0)
						drawBitmap(one,0,0 ,ALIGN_LEFT);

						if (counter % 60 == 20)
						drawBitmap(two,0,0 ,ALIGN_LEFT);

						if (counter % 60 == 40)
						drawBitmap(three,0,0 ,ALIGN_LEFT);

						if (counter % 60 == 10)
						drawBitmap(four,0,0 ,ALIGN_LEFT);
					}
					break;
				}
				default:break;
				}
		}


	//deleteBitmap(one);

	//deleteBitmap(two);

	//deleteBitmap(three);

	//deleteBitmap(four);

	if (kbd_unsubscribe() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}

	if (timer_unsubscribe_int() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}
}


int mainMenu()
{

	Bitmap* mainMenu= loadBitmap("/home/lcom/svn/proj/doc/mainmenu.bmp");
	OptionRectangle* options = newORectangle();;

	Mouse* mouse = startMouse();

	int ipc_status, r = 0;
	message msg;

	int counter = 0;
	unsigned long code;

	unsigned int i = 4;


	unsigned long irq_set_timer;
	irq_set_timer = timer_subscribe_int(); //subscribes timer
	if (irq_set_timer == -1) {
		printf("Error on time_subcribe_int() function\n");
		return 1;
	}


	unsigned long irq_set_mouse;
	irq_set_mouse = mouse_subscribe();
	if (irq_set_mouse == -1) {
		printf("Error on mouse_subscribe() function\n");
		return 1;
	}

	if (send_mouse(ENABLE) != 0)  // enable data reporting
			{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	unsigned long data;

	size_t packet_index = 0;
	unsigned short packet[3];
	int npackets = 0;

	int exit = 0;


	while (options->done != 1  )//|| option_two->done != 1)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive() function failed with: %d \n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
			{
				if (msg.NOTIFY_ARG & irq_set_mouse) {

					sys_inb(OUT_BUF, &data);
					packet[packet_index] = data;

					if (packet[0] & MOUSE_SYNC) {
						packet_index++;
					} else {
						packet_index = 0;
					}

					if (packet_index >= 3) {
						packet_index = 0;
						npackets++;

						if (packet[0] & BIT(4)) {
							//if(mouse->x > 1)
							mouse->x += -(256 - packet[1]);
						} else
							// if(mouse->x < 639)
							mouse->x += packet[1];
						if (packet[0] & BIT(5)) {
							//if(mouse->y > 1)
							mouse->y -= -(256 - packet[2]);
						} else
							//if(mouse->y > 479)
							mouse->y -= packet[2];

						if (packet[0] & BIT(0)) {

							if ((mouse->x >= 60) && (mouse->x <= 230)
									&& (mouse->y >= 200) && (mouse->y <= 400)) {
								options->done = 1;
							} else if ((mouse->x >= 300) && (mouse->x <= 500)
									&& (mouse->y >= 120) && (mouse->y <= 440)) {
								options->done = 1;
								exit = 1;
							}
						}


						if (mouse->x <= 2)
							mouse->x = 638;

						if (mouse->x >= 638)
							mouse->x = 2;

						if (mouse->y <= 2)
							mouse->y = 478;

						if (mouse->y >= 478)
							mouse->y = 2;

				}
				}

				if (msg.NOTIFY_ARG & irq_set_timer) {
					counter++;

					drawBitmap1(mainMenu, 0, 0, ALIGN_LEFT);
					drawBitmap1(options->img1, options->x1, options->y1, ALIGN_LEFT);
					drawBitmap1(options->img2, options->x2, options->y2, ALIGN_LEFT);
					drawMouse(mouse);

					DBtoVM();

				}
				break;
			}
			default:
				break;
			}
	}

	deleteBitmap(mainMenu);
	deleteBitmap(options->img1);
	deleteBitmap(options->img2);
	deleteBitmap(mouse->img);



    if (send_mouse(DISABLE) != 0)  // disable data reporting
			{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	if (mouse_unsubscribe() == -1) {
		printf("Error on mouse_unsubscribe() function \n");
		return 1;
	}

	if (timer_unsubscribe_int() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}


	free(mouse);

	if (exit == 1)
	{

		return -1;

	}
	else
	{

		return 0;
	}

}



void gameOver(Smiley* smiley)
{
	vg_init(0x111);

	Bitmap* go1 = loadBitmap("/home/lcom/svn/proj/doc/GO1.bmp");

	Bitmap* go2 = loadBitmap("/home/lcom/svn/proj/doc/GO2.bmp");

	Bitmap* s = loadBitmap("/home/lcom/svn/proj/doc/score.bmp");

	int ipc_status, r = 0;
	message msg;

	unsigned long irq_set_timer;
	unsigned long irq_set_keyboard;

	int counter = 0;
	unsigned long code;

	unsigned int i = 4;

	irq_set_timer = timer_subscribe_int(); //subscribes timer
	if (irq_set_timer == -1) {
		printf("Error on time_subcribe_int() function\n");
		return 1;
	}

	irq_set_keyboard = kbd_subscribe(); //subscribes keyboard
	if (irq_set_keyboard == -1) {
		printf("Error on kbd_subscribe() function\n");
		return 1;
	}

	while (code != ESC) //enter key
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive() function failed with: %d \n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
			{
				if (msg.NOTIFY_ARG & irq_set_keyboard) {
					code = kbd_data_reader();
				}
				if (msg.NOTIFY_ARG & irq_set_timer) {
					counter++;

					if (counter % 60 == 0)
						drawBitmap1(go1, 0, 0, ALIGN_LEFT);

					if (counter % 60 == 20)
						drawBitmap1(go2, 0, 0, ALIGN_LEFT);


					drawBitmap1(s,180,445,ALIGN_LEFT);

					updateScore(smiley->final_score);

					DBtoVM();

				}
				break;
			}
			default:
				break;
			}
	}

	if (kbd_unsubscribe() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}

	if (timer_unsubscribe_int() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}


	deleteBitmap(s);
}



/////////////////////////////////////////////////////////////////////////////

Mouse* startMouse()
{
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));
	mouse->x = 320;
	mouse->y = 240;
	mouse->img= loadBitmap("/home/lcom/svn/proj/doc/platform.bmp");

	mouse->done = 0;

	return mouse;
}

void drawMouse(Mouse* mouse)
{
	drawBitmap1(mouse->img, mouse->x, mouse->y, ALIGN_LEFT);
}

void deleteMouse(Mouse* mouse)
{
	free(mouse);
}


//////////////////////////

OptionRectangle* newORectangle()
{
	OptionRectangle* rectangle= (OptionRectangle*) malloc(sizeof(OptionRectangle));

	rectangle->img1 = loadBitmap("/home/lcom/svn/proj/doc/r1.bmp");
	rectangle->img2 = loadBitmap("/home/lcom/svn/proj/doc/r2.bmp");


	rectangle->x1 = 50;
    rectangle->y1 = 190;
    rectangle->x2 = 260;
    rectangle->y2 = 125;
	rectangle->done = 0;

	return rectangle;
}

void deleteORectangle(OptionRectangle* rectangle)
{
    free(rectangle);
}



void scoreMenu(Smiley* smiley)
{/*
	int score1, hightest_score;

	Bitmap* background = loadBitmap("/home/lcom/svn/proj/doc/scores.bmp");

	int ipc_status, r = 0;
	message msg;

	unsigned long irq_set_timer;
	unsigned long irq_set_keyboard;


	unsigned long seconds,minutes,hour,days,months

	int counter = 0;
	unsigned long code;

	unsigned int i = 4;

	irq_set_timer = timer_subscribe_int(); //subscribes timer
	if (irq_set_timer == -1) {
		printf("Error on time_subcribe_int() function\n");
		return 1;
	}

	irq_set_keyboard = kbd_subscribe(); //subscribes keyboard
	if (irq_set_keyboard == -1) {
		printf("Error on kbd_subscribe() function\n");
		return 1;
	}

	while (code != ESC) //enter key
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive() function failed with: %d \n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) //received notification
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
			{
				if (msg.NOTIFY_ARG & irq_set_keyboard) {
					code = kbd_data_reader();
				}
				if (msg.NOTIFY_ARG & irq_set_timer) {
					counter++;

					drawBitmap1(background, 0, 0, ALIGN_LEFT);

					//updateScore()
					DBtoVM();

				}
				break;
			}
			default:
				break;
			}
	}

	if (kbd_unsubscribe() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}

	if (timer_unsubscribe_int() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}*/
}
/////////////////////////////////////////////////7

void timeShow()
{
int ipc_status, r = 0;
	message msg;



	Bitmap* back = loadBitmap("/home/lcom/svn/proj/doc/END.bmp");

	unsigned long irq_set_timer;
	unsigned long irq_set_keyboard;

	int counter = 0;
	unsigned long code;

	unsigned int i = 4;

	Date* date;
	date = (Date*) startDate();

	irq_set_timer = timer_subscribe_int(); //subscribes timer
	if (irq_set_timer == -1) {
		printf("Error on time_subcribe_int() function\n");
		return 1;
	}

	irq_set_keyboard = kbd_subscribe(); //subscribes keyboard
	if (irq_set_keyboard == -1) {
		printf("Error on kbd_subscribe() function\n");
		return 1;
	}


		while(code != 0x001C) //enter key
		{
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
			{
				printf("driver_receive() function failed with: %d \n", r);
				continue;
			}


			if (is_ipc_notify(ipc_status)) //received notification
				switch (_ENDPOINT_P(msg.m_source))
				{
				case HARDWARE: //hardware interrupt notification
				{
					if (msg.NOTIFY_ARG & irq_set_keyboard)
					{
			          code = kbd_data_reader();
					}
					if (msg.NOTIFY_ARG & irq_set_timer)
					{
						drawBitmap1(back,0,0,ALIGN_LEFT);
						drawDate(date);
						drawTime(date);

						DBtoVM();
					}
					break;
				}
				default:break;
				}
		}


	if (kbd_unsubscribe() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}

	if (timer_unsubscribe_int() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}

	vg_exit();


}

////////////////////////////////////////

Date* startDate()
{
	Date* date = (Date*) malloc(sizeof(Date));


	date->x = 180;
	date->y = 200;
	date->x2 = 180;
	date->y2 = 400;

	date->digits[0] = loadBitmap("/home/lcom/svn/proj/doc/1.bmp");
	date->digits[1] = loadBitmap("/home/lcom/svn/proj/doc/2.bmp");
	date->digits[2] =  loadBitmap("/home/lcom/svn/proj/doc/3.bmp");
	date->digits[3] =  loadBitmap("/home/lcom/svn/proj/doc/4.bmp");
	date->digits[4] = loadBitmap("/home/lcom/svn/proj/doc/5.bmp");
	date->digits[5] = loadBitmap("/home/lcom/svn/proj/doc/6.bmp");
	date->digits[6] =  loadBitmap("/home/lcom/svn/proj/doc/7.bmp");
	date->digits[7] =  loadBitmap("/home/lcom/svn/proj/doc/8.bmp");
	date->digits[8] = loadBitmap("/home/lcom/svn/proj/doc/9.bmp");
	date->digits[9] = loadBitmap("/home/lcom/svn/proj/doc/0.bmp");


	date->day = getDay();
	date->month = getMonth();
	date->year = getYear();
	date->hours = getHours();
	date->minutes = getMinutes();
	date->seconds = getSeconds();

	return date;
}

Bitmap* digits(int digit,Date* date)
{
	switch (digit)
	{
	case 1:
		return date->digits[0];

	case 2:
		return date->digits[1];

	case 3:
		return date->digits[2];

	case 4:
		return date->digits[3];;

	case 5:
		return date->digits[4];

	case 6:
		return date->digits[5];

	case 7:
		return date->digits[6];

	case 8:
		return date->digits[7];

	case 9:
		return date->digits[8];

	case 0:
		return date->digits[9];
	}
}

void drawDate(Date* date)
{

	Bitmap* slash = loadBitmap("/home/lcom/svn/proj/doc/slash.bmp");
    unsigned long d1 = 0, d2 = 0;

    //dia
	d1 = date->day % 10;
	d2 = (date->day % 100)/10;

	Bitmap* d1b = digits(d1,date);
	Bitmap* d2b = digits(d2,date);

	drawBitmap1(d2b, date->x, date->y, ALIGN_LEFT);
	drawBitmap1(d1b, date->x + 30, date->y, ALIGN_LEFT);
	drawBitmap1(slash,date->x + 60, date->y, ALIGN_LEFT);


	//mes
	d1 = 0, d2 = 0;

	d1 = date->month % 10;
	d2 = (date->month % 100) / 10;

	Bitmap* d11b = digits(d1, date);
	Bitmap* d22b = digits(d2, date);

	drawBitmap1(d22b, date->x + 90, date->y, ALIGN_LEFT);
	drawBitmap1(d11b, date->x + 120, date->y, ALIGN_LEFT);
	drawBitmap1(slash,date->x + 150, date->y, ALIGN_LEFT);


	//ano
	d1 = 0, d2 = 0;

	d1 = date->year % 10;
	d2 = (date->year % 100) / 10;

	Bitmap* d111b = digits(d1, date);
	Bitmap* d222b = digits(d2, date);

	drawBitmap1(d22b, date->x + 180, date->y, ALIGN_LEFT);
	drawBitmap1(d11b, date->x + 210, date->y, ALIGN_LEFT);
}


void drawTime(Date* date)
{

	Bitmap* dots = loadBitmap("/home/lcom/svn/proj/doc/dots.bmp");
	unsigned long d1 = 0, d2 = 0;

	//hora

	d1 = date->hours % 10;
	d2 = (date->hours % 100) / 10;

	Bitmap* d1b = digits(d1, date);
	Bitmap* d2b = digits(d2, date);

	drawBitmap1(d2b, date->x2, date->y2, ALIGN_LEFT);
	drawBitmap1(d1b, date->x2 + 30, date->y2, ALIGN_LEFT);
	drawBitmap1(dots, date->x2 + 60, date->y2, ALIGN_LEFT);


	//minutos
	d1 = 0, d2 = 0;

	d1 = date->minutes % 10;
	d2 = (date->minutes  % 100) / 10;

	Bitmap* d11b = digits(d1, date);
	Bitmap* d22b = digits(d2, date);

	drawBitmap1(d22b, date->x2 + 90, date->y2, ALIGN_LEFT);
	drawBitmap1(d11b, date->x2 + 120, date->y2, ALIGN_LEFT);
	drawBitmap1(dots, date->x2 + 150, date->y2, ALIGN_LEFT);


	//segundos
	d1 = 0, d2 = 0;

	d1 = date->seconds % 10;
	d2 = (date->seconds % 100) / 10;

	Bitmap* d111b = digits(d1, date);
	Bitmap* d222b = digits(d2, date);

	drawBitmap1(d22b, date->x2 + 180, date->y2, ALIGN_LEFT);
	drawBitmap1(d11b, date->x2 + 210, date->y2, ALIGN_LEFT);


}



















