# SETUP AND CONFIGURE GWIDC WITH HUBITAT

>[!IMPORTANT]
>If the user has already configured the GWIDC with a static IP and
>verified the device's WiFi connection, review the section
>"Information Needed", then skip to the subheading under 
>STEP 2 "How to update the Home Automation Hub IP Address and Port
>information without rebooting into WiFi configuration mode". Then
>continue on to STEP 3.

## Information Needed

- GWIDC-Hubitat-v1-00 driver for Hubitat (download, needed for STEP 3)

- WiFi Network SSID and password (needed for STEP 2)

- GWIDC's MAC Address (from the device, needed for STEPS 2 and 3)

- Hubitat's Static IP address (obtain in STEP 1, needed for STEP 2)

- GWIDC Static IP address (create in STEP 1, needed for STEP 3)

## STEP 1 - Assign a Static IP on the Router DHCP Table

- Log into the WiFi router

- Verify that Hubitat hub is using a **static IP address**. Make a note
  of the assigned IP address.
  
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

    - On that same configuration page, enter the **Hubitat's static IP
      address** as the Hub IP and **39501** as the Hub Port.
      
    - Click the Save button

  - Once the user clicks the Save button, the GWIDC will attempt to connect
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
    pixel.

  - If the user has provided valid values for the Hub IP address and Hub
    port, the GWIDC will immediately send a STARTUP message to that IP
    address and port, indicated by four blue pixels. The GWIDC will idle
    in this state until it receives a command (requires STEP 3 to be
    completed)
  
- **Once the GWIDC is connected to WiFi, the user can send commands to
   the device via HTTP from a browser window on computer, laptop or cell
   phone connected to the same network.**  For example, verify the device
   configuration by entering the following in the browser's address field:\
     \
     `http://<GWIDC_IPAddress>/REPORT_SETTINGS`\
     (Replace `<GWIDC_IPAddress>` with the actual IP address of the device.)
  

## How to reset WiFi credentials and reboot to WiFi configuration mode.

- To reset (delete) WiFi credentials from the GWIDC's flash memory and reboot the device into WiFi configuration mode, 1) disconnect the GWIDC from its power source, 2) press and hold the reset button, 3) reconnect the device to power, then 4) release the reset button.

- This action will also erase any home automation hub IP and Port information that was saved in the GWIDC's flash memory.


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

## STEP 3 - Configure Hubitat 

- Overview: The user creates a virtual device using the GWIDC device
  driver and provides the static IP address. Once configured, Hubitat
  can send commands to the GWIDC through its device page and through
  dashboards, rules, etc.

- Step-by-step procedure:

  - Go to the **Drivers Code** page, click Add Driver, copy and paste in
    the text contents of the GWIDC-Hubitat-v1-00 file, then save. The
    driver should now be listed on the Drivers Code page.

  - Go to the **Devices** page, click Add Device, select Virtual, and
    select the GWIDC-Hubitat-v1-00 driver

  - Now go to the **Device** page for the newly-added GWIDC

    - Select the **Device Info** tab

      - Check/update the **Device Name** and **Device Label** as desired

      - Verify the **Type** is set to the GWIDC-Hubitat-v1-00 driver

      - Change the **Device Network Id** to the **MAC address** of the
        GWIDC (without the colons)

      - Save the changes

    - Select the **Device Preferences** tab

      - Enter the **static IP Address** for the GWIDC
     
      - Do not change default settings for other Preferences at this time

      - Save the changes

    - Select the **Command**s tab

      - Click on POLL or other commands to test the device


# TROUBLESHOOTING

- Verify that the GWIDC and Hubitat are connected to the same network

- Verify that the router is configured to use static IP addresses for
  both the Hubitat and the GWIDC

- From a computer, laptop or cell phone connected to the same network,
  use http://\<GWIDC_IPAddress\>/REPORT_SETTINGS to verify GWIDC network
  connectivity and IP address, and that it has the correct IP Address
  and port 39501 for the Hubitat hub

If all of the above checks out, then the problem is likely with the
Hubitat configuration

- Verify that the Hubitat hub is using the correct driver for the GWIDC,
  that the device ID is the same as the GWIDC MAC address (without
  colons), and that the IP address on the preferences page is set to the
  GWIDC's static IP

- If the GWIDC is still not responding to Hubitat commands, remove power
  from the GWIDC for a few seconds then restore power. After reboot, the
  device will attempt to communicate with the Hubitat.

- If the GWIDC is still not responding to Hubitat commands, reboot the
  router.

- If the GWIDC is still not responding to Hubitat commands, reboot
  Hubitat.

- If the GWIDC is still not responding to Hubitat, the user will need to
  investigate whether there is another configuration issue between the
  Hubitat and the router (e.g., a firewell setting?) or some other
  driver or setting on the Hubitat that is interfering with the ability
  of the Hubitat to communicate with the GWIDC

---

&copy; 2025 Tim Sakulich. GWIDC documentation is licensed under Creative Commons Attribution-ShareAlike 4.0 International. <br>
See: [`LICENSE-DOCS`](/LICENSE-DOCS)
