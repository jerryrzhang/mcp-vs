//Eample ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"

#define MOVE_SPEED 50
#define TURN_SPEED 1

uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

static int16_t lm = 0;
static int16_t rm = 0;
bool automatic_mode = false;
char serialString[60];
char serialString1[60];

uint16_t compValue = 1500;

uint8_t receivedData[6];

//static function prototypes, functions only called in this file

void init_serial(void) {
  serial2_init();
  serial0_init();
}

void init_servo(void) {
  TCCR1A = (1<<COM1A1);
  TCCR1B = (1<<WGM13) | (1<<CS11);
  ICR1 = 20000;
  PORTB = 0xFF;
  DDRB |= (1<<PB5);
}

void init_motors(void) {
  TCCR3A = (1<<COM3A1) | (1<<COM3B1); //clear rising set falling
  
  TCCR3B = (1<<WGM33) | (1<<CS31); //mode 8 prescaler 8

  ICR3 = 8000; //max
  PORTA = 0xFF; //portA output
  DDRA |= (1<<DDA0)|(1<<DDA1)|(1<<DDA2)|(1<<DDA3); //put A0-A3 into low impedance output mode
  PORTE = 0xFF; //portE output
  DDRE = (1<<PE3) | (1<<PE4); //these are outputs for pwm ocr3A and ocr3b
}



void init_all(void) {
  init_serial();
  adc_init();
  milliseconds_init();
  init_motors();
  init_servo();
  _delay_ms(10);
  automatic_mode = true;
  
}

int* calculate_data()
{
  static int calculatedData[6];

  int voltage;
  int light;
  calculatedData[0] = read_battery();

  return calculatedData;
}

void send_data()
{
  int* calculatedData;
  calculatedData = calculate_data();

  // checksum = calculatedData[0] + calculatedData[1];
  serial2_write_bytes(3, calculatedData[0], calculatedData[1]);
  last_send_ms = current_ms;
    
}

void receive_data()
{
  
  serial2_get_data(receivedData,5); 
  sprintf(serialString,"\nData 1: %3u, Data2: %3u", receivedData[0],receivedData[1]); 
  sprintf(serialString,"\nData 2: %3u, Data3: %3u", receivedData[2],receivedData[3]); 

  automatic_mode = receivedData[5];
  serial0_print_string(serialString); 
  
}

void move_motors(fc, rc) {

    // lm goes from 0 - 104
    // lm goes from 0 - 206
    lm = fc + rc - 208;
    rm = fc - rc;

    OCR3A = (int32_t)abs(lm)*8000/104; //lm speed from magnitude of lm
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

int sensor_distance1()
{
  int distance;
  distance = adc_read(0); // add calibration

  return distance;
}

int sensor_distance2()
{
  int distance;
  distance = adc_read(1); // add calibration

  return distance;
}

int sensor_distance3()
{
  int distance;
  distance = adc_read(2); // add calibration

  return distance;
}

void run_motors_auto()
{
  int left_distance = sensor_distance1();
  int front_distance = sensor_distance2();
  int right_distance = sensor_distance3();
  int turn_direction = 0;
  
  if (left_distance <= 10) // too close
  {
    // save_left
    move_motors(103 + (MOVE_SPEED * 1 * 1), 103 + (TURN_SPEED * 1 * 0.4));
  }
  else if (right_distance <= 10) // too close
  {
    // save_right
    move_motors(103 + (MOVE_SPEED * 1 * 1), 103 + (TURN_SPEED * -1 * 0.4));
  }
  else if (turn_direction)
  {
    // turn left
    move_motors(103 + (MOVE_SPEED * 1 * 0), 103 + (TURN_SPEED * turn_direction * 0.6));

    if (front_distance > 30) // far enough away from the wall meaning it has turned enough 
    {
      turn_direction = 0;
    }
  }
  else if (front_distance <= 10) // too close
  {
    // determine turn

    if (left_distance > right_distance) {
      turn_direction = -1;
    }
    else {
      turn_direction = 1;
    }

  }
  else
  {
    //move forward since everything is fine
    move_motors(103 + (MOVE_SPEED * 1 * 1), 103 + (TURN_SPEED * 0));
  }
}

int read_battery()
{
  char voltageString[50];
  int voltage = adc_read(0)*5*1.68/1.024 - 200;
  sprintf(voltageString,"voltage: %d mV\n",voltage);
  serial0_print_string(voltageString);

  return voltage;
}

int read_frequency()
{
  static int time_started = 0;
  static int changes;
  static bool previous_light;
  bool current_light;

  int light;
  light = adc_read(3);


  if (light > 500) // a high light value indicating that its on
  {
    current_light = 1;
  }
  else
  {
    current_light = 0;
  }

  if (time_started == 0)
  {
    time_started = milliseconds_now();
    changes = 0;
    current_light = 0;
    previous_light = 0;
  }

  if (milliseconds_now() - time_started < 10000); // 10 seconds
    
    if (current_light != previous_light)
    {
      changes += 1;
    }
    
    previous_light = current_light;
}

int main(void)
{

  init_all();
  //main function initialization
  uint8_t databyte1 = 55;
  uint8_t databyte2 = 80;

  
  uint8_t fc = 150;
  uint8_t rc = 150;

  while(1)
  {

    // --- servo calibration ---

    sprintf(serialString, "%d %d %d", sensor_distance1(), sensor_distance2(), sensor_distance3());
    serial0_print_string(serialString);

    // --- end servo calibration ---

    current_ms = milliseconds_now();
    
    if( (current_ms-last_send_ms) >= 10)
    {
      send_data();
    }

    if(serial2_available()) {
      receive_data();

        fc = 210 - receivedData[0];
        rc = receivedData[1];

        OCR1A = (receivedData[2] - 2) * 5 + 1000;
      //}
    }
    if (automatic_mode == true)
    {
      run_motors_auto();
    }
    else
    {
      set_motors_manual(fc,rc);
    }
    //read_battery();
  }


 return(1);
}//end main
