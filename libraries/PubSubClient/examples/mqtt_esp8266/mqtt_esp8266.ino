#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SparkFunBME280.h>
#include <Adafruit_NeoPixel.h>

String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

//-------------- definition mqtt-object
WiFiClient espClient; 
PubSubClient mqttclient(espClient);
//------------ reconnect mqtt-client
void mqttreconnect() { // Loop until we're reconnected 
  if (!mqttclient.connected()) { 
    while (!mqttclient.connected("IoTHackathon") { 
      Serial.print("Attempting MQTT connection...");
      if (mqttclient.connect("ESP8266Client")) {
        Serial.println("connected");
        for (int i=0;i<MAX_MQTT_SUB;i++) { // subscribe topic
          mqttclient.subscribe(mqtt_sub[i].topic);
          Serial.println("\nsubscribe");
          Serial.print(mqtt_sub[i].topic);
        }
      } 
      else { 
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  } 
  else { 
    mqttclient.loop(); 
  }
}

BME280 boschBME280;
//--------- list of mqtt callback functions 
#define MAX_MQTT_SUB 10
typedef void (*mqtthandle) ();
typedef struct {         // Typdeklaration Callback
  String topic;      // mqtt-topic
  String payload;    // mqtt-payload
  mqtthandle fun;    // callback function }subscribe_type; 
  subscribe_type mqtt_sub[MAX_MQTT_SUB]
    mqtt_sub_count=0;

  //--------- mqtt callback function 
  void mqttcallback(char* to, char* pay, unsigned int length) {
    String topic   = String(to);
    String payload = String(pay);
    for (int i=0;i<MAX_MQTT_SUB;i++) {
      if ((topic==mqtt_sub[i].topic) && (payload == mqtt_sub[i].payload)) 
        mqtt_sub[i].fun();
    }
  }

  Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2,13,NEO_GRBW + NEO_KHZ800);

  void NeoAus();
  void NeoAn();

  void setup()
  {
    Serial.begin(115200);
    //----------------------------------MQTT-Client 
    mqttclient.setServer("broker.mqtt-dashboard.com", 1883);
    mqttclient.setCallback(mqttcallback);

    Wire.begin(); // ---- Initialisiere den I2C-Bus 
    if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");

    boschBME280.settings.runMode = 3; 
    boschBME280.settings.tempOverSample  = 4; 
    boschBME280.settings.pressOverSample = 4;
    boschBME280.settings.humidOverSample = 4;
    boschBME280.begin();
    mqttclient.setCallback(mqttcallback);

    //--------- prepare mqtt subscription 
    mqtt_sub_count++; // add new element 
    if (mqtt_sub_count < MAX_MQTT_SUB) { 
      mqtt_sub[mqtt_sub_count-1].topic = "inTopic";
      mqtt_sub[mqtt_sub_count-1].payload = "0";
      mqtt_sub[mqtt_sub_count-1].fun = NeoAus; //callback function
    } 
    else Serial.println(" err max. mqtt subscription");

    //--------- prepare mqtt subscription 
    mqtt_sub_count++; // add new element 
    if (mqtt_sub_count < MAX_MQTT_SUB) { 
      mqtt_sub[mqtt_sub_count-1].topic = "inTopic";
      mqtt_sub[mqtt_sub_count-1].payload = "1";
      mqtt_sub[mqtt_sub_count-1].fun = NeoAn; //callback function
    } 
    else Serial.println(" err max. mqtt subscription");

    pixels.begin();//-------------- Initialisierung Neopixel
    delay(1);
    pixels.show();
    pixels.setPixelColor(0,0,0,0); // alle aus
    pixels.setPixelColor(1,0,0,0);
    pixels.show();                 // und anzeigen

    //------------ WLAN initialisieren 
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    delay(100);
    Serial.print ("\nWLAN connect to:");
    Serial.print("IoTHackathon");
    WiFi.begin("IoTHackathon","ITGipfel2016");
    while (WiFi.status() != WL_CONNECTED) { // Warte bis Verbindung steht 
      delay(500); 
      Serial.print(".");
    };
    Serial.println ("\nconnected, meine IP:"+ WiFi.localIP().toString());
    matrixausgabe_text = " Meine IP:" + WiFi.localIP().toString();
    matrixausgabe_index=0;


  }

  void loop() { // Kontinuierliche Wiederholung 
    mqttreconnect();
    mqttclient.publish("outTopic",String("Temp=")+String(boschBME280.readTempC()));
    Serial.print("\nmqtt publish: "); 
    Serial.print(String("Temp=")+String(boschBME280.readTempC()));
    mqttreconnect();
    mqttreconnect();
  }

  void NeoAn()
  {
    pixels.setPixelColor(true,63&0x3F,0&0x3F,0&0x3F);
    pixels.show();
  }

  void NeoAus()
  {
    pixels.setPixelColor(true,0&0x3F,0&0x3F,0&0x3F);
    pixels.show();
  }


