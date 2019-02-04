/*
 * Author: sstankovic
 * Date: 2018-03-06
 * Filename: WolkNode-TTN
 * Decription: Project core is based on https://github.com/jpmeijers/RN2483-Arduino-Library. 
 *             This program is meant to be used with an Arduino UNO, conencted to any RN2483 radio module and DS1820 temperature sensor.
 *             Don't forget to CHANGE CONNECTION PARAMETERS!
 *
 *     Copyright 2018 WolkAbout Technology s.r.o.
 */
#include <rn2xx3.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define debugSerial Serial

//#define OTAA                     //DEFINE CONNECTION MODE: ABP or OTAA
#define HEARTBEAT 60000          //in ms
#define BYTES_TO_SEND 3
#define ONE_WIRE_BUS 2

#ifdef OTAA                      // Copy from TTN Console/Device Overview your AppEUI and AppKey for OTAA mode
const char *appEui = "";
const char *appKey = "";
#else //ABP                      // Copy from TTN Console/Device Overview your DevAddr, NwkSKey and AppSKey for ABP mode
const char *devAddr = "";
const char *nwkSKey = "";
const char *appSKey = "";
#endif
bool join_result = false;


SoftwareSerial mySerial(10, 11); // RX, TX
rn2xx3 myLora(mySerial);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


void setup(){
  mySerial.begin(9600);
  debugSerial.begin(115200);
  debugSerial.println("Startup");

  initializeRadio();

  initLed();
  sensors.begin();
  delay(2000);
}

void loop(){
  setLed(HIGH);
  debugSerial.print("\n\rRequesting temperatures...");
  sensors.requestTemperatures();
  debugSerial.print(" Temperature is: ");
  float temperature = sensors.getTempCByIndex(0);
  debugSerial.print(temperature);
  debugSerial.print(" C \n\r");
  int temperature_high = temperature;
  int temperature_low = 100 * (temperature - temperature_high);

  byte payload[3];
  payload[0] = 0x01;
  payload[1] = lowByte(temperature_high);
  payload[2] = lowByte(temperature_low);

  debugSerial.print(" Sending payload to TTN ");
  join_result = myLora.tx(payload);
  if(!join_result){
    debugSerial.print("Failed to transmit to readings. \n\r Status is:");
    debugSerial.print(join_result);
    
    while(!join_result){
      debugSerial.println("...trying to reconnect to TTN Gateway. \n\rDo you have TTN coverage?");
      delay(60000);              //delay a minute before retry
      join_result = myLora.init();
    }
  }
  debugSerial.print("Successfully with join_result of");
  debugSerial.print(join_result);
  
  setLed(LOW);
  myLora.sleep(HEARTBEAT);
  delay(HEARTBEAT);
}



void initializeRadio(){
  pinMode(12, OUTPUT);    //reset RN2483
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  
  delay(100);             //wait for the RN2xx3's startup message
  mySerial.flush();

  myLora.autobaud();     //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.

  String hweui = myLora.hweui();
  while(hweui.length() != 16){
    debugSerial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    debugSerial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  debugSerial.println("When using OTAA, register this DevEUI: ");
  debugSerial.println(myLora.hweui());
  debugSerial.println("RN2xx3 firmware version:");
  debugSerial.println(myLora.sysver());

  debugSerial.println("Trying to join TTN");
  #ifdef OTAA
    debugSerial.println("-- OTAA SELECTED --\n\r-- JOIN OTAA --");
    join_result = myLora.initOTAA(appEui, appKey);
  #else
    debugSerial.println("-- ABP SELECTED --\n\r-- PERSONALIZE ABP --");
    join_result = myLora.initABP(devAddr, appSKey, nwkSKey);
  #endif

  while(!join_result){
    debugSerial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000);       //delay a minute before retry
    join_result = myLora.init();
  }
  debugSerial.println("Successfully joined TTN");
}

void initLed(){
    pinMode(4, OUTPUT);
}

void setLed(bool state){
  digitalWrite(4, state);
}
