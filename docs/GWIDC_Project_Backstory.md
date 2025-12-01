# The GWIDC Project

## <ins>G</ins>eneral-purpose <ins>W</ins>iFi-connected <ins>I</ins>ndicator <ins>D</ins>evi<ins>C</ins>e

I began this project to solve a challenge -- create a simple visual  indicator that I could place anywhere in my house to let me know whether my garage door was open or closed. With my large family, it was not an infrequent occurrence that the main garage door would be left open all day or that I would get up in the morning to find that someone had left the garage door open all night. We live in a very safe neighborhood, but I was concerned that complacency would eventually become regret.

The obvious solution would have been to go the garage and physically look at the door before going to bed...but what's the fun in that for an electronics nerd in the age of home automation tools?

Already a Hubitat enthusiast, I decided to purchase a Zooz Zen 17 relay controller, connect it to the garage door opener and install magnetic reed switches on the door track. Voila, I could now check the door status from anywhere using the Hubitat app on my cell phone, and even remotely open and close the door.

Still, I envisioned some sort of quick visual indicator of the door status in our kitchen and main bedroom so that I would not need to go look in person or take the time to open an app. I could have set up a rule in my home automation that would use a smart switch or smart bulb to indicate the door status. For better or worse, that did not appeal to my desire for a challenge!

I envisioned a simple, small device that would display a green LED when the door was shut and blink a red LED when the door was open--a device that could plug into any power outlet in my house. Visible but not obtrusive. After some quick research into commercial products it was clear that nothing was optimal for my application...or cheap.  

That led to an electronics and software design journey which--**truth be told**--sort of got out hand as I repeatedly tinkered to add "just one more" feature. After several generations of development over the past couple of years--not to mention more than a few eye rolls from my spouse--I have decided to declare the project complete. Well, mostly complete. At least for now.

The final (which really means "current") GWIDC design is a physical device built around an ESP8266 NodeMCU development board, NeoPixel ring, piezo buzzer, and C++ software, along with a custom driver for Hubitat written in Groovy. I did look into leveraging the excellent open source Hubduino and ST_Anything libraries as potential solutions, but in the end decided to develop my own custom solution.

I now have three of these devices positioned strategically in our house, communicating with Hubitat hub via WiFi. There is one in our kitchen, one in the hallway outside the main bedroom, and one in our home office. They are in fact quite helpful in solving the original problem of forgetting to close the garage door, and with the additional features and Hubitat's rules, they can visually alert me to other issues (e.g., from water leak sensors).

While I tried to be systematic about documenting the hardware and software design along the way, it’s one thing to do that out of practicality and another thing entirely to document the whole project for others: hardware build, software design, functionality of the device, and integration with the home automation hub. Over the past few weeks, I have worked to craft documentation worthy of sharing. 

Also, rather than keep referring to the hardware/software solution as "the device", I cheekily decided to give it a name: the General-purpose WiFi-connected Indicator DeviCe (GWIDC -- pronounced gee-wiz). That formality of naming the design should not mask the point that, in the end, this was simply a hobby project. Period.

I hope that others may find my design and documentation useful for their own needs, perhaps motivate some community-developed improvements, and maybe even inspire some completely novel ideas.

# Some final thoughts...

As a hobbyist and truly novice developer, I relied quite a lot on Internet searches for basic information, and found that some of the AI tools were very useful at helping with the development and debugging portions of my C++ code. The project's design-build-test-learn loop included lots and lots of learning (failures). Along the way I learned a few things about microcontrollers and the NodeMCU development board in particular; addressable LED strips and the NeoPixel Ring; the Arduino IDE and C++ programming; HTTP and JSON protocols; Hubitat device drivers and rules, LAN communication protocols, and automation rules; and, finally, how to document what I did.

That being said, I do not claim to have exercised best practices of coding or design, and I'm certain that others with more experience will quickly identify where I have made poor choices with the software and/or hardware. For example, my solution uses an unencrypted communication protocol (HTTP). Since GWIDC message traffic is confined to a secured home network, the risk seemed low. But that's an area where additional development could be useful.

In the end, the GWIDC is perfect for my needs and has proven very reliable with my Hubitat set up. Mostly, the project was a fun learning experience!
