//Example ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Controller.h"

#define JOYSTICK2_Y_PIN 14 
#define JOYSTICK2_X_PIN 15

#define JOYSTICK1_Y_PIN 1 
#define JOYSTICK1_X_PIN 0

// init serial strings
char serialString[60];
char serialString1[60];
char lcd_string[60];

// init received data array
uint8_t receivedData[6];

// init timestamps
uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

//init misc
uint8_t databyte1 = 10;
uint8_t databyte2 = 200;
uint16_t checksum = 0;
uint16_t adcHorizontal = 0;
uint16_t adcVertical = 0;
int adcHorizontal2 = 0;
uint16_t adcVertical2 = 0;
uint32_t last_time_interrupt;

int grabber = 0;

//init precision and auto modes
bool automatic_mode = true;
bool precision_mode = false;

uint16_t data = 0;

//static function prototypes, functions only called in this file
void init_serial()
{
  //init serial functions
  serial0_init();
  serial2_init();
  milliseconds_init(); // the powerful and all knowing milliseconds_init()
}

void initialise()
{
  //group initialize
  init_serial();
  adc_init();
  lcd_init();

  sei();

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

  // pull adc joystick data
  adcHorizontal = adc_read(JOYSTICK2_X_PIN)/5 + 2;
  adcVertical = adc_read(JOYSTICK2_Y_PIN)/5 + 2;

  adcHorizontal2 = adc_read(JOYSTICK1_X_PIN)/5 + 2;
  adcVertical2 = adc_read(JOYSTICK1_Y_PIN)/5 + 2;

  // push motor data into output array
  calculatedData[0] = adcHorizontal; // motor 
  calculatedData[1] = adcVertical;

  // grabber variable allows joysticks to adjust an absolute position
  
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

  //write all bytes to serial
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

  sprintf( lcd_string , "%04dmV %03ddHz" , data[0], data[2]); // show mv and dHz on serial

  lcd_puts( lcd_string ); //Print string to LCD second line, same as first line
  lcd_goto( 0x40 ); 

  //if voltage is too low, show a warning
  if (data[0] < 7000) {
    lcd_puts("LOW VOLTAGE");
  } else {
    //otherwise show the mode
    if (automatic_mode) {
      lcd_puts("Automatic Mode");
    } else {
      lcd_puts("Manual Mode");
    }
  }

  //print debug sensor and light values
  printf("light: %d left dist: %dmm front dist: %dmm right dist: %dmm",data[1],data[3],data[4],data[5]);

}

void receive_data()
{
  static uint16_t data[6];

  //receive data using serial library
  serial2_get_data(receivedData,6); 
  //sprintf(serialString,"\nData 1: %3u, Data2: %3u", receivedData[0],receivedData[1]); 
  //serial0_print_string(serialString); 

  //put received data into parsed data
  data[0] = (receivedData[0]-2) * 80; // voltage
  data[1] = (receivedData[1] - 2) * 5; // light
  data[2] = receivedData[2] * 2; // frequency
  data[3] = receivedData[3] * 4; // left distance
  data[4] = receivedData[4] * 4; // front distance
  data[5] = receivedData[5] * 4; // right distance
  lcd_display(data);
}

int main(void)
{


	//initialisation section, runs once
  initialise();


  // main while loop
  while (1)
  {
    //set the current time
    current_ms = milliseconds_now();

    //every 10ms, send new data
    if((current_ms - last_send_ms) >= 10) 
    {
      send_data();
    }

    //if we have received data, read it 
    if(serial2_available())
    {
      receive_data();
    }
  }
}

//debounce 1
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

//debounce 2
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