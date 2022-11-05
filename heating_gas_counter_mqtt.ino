#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "SSID"; // Enter your WiFi name
const char* password =  "WIFI-Password"; // Enter WiFi password
const char* mqttServer = "Hostname/ip of MQTT Server"; // insert the hostname or ip of your mqtt server
const int mqttPort = 1883;
const char* mqttUser = "username"; // username of mqtt server
const char* mqttPassword = "password"; // password of mqtt server
 
WiFiClient espClient;
PubSubClient client(espClient);

int prev = 1;
unsigned long now = 0; 
int state = 1;
 
void setup() {
  // initialize Serial console for debugging
  Serial.begin(115200);
  // connect to wifi
  WiFi.begin(ssid, password);
  // if connection failed retry after 0.5 sec
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  // initialize connection to mqtt server
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    //connect to mqtt server
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
      // failed to connect
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
  // publish initial message 
  client.publish("heizung/status", "start"); //Topic name
  // initialize input
  pinMode(D0,INPUT);
}
// ignore incomming messages
void callback(char* topic, byte* payload, unsigned int length) {
  return;
}
 
void loop() {
  // read input 
  int state = digitalRead(D0);
  Serial.println(state);
  // only detect falling edge and prevent firing on jumping signal
  if(millis()-now >= 1000 && prev == 1 && state == 0){
    now = millis();
    // send signal
    client.publish("heizung/zaehler", "impuls");
  }
  client.loop();
  prev = state;
}
