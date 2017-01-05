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

void setAlarmBySunrise() {
  ALARM_HOUR[0] = SUNRISE[0] - '0'; //try .toInt() //only think about first pos because sunrise-hour always <10
  ALARM_MINUTE[0] =  (10 * (SUNRISE[3] - '0')) + (SUNRISE[2] - '0');
}

void setAlarmByWeather() {
  // [*] Cloudy, [*] Rain, [*] Breezy, [*] Sunny, [*] Thunderstorms, Clear
  if (strstr(WEATHER_STATE.c_str(), "Rain") || strstr(WEATHER_STATE.c_str(), "Breezy") || strstr(WEATHER_STATE.c_str(), "Thunderstorms")) {
    ALARM_MINUTE[0] += 30;
    if(ALARM_MINUTE[0] >= 60) {
      ALARM_MINUTE[0] -= 60;
      ALARM_HOUR[0] += 1;
    }
  }
}

void setAlarmByTraffic() {
  /* code */
}
