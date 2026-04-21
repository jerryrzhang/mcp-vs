//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"


uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

char serialString[60];
char serialString1[60];

  uint8_t receivedData[2];

//static function prototypes, functions only called in this file

int main(void)
{
//main function initialization
  serial2_init();
  serial1_init();
  serial0_init();
  adc_init();
  milliseconds_init();
  _delay_ms(10);
  uint8_t databyte1 = 55;
  uint8_t databyte2 = 80;
  uint16_t horiz = 0;
  uint16_t adcVal = 0;


  TCCR1A = (1<<COM1A1);
  TCCR1B = (1<<WGM13) | (1<<CS11);
  ICR1 = 20000;
  PORTB = 0xFF;
  DDRB |= (1<<PB5);
  uint16_t compValue = 1500;

  while(1)
  {

    current_ms = milliseconds_now();
    
    if( (current_ms-last_send_ms) >= 10)
    {
      // sprintf(serialString, "%d\n", adc_read(0));
      // serial0_print_string(serialString);

      databyte1 = adc_read(0)/3 +2;
      serial2_write_bytes(2, databyte1, databyte2); 

      last_send_ms = current_ms;
    }

    if(serial2_available()) {
      serial2_get_data(receivedData, 3);
      //if (receivedData[2] == receivedData[0] + receivedData[1]){

        horiz = (receivedData[0] -2) * 5;
        OCR1A = horiz + 1000;
      //}
    }
  }


 return(1);
}//end main