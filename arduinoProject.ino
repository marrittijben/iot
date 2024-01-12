#include <WiFiS3.h>
#include <ArduinoMqttClient.h>

char ssid[] = "IoTatelierF2144";  // WiFi-naam (SSID)
char pass[] = "IoTatelier";      // WiFi-wachtwoord
WiFiClient wifiClient;           // WiFi-clientobject
MqttClient mqttClient(wifiClient);  // MQTT-clientobject met WiFi-client als parameter
const char broker[] = "192.168.144.1";  // IP-adres van de MQTT-broker
const char topic[] = "PepijnRietdijk/LDR";  // MQTT-topic om te abonneren
const int port = 1883;  // MQTT-brokerpoort
const char publishTopic[] = "marrittijben/LDR";  // MQTT-topic om te publiceren

const int trigPin = 9;  // Pin voor het triggeren van de ultrasone sensor
const int echoPin = 10;  // Pin voor het ontvangen van ultrasone signalen
const int led = 7;  // Pin voor de LED
const int resistor = A0;  // Pin voor de lichtafhankelijke weerstand (LDR)

float duration;
float distance;

int value;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(resistor, INPUT);
  Serial.begin(9600);  // Start seriële communicatie

  // Maak verbinding met het WiFi-netwerk
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(5000);  // Wacht 5 seconden voordat opnieuw te proberen
  }
  
  bool MQTTconnected = false;
  // Maak verbinding met de MQTT-broker
  while (!MQTTconnected) {
    if (!mqttClient.connect(broker, port))  // Probeer verbinding te maken
      delay(1);  // Wacht een milliseconde voordat opnieuw te proberen
    else
      MQTTconnected = true;  // Als verbinding is gelukt, stel MQTTconnected in op true
  }
  
  mqttClient.onMessage(onMqttMessage);  // Wijs de callbackfunctie toe voor inkomende MQTT-berichten
  mqttClient.subscribe(topic);  // Abonneer op het opgegeven MQTT-topic
  Serial.println(topic);  // Laat het geabonneerde topic zien op de seriële monitor
}

void loop() {
  value = analogRead(resistor);  // Lees de waarde van de lichtafhankelijke weerstand (LDR)
  digitalWrite(trigPin, HIGH);  // Start de ultrasone meting
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);  // Meet de duur van het ultrasone signaal
  distance = (duration * 0.034) / 2;  // Bereken de afstand op basis van de duur

  Serial.print("Afstand: ");
  Serial.println(distance);  // Toon de gemeten afstand op de seriële monitor
  Serial.print("Lichtinval: ");
  Serial.println(value);  // Toon de gemeten waarde van de LDR op de seriële monitor

  delay(1000);  // Wacht 1 seconde voordat opnieuw te meten en te publiceren via MQTT

  mqttClient.beginMessage("marrittijben/LDR");  // Begin een bericht voor het MQTT-topic
  mqttClient.print(value);  // Voeg de waarde van de LDR toe aan het bericht
  mqttClient.endMessage();  // Sluit het bericht af

  mqttClient.poll();  // Poll voor inkomende MQTT-berichten

  // Controleer of de afstand minder is dan 20 en de gemeten lichtwaarde minder dan 700
  if (distance < 20 && value < 700) {
    digitalWrite(led, HIGH);  // Schakel de LED in
    // delay(10000);  // Optioneel: wacht 10 seconden voordat iets anders te doen
  } else {
    digitalWrite(led, LOW);  // Schakel de LED uit
  }
}

// Callbackfunctie voor inkomende MQTT-berichten
void onMqttMessage(int messageSize) {
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());  // Lees en toon het ontvangen bericht op de seriële monitor
  }
  Serial.println();
  Serial.println();
}
