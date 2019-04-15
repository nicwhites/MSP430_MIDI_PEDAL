#include <msp430.h>
#include "WS2812Driver.h"



// WS2812 takes GRB format

typedef struct{

    uchar green;

    uchar red;

    uchar blue;

}LED;



static LED leds[NUM_LEDS] = { {0,0,0} };



// Initializes everything needed to use this library. This clears the strip.

void initStrip() {

    P1SEL |= OUTPUT_PIN;    // configure output pin as SPI output

    P1SEL2 |= OUTPUT_PIN;

    UCA0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, MSB, 8-bit SPI master

    UCA0CTL1 |= UCSSEL_2;   // SMCLK source (16 MHz)

    UCA0BR0 = 2;            // 16 MHz / 3 = .1875 us per bit

    UCA0BR1 = 0;

    UCA0CTL1 &= ~UCSWRST;   // Initialize USCI state machine

   clearStrip();           // clear the strip

}



// Sets the color of a certain LED (0 indexed)

void setLEDColor(uint z, uchar r, uchar g, uchar b) {

    leds[z].green = g;

    leds[z].red = r;

    leds[z].blue = b;

}

void setBrightness(uchar newBrightness)
{
    unsigned int i;

    for(i = NUM_LEDS; i > 0; i--)
    {

        leds[i].green = leds[i].green >> newBrightness;

        leds[i].red = leds[i].red >> newBrightness;

        leds[i].blue = leds[i].blue >> newBrightness;

    }



}

// Send colors to the strip and show them. Disables interrupts while processing.

void showStrip() {


    unsigned int i, j;

    for (i = 0; i < NUM_LEDS; i++) {

        uchar *rgb = (uchar *)&leds[i]; // get GRB color for this LED



        // send green, then red, then blue

        for (j = 0; j < 3; j++) {

            uchar mask = 0x80;    // b1000000



            // check each of the 8 bits

            while (mask != 0) {

                if (rgb[j] & mask) {        // most significant bit first

                    UCA0TXBUF = HIGH_CODE;  // send 1

                } else {

                    UCA0TXBUF = LOW_CODE;   // send 0

                }

                mask >>= 1;  // check next bit
            }

        }

    }

    _delay_cycles(800);

}



// Clear the color of all LEDs (make them black/off)

void clearStrip() {

    fillStrip(0x00, 0x00, 0x00);  // black

}



// Fill the strip with a solid color. This will update the strip.

void fillStrip(uchar r, uchar g, uchar b) {

    int i;

    for (i = 0; i < NUM_LEDS; i++) {

        setLEDColor(i, r, g, b);  // set all LEDs to specified color

    }

    showStrip();  // refresh strip

}
