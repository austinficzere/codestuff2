#include "Resources/image.h"
#include "Resources/frog.c"
#include "Resources/city.c"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include <unistd.h>
#include "controller.h"
#include <pthread.h>
#include "global.h"
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
void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation);
void clearObj(const struct imageStruct *img, int xOff, int yOff);
void drawBackground(struct Background *bg);
unsigned short int getPixel(int x, int y);
void drawGameState(struct gameState *gs);
void drawScore();
void drawTime();
void drawLives();
void drawSteps();
void drawMap(struct gameMap gm);
int tileToPixel();

const int SCREEN_X = 1280;
const int SCREEN_Y = 720;


int main(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	struct ControllerStruct *cs;
	cs = malloc(sizeof(struct ControllerStruct));

	pthread_t controller_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&controller_id,&attr,controller_thread, (void *) cs);

	struct gameState gs = initGameState();
	drawBackground(&bg);
	while(1){
		drawGameState(&gs);
	}
	
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

void drawGameState(struct gameState *gs)
{
	drawScore();
	drawTime();
	drawLives();
	drawSteps();
	drawMap(gs -> map);
}

void drawMap(struct gameMap gm){
	// Loop through each tile and draw any value packs

	int xOff,yOff;
	for(int i = 0;i<gm.rows;i++){
		for(int j = 0;j<gm.cols;j++){
			xOff = tileToPixel(SCREEN_X, gm.cols, j);
			yOff = tileToPixel(SCREEN_Y, gm.rows, i);

			if(gm.table[i][j].valuePack != 0){
				//draw image
			}
		}
	}

	// Draw frog
	xOff = tileToPixel(SCREEN_X, gm.cols, gm.frogX);
	yOff = tileToPixel(SCREEN_Y, gm.rows, gm.frogY);
	draw((int *)frogImage.image_pixels,frogImage.width,frogImage.height,xOff,yOff, gm.orientation);

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

int tileToPixel(int totalPixelLength, int totalTileLength, int currVal)
{
	return (totalPixelLength/totalTileLength)*currVal;
}

void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation){

		Pixel *pixel = malloc(sizeof(Pixel));

		int i = 0;
		for(int y = 0; y<height;y++){
			for(int x = 0;x<width;x++){
				pixel -> color = pixels[i];
				pixel -> x = xOff + x;
				pixel -> y = yOff + y;	
				drawPixel(pixel);
				i++;
			}
		}
		
		free(pixel);
}

void clearObj(const struct imageStruct *img, int xOff, int yOff){
	Pixel *pixel = malloc(sizeof(Pixel));

	int width = img -> width;
	int height = img -> height;

	for(int y = 0; y<height;y++){
		for(int x = 0;x<width;x++){
		// replace image with previous item
		pixel -> color = 0x0;
		pixel -> x = xOff + x;
		pixel -> y = yOff + y;

		drawPixel(pixel);
		}
	}
	free(pixel);
}

void drawBackground(struct Background *bg){
	const struct imageStruct *currBackground = bg -> backgrounds[bg -> currentB];
	draw((int *)currBackground -> image_pixels, currBackground -> width, currBackground -> height,0,0,0);
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
