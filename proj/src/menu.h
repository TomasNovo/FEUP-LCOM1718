#pragma once
#include "bitmap.h"
#include "smiley.h"
#include "test4.h"
#include "video_gr.h"
#include "rtc.h"

///////////////////////////


/** @defgroup menu menu
 * @{
 */

typedef struct
{
	int x1;				/**< x coordinate for the first option*/
	int y1;				/**< y coordinate for the first option*/
	int x2;				/**< x coordinate for the second option*/
	int y2;				/**< y coordinate for the second option*/
	Bitmap* img1;		/**< option1 Bitmap*/
	Bitmap* img2;		/**< option2 Bitmap*/
	int done;			/**< done controller*/
}OptionRectangle;

/**
 * @brief Create the option button
 * @return returns pointer to a object of OptionRectangle struct
 */

OptionRectangle* newORectangle();

/**
 * @brief Delete the option button
 * @param rectangle option button
 */

void deleteORectangle(OptionRectangle* rectangle);

/////////////////////////////////

typedef struct
{
	int x;				/**< initial x coordinate*/
	int y;				/**< initial y coordinate*/
	int done;			/**< done controller*/

	Bitmap* img;		/**< mouse Bitmap*/
}Mouse;

/**
 * @brief Create mouse
 * @return mouse pointer to a object of Mouse struct
 */

Mouse* startMouse();

/**
 * @brief Draw mouse
 * @param mouse pointer to a object of Mouse struct
 */

void drawMouse(Mouse* mouse);

/**
 * @brief Delete mouse
 * @param mouse pointer to a object of Mouse struct
 */

void deleteMouse(Mouse* mouse);

/**
 * @brief entering menu
 */

void entering();

/**
 * @brief Main Menu
 * @return 0 if success and -1 if not
 */

int mainMenu();

/**
 * @brief Game Over menu
 */

void gameOver();

/**
 * @brief Score Menu
 * @param smiley smiley score
 */

void scoreMenu(Smiley* smiley);


typedef struct
{

	Bitmap* digits[10];			/**< Bitmaps of digits from 0 to 9*/
	int x;						/**< x coordinate for the date*/
	int x2;						/**< x coordinate for the time*/
	int y;						/**< x coordinate for the date*/
	int y2;						/**< x coordinate for the time*/

	unsigned long day;			/**< day that was got from the rtc*/
	unsigned long month;		/**< month that was got from the rtc*/
	unsigned long year;			/**< year that was got from the rtc*/
	unsigned long hours;		/**< hours that was got from the rtc*/
	unsigned long minutes;		/**< minutes that was got from the rtc*/
	unsigned long seconds;		/**< seconds that was got from the rtc*/

}Date;

/**
 * @brief Create date
 * @return date pointer to a object of Date struct
 */

Date* startDate();

/**
 * @brief Convert the date to Bitmaps
 * @param digit digits to convert
 * @param date pointer to a object of Date struct
 * @return mouse pointer to a object of Mouse struct
 */

Bitmap* digits(int digit,Date* date);

/**
 * @brief Draw the date
 * @param date pointer to a object of Date struct
 */

void drawDate(Date* date);

/**
 * @brief Draw the time
 * @param date pointer to a object of Date struct
 */

void drawTime(Date* date);
