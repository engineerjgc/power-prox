
// FUNCTION : read_EEPROM_string() +++++++++++++++++++++++++++++++++++++++++++++++
// Read a string of characters at location (up to maxlen), and return as a String
// Will return what is read up to a null, or a 0xFF character, which generally
// means the EEPROM location has not been written to
String read_EEPROM_string(int location, int maxlen)
{
    String res = "";
    int n, loc = location;
    do {
        n = EEPROM.read(loc++);
        if ((n != 0) && (n != 0xFF)) res = res + char(n);
        if (loc > (location + maxlen)) break;
    } while ((n != 0) && (n != 0xFF));
    return res;
}
// End FUNCTION : read_EEPROM_string() -------------------------------------------


// FUNCTION : read_EEPROM_bytes() ++++++++++++++++++++++++++++++++++++++++++++++++
// Read len characters at location, and return in a character array at dest.
// Returns a pointer to dest.
byte* read_EEPROM_bytes(int location, int len, byte *dest)
{
    byte *n, loc = location;
    for (n = dest; n < (dest + len); n++) {
        *n = EEPROM.read(loc++);
    }
    return dest;
}
// End FUNCTION : read_EEPROM_string() -------------------------------------------


// FUNCTION : write_EEPROM_string() ++++++++++++++++++++++++++++++++++++++++++++++
// Write a string of characters at location (up to maxlen), and return as a String
// Will return what is read up to a null, or a 0xFF character, which generally
// means the EEPROM location has not been written to
void write_EEPROM_string(String val, int location, int maxlen)
{
    int n, loc = location;
    do {      // ***** TO BE WRITTEN ******
        n = EEPROM.read(loc++);
        //if ((n != 0) && (n != 0xFF)) res = res + char(n);
        if (loc > (location + maxlen)) break;
    } while ((n != 0) && (n != 0xFF));
}
// End FUNCTION : write_EEPROM_string() ------------------------------------------


// FUNCTION : write_EEPROM_bytes() +++++++++++++++++++++++++++++++++++++++++++++++
// Write len characters to location from src.
void write_EEPROM_bytes(int location, int len, byte *src)
{
    byte *loc;
    int n;
    loc = src;
    for (n = 0; n < len; n++) {
        EEPROM.write(location + n, *loc++);
    }
}
// End FUNCTION : write_EEPROM_bytes() -------------------------------------------


// FUNCTION : parse_instruction() ++++++++++++++++++++++++++++++++++++++++++++++++
// Parse and execute an instruction string (instr) received from (source)
// Echo incoming and outgoing data to the console
String parse_instruction(String instr, String source)
{
    String resp = "";
    String unit_s;
    int p, unit, st;
    digitalWrite(status_led, LOW);
    instr.trim();
    instr.replace(" ", "");
    instr.toUpperCase();
    
    // Query Command
    if ((p = instr.indexOf("?")) >= 0) {
        switch (instr.charAt(0)) {
            case 'S':
                unit_s = instr.substring(0, p);
                unit = unit_s.charAt(unit_s.length()-1) - 48;
                if ((unit > 0) && (unit <= NUM_CHANNELS)) resp = "STATE" + String(unit) + "=" + String(digitalRead(output_pin[unit-1]),DEC);
                else resp = "UNKNOWN CHANNEL";
                break;
            case 'C':
                unit_s = instr.substring(0, p);
                unit = unit_s.charAt(unit_s.length()-1) - 48;
                if ((unit > 0) && (unit <= NUM_CHANNELS)) resp = "CHNAME" + String(unit) + "=" + output_name[unit-1];
                else resp = "UNKNOWN CHANNEL";
                break;
            case 'N':
                resp = "NAME=" + unit_name;
                break;
            default:
                resp = "UNKNOWN COMMAND";
        }
    }
    
    // Assignment Command
    else if ((p = instr.indexOf("=")) >= 0) {
        switch (instr.charAt(0)) {
            case 'S':
                unit_s = instr.substring(0, p);
                unit = unit_s.charAt(unit_s.length()-1) - 48;
                st = instr.charAt(instr.length()-1) - 48;
                if (st) st = 1;
                if ((unit > 0) && (unit <= NUM_CHANNELS)) {
                    if (st == 0) digitalWrite(output_pin[unit-1], LOW);
                    else digitalWrite(output_pin[unit-1], HIGH);
                    resp = "STATE" + String(unit) + "=" + String(st,DEC) + ":OK";
                }
                else resp = "UNKNOWN CHANNEL";
                break;
            case 'C':
                unit_s = instr.substring(0, p);
                unit = unit_s.charAt(unit_s.length()-1) - 48;
                if ((unit > 0) && (unit <= NUM_CHANNELS)) {
                    output_name[unit-1] = instr.substring(p+1, p+CH_NAME_MAX_LEN);
                    write_EEPROM_string(output_name[unit-1], CH_NAME_EEPROM_START * (unit-1), CH_NAME_MAX_LEN);
                    resp = "CHNAME" + String(unit) + "=" + output_name[unit-1] + ":OK";
                }
                else resp = "UNKNOWN CHANNEL";
                break;
            case 'N':
                unit_name = instr.substring(p+1, p+ID_NAME_MAX_LEN);
                write_EEPROM_string(unit_name, ID_NAME_EEPROM_START, ID_NAME_MAX_LEN);
                resp = "NAME=" + unit_name + ":OK";
                break;
            default:
                resp = "UNKNOWN COMMAND";
        }
    }
    
    // General Command
    else if (instr.length() > 0) {
         switch (instr.charAt(0)) {
            case 'R':
                resp = "RESTARTING";
                break;
            default:
                resp = "UNKNOWN COMMAND";
         }
    }
    
    // Echo command to console
    Serial.println("[" + source + "] -> " + instr);
    Serial.println("[" + source + "] <- " + resp);

    digitalWrite(status_led, HIGH);
    return resp;
}
// End FUNCTION : parse_instruction() --------------------------------------------

