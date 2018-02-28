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
#include "kbd.h"
#include "test3.h"
#include "video_gr.h"
#include "video_test.h"
#include "pixmap.h"
#include "sprite.h"
#include "vbe.h"

void *video_test_init(unsigned short mode, unsigned short delay) {
	void *ptr = vg_init(mode);
	timer_test_int(delay);
	vg_exit();
	return ptr;
}

int video_test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {
	vg_init(0x105);
	square_paint(x, y, size, color);
	kbd_test_scan(0);
	vg_exit();
	return 0;
}

int video_test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {
	vg_init(0x105);
	line_paint(xi, yi, xf, yf, color);
	video_dump_fb();
	kbd_test_scan(0);
	vg_exit();
	return 0;
}

int video_test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {
	vg_init(0x105);
	xpm_test(xpm, xi, yi);
	video_dump_fb();
	kbd_test_scan(0);
	vg_exit();
	return 0;
}

int video_test_move(char *xpm[], unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, short speed, unsigned short frame_rate)
{
	vg_init(0x105);
	unsigned long code;
	unsigned long irq_set_keyboard;
	unsigned long irq_set_timer;
	int ipc_status;
	int r;
	message msg;

	int counter = 0;

	unsigned int deltax;
	deltax = xf - xi;
	unsigned int deltay;
	deltay = yf - yi;

	short speedx;
	short speedy;

	if (deltax == 0) {
		speedy = speed;
		speedx = 0;
	}

	if (deltay == 0) {
		speedx = speed;
		speedy = 0;
	}

	Sprite *s = create_sprite(xpm, xi, yi, speedx, speedy);

	irq_set_keyboard = kbd_subscribe(); //subscribes keyboard
	if (irq_set_keyboard == -1) {
		printf("Error on kbd_subscribe() function\n");
		return 1;
	}

	irq_set_timer = timer_subscribe_int(); //subscribes timer
	if (irq_set_timer == -1) {
		printf("Error on time_subcribe_int() function\n");
		return 1;
	}

	int frames_per_second = frame_rate / 60;

	int frame_counter = 0;

	while (((xi != xf) && (yi != yf)) || code != ESC) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive() function failed with: %d \n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) //received notification
				{
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: //hardware interrupt notification
			{
				if (msg.NOTIFY_ARG & irq_set_timer) {
					counter++;
					if (counter % frames_per_second == 0)
					{
						sprite_erase(s);
						if (speed < 0) {
							if (frame_counter % abs(speed) == 0) {
								if (deltax == 0) {
									if (yf > yi) {
										s->y = s->y + 1;
									}
									if (yf < yi) {
										s->y = s->y - 1;
									}
								}
								if (deltay == 0) {
									if (xf > xi) {
										s->x = s->x + 1;
									}
									if (xf < xi) {
										s->x = s->x - 1;
									}
								}
							}
							frame_counter++;
						}
						else if (speed > 0) {
							if (deltax == 0) {
								if (yf > yi) {
									s->y = s->y + speed;
								}
								if (yf < yi) {
									s->y = s->y - speed;
								}
							}

							if (deltay == 0) {
								if (xf > xi) {
									s->x = s->x + speed;
								}
								if (xf < xi) {
									s->x = s->x - speed;
								}
							}
						}
						draw_sprite(s);
					}
				}
				if (msg.NOTIFY_ARG & irq_set_keyboard) {
					code = kbd_data_reader();

				}
				break;
			}
			default:
				break;
			}
		}
	}

	video_dump_fb();
	destroy_sprite(s);
	vg_exit();

	if (kbd_unsubscribe() == -1) {
		printf("Error on kbd_unsubscribe() function \n");
		return 1;
	}

	if (timer_unsubscribe_int() == -1) {
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}
	return 0;
}

int video_test_controller()
{
	VbeInfoBlock info;

	if (vbe_controller_info(info) != 0)
	{
		return 1;
	}

	printf("0x%08x",info.VbeVersion);
	printf("\n");

	for (; info.VideoModePtr != -1; info.VideoModePtr++ )
	{
		printf("%lu", info.VideoModePtr);
		printf(":");
	}

	unsigned int vram_mem = info.TotalMemory * 64;
	printf('\n');
	printf("%lu", vram_mem);

	return 0;
}
