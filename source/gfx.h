#include "Resources/image.h"
#include "gamelogic.h"

#ifndef GFX_H_INCLUDED
#define GFX_H_INCLUDED

// define structs
const struct imageStruct frogImage32; 
const struct imageStruct startMenuImage;
const struct imageStruct pauseMenuImage;

struct harmObjectImg {
	int length;
	const struct imageStruct *imgs[]
};

struct harmObjectImg hObjImg;

// define functions
void drawGameState(struct gameState *prevState, struct gameState *gs);
void initGFX();
void drawMenuScreen(struct gameState *prevState,struct gameState *gs, int menuState);
void drawPauseScreen(struct gameState *prevState,struct gameState *gs, int pauseState);
void drawWinLoseScreen(struct gameState *prevState,struct gameState *gs);
int tileToPixel(int totalPixelLength, int totalTileLength, int currVal);

// define constants
const int SCREEN_X;
const int SCREEN_Y;

#endif