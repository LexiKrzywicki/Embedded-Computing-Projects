
#include "peripherals.h"

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
}

void setLeds(unsigned char state)
{
    // Input: state = hex values to display (in low nibble)
    // Output: none

    unsigned char mask = 0;

    // Turn all LEDs off to start
    P6OUT &= ~(BIT4 | BIT3 | BIT2 | BIT1);

    if (state & BIT0)
        mask |= BIT4; // Right most LED P6.4
    if (state & BIT1)
        mask |= BIT3; // next most right LED P.3
    if (state & BIT2)
        mask |= BIT1; // third most left LED P6.1
    if (state & BIT3)
        mask |= BIT2; // Left most LED on P6.2
    P6OUT |= mask;
}

void BuzzerOn(void)
{
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL = (TBSSEL__ACLK | ID__1 | MC__UP); // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL &= ~TBIE;                          // Explicitly Disable timer interrupts for safety

    TB0CCR0 = 128;     // Set the PWM period in ACLK ticks
    TB0CCTL0 &= ~CCIE; // Disable timer interrupts

    TB0CCTL5 = OUTMOD_7;   // Set/reset mode for PWM
    TB0CCTL5 &= ~CCIE;     // Disable capture/compare interrupts
    TB0CCR5 = TB0CCR0 / 2; // Configure a 50% duty cycle
}

void BuzzerOff(void)
{
    // Disable both capture/compare periods
    TB0CCTL0 = 0;
    TB0CCTL5 = 0;
}

void configKeypad(void)
{
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

unsigned char getKey(void)
{
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

    Sharp96x96_Init();
    Graphics_initContext(&g_sContext, &g_sharp96x96LCD);

    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    Graphics_flushBuffer(&g_sContext);
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
    // Display is using Timer A1
    // Not sure where Timer A1 is configured?
    Sharp96x96_SendToggleVCOMCommand(); // display needs this toggle < 1 per sec
}