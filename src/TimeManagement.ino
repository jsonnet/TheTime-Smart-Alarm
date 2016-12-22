//*** TIME MANAGEMENT ***

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void setTime() {
  //connectWiFi();
  WiFi.hostByName(ntpServerName, timeServerIP);
  sendNTPpacket(timeServerIP);
  delay(1000);
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("cannot set time");
  }
  else {
    udp.read(packetBuffer, NTP_PACKET_SIZE);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord; // this is NTP time (seconds since Jan 1 1900):
    const unsigned long seventyYears = 2208988800UL; // 70years
    unsigned long epoch = secsSince1900 - seventyYears; // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:

    sprintf(date, "%d", day(epoch));        // dayStr(day(epoch) || dayShortStr(day(epoch) || weekday(epoch);
    strcat(date, ".");
    char helper[2];
    sprintf(helper, "%d", month(epoch));    // monthStr(month(epoch)) || monthShortStr(month(epoch))
    strcat(strcat(date, helper), ".");

    epoch += TIMEZONE * 3600; // Add timezone offset
    HOUR = (epoch  % 86400L) / 3600; // Hours
    MINUTE = (epoch  % 3600) / 60; // Minutes
    //SECONDS = (epoch % 60); // Seconds
    epochTime = epoch;
  }
  //closeWiFi();
}

void _adjustTime(double sec){
  epochTime += sec;
  HOUR = (epochTime  % 86400L) / 3600; // Hours
  MINUTE = (epochTime  % 3600) / 60; // Minutes
  //SECONDS = (epochTime % 60); // Seconds
}
