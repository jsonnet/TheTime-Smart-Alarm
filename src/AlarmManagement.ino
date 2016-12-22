void alarm() {
  Serial.println("Alarm");
  do {
    tone(15, 300, 500);
    delay(500);
    tone(15, 500, 500);
    delay(500);
  } while(!(digitalRead(2) == LOW));

  noTone(15);

  while ((digitalRead(2) == LOW)) {
    changeLeftPixel(40, 0, 0);
    delay(2000);
    changeLeftPixel(0, 10, 0);
  }

  setTime();
}

void playMelody(){
  int tempo = 113;
  int beats[] = {1,1,1,1,1,1,4,4,2,1,1,1,1,1,1,4,4,2};
  char notes[] = "cdfda ag cdfdg gf ";

  for (int i = 0; i < sizeof(beats); i++) { // step through the song arrays
    int duration = beats[i] * tempo;

    if (notes[i] == ' ') {
      delay(duration);
    } else {
      tone(15, frequency(notes[i]), duration);
      delay(duration); // wait for tone to finish
    }
    delay(tempo/10); // brief pause between notes
  }
}

int frequency(char note) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  for (int i = 0; i < sizeof(names); i++) {
    if (names[i] == note) {
      return(frequencies[i]);
    }
  }
  return(0);
}


void setAlarmBySunrise() { //TODO Add to all 3 alarms
  ALARM_HOUR[0] = SUNRISE - '0'; //try .toInt() //only think about first pos because sunrise-hour always <10
  ALARM_MINUTE[0] =  (10 * (SUNRISE - '0')) + (SUNRISE - '0');
}


void setAlarmByWeather() { //TODO Add to all 3 alarms
  //TODO deprecated needs complete rewrite better use of if statement (switch don't work)

  // [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear
  if (strstr(WEATHER_STATE.c_str(), "Cloudy") || strstr(WEATHER_STATE.c_str(), "Rain") || strstr(WEATHER_STATE.c_str(), "Breezy") || strstr(WEATHER_STATE.c_str(), "Sunny") || strstr(WEATHER_STATE.c_str(), "Thunderstorms")) {
    int gesammtminuten = (ALARM_HOUR[0] * 60 + ALARM_MINUTE[0]) - 30; //XXX why?
    ALARM_MINUTE[0] = gesammtminuten % 60;
  }
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
