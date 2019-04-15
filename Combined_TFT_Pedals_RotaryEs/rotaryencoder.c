
#include <rotaryencoder.h>


void encoderInit(){

	P2OUT |= (ENCODER_A+ENCODER_B);	//enable pull-up resistor
	P2REN |= ENCODER_A+ENCODER_B;	//enable pull-up resistor
	P2IFG &= ~(ENCODER_A + ENCODER_B);			//clear interrupt flag
	P2IE |= (ENCODER_A + ENCODER_B);    			//enable interrupt for encoder

}

void stepCCW(){


       fillStrip(0xff,0x00,0x00);

}


void stepCW(){

    fillStrip(0x00,0x00,0xff);

}

