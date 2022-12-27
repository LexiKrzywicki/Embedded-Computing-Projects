/************** ECE2049 DEMO CODE ******************/
/**************  13 March 2019   ******************/
/***************************************************/

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

// Function Prototypes
void swDelay(char numLoops);
void swDelay2(double delayTime);
void win();
void lose();
void error();
// void handleLeds(char seq);
void game(int);
double delay = 50.0;
// double delayTime = 5

int level;
char flashArray[33];
bool flag;
bool isPressed;
int winLevel = 5; // set win level

// Declare globals here
enum STATE
{
    WELCOME,
    COUNT_DOWN, // counts 3 2 1
    SEQUENCE,
    CHECK_INPUT,
};

// int level;
enum STATE state = WELCOME;

// Main
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

    // generates random order to flash the LED
    int flashNum;

    state = WELCOME;
    level = 1;

    while (1) // Forever loop
    {

        // Check if any keys have been pressed on the 3x4 keypad
        currKey = getKey();
        int ind1;
        int ind2;
        setLeds(0);

        switch (state)
        {
        case WELCOME: // Display "Simon"

            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "SIMON", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            delay = 50.0;
            flag = true;
            isPressed = false;

            for (flashNum = 0; flashNum < 33; flashNum++) // generates a random number 1-4 32 times and stores in array
            {
                //  1 = D1, 4 = D4
                char value = (rand() % 4) + '1';
                flashArray[flashNum] = value;
            }

            state = COUNT_DOWN;
            break;

        case COUNT_DOWN:
            if (currKey == '*')
            {
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(2);

                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(2);

                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                swDelay(2);

                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "PLAY THE GAME!", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);

                level = 1;
                state = SEQUENCE;
            }
            break;

        case SEQUENCE:

            // once the max levels are reached, switches to

            if (level == winLevel + 1) // you win
            {
                win();
                swDelay(3);
                break;
            }

            for (ind1 = 0; ind1 < level; ind1++)
            {
                //               for (ind2 = 0; ind2 <= ind1; ind2++)
                //               {
                swDelay(1);
                switch (flashArray[ind1])
                { // array to led attacher
                case '1':
                    handleLeds('1');
                    BuzzerOn(175); //175, 150, 120, 80, lose: 250
                    break;
                case '2':
                    handleLeds('2');
                    BuzzerOn(150); //175, 150, 120, 80, lose: 250
                    break;
                case '3':
                    handleLeds('3');
                    BuzzerOn(120); //175, 150, 120, 80, lose: 250
                    break;
                case '4':
                    handleLeds('4');
                    BuzzerOn(80); //175, 150, 120, 80, lose: 250
                    break;
                }
                // swDelay(delay);
                swDelay2(delay);
                setLeds(0);
                BuzzerOff();
                //}
            }

            state = CHECK_INPUT;
            break;

        case CHECK_INPUT:

            game(0);

            break;

        } // closes switch          // end while (1)
    }     // closes while

} // closes main

void win()
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "YOU WON", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(3);
    state = WELCOME;
}

void lose()
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "YOU LOSE", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
    Graphics_flushBuffer(&g_sContext);
    BuzzerOn(250);
    swDelay(3);
    BuzzerOff();
    state = WELCOME;
}

void error()
{
    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, "ERROR", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "NO BRAIN CELLS", AUTO_STRING_LENGTH, 48, 65, TRANSPARENT_TEXT);

    Graphics_flushBuffer(&g_sContext);
    BuzzerOn(250); //175, 150, 120, 80, lose: 250
    swDelay(3);
    BuzzerOff();
    state = WELCOME;
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

void swDelay2(double delayTime) // blocking
{
    volatile double k, l; // volatile to prevent removal in optimization
                          // by compiler. Functionally this is useless code

    for (l = 0; l < delayTime; l++)
    {
        k = 10;       // SW Delay
        while (k > 0) // could also have used while (i)
            k--;
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

void game(int i)
{
    char currKey = getKey();

    if (i == level)
    {
        level++;
        delay = delay - 5;
        state = SEQUENCE;
        return;
    }

    while (resetGame() == false && currKey != '1' && currKey != '2' && currKey != '3' && currKey != '4' && currKey != '5' && currKey != '6' && currKey != '7' && currKey != '8' && currKey != '9' && currKey != '*' && currKey != '0' && currKey != '#')
    {
        currKey = getKey(); // do nothing keep looping
    }

    if (resetGame() == true) // RESET
    {

        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "RESET", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);

        swDelay(2);
        flag = false;
        state = WELCOME;
    }

    if (currKey == flashArray[i])
    {

        switch (flashArray[i])
        { // array to led attacher
        case '1':
            handleLeds(currKey);
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            BuzzerOn(175); //175, 150, 120, 80, lose: 250

            swDelay2(delay);
            Graphics_clearDisplay(&g_sContext);
            setLeds(0);
            break;
        case '2':
            handleLeds(currKey);
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            BuzzerOn(150); //175, 150, 120, 80, lose: 250

            swDelay2(delay);
            Graphics_clearDisplay(&g_sContext);
            setLeds(0);
            break;
        case '3':
            handleLeds(currKey);
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            BuzzerOn(120); //175, 150, 120, 80, lose: 250

            swDelay2(delay);
            Graphics_clearDisplay(&g_sContext);
            setLeds(0);
            break;
        case '4':
            handleLeds(currKey);
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "4", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            BuzzerOn(80); //175, 150, 120, 80, lose: 250

            swDelay2(delay);
            Graphics_clearDisplay(&g_sContext);
            setLeds(0);
            break;
        }
        BuzzerOff();

        i = i + 1;
        game(i);
        return;
    }

    if (flag)

    {
        if (currKey == '1' || currKey == '2' || currKey == '3' || currKey == '4')
        {
            lose();
        }
        else
        {
            error();
        }
    }
}
