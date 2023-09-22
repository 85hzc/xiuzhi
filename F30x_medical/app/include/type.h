/*!
    \file    type.h
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

#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include "string.h"
#include "gd32f30x.h"

/* the state of motor */
typedef enum{
    MC_STATE_INIT = 0,                                                          /* init state */
    MC_STATE_IDLE,                                                              /* idle state */
    MC_STATE_RUNNING,                                                           /* running state */
    MC_STATE_BRAKE,                                                             /* brake state */
    MC_STATE_FAULT,                                                             /* fault state */
}mc_state_e;

/* the running mode of the motor */
typedef enum{
    CONTROL_MODE_HALL = 1,                                                      /* hall mode */
    CONTROL_MODE_DETECT_HALL,                                                   /* hall detect mode */
}mc_control_mode;

/* the fault of the motor */
typedef enum{
    FAULT_NONE = 0,                                                             /* no faults */
    FAULT_VOLTAGE,                                                              /* under voltage or over voltage fault */
    FAULT_OVER_CURRENT,                                                         /* over current fault */
    FAULT_TEMPERATURE,                                                          /* over temperature fault */
    FAULT_HARDWARE                                                              /* hardware(mosfet) fault */
}mc_fault;

typedef enum{
    ONE = 1,                                                                    /* sector one */
    TWO,                                                                        /* sector two */
    THREE,                                                                      /* sector three */
    FOUR,                                                                       /* sector four */
    FIVE,                                                                       /* sector five */
    SIX                                                                         /* sector six */
}SECTOR;

/* motor running direction */
typedef enum{
    DIRECTION_CCW = 0,                                                          /* the direction is counter clockwise */
    DIRECTION_CW,                                                               /* the direction is clockwise */
}mc_direction;

/* ec11 input direction */
typedef enum{
    DIRECTION_INCREASE = 1,                                                      /* the direction of ec11 input is increase */
    DIRECTION_DECREASE,                                                          /* the direction of ec11 input is decrease */
}ec11_direction;

/* the structure of pid parameter */
typedef struct{
    uint16_t kp;                                                                /* the proportional factor */
    uint16_t kp_div;                                                            /* the proportional division factor */
    float ki;                                                                   /* the integral factor */
    uint16_t ki_div;                                                            /* the integral division factor */
    uint8_t i_index;                                                            /* the integral separation index */
    uint16_t i_separation_threshold;                                            /* the integral separation threshold */
    uint8_t i_anti_windup;                                                      /* the integral anti-windup flag */
    float ki_sum;                                                               /* the sum of the integral term */
    int32_t ki_sum_upper_limit;                                                 /* the upper limit of integral term */
    int32_t ki_sum_lower_limit;                                                 /* the lower limit of integral term */
    uint16_t kd;                                                                /* the differential factor */
    uint16_t kd_div;                                                            /* the differential division factor */
    int16_t upper_limit;                                                        /* the upper limit of ouput */
    int16_t lower_limit;                                                        /* the lower limit of ouput */
    int16_t error;                                                              /* the error of the input */
    int16_t output;                                                             /* the output varialbe */
    uint8_t direction_flag;                                                     /* the sign of the output variable */
}pid_parameter;

/* the structure of motor parameter */
typedef struct{
    SECTOR sector;                                                             /* the sector of applying voltage */
    float i_a;                                                                  /* current a in float format */
    float i_b;                                                                  /* current b in float format */
    float i_c;                                                                  /* current c in float format */
    float i_alpha;                                                              /* current alpha in float format */
    float i_beta;                                                               /* current beta in float format */
    float id;                                                                   /* current d in float format */
    float iq;                                                                   /* current q in float format */
    float i_abs;                                                                /* the magnitude of applying current */
    float id_filter;                                                            /* the filtered current d */
    float iq_filter;                                                            /* the filtered current q */
    float i_abs_filter;                                                         /* the filtered magnitude of applying current */

    float v_alpha;                                                              /* voltage alpha in float format */
    float v_beta;                                                               /* voltage beta in float format */
    float vd;                                                                   /* voltage d in float format */
    float vq;                                                                   /* voltage q in float format */
    float vd_filter;                                                            /* the filtered voltage d */
    float vq_filter;                                                            /* the filtered voltage q */
    float v_abs;                                                                /* the magnitude of applying voltage */

    uint16_t i_a_offset;                                                        /* the offset of current a */
    uint16_t i_b_offset;                                                        /* the offset of current b */
    uint16_t i_c_offset;                                                        /* the offset of current c */

    float torque;                                                               /* torque voltage */
    float P;                                                                    /* real power */
    float Q;                                                                    /* reactive power */
    float S;                                                                    /* apparent power */
}motor_state;

/* the structure of flags */
typedef struct{
    uint8_t running_flag;                                                       /* running flag */
    uint8_t windup_flag;                                                        /* applying voltage windup flag */
    uint8_t detect_hall_flag;                                                   /* hall detection flag */
    uint8_t closeloop_flag;                                                     /* close-loop control flag */
    uint8_t flux_weaken_flag;                                                   /* flux weakening flag */
    uint8_t adc_ready_flag;                                                     /* adc ready flag */
}mc_flags;

/* the structure of Luenberger/SMO observer */
typedef struct{
    float i_alpha;                                                              /* current alpha */
    float i_beta;                                                               /* current beta */
    float i_alpha_next;                                                         /* current alpha of the next period */
    float i_beta_next;                                                          /* current beta of the next period */
    float bemf_alpha;                                                           /* back electromotive force alpha */
    float bemf_beta;                                                            /* back electromotive force beta */
    float bemf_alpha_next;                                                      /* back electromotive force alpha of the next period */
    float bemf_beta_next;                                                       /* back electromotive force beta of the next period */
    float i_alpha_error;                                                        /* the error of the current between the sampling and the observing in the alpha axis */
    float i_beta_error;                                                         /* the error of the current between the sampling and the observing in the beta axis */
    float phase;                                                                /* observing electrical angle */
    float phase_pre;                                                            /* observing electrical angle of the previous period */
    float phase_compesation;                                                    /* compensating electrical angle */
    float speed;                                                                /* the mechanical speed */
    float speed_angular;                                                        /* the mechanical angular speed */
}state_observer;

typedef enum{
    NORMAL = 0,                                                                 /* normal region */
    REGION1,                                                                    /* region 1 */
    REGION2,                                                                    /* region 2 */
    LOW_MODULATION                                                              /* low modulation region */
}current_sample_region_type;

/* the step of 1-shunt sampling */
typedef enum{
    INIT = 0,                                                                   /* initializing step */
    FIRST_SAMPLE_SET,                                                           /* first sampling step */
    SECOND_SAMPLE_SET,                                                          /* second sampling step */
}one_shunt_sample_state_type;

/* the sampling current flag */
typedef enum{
    NONE = 0,
    IA,
    IB,
    IC,
}current_sample_type;

/* the structure of 1-shunt sampling */
typedef struct{
    uint16_t ta;                                                                /* the calculated time a */
    uint16_t tb;                                                                /* the calculated time b */
    uint16_t tc;                                                                /* the calculated time c */
    uint16_t delta_t1;                                                          /* the error of two timing */
    uint16_t delta_t2;                                                          /* the error of two timing */
    current_sample_region_type region;                                          /* the region of applying voltage */
    int16_t ta_mod1;                                                            /* the modified time a */
    int16_t ta_mod2;                                                            /* the modified time a */
    int16_t tb_mod1;                                                            /* the modified time b */
    int16_t tb_mod2;                                                            /* the modified time b */
    int16_t tc_mod1;                                                            /* the modified time c */
    int16_t tc_mod2;                                                            /* the modified time c */
    uint16_t samp1;                                                             /* the first sampling time */
    uint16_t samp2;                                                             /* the second sampling time */
    current_sample_type current1;                                               /* the flag of first sampling current */
    current_sample_type current2;                                               /* the flag of second sampling current */
}current_sample;

/* the page type to store the data */
typedef enum{
    MODE_PAGE = 0,
    CONFIG_PAGE,
}flash_page_type;

/* the hall structure */
typedef struct{
    uint8_t hall_state;                                                         /* the hall state */
    uint8_t pre_state;                                                          /* the previous hall state */
    float speed;                                                                /* the estimated speed */
    float phase;                                                                /* the estimated spatial angle */
    int16_t phase_shift;                                                        /* the phase shift of hall */
    uint8_t sequence_ccw[8];                                                    /* the ccw hall sequence */
    uint8_t sequence_cw[8];                                                     /* the cw hall sequence */
    uint16_t phase_table[8];                                                    /* the hall phase table */
}hall_structure;

#endif /* TYPE_H */
