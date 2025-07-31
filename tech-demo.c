// TechPager_Demo.ino - Clean demo version of romantic pager
// Hardware: ESP8266 (D1 Mini), SSD1306 128x64 OLED, Vibration Motor

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_A D5  // Cycle Notes
#define BUTTON_B D6  // Trigger Phone Finder
#define VIBRATION_PIN D7

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const String OTA_URL = "https://github.com/5i344A/esp8266-ani/raw/refs/heads/main/backup.bin";

// Pushbullet
const char* host = "api.pushbullet.com";
const int httpsPort = 443;
const char* PUSHBULLET_TOKEN = "Bearer YOUR_PUSHBULLET_TOKEN";

String notes[] = {
  "Welcome to TechPager!",
  "This is a showcase demo.",
  "Press A to cycle notes.",
  "Press B to ping your phone.",
  "Supports OTA and alerts!"
};

int noteIndex = 0;
bool messageReceived = false;
unsigned long lastCheck = 0;

void vibrate(int duration = 300) {
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(duration);
  digitalWrite(VIBRATION_PIN, LOW);
}

void drawSplash() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 20);
  display.println("TechPager");
  display.display();
  delay(2000);
}

void drawNote() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("-- Notes --");
  display.println();
  display.println(notes[noteIndex]);
  if (messageReceived) {
    display.setCursor(100, 56);
    display.print("[!]");
  }
  display.display();
}

void checkPushbullet() {
  if (millis() - lastCheck < 10000) return; // every 10s
  lastCheck = millis();

  WiFiClientSecure client;
  client.setInsecure();

  if (!client.connect(host, httpsPort)) return;

  String url = "/v2/pushes?limit=1";
  String auth = String("Authorization: ") + PUSHBULLET_TOKEN;

  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println(auth);
  client.println("Connection: close");
  client.println();

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
  }

  String payload = client.readString();
  if (payload.indexOf("TechPager") != -1) {
    messageReceived = true;
    vibrate(500);
  }
}

void triggerPhoneFinder() {
  HTTPClient http;
  http.begin("https://maker.ifttt.com/trigger/find_my_phone/with/key/YOUR_IFTTT_KEY");
  http.GET();
  http.end();
  vibrate();
}

void checkOTA() {
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  ESPhttpUpdate.update(OTA_URL);
}

void setup() {
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  drawSplash();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(100);

  drawNote();
  delay(1000);

  checkOTA();
}

void loop() {
  checkPushbullet();

  if (digitalRead(BUTTON_A) == LOW) {
    noteIndex = (noteIndex + 1) % (sizeof(notes) / sizeof(notes[0]));
    drawNote();
    delay(300);
  }

  if (digitalRead(BUTTON_B) == LOW) {
    triggerPhoneFinder();
    drawNote();
    delay(300);
  }
}
