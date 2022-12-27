#include "peripherals.h"

// Globals
tContext g_sContext; // user defined type used by graphics library

void initLeds(void)
{
    // Configure LEDs as outputs, initialize to logic low (off)
    // Note the assigned port pins are out of order test board
    // Red     P6.2
    // Green   P6.1
    // Blue    P6.3
    // Yellow  P6.4
    // smj -- 27 Dec 2016

    P6SEL &= ~(BIT4 | BIT3 | BIT2 | BIT1);
    P6DIR |= (BIT4 | BIT3 | BIT2 | BIT1);
    P6OUT &= ~(BIT4 | BIT3 | BIT2 | BIT1);

    P1SEL &= ~(BIT0);
    P1DIR |= (BIT0);
    P1OUT &= ~(BIT0);
    P4SEL &= ~(BIT7);
    P4DIR |= (BIT7);
    P4OUT &= ~(BIT7);
}
// S1: 7.0
// S2: 3.6
// S3: 2.2
// S4: 7.4
void configButton()
{
    // select pins should equal 0
    P7SEL &= ~(BIT0);
    P3SEL &= ~(BIT6);
    P2SEL &= ~(BIT2);
    P7SEL &= ~(BIT4);

    // set to inputs should equal 0
    P7DIR &= ~(BIT0);
    P3DIR &= ~(BIT6);
    P2DIR &= ~(BIT2);
    P7DIR &= ~(BIT4);

    P7REN |= (BIT0);
    P3REN |= (BIT6);
    P2REN |= (BIT2);
    P7REN |= (BIT4);

    P7OUT |= (BIT0);
    P3OUT |= (BIT6);
    P2OUT |= (BIT2);
    P7OUT |= (BIT4);
}

// bool resetGame(void)
// {
//     bool isPressed = false;
//     if ((P7IN & BIT0) == 0 && (P3IN & BIT6) == 0)
//     {
//         isPressed = true;
//     }

//     return isPressed;
// }

void configUserLED(char inbits)
{

    unsigned char mask = 0;

    // Turn all LEDs off to start

    if (inbits & BIT0)
    {
        mask |= BIT2; // Left most LED 6.2
        // P1OUT |= BIT0;
    }

    if (inbits & BIT1)
    {
        mask |= BIT1; // P6.1
        // P4OUT |= BIT7;
    }

    if (inbits & BIT2)
    {
        mask |= BIT3; // LED P6.3
    }

    if (inbits & BIT3)
    {
        mask |= BIT4; // Right most LED - 6.4
    }
    if (inbits == '0')
    {
        P6OUT &= ~(BIT4 | BIT3 | BIT2 | BIT1);
        P1OUT &= ~(BIT0);
        P4OUT &= ~(BIT7);
    }

    P6OUT |= mask;
}

/*
 * Enable a PWM-controlled buzzer on P3.5
 * This function makes use of TimerB0.
 */
void BuzzerOn(int pwm) // PWM vals: 175, 150, 120, 80, 250 for LOSE
{
    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL = (TBSSEL__ACLK | ID__1 | MC__UP); // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL &= ~TBIE;                          // Explicitly Disable timer interrupts for safety

    // Now configure the timer period, which controls the PWM period
    // Doing this with a hard coded values is NOT the best method
    // We do it here only as an example. You will fix this in Lab 2.
    TB0CCR0 = pwm;     // Set the PWM period in ACLK ticks
    TB0CCTL0 &= ~CCIE; // Disable timer interrupts

    // Configure CC register 5, which is connected to our PWM pin TB0.5
    TB0CCTL5 = OUTMOD_7;   // Set/reset mode for PWM
    TB0CCTL5 &= ~CCIE;     // Disable capture/compare interrupts
    TB0CCR5 = TB0CCR0 / 2; // Configure a 50% duty cycle
}

/*
 * Disable the buzzer on P7.5
 */
void BuzzerOff(void)
{
    // Disable both capture/compare periods
    TB0CCTL0 = 0;
    TB0CCTL5 = 0;
}

void configKeypad(void)
{
    // Configure digital IO for keypad
    // smj -- 27 Dec 2015

    // Col1 = P1.5 =
    // Col2 = P2.4 =
    // Col3 = P2.5 =
    // Row1 = P4.3 =
    // Row2 = P1.2 =
    // Row3 = P1.3 =
    // Row4 = P1.4 =

    P1SEL &= ~(BIT5 | BIT4 | BIT3 | BIT2);
    P2SEL &= ~(BIT5 | BIT4);
    P4SEL &= ~(BIT3);

    // Columns are ??
    P2DIR |= (BIT5 | BIT4);
    P1DIR |= BIT5;
    P2OUT |= (BIT5 | BIT4); //
    P1OUT |= BIT5;          //

    // Rows are ??
    P1DIR &= ~(BIT2 | BIT3 | BIT4);
    P4DIR &= ~(BIT3);
    P4REN |= (BIT3); //
    P1REN |= (BIT2 | BIT3 | BIT4);
    P4OUT |= (BIT3); //
    P1OUT |= (BIT2 | BIT3 | BIT4);
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

void swDelay2(int delayTime) // blocking
{
    volatile unsigned int k, l; // volatile to prevent removal in optimization
                                // by compiler. Functionally this is useless code

    for (l = 0; l < delayTime; l++)
    {
        k = 10;       // SW Delay
        while (k > 0) // could also have used while (i)
            k--;
    }
}

// int A = 75;
// int B = 66;
// int c = 59;
// int D = 56;
// int lD = 111;
// // int hD = 28;
// int E = 50;
// int lE = 99;
// // int hE = 25;
// int F = 44;
// int lF = 88;
// // int hf = 22;
// int G = 42;
// int lG = 84;
// int Bflat = 70;
// int rest = 0;

// int noteArray[34] = {75, 56, 75, 111,
//                      75, 56, 75,
//                      75, 56, 75, 56,
//                      44, 0, 50, 56, 59, 66, 70,
//                      75, 56, 75, 88, 84,
//                      75, 56, 75,
//                      56, 0, 66, 75, 84,
//                      88, 99, 111};

/*void configReset(void) //
{
    // configure digital IO

    // selecting pins
    P7SEL &= ~BIT0; // Button 1
    P3SEL &= ~BIT6; // Button 2
    P2SEL &= ~BIT2; // Button 3
    P7SEL &= ~BIT4; // Button 4

    // setting direction. theses are set to input
    P7DIR &= ~BIT0;
    P3DIR &= ~BIT6;
    P2DIR &= ~BIT2;
    P7DIR &= ~BIT4;

    // initiate pull-ups
    P7REN |= BIT0;
    P3REN |= BIT6;
    P2REN |= BIT2;
    P7REN |= BIT4;

    // set as pull up
    P7OUT |= BIT0;
    P3OUT |= BIT6;
    P2OUT |= BIT2;
    P7OUT |= BIT4;
}

// if S1 is pressed, return 0x01
// if S2 is pressed, return 0x02
// if S3 is pressed, return 0x04
// id S4 is pressed, retunr 0x08
// if S1 and S3 are pressed, return 0x05
unsigned char getButton(int button)
{
    unsigned char ret_val = 0;
    char buttonArray[4] = {0, 0, 0, 0};

    if ((P7IN & BIT0) == 0)
    {
        buttonArray[0] = '1';
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        configUserLED('1');
        // swDelay(2);
        // configUserLED(0);
    }
    if ((P3IN & BIT6) == 0)
    {
        buttonArray[1] = '2';
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        configUserLED('2');
        // swDelay(2);
        // configUserLED(0);
    }
    if ((P2IN & BIT2) == 0)
    {
        buttonArray[2] = '4';
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        // configUserLED('2');
        // swDelay(2);
        // configUserLED(0);
    }
    if ((P7IN & BIT4) == 0)
    {
        buttonArray[3] = '8';
        Graphics_clearDisplay(&g_sContext);
        Graphics_drawStringCentered(&g_sContext, "4", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        // configUserLED('1');
        // swDelay(2);
        // configUserLED(0);
    }

    ret_val = buttonArray[0] + buttonArray[1] + buttonArray[2] + buttonArray[3];
    if (ret_val != 0x00)
    {
        configUserLED(ret_val);
        swDelay(1);
        configUserLED(0);
    }

    return ret_val;
}
*/

unsigned char getKey(void)
{
    // Returns ASCII value of key pressed from keypad or 0.
    // Does not decode or detect when multiple keys pressed.
    // smj -- 27 Dec 2015
    // Updated -- 14 Jan 2018

    unsigned char ret_val = 0;

    // Set Col1 = ?, Col2 = ? and Col3 = ?
    P1OUT &= ~BIT5;
    P2OUT |= (BIT5 | BIT4);
    // Now check value from each rows
    if ((P4IN & BIT3) == 0)
        ret_val = '1';
    if ((P1IN & BIT2) == 0)
        ret_val = '4';
    if ((P1IN & BIT3) == 0)
        ret_val = '7';
    if ((P1IN & BIT4) == 0)
        ret_val = '*';
    P1OUT |= BIT5;

    // Set Col1 = ?, Col2 = ? and Col3 = ?
    P2OUT &= ~BIT4;
    // Now check value from each rows
    if ((P4IN & BIT3) == 0)
        ret_val = '2';
    if ((P1IN & BIT2) == 0)
        ret_val = '5';
    if ((P1IN & BIT3) == 0)
        ret_val = '8';
    if ((P1IN & BIT4) == 0)
        ret_val = '0';
    P2OUT |= BIT4;

    // Set Col1 = ?, Col2 = ? and Col3 = ?
    P2OUT &= ~BIT5;
    // Now check value from each rows
    if ((P4IN & BIT3) == 0)
        ret_val = '3';
    if ((P1IN & BIT2) == 0)
        ret_val = '6';
    if ((P1IN & BIT3) == 0)
        ret_val = '9';
    if ((P1IN & BIT4) == 0)
        ret_val = '#';
    P2OUT |= BIT5;

    return (ret_val);
}

void configDisplay(void)
{
    // Enable use of external clock crystals
    P5SEL |= (BIT5 | BIT4 | BIT3 | BIT2);

    // Initialize the display peripheral
    Sharp96x96_Init();

    // Configure the graphics library to use this display.
    // The global g_sContext is a data structure containing information the library uses
    // to send commands for our particular display.
    // You must pass this parameter to each graphics library function so it knows how to
    // communicate with our display.
    Graphics_initContext(&g_sContext, &g_sharp96x96LCD);

    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    Graphics_flushBuffer(&g_sContext);
}

/*
void setupSPI_DAC(void)
{
// ** Set UCSI A0 Reset=1 to configure control registers **
     UCB0CTL1 |= UCSWRST;
     // 3-pin (SCLK, SIMO, SOMI), 8-bit, this MSP430 is SPI master,
     // Clock polarity high, send data MSB first
     UCB0CTL0 = UCMST + UCSYNC + UCCKPH + UCMSB;
     // Use SMCLK as clock source, keep RESET = 1
     UCB0CTL1 = UCSWRST + UCSSEL_2;

     UCB0BR0 = 2;   // SCLK = SMCLK/2 = 524288Hz
     UCB0BR1 = 0;

     //UCB0MCTL = 0;   // write MCTL as 0

     // Enable UCSI A0
     UCB0CTL1 &= ~UCSWRST;
}
*/

//------------------------------------------------------------------------------
// Timer2 A0 Interrupt Service Routine
//------------------------------------------------------------------------------
// #pragma vector = TIMER2_A0_VECTOR
// __interrupt void TIMER2_A0_ISR(void)
// {

//     if(tdir)
//     {
//         timer_cnt++;
//     }
//     // Display is using Timer A1
//     // Not sure where Timer A1 is configured?
//     //Sharp96x96_SendToggleVCOMCommand(); // display needs this toggle < 1 per sec
// }

// #pragma vector = TIMER1_A0_VECTOR
// __interrupt void TIMER1_A0_ISR(void)
// {
//     // Display is using Timer A1
//     // Not sure where Timer A1 is configured?
//     Sharp96x96_SendToggleVCOMCommand(); // display needs this toggle < 1 per sec
// }
