//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

//static function prototypes, functions only called in this file

int main(void)
{
  DDRA = 0; //put PORTA into input mode
  PORTA = 0;
  PORTA |= (1<<PA7); // enable internal pullup PA7

  DDRC = 0xFF; //portc into output
  PORTC = 0; //clear leds

  bool horizontal = true;
  bool pressed = false;
  adc_init();
  _delay_ms(20);

  uint16_t adcVal = 0;
  while(1)
  {
    if (PINA & (1<<PA2)) {
      if (~pressed){
        horizontal = ~horizontal;
        pressed = true;
      }
    } else {
      if (pressed) {
        pressed = false;
      }
    }


    if (horizontal) {
      adcVal = adc_read(3); //returns a 10bit value on voltage at ADC3 from 0 - 1023
    } else {
      adcVal = adc_read(4);
    }

    //  changing this into 8 leds we get
    // 0-127-255-383-511-639-767-895-1023
    adcVal = adcVal / 128; //divides and truncates decimal
    uint8_t lookup[] = {
      0b11110000,
      0b01110000,
      0b00110000,
      0b00010000,
      0b00001000,
      0b00001100,
      0b00001110,
      0b00001111
    };

    PORTC = lookup[adcVal];
  }
 return(1);
}//end main 