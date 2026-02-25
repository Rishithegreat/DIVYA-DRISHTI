/*
 * ═══════════════════════════════════════════════════════════════════════════
 * Project: Divya Drishti - Braille Learning Device
 * Board: ESP32 (38-pin)
 * Description: Interactive Braille alphabet learning system with tactile 
 *              feedback using servos and audio guidance
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include <Arduino.h>
#include <ESP32Servo.h>
#include <DFRobotDFPlayerMini.h>

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION CONSTANTS
// ═══════════════════════════════════════════════════════════════════════════

// Hardware Serial for DFPlayer
#define FPSerial Serial1

// Pin Definitions
namespace Pins {
    // Navigation Buttons
    const int BUTTON_NEXT = 13;
    const int BUTTON_PREV = 14;
    
    // Servo Motors (Braille Dots 1-6)
    const int SERVO_DOT1 = 4;
    const int SERVO_DOT2 = 5;
    const int SERVO_DOT3 = 26;
    const int SERVO_DOT4 = 25;
    const int SERVO_DOT5 = 33;
    const int SERVO_DOT6 = 32;
    
    // DFPlayer Serial Communication
    const int DFPLAYER_RX = 16;
    const int DFPLAYER_TX = 17;
}

// Servo Positions (calibrated for tactile feedback)
namespace ServoAngles {
    // Dot 1 (Top Left)
    const int DOT1_UP = 175;
    const int DOT1_DOWN = 155;
    
    // Dot 2 (Middle Left)
    const int DOT2_UP = 20;
    const int DOT2_DOWN = 0;
    
    // Dot 3 (Bottom Left)
    const int DOT3_UP = 180;
    const int DOT3_DOWN = 165;
    
    // Dot 4 (Top Right)
    const int DOT4_UP = 0;
    const int DOT4_DOWN = 20;
    
    // Dot 5 (Middle Right)
    const int DOT5_UP = 150;
    const int DOT5_DOWN = 185;
    
    // Dot 6 (Bottom Right)
    const int DOT6_UP = 0;
    const int DOT6_DOWN =20;
}

// System Settings
namespace Settings {
    const int TOTAL_LETTERS = 26;
    const int INTRO_AUDIO_TRACK = 27;
    const int VOLUME_LEVEL = 25;
    const int SERVO_MOVEMENT_DELAY = 500;    // Time for servos to reach position
    const int AUDIO_DISPLAY_DELAY = 2000;     // Time to keep pattern displayed
    const int DEBOUNCE_DELAY = 250;           // Button debounce time
    const int INTRO_AUDIO_DURATION = 4000;    // Intro audio length
    const unsigned long SERIAL_BAUD = 115200;
    const unsigned long DFPLAYER_BAUD = 9600;
}

// ═══════════════════════════════════════════════════════════════════════════
// GLOBAL OBJECTS & VARIABLES
// ═══════════════════════════════════════════════════════════════════════════

// Servo Objects
Servo servoDot1, servoDot2, servoDot3;
Servo servoDot4, servoDot5, servoDot6;

// DFPlayer Object
DFRobotDFPlayerMini dfPlayer;

// State Variables
volatile bool nextButtonPressed = false;
volatile bool prevButtonPressed = false;
int currentPosition = 1;  // Current letter (1-26)

// Timing Variables
unsigned long lastActionTime = 0;
bool isDisplayingPattern = false;

// ═══════════════════════════════════════════════════════════════════════════
// FUNCTION DECLARATIONS
// ═══════════════════════════════════════════════════════════════════════════

// Initialization
void initializeServos();
void initializeDFPlayer();
void initializeButtons();

// Servo Control
void setDot1(bool raised);
void setDot2(bool raised);
void setDot3(bool raised);
void setDot4(bool raised);
void setDot5(bool raised);
void setDot6(bool raised);
void clearAllDots();

// Braille Pattern Control
void displayBraillePattern(int letterNumber);
void playAudioForLetter(int letterNumber);

// Navigation
void handleNextButton();
void handlePreviousButton();

// Interrupt Service Routines
void IRAM_ATTR nextButtonISR();
void IRAM_ATTR prevButtonISR();

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

void setup() {
    // Initialize Serial Monitor
    Serial.begin(Settings::SERIAL_BAUD);
    Serial.println("\n╔═══════════════════════════════════════════╗");
    Serial.println("║     Divya Drishti Braille Learner         ║");
    Serial.println("║     Initializing System...                ║");
    Serial.println("╚═══════════════════════════════════════════╝\n");
    
    // Initialize Hardware Components
    initializeButtons();
    initializeServos();
    initializeDFPlayer();
    
    // Clear all dots to starting position
    clearAllDots();
    delay(1000);
    
    // Play introduction audio
    Serial.println("Playing introduction...");
    dfPlayer.play(Settings::INTRO_AUDIO_TRACK);
    delay(Settings::INTRO_AUDIO_DURATION);
    
    // Display first letter (A)
    Serial.println("System ready! Starting with letter A\n");
    displayBraillePattern(currentPosition);
    playAudioForLetter(currentPosition);
    lastActionTime = millis();
    isDisplayingPattern = true;
}

// ═══════════════════════════════════════════════════════════════════════════
// MAIN LOOP
// ═══════════════════════════════════════════════════════════════════════════

void loop() {
    // Check if pattern display time has elapsed
    if (isDisplayingPattern && 
        (millis() - lastActionTime >= Settings::AUDIO_DISPLAY_DELAY)) {
        isDisplayingPattern = false;
    }
    
    // Handle button presses only when not displaying a pattern
    if (!isDisplayingPattern) {
        // Safely read and clear interrupt flags
        noInterrupts();
        bool nextPressed = nextButtonPressed;
        bool prevPressed = prevButtonPressed;
        nextButtonPressed = false;
        prevButtonPressed = false;
        interrupts();
        
        if (nextPressed) {
            handleNextButton();
        }
        
        if (prevPressed) {
            handlePreviousButton();
        }
    }
    
    // Small delay to prevent CPU overload
    delay(10);
}

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void initializeButtons() {
    pinMode(Pins::BUTTON_NEXT, INPUT_PULLUP);
    pinMode(Pins::BUTTON_PREV, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(Pins::BUTTON_NEXT), 
                    nextButtonISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(Pins::BUTTON_PREV), 
                    prevButtonISR, FALLING);
    
    Serial.println("✓ Navigation buttons initialized");
}

void initializeServos() {
    // Attach servos to pins
    servoDot1.attach(Pins::SERVO_DOT1);
    servoDot2.attach(Pins::SERVO_DOT2);
    servoDot3.attach(Pins::SERVO_DOT3);
    servoDot4.attach(Pins::SERVO_DOT4);
    servoDot5.attach(Pins::SERVO_DOT5);
    servoDot6.attach(Pins::SERVO_DOT6);
    
    Serial.println("✓ Servo motors initialized");
}

void initializeDFPlayer() {
    // Initialize DFPlayer Serial
    FPSerial.begin(Settings::DFPLAYER_BAUD, SERIAL_8N1, 
                   Pins::DFPLAYER_RX, Pins::DFPLAYER_TX);
    
    // Initialize DFPlayer
    if (!dfPlayer.begin(FPSerial)) {
        Serial.println("✗ DFPlayer initialization failed!");
        Serial.println("  Please check:");
        Serial.println("  - Wiring connections");
        Serial.println("  - SD card is inserted");
        Serial.println("  - Audio files are present");
        while (1) {
            delay(1000);  // Halt execution
        }
    }
    
    dfPlayer.volume(Settings::VOLUME_LEVEL);
    Serial.println("✓ DFPlayer audio module initialized");
}

// ═══════════════════════════════════════════════════════════════════════════
// SERVO CONTROL FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void setDot1(bool raised) {
    servoDot1.write(raised ? ServoAngles::DOT1_UP : ServoAngles::DOT1_DOWN);
}

void setDot2(bool raised) {
    servoDot2.write(raised ? ServoAngles::DOT2_UP : ServoAngles::DOT2_DOWN);
}

void setDot3(bool raised) {
    servoDot3.write(raised ? ServoAngles::DOT3_UP : ServoAngles::DOT3_DOWN);
}

void setDot4(bool raised) {
    servoDot4.write(raised ? ServoAngles::DOT4_UP : ServoAngles::DOT4_DOWN);
}

void setDot5(bool raised) {
    servoDot5.write(raised ? ServoAngles::DOT5_UP : ServoAngles::DOT5_DOWN);
}

void setDot6(bool raised) {
    servoDot6.write(raised ? ServoAngles::DOT6_UP : ServoAngles::DOT6_DOWN);
}

void clearAllDots() {
    setDot1(false);
    setDot2(false);
    setDot3(false);
    setDot4(false);
    setDot5(false);
    setDot6(false);
    delay(Settings::SERVO_MOVEMENT_DELAY);
}

// ═══════════════════════════════════════════════════════════════════════════
// BRAILLE PATTERN DISPLAY
// ═══════════════════════════════════════════════════════════════════════════

void displayBraillePattern(int letterNumber) {
    // Standard Grade 1 Braille Patterns
    // Braille dot numbering:
    //   1 • • 4
    //   2 • • 5
    //   3 • • 6
    
    switch(letterNumber) {
        case 1:  // A: dot 1
            setDot1(true);
            break;
            
        case 2:  // B: dots 1,2
            setDot1(true);
            setDot2(true);
            break;
            
        case 3:  // C: dots 1,4
            setDot1(true);
            setDot4(true);
            break;
            
        case 4:  // D: dots 1,4,5
            setDot1(true);
            setDot4(true);
            setDot5(true);
            break;
            
        case 5:  // E: dots 1,5
            setDot1(true);
            setDot5(true);
            break;
            
        case 6:  // F: dots 1,2,4
            setDot1(true);
            setDot2(true);
            setDot4(true);
            break;
            
        case 7:  // G: dots 1,2,4,5
            setDot1(true);
            setDot2(true);
            setDot4(true);
            setDot5(true);
            break;
            
        case 8:  // H: dots 1,2,5
            setDot1(true);
            setDot2(true);
            setDot5(true);
            break;
            
        case 9:  // I: dots 2,4
            setDot2(true);
            setDot4(true);
            break;
            
        case 10:  // J: dots 2,4,5
            setDot2(true);
            setDot4(true);
            setDot5(true);
            break;
            
        case 11:  // K: dots 1,3
            setDot1(true);
            setDot3(true);
            break;
            
        case 12:  // L: dots 1,2,3
            setDot1(true);
            setDot2(true);
            setDot3(true);
            break;
            
        case 13:  // M: dots 1,3,4
            setDot1(true);
            setDot3(true);
            setDot4(true);
            break;
            
        case 14:  // N: dots 1,3,4,5
            setDot1(true);
            setDot3(true);
            setDot4(true);
            setDot5(true);
            break;
            
        case 15:  // O: dots 1,3,5
            setDot1(true);
            setDot3(true);
            setDot5(true);
            break;
            
        case 16:  // P: dots 1,2,3,4
            setDot1(true);
            setDot2(true);
            setDot3(true);
            setDot4(true);
            break;
            
        case 17:  // Q: dots 1,2,3,4,5
            setDot1(true);
            setDot2(true);
            setDot3(true);
            setDot4(true);
            setDot5(true);
            break;
            
        case 18:  // R: dots 1,2,3,5
            setDot1(true);
            setDot2(true);
            setDot3(true);
            setDot5(true);
            break;
            
        case 19:  // S: dots 2,3,4
            setDot2(true);
            setDot3(true);
            setDot4(true);
            break;
            
        case 20:  // T: dots 2,3,4,5
            setDot2(true);
            setDot3(true);
            setDot4(true);
            setDot5(true);
            break;
            
        case 21:  // U: dots 1,3,6
            setDot1(true);
            setDot3(true);
            setDot6(true);
            break;
            
        case 22:  // V: dots 1,2,3,6
            setDot1(true);
            setDot2(true);
            setDot3(true);
            setDot6(true);
            break;
            
        case 23:  // W: dots 2,4,5,6
            setDot2(true);
            setDot4(true);
            setDot5(true);
            setDot6(true);
            break;
            
        case 24:  // X: dots 1,3,4,6
            setDot1(true);
            setDot3(true);
            setDot4(true);
            setDot6(true);
            break;
            
        case 25:  // Y: dots 1,3,4,5,6
            setDot1(true);
            setDot3(true);
            setDot4(true);
            setDot5(true);
            setDot6(true);
            break;
            
        case 26:  // Z: dots 1,3,5,6
            setDot1(true);
            setDot3(true);
            setDot5(true);
            setDot6(true);
            break;
    }
    
    delay(Settings::SERVO_MOVEMENT_DELAY);
}

void playAudioForLetter(int letterNumber) {
    // 0001.mp3 = A
    // 0002.mp3 = B
    // ...
    // 0026.mp3 = Z
    dfPlayer.play(letterNumber);
}


// ═══════════════════════════════════════════════════════════════════════════
// NAVIGATION HANDLERS
// ═══════════════════════════════════════════════════════════════════════════

void handleNextButton() {
    currentPosition++;
    if (currentPosition > Settings::TOTAL_LETTERS) {
        currentPosition = 1;
    }
    
    Serial.print("→ Next: Letter ");
    Serial.print(currentPosition);
    Serial.print(" (");
    Serial.print((char)('A' + currentPosition - 1));
    Serial.println(")");
    
    clearAllDots();
    displayBraillePattern(currentPosition);
    playAudioForLetter(currentPosition);
    
    lastActionTime = millis();
    isDisplayingPattern = true;
}

void handlePreviousButton() {
    currentPosition--;
    if (currentPosition < 1) {
        currentPosition = Settings::TOTAL_LETTERS;
    }
    
    Serial.print("← Previous: Letter ");
    Serial.print(currentPosition);
    Serial.print(" (");
    Serial.print((char)('A' + currentPosition - 1));
    Serial.println(")");
    
    clearAllDots();
    displayBraillePattern(currentPosition);
    playAudioForLetter(currentPosition);
    
    lastActionTime = millis();
    isDisplayingPattern = true;
}

// ═══════════════════════════════════════════════════════════════════════════
// INTERRUPT SERVICE ROUTINES (ISR)
// ═══════════════════════════════════════════════════════════════════════════

void IRAM_ATTR nextButtonISR() {
    static unsigned long lastInterruptTime = 0;
    unsigned long currentTime = millis();
    
    // Debounce: Only register press if enough time has passed
    if (currentTime - lastInterruptTime > Settings::DEBOUNCE_DELAY) {
        nextButtonPressed = true;
        lastInterruptTime = currentTime;
    }
}

void IRAM_ATTR prevButtonISR() {
    static unsigned long lastInterruptTime = 0;
    unsigned long currentTime = millis();
    
    // Debounce: Only register press if enough time has passed
    if (currentTime - lastInterruptTime > Settings::DEBOUNCE_DELAY) {
        prevButtonPressed = true;
        lastInterruptTime = currentTime;
    }
}
