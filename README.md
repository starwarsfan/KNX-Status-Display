# DIY Status Display

Based on [DIY Statusdisplay from KNXUF](https://knx-user-forum.de/forum/%C3%B6ffentlicher-bereich/knx-eib-forum/diy-do-it-yourself/1442271-diy-statusdisplay)

See folder [v1.3](v1.3) for original content

## Requirements

### Hardware

See [List on KNXUF](https://knx-user-forum.de/forum/%C3%B6ffentlicher-bereich/knx-eib-forum/diy-do-it-yourself/1442271-diy-statusdisplay#post1736599). Unfortunately EXP-Tech is no longer selling to end users, so you need to use a shop like [Ali-Express](https://de.aliexpress.com/wholesale?catId=0&initiative_id=SB_20220129141429&origin=y&SearchText=Nextion+HMI+2.4) for the display.

### Software

#### Arduino IDE

Download latest version from [here](https://www.arduino.cc/en/software)

#### KNX-TpUart

* Download [source zip](https://github.com/majuss/KnxTpUart/archive/refs/heads/master.zip) from Github
* If using Arduino IDE:
  * Create folder `%USERPROFILE%\Documents\Arduino\libraries\KnxTpUart`
  * Copy content of folder `src/` into folder `%USERPROFILE%\Documents\Arduino\libraries\KnxTpUart`

#### Nextion

* [Nextion Editor](https://nextion.tech/nextion-editor/) to create the display layout
* Nextion sources to interact with the display. Download [latest release](https://github.com/itead/ITEADLIB_Arduino_Nextion/releases/tag/v0.7.0) from Github (currently 0.7.0)
* If using Arduino IDE:
  * Extract downloaded archive into folder `%USERPROFILE%\Documents\Arduino\libraries\`

#### Inkscape

To create the images/backgrounds etc. for the display something like [Inkscape](https://inkscape.org/).

## Nextion-Tweaks

To enable interaction with the Nextion display in both ways, `NexConfig.h` needs to be tweaked:

* Open `%USERPROFILE%\Documents\Arduino\libraries\ITEADLIB_Arduino_Nextion-0.7.0\NexConfig.h` with editor of your choice
* Find the following snippet:
  ```
  /**
   * Define nexSerial for communicate with Nextion touch panel. 
   */
  #define nexSerial Serial2
  ```
* Comment the `#define...` statement and add three additional lines like this:
  ```
  /**
   * Define nexSerial for communicate with Nextion touch panel. 
   */
  //#define nexSerial Serial2
  #include <SoftwareSerial.h>
  extern SoftwareSerial HMISerial;
  #define nexSerial HMISerial
  ```
  (Credits to https://www.hackster.io/tsavascii/nextion-lcd-communicate-with-arduino-uno-188a44)

# Make it fly ;-)
1. Create background images with a tool of your choice
2. Create the display content using Nextion HMI editor. As an example see [Nextion/DisplaySmall.HMI](Nextion/DisplaySmall.HMI)
3. Flash the created *.tft file onto the display
4. Open [Status-Display-Sketch/Status-Display-Sketch.ino](Status-Display-Sketch/Status-Display-Sketch.ino) with Arduino IDE and modify to your needs. Especially the lines with the definitions of your KNX group addresses and the line with the physical address of the display.
5. Compile and upload to the Arduino Micro

# The Example
The example on this repository is for the display **NX3224T024** with a resolution of 320x240px. It's using landscape mode and has four pages:
* Welcome page
* Please wait, it's ringing
* Error page regarding finger print reader
* Simple keypad page

## Plain Display Behaviour
* The first two pages have an invisible button top left, with which you can jump to the next page
* The third page opens page four on "Yes" and goes back to page one on "No"
* The fourth page (keypad) just sends the component ID of each button

## The Sketch
* The sketch on [Status-Display-Sketch/Status-Display-Sketch.ino](Status-Display-Sketch/Status-Display-Sketch.ino) defines three group addresses to interact:
  * `knxGA01` should be DPT5 and is used to send the number of the page, which should be displayed. Number starts at `1` for the 1st page, `2` for the 2nd a.s.o. 
  * Sending `0` to `knxGA01`, the display will be turned off.
* Physical address defined on line 25
* 12 Buttons, which send their "value" back to KNX
  * Number buttons 0-9 as Integer (1ByteInt, DPT5) to `knxGA02`
  * Boolean (1Bit, DPT1) to`knxGA03`
    * `true` for OK
    * `false` for Cancel