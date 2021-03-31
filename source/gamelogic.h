#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED
struct gameState initGameState();
int isGameEnd(struct gameState *gs);

struct Tile
{
	int valuePack;
	int isFrogOn;
};

struct gameMap
{
	struct Tile **table;	
};

struct gameState
{
	struct gameMap map;
	int score;
	int numbLives;
	int time;
	int movesLeft;
	int quit;
	int hasWon;
	int hasLost;
	int gameStage;
};

#endif
