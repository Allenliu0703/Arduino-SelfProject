#include "DHT.h"
#include "SoftwareSerial.h"
#include "stdlib.h"

#define DHTPIN 2     // what pin we're connected to
#define LED 12
#define DHTTYPE DHT11   // DHT 11 
#define Baud_Rate 9600
#define SSID "Tomato24"
#define PASS "11111111"

SoftwareSerial esp8266(3,4);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LED, OUTPUT);
  esp8266.begin(Baud_Rate);
  Serial.begin(9600); 
  dht.begin();
  Serial.println("DHT11 test!");
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    delay(1000);
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
  esp8266.print("AT\n");
  delay(3000);
  esp8266.read(); /* get rid of the first elemnt of the serial buffer, which is the nullj */
  if(esp8266.find("OK")){  
    Serial.println("Managed to talk to esp8266");
    boolean wificonnected = connectWiFi();
    if(wificonnected){
      Serial.println("Connected to WiFi");
    }else{
      Serial.println("Failed to connect to WiFi");
      Error();
    }
  }else{
    Serial.println("Fail to talk to 8266");
    Error();
  }
}


void loop() {
  Error();
  
}



void Error(){
  while(true){
    digitalWrite(LED,HIGH);
    delay(1000);
    digitalWrite(LED,LOW);
    delay(1000);
  }
}


boolean connectWiFi(){
  //set ESP8266 mode with AT commands
  esp8266.println("AT+CWMODE=1");
  delay(3000);
   /* get rid of the first elemnt of the serial buffer, which is the nullj */
  if(esp8266.find("OK")){  
    Serial.println("Changed to station mode");
  }else{
    Serial.println("Failed to change to station mode");
  }

  //build connection command
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  
  //connect to WiFi network and wait 5 seconds
  esp8266.println(cmd);
  delay(5000);
  /*
  int x = esp8266.available();
  Serial.println(x);
  for(int count = 0; count <=25 ; count++){
    esp8266.read();
  }
  */
  //if connected return true, else false
  if(esp8266.find("OK")){
      Serial.print("Connected to ");
      Serial.println(SSID);
      return true;
  }else{
    Serial.print("Failed to connect to ");
    Serial.println(SSID);
    return false;
  }
}
