#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/syslib.h>
#include <stdio.h>
#include <stdlib.h>
#include "test4.h"
#include "i8042.h"
#include "i8254.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char *argv[])
{

	sef_startup();

	if (argc == 1) {
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);


}

static void print_usage(char **argv)
{

	printf("Usage: one of the following:\n"

			"\t service run %s -args \"packet <counter>\"\n"

			"\t service run %s -args \"async <idle_time>\"\n"

			"\t service run %s -args \"remote <period> <counter>\"\n"

			"\t service run %s -args \"gesture <length>\"\n",

			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short idle_time;
	unsigned short cnt;
	short length;
	unsigned long period;

	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {

		if (argc != 3) {
			printf("test4: wrong no. of arguments for mouse_test_packet() function\n");
			return -1;
		}

		cnt = parse_ulong(argv[2], 10); /* Parses string to unsigned long */

		if(cnt == ULONG_MAX) return -1;

		printf("test4::mouse_test_packet(%lu)\n", cnt);
		return mouse_test_packet(cnt);
	}

	else if (strncmp(argv[1], "async", strlen("async")) == 0) {

		if (argc != 3) {

			printf("test4: wrong no. of arguments for mouse_test_async() function\n");

			return 1;

		}



		idle_time = parse_ulong(argv[2],10);

		if (idle_time == ULONG_MAX)

			return 1;

		printf("test4::mouse_test_async(%lu)\n", idle_time);

		return mouse_test_async(idle_time);



	}

	else if (strncmp(argv[1], "remote", strlen("remote")) == 0) {

		if (argc != 4) {

			printf("test4: wrong no of arguments for mouse_test_remote() function\n");

			return 1;

		}

		period = parse_ulong(argv[2], 10);
		cnt = parse_ulong(argv[3], 10);
		/* Parses string to unsigned long */

		if (period == ULONG_MAX)

			return 1;

		if (cnt == ULONG_MAX)

			return 1;

		printf("test4::mouse_test_remote(%lu,%lu)\n", period, cnt);

		return mouse_test_remote(period, cnt);

	}

	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {

		if (argc != 3) {

			printf("test4: wrong no of arguments for mouse_test_gesture() function\n");

			return 1;

		}

		length = parse_ulong(argv[2], 10);                                                /* Parses string to unsigned long */

		if (length == ULONG_MAX)

			return 1;

		printf("test4::mouse_test_gesture(%lu)\n", length);

		return mouse_test_gesture(length);

	}

	else {

		printf("test4: %s - no valid function!\n", argv[1]);

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
		printf("test4: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
