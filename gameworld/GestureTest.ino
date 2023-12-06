#include "paj7620.h"

#define GESTURE_UPDATE_INTERVAL 1000  // Update gesture every 1000 milliseconds (1 second)
#define GES_REACTION_TIME  500
#define GES_ENTRY_TIME     800
#define GES_QUIT_TIME      1000

int gesture = 0;  // -1 (left), 0 (stop), 1 (right)
unsigned long lastGestureUpdate = 0;

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
    Serial.println("Please input your gestures:\n");
}

void loop() {
    updateGesture();  // Update the gesture value

    // Your other game logic goes here

    delay(100);  // Add a small delay to control the loop speed
}

void updateGesture() {
    uint8_t data = 0, error;

    // Check if it's time to update the gesture value
    if (millis() - lastGestureUpdate >= GESTURE_UPDATE_INTERVAL) {
        error = paj7620ReadReg(0x43, 1, &data);

        if (!error) {
            // When different gestures are detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
            switch (data) {
                case GES_DOWN_FLAG:
                    gesture = 1;
                    break;
                case GES_UP_FLAG:
                    gesture = -1;
                    break;
                default:
                    gesture = 0;  // Stop for other cases
                    break;
            }

            // Print the gesture value
            Serial.print("Gesture: ");
            Serial.println(gesture);
        }

        // Update the last gesture update time
        lastGestureUpdate = millis();
    }
}
