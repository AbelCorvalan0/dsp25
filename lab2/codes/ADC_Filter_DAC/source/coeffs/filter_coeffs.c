#include "arm_math.h"
#include "filter_coeffs.h"

#include "coeffs/Lowpass_Filter/lpCoeff/lp8k.h"
#include "coeffs/Lowpass_Filter/lpCoeff/lp16k.h"
#include "coeffs/Lowpass_Filter/lpCoeff/lp22k.h"
#include "coeffs/Lowpass_Filter/lpCoeff/lp44k.h"
#include "coeffs/Lowpass_Filter/lpCoeff/lp48k.h"

#include "coeffs/Highpass_Filter/hpCoeff/hp8k.h"
#include "coeffs/Highpass_Filter/hpCoeff/hp16k.h"
#include "coeffs/Highpass_Filter/hpCoeff/hp22k.h"
#include "coeffs/Highpass_Filter/hpCoeff/hp44k.h"
#include "coeffs/Highpass_Filter/hpCoeff/hp48k.h"

#include "coeffs/Bandstop_Filter/bsCoeff/bs8k.h"
#include "coeffs/Bandstop_Filter/bsCoeff/bs16k.h"
#include "coeffs/Bandstop_Filter/bsCoeff/bs22k.h"
#include "coeffs/Bandstop_Filter/bsCoeff/bs44k.h"
#include "coeffs/Bandstop_Filter/bsCoeff/bs48k.h"

#include "coeffs/Passband_Filter/pbCoeff/pb8k.h"
#include "coeffs/Passband_Filter/pbCoeff/pb16k.h"
#include "coeffs/Passband_Filter/pbCoeff/pb22k.h"
#include "coeffs/Passband_Filter/pbCoeff/pb44k.h"
#include "coeffs/Passband_Filter/pbCoeff/pb48k.h"

/* Definición real en memoria */
//const q15_t*

//const q15_t* coef[fsCount][filterCount];

const q15_t* coef[fsCount][filterCount] = {

    /* f8k */
    {
        /* LP */
        lp8k,
        /* HP */
        hp8k,
        /* PB */
        pb8k,
        /* BS */
        bs8k
    },

    /* f16k */
    {
        lp16k,
        hp16k,
        pb16k,
        bs16k
    },

    /* f22k */
    {
        lp22k,
        hp22k,
        pb22k,
        bs22k
    },

    /* f44k */
    {
        lp44k,
        hp44k,
        pb44k,
        bs44k
    },

    /* f48k */
    {
        lp48k,
        hp48k,
        pb48k,
        bs48k
    }
};

const uint16_t coef_taps[fsCount][filterCount] = {

    /* f8k */
    { lp8k_taps, hp8k_taps, pb8k_taps, bs8k_taps },

    /* f16k */
    { lp16k_taps, hp16k_taps, pb16k_taps, bs16k_taps },

    /* f22k */
    { lp22k_taps, hp22k_taps, pb22k_taps, bs22k_taps },

    /* f44k */
    { lp44k_taps, hp44k_taps, pb44k_taps, bs44k_taps },

    /* f48k */
    { lp48k_taps, hp48k_taps, pb48k_taps, bs48k_taps }
};
