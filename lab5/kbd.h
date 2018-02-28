#ifndef __KBD_H
#define __KBD_H

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>


int kbd_data_reader(); //handler

int kbd_subscribe(void);

int kbd_unsubscribe();

#endif
