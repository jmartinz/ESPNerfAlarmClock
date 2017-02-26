#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
#include <FS.h>                       // SPIFFS file-system: store web server html, CSS etc.



const char* ssid = "castan";
const char* password = "Eu1gD1MdCnNqA";
MDNSResponder mdns;

ESP8266WebServer server(80);

const int led = 13;
#define PIN   14        // //neopixel on nodeMCU pin D5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);


void handleRoot() {
  digitalWrite(led, 1);
  SPIFFS.begin(); // mount the fs
    File f = SPIFFS.open("/home.html", "r");
    if (f) {
      String s = f.readString();
      server.send(200, "text/html", s);
      f.close();
    } else {
      server.send(200, "text/plain","/home.html not found, have you flashed the SPIFFS?");
    }
  colorAll(strip.Color(0, 0, 255), 500); // Blue*/
  digitalWrite(led, 0);
}

void handleColor() {
    String message = "";
    uint32_t colorVuelta = strip.Color(0, 0, 0);
    if (server.arg(0)=="red"){
      message += "You ordered " + server.argName(0) + ": " + server.arg(0) + "\n";
      colorVuelta = strip.Color(255, 0, 0); // Red
    }
    if (server.arg(0)=="green"){
      message += "You ordered " + server.argName(0) + ": " + server.arg(0) + "\n";
      colorVuelta = strip.Color(0, 255, 0); // Green
    }
    if (server.arg(0)=="blue"){
      message += "You ordered " + server.argName(0) + ": " + server.arg(0) + "\n";
      colorVuelta = strip.Color(0, 0, 255); // Blue
    }

    server.send(200, "text/plain", message);
    colorWipe(colorVuelta, 50); // Red

}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
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

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/color", handleColor);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  strip.begin();
  strip.setBrightness(30); //adjust brightness here
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void){
  server.handleClient();
  // Some example procedures showing how to display to the pixels:

  /*colorWipe(strip.Color(255, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 255, 255), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue*/

}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      strip.setPixelColor(i-5, strip.Color(0, 0, 0));
      strip.show();
      delay(wait);
  }
}
void colorAll(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c );

  }
  strip.show();
  delay(wait);
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));

  }
}
