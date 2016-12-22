void getWoeid(){
  String cmd = "/v1/public/yql?q=select%20woeid%20from%20geo.places(1)%20where%20text=%22Saarlouis%2C%20de%22\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);
  //Woeid direct output of request no real xml format, sort of
  //woeid = antwort;
}

void getWeatherData() {
  String cmd = "/v1/public/yql?q=select%20item.condition.text%20from%20weather.forecast%20where%20woeid=690637%20\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);

  String weather = searchXML(antwort, "text");
  WEATHER_STATE = weather;

  // [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear
  if (!(strstr(weather.c_str(), "Cloudy") || strstr(weather.c_str(), "Rain") || strstr(weather.c_str(), "Breezy") || strstr(weather.c_str(), "Sunny") || strstr(weather.c_str(), "Thunderstorms"))) {
    //IF not able to get weatherdata print this
    WEATHER_STATE = "%weather%";
  }
}

void getTempData(){
  //String cmd = "/v1/public/yql?q=select%20item.condition.temp%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text='Saarlouis, de')\r\n"; // Why no working?
  String cmd = "/v1/public/yql?q=select%20item.condition.temp%20from%20weather.forecast%20where%20woeid=690637\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);

  double temper = 32;
  temper = strtod(searchXML(antwort, "temp").c_str(), NULL);
  temper = (temper - 32) / 1.8;
  //temper = round(temper * 10.0)/10;
  OUT_TEMP = temper;
  Serial.println(OUT_TEMP);
}

void getSunriseData(){
  String cmd = "/v1/public/yql?q=select%20astronomy.sunrise%20from%20weather.forecast%20where%20woeid=690637\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);

  SUNRISE = searchXML(antwort, "sunrise");

}
