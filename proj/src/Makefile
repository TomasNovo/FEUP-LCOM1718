COMPILER_TYPE= gnu

CC= gcc

PROG= proj
SRCS= main.c video_gr.c vbe.c timer.c kbd.c smiley.c bitmap.c menu.c test4.c StateMachine.c rtc.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.gcc.mk>
.include <bsd.prog.mk>