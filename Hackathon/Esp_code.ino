#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHT_PIN 15          // DHT sensor connected to GPIO 15
#define DHTTYPE DHT11
#define ULTRASONIC_TRIG 12   // Ultrasonic Trigger pin for water level measurement
#define ULTRASONIC_ECHO 13   // Ultrasonic Echo pin for water level measurement
#define SOIL_SENSOR_PIN 34    // Soil moisture sensor connected to analog pin A0
#define Sensor_Power 4
#define BUZZER_PIN 18  

const char* ssid = "Redmi Note 10 Pro";
const char* password = "9814230669";
const char* serverUrl = "http://192.168.169.147:5000/post-sensor-data";  // Python server IP and endpoint

DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  // Initialize sensors and buzzer
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(Sensor_Power, OUTPUT);


  dht.begin();
+
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

float readUltrasonicDistance() {
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);

  long duration = pulseIn(ULTRASONIC_ECHO, HIGH);
  return duration * 0.034 / 2;  // Convert duration to cm
}

int readSoilMoisture() {
   digitalWrite(Sensor_Power,HIGH);
   delay(20);
   int value= analogRead(SOIL_SENSOR_PIN);
   // Read soil moisture sensor value (0-1023)
      digitalWrite(Sensor_Power,LOW);
      return value;
}

void triggerBuzzer() {
  tone(BUZZER_PIN, 1000);  // Emit a 1 kHz tone
  delay(500);
  noTone(BUZZER_PIN);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Read sensor data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float distance = readUltrasonicDistance();
    int soilMoisture = readSoilMoisture();

    // Print sensor data to serial monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Water Level (Distance): ");
    Serial.print(distance);
    Serial.println(" cm");

    Serial.print("Soil Moisture: ");
    Serial.println(soilMoisture);

    // Trigger the buzzer based on conditions
    if (distance < 3 ) {  // Adjust these thresholds based on your requirements
      Serial.println("ALERT! Condition met. Triggering buzzer...");
      triggerBuzzer();
    }

    // Send data to the server
    if (!isnan(temperature) && !isnan(humidity)) {
      HTTPClient http;

      // Prepare the JSON payload
      String jsonData = "{\"temperature\":" + String(temperature) +
                        ",\"humidity\":" + String(humidity) +
                        ",\"waterLevel\":" + String(distance) +
                        ",\"soilMoisture\":" + String(soilMoisture) + "}";

      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      // Send POST request
      int httpResponseCode = http.POST(jsonData);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Server response: " + response);
      } else {
        Serial.println("Error sending data: " + String(httpResponseCode));
      }

      http.end();
    }

    delay(5000);  // Send data every 5 seconds
  } else {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.begin(ssid, password);
    delay(1000);
  }
}
