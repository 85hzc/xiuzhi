/*!
    \file    utils.h
    \brief   utils head file

    \version 2019-07-15, V1.0.0, FOC demo
    \version 2020-12-15, V1.1.0, FOC demo
    \version 2021-02-01, V1.2.0, FOC demo
    \version 2021-08-16, V1.3.0, FOC demo
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef UTILS_H
#define UTILS_H

#include "gd32f30x.h"

#define SPEED_LEN_BITS                          3
#define SPEED_LEN                               (1 << SPEED_LEN_BITS)
extern uint32_t speed_samples[];
extern uint8_t speed_index;

#define CURRENT_LEN_BITS                        4
#define CURRENT_LEN                             (1 << CURRENT_LEN_BITS)

extern uint32_t current_samples1[];
extern uint32_t current_samples2[];
extern uint8_t current_index1, current_index2;

/* store sample value to the specified array */
void utils_add_sample(uint32_t *buffer, uint32_t sample, uint8_t length_bit, uint8_t *index);
/* get the average value of all the data in the array */
uint32_t utils_average_sample(uint32_t *buffer, uint8_t length_bit);
/* initialize sampling array */
void utils_sample_init(void);
/* get the linear mapping value from one space to another */
uint32_t utils_map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
float utils_map_float(float x, float in_min, float in_max, float out_min, float out_max);
/* give the source data a step operation which makes it towards the destination data */
int16_t utils_step_towards(int16_t value, int16_t goal, int16_t step);
/* save test value in integer format */
void utils_save_test(int32_t value);
/* save test value in float format */
void utils_save_test_float(float value);
/* fast sine and cosine implementation */
void utils_fast_sincos_better(float angle, float *sin, float *cos, uint16_t offset);
/* anti-windup processing of the applying voltage */
uint8_t utils_saturate_vector_2d(int16_t *x, int16_t *y, uint32_t max);
/* nomalize the input angle */
uint16_t utils_angle_nomalize(float angle);
/* return the middle value of three */
int16_t utils_middle_value(int16_t a, int16_t b, int16_t c);
/* fast sin&cos function */
void utils_sin_cos_u16(uint16_t phase, int16_t* sin, int16_t* cos);
/* the arctan function */
int16_t utils_atan_2f(float y, float x);
/* low pass filter */
#define UTILS_LP_FAST(value, sample, filter_constant)   (value -= (filter_constant) * (value - (sample)))
#define HZC_LP_FAST(value, sample, filter_constant)   (value = ((1-filter_constant) * (value)) + (filter_constant) * (sample))

#endif /* UTILS_H */
