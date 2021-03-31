#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED
struct gameState initGameState();
int isGameEnd(struct gameState *gs);
void setCurrToPrev(struct gameState *prev, struct gameState *curr);

struct Tile
{
	int valuePack;
	int canStand;
};

struct gameMap
{
	int rows,cols;
	int orientation;
	int frogX,frogY;
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
