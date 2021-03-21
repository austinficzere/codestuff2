/* 
 * INFORMATION FOR TA
 * Class: CPSC 359
 * Names: Austin Ficzere(30093971) and Manbir Sandhu(30086574)
 * Description: This program determines which buttons on a SNES controller are being pressed. We print which button
 * is being pressed every time a button is pressed on the controller. When the user presses the START button on the 
 * controller, we exit the program.
*/

#include <unistd.h>
#include <wiringPi.h>
#include <stdio.h>
#include "initGPIO.h"

#define GPIO_SET_OFF 7          // 28/4
#define GPIO_CLEAR_OFF 10       // 40/4
#define GPIO_LEV_OFF 13         // 52/4
#define CLK 11                  // Pin number for clock line
#define LAT 9                   // Pin number for latch line
#define DAT 10                  // Pin Number for data line

// predefining functions to use in main
void Init_GPIO (int pinNumb, int func, unsigned int *gpio);
void Write_Latch(int isWriting, unsigned int *gpio);
void Write_Clock(int isWriting, unsigned int *gpio);
int Read_Data(unsigned int *gpio);
int readPinLevel(int pinNumb, unsigned int *gpio);
void writeToPin(int pinNumb, int isWriting, unsigned int *gpio);
void wait(int time);
int Read_SNES(unsigned int *gpio);
void printButton(int button);
int isButtonPressed(int button, int i);

// array of strings to represent the buttons on the SNES controller
const char * buttonsArr[] = { "B","Y", "Select", "Start", "Joy-pad UP", "Joy-pad DOWN", "Joy-pad LEFT", "Joy-pad RIGHT", "A", "X", "Left", "Right"};

// defining constants to know when buttons are pressed or none are pressed
const int NONE_PRESSED = 0b1111111111111111;
const int BTN_PRESSED = 0;

// Defining constant for the start button 
const int START_BUTTON = 3;

/*
@Params: none
@Returns: none
This is the main function of our program. It sets up our gpio, and then prints the authors of the program. Then,
we initialize our clock, latch, and data lines. Now, we check in a loop condition if START has been pressed on the 
controller. If it has then we exit the loop and otherwise we enter the loop. We then read the info from our SNES 
controller to determine if a button has been pressed. If a button was pressed, we print the correct button.
*/
int main()
{
        unsigned int *gpio = getGPIOPtr();

        printf("Created by Manbir Sandhu and Austin Ficzere\n\n");

        // initializing clock, latch, and data lines
        Init_GPIO(CLK, 1, gpio);
        Init_GPIO(LAT, 1, gpio);
        Init_GPIO(DAT, 0, gpio);

        // Initialzing the buttons pressed and the previously pressed button
        int button = NONE_PRESSED;
        int prevPressed = NONE_PRESSED;
	
        // Printing info for the user
        printf("Please press a button…\n\n");

        // looping while START is not pressed
        while(!isButtonPressed(button,START_BUTTON)){
                prevPressed = button;     // Updating the value of the previously pressed button
	        button = Read_SNES(gpio); // read info from SNES

                // Printing button, if the buttons pressed are different from the previous iteration
	        if(button!=prevPressed && button!=NONE_PRESSED){
                        printButton(button);
                }
                wait(750);
        }
        printf("Program is terminating…\n\n");
        return 0;
}

/*
@Params: 
        gpio: a pointer to the GPIO base address
@Returns: an int, button, that is a binary number with length 16, which tells us which buttons on the controller are
          pressed.
This function reads info from the SNES to determine which button has been pressed. We follow the steps from the slides
to get the correct binary number that represents the controller buttons, with 1 being not-pressed and 0 being pressed.
*/
int Read_SNES(unsigned int *gpio){

        // Initializing the button variable which will store the state of the buttons
        int button = 0;     

        // Signaling SNES to sample the state of the controller
        Write_Clock(1, gpio);   
        Write_Latch(1, gpio);
        wait(12);
        Write_Latch(0, gpio);

        // Pulse loop to read the state of each of the buttons (12 loops for the buttons and 4 unused loops)
        for(int i = 0;i<16;i++)
        {
                // Waiting 6 microseconds and writing 0 to the CLOCK line to synchronize pulse loop
                wait(6);
                Write_Clock(0,gpio);
                wait(6);

                // Reading the pin level of the DATA line and storing the result in the button variable 
                int b = Read_Data(gpio);
                button |= (b<<i);

                // Writing to clock again to synchronize the pulse loop
                Write_Clock(1,gpio);
        }

        // Returing the read buttons
	return button;
}

/*
@Params: 
        button: the binary number with length 16, which tells us which buttons on the controller are pressed.
        i: an int to tell us how many times to shift left to check a certain button.
@Returns: We return either 0 (false) or 1 (true), based on whether the button we are checking is pressed or not.
This function determines if a certain button on our controller is pressed.
*/
int isButtonPressed(int button, int i){
        return (button & (1<<i)) == BTN_PRESSED;
}

/*
@Params: 
        button: the int which tells us which buttons on the controller are pressed.
@Returns: none
This function prints which button is pressed on the controller. We loop through button binary number bit by bit 
and determine if the button is pressed. If it is, we use our buttonsArr to print the correct button.
*/
void printButton(int button)
{
        printf("You have pressed ");
        for(int i = 0;i<12;i++){ // looping through each bit
                if(isButtonPressed(button,i)){ // if the ith button is pressed, this statement is true
		        printf("%s, ", buttonsArr[i]); // printing the ith element of our buttonsArr
                }
        }
        printf("\n\n");
}

/*
@Params: 
        time: an int which is the time we want to delay in microseconds.
@Returns: none
This function uses delayMicroseconds to delay by a certain amount of time as specified by the user.
*/
void wait(int time)
{
        delayMicroseconds(time);
}

/*
@Params: 
        isWriting: An int which is used to specify what to set the latch pin too (low/high)
        (0 for low and 1 for high)
        gpio: a pointer to the GPIO base address
@Returns: none
This function is used to either write a 0 or 1 to the Latch pin (pin number 9)
*/
void Write_Latch(int isWriting, unsigned int *gpio){
        writeToPin(LAT,isWriting,gpio);
}

/*
@Params: 
        isWriting: An int which is used to specify what to set the latch pin too (low/high)
        (0 for low and 1 for high)
        gpio: a pointer to the GPIO base address
@Returns: none
This function is used to either write a 0 or 1 to the Clock pin (pin number 11)
*/
void Write_Clock(int isWriting, unsigned int *gpio)
{
        writeToPin(CLK,isWriting,gpio);
}

/*
@Params: 
        gpio: a pointer to the GPIO base address
@Returns: Int - 1 if the pin voltage is high, and 0 if the pin voltage is low
This function reads the value in the Data pin (pin 10)
*/
int Read_Data(unsigned int *gpio){
        return readPinLevel(DAT, gpio);
}

/*
@Params: 
        pinNumb: The pin number that is going to be initialized 
        func: The number of the function (0 for input, 1 for output, 2, ...., 7)
        gpio: a pointer to the GPIO base address
@Returns: none
This function initializes the function of a GPIO pin which is specified as input
*/
void Init_GPIO (int pinNumb, int func, unsigned int *gpio){
        gpio[pinNumb/10] &= ~(7<<((pinNumb%10)*3));
        gpio[pinNumb/10] |= (func << ((pinNumb%10)*3));
}

/*
@Params: 
        pinNumb: The pin number which we want to read the value of
        gpio: a pointer to the GPIO base address
@Returns: 
This function reads the value of a GPIO pin, which is specified in the input
*/
int readPinLevel(int pinNumb, unsigned int *gpio)
{
        return (gpio[GPIO_LEV_OFF] >> pinNumb) & 1;
}

/*
@Params: 
        pinNumb: the pin number we want to write to
        isWriting: what we want to write, either 0 or 1
        gpio: a pointer to the GPIO base address
@Returns: none
This function either writes a 0 or 1 to a pin number specified by the input.
*/
void writeToPin(int pinNumb, int isWriting, unsigned int *gpio){
        if(isWriting)
                gpio[(GPIO_SET_OFF)+(pinNumb/32)] |= 1<<pinNumb;
        else
                gpio[(GPIO_CLEAR_OFF)+(pinNumb/32)] |= 1<<pinNumb;
}