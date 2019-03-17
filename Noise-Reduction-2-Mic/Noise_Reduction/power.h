/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: power.h
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 19-Jun-2017 17:45:48
 */

#ifndef POWER_H
#define POWER_H

/* Include Files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "wienerAB_types.h"

/* Function Declarations */
extern void b_power(const emxArray_real32_T *a, emxArray_real32_T *y);
extern void power(const float a[512], float y[512]);

#endif

/*
 * File trailer for power.h
 *
 * [EOF]
 */
