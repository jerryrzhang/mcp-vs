//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

//PLEASE PLEASE PLEASE CHANGE THESE
#define JOYSTICK2_Y 14 
#define JOYSTICK2_X 15


//static function prototypes, functions only called in this file

char serialString[60];
char serialString1[60];
char lcd_string[60];
uint8_t receivedData[3];

uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

uint8_t databyte1 = 10;
uint8_t databyte2 = 200;
uint16_t checksum = 0;
uint16_t adcHorizontal = 0;
uint16_t adcVertical = 0;
int adcHorizontal2 = 0;
uint16_t adcVertical2 = 0;
uint32_t last_time_interrupt;

int grabber = 0;


bool automatic_mode = true;
bool precision_mode = false;

uint16_t data = 0;

//static function prototypes, functions only called in this file
void init_serial()
{
  serial0_init();
  serial2_init();
  milliseconds_init(); // makes serial work...
}

void init_timer()
{
  // pass controller has no timer
}

void initialise()
{
  init_serial();
  init_timer();
  adc_init();
  lcd_init();

  //init int1
  EICRA |= (1<<ISC11);
  EICRA &= ~(1<<ISC10);
  EIMSK |= (1<<INT1);

  //init int2
  EICRA |= (1<<ISC21);
  EICRA &= ~(1<<ISC20);
  EIMSK |= (1<<INT2);

}

int* calculate_data()
{
  static int calculatedData[6];

  adcHorizontal = adc_read(JOYSTICK2_X)/5 + 2;
  adcVertical = adc_read(JOYSTICK2_Y)/5 + 2;

  adcHorizontal2 = adc_read(0)/5 + 2;
  adcVertical2 = adc_read(1)/5 + 2;

  calculatedData[0] = adcHorizontal; // motor 
  calculatedData[1] = adcVertical;
  // instead makes it such that it will open and close based on the joystick and will not automaticall return
  
  grabber = grabber - ((adcHorizontal2 - 103) / 50);
  if (grabber >= 206)
  {
    grabber = 204;
  }
  else if (grabber <= 4)
  {
    grabber = 5;
  }
  calculatedData[3] = 0;

  calculatedData[2] = grabber;
  
  return calculatedData;
}

void send_data()
{
  int* calculatedData;
  calculatedData = calculate_data();

  checksum = calculatedData[0] + calculatedData[1];
  serial2_write_bytes(6, calculatedData[0], calculatedData[1], calculatedData[2], calculatedData[3], automatic_mode, precision_mode);
  last_send_ms = current_ms;

  sprintf(serialString1, "%d, %d\n", adc_read(0)/5 + 2, calculatedData[2]);
  serial0_print_string(serialString1);
    
}

void lcd_display(uint16_t data[])
{
  char lcd_string[33] = {0};//declare and initialise string for LCD
  lcd_clrscr();
  lcd_home();       // same as lcd_goto(0);

  sprintf( lcd_string , "%04dmV %03ddHz" , data[0], data[2]);

  lcd_puts( lcd_string ); //Print string to LCD second line, same as first line
  lcd_goto( 0x40 ); 

  if (automatic_mode) {
    lcd_puts("Automatic Mode");
  } else {
    lcd_puts("Manual Mode");
  }



  //print to string, %u special character to be replaced by variables in later arguments
  //%u for unsigned integers, %i,%d for signed integers
  //%lu for long unsigned ...

}

void receive_data()
{
  static uint16_t data[3];
  serial2_get_data(receivedData,3); 
  sprintf(serialString,"\nData 1: %3u, Data2: %3u", receivedData[0],receivedData[1]); 
  serial0_print_string(serialString); 

  data[0] = (receivedData[0]-2) * 80;
  data[1] = receivedData[1];
  data[2] = receivedData[2] * 2;
  lcd_display(data);
}

int main(void)
{


	//initialisation section, runs once
  initialise();


  while (1)
  {
    current_ms = milliseconds_now();

    if((current_ms - last_send_ms) >= 10) 
    {
      send_data();
    }


    if(serial2_available())
    {
      receive_data();
    }
  }
}


ISR(INT1_vect) {
  if ((milliseconds_now()-last_time_interrupt) > 20) {
   last_time_interrupt = milliseconds_now();
    if (automatic_mode == true) { 
      automatic_mode = false;
    } else { 
      automatic_mode = true;
     }
  }
}

ISR(INT2_vect) {
  if ((milliseconds_now()-last_time_interrupt) > 20) {
   last_time_interrupt = milliseconds_now();
    if (precision_mode == true) { 
      precision_mode = false;
    } else { 
      precision_mode = true;
     }
  }
}