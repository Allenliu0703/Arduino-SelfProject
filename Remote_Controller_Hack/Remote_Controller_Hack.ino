#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

/* YourDuinoStarter Example: nRF24L01 Transmit Joystick values
 - WHAT IT DOES: Reads Analog values on A0, A1 and transmits
   them over a nRF24L01 Radio Link to another transceiver.
 - SEE the comments after "//" on each line below
 - CONNECTIONS: nRF24L01 Modules See:
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED
   - 
   Analog Joystick or two 10K potentiometers:
   GND to Arduino GND
   VCC to Arduino +5V
   X Pot to Arduino A0
   Y Pot to Arduino A1
   
 - V1.00 11/26/13
   Based on examples at http://www.bajdi.com/
   Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <SPI.h>

/*-----( Declare Constants and Pin Numbers )-----*/
#define button_L  2
#define button_R  3
#define CE_PIN   9
#define CSN_PIN 10
#define JOYSTICK_X A0
#define JOYSTICK_Y A1
#define JOYSTICK_U A2
#define JOYSTICK_V A3

// NOTE: the "LL" at the end of the constant is "LongLong" type
const uint64_t pipe [4] = {0xE8E8F0F0E1LL,0xF0F0F0F0B4LL,0xF0F0F0F0A2LL,0xF0F0F0F0A1LL}; // Define the transmit pipe


/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
short data[6] ={0,0,0,0,1,1};  // 2 element array holding Joystick readings
int previous_state[2]={1,1} ;
int button[2]={0,0};
int onoff_state;
void setup()   /****** SETUP: RUNS ONCE ******/
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe[1]);
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  onoff_state = digitalRead(button_L);
  delay(1);
    if(onoff_state != previous_state[0] ){
      if(onoff_state == 0){
        data[4] = !data[4];
      }
    }
    previous_state[0] = onoff_state;
    
  onoff_state = digitalRead(button_R);
  delay(1);
    if(onoff_state != previous_state[1] ){
      if(onoff_state == 0){
        data[5] = !data[5];
      }
    }
    previous_state[1] = onoff_state;
  
  data[0] = analogRead(JOYSTICK_X);
  data[1] = analogRead(JOYSTICK_Y);
  data[2] = analogRead(JOYSTICK_U);
  data[3] = analogRead(JOYSTICK_V);
  
  
  
  /*
  debounce(button_A,data[2],previous_state[0]);
  debounce(button_B,data[3],previous_state[1]);
  debounce(button_C,data[4],previous_state[2]);
  debounce(button_D,data[5],previous_state[3]);
  debounce(button_E,data[6],previous_state[4]);
  debounce(button_F,data[7],previous_state[5]);
  debounce(button_K,data[8],previous_state[6]);
  */
  

  data[4] = digitalRead(button_L);
  data[5] = digitalRead(button_R);
  
  radio.write( data, sizeof(data) );
  
  Serial.print("X = ");
  Serial.print(data[0]);
  Serial.print(" Y = ");      
  Serial.print(data[1]);
  Serial.print("U = ");
  Serial.print(data[2]);
  Serial.print(" V = ");      
  Serial.print(data[3]);
  Serial.print(" L = ");      
  Serial.print(data[4]);
  Serial.print(" R = ");      
  Serial.println(data[5]);
  
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/



