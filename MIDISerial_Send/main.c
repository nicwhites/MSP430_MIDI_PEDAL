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


*------------------------------------------------------------------------------------
* The purpose of this library is for a UART send of a MIDI Control Command Output.
* When a button is pressed correspondin to the assigned pins in the const_data.h file,
* a MIDI signal is sent via UART.
*
* With the integration of this library with a second MSP430, there is a sync line on P1.5
* This is used instead of having to implement a Master/slave SPI system to ensure data correctness
*
* -----------------------------------------------------------------------------------------
*
*

 */


#define totalTracks 21
volatile unsigned char received_note = 0;
unsigned int i;


#include <msp430.h>
#include "const_data.h"

void send_MIDI(unsigned char buttonPressed);
void send_UART(char key);
void up();
void down();

const unsigned char pedal_array = (pedal_1 | pedal_2 | pedal_Arm | pedal_Up | pedal_Down);
unsigned int currentTrack[3] = {1,2,127};

char status[totalTracks];
int main(void)
{
    WDTCTL=WDTPW+WDTHOLD;

    //________________________________________________________________________________________
    //
    // CLOCK INIT
    //________________________________________________________________________________________
    //---------------------------------------------------------------------------------------//

    if (CALBC1_1MHZ==0xFF)   // If calibration constant erased
       {
          while(1);          // do not load, trap CPU!!
       }

    /*
     * CLK SETUP AND WATCHDOG DISABLED
     */
    BCSCTL1=CALBC1_16MHZ;       // SMCLK=~16MHz
    DCOCTL=CALDCO_16MHZ;
    CCTL0 = CCIE;               // CCR0 interrupt enabled

    /*
     * BUTTON ENABLES AND INTERRUPT ATTACH
     *
     */
              P2DIR &= ~(pedal_array);
              P2IE = (pedal_array);
              P2REN |= (pedal_array);
              P2IES = (pedal_array);
              P2IFG &= ~(pedal_array);

    //
    // UART RXD and TXD CONFIG FOR PINS 1.1 and 1.2
    //________________________________________________________________________________________
    //---------------------------------------------------------------------------------------//

    P1SEL  |=  BIT1 + BIT2;  // P1.1 UCA0RXD input
    P1SEL2 |=  BIT1 + BIT2;  // P1.2 UCA0TXD output

    //________________________________________________________________________________________
    //
    // UART(USCI_A0) CONFIG
    //________________________________________________________________________________________
    //---------------------------------------------------------------------------------------//


    UCA0CTL1 |= UCSSEL_2 + UCSWRST;               // UART Clock -> SMCLK

    UCA0BR0 = 134;                      // Baud Rate Setting for 1MHz 9600

    UCA0BR1 = 0;                        // Baud Rate Setting for 1MHz 9600

    UCA0MCTL = UCBRS_7;                 // Modulation Setting for 1MHz 9600

    UCA0CTL1 &= ~UCSWRST;               // Initialize UART Module



    //________________________________________________________________________________________
    //
    // ISR CONFIG
    //________________________________________________________________________________________
    //---------------------------------------------------------------------------------------//


    IE2 = UCB0TXIE;                     // enable TX interrupt
    IFG2 &= ~UCB0TXIFG;



while(1){
    _BIS_SR(GIE + CPUOFF);
}

}



//________________________________                      _________________________________|
//________________________________  BUTTON INTERRUPTS   _________________________________|
//________________________________                      _________________________________|

#pragma vector = PORT2_VECTOR
__interrupt void PORT2(void) {
    P2IE &= ~(pedal_array);
int flag = P2IFG&pedal_array;

    switch(flag){

    case pedal_1: send_MIDI(pedal_1);            // Transmit a byte

                    break;// clears P1.3 interrupt flag
    case pedal_2: send_MIDI(pedal_2); // toggle P1.6 LED output

                     break;                 // clears P1.4 interrupt flag
    case pedal_Arm:  send_MIDI(pedal_Arm);                  // toggle P2.5 LED output

                      break;
    case pedal_Up: up();
                  break;
    case pedal_Down: down();
                     break;
    }
    // loop to sync
    while(P1IN&BIT5);


    P2IFG &= ~pedal_array;
    P2IE = (pedal_array);

}

/*
 * MIDI Packaging
 */
void send_MIDI(unsigned char buttonPressed)
{

    unsigned int buttonPressedIdx; // used to find which pedal was pressed

    /*
     * Arm pedal press routine
     */
    if(buttonPressed&pedal_Arm)
    {
        buttonPressedIdx = 2;
        status[totalTracks - 1] ^= 0x01;

    }

    /*
     * Track pedal press routine
     */
    else
    {


     buttonPressedIdx = (buttonPressed - 0x01); // index for array
    status[currentTrack[buttonPressedIdx]] ^= currentTrack[buttonPressedIdx]; // XOR with itself in order to turn on/off

    }

    //Send Routine
   send_UART(0xBf); // CC MIDI
   send_UART(currentTrack[buttonPressedIdx]); // MIDI NOTE
   send_UART(0x45); // VELOCITY




}

/*
 * Up Pedal Pressed
 */
void up()
{

       if(currentTrack[0] < (totalTracks - 2))
       {
           currentTrack[0]++;
           currentTrack[1]++;

       }
       else{}



}

/*
 * Down Pedal Pressed
 */
void down(){

    if(currentTrack[0] > 0)
            {
              currentTrack[0]--;
              currentTrack[1]--;

            }
                else{}

}

/*
 * Send out UART Command
 */

void send_UART(char key)
{
    while(!(IFG2&UCA0TXIFG));
        UCA0TXBUF = key;
}

