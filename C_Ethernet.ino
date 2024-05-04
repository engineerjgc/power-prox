// CLASS : EthernetScan ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Operates the ethernet commmunications

bool ethernetState;

class EthernetScan : public Thread
{
public:
    EthernetScan(void);
protected:
    bool loop();
private:
    int id;
};

EthernetScan::EthernetScan(void)
{
  ethernetState = 0;
}

bool EthernetScan::loop()
{
    // Die if requested
    if(kill_flag)
        return false;

    // Print the status message:
    if (ethernetState == 0) {
      if (Ethernet.begin(mac) != 0) {
        server.begin();
        Serial.print("Server alive at ");
        Serial.println(Ethernet.localIP());
        ethernetState = 1;
      }
    }

    // Drop out
    sleep(1);
    return true;
}
