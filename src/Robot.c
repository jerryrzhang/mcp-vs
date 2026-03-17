//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"


int milliseconds1 = 0;
int seconds1 = 0;
int minutes1 = 0;
bool stopped = false;
int debounce = 0;
bool ignore = false;

bool stopped2 = false;
int debounce2 = 0;
bool ignore2 = false;

char serialString[60];

//static function prototypes, functions only called in this file

int main(void)
{
  DDRD = 0;
  PORTD = (1 << PD2) | (1 << PD3);

  adc_init();
  _delay_ms(20);

  uint16_t adcVal = 0;

  serial0_init();

  TCCR1A = 0;

  TCCR1B = (1<<CS12) | (1<<3); // 00000100 prescaler of 256 
   


  TIMSK1 = (1<<OCIE1A);

  OCR1A = 624; //compare register
  
  EICRA &= ~(1<<ISC21);
  EICRA |= (1<<ISC20);
  EIMSK |= (1<<INT2);

  EIMSK |= (1<<INT3);
  EICRA &= ~(1<<ISC31);
  EICRA |= (1<<ISC30);


  sei();

  while(1) {

  }
  


 return(1);
}//end main


ISR(TIMER1_COMPA_vect)
{
  debounce -= 1;
  debounce2 -= 1;
  if (!stopped) {
    milliseconds1 += 10;
  }
  if (milliseconds1 == 1000) {
    milliseconds1 = 0;
    seconds1 += 1;
  }
  if (seconds1 == 60) {
    seconds1 = 0;
    minutes1 += 1;
  }
  sprintf(serialString, "Time: %02d:%02d:%03d\n", minutes1, seconds1, milliseconds1);
  serial0_print_string(serialString);
  return;
}

ISR(INT2_vect) {
  if (debounce <= 0) {
    if (!ignore) {
      stopped = !stopped;
      ignore = true;
    } else {
      ignore = false;
    }
    debounce = 3;
  }
}

ISR(INT3_vect) {
  if (debounce2 <= 0) {
    if (!ignore2) {
      milliseconds1 = 0;
      seconds1 = 0;
      minutes1 = 0;
      ignore2 = true;
    } else {
      ignore2 = false;
    }
    debounce2 = 3;
  }
}