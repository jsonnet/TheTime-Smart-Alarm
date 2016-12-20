// Use data uploaded to the ESP (file into folder data & Tools -> ESP Sketch Uploader)

String network[3];
String weather[2];

void serverHomepage() {
  server.arg("ip") = "192.168.178.1";
  if (server.hasArg("ip") && server.hasArg("networkname") && server.hasArg("password")) {
    network[2] = server.arg("ip");
    network[0] = server.arg("networkname");
    network[1] = server.arg("password");
  }
  if (server.hasArg("location") && server.hasArg("worklocation")) {
    weather[0] = server.arg("location");
    weather[1] = server.arg("worklocation");
  }
  //TODO add alarm things

  saveToSettings();
  server.send(200, "text/html", getPage()); //change here to file
}

String getPage(){
  String s = " ";         // A String
  SPIFFS.begin();         // Init ESP filesystem
  //SPIFFS.format();      // ONLY needs to happen once
  File f = SPIFFS.open("/index.html", "r"); // Open file
  if(f) {
    s = f.readString();   // Read file
    f.close();            // Close file, needs to happen!
  }
  SPIFFS.end();           // Important close filesystem
  return s;               // Return whole file
}

void saveToSettings() {
  SPIFFS.begin();         // same as before
  File settingsFile = SPIFFS.open("/settings.txt", "w");
  if(!settingsFile) return;

  settingsFile.println("ssid=" + network[0]); // write to file
  settingsFile.println("psk=" + network[1]);
  settingsFile.println("ip=" + network[2]);

  settingsFile.close();
  SPIFFS.end();
}

void readFromSettings() {
  SPIFFS.begin();
  File settingsFile = SPIFFS.open("/settings.txt", "r");
  if(!settingsFile) return;

  while (settingsFile.available()) { // reads whole file
    String line = settingsFile.readStringUntil('\n'); // Each line
    line = line.substr(line.find("=") + 1); // Gets only the part after '='  //TODO Error
    Serial.println(line); //TODO
  }

  settingsFile.close();
  SPIFFS.end();
}
