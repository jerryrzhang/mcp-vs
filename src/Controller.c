//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

//static function prototypes, functions only called in this file

int main(void)
{
  DDRA = 0xFF;//put PORTA into output mode
  PORTA = 0; 
  bool forwards = true;
  int counter = 0;
  while(1)//main loop
  {
    if (forwards) {
      counter += 1;
    }
    else {
      counter -=1;
    }
    if (counter == 0) {
      forwards = true;
    }
    else if (counter == 8) {
      forwards = false;
    }

    _delay_ms(500);     //500 millisecond delay
    PORTA |= (1<<counter); 
  }
 return(1);
}//end main 