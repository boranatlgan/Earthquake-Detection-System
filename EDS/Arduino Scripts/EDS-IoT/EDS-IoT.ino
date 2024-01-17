//NOTE: Please add melodiesandpins.h library inside the folder.Otherwise, software errors may occur.In case the error occurs,try to read README files accordingly.

#include <SoftwareSerial.h>
#include <math.h>
#include <LiquidCrystal_I2C.h>
#include "melodiesandpins.h"

String nwName = "/*Your Wifi Network Name*/";   
String nwPassword = "/*Wifi Password*/";

int rxPin = 10;                                               
int txPin = 11;

String ip = "184.106.153.149";

SoftwareSerial esp(rxPin, txPin);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()

{
  for (int i = 0; i < toneNumber; i++)
  {
    tone(buzzerPin, notes[i]);
    delay(100);
    noTone(buzzerPin);
    delay(20);
  }
  noTone(buzzerPin);

  lcd.begin();  //initializing lcd
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600); // initializing serial
  Serial.println("Started");
  esp.begin(115200);                                          //We are starting the serial communication with ESP8266.
  esp.println("AT");                                          //We check for the module if active or not with AT command.
  Serial.println("AT command sent");
  while(!esp.find("OK")){                                     //We wait till the module is set and ready.
    esp.println("AT");
    Serial.println("ESP8266 couldn't find.");
    }
    Serial.println("OK command received");
  esp.println("AT+CWMODE=1");                                 //We specify the ESP8266 as client.
  while(!esp.find("OK")){                                     //Waiting till al the settings are set.
    esp.println("AT+CWMODE=1");
    Serial.println("Booting the system...");
    }
    Serial.println("Set as Client");
  Serial.println("Connecting to network...");
  esp.println("AT+CWJAP=\""+nwName+"\",\""+nwPassword+"\"");    //Connecting the network.
  while(!esp.find("OK"));                                     //We are waiting till the connection is established.
  Serial.println("Connection Established.");
  delay(1000);
  delay(100);
  lcd.setCursor(0, 0);
  lcd.print("Earthquake");
  lcd.setCursor(0, 1);
  lcd.print("Detector 1.0");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Calibrating The");
  lcd.setCursor(0, 1);
  lcd.print("System");
  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Calibrating");
  lcd.setCursor(0, 1);
  lcd.print("*");
  delay(2000);
  lcd.print("*");
  delay(2000);
  lcd.print("*");

  delay(3000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Completed");
  delay(3000);
  lcd.clear();
  buz = 0;
  digitalWrite(buzzerPin, buz);
  digitalWrite(ledPin, buz);
  for (int i = 0; i < samples; i++) // taking samples for calibration
  {
    xsample += analogRead(xPin);
    ysample += analogRead(yPin);
    zsample += analogRead(zPin);
  }

  xsample /= samples; // taking avg for x
  ysample /= samples;   // taking avg for y
  zsample /= samples; // taking avg for z

}

void loop()

{
  int value1 = analogRead(xPin); // reading x out
  int value2 = analogRead(yPin); //reading y out
  int value3 = analogRead(zPin); //reading z out

  int xValue = xsample - value1; // finding change in x
  int yValue = ysample - value2; // finding change in y
  int zValue = zsample - value3; // finding change in z

  /* comparing change with predefined limits*/

  if (xValue < minVal || xValue > maxVal  || yValue < minVal || yValue > maxVal  || zValue < minVal || zValue > maxVal)

  {

    if (buz == 0)
      start = millis(); // timer start
    buz = 1;     // buzzer / led flag activated

  }
  else if (buz == 1)       // buzzer flag activated then alerting earthquake
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Earthquake Alert   ");
    if (millis() >= start + buzTime) {
      buz = 0;
    }
  }

  else

  {
    delay(5);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Shake");
    lcd.setCursor(0, 1);
    lcd.print("Detected");
  }

  digitalWrite(buzzerPin, buz);    // buzzer on and off command
  digitalWrite(ledPin, buz);   // led on and off command

  /*sending values to processing to plot over the graph*/

  Serial.print("x=");
  Serial.println(xValue);
  Serial.print("y=");
  Serial.println(yValue);
  Serial.print("z=");
  Serial.println(zValue);

  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Connecting to thingspeak.
  if(esp.find("Error")){                                      //Checking if connection is established.
    Serial.println("AT+CIPSTART Error");
  }
  String data = "/*API key here*/";   /*Thingspeak command. We write our own channel API key here.*/
  /*Sending axis data*/
  data += "&field1=";
  data += int(xValue);
  data += "&field2=";
  data += int(yValue);
  data += "&field3=";
  data += int(zValue);
  data += "&field4=";
  data += buz;
  data += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //Specifying the data length.
  esp.println(data.length()+2);
  delay(2);
  if(esp.find(">")){                                          //Commands are executed when ESP8266 is ready.
    esp.print(data);                                          //Sending the data.
    Serial.println(data);
    Serial.println("Data sent.");
  }
  Serial.println("Connection ended.");
  esp.println("AT+CIPCLOSE");                                //Close the connection..


}
