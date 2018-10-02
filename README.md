# Intro
--------
WolkNode-TTN repo consists Arduino IDE example which makes connection to [The Things Network](https://www.thethingsnetwork.org/). It's based on [RN2483-Arduino-Library](https://github.com/jpmeijers/RN2483-Arduino-Library).
Device measure enviroment temperature data on hearbeat event. This data can be routed from TTN to [WolkAbout IoT Platform](https://demo.wolkabout.com/#/login) using apropiated decoder(below described as [Application Decoder](#decoder)) and WolkAbout IoT Platform wizard.

# Prerequest
-----
**Used Hardware:**

 * [Arduino UNO](https://store.arduino.cc/usa/arduino-uno-rev3)
 * [Mikroe LoRa click - RN2483](https://www.mikroe.com/lora-rf-click)
 * [DS18B20 Temperature Sensor](https://eu.mouser.com/ProductDetail/Maxim-Integrated/DS18B20%2b?qs=sGAEpiMZZMvbyeSUH4qH%2fLbikZ7SIep9)

**Firmware dependencies:**

 * Install following Arduino libs([HOW TO](https://www.arduino.cc/en/guide/libraries)):
   * OneWire
   * DallasTemperature
 
 * Include files:
   * src/rn2xx3.cpp and src/rn2xx3.h with *Sketch/Include Library/Add .ZIP Library...* in Arduino IDE. Original source is [RN2483-Arduino-Library](https://github.com/jpmeijers/RN2483-Arduino-Library).

# Example of usage
------

## Hardware Connection
<center>
![wolkttn-connection - small](https://user-images.githubusercontent.com/10706360/46349777-46f75680-c653-11e8-8517-2629b2f7b8c1.png)
</center>

## Firmware Deployment

* Open one of *WolkNode_TTN.ino* example
* **#define** LoRA connection mode, **OTAA** or **ABP**
* Insert your TTN device parameters. Copy *Example Code* from device overview into WolkNode_TTN.ino
* Verify and Upload to device

<a name="decoder">

# Application Decoder

-----
</a>

To route data from TTN to WolkAbout IoT Platform create device on WolkAbout IoT Platofrm and copy following decoder into ***Payload Formats*** section on *TTN Applications*. Note that name of methode *'.T'* over *var decoded* is *REF* from device manifest on WolkAbout IoT platform.

```
function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  var type = bytes[0];
  
  switch (type) {
    case 1:
      decoded.T = bytes[1] + bytes[2]/100;
      break;
    default:
      decoded.T = bytes[0];
  }
  
  return decoded;
}
```
