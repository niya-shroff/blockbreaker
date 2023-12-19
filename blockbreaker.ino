#include <Adafruit_NeoMatrix.h>
#include <Adafruit_SSD1306.h>
#include "paj7620.h"

#define width 16
#define height 16
#define PIN 2
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(width, height, PIN,
                                               NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                                               NEO_GRB + NEO_KHZ800);

// Game properties
int lives = 3;
int score = 0;
bool gameOver;
bool gameWon;

// Paddle properties
const int paddleSize = 4;
int paddlePosition = 6;
#define GES_TIME 0

// Ball properties
int ballX = 3, ballY = 3;
int ballDX = 1, ballDY = -1;
#define BALL_SPEED 10

// Block properties
bool blocks[width][height];

// LCD properties
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Colors
const int ballColor = matrix.Color(0, 0, 255);
const int paddleColor = matrix.Color(0, 255, 0);
const int blockColor = matrix.Color(255, 255, 255);
const int clear = matrix.Color(0, 0, 0);

void gameWorldInit() {
  // Initialize blocks
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      blocks[i][j] = j > height / 2;
  drawBlocks();

  // Initialize ball
  ballX = 2; ballY = 4;
  ballDX = 1; ballDY = -1;

  // Initialize paddle
  paddlePosition = 6;
  
  // Game status
  gameOver = false;
  gameWon = false;
  score = 0;
  lives = 3;
}

void setup() {
  Serial.begin(9600);

  // Initialize matrix
  matrix.begin();
  matrix.clear();
  matrix.setBrightness(15);

  // Initial paddleMove
  pinMode(34, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
  paj7620Init();
  
  gameWorldInit();
  updatedisplay();
}

unsigned int lastBallDrawTime = 0;
unsigned int paddleTime = 0;
void loop() {
  unsigned int currentTime = millis();

  if (currentTime - lastBallDrawTime > BALL_SPEED) {
    drawBall();
    drawBlocks();
    lastBallDrawTime = currentTime;
  }
  if (currentTime - paddleTime > GES_TIME) {
    drawPaddle();
    paddleTime = currentTime;
  }

  if (gameOver || gameWon) {
    gameWorldInit();
  }
  updatedisplay();
}

void drawBall() {
  matrix.drawPixel(ballX, ballY, matrix.Color(0, 0, 0));
  moveBall();
  matrix.drawPixel(ballX, ballY, ballColor);
  matrix.show();
}
void moveBall() {
  int newBallX = ballX + ballDX;
  int newBallY = ballY + ballDY;

  // Check for wall collisions
  if (newBallX < 0 || newBallX >= width - 1) {
    ballDX = -ballDX;
    moveBall();
    return;
  }
  if (newBallY >= height - 1) {
    ballDY = -ballDY;
    moveBall();
    return;
  }

  // Check for game over
  if (newBallY < 0) {
    lives--;
    if (lives == 0) {
      gameOver = true;
      matrix.clear();
      gameWorldInit();
      return;
    }
    ballX = 3; ballY = 3;
    ballDX = 1; ballDY = -1;
    return;
  }

  // Check for paddle collisions
  if (newBallY == 0 && newBallX >= paddlePosition && newBallX < paddlePosition+paddleSize) {
    ballDY = -ballDY;
    moveBall();
    return;
  }
  if (newBallY == 0 && (newBallX == paddlePosition-1 && ballDX == 1 || newBallX == paddlePosition+paddleSize && ballDX == -1)) {
    ballDY = -ballDY; ballDX = -ballDX;
    moveBall();
    return;
  }

  // Check for block collisions
  if (blocks[ballX][ballY+1] || blocks[ballX][ballY-1]) {
    if (blocks[ballX][ballY+1]) blocks[ballX][ballY+1] = false;
    else blocks[ballX][ballY-1] = false;
    matrix.drawPixel(ballX, ballY+1, clear);
    ballDY = -ballDY;
    score++;
    moveBall();
    return;
  }
  if (blocks[ballX+1][ballY] || blocks[ballX-1][ballY]) {
    if (blocks[ballX+1][ballY]) blocks[ballX + 1][ballY] = false;
    else blocks[ballX-1][ballY] = false;
    matrix.drawPixel(ballX+1, ballY, clear);
    ballDX = -ballDX;
    score++;
    moveBall();
    return;
  }
  if (blocks[newBallX][newBallY]) {
    blocks[newBallX][newBallY] = false;
    matrix.drawPixel(newBallX, newBallY, clear);
    ballDY = -ballDY; ballDX = -ballDX;
    score++;
    moveBall();
    return;
  }

  // Update ball position based on direction
  ballX += ballDX;
  ballY += ballDY;
}

void drawPaddle() {
  for (int i = paddlePosition; i < paddlePosition + paddleSize; i++)
    matrix.drawPixel(i, 0, clear);
  updatePaddlePos();
  for (int i = paddlePosition; i < paddlePosition + paddleSize; i++)
    matrix.drawPixel(i, 0, paddleColor);
  matrix.show();
}
void updatePaddlePos() {
  uint8_t data = 0;
  paj7620ReadReg(0x43, 1, &data);

  if (data == GES_UP_FLAG || data == GES_RIGHT_FLAG || data == GES_CLOCKWISE_FLAG || data == GES_BACKWARD_FLAG)
    if (paddlePosition < width - paddleSize)
      paddlePosition++;
  if (data == GES_DOWN_FLAG || data == GES_LEFT_FLAG || data == GES_COUNT_CLOCKWISE_FLAG || data == GES_FORWARD_FLAG)
    if (paddlePosition > 0)
      paddlePosition--;

  if (digitalRead(34) == LOW)
    if (paddlePosition < width - paddleSize)
      paddlePosition++;
  if (digitalRead(35) == LOW)
    if (paddlePosition > 0)
      paddlePosition--;
}

void drawBlocks() {
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      if (blocks[i][j]) 
        matrix.drawPixel(i, j, blockColor);
}

void updatedisplay()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.print("Lives: ");
  display.print(lives);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  if (gameWon)
  {
    display.println("Congratulations!");
    display.println("You've won!");
  }
  else
  {
    display.print("Keep playing...");
  }
  display.display();
  delay(100);
}