
#include "display.hpp"
#include "scale.hpp"

void setup() {
  Serial.begin(9600);
  Serial.println("Starting Coffee Grinder...");
  
  // Initialize the scale first
  setupScale();
  
  // Initialize the display
  setupDisplay();
  
  Serial.println("Initialization complete!");
}

void loop() {
  // Main loop - most work is done in tasks
  delay(100);
}
 