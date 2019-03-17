/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: wienerAB.h
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 19-Jun-2017 17:45:48
 */

#ifndef WIENERAB_H
#define WIENERAB_H

/* Include Files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "wienerAB_types.h"

/* Function Declarations */
extern void wienerAB(const float frame_in[200], float spl_threshold, float Srate,
                     float out[200]);
extern void wienerAB_free(void);
extern void wienerAB_init(void);

#endif

/*
 * File trailer for wienerAB.h
 *
 * [EOF]
 */
