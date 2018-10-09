/*
 * Author: sstankovic
 * Date: 2018-09-10
 * Filename: WolkNode-TTN
 * Decription: Project core is based on https://github.com/jpmeijers/RN2483-Arduino-Library. 
 *             This program is meant to be used with an Arduino UNO, conencted to any RN2483 radio module and DS1820 temperature sensor.
 *             Don't forget to CHANGE CONNECTION PARAMETERS!
 *
 *     Copyright 2018 WolkAbout Technology s.r.o.
 */
#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <CayenneLPP.h>

#define debugSerial Serial

#define OTAA                    //DEFINE CONNECTION MODE: ABP or OTAA
#define freqPlan TTN_FP_EU868   //Freq plan
#define HEARTBEAT 60000          //in ms
#define BYTES_TO_SEND 4
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
TheThingsNetwork ttn(mySerial, debugSerial, freqPlan);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
CayenneLPP lpp(30);


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

  debugSerial.print(" Formating to CayenneLPP \n\r");
  lpp.reset();
  lpp.addTemperature(1, get_temperature());

  debugSerial.print(" Sending payload to TTN ");
  byte payload[BYTES_TO_SEND];
  memcpy(payload, lpp.getBuffer(), BYTES_TO_SEND);
  join_result = ttn.sendBytes(lpp.getBuffer(), lpp.getSize());
  if(!join_result){
    debugSerial.print("Failed to transmit to readings. \n\r Status is:");
    debugSerial.print(join_result);
    
    while(!join_result){
      debugSerial.println("...trying to reconnect to TTN Gateway. \n\rDo you have TTN coverage?");
      delay(60000);              //delay a minute before retry
      join_result = join_ttn();
    }
  }
  debugSerial.print("Successfully");
  
  setLed(LOW);
  ttn.sleep(HEARTBEAT);
  delay(HEARTBEAT);
}


void initializeRadio(){
  pinMode(12, OUTPUT);    //reset RN2483
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);
  
  delay(100);             //wait for the RN2xx3's startup message

  debugSerial.println("Trying to join TTN");
  join_result = join_ttn();
  while(!join_result){
    debugSerial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000);       //delay a minute before retry
    join_result = join_ttn();
  }
  debugSerial.println("Successfully joined TTN");

  debugSerial.println("Device Status is:");
  ttn.showStatus();
}

bool join_ttn()
{
  bool result = false;

#ifdef OTAA
  debugSerial.println("-- OTAA SELECTED --\n\r-- JOIN OTAA --");
  result = ttn.join(appEui, appKey);
#else
  debugSerial.println("-- ABP SELECTED --\n\r-- PERSONALIZE ABP --");
  result = ttn.personalize(devAddr, nwkSKey, appSKey);
#endif

  return result;
}

float get_temperature()
{
  debugSerial.print("\n\rRequesting temperatures...");
  sensors.requestTemperatures();

  debugSerial.print(" Temperature is: ");
  float temperature = sensors.getTempCByIndex(0);
  debugSerial.print(temperature);
  debugSerial.print(" C \n\r");

  return temperature;
}

void initLed(){
    pinMode(4, OUTPUT);
}

void setLed(bool state){
  digitalWrite(4, state);
}

