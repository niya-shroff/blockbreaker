#include <Wire.h>
#include "paj7620.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define GESTURE_UPDATE_INTERVAL 1000  // Update gesture every 1000 milliseconds (1 second)
#define GES_REACTION_TIME  500
#define GES_ENTRY_TIME     800
#define GES_QUIT_TIME      1000

int gesture = 0;  // -1 (left), 0 (stop), 1 (right)
unsigned long lastGestureUpdate = 0;

// Define your OLED display
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Game variables
int lives = 3;
bool gameWon = false;

void setup() {
    uint8_t error = 0;

    Serial.begin(9600);
    Serial.println("\nPAJ7620U2 TEST DEMO: Recognize 9 gestures.");

    error = paj7620Init();
    if (error) {
        Serial.print("INIT ERROR, CODE:");
        Serial.println(error);
    } else {
        Serial.println("INIT OK");
    }

    if(!display.begin(SSD1306_I2C_ADDRESS, OLED_RESET, OLED_RESET)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    display.display();  
    delay(2000); 

    Serial.println("Please input your gestures:\n");
    updateDisplay();
}

void loop() {
    updateGesture();  // Update the gesture value

    if (!gameWon) {
       gameWon = checkGameWon();
       if (gameWon) {
           Serial.println("Congratulations! You've won the game!");
           updateDisplay();
        }
    }
    
    delay(100);  // Add a small delay to control the loop speed
}

void updateGesture() {
    uint8_t data = 0, error;

    // Check if it's time to update the gesture value
    if (millis() - lastGestureUpdate >= GESTURE_UPDATE_INTERVAL) {
        error = paj7620ReadReg(0x43, 1, &data);

        if (!error) {
            switch (data) {
                case GES_DOWN_FLAG:
                    gesture = 1;  // Move right
                    break;
                case GES_UP_FLAG:
                    gesture = -1;  // Move left
                    break;
                default:
                    gesture = 0;  // Stop for other cases
                    break;
            }
            Serial.print("Gesture: ");
            Serial.println(gesture);
            updateDisplay();
        }
        lastGestureUpdate = millis();
    }
}

void updateDisplay() {
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

}

bool checkGameWon() {
    // check if all blocks have been destroyed
    // Return true if the game is won, false otherwise
    // For now, assuming the game is won if lives are greater than 0
    return lives > 0;
}
