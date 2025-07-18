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
  -   Resolution DD:DDD (360*100=36000 or up to 1000)
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

My HW setup:
- two esp32 for rotator and sutter
- two MKS TMC2160OC
- two NEMA23 1.8°
- TWO 10:1 gearbox
- OPO sensor for rotator home
- two endswitch for shutter

Some tips:
- In native UI if you change the port you should restart it.
- The native UI cannot be closed when dome is connected.
- Roof can be controlled via serial (stand alone mode for roof controll), the roof calibration must be should this way, dont turn on rotator ESP during this procedure!!!
- During the roof calibration to determine the **shut_max_move** value there is a prodecure in function.cpp which instantly stop the moving when endstops hited. 
  This is the **void f_controls::monitor_es()**, this can be used for to measure the shut_max_move, just comment out the **stp.set_positon(shut_max_move);** -> **//stp.set_positon(shut_max_move);**
  Shutter must be closed and all endstop must be seted well btw this is the most important thing in roof!
  Type in Arduino IDE into the Serial monitor command **o#0** this will open the roof wait for the shutter to open fully and stop by itself type **p#0** and you will get the value for shut_max_move.

More functions and redesigns may comming!

![Untitled](https://github.com/user-attachments/assets/19ba08c0-2fa1-436f-8312-f864018e1b65)
