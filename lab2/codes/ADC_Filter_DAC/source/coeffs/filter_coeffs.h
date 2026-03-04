#ifndef FILTROS_COEFFS_H
#define FILTROS_COEFFS_H

#include <stdint.h>
#include "arm_math.h"

typedef enum{ f8k, f16k, f22k, f44k, f48k, fsCount } fs_t;
typedef enum{ LP, HP, PB, BS, filterCount }filter_t;

extern const q15_t*   coef[fsCount][filterCount];
extern const uint16_t coef_taps[fsCount][filterCount];

// /* Frecuencias */
// typedef enum {
//     FS_8K = 0,
//     FS_16K,
//     FS_22K,
//     FS_44K,
//     FS_48K,
//     fsCount
// } fs_index_t;

// /* Tipos de filtro */
// typedef enum {
//     FILTER_LP = 0,
//     FILTER_HP,
//     FILTER_PB,
//     FILTER_BS,
//     filterCount
// } filter_index_t;

#endif
