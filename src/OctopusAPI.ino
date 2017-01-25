#define PIN_LED 0                 // RGB LED on pin 0
#define BUTTON  2                 // Button on pin 2
#define PIN_ADC A0                // Sound Sensor -> analogRead(PIN_ADC)

//** Rotary encoder knob / button
#include <Encoder.h>
Encoder encoder(14, 12);

//** LEDs
#include <Adafruit_NeoPixel.h>    // NeoPixel LEDs
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2, 13, NEO_RGBW + NEO_KHZ800);

//** LED Matrix
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
Adafruit_IS31FL3731_Wing matrix = Adafruit_IS31FL3731_Wing();

//** Segment matrix
#include <Adafruit_LEDBackpack.h>
#define DISPLAY_ADDRESS   0x70
Adafruit_7segment bar = Adafruit_7segment();

//** Grove Sensor (Digital Light)
#include <Digital_Light_TSL2561.h>

//** WiFi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//**** PreInit
void kitInit() {
  Serial.begin (115200);          //BAUD rate
  Wire.begin();                   //Init Wire
  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");
  pixels.begin();                 //Init LEDs
  matrix.begin();                 //Init LED Matrix
  bar.begin(DISPLAY_ADDRESS);     //Init 7-Segment matrix
  TSL2561.init();                 //Init Digital Light Sensor
  #ifdef AP_SSID
  matrixAnzeigeScroll("Hello", 6);     //Starting screen
  //ESP.eraseConfig();                 //When failing activate
  connectWiFi();                       //Connect to AP
  #else
  WiFi.softAP("TheTime Smart Alarm"); //Access Point for first setup
  changeRightPixel(255, 153, 0);
  matrixAnzeigeScroll(WiFi.softAPIP(), 128);    //Show IP
  #endif
}

void connectWiFi(){
  WiFi.persistent(true);         //Don't save wifi
  WiFi.mode(WIFI_STA);            //Mode: Stationary AP
  WiFi.begin(AP_SSID, AP_PASS);   //Start wifi connection
  changeRightPixel(20, 0, 0);     //Status LED
  for(int i = 0; i <= 10 && WiFi.status() != WL_CONNECTED; i++) {
    delay(1000);
    Serial.print("... ");
  }
  if (WiFi.status() == WL_CONNECTED) {
    changeRightPixel(0, 0, 5);
    udp.begin(localPort);         //Init time server port
    Serial.println("\nWifi successfully connected!");
    Serial.println("connected, my IP:" + WiFi.localIP().toString());
  } else {
    changeRightPixel(0, 0, 0);
    WiFi.forceSleepBegin();       //Deactivate ESP chip
    Serial.println("\nWiFi switched off");
  }
}

void closeWiFi(){
  WiFi.disconnect();
  changeRightPixel(0, 0, 0);
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
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 16000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  while(client.available()) {
    String line = client.readStringUntil('\r');
    antwort = line;
  }
}

//**** Charlieplex Matrix //** pwm 0 - 255
void matrixAnzeige(String text, int x, int pwm) {
  int anzahlPixel = (text.length()) * 6;
  matrix.setTextColor(pwm < 255 ? pwm : 255);        //Color brightness
  matrix.setTextWrap(false);
  matrix.clear();
  matrix.setCursor(-(x%anzahlPixel), 0);
  matrix.print(text);
}

void matrixAnzeigeScroll(String text, int pwm) {
  int anzahlPixel = (text.length()) * 6;
  matrix.setTextColor(pwm < 255 ? pwm : 255);        //Color brightness
  matrix.setTextWrap(false);
  for (int x = 0; x >= -anzahlPixel; x--) { // Scroll text
    matrix.clear();
    matrix.setCursor(x, 0);
    matrix.print(text);
    delay(50);
  }
}

//**** 7-Segment Matrix
void segmentAnzeige(int hours, int minutes, int brightness) {
  bar.setBrightness(brightness < 15 ? brightness : 15); // brightness of the display
  bar.blinkRate(0); //0-3 (0 no blinking)
  int displayValue = hours*100 + minutes; // Time to numeric
  bar.print(displayValue, DEC); // Now print the time value to the display.
  if (hours == 0) {
    bar.writeDigitNum(1, 0);  // Pad hour 0.
    if (minutes < 10)
      bar.writeDigitNum(3, 0); // Prob. needs 0 for < 10 //watch out colon 2 are the dots
  }
  bar.writeDisplay(); // Push to Display
}

void segmentAnzeige(int num) {
  bar.print(num, DEC);
  bar.writeDisplay(); // Push to Display
}

bool isColon = false;
void blinkColon(){ //(seconds % 2 == 0)
  isColon = !isColon;
  bar.drawColon(isColon); // Blink the colon
  bar.writeDisplay(); // Push to Display
}

//**** Left Neopixel LED (RGB)
void changeLeftPixel(uint8_t r, uint8_t g, uint8_t b) {
  #define SEC 0x3F // MagicConstant - do not change
  // 0 - 255
  pixels.setPixelColor(1, g & SEC, r & SEC, b & SEC);
  pixels.show();
}

//**** Right Neopixel LED (RGB)
void changeRightPixel(uint8_t r, uint8_t g, uint8_t b) {
  #define SEC 0x3F // MagicConstant - do not change
  //0 - 255
  pixels.setPixelColor(0, g & SEC, r & SEC, b & SEC);
  pixels.show();
}

//**** Simplefied to get rotary position
int rotaryRead() {
  return encoder.read();
}

//**** Simplefied to get encode button pressed
int buttonPressed() {
  return digitalRead(BUTTON) == LOW;
}

float readLightLevel(){
  return TSL2561.readVisibleLux();
}
