#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "Resources/frog.c"
#include <unistd.h>
#include "controller.h"
#include <pthread.h>
#include "global.h"

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
	int currentB
}

struct fbs framebufferstruct;

void drawPixel(Pixel *pixel); 
void drawPlayer(Player *player);
void clearPlayer(Player *player);

int main(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	Player *player = malloc(sizeof(Player));
	player -> x = 30;
	player -> y = 30;
	player -> speed = 10;

	struct ControllerStruct *cs;
	cs = malloc(sizeof(struct ControllerStruct));

	pthread_t controller_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&controller_id,&attr,controller_thread, (void *) cs);


	drawPlayer(player);
	clearPlayer(player);

	while(1){
		drawPlayer(player);
		wait(70000);
		clearPlayer(player);	
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


void init()
{

}


void drawPlayer(Player *player){
		
		Pixel *pixel = malloc(sizeof(Pixel));
		int width = frogImage.width;
		int height = frogImage.height;

		int *frogPtr = (int *) frogImage.image_pixels;	
		int i = 0;

		for(int y = 0;y<height;y++){
			for(int x = 0;x<width;x++){
				if(frogPtr[i]!=0){
					pixel -> color = frogPtr[i];
					pixel -> x = (player -> x) + x;
					pixel -> y = (player -> y) + y;	
				
					drawPixel(pixel);
				}
				i++;
			}
		}
		
		free(pixel);
}

void clearPlayer(Player *player){
	Pixel *pixel = malloc(sizeof(Pixel));

	int width = frogImage.width;
	int height = frogImage.height;

	for(int y = 0; y<height;y++){
		for(int x = 0;x<width;x++){
		pixel -> color = 0x0;
		pixel -> x = (player -> x) + x;
		pixel -> y = (player -> y) + y;

		drawPixel(pixel);
		}
	}
	free(pixel);
}

void drawBackground(struct *Background){
	Pixel *pixel = malloc(sizeof(Pixel));

	int width = 
	int height = 

	for(int y = 0;y<height;y++){
		for(int x = 0;x<width;x++){
		pixel 
		pixel -> x = x;
		pixel -> y = y;

		drawPixel(pixel);
		}
	}

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
