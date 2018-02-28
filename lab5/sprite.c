#include "sprite.h"
/**Creates a new sprite with pixmap "pic", with specified
*position (within the screen limits) and speed;
*Does not draw the sprite on the screen
*Returns NULL on invalid pixmap.
*/

Sprite *create_sprite(char*pic[], int x, int y,int xspeed, int yspeed)
{
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	if( sp == NULL )
	return NULL;
	// read the sprite pixmap
	sp->map = read_xpm(pic, &(sp->width), &(sp->height));
	if( sp->map == NULL )
	{
	free(sp);
	return NULL;
	}

	sp->x = x;
	sp->y = y;
	sp->xspeed = xspeed;
	sp->yspeed = yspeed;

	return sp;
}

void draw_sprite(Sprite *s)
{
	int wd = s->width;
	int ht = s->height;
	int index = 0;

	unsigned int y = 0;
	unsigned int x = 0;

	for (y = 0; y < wd; y++)
	{
		for (x = 0; x < ht; x++)
		{
			pixel_paint(s->x + x, s->y + y, s->map[index++]  );
		}
	}
}

int sprite_erase(Sprite *s)
{

	int wd = s->width;
	int ht = s->height;


	unsigned int y = 0;
	unsigned int x = 0;

	for (y = 0; y < wd; y++)
	{
		for (x = 0; x < ht; x++)
		{
			pixel_paint(s->x + x, s->y + y, 0 );
		}
	}
	return 0;

}




void destroy_sprite(Sprite *sp)
{
	if( sp == NULL )
		return;
	if( sp ->map )
		free(sp->map);
	free(sp);
	sp = NULL;     // XXX: pointer is passed by value
                   //  should do this @ the caller
}
