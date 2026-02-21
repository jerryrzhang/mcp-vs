//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

//static function prototypes, functions only called in this file

int main(void)
{
  DDRC = 0xFF;//put PORTA into output mode
  PORTC = 0; 
  while(1)//main loop
  {
  //   if (forwards) {
  //     counter += 1;
  //   }
  //   else {
  //     counter -=1;
  //   }
  //   if (counter == 0) {
  //     forwards = true;
  //   }
  //   else if (counter == 8) {
  //     forwards = false;
  //   }
  //  //500 millisecond delay
  //   PORTA |= (1<<counter); 
  //       _delay_ms(500);  
  //   PORTA &= (1<<counter);
  //       _delay_ms(500);
    
    // PORTA = 0b10000000;
    //   _delay_ms(500);
    // PORTA = 0b01000000;
    //   _delay_ms(500);
    for(int i = 0; i <= 7; i++) {
      PORTC = (1 << i);
        _delay_ms(500);
    };
    for(int i = 7; i >= 0; i--) {
      PORTC = (1 << i);
        _delay_ms(500);
    };

  }
 return(1);
}//end main 