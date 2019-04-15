

#include <msp430g2553.h>
#include "typedef.h"
#include "ascii_char.h"
#include "lib_lcd.h"
#include "delay.h"


void drawString(sint x, sint y, const uchar *str, uint f_color, uint b_color, uchar fontSize)
{	

    int bgFillArea = y+ (f_height*(fontSize+1)) - 1;
    int x_spacing = (f_width * fontSize + space_char);
    int x_limit = -(sint)((f_width-1)*(fontSize+1));
    uchar bit_num;

	while(*str != '\0'  && x < lcd_width)
	{

		if(x >= x_limit)	 // part of char inside display
		{
		    // draw character(x position, y position, pointer to constant string, font color, background font color, size x font, size y font)
			draw_char(x,y,*str,f_color,b_color, fontSize);
			//if background color is not equal to transparent, paint it
			if(b_color!=TRANSP)
			{
			    // assign
				bit_num=space_char;
				while(bit_num--);	// draw a horizontal area (rect) in b_color
					fill_rect(x-space_char,x-1,y,bgFillArea,b_color);
			}
		}
		*str++;
		x+=x_spacing;
	}
}


void draw_char(sint x, sint y, uchar character, uint f_color, uint b_color, uchar fontSize)
{
	draw_bmap_8bit_v(x,y,ascii_table[character],f_width,f_height,f_color,b_color,fontSize);
}

void draw_bmap_8bit_v(sint x, sint y, const uchar *bmap, uint width, uint height, 
								uint f_color, uint b_color, uchar fontSize)
{
	sint x_off=0,y_off=0,x_index=0;

	uchar bit_num = 0;
	uint w_index = 0,h_index = 0;
	uint byte = height;

	// if out of bounds
	if(x>=lcd_width||y>=lcd_height) return;
	if(y+(sint)height<0) return;
	if(x+(sint)width<0) return;




	// setting the Y offset index position in bitmap
	byte=height;		// save real height

	if(byte%8) byte+=8;
	byte = byte >> 3;
	


	////////// iterating through the bitmap and putting a pixel at the location /////////////////////////////

	while(h_index<byte && x<lcd_width)
	{
	    int index_offset = ((h_index+y_off)*width);

		// vertical bit tester
		while(bit_num<8)
		{

			// horizontal pixel writing
			while(w_index < width && (x+x_index)<lcd_width)
			{




				// if bit is "1" write a pixel in foreground color
				if(bmap[w_index + index_offset] & (1<<bit_num))
					draw_pixel_xy(y,x+x_index,f_color, fontSize);



				// if bit is "0" write a pixel in background color
				else if(b_color!=TRANSP)
					draw_pixel_xy(y,x+x_index,b_color, fontSize);




				w_index++;
				x_index+=fontSize+1;
			}
			w_index=x_off;
			x_index=0;
			bit_num++;
			y+=fontSize+1;
		}



		h_index++;
		w_index=x_off;
		bit_num=0;
	}
}

void drawLine(sint x_center, sint y_center, uint color, uchar length, uchar fontSize)
{

    unsigned int f;

    draw_pixel_xy(y_center, x_center, color, fontSize);

    sint x_minus, x_plus;

    x_minus = x_center - 1;
    x_plus = x_center + 1;

    for(f = length; f > 0; f--)
    {
        draw_pixel_xy(y_center, x_minus, color, fontSize);
        draw_pixel_xy(y_center, x_plus, color, fontSize);
        x_minus -= 1;
        x_plus +=1;
    }



}

void fill_rect(sint x0, sint x1, sint y0, sint y1, uint color)
{

	if(y0>=lcd_height||x0>=lcd_width) return;	// overflow
	if((x0+x1)<0||(y0+y1)<0) return;			// outside display
	if(x0<0) x0=0;						
	if(y0<0) y0=0;
	if(x1>=lcd_width) x1=lcd_width;	
	if(y1>=lcd_height) y1=lcd_height;


	uint y_start=y0;
	while(x0<=x1)
	{
		set_cursor(x0++,y0);
		while(y0++<=y1)
			draw_pixel(color);
		y0=y_start;
	}
} 

void draw_pixel_xy(sint y, sint x, uint color, uchar fontSize)
{

	if(fontSize)
		fill_rect(x,x+fontSize,y,y+fontSize,color);
	else 
	{
		if(set_cursor(x,y))
			draw_pixel(color);
	}
}





uint set_RGB16(uchar R, uchar G, uchar B)
{
	return (uint)(R<<11)+(uint)(G<<5)+(uint)B;
}

void draw_pixel(uint color)
{
	set_instruction(1,color>>8);
	set_instruction(1,color&0xFF);	
}

uchar check_xy(sint x, sint y)
{
	if(x<0||x>=lcd_width||y<0||y>=lcd_height) return 0;
	return 1;
}

void fill_display(uint width, uint height, uint color)
{
	set_cursor(0,0);
	while(height--)
	{
		while(width--)
			draw_pixel(color);
		width=lcd_width;
	}
}

uchar set_cursor(sint x, sint y)
{
	if(!set_cursor_x(x)||!set_cursor_y(y)) return 0;
	return 1;
}

uchar set_cursor_x(sint x)
{
	if(x<0||x>=lcd_width) return 0;
	set_instruction(0,0x2B);
	set_instruction(1,x>>8);
	set_instruction(1,x&0xFF);
	MEM_WRITE;
	return 1;
}

uchar set_cursor_y(sint y)
{
	if(y<0||y>=lcd_height) return 0;
	set_instruction(0,0x2A);
	set_instruction(1,y>>8);
	set_instruction(1,y&0xFF);
	MEM_WRITE;
	return 1;
}

void set_instruction(uchar register_sel, uchar number)
{
	if(register_sel) 
		P1OUT|=DC;				// data
	else P1OUT&=~DC;			// command
	
	P1OUT&=~CS;					// start condition
	//while((IFG2 & UCB0TXIFG));	// TX buffer ready?
  	UCB0TXBUF=number;			// start transmission

}
