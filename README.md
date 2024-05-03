# PowerProx

A power control module in a 1U rack-mount case to allow both local and remote control of AC and DC power outlets.

# User Manual

## Connections (Rear Panel)

* 230VAC Mains Input (IEC)
* 3 x Switched 230VAC Mains Output (IEC)
* 4 x 12VDC Output (2.1mm DC Socket)
* Ethernet 100BaseT Port
* USB Programming & Configuration

## Controls & Indicators (Front Panel)

* Power Indicator (Red)
* Communications Indicator (Yellow)
* 3 x Output Channel Pushbutton with Status Indicator

# Technical Manual

The unit consists of:
* A [Freetronics EtherTen](https://www.freetronics.com.au/products/etherten) Arduino processor unit
* An ESP8266 WiFi bridge
* 3 x 10A Solid-State Output Relays
* Custom circuitry to power the CPU and drive SSR's
* A 12VDC Power Supply
* Front-panel LED's and pushbuttons

![Front View](/assets/images/front.png)

![Rear View](/assets/images/rear.png)

Accompanying that, the code for the unit is wrtten using standard Arduino language, and is included in this repo.
