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

    // Get the naming from EEPROM
//    read_EEPROM_bytes(MAC_EEPROM_START, 6, mac);
    unit_name = read_EEPROM_string(ID_NAME_EEPROM_START, ID_NAME_MAX_LEN);
    if (unit_name.length() < 1) {
        unit_name = "Output Unit ";
        String macaddr = "";
        for(i = 0; i < 6; i++)
        {
           macaddr += String(mac[i],HEX);
        }
        macaddr.toUpperCase();
        unit_name += macaddr;
    }
    for(i = 0; i < NUM_CHANNELS; i++)
    {
        output_name[i] = read_EEPROM_string(CH_NAME_EEPROM_START + (i * CH_NAME_MAX_LEN), CH_NAME_MAX_LEN);
        if (output_name[i].length() < 1) {
            output_name[i] = "Output " + String(i+1);
        }
    }

    // Setup serial debug port, and send a message displaying what we are
    Serial.begin(9600);
    delay(1000);
    Serial.println("");
    Serial.println("===================================================");
    Serial.print("I/O Controller, ");
    Serial.println(VERSION_CODE);
    Serial.println("Think3 Ltd, 2015");
    Serial.println("think3.co.nz");
    Serial.println("---------------------------------------------------");
    Serial.println("Unit Name:");
    Serial.println(unit_name);
    Serial.println("");
    Serial.println("Ethernet MAC Address:");
    for(i = 0; i < 6; i++)
    {
        Serial.print(mac[i], HEX);
        if (i < 5) Serial.print(":");
    }
    Serial.println("");
    Serial.println("");
    Serial.println("Channel Names:");
    for(i = 0; i < NUM_CHANNELS; i++)
    {
      Serial.println("CH" + String(i+1) + " = " + output_name[i]);
    }
    Serial.println("===================================================");
    Serial.println("");

    // Start the ethernet subsystem
    main_thread_list->add_thread(new EthernetScan());
}
// End FUNCTION : setup() --------------------------------------------------------


