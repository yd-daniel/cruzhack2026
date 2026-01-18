/**
 * @file RollingAverage.h
 *
 * Defines and manipulates a data stream to keep track of a rolling average
 */

#include <stddef.h>
#include "RollingAverage.h"

void RollingAverage_init(RollingAverage *rollingAverage, float *buffer, size_t size)
{
  rollingAverage->buffer = buffer;
  rollingAverage->size = size;
  rollingAverage->index = 0;
  rollingAverage->count = 0;
  rollingAverage->sum = 0.0f;
}

void RollingAverage_addValue(RollingAverage *rollingAverage, float value)
{
  if (rollingAverage->size == 0)
  {
    return;
  }

  if (rollingAverage->count < rollingAverage->size)
  {
    rollingAverage->buffer[rollingAverage->index] = value;
    rollingAverage->sum += value;
    rollingAverage->count++;
  }
  else
  {
    rollingAverage->sum -= rollingAverage->buffer[rollingAverage->index];
    rollingAverage->buffer[rollingAverage->index] = value;
    rollingAverage->sum += value;
  }

  rollingAverage->index = (rollingAverage->index + 1) % rollingAverage->size;
}

float RollingAverage_getAverage(const RollingAverage *rollingAverage)
{
  if (rollingAverage->count == 0)
  {
    return 0.0f;
  }
  return rollingAverage->sum / (float)rollingAverage->count;
}

void RollingAverage_getBuffer(const RollingAverage *rollingAverage, float *bufferOut)
{
  if (rollingAverage->size == 0)
  {
    return;
  }

  size_t start = (rollingAverage->count < rollingAverage->size) ? 0 : rollingAverage->index;

  for (size_t i = 0; i < rollingAverage->size; i++)
  {
    if (i < rollingAverage->count)
    {
      bufferOut[i] = rollingAverage->buffer[(start + i) % rollingAverage->size];
    }
    else
    {
      bufferOut[i] = 0.0f;
    }
  }
}
