#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>   // Web server library

#define RELAY_PIN 23       // Pump relay pin connected to GPIO 23
#define SOIL_SENSOR_PIN 35 // Soil moisture sensor digital output connected to GPIO 35
#define WATER_SENSOR_PIN 34 // GPIO for the water sensor signal (analog pin)
#define VCC_PIN 4          // GPIO 4 for VCC to the sensor

const char* ssid = "Patt";         // Replace with your Wi-Fi SSID
const char* password = "12345678"; // Replace with your Wi-Fi password
String lineToken = "p5aL9cbT0V4N3oHECOmoMOkgBSGU41CwXMX4FurRn4E"; // Replace with your LINE Notify token

String waterSensorId = "id01";
int waterLevel = 0; // Water level value from sensor

const char* host = "192.168.136.71"; // Server IP address

WebServer server(80); // Create a web server object on port 80

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);        // Set the relay pin as output
  pinMode(SOIL_SENSOR_PIN, INPUT);   // Set the soil moisture sensor pin as input
  pinMode(WATER_SENSOR_PIN, INPUT);  // Set the water sensor pin as input
  pinMode(VCC_PIN, OUTPUT);          // Set VCC pin to power sensor
  digitalWrite(VCC_PIN, HIGH);       // Provide power to the sensor

  wifiConnect(); // Connect to Wi-Fi

// Define web server route to activate the pump
server.on("/activatePump", []() {
  digitalWrite(RELAY_PIN, HIGH);      // Activate the pump
  server.sendHeader("Access-Control-Allow-Origin", "*"); // Allow all origins
  server.send(200, "text/plain", "Pump activated for 5 seconds");
  delay(5000);                        // Keep pump on for 5 seconds
  digitalWrite(RELAY_PIN, LOW);       // Deactivate the pump
});


  server.begin();  // Start the web server
}

void loop() {
  delay(500);  // Small delay to stabilize sensors

  // Read the water and soil moisture sensor values
  waterLevel = analogRead(WATER_SENSOR_PIN);  
  int soilMoistureValue = digitalRead(SOIL_SENSOR_PIN);

  Serial.print("Water Level: ");
  Serial.println(waterLevel);  // Display water level
  Serial.print("Soil Moisture Value: ");
  Serial.println(soilMoistureValue); // 0 = Dry, 1 = Wet

  // Control the pump based on the soil moisture
  if (soilMoistureValue == LOW) {  // If soil is dry (LOW)
    digitalWrite(RELAY_PIN, HIGH); // Activate pump
    Serial.println("Pump ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Deactivate pump
    Serial.println("Pump OFF");
  }

  // Send data to the server
  sendWaterDataToServer();

  // Send LINE notification if no water detected
  if (waterLevel < 700) {
    sendLineMessage("No water detected by sensor! I need water!!!");
  }

  server.handleClient();  // Handle incoming client requests
  delay(1000);  // Check again every 1 second
}

// Function to send a message using LINE Notify
void sendLineMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://notify-api.line.me/api/notify");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Bearer " + lineToken);

    String postData = "message=" + message;
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

// Function to send water data to the server
void sendWaterDataToServer() {
  Serial.print("Connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }

  // Construct the request URL with sensor data
  String url = "/watersensordataupdate.php?sensorID=" + waterSensorId + "&waterLevel=" + String(waterLevel);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // Send HTTP GET request
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  // Wait for server response
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout!");
      client.stop();
      return;
    }
  }
}

// Function to connect/reconnect to Wi-Fi
void wifiConnect() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  // Wait for the connection to establish
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println("================================");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Sensor ID: ");
  Serial.println(waterSensorId);
  Serial.println("================================");
}
