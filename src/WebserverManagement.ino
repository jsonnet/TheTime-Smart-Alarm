// Use data uploaded to the ESP (file into folder data)

String network[3];
String weather[2];

void serverHomepage() { //
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
  String s = " ";
  SPIFFS.begin();
  //SPIFFS.format(); //TODO comment out, ONLY needs to happen once
  File f = SPIFFS.open("/index.html", "r");
  if(f){
    s = f.readString();
    f.close();
  }
  SPIFFS.end();
  return s;
}

