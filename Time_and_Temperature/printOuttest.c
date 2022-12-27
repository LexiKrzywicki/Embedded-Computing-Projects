
#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

char charArray[3] = {"g", "h", "l"};

int main()
{
    WDTCTL = WDTPW | WDTHOLD;
    configDisplay();

    char value = charArray[1];

    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, value, AUTO_STRING_LENGTH, 48, 40, OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
}

//14.7psi = 1 atm
//1.0 KPa = 0.145 psi

unsigned int adcPressure; //define input pressure
double refV = 2.5;   //refV from 2)
double res = 0.12;   //res  from 2)
int bits = 16384; //2^14

double adcPressureToAtmospheres() //converts adcPressure to Atm
{
    //convert adc to pressure
    double volts = ((adcPressure / bits) * 2000); //mV
    double pressure = volts / (refV + res); //kPa

    //convert pressure to Atm
    pressure *= 0.145; //convert to psi (1.0 KPa = 0.145 psi)
    pressure /= 14.7;  //convert to Atm (14.7psi = 1 atm)

    return pressure;
}
