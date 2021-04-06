#include "Resources/image.h"

#ifndef GFX_H_INCLUDED
#define GFX_H_INCLUDED

const struct imageStruct busImage; 
const struct imageStruct frogImage32; 
void drawGameState(struct gameState *prevState,struct gameState *gs);
void initGFX();
void drawMenuScreen();
void drawPauseScreen();
int tileToPixel(int totalPixelLength, int totalTileLength, int currVal);

const int SCREEN_X;
const int SCREEN_Y;

#endif