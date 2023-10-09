#include <ESP8266WiFi.h> 
#include <PubSubClient.h> //download library PubSubClient by Nick O'Leary

extern "C"{
  #include "user_interface.h" 
}

#define WIFI_SSID "XXXXXXXXXXXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXXXXXXXXXXX"
#define MQTT_SERVER "mqtt3.thingspeak.com"
#define MQTT_DEVICE_CLIENT_ID "XXXXXXXXXXXXXXXXXXX"   //insert data from the device registered in thingspeak
#define MQTT_DEVICE_USERNAME "XXXXXXXXXXXXXXXXXXX"      
#define MQTT_DEVICE_PASSWORD "XXXXXXXXXXXXXXXXXXX"    
#define TOPIC "channels/XXXXXXX/publish"              //insert channel id thingspeak
#define PORT 1883

os_timer_t mTimer;
bool _timeout = false;

WiFiClient client;
PubSubClient mqttClient(client);  

void tCallback(void *tCall){
    _timeout = true;
}

void usrInit(void) {
    os_timer_setfn(&mTimer, tCallback, NULL); //Indicates the subroutine to the timer.
    os_timer_arm(&mTimer, 10000, true); //Indicates to the Timer the time in ms and whether it will be repeated or just once (loop = true)
}

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void mqtt_connection() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection ");
    if (mqttClient.connect(MQTT_DEVICE_CLIENT_ID, MQTT_DEVICE_USERNAME,MQTT_DEVICE_PASSWORD)) {
      Serial.println("Connected!");
    } else {
      Serial.print(".");
      delay(1000);
    }
  }
}

long mockSensorValue(){
  //mock a number between 30 and 40
  return rand() % 10 + 30;
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  mqttClient.setServer(MQTT_SERVER, PORT);
  usrInit(); //start interrupt 
}

void loop() {
  if (!mqttClient.connected()){
    mqtt_connection();
  }
 
  if (_timeout) { 
    float sensor1 = mockSensorValue();
    float sensor2 = mockSensorValue();
    int sensor3 = (int)mockSensorValue();

    String postStr = "field1=";
             postStr += String(sensor1);
             postStr +="&field2=";
             postStr += String(sensor2);
             postStr +="&field3=";
             postStr += String(sensor3);
             postStr += "&status=MQTTPUBLISH";
    if (mqttClient.publish(TOPIC, postStr.c_str())){
      Serial.println("Message published in topic " + String(TOPIC));
      Serial.print("Sensor1: ");
      Serial.print(sensor1);
      Serial.print(" Sensor2: ");
      Serial.print(sensor2);
      Serial.print(" Sensor3: ");
      Serial.println(sensor3);
    }  
    _timeout = false;
  }
  mqttClient.loop();
}
