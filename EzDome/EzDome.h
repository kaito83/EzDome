#ifndef EzDome_h
#define EzDome_h
#include <Arduino.h>;

#define version "3.10b"

////CONFIG SECTION, modify only #defines////
//rotator const  params
#define rot_microstp 8                   //NO Software microstepping, set on the hardware
#define rot_step_rev 200 * rot_microstp  //motor steps * microsteps
#define rot_goto_spd 3500                  //speed
#define rot_acc 700                       //acceleration
#define find_home_overlap_percent 1.1   //overlap during the home search
#define rot_home_offset 0//4800 // Applies a step offset if home sensor is not at 0° (true north), (-West,+EAST) 
                              //Math to AZ: rot_home_offset / rot_step_DD(Init param #3 or rot_step_DD in under dont touch section) / resolution_factor
#define rot_max_ALT 87                   //maximum ALT due to roof visibilty, if this value reached the rotator flips 180° and ignoring the azimuth until data lower than rot_max_ALT
#define resolution_factor 100            //min 10,100,1000 max

//rotator gear ratios, example big gear 1000 little 10, ratio is 1000:10 -> 100:1, G1 = 100
#define G1 36  //36
#define G2 10  //10

//Sensors, endstop settings
#define rotator_es_home 13
#define rotator_dht_pin 5
#define rotator_dht_read 30000  //dht update, example: ms 5000 = 5 sec,
#define rotator_dht_type DHT22

////rotator pin mappings, set the right pins
//rotator stepper
#define rotator_en 18 //12 testbaord
#define rotator_stp 21 //25 testboard
#define rotator_dir 19 //27 testbpard
#define rotator_en_invert true
#define rotator_stp_invert false
#define rotator_dir_invert false

//fan pin pwm pins
#define fan0_PWM 32
#define fan0_pin0 33
#define fan0_pin1 25
//#define fan1_PWM 26
//#define fan1_pin0 27
//#define fan1_pin1 14

//BLE settings
#define BLE_NAME "EzDome"                                  //this MUST be the same in EzShutter
#define BLE_ROTUID "473da924-c93a-11e9-a32f-457a446f6d65"  //ROTUID this MUST be the same in EzShutter
#define BLE_TXUID "473dab7c-5258-5458-0000-457a446f6d65"   //this MUST be the same in EzShutter -> BLE_RXUID
#define BLE_RXUID "473dab7c-5458-5258-0000-457a446f6d65"   //this MUST be the same in EzShutter -> BLE_TXUID
////END OFF CONFIG SECTION////


////DONT TOUCH THESE STUFF BELOW!!!!
////calculated params, and other stuff, you don't have to do anything here!!!!!!!!
const long rot_full_rotation = G1 * G2 * rot_step_rev;
const double rot_step_DD = double(rot_full_rotation) / (360.0 * resolution_factor);  // for azimuthDDD.dd stepping

////ROTATOR COMMANDS
//IN
const char ROT_I_REQUEST_INIT = 'I';
const char ROT_I_STOP = 'P';            //Stop rotator
const char ROT_I_EMERGENCY_STOP = 'Y';  //Emergency stop rotator
const char ROT_I_SYNC = 'S';            //Sync to mount
const char ROT_I_RESET = 'R';           //reset, reboot
const char ROT_I_QUERY_POS = 'Q';       //query position
const char ROT_I_FAN = 'A';             //fan controll
const char ROT_I_ALT = 'L';             //Altitude data

//IN/OUT
const char ROT_IO_DDDPOS = 'D';  //GOTO DDD.d*factor position
const char ROT_IO_HOME = 'H';    //Rotator find home

//OUT
const char ROT_O_PARAMS = 'P';
////ROT_O_PARAMS params type with values, example :P#2:460800
//  0 firmware version
//  1 motor_step_rev(step*micro)
//  2 rotator_full_rotation / step
//  3 steps/DDD:
//  4 pos_rotator

const char ROT_O_INFORMATION = 'F';
////ROT_O_INFORMATION
// 0 Rotator emergency STOP!
// 1 Rotator STOP!
// 2 Rotator finding home
// 3 Rotator reached home nothing to do
// 4 Rotator at home, zerosearch finished, reset Rotator pos to rot_home_offset
// 5 Rotator fliped AZ ignored until ALT limit below
// 6 Rotator return to normal AZ movement
// 7 Rotator not at home
const char ROT_O_PING = 'G';
//DHT sensor char
const char ROT_DHT = 'T';

////SHUTTER COMMANDS
//IN
const char SHUT_I_INIT = 'i';
const char SHUT_I_EMERGENCY_CLOSE = 'e';  //used for fast closing like rain or etc.
const char SHUT_I_EMERGENCY_STOP = 'y';
const char SHUT_I_RESET = 'r';  //reboot system
const char SHUT_I_QRY_ENDSTOP = 'q';

//N/OUT
const char SHUT_IO_CLOSE = 'c';
const char SHUT_IO_OPEN = 'o';
//const char  SHUT_IO_VOLTAGE 'w' //not developed yet

//OUT
const char SHUT_O_VER = 'v';
const char SHUT_O_INFORMATION = 'f';
////SHUT_O_INFORMATION
// 0 BLE disconnected
// 1 BLE Connected
// 2 BLE is running

const char C_TEST = 'x';

const String DELIMITER = "#";

//global vars
extern bool rot_swathome;
extern bool rot_zerosearch;
extern bool emergency_stop;
extern bool BLE_connected;
extern bool rot_ignore_AZ;

#endif