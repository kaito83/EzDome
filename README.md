# EzDome
Free open source observatory Dome firmware with ASCOM support and Native UI.

This project was created because I couldn't find any reasonable free firmware for observatory control.
It doesn't include any fancy features, like Ethernet connection and an off-grid system for power outages.
It only knows what I needed.
Only ESP is supported due to the BLE (bluetooth low energy) connection, although a Nano 32 would also enought to run firmware. 

The system requires two ESP controllers to operate.
In my case, the shutter runs on battery power and is charged by a solar panel.
The shutter acts as the "client" and connects to the rotator, which serves as the "server."

EzShutter can work as stand alone if just want rollof observatory.

Sorry but I won't and don't intend to help with hardware installation or where to connect what and etc. Google and ChatGPT are your friends—look it up!!!
------------
Features:

EzDome(rotator): 
  -   Resolution DD:MM (360*60=21600)
  -   Home find, shortest route on GOTO
  -   PWM controll for fan or etc.
  -   Etc, what a simple Dome needs to know
  -   ALT limit function
  -   DHT sensor data
    
EzShutter(Shutter, rolo, window etc):
  -   Opening / closing with endstops
  -   Movement check with error report
  -   EzShutter can work as standalone if just want roll-off observatory
  -   Function to controll relay for cut power off from TMC driver, to reduce battery consumption
  -   External open/close function


In Arduino IDE download the necessary libraries.
Set your pinouts in EzDome.h, EzShutter.h and other settings (only end of Config end), leave the BLE settings as default unless you know what you're doing.
Flashing with Arduino IDE, choose ESP32 Dev module, 240Mhz Wifi/BT, flash mode DIO.

Serial communication bitrate: 9600
------------

Stand alone UI (EzDome.exe):
- UDP server to commicate with ASCOM driver
- ALT limit function can be enabled
- DHT sensor data
- Port number for only can be modified in registry now (Computer\HKEY_CURRENT_USER\SOFTWARE\EzDome)

ASCOM driver(EzDome Setup.exe):
- I hate VC components (to ugly for me) it's not stande alone anymore, external exe need to controll the dome, communication via UDP, default is port 9999 it can be any, look above

More functions and redesigns may comming!

![Untitled](https://github.com/user-attachments/assets/19ba08c0-2fa1-436f-8312-f864018e1b65)
