// Team 5
// Lab 3: “Making a Time and Temperature Display”

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

long unsigned int inTime, prevTimer, timer_cnt = 14769420; // Jun 20th 10:37:00

int i = 0;
int indexArray[] = {0, 1, 2, 3, 4, 5};

int seconds, minutes, hours, oldDays, days, months, editMonth;
float inAvgTempC, inAvgTempF, in_temp, TempC[], degCPerBit, slope;
unsigned char monthArray[], dayArray[], hourArray[], minArray[], secArray[], cArray[], fArray[];
unsigned char button;
unsigned int in_value;
float tempC[30];

float degC;
float degF;
float celsiusPerBit;
unsigned int A10results, A0results;

unsigned int tempSense;
unsigned long timer = 0;

volatile float temperatureDegC;
volatile float temperatureDegF;
volatile float degC_per_bit;
volatile unsigned int bits30, bits85;

#define CALADC12_15V_30C *((unsigned int *)0x1A1A) // 6682
#define CALADC12_15V_85C *((unsigned int *)0x1A1C) // 6684

// function prototypes
void displayTime(long unsigned int inTime);
void displayTemp(float inAvgTempC);
void startTimerA2();
void stopTimerA2(int reset);
void configADC12();
void configLPB();
void getTemp();
unsigned char getLPB();
void configScroll();
unsigned int getScroll();
void delay(int delayTime);
void setMonths();
void setDays();
void setHours();
void setMinutes();
void setSeconds();
void lockTime();

__interrupt void TimerA2_ISR();
__interrupt void ADC12ISR();

int main()
{
    volatile float degCPerBit;

    WDTCTL = WDTPW | WDTHOLD;
    REFCTL0 &= ~REFMSTR;

    configDisplay();
    configLPB();

    _BIS_SR(GIE);

    Graphics_clearDisplay(&g_sContext);
    displayTime(timer_cnt); // initate time with Lexi's B-day
    Graphics_flushBuffer(&g_sContext);

    startTimerA2();

    configADC12();
    int counter; // temp counter for averaging
    int a;
    float avTemp;
    bool clear;
    char buttonRead;

    while (1)
    {

        // mod = time % 12
        //  if mod <= 2 display date,
        buttonRead = getLPB();

        if (timer_cnt != prevTimer) // checks to see if one seconds passed
        {
            prevTimer = timer_cnt;
            getTemp(); // get temperature reading

            tempC[counter++] = temperatureDegC;
            if (counter >= 30)
            {
                avTemp = 0;
                for (a = 0; a < 30; a++)
                {
                    avTemp += tempC[a];
                }
                avTemp /= 30;
                counter = 0;
            }
            else
            {
                avTemp = 0;
                for (a = 0; a < counter; a++)
                {
                    avTemp += tempC[a];
                }
                avTemp /= counter;
            }

            displayTime(timer_cnt);
            displayTemp(avTemp);
        }

        if (buttonRead == 'L')
        {
            i = 0;
            configScroll();
            stopTimerA2(0); // stops timer
            clear = false;

            while (buttonRead != 'R')
            {
                buttonRead = getLPB();
                getScroll();
                if (buttonRead == 'L' && clear)
                {
                    Graphics_flushBuffer(&g_sContext);
                    Graphics_clearDisplay(&g_sContext);
                    i++;
                    clear = false;
                }

                switch (indexArray[i])
                {
                case 0:
                    if (!clear)
                    {
                        clear = true;
                        Graphics_clearDisplay(&g_sContext);
                    }

                    setMonths();
                    Graphics_drawStringCentered(&g_sContext, "EDIT MONTHS", AUTO_STRING_LENGTH, 45, 25, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, monthArray, AUTO_STRING_LENGTH, 45, 35, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                    break;
                case 1:
                    if (!clear)
                    {
                        clear = true;
                        Graphics_clearDisplay(&g_sContext);
                    }

                    setDays();
                    Graphics_drawStringCentered(&g_sContext, "EDIT DAYS", AUTO_STRING_LENGTH, 45, 25, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, dayArray, AUTO_STRING_LENGTH, 45, 35, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                    break;
                case 2:
                    if (!clear)
                    {
                        clear = true;
                        Graphics_clearDisplay(&g_sContext);
                    }
                    setHours();
                    Graphics_drawStringCentered(&g_sContext, "EDIT HOURS", AUTO_STRING_LENGTH, 45, 25, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, hourArray, AUTO_STRING_LENGTH, 45, 35, OPAQUE_TEXT);

                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                    break;
                case 3:
                    if (!clear)
                    {
                        clear = true;
                        Graphics_clearDisplay(&g_sContext);
                    }

                    setMinutes();
                    Graphics_drawStringCentered(&g_sContext, "EDIT MINUTES", AUTO_STRING_LENGTH, 45, 25, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, minArray, AUTO_STRING_LENGTH, 45, 35, OPAQUE_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                    break;
                case 4:
                    if (!clear)
                    {
                        clear = true;
                        Graphics_clearDisplay(&g_sContext);
                    }

                    setSeconds();
                    Graphics_drawStringCentered(&g_sContext, "EDIT SECONDS", AUTO_STRING_LENGTH, 45, 25, OPAQUE_TEXT);
                    Graphics_drawStringCentered(&g_sContext, secArray, AUTO_STRING_LENGTH, 45, 35, OPAQUE_TEXT);

                    Graphics_flushBuffer(&g_sContext);
                    getLPB();
                    break;
                case 5:
                    i = 0;
                    break;
                }
            }
            Graphics_flushBuffer(&g_sContext);
            Graphics_clearDisplay(&g_sContext);
            i = 0;
            // method to convert editted time to timer_cnt
            lockTime();
            configADC12();
            startTimerA2();
        }
    }
}

unsigned char getLPB()
{

    if ((P2IN & BIT1) == 0) // LPB Left
    {
        button = 'L';
    }
    else if ((P1IN & BIT1) == 0) // LPB Right
    {
        button = 'R';
    }
    else
    {
        button = 'n';
    }

    return button;
}

void delay(int delayTime)
{
    int freezeTime = timer_cnt;
    while (timer_cnt < freezeTime + delayTime)
    { // do nothing
    }
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

    int mod = timer_cnt % 12;

    if (mod < 3)
    {
        if (mod == 0)
        {
            Graphics_clearDisplay(&g_sContext);
        }
        Graphics_drawStringCentered(&g_sContext, monthArray, AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, dayArray, AUTO_STRING_LENGTH, 59, 25, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
    }
    else if (mod < 6)
    {
        if (mod == 3)
        {
            Graphics_clearDisplay(&g_sContext);
        }
        Graphics_drawStringCentered(&g_sContext, hourArray, AUTO_STRING_LENGTH, 33, 40, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, minArray, AUTO_STRING_LENGTH, 50, 40, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, secArray, AUTO_STRING_LENGTH, 68, 40, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
    }
}

void displayTemp(float inAvgTempC)
{
    int cHund = (int)inAvgTempC / 100;
    int cTen = ((int)inAvgTempC - cHund * 100) / 10;
    int cOne = ((int)inAvgTempC - (cHund * 100) - (cTen * 10));

    cArray[0] = cHund + 0x30;
    cArray[1] = cTen + 0x30;
    cArray[2] = cOne + 0x30;
    cArray[3] = 0x2E;
    cArray[4] = (inAvgTempC - (int)inAvgTempC) * 10 + 0x30;

    if (cHund == 0)
    {
        cArray[0] = 0x20;
    }
    if (cHund >= 1)
    {
        cArray[0] = cHund + 0x30;
    }

    inAvgTempF = inAvgTempC * 9 / 5 + 32;

    int fHund = (int)inAvgTempF / 100;
    int fTen = ((int)inAvgTempF - fHund * 100) / 10;
    int fOne = ((int)inAvgTempF - (fHund * 100) - (fTen * 10));

    if (fHund == 0)
    {
        fArray[0] = 0x20;
    }
    if (fHund >= 1)
    {
        fArray[0] = fHund + 0x30;
    }

    fArray[1] = fTen + 0x30;
    fArray[2] = fOne + 0x30;
    fArray[3] = 0x2E;
    fArray[4] = (inAvgTempF - (int)inAvgTempF) * 10 + 0x30;

    int mod = timer_cnt % 12;

    if (mod < 9 && mod > 5)
    {
        if (mod == 6)
        {
            Graphics_clearDisplay(&g_sContext);
        }
        Graphics_drawStringCentered(&g_sContext, cArray, AUTO_STRING_LENGTH, 44, 55, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "C", AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
    }
    if (mod < 12 && mod > 8)
    {
        if (mod == 9)
        {
            Graphics_clearDisplay(&g_sContext);
        }
        Graphics_drawStringCentered(&g_sContext, fArray, AUTO_STRING_LENGTH, 44, 70, OPAQUE_TEXT);
        Graphics_drawStringCentered(&g_sContext, "F", AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
        Graphics_flushBuffer(&g_sContext);
    }
}

void startTimerA2()
{
    // Use ACLK, 16 Bit, up mode, 1 divider
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 32767; // 32767+1 = 32768 ACLK tics = 1 second
    TA2CCTL0 = CCIE;
}

void stopTimerA2(int reset)
{
    TA2CTL = MC_0;     // stop timer
    TA2CCTL0 &= ~CCIE; // TA2CCR0 interrupt disabled

    if (reset)
        timer_cnt = 0;
}

void configADC12()
{
    bits30 = CALADC12_15V_30C;
    bits85 = CALADC12_15V_85C;

    REFCTL0 &= ~REFMSTR; // Reset REFMSTR to hand over control of
    // internal reference voltages to
    // ADC12_A control registers

    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON; // Internal ref = 1.5V

    ADC12CTL1 = ADC12SHP; // Enable sample timer

    // Using ADC12MEM0 to store reading
    ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10; // ADC i/p ch A10 = temp sense
    // ACD12SREF_1 = internal ref = 1.5v

    __delay_cycles(100);   // delay to allow Ref to settle
    ADC12CTL0 |= ADC12ENC; // Enable conversion
}

void getTemp()
{
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;

    degC_per_bit = ((float)(85.0 - 30.0)) / ((float)(bits85 - bits30));

    while (ADC12CTL1 & ADC12BUSY)
    {
        __no_operation();
    }

    in_temp = ADC12MEM0 & 0x0FFF;
    temperatureDegC = (float)((long)in_temp - CALADC12_15V_30C) * degC_per_bit + 30.0;
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
    ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0; // ADC12INCH0 = Scroll wheel = A0
                                            // ACD12SREF_0 = Vref+ = Vcc
    __delay_cycles(100);                    // delay to allow Ref to settle
    ADC12CTL0 |= ADC12ENC;                  // Enable conversion
}

unsigned int getScroll()
{

    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC;

    while (ADC12CTL1 & ADC12BUSY)
    {
        __no_operation();
    }

    in_value = ADC12MEM0 & 0x0FFF; // Read results if conversion done

    return in_value;
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TimerA2_ISR()
{
    timer_cnt++;
}

void setMonths()
{

    slope = 12.0 / 4096.0;
    int monthValue;
    monthValue = slope * in_value; // returns a value between 0 and 11
    editMonth = monthValue;
    switch (monthValue)
    {
    case 0:
        monthArray[0] = 'J';
        monthArray[1] = 'A';
        monthArray[2] = 'N';
        break;
    case 1:
        monthArray[0] = 'F';
        monthArray[1] = 'E';
        monthArray[2] = 'B';
        break;
    case 2:
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'R';
        break;

    case 3:
        monthArray[0] = 'A';
        monthArray[1] = 'P';
        monthArray[2] = 'R';
        break;
    case 4:
        monthArray[0] = 'M';
        monthArray[1] = 'A';
        monthArray[2] = 'Y';
        break;
    case 5:
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'N';
        break;
    case 6:
        monthArray[0] = 'J';
        monthArray[1] = 'U';
        monthArray[2] = 'L';
        break;
    case 7:
        monthArray[0] = 'A';
        monthArray[1] = 'U';
        monthArray[2] = 'G';
        break;
    case 8:
        monthArray[0] = 'S';
        monthArray[1] = 'E';
        monthArray[2] = 'P';
        break;
    case 9:
        monthArray[0] = 'O';
        monthArray[1] = 'C';
        monthArray[2] = 'T';
        break;
    case 10:
        monthArray[0] = 'N';
        monthArray[1] = 'O';
        monthArray[2] = 'V';
        break;
    case 11:
        monthArray[0] = 'D';
        monthArray[1] = 'E';
        monthArray[2] = 'C';
        break;
    }
}

void setDays()
{
    float daySlope = 31.0; // 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    if (editMonth == 3 || editMonth == 5 || editMonth == 8 || editMonth == 10)
    {
        daySlope = 30.0;
    }
    else if (editMonth == 1)
    {
        daySlope = 28.0;
    }

    slope = daySlope / 4096.0; // different based on num of days in a month
    int dayValue = slope * in_value + 1;

    dayArray[0] = dayValue / 10 + 0x30;
    dayArray[1] = dayValue % 10 + 0x30;
    dayArray[2] = ' ';
}

void setHours()
{
    slope = 24.0 / 4096.0;
    int hourValue = slope * in_value; // a value between 0 and 23

    hourArray[0] = hourValue / 10 + 0x30;
    hourArray[1] = hourValue % 10 + 0x30;
    hourArray[2] = ' ';
}

void setMinutes()
{
    slope = 60.0 / 4096.0;
    int minuteValue = slope * in_value; // a value between 0 and 59

    minArray[0] = minuteValue / 10 + 0x30;
    minArray[1] = minuteValue % 10 + 0x30;
    minArray[2] = ' ';
}

void setSeconds()
{
    slope = 60.0 / 4096.0;
    int secondsValue = slope * in_value; // a value between 0 and 23

    secArray[0] = secondsValue / 10 + 0x30;
    secArray[1] = secondsValue % 10 + 0x30;
}

void lockTime()
{
    long fakeTime = 0.0; // new timer variable
    //
    long sec = (secArray[0] - 0x30) * 10.0 + (secArray[1] - 0x30);
    long min = (minArray[0] - 0x30) * 10.0 + (minArray[1] - 0x30);
    long hour = (hourArray[0] - 0x30) * 10.0 + (hourArray[1] - 0x30);
    long day = (dayArray[0] - 0x30) * 10.0 + (dayArray[1] - 0x30);

    switch (editMonth)
    {
    case 0:
        fakeTime = 0.0;
        break;
    case 1:
        fakeTime = 2678400.0;
        break;
    case 2:
        fakeTime = 5097600.0;
        break;
    case 3:
        fakeTime = 7776000.0;
        break;
    case 4:
        fakeTime = 10368000.0;
        break;
    case 5:
        fakeTime = 13046400.0;
        break;
    case 6:
        fakeTime = 15638400.0;
        break;
    case 7:
        fakeTime = 18316800.0;
        break;
    case 8:
        fakeTime = 20995200.0;
        break;
    case 9:
        fakeTime = 23587200.0;
        break;
    case 10:
        fakeTime = 26265600.0;
        break;
    case 11:
        fakeTime = 28857600.0;
        break;
    }

    fakeTime += sec + min * 60.0 + hour * 3600.0 + (day - 1) * 86400.0;
    timer_cnt = fakeTime;
}
