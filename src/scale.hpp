#pragma once

#include <SimpleKalmanFilter.h>
#include "HX711.h"

#define STATUS_EMPTY 0
#define STATUS_GRINDING_IN_PROGRESS 1
#define STATUS_GRINDING_FINISHED 2
#define STATUS_GRINDING_FAILED 3

#define CUP_WEIGHT 20.2
#define CUP_DETECTION_TOLERANCE 5 // 5 grams tolerance above or bellow cup weight to detect it

#define LOADCELL_DOUT_PIN 10  // D6 on XIAO ESP32C3
#define LOADCELL_SCK_PIN 8  // D7 on XIAO ESP32C3

#define LOADCELL_SCALE_FACTOR 1809.02

#define TARE_MEASURES 20 // use the average of measure for taring
#define SIGNIFICANT_WEIGHT_CHANGE 5 // 5 grams changes are used to detect a significant change
#define COFFEE_DOSE_WEIGHT 18
#define MAX_GRINDING_TIME 20000 // 20 seconds diff
#define GRINDING_FAILED_WEIGHT_TO_RESET 500 // force on balance need to be measured to reset grinding

#define GRINDER_ACTIVE_PIN D1  // D5 on XIAO ESP32C3

#define TARE_MIN_INTERVAL 10 * 1000 // auto-tare at most once every 10 seconds

extern double scaleWeight;
extern unsigned long scaleLastUpdatedAt;
extern unsigned long lastSignificantWeightChangeAt;
extern unsigned long lastTareAt;
extern bool scaleReady;
extern int scaleStatus;
extern double cupWeightEmpty;
extern unsigned long startedGrindingAt;
extern unsigned long finishedGrindingAt;

void setupScale();
