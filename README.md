This repository contains a frogger game.

To compile the game, type make in the correct directory where the files are located.

Then to run the game, type ./myProg in the correct directory and the game will start.

Also, to see the game, make sure to press ctrl + alt + f1 in the terminal to open the correct terminal to be able to see the game.

To make the river stage easier, the controller only takes input from single presses. So holding down a button will only move you one space in the 
desired direction. To move multiple spaces, you need to hit the button multiple times.

The value packs are as follows: Coin -> Adds 10 to score
                                Boots -> Adds 10 steps to moves left
                                Clock -> Adds 10 seconds to time
                                Heart -> Adds 1 life to lives

The values packs are spawned in after 30 seconds into each challenge (for example if you reach the second stage when there is 270 seconds
left, the value pack will only spawn when there are 240 seconds left in the current state). Moreover, the value packs will continue to spawn
in each 8 second interval after the initial spawn.

The code is broken up into three different file(s), we have the main file which contains the main loop and creates the controller thread.
The main file just initializes all the different objects and then runs the game loop until the player exits the game.

We also have the gamelogic file(s) (.h and .c), which handles all of the game logic such as updating the game state based on a button press, initializing
each of the maps, checking for collision with the frog, and various other miscellaneous functions needed for the game logic.

The final file we use is the gfx file(s) (.h and .c) which handle drawing the various objects to the screen. We have functions that draw
the current gamestate, we also have functions that draw the menu screen and pause screen, and various other screens and selectors.