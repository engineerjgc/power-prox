// -*- mode: c++ -*-

// Arduino-Compatible Multi-Threading Library (mthread)

// Copyright (C) 2010-2012 Jonathan Lamothe <jonathan@jlamothe.net>

// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this program.  If not, see:
// http://www.gnu.org/licenses/

// See license.txt for more details.

// * * *

// This program demonstrates the SwitchInput class.  It assumes three
// switches are hooked up to pins 2, 3 and 4.  The switch on pin 2
// should be a dry contact to ground (the internal pull-up is used on
// this switch).  The switch on pin 3 should be a dry contact to
// ground with an external pull-up resistor.  The switch on pin 4
// should connect to the device's power output, with a pull-down
// resistor to ground.  There shold also be an LED connected to pin 5
// (with an appropriate resistor).

// When any of these switches close, the LED will illuminate.  When
// any of the switches open, the LED will go out.

// System Includes
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <mthread.h>

// Definitions
#define NUM_CHANNELS 3

// IO Point Mapping
int button_pin[] = { 5, 3, 2 };
int output_pin[] = { 7, 8, 9 };
int status_led = 6;

// Global Variables
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(2041);


// CLASS : ToggleButton ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Operates each of the buttons by toggling the corresponding output
// when the button is pressed.
// Each button runs as a seperate thread

class ToggleButton : public SwitchInput
{
public:
    ToggleButton(int pin, Type type, int ch);
    void on_close();
protected:
    int channel;
};

ToggleButton::ToggleButton(int pin, Type type, int ch) :
    SwitchInput(pin, DEFAULT_DEBOUNCE, type)
{
    channel = ch;
}

void ToggleButton::on_close()
{
    Serial.print("Toggle channel ");
    Serial.println(channel + 1);
    digitalWrite(output_pin[channel], !digitalRead(output_pin[channel]));
}

// End CLASS : ToggleButton -----------------------------------------------------


// FUNCTION : setup() +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Main setup function for Arduino OS
void setup()
{    
    // Configure I/O Ports and event handlers for pushbuttons
    int i;
    for(i = 0; i < NUM_CHANNELS; i++)
    {
        main_thread_list->add_thread(new ToggleButton(button_pin[i], SwitchInput::pull_up_internal, i));
        pinMode(output_pin[i], OUTPUT);
        digitalWrite(output_pin[i], LOW);
    }
    pinMode(status_led, OUTPUT);
    digitalWrite(status_led, LOW);

    // Setup serial debug port, and send a message displaying what we are
    Serial.begin(9600);
    delay(1000);
    Serial.println("");
    Serial.println("===================================================");
    Serial.println("I/O Controller, V1.0");
    Serial.println("Think3 Ltd, 2013");
    Serial.println("think3.co.nz");
    Serial.println("===================================================");
    Serial.println("");
}

// End FUNCTION : setup() --------------------------------------------------------
