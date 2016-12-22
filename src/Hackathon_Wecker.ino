#include <math.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiUdp.h>
ESP8266WebServer server(80);

#define AP_SSID "Joshua's iPhone"               // AP Network
#define AP_PASS "winnerofhackathon"             // PW Network

//** GENERAL **
// http://maps.google.com/maps/api/geocode/xml?address= or /json?address=bla+bla
String HOME_ADDR [2] = {"49.2397389", "6.694573"};    // Home address
String WORK_ADDR [2] = {"49.319104", "6.751235"};     // Work address

//** WEATHER **
int OUT_TEMP = 0;                               // Weather temperature
String WEATHER_STATE;                           // State of weather [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear
String SUNRISE;                                 //Sunrise time

//** ALARM **
int ALARM_HOUR[3] = {23,23,23};                 // Hour of alarm // *TODO change to array for multiple
int ALARM_MINUTE[3] = {59,59,59};               // Minute of alarm // *TODO change to array
const int RING_FOR = 60000;                     // Millisec of ring time
boolean alarmDays [7] = { false, false, false, false, false, false, false };     // days the alarm goes off SUN, MON ...

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
long previousMillisMinute = 0;
const long intervalMinute = 60000;               // 1 minute
long previousMillisTime = 0;
const long intervalTime = 3600000;               // 1 hour

//** ACTUAL TIME **
int HOUR;
int MINUTE;
int TIMEZONE = 1;
long epochTime;
char date[15];

String _display() {
  if (!buttonPressed()) {
    char str[10];
    sprintf(str, "%d", MINUTE);

    char str2[10];
    sprintf(str2, "%d", HOUR);

    strcat(str2, ":");
    if (MINUTE < 10)
      strcat(str2, "0");
    strcat(str2, str);
    return str2;        // Return HOUR : MINUTES
  } else
    return displayInfos();
}

String displayInfos(){
  char str3[50] = {0};
  memset(str3, 0, sizeof(str3));        // Clear array
  return strcat(strcat(strcat(strcat(strcat(str3, " "), date), " "), WEATHER_STATE.c_str()), " ") + String(OUT_TEMP) + "C";       // Return DATE WEATHER TEMP
}

void loop() {
  server.handleClient();                        //webserver handle clients

  //** Brightness of the display
  int v = round(pow(1.05, rotaryRead()));       //expon. wachstum ? oder besser betrag von expon. verkleinerung
  if (rotaryRead() == 0)
    v = readLightLevel() < 50 ? readLightLevel() + 2 : (double)(readLightLevel() / 100 * 5); //TODO
  matrixAnzeige(_display(), v);                 // Draw information of screen
  //TODO segmentAnzeige(HOUR, MINUTE, 10);
  //TODO blinkColon(bool); (sec % 2 == 0)

  //** every minute do
  unsigned long currentMillisMinute = millis();
  if (currentMillisMinute - previousMillisMinute >= intervalMinute) {
    _adjustTime((currentMillisMinute - previousMillisMinute - intervalMinute) / 1000);    // Adjust time by lost one over updating delay
    MINUTE++;
    epochTime += 60;
    if (MINUTE >= 60) {
      MINUTE -= 60;
      if (HOUR <= 24)
        HOUR++;
      else
        HOUR = 0;
    }
    previousMillisMinute = currentMillisMinute; // Reset timer
  }

  //** Check for alarm
  if (HOUR == ALARM_HOUR[0] && MINUTE == ALARM_MINUTE[0]) { //TODO add multiple alarms
    alarm();
  }

  //** Reupdate time from udp server every hour
  unsigned long currentMillisTime = millis();
  if (currentMillisTime - previousMillisTime >= intervalTime) {
    setTime();
    previousMillisTime = currentMillisTime;     // Reset timer
  }
}

void setup() {
  kitInit();                  //Init board
  readFromSettings();         //Get settings
  
  delay(1000);                //Wait for delay

  if (WiFi.status() == WL_CONNECTED) {
    //getWoeid();             // Use for getting location id
    getWeatherData();         // Store current weather data
    getTempData();            // Store current temperatur
    getSunriseData();         // Store sunrise time
    setTime();                // Set time with udp
  }

  //TODO: Set the settings from webinterface (alarm)

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Webserver starting");
    server.on("/", serverHomepage);     //Set webserver
    server.begin();                     //Init webserver
  }
}
