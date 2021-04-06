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

    int where = 0;
    int where2 = 0;

    while(!gs.quit){
        if(gs.state == 0)
        {
            drawMenuScreen();
            where = updateMenuScreen(&gs, cs -> controllerButton, where);
            if (where == 2)
            {
                gs.state = 2;
            }
            if (where == 3)
            {
                gs.quit = 1;
            }
        }
        else if (gs.state == 1)
        {
            drawPauseScreen();
            where2 = updateMenuScreen(&gs, cs -> controllerButton, where2);
            if (where2 == 2)
            {
                gs = initGameState();
            }
            else if (where2 == 3)
            {
                gs.quit = 1;
            }
            else if (where2 == -1)
            {
                gs.state = 2;
            }
        }
        else 
        {
            drawGameState(&prevState,&gs);
        }
        wait(33333);
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
