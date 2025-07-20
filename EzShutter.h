#ifndef EzDome_h
#define EzDome_h
#include <Arduino.h>;

#define version "2.3b"

////All consts/defins need to be seted!!!!!!!!!
//shutter const  params
#define shut_goto_spd 3500
#define shut_acc 800
#define shut_max_move 124450L // L letter is necessary = long 
#define shut_overlap_move 700 //this parameter added to open & close maximum positons to prevent unwatented endstop switch failer during movement, with overlap

////shutter pin mappings, set the right pins
//enstops
#define shutter_espin_open 14
#define shutter_espin_close 27
//buttons
#define shutter_bpin_open 19 
#define shutter_bpin_close 21

//shutter stepper settings
#define shutter_en 33//12//33
#define shutter_stp 25//25
#define shutter_dir 26//27//26
#define shutter_en_invert true
#define shutter_stp_invert false
#define shutter_dir_invert true

//relay controll, for cutoff power from tmc2160, to minimize power consumption
#define shutter_rly_pin 23

#define BLE_NAME "EzDome" //this MUST be the same in EzDome
#define BLE_ROTUID "473da924-c93a-11e9-a32f-457a446f6d65"  //BLE_ROTUID this MUST be the same in EzDome
#define BLE_RXUID "473dab7c-5258-5458-0000-457a446f6d65"   //this MUST be the same in EzDome -> BLE_TXUID
#define BLE_TXUID "473dab7c-5458-5258-0000-457a446f6d65"   //this MUST be the same in EzDome -> BLE_RXUID

////CONFIGURATION END////

////DONT TOUCH THESE STUFF BELOW!!!!

// SHUTTER COMMANDS
// IN
const char SHUT_I_INIT = 'i';
const char SHUT_I_EMERGENCY_CLOSE = 'e';
const char SHUT_I_EMERGENCY_STOP = 'y';
const char SHUT_I_RESET = 'r';
const char SHUT_I_QRY_ENDSTOP = 'q';

// N/OUT
const char SHUT_IO_CLOSE = 'c';
const char SHUT_IO_OPEN = 'o';
const char SHUT_IO_QRY_STEPPER_POS = 'p';
// const char SHUT_IO_VOLTAGE = 'w'; // not developed yet

// OUT
const char SHUT_O_INFORMATION = 'f';
const char SHUT_O_PING = 'g';

// TESTING
const char C_TEST = 'x';

// ROTATOR SIDE COMMAND
const char SHUT_O_INIT = 'i';

// delimiter
const String DELIMITER = "#";

//global vars
extern bool shut_es_open;
extern bool shut_es_close;
extern bool emergency_stop;
extern bool shut_opening; // false when closing true when opening
extern bool ble_connected;
#endif