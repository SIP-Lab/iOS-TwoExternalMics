/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: abs.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 19-Jun-2017 17:45:48
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "wienerAB.h"
#include "abs.h"
#include "wienerAB_rtwutil.h"

/* Function Definitions */

/*
 * Arguments    : const creal32_T x[512]
 *                float y[512]
 * Return Type  : void
 */
void b_abs(const creal32_T x[512], float y[512])
{
  int k;
  for (k = 0; k < 512; k++) {
    y[k] = rt_hypotf_snf(x[k].re, x[k].im);
  }
}

/*
 * File trailer for abs.c
 *
 * [EOF]
 */
