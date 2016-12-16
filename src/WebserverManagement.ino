// Use data uploaded to the ESP (file into folder data & Tools -> ESP Sketch Uploader)

String network[3];
String weather[2];

void serverHomepage() {
  server.arg("ip") = "192.168.178.1";
  if (server.hasArg("ip") && server.hasArg("networkname") && server.hasArg("password")) {
    network[0] = server.arg("ip");
    network[1] = server.arg("networkname");
    network[2] = server.arg("password");
  }
  if (server.hasArg("location") && server.hasArg("worklocation")) {
    weather[0] = server.arg("location");
    weather[1] = server.arg("worklocation");
  }
  //TODO add alarm things
  server.send(200, "text/html", getPage()); //change here to file
}

String getPage(){
  String s = " ";         // A String
  SPIFFS.begin();         // Init EPROM filesystem
  //SPIFFS.format();      // ONLY needs to happen once
  File f = SPIFFS.open("/index.html", "r"); // Open file
  if(f){
    s = f.readString();   // Read file
    f.close();            // Close file, needs to happen!
  }
  SPIFFS.end();           // Important close filesystem
  return s;               // Return whole file
}

