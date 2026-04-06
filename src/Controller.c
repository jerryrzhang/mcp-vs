//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

char serialString[60];
char serialString1[60];
uint8_t recievedData[2];

uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

char serialString[60];

//static function prototypes, functions only called in this file

int main(void)
{
//main function initialization
  serial0_init();
  serial1_init();
  milliseconds_init();
  _delay_ms(20);

  while(1)
  {
    //main loop
    current_ms = milliseconds_now();
    
    //sending section
    if( (current_ms-last_send_ms) >= 500) //sending rate controlled here
    {
      //Arbitrary process to update databytes
    //   databyte1++;
    //   databyte2+=2;
      
      //Function takes the number of bytes to send followed by the databytes as arguments
      //serial2_write_bytes(2, databyte1, databyte2); 
      sprintf(serialString, "Data: %d\n", serial1_available());
      serial0_print_string(serialString);
    //   serial0_print_string("written\n");
      last_send_ms = current_ms;
    }
  }


 return(1);
}//end main