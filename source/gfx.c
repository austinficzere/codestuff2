#include "Resources/image.h"
#include "Resources/frogImage32.c"
#include "Resources/city.c"
#include "Resources/bus.c"
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
	const struct imageStruct *vp[8]
} vPacks = {{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}};

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
void drawMenuScreen();
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

void drawMenuScreen(){

}

void drawPauseScreen(){
	
}

int tileToPixel(int totalPixelLength, int totalTileLength, int currVal)
{
	return (totalPixelLength/totalTileLength)*currVal;
}

void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation, int transparent){

		Pixel *pixel = malloc(sizeof(Pixel));
		int i = 0;

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
