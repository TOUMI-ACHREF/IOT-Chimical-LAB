#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Définir les broches pour le capteur de flamme
const int sensorDigitalPin = 25;  // Broche pour la sortie numérique DO du capteur de flamme
const int sensorAnalogPin = 34;   // Broche pour la sortie analogique AO du capteur de flamme
const int analogThreshold = 400;  // Seuil pour la détection de la flamme
const int mq5Pin = 35;  // GPIO 34 sur ESP32 pour le capteur MQ5

const int trigPin = 19;
const int echoPin = 18;

// Définir la broche du buzzer
const int buzzerPin = 15;

// Constantes pour le capteur à ultrasons
#define SOUND_SPEED 0.034  // Vitesse du son en cm/us
#define CM_TO_INCH 0.393701

#define SEALEVELPRESSURE_HPA (1013.25)

// Configuration Wi-Fi
const char* ssid = "Redmi note 10";       // Remplacez par le nom de votre réseau Wi-Fi
const char* password = "hello123hello"; // Remplacez par votre mot de passe Wi-Fi
const char* serverURL = "http://192.168.255.167:3000/api/data"; // URL du serveur (Remplacez <votre-ip> par l'IP de votre serveur)
//192.168.1.17

// Définir le capteur BME280
Adafruit_BME280 bme;  // Utilisation en mode I2C

unsigned long delayTime;
long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Test des capteurs BME280 et de flamme"));

  // Connexion Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connexion au Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnecté au Wi-Fi !");

  // Initialisation du capteur BME280
  bool status = bme.begin(0x76);
  if (!status) {
    Serial.println("Capteur BME280 non trouvé, vérifiez le câblage !");
    while (1);
  }

  // Initialisation des broches pour le capteur de flamme
  pinMode(sensorDigitalPin, INPUT);
  pinMode(sensorAnalogPin, INPUT);

  // Initialisation des broches pour le capteur à ultrasons
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(buzzerPin, OUTPUT);  // Buzzer en sortie

  Serial.println("-- Test par défaut --");
  delayTime = 8000;
}

void loop() {
  // Lire les valeurs des capteurs
  bool alert = false;

  if (checkBME280Sensor()) alert = true;
  if (checkFlameSensor()) alert = true;
  if (checkMQ5Sensor()) alert = true;
  // if (checkUltrasonicSensor()) alert = true;

  // Activer le buzzer si un problème est détecté
  if (alert) {
    tone(buzzerPin, 1000);  // Active le buzzer à 1000 Hz
  } else {
    noTone(buzzerPin);      // Désactive le buzzer
  }

  // Lecture des valeurs du capteur BME280
  printBMEValues();
  Serial.println("--------------------------");
  // Lecture des valeurs du capteur de flamme
  readFlameSensor();
  Serial.println("--------------------------");
  // Lecture des valeurs du capteur MQ5
  readMQ5Sensor();
  Serial.println("--------------------------");
  // Lecture des valeurs du capteur à ultrasons
  readUltrasonicSensor();
  Serial.println("==========================");

  // Collecte des données des capteurs
  String sensorData = collectSensorData();

  // Envoi des données au serveur
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(sensorData);

    if (httpResponseCode > 0) {
      Serial.print("Données envoyées avec succès : ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erreur lors de l'envoi : ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }

  delay(delayTime);
}

String collectSensorData() {
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;
  int flameStatus = digitalRead(sensorDigitalPin);
  int flameAnalog = analogRead(sensorAnalogPin);
  int mq5Value = analogRead(mq5Pin);
  long distance = duration * SOUND_SPEED / 2;

  // Construction des données JSON
  String jsonData = "{";
  jsonData += "\"bme\":{\"temperature\":" + String(temperature) +
              ",\"humidity\":" + String(humidity) +
              ",\"pressure\":" + String(pressure) + "},";
  jsonData += "\"flame\":{\"status\":" + String(flameStatus) +
              ",\"analog\":" + String(flameAnalog) + "},";
  jsonData += "\"mq5\":{\"value\":" + String(mq5Value) + "},";
  jsonData += "\"ultrasonic\":{\"distance\":" + String(distance) + "}";
  jsonData += "}";

  return jsonData;
}

void printBMEValues() {
  Serial.print("Température = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pression = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Humidité = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void readFlameSensor() {
  int flameStatus = digitalRead(sensorDigitalPin);
  int flameAnalogValue = analogRead(sensorAnalogPin);

  Serial.print("Valeur analogique (AO) : ");
  Serial.println(flameAnalogValue);

  if (flameStatus == LOW && flameAnalogValue < analogThreshold) {
    Serial.println("** Flamme détectée !!! **");
  } else {
    Serial.println("Pas de feu détecté");
  }

  Serial.println();
}

void readMQ5Sensor() {
  int sensorValue = analogRead(mq5Pin);
  float voltage = sensorValue * (3.3 / 4095.0);
  Serial.print("Valeur du capteur MQ5 : ");
  Serial.print(sensorValue / 10);  // Divisé par 10 pour réduire l'affichage
  Serial.println();
}

void readUltrasonicSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  distanceInch = distanceCm * CM_TO_INCH;

  Serial.print("Distance (cm) : ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch) : ");
  Serial.println(distanceInch);

  Serial.println();
}

bool checkBME280Sensor() {
  float temp = bme.readTemperature();
  float humidity = bme.readHumidity();
  if (temp > 50 || humidity < 20) {
    Serial.println("Alerte BME280 : Température ou humidité hors limites !");
    return true;
  }
  return false;
}

bool checkFlameSensor() {
  int flameStatus = digitalRead(sensorDigitalPin);
  int flameAnalogValue = analogRead(sensorAnalogPin);
  if (flameStatus == LOW && flameAnalogValue < analogThreshold) {
    Serial.println("Alerte Flamme : Flamme détectée !");
    return true;
  }
  return false;
}

bool checkMQ5Sensor() {
  int sensorValue = analogRead(mq5Pin);
  if (sensorValue > 3000) {
    Serial.println("Alerte MQ5 : Gaz détecté !");
    return true;
  }
  return false;
}

bool checkUltrasonicSensor() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  if (distanceCm < 10) {
    Serial.println("Alerte Ultrason : Obstacle détecté !");
    return true;
  }
  return false;
}
