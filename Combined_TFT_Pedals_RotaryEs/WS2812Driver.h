
/*
 *
 * AUTHOR: NIC WHITES
 *
 * \\\\\\\\\\\\\_____/\\\______________/\\\___/\\\\____________/\\\\_
_\/\\\/////////\\\__\/\\\_____________\/\\\__\/\\\\\\________/\\\\\\_
__\/\\\_______\/\\\__\/\\\_____________\/\\\__\/\\\//\\\____/\\\//\\\_
___\/\\\\\\\\\\\\\\___\//\\\____/\\\____/\\\___\/\\\\///\\\/\\\/_\/\\\_
____\/\\\/////////\\\___\//\\\__/\\\\\__/\\\____\/\\\__\///\\\/___\/\\\_
_____\/\\\_______\/\\\____\//\\\/\\\/\\\/\\\_____\/\\\____\///_____\/\\\_
______\/\\\_______\/\\\_____\//\\\\\\//\\\\\______\/\\\_____________\/\\\_
_______\/\\\\\\\\\\\\\/_______\//\\\__\//\\\_______\/\\\_____________\/\\\_
________\/////////////__________\///____\///________\///______________\///__

 * MIDI PEDAL CONTROL PROGRAM USING A TFT, AN LED STRIP, PUSH BUTTONS, AND ROTARY ENCODERS
 *
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\PURPOSE\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * The purpose of this program is to utilize a foot pedal midi control system in order to
 *  allow for easier transitions between tracks by artists
 *  Also, to get a kickass grade for our CPEN senior design project
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\UPDATES\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * --------------------------------------------------------------------------------------------------------
 *\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 */

#include "typedef.h"

#define OUTPUT_PIN  BIT2;  // Set to whatever UCA0SIMO is on your processor (Px.7 here)
#define NUM_LEDS    (42)    // NUMBER OF LEDS IN YOUR STRIP




// Transmit codes

#define HIGH_CODE   (0xF0)      // b11110000

#define LOW_CODE    (0xC0)      // b11000000



// Configure processor to output to data strip

void initStrip(void);



// Send colors to the strip and show them. Disables interrupts while processing.

void showStrip(void);



// Set the color of a certain LED

void setLEDColor(uint z, uchar r, uchar g, uchar b);



// Clear the color of all LEDs (make them black/off)

void clearStrip(void);

void setBrightness(uchar newBrightness);

// Fill the strip with a solid color. This will update the strip.

void fillStrip(uchar r, uchar g, uchar b);
