typedef struct {
	int color;
	int x, y;
} Pixel;

typedef struct {
	int x,y;
	int speed;
	char* image;
} Player;


void drawPixel(Pixel *pixel); 
void drawPlayer(Player *player)




void drawPlayer(Player *player){
	
}

/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location = (pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}
