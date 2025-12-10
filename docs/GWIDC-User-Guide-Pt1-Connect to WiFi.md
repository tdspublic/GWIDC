# GWDIC USER GUIDE PART 1: <br>CONNECT TO WIFI NETWORK

Once the GWIDC is connected to the network, the user can directly control it
through a browser window on a computer, laptop or cell phone connected to the 
same network. No home automation hub is required.  The user can also configure 
the device for a home automation setup at a later time without having to reset 
WiFi credentials.

## Information Needed

- WiFi Network SSID and password

- GWIDC's MAC Address

## STEP 1 - Assign a Static IP on the Router DHCP Table

- Log into the WiFi router

- Using **MAC Address** of the GWIDC device, create an entry in the DHCP
  Table to assign a unique static IP address to the GWIDC. Make a note
  of this IP address.

## STEP 2 - Configure the GWIDC WiFi Connection

- Overview: In WiFi configuration mode, GWIDC creates a local access
  portal named GWIDC_AP with IP address 192.168.4.1 where the user
  selects from a list of available WiFi networks, provides the
  corresponding password, and provides the static IP address and port of
  the Hubitat hub. Once saved, this information persists in the GWIDC's
  flash memory across power disruptions, reboots, or network disconnects
  until the device is RESET by the user (see below).

- Step-by-step procedure:

  - Connect the GWIDC to power. If a single red pixel at position 0 is
    lit, the device is in WiFi configuration mode. If the device shows
    anything other than a single red pixel then it is not in WiFi
    configuration mode (in that case, RESET the device to clear any
    previously saved credentials--see below).

  - Bring a laptop or cell phone with WiFi close to GWIDC, go to the
    network settings page and look for an access portal named
    **GWIDC_AP** (the IP address is 192.168.4.1). Connect to the access
    portal. Once connected to the access portal, a configuration menu
    will open in a browser window.

    - From that configuration menu, click on Configure WiFi, select the
      desired WiFi **SSID** from the list of available networks, and
      enter the corresponding **password**

    - Click the Save button

    - NOTE: That same configuration page has fields for the home automation
      hub's static IP address and Port. Leaving these fields blank (i.e.,
      null values) does not limit the user's ability to control the device
      directly via HTTP. The user can also update these values on the
      device at a later time via HTTP message.

  - Once the user clicks the Save button, the GWIDC will  attempt to connect
    to the WiFi network using the supplied SSID and password. Success of this
    initial connection usually happens within seconds, and is indicated by
    a single green pixel flashing four times on the device.\
    \
    NOTE: If after a minute or so the device continues to display a
    single red pixel or nothing at all, that means that the device has
    not been able to connect to the network with the SSID and password
    that the user specified via the access portal. Manually RESET the
    device (see below) and repeat STEP 2 from the top.

  - After the GWIDC establishes a successful WiFi connection for the
    first time, it will reboot. The device will now automatically
    reconnect to the configured WiFi network after every reboot until
    the network configuration is RESET by the user.

- GWIDC indications during normal bootup

  - When the GWIDC reboots, it will first display a single yellow pixel
    to indicate that is attempting to connect to the network using the
    saved WiFi credentials. This usually takes a couple of seconds.

  - Once the GWIDC is connected to WiFi it will display a single blue
    pixel. NOTE: If the user has already provided values for the home automation
    hub IP address and port, the GWIDC will immediately send a message to
    that IP and port and will display four blue pixels.
  
- **Once the GWIDC is connected to WiFi, the user can send commands to
   the device via HTTP from a browser window on computer, laptop or cell
   phone connected to the same network.**  For example, verify the device
   configuration by entering the following in the browser's address field:\
     \
     `http://<GWIDC_IPAddress>/REPORT_SETTINGS`\
     (Replace `<GWIDC_IPAddress>` with the actual IP address of the device.)
  

## How to reset WiFi credentials and reboot to WiFi configuration mode.

  - To reset (delete) WiFi credentials from the GWIDC's flash memory and reboot
    the device into WiFi configuration mode, 1) disconnect the GWIDC from its
    power source, 2) press and hold the reset button, 3) reconnect the device
    to power, then 4) release the reset button.

  - This action will also erase any home automation hub IP and Port information
    that was saved in the GWIDC's flash memory.


## How to update the Home Automation Hub IP Address and Port information<br>without rebooting into WiFi configuration mode

  - If the GWIDC is successfully connected to a WiFi network, the user
      can update the home automation hub IP Address and Port though a browser
      on a computer, laptop or cell phone connected to the same network
      by entering the following in the browser's address field:\
      \
     `http://<GWIDC_IPAddress>/HUB_CONFIG?HUB_IP=<HubIPAddress>&HUB_PORT=<HubPort>`\
      (Replace `<GWIDC_IPAddress>`, `<HubIPAddress>`, and `<HubPort>` with their actual values)

- Using `"CLEAR"` as a parameter for this command clears the current and saved
  values of the IP and Port and sets those to null values.\
  \
  `http://<GWIDC_IPAddress>HUB_CONFIG?CLEAR`

- The updated information will persist in the GWIDC's flash memory
  across power disruptions, reboots, or network disconnects.


## TROUBLESHOOTING

- From a computer, laptop or cell phone connected to the same network,
  use `http://<GWIDC_IPAddress>/REPORT_SETTINGS` to verify GWIDC network
  connectivity and IP address

---

&copy; 2025 Tim Sakulich. GWIDC documentation is licensed under Creative Commons Attribution-ShareAlike 4.0 International. <br>
See: [`LICENSE-DOCS`](/LICENSE-DOCS)
