//read PPM signals from 2 channels of an RC reciever
//http://arduino.cc/forum/index.php/topic,42286.0.html
 
//接收机两个通道分别接arduino的数字口2、3脚
//Most Arduino boards have two external interrupts: numbers 0 (on digital pin 2) and 1 (on digital pin 3). 
//The Arduino Mega has an additional four: numbers 2 (pin 21), 3 (pin 20), 4 (pin 19), and 5 (pin 18).
int ppm1 = 2;  
int ppm2 = 3;
int ppm3 = 10;


const int L_IN1 = 4, L_IN2 =5, R_IN1 = 7, R_IN2 = 8;
const int L_EN = 6, R_EN = 9;
 
unsigned long rc1_PulseStartTicks,rc2_PulseStartTicks;        
volatile int rc1_val, rc2_val;
int rc3_val;  
unsigned int power; 
unsigned int turn;
unsigned int straight;
int left, right, forward, backward;

void setup() {
 
        Serial.begin(9600);  
 
        //PPM inputs from RC receiver
        pinMode(ppm1, INPUT); 
        pinMode(ppm2, INPUT); 
 
        // 电平变化即触发中断
        attachInterrupt(0, rc1, CHANGE);    
        attachInterrupt(1, rc2, CHANGE);    
}
 
void rc1()
{
        // did the pin change to high or low?
        if (digitalRead( ppm1 ) == HIGH)
                rc1_PulseStartTicks = micros();    // store the current micros() value
        else
                rc1_val = micros() - rc1_PulseStartTicks; 
}
 
void rc2()
{
        // did the pin change to high or low?
        if (digitalRead( ppm2 ) == HIGH)
                rc2_PulseStartTicks = micros();    
        else
                rc2_val = micros() - rc2_PulseStartTicks; 
}
 
void loop() {
 
        //print values
        /*
        Serial.print("channel 1:  ");
        Serial.print(rc1_val);  
        Serial.print("        ");
        Serial.print("channel 2:  ");   
        Serial.print(rc2_val);  
        */
        rc3_val = pulseIn(ppm3, HIGH, 20000);
        //Serial.print("channel 3:  ");   
        //Serial.println(rc3_val);  
        power = map(rc2_val, 1016, 2024, 0, 255);
        turn = rc1_val-16;
        straight = rc3_val;
        left = turn - 1500;
        right = 1500 - turn;
        forward = straight - 1500;
        backward = 1500 -straight;
        /*
        Serial.print (left);
        Serial.print("        ");
        Serial.print (right);
        Serial.print("        ");
        Serial.print (forward);
        Serial.print("        ");
        Serial.println (backward);
        */
        /*
        Serial.print("power:  ");
        Serial.print(power);  
        Serial.print("        ");
        Serial.print("turn:  ");   
        Serial.println(turn);  
        */
        analogWrite(L_EN, power);
        analogWrite(R_EN,power);
        
        if ( left >= 300 && left >= right && left >= forward && left >= backward){
          Serial.println("turn left");
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, HIGH);
          digitalWrite(R_IN1,HIGH);
          digitalWrite(R_IN2, LOW);
        }else if ( right >= 300 && right >= left && right >= forward && right >= backward){
          Serial.println("turn right");
          digitalWrite(L_IN1 , HIGH);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,LOW);
          digitalWrite(R_IN2, HIGH);
        }else if ( forward >= 300 && forward >= left && forward >= right && forward >= backward){
          Serial.println("go forward");
          digitalWrite(L_IN1 , HIGH);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,HIGH);
          digitalWrite(R_IN2, LOW);
        }else if ( backward >= 300 && backward >= forward && backward >= right && backward >= left){
          Serial.println("go back");
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, HIGH);
          digitalWrite(R_IN1, LOW);
          digitalWrite(R_IN2, HIGH);
        }else {
          Serial.println("stop");
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,LOW);
          digitalWrite(R_IN2, LOW);
        }
        /*
        if (turn >= 1100 && turn <= 1900 && straight >= 1100 && straight <=1900){
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,LOW);
          digitalWrite(R_IN2, LOW);
        }else if (turn > 1900 && straight >1900){
          if (straight >190
        
        if (turn >1900){
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, HIGH);
          digitalWrite(R_IN1,HIGH);
          digitalWrite(R_IN2, LOW);
        }else if(turn<1100){
          digitalWrite(L_IN1 , HIGH);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,LOW);
          digitalWrite(R_IN2, HIGH);
        }else {
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,LOW);
          digitalWrite(R_IN2, LOW);
        }
        if (straight >1900){
          digitalWrite(L_IN1 , HIGH);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,HIGH);
          digitalWrite(R_IN2, LOW);
        }else if (straight < 1100){
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, HIGH);
          digitalWrite(R_IN1, LOW);
          digitalWrite(R_IN2, HIGH);
        }else {
          digitalWrite(L_IN1 , LOW);
          digitalWrite(L_IN2, LOW);
          digitalWrite(R_IN1,LOW);
          digitalWrite(R_IN2, LOW);
        }
         
         */
}

