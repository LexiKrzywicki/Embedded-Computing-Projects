#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

long unsigned int inTime, timer_cnt, prevTimer, lBday = 14769420;

int seconds, minutes, hours, oldDays, days, months;
unsigned char monthArray[], dayArray[], hourArray[], minArray[], secArray[], cArray[], fArray[];
unsigned char button;
unsigned int in_value;

unsigned int A10results, A0results;
unsigned long timer = 0;
volatile unsigned int bits30, bits85;

// temperature variables
unsigned int tempSense;
float degC;
float degF;
float celsiusPerBit;
char dispCelsius[4], dispFahrenheit[4];
float tempC[60];

#define CALADC12_15V_30C *((unsigned int *)0x1A1A) // 6682
#define CALADC12_15V_85C *((unsigned int *)0x1A1C) // 6684

// function prototypes
void displayTime(long unsigned int inTime);
void convertTemp(float tempC);
void displayTemp();
void startTimerA2();
void configADC12();
void configLPB();
unsigned char getLPB();
void configScroll();
void delay(int delayTime);
void clearADC();
__interrupt void TimerA2_ISR();
__interrupt void ADC12ISR();

int main()
{
    volatile float degCPerBit;

    WDTCTL = WDTPW | WDTHOLD;
    REFCTL0 &= ~REFMSTR;

    initLeds(); // extra credit
    configDisplay();

    _BIS_SR(GIE);

    Graphics_clearDisplay(&g_sContext);
    displayTime(lBday); // initate time with Lexi's B-day
    Graphics_flushBuffer(&g_sContext);

    startTimerA2();

    while (1)
    {
        getLPB();
        configADC12();
        configScroll();
        clearADC();

        if (timer_cnt != prevTimer) // checks to see if one seconds passed
        {
            prevTimer = timer_cnt;
            displayTime(lBday);
            convertTemp(tempC[timer % 60]);
			displayTemp();

            // Gaphics_drawStringCentered(&g_sContext, in_value, AUTO_STRING_LENGTH, 39, 10, OPAQUE_TEXT);
            // Graphics_flushBuffer(&g_sContext);
            lBday++;
        }

        int indexArray[] = {1, 2, 3, 4, 5};

        int i = 0;
        if (getLPB() == 'L')
        {
            Graphics_clearDisplay(&g_sContext);
            Graphics_drawStringCentered(&g_sContext, "EDIT MODE", AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);

            while (getLPB() != 'R')
            {
                if (getLPB() == 'L')
                {
                    i++;
                }

                if (indexArray[i] == 1)
                {
                    // months = scroll wheel
                    Graphics_drawStringCentered(&g_sContext, "EDIT MONTHS", AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                }
                else if (indexArray[i] == 2)
                {
                    // days = scroll wheel
                    Graphics_drawStringCentered(&g_sContext, "EDIT DAYS", AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                }
                else if (indexArray[i] == 3)
                {
                    // hours = scroll wheel
                    Graphics_drawStringCentered(&g_sContext, "EDIT HOURS", AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                }
                else if (indexArray[i] == 4)
                {
                    // minutes = scroll wheel
                    Graphics_drawStringCentered(&g_sContext, "EDIT MINUTES", AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                }
                else if (indexArray[i] == 5)
                {
                    // seconds = scroll wheel
                    Graphics_drawStringCentered(&g_sContext, "EDIT SECONDS", AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    button = '0';
                    i = 0;
                    delay(1);
                    Graphics_clearDisplay(&g_sContext);
                    Graphics_flushBuffer(&g_sContext);
                }
            }
        }
    }
}

unsigned char getLPB()
{
    button = 'n';

    if ((P2IN & BIT1) == 0) // LPB Left
    {
        button = 'L';
    }
    if ((P1IN & BIT1) == 0) // LPB Right
    {
        button = 'R';
    }

    return button;
}

// takes in time in seconds and prints the date and time to LCD
void displayTime(long unsigned int inTime)
{

    seconds = (inTime % 60);
    minutes = (inTime / (60)) % 60;
    hours = (inTime / (3600)) % 24;
    oldDays = (inTime / (86400)); // 3200 * 24

    hourArray[0] = hours / 10 + 0x30;
    hourArray[1] = hours % 10 + 0x30;
    hourArray[2] = 0x3A;
    minArray[0] = minutes / 10 + 0x30;
    minArray[1] = minutes % 10 + 0x30;
    minArray[2] = 0x3A;
    secArray[0] = seconds / 10 + 0x30;
    secArray[1] = seconds % 10 + 0x30;
    secArray[2] = 0x20; // to get rid of "."

    int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    short int done = 0;

    int i;
    for (i = 0; i < 12 && !done; i++) // find month and day
    {
        if (oldDays < monthDays[i])
        {
            days = oldDays + 1;
            months = i + 1;
            done = 1;
        }
        else
        {
            oldDays -= monthDays[i];
        }
    }

    switch (months)
    {
    case 1:
        monthArray[0] = 'J';
        monthArray[1] = 'A';
        monthArray[2] = 'N';
        break;
    case 2:
        monthArray[0] = 'F';
        monthArray[1] = 'E';
        monthArray[2] = 'B';
        break;
    case 3:
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'R';
        break;

    case 4:
        monthArray[0] = 'A';
        monthArray[1] = 'P';
        monthArray[2] = 'R';
        break;
    case 5:
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'Y';
        break;
    case 6:
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'N';
        break;
    case 7:
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'L';
        break;
    case 8:
        monthArray[0] = 'A';
        monthArray[1] = 'U';
        monthArray[2] = 'G';
        break;
    case 9:
        monthArray[0] = 'S';
        monthArray[1] = 'E';
        monthArray[2] = 'P';
        break;
    case 10:
        monthArray[0] = 'O';
        monthArray[1] = 'C';
        monthArray[2] = 'T';
        break;
    case 11:
        monthArray[0] = 'N';
        monthArray[1] = 'O';
        monthArray[2] = 'V';
        break;
    case 12:
        monthArray[0] = 'D';
        monthArray[1] = 'E';
        monthArray[2] = 'C';
        break;
    }

    dayArray[0] = (days) / 10 + 0x30;
    dayArray[1] = (days) % 10 + 0x30;
    dayArray[2] = 0x20;

    Graphics_flushBuffer(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, monthArray, AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, dayArray, AUTO_STRING_LENGTH, 59, 25, OPAQUE_TEXT);

    Graphics_drawStringCentered(&g_sContext, hourArray, AUTO_STRING_LENGTH, 33, 40, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, minArray, AUTO_STRING_LENGTH, 50, 40, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, secArray, AUTO_STRING_LENGTH, 68, 40, OPAQUE_TEXT);
}

void convertTemp(float tempC)
{

	degF = tempC * 9.0 / 5.0 + 32.0;

	unsigned int degC_10ths = tempC * 10, degF_10ths = degF * 10;
	char degC100, degC10, degC1, degC10th;
	char degF100, degF10, degF1, degF10th;

	degC100 = degC_10ths / 1000;
	degC10 = (degC_10ths - degC100 * 1000) / 100;
	degC1 = (degC_10ths - degC100 * 1000 - degC10 * 100) / 10;
	degC10th = degC_10ths % 10;
	degF100 = degF_10ths / 1000;
	degF10 = (degF_10ths - degF100 * 1000) / 100;
	degF1 = (degF_10ths - degF100 * 1000 - degF10 * 100) / 10;
	degF10th = degF_10ths % 10;

	dispCelsius[0] = degC100 + 0x30;
	dispCelsius[1] = degC10 + 0x30;
	dispCelsius[2] = degC1 + 0x30;
	dispCelsius[3] = degC10th + 0x30;
	dispFahrenheit[0] = degF100 + 0x30;
	dispFahrenheit[1] = degF10 + 0x30;
	dispFahrenheit[2] = degF1 + 0x30;
	dispFahrenheit[3] = degF10th + 0x30;
}

void displayTemp()
{
	GrStringDrawCentered(&g_sContext, dispCelsius, 3, 35, 45, OPAQUE_TEXT);
	GrStringDrawCentered(&g_sContext, ".", 1, 47, 45, OPAQUE_TEXT);
	GrStringDrawCentered(&g_sContext, dispCelsius + 3, 1, 52, 45, OPAQUE_TEXT);
	GrStringDrawCentered(&g_sContext, "C", 1, 60, 45, OPAQUE_TEXT);
	GrStringDrawCentered(&g_sContext, dispFahrenheit, 3, 35, 55, OPAQUE_TEXT);
	GrStringDrawCentered(&g_sContext, ".", 1, 47, 55, OPAQUE_TEXT);
	GrStringDrawCentered(&g_sContext, dispFahrenheit + 3, 1, 52, 55, OPAQUE_TEXT);
	GrStringDrawCentered(&g_sContext, "F", 1, 60, 55, OPAQUE_TEXT);
	GrFlush(&g_sContext);
}

void configScroll()
{
    P8SEL &= ~BIT0;
    P8DIR |= BIT0;
    P8OUT |= BIT0;
    REFCTL0 &= ~REFMSTR;
    // Reset REFMSTR to hand over control of internal reference voltages to
    // ADC12_A control registers

    ADC12CTL0 = ADC12SHT0_9 | ADC12ON;
    ADC12CTL1 = ADC12SHP; // Enable sample timer

    // Use ADC12MEM0 register for conversion results
    ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_5; // ADC12INCH5 = Scroll wheel = A5
                                            // ACD12SREF_0 = Vref+ = Vcc
    __delay_cycles(100);                    // delay to allow Ref to settle
    ADC12CTL0 |= ADC12ENC;                  // Enable conversion

    // Poll busy bit waiting for conversion to complete
    while (ADC12CTL1 & ADC12BUSY)
    {
        __no_operation();
    }

    in_value = ADC12MEM0; // Read results if conversion done
}

void delay(int delayTime)
{
    int freezeTime = timer_cnt;
    while (timer_cnt < freezeTime + delayTime)
    { // do nothing
    }
}

void startTimerA2()
{
    // Use ACLK, 16 Bit, up mode, 1 divider
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 32767; // 32767+1 = 32768 ACLK tics = 1 second
    TA2CCTL0 = CCIE;
}

void configADC12()
{
    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON | ADC12MSC; // Internal ref = 1.5V
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1;                      // Enable sample timer
    // Using ADC12MEM0 to store reading
    ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10; // ADC i/p ch A10 = temp sense
                                             // ACD12SREF_1 = internal ref = 1.5v
    P6SEL |= BIT0;
    ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_0 + ADC12EOS;
    ADC12IE = BIT1; // delay to allow Ref to settle
    ADC12CTL0 |= ADC12ENC;
    // Use calibration data stored in info memory
    bits30 = CALADC12_15V_30C;
    bits85 = CALADC12_15V_85C;
    celsiusPerBit = ((float)(85.0 - 30.0)) / ((float)(bits85 - bits30));
}

void clearADC()
{
    ADC12CTL0 &= ~ADC12SC; // clear starting bit
    ADC12CTL0 |= ADC12SC;  // sample and conversion start
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR()
{
    A10results = ADC12MEM0 & 0x0FFF;
    tempSense = A10results;
    degC = (float)((long)tempSense - CALADC12_15V_30C) * celsiusPerBit + 30.0;
    A0results = ADC12MEM1 & 0x0FFF;
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR()
{
    timer_cnt++;
    tempC[timer % 60] = degC;
}
