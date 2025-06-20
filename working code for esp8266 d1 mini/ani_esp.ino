#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

void vibrateBuzz(int duration);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int buttonA = D3;
const int buttonB = D4;
const int buttonOTA = D1;

const char* ssidList[] = {"Soorya ", "OnePlus12", "LoveLink"};
const char* passwordList[] = {"mgm123@#", "gfg123@#", "heartwifi@2024"};
const char* pushToken = "o.ECclJ7uRP0wqiXXn0ryZflwgPS9lk9nJ";
const char* host = "api.pushbullet.com";
const int httpsPort = 443;

const char* otaUrl = "https://esp8266-ota.pages.dev/backup.bin";

const char* memories[] = {
  "1st & foremost I love you",
  "14th Nov - Made you mine\n6 is your bday, I remember ok hehe",
  "Those little dates we went together\nwere the best days of my life ",
  "When you made me feel safe,\nI really felt like a kid",
  "Holding your hands is still my weakness",
  "ur cheeks r to die for",
  "My kid is damn lucky to have u",
  "never ever feel like u r not enough\nbecause u are",
  "when things get tough, call me\n ill never let u go",
  "no matter how tough it gets ill always be there",
  "u really annoy me but i love it",
  "u bring the spice into my life",
  "u are gonnna be a great doctor",
  "ur belly is really cozy to hold",
  "u r perfect baby",
  "remember the time u made me eat\nall the left over chocolala on feb 14th?",
  "wear the ring idiot ik u not wearing rn"
};
const int memoryCount = sizeof(memories) / sizeof(memories[0]);
int currentMemory = 0;
unsigned long lastMemoryTime = 0;

enum State { SPLASH, HEART, MEMORY, SENDING, SENT_OK, SENT_FAIL };
State screenState = SPLASH;
unsigned long screenTimer = 0;
bool heartBeatUp = true;
unsigned long lastHeartUpdate = 0;

unsigned long lastDebounceTimeA = 0;
unsigned long lastDebounceTimeB = 0;
unsigned long lastDebounceTimeOTA = 0;
const unsigned long debounceDelay = 50;

const int motorPin = D0;

void setup() {
  Serial.begin(115200);
  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonOTA, INPUT_PULLUP);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);

  vibrateBuzz(150);
  delay(100);
  vibrateBuzz(150);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  showSplash();
  delay(2500);
  showHeart();
  screenState = HEART;
}

void loop() {
  static bool lastA = HIGH;
  static bool lastB = HIGH;
  static bool lastOTA = HIGH;

  bool currA = digitalRead(buttonA);
  bool currB = digitalRead(buttonB);
  bool currOTA = digitalRead(buttonOTA);

  if (lastA == HIGH && currA == LOW && (millis() - lastDebounceTimeA) > debounceDelay) {
    lastDebounceTimeA = millis();
    handleButtonA();
  }
  if (lastB == HIGH && currB == LOW && (millis() - lastDebounceTimeB) > debounceDelay) {
    lastDebounceTimeB = millis();
    handleButtonB();
  }
  if (lastOTA == HIGH && currOTA == LOW && (millis() - lastDebounceTimeOTA) > debounceDelay) {
    lastDebounceTimeOTA = millis();
    handleOTAUpdate();
  }

  lastA = currA;
  lastB = currB;
  lastOTA = currOTA;

  if (screenState == MEMORY && millis() - lastMemoryTime > 45000) {
    showHeart();
    screenState = HEART;
  }

  if (screenState == HEART && millis() - lastHeartUpdate > 500) {
    drawBeatingHeart(heartBeatUp);
    heartBeatUp = !heartBeatUp;
    lastHeartUpdate = millis();
  }

  if ((screenState == SENT_OK || screenState == SENT_FAIL) && millis() > screenTimer) {
    showHeart();
    screenState = HEART;
  }
}

void handleButtonA() {
  vibrateBuzz(120);
  currentMemory = (currentMemory + 1) % memoryCount;
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - 1, 5, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(15, 3);
  display.println("Our Memory Vault");
  display.drawFastHLine(10, 13, 108, SSD1306_WHITE);
  display.setCursor(5, 20);
  display.setTextWrap(true);
  display.println(memories[currentMemory]);
  display.display();
  screenState = MEMORY;
  lastMemoryTime = millis();
}

void handleButtonB() {
  vibrateBuzz(120);
  sendPushNotification();
}

void showSplash() {
  display.clearDisplay();
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - 1, 6, SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(20, 22);
  display.println("For Ani");
  display.setTextSize(1);
  display.setCursor(28, 46);
  display.println("With Love <3");
  display.display();
}

void showHeart() {
  screenState = HEART;
  lastHeartUpdate = 0;
  heartBeatUp = true;
  drawBeatingHeart(true);
}

void drawBeatingHeart(bool large) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(36, 0);
  display.println("Love u ani");

  int cx = SCREEN_WIDTH / 2;
  int cy = SCREEN_HEIGHT / 2 + 10;
  int r = large ? 8 : 6;

  display.fillCircle(cx - r, cy - r, r, SSD1306_WHITE);
  display.fillCircle(cx + r, cy - r, r, SSD1306_WHITE);
  display.fillTriangle(cx - 2 * r, cy - r, cx + 2 * r, cy - r, cx, cy + 2 * r, SSD1306_WHITE);

  for (int i = 0; i < 10; i++) {
    int x = random(0, SCREEN_WIDTH);
    int y = random(0, SCREEN_HEIGHT);
    if (random(0, 2) == 0) {
      display.drawPixel(x, y, SSD1306_WHITE);
    }
  }

  display.display();
}

void showMessage(const char* msg) {
  display.clearDisplay();
  display.setTextSize(1);
  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - 1, 5, SSD1306_WHITE);
  display.setCursor(10, 28);
  display.print(msg);
  display.display();
}

void sendPushNotification() {
  screenState = SENDING;
  showMessage("Sending magic...");

  if (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 3; i++) {
      WiFi.begin(ssidList[i], passwordList[i]);
      unsigned long start = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
        delay(200);
      }
      if (WiFi.status() == WL_CONNECTED) break;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    showMessage("Wi-Fi Failed");
    screenState = SENT_FAIL;
    screenTimer = millis() + 3000;
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  if (!client.connect(host, httpsPort)) {
    showMessage("Push Failed");
    screenState = SENT_FAIL;
    screenTimer = millis() + 3000;
    return;
  }

  String body = "{\"type\": \"note\", \"title\": \"From Ani\", \"body\": \"She pressed the button!\"}";

  client.print(String("POST ") + "/v2/pushes" + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Bearer " + pushToken + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + body.length() + "\r\n\r\n" +
               body);

  showMessage("Delivered with love");
  screenState = SENT_OK;
  screenTimer = millis() + 3000;
}

void vibrateBuzz(int duration = 150) {
  digitalWrite(motorPin, HIGH);
  delay(duration);
  digitalWrite(motorPin, LOW);
}

void handleOTAUpdate() {
  for (int i = 3; i > 0; i--) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(30, 5);
    display.println("OTA Update v6.4");
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("Starting in");
    display.setCursor(50, 48);
    display.print(i);
    display.display();
    delay(1000);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(30, 22);
  display.println("Checking for");
  display.setCursor(40, 35);
  display.println("updates");
  display.display();

  display.setCursor(45, 40);
  display.println("2...");
  display.display();
  delay(1000);

  display.setCursor(45, 40);
  display.println("1...");
  display.display();
  delay(1000);

  showMessage("Checking for updates...");

  if (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 3; i++) {
      WiFi.begin(ssidList[i], passwordList[i]);
      unsigned long start = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
        delay(200);
      }
      if (WiFi.status() == WL_CONNECTED) break;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    showMessage("Wi-Fi Failed");
    delay(3000);
    showHeart();
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();

  Serial.print("Connecting to: ");
  Serial.println(otaUrl);

  t_httpUpdate_return ret = ESPhttpUpdate.update(client, otaUrl);

  Serial.printf("OTA Result: %d - %s\n", ret, ESPhttpUpdate.getLastErrorString().c_str());

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      showMessage("OTA Failed");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      showMessage("No Update Found");
      break;
    case HTTP_UPDATE_OK:
      showMessage("Updated! Rebooting");
      delay(1000);
      ESP.restart();
      break;
  }

  delay(3000);
  showHeart();
}
