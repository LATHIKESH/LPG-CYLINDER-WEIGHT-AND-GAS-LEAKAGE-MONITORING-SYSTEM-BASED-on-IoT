#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#define RX 2
#define TX 3 
HX711 scale;
float calibration_factor = 52; // this calibration factor is adjusted according to my load cell
float units;
float ounces;
String WIFI_SSID = "your wifi name";
String WIFI_PASS = "your password"; 
String API = "IM7KD5UVFY897OCB";
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand;
int servoPin = 6;
int buzzerPin = 7; 
int ledPin=8;
//int angle = 360;
Servo Servo1; 
boolean found = false;   
SoftwareSerial esp8266(RX,TX);
LiquidCrystal_I2C lcd(0x27,16,2);
 void setup() {
 lcd.init();
 lcd.clear();         
 lcd.backlight();
 Serial.begin(9600);
 scale.begin(4,5);
 esp8266.begin(115200);
Servo1.attach(servoPin);
 pinMode(buzzerPin, OUTPUT);
 pinMode(ledPin, OUTPUT);
 sendCommand("AT",5,"OK");
 sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ WIFI_SSID +"\",\""+ WIFI_PASS +"\"",20,"OK");
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); 
  Serial.println(zero_factor);
}

 void loop() {
  int gas=analogRead(A0);
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Weight: ");
  units = scale.get_units(), 1000;
  if (units <= 0)
  {
    units = 0.00;
  }
  ounces = units * 0.035274;
  Serial.print(units);
  Serial.print(" grams");
  lcd.setCursor(0,0); 
  lcd.print("WEIGHT: ");
  lcd.setCursor(9,0); 
  lcd.print(units,0);
  lcd.setCursor(14,0); 
  lcd.print("g");
  lcd.setCursor(0,1);   
  lcd.print("MQ6: ");
  lcd.setCursor(5,1); 
  lcd.print(gas);
  lcd.setCursor(9,1); 
  lcd.print("PPM");
  Serial.print("MQ6: ");
  Serial.print(gas);
  Serial.println("PPM");
  delay(1000);
  lcd.clear();
  if(units<500)
 {
  digitalWrite(ledPin, HIGH);
 }
else
 {
digitalWrite(ledPin, LOW);
 }
 if(gas>100)
 {
  Servo1.write(180); 
  digitalWrite(buzzerPin, HIGH);
 }
 else
 {
  Servo1.write(0);
  digitalWrite(buzzerPin, LOW);
 }
  
 String getData="GET /update?api_key="+ API+"&field1="+gas+ "&field2="+units;
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 1;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 1;
  }}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
    countTimeCommand++;
  } 
  if(found == true)
  {
    Serial.println("OK");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  } 
  found = false;
 }
