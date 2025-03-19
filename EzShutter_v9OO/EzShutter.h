#ifndef EzDome_h
#define EzDome_h
#include <Arduino.h>;

#define version "2.1b"

////All consts/defins need to be seted!!!!!!!!!
//shutter const  params
#define shut_goto_spd 3000
#define shut_acc 800
#define shut_max_move (long)246500
#define shut_emergency_mult 10 //case of emergency close, multiply the base acc/speed
#define shut_overlap_move 700 //this parameter added to open & close maximum positons to prevent unwatented endstop switch failer overlap

////shutter pin mappings, set the right pins (mine is CNC sheild v3 with wemos d1)
//enstops
#define shutter_espin_open 13 //CNC sheild = X
#define shutter_espin_close 12 //CNC sheild = Y
//buttons
#define shutter_bpin_open 23 
#define shutter_bpin_close 22

//shutter stepper settings
#define shutter_en 21
#define shutter_stp 19
#define shutter_dir 18
#define shutter_en_invert true
#define shutter_stp_invert true
#define shutter_dir_invert false
#define shutter_mot_auto_poff // automatic motor power off higly recommended to power save, uncomment if disable

//relay controll, for cutoff power for tmc2160
#define shutter_rly_pin 19
#define shutter_rly_ctrl true

#define BLE_NAME "EzDome" //this MUST be the same in EzDome
#define BLE_ROTUID "473da924-c93a-11e9-a32f-457a446f6d65"  //BLE_ROTUID this MUST be the same in EzDome
#define BLE_RXUID "473dab7c-5258-5458-0000-457a446f6d65"   //this MUST be the same in EzDome -> BLE_TXUID
#define BLE_TXUID "473dab7c-5458-5258-0000-457a446f6d65"   //this MUST be the same in EzDome -> BLE_RXUID

////CONFIGURATION END////

////DONT TOUCH THESE STUFF BELOW!!!!

////SHUTTER COMMANDS
//IN
#define SHUT_I_INIT 'i'
#define SHUT_I_EMERGENCY_CLOSE 'e' //used for fast closing like rain or etc.
#define SHUT_I_EMERGENCY_STOP  'y'
#define SHUT_I_RESET 'r'  //reboot system
#define SHUT_I_QRY_ENDSTOP 'q' //used for querying endstop


//N/OUT
#define SHUT_IO_CLOSE 'c' //OUT command value closing start 1, 0 closed, -1 endstop not engage
#define SHUT_IO_OPEN 'o'  // OUT command value opening start 1, 0 opened, -1 endstop not engage
#define SHUT_IO_QRY_STEPPER_POS 'p' //query stepper position to determine max open position
//const char SHUT_IO_VOLTAGE = 'w'; //not developed yet

//OUT
#define SHUT_O_INFORMATION 'f'
#define SHUT_O_PING 'g'

//TESTING
#define C_TEST 'x'  //for testing

//ROTATOR SIDE COMMAND
#define SHUT_O_INIT 'i'
////COMMANDS END
#define DELIMITER "#"

//global vars
extern bool shut_es_open;
extern bool shut_es_close;
extern bool emergency_stop;
extern bool shut_opening; // false when closing true when opening
extern bool ble_connected;



#endif