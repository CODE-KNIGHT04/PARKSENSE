
#include "credentials.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
// -----------------------------------------------------
// Clients
// -----------------------------------------------------
WiFiClient espClient;
PubSubClient mqttClient(espClient);
// -----------------------------------------------------
// LCD Setup
// -----------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
// -----------------------------------------------------
// LED Pins
// -----------------------------------------------------
const int RED_LED = 12;
const int GREEN_LED = 13;
const int YELLOW_LED = 6;
// -----------------------------------------------------
// Ultrasonic Sensors
// -----------------------------------------------------
const int ENTRY_TRIG = 5;
const int ENTRY_ECHO = 4;
const int EXIT_TRIG = 18;
const int EXIT_ECHO = 19;
// -----------------------------------------------------
// Stepper Motors
// -----------------------------------------------------
const int STEPS_PER_REV = 2048;
Stepper exitMotor(STEPS_PER_REV, 8, 10, 11, 2);
Stepper entryMotor(STEPS_PER_REV, 20, 21, 22, 23);
// -----------------------------------------------------
// Parking Variables
// -----------------------------------------------------
const int MIN_DISTANCE = 8;
const int MAX_DISTANCE = 20;
int totalSlots = 10;
int entryCount = 0;
int exitCount = 0;
bool entryGateOpen = false;
bool exitGateOpen = false;
unsigned long entryOpenTime = 0;
unsigned long exitOpenTime = 0;
const unsigned long gateDuration = 5000;
// -----------------------------------------------------
// Ultrasonic Distance Function
// -----------------------------------------------------
float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  float distance = duration * 0.034 / 2;
  return distance;
}
// -----------------------------------------------------
// MQTT Callback
// -----------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {message += (char)payload[i];}
  Serial.print("MQTT Message: ");
  Serial.println(message);
  if (message == "LED ON") {
    digitalWrite(GREEN_LED, HIGH);
  }
  else if (message == "LED OFF") {
    digitalWrite(GREEN_LED, LOW);
  }
}
// -----------------------------------------------------
// MQTT Reconnect
// -----------------------------------------------------
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting MQTT...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Connected");
      mqttClient.subscribe("LED");
      Serial.println("Subscribed to LED topic");
    }
    else {
      Serial.print("MQTT Failed, rc=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}
// -----------------------------------------------------
// SEND DATA TO MQTT
// -----------------------------------------------------

void updateParkingData() {
  int parkingStatus;
  if (totalSlots == 0) {
    parkingStatus = 0;
  }
  else {
    parkingStatus = 1;
  // ======================================
  // CREATE JSON PAYLOAD
  // ======================================
  String payload = "{";
  payload += "\"free_slots\":";
  payload += totalSlots;
  payload += ",";
  payload += "\"entry_count\":";
  payload += entryCount;
  payload += ",";
  payload += "\"exit_count\":";
  payload += exitCount;
  payload += ",";
  payload += "\"parking_status\":";
  payload += parkingStatus;
  payload += "}";
  // ======================================
  // MQTT PUBLISH
  // ======================================
  mqttClient.publish("esp32/data",payload.c_str());
  Serial.println("MQTT Data Sent:");
  Serial.println(payload);
}
}
// -----------------------------------------------------
// Setup
// -----------------------------------------------------
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(callback);
  Wire.begin(16, 17);
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(ENTRY_TRIG, OUTPUT);
  pinMode(ENTRY_ECHO, INPUT);
  pinMode(EXIT_TRIG, OUTPUT);
  pinMode(EXIT_ECHO, INPUT);
  entryMotor.setSpeed(10);
  exitMotor.setSpeed(10);
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
  lcd.setCursor(0, 1);
  lcd.print("10 Slots Free");
  delay(2000);
  lcd.clear();
  updateParkingData();
}
// -----------------------------------------------------
// Main Loop
// -----------------------------------------------------
void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
  float entryDistance =getDistance(ENTRY_TRIG, ENTRY_ECHO);
  float exitDistance =getDistance(EXIT_TRIG, EXIT_ECHO);
  // Parking Available LED
  if (totalSlots > 0) {
    digitalWrite(YELLOW_LED, HIGH);
  }
  else {
    digitalWrite(YELLOW_LED, LOW);
  }
  // Entry Detection
  if (!entryGateOpen && entryDistance >= MIN_DISTANCE && entryDistance <= MAX_DISTANCE) {
    if (totalSlots <= 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PARKING FULL");
      lcd.setCursor(0, 1);
      lcd.print("NO ENTRY");
      return;
    }
    totalSlots--;
    entryCount++;
    entryGateOpen = true;
    entryOpenTime = millis();
    digitalWrite(GREEN_LED, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ENTRY OPEN");
    lcd.setCursor(0, 1);
    lcd.print("Slots:");
    lcd.print(totalSlots);
    entryMotor.step(128);
    updateParkingData();
  }
  // Exit Detection
  if (!exitGateOpen && exitDistance >= MIN_DISTANCE && exitDistance <= MAX_DISTANCE) {
    if (totalSlots < 10) {
      totalSlots++;
      exitCount++;
    }
    exitGateOpen = true;
    exitOpenTime = millis();
    digitalWrite(RED_LED, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EXIT OPEN");
    lcd.setCursor(0, 1);
    lcd.print("Slots:");
    lcd.print(totalSlots);
    exitMotor.step(128);
    updateParkingData();
  }
  // Close Entry Gate
  if (entryGateOpen && millis() - entryOpenTime >= gateDuration) {
    entryMotor.step(-128);
    digitalWrite(GREEN_LED, LOW);
    entryGateOpen = false;
  }
  // Close Exit Gate
  if (exitGateOpen &&millis() - exitOpenTime >= gateDuration) {
    exitMotor.step(-128);
    digitalWrite(RED_LED, LOW);
    exitGateOpen = false;
  }
  // LCD Idle Screen
  if (!entryGateOpen && !exitGateOpen) {
    lcd.setCursor(0, 0);
    lcd.print("Free Slots:");
    lcd.print(totalSlots);
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print("Waiting...   ");
  }
  yield();
  delay(100);
}
