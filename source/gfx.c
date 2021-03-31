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

typedef struct {
	int x,y;
	int speed;
	int orientation;
} Player;

struct Background {
	// arrray of background
	const struct imageStruct *backgrounds[4];
	int currentB;
} bg = {{&cityImage, NULL, NULL, NULL}, 0};

struct fbs framebufferstruct;

void drawPixel(Pixel *pixel); 
void draw(int *pixels, int width, int height, int xOff, int yOff, int orientation);
void clearObj(const struct imageStruct *img, int xOff, int yOff);
void drawBackground(struct Background *bg);
void init(Player *player);
unsigned short int getPixel(int x, int y);

const int PLAYER_SPD = 10;


int main(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	Player *player = malloc(sizeof(Player));
	
	init(player);

	struct ControllerStruct *cs;
	cs = malloc(sizeof(struct ControllerStruct));

	pthread_t controller_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&controller_id,&attr,controller_thread, (void *) cs);

	
	struct gameState gs = initGameState();
	while(1){

		draw((int *) frogImage.image_pixels, frogImage.width,frogImage.height, player->x, player->y, player -> orientation);
		wait(70000);
		clearObj(&frogImage,player ->x, player->y);
		if ((cs -> controllerButton) == 0b1111111111101111)
		{
			player -> y = (player -> y) - (player-> speed);
		}
		else if ((cs -> controllerButton) == 0b1111111111011111)
		{
			player -> y = (player -> y) + (player-> speed);
		}
		else if ((cs -> controllerButton) == 0b1111111110111111)
		{
			player -> x = (player -> x) - (player-> speed);
		}
		else if ((cs -> controllerButton) == 0b1111111101111111)
		{
			player -> x = (player -> x) + (player-> speed);
		}
	}
	
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}


void init(Player *player)
{
	player -> x = 30;
	player -> y = 30;
	player -> speed = PLAYER_SPD;
	player -> orientation = 0;
	
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
