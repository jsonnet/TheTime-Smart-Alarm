// Use data uploaded to the ESP (file into folder data & Tools -> ESP Sketch Uploader)
#include <ArduinoJson.h>

String network[2];
String weather[2];
String alarm0[12];
String alarm1[9];
String alarm2[9];

void serverHomepage() {
  if (server.hasArg("networkname") && server.hasArg("password")) {
    network[0] = server.arg("networkname");
    network[1] = server.arg("password");
  }
  if (server.hasArg("locationhome") && server.hasArg("locationwork")) {
    weather[0] = server.arg("locationhome");
    weather[1] = server.arg("locationwork");
  }
  if (server.hasArg("alarmtime0") && server.hasArg("sunset0")
      && server.hasArg("snooze0") && server.hasArg("traffic0") && server.hasArg("weather0")) {
    alarm0[0] = server.arg("monday0");
    alarm0[1] = server.arg("tuesday0");
    alarm0[2] = server.arg("wednesday0");
    alarm0[3] = server.arg("thursday0");
    alarm0[4] = server.arg("friday0");
    alarm0[5] = server.arg("saturday0");
    alarm0[6] = server.arg("sunday0");
    alarm0[7] = server.arg("alarmtime0");
    alarm0[8] = server.arg("sunset0");
    alarm0[9] = server.arg("snooze0");
    alarm0[10] = server.arg("traffic0");
    alarm0[11] = server.arg("weather0");
  }
  if (server.hasArg("alarmtime1") && server.hasArg("snooze1")) {
    alarm1[0] = server.arg("monday1");
    alarm1[1] = server.arg("tuesday1");
    alarm1[2] = server.arg("wednesday1");
    alarm1[3] = server.arg("thursday1");
    alarm1[4] = server.arg("friday1");
    alarm1[5] = server.arg("saturday1");
    alarm1[6] = server.arg("sunday1");
    alarm1[7] = server.arg("alarmtime1");
    alarm1[8] = server.arg("snooze1");
  }
  if (server.hasArg("alarmtime2") && server.hasArg("snooze2")) {
    alarm2[0] = server.arg("monday2");
    alarm2[1] = server.arg("tuesday2");
    alarm2[2] = server.arg("wednesday2");
    alarm2[3] = server.arg("thursday2");
    alarm2[4] = server.arg("friday2");
    alarm2[5] = server.arg("saturday2");
    alarm2[6] = server.arg("sunday2");
    alarm2[7] = server.arg("alarmtime2");
    alarm2[8] = server.arg("snooze2");
  }
  saveToSettings(); // Save to Settings after getting data
  server.send(200, "text/html", getPage()); //change here to file
}

String getPage(){
  String s = " ";         // A String
  bool wat = SPIFFS.begin();         // Init ESP filesystem
  //SPIFFS.format();      // ONLY needs to happen once
  File f = SPIFFS.open("/index.html", "r");
  Serial.println(f);
  if(f) {
    s = f.readString();   // Read file
    f.close();            // Close file, needs to happen!
  }
  SPIFFS.end();           // Important close filesystem
  //Serial.println(s);
  return s;               // Return whole file
}

bool readFromSettings() {
  SPIFFS.begin();         // Init ESP filesystem
  File configFile = SPIFFS.open("/settings.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  network[0] = json["apname"].asString();
  network[1] = json["appass"].asString();
  weather[0] = json["lochome"].asString();
  weather[1] = json["locwork"].asString();

  alarm0[0] = json["monday0"].asString();
  alarm0[1] = json["tuesday0"].asString();
  alarm0[2] = json["wednesday0"].asString();
  alarm0[3] = json["thursday0"].asString();
  alarm0[4] = json["friday0"].asString();
  alarm0[5] = json["saturday0"].asString();
  alarm0[6] = json["sunday0"].asString();
  alarm0[7] = json["alarmtime0"].asString();
  alarm0[8] = json["sunset0"].asString();
  alarm0[9] = json["snooze0"].asString();
  alarm0[10] = json["traffic0"].asString();
  alarm0[11] = json["weather0"].asString();

  alarm1[0] = json["monday1"].asString();
  alarm1[1] = json["tuesday1"].asString();
  alarm1[2] = json["wednesday1"].asString();
  alarm1[3] = json["thursday1"].asString();
  alarm1[4] = json["friday1"].asString();
  alarm1[5] = json["saturday1"].asString();
  alarm1[6] = json["sunday1"].asString();
  alarm1[7] = json["alarmtime1"].asString();
  alarm1[8] = json["snooze1"].asString();

  alarm2[0] = json["monday2"].asString();
  alarm2[1] = json["tuesday2"].asString();
  alarm2[2] = json["wednesday2"].asString();
  alarm2[3] = json["thursday2"].asString();
  alarm2[4] = json["friday2"].asString();
  alarm2[5] = json["saturday2"].asString();
  alarm2[6] = json["sunday2"].asString();
  alarm2[7] = json["alarmtime2"].asString();
  alarm2[8] = json["snooze2"].asString();

  Serial.println(weather[0]);

  configFile.close();
  SPIFFS.end();           // Important close filesystem
  return true;
}

bool saveToSettings() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["apname"] = network[0];
  json["appass"] = network[1];
  json["lochome"] = weather[0];
  json["locwork"] = weather[1];
  json["monday0"] = alarm0[0];
  json["tuesday0"] = alarm0[1];
  json["wednesday0"] = alarm0[2];
  json["thursday0"] = alarm0[3];
  json["friday0"] = alarm0[4];
  json["saturday0"] = alarm0[5];
  json["sunday0"] = alarm0[6];
  json["alarmtime0"] = alarm0[7];
  json["sunset0"] = alarm0[8];
  json["snooze0"] = alarm0[9];
  json["traffic0"] = alarm0[10];
  json["weather0"] = alarm0[11];
  json["monday1"] = alarm1[0];
  json["tuesday1"] = alarm1[1];
  json["wednesday1"] = alarm1[2];
  json["thursday1"] = alarm1[3];
  json["friday1"] = alarm1[4];
  json["saturday1"] = alarm1[5];
  json["sunday1"] = alarm1[6];
  json["alarmtime1"] = alarm1[7];
  json["snooze1"] = alarm1[8];
  json["monday2"] = alarm2[0];
  json["tuesday2"] = alarm2[1];
  json["wednesday2"] = alarm2[2];
  json["thursday2"] = alarm2[3];
  json["friday2"] = alarm2[4];
  json["saturday2"] = alarm2[5];
  json["sunday2"] = alarm2[6];
  json["alarmtime2"] = alarm2[7];
  json["snooze2"] = alarm2[8];

  SPIFFS.begin();         // Init ESP filesystem
  File configFile = SPIFFS.open("/settings.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  /*for(int i = 0; i < 2; i++) {
     Serial.println(network[i]);
     }
     for(int i = 0; i < 2; i++) {
     Serial.println(weather[i]);
     }
     for(int i = 0; i < 12; i++) {
     Serial.println(alarm0[i]);
     }
     for(int i = 0; i < 9; i++) {
     Serial.println(alarm1[i]);
     }
     for(int i = 0; i < 9; i++) {
     Serial.println(alarm2[i]);
     }*/

  json.printTo(configFile);
  configFile.close();
  SPIFFS.end();
  setData();
  return true;
}

void setData(){
  HOME_ADDR = weather[0];
  WORK_ADDR = weather[1];
  CITY = weather[0];
  //AP_SSID = network[0]; //FIXME read-only
  //AP_PASS = network[1];
  char houralarm1[2] = {alarm0[7].charAt(0), alarm0[7].charAt(1)};
  ALARM_HOUR[0] = atoi(houralarm1);
  char houralarm2[2] = {alarm1[7].charAt(0), alarm1[7].charAt(1)};
  ALARM_HOUR[1] = atoi(houralarm2);
  char houralarm3[2] = {alarm2[7].charAt(0), alarm2[7].charAt(1)};
  ALARM_HOUR[2] = atoi(houralarm3);

  char minutealarm1[2] = {alarm0[7].charAt(3), alarm0[7].charAt(4)};
  ALARM_MINUTE[0] = atoi(minutealarm1);
  char minutealarm2[2] = {alarm1[7].charAt(3), alarm1[7].charAt(4)};
  ALARM_MINUTE[1] = atoi(minutealarm2);
  char minutealarm3[2] = {alarm2[7].charAt(3), alarm2[7].charAt(4)};
  ALARM_MINUTE[2] = atoi(minutealarm3);
  /*for(int i = 0; i < 7; i++) { //FIXME Problem with String
     if(strcmp(alarm0[i].c_str(), "yes") == 0)
      alarmDays[0][i] = true;
     if(strcmp(alarm1[i].c_str(), "yes") == 0)
      alarmDays[1][i] = true;
     if(strcmp(alarm2[i].c_str(), "yes") == 0)
      alarmDays[2][i] = true;
     Serial.println("loop");
     }*/
  /*if(strcmp(alarm0[8].c_str(), "yes") == 0)
     SUNRISE_ALARM = true;
     if(strcmp(alarm0[10].c_str(), "yes") == 0)
     TRAFFIC_ALARM = true;
     if(strcmp(alarm0[11].c_str(), "yes") == 0)
     WEATHER_ALARM = true;*/
}
