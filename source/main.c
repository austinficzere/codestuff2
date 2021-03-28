#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "Resources/frog.c"
#include <unistd.h>
#include "controller.c"

typedef struct {
	int color;
	int x, y;
} Pixel;

typedef struct {
	int x,y;
	int speed;
} Player;

typedef struct {
	int controllerButton;
} Memory;

struct fbs framebufferstruct;

void drawPixel(Pixel *pixel); 
void drawPlayer(Player *player);
void clearPlayer(Player *player);

int main(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	Player *player = malloc(sizeof(Player));
	player -> x = 5;
	player -> y = 5;
	player -> speed = 10;

	Memory *mem = malloc(sizeof(Memory));
	mem -> controllerButton = 0b1111111111111111;

	drawPlayer(player);
	clearPlayer(player);

	while(1){
		drawPlayer(player);
		sleep(1);
		clearPlayer(player);

		controller(mem);
		
		if ((mem -> controllerButton) == 0b1111111111101111)
		{
			player -> y = (player -> y) - (player-> speed);
		}
		else if ((mem -> controllerButton) == 0b1111111111011111)
		{
			player -> y = (player -> y) + (player-> speed);
		}
		else if ((mem -> controllerButton) == 0b1111111110111111)
		{
			player -> x = (player -> x) - (player-> speed);
		}
		else if ((mem -> controllerButton) == 0b1111111101111111)
		{
			player -> x = (player -> x) + (player-> speed);
		}
	}
	
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}


void drawPlayer(Player *player){
		
		Pixel *pixel = malloc(sizeof(Pixel));
		int width = frogImage.width;
		int height = frogImage.height;

		int *frogPtr = (int *) frogImage.image_pixels;	
		int i = 0;

		for(int y = 0;y<height;y++){
			for(int x = 0;x<width;x++){
				pixel -> color = frogPtr[i];
				pixel -> x = (player -> x) + x;
				pixel -> y = (player -> y) + y;	
				
				drawPixel(pixel);
				i++;
			}
		}
		
		free(pixel);
}

void clearPlayer(Player *player){\
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
}

/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location = (pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}
