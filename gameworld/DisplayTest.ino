#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Define your OLED display
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Game variables
int lives = 3;
bool gameWon = false;

void setup() {
    updatedisplay();
}

void loop() {
    if (!gameWon) {
       gameWon = checkGameWon();
       if (gameWon) {
           Serial.println("Congratulations! You've won the game!");
           updatedisplay();
        }
    }
    
}

void updatedisplay() {
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
    if (gameWon) {
        display.println("Congratulations!");
        display.println("You've won!");
    } else {
        display.print("Keep playing...");
    }
    display.display();
    delay(100); 
}

bool checkGameWon() {
  return lives > 0;
}
