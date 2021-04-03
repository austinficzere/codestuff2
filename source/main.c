#include "controller.h"
#include <pthread.h>
#include "global.h"
#include "gamelogic.h"
#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    int sTime = time(0);
    srand(sTime);
	struct ControllerStruct *cs;
	cs = malloc(sizeof(struct ControllerStruct));

	pthread_t controller_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&controller_id,&attr,controller_thread, (void *) cs);

    struct gameState gs = initGameState();
    struct gameState prevState = initGameState();
    prevState.map.frogX = 0;

    initGFX();

    while(!gs.quit){
        drawGameState(&prevState,&gs);
        wait(16667);
        // update gamestate
        setCurrToPrev(&prevState, &gs);
       	updateGameState(&gs, cs -> controllerButton, sTime);
        
        while (gs.gameStage == -1)
        {
            // drawPauseScreen();
            // update gamestate
        }

        // Change prev and current
    }

    return 0;
}
