#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/syslib.h>
#include <stdio.h>
#include <stdlib.h>
#include "test5.h"
#include "i8042.h"
#include "i8254.h"
#include "pixmap.h"
#include "video_gr.h"
#include "lmlib.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}


static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init <mode> <delay>\"\n"
			"\t service run %s -args \"square <x> <y> <size> <color>\"\n"
			"\t service run %s -args \"line <xi> <yi> <xf> <yf> <color>\"\n"
			"\t service run %s -args \"xpm <*xpm> <xi> <yi>\"\n"
			"\t service run %s -args \"move <*xpm> <xi> <yi> <xf> <yf> <speed> <frame_rate>\"\n"
			"\t service run %s -args \"controller <>\"\n",
			argv[0], argv[0], argv[0], argv[0],argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short mode, delay;
	unsigned short x, y, size;
	unsigned short color;
	unsigned short xi, yi, xf, yf;
	char **xpm;
	short speed;
	unsigned short frame_rate;

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("test5: wrong no. of arguments for video_test_init()\n");
			return 1;
		}

		mode = parse_ulong(argv[2], 16);						/* Parses string to unsigned long */
		if (mode == ULONG_MAX)
			return 1;

		delay = parse_ulong(argv[3], 10);						/* Parses string to unsigned long */
		if (delay == ULONG_MAX)
			return 1;

		printf("test5::video_test_init(%lu,%lu)\n", mode, delay);
		return video_test_init(mode,delay);
	}

	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf("test5: wrong no. of arguments for video_test_square()\n");
			return 1;
		}

		x = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (x == ULONG_MAX)
			return 1;

		y = parse_ulong(argv[3], 10);						/* Parses string to unsigned long */
		if (y == ULONG_MAX)
			return 1;

		size = parse_ulong(argv[4], 10);						/* Parses string to unsigned long */
		if (size == ULONG_MAX)
			return 1;

		color = parse_ulong(argv[5], 10);						/* Parses string to unsigned long */
		if (color == ULONG_MAX)
			return 1;

		printf("test5::video_test_square(%lu,%lu,%lu,%lu)\n",x,y,size,color);
		return video_test_square(x,y,size,color);
	}
	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("test5: wrong no of arguments for video_test_line()\n");
			return 1;
		}

		xi= parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (xi == ULONG_MAX)
			return 1;

		yi= parse_ulong(argv[3], 10);						/* Parses string to unsigned long */
		if (yi == ULONG_MAX)
		    return 1;

		xf= parse_ulong(argv[4], 10);						/* Parses string to unsigned long */
		if (xf == ULONG_MAX)
			return 1;

		yf= parse_ulong(argv[5], 10);						/* Parses string to unsigned long */
		if (yf == ULONG_MAX)
			return 1;

		color= parse_ulong(argv[6], 10);						/* Parses string to unsigned long */
		if (color == ULONG_MAX)
			return 1;


		printf("test5::video_test_line(%lu,%lu,%lu,%lu,%lu)\n", xi,yi,xf,yf,color);
		return video_test_line(xi,yi,xf,yf,color);
	}
	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0)
	{
		if (argc != 5)
		{
			printf("test5: wrong no of arguments for video_test_xmp()\n");
			return 1;
		}

		if (strncmp(argv[2], "pic1", strlen("pic1")) == 0 )
		{
			xpm = pic1;
		}
		else if (strncmp(argv[2], "pic2", strlen("pic2")) == 0 )
		{
			xpm = pic2;
		}
		else if (strncmp(argv[2], "cross", strlen("cross")) == 0 )
		{
			xpm = cross;
		}
		else if (strncmp(argv[2], "pic3", strlen("pic3")) == 0 )
		{
			xpm = pic3;
		}
		else if (strncmp(argv[2], "penguin", strlen("penguin")) == 0 )
		{
			xpm = penguin;
		}
		else
		{
			printf("test5::Image not found\n");
			return 1;
		}


		xi= parse_ulong(argv[3], 10);						/* Parses string to unsigned long*/
		if (xi == ULONG_MAX)
			return 1;

		yi= parse_ulong(argv[4], 10);						/* Parses string to unsigned long*/
		if (yi == ULONG_MAX)
			return 1;


		printf("test5::video_test_xpm(%s,%lu,%lu)\n",xpm,xi,yi);
		return video_test_xpm(xpm,xi,yi);
	}

	else if (strncmp(argv[1], "move", strlen("move")) == 0)
	{
		if (argc != 9)
		{

			printf("test5: wrong no of arguments for video_test_move()\n");
			return 1;
		}

		if (strncmp(argv[2], "pic1", strlen("pic1")) == 0)
		{
			xpm = pic1;
		}
		else if (strncmp(argv[2], "pic2", strlen("pic2")) == 0)
		{
			xpm = pic2;
		}
		else if (strncmp(argv[2], "cross", strlen("cross")) == 0)
		{
			xpm = cross;
		}
		else if (strncmp(argv[2], "pic3", strlen("pic3")) == 0)
		{
			xpm = pic3;
		}
		else if (strncmp(argv[2], "penguin", strlen("penguin")) == 0)
		{
			xpm = penguin;
		}
		else
		{
			printf("test5::Image not found\n");
			return 1;
		}

		xi = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (xi == ULONG_MAX)
			return 1;

		yi = parse_ulong(argv[4], 10); /* Parses string to unsigned long */
		if (yi == ULONG_MAX)
			return 1;

		xf = parse_ulong(argv[5], 10); /* Parses string to unsigned long */
		if (xf == ULONG_MAX)
			return 1;

		yf = parse_ulong(argv[6], 10); /* Parses string to unsigned long */
		if (yf == ULONG_MAX)
			return 1;


		speed = parse_ulong(argv[7], 10); /* Parses string to unsigned long */
		if (speed == ULONG_MAX)
			return 1;

		frame_rate = parse_ulong(argv[8], 10); /* Parses string to unsigned long */
		if (frame_rate == ULONG_MAX)
			return 1;


		printf("test5::video_test_move(%s,%lu,%lu,%lu,%lu,%lu,%lu)\n",xpm,xi,yi,xf,yf,speed,frame_rate);
				return video_test_move(xpm,xi,yi,xf,yf,speed,frame_rate);

	}
	else if (strncmp(argv[1], "controller", strlen("controller")) == 0)
	{
		if (argc != 2)
		{
			printf("test5: wrong no of arguments for video_test_controller()\n");
			return 1;
		}

		printf("test5::video_test_controller()\n");
		return video_test_controller();
	}
	else
	{
		printf("test5: %s - no valid function!\n", argv[1]);
		return 1;
	}


}


static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);			

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("test5: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
