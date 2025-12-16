#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define LED_PIN 4   // GPIO4 = D2

const char* ssid = "ESP8266-AP";
const char* password = "12345678";

ESP8266WebServer server(80);
bool ledState = false;

const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP8266 Control</title>
<style>
body{background:#1a1a1a;color:#fff;font-family:system-ui;padding:15px}
.container{max-width:600px;margin:auto}
.status{background:#2a2a2a;padding:10px;border-radius:5px;margin-bottom:15px}
button{width:48%;padding:12px;border:none;border-radius:5px;font-size:16px}
.on{background:#00c853;color:#000}
.off{background:#d50000;color:#fff}
.row{display:flex;gap:10px}
</style>
</head>

<body>
<div class="container">
  <h1>ESP8266 AP LED Control</h1>
  <p><b>Create by:</b> Nihal MP</p>

  <div class="status" id="status">Checking...</div>

  <div class="row">
    <button class="on" onclick="setLED('on')">LED ON</button>
    <button class="off" onclick="setLED('off')">LED OFF</button>
  </div>
</div>

<script>
function updateStatus(){
  fetch('/status')
  .then(r=>r.json())
  .then(d=>{
    document.getElementById('status').innerText =
      "LED Status: " + (d.led ? "ON" : "OFF");
  });
}

function setLED(s){
  fetch('/led?state='+s);
}

setInterval(updateStatus,2000);
updateStatus();
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", MAIN_PAGE);
}

void handleStatus() {
  server.send(200, "application/json",
    String("{\"led\":") + (ledState ? "true" : "false") + "}");
}

void handleLED() {
  if (server.arg("state") == "on") {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
  } else {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/led", handleLED);

  server.begin();
}

void loop() {
  server.handleClient();
}
