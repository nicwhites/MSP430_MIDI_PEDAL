





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
 * VERSION NUMBER: .1
 * 3/21/2019: Library written and compiled with use of; 1 TFT
 *                                                      3 Push buttons
 *                                                      42 LEDS
 * --------------------------------------------------------------------------------------------------------
 * VERSION NUMBER : .2
 * 4/1/2019: Setup up new functions for Up and Down buttons along with making the printing of the TFT commands more
 *          efficient with less of a lag time. Noticed speed increased significantly compared to previous version
 *          which had a myriad of multiplication fuctions that only needed to be done once instead of n times.
 *
 *
 * --------------------------------------------------------------------------------------------------------
 *\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 */


// Includes...
#include <msp430.h> 
#include "typedef.h"
#include "init_display.h"
#include "lib_lcd.h"
#include "delay.h"
#include "const_data.h"
#include "WS2812Driver.h"

///////// Interrupt Function Calls //////////////////////////
void logo();
void pedalPress(uint n, struct TrackPedals * p);
void up(struct TrackPedals *p);
void down(struct TrackPedals *p);
void arm(struct TrackPedals *p);
void UPDATE();


//---Definitions------//
#define menuTextSize (6)

const int totalTracks = 20;


////////// Variables ///////////
uchar space_char=6;     // space between chars
unsigned int x,y,f;
unsigned char pedalText = 12;
unsigned char pedal_array = (pedal_1 | pedal_2 | pedal_Arm | pedal_Up | pedal_Down);
uchar state, oldstate, i;
struct TrackPedals Pedals;




/*
 * Main Function
 */
void main(void) {

    /*
     * CLK SETUP AND WATCHDOG DISABLED
     */
    WDTCTL=WDTPW+WDTHOLD;
    BCSCTL1=CALBC1_16MHZ;       // SMCLK=~16MHz
    DCOCTL=CALDCO_16MHZ;
    CCTL0 = CCIE;               // CCR0 interrupt enabled

/*
 * Setup of pedals
 */

        Pedals.interAttach[0] = pedal_1;
        Pedals.interAttach[1] = pedal_2;
        Pedals.interAttach[2] = pedal_Arm;

        Pedals.currentTrack[0] = 0;
        Pedals.currentTrack[1] = 1;

        Pedals.currentMode[0] = 0;
        Pedals.currentMode[1] = 0;
        Pedals.currentMode[2] = 0;


        Pedals.offset[0] = 0;
        Pedals.offset[1] = 14;
        Pedals.offset[2] = 28;




          if (CALBC1_16MHZ==0xFF)    // If calibration constant erased
          {

              while(1);              // do not load, trap CPU!!

          }

/*
* TFT INITIALIZATION
*/

          init_USCI();                // init. USCI (SPI)
          init_LCD(C_BLACK);          // init. Display

/*
 *
 * LED STRIP INITIALIZATION
 *
 */
              initStrip();

/*
 * Logo Animation
 */
   wait_ms(1000);
   fillStrip(0x00,0x0c,0x00);
   logo();
   wait_ms(2000);

/*
 * Screen Setup
 */

  space_char = 5;
  fill_display(lcd_height,lcd_width,C_BLACK);
  fillStrip(0x00,0x00,0x4f);
  UPDATE();
  //wait_ms(1000);



   /*
    * BUTTON ENABLES AND INTERRUPT ATTACH
    *
    */
             P2DIR = ~(pedal_array);
             P2IE = (pedal_array);
             P2REN |= (pedal_array);
             P2IES = (pedal_array);
             P2IFG &= ~(pedal_array);

/*
 * LPM W/INTERRUPTS ENABLED
 */

    while(1)
    {
        _BIS_SR(CPUOFF + GIE);
    }

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * INTERRUPT VECTOR PORT_2
 */

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{


    P2IE &= ~(pedal_array);

    // Sync Line on
    P2OUT |= BIT5;
    if(P2IFG&(pedal_Up))
        up(&Pedals);
    else if(P2IFG&pedal_Down)
        down(&Pedals);
    else if(P2IFG&pedal_Arm)
        arm(&Pedals);
    else
    pedalPress(NUM_LEDS, &Pedals);  // set Lights based on interrupt input


    // Update LED and TFT
    UPDATE();

    // Sync Line off
    P2OUT ^= BIT5;


    P2IFG &= ~(pedal_array);
    P2IE = (pedal_array );

}


/*
 * LOGO FUNCTION
 */

void logo(void)
{


    x = 0;
    y = 0;
   int x_offset = 0;
   int count = 0;
    while (y < 240)
    {
        while(count < 61)
            {
                draw_char((count++ + x_offset),(y),logoText[x++],C_GREEN,TRANSP,0);
                x_offset += 5;
                if(y < 120) wait_us(800);
            }
        if(y > 120 && y < 200)
        {
               drawString(3,100,introText,set_RGB16(y-120,y-120,y-120),TRANSP,3);

         }
        else if(y > 200)
        {
            drawString(3,100,introText,set_RGB16(0,0,0),TRANSP,3);
        }


        x_offset = 0;
        count = 0;
        y+=8;
    }


    x = 0;

  while(x < 31){

    drawString(3,100,introText,set_RGB16(x,(x+x),x),TRANSP,3);
    x+= 3;

     }

}





/*
 * PEDAL PRESS INTERRUPT FUNCTION
 */

void pedalPress(uint n, struct TrackPedals * p)
{


    for( i = 0; i < 2; i++)
    {
        if((p->interAttach[i])&P2IFG)
        {
              int currentMode = p->currentMode[p->currentTrack[i]];

              switch(currentMode){
              case 0x00:p->currentMode[p->currentTrack[i]] = 0x01; break;
              case 0x01:p->currentMode[p->currentTrack[i]] = 0x00; break;
              case 0x02:p->currentMode[p->currentTrack[i]] = 0x03; break;
              case 0x03:p->currentMode[p->currentTrack[i]] = 0x03; break;

              }
       }
      }


}


/*
 * Arm Function
 */

void arm(struct TrackPedals *p)
{
    uchar none_Armed = 1; // used to turn Arm strip blue if there are no armed tracks

    for (i = 0; i < totalTracks; i++){

             switch(p->currentMode[i])
             {
             case 1:p->currentMode[i] = 2;
                    p->ARMode = 2;
                    none_Armed = 0;
                    break;
             case 2: none_Armed = 0;break;
             case 3:  p->currentMode[i] = 0; break;

             }

     }

         for(f = Pedals.offset[2]; f < (Pedals.offset[2] + LEDS_PER_PEDAL); f++)
         {
             if(none_Armed)
             {
                 setLEDColor(f,0x00,0x00,0x4f);
             }
             else
                 setLEDColor(f,modeColors[p->ARMode][0],modeColors[p->ARMode][1],modeColors[p->ARMode][2]);
        }
     }


/*
 * Up Pedal Pressed
 */
void up(struct TrackPedals * p)
{

       if(p->currentTrack[0] < (totalTracks - 2))
       {
           p->currentTrack[0]++;
           p->currentTrack[1]++;

       }
       else{}



}

/*
 * Down Pedal Pressed
 */
void down(struct TrackPedals *p){

    if(p->currentTrack[0] > 0)
            {
              p->currentTrack[0]--;
              p->currentTrack[1]--;

            }
                else{}

}

/*
 * LED and TFT Update command
 */


void UPDATE()
{
    // Update LED Strip around Track Pedals
    for(i = 0; i < 2; i++)
    {
               for(f = Pedals.offset[i]; f < (Pedals.offset[i] + LEDS_PER_PEDAL); f++)
               {
                   if((f >= (Pedals.offset[i] + LEDS_PER_PEDAL - 2)) && (Pedals.currentMode[Pedals.currentTrack[i]] == 0x03))
                   {
                       setLEDColor(f,0x00,0x4f,0x00);
                   }
                   else
                   {
                   setLEDColor(f,modeColors[Pedals.currentMode[Pedals.currentTrack[i]]][0],modeColors[Pedals.currentMode[Pedals.currentTrack[i]]][1],modeColors[Pedals.currentMode[Pedals.currentTrack[i]]][2]);
                   }

               }
    }
    showStrip();

    //Update TFT
    for(i = 0; i < 2; i++)
    {

     drawString(numberOffset_x[i], numberOffset_y,tracks[Pedals.currentTrack[i]], C_WHITE, trackBackgroundColor[Pedals.currentMode[Pedals.currentTrack[i]]], pedalText);
    }

}



