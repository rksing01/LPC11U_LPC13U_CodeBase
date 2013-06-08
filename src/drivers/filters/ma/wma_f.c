/**************************************************************************/
/*!
    @file     wma_f.c
    @author   Nguyen Quang Huy, Nguyen Thien Tin
    @brief    A simple moving average filter using float values

    @code

    // Declare a data buffer 8 values wide
    float wma_buffer[8];
    float wma_weight[8] = { 0.1F, 0.1F, 0.125F, 0.125F, 0.25F, 0.25F, 0.5F, 1.0F }

    // Now declare the filter with the window size and a buffer pointer
    wma_f_t wma = { .k = 0,
                    .size = 8,
                    .avg = 0,
                    .weight = wma_weight,
                    .buffer = wma_buffer };

    // Initialise the moving average filter
    if (wma_f_init(&wma))
    {
      printf("Something failed during filter init!\n");
    }

    // Add some values
    wma_f_add(&wma, 1.0);
    wma_f_add(&wma, 2.1);
    wma_f_add(&wma, -30.2);
    wma_f_add(&wma, -35.3);
    wma_f_add(&wma, 11.4);
    wma_f_add(&wma, 35.5);
    wma_f_add(&wma, 30.6);
    wma_f_add(&wma, 20.7); // We should have an avg value starting here
    wma_f_add(&wma, 3.8);
    wma_f_add(&wma, 10.9);

    printf("WINDOW SIZE   : %f\n", wma.size);
    printf("TOTAL SAMPLES : %f\n", wma.k);
    printf("CURRENT AVG   : %f\n", wma.avg);
    printf("\n");

    @endcode
    
    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2013, K. Townsend (microBuilder.eu)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
 */
/**************************************************************************/
#include "wma_f.h"

/**************************************************************************/
/*!
     @brief Initialises the wma_f_t instance

     @param[in]  wma
                 Pointer to the wma_f_t instance that includes the
                 window size, a pointer to the data buffer,
                 the current average (the output value), etc.
*/
/**************************************************************************/
error_t wma_f_init ( wma_f_t *wma )
{
  /* Check if the window size is valid (!= 0 and is a power of 2) */
  if (0 == wma->size) return ERROR_UNEXPECTEDVALUE;

  wma->avg = 0;
  wma->k = 0;
  wma->sum_weight = 0;
  
  for (uint16_t i = 0; i < wma->size; i++)
  {
    wma->sum_weight += wma->weight[i];
  }
  
  return ERROR_NONE;
}

/**************************************************************************/
/*!
     @brief Adds a new value to the wma_f_t instances

     @param[in]  wma
                 Pointer to the wma_f_t instances
     @param[in]  x
                 Value to insert
*/
/**************************************************************************/
void wma_f_add(wma_f_t *wma, float x)
{
  float *pSource = wma->buffer + wma->k % wma->size;

  /* Add new value into the data buffer of the filter */
  *pSource = x;

  /* Increase the total samples processed */
  wma->k++;

  /* Wait for 'window-size' worth of samples before averaging */
  if (wma->k < wma->size)
    return;

  /* Recalculate the total value over the entire buffer */
  double total = 0;
  uint16_t current_pos = wma->k % wma->size;
  for (uint16_t i = 0; i < wma->size; i++)
  {
    total += wma->buffer[(i + current_pos) % wma->size] * wma->weight[i];
  }

  /* Update the current average value */
  wma->avg = (float)(total / wma->sum_weight);
}
