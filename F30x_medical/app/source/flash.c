/*!
    \file    flash.c
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

/* the definition of FMC operation */
#define FMC_PAGE_SIZE                   ((uint16_t)0x0800U)
#define FMC_WRITE_START_ADDR            ((uint32_t)0x0803E000U)
#define FMC_WRITE_END_ADDR              ((uint32_t)0x08040000U)

#define FMC_MODE_SHIFT_ADDR             ((uint32_t)0x0803E000U)
#define FMC_CONFIG_ADDR                 ((uint32_t)0x0803E800U)

/*
** fmc_data[0]: [0] drive_mode  [1]genertor_gear 
** fmc_data[1]: [0] [1]g_torque_offset
*/
uint8_t fmc_data[3][8];
uint32_t address = 0x00;
/* calculate the number of page to be programmed/erased */
uint32_t PageNum = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t WordNum = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 2);

/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  page: the page will be erased
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(flash_page_type page_type)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    /* erase the flash pages */
    fmc_page_erase(FMC_WRITE_START_ADDR + (FMC_PAGE_SIZE * (uint8_t)page_type));
    fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc data
    \param[in]  page_type: type of the writing data
    \param[out] none
    \retval     none
*/
void fmc_data_program(flash_page_type page_type)
{
    // uint8_t i;
    /* unlock the flash program/erase controller */
    fmc_unlock();
    
    switch(page_type){
        /* program mode shift data */
        case MODE_PAGE:
            address = FMC_MODE_SHIFT_ADDR /*+ (FMC_PAGE_SIZE * (uint8_t)page_type)*/;
            
            fmc_halfword_program(address, cup_count);
            address += 2;
            fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

            fmc_halfword_program(address, temperature_set);
            address += 2;
            fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

            fmc_halfword_program(address, enzyme_rate);
            address += 2;
            fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);

            fmc_halfword_program(address, water_set);
            address += 2;
            fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
            break;

        /* program config data */
        case CONFIG_PAGE:
            address = FMC_CONFIG_ADDR /*+ (FMC_PAGE_SIZE * (uint8_t)page_type)*/;
            
            //fmc_halfword_program(address, (uint16_t)(g_torque_offset*100));
            address += 2;
            fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
            break;
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      read data from the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_data_read(void)
{
    uint32_t i;
    uint16_t *ptrd1;

    ptrd1 = (uint16_t *)FMC_MODE_SHIFT_ADDR;
    /* read flash */
    for(i=0;i<8;i++){
        fmc_data[0][i] = *ptrd1++;
    }
    ptrd1 = (uint16_t *)FMC_CONFIG_ADDR;
    /* read flash */
    for(i=0;i<8;i++){
        fmc_data[1][i] = *ptrd1++;
    }
}
