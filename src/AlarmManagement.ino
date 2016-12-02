void alarm() {
  Serial.println("Alarm");
  tone(15, 1000);

  while (!(digitalRead(2) == LOW)) {
    Serial.println("Alarm");
    changeLeftPixel(40, 0, 0);
    delay(1000);
    changeLeftPixel(0, 10, 0);
  }
  noTone(15);
  setTime();
}

void setAlarmBySunrise() {
  String cmd = "/v1/public/yql?q=select%20astronomy.sunrise%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text='Saarlouis, de')\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);
  
  String s = searchXML(antwort, "sunrise");

  ALARM_HOUR = s[0] - '0'; //try .toInt() //only think about first pos because sunrise-hour always <10
  ALARM_MINUTE =  (10 * (s[2] - '0')) + (s[3] - '0');
}

void setAlarmByWeather() {
  String cmd = "/v1/public/yql?q=select%20item.condition.text%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text='Saarlouis, de')\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);
  
  String weather = searchXML(antwort, "text");
  WEATHER_STATE = " " + weather;
  Serial.println(WEATHER_STATE);

  // [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms,
  //if (strstr(weather.c_str(), "Cloudy") || strstr(weather.c_str(), "Rain") || strstr(weather.c_str(), "Breezy") || strstr(weather.c_str(), "Sunny") || strstr(weather.c_str(), "Thunderstorms")) {
  //  int gesammtminuten = (ALARM_HOUR * 60 + ALARM_MINUTE) - 30;
  //  ALARM_MINUTE = gesammtminuten % 60;
  //  Serial.println(ALARM_MINUTE);
  //}
}

String searchXML(String xml, String suchtext) {
  float val = 0. / 0.;
  String valStr;
  int start, ende;
  suchtext = suchtext + '=' + '"';
  start = xml.indexOf(suchtext);
  delay(10);
  if (start > 0) {
    start = start + suchtext.length();
    ende =  xml.indexOf('"', start);
    valStr = xml.substring(start, ende);
    //val = valStr;
  } else
    Serial.print("error - no such item: " + suchtext);
  return valStr;
}
