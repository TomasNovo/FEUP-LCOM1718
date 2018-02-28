#ifndef __SPRITE_H
#define __SPRITE_H

#include "read_xpm.h"
#include "video_gr.h"

typedef struct
{
int x, y; // current position
int width, height;  // dimensions
int xspeed, yspeed; // current speed
char*map;          // the pixmap
} Sprite;



Sprite *create_sprite(char*pic[], int x, int y,int xspeed, int yspeed);
void draw_sprite(Sprite *s);
void destroy_sprite(Sprite *sp);


#endif

