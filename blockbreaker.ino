#include <Adafruit_NeoMatrix.h>
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
bool gameOver = false;
bool gameWon = false;

// Paddle properties
const int paddleSize = 4;
int paddlePosition = 6;
uint8_t gesture = 0;
#define GES_REACTION_TIME 800
#define GES_QUIT_TIME 1000

// Ball properties
int ballX = 2, ballY = 3;
int ballDX = 1, ballDY = -1;
#define BALL_SPEED 1000

// Block properties
bool blocks[width][height];

// Colors
const int ballColor = matrix.Color(0, 0, 255);
const int paddleColor = matrix.Color(0, 255, 0);
const int blockColor = matrix.Color(255, 255, 255);
const int clear = matrix.Color(0, 0, 0);

void setup() {
  Serial.begin(9600);

  // Initialize matrix
  matrix.begin();
  matrix.clear();
  matrix.setBrightness(15);

  // Initialize blocks
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      blocks[i][j] = j > height / 2;

  // Initialize gesture sensor
  paj7620Init();
}

unsigned int lastBallDrawTime = 0;
void loop() {
  unsigned int currentTime = millis();

  if (currentTime - lastBallDrawTime > BALL_SPEED) {
    drawBall();
    drawPaddle();
    drawBlocks();
    lastBallDrawTime = currentTime;
  }
}

void drawBall() {
  matrix.drawPixel(ballX, ballY, matrix.Color(0, 0, 0));
  moveBall();
  matrix.drawPixel(ballX, ballY, ballColor);
  matrix.show();
}
void moveBall() {
  // Update ball position based on direction
  ballX += ballDX;
  ballY += ballDY;

  // Wall collision
  if (ballX <= 0 || ballX >= 15)
    ballDX = -ballDX;
  if (ballY <= 0)
    ballDY = -ballDY;

  // paddle collision
  if (ballY == height - 2 && ballX >= paddlePosition && ballX < paddlePosition + paddleSize)
    ballDY = -ballDY;

  // Check for block collisions
  if (blocks[ballX][ballY]) {
    blocks[ballX][ballY] = false;
    ballDY = -ballDY;
  }
}

void drawPaddle() {
  for (int i = paddlePosition; i < paddlePosition + paddleSize; i++)
    matrix.drawPixel(i, 0, clear);
  updatePaddlePos();
  for (int i = paddlePosition; i < paddlePosition + paddleSize; i++)
    matrix.drawPixel(i, 0, paddleColor);
}
void updatePaddlePos() {
  paj7620ReadReg(0x43, 1, &gesture);
  if (gesture == GES_RIGHT_FLAG)
    if (paddlePosition < width - paddleSize) {
      Serial.println("right");
      paddlePosition++;
    }
    else if (gesture == GES_LEFT_FLAG)
      if (paddlePosition > 0) {
        Serial.println("left");
        paddlePosition--;
      }
}

void drawBlocks() {
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      if (blocks[i][j])
        matrix.drawPixel(i, j, blockColor);
}