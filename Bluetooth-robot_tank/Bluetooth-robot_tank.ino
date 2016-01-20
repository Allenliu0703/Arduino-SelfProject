int tx=1;
int rx=0;
char inSerial[15];
#define EN1 5
#define M1 4
#define EN2 6
#define M2 7



void setup() {
  Serial.begin(115200);
  //pinMode(EN_R, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  //pinMode(EN_L, OUTPUT);
 
  
}

void loop() {
    int i = 0;
    delay(100);                                         
    if (Serial.available() > 0) {             
       while (Serial.available() > 0) {
         inSerial[i]=Serial.read(); 
         i++;      
       }
       inSerial[i]='\0';
      Serial.println(inSerial);
      Check_Protocol(inSerial);
     }
    
}

void Check_Protocol(char inStr[]){   
  Serial.println(inStr);
  
  if(!strcmp(inStr,"w")){    
    Serial.println("move forward");
    move_Forward(255);  
  }
       
  if(!strcmp(inStr,"s")){    
    Serial.println("move back");
    move_Back(255);  
  }
   
   if(!strcmp(inStr,"a")){    
    Serial.println("turn left");
    turn_Left(120);  
  }
       
    if(!strcmp(inStr,"d")){    
    Serial.println("turn right");
    turn_Right(120);  
  }
    if(!strcmp(inStr,"f")){    
    Serial.println("stop");
    freeze();  
  }
}

void move_Forward(unsigned int x){
  if (x > 255)
    x=255;
  digitalWrite(M1,HIGH);
  digitalWrite(M2,HIGH);
  analogWrite(EN1,x);
  analogWrite(EN2,x-9);
}

void move_Back(unsigned int x){
  if (x > 255)
    x=255;
  digitalWrite(M1,LOW);
  digitalWrite(M2,LOW);
  analogWrite(EN1,x);
  analogWrite(EN2,x-40);
}

void turn_Right(unsigned int x){
  if (x > 255)
    x=255;
  digitalWrite(M1,LOW);
  digitalWrite(M2,HIGH);
  analogWrite(EN1,x);
  analogWrite(EN2,x);
}

void turn_Left(unsigned int x){
  if (x > 255)
    x=255;
  digitalWrite(M1,HIGH);
  digitalWrite(M2,LOW);
  analogWrite(EN1,x);
  analogWrite(EN2,x);
}

void freeze(){
  analogWrite(EN1,0);
  analogWrite(EN2,0);
}

  
