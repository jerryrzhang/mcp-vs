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
  DDRA = 0xFF;
  PORTA = 0;

  adc_init();
  _delay_ms(20);

  uint16_t adcVal = 0;

  serial0_init();

  
  while(1) {
    // PORTA |= (1);
    // _delay_ms(1000);

    // PORTA %= (1);
    // _delay_ms(1000);

    uint8_t adc_raw = adc_read(0)/8;
    uint16_t distance_lut[] = {
      2048, //8
      1024, //16
      682, //24
      512, //32
      410, //40
      341, //48
      292, //56
      256, //64
      228, //72
      205, //80
      186, //88
      170, //96
      157, //104
      146, //112
      137, //120
      128, //128
      121, //136
      114, //144
      108, //152
      102, //160
      97, //168
      92, //176
      88, //184
      85, //192
      81, //200
      79, //208
      76, //216
      74, //224
      71, //232
      69, //240
      67, //248
      65, //256
      63, //264
      61, //272
      60, //280
      58, //288
      57, //296
      55, //304
      54, //312
      53, //320
      52, //328
      50,  //336
      512, //344
      410, //352
      341, //360
      292, //368
      256, //376
      228, //384
      205, //392
      186, //400
      170, //408
      157, //416
      146, //424
      137, //432
      128, //440
      121, //448
      114, //456
      108, //464
      102, //472
      97, //480
      92, //488
      88, //496
      85, //504
      81  //512
    };

    adcVal = 3000/(adc_read(0)+20)-1.5;


    if (adcVal > 50) {
      adcVal = 50;
    }

    sprintf(serialString, "\n Distance? %ucm", adcVal);
    serial0_print_string(serialString);
    _delay_ms(100);

    // if(adcVal > 250) {
    //   PORTA |= 1;
    // } else {
    //   PORTA &= ~(1);
    // }
  }
  


 return(1);
}//end main


ISR(INT2_vect) {
  //if ((milliseconds_now()-last_time_interrupt) > 20) {
  //  last_time_interrupt = milliseconds_now();
    falling += 1;
  //}
}
