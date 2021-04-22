/* 
 * INFORMATION FOR TA
 * Class: CPSC 359
 * Names: Austin Ficzere(30093971) and Manbir Sandhu(30086574)
 * Description: The main file of our game, it sets up our structures and then updates the game
 * to keep in each state while the player hasnt quit the game.
*/

#include "controller.h"
#include <pthread.h>
#include "global.h"
#include "gamelogic.h"
#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// constants
const int MENU_STATE = 0;
const int PAUSE_STATE = 1;
const int GAME_STATE = 2;
const int END_STATE = 3;

/*
@Params: none
@Returns: 1 when the game is finished
Initialize our structs and enter the game loop to keep updating things while the player has not quit.
*/
int main(){
    int sTime = time(0);
    srand(sTime);
	struct ControllerStruct *cs;
	cs = malloc(sizeof(struct ControllerStruct));
    cs -> isGameOn = 1;

	pthread_t controller_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&controller_id,&attr,controller_thread, (void *) cs);

    struct gameState gs = initGameState();
    struct gameState prevState = initGameState();
    prevState.state = -1;

    initGFX();

    int currMenuState = 0;
    int currPauseState = 0;
    
    // while the player hasn't quit
    while(!gs.quit){
        wait(33333);
        // if in menustate we draw the screen and update the screen
        if(gs.state == MENU_STATE)
        {
            drawMenuScreen(&prevState,&gs,currMenuState);
	        prevState.state = gs.state;
            currMenuState = updateMenuScreen(&gs, cs -> controllerButton, currMenuState);
        }
        // if in pausestate we draw the screen and update the screen
        else if (gs.state == PAUSE_STATE)
        {
            drawPauseScreen(&prevState,&gs,currPauseState);
	        prevState.state = gs.state;
            currPauseState = updatePauseScreen(&gs, cs -> controllerButton, currPauseState);
            // if in endstate we draw the screen and update the screen
        }else if (gs.state == END_STATE){
            drawWinLoseScreen(&prevState,&gs);
            prevState.state = gs.state;
            updateEndScreen(&gs, cs -> controllerButton);
        }
        // if in gamestate we draw the screen and update the screen
        else {
            drawGameState(&prevState,&gs);
            setCurrToPrev(&prevState, &gs);
       	    updateGameState(&gs, cs -> controllerButton);
        }
        cs -> controllerButton = NONE_PRESSED;
    }

    return 1;
}
