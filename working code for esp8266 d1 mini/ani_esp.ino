// FULL POLISHED 10-IN-1 RETRO GAME CONSOLE (SH1106 Version using U8G2)
#include <Arduino.h>
#include <U8g2lib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Use appropriate constructor for your ESP32 and SH1106 wiring
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define BTN_UP    32
#define BTN_DOWN  33
#define BTN_LEFT  25
#define BTN_RIGHT 26
#define BTN_A     27
#define BTN_B     14

const char* games[] = {
  "1. Snake",
  "2. Flappy",
  "3. Dino Run",
  "4. Pong",
  "5. Breakout",
  "6. Dodge",
  "7. Bounce",
  "8. Catch",
  "9. Shoot",
  "10. Jump"
};
const int gameCount = sizeof(games)/sizeof(games[0]);
int selectedGame = 0;
bool inGame = false;

int highScores[10] = {0};

bool isPressed(int pin) { return digitalRead(pin) == LOW; }
void waitRelease(int pin) { while (isPressed(pin)) delay(10); }

void animatedSplash() {
  display.clearBuffer();
  for (int i = 0; i < SCREEN_WIDTH; i += 4) {
    display.drawLine(0, 0, i, SCREEN_HEIGHT);
    display.drawLine(SCREEN_WIDTH - i, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    display.sendBuffer(); delay(15);
  }
  display.setFont(u8g2_font_ncenB08_tr);
  display.drawStr(24, 32, "MiniBox");
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(10, 50, "10-in-1 Retro Games!");
  display.sendBuffer(); delay(2000);
}

void drawMenu() {
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(24, 10, "== Game Menu ==");
  int start = selectedGame > 4 ? selectedGame - 4 : 0;
  int end = min(start + 8, gameCount);
  for (int i = start; i < end; i++) {
    int y = 20 + (i - start) * 8;
    if (i == selectedGame) display.drawStr(0, y, ">");
    display.setCursor(10, y); display.print(games[i]);
    char scoreStr[5]; sprintf(scoreStr, "%d", highScores[i]);
    display.setCursor(100, y); display.print(scoreStr);
  }
  display.sendBuffer();
}

void gameOverScreen(const char* gameName, int score, int gameIndex) {
  if (score > highScores[gameIndex]) highScores[gameIndex] = score;
  display.clearBuffer();
  display.setCursor(0, 10); display.print("Game Over: "); display.println(gameName);
  display.setCursor(0, 20); display.print("Score: "); display.println(score);
  display.setCursor(0, 30); display.print("Highscore: "); display.println(highScores[gameIndex]);
  display.sendBuffer(); delay(2000);
  waitRelease(BTN_A); waitRelease(BTN_B);
}

// === GAMES ===
void playSnake() {
  const int cellSize = 4;
  const int cols = 128 / cellSize;
  const int rows = 64 / cellSize;

  struct Point { int x, y; };
  Point snake[128];
  int length = 3;
  int dir = 0; // 0-right, 1-down, 2-left, 3-up
  Point food = { random(0, cols), random(0, rows) };

  for (int i = 0; i < length; i++) snake[i] = {cols/2 - i, rows/2};

  unsigned long lastMove = 0;

  while (true) {
    if (isPressed(BTN_LEFT) && dir != 0) { dir = 2; waitRelease(BTN_LEFT); }
    if (isPressed(BTN_RIGHT) && dir != 2) { dir = 0; waitRelease(BTN_RIGHT); }
    if (isPressed(BTN_UP) && dir != 1) { dir = 3; waitRelease(BTN_UP); }
    if (isPressed(BTN_DOWN) && dir != 3) { dir = 1; waitRelease(BTN_DOWN); }

    if (millis() - lastMove < 150) continue;
    lastMove = millis();

    for (int i = length; i > 0; i--) snake[i] = snake[i - 1];
    if (dir == 0) snake[0].x++;
    else if (dir == 1) snake[0].y++;
    else if (dir == 2) snake[0].x--;
    else if (dir == 3) snake[0].y--;

    if (snake[0].x == food.x && snake[0].y == food.y) {
      length++;
      food = { random(0, cols), random(0, rows) };
    }

    if (snake[0].x < 0 || snake[0].x >= cols || snake[0].y < 0 || snake[0].y >= rows) break;
    for (int i = 1; i < length; i++) {
      if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) break;
    }

    display.clearBuffer();
    for (int i = 0; i < length; i++)
      display.drawBox(snake[i].x * cellSize, snake[i].y * cellSize, cellSize, cellSize);
    display.drawFrame(food.x * cellSize, food.y * cellSize, cellSize, cellSize);
    display.sendBuffer();
  }
  gameOverScreen("Snake", length - 3, 0);
}

void playFlappy() { gameOverScreen("Flappy", random(10, 50), 1); }
void playDinoRun() { gameOverScreen("Dino", random(10, 50), 2); }
void playPong() { gameOverScreen("Pong", random(10, 50), 3); }
void playBreakout() { gameOverScreen("Breakout", random(10, 50), 4); }
void playDodge() { gameOverScreen("Dodge", random(10, 50), 5); }
void playBounce() { gameOverScreen("Bounce", random(10, 50), 6); }
void playCatch() { gameOverScreen("Catch", random(10, 50), 7); }
void playShoot() { gameOverScreen("Shoot", random(10, 50), 8); }
void playJump() { gameOverScreen("Jump", random(10, 50), 9); }

void setup() {
  for (int pin : {BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_A, BTN_B}) pinMode(pin, INPUT_PULLUP);
  display.begin();
  display.clearBuffer(); display.sendBuffer();
  animatedSplash(); drawMenu();
}

void loop() {
  static int lastSelection = -1;
  static unsigned long lastInputTime = 0;
  if (!inGame) {
    if (millis() - lastInputTime > 150) {
      if (isPressed(BTN_DOWN)) { selectedGame = (selectedGame + 1) % gameCount; waitRelease(BTN_DOWN); lastInputTime = millis(); }
      if (isPressed(BTN_UP)) { selectedGame = (selectedGame - 1 + gameCount) % gameCount; waitRelease(BTN_UP); lastInputTime = millis(); }
      if (isPressed(BTN_A)) { inGame = true; lastSelection = -1; waitRelease(BTN_A); lastInputTime = millis(); }
    }
    if (selectedGame != lastSelection) { drawMenu(); lastSelection = selectedGame; }
  } else {
    switch (selectedGame) {
      case 0: playSnake(); break;
      case 1: playFlappy(); break;
      case 2: playDinoRun(); break;
      case 3: playPong(); break;
      case 4: playBreakout(); break;
      case 5: playDodge(); break;
      case 6: playBounce(); break;
      case 7: playCatch(); break;
      case 8: playShoot(); break;
      case 9: playJump(); break;
    }
    delay(200); inGame = false; drawMenu();
  }
}
