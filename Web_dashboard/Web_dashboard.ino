#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Wi-Fi credentials
const char* ssid = "@ba_sithuu";
const char* password = "00000000";

// BMP180 sensor
Adafruit_BMP085 bmp;

// Web server on port 80
WebServer server(80);

// HTML dashboard
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Weather Monitor</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background: #eef3f8;
      text-align: center;
      margin: 0;
      padding: 20px;
    }
    h1 { color: #123b67; }
    .card {
      background: white;
      padding: 22px;
      margin: 15px auto;
      max-width: 350px;
      border-radius: 14px;
      box-shadow: 0 3px 12px #0002;
    }
    .value {
      font-size: 32px;
      font-weight: bold;
      color: #087f8c;
    }
    .label { color: #555; }
    #status { color: #16803c; }
  </style>
</head>
<body>
  <h1>BMP180 Monitoring</h1>
  <p>ESP32 Live Sensor Dashboard</p>

  <div class="card">
    <div class="label">Temperature</div>
    <p class="value"><span id="temp">--</span> &deg;C</p>
  </div>

  <div class="card">
    <div class="label">Atmospheric Pressure</div>
    <p class="value"><span id="pressure">--</span> hPa</p>
  </div>

  <div class="card">
    <div class="label">Estimated Altitude</div>
    <p class="value"><span id="altitude">--</span> m</p>
  </div>

  <p id="status">Connecting to sensor...</p>
  <p>Updates every 2 seconds</p>

  <script>
    async function updateData() {
      try {
        const response = await fetch('/data');
        if (!response.ok) throw new Error('Sensor request failed');

        const data = await response.json();

        document.getElementById('temp').textContent =
          data.temperature.toFixed(2);
        document.getElementById('pressure').textContent =
          data.pressure.toFixed(2);
        document.getElementById('altitude').textContent =
          data.altitude.toFixed(2);

        document.getElementById('status').textContent =
          'Sensor connected';
      } catch (error) {
        document.getElementById('status').textContent =
          'Connection lost - retrying...';
      }
    }

    updateData();
    setInterval(updateData, 2000);
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", webpage);
}

void handleData() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;
  float altitude = bmp.readAltitude();

  String json = "{";
  json += "\"temperature\":" + String(temperature, 2) + ",";
  json += "\"pressure\":" + String(pressure, 2) + ",";
  json += "\"altitude\":" + String(altitude, 2);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(21, 22);

  // Initialize BMP180
  if (!bmp.begin()) {
    Serial.println("BMP180 not detected!");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("BMP180 initialized");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("Open website: http://");
  Serial.println(WiFi.localIP());

  // Configure web routes
  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}