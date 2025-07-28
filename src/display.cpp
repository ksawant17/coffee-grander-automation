#include "display.hpp"
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMono9pt7b.h> // Font for all numbers
#include <Fonts/FreeSans9pt7b.h> // Font for headlines
#include <Fonts/Picopixel.h> // Small font for labels

// Global variables from scale.hpp - these should be declared in scale.cpp
extern double scaleWeight;
extern unsigned long scaleLastUpdatedAt;
extern unsigned long lastSignificantWeightChangeAt;
extern unsigned long lastTareAt;
extern bool scaleReady;
extern int scaleStatus;
extern double cupWeightEmpty;
extern unsigned long startedGrindingAt;
extern unsigned long finishedGrindingAt;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

TaskHandle_t DisplayTask;

#define SLEEP_AFTER_MS 100 * 1000 // sleep after 10 seconds

// Helper: center text on screen at y
void centerPrintToScreen(const char *str, int16_t y, const GFXfont *font = nullptr) {
  int16_t x1, y1;
  uint16_t w, h;
  if (font) display.setFont(font);
  else display.setFont();
  display.getTextBounds(str, 0, y, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, y + h); // y+h to match baseline
  display.print(str);
  display.setFont(); // Reset to default if needed
}

void updateDisplay(void * parameter) {
  char buf[64];

  for(;;) {
    display.clearDisplay();
    
    // Show weight immediately after initialization, don't wait for significant weight change
    if (scaleLastUpdatedAt == 0) {
      display.setFont(&FreeSans9pt7b);
      centerPrintToScreen("Init...", 5, &FreeSans9pt7b);
    } else if (!scaleReady) {
      display.setFont(&FreeSans9pt7b);
      centerPrintToScreen("SCALE ERROR", 5, &FreeSans9pt7b);
    } else {
      if (scaleStatus == STATUS_GRINDING_IN_PROGRESS) {
        // Top line: Grinding status
        display.setFont(&FreeSans9pt7b);
        centerPrintToScreen("Grinding...", 0, &FreeSans9pt7b);

        // Weight display: current -> target
        display.setFont(&FreeMono9pt7b);
        snprintf(buf, sizeof(buf), "%3.1fg", scaleWeight - cupWeightEmpty);
        display.setCursor(0, 30);
        display.print(buf);

        // Arrow
        display.setCursor(60, 30);
        display.print(">");

        display.setCursor(70, 30);
        snprintf(buf, sizeof(buf), "%3.1fg", (float)COFFEE_DOSE_WEIGHT);
        display.print(buf);

        // Time display at bottom
        display.setFont(&Picopixel);
        snprintf(buf, sizeof(buf), "%3.1fs", (double)(millis() - startedGrindingAt) / 1000);
        centerPrintToScreen(buf, 50, &Picopixel);
        
      } else if (scaleStatus == STATUS_EMPTY) {
        // Weight label
        display.setFont(&FreeSans9pt7b);
        centerPrintToScreen("Weight:", 0, &FreeSans9pt7b);

        // Weight value
        display.setFont(&FreeMono9pt7b);
        snprintf(buf, sizeof(buf), "%3.1fg", scaleWeight);
        centerPrintToScreen(buf, 25, &FreeMono9pt7b);
        
      } else if (scaleStatus == STATUS_GRINDING_FAILED) {
        // Error message
        display.setFont(&FreeSans9pt7b);
        centerPrintToScreen("Grinding failed", 5, &FreeSans9pt7b);

        display.setFont(&Picopixel);
        centerPrintToScreen("Press balance", 15, &Picopixel);
        centerPrintToScreen("to reset", 25, &Picopixel);
        
      } else if (scaleStatus == STATUS_GRINDING_FINISHED) {
        // Success message
        display.setFont(&FreeSans9pt7b);
        centerPrintToScreen("Finished!", 0, &FreeSans9pt7b);

        // Final weight display
        display.setFont(&FreeMono9pt7b);
        snprintf(buf, sizeof(buf), "%3.1fg", scaleWeight - cupWeightEmpty);
        display.setCursor(0, 30);
        display.print(buf);

        // Arrow
        display.setCursor(60, 30);
        display.print(">");

        // Arrow
        display.setCursor(70, 30);
        snprintf(buf, sizeof(buf), "%3.1fg", (float)COFFEE_DOSE_WEIGHT);
        display.print(buf);

        // Time display
        display.setFont(&Picopixel);
        snprintf(buf, sizeof(buf), "%3.1fs", (double)(finishedGrindingAt - startedGrindingAt) / 1000);
        centerPrintToScreen(buf, 50, &Picopixel);
      }
    }
    display.display();
    delay(100); // Update every 100ms
  }
}

void setupDisplay() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  // Clear the buffer again and set up text properties
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSans9pt7b);

  // Test pattern to verify display is working
  display.setCursor(0, 20);
  display.println("Display Ready!");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();

  // Create the display update task
  xTaskCreatePinnedToCore(
      updateDisplay, /* Function to implement the task */
      "Display", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &DisplayTask,  /* Task handle. */
      1); /* Core where the task should run */
}
