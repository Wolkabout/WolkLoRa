#include <rn2xx3.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

const char *appEui = "";
const char *appKey = "";

//create an instance of the rn2xx3 library,
//giving the software serial as port to use
rn2xx3 myLora(mySerial);

void setup()
{
  //output LED pin
  pinMode(4, OUTPUT);
  led_off();
  
  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  initialize_radio();

  //transmit a startup message
  myLora.tx("TTN Mapper on TTN Enschede node");
  
  delay(2000);
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while (hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccesful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;

  join_result = myLora.initOTAA(appEui, appKey);
  //if you want ABP activation, use the following line and not the previous one
  //join_result = myLora.initABP(String addr, String AppSKey, String NwkSKey)

  while (!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

// the loop routine runs over and over again forever:
void loop()
{
  Serial.print("TXing");
  //myLora.txCnf("!"); //one byte, blocking function

  switch (myLora.txCnf("!")) //one byte, blocking function
  {
    case TX_FAIL:
      {
        Serial.println("TX unsuccessful or not acknowledged");
        break;
      }
    case TX_SUCCESS:
      {
        Serial.println("TX successful and acknowledged");
        break;
      }
    case TX_WITH_RX:
      {
        String received = myLora.getRx();
        received = myLora.base16decode(received);
        Serial.print("Received downlink: " + received);
        
        if(received == "ON")
        {
          led_on();  
        }
        else
        {
          led_off();
        }
        
        break;
      }
    default:
      {
        Serial.println("Unknown response from TX function");
      }
  }
  
  delay(600000);
}

void led_on()
{
  digitalWrite(4, 1);
}

void led_off()
{
  digitalWrite(4, 0);
}