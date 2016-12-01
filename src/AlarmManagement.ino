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
  String cmd = "/v1/public/yql?q=select%20astronomy.sunrise%20from%20weather.forecast%20where%20woeid=690637\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);
  
  String s = searchXML(antwort, "sunrise");

  ALARM_HOUR = s[0] - '0'; //try .toInt()
  ALARM_MINUTE =  (10 * (s[2] - '0')) + (s[3] - '0');
}

void setAlarmByWeather() {
  String cmd = "/v1/public/yql?q=select%20item.WEATHER_STATE.text%20from%20weather.forecast%20where%20woeid=690637\r\n";
  String host = "query.yahooapis.com";
  String antwort;

  httpGET(host, cmd, antwort);
  
  String weather = searchXML(antwort, "text");
  WEATHER_STATE = " " + weather;
  Serial.println(WEATHER_STATE);

}

String searchXML(String xml, String suchtext) {
  float val = 0. / 0.;               // Rückgabewert NaN
  String valStr;                     // Hilfsstring
  int start, ende;                   // Index im Text
  suchtext = suchtext + '=' + '"';   // Suchtext
  start = xml.indexOf(suchtext);     // Suche Text
  delay(10);
  if (start > 0) {                   // Item gefunden
    start = start + suchtext.length(); // hinter Item kommt Zahl
    ende =  xml.indexOf('"', start); // Ende der Zahl
    valStr = xml.substring(start, ende); // Zahltext
    //val = valStr;         // Text in Float
  } //else                             // Item nicht gefunden
   // Serial.print("error - no such item: " + suchtext);
  return valStr;
}
