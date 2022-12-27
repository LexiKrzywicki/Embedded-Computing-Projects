/************** ECE2049 Lab 2 Prelab ******************/

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

int main(void)
{
    unsigned char currKey = 0;

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer. Always need to stop this!!
                              // You can then configure it properly, if desired

    // Useful code starts here
    initLeds();
    configDisplay();
    configKeypad();
    configReset();

    configUserLED('1');
    swDelay(1);
    configUserLED('2');
    swDelay(1);
    configUserLED('4');
    swDelay(1);
    configUserLED('8');
    swDelay(1);
    configUserLED(0);

    while (1)
    {
        getButton();

    }
}

void swDelay(char numLoops) // blocking
{
    volatile unsigned int i, j; // volatile to prevent removal in optimization
                                // by compiler. Functionally this is useless code

    for (j = 0; j < numLoops; j++)
    {
        i = 50000;    // SW Delay
        while (i > 0) // could also have used while (i)
            i--;
    }
}

void handleLeds(char seq)
{

    switch (seq)
    {
    case '1':
        setLeds(8);
        break;
    case '2':
        setLeds(4);
        break;
    case '3':
        setLeds(2);
        break;
    case '4':
        setLeds(1);
        break;
    }
}