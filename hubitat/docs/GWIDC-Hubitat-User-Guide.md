# GWIDC WITH HUBITAT - USERS GUIDE

## CAPABILITIES

The GWIDC driver includes the following capabilities available through the device Commands tab and also available to apps (e.g., rules)

|Command | Description|
|:----|:----|
|Beep| Causes the GWIDC piezo buzzer to beep one time.|
|Device Notification | Allows access to the full range of GWIDC display modes and display mode parameters beyond Hubitat default commands.  See [USING THE DEVICE NOTIFICATION CAPABILITY](#USING-THE-DEVICE-NOTIFICATION-CAPABILITY) section below and the GWIDC Users Guide for more details.|
|Initialize | Used to simulate receiving a STARTUP message from the GWIDC, and results in a call to the driver push() method. Mostly for troubleshooting.|
|On, Off| Allows the GWIDC to be treated like an indicator bulb device by apps and rules. <br><br>The display modes and parameters used for these capabilities can be changed on the Preferences page (see illustrations in next section for their default values).|
|Poll| Updates the device Current State values in Hubitat based on the current settings reported by the GWIDC. These values are shown on the device’s Commands tab and available to apps and rules. |
|Push |The driver includes this capability simply as a way to trigger rules within Hubitat.  <br><br>When the GWIDC device sends a JSON message containing STARTUP as the mode, the driver calls the initialize() method, which then creates a push() event. That push() event is seen by Rule Machine and other apps, and is useful to synchronize the GWIDC based on the current status of other devices (such as a door being open or closed).  It is a workaround to writing separate application code. It performs no actions on the GWIDC device itself.|
|Strobe, Siren, Both| Allows the GWIDC to be treated like an alarm device. <br><br>The base RGB color used for the strobing capability can be changed on the Preferences page (see illustrations in next section for their default values).|

<br>

## DEFAULT DISPLAY MODES AND PARAMETERS

|Pixel Display|Description|
|:---:|:----|
|Hubitat "Off" command<br>![Image of Neopixel with red pixel at top](images/GWIDC-Hubitat-Off.png)| Defaults to green pixels in triangle configuration. <br><br>EXPLANATION: In the Preferences page for the device, the default value for "Display Mode for 'Off'" is `PATTERN=GOOOGOOOGOOO&LEVEL=1&TONE=OFF`|
|Hubitat "On" command<br>![Image of Neopixel with green pixel at top](images/GWIDC-Hubitat-On.png)| Defaults to red pixels (slowly pulsating)<br><br>EXPLANATION: In the Preferences page for the device, the default value for "Display Mode for 'On'" is `PULSE&RGB=FF0000&LEVEL=40&SPEED=2&TONE=OFF`|
|Hubitat "Strobe" command<br>![Image of Neopixel with green pixel at top](images/GWIDC-Hubitat-Strobe.png)| Defaults to four flashing white pixels on red background<br><br>EXPLANATION: In the Preferences page for the device, the default value for "RGB Color for 'Strobe' (hex)" is `220000` (NOTE: the driver automatically uses the RGBSTROBE display mode with max brightness)|
|Any other pixel colors/patterns|The GWIDC pixels and the piezo buzzer change in response to properly notification text strings from Hubitat |

<br>

# USING THE DEVICE NOTIFICATION CAPABILITY

The Device Notification capability is a way to access to the full range of GWIDC display modes and display mode parameters beyond Hubitat default commands. 

Refer to Part 3 of the GWIDC Users Guide for the full list of display modes and display mode parameters.

Unlike submitting HTTP messages to the GWIDC through a browser window, DO NOT include the IP Address of the device or the question mark symbol in the Device Notification text string. The device's IP address is already configured in the Preferences tab during initial setup of the device on Hubitat.

For example, to set the device mode to FLASHER, RGB color to 00FFFF, BRIGHTNESS to 100, SPEED to 2 (medium), and activate the piezo buzzer, the user would submit the following as the Device Notification text string via the device Commands tab or as an action in a rule.<br><br>`FLASHER&RGB=00FFFF&LEVEL=100&SPEED=2&TONE=ON` 

>[!NOTE]
>The GWIDC driver in Hubitat only supports sending display mode and display mode parameters via the Device Notification capability. The driver does not support sending GWIDC standalone commands show such as HUB_CONFIG, REPORT_SETTINGS and VERSION_INFO using the Device Notification capability.
