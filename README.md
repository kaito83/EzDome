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
  -   Etc, what a simple Dome needs to know
    
EzShutter(Shutter, rolo, window etc):
  -   Opening / closing with endstops
  -   Movement check with error report
  -   EzShutter can work as stand alone if just want rollof observatory


Set your pinouts in EzDome.h, EzShutter.h and other settings (only end of Config end), leave the BLE settings as default unless you know what you're doing.
Flashing with Arduino IDE, choose ESP32 Dev module, 240Mhz Wifi/BT, flash mode DIO.
Serial communication bitrate: 9600
