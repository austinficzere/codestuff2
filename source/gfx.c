/* 
 * INFORMATION FOR TA
 * Class: CPSC 359
 * Names: Austin Ficzere(30093971) and Manbir Sandhu(30086574)
 * Description: The graphics of the game, handles drawing everything to the screen.
*/

#include "Resources/image.h"
#include "Resources/sprites/frogImage32.c"
#include "Resources/backgrounds/city.c"
#include "Resources/backgrounds/railroads.c"
#include "Resources/backgrounds/runway.c"
#include "Resources/backgrounds/river.c"
#include "Resources/menus/pauseMenu.c"
#include "Resources/menus/startMenu.c"
#include "Resources/menus/loseMenu.c"
#include "Resources/menus/winMenu.c"
#include "Resources/menus/menuSelector.c"
#include "Resources/menus/pauseSelector.c"
#include "Resources/sprites/coinImg.c"
#include "Resources/sprites/heartImg.c"
#include "Resources/sprites/clockImg.c"
#include "Resources/sprites/bootsImg.c"
#include "Resources/HUD/HUDimages.c"
#include "Resources/HUD/numbers.c"
#include "Resources/sprites/bus.c"
#include "Resources/sprites/bus2.c"
#include "Resources/sprites/train.c"
#include "Resources/sprites/plane.c"
#include "Resources/sprites/person.c"
#include "Resources/sprites/log.c"
#include "Resources/sprites/pad.c"
#include "gfx.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include <unistd.h>
#include <math.h>

// pixel struct
typedef struct {
	unsigned short int color;
	int x, y;
} Pixel;

// backhround struct
struct Background {
	// arrray of background
	const struct imageStruct *backgrounds[4];
	int currentB;
} bg = {{&cityImage, &railRoadImage, &runwayImage, &riverImage}, 0};

// value pack struct
struct ValuePacks {
	int length;
	const struct imageStruct *vp[];
} vPacks = {8,{NULL,&heartImage,&coinImage,&clockImage, &bootsImage,}};

// number image struct
struct NumbersImg{
	int length;
	const struct imageStruct *nums[];
} NumberImages = {10,{&zeroImage,&oneImage,&twoImage,&threeImage,&fourImage,&fiveImage,&sixImage,&sevenImage,&eightImage,&nineImage}};

// hud image struct
struct HUDImg{
	int length;
	const struct imageStruct *itemImgs[];
} HUDImages = {4,{&livesHUDImage,&scoreHUDImage,&stepsHUDImage,&timeHUDImage}};

// harm object struct
struct harmObjectImg hObjImg = {7,{&busImage,&bus2Image,&trainImage,&personImage,&planeImage,&logImage,&padImage}};

// frame buffer struct
struct fbs framebufferstruct;

// predefining functions
void drawPixel(Pixel *pixel); 
void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation, int transparent);
void clearObj(const struct imageStruct *img, const struct imageStruct *rplc, int xOff, int yOff);
void drawBackground(struct Background *bg);
unsigned short int getPixel(int x, int y);
void drawMenuScreen();
void drawPauseScreen();
void drawMap(struct gameMap prevMap, struct gameMap gm, int changeState);
int tileToPixel();
void initGFX();
void drawSelector(const struct imageStruct *image, int xOff, int yOff);
void drawNumber(int xOff, int yOff, int number, int toClear);
void drawHUDItem(int prevNumber, int currNumber, int xOff, int yOff, int HUDtype);
void drawFrog(struct gameState *prevState, struct gameState *gs);

// constants
const int SCREEN_X = 1280;
const int SCREEN_Y = 720;
const int TRANSPARENT = 1;
const int TO_CLEAR = 1;
const int HUD_LIVES = 0;
const int HUD_SCORE = 1;
const int HUD_STEPS = 2;
const int HUD_TIME = 3;
const int HUD_YOFF = 10;

// X and Y off for the scores
const int SCORE_XOFF = 580;
const int SCORE_YOFF = 350;

// X off for the score HUD
const int HUD_SCORE_XOFF = 10;

// X off for the time HUD
const int HUD_TIME_XOFF = 200;

// X off for the lives HUD
const int HUD_LIVES_XOFF = 400;

// X off the the steps HUD
const int HUD_STEPS_XOFF = 600;

// X and Y positions for the different selectors
const int SELECTOR_XOFF = 535;
const int SELECTOR_YOFF1 = 415;
const int SELECTOR_YOFF2 = 505;
const int PSEL_XOFF = 574;
const int PSEL_YOFF1 = 363;
const int PSEL_YOFF2 = 423;

/*
@Params: none
@Returns: none
Initialize the frame buffer.
*/
void initGFX(){
	framebufferstruct = initFbInfo();
}

/*
@Params: 
        prevState: pointer to the previous gamestate
		gs: pointer to the current gamestate
@Returns: none
Draws either the win or lose screen based on if the hasWon/hasLost flags are set, also draws the score to the screen.
*/
void drawWinLoseScreen(struct gameState *prevState,struct gameState *gs){
	int xOff = (SCREEN_X/2) - (winMenuImage.width/2);
	int yOff = (SCREEN_Y/2) - (winMenuImage.height/2);

	if(prevState -> state  != gs -> state){
		if(gs -> hasWon){
			draw((int *)winMenuImage.image_pixels, winMenuImage.width, winMenuImage.height,xOff,yOff,0,!TRANSPARENT);
		} else if(gs -> hasLost){
			draw((int *)loseMenuImage.image_pixels, loseMenuImage.width, loseMenuImage.height,xOff,yOff,0,!TRANSPARENT);
		}
		drawNumber(SCORE_XOFF,SCORE_YOFF,gs -> score,!TO_CLEAR);
    }
}

/*
@Params: 
        prevState: pointer to the previous gamestate
		gs: pointer to the current gamestate
@Returns: none
Draws the game state to the screen, including the background, frog, and hud items.
*/
void drawGameState(struct gameState *prevState, struct gameState *gs)
{
	// if the background changed because we went to a different stage of the game, draw the new one
	int changeState = prevState -> state != gs -> state;
	if((prevState -> gameStage != gs -> gameStage) || changeState){
		bg.currentB = gs -> gameStage;
		drawBackground(&bg);
	}
	
	// draw the hud items
	drawHUDItem(prevState -> score, gs -> score, HUD_SCORE_XOFF, HUD_YOFF ,HUD_SCORE);
	drawHUDItem(prevState -> time, gs -> time, HUD_TIME_XOFF,HUD_YOFF,HUD_TIME);
	drawHUDItem(prevState -> numbLives, gs -> numbLives, HUD_LIVES_XOFF, HUD_YOFF, HUD_LIVES);
	drawHUDItem(prevState -> movesLeft, gs -> movesLeft, HUD_STEPS_XOFF, HUD_YOFF,HUD_STEPS);

	// draw the game map
	drawMap(prevState -> map[prevState -> gameStage], gs -> map[gs -> gameStage], changeState);
	if((prevState -> frogX != gs -> frogX) || (prevState -> frogY != gs -> frogY) || changeState || gs -> gameStage == 3){
		drawFrog(prevState,gs);
	}
}

/*
@Params: 
        xOff: x offset where we want to draw
		yOff: y offset where we want to draw
		number: the number we want to display to the screen
		toClear: if we want to clear the number of draw the number
@Returns: none
Draws or clears a number on the screen for the hud items.
*/
void drawNumber(int xOff, int yOff, int number, int toClear){

	// An array to hold the digits
	int digits[100];
	int i = 0;

	// Case for when the number is zero
	if(number == 0){
		digits[0] = 0;
		i++;
	}

	// stores the digits of the number in an array so we can draw them later
	while(number!=0){
		digits[i] = number%10;
		number/=10;
		i++;
	}

	int currXOff = xOff;
	// for each number in the digits array, we draw it, or if toClear is set we clear them.
	for(int j = i-1;j>=0;j--){
		if(!toClear){
			draw((int *)NumberImages.nums[digits[j]] -> image_pixels,NumberImages.nums[digits[j]] -> width,
			NumberImages.nums[digits[j]] -> height, currXOff, yOff , 0,TRANSPARENT);
		}else{
			clearObj(NumberImages.nums[digits[j]], bg.backgrounds[bg.currentB], currXOff, yOff);
		}

		currXOff += 10 + NumberImages.nums[digits[j]] -> width;
	}
}

/*
@Params: 
        prevState: pointer to the previous gamestate
		gs: pointer to the current gamestate
@Returns: none
Draws the frog to the screen and clears the previous position of the frog.
*/
void drawFrog(struct gameState *prevState, struct gameState *gs){
		// Finding the X and Y pixel coordinates of the frog
		int xOff = tileToPixel(SCREEN_X, prevState -> map[prevState -> gameStage].cols, prevState -> frogX);
		int yOff = tileToPixel(SCREEN_Y, prevState -> map[prevState -> gameStage].rows, prevState -> frogY);

		// Clearing the image of the frog if it has moved
		if((prevState -> frogX != gs -> frogX) || (prevState -> frogY != gs -> frogY))
			clearObj(&frogImage32,bg.backgrounds[bg.currentB],xOff,yOff);

		// Drawing current frog
		xOff = tileToPixel(SCREEN_X, gs -> map[prevState -> gameStage].cols, gs -> frogX);
		yOff = tileToPixel(SCREEN_Y, gs -> map[prevState -> gameStage].rows, gs -> frogY);
		draw((int *)frogImage32.image_pixels,frogImage32.width,frogImage32.height,xOff,yOff, gs -> orientation,TRANSPARENT);
}

/*
@Params: 
        prevMap: pointer to the previous gamemap
		gm: pointer to the current gamemap
		changeState: 
@Returns: none
Draws harm objects and draws value packs to the screen.
*/
void drawMap(struct gameMap prevMap, struct gameMap gm, int changeState){

	// clear previous position of harm object and draw new one
	for(int i = 0;i<gm.numbOfHarm;i++){
		clearObj(prevMap.hObjs[i].img,bg.backgrounds[bg.currentB],prevMap.hObjs[i].drawX,prevMap.hObjs[i].drawY);
		const struct imageStruct *currImg =  gm.hObjs[i].img;
		draw( (int *)currImg -> image_pixels,currImg -> width, currImg -> height, gm.hObjs[i].drawX,gm.hObjs[i].drawY,gm.hObjs[i].orientation,TRANSPARENT);
	}

	// draw value packs or clear them of they have been picked up
	int xOff,yOff;
	for(int i = 0;i<gm.rows;i++){
		for(int j = 0;j<gm.cols;j++){

			// Finding the x and y positions of current tile
			xOff = tileToPixel(SCREEN_X, gm.cols, j);
			yOff = tileToPixel(SCREEN_Y, gm.rows, i);
		
			// Checking if there is a value pack at current tile
			if (gm.table[i][j].valuePack!=0){
				// Drawing the value pack
				draw((int *)vPacks.vp[gm.table[i][j].valuePack] -> image_pixels, vPacks.vp[gm.table[i][j].valuePack] -> width, 
				vPacks.vp[gm.table[i][j].valuePack] -> height,xOff,yOff,0,TRANSPARENT);
			}else if(gm.table[i][j].valuePack == 0 && prevMap.table[i][j].valuePack !=gm.table[i][j].valuePack){
				// Clearing the value pack if frog has picked it up
				clearObj(vPacks.vp[prevMap.table[i][j].valuePack],bg.backgrounds[bg.currentB],xOff,yOff);
			}
		}
	}
}

/*
@Params: 
        prevNumber: the number we want to clear
		currNumber: the number we want to draw
		xOff: x offset where we want to draw
		yOff: y offset where we want to draw
		HUDtype: the hud we are drawing
@Returns: none
Draws the hud items to the screen (score, lives, time, moves left)
*/
void drawHUDItem(int prevNumber, int currNumber, int xOff, int yOff, int HUDtype){

	// Drawing the HUD item
	draw((int *)HUDImages.itemImgs[HUDtype] -> image_pixels,HUDImages.itemImgs[HUDtype] -> width,HUDImages.itemImgs[HUDtype] -> height,xOff,yOff, 0,TRANSPARENT);

	// Clearing previous number and drawing current
	drawNumber(xOff+HUDImages.itemImgs[HUDtype] -> width+10,yOff,prevNumber,TO_CLEAR);
	drawNumber(xOff+HUDImages.itemImgs[HUDtype] -> width+10,yOff,currNumber,!TO_CLEAR);
}

/*
@Params: 
        prevState: pointer to the previous gamestate
		gs: pointer to the current gamestate
		menuState: the state of the menu we are in
@Returns: none
Draws the menu screen and the selectors for the buttons.
*/
void drawMenuScreen(struct gameState *prevState,struct gameState *gs, int menuState){

	// Chceking if previous state is not equal to current
	if(prevState -> state  != gs -> state){
		// Drawing menu screen
		draw((int *)startMenuImage.image_pixels, startMenuImage.width, startMenuImage.height,0,0,0,!TRANSPARENT);
    }

	if(menuState == 0){
		// Clearing the previous menu selector
		clearObj(&menuSelector, &startMenuImage, SELECTOR_XOFF, SELECTOR_YOFF2);

		// Drawing the new selector
		drawSelector(&menuSelector, SELECTOR_XOFF, SELECTOR_YOFF1);
	}else{
		// Clearing the previous menu selector
		clearObj(&menuSelector, &startMenuImage, SELECTOR_XOFF, SELECTOR_YOFF1);

		// Drawing the new selector
		drawSelector(&menuSelector, SELECTOR_XOFF, SELECTOR_YOFF2);
	}
}

/*
@Params: 
        prevState: pointer to the previous gamestate
		gs: pointer to the current gamestate
		pauseState: the state of the pause screen we are in
@Returns: none
Draws the pause screen and the selectors for the buttons.
*/
void drawPauseScreen(struct gameState *prevState,struct gameState *gs, int pauseState){
	// X and Y offsets for the centre of the screen
	int xOff = (SCREEN_X/2) - (pauseMenuImage.width/2);
	int yOff = (SCREEN_Y/2) - (pauseMenuImage.height/2);

	// Drawing pause screen if the current state is not equal to the previous state
	if(prevState -> state  != gs -> state){
		draw((int *)pauseMenuImage.image_pixels, pauseMenuImage.width, pauseMenuImage.height,xOff,yOff,0,!TRANSPARENT);
    }

	// Redrawing pause screen and new selector
	if(pauseState == 0){
		draw((int *)pauseMenuImage.image_pixels, pauseMenuImage.width, pauseMenuImage.height,xOff,yOff,0,!TRANSPARENT);
		drawSelector(&pauseSelector, 574, PSEL_YOFF1);
	}else{
		//  Redrawing the pause screen and new selector
		draw((int *)pauseMenuImage.image_pixels, pauseMenuImage.width, pauseMenuImage.height,xOff,yOff,0,!TRANSPARENT);
		drawSelector(&pauseSelector, 574, PSEL_YOFF2);
	}

	if(gs -> state == 2){
		clearObj(&pauseMenuImage, bg.backgrounds[bg.currentB], xOff,yOff);
	}

}

/*
@Params: 
        totalPixelLength: the length of the pixel
		totalTileLength: the length of the tile
		currVal: value used to determine where on screen we are
@Returns: the spot where we draw the image based on pixels and not on tiles
Converts the tile x/y value to a pixel value.
*/
int tileToPixel(int totalPixelLength, int totalTileLength, int currVal)
{
	return (totalPixelLength/totalTileLength)*currVal;
}

/*
@Params: 
        pixels: the pointer to the pixels we want to draw
		width: width of the pixels
		height: height of the pixels
		xOff: x offset where we want to draw
		yOff: y offset where we want to draw
		orientation: the orientation we want to draw the image in (up, down, left, right)
		transparent: if the background is trnsparent of not in the image
@Returns: none
Draws some pixels to the screen.
*/
void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation, int transparent){

	
	Pixel *pixel = malloc(sizeof(Pixel));
	int i = 0;

	int xSet,xEnd,xInc;
	int ySet,yEnd,yInc;

	// Setting the values of the looop condition based on the orientation of the iamge
	int flip = 0;
	if(orientation == -1){

		// This orientation is for a reflection on the y axis of the image
		xSet = width -1;
		ySet = 0;
		xInc = -1;
		yInc = 1;

		xEnd = -1;
		yEnd = height;
	}
	else if(orientation == 0){

		// Standard orientation of the image
		xSet = ySet = 0;
		xInc = yInc = 1;

		xEnd = width;
		yEnd = height;

	}
	else if(orientation == 1){

		// Orientation for a 90 degree rotation clockwise
		flip = 1;

		ySet = height-1;
		yEnd = -1;
		yInc = -1;

		xSet = 0;
		xEnd = width;
		xInc = 1;
	}
	else if(orientation == 2){
		// Orientation for a 180 degree rotation clockwise
		xSet = width - 1;
		ySet = height - 1;

		xInc = yInc = -1;

		yEnd = xEnd = -1;
	}
	else{

		// Orientation for a 270 degree rotation clockwise
		flip = 1;

		ySet = 0;
		yEnd = height;
		yInc = 1;

		xSet = width -1;
		xEnd = -1;
		xInc = -1;
	}

	// drawing each pixel to the screen

	// Setting the x and y conidation of the loop
	int y = ySet;
	while(y!=yEnd){
		int x = xSet;
		while(x!=xEnd){

			// Drawing a pixel if the image is not transparent, or it is transparent but the pixel color is 0
			if(!transparent || (transparent && pixels[i]!=0)){
				pixel -> color = pixels[i];
				pixel -> x = xOff + (flip ? y : x);
				pixel -> y = yOff + (flip ? x : y);	
				drawPixel(pixel);
			}
			i++;
			x += xInc;
		}
		y += yInc;
	}
	
	free(pixel);
}

/*
@Params: 
        img: pointer to the image we want to clear
		rplc: pointer to the image we replace the image we are clearing with
		xOff: x offset where we want to clear
		yOff: y offset where we want to clear
@Returns: none
Clears an image from the screen and replaces it with another
*/
void clearObj(const struct imageStruct *img, const struct imageStruct *rplc,int xOff, int yOff){
	Pixel *pixel = malloc(sizeof(Pixel));

	// Storing width and heigh of the image that will be replaced
	int width = img -> width;
	int height = img -> height;

	// Getting pixels array
	int *pixels = (int *) rplc -> image_pixels;

	int i;

	for(int y = 0; y<height;y++){
		for(int x = 0;x<width;x++){
		// replace image with previous item

		// Calulcating the pixel position for which the replacement is on top of the original
		i = (rplc -> width ) * (yOff+y) + (xOff+x);

		pixel -> color = i< (rplc -> width * rplc -> height) ? pixels[i] : 0x0;
		pixel -> x = xOff + x;
		pixel -> y = yOff + y;

		// Drawing the replacement image over the previous
		drawPixel(pixel);

		}
	}
	free(pixel);
}

/*
@Params: 
        bg: pointer to the background struct
@Returns: none
Draws the background to the screen.
*/
void drawBackground(struct Background *bg){
	const struct imageStruct *currBackground = bg -> backgrounds[bg -> currentB];
	draw((int *)currBackground -> image_pixels, currBackground -> width, currBackground -> height,0,0,0,!TRANSPARENT);
}

/*
@Params: 
        image: pointer to the image we want to draw
		xOff: x offset where we want to draw
		yOff: y offset where we want to draw
@Returns: none
Draws the button selector to the correct position to select the correct button.
*/
void drawSelector(const struct imageStruct *image, int xOff, int yOff)
{
	draw((int *)image -> image_pixels, image -> width, image -> height,xOff,yOff,0,TRANSPARENT);
}

/*
@Params: 
        pixel: pointer to the pixel we are drawing
@Returns: none
Draws a pixel to the screen.

This code was based on the code provided in the tutorial
*/
void drawPixel(Pixel *pixel){
	long int location = (pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}

/*
@Params: 
        x: x value of pixel we want
		y: y value of pixel we want
@Returns: the colour value at the current pixel
Gets the colour of the current pixel.
*/
unsigned short int getPixel (int x, int y)
{
	long int location = (x + framebufferstruct.xOff) * (framebufferstruct.bits/8) + (y + framebufferstruct.yOff)* framebufferstruct.lineLength;
	return *((unsigned short int*)(framebufferstruct.fptr + location));
}
