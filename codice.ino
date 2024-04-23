#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Vodafone-EAksGUM2r";
const char* password = "Y1gs%n5LY9t8nRPr2Vx7fCYxYl";
const int pirPin = 13;
const int relay1Pin = 12;
const int relay2Pin = 14;
const int relay3Pin = 27;
const int alarmSwitchPin = 4;

unsigned long startTime = 0;
const unsigned long activationDuration = 10000;
bool isAlarmActive = false;

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(pirPin, INPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(alarmSwitchPin, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<h1>Antifurto</h1>";
    html += "<p>Stato: ";
    html += (isAlarmActive ? "Attivo" : "Disattivo");
    html += "</p>";
    html += "<p><a href=\"/toggle\">Cambia Stato</a></p>";
    request->send(200, "text/html", html);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    isAlarmActive = !isAlarmActive;
    request->redirect("/");
  });

  server.begin();
}

void loop() {
  if (isAlarmActive && digitalRead(pirPin) == HIGH) {
    startTime = millis();

    digitalWrite(relay1Pin, HIGH);
    digitalWrite(relay2Pin, HIGH);
    digitalWrite(relay3Pin, HIGH);

    while (millis() - startTime < activationDuration) {
      digitalWrite(relay1Pin, !digitalRead(relay1Pin));
      delay(500);

      if ((millis() - startTime) % 2000 < 1000) {
        digitalWrite(relay2Pin, HIGH);
        digitalWrite(relay3Pin, LOW);
      } else {
        digitalWrite(relay2Pin, LOW);
        digitalWrite(relay3Pin, HIGH);
      }
    }

    digitalWrite(relay1Pin, LOW);
    digitalWrite(relay2Pin, LOW);
    digitalWrite(relay3Pin, LOW);
  }

  delay(1);
}