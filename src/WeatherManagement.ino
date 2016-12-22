
String getWoeid(){
  //String cmd = "/v1/public/yql?q=select%20woeid%20from%20geo.places(1)%20where%20text=%22Saarlouis%2C%20de%22&format=xml\r\n";
  strcat(cmd, CITY.c_str());
  strcat(cmd, "%2C%20de%22&format=xml\r\n");
  Serial.println(cmd);
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);
  int anfang = antwort.indexOf("woeid") + 6;
  int ende = anfang +6;
  //Serial.println(antwort.substring(anfang, ende));

  return antwort.substring(anfang, ende);
}

void getWeatherData() {

  // String cmd = "/v1/public/yql?q=select%20item.condition.text%20from%20weather.forecast%20where%20woeid=690637%20&format=xml\r\n";
  char cmd[200] = "/v1/public/yql?q=select%20item.condition.text%20from%20weather.forecast%20where%20woeid=";
  strcat(cmd, WOEID.c_str());
  strcat(cmd, "%20&format=xml\r\n");

  String host = "query.yahooapis.com";
  String antwort;

  //Starts to fetch data
  Serial.println();
  Serial.print("Retrieving weather-data: ");

  //first pull of answer from the server
  httpGET(host, cmd, antwort);

  /*if answer is not longer than 305 charackters (310 to be sure)
    the important pieces are not in the answertext of the server
    --> new attempt to pull correct answer*/
  while(antwort.length() <=310){
      httpGET(host, cmd, antwort);
      //Indicates the number of attemps
      Serial.print("... ");
  }

  Serial.println("\nDONE");

  String weather = searchXML(antwort, "text");
  WEATHER_STATE = weather;

  //TODO try icons
  // [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear
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
  else
    WEATHER_STATE = "%weather%"; // If getting data fails
}

void getTempData(){
  //String cmd = "/v1/public/yql?q=select%20item.condition.temp%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text='Saarlouis, de')\r\n"; // Why no working?
  //String cmd = "/v1/public/yql?q=select%20item.condition.temp%20from%20weather.forecast%20where%20woeid=690637&format=xml\r\n";
  char cmd[200] = "/v1/public/yql?q=select%20item.condition.temp%20from%20weather.forecast%20where%20woeid=";
  strcat(cmd, WOEID.c_str());
  strcat(cmd, "%20&format=xml\r\n");

  String host = "query.yahooapis.com";
  String antwort;



  //Starts to fetch data
  Serial.println();
  Serial.print("Retrieving temperature-data: ");

  //first pull of answer from the server
  httpGET(host, cmd, antwort);

  /*if answer is not longer than 305 charackters (310 to be sure)
    the important pieces are not in the answertext of the server
    --> new attempt to pull correct answer*/
  while(antwort.length() <=310){
      httpGET(host, cmd, antwort);
      //Indicates the number of attemps
      Serial.print("... ");
  }

  Serial.println("\nDONE");

  double temper = 32;
  temper = strtod(searchXML(antwort, "temp").c_str(), NULL);
  temper = (temper - 32) / 1.8;
  OUT_TEMP = temper;

}

void getSunriseData(){
 // String cmd = "/v1/public/yql?q=select%20astronomy.sunrise%20from%20weather.forecast%20where%20woeid=690637&format=xml\r\n";
  char cmd[200] = "/v1/public/yql?q=select%20astronomy.sunrise%20from%20weather.forecast%20where%20woeid=";
  strcat(cmd, WOEID.c_str());
  strcat(cmd, "%20&format=xml\r\n");

  String host = "query.yahooapis.com";
  String antwort;

  //Starts to fetch data
  Serial.println();
  Serial.print("Retrieving sunset-data: ");

  //first pull of answer from the server
  httpGET(host, cmd, antwort);

  /*if answer is not longer than 305 charackters (310 to be sure)
    the important pieces are not in the answertext of the server
    --> new attempt to pull correct answer*/
  while(antwort.length() <=310){
      httpGET(host, cmd, antwort);
      //Indicates the number of attemps
      Serial.print("... ");
  }

  Serial.println("\nDONE");

  SUNRISE = searchXML(antwort, "sunrise");
}
