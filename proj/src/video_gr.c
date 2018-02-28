#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR  0xE0000000
#define H_RES           640
#define V_RES           480
#define BITS_PER_PIXEL    16

/* Private global variables */

static char* video_mem;     /* Process (virtual) address to which VRAM is mapped */
static char* double_buffer;

static unsigned h_res;      /* Horizontal screen resolution in pixels */
static unsigned v_res;      /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned int vram_size;

int vg_exit() {

  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK )
  {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  }
  else
      return 0;
}


void *vg_init(unsigned short mode)
{

    struct reg86u reg;
    reg.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
    reg.u.w.bx = 1<<14|mode; // set bit 14: linear framebuffer
    reg.u.b.intno = 0x10;

    if( sys_int86(&reg) != OK )
    {
        printf("set_vbe_mode: sys_int86() failed \n");
        return NULL;
    }

    int r;
    struct mem_range mr;
    unsigned int vram_base; /*VRAMâ€™s physical address*/


    vram_base = VRAM_PHYS_ADDR;
    vram_size = V_RES * H_RES * BITS_PER_PIXEL / 8;
    bits_per_pixel = BITS_PER_PIXEL;
    v_res = V_RES;
    h_res = H_RES;

    /*Allow memory mapping*/
    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if ( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    /*Map memory*/
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    double_buffer =malloc(vram_size);

    if (video_mem == MAP_FAILED) panic("couldn't map video memory\n");

    return video_mem;
}
void pixel_paint(unsigned short x, unsigned short y, unsigned long color)
{
    if (x < 0 || x > (h_res - 1) || y < 0 || y > ( v_res - 1) )
        return;

    char* pointer = video_mem;
    pointer = pointer + (x  + y*h_res)*bits_per_pixel/8;
    *pointer = color;
}

void square_paint(unsigned short x, unsigned short y,unsigned short size, unsigned long color)
{
    unsigned int i;
    unsigned int u;

    for ( i = 0; i < size; i++ )
    {
        for (u = 0; u < size ; u++)
        {
            pixel_paint(x + i, y + u, color);
        }
    }
}

void line_paint(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned short color)
{
    //algorithm source: https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C

    int dx = abs(xf-xi), sx = xi<xf ? 1 : -1;
      int dy = abs(yf-yi), sy = yi<yf ? 1 : -1;
      int err = (dx>dy ? dx : -dy)/2, e2;

      for(;;)
      {
        pixel_paint(xi,yi, color);
        if (xi==xf && yi==yf) break;

        e2 = err;

        if (e2 >-dx)
        { err -= dy; xi += sx; }
        if (e2 < dy)
        { err += dx; yi += sy; }
      }

      printf("line painted\n");

}

void fillDispay(color)
{
    unsigned int i;
    unsigned int u;

    for ( i = 0; i <= 640; i++ )
    {
        for (u = 0; u <= 480 ; u++)
        {
            pixel_paint(i, u, color);
        }
    }
}

char* get_video_mem()
{
    return video_mem;
}

char* get_double_buffer()
{
    return double_buffer;
}

void DBtoVM()
{
    memcpy(video_mem, double_buffer, vram_size);
}
