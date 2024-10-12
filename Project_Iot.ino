
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Patta";         // Replace with your Wi-Fi SSID
const char* password = "0864395473"; // Replace with your Wi-Fi password
String lineToken = "p5aL9cbT0V4N3oHECOmoMOkgBSGU41CwXMX4FurRn4E"; // Replace with your LINE Notify token

int waterSensorPin = 34;  // GPIO for the sensor signal (analog pin)
int vccPin = 4;           // GPIO 4 for VCC
int waterLevel = 0;
int waterLevelThreshold = 1000; 
String waterSensorId = "id01";
const char* host = "192.168.0.133";
void setup() {
  // Set up WiFi
  Serial.begin(115200);
  WiFi.begin(ssid, password);
   if (WiFi.status() != WL_CONNECTED) {
    wifiConnect();
  }
  
  // Wait for the connection to establish
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  pinMode(vccPin, OUTPUT);      // Set D4 as an output pin
  digitalWrite(vccPin, HIGH);   // Provide power to the sensor
}



void loop() {
  waterLevel = analogRead(waterSensorPin);  // Read the sensor value
  Serial.print("Water Level: ");
  Serial.println(waterLevel);  // Print the value to Serial Monitor
  sendWaterDataToServer();
  // If the water level is below a certain threshold (no water)
  if (waterLevel < 700) {
    sendLineMessage("No water detected by sensor! I need water!!!");
  }

    delay(180000);   // Wait 3 minutes between readings
}



// Function to send message through LINE Notify
void sendLineMessage(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://notify-api.line.me/api/notify"); // LINE Notify API URL
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


void sendWaterDataToServer() {
  Serial.print("connecting to ");
  Serial.println(host);
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/watersensordataupdate.php";
  url += "?sensorID=";
  url += waterSensorId;
  url += "&waterLevel=";
  url += waterLevel;

  Serial.print("Requesting URL: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  digitalWrite(5, LOW);
  delay(50);
  digitalWrite(5, HIGH);
  delay(50);
}



void wifiConnect() {
  //--------- WiFi----------------------------
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(5, HIGH);
    delay(100);
    digitalWrite(5, LOW);
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("================================");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Sensor ID: ");
  Serial.println(waterSensorId);
  Serial.println("================================");
  digitalWrite(5, HIGH);
  delay(250);
  //------------------------------------------
}