#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
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
        display.print("Congratulations! You've won!");
    } else {
        display.print("Keep playing...");
    }
    display.display();
    delay(100); 
}

bool checkGameWon() {
    // check if all blocks have been destroyed
    // Return true if the game is won, false otherwise
    // For now, assuming the game is won if lives are greater than 0
    return lives > 0;
}
