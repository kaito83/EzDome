#ifndef EzDome_h
#define EzDome_h
#include <Arduino.h>;

#define version "2.49b"

String getValue(String data, char separator, int index);

////CONFIG SECTION////
//rotator const  params
#define rot_microstp 8                  //NO Software microstepping, set on the hardware
#define rot_step_rev 200 * rot_microstp  //motor steps * microsteps
#define rot_goto_spd 1100               //speed
#define rot_acc 250                     //acceleration
#define find_home_overlap_percent 1.02   //overlap during the home search

//rotator gear ratios, example big gear 1000 little 10, ratio is 1000:10 / 100:1, G1 = 100
#define G1 36
#define G2 4

////rotator pin mappings, set the right pins
//Endstop
#define rotator_es_home 34
//rotator stepper
#define rotator_en 18
#define rotator_stp 21
#define rotator_dir 19
#define rotator_en_invert true
#define rotator_stp_invert false
#define rotator_dir_invert true

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
#define rot_full_rotation (long)G1 * G2 * rot_step_rev
#define rot_step_DM float(rot_full_rotation) / float(21600)  // for azimuthDM

////ROTATOR COMMANDS
//IN
#define ROT_I_REQUEST_INIT 'I'
#define ROT_I_STOP 'P'            //Stop rotator
#define ROT_I_EMERGENCY_STOP 'Y'  //Emergency stop rotator
#define ROT_I_SYNC 'S'            //Sync to mount
#define ROT_I_RESET 'R'           //reset, reboot
#define ROT_I_QUERY_POS 'Q'       //query position
#define ROT_I_FAN 'A'             //fan controll

//IN/OUT
#define ROT_IO_DMPOS 'D'  //GOTO DM position
#define ROT_IO_HOME 'H'   // Rotator find home

//OUT
#define ROT_O_PARAMS 'P'
////ROT_O_PARAMS params type with values, example :P#2:460800
//  0 firmware version
//  1 motor_step_rev(step*micro)
//  2 rotator_full_rotation / step
//  3 steps/DM:
//  4 pos_rotator

#define ROT_O_INFORMATION 'F'
////ROT_O_INFORMATION
// 0 Rotator emergency STOP!
// 1 Rotator STOP!
// 2 Rotator finding home
// 3 Rotator reached home nothing to do
// 4 Rotator at home, zerosearch finished, reset Rotator pos to 0
#define ROT_O_PING 'G'

////SHUTTER COMMANDS
//IN
#define SHUT_I_INIT 'i'
#define SHUT_I_EMERGENCY_CLOSE 'e'  //used for fast closing like rain or etc.
#define SHUT_I_EMERGENCY_STOP 'y'
#define SHUT_I_RESET 'r'  //reboot system
#define SHUT_I_QRY_ENDSTOP 'q'

//N/OUT
#define SHUT_IO_CLOSE 'c'
#define SHUT_IO_OPEN 'o'
//#define SHUT_IO_VOLTAGE 'w' //not developed yet

//OUT
#define SHUT_O_VER 'v'
#define SHUT_O_INFORMATION 'f'
////SHUT_O_INFORMATION
// 0 BLE disconnected
// 1 BLE Connected
// 2 BLE is running

#define C_TEST 'x'

#define DELIMITER "#"

//global vars
extern bool rot_swathome;
extern bool rot_zerosearch;
extern bool emergency_stop;
extern bool BLE_connected;

#endif