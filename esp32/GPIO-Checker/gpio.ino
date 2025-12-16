#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32-CONTROL";
const char* password = "12345678";

WebServer server(80);

// ================= GPIO CONFIG =================
int gpioPins[] = {
  2,4,5,12,13,14,15,
  16,17,18,19,21,22,
  23,25,26,27,32,33
};

const int pinCount = sizeof(gpioPins) / sizeof(gpioPins[0]);
int activePin = -1;
// ==============================================

void disableAllPins() {
  for (int i = 0; i < pinCount; i++) {
    digitalWrite(gpioPins[i], LOW);
  }
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Control Panel</title>
<style>
:root{
  --bg:#0b1220;
  --card:#111827;
  --accent:#22c55e;
  --off:#1f2937;
  --text:#e5e7eb;
  --muted:#9ca3af;
}
*{box-sizing:border-box;}
body{
  margin:0;
  font-family:system-ui,-apple-system,BlinkMacSystemFont;
  background:var(--bg);
  color:var(--text);
}
.header{
  padding:18px;
  text-align:center;
  font-size:22px;
  font-weight:600;
  letter-spacing:0.5px;
}
.sub{
  text-align:center;
  font-size:13px;
  color:var(--muted);
  margin-bottom:10px;
}
.container{
  padding:14px;
}
.grid{
  display:grid;
  grid-template-columns:repeat(auto-fill,minmax(140px,1fr));
  gap:14px;
}
.card{
  background:var(--card);
  border-radius:14px;
  padding:16px;
  text-align:center;
  transition:0.2s;
  border:2px solid transparent;
}
.card.active{
  border-color:var(--accent);
  background:linear-gradient(160deg,#0f172a,#052e16);
}
.pin{
  font-size:18px;
  font-weight:600;
  margin-bottom:10px;
}
.state{
  font-size:12px;
  color:var(--muted);
  margin-bottom:12px;
}
button{
  width:100%;
  padding:10px;
  border:none;
  border-radius:10px;
  font-size:14px;
  font-weight:600;
  cursor:pointer;
}
.onBtn{
  background:var(--accent);
  color:#000;
}
.offBtn{
  background:var(--off);
  color:var(--text);
}
footer{
  text-align:center;
  padding:14px;
  font-size:12px;
  color:var(--muted);
}
</style>
</head>
<body>

<div class="header">ESP32 GPIO CONTROL</div>
<div class="sub">Single-Pin Active Mode</div>

<div class="container">
<div class="grid">
)rawliteral";

  for (int i = 0; i < pinCount; i++) {
    bool isActive = (activePin == i);

    html += "<div class='card ";
    if (isActive) html += "active";
    html += "'>";

    html += "<div class='pin'>GPIO ";
    html += String(gpioPins[i]);
    html += "</div>";

    html += "<div class='state'>";
    html += isActive ? "ACTIVE" : "INACTIVE";
    html += "</div>";

    html += "<form action='/set' method='get'>";
    html += "<input type='hidden' name='pin' value='" + String(i) + "'>";
    html += "<button class='";
    html += isActive ? "onBtn" : "offBtn";
    html += "'>";
    html += isActive ? "SELECTED" : "ACTIVATE";
    html += "</button></form>";

    html += "</div>";
  }

  html += R"rawliteral(
</div>
</div>

<footer>
Created by <b>Nihal MP</b>
</footer>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleSet() {
  int index = server.arg("pin").toInt();
  if (index < 0 || index >= pinCount) {
    server.send(400, "text/plain", "Invalid pin");
    return;
  }

  disableAllPins();
  digitalWrite(gpioPins[index], HIGH);
  activePin = index;

  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < pinCount; i++) {
    pinMode(gpioPins[i], OUTPUT);
    digitalWrite(gpioPins[i], LOW);
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("AP READY");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
}
