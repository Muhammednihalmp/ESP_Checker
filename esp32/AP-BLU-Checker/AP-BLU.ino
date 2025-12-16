#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <BleKeyboard.h>

// ================= CONFIG =================
#define LED_PIN 2

const char* ssid = "ESP32-AP";
const char* password = "12345678";

// =========================================
WebServer server(80);
BleKeyboard bleKeyboard("ESP32 BLU", "Nihal MP", 100);

bool ledState = false;

// ================= HTML ===================
const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Control Panel</title>
<style>
body{
  margin:0;
  padding:15px;
  background:#1a1a1a;
  color:#fff;
  font-family:system-ui
}
.container{
  max-width:600px;
  margin:auto
}
.status{
  padding:10px;
  background:#2a2a2a;
  border-radius:5px;
  display:flex;
  align-items:center;
  font-size:.9em;
  margin-bottom:15px
}
.dot{
  width:8px;
  height:8px;
  border-radius:50%;
  margin-right:8px
}
.on{background:#0f0}
.off{background:#f00}

button{
  width:48%;
  padding:12px;
  border:none;
  border-radius:5px;
  font-size:16px;
  cursor:pointer;
}
.btn-on{background:#00c853;color:#000}
.btn-off{background:#d50000;color:#fff}
.row{display:flex;gap:10px}
</style>
</head>

<body>
<div class="container">
  <h1>ESP32 AP / BLE Control</h1>
  <p>By Nihal MP</p>

  <div class="status">
    <span class="dot off" id="dot"></span>
    <span id="status">Checking...</span>
  </div>
  
  <div class="row">
    <button class="btn-on" onclick="setLED('on')">LED ON</button>
    <button class="btn-off" onclick="setLED('off')">LED OFF</button>
  </div>
</div>

<script>
function updateStatus(){
  fetch('/status')
  .then(r=>r.json())
  .then(d=>{
    const dot=document.getElementById('dot');
    const txt=document.getElementById('status');

    if(d.ble){
      dot.className='dot on';
      txt.textContent='BLE Connected | LED: ' + (d.led ? 'ON' : 'OFF');
    }else{
      dot.className='dot off';
      txt.textContent='BLE Disconnected | LED: ' + (d.led ? 'ON' : 'OFF');
    }
  });
}

function setLED(state){
  fetch('/led?state='+state);
}

setInterval(updateStatus,2000);
updateStatus();
</script>
</body>
</html>
)rawliteral";

// ================= HANDLERS =================
void handleRoot() {
  server.send_P(200, "text/html", MAIN_PAGE);
}

void handleStatus() {
  String json = "{";
  json += "\"ble\":";
  json += bleKeyboard.isConnected() ? "true" : "false";
  json += ",";
  json += "\"led\":";
  json += ledState ? "true" : "false";
  json += "}";

  server.send(200, "application/json", json);
}

void handleLED() {
  if (server.hasArg("state")) {
    String s = server.arg("state");

    if (s == "on") {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
    } 
    else if (s == "off") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
    }
  }
  server.send(200, "text/plain", "OK");
}

// ================= SETUP ====================
void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  bleKeyboard.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/led", HTTP_GET, handleLED);

  server.begin();
  Serial.println("Web server started");
}

// ================= LOOP =====================
void loop() {
  server.handleClient();
}
