//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

//static function prototypes, functions only called in this file

char serialString[60];
char serialString1[60];
char lcd_string[60];
uint8_t recievedData[2];

uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

uint8_t databyte1 = 10;
uint8_t databyte2 = 200;
uint16_t checksum = 0;
uint16_t adcHorizontal = 0;
uint16_t adcVertical = 0;

uint16_t data = 0;

//static function prototypes, functions only called in this file

int main(void)
{

  char lcd_string[33] = {0}; //declare and initialise string for LCD

	//initialisation section, runs once
	lcd_init(); //initialise 

  serial0_init();
  serial2_init();
  milliseconds_init();
  adc_init();
  while (1)
  {
    current_ms = milliseconds_now();

    if((current_ms - last_send_ms) >= 10) {
      adcHorizontal = adc_read(0);
      adcVertical = adc_read(1);

      databyte1 = adcHorizontal/5+2; //2-206
      databyte2 = adcVertical/5+2;


      checksum = databyte1 + databyte2;
      serial2_write_bytes(3, databyte1, databyte2, checksum);
      last_send_ms = current_ms;
    }


    if(serial2_available())
    {
      serial2_get_data(recievedData,2); 
      sprintf(serialString,"\nData 1: %3u, Data2: %3u", recievedData[0],recievedData[1]); 
      serial0_print_string(serialString); 
      
      lcd_clrscr();
      lcd_home();       // same as lcd_goto(0);
      lcd_puts( "Distance" ); //Print string to LCD first line
      lcd_goto( 0x40 );     //Put cursor to first character on second line


      data = 3000/( (recievedData[0]-2) * 3 + 20 ) - 1.5;
      sprintf( lcd_string , "%dcm" , data); 
      //print to string, %u special character to be replaced by variables in later arguments
      lcd_puts( lcd_string ); //Print string to LCD second line, same as first line
      //%u for unsigned integers, %i,%d for signed integers
      //%lu for long unsigned ...
    }
  }
}

