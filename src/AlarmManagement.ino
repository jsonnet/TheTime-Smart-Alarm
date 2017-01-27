void alarm() {
  do {
    tone(15, 300, 500);
    changeRightPixel(0, 0, 0);
    changeLeftPixel(255, 0, 0);
    delay(500);
    tone(15, 500, 500);
    changeLeftPixel(0, 0, 0);
    changeRightPixel(0, 255, 0);
    delay(500);
  } while(!(digitalRead(2) == LOW));

  noTone(15);
  changeLeftPixel(0, 0, 0);
  changeRightPixel(0, 0, 0);
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

void setAlarmBySunrise() {
  if(strcmp(SUNRISE.c_str(), "")) // If SUNRISE data is empty eg. could not get at startup
    return;
  ALARM_HOUR[0] = SUNRISE[0] - '0'; //try .toInt() //only think about first pos because sunrise-hour always prior 10
  ALARM_MINUTE[0] =  (10 * (SUNRISE[3] - '0')) + (SUNRISE[2] - '0');
  // Now add 30 Minutes to sunrise for better wakeup
  ALARM_MINUTE[0] += 30;
  if(ALARM_MINUTE[0] > 60) {
    ALARM_MINUTE[0] -= 60;
    ALARM_HOUR[0] += 1;
  }
}

void setAlarmByWeather() {
  // [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear, [*] Snow
  if (strstr(WEATHER_STATE.c_str(), "Rain") || strstr(WEATHER_STATE.c_str(), "Snow") || strstr(WEATHER_STATE.c_str(), "Thunderstorms") || OUT_TEMP < -1) {
    ALARM_MINUTE[0] -= 30;
    if(ALARM_MINUTE[0] < 0) {
      ALARM_MINUTE[0] += 30;
      ALARM_HOUR[0] -= 1;
    }
  }
}

void setAlarmByTraffic() {
  String host = "maps.googleapis.com";
  String cmd = "maps/api/distancematrix/xml?origins=" + HOME_ADDR + "&destinations=" + WORK_ADDR + "&key=AIzaSyCa-lnY9bJN5gGNKnhWaHCE9SnI82X0QgQ";
  String antwort;
  // response ["row"][0]["element"][0]["duration_in_traffic"]["value"] -> in sec -> round(value/60)
  httpGET(host, cmd, antwort, true);

  int duration = searchXML(antwort, "value").toInt(); //XXX doesnt actually work

  ADD_TRAVEL_TIME = round(duration/60);
  ALARM_MINUTE[0] -= ADD_TRAVEL_TIME;
  if(ALARM_MINUTE[0] < 0) {
    ALARM_MINUTE[0] += ADD_TRAVEL_TIME;
    ALARM_HOUR[0] -= 1;
  }
}
