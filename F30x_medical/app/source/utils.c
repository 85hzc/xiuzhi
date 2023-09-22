/*!
    \file    utils.c
    \brief   FOC demo

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

#include "global.h"

/* speed buffer */
uint32_t speed_samples[SPEED_LEN];
uint8_t speed_index = 0;

/* current buffer 1 */
uint32_t current_samples1[CURRENT_LEN];
uint8_t current_index1 = 0;

/* current buffer 2 */
uint32_t current_samples2[CURRENT_LEN];
uint8_t current_index2 = 0;

/* test value array */
int32_t test[20] = {0};
float test_float[20] = {0};

/*!
    \brief      store sample value to the specified array
    \param[in]  *buffer: the pointer of the destination array
    \param[in]  sample: the value which will be stored in the array
    \param[in]  length_bit: the length of the array, expressed in the length_bit power of 2
    \param[in]  *index: the index of the array
    \param[out] none
    \retval     none
*/
void utils_add_sample(uint32_t *buffer, uint32_t sample, uint8_t length_bit, uint8_t *index)
{
    uint8_t cnt_mask = 0xFF >> (8 - length_bit);
    buffer[*index] = sample;
    *index += 1;
    *index &= cnt_mask;
}

/*!
    \brief      get the average value of all the data in the array
    \param[in]  *buffer: the pointer of the destination array
    \param[in]  length_bit: the length of the array, expressed in the length_bit power of 2
    \param[out] none
    \retval     the average value
*/
uint32_t utils_average_sample(uint32_t *buffer, uint8_t length_bit)
{
    uint8_t i, max = (1 << length_bit);
    uint32_t sum = 0;
    for(i=0; i<max; i++){
        sum += buffer[i];
    }
    sum /= max;
    
    return (sum);
}

/*!
    \brief      initialize sampling array
    \param[out] none
    \param[out] none
    \retval     none
*/
void utils_sample_init(void)
{
    uint8_t i;
    
    for(i=0; i<SPEED_LEN; i++){
        speed_samples[i] = 0;
    }
    
    for(i=0; i<CURRENT_LEN; i++){
        current_samples1[i] = 0;
        current_samples2[i] = 0;
    }
}

/*!
    \brief      get the linear mapping value from one space to another
    \param[in]  x: the input data
    \param[in]  in_min: the minimum value of the primary space
    \param[in]  in_max: the maximum value of the primary space
    \param[in]  out_min: the minimum value of the secondary space
    \param[in]  out_max: the maximum value of the secondary space
    \param[out] none
    \retval     the mapping value
*/
uint32_t utils_map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float utils_map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/*!
    \brief      give the source data a step operation which makes it towards the destination data
    \param[in]  value: the source data
    \param[in]  goal: the destination data
    \param[in]  step: the magnitude of the step operation
    \param[out] none
    \retval     the data stepped increased or decreased
*/
int16_t utils_step_towards(int16_t value, int16_t goal, int16_t step)
{
    if(value < goal){
        if((value + step) < goal){
            value += step;
        }else{
            value = goal;
        }
    }else if (value > goal){
        if((value - step) > goal){
            value -= step;
        }else{
            value = goal;
        }
    }
    return (value);
}

/*!
    \brief      save test value in integer format
    \param[in]  value: the storing data
    \param[out] none
    \retval     none
*/
void utils_save_test(int32_t value)
{
    static int8_t i;

    test[i++] = value;

    if(i == 20) i = 0;
}

/*!
    \brief      save test value in float format
    \param[in]  value: the storing data
    \param[out] none
    \retval     none
*/
void utils_save_test_float(float value)
{
    static int16_t i;

    test_float[i++] = value;

    if(i == 20) 
        i = 0;
}

/*!
    \brief      fast sine and cosine implementation
    \param[in]  angle: the angle in degree, which is multipled by 100
    \param[in]  sin: A pointer to store the sine value
    \param[in]  cos: A pointer to store the cosine value
    \param[in]  offset: the offset of angle in degree, which is multipled by 100
    \param[out] none
    \retval     none
*/
void utils_fast_sincos_better(float angle, float *sin, float *cos, uint16_t offset)
{
    /* the angle in radian */
    angle = (angle - 100*offset) / 18000*M_PI;

    /* keep the angle from -PI to PI */
    while(angle < -M_PI){
        angle += 2.0f * M_PI;
    }
    while(angle >  M_PI){
        angle -= 2.0f * M_PI;
    }

    /* sine computation */
    if(angle < 0.0f){
        *sin = 1.27323954f * angle + 0.405284735f * angle * angle;

        if(*sin < 0.0f){
            *sin = 0.225f * (*sin * -*sin - *sin) + *sin;
        }else{
            *sin = 0.225f * (*sin * *sin - *sin) + *sin;
        }
    }else{
        *sin = 1.27323954f * angle - 0.405284735f * angle * angle;

        if(*sin < 0.0f){
            *sin = 0.225f * (*sin * -*sin - *sin) + *sin;
        }else{
            *sin = 0.225f * (*sin * *sin - *sin) + *sin;
        }
    }

    /* cosine computation */
    angle += 0.5f * M_PI;
    if(angle >  M_PI){
        angle -= 2.0f * M_PI;
    }

    if(angle < 0.0f){
        *cos = 1.27323954f * angle + 0.405284735f * angle * angle;

        if(*cos < 0.0f){
            *cos = 0.225f * (*cos * -*cos - *cos) + *cos;
        }else{
            *cos = 0.225f * (*cos * *cos - *cos) + *cos;
        }
    }else{
        *cos = 1.27323954f * angle - 0.405284735f * angle * angle;

        if(*cos < 0.0f){
            *cos = 0.225f * (*cos * -*cos - *cos) + *cos;
        }else{
            *cos = 0.225f * (*cos * *cos - *cos) + *cos;
        }
    }
}

/*!
    \brief      anti-windup processing of the applying voltage
    \param[out] x: the pointer of the first input volatage
    \param[out] y: the pointer of the second input volatage
    \param[in]  max: the max magnitude of the applying voltage
    \retval     none
*/
uint8_t utils_saturate_vector_2d(int16_t *x, int16_t *y, uint32_t max)
{
    uint8_t retval = 0;
    float mag;
    
    arm_sqrt_f32(*x * *x + *y * *y, &mag);

    if (mag < 1e-7f){
        mag = 1e-7f;
    }

    if(mag > max){
        const float f = max / mag;
        *x *= f;
        *y *= f;
        retval = 1;
    }

    return retval;
}

/*!
    \brief      nomalize the input angle
    \param[in]  angle: the angle to be nomalized
    \param[out] none
    \retval     the nomalized angle
*/
uint16_t utils_angle_nomalize(float angle)
{
    if(angle > 36000) angle -= 36000;
    else if(angle < 0) angle += 36000;
    
    return (angle);
}

/*!
    \brief      return the middle value of three
    \param[in]  a: the first variable
    \param[in]  b: the second variable
    \param[in]  c: the third variable
    \param[out] none
    \retval     the nomalized angle
*/
int16_t utils_middle_value(int16_t a, int16_t b, int16_t c)
{
    if((a-b)*(b-c)>0){
        return b;
    }else if((b-a)*(a-c)>0){
        return a;
    }else{
        return c;
    }
}

int16_t sin_table[91] = {0x0000,0x023B,0x0477,0x06B2,0x08ED,0x0B27,0x0D61,0x0F99,0x11D0,0x1405
                        ,0x1639,0x186C,0x1A9C,0x1CCA,0x1EF7,0x2120,0x2347,0x256C,0x278D,0x29AB
                        ,0x2BC6,0x2DDE,0x2FF2,0x3203,0x340F,0x3617,0x381C,0x3A1B,0x3C17,0x3E0D
                        ,0x3FFF,0x41EC,0x43D3,0x45B6,0x4793,0x496A,0x4B3B,0x4D07,0x4ECD,0x508C
                        ,0x5246,0x53F9,0x55A5,0x574B,0x58E9,0x5A81,0x5C12,0x5D9C,0x5F1E,0x6099
                        ,0x620C,0x6378,0x64DC,0x6638,0x678D,0x68D9,0x6A1D,0x6B58,0x6C8B,0x6DB6
                        ,0x6ED9,0x6FF2,0x7103,0x720B,0x730A,0x7400,0x74EE,0x75D2,0x76AD,0x777E
                        ,0x7846,0x7905,0x79BB,0x7A67,0x7B09,0x7BA2,0x7C31,0x7CB7,0x7D32,0x7DA4
                        ,0x7E0D,0x7E6B,0x7EC0,0x7F0A,0x7F4B,0x7F82,0x7FAF,0x7FD2,0x7FEB,0x7FFA
                        ,0x7FFF};
void utils_sin_cos_u16(uint16_t phase, int16_t* sin, int16_t* cos)
{
    phase /= 100U;
    if(phase<90){
        *sin = sin_table[phase];
        *cos = sin_table[90-phase];
    }else if(phase>=90&&phase<180){
        *sin = sin_table[180-phase];
        *cos = -sin_table[phase-90];
    }else if(phase>=180&&phase<270){
        *sin = -sin_table[phase-180];
        *cos = -sin_table[270-phase];
    }else if(phase>=270&&phase<360){
        *sin = -sin_table[360-phase];
        *cos = sin_table[phase-270];
    }
}

/*!
    \brief      the arctan function
    \param[in]  phase: scaled input value in degrees, -18000-18000
    \param[in]  sin: points to the processed sine output
    \param[in]  cos: points to the processed cosine output
    \param[out] none
    \retval     none
*/
uint16_t table[501] = { 0x0000, 0x000b, 0x0016, 0x0022, 0x002d, 0x0039, 0x0044, 0x0050, 0x005b, 0x0067, 
                        0x0072, 0x007e, 0x0089, 0x0094, 0x00a0, 0x00ab, 0x00b7, 0x00c2, 0x00ce, 0x00d9, 
                        0x00e5, 0x00f0, 0x00fb, 0x0107, 0x0112, 0x011e, 0x0129, 0x0135, 0x0140, 0x014b, 
                        0x0157, 0x0162, 0x016e, 0x0179, 0x0185, 0x0190, 0x019b, 0x01a7, 0x01b2, 0x01be, 
                        0x01c9, 0x01d4, 0x01e0, 0x01eb, 0x01f6, 0x0202, 0x020d, 0x0219, 0x0224, 0x022f, 
                        0x023b, 0x0246, 0x0251, 0x025d, 0x0268, 0x0273, 0x027f, 0x028a, 0x0295, 0x02a0, 
                        0x02ac, 0x02b7, 0x02c2, 0x02ce, 0x02d9, 0x02e4, 0x02ef, 0x02fb, 0x0306, 0x0311, 
                        0x031c, 0x0328, 0x0333, 0x033e, 0x0349, 0x0355, 0x0360, 0x036b, 0x0376, 0x0381, 
                        0x038d, 0x0398, 0x03a3, 0x03ae, 0x03b9, 0x03c4, 0x03cf, 0x03db, 0x03e6, 0x03f1, 
                        0x03fc, 0x0407, 0x0412, 0x041d, 0x0428, 0x0433, 0x043e, 0x0449, 0x0454, 0x045f, 
                        0x046a, 0x0476, 0x0481, 0x048c, 0x0496, 0x04a1, 0x04ac, 0x04b7, 0x04c2, 0x04cd, 
                        0x04d8, 0x04e3, 0x04ee, 0x04f9, 0x0504, 0x050f, 0x051a, 0x0525, 0x052f, 0x053a, 
                        0x0545, 0x0550, 0x055b, 0x0566, 0x0570, 0x057b, 0x0586, 0x0591, 0x059b, 0x05a6, 
                        0x05b1, 0x05bc, 0x05c6, 0x05d1, 0x05dc, 0x05e6, 0x05f1, 0x05fc, 0x0606, 0x0611, 
                        0x061c, 0x0626, 0x0631, 0x063c, 0x0646, 0x0651, 0x065b, 0x0666, 0x0670, 0x067b, 
                        0x0685, 0x0690, 0x069a, 0x06a5, 0x06af, 0x06ba, 0x06c4, 0x06cf, 0x06d9, 0x06e4, 
                        0x06ee, 0x06f8, 0x0703, 0x070d, 0x0717, 0x0722, 0x072c, 0x0736, 0x0741, 0x074b, 
                        0x0755, 0x0760, 0x076a, 0x0774, 0x077e, 0x0789, 0x0793, 0x079d, 0x07a7, 0x07b1, 
                        0x07bb, 0x07c6, 0x07d0, 0x07da, 0x07e4, 0x07ee, 0x07f8, 0x0802, 0x080c, 0x0816, 
                        0x0820, 0x082a, 0x0834, 0x083e, 0x0848, 0x0852, 0x085c, 0x0866, 0x0870, 0x087a, 
                        0x0884, 0x088e, 0x0897, 0x08a1, 0x08ab, 0x08b5, 0x08bf, 0x08c8, 0x08d2, 0x08dc, 
                        0x08e6, 0x08ef, 0x08f9, 0x0903, 0x090d, 0x0916, 0x0920, 0x092a, 0x0933, 0x093d, 
                        0x0946, 0x0950, 0x095a, 0x0963, 0x096d, 0x0976, 0x0980, 0x0989, 0x0993, 0x099c, 
                        0x09a6, 0x09af, 0x09b9, 0x09c2, 0x09cb, 0x09d5, 0x09de, 0x09e8, 0x09f1, 0x09fa, 
                        0x0a04, 0x0a0d, 0x0a16, 0x0a1f, 0x0a29, 0x0a32, 0x0a3b, 0x0a44, 0x0a4e, 0x0a57, 
                        0x0a60, 0x0a69, 0x0a72, 0x0a7b, 0x0a85, 0x0a8e, 0x0a97, 0x0aa0, 0x0aa9, 0x0ab2, 
                        0x0abb, 0x0ac4, 0x0acd, 0x0ad6, 0x0adf, 0x0ae8, 0x0af1, 0x0afa, 0x0b03, 0x0b0c, 
                        0x0b14, 0x0b1d, 0x0b26, 0x0b2f, 0x0b38, 0x0b41, 0x0b49, 0x0b52, 0x0b5b, 0x0b64, 
                        0x0b6c, 0x0b75, 0x0b7e, 0x0b86, 0x0b8f, 0x0b98, 0x0ba0, 0x0ba9, 0x0bb2, 0x0bba, 
                        0x0bc3, 0x0bcb, 0x0bd4, 0x0bdd, 0x0be5, 0x0bee, 0x0bf6, 0x0bff, 0x0c07, 0x0c0f, 
                        0x0c18, 0x0c20, 0x0c29, 0x0c31, 0x0c39, 0x0c42, 0x0c4a, 0x0c52, 0x0c5b, 0x0c63, 
                        0x0c6b, 0x0c74, 0x0c7c, 0x0c84, 0x0c8c, 0x0c95, 0x0c9d, 0x0ca5, 0x0cad, 0x0cb5, 
                        0x0cbd, 0x0cc6, 0x0cce, 0x0cd6, 0x0cde, 0x0ce6, 0x0cee, 0x0cf6, 0x0cfe, 0x0d06, 
                        0x0d0e, 0x0d16, 0x0d1e, 0x0d26, 0x0d2e, 0x0d36, 0x0d3e, 0x0d45, 0x0d4d, 0x0d55, 
                        0x0d5d, 0x0d65, 0x0d6d, 0x0d75, 0x0d7c, 0x0d84, 0x0d8c, 0x0d94, 0x0d9b, 0x0da3, 
                        0x0dab, 0x0db2, 0x0dba, 0x0dc2, 0x0dc9, 0x0dd1, 0x0dd9, 0x0de0, 0x0de8, 0x0def, 
                        0x0df7, 0x0dfe, 0x0e06, 0x0e0d, 0x0e15, 0x0e1c, 0x0e24, 0x0e2b, 0x0e33, 0x0e3a, 
                        0x0e42, 0x0e49, 0x0e50, 0x0e58, 0x0e5f, 0x0e66, 0x0e6e, 0x0e75, 0x0e7c, 0x0e84, 
                        0x0e8b, 0x0e92, 0x0e99, 0x0ea1, 0x0ea8, 0x0eaf, 0x0eb6, 0x0ebd, 0x0ec5, 0x0ecc, 
                        0x0ed3, 0x0eda, 0x0ee1, 0x0ee8, 0x0eef, 0x0ef6, 0x0efd, 0x0f04, 0x0f0b, 0x0f12, 
                        0x0f19, 0x0f20, 0x0f27, 0x0f2e, 0x0f35, 0x0f3c, 0x0f43, 0x0f4a, 0x0f51, 0x0f58, 
                        0x0f5f, 0x0f66, 0x0f6c, 0x0f73, 0x0f7a, 0x0f81, 0x0f88, 0x0f8e, 0x0f95, 0x0f9c, 
                        0x0fa3, 0x0fa9, 0x0fb0, 0x0fb7, 0x0fbd, 0x0fc4, 0x0fcb, 0x0fd1, 0x0fd8, 0x0fde, 
                        0x0fe5, 0x0fec, 0x0ff2, 0x0ff9, 0x0fff, 0x1006, 0x100c, 0x1013, 0x1019, 0x1020, 
                        0x1026, 0x102d, 0x1033, 0x103a, 0x1040, 0x1046, 0x104d, 0x1053, 0x105a, 0x1060, 
                        0x1066, 0x106d, 0x1073, 0x1079, 0x107f, 0x1086, 0x108c, 0x1092, 0x1098, 0x109f, 
                        0x10a5, 0x10ab, 0x10b1, 0x10b7, 0x10be, 0x10c4, 0x10ca, 0x10d0, 0x10d6, 0x10dc, 
                        0x10e2, 0x10e8, 0x10ee, 0x10f5, 0x10fb, 0x1101, 0x1107, 0x110d, 0x1113, 0x1119, 
                        0x111f, 0x1125, 0x112a, 0x1130, 0x1136, 0x113c, 0x1142, 0x1148, 0x114e, 0x1154, 
                        0x115a, 0x115f, 0x1165, 0x116b, 0x1171, 0x1177, 0x117c, 0x1182, 0x1188, 0x118e, 
                        0x1194, };
int16_t utils_atan_2f(float y, float x)
{
    uint16_t index;
    int16_t phase;
    
    if(y>=0&&x>=0){
        if(y<=x){
            index = (y*500)/x;
            phase = table[index];
        }else{
            index = (x*500)/y;
            phase = 9000U - table[index];
        }
    }else if(y<0&&x>=0){
        if(-y<=x){
            index = -(y*500)/x;
            phase =  - table[index];
        }else{
            index = -(x*500)/y;
            phase = -9000 + table[index];
        }
    }else if(y<0&&x<0){
        if(-y<=-x){
            index = (y*500)/x;
            phase = -18000 + table[index];
        }else{
            index = (x*500)/y;
            phase = -9000 - table[index];
        }
    }else{
        if(y<=-x){
            index = -(y*500)/x;
            phase = 18000U - table[index];
        }else{
            index = -(x*500)/y;
            phase = 9000U + table[index];
        }
    }
    
    return phase;
}
