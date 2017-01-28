#include <math.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiUdp.h>
ESP8266WebServer server(80);

#define AP_SSID "ANNInet"               // AP Network
#define AP_PASS "lukas8-kati8"             // PW Network

//** GENERAL **
String HOME_ADDR = "Überherrn";    // Home address
String WORK_ADDR = "Saarlouis";     // Work address

//** WEATHER **
String WOEID;
int OUT_TEMP = 0;                               // Weather temperature
String WEATHER_STATE;                           // State of weather [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear
String SUNRISE;                                 // Sunrise time
String CITY = "Saarlouis";

//** ALARM **
int ALARM_HOUR[3] = {23,23,23};                 // Hour of alarm
int ALARM_MINUTE[3] = {59,59,59};               // Minute of alarm
bool ALARM_ACTIV[3] = {true, false, false};     // Active alarms
bool WEATHER_ALARM = false;
bool SUNRISE_ALARM = false;
bool TRAFFIC_ALARM = false;
bool set = false; // helps to set alarm to weather, sunrise or traffic only once
bool alarmDays[7] = { false, false, false, false, false, false, false };     // days the alarm goes off SUN, MON ...

//** TRAFFIC **
int ADD_TRAVEL_TIME = 0;                        // Additional time for travel and traffic

//** TIME NTP **
const unsigned int localPort = 80;
IPAddress timeServerIP;
const char* ntpServerName = "pool.ntp.org";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
WiFiUDP udp;

//** TIMING **
long previousMillisBlink = 0;
const long intervalBlink = 1000;                 // 1 sec
long previousMillisMinute = 0;
const long intervalMinute = 60000;               // 1 minute
long previousMillisTime = 0;
const long intervalTime = 3600000;               // 1 hour

/*//** Other ** [WIP]
   long buttonTimer = 0;
   long longPressTime = 250;
   bool buttonActive = false;
   bool longPressActive = false;*/

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
  segmentAnzeige(HOUR, MINUTE, ceil(v/17));
}

unsigned int count = 0;
void loop() {
  server.handleClient(); //webserver handle clients
  float light = readLightLevel();

  //** Brightness of the display and display of things
  int v = (round(pow((light/2), 0.85)) + 2); // Calc the brightness lightLevel^0.8
  if(((HOUR >= ALARM_HOUR[0] && ALARM_ACTIV[0]) || (HOUR >= ALARM_HOUR[1] && ALARM_ACTIV[1])
      || (HOUR >= ALARM_HOUR[2] && ALARM_ACTIV[2]) || HOUR >= 5) && (HOUR <= 23)) // Shut off display at night (eg. night mode)
    matrixAnzeige(displayInfos(), count, v); // Draw information of screen

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
    }
    displaySegment(v*4);

    //** Check for alarm
    if ((HOUR == ALARM_HOUR[0] && MINUTE == ALARM_MINUTE[0] && ALARM_ACTIV[0])
        || (HOUR == ALARM_HOUR[1] && MINUTE == ALARM_MINUTE[1] && ALARM_ACTIV[1])
        || (HOUR == ALARM_HOUR[2] && MINUTE == ALARM_MINUTE[2] && ALARM_ACTIV[2])) {
      alarm();
      set = false;
    }

    //** Check for alarm changes
    if((set == false) && MINUTE == ALARM_MINUTE[0] && HOUR == (ALARM_HOUR[0] - 1 && ALARM_ACTIV[0])) {
      connectWiFi();
      if(WEATHER_ALARM)
        if(WiFi.status() == WL_CONNECTED) {
          getWeatherData();
          getTempData();
          setAlarmByWeather();
        }
      if(SUNRISE_ALARM)
        setAlarmBySunrise();
      if(TRAFFIC_ALARM)
        setAlarmByTraffic();
      set = true;
    }

    previousMillisMinute = currentMillisMinute; // Reset timer
  }

/*
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
   }*/

  //** every hour do
  unsigned long currentMillisTime = millis();
  if (currentMillisTime - previousMillisTime >= intervalTime) {
    if (WiFi.status() != WL_CONNECTED && (((HOUR >= ALARM_HOUR[0] && ALARM_ACTIV[0]) || (HOUR >= ALARM_HOUR[1] && ALARM_ACTIV[1])
                                           || (HOUR >= ALARM_HOUR[2] && ALARM_ACTIV[2]) || HOUR >= 5) && (HOUR <= 23))) {
      connectWiFi();            // Connect Wifi if turned off before
    }
    if (WiFi.status() == WL_CONNECTED) {
      getWeatherData();         // Store current weather data
      getTempData();            // Store current temperatur
      getSunriseData();         // Store sunrise time
      setTime();                // Set time with udp
    }

    //** Shut off WiFi at night (and min 30min on)
    if(HOUR == 23 && millis() >= 30000)
      closeWiFi();

    previousMillisTime = currentMillisTime;     // Reset timer
  }

  count++; // Var for display scroll
  delay(20);
}

void setup() {
  //readFromSettings();         //Get settings
  kitInit();                  //Init board
   
  // String xml = "asdjfadkjgadgö<destination_value>asldfas<value>12345</value>dj<value>888888</value>lajdf<destination_thing>sakldfösafj";
  // searchXML(xml, "<destination_value>", "<value>", "</value>");

  if (WiFi.status() == WL_CONNECTED) {
    //TODO set settings (alarm eg)
    //TODO: retrieve city data from webserver/settingsfile
    WOEID = getWoeid();       // Use for getting location id
    getWeatherData();         // Store current weather data
    getTempData();            // Store current temperatur
    getSunriseData();         // Store sunrise time
    setTime();                // Set time with udp
  }

  Serial.println("Webserver starting");
  server.on("/", serverHomepage);     //Set webserver
  server.begin();                     //Init webserver

  changeRightPixel(0, 0, 0);
  displaySegment(10);
}
