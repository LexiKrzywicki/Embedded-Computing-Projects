#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

long unsigned int inTime;
int seconds, minutes, hours, oldDays, days, months;
float inAvgTempC, inAvgTempF;
unsigned char monthArray[], dayArray[], hourArray[], minArray[], secArray[], cArray[], fArray[];

void displayTime(long unsigned int inTime);
void displayTemp(float inAvgTempC);

int main()
{
    WDTCTL = WDTPW | WDTHOLD;

    configDisplay();

    Graphics_clearDisplay(&g_sContext);
    displayTime(14769420);
    displayTemp(24.7);
    Graphics_flushBuffer(&g_sContext);
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
    dayArray[2] = 0x20; // to get rid of "."

    Graphics_drawStringCentered(&g_sContext, monthArray, AUTO_STRING_LENGTH, 39, 25, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, dayArray, AUTO_STRING_LENGTH, 59, 25, OPAQUE_TEXT);

    Graphics_drawStringCentered(&g_sContext, hourArray, AUTO_STRING_LENGTH, 33, 40, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, minArray, AUTO_STRING_LENGTH, 50, 40, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, secArray, AUTO_STRING_LENGTH, 68, 40, OPAQUE_TEXT);
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

    Graphics_drawStringCentered(&g_sContext, cArray, AUTO_STRING_LENGTH, 44, 55, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "C", AUTO_STRING_LENGTH, 64, 55, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, fArray, AUTO_STRING_LENGTH, 44, 70, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, "F", AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
}