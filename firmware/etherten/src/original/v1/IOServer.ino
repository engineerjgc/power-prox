/*
 I/O Server
 
 A simple telnet-like server that response to queries, assignments and commands
 for controlling the state of server real-world outputs.
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13 (as per EtherTen board)
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 14 Nov 2013
 by James Cooper
 using basic framework supplied by WebServer example in Arduino IDE
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <Bounce.h>

// Generate global variables here.
// mac[] is the MAC address to use for ethernet
// communications.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(2041);
byte ethernetState = 0;
String cmd = "";
int p;
EthernetClient client;
byte newConnection = true;

// I/O Mapping
int statusLED = 6;
int out1 = 7;
int out2 = 8;
int out3 = 9;
int pushbutton1 = 5;
int pushbutton2 = 3;
int pushbutton3 = 2;

// Debounce pushbuttons
Bounce pb1 = Bounce(pushbutton1, 10); 
Bounce pb2 = Bounce(pushbutton2, 10);
Bounce pb3 = Bounce(pushbutton3, 10);

void setup() {
 // Open serial communications and wait for port to open:
  pinMode(statusLED, OUTPUT);     
  pinMode(out1, OUTPUT);     
  pinMode(out2, OUTPUT);     
  pinMode(out3, OUTPUT);     
  pinMode(pushbutton1, INPUT_PULLUP);     
  pinMode(pushbutton2, INPUT_PULLUP);     
  pinMode(pushbutton3, INPUT_PULLUP);     
  digitalWrite(statusLED, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(out1, LOW);
  digitalWrite(out2, LOW);
  digitalWrite(out3, LOW);
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  delay(2000);
}


void loop() {
  
  // Perform checks every scan
  checkEthernet();
  pb1.update();
  pb2.update();
  pb3.update();
  
  // Operate buttons
  if (pb1.fallingEdge()) digitalWrite(out1, !digitalRead(out1));
  if (pb2.fallingEdge()) digitalWrite(out2, !digitalRead(out2));
  if (pb3.fallingEdge()) digitalWrite(out3, !digitalRead(out3));
  
  // listen for incoming clients
  if (client) {
    if (newConnection == true) {
      digitalWrite(statusLED, HIGH);
      Serial.println("Client connected");
      client.println("IOCONTROL V0.1");
      while (client.available()) client.read();  // Flush any chars in the incoming buffer
      boolean currentLineIsBlank = true;
      client.print(">");  // For console use
      newConnection = false;
    }
    if (client.connected()) {
      if (client.available()) {

        // Get new character and add to command string
        digitalWrite(statusLED, LOW);
        char c = client.read();
        if (c != 0x0A) {
          if (c != 0x0D) cmd = cmd + c;
        }
        else {
          Serial.println("");
          cmd.trim();
          cmd.replace(" ", "");
          cmd.toUpperCase();
          if ((p = cmd.indexOf("?")) >= 0) {
            Serial.print("QUERY: ");
            Serial.println(cmd);
            if (cmd.charAt(0) == 'S') {
              String unit_s = cmd.substring(0, p);
              int unit = unit_s.charAt(unit_s.length()-1) - 48;
              switch (unit) {
                case 1:
                  client.print("STATE1=");
                  client.println(digitalRead(out1),DEC);
                  break;
                case 2:
                  client.print("STATE2=");
                  client.println(digitalRead(out2),DEC);
                  break;
                case 3:
                  client.print("STATE3=");
                  client.println(digitalRead(out3),DEC);
                  break;
                default:
                  client.println("UNKNOWN OUTPUT");
                  break;
              }
            }
            else {
              client.println("UNKNOWN PARAMETER");
            }
          }
          else if ((p = cmd.indexOf("=")) >= 0) {
            Serial.print("ASSIGNMENT: ");
            Serial.println(cmd);
            if (cmd.charAt(0) == 'S') {
              String unit_s = cmd.substring(0, p);
              int unit = unit_s.charAt(unit_s.length()-1) - 48;
              int st = cmd.charAt(cmd.length()-1) - 48;
              switch (unit) {
                case 1:
                  if (st == 0) digitalWrite(out1, LOW);
                  else digitalWrite(out1, HIGH);
                  client.print("STATE1=");
                  client.print(st,DEC);
                  client.println(":OK");
                  break;
                case 2:
                  if (st == 0) digitalWrite(out2, LOW);
                  else digitalWrite(out2, HIGH);
                  client.print("STATE2=");
                  client.print(st,DEC);
                  client.println(":OK");
                  break;
                case 3:
                  if (st == 0) digitalWrite(out3, LOW);
                  else digitalWrite(out3, HIGH);
                  client.print("STATE3=");
                  client.print(st,DEC);
                  client.println(":OK");
                  break;
                default:
                  client.println("UNKNOWN OUTPUT");
                  break;
              }
            }
            else {
              client.println("UNKNOWN PARAMETER");
            }
          }
          else if (cmd.length() > 0) {
            Serial.print("COMMAND: ");
            Serial.println(cmd);
            if (cmd.charAt(0) == 'Q') {
              client.println("BYE");
              delay(1);
              client.stop();
              digitalWrite(statusLED, LOW);
              Serial.println("");
              Serial.println("Client disonnected.");
              newConnection = true;
            }
            else {
              client.println("UNKNOWN COMMAND");
            }
          }
          cmd = "";
          if (!newConnection) {
            client.print(">");  // For console use
            digitalWrite(statusLED, HIGH);
          }
        }
      }
    }
  }
  else {
    client = server.available();
  }
}

void checkEthernet() {
  if (ethernetState == 0) {
    if (Ethernet.begin(mac) != 0) {
      server.begin();
      Serial.print("Server alive at ");
      Serial.println(Ethernet.localIP());
      ethernetState = 1;
    }
  }
}
