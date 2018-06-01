#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "names.h"
#include "web_admin.h"
#include "web_interface.h"
#include "web_iro_js.h"

const char* ssid = "your_ssid";
const char* password = "your_wifi_password";
const char* deviceName = "wifi-rgb";

#define BUILTIN_LED 2 // internal ESP-12 LED on GPIO2

#define REDPIN 12
#define GREENPIN 14
#define BLUEPIN 5

ESP8266WebServer server(80);
IPAddress clientIP(192, 168, 178, 254); //the IP of the device
IPAddress gateway(192, 168, 178, 1); //the gateway
IPAddress subnet(255, 255, 255, 0); //the subnet mask

void setup(void) {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(deviceName);
  WiFi.config(clientIP, gateway, subnet); // Remove for DHCP

  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, LOW); // Turn the LED ON by making the voltage LOW after wifi is connected

  // adjust the PWM range
  // see https://esp8266.github.io/Arduino/versions/2.0.0/doc/reference.html#analog-output
  analogWriteRange(255);

  // Root and 404
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  // REST-API
  server.on("/api/v1/state", HTTP_POST, handleApiRequest);

  // iro.js User Interface and Javascript
  server.on("/ui", HTTP_GET, []() {
    server.send(200, "text/html", WEBINTERFACE);
  });
  server.on("/admin", HTTP_GET, []() {
    server.send(200, "text/html", WEBADMIN);
  });
  server.on("/iro.min.js", HTTP_GET, []() {
    server.send(200, "application/javascript", IRO_JS);
  });

  server.begin();
  Serial.println("WifiRGB HTTP server started");
}

void loop(void) {
  server.handleClient();
}


void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266 wifi rgb!");
}


void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleApiRequest() {

    if (server.hasArg("plain") == false) { //Check if body received
      server.send(200, "text/plain", "Body not received");
      return;
    }

    DynamicJsonBuffer jsonBuffer(190);
    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));

    Serial.println("JSON Body: ");
    root.printTo(Serial);
    Serial.println();

    const char* state = root["state"]; // "ON" or "OFF"
    if(strcmp("OFF", state) == 0) {
       Serial.println("Switching OFF!");
      // Set Output
      analogWrite(REDPIN, 0);
      analogWrite(GREENPIN, 0);
      analogWrite(BLUEPIN, 0);

      server.send(200, "application/json", server.arg("plain"));
      return;
    }

    int brightness = root["brightness"];
    Serial.print("Brightness: ");
    Serial.println(brightness);

    RGB rgb = {0, 0, 0};

    root["color"].printTo(Serial);

    // If RGB mode: Parse RGB values
    if(strcmp("rgb", root["color"]["mode"]) == 0) {
      Serial.println("Reading RGB values...");
      rgb.r = root["color"]["r"];
      rgb.b = root["color"]["g"];
      rgb.g = root["color"]["b"];
    }

    // Parse HSV
    if(strcmp("hsv", root["color"]["mode"]) == 0) {
      Serial.println("Reading HSV values...");
      rgb = hsvToRgb(root["color"]["h"], root["color"]["s"], root["color"]["v"]);
    }

    // DEBUG: Parsed values
    Serial.println("Parsed Values:");
    Serial.print("r=");
    Serial.print(rgb.r);
    Serial.print(", g=");
    Serial.print(rgb.g);
    Serial.print(", b=");
    Serial.println(rgb.b);

    // Map Brightness
    byte mappedRed = map(rgb.r, 0, 100, 0, brightness);
    byte mappedGreen = map(rgb.g, 0, 100, 0, brightness);
    byte mappedBlue = map(rgb.b, 0, 100, 0, brightness);

    Serial.println("Brighness Mapped RGB Values:");
    Serial.print("r=");
    Serial.print(mappedRed);
    Serial.print(", g=");
    Serial.print(mappedGreen);
    Serial.print(", b=");
    Serial.println(mappedBlue);


    // TODO: support different modes
    const char* jsonrgbmode = root["mode"]; // "SOLID"

    // Set Output
    analogWrite(REDPIN, mappedRed);
    analogWrite(GREENPIN, mappedGreen);
    analogWrite(BLUEPIN, mappedBlue);

    server.send(200, "application/json", server.arg("plain"));
}

// this is a modified version of https://gist.github.com/hdznrrd/656996
RGB hsvToRgb(double h, double s, double v) {
  int i;
  double f, p, q, t;
  byte r, g, b;

  h = max(0.0, min(360.0, h));
  s = max(0.0, min(100.0, s));
  v = max(0.0, min(100.0, v));

  s /= 100;
  v /= 100;

  if (s == 0) {
    // Achromatic (grey)
    r = g = b = round(v * 255);
    return {0,0,0};
  }

  h /= 60; // sector 0 to 5
  i = floor(h);
  f = h - i; // factorial part of h
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));
  switch (i) {
    case 0:
      r = round(255 * v);
      g = round(255 * t);
      b = round(255 * p);
      break;
    case 1:
      r = round(255 * q);
      g = round(255 * v);
      b = round(255 * p);
      break;
    case 2:
      r = round(255 * p);
      g = round(255 * v);
      b = round(255 * t);
      break;
    case 3:
      r = round(255 * p);
      g = round(255 * q);
      b = round(255 * v);
      break;
    case 4:
      r = round(255 * t);
      g = round(255 * p);
      b = round(255 * v);
      break;
    default: // case 5:
      r = round(255 * v);
      g = round(255 * p);
      b = round(255 * q);
  }

  return {r, g, b};
}
