// Team 5
// Lab 4

#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include "peripherals.h"

int i = 0;
bool clear = false, climb, high = true;

float slope;
unsigned int dac_Codes[17] = {0, 256, 512, 768, 1024, 1280, 1536, 1792, 2048, 2304, 2560, 2816, 3072, 3328, 3584, 3840, 4095, 4351, 4607}; // scroll vals
unsigned char button;
unsigned int in_value, codeValue = 0, adc_scrollWheel = 0;
long unsigned int indexSquare = 0, timer_cnt = 0, leap_cnt = 0, last_cnt = 0, triInterrupt, volts_code, timer_tri = 0;
long double floatVoltage = 0.0;
unsigned long timer = 0;

void startTimerA2();
void stopTimerA2(int reset);
void configScroll();
void setDAC(unsigned int dac_code);
void DACInit();
void delay(int delayTime);
unsigned int getScroll();
long double configADC12();
int getButton();

#pragma vector = TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void)
{

    if (leap_cnt < 12)
    { // 43 for 0.0005
        timer_cnt++;
        leap_cnt++;
    }

    else
    {
        leap_cnt = 0;
    }
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void)
{
    codeValue = ADC12MEM1 & 0x0FFF;
    adc_scrollWheel = ADC12MEM0 & 0x0FFF;
}

enum STATE
{
    WELCOME,
    BUTTON,
    DC,
    SQUARE,
    SAWTOOTH,
    TRIANGLE
};

enum STATE state = WELCOME;

int main()
{
    volatile float degCPerBit;

    WDTCTL = WDTPW | WDTHOLD;
    REFCTL0 &= ~REFMSTR;

    configDisplay();
    configKeypad();
    configButton();

    _BIS_SR(GIE);

    DACInit();
    configADC12();
    startTimerA2();

    while (1)
    {
        while(1){


        long double voltage;
        unsigned int codeValue2;

        voltage = configADC12();
        codeValue2 = ADC12MEM1 & 0x0FFF;
        setDAC(adc_scrollWheel);
        }

        switch (state)
        {
        case WELCOME:

            if (clear == false) // flag for clearing display
            {
                Graphics_clearDisplay(&g_sContext);
                clear = true;
            }
            Graphics_drawStringCentered(&g_sContext, "Wave Generator", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Press button:", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "1 = DC", AUTO_STRING_LENGTH, 20, 45, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "2 = SQUARE WAVE", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "3 = SAWTOOTH", AUTO_STRING_LENGTH, 40, 65, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "4 = TRIANGLE", AUTO_STRING_LENGTH, 40, 75, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            state = BUTTON;
            break;

        case BUTTON:

            switch (getButton())
            {
            case 1:
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "DC", AUTO_STRING_LENGTH, 48, 15, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                state = DC;
                button = 0;
                break;
            case 2:
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "SQUARE WAVE", AUTO_STRING_LENGTH, 48, 15, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                state = SQUARE;
                button = 0;
                break;
            case 3:
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "SAWTOOTH", AUTO_STRING_LENGTH, 48, 15, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                state = SAWTOOTH;
                button = 0;
                break;
            case 4:
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "TRIANGLE", AUTO_STRING_LENGTH, 48, 15, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                state = TRIANGLE;
                button = 0;
                break;
            }
            break;

        case DC:
            if (getButton() == 0)
            {

                // setDAC(adc_scrollWheel);
                // configADC12();

            }
            break;

        case SQUARE: // 100 Hz
            if (getButton() == 0)
            {
                if (high == true)
                {
                    configADC12();
                    setDAC(adc_scrollWheel); // high
                    high = false;
                }
                else if (high == false)
                {
                    setDAC(0); // low
                    high = true;
                }

                last_cnt = timer_cnt;
                while (timer_cnt < last_cnt + 6)
                {
                    __no_operation();
                }
            }
            break;

        case SAWTOOTH: // 75Hz
            if (getButton() == 0)
            {
                volts_code = (timer_cnt % 133) * 62;

                setDAC(volts_code);
            }
            break;

        case TRIANGLE: // 150Hz

            if (getButton() == 0)
            {

                for (i = 0; i <= 16; i++)
                {
                    setDAC(dac_Codes[i]);
                    triInterrupt = timer_cnt;
                    while (timer_cnt < triInterrupt + 1)

                        __no_operation();
                }
                for (i = 15; i >= 0; i--)
                {
                    setDAC(dac_Codes[i]);
                    triInterrupt = timer_cnt;
                    while (timer_cnt < triInterrupt + 1 )
                        __no_operation();
                }
            }
            break;
        }
    }
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
        clear = false;
        state = WELCOME;
        button = 0;
    }
    else if ((P7IN & BIT0) == 0) // S1
    {
        button = 1;
    }
    else if ((P3IN & BIT6) == 0) // S2
    {
        button = 2;
    }
    else if ((P2IN & BIT2) == 0) // S3
    {
        button = 3;
    }
    else if ((P7IN & BIT4) == 0) // S4
    {
        button = 4;
    }
    else
    {
        button = 0;
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

void startTimerA2()
{
    TA2CTL = TASSEL_2 + MC_1 + ID_0; // SMCLK
    TA2CCR0 = 190;
    TA2CCTL0 = CCIE;
}

void stopTimerA2(int reset)
{
    TA2CTL = MC_0;     // stop timer
    TA2CCTL0 &= ~CCIE; // TA2CCR0 interrupt disabled

    if (reset)
        timer_cnt = 0;
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

void DACInit()
{
    DAC_PORT_LDAC_SEL &= ~DAC_PIN_LDAC;
    DAC_PORT_LDAC_DIR |= DAC_PIN_LDAC;
    DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC; // Deassert LDAC
    DAC_PORT_CS_SEL &= ~DAC_PIN_CS;
    DAC_PORT_CS_DIR |= DAC_PIN_CS;
    DAC_PORT_CS_OUT |= DAC_PIN_CS; // Deassert CS
}

void setDAC(unsigned int dac_code)
{
    DAC_PORT_CS_OUT &= ~DAC_PIN_CS;

    dac_code |= 0x3000; // Add control bits to DAC word
    uint8_t lo_byte = (unsigned char)(dac_code & 0x00FF);
    uint8_t hi_byte = (unsigned char)((dac_code & 0xFF00) >> 8);
    DAC_SPI_REG_TXBUF = hi_byte;

    while (!(DAC_SPI_REG_IFG & UCTXIFG))
    {
        _no_operation();
    }

    DAC_SPI_REG_TXBUF = lo_byte;

    while (!(DAC_SPI_REG_IFG & UCTXIFG))
    {
        _no_operation();
    }

    DAC_PORT_CS_OUT |= DAC_PIN_CS;

    DAC_PORT_LDAC_OUT &= ~DAC_PIN_LDAC; // Assert LDAC
    __delay_cycles(10);
    DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC; // De-assert LDAC
}

long double configADC12()
{
    REFCTL0 &= ~REFMSTR;
    ADC12CTL0 = ADC12SHT0_9 | ADC12ON | ADC12MSC;

    ADC12CTL1 = ADC12SHP + ADC12CONSEQ_1;
    ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0;
    ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_1 + ADC12EOS;
    __delay_cycles(100);
    P6SEL = P6SEL | (BIT1);

    ADC12IE = BIT1;
    _BIS_SR(GIE);
    ADC12CTL0 &= ~ADC12SC;
    ADC12CTL0 |= ADC12SC + ADC12ENC;
//    floatVoltage = ((double)(((long)codeValue) / 4095.0));
//    floatVoltage = ((double)(floatVoltage * ((long)3.3)));

    floatVoltage = (double)(((long)codeValue)/4095.0) * (long)3.3 + (long)3.3;
    return floatVoltage;
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
