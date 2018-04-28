# esp8266-arduino-wifirgb

A RESTful API implementation to control an RGB LED Strip using an ESP8266 microcontroller. Supports RGB or HSV color model.

## Schematics
![WifiRGB Schematics](https://github.com/c-klinger/esp8266-arduino-wifirgb/raw/master/schematics/wifirgb_schematics.png)

The circuit uses an LD1117V33 voltage regulator with two capacitors as specified in the datasheet. The connection of the ESP-12 is spoecified in th the [improved stability schematic](https://github.com/esp8266/Arduino/blob/master/doc/ESP_improved_stability.png) of the [ESP8266 for Arduino](https://github.com/esp8266/Arduino/). Three IRLB8721 N-channel MOSFETs (one per RGB Stripe channel) are used as power transistors to save our ESP-12 from an early death. The two tactile switches are added for easier programming.

## Code
### Configuration

In the main Arduino Sketch [WifiRGB.ino](https://github.com/c-klinger/esp8266-arduino-wifirgb/blob/master/esp8266-arduino-wifirgb/WifiRGB.ino) some changes need to be done before uploading to the ESP8266 microcontroller.

#### Wifi Network Settings
Required network settings are the Network SSID and password:
```c
const char* ssid = "your_ssid";
const char* password = "your_wifi_password";
const char* deviceName = "wifi-rgb";
```

You can choose between a static or -with DHCP- dynamic IP adress. Default is an static IP adress configured in the following code block:
```c
IPAddress clientIP(192, 168, 178, 250); //the IP of the device
IPAddress gateway(192, 168, 178, 1); //the gateway
IPAddress subnet(255, 255, 255, 0); //the subnet mask
```

If you want to use DHCP just ignore those settings and comment or remove the following line:
```c
WiFi.config(clientIP, gateway, subnet);
```

#### Internal LED
The internal microcontroller LED is used to indicate the Wifi connection status.
```c
#define BUILTIN_LED 2 // internal ESP-12 LED on GPIO2
```

#### RGB Output Pins
If you want to use own schematics you can adjust the different output channels in the following code block.

```c
#define REDPIN 12
#define GREENPIN 14
#define BLUEPIN 5
```

## Usage

### RESTful API

The controller accecpts POST request at http://ip-adress/api/v1/state. The request must contain one of the following JSON objects in body.

#### RGB
```json
{
  "state": "ON",
  "brightness": 100,
  "color": {
    "mode": "rgb",
    "r": 255,
    "g": 125,
    "b": 75
  },
  "mode": "SOLID"
}
```

#### HSV
```json
{
  "state": "ON",
  "brightness": 100,
  "color": {
    "mode": "hsv",
    "h": 250,
    "s": 50,
    "v": 50
  },
  "mode": "SOLID"
}
```

### Graphical User Interface

A User Interface using [iro.js](https://github.com/jaames/iro.js) is available at http://ip-adress/ui. An Internet connection is required to load additional Javascript libraries from CDN.

## License
Copyright (c) 2018 Chris Klinger. Licensed under MIT license, see  [LICENSE](https://github.com/c-klinger/esp8266-arduino-wifirgb/blob/master/LICENSE.md) for the full license.

## Bugs
See <https://github.com/c-klinger/esp8266-arduino-wifirgb/issues>.
