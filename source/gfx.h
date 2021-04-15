#include "Resources/image.h"
#include "gamelogic.h"

#ifndef GFX_H_INCLUDED
#define GFX_H_INCLUDED

const struct imageStruct frogImage32; 
const struct imageStruct startMenuImage;
const struct imageStruct pauseMenuImage;

struct harmObjectImg {
	int length;
	const struct imageStruct *imgs[]
};

struct harmObjectImg hObjImg;

void drawGameState(struct gameState *prevState, struct gameState *gs);
void initGFX();
void drawMenuScreen();
void drawPauseScreen();
void drawWinLoseScreen(struct gameState *prevState,struct gameState *gs);
int tileToPixel(int totalPixelLength, int totalTileLength, int currVal);

const int SCREEN_X;
const int SCREEN_Y;

#endif