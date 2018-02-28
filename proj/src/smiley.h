#pragma once
#include "bitmap.h"

/** @defgroup smiley smiley
 * @{
 */


////////////////////// COLLISION RECTANGLES ///////////////////////////////

typedef struct
{
	int xi; /**< first x coordinate*/
	int xf; /**< last x coordinate*/
	int yi; /**< first y coordinate*/
	int yf; /**< last y coordinate*/

}Rectangle;

/**
 * @brief Create a new Rectangle
 * @param xi first x coordinate
 * @param yi first y coordinate
 * @param xf last x coordinate
 * @param yf last y coordinate
 * @return  new rectangle
 */

Rectangle* newRectangle(int xi, int yi, int xf, int yf);


/**
 * @brief Delete the created rectangle
 * @param rectangle first rectangle
 */

void deleteRectangle(Rectangle* rectangle);


////////////////////////////////// INITIAL BAR STRUCT ////////////////////////////////////////////
typedef struct
{
	int x;			/**< first x coordinate*/
	int y;			/**< first y coordinate*/
	int draw;		/**< draw controller*/

	Bitmap* img;	/**< Bitmap of the ground*/
}Ground;

/**
 * @brief Create a new Ground
 * @return  pointer to a object of ground struct
 */

Ground* startGround();

/**
 * @brief Update the y position
 * @param pointer to a object of gound struct
 */

void updateGround(Ground* ground);

/**
 * @brief Draw the created ground
 * @param pointer to a object of gound struct
 */

void drawGround(Ground* ground);

/**
 * @brief Delete the created ground
 * @param pointer to a object of gound struct
 */

void deleteGround(Ground* ground);


////////////////////////////////// PLAYER STRUCT ////////////////////////////////////////////
typedef struct
{
	int IRQ_SET_KBD;			/**< keyboard irq*/
	int IRQ_SET_TIMER;			/**< timer irq*/
	int x;						/**< x coordinate (central)*/
	int y;						/**< y coordinate (central)*/
	int done;					/**< done controller*/
	int draw;					/**< draw controller*/
	int w;						/**< width in pixels*/
	int h;						/**< height in pixels*/
	unsigned long scancode;		/**< scan code of the keyboard of the last interrupt*/
	short xc;					/**< collision x*/
	short yc; 					/**< collision y*/

	Rectangle* rs;				/**< Rectangle for collisions*/

	Bitmap* emoji;				/**< emoji Bitmap*/
}Smile;

/**
 * @brief Create the game main character
 * @return returns pointer to a object of smile struct
 */

Smile* newSmile();

/**
 * @brief Update smile position
 * @param smile pointer to a object of smile struct
 */

void updateSmile(Smile* smile);

/**
 * @brief draw smile
 * @param smile pointer to a object of smile struct
 * @param x x coordinate
 * @param y y coordinate
 */

void drawSmile(Smile* smile, unsigned short x, unsigned short y);

/**
 * @brief delete smile and unsubscribe keyboard
 * @param smile pointer to a object of smile struct
 */

void deleteSmile(Smile* smile);



////////////////////////////////// PLATFORM STRUCT ////////////////////////////////////////////

typedef struct
{
	//x= 30, y = 15
	int x;				/**< x coordinate (upper left) */
	int y;				/**< x coordinate (upper left) */
	int direction; 		/**< this is flag to see if the platform goes right or left */
	int w;				/**< width in pixels */
	int h;				/**< height in pixels*/
	int raio;			/**< 1/4 of the width, it's for the collisions*/

	int c1x;			/**< x coordinate of the center of the first circumference*/
	int c1y;			/**< y coordinate of the center of the first circumference*/
	int c2x;			/**< x coordinate of the center of the second circumference*/
	int c2y;			/**< y coordinate of the center of the second circumference*/

	int touched;		/**< this is flag to see if the platform touched the main character*/


	Bitmap* image;		/**< platform Bitmap*/
	Rectangle* rp;		/**< Rectangle for collisions*/

}Platform;

/**
 * @brief Create the platform
 * @param img platform Bitmap
 * @return returns pointer to a object of platform struct
 */

Platform* newPlatform(Bitmap* img);

/**
 * @brief Update platform position
 * @param platform pointer to a object of platform struct
 */

void updatePlatform(Platform* platform);

/**
 * @brief draw platform
 * @param platform pointer to a object of platform struct
 */

void drawPlatform(Platform* platform);

/**
 * @brief delete platform
 * @param platform pointer to a object of platform struct
 */

void deletePlatform(Platform* platform);





/////////////////////////////////

/**
 * @brief Calculate the distance between 2 objects
 * @param x1 x coordinate of the first object
 * @param x2 x coordinate of the second object
 * @param y1 y coordinate of the first object
 * @param y2 y coordinate of the second object
 * @return returns the value of the distance
 */

double distance(int x1, int x2, int y1, int y2);

/////////////////////////////////////////////

typedef struct
{

	int value;						/**< score value*/
	Bitmap* digits[10];				/**< score numbers bitmaps*/
	int x1;							/**< coordinate for the score display*/
	int x2;							/**< coordinate for the score displayr*/
	int x3;							/**< coordinate for the score display*/
	int x4;							/**< coordinate for the score display*/
	int y;							/**< coordinate for the score display*/

	unsigned long day;				/**< day that was got from the rtc*/
	unsigned long month;			/**< month that was got from the rtc*/
	unsigned long year;				/**< year that was got from the rtc*/
	unsigned long hours;			/**< hours that was got from the rtc*/
	unsigned long minutes;			/**< minutes that was got from the rtc*/
	unsigned long seconds;			/**< seconds that was got from the rtc*/

}Score;

/**
 * @brief create the score with the value 0 and the position for the display
 * @return score pointer to a object of score struct
 */

Score* startScore();

/**
 * @brief Transform the number to bitmap
 * @param digit number to convert
 * @param score pointer to a object of score struct
 * @return returns the bitmap
 */

Bitmap* scoreDigits(int digit, Score* score);

/**
 * @brief Draw the player's score
 * @param d1 x coordinate of the first object
 * @param d2 x coordinate of the second object
 * @param d3 y coordinate of the first object
 * @param d4 y coordinate of the second object
 * @param score pointer to a object of score struct
 */

void scoreDraw(int d1, int d2, int d3, int d4, Score* score);

/**
 * @brief Update the player score
 * @param score pointer to a object of score struct
 */

void updateScore(Score* score);



////////////////////////////////////  GAME STRUCT ///////////////////////////////////////////
	typedef struct // game struct
	{
		int done;						/**< done controller*/
		int draw;						/**< draw controller*/
		unsigned long scancode;			/**< scan code of the keyboard of the last interrupt*/

		Smile* smile;					/**< pointer to a object of smile struct */
		Platform* platforms[8];			/**< group of 8 platforms */
		Platform* platform;				/**< pointer to a object of platform struct */
		Bitmap* background;				/**< background Bitmap*/
		Ground* floor;					/**< pointer to a object of ground struct */
		Score* score;					/**< pointer to a object of score struct */
		int final_score;				/**< players final score */
	}Smiley;

	/**
	 * @brief Create the game structure
	 * @return returns pointer to a object of smiley struct
	 */

	Smiley* startSmiley();

	/**
	 * @brief Update platform position
	 * @param smiley pointer to a object of smiley struct
	 */

	void updateSmiley(Smiley* smiley);

	/**
	 * @brief draw game background
	 * @param smiley pointer to a object of smiley struct
	 * @param x x coordinate
	 * @param y y coordinate
	 */

	void drawSmiley(Smiley* smiley, unsigned short x, unsigned short y);

	/**
	 * @brief delete smiley
	 * @param smiley pointer to a object of smiley struct
	 */

	void stopSmiley(Smiley* smiley);
