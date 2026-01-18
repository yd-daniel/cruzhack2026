/**
 * @file oled.cpp
 *
 * Defines oled data struct and oled menu options and functions
 */

#include "Oled.h"
#include <string.h>

void drawBar(char *xbm, int barIndex, int barHeight);
void drawBarPercentage(char *xbm, int barPercent);

/**
 * Initialize the OLED UI state
 */
void Oled_init(Oled *oled)
{
  oled->currentMenu = BAR_CHART;
  oled->warningPopup = 0;
}

/**
 * Returns the corresponding XBM to function menu
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 * The function expects the array to have the least recent value in index 0
 */
void Oled_getBarChartXBM(char *xbm, float *decibelArray)
{
  const int BAR_CHART_SIZE = 28;
  const int BAR_CHART_MAX_HEIGHT = 44;
  const int MAX_DECIBEL = 100;
  float decibelPercent;

  // Get Bar Chart Template
  memcpy(xbm, BAR_CHART_XBM, sizeof(BAR_CHART_XBM));

  // Draw Bars
  for (int i = 0; i < BAR_CHART_SIZE; i++)
  {
    decibelPercent = decibelArray[i] / MAX_DECIBEL;
    if (decibelPercent < 0.0f) decibelPercent = 0.0f;
    if (decibelPercent > 1.0f) decibelPercent = 1.0f;

    int barHeight = (int)(decibelPercent * BAR_CHART_MAX_HEIGHT);
    if (barHeight < 0) barHeight = 0;
    if (barHeight > BAR_CHART_MAX_HEIGHT) barHeight = BAR_CHART_MAX_HEIGHT;

    drawBar(xbm, i, barHeight);
  }
}

/**
 * Returns the corresponding XBM to function menu
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 */
void Oled_getSummaryXBM(char *xbm)
{
  memcpy(xbm, SUMMARY_XBM, sizeof(SUMMARY_XBM));
}

/**
 * Returns the corresponding XBM to function menu
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 */
void Oled_getSleepXBM(char *xbm)
{
  memcpy(xbm, SLEEP_XBM, sizeof(SLEEP_XBM));
}

/**
 * Returns the corresponding XBM to function menu
 * Writes to caller provided XBM char array
 * For 128 x 32 oled
 */
void Oled_getDecibelXBM(char *xbm)
{
  memcpy(xbm, DECIBEL_XBM, sizeof(DECIBEL_XBM));
}

/**
 * Returns the corresponding XBM to function menu
 * Writes to caller provided XBM char array
 * For 128 x 64 oled
 */
void Oled_getWarningXBM(char *xbm, float percentFilled)
{
  int barPercent = (int)(percentFilled * 100.0f);
  if (barPercent < 0) barPercent = 0;
  if (barPercent > 100) barPercent = 100;

  memcpy(xbm, WARNING_XBM, sizeof(WARNING_XBM));
  drawBarPercentage(xbm, barPercent);
}

/**
 * Helper func for Bar Chart Menu, draws each bar
 * Indexed from 0 to 27, each 4px wide, 0111.
 */
void drawBar(char *xbm, int barIndex, int barHeight)
{
  const __uint8_t BAR_TEMPLATE = 0x0E;       // 01110000
  const __uint8_t DISPLAY_WIDTH = 16;        // bytes
  const int BAR_ZERO = (59 * DISPLAY_WIDTH); // bytes, bottom left of first bar's white space

  // accounts for left half / right half of byte
  int barShift = 0;
  if (barIndex % 2 == 1)
  {
    barShift = 4;
  }

  // decrements by display width bytes for every barHeight
  for (int i = 0; i < barHeight; i++)
  {
    xbm[BAR_ZERO + (i * (-DISPLAY_WIDTH))] |= BAR_TEMPLATE >> barShift;
  }
}

/**
 * Helper func for warning time progress bar
 */
void drawBarPercentage(char *xbm, int barPercent)
{
  const int BAR_WIDTH = 103;                     // pixels
  const int BAR_HEIGHT = 8;                      // pixels
  const __uint8_t DISPLAY_WIDTH = 16;            // bytes
  const int BAR_ZERO = (42 * DISPLAY_WIDTH) + 1; // bytes, bottom left of progress bar
  const int BAR_ZERO_PIXEL = 3;                  // pixels

  int fillPixels = (BAR_WIDTH * barPercent) / 100;
  if (fillPixels < 0) fillPixels = 0;
  if (fillPixels > BAR_WIDTH) fillPixels = BAR_WIDTH;

  for (int i = 0; i < BAR_HEIGHT; i++)
  {
    int rowStart = BAR_ZERO + (i * (-DISPLAY_WIDTH));

    for (int x = 0; x < fillPixels; x++)
    {
      int bitPos = BAR_ZERO_PIXEL + x;
      int byteOffset = bitPos / 8;
      int bitOffset = bitPos % 8;
      xbm[rowStart + byteOffset] |= (1 << bitOffset);
    }
  }
}
