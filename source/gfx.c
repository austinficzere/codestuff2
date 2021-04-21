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

typedef struct {
	unsigned short int color;
	int x, y;
} Pixel;

struct Background {
	// arrray of background
	const struct imageStruct *backgrounds[4];
	int currentB;
} bg = {{&cityImage, &railRoadImage, &runwayImage, &riverImage}, 0};

struct ValuePacks {
	int length;
	const struct imageStruct *vp[];
} vPacks = {8,{NULL,&heartImage,&coinImage,&clockImage, &bootsImage,}};

struct NumbersImg{
	int length;
	const struct imageStruct *nums[];
} NumberImages = {10,{&zeroImage,&oneImage,&twoImage,&threeImage,&fourImage,&fiveImage,&sixImage,&sevenImage,&eightImage,&nineImage}};

struct HUDImg{
	int length;
	const struct imageStruct *itemImgs[];
} HUDImages = {4,{&livesHUDImage,&scoreHUDImage,&stepsHUDImage,&timeHUDImage}};

struct harmObjectImg hObjImg = {7,{&busImage,&bus2Image,&trainImage,&personImage,&planeImage,&logImage,&padImage}};

struct fbs framebufferstruct;


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

const int SCREEN_X = 1280;
const int SCREEN_Y = 720;
const int TRANSPARENT = 1;
const int TO_CLEAR = 1;
const int HUD_LIVES = 0;
const int HUD_SCORE = 1;
const int HUD_STEPS = 2;
const int HUD_TIME = 3;
const int HUD_YOFF = 10;

//munmap(framebufferstruct.fptr, framebufferstruct.screenSize);

void initGFX(){
	framebufferstruct = initFbInfo();
}

void drawWinLoseScreen(struct gameState *prevState,struct gameState *gs){
	int xOff = (SCREEN_X/2) - (winMenuImage.width/2);
	int yOff = (SCREEN_Y/2) - (winMenuImage.height/2);

	if(prevState -> state  != gs -> state){
		if(gs -> hasWon){
			draw((int *)winMenuImage.image_pixels, winMenuImage.width, winMenuImage.height,xOff,yOff,0,!TRANSPARENT);
		} else if(gs -> hasLost){
			draw((int *)loseMenuImage.image_pixels, loseMenuImage.width, loseMenuImage.height,xOff,yOff,0,!TRANSPARENT);
		}
		drawNumber(100,100,gs -> score,!TO_CLEAR);
    }
}

void drawGameState(struct gameState *prevState, struct gameState *gs)
{
	int changeState = prevState -> state != gs -> state;
	if((prevState -> gameStage != gs -> gameStage) || changeState){
		bg.currentB = gs -> gameStage;
		drawBackground(&bg);
	}
	
	drawHUDItem(prevState -> score, gs -> score, 10, HUD_YOFF ,HUD_SCORE);
	drawHUDItem(prevState -> time, gs -> time, 200,HUD_YOFF,HUD_TIME);
	drawHUDItem(prevState -> numbLives, gs -> numbLives, 400,HUD_YOFF, HUD_LIVES);
	drawHUDItem(prevState -> movesLeft, gs -> movesLeft, 600,HUD_YOFF,HUD_STEPS);

	drawMap(prevState -> map[prevState -> gameStage], gs -> map[gs -> gameStage], changeState);
	if((prevState -> frogX != gs -> frogX) || (prevState -> frogY != gs -> frogY) || changeState){
		drawFrog(prevState,gs);
	}
}

void drawNumber(int xOff, int yOff, int number, int toClear){
	int digits[100];
	int i = 0;
	while(number!=0){
		digits[i] = number%10;
		number/=10;
		i++;
	}

	int currXOff = xOff;
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

void drawFrog(struct gameState *prevState, struct gameState *gs){
		// clear 
		int xOff = tileToPixel(SCREEN_X, prevState -> map[prevState -> gameStage].cols, prevState -> frogX);
		int yOff = tileToPixel(SCREEN_Y, prevState -> map[prevState -> gameStage].rows, prevState -> frogY);

		clearObj(&frogImage32,bg.backgrounds[bg.currentB],xOff,yOff);

		// Drawing current frog
		xOff = tileToPixel(SCREEN_X, gs -> map[prevState -> gameStage].cols, gs -> frogX);
		yOff = tileToPixel(SCREEN_Y, gs -> map[prevState -> gameStage].rows, gs -> frogY);
		draw((int *)frogImage32.image_pixels,frogImage32.width,frogImage32.height,xOff,yOff, gs -> orientation,TRANSPARENT);
}

void drawMap(struct gameMap prevMap, struct gameMap gm, int changeState){
	// Loop through each tile and draw any value packs

	for(int i = 0;i<gm.numbOfHarm;i++){
		clearObj(prevMap.hObjs[i].img,bg.backgrounds[bg.currentB],prevMap.hObjs[i].drawX,prevMap.hObjs[i].drawY);
		const struct imageStruct *currImg =  gm.hObjs[i].img;
		draw( (int *)currImg -> image_pixels,currImg -> width, currImg -> height, gm.hObjs[i].drawX,gm.hObjs[i].drawY,gm.hObjs[i].orientation,TRANSPARENT);
	}

	int xOff,yOff;
	for(int i = 0;i<gm.rows;i++){
		for(int j = 0;j<gm.cols;j++){
			xOff = tileToPixel(SCREEN_X, gm.cols, j);
			yOff = tileToPixel(SCREEN_Y, gm.rows, i);
		
			if (gm.table[i][j].valuePack!=0){
				draw((int *)vPacks.vp[gm.table[i][j].valuePack] -> image_pixels, vPacks.vp[gm.table[i][j].valuePack] -> width, 
				vPacks.vp[gm.table[i][j].valuePack] -> height,xOff,yOff,0,TRANSPARENT);
			}else if(gm.table[i][j].valuePack == 0 && prevMap.table[i][j].valuePack !=gm.table[i][j].valuePack){
				clearObj(vPacks.vp[prevMap.table[i][j].valuePack],bg.backgrounds[bg.currentB],xOff,yOff);
			}
		}
	}
}

void drawHUDItem(int prevNumber, int currNumber, int xOff, int yOff, int HUDtype){
	draw((int *)HUDImages.itemImgs[HUDtype] -> image_pixels,HUDImages.itemImgs[HUDtype] -> width,HUDImages.itemImgs[HUDtype] -> height,xOff,yOff, 0,TRANSPARENT);
	drawNumber(xOff+HUDImages.itemImgs[HUDtype] -> width+10,yOff,prevNumber,TO_CLEAR);
	drawNumber(xOff+HUDImages.itemImgs[HUDtype] -> width+10,yOff,currNumber,!TO_CLEAR);
}

void drawMenuScreen(struct gameState *prevState,struct gameState *gs, int menuState){
	if(prevState -> state  != gs -> state){
		draw((int *)startMenuImage.image_pixels, startMenuImage.width, startMenuImage.height,0,0,0,!TRANSPARENT);
    }

	if(menuState == 0){
		clearObj(&menuSelector, &startMenuImage, 535, 505);
		drawSelector(&menuSelector, 535, 415);
	}else{
		clearObj(&menuSelector, &startMenuImage, 535, 415);
		drawSelector(&menuSelector, 535, 505);
	}
}

void drawPauseScreen(struct gameState *prevState,struct gameState *gs, int pauseState){
	int xOff = (SCREEN_X/2) - (pauseMenuImage.width/2);
	int yOff = (SCREEN_Y/2) - (pauseMenuImage.height/2);

	if(prevState -> state  != gs -> state){
        draw((int *)pauseMenuImage.image_pixels, pauseMenuImage.width, pauseMenuImage.height,xOff,yOff,0,!TRANSPARENT);
    }

	if(pauseState == 0){
		clearObj(&pauseSelector, &pauseMenuImage, 574, 423);
		drawSelector(&pauseSelector, 574, 363);
	}else{
		clearObj(&pauseSelector, &pauseMenuImage, 574, 363);
		drawSelector(&pauseSelector, 574, 423);
	}

	if(gs -> state == 2){
		clearObj(&pauseMenuImage, bg.backgrounds[bg.currentB], xOff,yOff);
	}
}

int tileToPixel(int totalPixelLength, int totalTileLength, int currVal)
{
	return (totalPixelLength/totalTileLength)*currVal;
}

void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation, int transparent){

	
	Pixel *pixel = malloc(sizeof(Pixel));
	int i = 0;

	int xSet,xEnd,xInc;
	int ySet,yEnd,yInc;

	int flip = 0;
	if(orientation == -1){
		xSet = width -1;
		ySet = 0;
		xInc = -1;
		yInc = 1;

		xEnd = -1;
		yEnd = height;
	}
	else if(orientation == 0){
		xSet = ySet = 0;
		xInc = yInc = 1;

		xEnd = width;
		yEnd = height;

	}
	else if(orientation == 1){
		flip = 1;

		ySet = height-1;
		yEnd = -1;
		yInc = -1;

		xSet = 0;
		xEnd = width;
		xInc = 1;
	}
	else if(orientation == 2){
		xSet = width - 1;
		ySet = height - 1;

		xInc = yInc = -1;

		yEnd = xEnd = -1;
	}
	else{
		flip = 1;

		ySet = 0;
		yEnd = height;
		yInc = 1;

		xSet = width -1;
		xEnd = -1;
		xInc = -1;
	}

	int y = ySet;
	while(y!=yEnd){
		int x = xSet;
		while(x!=xEnd){
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
void clearObj(const struct imageStruct *img, const struct imageStruct *rplc,int xOff, int yOff){
	Pixel *pixel = malloc(sizeof(Pixel));

	int width = img -> width;
	int height = img -> height;

	int *pixels = (int *) rplc -> image_pixels;

	int i;

	for(int y = 0; y<height;y++){
		for(int x = 0;x<width;x++){
		// replace image with previous item
		i = (rplc -> width ) * (yOff+y) + (xOff+x);
		pixel -> color = i< (rplc -> width * rplc -> height) ? pixels[i] : 0x0;
		pixel -> x = xOff + x;
		pixel -> y = yOff + y;

		drawPixel(pixel);

		}
	}
	free(pixel);
}

void drawBackground(struct Background *bg){
	const struct imageStruct *currBackground = bg -> backgrounds[bg -> currentB];
	draw((int *)currBackground -> image_pixels, currBackground -> width, currBackground -> height,0,0,0,!TRANSPARENT);
}

void drawSelector(const struct imageStruct *image, int xOff, int yOff)
{
	draw((int *)image -> image_pixels, image -> width, image -> height,xOff,yOff,0,TRANSPARENT);
}

/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location = (pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}

unsigned short int getPixel (int x, int y)
{
	long int location = (x + framebufferstruct.xOff) * (framebufferstruct.bits/8) + (y + framebufferstruct.yOff)* framebufferstruct.lineLength;
	return *((unsigned short int*)(framebufferstruct.fptr + location));
}
