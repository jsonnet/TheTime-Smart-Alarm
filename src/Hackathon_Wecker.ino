#include <math.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiUdp.h>
ESP8266WebServer server(80);

//#define AP_SSID "Joshua's iPhone"               // AP Network
//#define AP_PASS "winnerofhackathon"             // PW Network

//** GENERAL **
// http://maps.google.com/maps/api/geocode/xml?address= or /json?address=bla+bla
String HOME_ADDR [2] = {"49.2397389", "6.694573"};    // Home address
String WORK_ADDR [2] = {"49.319104", "6.751235"};     // Work address
bool Segment = true;

//** WEATHER **
String WOEID;
int OUT_TEMP = 0;                               // Weather temperature
String WEATHER_STATE;                           // State of weather [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear
String SUNRISE;                                 //Sunrise time
String CITY = "Saarlouis";

//** ALARM **
int ALARM_HOUR[3] = {23,23,23};                 // Hour of alarm
int ALARM_MINUTE[3] = {59,59,59};               // Minute of alarm
const int RING_FOR = 60000;                     // Millisec of ring time
bool alarmDays [7] = { false, false, false, false, false, false, false };     // days the alarm goes off SUN, MON ...

//** TRAFFIC **
const String MAPS_HOST = "maps.googleapis.com/maps/api/distancematrix/xml?origins=" + HOME_ADDR[0] + "," + HOME_ADDR[1] + "&destinations=" + WORK_ADDR[0] + "," + WORK_ADDR[1] + "&key=[key]";
// response ["rows"][0]["elements"][0]["duration_in_traffic"]["value"]
int ADD_TRAVEL_TIME = 0;                        // Additional time for travel and traffic

//** TIME NTP **
const unsigned int localPort = 80;
IPAddress timeServerIP;
const char* ntpServerName = "pool.ntp.org";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];
WiFiUDP udp;

//** TIMING **
long previousMillisBlink = 0;
const long intervalBlink = 1000;                 // 1 sec
long previousMillisMinute = 0;
const long intervalMinute = 60000;               // 1 minute
long previousMillisTime = 0;
const long intervalTime = 3600000;               // 1 hour

//** Other **
long buttonTimer = 0;
long longPressTime = 250;
bool buttonActive = false;
bool longPressActive = false;

//** ACTUAL TIME **
int HOUR;
int MINUTE;
int TIMEZONE = 1;
long epochTime;
char date[15];

String displayInfos(){
  char str3[50] = {0};
  memset(str3, 0, sizeof(str3));        // Clear array
  return strcat(strcat(strcat(strcat(strcat(str3, " "), date), " "), WEATHER_STATE.c_str()), " ") + String(OUT_TEMP) + "C";       // Return DATE WEATHER TEMP
}

void displaySegment(int v){
  segmentAnzeige(HOUR, MINUTE, (v/17)+1);
}

int count = -1;
void loop() {
  server.handleClient();                        //webserver handle clients
  count++;
  float light = readLightLevel();
  int rotary = rotaryRead();

  //** Brightness of the display and display of things
  int v = round(pow(1.05, rotary));       //expon. wachstum ? oder besser betrag von expon. verkleinerung
  if (rotary == 0)
    v = light < 50 ? light + 2 : (double)(light / 50 * 5);
  if(rotary <= -4)
    v = 0;
  matrixAnzeige(displayInfos(), count, v);                 // Draw information of screen

  unsigned long currentMillisBlink = millis();
  if (currentMillisBlink - previousMillisBlink >= intervalBlink) {
    blinkColon();                          // toggle colon on time segment
    previousMillisBlink = currentMillisBlink;
  }

  //** every minute do
  unsigned long currentMillisMinute = millis();
  if (currentMillisMinute - previousMillisMinute >= intervalMinute) {
    _adjustTime((currentMillisMinute - previousMillisMinute - intervalMinute) / 1000);    // Adjust time by lost one over updating delay
    MINUTE++;
    epochTime += 60;
    if (MINUTE >= 60) { // reset to 0 when minute would be 60
      MINUTE -= 60;
      if (HOUR <= 24) // reset to 0 when hour would be 24
        HOUR++;
      else
        HOUR = 0;
    displaySegment(v*4);
    }
    previousMillisMinute = currentMillisMinute; // Reset timer
  }

  //** Check for alarm
  if ((HOUR == ALARM_HOUR[0] && MINUTE == ALARM_MINUTE[0]) || (HOUR == ALARM_HOUR[1] && MINUTE == ALARM_MINUTE[1]) || (HOUR == ALARM_HOUR[2] && MINUTE == ALARM_MINUTE[2])) {
    alarm();
  }

  //** Config mode [WIP]
  if (buttonPressed()) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true)
        longPressActive = false;
      else
        Serial.println("short press");
      buttonActive = false;
    }
  }

  //** Reupdate time from udp server every hour
  unsigned long currentMillisTime = millis();
  if (currentMillisTime - previousMillisTime >= intervalTime) {
    setTime();
    previousMillisTime = currentMillisTime;     // Reset timer
  }
  delay(20);
}

void setup() {
  kitInit();                  //Init board
  //readFromSettings();         //Get settings

  if (WiFi.status() == WL_CONNECTED) {
    //TODO set settings (alarm eg)
    //TODO: retrieve city data from webserver/settingsfile
    WOEID = getWoeid();       // Use for getting location id
    getWeatherData();         // Store current weather data
    getTempData();            // Store current temperatur
    getSunriseData();         // Store sunrise time
    setTime();                // Set time with udp

    Serial.println("Webserver starting");
    server.on("/", serverHomepage);     //Set webserver
    server.begin();                     //Init webserver
  }
  changeRightPixel(0, 0, 0);
}
