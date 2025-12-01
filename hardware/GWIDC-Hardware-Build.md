# GWIDC HARDWARE BUILD

## Components and Connections

|Component|Notes|
|:---|:---|
|NodeMCU 1.0 |ESP8266-based Development Board built around ESP-12E module|
|Power Supply|Connect standard USB power via NodeMCU micro USB port<br>/or/ <br>use a standalone 5VDC 2A power supply connected to VIN and GND pins of the NodeMCU |
|Neopixel Ring (12 pixel)<br> with 330 ohm resistor<br> and 1000 µF capacitor|Connect Neopixel Ring PWR to VIN on the NodeMCU board<br>Connect Neopixel Ring GND to GND on the NodeMCU board<br>Connect Neopixel Ring IN to D7 (GPIO13) on the NodeMCU board via 330 ohm resistor<br>Connect 1000uF Capacitor across GND and PWR |
|Piezo Buzzer|Connect Piezo's positive lead to D2 (GPIO4) on the NodeMCU board<br>Connect the Peizo's negative lead to GND|
|Micro Button Switch<br>and 10k ohm resistor|Connect one of the switch leads to the D1 pin on the NodeMCU board<br>Connect the other switch lead to GND on the NodeMCU board<br>Connect D1 pin on the NodeMCU board to VIN via 10k ohm resister|
|Project Box|If desired for aesthetics. The entire hardware build can fit inside an ABS Plastic Project Box measuring 3.15 x 1.97 x 1.02 inch (80 x 50 x 26 mm). <br><br>Creating a 1-3/8 inch round hole in the face of the box allows the Neopixel ring to be mounted on the inside of the box with the pixels facing outward. The box will also need a small opening on one side for a USB power cable to connect to the micro USB port on the NodeMCU board.|

## Wiring Diagram

![Diagram of GWIDC Hardware Components and Wiring](images/GWIDC-Hardware-Diagram.jpg)


## Firmware

Once the hardware build is complete, upload the latest GWIDC software release via the USB serial port on the NodeMCU board (see [`src/README.md`](src/README.md)). 

Though the initial software upload must be done via NodeMCU serial port, the GWIDC does have an Over-the-Air (OTA) update capability. Once the GWIDC is connected to a WiFi network, new software releases can be uploaded over WiFi using the Arduino IDE.

<br>

## Photos of GWIDC in a Project Box

![GWIDC Photo 1](images/GWIDC-Pic1.jpeg)  ![GWIDC Photo 2](images/GWIDC-Pic2.jpeg)  ![GWIDC Photo 3](images/GWIDC-Pic3.jpeg)

