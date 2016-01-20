#include <IRremote.h>


/* ARDUINO MEGA 2560      4WD ROBOT CAR
  
  IR Sensor:      HX1838
        1 - VCC 5V!!!!
        2 - GND
        3 - DD  to Arduino Mega Pin 6  (Digital Pin)
        
  Ultra Sonic Module:    HC-SR04 
        1 - VCC 5V!!!!
        2 - Trig  to Arduino Mega Pin 30   (Digital Pin)
        3 - Echo  to Arduino Mega Pin 31   (Digital Pin)
        4 - GND
  
  DC motors :
        IN1 to Arduino Mega Pin 46    (Digital Pin)  //top left
        IN2 to Arduino Mega Pin 47    (Digital Pin)
        IN3 to Arduino Mega Pin 48    (Digital Pin)  //bot left
        IN4 to Arduino Mega Pin 49    (Digital Pin)
        IN5 to Arduino Mega Pin 42    (Digital Pin)  //top right
        IN6 to Arduino Mega Pin 43    (Digital Pin)
        IN7 to Arduino Mega Pin 44    (Digital Pin)  //bot right
        IN8 to Arduino Mega Pin 53    (Digital Pin)
        
        ENA_L to Arduino Mega Pin 5   (PWM Pin)
        ENB_L to Arduino Mega Pin 4   (PWM Pin)
        ENA_R to Arduino Mega Pin 8   (PWM Pin)
        ENB_R to Arduino Mega Pin 7   (PWM Pin)
        
   Servo:
        1 - GND
        2 - VCC 5V!!!!!
        3 - Signal to Arduino Mega Pin 8 (PWM Pin)
        
        
   IR Distance Sensors:
        Four Obstacle Check Sensors:
          1 - VCC 5V!!! (Blue)
          2 - GND (Green)
          3 - IR_Left to Arduino Mega Pin 18 (Yellow)
          4 - IR_Frontleft to Arduino Mega Pin 19 (Orange)
          5 - IR_Frontright to Arduino Mega Pin 20 (Red)
          6 - IR_Right to Arduino Mega Pin 21 (brown)
        
        Stairs Check Sensor:
          1 - VCC 5V!! (Red)
          2 - GND (Brown)
          3 - IR_Floor to Arduino Mega Pin 2 (Purple)
    
*/



#include <Servo.h>

Servo myservo;

#define IR_Sensoractive 1 //night mode!
 
#define RECV_PIN  6
#define my_Servo  38
#define ultra_Trig  30
#define ultra_Echo  31
#define motor_Leftfront1  46
#define motor_Leftfront2  47
#define motor_Leftback1  48
#define motor_Leftback2  49
#define motor_Rightfront1  42
#define motor_Rightfront2  43
#define motor_Rightback1  44
#define motor_Rightback2  53
#define motor_ENA_L  5
#define motor_ENB_L  4
#define motor_ENA_R  8
#define motor_ENB_R  7
#define IR_Left  18
#define IR_Frontleft  19
#define IR_Frontright  20
#define IR_Right  21
#define IR_Floor  2

int distance_Halfleft, distance_Left, distance_Right, distance_Front, distance_Halfright, distance_Largest;
int block_Halfleft, block_Left, block_Right, block_Halfright, floor_Check;
int counter = 0, power=1, modecontrol=0;
volatile int myspeed = 100;
const int distance_Limit = 27; //Distance limit for obstacles in front           
const int sidedistance_Limit = 12;
const int sidesidedistance_Limit = 9;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  Serial.begin(9600);
  
  myservo.attach(my_Servo);
  
  
  pinMode(motor_Leftfront1,OUTPUT);
  pinMode(motor_Leftfront2,OUTPUT);
  pinMode(motor_Leftback1,OUTPUT);
  pinMode(motor_Leftback2,OUTPUT);
  pinMode(motor_Rightfront1,OUTPUT);
  pinMode(motor_Rightfront2,OUTPUT);
  pinMode(motor_Rightback1,OUTPUT);
  pinMode(motor_Rightback2,OUTPUT);
  
  pinMode(ultra_Trig,OUTPUT);
  pinMode(ultra_Echo,INPUT);
  
  irrecv.enableIRIn(); // Start the receiver
  //block_Check();
  
  attachInterrupt(0, stair_Detect, FALLING);
 
  attachInterrupt(2, move_Left, FALLING);
  attachInterrupt(3, move_Left, FALLING);
  attachInterrupt(4, move_Right, FALLING);
  attachInterrupt(5, move_Right, FALLING);
  
}

void loop() {
  
  if (irrecv.decode(&results)){ //Check if the remote control is sending a signal
    if(results.value==0xEE886D7F){ //If an 'CH+' is received, turn on robot
      power=1; }
    if(results.value==0xE318261B){ //If a 'CH-' is received, turn off robot
      car_Freeze();
      power=0; }
    if(results.value==0x511DBB){ //If an 'CH' is received, switch operating mode from automatic robot to remote control (press also "*" to return to automatic robot mode)
      detachInterrupt(0);
      
      detachInterrupt(2);
      detachInterrupt(3);
      detachInterrupt(4);
      detachInterrupt(5);
      modecontrol=1; //  Activate remote control operating mode
      car_Freeze(); //The robot stops and starts responding to the user's directions
    }
    if (results.value== 0xD7E84B1B){ // '>>|'
      attachInterrupt(0, stair_Detect, FALLING);
 
      attachInterrupt(2, move_Left, FALLING);
      attachInterrupt(3, move_Left, FALLING);
      attachInterrupt(4, move_Right, FALLING);
      attachInterrupt(5, move_Right, FALLING);
      modecontrol=0;
      car_Freeze();
    }
    irrecv.resume(); // receive the next value
  }
  
  while(modecontrol==1&&power == 1){ //The system gets into this loop during the remote control mode until modecontrol=0 (with '*')
    if (irrecv.decode(&results)){ //If something is being received
      translateIR();//Do something depending on the signal received
      irrecv.resume(); // receive the next value
     }
  }
  if(power==1){
      block_Check();
      self_Drive();
  }
 
  /***
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    process_command(results.value);
    process_state();
    irrecv.resume(); // Receive the next value
  }
  ***/
}

void translateIR() { //Used when robot is switched to operate in remote control mode
  switch(results.value)
  {
    case 0x9716BE3F: // '1'
      break;
      
    case 0x3D9AE3F7: // '2'
      move_Forward(myspeed);
      break;
      
    case 0x6182021B: // '3'
      
      break;
      
    case 0x8C22657B: // '4'
      turn_Left(120);
      break;
      
    case 0x488F3CBB: // '5'
      car_Freeze();
      break;
      
    case 0x449E79F: // '6'
      turn_Right(120);
      break;
    case 0x32C6FDF7: // '7'
      
      break;
    case 0x1BC0157B: // '8'
      move_Backward(myspeed);
      
      break;
    case 0x3EC3FC1B: // '9'
     
      break;
    case 0xC101E57B: // '0'
     
      break;
    case 0xC97483BFB: // '100+'
      
      break;
    case 0xF0C41643: // '200+'
     
      break;
    case 0xE318261B: // 'CH-'
      power = 0;
      break;
    case 0x511DBB: // 'CH'
      detachInterrupt(0);
      
      detachInterrupt(2);
      detachInterrupt(3);
      detachInterrupt(4);
      detachInterrupt(5);
      modecontrol=1; //  Activate remote control operating mode
      car_Freeze(); //The robot stops and starts responding to the user's directions
      break;
    case 0xEE886D7F: // 'CH+'
      power = 1;
      break;
    case 0x52A3D41F: // '|<<'
      
    case 0xD7E84B1B: // '>>|'
      attachInterrupt(0, stair_Detect, FALLING);
 
      attachInterrupt(2, move_Left, FALLING);
      attachInterrupt(3, move_Left, FALLING);
      attachInterrupt(4, move_Right, FALLING);
      attachInterrupt(5, move_Right, FALLING);
      modecontrol=0;
      car_Freeze();
      break;
    case 0x20FE4DBB: // '>||'
     
      break;
    case 0xF076C13B: // '-'
      myspeed -= 50 ;
      if (myspeed >= 255){
        myspeed = 255;
      }else if (myspeed <= 0){
        myspeed = 0;
      }
      break;
    case 0xA3C8EDDB: // '+'
      myspeed += 50 ;
      if (myspeed >= 255){
        myspeed = 255;
      }else if (myspeed <= 0){
        myspeed = 0;
      }
      break;
    case 0xE5CFBD7F: // 'EQ'
     
      break;
    default:
      break;
      ;
    
  }// End Case
  delay(500); // Do not get immediate repeat
} 

/*** Motor Controls

  move right
  move forward
  move backward
  move left
  freeze


***/
void move_Backward ( int car_Speed) 
{
  digitalWrite ( motor_Leftfront1, LOW);
  digitalWrite ( motor_Leftfront2, HIGH);
  digitalWrite ( motor_Leftback1, LOW);
  digitalWrite ( motor_Leftback2, HIGH);
  digitalWrite ( motor_Rightfront1, LOW);
  digitalWrite ( motor_Rightfront2, HIGH);
  digitalWrite ( motor_Rightback1, LOW);
  digitalWrite ( motor_Rightback2, HIGH);
  
  if ( car_Speed > 255){
    car_Speed = 255;
  }else if ( car_Speed <0){
    car_Speed = 0;
  }else {
    analogWrite ( motor_ENA_L, car_Speed);
    analogWrite ( motor_ENB_L, car_Speed);
    analogWrite ( motor_ENA_R, car_Speed);
    analogWrite ( motor_ENB_R, car_Speed);
  }
}

void move_Forward ( int car_Speed) {
  digitalWrite ( motor_Leftfront1, HIGH);
  digitalWrite ( motor_Leftfront2, LOW);
  digitalWrite ( motor_Leftback1, HIGH);
  digitalWrite ( motor_Leftback2, LOW);
  digitalWrite ( motor_Rightfront1, HIGH);
  digitalWrite ( motor_Rightfront2, LOW);
  digitalWrite ( motor_Rightback1, HIGH);
  digitalWrite ( motor_Rightback2, LOW);
  
  if ( car_Speed > 255){
    car_Speed = 255;
  }else if ( car_Speed <0){
    car_Speed = 0;
  }else {
    analogWrite ( motor_ENA_L, car_Speed);
    analogWrite ( motor_ENB_L, car_Speed);
    analogWrite ( motor_ENA_R, car_Speed);
    analogWrite ( motor_ENB_R, car_Speed);
  }
}

void turn_Left ( int car_Speed) 
{
  digitalWrite ( motor_Leftfront1, LOW);
  digitalWrite ( motor_Leftfront2, HIGH);
  digitalWrite ( motor_Leftback1, LOW);
  digitalWrite ( motor_Leftback2, HIGH);
  digitalWrite ( motor_Rightfront1, HIGH);
  digitalWrite ( motor_Rightfront2, LOW);
  digitalWrite ( motor_Rightback1, HIGH);
  digitalWrite ( motor_Rightback2, LOW);
  
  if ( car_Speed > 255){
    car_Speed = 255;
  }else if ( car_Speed <0){
    car_Speed = 0;
  }else {
    analogWrite ( motor_ENA_L, car_Speed);
    analogWrite ( motor_ENB_L, car_Speed);
    analogWrite ( motor_ENA_R, car_Speed);
    analogWrite ( motor_ENB_R, car_Speed);
  }
}

void turn_Right ( int car_Speed) 
{
  digitalWrite ( motor_Leftfront1, HIGH);
  digitalWrite ( motor_Leftfront2, LOW);
  digitalWrite ( motor_Leftback1, HIGH);
  digitalWrite ( motor_Leftback2, LOW);
  digitalWrite ( motor_Rightfront1, LOW);
  digitalWrite ( motor_Rightfront2, HIGH);
  digitalWrite ( motor_Rightback1, LOW);
  digitalWrite ( motor_Rightback2, HIGH);
  
  if ( car_Speed > 255){
    car_Speed = 255;
  }else if ( car_Speed <0){
    car_Speed = 0;
  }else {
    analogWrite ( motor_ENA_L, car_Speed);
    analogWrite ( motor_ENB_L, car_Speed);
    analogWrite ( motor_ENA_R, car_Speed);
    analogWrite ( motor_ENB_R, car_Speed);
  }
}

void car_Freeze (){
  analogWrite ( motor_ENA_L, 0);
  analogWrite ( motor_ENB_L, 0);
  analogWrite ( motor_ENA_R, 0);
  analogWrite ( motor_ENB_R, 0);
}

void stair_Detect( ) {
  car_Freeze();
  move_Backward(255);
  delay(100);
  turn_Right(150);
  delay(600);
}

void move_Left(){
  car_Freeze();
  move_Backward(150);
  delay(100);
  turn_Left(150);
  delay(300);
}
void move_Right(){
  car_Freeze();
  move_Backward(150);
  delay(100);
  turn_Right(150);
  delay(300);
}
  

/*** Ultra Sonic Module  distance Measurement

***/
int measure_distance(){
  int duration;
  int distance;
  digitalWrite(ultra_Trig,HIGH);
  delayMicroseconds(100);
  digitalWrite(ultra_Trig,LOW);
  duration = pulseIn(ultra_Echo,HIGH);
  distance = (duration/2)/29.1;
  //Serial.print(distance);
  //Serial.print("cm");
  return distance;
}

/*** Self Drive    Block detect

***/
void block_Check () {
  //Serial.println("Block check");
  distance_Front = measure_distance();
  if ( distance_Front <= distance_Limit){
    car_Freeze();
  }
 // Serial.print ("Front = " ) ;
  //Serial.print (distance_Front);
  distance_Largest = distance_Front;
  
  myservo.write(135);
  delay(200);
  distance_Halfleft = measure_distance();
  if ( distance_Halfleft <= sidedistance_Limit){
    car_Freeze();
  }
  //Serial.print ("slightlyleft = ");
  //Serial.print (distance_Halfleft);
  if ( distance_Largest <= distance_Halfleft){
    distance_Largest = distance_Halfleft;
  }
  
  myservo.write(180);
  delay(200);
  distance_Left = measure_distance();
  if(distance_Left <=sidesidedistance_Limit){
    car_Freeze();
  }
  //Serial.print ("Left = ") ;
  //Serial.print (distance_Left);
  if ( distance_Largest <= distance_Left){
    distance_Largest = distance_Left;
  } 
  
  myservo.write(135);
  delay(200);
  distance_Halfleft = measure_distance();
  if ( distance_Halfleft <= sidedistance_Limit){
    
    car_Freeze();
  }
  //Serial.print ("slightlyleft = ");
  //Serial.print (distance_Halfleft);
  if ( distance_Largest <= distance_Halfleft){
    distance_Largest = distance_Halfleft;
  } 

  
  myservo.write(90);
  delay(200);
  distance_Front = measure_distance();
  if ( distance_Front <= distance_Limit){
    move_Backward(150);
    delay(200);
    car_Freeze();
  }
  //Serial.print ("Front = " ) ;
  //Serial.print (distance_Front);
  if ( distance_Largest <= distance_Front){
    distance_Largest = distance_Front;
  } 
  
  myservo.write(45);
  delay(200);
  distance_Halfright = measure_distance();
  if(distance_Halfright <= sidedistance_Limit){
    car_Freeze();
  }
  //Serial.print ("SlightlyRight = ") ;
  //Serial.print (distance_Halfright);
  if ( distance_Largest <= distance_Halfright){
    distance_Largest = distance_Halfright;
  } 
  
  
  myservo.write(0);
  delay(200);
  distance_Right = measure_distance();
  if(distance_Right <= sidesidedistance_Limit){
    car_Freeze();
  }
  //Serial.print ("Right = ") ;
  //Serial.print (distance_Right);
  if ( distance_Largest <= distance_Right){
    distance_Largest = distance_Right;
  } 
  
  myservo.write(90);
  delay(400);
  
  //Serial.print ("largest = ") ;
  //Serial.println (distance_Largest);;

}



void self_Drive ( ) {

  if ( distance_Front >= distance_Limit && distance_Halfleft >=sidedistance_Limit && distance_Halfright >= sidedistance_Limit) {
    move_Forward(100);
    counter++;
    if (counter == 130){
      counter = 0;
      car_Freeze();
      block_Check();
      if ( distance_Largest == distance_Left) {
        turn_Left(150);
        //Serial.println("turn left");
        delay(600);
        car_Freeze();
        //Serial.println("stop");
        //block_Check();
        //self_Drive();
      }else if (distance_Largest == distance_Right) {
        turn_Right(150);
        //Serial.println("turn right");
        delay(600);
        car_Freeze();
        //Serial.println("stop");
        //block_Check();
        //self_Drive();
      }else if (distance_Largest == distance_Halfright) {
        turn_Right(150);
        //Serial.println("turn half right");
        delay(400);
        car_Freeze();
        //block_Check();
        //self_Drive();
      }else if (distance_Largest == distance_Halfleft) {
        turn_Left(150);
        //Serial.println("turn half Left");
        delay(400);
        car_Freeze();
        //Serial.println("stop");
        //block_Check();
        //self_Drive();
      }else{
        if ( distance_Left >= sidesidedistance_Limit || distance_Right >= sidesidedistance_Limit){
          move_Forward(100);
        }else{
          move_Backward(100);
          delay(100);
          turn_Right(150);
          //Serial.println("turn right");
          delay(1300);
          car_Freeze();
          //Serial.println("stop");
          //block_Check();
          //self_Drive();
        }
      }
   }
      
    //Serial.println("move forward");
  }else {
    block_Check();
    if ( distance_Largest == distance_Left) {
      turn_Left(150);
      //Serial.println("turn left");
      delay(600);
      car_Freeze();
      //Serial.println("stop");
      //block_Check();
      //self_Drive();
    }else if (distance_Largest == distance_Right) {
      turn_Right(150);
      //Serial.println("turn right");
      delay(600);
      car_Freeze();
      //Serial.println("stop");
      //block_Check();
      //self_Drive();
    }else if (distance_Largest == distance_Halfright) {
      turn_Right(150);
      //Serial.println("turn half right");
      delay(400);
      car_Freeze();
      //block_Check();
      //self_Drive();
    }else if (distance_Largest == distance_Halfleft) {
      turn_Left(150);
      //Serial.println("turn half Left");
      delay(400);
      car_Freeze();
      //Serial.println("stop");
      //block_Check();
      //self_Drive();
    }else{
      if ( distance_Left >= sidesidedistance_Limit || distance_Right >= sidesidedistance_Limit){
        move_Forward(100);
      }else{
        move_Backward(100);
        delay(100);
        turn_Right(150);
        //Serial.println("turn right");
        delay(1300);
        car_Freeze();
        //Serial.println("stop");
        //block_Check();
        //self_Drive();
      }
    }
 }
}

void IR_Sensors(){
  floor_Check = digitalRead(IR_Floor);
  if (floor_Check != 0){
      //Serial.println("stairs at front");
      car_Freeze();
      move_Backward(255);
      delay(100);
      turn_Right(150);
      delay(900);
      car_Freeze();
      self_Drive();
   }
  //Serial.println(floor_Check);
  if (IR_Sensoractive == 1){
    block_Left = digitalRead (IR_Left);
    //Serial.println(block_Left);
    block_Halfleft = digitalRead (IR_Frontleft);
    //Serial.println(block_Halfleft);
    block_Halfright = digitalRead (IR_Frontright);
    //Serial.println(block_Halfright);
    block_Right = digitalRead(IR_Right);
    //Serial.println(block_Right);
    if (block_Left == 0 || block_Halfleft==0 || block_Halfright == 0 || block_Right == 0){
      move_Backward(150);
      delay(200);
      car_Freeze();
      block_Check();
      self_Drive(); 
    }
  }    
}

void Ultrasonic_Sensor (){
  myservo.write(180);
  delay(500);
  distance_Left = measure_distance();
  Serial.print ("Left = ") ;
  Serial.print (distance_Left);
  
  myservo.write(90);
  delay(500);
  distance_Front = measure_distance();
  
  myservo.write(0);
  delay(500);
  distance_Right = measure_distance();
  Serial.print ("Right = ") ;
  Serial.print (distance_Right);
  
  myservo.write(90);
  delay(500);
  distance_Front = measure_distance();
  Serial.print ("Front = ") ;
  Serial.print (distance_Front);
  
  if (distance_Front <= 20 && distance_Front >= distance_Left && distance_Front >= distance_Right){
    Serial.println("Road End");
    turn_Right(150);
    delay(800);
  }else if (distance_Left > distance_Right){
    turn_Left(150);
    delay(200);
    IR_Sensors();
  }else if (distance_Right >= distance_Left){
    turn_Right(150);
    delay(200);
    IR_Sensors();
  }
}


/*** IR signal Decode!


***/

/*
void process_command(int cmd)
{
  switch(cmd){
    case 0x9716BE3F: // '1'
      
      break;
    case 0x3D9AE3F7: // '2'
      move_forward = 1;
      move_backward = 0;
      turn_left = 0;
      turn_right = 0;
      freeze = 0;
      self_drive = 0;
      
      break;
    case 0x6182021B: // '3'
      
      break;
    case 0x8C22657B: // '4'
      move_forward = 0;
      move_backward = 0;
      turn_left = 1;
      turn_right = 0;
      freeze = 0;    
      self_drive = 0;
      
      break;
    case 0x488F3CBB: // '5'
      move_forward = 0;
      move_backward = 0;
      turn_left = 0;
      turn_right = 0;
      freeze = 1;
      self_drive = 0;
      
     
      break;
    case 0x449E79F: // '6'
      move_forward = 0;
      move_backward = 0;
      turn_left = 0;
      turn_right = 1;
      freeze = 0;
      self_drive = 0;
      
     
      break;
    case 0x32C6FDF7: // '7'
     
      break;
    case 0x1BC0157B: // '8'
      move_forward = 0;
      move_backward = 1;
      turn_left = 0;
      turn_right = 0;
      freeze = 0;
      self_drive = 0;
      
      break;
    case 0x3EC3FC1B: // '9'
     
      break;
    case 0xC101E57B: // '0'
     
      break;
    case 0xC97483BFB: // '100+'
     
      break;
    case 0xF0C41643: // '200+'
     
      break;
    case 0xE318261B: // 'CH-'
      
      break;
    case 0x511DBB: // 'CH'
     
      break;
    case 0xEE886D7F: // 'CH+'
     
      break;
    case 0x52A3D41F: // '|<<'
      move_forward = 0;
      move_backward = 0;
      turn_left = 0;
      turn_right = 0;
      freeze = 0;
      self_drive = 1;
      break;
    case 0xD7E84B1B: // '>>|'
      self_drive = 0;
      break;
    case 0x20FE4DBB: // '>||'
     
      break;
    case 0xF076C13B: // '-'
     
      break;
    case 0xA3C8EDDB: // '+'
     
      break;
    case 0xE5CFBD7F: // 'EQ'
     
      break;
    default:
      break;
      ;
  }

  
}

void process_state(){
  if (previous_state != 2 && move_forward == 1){
    digitalWrite(3,LOW);
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
    digitalWrite(8,LOW);
    digitalWrite(2,HIGH);
    digitalWrite(4,LOW);
    digitalWrite(7,HIGH);
    digitalWrite(9,LOW);
    previous_state = 2;
  }else if( previous_state != 8 && move_backward == 1){
    digitalWrite(2,LOW);
    digitalWrite(4,LOW);
    digitalWrite(7,LOW);
    digitalWrite(9,LOW);
    digitalWrite(3,HIGH);
    digitalWrite(5,LOW);
    digitalWrite(6,HIGH);
    digitalWrite(8,LOW);
    previous_state = 8;
  }else if(previous_state != 4 && turn_left == 1){
    digitalWrite(2,LOW);
    delay(1);
    digitalWrite(4,LOW);
    delay(1);
    digitalWrite(6,LOW);
    delay(1);
    digitalWrite(8,LOW);
    delay(1);
    digitalWrite(7,HIGH);
    delay(1);
    digitalWrite(9,LOW);
    delay(1);
    digitalWrite(3,HIGH);
    delay(1);
    digitalWrite(5,LOW);
    delay(1);
    previous_state = 4;
  }
  else if(previous_state != 6 && turn_right == 1){
    digitalWrite(7,LOW);
    delay(1);
    digitalWrite(9,LOW);
    delay(1);
    digitalWrite(6,LOW);
    delay(1);
    digitalWrite(8,LOW);
    delay(1);
    digitalWrite(6,HIGH);
    delay(1);
    digitalWrite(8,LOW);
    delay(1);
    digitalWrite(2,HIGH);
    delay(1);
    digitalWrite(4,LOW);
    delay(1);
    
    previous_state = 6;
  }else if(previous_state != 5 && freeze == 1){
    digitalWrite(7,LOW);
    digitalWrite(9,LOW);
    digitalWrite(6,LOW);
    digitalWrite(8,LOW);
    digitalWrite(2,LOW);
    digitalWrite(4,LOW);
    digitalWrite(3,LOW);
    digitalWrite(5,LOW);
    previous_state = 5;;
  } else if ( self_drive == 1){
     while(1){
       myservo.writeMicroseconds(3000);
       delay(400);
       left = measure_distance();
       if (left >= largest_distance){
         largest_distance = left;
         digitalWrite(2,LOW);
          delay(1);
          digitalWrite(4,LOW);
          delay(1);
          digitalWrite(6,LOW);
          delay(1);
          digitalWrite(8,LOW);
          delay(1);
          digitalWrite(7,HIGH);
          delay(1);
          digitalWrite(9,LOW);
          delay(1);
          digitalWrite(3,HIGH);
          delay(1);
          digitalWrite(5,LOW);
          delay(1);
       }
       delay(20);
       myservo.writeMicroseconds(1850);
       delay(400);
       front = measure_distance();
       delay(20);
       if (front >= largest_distance){
         largest_distance = front;
         digitalWrite(3,LOW);
          digitalWrite(5,LOW);
          digitalWrite(6,LOW);
          digitalWrite(8,LOW);
          digitalWrite(2,HIGH);
          digitalWrite(4,LOW);
          digitalWrite(7,HIGH);
          digitalWrite(9,LOW);
       }
       myservo.writeMicroseconds(1200);
       delay(400);
       right = measure_distance();
       delay(20);
       if (right >= largest_distance){
         largest_distance = right;
         digitalWrite(7,LOW);
          delay(1);
          digitalWrite(9,LOW);
          delay(1);
          digitalWrite(6,LOW);
          delay(1);
          digitalWrite(8,LOW);
          delay(1);
          digitalWrite(6,HIGH);
          delay(1);
          digitalWrite(8,LOW);
          delay(1);
          digitalWrite(2,HIGH);
          delay(1);
          digitalWrite(4,LOW);
          delay(1);
       }
       myservo.writeMicroseconds(1850);
       delay(400); 
       front = measure_distance();
       delay(20);
       if (front >= largest_distance){
         largest_distance = front;
         digitalWrite(3,LOW);
          digitalWrite(5,LOW);
          digitalWrite(6,LOW);
          digitalWrite(8,LOW);
          digitalWrite(2,HIGH);
          digitalWrite(4,LOW);
          digitalWrite(7,HIGH);
          digitalWrite(9,LOW);
       }
       if (front <= 20 &&left <= 20 && right <= 20){
         digitalWrite(7,LOW);
          digitalWrite(9,LOW);
          digitalWrite(6,LOW);
          digitalWrite(8,LOW);
          digitalWrite(6,HIGH);
          digitalWrite(8,LOW);
          digitalWrite(2,HIGH);
          digitalWrite(4,LOW);
       }
          
     }
     
     previous_state = 10;
  }
}
*/

