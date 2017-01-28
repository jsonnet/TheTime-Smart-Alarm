// Use data uploaded to the ESP (file into folder data & Tools -> ESP Sketch Uploader)

String network[2];
String weather[2];
String alarm0[12];
String alarm1[9];
String alarm2[9];

void serverHomepage() {
  if (server.hasArg("ip") && server.hasArg("networkname") && server.hasArg("password")) {
    network[0] = server.arg("networkname");
    network[1] = server.arg("password");
  }
  if (server.hasArg("locationhome") && server.hasArg("locationwork")) {
    weather[0] = server.arg("locationhome");
    weather[1] = server.arg("locationwork");
  }
  if (server.hasArg("monday0") && server.hasArg("tuesday0") && server.hasArg("wednesday0")
      && server.hasArg("thursday0") && server.hasArg("friday0") && server.hasArg("saturday0")
      && server.hasArg("sunday0") && server.hasArg("alarmtime0") && server.hasArg("sunset0")
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
  if (server.hasArg("monday1") && server.hasArg("tuesday1") && server.hasArg("wednesday1")
      && server.hasArg("thursday1") && server.hasArg("friday1") && server.hasArg("saturday1")
      && server.hasArg("sunday1") && server.hasArg("alarmtime1") && server.hasArg("snooze1")) {
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
  if (server.hasArg("monday2") && server.hasArg("tuesday2") && server.hasArg("wednesday2")
      && server.hasArg("thursday2") && server.hasArg("friday2") && server.hasArg("saturday2")
      && server.hasArg("sunday2") && server.hasArg("alarmtime2") && server.hasArg("snooze2")) {
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
  //saveToSettings(); // Save to Settings after getting data
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

void saveToSettings() {
  SPIFFS.begin();         // same as before
  File settingsFile = SPIFFS.open("/settings.txt", "w+");
  if(!settingsFile) return;

  settingsFile.println("ssid=" + network[0]); // write to file
  settingsFile.println("psk=" + network[1]);

  settingsFile.println("location=" + weather[0]);
  settingsFile.println("worklocation=" + weather[1]);

  settingsFile.close();
  SPIFFS.end();
}

void readFromSettings() {
  SPIFFS.begin();
  File settingsFile = SPIFFS.open("/settings.txt", "r");
  if(!settingsFile) return;

  while (settingsFile.available()) { // reads whole file
    String line = settingsFile.readStringUntil('\n'); // Each line
    //line = strtok(line.c_str(), new char[] {"="}); // Split line at '=' //XXX help why no working with "=" ?
    Serial.println(line); //TODO store somewhere
  }

  settingsFile.close();
  SPIFFS.end();
}
