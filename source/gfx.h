#include "Resources/image.h"

#ifndef GFX_H_INCLUDED
#define GFX_H_INCLUDED

const struct imageStruct busImage; 
void drawGameState(struct gameState *prevState,struct gameState *gs);
void initGFX();

#endif