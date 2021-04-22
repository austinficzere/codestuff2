/* 
 * INFORMATION FOR TA
 * Class: CPSC 359
 * Names: Austin Ficzere(30093971) and Manbir Sandhu(30086574)
 * Description: The game logic of our game, updates the game screens, has logic for colliding objects, and 
 * initializes our structures.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamelogic.h"
#include <time.h>
#include "controller.h"
#include "global.h"
#include "gfx.h"

// constants
const int NUMB_LIVES = 5;
const int START_TIME = 300;
const int NUMB_MOVES = 300;
const int MAP_ROWS = 20;
const int MAP_COLS = 30;
const int H_OBJ = 15;
const int NUMB_OF_STAGES = 4;
const int ITEM_SPAWN_DIFF = 3;
const int LIFE_VALUE = 1;
const int TIME_VALUE = 4;
const int STEP_VALUE = 3;
const int SCORE_VALUE = 2;

// defining functions/structures
struct gameState initGameState();
int isGameEnd(struct gameState *gs);
void spawnValuePacks(int sTime, struct gameState *gs);
void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr);
void updateHarmObjects(struct gameMap gm);
int collides(int left1, int right1, const struct imageStruct *img1, int left2, int right2, const struct imageStruct *img2);
int frogCollideHarm(struct gameMap gm, int frogX, int frogY);
int pixelToTile(int totalPixelLength, int totalTileLength, int currVal);
int updateFrogWater(struct gameMap gm, int frogX, int frogY);

/*
@Params: 
        rows: number of rows
		cols: number of columns
@Returns: a pointer of pointers of Tile structures, which is essentially a 2D array
We make space for our 2D array of Tile structs and then return the 2D array.
*/
struct Tile **createTable(int rows, int cols)
{
	// Allocating memory for each row of the table
	struct Tile **table = (struct Tile **) malloc(rows*sizeof(struct Tile *));
	for(int i = 0;i<rows;i++)
	{
		// Allocating memory for each column of the table
		table[i] = (struct Tile *) malloc(cols * sizeof(struct Tile));
	}
	return table;
}

/*
@Params: 
        tc: a Tile struct we want to copy
@Returns: a new Tile struct with the same values as the Tile struct that was passed in
We copy a Tile struct and return the new copied Tile.
*/
struct Tile copyTile(struct Tile tc){
	// Creating a new instnace of the tile
	struct Tile ret;

	// Copying old values to the new tile
	ret.valuePack = tc.valuePack;
	ret.canStand = tc.canStand;

	return ret;
}

/*
@Params: totalPixelLength: The total length of the screen X or screen Y
		 totalTileLength: The number of tiles in the orientation
		 currVal: The current pixel value

@Returns: We convert the pixel position from either the rows or columns into the 
row/column position

*/
int pixelToTile(int totalPixelLength, int totalTileLength, int currVal){
	return ((totalTileLength * currVal)/totalPixelLength);
}

/*
@Params: 
        gs: pointer to the gamestate
@Returns: 1 if the game is over and 0 otherwise
Checks if the game is over, so either the lives/time/movesLeft are 0, or we have won/lost/quit.
*/
int isGameEnd(struct gameState *gs)
{
	if(gs -> hasWon)
		return 1;
	// Checking each of the losing conditions 
	else if(gs->numbLives == 0 || gs->time == 0 || gs -> movesLeft == 0 || gs -> hasLost || gs -> quit){
		gs -> hasLost = 1;
		return 1;	
	}
	return 0;
}


int updateEndScreen(struct gameState *gs, int button){
	if(button != NONE_PRESSED){
		*gs = initGameState();
		return 1;
	}
	return 0;
}

/*
@Params: 
        gs: pointer to the gamestate
		button: the button binary num that tells us which buttons are pressed
		menuState: an int that tells us which button on the screen is selected
@Returns: the new menuState if it was changed or the old one if it was not changed
Updates the menu screen and checks if we have switched the button selected or if the user has pressed one of the
buttons.
*/
int updateMenuScreen(struct gameState *gs, int button, int menuState)
{
	// if we are selecting play and they press A
	if (menuState == 0 && isButtonPressed(button,A_BUTTON))
	{

		// Chaning the gamestate to the actual game
		gs -> state = 2;
		gs -> startTime = time(0);
	}
	// if we are selecting quit and they press A
	else if (menuState == 1 && isButtonPressed(button,A_BUTTON))
	{
		gs -> quit = 1;
	}
	// if they press up we select the top button
	else if (isButtonPressed(button,UP_BUTTON))
	{
		menuState = 0;
	}
	// if they press down we select the bottom button
	else if (isButtonPressed(button,DOWN_BUTTON))
	{
		menuState = 1;
	}
	return menuState;
}

/*
@Params: 
        gs: pointer to the gamestate
		button: the button binary num that tells us which buttons are pressed
		pauseState: an int that tells us which button on the screen is selected
@Returns: the new pauseState if it was changed or the old one if it was not changed
Updates the pause screen and checks if we have switched the button selected or if the user has pressed one of the
buttons.
*/
int updatePauseScreen(struct gameState *gs, int button, int pauseState)
{
	// if they press restart button, we reset the game
	if ((pauseState == 0 || pauseState == 1) && isButtonPressed(button,A_BUTTON))
	{
		*gs = initGameState();
		if(pauseState == 0){
			gs -> state = 2;
		}
	}
	// if they press up we select the top button
	else if (isButtonPressed(button,UP_BUTTON))
	{
		pauseState = 0;
	}
	// if they press down we select the bottom button
	else if (isButtonPressed(button,DOWN_BUTTON))
	{
		pauseState = 1;
	}
	// if they press start we resume the game
	else if (isButtonPressed(button,START_BUTTON))
	{
		gs -> state = 2;
	}
	return pauseState;
}

/*
@Params: 
        gs: pointer to the gamestate
		button: the button binary num that tells us which buttons are pressed
		startTime: the time the game started
@Returns: none
Updates the game frog when a button is pressed, checks for collision, checks if the game ended, spawns
value packs, updates the harm objects, and updates the game state values.
*/
void updateGameState(struct gameState *gs, int button)
{
	// check if game is over
	if (isGameEnd(gs)){
		gs -> state = 3;
	}

	if(gs -> map[gs -> gameStage].timeEnter == -1){
		gs -> map[gs -> gameStage].timeEnter = time(0);
	}

	// Checking if the frog collides with any objects in the first three stages
	if(frogCollideHarm(gs -> map[gs -> gameStage], gs -> frogX, gs -> frogY) && gs -> gameStage != 3){

		// Resetting the number of lives and resetting the frog to the beginning position
		gs -> numbLives--;
		gs -> frogX = MAP_COLS/2;
		gs -> frogY = MAP_ROWS-2;
	}

	// Checking if the frog is not colliding with any of the logs and is in the water
	if(!(frogCollideHarm(gs -> map[gs -> gameStage], gs -> frogX, gs -> frogY)) && gs -> gameStage == 3 && ((gs -> frogY>=2 && gs ->frogY<=7) || ( gs -> frogY >= 12 && gs -> frogY <= 17))){
		// Decreasing the number of lives and resetting the frog to the beginning position
		gs -> numbLives--;
		gs -> frogX = MAP_COLS/2;
		gs -> frogY = MAP_ROWS-2;
	}else if (gs -> gameStage == 3){
		// Updating the frogs X position 
		gs -> frogX = updateFrogWater(gs -> map[gs -> gameStage], gs -> frogX, gs -> frogY);
	}

	// Updating the positions of the harm objects
	updateHarmObjects(gs -> map[gs -> gameStage]);

	// if they press start the pause screen opens
	if (isButtonPressed(button,START_BUTTON)){
		gs -> state = 1;
	}

	// spawn value packs
	spawnValuePacks(gs -> map[gs -> gameStage].timeEnter, gs);

	// checking for d pad button presses
	if(button!=NONE_PRESSED){
		// If the button pressed is up we move the frog up and change the orientation of the frog
		if (isButtonPressed(button,UP_BUTTON) && gs -> frogY>0)
		{
			gs -> orientation = 0;
			gs -> frogY--;

			// Decrementing the number of moves left
			gs -> movesLeft--;
		}
		// If the button pressed is up we move the frog down and change the orientation of the frog
		if (isButtonPressed(button,DOWN_BUTTON) && gs -> frogY<MAP_ROWS-1)
		{
			gs -> orientation = 2;
			gs -> frogY++;

			// Decrementing the number of moves left
			gs -> movesLeft--;
		}

		// If the button pressed is left we move the frog down and change the orientation of the frog
		if (isButtonPressed(button,LEFT_BUTTON))
		{
			gs -> orientation = 3;

			// If the frog is not at the end of the screen we just move the frog left
			if(gs -> frogX>0)
				gs -> frogX--;
			// Otherwise we move the frog to the rightmost position
			else
				gs -> frogX = MAP_COLS -1;
			gs -> movesLeft--;
		}
		// If the button pressed is left we move the frog down and change the orientation of the frog
		if (isButtonPressed(button,RIGHT_BUTTON))
		{
			gs -> orientation = 1;

			// If the frog is not at the end of the screen we just move the frog right
			if(gs -> frogX<MAP_COLS)
				gs -> frogX++;
			else
				gs -> frogX = 0;
			gs -> movesLeft--;
		}
	}

	// We check if we are standing ontop of a valuepack
	if(gs -> map[gs -> gameStage].table[gs -> frogY][gs -> frogX].valuePack != 0){
		// Getting the value of the valuepack
		int valueStand  = gs -> map[gs -> gameStage].table[gs -> frogY][gs -> frogX].valuePack;

		// If the valuepack is the first one, we increment lives
		if (valueStand == LIFE_VALUE)
		{
			gs -> numbLives++;
		}

		// We increment score if it is the second
		else if (valueStand == SCORE_VALUE)
		{
			gs -> score+=10;
		}

		// We increment the time if it is the third
		else if (valueStand == TIME_VALUE)
		{
			gs -> startTime = (gs -> startTime) + 10;
		}

		// We increment the number of moves left if it is the fourth
		else if (valueStand == STEP_VALUE)
		{
			gs -> movesLeft = (gs -> movesLeft) + 10;
		}

		gs -> map[gs -> gameStage].table[gs -> frogY][gs -> frogX].valuePack = 0;
	}

	// if we reach the end of screen
	if (gs -> frogY == 0)
	{
		// check if we are in last stage, if we are they win
		if (gs -> gameStage == 3)
			{
				gs -> hasWon = 1;
				gs -> score = ((gs -> time) + (gs -> movesLeft) + (gs -> numbLives)) * 2;
			}
			// otherwise go to next stage
			else
			{
				gs -> gameStage++;
				gs -> frogY = MAP_ROWS - 2;
				gs -> score+=10;
			}
	// check if the go back and want to return to previous stage
	}else if(gs -> gameStage != 0 && gs -> frogY == MAP_ROWS -1){
		gs -> gameStage--;
		gs -> frogY = 1;
	}

	// update time
	gs -> time = START_TIME - (time(0) - gs -> startTime);

}

/*
@Params: 
        gm: the gameMap struct
		frogX: The x position of the frog on the 2D grid
		frogY: The y position of the frog on the 2D grid
@Returns: frogX: The updated frog x poisition
This function updates the frogs position when on the logs in the water
*/
int updateFrogWater(struct gameMap gm, int frogX, int frogY){

	// Finding the x position of the frog in pixels
	int xOff = tileToPixel(SCREEN_X, gm.cols, frogX);

	// Finding the y position of the frog in pixels
	int yOff = tileToPixel(SCREEN_Y, gm.rows, frogY);
	for(int i = 0;i<gm.numbOfHarm;i++){

		// Checking if the frog collides with the current object
		if(collides(xOff,yOff,&frogImage32,gm.hObjs[i].drawX,gm.hObjs[i].drawY,gm.hObjs[i].img)){

			if(gm.hObjs[i].speed>0&& pixelToTile(SCREEN_X,MAP_COLS,gm.hObjs[i].drawX) != pixelToTile(SCREEN_X,MAP_COLS,gm.hObjs[i].drawX+gm.hObjs[i].speed)){
				frogX++;
			}else if(gm.hObjs[i].speed<0 && pixelToTile(SCREEN_X,MAP_COLS,gm.hObjs[i].drawX) != pixelToTile(SCREEN_X,MAP_COLS,gm.hObjs[i].drawX+gm.hObjs[i].speed))
			{
				frogX--;
			}
		}
	}

	// Returing the updated frogX
	return frogX;
}

/*
@Params: 
        prev: a pointer to the previous game map
		curr: a pointer to the current game map
@Returns: none
We set the current game map to previous one.
*/
void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr){

	// Copying each tile in the map
	for(int i = 0;i<MAP_ROWS;i++){
		for(int j = 0;j<MAP_COLS;j++){
			prev -> table[i][j] = copyTile(curr -> table[i][j]);
		}
	}

	prev -> timeEnter = curr -> timeEnter;

	// Copying the values in the harms objects
	for(int i = 0;i<curr -> numbOfHarm;i++){
		prev -> hObjs[i].img = curr -> hObjs[i].img;
		prev -> hObjs[i].drawX = curr -> hObjs[i].drawX;
		prev -> hObjs[i].drawY = curr -> hObjs[i].drawY;
		prev -> hObjs[i].speed= curr -> hObjs[i].speed;
	}
}

/*
@Params: 
        prev: a pointer to the previous game state
		curr: a pointer to the current game state
@Returns: none
We set the current game state to previous.
*/
void setCurrToPrev(struct gameState *prev, struct gameState *curr){

	// Copying the maps
	for(int i = 0;i<NUMB_OF_STAGES;i++){
		setCurrToPrevMap(&(prev -> map[i]), &(curr -> map[i]));
	}

	// Copying all other instanve variables
	prev -> frogX = curr -> frogX;
	prev -> frogY = curr -> frogY;
	prev -> orientation = curr -> orientation;
	prev -> score = curr -> score;
	prev -> numbLives = curr -> numbLives;
	prev -> time = curr -> time;
	prev -> movesLeft = curr -> movesLeft;
	prev -> quit = curr -> quit;
	prev -> hasWon = curr -> hasWon;
	prev -> hasLost = curr -> hasLost;
	prev -> gameStage = curr -> gameStage;
	prev -> state = curr -> state;

}

/*
@Params: 
        lowerBound: the lower bound of the random num
		upperBound: th upper bound of the random num
@Returns: the random number within the range
Finds a random number in a given range.
*/
int randomNumb(int lowerBound, int upperBound){
	return (rand() % (upperBound-lowerBound+1)) +lowerBound;
}

/*
@Params: 
        hObjs: a pointer to the harmObject struct
		numbOfHarm: the number of harm objects we want
		gameStage: the current stage of the game
@Returns: none
Initializes the harm objects on the screen to random spots.
*/
void initHarmObjects(struct harmObject *hObjs, int numbOfHarm, int gameStage){
	int low;
	int high;

	// The indicies for the harm objects in the first stage
	if(gameStage == 0){
		low = 0;
		high = 1;
	}else if(gameStage == 1){
	// The indices for the harm objects in the second stage
		low = 2;
		high = 3;
	}else if(gameStage == 2){
	// The indicies for the harm objects in the third stage
		low = 4;
		high = 4;
	}else{
	// The indicies for the harm objects in the fourth stage
		low = 5;
		high = 6;
	}
	if(gameStage != 3){
		// Initializing harm objects for stages 1-3
		for(int i = 0;i<numbOfHarm;i++){
			// Setting the image of the objects to a random image
			hObjs[i].img = hObjImg.imgs[randomNumb(low,high)];

			// Setting the speed of the objects
			hObjs[i].speed = randomNumb(1,20);

			// Setting the a random Y position for the objects
			hObjs[i].drawY = randomNumb(100,SCREEN_Y-110);

			// Setting a random orientation for the object
			hObjs[i].orientation = (randomNumb(0,1) ? 0 : -1);
			if(hObjs[i].orientation == -1){
				hObjs[i].speed = -(hObjs[i].speed);
			}
			hObjs[i].drawX = randomNumb(100,SCREEN_X-100);
		}
	}else{
		int row = 17;
		int col = randomNumb(0,MAP_COLS);
		int off = 0;
		for(int i = 0;i<numbOfHarm;i++){

			// Initializing the image
			hObjs[i].img = hObjImg.imgs[randomNumb(low,low)];

			// Initiailizing the speed
			hObjs[i].speed = (i%2? 2 : 3);

			// Initiailizing the X position
			hObjs[i].drawX = tileToPixel(SCREEN_X,MAP_COLS,col);

			hObjs[i].orientation = (i%2? 0 : -1);
			if(hObjs[i].orientation == -1){
				hObjs[i].speed = -(hObjs[i].speed);
			}
			// Initilizing the y position
			hObjs[i].drawY = tileToPixel(SCREEN_Y,MAP_ROWS,row);

			// Changing the row
			row--;

			// Accounting for the middle area
			if(row == 11)
				row = 7;
			else if(row == 1){
				row = 17;
				off = 1;
			}

			// Updating the columns of the harm object
			if(!off){
				col = randomNumb(3,(MAP_COLS/2)-2);
			}
			else{
				col = randomNumb((MAP_COLS/2)+2,MAP_COLS-3);
			}
		}
	}
}

/*
@Params: 
        gameStage: the stage of the game
@Returns: the initial gameMap struct
Intiializing the gameMap.
*/
struct gameMap initGameMap(int gameStage)
{
	// Creating the map struct
	struct gameMap map;

	// Creating the table
	map.table = createTable(MAP_ROWS,MAP_COLS);

	// Allocating memory for the stages !=3
	if(gameStage != 3){
		map.hObjs = malloc(H_OBJ * sizeof(struct harmObject));
		initHarmObjects(map.hObjs,H_OBJ, gameStage);
		map.numbOfHarm = H_OBJ;
	} else{

		// Allocating memory for the final stage
		map.hObjs = malloc(19 * sizeof(struct harmObject));
		initHarmObjects(map.hObjs,19, gameStage);
		map.numbOfHarm = 19;
	}

	// Intiializing time for last item spawn
	map.lastItemSpawn = time(0);
	map.timeEnter = -1;

	map.rows = MAP_ROWS;
	map.cols = MAP_COLS;
	for(int i = 0;i<MAP_ROWS;i++){
		for(int j = 0;j<MAP_COLS;j++){
			map.table[i][j].valuePack = 0;
		}
	}

	return map;
}

/*
@Params: none
@Returns: the initial gameState struct
Intializing the gameState.
*/
struct gameState initGameState()
{
	// Creating the gamestate struct
	struct gameState gs;

	// Initializing each of the maps
	for(int i = 0;i<NUMB_OF_STAGES;i++){
		gs.map[i] = initGameMap(i);
	}

	// Initializing the other instance variables
	gs.frogX = MAP_COLS/2;
	gs.frogY = MAP_ROWS-2;
	gs.orientation = 0;
	gs.score = 0;
	gs.numbLives = NUMB_LIVES;
	gs.time = START_TIME;
	gs.movesLeft = NUMB_MOVES;
	gs.quit = 0;
	gs.hasWon = 0;
	gs.hasLost = 0;
	gs.gameStage = 0;
	gs.state = 0;
	gs.startTime = time(0);
	return gs;
}

/*
@Params: 
        gm: the gameMap struct
@Returns: none
Updates the posisitions of the harm objects on the screen.
*/
void updateHarmObjects(struct gameMap gm){
	for(int i = 0;i<gm.numbOfHarm;i++){

		// Updating the x position of the harm objects
		gm.hObjs[i].drawX = gm.hObjs[i].drawX + gm.hObjs[i].speed;
		int width = gm.hObjs[i].img -> width;

		// Resetting the objects position if is out of the screen
		if(gm.hObjs[i].drawX + width<0){
			gm.hObjs[i].drawX = SCREEN_X-(gm.hObjs[i].img -> width);

		}else if(gm.hObjs[i].drawX >= SCREEN_X){
			gm.hObjs[i].drawX = 0;
		}
	}
}

/*
@Params: 
        gm: the gameMap of the game
		frogX: the x position of the frog on the screen
		frogY: the y position of the frog on the screen
@Returns: 1 if the frog has collided with something and 0 otherwise
Checks if the frog has collided with a harm object.
*/
int frogCollideHarm(struct gameMap gm, int frogX, int frogY){
	int xOff = tileToPixel(SCREEN_X, gm.cols, frogX);
	int yOff = tileToPixel(SCREEN_Y, gm.rows, frogY);

	// Checking if the frog collides with any of the objects in the current map
	for(int i = 0;i<gm.numbOfHarm;i++){
		if(collides(xOff,yOff,&frogImage32,gm.hObjs[i].drawX,gm.hObjs[i].drawY,gm.hObjs[i].img)){
			return 1;
		}
	}
	return 0;
}

/*
@Params: 
        x1: x value of image 1
		y1: y value of image 1
		img1: pointer to image 1
		x2: x value of image 2
		y2: y value of image 2
		img2: pointer to image 2
@Returns: 1 if the image has collided with another image and 0 otherwise
Checks if two images have collided on the screen.
*/
int collides(int x1, int y1, const struct imageStruct *img1, int x2, int y2, const struct imageStruct *img2){
	if (x1 < x2  + img2 -> width &&
   x1 + img1 -> width > x2 &&
   y1 < y2 + img2 -> height &&
   y1 + img1 -> height > y2) {
	   return 1;
	}
	return 0;
}

/*
@Params: 
        sTime: the start time of the game
		gs: the gameState struct
@Returns: none
Spawns value packs in the game if we have waited 30 seconds to random spots on the screen.
*/
void spawnValuePacks(int sTime, struct gameState *gs)
{
	int cTime = time(0) - sTime;
	int spawnDiff = time(0) - gs -> map[gs -> gameStage].lastItemSpawn;
	
	if (cTime > 30 && spawnDiff > ITEM_SPAWN_DIFF)
	{
		int i = randomNumb(3, MAP_ROWS-3);
		int j = randomNumb(0, MAP_COLS-1);
		gs -> map[gs -> gameStage].table[i][j].valuePack = randomNumb(1, 4);
		gs -> map[gs -> gameStage].lastItemSpawn = time(0);
	}
}
