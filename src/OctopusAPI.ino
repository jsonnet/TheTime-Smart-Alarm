#define PIN_LED 0              // normale LED an GPIO 0
#define BUTTON  2              // Druckknopf  an GPIO 2
#define PIN_ADC A0             // Sound Sensor -> analogRead(PIN_ADC)

//** Drehknopf
#include <Encoder.h>           // Drehknopf
Encoder encoder(14, 12);       // Drehknopf ist an GPIO14 und 12 angeschlossen

//** LEDs
#include <Adafruit_NeoPixel.h> // NeoPixel Bibliothek, zwei Pixel an GPIO13
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2, 13, NEO_RGBW + NEO_KHZ800);

//** LED Matrix
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
Adafruit_IS31FL3731_Wing matrix = Adafruit_IS31FL3731_Wing();

//** Bosch environment
#include <SparkFunBME280.h>
BME280 boschBME280;

//** Grove Sensor
#include <Digital_Light_TSL2561.h>

//** WiFi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>


//*************************************************************************************
//**** Initialisierung - IoT-Kit
//****
void kitInit() {
  Serial.begin ( 115200 );
  Wire.begin();
  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");
  pixels.begin();
  //boschBME280init();
  matrix.begin();
  TSL2561.init();
  
#ifdef AP_SSID
  matrixAnzeige("Hello", 6);
  //ESP.eraseConfig();
  connectWiFi();
#endif
}

void connectWiFi(){
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  changeRightPixel(20, 0, 0);
  for(int i = 0; i <= 10 && WiFi.status() != WL_CONNECTED; i++)
    delay(1000);
  if (WiFi.status() == WL_CONNECTED) {
    changeRightPixel(0, 0, 5);
    udp.begin(localPort);
    Serial.println ("\nconnected, my IP:" + WiFi.localIP().toString());
  } else {
    changeRightPixel(0, 0, 0);
    WiFi.forceSleepBegin();
    Serial.println ("\nWiFi switched off");
  }
}

void closeWiFi(){
  // Some if Statement, so webserver can be accessed
  // So only disconnect after some time or in the night or energy saving mode
  WiFi.disconnect();
  changeLeftPixel(0, 0, 0);
  WiFi.forceSleepBegin();
}

void httpGET(String host, String url, String &antwort){
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host.c_str(), httpPort)) {
    Serial.println("connection failed");
    return;
  }
  client.print(String("GET ") + host + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  //Serial.println("request sent");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 6000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line);
    antwort = line;
  }
}

//**** Unterprogramme auslesen des Bosch-Sensors
void boschBME280init() {
  boschBME280.settings.runMode = 3; //  1 forced, 3, Normal mode
  boschBME280.settings.tempOverSample  = 4;
  boschBME280.settings.pressOverSample = 4;
  boschBME280.settings.humidOverSample = 4;
  boschBME280.begin(); // los geht es
}

float temperaturRead() {
  return boschBME280.readTempC();            // Celsius
}
float luftdruckRead() {
  return boschBME280.readFloatPressure();    // Pa
}
float luftfeuchteRead() {                    // Prozent
  return boschBME280.readFloatHumidity();
}


//**** Unterprogramm Textausgabe auf Charlieplex Matrix //** pwm 0 - 255
void matrixAnzeige(String text, int pwm) {
  int anzahlPixel = (text.length()) * 6;
  matrix.setTextColor(pwm);
  matrix.setTextWrap(false);
  for (int x = 1; x >= -anzahlPixel; x--) { // Scrollen
    matrix.clear();
    matrix.setCursor(x, 0);
    matrix.print(text);
    delay(75);
  }
}

//**** Unterprogramm zur Ausgabe auf linkes Neopixel (RGB)
void changeLeftPixel(uint8_t r, uint8_t g, uint8_t b) {
#define SEC 0x3F // MagicConstant - do not change
  // 0 - 255
  pixels.setPixelColor(1, g & SEC, r & SEC, b & SEC); //
  pixels.show();   // Ausgabe der Farbinformation
}

//**** Unterprogramm zur Ausgabe auf rechtes Neopixel (RGB)
void changeRightPixel(uint8_t r, uint8_t g, uint8_t b) {
#define SEC 0x3F // MagicConstant - do not change
  //0 - 255
  pixels.setPixelColor(0, g & SEC, r & SEC, b & SEC); //
  pixels.show();   // Ausgabe der Farbinformation
}

//**** Unterprogramm Auslesen Position Drehknopf / Encoder
int drehRead() {
  return encoder.read();
}

//**** Unterprogramm Abfrage des Drückknopfes / Encoder
int buttonPressed() {
  return digitalRead(BUTTON) == LOW;
}

long readLightLevel(){
  return TSL2561.readVisibleLux();
}
