// _*_ IOServer _*_

// Operates the IO Control Module
// Each of the outputs is toggled by a switch input, or controlled by connecting to the
// device and sending the appropriate command to set the state of the output.  The state
// can also be read back.  The connection is by either the console (serial over USB), or
// via ethernet (UDP connection).
//
// Uses Freetronics EtherTen module.  Select Arduino Uno in boards manager.
//
// Makes use of the following 3rd-party libraries:
// mthread
//
// Think3 Ltd, 2013
//
// Revision History
// V1.0  2013-11-30  Initial coding


// System Includes
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <mthread.h>

// Revision Constants
#define VERSION_CODE "V1.0"

// Definitions
#define NUM_CHANNELS    3
#define MAC_EEPROM_START  0
#define ID_NAME_MAX_LEN  30
#define ID_NAME_EEPROM_START  6
#define CH_NAME_MAX_LEN  30
#define CH_NAME_EEPROM_START  36

// IO Point Mapping
int button_pin[] = { 5, 3, 2 };
int output_pin[] = { 7, 8, 9 };
int status_led = 6;

// Global Variables
byte mac[]  = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(2041);
String unit_name = "";
String output_name[NUM_CHANNELS];

