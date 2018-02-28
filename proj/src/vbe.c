#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	mmap_t map;

	struct reg86u r;

	lm_init();
	lm_alloc(sizeof(vbe_mode_info_t),&map);


	r.u.w.ax = 0x4F01; /*VBE get mode info*/
	/* translate the buffer linear addres to a far pointer*/
	r.u.w.es = PB2BASE(map.phys); /* set a segment base*/
	r.u.w.di = PB2OFF(map.phys); /*set the offset accordingly*/
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != OK)
	{
		return 1;
	}

	lm_free(&map);

	memcpy(vmi_p, map.virtual, sizeof(vbe_mode_info_t) );

	return 0;
}

