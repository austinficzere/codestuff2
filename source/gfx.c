#include "Resources/image.h"
#include "Resources/frogImage32.c"
#include "Resources/city.c"
#include "Resources/bus.c"
#include "Resources/pauseMenu.c"
#include "Resources/bus2.c"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include <unistd.h>
#include "gamelogic.h"

typedef struct {
	int color;
	int x, y;
} Pixel;

struct Background {
	// arrray of background
	const struct imageStruct *backgrounds[4];
	int currentB;
} bg = {{&cityImage, NULL, NULL, NULL}, 0};

struct ValuePacks {
	int length;
	const struct imageStruct *vp[];
} vPacks = {8,{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}};

struct harmObjectImg {
	int length;
	const struct imageStruct *imgs[]
} hObjImg = {2,{&busImage,&bus2Image}};

struct fbs framebufferstruct;

void drawPixel(Pixel *pixel); 
void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation, int transparent);
void clearObj(const struct imageStruct *img, const struct imageStruct *rplc, int xOff, int yOff);
void drawBackground(struct Background *bg);
unsigned short int getPixel(int x, int y);
void drawGameState(struct gameState *prevState,struct gameState *gs);
void drawScore();
void drawTime();
void drawLives();
void drawSteps();
//void drawMenuScreen(struct MenuScreen *ms);
void drawPauseScreen();
void drawMap(struct gameMap prevMap, struct gameMap gm);
int tileToPixel();
void initGFX();

const int SCREEN_X = 1280;
const int SCREEN_Y = 720;
const int TRANSPARENT = 1;

//munmap(framebufferstruct.fptr, framebufferstruct.screenSize);

void initGFX(){
	framebufferstruct = initFbInfo();
	drawBackground(&bg);
}

void drawGameState(struct gameState *prevState,struct gameState *gs)
{
	if(prevState -> score != gs -> score)
		drawScore();
	if(prevState -> time != gs -> time)
		drawTime();
	if(prevState -> numbLives != gs -> numbLives)
		drawLives();
	if(prevState -> movesLeft != gs -> movesLeft)
		drawSteps();

	drawMap(prevState -> map, gs -> map);
}

void drawMap(struct gameMap prevMap, struct gameMap gm){
	// Loop through each tile and draw any value packs

	int xOff,yOff;
	for(int i = 0;i<gm.rows;i++){
		for(int j = 0;j<gm.cols;j++){
			xOff = tileToPixel(SCREEN_X, gm.cols, j);
			yOff = tileToPixel(SCREEN_Y, gm.rows, i);

			if(gm.table[i][j].valuePack != 0){
				if(prevMap.table[i][j].valuePack != gm.table[i][j].valuePack){
					// clear previous value

					// Draw current 
				}
			}
		}
	}

	for(int i = 0;i<gm.numbOfHarm;i++){
		clearObj(prevMap.hObjs[i].img,&cityImage,prevMap.hObjs[i].drawX,prevMap.hObjs[i].drawY);
		const struct imageStruct *currImg =  gm.hObjs[i].img;
		draw( (int *)currImg -> image_pixels,currImg -> width, currImg -> height, gm.hObjs[i].drawX,gm.hObjs[i].drawY,gm.hObjs[i].orientation,TRANSPARENT);
	}

	// Draw frog
	if((prevMap.frogX != gm.frogX) || (prevMap.frogY != gm.frogY)){
		// clear 
		xOff = tileToPixel(SCREEN_X, prevMap.cols, prevMap.frogX);
		yOff = tileToPixel(SCREEN_Y, prevMap.rows, prevMap.frogY);

		clearObj(&frogImage32,&cityImage,xOff,yOff);

		// Drawing current frog
		xOff = tileToPixel(SCREEN_X, gm.cols, gm.frogX);
		yOff = tileToPixel(SCREEN_Y, gm.rows, gm.frogY);
		draw((int *)frogImage32.image_pixels,frogImage32.width,frogImage32.height,xOff,yOff, gm.orientation,TRANSPARENT);
	}

}

void drawScore(){

}

void drawTime(){

}

void drawLives(){

}

void drawSteps()
{

}

//void drawMenuScreen(struct MenuScreen *ms){
//	draw((int *)menuScreenImage.image_pixels, menuScreenImage.width, menuScreenImage.height,0,0,0,!TRANSPARENT);
//}

void drawPauseScreen(){
	int xOff = (SCREEN_X/2) - (pauseMenuImage.width/2);
	int yOff = (SCREEN_Y/2) - (pauseMenuImage.height/2);
	draw((int *)pauseMenuImage.image_pixels, pauseMenuImage.width, pauseMenuImage.height,xOff,yOff,0,!TRANSPARENT);
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
		
		if(orientation == 0){
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

		/*
		for(int y = 0; y<height;y++){
			for(int x = 0;x<width;x++){
				if(!transparent || (transparent && pixels[i]!=0)){
					pixel -> color = pixels[i];
					pixel -> x = xOff + x;
					pixel -> y = yOff + y;	
					drawPixel(pixel);
				}
				i++;
			}
		}
		*/
		
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
