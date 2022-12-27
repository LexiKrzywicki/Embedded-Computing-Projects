/************** ECE2049 Lab 2 Prelab ******************/

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

// Function Prototypes
void lightLED(int m);
void game(int i);
int getButton();
void timerDelay(int delayTime);
void runtimerA2();

char check, conv;
bool x = true, displayStart = false, hit = false;
int m, button, losses, i = 0, countdownTimer = 3;
long unsigned int timer_cnt = 0, timer_delay = 0;


const int tempo = 200; // set tempo for entire song
const int e = tempo / 2; // eigth note
const int q = tempo;     // quarter note
const int h = tempo * 2; // half note

const int A = 75, B = 66, Bflat = 70, Csharp = 59, D = 56, Dl = 111, E = 50, El = 99, F = 44, Fl = 88, G = 42, Gl = 84, rest = 0;

// a d a d a d a        a d a d f       e d c b bphat a d a f g a        d        a d b a g f e d
int noteArray[34] = {A, D, A, Dl,
                     A, D, A,
                     A, D, A, D,
                     F, rest, E, D, Csharp, B, Bflat,
                     A, D, A, Fl, Gl,
                     A, D, A,
                     D, rest, B, A, Gl,
                     Fl, El, Dl};

int noteArray2[34] = {A, D, A, D,
                      A, D, A,
                      A, D, A, D,
                      F, rest, E, D, Csharp, B, Bflat,
                      A, D, A, F, Gl,
                      A, D, A,
                      D, rest, B, A, G,
                      F, E, D};

int lengthArray[34] = {q, q, q, q,
                       q, q, h,
                       q, q, q, q,
                       q, e, e, e, e, e, e,
                       q, q, q, e, e,
                       q, q, h,
                       q, e, e, q, q,
                       q, q, q};

enum STATE
{
    WELCOME,
    COUNT_DOWN,
    PLAY_SONG,
    LOSE,
    WIN
};

enum STATE state = WELCOME;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    runtimerA2();

    initLeds();
    configDisplay();
    configKeypad();
    configButton();
    _BIS_SR(GIE);

    state = WELCOME;

    while (1)
    {

        switch (state)
        {
        case WELCOME: // 1. Welcome
            if (displayStart == false)
            {
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "MSP430 HERO", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Please press *", AUTO_STRING_LENGTH, 48, 40, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                configUserLED('0');

                displayStart = true; // escape looping test
                i = 0;               // 2. i = 0
            }
            else if (displayStart == true)
            {
                if (getKey() == '*')
                {
                    state = COUNT_DOWN;
                }
            }
            break;

        case COUNT_DOWN:
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 55, OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            configUserLED('1');
            timerDelay(200);

            configUserLED('0');
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 55, OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            configUserLED('2');
            timerDelay(200);

            configUserLED('0');
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 55, OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            configUserLED('1');
            timerDelay(200);

            configUserLED('0');
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "GO", AUTO_STRING_LENGTH, 48, 55, OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            configUserLED('1');
            configUserLED('2');
            timerDelay(200);

            configUserLED('0');
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "PLAY GAME", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            timerDelay(200);
            Graphics_clearDisplay(&g_sContext);
            state = PLAY_SONG;
            break;

        case PLAY_SONG:

            if (i == 34)
            {
                state = WIN;
            }
            else
            {
                button = 0;
                hit = false;
                game(i);
                i++;
            }
            break;

        case LOSE:
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "WHY...WHY", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            timerDelay(200);
            displayStart = false;

            state = WELCOME;
            break;
        case WIN:
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "YOU WON", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            timerDelay(200);
            state = WELCOME;
            break;
        }
    }
}

void runtimerA2()
{
    TA2CTL = TASSEL_1 + MC_1 + ID_0; // 327680
    TA2CCR0 = 163;                   // 163-1 = 162 ACLK tics = 0.005 seconds resolution
    TA2CCTL0 = CCIE;                 // TA2CCR0 interrupt enabled
}

void game(int i)
{

    configUserLED('0');
    BuzzerOn(noteArray[i]); // note sounds
    lightLED(noteArray[i]); // 3. LED i    < is this going to be a problem if we need to light 2 LEDs

    timer_cnt = 0; // reset note timer to 0

    while (timer_cnt < lengthArray[i]) // while timer is less than note duration
    {
        getButton(); // set press as button
        if (hit == false)
        {
            if (button == noteArray2[i]) // 4. if button equals song char
            {
                hit = true;
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "Hit", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
            }
        }
    }

    if (hit == false) // if win variable not true, loose
    {
        if (losses == 5) // if losses equal to 5 total
        {
            state = LOSE; // step 1:
            losses = 0;   // rest losses
        }
        else
        {
            losses++; // index losses
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "Miss", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
        }
    }
    BuzzerOff();
    timerDelay(50);
}

int getButton()
{
    // S1: 7.0
    // S2: 3.6
    // S3: 2.2
    // S4: 7.4

    button = 0;

    if (getKey() == '#')
    {
        BuzzerOff();
        configUserLED('0');
        displayStart = false;
        state = WELCOME;
    }
    else if ((P7IN & BIT0) == 0) // S1
    {
        button = A;
    }
    else if ((P3IN & BIT6) == 0) // S2
    {
        button = B;
    }
    else if ((P2IN & BIT2) == 0) // S3
    {
        button = Csharp;
    }
    else if ((P7IN & BIT4) == 0) // S4
    {
        button = D;
    }
    else if (((P7IN & BIT0) && (P3IN & BIT6)) == 0)
    {
        button = Bflat;
    }
    else if (((P7IN & BIT0) && (P7IN & BIT4)) == 0)
    {
        button = E;
    }
    else if (((P7IN & BIT0) && (P2IN & BIT2)) == 0)
    {
        button = F;
    }
    else if (((P7IN & BIT4) && (P3IN & BIT6)) == 0)
    {
        button = G;
    }

    return button;
}

void lightLED(const int m)
{ // lights LEDs based on input int
    switch (m)
    {
    case A: // A
        configUserLED('1');
        break;
    case B: // B
        configUserLED('2');
        break;
    case Bflat: // B flat
        configUserLED('1');
        configUserLED('2');
        break;
    case Csharp: // Csharp
        configUserLED('4');
        break;
    case D: // D or DL
        configUserLED('8');
        break;
    case Dl: // D or DL
        configUserLED('8');
        break;
    case E: // E or EL
        configUserLED('1');
        configUserLED('8');
        break;
    case El: // E or EL
        configUserLED('1');
        configUserLED('8');
        break;
    case F: // F or FL
        configUserLED('1');
        configUserLED('4');
        break;
    case Fl:
        configUserLED('1');
        configUserLED('4');
        break;
    case G: // G or GL
        configUserLED('2');
        configUserLED('8');
        break;
    case Gl: // G or GL
        configUserLED('2');
        configUserLED('8');
        break;
    case rest: // no note
        configUserLED('0');
        break;
    }
}

void timerDelay(int delayTime)
{
    timer_delay = 0;
    while (timer_delay < delayTime)
    {
        //do nothing
    }
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR()
{
    timer_cnt++;
    timer_delay++;
}
