/**
 * \file
 * \brief
 * \details
 * \author      Ricardo Andres Velásquez
 * \version     0.0.1
 * \date        05/10/2023
 * \copyright   Unlicensed
 */

#include "time_base.h"
#include <stdint.h>


void tb_init(time_base_t *t, uint64_t us, bool en){
    t->next = time_us_64() + us ;
    t->delta = us;
    t->en = en;
}