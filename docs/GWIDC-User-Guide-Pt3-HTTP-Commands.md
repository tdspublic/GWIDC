# GWIDC USER GUIDE PART 3:<br>STANDALONE COMMANDS, DISPLAY MODES & PARAMETERS

<br>

## STANDALONE COMMANDS

These commands are mutually exclusive (cannot be combined).
These can be submitted directly via web browser and are used by the Hubitat device driver.

|Command|Description|
|:-----|:-----|
|`BEEP` |Briefly sounds the piezo buzzer. <br><br>Syntax is `http://<GWIDC_IPAddress>/BEEP`|
|`HUB_CONFIG`|Enables run time change to the home automation hub IP and port (i.e., Hubitat) values stored on the GWIDC without having to RESET the device to WiFi configuration mode. The device returns a plain text message confirming the change.<br><br>Syntax is `http://<GWIDC_IPAddress>/HUB_CONFIG?HUB_IP=<HubIPAddress>&HUB_PORT=<HubPort>`<br><br>NOTE: Changing these values does not automatically send a message to notify the target hub. Use the STARTUP command to let the target hub know the GWIDC is available.<br><br>Passing "CLEAR" as the sole argument clears the current and saved values of the IP and Port.<br><br>Syntax is `http://<GWIDC_IPAddress>/HUB_CONFIG?CLEAR`|
|`POLL`|Returns a formatted JSON message with current device settings. (See the section titled [JSON Formatted Responses](#JSON-Formatted-Responses) for details.) <br><br>Syntax is `http://<GWIDC_IPAddress>/POLL`|
|`REPORT_SETTINGS`|Returns a plain text report of device connection, hub IP/port values, and device status variables. Designed for use with a web browser to help with diagnostics and troubleshooting. <br><br>Syntax is `http://<GWIDC_IPAddress>/REPORT_SETTINGS`|
|`HUBSYNC`|Commands the GWIDC to send a formatted JSON message to the home automation hub IP and port (if configured), notifying the hub that the device ready to be synchronized. This is useful to trigger device initialization rules on the hub. The GWIDC displays four blue pixels.<br><br>Syntax is `http://<GWIDC_IPAddress>/HUBSYNC`|
|`VERSION_INFO`|Returns device hardware ID, build version, and the build date & time. <br><br>Syntax is `http://<GWIDC_IPAddress>/VERSION_INFO`|

<br>

## SETTING GWIDC DISPLAY MODES

The following set the pixel display mode for the device under normal operations. These can be submitted directly via web browser and are used by the Hubitat device driver. The GWIDC also responds with a JSON formatted message whenever the display mode is changed. See the section titled [JSON Formatted Responses](#JSON-Formatted-Responses).

Display modes are mutually exclusive.

|Display Mode|Description|
|:-----|:-----|
|`PATTERN=<value>`|Sets the color of individual pixels, where \<value> is a 12-character string where each character corresponds to a pixel position, and can be one of R (red) G (green) B (blue) Y (yellow) C (cyan) P (purple) W (white) or O (off). Other values and additional characters are ignored.<br><br>Example - to display a pattern of White, Red, Green, Blue, Off, Off, White, Red, Green, Blue, Off, Off, use <br>`http://<GWIDC_IPAddress>/?PATTERN=WRGBOOWRGBOO`<br><br>NOTE: This mode should be used in combination with the LEVEL parameter (see below) to set overall pixel ring brightness. <br>|
|`FLASH`|Flashing effect using the current color. <br><br>Syntax is `http://<GWIDC_IPAddress>/?FLASH`<br><br>NOTE: This mode should be used in combination with the LEVEL, SPEED, TIMERMODE, and TONE parameters (see below) to set the overall pixel ring brightness, the flash interval, and control the piezo buzzer.<br><br>OPTIONAL: Passing the argument "DEFAULT" flashes white. <br><br>Example - `http://<GWIDC_IPAddress>/?FLASH=DEFAULT`<br>|
|`PULSE`|Pulsating effect using the current color.<br><br>Syntax is `http://<GWIDC_IPAddress>/?PULSE`<br><br>NOTE: This mode should be used in combination with the RGB, LEVEL, SPEED, and TIMERMODE parameters (see below) to set the overall pixel ring brightness and the pulsating rate.<br><br>OPTIONAL: Passing the argument "DEFAULT" pulses in white. <br><br>Example - `http://<GWIDC_IPAddress>/?PULSE=DEFAULT`<br>|
|`SOLID`|All pixels display the current color. <br><br>Syntax is `http://<GWIDC_IPAddress>/?SOLID`<br><br>NOTE: This mode should be used in combination with the RGB and LEVEL parameters (see below) to set the overall pixel ring brightness.<br><br>OPTIONAL: Passing the argument "DEFAULT" displays all white pixels. <br><br>Example - `http://<GWIDC_IPAddress>/?SOLID=DEFAULT`<br>|
|`STROBE`|Strobing effect using the current color. <br><br>Syntax is `http://<GWIDC_IPAddress>/?STROBE`<br><br>NOTE: This mode should be used in combination with the RGB, LEVEL, SPEED, TIMERMODE and TONE parameters (see below) to set the overall pixel ring brightness, the strobing interval, and control the piezo buzzer.<br><br>OPTIONAL: Passing the argument "DEFAULT" strobes white on white. <br><br>Example - `http://<GWIDC_IPAddress>/?STROBE=DEFAULT`<br>|
|`TRAIL`|Creates effect of a dot moving around the ring with a trail of fading pixels behind it. The length of the trail is impacted by the intensity of the RGB color and the brightness LEVEL settings.<br><br>Syntax is `http://<GWIDC_IPAddress>/?TRAIL`<br><br>NOTE: This mode should be used in combination with the RGB, LEVEL, SPEED, and TIMERMODE parameters (see below) to set the overall pixel ring brightness and speed of the pixel motion.<br><br>OPTIONAL: Passing the argument "DEFAULT" uses a white dot and trail. <br><br>Example - `http://<GWIDC_IPAddress>/?TRAIL=DEFAULT`|
|`RAINBOW`|Simply for fun! Displays a rotating color rainbow.<br><br>Syntax is `http://<GWIDC_IPAddress>/?RAINBOW`<br><br>OPTIONAL: Passing the argument "DEFAULT" uses default parameters for this display mode.<br><br>Example - `http://<GWIDC_IPAddress>/?RAINBOW=DEFAULT`|


<br>

## SETTING PARAMETERS TO MODIFY GWIDC DISPLAY MODES

The following can be used to modify the behavior of the device’s current display mode under normal operations. These can be submitted directly via web browser and are used by the Hubitat device driver. The GWIDC also responds with a JSON formatted message whenever any of these parameters are changed. See the section titled [JSON Formatted Responses](#JSON-Formatted-Responses).

Basic syntax is `http://<GWIDC_IPAddress>/?<Parameter>=<value>`

|Parameter|Description|
|:-----|:-----|
|`LEVEL=<value>`|Sets the overall brightness of the Neopixel ring, where \<value> is an integer in the range 0-100. Invalid input is ignored.<br><br>Can be used with any display mode.<br><br>Example: to change the current display mode to use brightness level of 50, use <br>`http://<GWIDC_IPAddress>/?LEVEL=50`|
|`RGB=<value>`|Sets the primary pixel color, where \<value> is a 6-character string of hexadecimal digits representing the RGB color (omit the hashmark # prefix). Invalid input is ignored. <br><br>Can be used with display modes FLASH, PULSE, SOLID, STROBE, and TRAIL. Ignored by other modes. <br><br>Example: to change the current display modes to use cyan, use <br>`http://<GWIDC_IPAddress>/?RGB=00FFFF`|
|`SPEED=<value>`|Sets the speed of the <ins>visual</ins> and <ins>tone</ins> effects for the current display mode, where `<value>` is an integer in the range 0 - 4. Smaller numbers equate to “slower”, higher numbers to "faster". Invalid input is ignored. <br><br>Can be used with any display mode. <br><br>Example: to set the speed to a medium value, use `http://<GWIDC_IPAddress>/?SPEED=2`|
|`TONE=<value>`|Determines whether the Piezo buzzer is active or not, where \<value> is either "ON" or a "OFF". The pattern of beeps is determined by the current display mode. <br><br>Can be used with any display mode. <br><br>Example: to enable the buzzer with the current display mode, use <br>`http://<GWIDC_IPAddress>/?TONE=ON`|


Modifiers can be used individually or in combination, using an ampersand (“&”) as a separator character in the HTTP string.

Syntax is `http://<GWIDC_IPAddress>/?<Mode>&<Parameter1>=<value1>&<Parameter2>=<value2>...`

For example, the following HTTP string will command the device to flash all pixels on and off together (FLASH) in cyan (RGB=00FFFF), at max brightness (LEVEL=100), at a modest pace (SPEED=2), while beeping the piezo buzzer (TONE=ON)

`http://<GWIDC_IPAddress>/?FLASH&RGB=00FFFF&LEVEL=100&SPEED=2&TONE=ON`

<br>

## JSON FORMATTED RESPONSES

When the GWIDC receives a POLL or STARTUP command, or a display mode command and//or display mode parameter it responds with a JSON formatted message confirming the device's settings.

|JSON Key | Value|
|:---|:---|
|Name|Hardware name that is factory-encoded into the NodeMCU board of the GWIDC (constant). For more information on this, search for "NodeMCU device name specifications" on the Internet.|
|Mode|Current display mode setting.|
|RGB|Current RGB color setting.<br><br>NOTE: When the device is in PATTERN display mode, this value is actually a calculated "average" of the RBG values across all 12 pixels.|
|Pattern|12-character string representing the current color of each pixel. <br><br>When the device is in the PATTERN display mode, each pixel can be R (red) G (green) B (blue) Y (yellow) C (cyan) P (purple) W (white) or O (off). <br><br>For display modes other than PATTERN, POLL returns the string "************". This is because in those other modes, the overall color setting is controlled collectively using the RGB= parameter, which can be set to any valid RGB value (16M), not limited to the eight colors listed above.|
|Level|Current brightness level setting|
|Speed|Current SPEED setting|
|Tone|Current piezo buzzer setting|

An earlier example changed the device settings to the FLASH display mode, RGB color to 00FFFF, LEVEL to 100, SPEED to 2, and made the piezo buzzer active using the command 

`http://<GWIDC_IPAddress>/?FLASH&RGB=00FFFF&LEVEL=100&SPEED=2&TONE=ON` 

The resulting JSON response from the device would be

`{"Name":"ESP-######","Mode":"FLASH","RGB":"00FFFF","Pattern":"************","Level":100,"Speed":2,"Tone":ON}`

(where ESP-###### represents the factory-encoded name of the specific NodeMCU board of the GWIDC).

<br>

To obtain a formatted JSON response of the current device settings, use `http://<GWIDC_IPAddress>/POLL` 


---

&copy; 2025 Tim Sakulich. GWIDC documentation is licensed under Creative Commons Attribution-ShareAlike 4.0 International. <br>
See: [`LICENSE-DOCS`](/LICENSE-DOCS)
