/**
 * @file oled.h
 * 
 * Defines oled data struct and oled menu options and functions
 */

#ifndef OLED_H
#define OLED_H

typedef enum {
    BAR_CHART = 1,
    SUMMARY,
    SLEEP
} Menus;

typedef struct {
    Menus currentMenu;
    int warningPopup;
} Oled;

/**
 * Menu Templates
 */
#include "../lib/Oled/menuXBMs/bar-chart-menu.h"
extern const unsigned char BAR_CHART_XBM;
#include "../lib/Oled/menuXBMs/summary-menu.h"
extern const unsigned char SUMMARY_XBM;
#include "../lib/Oled/menuXBMs/sleep-menu.h"
extern const unsigned char SLEEP_XBM;
#include "../lib/Oled/menuXBMs/decibelLevel.h"
extern const unsigned char DECIBEL_XBM;
#include "../lib/Oled/menuXBMs/warning.h"
extern const unsigned char WARNING_XBM;

/**
 * Initialize the OLED UI state
 */
void Oled_init(Oled *oled);

/**
 * Returns the corresponding XBM to function menu 
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 */
void Oled_getBarChartXBM(char *xbm, float *decibelArray);

/**
 * Returns the corresponding XBM to function menu 
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 */
void Oled_getSummaryXBM(char *xbm);

/**
 * Returns the corresponding XBM to function menu 
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 */
void Oled_getSleepXBM(char *xbm);

/**
 * Returns the corresponding XBM to function menu 
 * Writes to caller provided XBM char array
 * For 128 x 32 oled
 */
void Oled_getDecibelXBM(char *xbm);

/**
 * Returns the corresponding XBM to function menu 
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 */
void Oled_getWarningXBM(char *xbm, float percentFilled);

#endif // OLED_UI_H
