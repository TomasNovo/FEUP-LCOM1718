#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "video_gr.h"
#include "smiley.h"
#include "i8042.h"
#include "i8254.h"
#include "bitmap.h"
#include "menu.h"
#include "rtc.h"

int main()
{
	sef_startup();

	vg_init(0x111);
	/*unsigned long s,M,d,m,y,h;
	d = getDay();
	m = getMonth();
	y = getYear();
	h = getHours();
	M = getMinutes();
	s = getSeconds();

	printf("Day: %u",d);
	printf("Month: %u",m);
	printf("Year: %u",y);
	printf("H: %u",h);
	printf("M: %u", M);
	printf("s: %u", s);

*/

	//move_smile();
	//move_kbd();
	//move_test();
	 entering();


	 if(mainMenu() == 0)
	 {

	Smiley* jogo = (Smiley*) startSmiley();


		drawSmiley(jogo, 0,0);
		while (!jogo->done)
		{
			updateSmiley(jogo);
		}

		stopSmiley(jogo);

	gameOver(jogo);

	timeShow();
	 }

	vg_exit();

	return 0;
}

