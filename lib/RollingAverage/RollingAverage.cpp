/**
 * @file RollingAverage.h
 *
 * Defines and manipulates a data stream to keep track of a rolling average
 */

#include <stddef.h>         // for size_t
#include "RollingAverage.h" // header file

/**
 * Initialize the rolling average structure
 */
void RollingAverage_init(RollingAverage *rollingAverage, float *buffer, size_t size)
{
  rollingAverage->buffer = buffer;
  rollingAverage->size = size;
  rollingAverage->index = 0;
  rollingAverage->count = 0;
  rollingAverage->sum = 0.0f;
}

/**
 * Adds new value to the running sum and removes the least recent element (at current index)
 */
void RollingAverage_add(RollingAverage *rollingAverage, float value)
{
  if (rollingAverage->count < rollingAverage->size) // if buffer is not filled
  {
    rollingAverage->count++;
  }
  else
  {
    // subtract value being overwritten
    rollingAverage->sum -= rollingAverage->buffer[rollingAverage->index];
  }

  // sets current index to the new value
  rollingAverage->buffer[rollingAverage->index] = value;
  rollingAverage->sum += value;

  // increments index and loops if beyond size of buffer
  rollingAverage->index = (rollingAverage->index + 1) % rollingAverage->size;
}

/**
 * Divides the sum by the count to get the average
 */
float RollingAverage_getAverage(const RollingAverage *rollingAverage)
{
  if (rollingAverage->count == 0) // edge-case where function returns undefined
  {
    return 0.0f;
  }
  return rollingAverage->sum / rollingAverage->count;
}

/**
 * Starts from index and sets arrayOut[i] to the element in buffer, creating an array
 * from the circular buffer
 */
void RollingAverage_getBuffer(const RollingAverage *rollingAverage, float *arrayOut)
{
  int bufferIndex;
  for (int i = 0; i < rollingAverage->size; i++)
  {
    // circular index i past current index of rolling average
    bufferIndex = (rollingAverage->index + i) % rollingAverage->size;
    arrayOut[i] = rollingAverage->buffer[bufferIndex];
  }
}