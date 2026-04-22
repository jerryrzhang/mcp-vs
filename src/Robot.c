//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"


uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

static int16_t lm = 0;
static int16_t rm = 0;

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

  
  uint8_t fc = 150;
  uint8_t rc = 150;


  TCCR3A = (1<<COM3A1) | (1<<COM3B1);
  
  TCCR3B = (1<<WGM33) | (1<<CS31);

  ICR3 = 8000;
  PORTA = 0xFF;
  DDRA |= (1<<DDA0)|(1<<DDA1)|(1<<DDA2)|(1<<DDA3); //put A0-A3 into low impedance output mode
  PORTE = 0xFF;
  DDRE = (1<<PE3) | (1<<PE4);

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

        fc = 210 - receivedData[0];
        rc = receivedData[1];

      //}
    }

    lm = fc + rc - 208;
    rm = fc - rc;

    OCR3A = (int32_t)abs(lm)*8000/104; //lm speed from magnitude of lm
    sprintf(serialString, "%d %d %d %d\n", lm, rm, fc, rc);
    serial0_print_string(serialString);
    OCR3B = (int32_t)abs(rm)*8000/104; //lm speed from magnitude of rm

    if(lm>=0) //if lm is positive
    {
      //set direction forwards
      PORTA |= (1<<PA0);
      PORTA &= ~(1<<PA1);
    }
    else
    {
      //set direction reverse
      PORTA &= ~(1<<PA0);
      PORTA |= (1<<PA1);
    }

    if(rm>=0) //if rm is positive
    {
      //set direction forwards
      PORTA |= (1<<PA2);
      PORTA &= ~(1<<PA3);
    }
    else
    {
      //set direction reverse
      PORTA &= ~(1<<PA2);
      PORTA |= (1<<PA3);
    }

  }


 return(1);
}//end main