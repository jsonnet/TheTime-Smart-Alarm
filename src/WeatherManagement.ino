String getWoeid(){
  char cmd[200] = "/v1/public/yql?q=select%20woeid%20from%20geo.places(1)%20where%20text=%22";
  strcat(cmd, CITY.c_str());
  strcat(cmd, "%2C%20de%22&format=xml\r\n");
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort, false);
  int anfang = antwort.indexOf("woeid") + 6;
  int ende = anfang +6;

  return antwort.substring(anfang, ende);
}

void getWeatherData() {
  char cmd[200] = "/v1/public/yql?q=select%20item.condition.text%20from%20weather.forecast%20where%20woeid=";
  strcat(cmd, WOEID.c_str());
  strcat(cmd, "%20&format=xml\r\n");

  String host = "query.yahooapis.com";
  String antwort;

  Serial.print("\nRetrieving weather-data: ");

  httpGET(host, cmd, antwort, false); //first pull of answer from the server

  int attempts = 0;
  /*if answer is not longer than 305 charackters (310 to be sure)
     the important pieces are not in the answertext of the server
     --> new attempt to pull correct answer*/
  while(antwort.length() <=310) {
    if(attempts >= 10) return;
    httpGET(host, cmd, antwort, false);
    Serial.print("... "); //Indicates the number of attemps
    attempts++;
  }

  Serial.println("DONE");

  String weather = searchXML(antwort, "text");
  WEATHER_STATE = weather;

  //TODO try icons
  // [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear, [*] Snow
  if (strstr(weather.c_str(), "Cloudy") )
    WEATHER_STATE = "Cloudy";
  else if (strstr(weather.c_str(), "Rain"))
    WEATHER_STATE = "Rain";
  else if (strstr(weather.c_str(), "Breezy"))
    WEATHER_STATE = "Breezy";
  else if (strstr(weather.c_str(), "Sunny"))
    WEATHER_STATE = "Sunny";
  else if (strstr(weather.c_str(), "Thunderstorms"))
    WEATHER_STATE = "Thunder";
  else if (strstr(weather.c_str(), "Clear"))
    WEATHER_STATE = "Clear";
  else {
    Serial.println(WEATHER_STATE);
    WEATHER_STATE = weather; // If getting data fails
  }
}

void getTempData(){
  char cmd[200] = "/v1/public/yql?q=select%20item.condition.temp%20from%20weather.forecast%20where%20woeid=";
  strcat(cmd, WOEID.c_str());
  strcat(cmd, "%20&format=xml\r\n");

  String host = "query.yahooapis.com";
  String antwort;

  Serial.print("\nRetrieving temperature-data: ");

  httpGET(host, cmd, antwort, false); //first pull of answer from the server

  int attempts = 0;
  /*if answer is not longer than 305 charackters (310 to be sure)
     the important pieces are not in the answertext of the server
     --> new attempt to pull correct answer*/
  while(antwort.length() <=310) {
    if(attempts >= 10) return;
    httpGET(host, cmd, antwort, false);
    Serial.print("... "); //Indicates the number of attemps
    attempts++;
  }

  Serial.println("DONE");

  // convert fahrenheit to celsius
  double temper = 32;
  temper = strtod(searchXML(antwort, "temp").c_str(), NULL);
  temper = (temper - 32) / 1.8;
  OUT_TEMP = temper;
}

void getSunriseData(){
  char cmd[200] = "/v1/public/yql?q=select%20astronomy.sunrise%20from%20weather.forecast%20where%20woeid=";
  strcat(cmd, WOEID.c_str());
  strcat(cmd, "%20&format=xml\r\n");

  String host = "query.yahooapis.com";
  String antwort;

  Serial.print("\nRetrieving sunset-data: ");

  httpGET(host, cmd, antwort, false); //first pull of answer from the server

  int attempts = 0;
  /*if answer is not longer than 305 charackters (310 to be sure)
     the important pieces are not in the answertext of the server
     --> new attempt to pull correct answer*/
  while(antwort.length() <=310) {
    if(attempts >= 10) return;
    httpGET(host, cmd, antwort, false);
    Serial.print("... "); //Indicates the number of attemps
    attempts++;
  }

  Serial.println("DONE");

  SUNRISE = searchXML(antwort, "sunrise");
}

String searchXML(String xml, String suchtext) {
  String valStr;
  int start, ende;
  suchtext = suchtext + '=' + '"';
  start = xml.indexOf(suchtext);
  delay(10);
  if (start > 0) {
    start = start + suchtext.length();
    ende =  xml.indexOf('"', start);
    valStr = xml.substring(start, ende);
  } else
    Serial.print("error - no such item: " + suchtext);
  return valStr;
}
