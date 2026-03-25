#ifndef FILTROS_COEFFS_H
#define FILTROS_COEFFS_H

#include <stdint.h>
#include "arm_math.h"

typedef enum{ f8k, f16k, f22k, f44k, f48k, fsCount } fs_t;
typedef enum{ LP, HP, PB, BS, filterCount }filter_t;

extern const q15_t*   coef[fsCount][filterCount];
extern const uint16_t coef_taps[fsCount][filterCount];

#endif
