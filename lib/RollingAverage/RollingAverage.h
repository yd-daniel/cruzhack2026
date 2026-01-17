/**
 * @file RollingAverage.h
 *
 * Defines and manipulates a data stream to keep track of a rolling average
 */
#ifndef ROLLING_AVERAGE_H
#define ROLLING_AVERAGE_H

#include <stddef.h> // for size_t

typedef struct
{
  float *buffer; // pointer to user-provided buffer
  size_t size;   // max number of elements
  size_t index;  // index of the least recent element
  size_t count;  // number of valid elements
  float sum;     // running sum
} RollingAverage;

/**
 * Initialize the rolling average structure
 * Buffer must be provided
 */
void RollingAverage_init(RollingAverage *rollingAverage, float *buffer, size_t size);

/**
 * Add a new value to the rolling average
 */
void RollingAverage_addValue(RollingAverage *rollingAverage, float value);

/**
 * Get the current rolling average
 */
float RollingAverage_getAverage(const RollingAverage *rollingAverage);

/**
 * Gets the array in order of least recent (index 0) to most recent (last index) elements
 * Writes to a provided buffer
 */
void RollingAverage_getBuffer(const RollingAverage *rollingAverage, float *bufferOut);

#endif