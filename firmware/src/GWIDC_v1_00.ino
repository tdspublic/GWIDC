// [G]eneral-purpose [W]iFi-connected [I]ndicator [D]evi[C]e (GWIDC)

// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Timothy Sakulich

//  Changelog:
//    1.0 (12/10/2025)
//      - Initial Release

//These definitions are used to embed sketch information in the code itself
#define BUILDVERSION "GWIDC_v1.00"
#define SKETCHNAME __FILE__
#define BUILDDATE __DATE__
#define BUILDTIME __TIME__

// The definition "DEBUG" leverages the Arduino IDE macros capability.
// When defined, the compiler includes additional code that serves to 
// generate verbose debugging messages through the serial port. 
// Recommend commenting out this line for final compilation of the sketch:
#define DEBUG


// The definition "ALLOW_REMOTE_RESET" leverages the Arduino IDE macros 
// capability. When defined, the compiler includes additional code that provides 
// capability to clear all WiFi credentials via HTTP using a RESET command. While 
// useful during testing, this feature was not intended to be permanent. 
// Recommend commenting this line out for final compilation of the sketch:
//#define ALLOW_REMOTE_RESET


/*

This sketch was developed as the software core to an indicator device
comprised of an NodeMCU 1.0 Development Board connected to a NeoPixel Ring,
(12 Pixel) piezo buzzer, and microswitch button. The device is controllable 
over WiFi through HTTP messages.

When properly configured with a home automation hub IP address and port
on the same local network, the device can serve a remote alert or
indicator triggered by home automation applications and rules. This
sketch was designed and tested in combination with a Hubitat C-8 hub
running a custom device driver written in Groovy.

KEY FEATURES:

- User supplies the WiFi SSID and password credentials through an access
  portal created by the device using functionality provided by the
  WiFiManager.h library. This avoids hard coding WiFi credentials into
  the sketch. The access portal is named GWIDC_AP, and appears at local
  IP address of 192.168.4.1. Using functionality of the preferences.h
  library, WiFi credentials are saved to flash memory and persist across
  reboots or network disconnects until a manual RESET by the user (see
  below)
- User can supply the IP address and port of a home automation hub,
  enabling the device to be used by the hub as a remote indicator. This
  specific sketch was designed and tested in combination with a Hubitat
  C8 hub and a custom device driver written in Groovy. The hub IP and
  Port information are also saved to flash memory and persist across
  device reboots until a manual RESET by the user (see below) or changed
  via HTTP message (see below)
- Manual RESET - When a normally open button is connected between D1 and
  GND on the NodeMCU board, pressing that button will clear WiFi
  credentials and the home automation hub IP/Port information from flash
  memory and reboot the device to the GWIDC_AP access portal.
- The sketch includes over-the-air updating capability using the
  ArduinoOTA.h library. This allows a new compiled code to be uploaded
  to the GWIDC device over WiFi via the Arduino IDE without having to
  access the physical serial port on the NodeMCU board. (Of course, it
  will be important for the new sketch to continue to provide OTA
  functionality as well!)

HARDWARE COMPONENTS

- NodeMCU 1.0 Development Board - USB power through the micro USB port
  on the NodeMCU board, or use a separate 5 VDC power supply
  connected to VIN and GND on the NodeMCU board.
- Neopixel Ring (12 pixel) - Connect Neopixel GND to GND;
  Neopixel PWR to VIN on the NodeMCU Board; and Neopixel IN to D7
  (GPIO13) on the NodeMCU board via 330 ohm resistor. Include a 1000uF
  Capacitor across the Neopixel GND and PWR
- Piezo Buzzer - Connect positive lead to D2 (GPIO4) on the NodeMCU board
  and the negative lead to GND
- Micro Button Switch - Connect 10k ohm resister between the NodeMCU VIN
  and D2 pins. Connect one terminal of the microswitch to D2 and the
  other to GND.

SUMMARY OF STANDALONE HTTP COMMANDS

Syntax is http://<GWIDC_IPAddress><Command>

- BEEP - Briefly sounds the piezo buzzer.
- HUB_CONFIG - Enables run time change to the home automation hub IP and
  port (i.e., Hubitat) values stored on the GWIDC without having to
  RESET the device to WiFi configuration mode.
- POLL - Returns a formatted JSON message with current device settings.
- REPORT_SETTINGS - Returns a plain text report of device connection,
  hub IP/port values, and device status variables. Designed for use with
  a web browser to help with routine diagnostics and troubleshooting.
- STARTUP - Commands the GWIDC to send a formatted JSON message to the
  home automation hub IP and port (if configured), notifying the hub
  that the device is in STARTUP mode.
- VERSION_INFO - Returns device hardware ID, build version, and the
  build date & time.

SUMMARY OF GWIDC DISPLAY MODES - VIA HTTP MESSAGE

The following set the pixel display mode for the device under normal
operations. Syntax is http://<GWIDC_IPAddress>/?<Display_Mode>

- PATTERN=<value> Sets the color of individual pixels, where
  <value> is a 12-character string where each character corresponds to
  a pixel position, and can be one of R (red) G (green) B (blue) Y
  (yellow) C (cyan) P (purple) W (white) or O (off). Other values and
  additional characters are ignored.
- FLASH - Flashing effect using the current color setting.
- PULSE - Pulsating effect using the current color setting.
- SOLID - All pixels display the current color setting.
- STROBE - Strobing effect using the current color setting.
- TRAIL - Creates effect of a dot moving around the ring with a trail
  of fading pixels behind it using the current color setting.
- SOS - Flashes with an S.O.S. pattern
- RAINBOW - Simply for fun! Displays a rotating color rainbow.

SUMMARY OF GWIDC DISPLAY MODE PARAMETERS - VIA HTTP MESSAGE

The following can be used to modify the behavior of the device's current
display mode under normal operations. Basic syntax is
http://<GWIDC_IPAddress>/?<Parameter>=<value>

- LEVEL=<value> - Sets the overall brightness of the Neopixel
  ring (value must be in the range 0 - 100)
- SPEED=<value> - Sets the "speed" of the display mode effect 
  for modes other than SOLID and PATTERN
  where 0 = slowest and 4 = fastest (value must be in the range 0 - 4)
- RGB=<value> - Sets the primary pixel color, where <value> is a
  6-character string of hexadecimal digits representing the RGB color
  (omit the hashmark \# prefix).
- TONE=<value> - Sets the piezo buzzer to active (value=1) or off
  (value=0)

Display mode parameters can be set individually or in combination, using
an ampersand ("&") as a separator character in the HTTP string.

Syntax is
http://<GWIDC_IPAddress>/?<Display_Mode>&<Parameter1>=<value1>&<Parameter2>=<value2>.

*/


//Multiple libraries are required for this sketch
#include <string.h>
#include <Preferences.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h> 
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>


//
//Define static values for hardware and connections to the NodeMCU

//Neopixel device -- data pin, global constants and global variables
#define NEOPIXEL_DATA_PIN D7   // NodeMCU GPIO pin connected to the NeoPixel
#define NUM_PIXELS 12          // number of NeoPixels in the ring
#define FIXED_PIXEL_INTERVAL 3 // defines the pixel interval used in STROBE
                               // e.g., FIXED_PIXEL_INTERVAL 3 strobes every 3rd pixel
Adafruit_NeoPixel strip(NUM_PIXELS, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);

//Reset button
#define BUTTON_PIN D1          // NodeMCU pin connected to the button
const long LONG_PRESS_DURATION = 5000; // 5 seconds in milliseconds
const long WIFI_CHECK_INTERVAL = 5000; // 5 seconds in milliseconds
long buttonPressTime = 0;

//Piezo buzzer -- pin connection, global constants and global variables
#define PIEZO_BUZZER D2       // used for piezo buzzer
bool piezo_Active = false;    // piezo active default to off

//Onboard LED
#define LED_ONBOARD D4        // NodeMCU Built in LED 


//
//Definitions for WiFi and Home Automation hub connections
char TargetHub_URL[80];

char hub_ip[16]; // For IP address (e.g., "192.168.1.100")
char hub_port[6]; // For port number (e.g., "8080")
bool hub_validated = false; // set to true once a valid home automation hub IP and Port is provided

//
//Define the WiFi and HTTP capabilities
ESP8266WebServer server(80); //Server on port 80

// Global variables for WiFi event handlers
// These must be global so their state can be modified outside of setup()
WiFiEventHandler onDisconnectedHandler;
WiFiEventHandler onConnectedHandler;

// Initialize WiFiManager
WiFiManager wifiManager;
WiFiClient wificlient;
HTTPClient httpclient;

// Declare a preferences instance for saving data to flash memory
// to enable persistence across reboots
Preferences preferences;


//
//Declare variables that serve as parameters for device settings. 
//These paraeters are exposed and set via HTTP messages for device control.
char name_Param[10];      // Updated after WiFi connection is established
char mode_Param[40];      // Current dsiplay mode for the device
char rgb_Param[7];      // Current RGB color (HEX)
char pattern_Param[NUM_PIXELS+1];
uint16_t level_Param = 50; // Current brightness level in the range 0-100
uint16_t speed_Param = 2; // Current speed setting (integer value for lookup tables)
char tone_Param[4];       // ON or OFF -- set in combination with piezo_Active


//
// Misc global variables and constants
uint16_t strip_Brightness;      //actual Neopixel brightness in range 0-255 
uint16_t timing_Pattern = 0;
uint16_t current_trail_pixel = 0;  // Needed for TRAIL mode
uint16_t TRAIL_FADE = 30;        // Amount of color value to decrement each successive pixel

/// Declare global constants and variables for managing PULSE patterns in the main loop
uint16_t pulse_strip_Brightness = 0; // will be calculated in real time
uint16_t pulse_Range = 0;       // will be calculated in real time
uint16_t pulse_Direction = 1;   // will toggle in real time

/// rainbow_Timer is for the RAINBOW indicator state
uint16_t rainbow_Timer = 0;

// Timing Patterns - this is a series of numbers defining the interval between 
// toggling pixels/piezo buzzer. A zero before the end of the pattern 
// indicates a short pattern that restarts when it detects a zero. 
// NOTE: Undeclared global array elements automatically initialize to zeros
const byte NUM_TIMING_PATTERNS = 6;  // number of patterns
const byte MAX_PATTERN_LEN = 18;     // max pattern length
const uint16_t pattern[NUM_TIMING_PATTERNS][MAX_PATTERN_LEN] =
{ {1,0},             // constant for SOLID, PATTERN, RAINBOW
  {1,1,0},           // even tempo for FLASH, PULSE, TRAIL
  {10,1,0},          // strobe pattern 1 for STROBE
  {10,1,1,1,0},      // strobe pattern 2 for STROBE
  {15,1,1,1,1,2,0},  // strobe pattern 3 for STROBE
  {7, 1,1,1,1,1, 2, 3,2,3,2,3, 2, 1,1,1,1,1},  //for SOS pattern
};

// The speed of the effect for a given mode is determined by the values in the following 
// lookup tables. The values are msptu = milliseconds per timing unit.  These values have 
// been tested visually to ensure the effects (e.g., FLASH) are neither too fast nor too 
// slow to be useful or pleasing
const uint16_t NUM_SPEEDS = 5;  // Number of speeds contained in the following lookup tables
const uint16_t msptu_for_FLASH[] = {1200,800,400,200,100};
const uint16_t msptu_for_PULSE[] = {120,90,60,30,15};
const uint16_t msptu_for_RAINBOW[] = {30,20,10,5,2};
const uint16_t msptu_for_SOS[]     = {250,200,150,80,60};
const uint16_t msptu_for_STROBE[]  = {300,200,100,50,25};
const uint16_t msptu_for_TRAIL[]   = {110,90,70,40,20};

bool flag_pattern_restart = true;

// Declare global variables for tracking time in the main loop
byte step = 0;                // current step in pattern default to start
unsigned long current_Time;   // current time for test
unsigned long start_Time;     // start time of this step

uint16_t ms_per_unit = 0;          // initialize the timing per unit


//******************************************************************************
//********************************** SETUP *************************************
//******************************************************************************

void setup() {
  Serial.begin(115200);
  delay(10);

  #ifdef DEBUG
  Serial.println("\nRunning Setup()");
  #endif

  //Onboard LED port Direction output
  pinMode(LED_ONBOARD, OUTPUT);
  digitalWrite(LED_ONBOARD, HIGH); //LED off

  //Piezo Direction output
  pinMode(PIEZO_BUZZER, OUTPUT);
  digitalWrite(PIEZO_BUZZER, LOW); //Piezo Buzzer off

  // Blink the onboard LED to indicate startup
  digitalWrite(LED_ONBOARD, LOW);
  delay(200);
  digitalWrite(LED_ONBOARD, HIGH);

  //NEOPIXEL initialize strip
  strip.begin();

  // Configure the button pin with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Check if button is held down at boot to perform a factory reset
  // This is a robust method to catch a factory reset on startup.
  if (digitalRead(BUTTON_PIN) == LOW) {

    #ifdef DEBUG
    Serial.println("Button held on startup. Restarting to GWIDC_AP access portal...");
    #endif

    // Turn off all pixels
    strip.clear();
    strip.show(); 

    //Turn off wifi event handlers 
    onConnectedHandler = nullptr;
    onDisconnectedHandler = nullptr;
    // Erase saved WiFi credentials and restart the GWIDC device.
    wifiManager.resetSettings();        
    ESP.restart(); 
  }
  

  // NEOPIXEL Indicate Initializing WiFi (yellow at pixel 0)
  strcpy(mode_Param, "BOOTING");
  const int temp_strip_Brightness = 40;
  strip.setBrightness(temp_strip_Brightness);
  strip.clear();
  strip.setPixelColor(0, 255, 255, 0);
  strip.show(); 

  // If there no saved SSID and password then call the configuration portal
  // to obtain new credentials and reboot
  if (!wifiManager.getWiFiIsSaved()) ConfigurationPortal();

  // If execution gets this point, then there are saved SSID and password credentials.
  // So attempt to connect. 

  // Do NOT start access portal on failure to connect.  
  // If connection keeps failing, the user may have to disconnect power to the GWIDC device  
  // and force a manual reset. (Press the reset button while reconnecting power).

  wifiManager.setEnableConfigPortal(false);  
  wifiManager.setConnectTimeout(60); 
  wifiManager.autoConnect("GWIDC_AP");
  
  WiFi.hostname().toCharArray(name_Param, sizeof(name_Param));
  //name_Param = WiFi.hostname();

  
  //NEOPIXEL Indicate WiFi Connected (green at pixel 0)
  strip.clear();
  strip.setPixelColor(0, 0, 255, 0);
  strip.show();         


  #ifdef DEBUG
  Serial.print("Device is connected to ");
  Serial.print(WiFi.SSID());
  Serial.print(" with IP address ");
  Serial.print(WiFi.localIP());
  Serial.print (" as ");
  Serial.print (WiFi.hostname());
  Serial.print(" with MAC Address: ");
  Serial.println(WiFi.macAddress()); 
  //NOTE -- The following lines should only be used for specific diagnostics
  //Serial.print ("Password /");
  //Serial.print(WiFi.psk());
  //Serial.println ("/");
  #endif

  //Retrieve Target Hub IP and Port from flash and set up the Target Hub URL
  //open in read only mode (second parameter = True)
  preferences.begin("TargetHub_Config", true);

  hub_validated = preferences.getBool ("hub_validated", false);
  if (hub_validated) {
      String tempString = preferences.getString("hub_ip", "Not Found");
      tempString.toCharArray(hub_ip,sizeof(hub_ip));

      tempString = preferences.getString("hub_port", "Not Found");
      tempString.toCharArray(hub_port,sizeof(hub_port));

      tempString = "http://" + String(hub_ip) + ":" + String(hub_port) + "/";
      tempString.toCharArray(TargetHub_URL,sizeof(TargetHub_URL));
      
      // Use the validated IP and port

      { 
        #ifdef DEBUG     
        Serial.print("Server IP: ");
        Serial.println(hub_ip);
        Serial.print("Server Port: ");
        Serial.println(hub_port);
        Serial.print("Device set to send http messages to ");
        Serial.println (TargetHub_URL);
        #endif
      }
  }
  else {
    #ifdef DEBUG
    Serial.println ("No Target Server ");
    #endif

    strcpy(TargetHub_URL,"<< Not Set >>");
  }

  preferences.end();

  //
  //  ****** THE FOLLOWING CODE MUST BE INCLUDED FOR OTA UPDATES ******
  // The following code enables over-the-air flashing of the ESP8266 NodeMCU sketch 
  // (without connection to serial port)


  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  Serial.println("OTA Ready");

  // Declare the handlers for HTTP calls to the GWIDC device IP
  server.on("/", handleRoot);
  server.on("/BEEP", handleBEEP);
  server.on("/HUB_CONFIG", handleHUB_CONFIG);
  server.on("/POLL", handlePOLL);
  server.on("/REPORT_SETTINGS", handleREPORT_SETTINGS);
  server.on("/VERSION_INFO", handleVERSION_INFO);
  server.on("/STARTUP", handleSTARTUP);

  #ifdef ALLOW_REMOTE_RESET
  server.on("/RESET", handleRESET);
  #endif

  server.begin(); //Start server

  Serial.println("HTTP server started");

  //NEOPIXEL Indicate completed setup() (blue at pixel 0)
  strcpy(mode_Param, "READY");
  strip.clear();
  strip.setPixelColor(0, 0, 0, 255);
  strip.show();  

  //Initialize reamaing global parameters used to control the GWIDC device
  for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i]='-';}
  pattern_Param[NUM_PIXELS+1] = '\0';  // terminate with null char
  strcpy(rgb_Param,"000000");
  level_Param = 100;
  strip_Brightness = scale100to255 (level_Param);
  speed_Param = 2;
  strcpy(tone_Param, "OFF");
  piezo_Active = false;
  
  // Do a loop for one second
  unsigned long temppreviousMillis = millis();
  while ((temppreviousMillis - millis()) < 1000); {}

  // If the home automation hub IP & port is validated, Make an initial call to TargetHub_URL;
  if (hub_validated) handleSTARTUP();
}


//******************************************************************************
//********************************** LOOP **************************************
//******************************************************************************

void loop() {

  ArduinoOTA.handle();   //Handle OTA request
  
  server.handleClient();   // Listen for incoming HTTP traffic

  // Check for button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (buttonPressTime == 0) {
      buttonPressTime = millis();

      Serial.println("Button pressed. Hold for 5 seconds to clear preferences.");
    }
    
    // Check for a long press
    if (millis() - buttonPressTime >= LONG_PRESS_DURATION) {
      Serial.println("Long press detected! Calling Reset Configuration Portal...");
        // Turn off all pixels
        strip.clear();
        strip.show(); 
        //Turn off wifi event handlers 
        onConnectedHandler = nullptr;
        onDisconnectedHandler = nullptr;
        // Erase saved WiFi credentials and restart the GWIDC device.   
        wifiManager.resetSettings();   
        ESP.restart(); 
    }
  } else {
    // Reset the press timer when the button is released
    buttonPressTime = 0;
  }


  current_Time = millis();      // get current time for compare

  // The TIMER variables are counters used to create various effects in the lighting/beeper
  // as the code loops, dependending the state of the indicator
  if ( ((current_Time - start_Time) > pattern[timing_Pattern][step]*ms_per_unit) || flag_pattern_restart ) {

    flag_pattern_restart = false;

    if (strcmp(mode_Param,"RAINBOW") == 0) {
      strip.setBrightness(strip_Brightness);
      rainbow_Timer = (rainbow_Timer + 256) % 65536;
      strip.rainbow(rainbow_Timer);
      strip.show();
    }

    else if (strcmp(mode_Param,"PATTERN") == 0) {
      strip.setBrightness(strip_Brightness);
      for(int i=0; i< NUM_PIXELS; i++){
        if      (pattern_Param[i] == 'O') strip.setPixelColor(i,0,0,0);
        else if (pattern_Param[i] == 'R') strip.setPixelColor(i,255,0,0);
        else if (pattern_Param[i] == 'G') strip.setPixelColor(i,0,255,0);
        else if (pattern_Param[i] == 'B') strip.setPixelColor(i,0,0,255);
        else if (pattern_Param[i] == 'Y') strip.setPixelColor(i,255,255,0);
        else if (pattern_Param[i] == 'C') strip.setPixelColor(i,0,255,255);
        else if (pattern_Param[i] == 'P') strip.setPixelColor(i,255,0,255);
        else if (pattern_Param[i] == 'W') strip.setPixelColor(i,255,255,255);  
        //simply ignore other values            
      }
      strip.show();
    }

    else if (strcmp(mode_Param,"FLASH") == 0) { 
      strip.setBrightness(strip_Brightness);
      if (step % 2 == 0) set_ring_color (rgb_Param);
      else set_ring_color ("000000");
      strip.show();
    }

    else if (strcmp(mode_Param,"PULSE") == 0) { 
      pulse_strip_Brightness = pulse_strip_Brightness + pulse_Direction;
      // have to recalculate each iteration for whatever the current strip_Brightness is set to
      pulse_Range = 2*strip_Brightness/3;

      // If reached the max upper level (i.e., the value of strip_Brightness), start dimming
      if (pulse_strip_Brightness > strip_Brightness) {
        pulse_strip_Brightness = strip_Brightness;
        pulse_Direction = -1;
      }

      // If reached the max lower level, start brightening
      else if (pulse_strip_Brightness < strip_Brightness - pulse_Range) {
        pulse_strip_Brightness = strip_Brightness - pulse_Range;
        pulse_Direction = 1;
      }
      
      strip.setBrightness(pulse_strip_Brightness);
      colorWipe(hexToUint32(rgb_Param),0);
      strip.show();
    }

    else if (strcmp(mode_Param,"SOLID") == 0) {
      strip.setBrightness(strip_Brightness);
      set_ring_color (rgb_Param);
      strip.show();
    }

    else if (strcmp(mode_Param,"SOS") == 0) { 
      strip.setBrightness(strip_Brightness);
      if (step % 2 == 0) set_ring_color (rgb_Param);
      else set_ring_color ("000000");
      strip.show();
    }

    else if (strcmp(mode_Param,"STROBE") == 0) {
      strip.setBrightness(strip_Brightness);
      set_ring_color (rgb_Param);
      if (step % 2 == 0) {
      for (int i = 0; i < NUM_PIXELS; i++) {
          if ((i % FIXED_PIXEL_INTERVAL) == 0) {
            strip.setPixelColor(i, strip.Color( 255, 255, 255));
          }
        }
      }
      strip.show();
    }

    else if (strcmp(mode_Param,"TRAIL") == 0) {
      strip.setBrightness(strip_Brightness); 
      if (step % 2 == 0) fadePixelTrail();
      strip.show();
    }
      
    if (piezo_Active) {
      digitalWrite(LED_ONBOARD, (step % 2 == 1)); // change led state
      digitalWrite(PIEZO_BUZZER, (step % 2 == 0)); //change piezo buzzer state
    }

    step++;  // next step

    start_Time = current_Time;  // start next led pattern step time
    if ((step >= MAX_PATTERN_LEN)                    // end of pattern
        || (pattern[timing_Pattern][step] == 0)) { // or short pattern
      // end of pattern processing
      step = 0;   //restart pattern
    }
  }

} // End of loop 


//******************************************************************************
//***************************** HTTP Handlers **********************************
//******************************************************************************

void handleBEEP() {
  String message = "============ Called handleBEEP()";
  IPAddress remoteIp = server.client().remoteIP(); // Get the IP address of the client

  #ifdef DEBUG
  Serial.println(message);
  Serial.print("IP of Requesting Client: ");
  Serial.println(remoteIp);
  #endif

  digitalWrite(LED_ONBOARD, LOW); //LED off
  digitalWrite(PIEZO_BUZZER, HIGH); //Piezo Buzzer off
  delay (40);
  digitalWrite(LED_ONBOARD, HIGH); //LED off
  digitalWrite(PIEZO_BUZZER, LOW); //Piezo Buzzer off
  
  // Send HTTP reply to requesting IP address
  server.send(200, "text/html", message);
}


void handleHUB_CONFIG (){
  String message = "============ Called handleHUB_CONFIG()";
  IPAddress remoteIp = server.client().remoteIP(); // Get the IP address of the client

  #ifdef DEBUG  
  Serial.println(message);
  Serial.print("IP of Requesting Client: ");
  Serial.println(remoteIp);
  #endif

  char temp_hub_ip[16];
  char temp_hub_port[6];

  bool validIP = false;
  bool validport = false;

  if (server.hasArg("CLEAR")) {
    // clear all previously stored values
    hub_validated = false;
    strcpy (hub_ip,"");
    strcpy (hub_port,"");
    strcpy (TargetHub_URL,"");

    // Save to flash memory
    preferences.begin("TargetHub_Config", false);
    preferences.putBool("hub_validated", hub_validated);
    preferences.putString("hub_ip", hub_ip);
    preferences.putString("hub_port", hub_port);
    preferences.end();

    message = ("Cleared home automation hub IP and Port values.");
  }

  else {
    if (server.hasArg("HUB_IP")) {
      String temp_string = server.arg("HUB_IP");
      int temp_string_length = temp_string.length();
      if (temp_string_length <= 15) {
        temp_string.toCharArray(temp_hub_ip,16);
        // IP Address Validation (basic check for format)
        IPAddress parsedIP;
        validIP = parsedIP.fromString(temp_hub_ip);
      }
    } 
    if (!validIP) {
      #ifdef DEBUG
      Serial.println("Invalid or missing IP address!");
      #endif
    }

    if (server.hasArg("HUB_PORT")) {
      String temp_string = server.arg("HUB_PORT");
      int temp_string_length = temp_string.length();
      if (temp_string_length <= 5) {
        temp_string.toCharArray(temp_hub_port,6);
        int port_num = atoi(temp_hub_port);
        validport = ((port_num >= 0) && (port_num <= 65535));
      }
    }
    if (!validport) {
      #ifdef DEBUG
      Serial.println("Invalid or missing port number!");
      #endif
    }

    hub_validated = validIP && validport;

    // If validation passes, update home automation hub IP & port and save to flash memory
    // otherwise make no changes to current values or flash memory
    if (hub_validated) {
      preferences.begin("TargetHub_Config", false);
      preferences.putBool("hub_validated", hub_validated);
      // Save to flash memory
      strcpy(hub_ip, temp_hub_ip);
      strcpy(hub_port, temp_hub_port);
      preferences.putString("hub_ip", hub_ip);
      preferences.putString("hub_port", hub_port);
      preferences.end();

      String tempString = "http://" + String(hub_ip) + ":" + String(hub_port) + "/";
      tempString.toCharArray(TargetHub_URL,sizeof(TargetHub_URL));

      message = "Device set to send http messages to " + String(TargetHub_URL);
      }
    else { 
      // Do not save hub_IP or hub_port to flash memory
      message = ("Home automation hub IP and Port values failed validation! No changes made.");
    }
  }
 
  #ifdef DEBUG
  Serial.println (message);
  #endif

  // Send HTTP reply to requesting IP address
  server.send(200, "text/html", message);

}


void handlePOLL() {
  String message = "============ Called handlePOLL()";

  IPAddress remoteIp = server.client().remoteIP(); // Get the IP address of the client

  #ifdef DEBUG
  Serial.println (message);
  Serial.print("IP of Requesting Client: ");
  Serial.println(remoteIp);
  #endif

  StaticJsonDocument<200> doc;
  doc["Name"]       = name_Param;
  doc["Mode"]       = mode_Param;
  doc["RGB"]        = rgb_Param;
  doc["Pattern"]    = pattern_Param;
  doc["Level"]      = level_Param;
  doc["Speed"]      = speed_Param;
  doc["Tone"]       = tone_Param;

  String jsonString;
  serializeJson(doc, jsonString);

  if (!remoteIp) { // If requesting IP is not set, then this is GWIDC device startup condition.
    // In that case, check to see if there is a validated HTTP server TargetHUB_URL to post to
    // If so, post message to that server
    if (hub_validated) {
      #ifdef DEBUG
      Serial.print ("Now Sending http json message: ");
      Serial.println(jsonString);
      #endif

      httpclient.begin(wificlient,TargetHub_URL);
      httpclient.POST (jsonString);
      httpclient.end();
      //NOTE to SELF: need to add error handling here

      #ifdef DEBUG
      Serial.print("Message sent to ");
      Serial.println (TargetHub_URL);
      #endif
      }
    
    else { // No validated server to post to
      #ifdef DEBUG
      Serial.println("No validated HTTP server to post to.");
      #endif
    }
  }
  else { // Reply to the requesting IP
    server.send(200, "application/json", jsonString);
  }

}



void handleREPORT_SETTINGS() {
  String message = "============ Called handleREPORT_SETTINGS()";
  IPAddress remoteIp = server.client().remoteIP(); // Get the IP address of the client

  #ifdef DEBUG
  Serial.println (message);
  Serial.print("IP of Requesting Client: ");
  Serial.println(remoteIp);
  #endif

  message = "****** DEVICE INFORMATION ****** <br><br>";

  message = message + "General-purpose WiFi-based Indicator DeviCe (GWIDC)" + "<br>";
  message = message + "Currently Running Software Version " + BUILDVERSION + "<br>";
  message = message + "Source Code " + SKETCHNAME + " ";
  message = message + "("+ BUILDDATE + "  " + BUILDTIME + ")<br><br>";

  message = message + "****** Current WiFI Connection and Target IP & Port ******" + "<br><br>";
  message = message + "SSID: " + WiFi.SSID() + "<br>";
  message = message + "IP address: " + WiFi.localIP().toString() + "<br>";
  const char* hostname_char_array = WiFi.getHostname();
  message = message + "Device Host name: " + String(hostname_char_array) + "<br>";
  message = message + "MAC Address: " + WiFi.macAddress() + "<br>"; 

  //message = message + "Target Hub Values Set: " + hub_validated + "<br>";
  if (hub_validated) message = message + "Target Hub Values Set: " + "Yes" + "<br>";
  else               message = message + "Target Hub Values Set: " + "No" + "<br>";

  message = message + "Target Hub IP: " + hub_ip + "<br>";
  message = message + "Target Hub Port: " + hub_port + "<br>";
  message = message + "Target Hub URL: " + TargetHub_URL + "<br><br>";
  message = message + "****** Current State Variables ******" + "<br><br>";
  message = message + "Name: "    + name_Param + "<br>";
  message = message + "Mode: "    + mode_Param + "<br>";
  message = message + "RGB: "     + rgb_Param + "<br>";
  message = message + "Pattern: " + pattern_Param + "<br>";
  message = message + "Level: "   + level_Param + "<br>";
  message = message + "Speed: "   + speed_Param + "<br>";
  message = message + "Tone: "    + tone_Param + "<br>";
 
  
  // Send HTTP reply to requesting IP address
  server.send(200, "text/html", message);
}


void handleVERSION_INFO() {
  String message = "============ Called handleVERSION_INFO()";
  IPAddress remoteIp = server.client().remoteIP(); // Get the IP address of the client

  #ifdef DEBUG
  Serial.println (message);
  Serial.print("IP of Requesting Client: ");
  Serial.println(remoteIp);
  #endif

  StaticJsonDocument<200> doc;
  doc["Hardware_ID"]     = name_Param;
  doc["Build_Version"]   = BUILDVERSION;
  doc["Build_Date"]      = BUILDDATE;
  doc["Build_Time"]      = BUILDTIME;

  String jsonString;
  serializeJson(doc, jsonString);
  
  // Send HTTP reply to requesting IP address
  server.send(200, "application/json", jsonString);
}


#ifdef ALLOW_REMOTE_RESET
void handleRESET() {
  String message = "Received RESET. ";
  message = message + "<br>Once the indicator light turns red, remove power from GWIDC device, then plug it back in.";
  message = message + "<br>The device will start up and create a local access portal named GWIDC_AP.";
  message = message + "<br>Connect to that portal in order to supply the new WiFi credentials.";

  #ifdef DEBUG
  Serial.println(message);
  #endif

  // Send HTTP reply to requesting IP address
  server.send(200, "text/html", message);

  // Turn off all pixels
  strip.clear();
  strip.show(); 

  //Turn off wifi event handlers 
  onConnectedHandler = nullptr;
  onDisconnectedHandler = nullptr;
  // Erase saved WiFi credentials and restart the GWIDC device.  
  wifiManager.resetSettings();      
  ESP.restart(); 
}
#endif


void handleRoot() {
  String message = "============ Called handleRoot()";
  IPAddress remoteIp = server.client().remoteIP(); // Get the IP address of the client

  #ifdef DEBUG  
  Serial.println(message);
  Serial.print("IP of Requesting Client: ");
  Serial.println(remoteIp);
  #endif
      
  if (server.hasArg("PATTERN")) {
    // Set to a pattern of colors using letter labels for each pixel
    // R (red) G (green) B (blue) Y (yellow) C (cyan) P (purple) W (white) O (off)
    
    String message = "Received PATTERN = " + server.arg("PATTERN");
    Serial.println(message);
    String argument_value = server.arg("PATTERN");

    //Ensure argument length is same as NUM_PIXELS, otherwise it is invalid input
    if (argument_value.length() == NUM_PIXELS) {
      argument_value.toUpperCase();
      for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i] = argument_value.charAt(i);}
      pattern_Param[NUM_PIXELS] = '\0';  // terminate with null char

      //Use the input to compute an "average" color across the Pattern
      uint32_t red = 0;
      uint32_t green = 0;
      uint32_t blue = 0;
      for(int i=0; i< NUM_PIXELS; i++){
        if      (pattern_Param[i] == 'O') {}
        else if (pattern_Param[i] == 'R') {red = red + 255;}
        else if (pattern_Param[i] == 'G') {green = green + 255;}
        else if (pattern_Param[i] == 'B') {blue = blue + 255;}
        else if (pattern_Param[i] == 'Y') {red = red + 255; green = green + 255;}
        else if (pattern_Param[i] == 'C') {green = green + 255; blue = blue + 255;}
        else if (pattern_Param[i] == 'P') {red = red + 255; blue = blue + 255;}
        else if (pattern_Param[i] == 'W') {red = red + 255; green = green + 255; blue = blue + 255;}
        //simply ignore other values    
      }
      red = round(red/NUM_PIXELS);
      green = round(green/NUM_PIXELS);
      blue = round(blue/NUM_PIXELS);
      rgbToHex(red, green, blue, rgb_Param);

      strcpy(mode_Param, "PATTERN");
      timing_Pattern = 0;
      ms_per_unit = 0;

      reset_for_new_pattern();
      
      #ifdef DEBUG
      Serial.print("Set to ");
      Serial.print(mode_Param);
      Serial.print(" (computed average rgb_Param is ");
      Serial.print(rgb_Param);
      Serial.println("). Checking for additional parameters.");
      #endif
    }
  }

  else if (server.hasArg("FLASH")) {
    // Flash with a single color (hexidecimal value) 
    String message = "Received FLASH";
    
    #ifdef DEBUG
    Serial.println(message);
    #endif

    // Set the pattern string to all 'X's
    for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i]='*';}
    pattern_Param [NUM_PIXELS] = '\0'; // terminate with null char

    strcpy(mode_Param, "FLASH");
    timing_Pattern = 1;  // use the timing pattern for FLASH
    ms_per_unit =  msptu_for_FLASH[speed_Param];

    // Check to see if "DEFAULT" was specified for this mode
    // if so, set the mode to a white FLASH at medium speed without piezo
    if (server.arg("FLASH") == "DEFAULT") {
      strcpy(rgb_Param,"FFFFFF");
      level_Param = 50;
      strip_Brightness = scale100to255(level_Param);
      speed_Param = 2; // medium speed
      ms_per_unit = msptu_for_FLASH[speed_Param]; 
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
    }

    reset_for_new_pattern();

    #ifdef DEBUG
    Serial.print("Set to ");
    Serial.print(mode_Param);
    Serial.println(". Checking for additional parameters.");
    #endif
  } 


  else if (server.hasArg("PULSE")) {
    // Breathing effect with a single color     
    // R (red) G (green) B (blue) Y (yellow) C (cyan) P (purple) W (white)
    String message = "Received PULSE";

    #ifdef DEBUG
    Serial.println(message); 
    #endif

    // Set the pattern string to all 'X's
    for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i]='*';}
    pattern_Param [NUM_PIXELS] = '\0'; // terminate with null char

    strcpy(mode_Param, "PULSE");
    timing_Pattern=1; // use the timing pattern for PULSE
    ms_per_unit =  msptu_for_PULSE[speed_Param];

    // Check to see if "DEFAULT" was specified for this mode
    // if so, set the mode to a white pulse at medium speed without piezo
    if (server.arg("PULSE") == "DEFAULT") {
      strcpy(rgb_Param,"FFFFFF");
      level_Param = 50;
      strip_Brightness = scale100to255(level_Param);
      speed_Param = 2; // medium speed
      ms_per_unit = msptu_for_PULSE[speed_Param]; 
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
    }

    reset_for_new_pattern();

    #ifdef DEBUG
    Serial.print("Set to ");
    Serial.print(mode_Param);
    Serial.println(". Checking for additional parameters.");
    #endif
  }
  

  else if (server.hasArg("SOLID")) {
    // Set to a single color (hexidecimal value)

    String message = "Received SOLID";
    #ifdef DEBUG    
    Serial.println(message);
    #endif
  
    // Set the pattern string to all 'X's
    for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i]='*';}
    pattern_Param [NUM_PIXELS] = '\0'; // terminate with null char

    strcpy(mode_Param, "SOLID");
    timing_Pattern=0;
    ms_per_unit = 0;

    // Check to see if "DEFAULT" was specified for this mode
    // if so, set the mode to white without piezo
    if (server.arg("TRAIL") == "DEFAULT") {
      strcpy(rgb_Param,"FFFFFF");
      level_Param = 50;
      strip_Brightness = scale100to255(level_Param);
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
    }

    reset_for_new_pattern();

    #ifdef DEBUG
    Serial.print("Set to ");
    Serial.print(mode_Param);
    Serial.println(". Checking for additional parameters.");
    #endif
  }



  else if (server.hasArg("SOS")) {
    // Flash with a single color (hexidecimal value) 
    String message = "Received SOS";
    
    #ifdef DEBUG
    Serial.println(message);
    #endif

    // Set the pattern string to all 'X's
    for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i]='*';}
    pattern_Param [NUM_PIXELS] = '\0'; // terminate with null char

    strcpy(mode_Param, "SOS");
    timing_Pattern = 5;  // use the timing pattern for SOS
    ms_per_unit =  msptu_for_SOS[speed_Param];

    // Check to see if "DEFAULT" was specified for this mode
    // if so, set the mode to a white FLASH at medium speed without piezo
    if (server.arg("SOS") == "DEFAULT") {
      strcpy(rgb_Param,"FFFFFF");
      level_Param = 50;
      strip_Brightness = scale100to255(level_Param);
      speed_Param = 2; // medium speed
      ms_per_unit = msptu_for_SOS[speed_Param];
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
    }

    reset_for_new_pattern();

    #ifdef DEBUG
    Serial.print("Set to ");
    Serial.print(mode_Param);
    Serial.println(". Checking for additional parameters.");
    #endif
  } 



  else if (server.hasArg("STROBE")) {
    // Strobe wtih a single color (hexidecimal value)
    String message = "Received STROBE";

    #ifdef DEBUG
    Serial.println(message);
    #endif

    // Set the pattern string to all '*'s, but with 'W's at the strobing pixels
    for (int i = 0; i < NUM_PIXELS; i++) {
      if ((i % FIXED_PIXEL_INTERVAL) == 0) {
        //strip.setPixelColor(i, 16, 16, 0);
        pattern_Param[i]='W';
        }
      else {
        //strip.setPixelColor(i, 0, 0, 0);
        pattern_Param[i]='*';
      }
    }
    pattern_Param [NUM_PIXELS] = '\0'; // terminate with null char

    strcpy(mode_Param, "STROBE");
    timing_Pattern = 3;  // Use the timing pattern for double strobe unless changed
                   // by a passed argument (see below)
    ms_per_unit =  msptu_for_STROBE[speed_Param];

    // Check to see if "DEFAULT" was specified for this mode
    // if so, set the mode to a white strobe at medium speed without piezo
    if (server.arg("STROBE") == "DEFAULT") {
      strcpy(rgb_Param,"222222");
      level_Param = 50;
      strip_Brightness = scale100to255(level_Param);
      speed_Param = 2; // medium speed
      ms_per_unit = msptu_for_STROBE[speed_Param]; 
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
    }
    // Check to see if a strobe pattern was specified for
    // this mode 
    else if (server.arg("STROBE") == "1") timing_Pattern=2;
    else if (server.arg("STROBE") == "2") timing_Pattern=3;
    else if (server.arg("STROBE") == "3") timing_Pattern=4;

    reset_for_new_pattern();

    #ifdef DEBUG
    Serial.print("Set to ");
    Serial.print(mode_Param);
    Serial.println(". Checking for additional parameters.");
    #endif
  }


  else if (server.hasArg("TRAIL")) {
    // Flash with a single color (hexidecimal value) 
    String message = "Received TRAIL";
    
    #ifdef DEBUG
    Serial.println(message);
    #endif

    // Set the pattern string to all 'X's
    for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i]='*';}
    pattern_Param [NUM_PIXELS] = '\0'; // terminate with null char

    strcpy(mode_Param, "TRAIL");
    timing_Pattern = 1;  // Use timing pattern for TRAIL
    ms_per_unit =  msptu_for_TRAIL[speed_Param];

    // Check to see if "DEFAULT" was specified for this mode
    // if so, set the mode to a white dot trail at medium speed without piezo
    if (server.arg("TRAIL") == "DEFAULT") {
      strcpy(rgb_Param,"FFFFFF");
      level_Param = 50;
      strip_Brightness = scale100to255(level_Param);
      speed_Param = 2; // medium speed
      ms_per_unit = msptu_for_TRAIL[speed_Param]; // medium speed
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
    }

    reset_for_new_pattern();

    #ifdef DEBUG
    Serial.print("Set to ");
    Serial.print(mode_Param);
    Serial.println(". Checking for additional parameters.");
    #endif
  } 


  else if (server.hasArg("RAINBOW")) {
    // Flash with a single color (hexidecimal value) 
    String message = "Received TRAIL";
    
    #ifdef DEBUG
    Serial.println(message);
    #endif


    strcpy(mode_Param, "RAINBOW");
    strcpy(rgb_Param,"FFFFFF");
    timing_Pattern = 0; 
    ms_per_unit =  msptu_for_RAINBOW[speed_Param];

    for(int i=0; i< NUM_PIXELS; i++) {pattern_Param[i]='*';}
    pattern_Param[NUM_PIXELS] = '\0';  // terminate with null char

    // Check to see if "DEFAULT" was specified for this mode
    if (server.arg("RAINBOW") == "DEFAULT") {
      level_Param = 50;
      strip_Brightness = scale100to255(level_Param);
      speed_Param = 2; // medium speed
      ms_per_unit =  msptu_for_RAINBOW[speed_Param];
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
    }

  reset_for_new_pattern();

  #ifdef DEBUG
    Serial.print("Set to ");
    Serial.print(mode_Param);
    Serial.println(". Checking for additional parameters.");
    #endif
  }


  if (server.hasArg("LEVEL")) {
    // Set the overall brightness level

    String message = "Received LEVEL = " + server.arg("LEVEL");

    #ifdef DEBUG
    Serial.println(message);
    #endif

    // Validate that the input is an integer in the right range
    // then set the level and computed brightness accordingly
    String temp_str = server.arg("LEVEL");
    if (isValidInteger (temp_str)) {
      int temp_int = temp_str.toInt();
      if (temp_int >= 0 && temp_int <= 100) {
        // input is valid, so set the level and brightness variables
        level_Param = temp_int;
        strip_Brightness = scale100to255 (level_Param);  //Scale to Neopixel range 0 - 255
        strip.setBrightness(strip_Brightness); 
      }
    }
    else { // Not a valid input, ignore the input
      #ifdef DEBUG
      Serial.println("Invalid input -- ignored.");
      #endif
    } 

  }


  if (server.hasArg("SPEED")) {
    //Set the speed of the display mode

    String message = "Received SPEED = " + server.arg("SPEED");

    #ifdef DEBUG
    Serial.println(message);
    #endif

    // Validate that the input is an integer in right range
    // then use the lookup table corresponding to the current 
    // display mode to set the timing ms_per_unit variable
    String temp_str = server.arg("SPEED");
    if (isValidInteger (temp_str)) {
      int temp_int = temp_str.toInt();
      if (temp_int >= 0 && temp_int <= (NUM_SPEEDS-1)) {
        speed_Param = temp_int; 
        // input is valid, so set the actual timing based on current mode
        if      (strcmp(mode_Param,"FLASH") == 0) ms_per_unit =  msptu_for_FLASH[speed_Param]; 
        else if (strcmp(mode_Param,"PULSE") == 0) ms_per_unit =  msptu_for_PULSE[speed_Param]; 
        else if (strcmp(mode_Param,"RAINBOW") == 0) ms_per_unit =  msptu_for_RAINBOW[speed_Param];
        else if (strcmp(mode_Param,"SOS") == 0) ms_per_unit =  msptu_for_SOS[speed_Param]; 
        else if (strcmp(mode_Param,"STROBE") == 0) ms_per_unit =  msptu_for_STROBE[speed_Param]; 
        else if (strcmp(mode_Param,"TRAIL") == 0) ms_per_unit =  msptu_for_TRAIL[speed_Param]; 
        else {} // do nothing -- timing not used in to PATTERN or SOLID modes
      }
    }
    else { // Not a valid input, ignore the input
      #ifdef DEBUG
      Serial.println("Invalid input -- ignored.");
      #endif
    } 

  }

  if (server.hasArg("RGB")) {
    //Set the flash pattern

    String message = "Received RGB = " + server.arg("RGB");

    #ifdef DEBUG
    Serial.println(message);
    #endif

    String argument_value = server.arg("RGB");

    //Ensure argument was 6 characters, otherwise it is invalid input
    if (argument_value.length() == 6) {
      argument_value.toUpperCase();

      //Attempt to set the color -- if valid RGB hex value, then update indicator state variables
      if (isvalid_RGBHEX_COLOR(argument_value.c_str())) {
        argument_value.toCharArray(rgb_Param, sizeof(rgb_Param)); 

        #ifdef DEBUG
        Serial.print("Set to rgb_Param ");
        Serial.println (rgb_Param);
        #endif
      }
    }
    else {// ignore the input and do nothing
      #ifdef DEBUG
      Serial.println("Invalid input. Checking for other parameters.");
      #endif
    }
  }


  if (server.hasArg("TONE")) {
    //Set the piezo on or off
    String message = "Received TONE = " + server.arg("TONE");

    #ifdef DEBUG
    Serial.println(message);
    #endif

    if (server.arg("TONE") == "ON") {
      strcpy(tone_Param, "ON");
      piezo_Active = true;
    }
    else if (server.arg("TONE") == "OFF"){
      strcpy(tone_Param, "OFF");
      piezo_Active = false;
      //immediately turn off
      digitalWrite(LED_ONBOARD, 1) ; // turn off light
      digitalWrite(PIEZO_BUZZER, 0); //turn off piezo
    }
    else {// ignore the input and do nothing
      #ifdef DEBUG
      Serial.println("Invalid input. Checking for other parameters.");
      #endif
    }
  }

  // Call handlePOLL() to communicate with TargetHub_URL client and HTTP Server
  // POLL() will handle HTTP reply to requesting IP address
  handlePOLL();
} 


void handleSTARTUP() {
  String message = "============ Called handleSTARTUP()";
  IPAddress remoteIp = server.client().remoteIP(); // Get the IP address of the client

  #ifdef DEBUG
  Serial.println (message);
  Serial.print("IP of Requesting Client: ");
  Serial.println(remoteIp);
  #endif

  strcpy(mode_Param, "STARTUP");
  digitalWrite(LED_ONBOARD, HIGH); //LED off
  digitalWrite(PIEZO_BUZZER, LOW); //Piezo Buzzer off
  const int temp_LED_LEVEL = 255;

  strip.setBrightness(temp_LED_LEVEL);

  // NEOPIXEL Indicate STARTUP state (blue pixels at fixed intervals around the NEOPIXEL)
  for (int i = 0; i < NUM_PIXELS; i++) {
    if ((i % FIXED_PIXEL_INTERVAL) == 0) {
      strip.setPixelColor(i, 0, 0, 16);
      pattern_Param[i]='B';
      }
    else {
      strip.setPixelColor(i, 0, 0, 0);
      pattern_Param[i]='O';
    }
  }
  // Set the "average" color of the Neopixel at this point
  // Not really necessary but provides consistency in reporting settings
  strcpy(rgb_Param,"000005");
  strip.show();   

  
  // Make initial attempt to communicate with Hubitat
  // POLL() will handle HTTP reply to requesting IP address
  handlePOLL();
}


//******************************************************************************
//****************************** MISC Functions ********************************
//******************************************************************************

int scale100to255 (int input_val) {
  // Convert the input from 0-100 range (used by the LEVEL= parameter) to the 
  // 0-255 range (needed by Neopixel strip.brightness()). On the Neopixel, 
  // small incremental changes in the numerical brightness are perceptually 
  // more dramatic at the lower end of the range (closer to 0) than at the 
  // higher end (closer to 255). In recognition of that, this sketch uses a 
  // nonlinear quadratic formula for the conversion. At the low end of the 
  // range, small changes in LEVEL result in small changes in the calculated
  // brightness value. At the high end of the scale, small changes in LEVEL 
  // result in much larger changes to the calculated brightness value.

  // The quadratic curve passes through (x,y) coordinates (0,0) and (100,255) 
  // with slope=1 at (0,0). The equation is y = 0.0155*x^2 + x. 

  // Using float for the calculations prevents integer truncation.
  float y_float = 0.0155 * (float)input_val * (float)input_val + (float)input_val;

  //Now round to nearest integer
  int y_integer = round(y_float);

  return y_integer;
}

/*
int scale255to100 (int input_val) {
  // This is the inverse function of scale100to255(). It converts the 0-255 range 
  // (used for Neopixel strip.brightness()) back to the 0-100 range (used by the 
  // LEVEL= parameter). While not used by GWIDC functionality as currently 
  // designed, the funciton is provided here for future reference in the event 
  // that new device functionality can make use of it.

  // Using float for the calculations prevents integer truncation. 

  float coefficient = 0.0155;
  float result = (float)input_val/coefficient + (0.5/coefficient)*(0.5/coefficient);
  result = sqrtf(result) - (0.5/coefficient);
  int x_integer = round(result);
  return x_integer;
}
*/

// Helper function to check if a String contains only digits
bool isValidInteger(String str) {
  if (str.length() == 0) return false;
  // Check for an optional sign at the beginning
  int startIndex = 0;
  if (str.charAt(0) == '+' || str.charAt(0) == '-') {
    startIndex = 1;
  }
  // Check that all subsequent characters are digits
  for (int i = startIndex; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) return false;
  }
  return true;
}

// Helper function to check if a char array is a valid hexadecimal
// RGB color value
bool isvalid_RGBHEX_COLOR(const char *data) {
  int red, green, blue;
  int result = sscanf(data, "%02x%02x%02x", &red, &green, &blue);
  if (result == 3) {
    return true;
  }
  else return false;
}


uint32_t hexToUint32(const char* hexString) {
  // strtoul(string, endptr, base)
  // string: The C-style string containing the hexadecimal number.
  // endptr: A pointer to a char pointer that will point to the first character
  //         in the string that is not part of the number. Can be NULL if not needed.
  // base: The base of the number (16 for hexadecimal).
  return strtoul(hexString, NULL, 16);
}


void rgbToHex(int r, int g, int b, char* hexColor) {
  // Ensure values are within the 0-255 range
  r = constrain(r, 0, 255);
  g = constrain(g, 0, 255);
  b = constrain(b, 0, 255);

  // Use sprintf to format the hexadecimal string into the char array
  // %02X ensures two uppercase hexadecimal digits are used for each component
  sprintf(hexColor, "%02X%02X%02X", r, g, b);
}

bool set_ring_color(const char *data) {
  int red, green, blue;
  int result = sscanf(data, "%02x%02x%02x", &red, &green, &blue);
  if (result == 3) {
    colorWipe(strip.Color(red,   green,   blue), 0); // Red
    return true;
  }
  else return false;
}


void computeAverageColorHex(char* hexColorBuffer) {
  unsigned long totalRed = 0;
  unsigned long totalGreen = 0;
  unsigned long totalBlue = 0;

  // Iterate through each pixel and sum up the R, G, and B components
  for (int i = 0; i < NUM_PIXELS; i++) {
    uint32_t pixelColor = strip.getPixelColor(i);

    totalRed += (pixelColor >> 16) & 0xFF;   // Extract Red component
    totalGreen += (pixelColor >> 8) & 0xFF;  // Extract Green component
    totalBlue += pixelColor & 0xFF;          // Extract Blue component

    // NOTE TO SELF
    String message = "COMPUTE Pixel ";
    message = message + i + "  " + pixelColor;
    Serial.println (message); 
  }

  // Calculate the average R, G, and B values
  byte avgRed = totalRed / NUM_PIXELS;
  byte avgGreen = totalGreen / NUM_PIXELS;
  byte avgBlue = totalBlue / NUM_PIXELS;

  // Format the average RGB values into a hex string (e.g., "RRGGBB")
  sprintf(hexColorBuffer, "%02X%02X%02X", avgRed, avgGreen, avgBlue);
}


// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
}


// Fades a trail of pixels
void fadePixelTrail() {
     // Move the dot forward
    // Advance to the next pixel, wrapping around the ring
    current_trail_pixel = (current_trail_pixel + 1) % NUM_PIXELS;

    // Set the new pixel to the trail color
    strip.setPixelColor(current_trail_pixel, hexToUint32(rgb_Param));

    // Fade the trail
    for (int i = 0; i < NUM_PIXELS; i++) {
    // Get the current color of the pixel
    uint32_t color = strip.getPixelColor(i);
    // Extract individual R, G, B components
    byte red = (color >> 16) & 0xFF;
    byte green = (color >> 8) & 0xFF;
    byte blue = color & 0xFF;
    
    // Reduce brightness of each color component
    red = (red > TRAIL_FADE) ? red - TRAIL_FADE : 0;
    green = (green > TRAIL_FADE) ? green - TRAIL_FADE : 0;
    blue = (blue > TRAIL_FADE) ? blue - TRAIL_FADE : 0;
    
    // Set the new, faded color
    strip.setPixelColor(i, strip.Color(red, green, blue));
    }
}


void reset_for_new_pattern() { // used to reset when starting new timing or pixel modality
      #ifdef DEBUG
      Serial.println("Resetting for new pattern");
      #endif

      flag_pattern_restart = true;
      start_Time = millis();
      step = 0;                       // start at beginning of timing pattern      
      current_trail_pixel = 0;        // start at the top
      // Clear other pixel states 
      // This is mainly to clear the "trail" of pixels if TRAIL was previous mode
      strip.clear();
      strip.show(); 
      

}    



//******************************************************************************
//********* Functions to Manage Saved WiFi Credentials and Preferences *********
//******************************************************************************


void ConfigurationPortal() {

  #ifdef DEBUG
  Serial.println("Resetting WiFi and Preferences...");
  #endif

  //Turn off wifi event handlers 
  onConnectedHandler = nullptr;
  onDisconnectedHandler = nullptr;
  //First, double check to erase any saved WiFi credentials
  wifiManager.resetSettings();  

  preferences.begin("TargetHub_Config", false);
  preferences.clear();
  preferences.end();

  digitalWrite(LED_ONBOARD, LOW);
  //NEOPIXEL Indicate ConfigurationPortal (red at pixel 0)
  const int temp_LED_LEVEL = 255;
  const int pixel_brightness = 16;
  strip.setBrightness(temp_LED_LEVEL);
  strip.clear();
  strip.setPixelColor(0, pixel_brightness, 0, 0);
  strip.show(); 

  // Clear out all old preferences

  #ifdef DEBUG
  Serial.println("Clearing preferences...");
  #endif


  // Set up On Demand Access Portal to obtain SSDI, PW and preferences to save
  preferences.begin("TargetHub_Config", false);

  wifiManager.setPreSaveConfigCallback(saveConfigCallback);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setConfigPortalTimeout(180);  //180 seconds


  WiFiManagerParameter custom_ip_param("hub_ip", "Hub IP", hub_ip, 15);
  WiFiManagerParameter custom_port_param("hub_port", "Hub Port", hub_port, 5);

  wifiManager.addParameter(&custom_ip_param);
  wifiManager.addParameter(&custom_port_param);


  //wifiManager.setCustomHeadElement("<style>body{max-width:500px;margin:auto}</style>");
  wifiManager.setConnectTimeout(60);
  if (!wifiManager.startConfigPortal("GWIDC_AP")) {

      #ifdef DEBUG
      Serial.println ("Unable to connect to SSID.  Robooting...");
      #endif

      //flash red LED here?
      ESP.restart(); 
  }
  #ifdef DEBUG
  Serial.println ("Control Returned from GWIDC_AP access portal.");
  #endif

  // Retrieve values from custom parameters
  strcpy(hub_ip, custom_ip_param.getValue());
  strcpy(hub_port, custom_port_param.getValue());


  // IP Address Validation (basic check for format)
  IPAddress parsedIP;
  bool validIP = parsedIP.fromString(hub_ip);
  if (!validIP) {

    #ifdef DEBUG
    Serial.println("Invalid IP address format!");
    #endif
  }

  // Port Validation (check if it's a valid number)
  int port_num = atoi(hub_port);
  bool validport = ((port_num >= 0) && (port_num <= 65535));
  if (!validport) {

    #ifdef DEBUG
    Serial.println("Invalid port number!");
    #endif
  }

  hub_validated = validIP && validport;
  preferences.putBool("hub_validated", hub_validated);

  // If validation passes, use the IP and port for your application logic
  if (hub_validated) {
    // Save to flash memory
    preferences.putString("hub_ip", hub_ip);
    preferences.putString("hub_port", hub_port);


    String tempString = "http://" + String(hub_ip) + ":" + String(hub_port) + "/";
    tempString.toCharArray(TargetHub_URL,sizeof(TargetHub_URL));

    #ifdef DEBUG
    Serial.print("Hub IP: ");
    Serial.println(hub_ip);
    Serial.print("Hub Port: ");
    Serial.println(hub_port);
    Serial.print("Device set to send http messages to ");
    Serial.println (TargetHub_URL);
    #endif
  }
  else { 
    // Do not save hub_IP or hub_port to flash memory

    #ifdef DEBUG
    Serial.println ("Home automation hub IP and Port values failed validation!");
    #endif
    
    strcpy(TargetHub_URL,"<< Not Set >>");
  }

  preferences.end();

  #ifdef DEBUG
  Serial.println("Exiting Config Portal. Rebooting in 3 seconds...");
  #endif

  //Clear the Neopixel pixel of any other indications (e.g., from WiFi events) 
  strip.clear();

  //NEOPIXEL indicate succesful return from GWIDC_AP access portal and ready to reboot.
  //Written as a nonblocking loop, without using delay() statement, so as not to interfere 
  //with stability of newly established WiFi connection

  int blinkCount = 0; // Reset blink counter
  int pixelnumber = 0;
  bool temptoggle = false;
  unsigned long temppreviousMillis = millis();
  while (blinkCount < 8) { // blink pixel 0 four times
    unsigned long tempcurrentMillis = millis();
    if (tempcurrentMillis - temppreviousMillis >= 500) {
      temppreviousMillis = tempcurrentMillis;
      temptoggle = !temptoggle; // Toggle the state
      digitalWrite(LED_ONBOARD, temptoggle); // toggle the onboard LED
      strip.setPixelColor(0, 0, pixel_brightness * temptoggle, 0);
      strip.show(); 
      blinkCount++; 
    }
    yield(); // Allow other tasks to run so as not to interfere 
             // with stability of newly established WiFi connection
  }
  
  // Restart the ESP8266 to apply changes
  ESP.restart(); 
}


void saveConfigCallback() { // NOTE TO SELF -- placeholder
}


void onStationConnected(const WiFiEventStationModeConnected& evt) {
  const int temp_strip_Brightness = 255;
  const int pixel_brightness = 16;
  Serial.print("Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());
  //NEOPIXEL Indicate Reconnect (blue at pixel 0)
  strip.setBrightness(temp_strip_Brightness);
  strip.clear();
  strip.setPixelColor(0, 0, 0, pixel_brightness);
  strip.show(); 
}


void onStationDisconnected(const WiFiEventStationModeDisconnected& evt) {
  const int temp_strip_Brightness = 255;
  const int pixel_brightness = 16;
  Serial.println("WiFi Disconnected!");
  //NEOPIXEL Indicate Disconnect (yellow pixels positions 0 and 6)
  strip.setBrightness(temp_strip_Brightness);
  strip.clear();
  strip.setPixelColor(0, pixel_brightness, pixel_brightness, 0);
  strip.setPixelColor(6, pixel_brightness, pixel_brightness, 0);
  strip.show(); 
}
