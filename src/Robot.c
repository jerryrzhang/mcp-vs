//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

//static function prototypes, functions only called in this file

int main(void)
{
  DDRA = 0; //put PORTA into input mode
  DDRA |= (1<<PA3);
  PORTA = 0;
  PORTA |= (1<<PA7); // enable internal pullup PA7

  adc_init();
  _delay_ms(20);

  uint16_t adcVal = 0;

  while(1)
  {
    if (!(PINA & (1<<PA7))) //PA7 low / button pressed
    {
      PORTA |= (1<<PA3);
    }
    else
    {
      PORTA &= ~(1<<PA3);
    }
    adcVal = adc_read(3); //returns a 10bit value on voltage at ADC3


  }
 return(1);
}//end main 