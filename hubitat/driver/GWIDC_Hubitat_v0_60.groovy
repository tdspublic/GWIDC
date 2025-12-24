// Driver for General-purpose WiFi Indicator DeviCe (GWIDC)
// Compatible with GWIDC Firmware v0_60

// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Timothy Sakulich

metadata {
    definition (name: "GWIDC_Hubitat_v0_60", namespace: "tjsakulich", author: "Timothy Sakulich") {
        capability "Actuator"
        capability "Alarm"
        capability "Bulb"        
        capability "Initialize"
        capability "Notification"
        capability "PushableButton"
        capability "Switch"
        capability "Tone"  
        
        /*
        // Including the "SwitchLevel" capability in this driver will make 
        // the Set Level command available on the device Commands tab and
        // to other apps and rules.  However, this command is not strictly
        // needed since GWIDC brightness can be controlled via 
        // deviceNotification using the LEVEL= parameter. Still, to enable
        // the Set Level command, uncomment the following line and uncomment 
        // the code for the corresponding method as indicated below. 
        
        capability "SwitchLevel"
        */
        
        /*
        // Including the "ColorControl" capability in this driver will make 
        // the setColor, setHue, and setSaturation commands available on the 
        // device Commands tab and to other apps and rules.  However, these 
        // commands are not strictly needed since GWIDC color can be controlled
        // via deviceNotification using the RGB= parameter. Moreover, it can 
        // be confusing to use Hubitat's standard color controls for the 
        // GWIDC in display modes where the 12 pixels are not all set to the 
        // same color in the first place. Still, to enable Hubitat's 
        // color controls capability, uncomment the following line and 
        // uncomment the code for the corresponding methods as indicated below.

        capability "ColorControl"
        */
        
        attribute "DeviceName", "string"
        attribute "Mode", "string"
        attribute "RGB", "string"
        attribute "Pattern", "string"
        attribute "Level", "number"
        attribute "Speed", "number"
        attribute "Tone", "string"

        command "poll"      
    }
 
    preferences {
        section("GWIDC Settings") {
            input("DeviceIP", "string", title: "GWIDC Device IP", required: true)
            input("EnableDebug", "bool", title: "Enable debug logging?", defaultValue: false, required: false)
            input("GWIDC_On", "string", title: "Display Mode for 'On'", required: false)
	        input("GWIDC_Off", "string", title: "Display Mode for 'Off'" , required: false)
	        input("GWIDC_Strobe_RGB", "string", title: "RGB Color for 'Strobe' (hex)", required: false)
	    }   
   } 
}


//====================================================
//       Methods Automatically Called by Hubitat
//====================================================

def installed() {
   // Runs when Hubitat first starts up and connects to the device
   if (EnableDebug) log.debug "Executing installed()"
   GWIDC_On = "PULSE&RGB=FF0000&LEVEL=50&SPEED=2&TONE=OFF"
   GWIDC_Off = "PATTERN=GOOOGOOOGOOO&LEVEL=1&SPEED=0&TONE=OFF"
   GWIDC_Strobe_RGB = "220000"
    
   // the button capability creates a way to trigger rules within Rule Machine
   // e.g., a rule that resynchronizes the GWIDC(s)
   sendEvent(name: "numberOfButtons", value: 1)
}

//====================================================
def updated() {
   if (EnableDebug) log.debug "Executing updated()"
   // Do nothing
}

//====================================================
//    Methods Required to Implement Capabilities
//====================================================

def beep() {
    CommandMessage = "BEEP"
    if (EnableDebug) log.debug "Executing 'beep()' (Sending ${CommandMessage} to ${DeviceIP})"
    
    asynchttpGet(null,[uri: "http://${DeviceIP}/${CommandMessage}"])    
}


//====================================================
def both() {
    CommandMessage = "?STROBE&RGB=${GWIDC_Strobe_RGB}&LEVEL=100&SPEED=2&TONE=ON"
    
    log.info "${device.label} Executing 'both()' (${CommandMessage})"
    
    asynchttpGet("myCallback",[uri: "http://${DeviceIP}/${CommandMessage}"]) 
}


//====================================================
def initialize() {
    // Can be called from Device Commands page
    // Also triggered when GWIDC reports "STARTUP"
    
    CommandMessage = "VERSION_INFO"   
    if (EnableDebug) log.debug "Executing initialized()"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/VERSION_INFO",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("VersionInfoCallback", params)  
      
   // creating the following push() event can be used to trigger a rule 
   // in Rule Machine to resynchronize the GWIDC(s)
   push(1)
}


//====================================================
def off() {
    CommandMessage = "?" + settings.GWIDC_Off
    
    log.info "${device.label} Executing 'off()' (${CommandMessage})"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)
    
    sendEvent(name: "switch", value: "off", isStateChange: false)
}


//====================================================
def on() {    
    CommandMessage = "?" + settings.GWIDC_On
    
    log.info "${device.label} Executing 'on()' (${CommandMessage})"

    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)
   
    sendEvent(name: "switch", value: "on", isStateChange: false) 
}


//====================================================
def poll() {
    CommandMessage = "POLL"   
    if (EnableDebug) log.debug "Executing 'poll()'"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/POLL",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)
}


//====================================================
def push(buttonNumber) {    
    log.info "${device.label} Executing 'push(${buttonNumber})'"
 
    //Generates an event for anything that is subscribed
    sendEvent(name: "pushed", value: "${buttonNumber}", isStateChange: true, type: "digital")
}


//====================================================
def refresh() {
    poll()
}


//====================================================
def siren() {
    CommandMessage = "?STROBE&RGB=000000&LEVEL=0&SPEED=2&TONE=ON"
    
    log.info "${device.label} Executing 'siren()' (${CommandMessage})"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)
}


//====================================================
def strobe() {
    CommandMessage = "?STROBE&RGB=${GWIDC_Strobe_RGB}&LEVEL=100&SPEED=2&TONE=OFF"
    log.info "${device.label} Executing 'strobe()' (${CommandMessage})"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)
}


//====================================================
def deviceNotification(commandtext) {
    
    CommandMessage = "?" + commandtext
    
    log.info "${device.label} Executing Notification ${commandtext}"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)  
}



//====================================================
//            CALL BACKS AND PARSE METHODS
//        FOR HANDLING MESSAGES FROM THE GWIDC
//====================================================

// The myCallback() call back method is designed to handle the response 
// to an asynchttpGet() message sent to the GWIDC when the expected 
// response is a JSON-formatted list of the device's current mode 
// and parameters.

private myCallback(response, data) {
    if (response.getStatus() == 200) {
        try {
            def jsonResponse = parseJson(response.data)
            if (EnableDebug) log.debug "Received JSON: ${jsonResponse}"

            // Update device attributes
            sendEvent(name: "DeviceName", value: jsonResponse.Name)
            sendEvent(name: "Mode",       value: jsonResponse.Mode)
            sendEvent(name: "RGB",        value: jsonResponse.RGB)
            sendEvent(name: "Pattern",    value: jsonResponse.Pattern)
            sendEvent(name: "Level",      value: jsonResponse.Level)    
            sendEvent(name: "Speed",      value: jsonResponse.Speed)
            sendEvent(name: "Tone",       value: jsonResponse.Tone)
            
            // the following code checks to see if the GWIDC is requesting to be sync'd
    		// and then calls initialize() 
   			if (jsonResponse.Mode == "HUBSYNC") {
     		  if (EnableDebug) log.debug "${device.label} Requested HUBSYNC"
       		initialize()
    		}
        } catch (e) {
            log.error "${device.label} Failed to parse JSON response: ${e}"
        }
    } else {
        log.error "${device.label} HTTP request failed with status: ${response.getStatus()}"
    }
}


// The VersionInfoCallback() call back method is designed to handle the  
// JSON response to an asynchttpGet() message sent to the GWIDC requesting 
// VERSION_INFO. 

private VersionInfoCallback(response, data) {
    if (response.getStatus() == 200) {
        try {
            def jsonResponse = parseJson(response.data)
            if (EnableDebug) log.debug "Received JSON: ${jsonResponse}"
            
            // Update static device info for display on the Device Info page
            device.updateDataValue("Device Hardware ID", jsonResponse.Hardware_ID)
            device.updateDataValue("Firmware Version", jsonResponse.Build_Version)
            device.updateDataValue("Version Build Date", jsonResponse.Build_Date)
            device.updateDataValue("Version Build Time", jsonResponse.Build_Time)

        } catch (e) {
            log.error "${device.label} Failed to parse JSON response: ${e}"
        }
    } else {
        log.error "${device.label} HTTP request failed with status: ${response.getStatus()}"
    }
}


// The parse() method handles any "unsolicited" messages from the GWIDC 
// (i.e., anything that is not a response to an asynchttpGet() request). 
// In particular, this method handles the situation when the GWIDC first powers 
// up and sends a JSON message containing "HUBSYNC" as the device display mode. 

private parse(String description) { 
    if (EnableDebug) log.debug "Executing 'parse()'"
    
    def msg = parseLanMessage(description)
	def body = msg.body
    body = parseJson(body)
    
    // the following code checks to see if the GWIDC is requesting to be sync'd
    // and then calls initialize() 
    if (body.Mode == "HUBSYNC") {
       log.info "${device.label} Requested HUBSYNC"
       initialize()
    }
    
}




/*

//===================================================================
//Additional Method Required to Implement SwitchLevel Capability
//===================================================================

def setLevel(level) {
    CommandMessage = "?LEVEL=${level}"   
    log.info "${device.label} Executing 'setLevel' ${level}"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)  
}
*/






/*
// As noted above, including the "ColorControl" capability for the GWIDC 
// will make the setColor, setHue, and setSaturation commands available  
// on the device Commands tab and to other apps and rules.  However, these 
// commands are not strictly needed since GWIDC color can be controlled
// via deviceNotification using the RGB= parameter. Moreover, it can 
// be confusing to use Hubitat's standard color controls for the 
// GWIDC in display modes where the 12 pixels are not all set to the 
// same color in the first place.  Still, if the Hubitat's 
// color controls capability is included, the following methods will be 
// required for that functionality. 


//===================================================================
//Additional Methods Required to Implement Color Control Capabilities
//===================================================================

def setColor(value) {
    if (value.hue == null || value.saturation == null) return      
    New_RGBvalue = HSLtoRGB(value.hue, value.saturation, value.level)    
    CommandMessage = "?RGB=" + New_RGBvalue

    log.info "${device.label} Executing 'setColor' ${value} ==> ${New_RGBvalue}"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)       
}


//====================================================
def setHue(value) {
 
    // Set hue while keeping the current saturation and level from the device's current color value
    Current_RGBvalue = device.currentValue("RGB")
    Current_hue = RGBtoHSL(Current_RGBvalue).hue
    Current_saturation = RGBtoHSL(Current_RGBvalue).saturation
    Current_level = RGBtoHSL(Current_RGBvalue).level
    
    New_RGBvalue = HSLtoRGB(value, Current_saturation, Current_level)
    CommandMessage = "?RGB=" + New_RGBvalue
    
    log.info "${device.label} Executing 'setHue' ${value} ==> ${New_RGBvalue}"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)   
}


//====================================================
def setSaturation(value) {
    
    Current_RGBvalue = device.currentValue("RGB")
    Current_hue = RGBtoHSL(Current_RGBvalue).hue
    Current_saturation = RGBtoHSL(Current_RGBvalue).saturation
    Current_level = RGBtoHSL(Current_RGBvalue).level
    
    // Set saturation while keeping the current hue and level from the device's current color value
    New_RGBvalue = HSLtoRGB(Current_hue, value, Current_level)
    
    CommandMessage = "?RGB=" + New_RGBvalue
    
    log.info "${device.label} Executing 'setSaturation' ${value} ==> ${New_RGBvalue}"
    
    def ip = settings.DeviceIP
    if (!ip) {
        log.error "Device IP Address not configured."
        return
    }

    def params = [
        uri: "http://${ip}/${CommandMessage}",
        contentType: "application/json"
    ]

    if (EnableDebug) log.debug "Sending asynchronous GET request ${params.uri}"
    asynchttpGet("myCallback", params)    
}


//===================================================================
//                   Additional Utility Functions 
//           Required to Implement Color Control Capabilities
//===================================================================

private hue2rgb(p, q, t){
            if(t < 0) t += 1;
            if(t > 1) t -= 1;
            if(t < 1/6) return p + (q - p) * 6 * t;
            if(t < 1/2) return q;
            if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
            return p;
        }


private HSLtoRGB(h,s,l) {  
    //logDebug ("Executing HSLtoRGB()")
    String HEXCode = "FFFFFF"
    
    int RedValue = 0 
    int GreenValue = 0 
    int BlueValue = 0 
    
    if(s == 0) {RedValue = GreenValue = BlueValue = l * 2.55 } // achromatic
    else{
        // Adjustvinput values to convert from Hubitat HSL Color Mapping Ranges 
        h /= 100.0    // Hubitat Hue is [0,100] and needs to be [0,1]
        s /= 100.0    // Hubitat Saturation is [0,100] and needs to be [0,1]
        l /= 200.0    // Hubitat Level is [0,100] and needs to be [0,0.5]
        
        q = l < 0.5 ? l * (1 + s) : l + s - l * s
        p = 2.0 * l - q
        
        RedValue = Math.round(hue2rgb(p, q, h + 1/3.0) * 255)     
        GreenValue = Math.round(hue2rgb(p, q, h) * 255)
        BlueValue = Math.round(hue2rgb(p, q, h - 1/3.0) * 255)
    } 
    
    String RedHEXString = Integer.toHexString(RedValue)
    if (RedValue < 16) { RedHEXString = "0" + RedHEXString }
    String GreenHEXString = Integer.toHexString(GreenValue)
    if (GreenValue < 16) { GreenHEXString = "0" + GreenHEXString }
    String BlueHEXString = Integer.toHexString(BlueValue)
    if (BlueValue < 16) { BlueHEXString = "0" + BlueHEXString }

    HEXCode = RedHEXString+GreenHEXString+BlueHEXString
    //if (EnableDebug) log.debug "HSLtoRGB ============ ${HEXCode}"       
       
    return HEXCode  
}

    
private RGBtoHSL(HEXCode) {
    //logDebug ("Executing RGBtoHSL()")
    int red = Integer.valueOf(HEXCode.substring(0, 2), 16)
    int green = Integer.valueOf(HEXCode.substring(2, 4), 16)
    int blue = Integer.valueOf(HEXCode.substring(4, 6), 16)
    
    float r = red / 255f
    float g = green / 255f
    float b = blue / 255f
    
    //if (EnableDebug) log.debug "RGBtoHSL ============ red   = ${red} ==== ${r}"
    //if (EnableDebug) log.debug "RGBtoHSL ============ green = ${green} ==== ${g}"
    //if (EnableDebug) log.debug "RGBtoHSL ============ blue  = ${blue} ==== ${b}"
       
    float max = [r, g, b].max()    
    //if (EnableDebug) log.debug "RGBtoHSL ============ max = ${max}"
    
    float min = [r, g, b].min()                
    //if (EnableDebug) log.debug "RGBtoHSL ============ min = ${min}"
    
    float h = (max + min) / 2.0;
    float s = h
    float l = h

    if (max == min) {
      h = 0
      s = 0   
      l = max / 2
    }
    else {
      d = max - min;
      s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
      if (max == r) h = (g - b) / d + (g < b ? 6 : 0);
      if (max == g) h = (b - r) / d + 2;
      if (max == b) h = (r - g) / d + 4;
      h /= 6;

    }
    
    hue = (h * 100).round()        // Convert hue to range [0...100]
    saturation = (s * 100).round() // Convert saturation to range [0...100]
    level = (l * 200).round()      // Convert level to range [0...100]
    
    return [
        "red": red.asType(int),
        "green": green.asType(int),
        "blue": blue.asType(int),
        "hue": hue.asType(int),
        "saturation": saturation.asType(int),
        "level": level.asType(int),
    ]
}

*/