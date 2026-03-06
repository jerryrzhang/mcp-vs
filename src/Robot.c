//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

//static function prototypes, functions only called in this file

volatile uint32_t value = 0;
uint8_t falling = 0;
uint32_t last_time = 0;
char serialString[60] = {};
uint32_t last_time_interrupt = 0;

int main(void)
{
  DDRD &= ~(1<<PD2);
  PORTD = 0;


  
  cli();

  EICRA |= (1<<ISC21);
  EICRA &= ~(1<<ISC20);
  EIMSK |= (1<<INT2);

  sei();

  serial0_init();
  milliseconds_init();
  while(1)
  {
    if ((milliseconds_now()-last_time)>1000) {
      last_time = milliseconds_now();
      sprintf(serialString, "\nFalling Edges in the last second = %u", falling);
      serial0_print_string(serialString);
      falling = 0;
    }
  }

 return(1);
}//end main


ISR(INT2_vect) {
  //if ((milliseconds_now()-last_time_interrupt) > 20) {
  //  last_time_interrupt = milliseconds_now();
    falling += 1;
  //}
}
