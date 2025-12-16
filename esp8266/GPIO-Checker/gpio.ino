#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ESP8266-CONTROL";
const char* password = "12345678";

ESP8266WebServer server(80);

// ================= PIN CONFIG =================
// GPIO values
int gpioPins[] = {
  16, // D0
  5,  // D1
  4,  // D2
  0,  // D3
  2,  // D4
  14, // D5
  12, // D6
  13, // D7
  15  // D8
};

// Labels shown in GUI
String pinLabels[] = {
  "D0","D1","D2","D3","D4","D5","D6","D7","D8"
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
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP8266 Control Panel</title>
<style>
:root{
  --bg:#0b1220;
  --card:#111827;
  --accent:#22c55e;
  --off:#1f2937;
  --text:#e5e7eb;
  --muted:#9ca3af;
}
body{
  margin:0;
  font-family:system-ui;
  background:var(--bg);
  color:var(--text);
}
.header{
  padding:16px;
  text-align:center;
  font-size:20px;
  font-weight:600;
}
.sub{
  text-align:center;
  font-size:12px;
  color:var(--muted);
}
.grid{
  display:grid;
  grid-template-columns:repeat(auto-fit,minmax(130px,1fr));
  gap:14px;
  padding:14px;
}
.card{
  background:var(--card);
  border-radius:14px;
  padding:16px;
  text-align:center;
  border:2px solid transparent;
}
.card.active{
  border-color:var(--accent);
  background:linear-gradient(160deg,#0f172a,#052e16);
}
.pin{
  font-size:18px;
  font-weight:600;
  margin-bottom:8px;
}
.state{
  font-size:12px;
  color:var(--muted);
  margin-bottom:10px;
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
.onBtn{background:var(--accent);color:#000;}
.offBtn{background:var(--off);color:var(--text);}
footer{
  text-align:center;
  padding:12px;
  font-size:12px;
  color:var(--muted);
}
</style>
</head>
<body>

<div class="header">ESP8266 GPIO CONTROL</div>
<div class="sub">Single Pin Active Mode</div>

<div class="grid">
)rawliteral";

  for (int i = 0; i < pinCount; i++) {
    bool active = (activePin == i);

    html += "<div class='card ";
    if (active) html += "active";
    html += "'>";

    html += "<div class='pin'>";
    html += pinLabels[i];
    html += "</div>";

    html += "<div class='state'>";
    html += active ? "ACTIVE" : "INACTIVE";
    html += "</div>";

    html += "<form action='/set' method='get'>";
    html += "<input type='hidden' name='pin' value='" + String(i) + "'>";
    html += "<button class='";
    html += active ? "onBtn" : "offBtn";
    html += "'>";
    html += active ? "SELECTED" : "ACTIVATE";
    html += "</button></form>";

    html += "</div>";
  }

  html += R"rawliteral(
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

  Serial.println("ESP8266 AP READY");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
}
