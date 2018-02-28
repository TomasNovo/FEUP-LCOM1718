#include <minix/drivers.h>
#include <minix/driver.h>
#include "smiley.h"

#include "video_gr.h"
#include "kbd.h"
#include "i8042.h"
#include "bitmap.h"
#include "timer.h"
#include "test4.h"
#include "rtc.h"
#include <stdio.h>
#include <math.h>

const int fps=25;

float GRAVITY = 1;


Smiley* startSmiley()
{
	Smiley* smiley = (Smiley*) malloc(sizeof(Smiley));
	smiley->done = 0, smiley->draw = 1;
	smiley->background = loadBitmap("/home/lcom/svn/proj/doc/background.bmp");
    smiley->floor = (Ground*) startGround();


	smiley->smile = (Smile*) newSmile();


	Bitmap* platform_img = loadBitmap("/home/lcom/svn/proj/doc/platform.bmp");

	smiley->platform = (Platform*) newPlatform(platform_img);


	printf("starting platforms");

	unsigned int i = 0;

	for (i=0; i < 8; i++)
	{
		smiley->platforms[i] = newPlatform(platform_img);
	}

	smiley->score = (Score*) startScore();
	smiley->final_score = (Score*) startScore();

	return smiley;
}

void drawSmiley(Smiley* smiley,unsigned short x,unsigned short y)
{
	drawBitmap1(smiley->background,0,0 ,ALIGN_LEFT);
}


void updateSmiley(Smiley* smiley)
{
	vg_init(0x111);
	int ipc_status, r = 0;
	message msg;

	unsigned long irq_set_timer;
	unsigned long irq_set_keyboard;
	unsigned long irq_set_mouse;

	unsigned int counter = 0;
	unsigned long timer;

	unsigned int i;


	irq_set_timer = timer_subscribe_int(); //subscribes timer
	if (irq_set_timer == -1)
	{
		printf("Error on time_subcribe_int() function\n");
		return 1;
	}

	irq_set_keyboard = kbd_subscribe(); //subscribes keyboard
	if (irq_set_keyboard == -1)
	{
		printf("Error on kbd_subscribe() function\n");
		return 1;
	}

	irq_set_mouse = mouse_subscribe();
	if (irq_set_mouse == -1)
	{
		printf("Error on mouse_subscribe() function\n");
		return 1;
	}

	if (send_mouse(ENABLE) != 0)  // enable data reporting
		{
			printf("Error in send_mouse() function\n");
			return -1;
		}

	unsigned long data;

	size_t packet_index = 0;
	unsigned short packet[3];
	int npackets = 0;

	unsigned int score = 0;
	int scorecounter = 0;

	while(smiley->smile->scancode != ESC)
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
		{
			printf("driver_receive() function failed with: %d \n", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) //received notification
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: //hardware interrupt notification
			{
				if (msg.NOTIFY_ARG & irq_set_mouse) {

					sys_inb(OUT_BUF, &data);

					packet[packet_index] = data;

					if (packet[0] & MOUSE_SYNC) {
						packet_index++;
					} else {
						packet_index = 0;
					}

					if (packet_index >= 3) {
						packet_index = 0;
						npackets++;

						if (packet[0] & BIT(4)) {
							packet[1] = ~packet[1] & 0xFF;
							packet[1] = packet[1] + 1;
							smiley->smile->x -= 8;
						} else
							smiley->smile->x += 8;
					}
				}


				if (msg.NOTIFY_ARG & irq_set_keyboard)
				{
					smiley->smile->scancode = kbd_data_reader();
				}

				if (msg.NOTIFY_ARG & irq_set_timer) {


					score++;


		/*			for ( i = 0; i < 5; i++)
					{
						if ((smiley->platforms[i]->x < smiley->smile->xc) &&  (smiley->smile->xc < smiley->platforms[i]->pxc))
							if (smiley->smile->yc == smiley->platforms[i]->y)
                                     smiley->smile->y == 30 ;
					}
*/

					drawSmiley(smiley, 0, 0);
					drawGround(smiley->floor);
					//drawSmiley(smiley,0,0);


					if ((smiley->smile->y + smiley->smile->h) >= smiley->floor->y)
											break;


					updateSmile(smiley->smile);



					for (i = 0; i < 8 ; i++)
					{


						drawPlatform(smiley->platforms[i]);

						int smile_center_x = smiley->smile->x;
						int smile_center_y = smiley->smile->y + (smiley->smile->h / 2);

if ( (distance(smile_center_x, smiley->platforms[i]->c1x, smile_center_y, smiley->platforms[i]->c1y) < smiley->platforms[i]->raio + (smiley->smile->w/2)) || (distance(smile_center_x, smiley->platforms[i]->c2x, smile_center_y, smiley->platforms[i]->c2y) < smiley->platforms[i]->raio + (smiley->smile->w / 2)))
{
	smiley->platforms[i]->touched = 1;
	smiley->score->value++;
	smiley->smile->y -= 70;
}

/*
if (smiley->smile->touched == 1)
	*/



						updatePlatform(smiley->platforms[i]);
						//printf("oi ?");
					}




					updateScore(smiley->score);

					DBtoVM();


				}

				break;

			}
			default:break;

			}
	}
	smiley->done = 1;
	deleteSmile(smiley->smile);
	deletePlatform(smiley->platform);


	smiley->final_score = smiley->score;


	if (send_mouse(DISABLE) != 0)  // disable data reporting
			{
		printf("Error in send_mouse() function\n");
		return -1;
	}

	if (mouse_unsubscribe() == -1) {
		printf("Error on mouse_unsubscribe() function \n");
		return 1;
	}


	if (timer_unsubscribe_int() == -1)
	{
		printf("Error on timer_unsubscribe_int() function \n");
		return 1;
	}

	if (kbd_unsubscribe() == -1)
	{
		printf("Error on kbd_unsubscrive() function \n");
		return 1;
	}

	deleteBitmap(smiley->background);
	deleteBitmap(smiley->smile->emoji);

	for (i = 0; i < 8; i++)
	{
		deleteBitmap(smiley->platforms[i]->image);
	}

	deleteBitmap(smiley->floor);


	vg_exit();
}


void stopSmiley(Smiley* smiley)
{
	kbd_unsubscribe();
	free(smiley);
}

/////////////////*


Platform* newPlatform(Bitmap* img)
{
	Platform* platform = (Platform*) malloc(sizeof(Platform));
	platform->x = rand() % 480 + 0 ;
	platform->y = rand() % 400 + 50;
	platform->image = img;

	platform->w = 30;
	platform->h = 15;

	platform->raio = (platform->w / 4);

	platform->c1x = platform->x + platform->raio;
	platform->c1y = platform->y + platform->raio;
	platform->c2x = platform->x +  3*platform->raio;
	platform->c2y = platform->c1y;

	platform->direction = rand() % 2; // 0 = R, 1 = L
	platform->touched = 0;

	platform->rp = newRectangle(platform->x,platform->y, platform->x + 30, platform->y + 15);

	return platform;
}

void updatePlatform(Platform* platform)
{
 //a plataforma vai subir à medida que o boneco progride

	if (platform->direction == 0)
		{
		    platform->x += 1;
		    if (platform->x == 630)
		    {
		    	platform->x = 2;
		    }
		}

	if (platform->direction == 1)
	{
		platform->x -= 1;
		if (platform->x == 2)
		{
			platform->x = 630;
		}
	}

	platform->c1x = platform->x + platform->raio;
	platform->c1y = platform->y + platform->raio;
	platform->c2x = platform->x +  3*platform->raio;
	platform->c2y = platform->c1y;

}

void drawPlatform(Platform* platform)
{
	drawBitmap1(platform->image, platform->x, platform->y, ALIGN_LEFT);
}

void deletePlatform(Platform* platform)
{
	free(platform);
}


//////////////////////

Smile* newSmile()
{
	Smile* smile = (Smile*) malloc(sizeof(Smile));
	smile->x = 320;
	smile->y = 0;
	//smile->IRQ_SET_KBD = kbd_subscribe();

	smile->w = 62; // 62
	smile->h = 62;
	smile->done = 0;
	smile->draw = 1;


	smile->xc = smile->x; // x final
	smile->yc = smile->y + smile->w; // y final9

	smile->emoji = loadBitmap("/home/lcom/svn/proj/doc/gg.bmp");


	smile->rs = newRectangle(smile->x, smile->y, smile->x + 62, smile->y + 62);

	return smile;
}


void updateSmile(Smile* smile)
{
	//drawSmile(smile, smile->x, smile->y);

    smile->y += GRAVITY;


    if (smile->x == 640)
    	smile->x = 0;


    if (smile->x == 0)
    	smile->x = 640;



    if (smile->scancode == 0x1E) {
		if (smile->x > 0) {
			//square_paint(x, y, 100, 1);
			smile->x -= 10;
		//	drawSmile(smile, smile->x, smile->y);
		}
	}

	if (smile->scancode == 0x20) {

		if (smile->x < 540) {
			//square_paint(x, y, 100, 1);
			smile->x += 10;
			//drawSmile(smile, smile->x, smile->y);
		}
	}

	/*if (smile->scancode == 0x11) {
		if (smile->y > 0) {
			//square_paint(x, y, 100, 1);
			smile->y -= 10;
			//drawSmile(smile, smile->x, smile->y);
		}
	}*/

	/*if (smile->scancode == 0x1F) {
		if (smile->y < 380) {
			//square_paint(x, y, 100, 1);
			smile->y += 10;
			drawSmile(smile, smile->x, smile->y);
		}
	}*/

	if(smile->x + smile->w / 2 >= 0)
	drawSmile(smile, smile->x, smile->y);
}

void drawSmile(Smile* smile, unsigned short x, unsigned short y)
{


    if (smile->x == 640)
    	smile->x = 0;


    if (smile->x == 0)
    	smile->x = 640;


	if(smile->x + smile->w / 2 >= 0)
	drawBitmap1(smile->emoji, smile->x, smile->y, ALIGN_CENTER);
}
void deleteSmile(Smile* smile)
{
	kbd_unsubscribe();
	free(smile);
}

///////////////////////////////////////////////////////////////////////////////////////////



Ground* startGround()
{
	Ground* ground = (Ground*) malloc(sizeof(Ground));
	ground->x = 0 ;
	ground->y = 450;
	ground->img= loadBitmap("/home/lcom/svn/proj/doc/ground.bmp");

	return ground;
}

void updateGround(Ground* ground)
{
	ground->y += 1;
}

void drawGround(Ground* ground)
{
	drawBitmap1(ground->img, ground->x, ground->y, ALIGN_LEFT);
}

void deleteGround(Ground* ground)
{
	free(ground);
}

////////////////////////////////7

Rectangle* newRectangle(int xi, int yi, int xf, int yf)
{
	Rectangle* rectangle = (Rectangle*) malloc(sizeof(Rectangle));
	rectangle->xi = xi;
	rectangle->xf = xf;
	rectangle->yi = yi;
	rectangle->yf = yf;

}
int rectanglesCollision(Rectangle* r1, Rectangle* r2)
{
	return !(r2->xi > r1->xf || r2->xf < r1->xi || r2->yi > r1->yf || r2->yf < r1->yi);
}

void deleteRectangle(Rectangle* rectangle)
{
	free(rectangle);
}

/////////////////

Score* startScore()
{
	Score* score = (Score*) malloc(sizeof(Score));

	score->value = 0;
	score->x1 = 610;
	score->x2 = 580;
	score->x3 = 550;
	score->x4 = 520;
	score->y = 420;

	score->digits[0] = loadBitmap("/home/lcom/svn/proj/doc/1.bmp");
	score->digits[1] = loadBitmap("/home/lcom/svn/proj/doc/2.bmp");
	score->digits[2] =  loadBitmap("/home/lcom/svn/proj/doc/3.bmp");
	score->digits[3] =  loadBitmap("/home/lcom/svn/proj/doc/4.bmp");
	score->digits[4] = loadBitmap("/home/lcom/svn/proj/doc/5.bmp");
	score->digits[5] = loadBitmap("/home/lcom/svn/proj/doc/6.bmp");
	score->digits[6] =  loadBitmap("/home/lcom/svn/proj/doc/7.bmp");
	score->digits[7] =  loadBitmap("/home/lcom/svn/proj/doc/8.bmp");
	score->digits[8] = loadBitmap("/home/lcom/svn/proj/doc/9.bmp");
	score->digits[9] = loadBitmap("/home/lcom/svn/proj/doc/0.bmp");


	score->day = getDay();
	score->month = getMonth();
	score->year = getYear();
	score->hours = getHours();
	score->minutes = getMinutes();
	score->seconds = getSeconds();
	return score;
}

Bitmap* scoreDigits(int digit, Score* score)
{
	switch (digit)
	{
	case 1:
		return score->digits[0];

	case 2:
		return score->digits[1];

	case 3:
		return score->digits[2];

	case 4:
		return score->digits[3];;

	case 5:
		return score->digits[4];

	case 6:
		return score->digits[5];

	case 7:
		return score->digits[6];

	case 8:
		return score->digits[7];

	case 9:
		return score->digits[8];

	case 0:
		return score->digits[9];
	}
}

void scoreDraw(int d1, int d2, int d3, int d4, Score* score)
{
	Bitmap* d1b = scoreDigits(d1,score);

	Bitmap* d2b = scoreDigits(d2,score);

	Bitmap* d3b = scoreDigits(d3,score);

	Bitmap* d4b = scoreDigits(d4,score);


	drawBitmap1(d1b,610,420,ALIGN_CENTER);

	drawBitmap1(d2b,(610-30),420,ALIGN_CENTER);

	drawBitmap1(d3b,(610-60),420,ALIGN_CENTER);

	drawBitmap1(d4b,(610-90),420,ALIGN_CENTER);

}



void updateScore(Score* score)
{

	int d1; //1 digito a contar da direita
		d1 = score->value % 10;
	int d2;
		d2 = (score->value % 100)/10;
	int d3;
		d3 = (score->value % 1000)/100;
	int d4; // 4 digito a contar da direita
		d4 = (score->value % 10000)/1000;

		scoreDraw(d1,d2,d3,d4, score);

}
/////////////////

double distance(int x1, int x2, int y1, int y2)
{
	return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}





