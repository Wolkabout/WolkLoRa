# Intro
--------
WolkTTN repo consists of Arduino IDE example which makes connection to [The Things Network](https://www.thethingsnetwork.org/). It's based on [RN2483-Arduino-Library](https://github.com/jpmeijers/RN2483-Arduino-Library). After connecting, it polls every 10 minutes and depending on the message downlinked from The Things Network it will switch a LED on or off. On if the message is "ON" (4F 4E), or off if the message is anything else.

# Prerequest
-----
**Used Hardware:**

 * [Arduino UNO](https://store.arduino.cc/usa/arduino-uno-rev3)
 * [Mikroe LoRa click - RN2483](https://www.mikroe.com/lora-rf-click)
 *	LED diode

**Firmware dependencies:**

* Include files:
   * src/rn2xx3.cpp and src/rn2xx3.h with *Sketch/Include Library/Add .ZIP Library...* in Arduino IDE. Original source is [RN2483-Arduino-Library](https://github.com/jpmeijers/RN2483-Arduino-Library).

# Example of usage
------

## Firmware Deployment

* Open the *WolkDownlink.ino* example
* Chose OTAA or ABP activation mode (lines 66-68)
* Insert your TTN device parameters.
* Verify and Upload to device

## Sending the downlink to device
------

You send the downlink via The Things Network platform.