#include "Resources/image.h"

#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED
struct gameState initGameState();
int isGameEnd(struct gameState *gs);
void setCurrToPrev(struct gameState *prev, struct gameState *curr);
void updateGameState(struct gameState *gs, int button, int startTime);
int updatePauseScreen(struct gameState *gs, int button, int pauseState);
int updateMenuScreen(struct gameState *gs, int button, int menuState);

struct Tile
{
	int valuePack;
	int canStand;
};

struct harmObject
{
	int drawX,drawY;
	int speed;
	int orientation;
	const struct imageStruct *img;
};

struct gameMap
{
	int rows,cols;
	int numbOfHarm;
	int lastItemSpawn;
	struct harmObject *hObjs;
	struct Tile **table;	
};

struct gameState
{
	struct gameMap map[4];
	int orientation;
	int frogX,frogY;
	int score;
	int numbLives;
	int time;
	int movesLeft;
	int quit;
	int hasWon;
	int hasLost;
	int gameStage;
	int state;
};

#endif
