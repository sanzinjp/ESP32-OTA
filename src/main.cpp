#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WebServer.h>
#include <WiFi.h>

#define DHTPIN 21
#define DHTTYPE DHT22
#define LEDPIN 2

const char *ssid = "TP-Link_60_502";
const char *password = "isgm1234";

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);

float temperature = 0;
float humidity = 0;
String ledState = "OFF";

// Forward declarations
void connectToWifi();
void beginServer();
void handleRoot();
void handleSubmit();
String getPage();

void setup() {
  pinMode(LEDPIN, OUTPUT);
  dht.begin();
  Serial.begin(115200);

  connectToWifi();
  beginServer();
}

void loop() {
  server.handleClient();

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  delay(1000);
}

void connectToWifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void beginServer() {
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  if (server.hasArg("LED")) {
    handleSubmit();
  } else {
    server.send(200, "text/html", getPage());
  }
}

void handleSubmit() {
  String LEDValue = server.arg("LED");

  if (LEDValue == "1") {
    digitalWrite(LEDPIN, HIGH);
    ledState = "On";
  } else if (LEDValue == "0") {
    digitalWrite(LEDPIN, LOW);
    ledState = "Off";
  } else {
    Serial.println("Error: Invalid LED value");
  }

  server.send(200, "text/html", getPage());
}

String getPage() {
  String page = "<html><head><meta http-equiv='refresh' content='10'/>";
  page += "<title>ESP32 WebServer</title>";
  page += "<style>body{font-family:Arial;}</style></head><body>";
  page += "<h1>ESP32 WebServer</h1>";
  page += "<h3>DHT22 Sensor</h3>";
  page += "<ul><li>Temperature: ";
  page += temperature;
  page += "&deg;C</li><li>Humidity: ";
  page += humidity;
  page += "%</li></ul>";
  page += "<h3>LED Control</h3>";
  page += "<form action='/' method='POST'>";
  page += "<input type='radio' name='LED' value='1'> ON<br>";
  page += "<input type='radio' name='LED' value='0'> OFF<br>";
  page += "<input type='submit' value='Submit'>";
  page += "</form>";
  page += "<p>LED is currently <strong>" + ledState + "</strong></p>";
  page += "</body></html>";
  return page;
}
