#include "Resources/image.h"

#ifndef LOGIC_H_INCLUDED
#define LOGIC_H_INCLUDED

// Function definitions
struct gameState initGameState();
int isGameEnd(struct gameState *gs);
void setCurrToPrev(struct gameState *prev, struct gameState *curr);
void updateGameState(struct gameState *gs, int button);
int updatePauseScreen(struct gameState *gs, int button, int pauseState);
int updateMenuScreen(struct gameState *gs, int button, int menuState);
int updateEndScreen(struct gameState *gs, int button);

// Defining constants
const int MENU_STATE;
const int PAUSE_STATE;
const int GAME_STATE;
const int END_STATE;


// Struct definition for each tile
struct Tile
{
	int valuePack;
	int canStand;
};


// Struct definition for a harm object
struct harmObject
{
	int drawX,drawY;	// X and Y drawing positions
	int speed;			// Speed of the object
	int orientation;	// Orientation of the object
	const struct imageStruct *img;	// Image of the harm object
};

// Struct definition for the game map
struct gameMap
{
	int rows,cols;		// Number of rows and columns in the game map
	int numbOfHarm;		// Number of harm objets in the game map
	int lastItemSpawn;	// The time when the last time spawned
	int timeEnter;		// Time when the frog has entered the stage
	struct harmObject *hObjs;	// The array of harm objects
	struct Tile **table;		// The 2D array of tables
};

// The struct definition for a game state
struct gameState
{
	struct gameMap map[4];		// A game map for each of the stages
	int orientation;			// Orientation of the frog
	int frogX,frogY;			// The X and Y coordinates of the frog
	int score;					// The score in the game
	int numbLives;				// The number of lives left	
	int time;					// The time left
	int movesLeft;				// The number of moves left
	int quit;					// The quit flag
	int hasWon;					// The win condition flag
	int hasLost;				// The lost condition flag
	int gameStage;				// The number of the current gamestage
	int state;					// The state of the game (playing, menu,pause)
	int startTime;				// The time the player started the game
};

#endif
