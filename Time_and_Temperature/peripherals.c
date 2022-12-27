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

void configLPB()
{
    // configure digital IO
    P2SEL &= ~BIT1;
    P1SEL &= ~BIT1;

    // set P1.1 and P2.1 as input
    P2DIR &= ~BIT1;
    P1DIR &= ~BIT1;

    // initiate pull-ups
    P2REN |= BIT1;
    P1REN |= BIT1;

    // set pull up
    P2OUT |= BIT1;
    P1OUT |= BIT1;
}

void configDisplay(void)
{
    // Enable use of external clock crystals
    P5SEL |= (BIT5 | BIT4 | BIT3 | BIT2);

    Sharp96x96_Init();
    
    Graphics_initContext(&g_sContext, &g_sharp96x96LCD);

    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    Graphics_flushBuffer(&g_sContext);
}
