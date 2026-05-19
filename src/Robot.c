//Eample ATmega2560 Project
//File: ATmega2560Project.c
//An example file for second year mechatronics project

//include this .c file's header file
#include "Robot.h"

// macros for values that may need to be changed depending on the scenario
#define MOVE_SPEED 40
#define TURN_SPEED 50
#define LIGHT_THRESHOLD 450
#define RECORDING_PERIOD 8

// global variables
uint32_t current_ms = 0;
uint32_t last_send_ms = 0;

uint32_t time_started;

static int16_t lm = 0;
static int16_t rm = 0;
bool automatic_mode = true;
bool precision_mode = false;
char serialString[60];
char serialString1[60];
uint32_t lastread;

uint16_t compValue = 1500;

uint8_t receivedData[6];
int changes; // for light 
int frequency = 0; // for light
int turn_direction = 0;
bool reading_light = false;
bool moving = true;

//static function prototypes, functions only called in this file

// initalisation functions
void init_serial(void) {
  serial2_init();
  serial0_init();
}

void init_servo(void) {
  TCCR1A = (1<<COM1A1);
  TCCR1B = (1<<WGM13) | (1<<CS11);  // mode 8, prescaler 8
  ICR1 = 20000; // servo range from 0 - 20000
  PORTB = 0xFF; // port B output
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


// calls the init functions and init functions from outside this program
void init_all(void) {
  init_serial();
  adc_init();
  milliseconds_init();
  init_motors();
  init_servo();
  _delay_ms(10);
  automatic_mode = true;
  
}

// calculates data to be sent to controller
int* calculate_data()
{
  static int calculatedData[6];

  int voltage;

  // calculates the highest light value
  int light;
  int right_light;
  right_light = adc_read(4);
  light = adc_read(5);
  if (right_light > light)
  {
    light = right_light;
  }


  calculatedData[0] = read_battery(); // voltage
  calculatedData[1] = (light / 5) + 2; // highest light
  calculatedData[2] = frequency; // frequency of light

  // makes sure the distances dont go over the limit for serial communication
  if (sensor_distance1() > 1000)
  {
    calculatedData[3] = 250;
  }
  else
  {
    calculatedData[3] = sensor_distance1() / 4; // left distance
  }
  
  if (sensor_distance2() > 1000)
  {
    calculatedData[4] = 250;
  }
  else
  {
    calculatedData[4] = sensor_distance2() / 4; // front distance
  }

  if (sensor_distance3() > 1000)
  {
    calculatedData[5] = 250;
  }
  else
  {
    calculatedData[5] = sensor_distance3() / 4; // right distance
  }

  return calculatedData;
}

// sends the data to controller
void send_data()
{
  int* calculatedData;
  // gets the calculated data to send
  calculatedData = calculate_data();

  serial2_write_bytes(6, calculatedData[0],calculatedData[1],calculatedData[2],calculatedData[3],calculatedData[4],calculatedData[5]);

  last_send_ms = current_ms; // updates the time so it will wait 10ms to send data again
    
}

// recieves the data from controller
void receive_data()
{
  
  serial2_get_data(receivedData,6); 
  //sprintf(serialString,"\nData 1: %3u, Data2: %3u", receivedData[0],receivedData[1]); 
  //sprintf(serialString,"\nData 2: %3u, Data3: %3u", receivedData[2],receivedData[3]); 

  // sets the mode information it got from the controller
  automatic_mode = receivedData[4];
  precision_mode = receivedData[5];
  
  //serial0_print_string(serialString); 
  
}

// function that sends signals to the h bridge based on the fc and rc inputs
void move_motors(int fc, int rc) {

    // lm goes from (-206) to 206 technically but when moving in an orthogonal direction it only goes to 104
    lm = fc + rc - 208;
    rm = fc - rc;

    // sets it from 0 - 100 when it gets to 104 setting the usuable joystick area to a diamond
    OCR3A = (int32_t)abs(lm)*8000/104; //lm speed from magnitude of lm
    //serial0_print_string(serialString);
    OCR3B = (int32_t)abs(rm)*8000/104; //rm speed from magnitude of rm

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

// calculates the distance for the left sensor 
int sensor_distance1()
{
  int distance;
  distance = 41000/(adc_read(1) + 45) - 30; // add calibration

  return distance;
}

// calculates the distance for the front sensor 
int sensor_distance2()
{
  int distance;
  distance = 90000/(adc_read(2) + 50) - 70; // add calibration

  return distance;
}

// calculates the distance for the right sensor 
int sensor_distance3()
{
  int distance;
  distance = 38000/(adc_read(3) + 40) - 30; // add calibration

  return distance;
}

// algorithm that decides which way the robot moves
void run_motors_auto()
{
  // gets the distances for each sensor
  int left_distance = sensor_distance1();
  int front_distance = sensor_distance2();
  int right_distance = sensor_distance3();
  
  // the robot is turning it will do an on the spot turn in that direction
  if (turn_direction != 0)
  {
    // turn left or right depending on turn_direction
    move_motors(103, 103 + (TURN_SPEED * turn_direction * 2));

    if (front_distance > 100) // far enough away from the wall meaning it has turned enough so it will stop turning
    {
      turn_direction = 0;
    }
  }
  else if (front_distance <= 80 && turn_direction == 0) // if it is to close to the wall in front it will determine turn direction
  {
    // determine turn with which side is further away
    // -1 for left, 1 for right
    if (left_distance > right_distance) {
      turn_direction = -1;
    }
    else {
      turn_direction = 1;
    }

  }
  else if (left_distance <= 90) // too close on the left so it veers right
  {
    // save_left
    move_motors(103 + (MOVE_SPEED * 1 * 1), 103 + (TURN_SPEED * -1 * 0.5));
  }
  else if (right_distance <= 90) // too close on the right so it veers left 
  {
    // save_right
    move_motors(103 + (MOVE_SPEED * 1 * 1), 103 + (TURN_SPEED * 1 * 0.5));
  }
  else
  {
    //if it isnt to close to any wall it will just move forward
    move_motors(103 + (MOVE_SPEED * 1 * 1), 103 + (TURN_SPEED * 0));
  }
}

// calculates the battery voltage
int read_battery()
{
  char voltageString[50];
  // voltage divider by 2.5 
  int voltage = (adc_read(0)*5*2.5/1.024)/80 + 2;
  sprintf(voltageString,"voltage: %d mV\n",voltage);
  //serial0_print_string(voltageString);

  return voltage;
}

// calculates the frequency of the light
void read_frequency()
{

  // sets up the light of the previous run of the function and the current
  static bool previous_light;
  bool current_light;

  // finds the highest light value
  int light;
  int right_light;
  right_light = adc_read(4);
  light = adc_read(5);
  if (right_light > light)
  {
    light = right_light;
  }


  if (light > LIGHT_THRESHOLD) // if the light is high enough the current light is 1 else its 0 
  {
    current_light = 1;
  }
  else
  {
    current_light = 0;
  }

  // potentially redundant
  if (time_started == 0 || milliseconds_now() - time_started > 15000) //15 seconds gives time for it to get away from the light
  {
    serial0_print_string("\nfrequency window reset\n");
    time_started = milliseconds_now();
    changes = 0;
    current_light = 0;
    previous_light = 0;
  }

  // if (recording_period) seconds have not passed it will check if the light was different to the previous light 
  if ((milliseconds_now() - time_started) < RECORDING_PERIOD * 1000)
  {
    if (current_light != previous_light)
    {
      // if it is different changes is incrementing howing that the state of the light changed
      changes += 1;
      //sprintf(serialString, "\ntransition: changes=%d light=%d", changes, light);
      //serial0_print_string(serialString);
    }
    // previous light is set to current light so the next loop runs correctly 
    previous_light = current_light;
  }
  else // if its been over (recording_period) seconds it will output the frequency of the light as it is done calculating
  {
    frequency = (changes * 5) / (2*RECORDING_PERIOD); // times by 10 to decihertz but divided by 2 again so it fits in sending data
    //sprintf(serialString, "\nMEASUREMENT DONE: changes=%d freq=%ddHz\n", changes, frequency * 2);
    //serial0_print_string(serialString);
    moving = true; // makes the robot start moving again
  }
  
}










int main(void)
{
  //main function initialization
  init_all();

  // redundant i think was test data 
  uint8_t databyte1 = 55;
  uint8_t databyte2 = 80;

  
  uint8_t fc = 103;
  uint8_t rc = 103;
  int right_light;
  int light;

  // main loop
  while(1)
  {
    // --- servo calibration ---

    //sprintf(serialString, "%d %d %d\n", sensor_distance1(), sensor_distance2(), sensor_distance3());
    //serial0_print_string(serialString);

    // --- end servo calibration ---

    // gets the time of current loop
    current_ms = milliseconds_now();
    
    // if 10ms have passed sends data to controller
    if( (current_ms-last_send_ms) >= 10)
    {
      send_data();
    }

    // if controller has sent data it will recieve it 
    if(serial2_available()) {
      receive_data();

        fc = 210 - receivedData[0]; // inverted since joystick was inverted
        rc = receivedData[1];

        // receiveddata[2] ranges from 2 - 206 
        // so 620 - 206 * 5 + 620
        // 620 -  of the 620 - 2420 us range for the sensor 
        OCR1A = (receivedData[2] - 2) * 5 + 620; // moves the grabber 

    }

    // finds the bigger light
    right_light = adc_read(4);
    light = adc_read(5);
    if (right_light > light)
    {
      light = right_light;
    }

    // test code 
    //if (milliseconds_now() - lastread > 1000) {
      //lastread = milliseconds_now();
      //sprintf(serialString, "\nlight: %d, moving: %d", light, moving);
      //serial0_print_string(serialString);
    //}

    // if the light is high enough 
    if (light > LIGHT_THRESHOLD + 40 && reading_light == false)
    {

      //serial0_print_string("\nBEACON DETECTED - stopping\n");

      // resets the frequency calculation and makes the robot stop moving 
      changes = 0;
      moving = false;
      reading_light = true;
      time_started = milliseconds_now();
    }
    

    // if 15 seconds have passed reading_light resets so the robot can detect another beacon
    if ((milliseconds_now() - time_started) > 15000)
    {
      reading_light = false;
    }

    // robot will move if not measuring a beacon
    if (moving == true)
    {

      // if the robot is in automatic mode it will run the automatic algorithm
      if (automatic_mode == true)
      {
        run_motors_auto();
        //serial0_print_string("\n MOVING")
      }
      else
      {
        // we probably wont use precision mode 
        if (precision_mode == true) {
          move_motors((fc - 103)/2 + 103, (rc - 103)/2 + 103);
        } else {
          move_motors(fc,rc); // directly moves motors based on joystick inputs
        }
      }
    }
    else // if it isnt moving it will read the light and turn the motors off since otherwise they keep going 
    {
      read_frequency();
      OCR3A = 0;
      OCR3B = 0;
    }
  }


 return(1);
}//end main
